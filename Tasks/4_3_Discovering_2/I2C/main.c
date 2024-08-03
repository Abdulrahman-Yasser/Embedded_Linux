#include "my_i2c.h"

const char* i2c_device = "/dev/i2c-3";
const char rpi_address = 0x3f;

uint8_t bits = 8;

int main(){
    int temp;
    float i2c_temp_degC;
    int16_t i2c_temperature;
    char temp_l, temp_h;

    int i2c_fd;
    
    unsigned char i2c_tx_buf[2];
    unsigned char i2c_rx_buf[2];

    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data msgset[1];

    i2c_fd = open(i2c_device, O_RDWR);
    if(i2c_fd < 0)
        printf("Can't open the file\n");
    
    i2c_tx_buf[0] = 0x01;
    
    msgs[0].addr = rpi_address;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = i2c_tx_buf;
    
    msgs[1].addr = rpi_address;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = i2c_rx_buf;

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 2;

    if(ioctl(i2c_fd, I2C_RDWR, &msgset) < 0){
        printf("ioctl(I2C_RDWR) in i2c_read\n");
    }

    if(i2c_rx_buf[0] == 0xa0){
        printf("STTS22H Detected\n");
    }

    i2c_tx_buf[0] = 0x04;
    i2c_tx_buf[1] = 0x0c;

    msgs[0].addr = rpi_address;
    msgs[0].flags = 0;
    msgs[0].len = 2;
    msgs[0].buf = i2c_tx_buf;
    
    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 1;

    if(ioctl(i2c_fd, I2C_RDWR, &msgset) < 0){
        printf("ioctl(I2C_RDWR) in i2c_write\n");
    }

    while(1){
        
        /* read temperature from the I2C sensor */
        i2c_tx_buf[0] = 0x06;

        msgs[0].addr = rpi_address;
        msgs[0].flags = 0;
        msgs[0].len = 1;
        msgs[0].buf = i2c_tx_buf;

        msgs[1].addr = rpi_address;
        msgs[1].flags = I2C_M_RD;
        msgs[1].len = 2;
        msgs[1].buf = i2c_rx_buf;

        msgset[0].msgs = msgs;
        msgset[0].nmsgs = 2;

        if(ioctl(i2c_fd, I2C_RDWR, &msgset) < 0){
            printf("ioctl(I2C_RDWR) in i2c_read");
        }

        i2c_temperature = i2c_rx_buf[1] << 8 | i2c_rx_buf[0];
        i2c_temp_degC = (float) i2c_temperature / 100;

        printf("I2C Temp Deg C %4.2f", i2c_temp_degC);

        usleep(1000000);
    }
    
}
