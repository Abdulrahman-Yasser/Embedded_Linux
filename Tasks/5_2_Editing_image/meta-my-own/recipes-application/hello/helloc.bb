SUMMARY = "hello world"
LICENSE = "CLOSED"
SRC_URI = "file://main.c" 
S = "${WORKDIR}"
do_compile(){
	bbwarn "the compiler is ${CC}"	# CC is a variable that holds the compiler name
	bbwarn "Tamam, The ldflags is ${LDFLAGS}"
	${CC} ${LDFLAGS} main.c -o hello_c -DECONFIG_VALUE=12
}

do_install(){
	install -d ${D}/usr/bin
	cp ${WORKDIR}/hello_c ${D}/usr/bin 
}