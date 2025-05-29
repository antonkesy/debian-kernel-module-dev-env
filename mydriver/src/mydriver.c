#include "./dev_entry/dev_entry.h"
#include "./led_control/led_control.h"
#include "./mydriver.h"
#include "./sysfs_entry/sysfs_entry.h"

#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Kesy");
MODULE_DESCRIPTION("mydriver: LED Control");
MODULE_VERSION("1.0");

static int __init mydriver_init(void)
{
	int ret;
	printk(KERN_INFO "Start %s!\n", DRIVER_NAME);

	ret = led_init();
	if (ret) {
		printk(KERN_ERR "Failed to initialize LED driver.\n");
		return ret;
	}

	ret = dev_entry_init();
	if (ret) {
		printk(KERN_ERR "Failed to initialize char_output driver.\n");
		return ret;
	}

	ret = sysfs_entry_init();
	if (ret) {
		printk(KERN_ERR "Failed to initialize sysfs entry.\n");
		return ret;
	}

	return ret;
}

static void __exit mydriver_exit(void)
{
	printk(KERN_INFO "Stop %s!\n", DRIVER_NAME);
	led_exit();
	// FIXME: dev + sysfs sometimes (refcount_t: underflow; use-after-free)
	dev_entry_exit();
	sysfs_entry_exit();
}

module_init(mydriver_init);
module_exit(mydriver_exit);
