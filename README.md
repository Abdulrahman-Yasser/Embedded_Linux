# Embedded_Linux
This repo will contain some projects that I've faced during learning Embedded Linux.

- If you have a problem with HDMI in RPI-4 : 
	- config.txt content :
	
		max_framebuffers=2
		force_hotplug=1
		hdmi_force_hotplug:0=1
		#hdmi_safe=1
		config_hdmi_boost=4
		hdmi_group:0=1
		hdmi_mode:0=4
		#arm_64bit=0
		boot_delay=1
	
	- cmdline.txt content : 
		video=HDMI-A-1:1024x768@60D

- I had a problem for not booting and stucking at image of raspberry 
	- Add to config.tst
		kernel=yourImage.img

- Problem :  end kernel panic not syncing attempted to kill init raspberry
	It happened because the kernel tries to run systemd code.
	running file /sbin/init show us a 64 bit executable.
	so kernel tries to run 64-bit on 32-bit image.
	- The solution is to delete /sbin/init
	
- problem : Entering in initramfs : 
	solution : fix the rootfs in your cmdline.txt

- problem : not detecting wlan0 ?
	solution : add arm_64bit=0
	solution 2 : add the right modules for the right (uname -r)
- problem in not connecting to the wayland
[20:28:57.062406] [C] Failed to connect to Wayland display. No such file or directory

export XDG_RUNTIME_DIR=/run/user/1001
export WAYLAND_DISPLAY=wayland-0


The problem was actually 3 separate issues stacked on top of each other:
1. Missing -audiodev link (first problem)
Your original command had -device es1370 but no -audiodev backend. QEMU was emulating the sound card but had nowhere to send the audio — it was just discarding it silently.
2. Yocto's QEMU was compiled without PulseAudio support
When we added -audiodev pa,id=agl, it failed with Parameter 'driver' does not accept value 'pa' because the Yocto-built QEMU binary was compiled without PulseAudio support. Your host runs PulseAudio, so ALSA couldn't reach your speakers either, giving the -5 Input/output error.
3. Wrong QEMU binary
The fix was replacing the Yocto QEMU binary with a symlink to your system's /usr/bin/qemu-system-x86_64 (installed via apt), which IS compiled with full PulseAudio support.

The winning combination:


- System QEMU (/usr/bin/qemu-system-x86_64) → has PulseAudio support
- device es1370 → the virtual sound card the guest can see
- audiodev pa,id=agl → routes audio to your host's PulseAudio
- audiodev=agl → links the card to the backend

////////////////////////////////////////////////////////////////////////
runqemu - ERROR - Failed to run qemu: qemu-system-x86_64: -audiodev pa,id=agl: Parameter 'driver' does not accept value 'pa'
runqemu - INFO - Cleaning up

ln -sf /usr/bin/qemu-system-x86_64 \
   /media/abdu/LinuxHome/Embedded_Linux/git_ignoring/AGL/trout/qemux86-64/tmp/work/x86_64-linux/qemu-helper-native/1.0/recipe-sysroot-native/usr/bin/qemu-system-x86_64

runqemu qemux86-64 serial qemuparams="-device intel-hda -device hda-duplex,audiodev=agl -audiodev pa,id=agl"

root@qemux86-64:~# gst-launch-1.0 audiotestsrc wave=sine freq=440 num-buffers=100 ! alsasink device=hw:0,0

//////////////////////////////////////////////////////////////////////////

root@qemux86-64:~# aplay -l
**** List of PLAYBACK Hardware Devices ****
card 0: AudioPCI [Ensoniq AudioPCI], device 0: ES1370/1 [ES1370 DAC2/ADC]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 0: AudioPCI [Ensoniq AudioPCI], device 1: ES1370/2 [ES1370 DAC1]
  Subdevices: 1/1
  Subdevice #0: subdevice #0


to make the app get the sound automatically 
cat > /etc/asound.conf << 'EOF'
pcm.!default {
    type hw
    card 0
    device 0
}
ctl.!default {
    type hw
    card 0
}
EOF

gst-launch-1.0 audiotestsrc wave=sine freq=440 num-buffers=100 ! autoaudiosink
//////////////////////////////////////////////////////////////////////////////////
root@qemux86-64:~# aplay -l
aplay: device_list:279: no soundcards found...

runqemu qemux86-64 serial qemuparams="-device es1370,audiodev=agl -audiodev pa,id=agl"
////////////////////////////////////////////////////////////////////////////
