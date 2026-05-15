#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abdulrahman Yasser ");
MODULE_DESCRIPTION("A hello world LKM");

int callback_to_update(const char *val, const struct kernel_param *kp);

int my_value = 0;

/* @brief This function is called when the module is loaded into the kernel */
static int __init ModuleInit(void){
	printk("HEllo, kernel\n");
	panic("KERNEL IS PANICING, AAAAAAH");
	return 0;
}

int callback_to_update(const char *val, const struct kernel_param *kp)
{
	int res = param_set_int(val, kp);
	if(res == 0)
	{
		printk(KERN_INFO "New value has been written %d\n", my_value);
		return 0;
	}
	return -1;
}

const struct kernel_param_ops my_param_ops = {
	.set = &callback_to_update,
	.get = &param_get_int,
};

module_param_cb(my_value, &my_param_ops, &my_value, S_IRUGO|S_IWUSR );

/* @brief This function is called when the module is removed from the kernel */

static void __exit ModuleExit(void){
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

