
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

#include <interface.h>

#define YUY2_BYTES_PER_PIXEL 2
#define DEFAULT_BUFFERS 4
#define MIN_BUFFERS 2
#define MAX_BUFFERS 16

#define MAX_PATH 512

static _Atomic int active_detections = 0;
static const int MAX_CONCURRENT_DETECTIONS = 2;
const char* dataPath = "/path/to/cam_infer_models/";
char yoloBasePath[MAX_PATH];
char yoloClassesFile[MAX_PATH];

uint32_t frame_width;
uint32_t frame_height;
bool detect_done = false;



struct impl {
    struct pw_main_loop *loop;
    struct pw_context *context;
    struct pw_core *core;

    struct pw_stream *capture;
    struct pw_stream *raw_playback;
    struct pw_stream *detection_playback;

    struct spa_hook capture_listener;
    struct spa_hook raw_playback_listener;
    struct spa_hook detection_playback_listener;

    struct spa_video_info_raw capture_info;
    struct spa_video_info_raw raw_playback_info;
    struct spa_video_info_raw detection_playback_info;
};



int main(int argc, char *argv[]){
    struct impl *impl;
    const struct spa_pod *params[1];
    uint8_t buffer[1024];
    struct spa_pod_builder b;
    int res;
    spa_autofree char *link_group = NULL;

    impl = calloc(1, sizeof(struct impl));
    if (impl == NULL)
        return -1;

    // Initialize PipeWire
    pw_init(&argc, &argv);

    // Create main loop
    impl->loop = pw_main_loop_new(NULL);
    if (impl->loop == NULL) {
        fprintf(stderr, "Failed to create main loop\n");
        return -1;
    }

    // Create context
    impl->context = pw_context_new(pw_main_loop_get_loop(impl->loop), NULL, 0);
    if (impl->context == NULL) {
        fprintf(stderr, "Failed to create context\n");
        return -1;
    }

    // Connect to PipeWire
    impl->core = pw_context_connect(impl->context, NULL, 0);
    if (impl->core == NULL) {
        fprintf(stderr, "Failed to connect to PipeWire\n");
        return -1;
    }

    // Setup camera format
    impl->capture_info = (struct spa_video_info_raw) {
        .format = SPA_VIDEO_FORMAT_YUY2,
        .size.width = 640,
        .size.height = 480,
    };
    impl->raw_playback_info = impl->capture_info;
    impl->detection_playback_info = impl->capture_info;

    link_group = spa_aprintf ("camera-filter-%d", getpid());

    // Create capture stream
    impl->capture = pw_stream_new(impl->core, "filter-capture",
            pw_properties_new(
                PW_KEY_MEDIA_TYPE, "Video",
                PW_KEY_MEDIA_CATEGORY, "Capture",
                PW_KEY_MEDIA_ROLE, "Camera",
                PW_KEY_NODE_DESCRIPTION, "camera sink",
		PW_KEY_MEDIA_CLASS, "Stream/Input/Video",
                PW_KEY_NODE_LINK_GROUP, link_group,
                NULL));

    // Create raw playback stream
    impl->raw_playback = pw_stream_new(impl->core, "raw-playback",
            pw_properties_new(
                PW_KEY_MEDIA_TYPE, "Video",
                PW_KEY_MEDIA_CATEGORY, "Playback",
                PW_KEY_MEDIA_ROLE, "Camera",
                PW_KEY_NODE_DESCRIPTION, "raw playback",
                PW_KEY_NODE_NAME, "camera-raw-output",
		PW_KEY_MEDIA_CLASS, "Stream/Output/Video",
                PW_KEY_NODE_LINK_GROUP, link_group,
                NULL));

    // Add listeners
    pw_stream_add_listener(impl->capture,
            &impl->capture_listener,
            &capture_stream_events, impl);
    pw_stream_add_listener(impl->raw_playback,
            &impl->raw_playback_listener,
            &playback_stream_events, impl);

}