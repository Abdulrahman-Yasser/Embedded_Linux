/*
 * cam_viewer.c
 *
 * Captures video from a V4L2 camera (default: /dev/video1) and exposes it
 * as a PipeWire output node named "cam-video-output".
 *
 * Any PipeWire-aware viewer (gst-launch, mpv, qpwgraph) can connect to that
 * output node to display the stream.
 *
 * Usage:
 *   ./cam_viewer                        # auto-connect to any available camera
 *   ./cam_viewer <node-name-or-id>      # target a specific PipeWire node
 *
 * To find your camera's PipeWire node name:
 *   pw-cli ls Node | grep -i video
 *
 * To view the output:
 *   gst-launch-1.0 pipewiresrc target-object=cam-video-output ! videoconvert ! autovideosink
 */

#define _GNU_SOURCE   /* for asprintf used by spa_aprintf */

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <spa/utils/result.h>
#include <spa/param/video/raw.h>
#include <spa/param/video/format-utils.h>

#include <pipewire/pipewire.h>

#define YUY2_BYTES_PER_PIXEL  2
#define DEFAULT_BUFFERS       4
#define MIN_BUFFERS           2
#define MAX_BUFFERS           8

/* ── central state ───────────────────────────────────────────────────────── */

struct impl {
    struct pw_main_loop *loop;
    struct pw_context   *context;
    struct pw_core      *core;

    struct pw_stream *capture;   /* INPUT  — video from camera      */
    struct pw_stream *output;    /* OUTPUT — PipeWire node for viewers */

    struct spa_hook capture_listener;
    struct spa_hook output_listener;

    struct spa_video_info_raw format; /* filled in on_param_changed */
};

/* ── helpers ─────────────────────────────────────────────────────────────── */

static void copy_buffer(struct pw_buffer *in, struct pw_buffer *out)
{
    assert(in->buffer->n_datas == out->buffer->n_datas);

    for (uint32_t i = 0; i < in->buffer->n_datas; i++) {
        struct spa_data *src = &in->buffer->datas[i];
        struct spa_data *dst = &out->buffer->datas[i];

        memcpy(dst->data, src->data, src->chunk->size);
        dst->chunk->offset = 0;
        dst->chunk->size   = src->chunk->size;
        dst->chunk->stride = src->chunk->stride;
    }
}

/* ── stream callbacks ────────────────────────────────────────────────────── */

/*
 * on_process — called by PipeWire every time the camera produces a frame.
 *
 * Sequence:
 *   1. dequeue input buffer from capture stream
 *   2. dequeue output buffer from output stream
 *   3. copy pixels in → out
 *   4. queue output buffer back  (marks it ready for viewers)
 *   5. trigger output stream     (wakes downstream consumers)
 *   6. queue input buffer back   (returns it to the camera)
 */
static void on_process(void *userdata)
{
    struct impl *impl = userdata;
    struct pw_buffer *in, *out;

    /* 1. get the new camera frame */
    if ((in = pw_stream_dequeue_buffer(impl->capture)) == NULL) {
        pw_log_warn("no input buffer available");
        return;
    }

    /* 2 & 3 & 4. copy to output if a buffer is ready */
    if ((out = pw_stream_dequeue_buffer(impl->output)) != NULL) {
        copy_buffer(in, out);
        pw_stream_queue_buffer(impl->output, out);   /* 4 */
        pw_stream_trigger_process(impl->output);     /* 5 */
    } else {
        pw_log_debug("no output buffer — dropping frame");
    }

    /* 6. always return the input buffer */
    pw_stream_queue_buffer(impl->capture, in);
}

/*
 * on_param_changed — called once when the camera and graph agree on a format,
 * and again any time the format changes.
 *
 * Responsibilities:
 *   a. parse the negotiated format (width, height, pixel format)
 *   b. compute the frame buffer size
 *   c. push buffer config back to the capture stream
 *   d. push format + buffer config to the output stream
 */
