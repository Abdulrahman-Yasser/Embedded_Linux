#ifndef MY_I2C_HPP
#define MY_I2C_HPP


#include <string>
#include <linux/i2c.h>

class my_i2c
{
public:
    my_i2c(std::string file_path, unsigned int file_permissions);
    int write_on_i2c(unsigned int address, char *data, char data_length);
    int read_from_i2c(unsigned int address, char *data, char data_length);
    int read_write_multiple_msgs(unsigned int address, struct i2c_msg *the_many_msgs, char how_many);

private:

    enum class i2c_flags
    {
        i2c_read = I2C_M_RD,
        i2c_10_address_mode = I2C_M_TEN,
        i2c_DMA_safe = I2C_M_DMA_SAFE,
        i2c_receive_length = I2C_M_RECV_LEN,
        i2c_no_read_ack = I2C_M_NO_RD_ACK,
        i2c_ignore_nack = I2C_M_IGNORE_NAK,
        i2c_rev_dir_address = I2C_M_REV_DIR_ADDR,
        i2c_no_start = I2C_M_NOSTART,
        i2c_send_stop = I2C_M_STOP
    };

    enum class error_state
    {
        no_error,
        writing_error,
        read_error,
        opening_error
    };

    error_state i2c_error_state;
    int i2c_fd;
    std::string i2c_path;
};

#endif