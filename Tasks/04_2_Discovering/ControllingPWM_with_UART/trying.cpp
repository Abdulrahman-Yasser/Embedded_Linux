#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


enum this_is_1{
    tamam,
    ok,
    mwafeq
};


enum this_is_2{
    msh_tamam,
    msh_ok,
    msh_mwafeq
};


int main(){
    this_is_2 the_2;
    this_is_1 the_1;
    std::cout << std::is_same<the_1, the_2>::value << std::endl;
			// if (std::is_same<T, TermioWrapper::C_flags>::value)
			// {
			// 	printf("\nYou Entered C_flag");
			// 	the_terminals_settings.c_cflag &= ~(the_flag);
			// 	the_terminals_settings.c_cflag |= (the_flag & Enable_or_value);
			// }
}