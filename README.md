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


