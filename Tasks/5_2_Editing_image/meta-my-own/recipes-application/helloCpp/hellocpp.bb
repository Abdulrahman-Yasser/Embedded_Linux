SUMMARY="Counter in Cpp"

LICENSE="CLOSED"

SRC_URI="file://main.cpp"
S = "${WORKDIR}"

do_compile(){
    bbwarn "hellocpp is running now"
    ${CXX} ${LDFLAGS} main.cpp -o hello_cpp
}
do_install(){
    install -d ${D}/usr/bin
    cp ${WORKDIR}/hello_cpp ${D}/usr/bin
}
