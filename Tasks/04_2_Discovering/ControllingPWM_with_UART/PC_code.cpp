#include "TermioWrapper.hpp"
#include <string>

int main(){
    std::string the_path = "/home/abdu/Study/Embedded_Linux/Tasks/04_2_Discovering/ControllingPWM_with_UART/test.txt";
    my_wrapper::TermioWrapper term_on_pc(the_path);
    term_on_pc.change_configurations(TermioWrapper::C_flags::C_PARENB, 0);
    term_on_pc.change_configurations(TermioWrapper::C_flags::C_STOPB, 0);
    term_on_pc.change_configurations(TermioWrapper::C_flags::C_S8, 0);
    term_on_pc.change_configurations(TermioWrapper::C_flags::C_RTSCTS, 0);
    term_on_pc.change_configurations(TermioWrapper::C_flags::C_READ | TermioWrapper::C_flags::C_LOCAL, 0);

    term_on_pc.change_configurations(TermioWrapper::I_flags::I_IXOFF | TermioWrapper::I_flags::I_IXON | TermioWrapper::I_flags::I_IXANY , 0);
    term_on_pc.change_configurations(TermioWrapper::I_flags::I_ICRNL, 0);

}