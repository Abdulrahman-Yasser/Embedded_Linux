include recipes-core/images/core-image-minimal.bb

IMAGE_FEATURES += " ssh-server-dropbear"
IMAGE_INSTALL += " helloc hellocpp strace"
IMAGE_INSTALL += "hello-recipetool"

