#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#include <linux/i2c.h>
#include <linux/kernel.h>

static struct i2c_adapter* my_i2c_adapter = NULL;
static struct i2c_client* my_i2c_client = NULL;


/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abdulrahman Yasser ");
MODULE_DESCRIPTION("A simple gpio driver for Using an I2C in both read and write!\n"); 
MODULE_SUPPORTED_DEVICE("None");


/* Defines for device identification */
#define I2C_BUS_AVAILABLE	1	/* The I2C Bus available on the raspberry */ 
#define SLAVE_DEVICE_NAME	"GPIO_EXPANDER"	/* Device and Driver Name */
#define SLAVE_ADDRESS		0x27		/* I2C slave address */

static const struct i2c_device_id my_i2c_id[] = {
	{SLAVE_ADDRESS, 0},
	{}
};

static struct i2c_driver my_i2c_driver = {
	.driver = {
		.name = SLAVE_DEVICE_NAME,
		.owner = THIS_MODULE
	}
};

static struct i2c_board_info my_i2c_board_info = {
	I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SLAVE_ADDRESS)
};



/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;


#define DRIVER_NAME "i2c_gpio_expander_device"
#define DRIVER_CLASS "i2c_gpio_expander_class"

/* Buffer for data */
static char buffer[255];
static int buffer_pointer;

/**
 * @brief Read data out of the buffer
 * */
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs){
	int to_copy, not_copied, delta;
	char tmp[3] = "\n";

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(tmp));

	/* Read value of button */
	printk("Value of button : %d\n", gpio_get_value(17));
	tmp[0] = gpio_get_value(17) + '0';

	/* Copy data from user */
	not_copied = copy_to_user(user_buffer, &tmp, to_copy);
	
	/* Calculate data */
	delta = to_copy - not_copied;
	return delta;
}

/**
 * @brief Write data out of the buffer
 * */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs){
	int to_copy, not_copied, delta;
	char value;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(value));

	/* Copy data from user */
	not_copied = copy_from_user(&value, user_buffer, to_copy);
	
	/* Setting the LED */
	switch(value){
		case '0':
			gpio_set_value(4, 0);
			break;
		case '1':
			gpio_set_value(4, 1);
			break;
		defaule:
			printk("Invalue Input!\n");
			break;
	};

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
	.read = driver_read,
	.write = driver_write
};



/** 
 * @brief This function is called when the module is loaded into the kernel
 * */
static int __init ModuleInit(void){
	int retval;
	printk("HEllo, kernel\n");
	
	/* Allocate a device number */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0){
		printk("Device Nr - Could not be allocated");
		return -1;
	}
	printk("read_write - Device Nr - Major : %d, Minor : %d was registered!\n", my_device_nr >> 20, my_device_nr & 0xfffff);

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
	
	
	return 0;

Gpio17Error:
	gpio_free(17);
Gpio4Error:
	gpio_free(4);
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
	gpio_set_value(4, 0);
	gpio_free(17);
	gpio_free(4);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

