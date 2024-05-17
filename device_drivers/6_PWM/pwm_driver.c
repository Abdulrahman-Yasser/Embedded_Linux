#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#include <linux/pwm.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abdulrahman Yasser ");
MODULE_DESCRIPTION("A simple driver to access the Hardware PWM.");  



/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;


#define DRIVER_NAME "my_pwm_driver"
#define DRIVER_CLASS "my_pwm_class"



/* Buffer for data */
struct pwm_device *pwm0 = NULL;
u32 pwm_on_time = 500000000;

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
	
	/* Set PWM on time */
	if(value < 'a' || value > 'j'){
		printk("Invalid Value!\n");
	}else{
		pwm_config(pwm0, 1000000000 * (value - 'a'), 1000000000);
	}

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
	
	pwm0 = pwm_request(0, "my-pwm");
	if(pwm0 == NULL){
		printk("Could not get PWM!\n");
		goto AddError;
	}

	pwm_config(pwm0, pwm_on_time, 1000000000);
	pwm_enable(pwm0);

	return 0;	
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
	pwm_disable(pwm0);
	pwm_free(pwm0);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

