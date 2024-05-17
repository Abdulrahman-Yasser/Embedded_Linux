#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xdc658e53, "module_layout" },
	{ 0x954ad0e5, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x6f021618, "class_destroy" },
	{ 0xbfeffc40, "device_destroy" },
	{ 0xfe990052, "gpio_free" },
	{ 0x6ac65f80, "gpiod_direction_input" },
	{ 0xb04bee0c, "gpiod_direction_output_raw" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xfbb5c7d2, "cdev_add" },
	{ 0xf4837e68, "cdev_init" },
	{ 0xef4c61eb, "device_create" },
	{ 0xe05148c6, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xee6fef60, "gpiod_get_raw_value" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0xdb2a7382, "gpiod_set_raw_value" },
	{ 0x9713980d, "gpio_to_desc" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x92997ed8, "_printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "3FA363A815256ACE498F811");
