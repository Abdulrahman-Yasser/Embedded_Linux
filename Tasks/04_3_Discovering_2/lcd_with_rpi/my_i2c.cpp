#include "my_i2c.hpp"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
// #include <getopt.h>
#include <fcntl.h>
#include <time.h>

#include <sys/ioctl.h>
#include <linux/ioctl.h>
// #include <sys/stat.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

my_i2c::my_i2c(std::string file_path, unsigned int file_permissions):
i2c_path(file_path)
{
    i2c_fd = open(file_path.c_str(), file_permissions);
    if (i2c_fd < 0)
    {
        printf("Can't open the file\n");
        i2c_error_state = my_i2c::error_state::opening_error;
    }
    i2c_error_state = my_i2c::error_state::no_error;
}

int my_i2c::write_on_i2c(unsigned int address, char *data, char data_length)
{
    if(i2c_error_state != my_i2c::error_state::no_error){
        printf("An error occured once\n");
        return -1;
    }
    struct i2c_msg msgs[1];
    struct i2c_rdwr_ioctl_data msgset[1];

    msgs[0].addr = (__u16)address;
    msgs[0].flags = (__u16)0;
    msgs[0].len = (__u16)data_length;
    msgs[0].buf = (__u8*)data;

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 1;

    if(ioctl(i2c_fd, I2C_RDWR, &msgset) < 0){
        printf("ioctl(I2C_RDWR) in i2c_read\n");
        i2c_error_state = my_i2c::error_state::writing_error;
        return -1;
    }
    return 0;
}

int my_i2c::read_from_i2c(unsigned int address, char *data, char data_length)
{
    if(i2c_error_state != my_i2c::error_state::no_error){
        printf("An error occured once\n");
        return -1;
    }

    struct i2c_msg msgs[1];
    struct i2c_rdwr_ioctl_data msgset[1];

    msgs[0].addr = (__u16)address;
    msgs[0].flags = (__u16)0;
    msgs[0].len = (__u16)data_length;
    msgs[0].buf = (__u8*)data;

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 1;

    if(ioctl(i2c_fd, I2C_RDWR, &msgset) < 0){
        printf("ioctl(I2C_RDWR) in i2c_read\n");
        i2c_error_state = my_i2c::error_state::read_error;
        return -1;
    }
    return 0;
}

int my_i2c::read_write_multiple_msgs(unsigned int address, struct i2c_msg *the_many_msgs, char how_many){

    struct i2c_rdwr_ioctl_data msgset[1];

    msgset[0].msgs = the_many_msgs;
    msgset[0].nmsgs = how_many;

    if(ioctl(i2c_fd, I2C_RDWR, &msgset) < 0){
        printf("ioctl(I2C_RDWR) in i2c_read");
        i2c_error_state = my_i2c::error_state::read_error;
        return -1;
    }
    return 0;
}
