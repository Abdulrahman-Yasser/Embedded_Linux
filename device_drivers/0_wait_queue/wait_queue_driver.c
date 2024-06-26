#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#include <linux/kthread.h>
#include <linux/wait.h>

#include <linux/err.h>


uint32_t write_count = 0;
uint32_t read_count = 0;
static struct task_struct *wait_thread;

DECLARE_WAIT_QUEUE_HEAD(wait_queue_etx);

#define WR_VALUE _IOW('a', 'a', int32_t*)
#define RD_VALUE _IOR('a', 'b', int32_t*)

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
int wait_queue_flag = 0;


/*********** Driver functions ******************/
static int etx_open(struct inode *inode, struct file* file);
static int etx_release(struct inode *inode, struct file* file);
static ssize_t etx_read(struct file* filep, char __user *buf, size_t len, loff_t *off);
static ssize_t etx_write(struct file* filep, const char *buf, size_t len, loff_t *off);

/* Entry and Exit */
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

/* procfs operation structure */
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = etx_read,
	.write = etx_write,
	.open = etx_open,
	.release = etx_release
};

static int wait_function(void* unused){
	while(1){
		pr_info("Waiting For Event ...!\n");
		wait_event_interruptible(wait_queue_etx, wait_queue_flag != 0);
		switch(wait_queue_flag){
		case 1:
			pr_info("Event Came from Read fnuction - %d\n", ++read_count);
			break;
		case 2:
			pr_info("Event Came from Write fnuction - %d\n", ++write_count);
			break;
		case 3:
			pr_info("Event Came From Exit Function \n");
			return 0;
		default:
			pr_info("Default choices - Not in the cases \n");
		}
		wait_queue_flag = 0;
	}
	do_exit(0);
	return 0;
}

static int etx_open(struct inode *inode, struct file* file){
	pr_info("Device file Opened...!!\n");
	return 0;
}

static int etx_release(struct inode *inode, struct file* file){
	pr_info("Device file Closed...!!\n");
	return 0;
}

static ssize_t etx_read(struct file* filep, char __user *buf, size_t len, loff_t *off){
	wait_queue_flag = 1;
	wake_up_interruptible(&wait_queue_etx);
	pr_info("Read function\n");
	return 0;
}

static ssize_t etx_write(struct file* filep, const char *buf, size_t len, loff_t *off){
	wait_queue_flag = 2;	
	wake_up_interruptible(&wait_queue_etx);
	pr_info("Write function ");
	return len;
}

static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if(IS_ERR(dev_class = class_create(THIS_MODULE,"etx_class"))){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device"))){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
        pr_info("Device Driver added in /dev !!!\n");
	
	wait_thread = kthread_create(wait_function, NULL, "WaitThread");
	
	if(wait_thread){
		pr_info("Thread Created successfully\n");
		wake_up_process(wait_thread);
	}else{
		pr_info("Thread creation failed\n");
	}
	pr_info("Device Driver Insert ... Done!!!\n");
	return 0;
	
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;

}

/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
	wait_queue_flag = 2;
	wake_up_interruptible(&wait_queue_etx);
	device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}



module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("Simple Linux device driver (IOCTL)");