static void on_param_changed(void *userdata, uint32_t id,
                             const struct spa_pod *param)
{
    struct impl *impl = userdata;
    const struct spa_pod *params[2];
    uint8_t buf[1024];
    struct spa_pod_builder b;
    uint32_t size;

    if (param == NULL || id != SPA_PARAM_Format)
        return;

    /* a. extract width / height */
    spa_format_video_raw_parse(param, &impl->format);
    pw_log_info("format negotiated: %dx%d",
                impl->format.size.width, impl->format.size.height);

    /* b. frame size in bytes, rounded up to 4 */
    size = SPA_ROUND_UP_N(
        impl->format.size.width *
        impl->format.size.height *
        YUY2_BYTES_PER_PIXEL, 4);

    /* c. buffer config for capture stream */
    spa_pod_builder_init(&b, buf, sizeof(buf));
    params[0] = spa_pod_builder_add_object(&b,
        SPA_TYPE_OBJECT_ParamBuffers, SPA_PARAM_Buffers,
        SPA_PARAM_BUFFERS_buffers,  SPA_POD_CHOICE_RANGE_Int(DEFAULT_BUFFERS, MIN_BUFFERS, MAX_BUFFERS),
        SPA_PARAM_BUFFERS_blocks,   SPA_POD_Int(1),
        SPA_PARAM_BUFFERS_size,     SPA_POD_Int(size),
        SPA_PARAM_BUFFERS_dataType, SPA_POD_CHOICE_FLAGS_Int(1 << SPA_DATA_MemFd));

    pw_stream_update_params(impl->capture, params, 1);

    /* d. forward format + buffer config to output */
    spa_pod_builder_init(&b, buf, sizeof(buf));
    params[0] = spa_format_video_raw_build(&b, SPA_PARAM_EnumFormat, &impl->format);
    params[1] = spa_pod_builder_add_object(&b,
        SPA_TYPE_OBJECT_ParamBuffers, SPA_PARAM_Buffers,
        SPA_PARAM_BUFFERS_buffers,  SPA_POD_CHOICE_RANGE_Int(DEFAULT_BUFFERS, MIN_BUFFERS, MAX_BUFFERS),
        SPA_PARAM_BUFFERS_blocks,   SPA_POD_Int(1),
        SPA_PARAM_BUFFERS_size,     SPA_POD_Int(size),
        SPA_PARAM_BUFFERS_dataType, SPA_POD_CHOICE_FLAGS_Int(1 << SPA_DATA_MemFd));

    pw_stream_update_params(impl->output, params, 2);
}

/*
 * on_state_changed — shared by both streams.
 * Logs every transition; quits the loop on unrecoverable error.
 */
static void on_state_changed(void *userdata, enum pw_stream_state old,
                             enum pw_stream_state state, const char *error)
{
    struct impl *impl = userdata;

    printf("stream state: %s → %s\n",
           pw_stream_state_as_string(old),
           pw_stream_state_as_string(state));

    if (error)
        printf("  error: %s\n", error);

    if (state == PW_STREAM_STATE_ERROR)
        pw_main_loop_quit(impl->loop);
}

/* capture: needs all three callbacks */
static const struct pw_stream_events capture_events = {
    PW_VERSION_STREAM_EVENTS,
    .process       = on_process,
    .param_changed = on_param_changed,
    .state_changed = on_state_changed,
};

/* output: only needs state monitoring */
static const struct pw_stream_events output_events = {
    PW_VERSION_STREAM_EVENTS,
    .state_changed = on_state_changed,
};

/* ── signal handler ──────────────────────────────────────────────────────── */

static void signal_handler(int signo)
{
    if (signo == SIGINT) {
        fprintf(stderr, "\nCaught SIGINT, exiting...\n");
        exit(0);
    }
}

