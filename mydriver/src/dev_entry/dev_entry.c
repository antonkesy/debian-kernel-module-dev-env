#include "./dev_entry.h"
#include "../led_control/led_control.h"
#include "../mydriver.h"

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define CLASS_NAME "chrdev"
#define USED_LED GREEN

static struct class *char_class = NULL;
static struct device *char_dev = NULL;

static int major = 0;

#define BUF_SIZE 1024
static size_t buffer_len = 0;
static char buffer[BUF_SIZE];
static char my_text[BUF_SIZE] =
	"I’ll tell you a DNS joke but be advised, it could take up to 24 hours "
	"for everyone to get it."
	"I'd tell you a UDP joke, but you might not get it."
	"I'll tell you a TCP joke. Do you want to hear it?";

#define IOCTL_MAGIC 'L'
#define IOCTL_SET_LED _IOW(IOCTL_MAGIC, 0, bool)
#define IOCTL_GET_LED_STATUS _IOR(IOCTL_MAGIC, 1, bool)

static int dev_open(struct inode *inodep, struct file *filep);
static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
			loff_t *offset);
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len,
			 loff_t *offset);
static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.unlocked_ioctl = dev_ioctl,
};

int dev_entry_init(void)
{
	printk(KERN_INFO "Init char reader for /dev/%s\n", DRIVER_NAME);

	major = register_chrdev(0, DRIVER_NAME, &fops);
	char_class = class_create(CLASS_NAME);
	char_dev = device_create(char_class, NULL, MKDEV(major, 0), NULL,
				 DRIVER_NAME);

	return 0;
}

void dev_entry_exit(void)
{
	device_destroy(char_class, MKDEV(major, 0));
	class_unregister(char_class);
	class_destroy(char_class);
	unregister_chrdev(major, DRIVER_NAME);
}

static int dev_open(struct inode *inodep, // NOLINT(misc-unused-parameters)
		    struct file *filep) // NOLINT(misc-unused-parameters)
{
	printk(KERN_INFO "Device opened\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, // NOLINT(misc-unused-parameters)
			char *buffer, size_t len, loff_t *offset)
{
	if (*offset >= BUF_SIZE) {
		return 0;
	}

	if (*offset + len > BUF_SIZE) {
		len = BUF_SIZE - *offset;
	}

	if (copy_to_user(buffer, my_text + *offset, len) != 0) {
		return -EFAULT;
	}

	*offset += len;
	return (ssize_t)len;
}

static ssize_t dev_write(struct file *filep, // NOLINT(misc-unused-parameters)
			 const char __user *user_buffer, size_t len,
			 loff_t *offset) // NOLINT(misc-unused-parameters)
{
	if (len > BUF_SIZE) {
		return -EINVAL;
	}

	ssize_t ret = (ssize_t)copy_from_user(buffer, user_buffer, len);
	if (ret != 0) {
		return -EFAULT;
	}

	buffer_len = len;
	printk(KERN_INFO "Received message: %.*s\n", (int)len, buffer);

	return (ssize_t)len;
}

static long
dev_ioctl(struct file *filep, // NOLINT(misc-unused-parameters)
	  unsigned int cmd, // NOLINT(bugprone-easily-swappable-parameters)
	  unsigned long arg)
{
	bool led_status;
	// NOLINTNEXTLINE(performance-no-int-to-ptr)
	bool __user *user_ptr = (bool __user *)arg;

	switch (cmd) {
	case IOCTL_SET_LED:
		if (copy_from_user(&led_status, user_ptr, sizeof(led_status))) {
			return -EFAULT;
		}
		set_led(USED_LED, bool_to_mode(led_status));
		break;
	case IOCTL_GET_LED_STATUS:
		led_status = is_led_on(USED_LED);
		if (copy_to_user(user_ptr, &led_status, sizeof(led_status))) {
			return -EFAULT;
		}
		printk(KERN_INFO "LED status: %s\n", led_status ? "on" : "off");
		break;
	default:
		return -EINVAL;
	}
	return 0;
}
