#include "TermioWrapper.hpp"
#include <string>
#include <iostream>
#include <unistd.h>
int main(){
    struct termios my_termios;
    my_termios.c_cflag = 0;
    my_termios.c_iflag = 0;
    my_termios.c_oflag = 0;
    my_termios.c_lflag = 0;
    std::string the_path = "/dev/ttyUSB0";
    my_wrapper::TermioWrapper term_on_pc(the_path, my_termios);

    // Control Flags
    char i[255] = "";



    term_on_pc.change_configurations<my_wrapper::TermioWrapper::C_flags>(my_wrapper::TermioWrapper::C_flags::C_READ , 1);
    term_on_pc.change_configurations<my_wrapper::TermioWrapper::C_flags>(my_wrapper::TermioWrapper::C_flags::C_LOCAL , 1);
    term_on_pc.change_configurations<my_wrapper::TermioWrapper::C_flags>(my_wrapper::TermioWrapper::C_flags::C_S8 , 1);
    term_on_pc.change_configurations<my_wrapper::TermioWrapper::Speed_Options>(my_wrapper::TermioWrapper::Speed_Options::B_9600 , 1);

    term_on_pc.change_configurations<my_wrapper::TermioWrapper::I_flags>(my_wrapper::TermioWrapper::I_flags::I_IGNPAR , 1);


    term_on_pc.write_configuration_buffer();


    std::string transmitting = "From Pc : ";

    while(1){
        std::cout << "Enter the data to transmit to the other device !\n";
        std::getline(std::cin, transmitting);
        term_on_pc.sent_data(transmitting.c_str());
        std::cout << "Enter the data from the other device to read it !\n";
        sleep(10);
        term_on_pc.read_data(i);
    }





    // term_on_pc.change_configurations<my_wrapper::TermioWrapper::C_flags>(my_wrapper::TermioWrapper::C_flags::C_PARENB, 0);
    // term_on_pc.change_configurations<my_wrapper::TermioWrapper::C_flags>(my_wrapper::TermioWrapper::C_flags::C_STOPB, 0);
    // term_on_pc.change_configurations<my_wrapper::TermioWrapper::C_flags>(my_wrapper::TermioWrapper::C_flags::C_S8, 0);
    // // term_on_pc.change_configurations<my_wrapper::TermioWrapper::C_flags>(my_wrapper::TermioWrapper::C_flags::C_RTSCTS, 0);
    // term_on_pc.change_configurations<my_wrapper::TermioWrapper::C_flags>(my_wrapper::TermioWrapper::C_flags::C_LOCAL, 1);
    // term_on_pc.change_configurations<my_wrapper::TermioWrapper::C_flags>(my_wrapper::TermioWrapper::C_flags::C_READ, 1);

    // term_on_pc.change_configurations<my_wrapper::TermioWrapper::I_flags>(my_wrapper::TermioWrapper::I_flags::I_IXANY , 0);
    // term_on_pc.change_configurations<my_wrapper::TermioWrapper::I_flags>(my_wrapper::TermioWrapper::I_flags::I_IXOFF , 0);
    // term_on_pc.change_configurations<my_wrapper::TermioWrapper::I_flags>(my_wrapper::TermioWrapper::I_flags::I_IXON, 0);
    // term_on_pc.change_configurations<my_wrapper::TermioWrapper::I_flags>(my_wrapper::TermioWrapper::I_flags::I_ICRNL, 0);
    
}
