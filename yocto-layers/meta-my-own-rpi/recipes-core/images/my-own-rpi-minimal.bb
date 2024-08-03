include recipes-core/images/rpi-test-image.bb


IMAGE_FEATURES += " ssh-server-dropbear"

IMAGE_INSTALL += " helloc hellocpp strace"

