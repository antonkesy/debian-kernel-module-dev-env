#include <linux/init.h>
#include <linux/module.h>
#include "mydriver.h"

MODULE_LICENSE("MIT");
MODULE_AUTHOR("Anton Kesy");
MODULE_DESCRIPTION("mydriver");
MODULE_VERSION("1.0");

static int __init mydriver_init(void)
{
	int ret;
	printk(KERN_INFO "Start %s!\n", DRIVER_NAME);

	return ret;
}

static void __exit mydriver_exit(void)
{
	printk(KERN_INFO "Stop %s!\n", DRIVER_NAME);
}

module_init(mydriver_init);
module_exit(mydriver_exit);
