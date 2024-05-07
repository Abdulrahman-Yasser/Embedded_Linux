#include <iostream>


enum class a_try : char{
    a_1=1, a_2=2, a_3=3
};


int main(){
    a_try first, sec, thrd;
    first = a_try::a_1;
    sec = a_try::a_2;
    thrd = a_try::a_3;

    std::cout << (unsigned char)first << "-" << std::endl;
    std::cout << (unsigned char)sec << std::endl;
    std::cout << (unsigned char)thrd << std::endl;

    char z = (unsigned char)first + (unsigned char)sec;
    std::cout << z << std::endl;
}