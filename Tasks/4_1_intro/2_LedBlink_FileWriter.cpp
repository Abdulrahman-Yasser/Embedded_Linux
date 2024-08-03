#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

int write_to_file(int data, std::string path){
    std::ofstream fout;
    fout.open(path);
    while(fout){
        fout << data;
        return 1;
    }
    return 0;
}

int main(){
    int err_chk;
    /* Exporting the pin */
    err_chk = write_to_file(20, "/sys/class/gpio/export");
    if(err_chk == 0){
        std::cout << "\nCan't even initialize the pin with writing 20 into export";
        return 0;
    }

    while(1){
        write_to_file(1, "/sys/class/gpio/export/gpio20/value");
        sleep(1);
        write_to_file(0, "/sys/class/gpio/export/gpio20/value");
        sleep(1);
    }
}
