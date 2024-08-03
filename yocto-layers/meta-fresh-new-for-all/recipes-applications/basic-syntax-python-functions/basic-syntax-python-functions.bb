SUMMARY = "hello world"
LICENSE = "CLOSED"

python do_an_entry() { 
    bb.plain("hello")
}

addtask an_entry before do_compile after do_unpack