/* ── main ────────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[])
{
    struct impl *impl;
    const struct spa_pod *params[1];
    uint8_t buf[1024];
    struct spa_pod_builder b;
    int res;
    char *link_group = NULL;

    /* optional: override which camera node to capture from */
    const char *target = (argc > 1) ? argv[1] : NULL;

    impl = calloc(1, sizeof(*impl));
    if (!impl)
        return -1;

    /* ── 1. library init ─────────────────────────────────────────────────── */
    pw_init(&argc, &argv);

    /* ── 2. event loop ───────────────────────────────────────────────────── */
    impl->loop = pw_main_loop_new(NULL);
    if (!impl->loop) {
        fprintf(stderr, "failed to create main loop\n");
        return -1;
    }

    /* ── 3. context (loads config, discovers modules) ────────────────────── */
    impl->context = pw_context_new(pw_main_loop_get_loop(impl->loop), NULL, 0);
    if (!impl->context) {
        fprintf(stderr, "failed to create context\n");
        return -1;
    }

    /* ── 4. core (UNIX socket connection to the PipeWire daemon) ─────────── */
    impl->core = pw_context_connect(impl->context, NULL, 0);
    if (!impl->core) {
        fprintf(stderr, "failed to connect to PipeWire daemon\n");
        return -1;
    }

    /* link_group ties capture + output together in the graph */
    link_group = spa_aprintf("cam-viewer-%d", getpid());

    /* ── 5a. capture stream ──────────────────────────────────────────────── */
    impl->capture = pw_stream_new(impl->core, "cam-capture",
        pw_properties_new(
            PW_KEY_MEDIA_TYPE,      "Video",
            PW_KEY_MEDIA_CATEGORY,  "Capture",
            PW_KEY_MEDIA_ROLE,      "Camera",
            PW_KEY_MEDIA_CLASS,     "Stream/Input/Video",
            /* target: node name/id passed as argv[1], or PIPEWIRE_NODE env var */
            PW_KEY_TARGET_OBJECT,   target,   /* NULL = auto */
            PW_KEY_NODE_LINK_GROUP, link_group,
            NULL));

    /* ── 5b. output stream (the node viewers connect to) ─────────────────── */
    impl->output = pw_stream_new(impl->core, "cam-video-output",
        pw_properties_new(
            PW_KEY_MEDIA_TYPE,      "Video",
            PW_KEY_MEDIA_CATEGORY,  "Playback",
            PW_KEY_MEDIA_ROLE,      "Camera",
            PW_KEY_MEDIA_CLASS,     "Stream/Output/Video",
            PW_KEY_NODE_NAME,       "cam-video-output",  /* stable name for viewers */
            PW_KEY_NODE_LINK_GROUP, link_group,
            NULL));

    /* ── 6. register event listeners ─────────────────────────────────────── */
    pw_stream_add_listener(impl->capture,
                           &impl->capture_listener, &capture_events, impl);
    pw_stream_add_listener(impl->output,
                           &impl->output_listener,  &output_events,  impl);

    /* ── 7. initial format proposal (camera will negotiate final values) ──── */
    struct spa_video_info_raw fmt = {
        .format      = SPA_VIDEO_FORMAT_YUY2,
        .size.width  = 640,
        .size.height = 480,
    };

    /* ── 8. connect capture stream ───────────────────────────────────────── */
    spa_pod_builder_init(&b, buf, sizeof(buf));
    params[0] = spa_format_video_raw_build(&b, SPA_PARAM_EnumFormat, &fmt);

    res = pw_stream_connect(impl->capture,
            PW_DIRECTION_INPUT,
            PW_ID_ANY,
            PW_STREAM_FLAG_AUTOCONNECT |  /* auto-link to camera source        */
            PW_STREAM_FLAG_MAP_BUFFERS,   /* mmap buffers into our address space */
            params, 1);
    if (res < 0) {
        fprintf(stderr, "capture connect failed: %s\n", spa_strerror(res));
        return -1;
    }

    /* ── 9. connect output stream ────────────────────────────────────────── */
    spa_pod_builder_init(&b, buf, sizeof(buf));
    params[0] = spa_format_video_raw_build(&b, SPA_PARAM_EnumFormat, &fmt);

    res = pw_stream_connect(impl->output,
            PW_DIRECTION_OUTPUT,
            PW_ID_ANY,
            PW_STREAM_FLAG_MAP_BUFFERS |  /* mmap buffers                 */
            PW_STREAM_FLAG_TRIGGER,       /* we call trigger manually     */
            params, 1);
    if (res < 0) {
        fprintf(stderr, "output connect failed: %s\n", spa_strerror(res));
        return -1;
    }

    /* ── 10. run ──────────────────────────────────────────────────────────── */
    signal(SIGINT, signal_handler);

    printf("Running — press Ctrl+C to exit\n");
    if (target)
        printf("Targeting camera node: %s\n", target);
    else
        printf("Auto-connecting to any available camera\n");
    printf("\nTo view the output:\n");
    printf("  gst-launch-1.0 pipewiresrc target-object=cam-video-output ! videoconvert ! autovideosink\n");
    printf("  mpv av://v4l2:/dev/video0  (or connect via qpwgraph)\n\n");

    pw_main_loop_run(impl->loop);   /* blocks until error or SIGINT */

    /* ── 11. cleanup ─────────────────────────────────────────────────────── */
    free(link_group);
    pw_stream_destroy(impl->capture);
    pw_stream_destroy(impl->output);
    pw_context_destroy(impl->context);
    pw_main_loop_destroy(impl->loop);
    pw_deinit();
    free(impl);

    return 0;
}
