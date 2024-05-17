#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#include <linux/gpio.h>

#include <linux/delay.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abdulrahman Yasser ");
MODULE_DESCRIPTION("A simple driver for LCD display HD44780 - All you need to do is writing to the specified file in /dev \n"); 



/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;


#define DRIVER_NAME "my_lcd_driver"
#define DRIVER_CLASS "my_lcd_class"

/* Buffer for data */
static char lcd_buffer[32];

unsigned int my_gpios[] = {
	3,
	2,
	4,
	17,
	27,
	22,
	10,
	9,
	11,
	5
};

#define REGISTER_SELECT my_gpios[1]

void lcd_enable(void){
	gpio_set_value(my_gpios[0], 1);
	msleep(5);
	gpio_set_value(my_gpios[0], 0);
}

static void lcd_send_byte(unsigned char data){
	int i;
	for(i = 0; i < 8; i++){
		gpio_set_value(my_gpios[i+2], (data >> i) & 0x01 );
	}
	lcd_enable();
	msleep(5);
}

void lcd_write_cmd(unsigned char data){
	gpio_set_value(REGISTER_SELECT, 0);
	lcd_send_byte(data);
}

void lcd_write_data(unsigned char data){
	gpio_set_value(REGISTER_SELECT, 1);
	lcd_send_byte(data);
}







/**
 * @brief Write data out of the buffer
 * */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs){
	int to_copy, not_copied, delta, i;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(lcd_buffer));

	/* Copy data from user */
	not_copied = copy_from_user(lcd_buffer, user_buffer, to_copy);
	
	/* Clearinng the display */
	lcd_write_cmd(0x01);
	

	/* Writing on the LCD */
	for(i = 0; i < to_copy && i < 16; i++){
		lcd_write_data( lcd_buffer[i] );
	}
	if(i != to_copy - 1){
		int j;
		lcd_write_cmd(0xC0);
		for(j = i; j < to_copy && j < 32; j++){
			lcd_write_data( lcd_buffer[j] );
		}
	}
	lcd_write_cmd(0x80);

	/* Calculate data */
	delta = to_copy - not_copied;
	return delta;
}


/**
 * @brief This function is called when the device file is opened
 * */
static int driver_open(struct inode *device_file, struct file *instance){
	printk("dev number - open was called\n");
	return 0;
}


/**
 * @brief This function is called when the device file is closed
 * */
static int driver_close(struct inode *device_file, struct file *instance){
	printk("dev number - close was called\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.write = driver_write
};



/** 
 * @brief This function is called when the module is loaded into the kernel
 * */
static int __init ModuleInit(void){
	int i;
	char* pins_names[] = {"ENABLE_PIN", "REGISTER_SELECT", "DATA_PIN_0", "DATA_PIN_1","DATA_PIN_2","DATA_PIN_3","DATA_PIN_4","DATA_PIN_5","DATA_PIN_6","DATA_PIN_7","DATA_PIN_8"};
	printk("Hello, kernel\n");
	
	/* Allocate a device number */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0){
		printk("Device Nr - Could not be allocated");
		return -1;
	}
	printk("Character device number allocated - Device Nr - Major : %d, Minor : %d was registered!\n", my_device_nr >> 20, my_device_nr & 0xfffff);

	/* First create a virtual device class, The file will appear in /dev/class */
	/* Create device class */
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL){
		printk("Device class can not be created!\n");
		goto ClassError;
	}
	
	/* Device will appear in /sys/devices/virtual/class_name/device_name and /sys/device_name and /proc/modules */
	/* Create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL){
		printk("Can not create device file\n");
		goto FileError;
	}
	
	/* Initialize device file, adding character device */
	cdev_init(&my_device, &fops);

	/* Registering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1){
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}
	
	printk("LCD-driver - GPIO init\n");
	for(i = 0; i < 10; i++){
		if(gpio_request(my_gpios[i], pins_names[i])){
			printk("lcd-driver - Registering of device to kernel failed!\n");
			goto GpioInitError;
		}
	}

	printk("LCD-driver - Set GPIO to output\n");
	for(i = 0; i < 10; i++){
		if(gpio_direction_output(my_gpios[i], 0)){
			printk("lcd-driver - Error setting GPIO %d to output!\n", my_gpios[i]);
			goto GpioDirectionError;
		}
	}
	
	/* Init the display */
	lcd_write_cmd(0x30);	/* Set the display for 8 bit data Interface*/
	
	lcd_write_cmd(0xf);	/* Turn display on, turn cursor on, set cursor blinking */

	lcd_write_cmd(0x1);	/* Clear display */

	char text[] = "Hello there!";
	for(i = 0; i < sizeof(text)-1; i++){
		lcd_write_cmd(text[i]);
	}
	
	return 0;



GpioDirectionError:
	i = 9;

GpioInitError:
	for(;i >= 0; i--){
		gpio_free(my_gpios[i]);
	}

AddError:
	device_destroy(my_class, my_device_nr);

FileError:
	class_destroy(my_class);

ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;

}


/**
 * @brief This function is called when the module is removed from the kernel
 * */

static void __exit ModuleExit(void){
	int i;
	lcd_write_cmd(0x01);
	for(i = 0; i < 10; i++){
		gpio_set_value(my_gpios[i], 0);
		gpio_free(my_gpios[i]);
	}
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

