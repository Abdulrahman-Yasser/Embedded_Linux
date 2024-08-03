include recipes-core/images/core-image-minimal.bb
inherit extrausers  
EXTRA_USERS_PARAMS = "usermod -P MyPass root;"
IMAGE_FEATURES += " ssh-server-dropbear"
IMAGE_INSTALL += " helloc hellocpp strace"
