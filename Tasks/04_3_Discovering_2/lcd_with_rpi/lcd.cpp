

#include "lcd.hpp"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
// #include <getopt.h>
#include <fcntl.h>
#include <time.h>

#include <sys/ioctl.h>
#include <linux/ioctl.h>
// #include <sys/stat.h>
// #include <linux/types.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include "my_i2c.hpp"

namespace my_lcd{
    lcd::lcd(unsigned int address, my_i2c the_i2c, lcd_fnc the_function):
    lcd_address(address), the_used_i2c(the_i2c), my_current_functionality(the_function){
        lcd_error_state = error_state::no_error;
        _initialize_lcd();
    }

    int lcd::write_chars(std::string data){
        return _write_data(data);
    }

    int lcd::read_chars(std::string data){
        return 0;
    }

    int lcd::write_command(lcd_commands the_command){
        return _write_cmd((unsigned char)the_command);
    }

    int lcd::_initialize_lcd(void){
        int result = 0;
        char temp[1];
        /* It must be 4-bit mode,to fit the RS, En, RW and 4-bits data */
        usleep(50000);

        _write_cmd(0x33);
        usleep(2000);

        _write_cmd(0x32);
        usleep(2000);

        _write_cmd(0x28);
        usleep(2000);

        _write_cmd(0xe0);
        usleep(2000);

        _write_cmd(0x01);
        usleep(2000);

        _write_cmd(0x06);
        usleep(2000);

        _write_cmd(0x01);
        usleep(2000);

        if(result != 0)
            lcd_error_state = error_state::opening_error;

        return result;
    }

    int lcd::_write_cmd(unsigned char cmd){
        if(lcd_error_state != error_state::no_error){
            return -1;
        }
        unsigned char dataU, dataL;
        char data_t[4];
        int result;

        dataU = cmd & 0xf0;
        dataL = (cmd << 4)&0xf0;
        data_t[0] = dataU | 0x0C;  // en =1, rs =0
        data_t[1] = dataU | 0x08;  // en =0, rs =0
        data_t[2] = dataL | 0x0C;  // en =1, rs =0
        data_t[3] = dataL | 0x08;  // en =0, rs =0

        result = the_used_i2c.write_on_i2c(lcd_address, data_t, 1);
        usleep(1);
        result = the_used_i2c.write_on_i2c(lcd_address, &data_t[1], 1);
        if(result < 0)
            lcd_error_state = error_state::writing_cmd_error;
        usleep(100);

        result = the_used_i2c.write_on_i2c(lcd_address, &data_t[2], 1);
        usleep(1);
        result = the_used_i2c.write_on_i2c(lcd_address, &data_t[3], 1);
        if(result < 0)
            lcd_error_state = error_state::writing_cmd_error;
        usleep(100);

        return result;
    }

    int lcd::_write_data(std::string data){
        if(lcd_error_state != error_state::no_error){
            return -1;
        }

        int i = 0, result = 0;

        for(int i = 0; i < data.length(); i++){
            unsigned char dataU, dataL;
            char data_t[4];
            dataU = data[i] & 0xf0;
            dataL = (data[i] << 4)&0xf0;
            data_t[0] = dataU | 0x0D;  // en =1, rs =1
            data_t[1] = dataU | 0x09;  // en =0, rs =1
            data_t[2] = dataL | 0x0D;  // en =1, rs =1
            data_t[3] = dataL | 0x09;  // en =0, rs =1

            result |= the_used_i2c.write_on_i2c(lcd_address, data_t, 1);
            usleep(1);
            result |= the_used_i2c.write_on_i2c(lcd_address, &data_t[1], 1);
            usleep(1);
            result |= the_used_i2c.write_on_i2c(lcd_address, &data_t[2], 1);
            usleep(1);

            result |= the_used_i2c.write_on_i2c(lcd_address, &data_t[3], 1);
            usleep(100);

        }

        if(result != 0)
            lcd_error_state = error_state::writing_data_error;

        return result;
    }

    int lcd::_read_cmd(void){
        return -1;
    }

    int lcd::_read_data(void){
        return -1;
    }


}