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
#include <linux/err.h>

#include <linux/proc_fs.h>


#define LINUX_KERNEL_VERSION 515

#define WR_VALUE _IOW('a', 'a', int32_t*)
#define RD_VALUE _IOR('a', 'b', int32_t*)

int32_t value = 0;
char etx_array[20]="trying_procfs\n";
static int len = 1;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static struct proc_dir_entry *parent;

/*********** Driver functions ******************/
static int etx_open(struct inode *inode, struct file* file);
static int etx_release(struct inode *inode, struct file* file);
static ssize_t etx_read(struct file* filep, char __user *buf, size_t len, loff_t *off);
static ssize_t etx_write(struct file* filep, const char *buf, size_t len, loff_t *off);
static long etx_ioctl(struct file* file, unsigned int cmd, unsigned long arg);

/************ Procfs Functions ******************/
static int open_proc(struct inode *inode, struct file *file);
static int release_proc(struct inode *inode, struct file *file);
static ssize_t read_proc(struct file *filep, char __user *buffer, size_t length, loff_t* offset);
static ssize_t write_proc(struct file *filep, const char *buffer, size_t length, loff_t* off);


/* Entry and Exit */
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);


/* File operationn structure */
static struct proc_ops proc_fops = {	
	.proc_open = open_proc,
	.proc_read = read_proc,
	.proc_write = write_proc,
	.proc_release = release_proc
};

/* procfs operation structure */
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = etx_read,
	.write = etx_write,
	.open = etx_open,
	.unlocked_ioctl = etx_ioctl,
	.release = etx_release
};


static int etx_open(struct inode *inode, struct file* file){
	pr_info("Device file Opened...!!\n");
	return 0;
}

static int etx_release(struct inode *inode, struct file* file){
	pr_info("Device file Closed...!!\n");
	return 0;
}

static ssize_t etx_read(struct file* filep, char __user *buf, size_t len, loff_t *off){
	pr_info("Read function\n");
	return 0;
}

static ssize_t etx_write(struct file* filep, const char *buf, size_t len, loff_t *off){
	pr_info("write function\n");
	return len;
}


static long etx_ioctl(struct file* file, unsigned int cmd, unsigned long arg){
	switch(cmd){
		case WR_VALUE:
			if(copy_from_user(&value, (int32_t*) arg, sizeof(value))){
					pr_err("Data write : ERR!..\n");
			}
			pr_info("Value = %d\n", value);
			break;
		case RD_VALUE:
			if(copy_to_user((int32_t*) arg,&value, sizeof(value))){
					pr_err("Data write : ERR!..\n");
			}
			break;
		default:
			pr_info("Default\n");
	}
	return 0;
}


static int open_proc(struct inode *inode, struct file *file){
	pr_info("File created in /proc\n");
	return 0;
}

static int release_proc(struct inode *inode, struct file *file){
	pr_info("File closed\n");
	return 0;
}

static ssize_t read_proc(struct file *filep, char __user *buffer, size_t length, loff_t* offset){
	pr_info("proc file read.....\n");
	if(len){
		len = 0;
	}else{
		len = 1;
		return 0;
	}
	if(copy_to_user(buffer, etx_array, 20)){
		pr_err("Data Send : Error!\n");
	}
	return length;
}

static ssize_t write_proc(struct file *filep, const char *buffer, size_t length, loff_t* off){
	pr_info("Writing to /proc\n");
	if(copy_from_user(etx_array, buffer, length)){
		pr_err("Err while writin\n");
	}
	return length;
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

	parent = proc_mkdir("etx", NULL);

	if(parent == NULL){
		pr_info("Error creating proc entry\n");
		goto r_device;
	}

	proc_create("etx_proc", 0666, parent, &proc_fops);
        pr_info("Device Driver Insert ...Done!!!\n");


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
	proc_remove(parent);
	
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

