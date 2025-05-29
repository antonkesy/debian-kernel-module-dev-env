#include "./sysfs_entry.h"
#include "../led_control/led_control.h"
#include "../mydriver.h"

#include <linux/gpio.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>

static ssize_t led_green_show(struct kobject *kobj, struct kobj_attribute *attr,
			      char *buf);
static ssize_t led_red_show(struct kobject *kobj, struct kobj_attribute *attr,
			    char *buf);
static ssize_t led_green_store(struct kobject *kobj,
			       struct kobj_attribute *attr, const char *buf,
			       size_t count);
static ssize_t led_red_store(struct kobject *kobj, struct kobj_attribute *attr,
			     const char *buf, size_t count);
static ssize_t led_green_blink_time_show(struct kobject *kobj,
					 struct kobj_attribute *attr,
					 char *buf);
static ssize_t led_red_blink_time_show(struct kobject *kobj,
				       struct kobj_attribute *attr, char *buf);
static ssize_t led_green_blink_time_store(struct kobject *kobj,
					  struct kobj_attribute *attr,
					  const char *buf, size_t count);
static ssize_t led_red_blink_time_store(struct kobject *kobj,
					struct kobj_attribute *attr,
					const char *buf, size_t count);

#define LED_ATTR_PERMISSIONS 0664
static struct kobj_attribute led_green_attribute = __ATTR(
	led_green, LED_ATTR_PERMISSIONS, led_green_show, led_green_store);
static struct kobj_attribute led_red_attribute =
	__ATTR(led_red, LED_ATTR_PERMISSIONS, led_red_show, led_red_store);
static struct kobj_attribute led_green_blink_time_attribute =
	__ATTR(led_green_blink_time, LED_ATTR_PERMISSIONS,
	       led_green_blink_time_show, led_green_blink_time_store);
static struct kobj_attribute led_red_blink_time_attribute =
	__ATTR(led_red_blink_time, LED_ATTR_PERMISSIONS,
	       led_red_blink_time_show, led_red_blink_time_store);

static struct attribute *attrs[] = {
	&led_green_attribute.attr,
	&led_red_attribute.attr,
	&led_green_blink_time_attribute.attr,
	&led_red_blink_time_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *mydriver_kobj;

int sysfs_entry_init(void)
{
	printk(KERN_INFO "Init /sys/kernel/%s entry\n", DRIVER_NAME);
	int ret;

	mydriver_kobj = kobject_create_and_add(DRIVER_NAME, kernel_kobj);
	if (!mydriver_kobj) {
		return -ENOMEM;
	}

	ret = sysfs_create_group(mydriver_kobj, &attr_group);
	if (ret) {
		kobject_put(mydriver_kobj);
	}

	return ret;
}

void sysfs_entry_exit(void)
{
	sysfs_remove_group(mydriver_kobj, &attr_group);
	kobject_put(mydriver_kobj);
}

static ssize_t led_show(enum LED led, char *buf)
{
	char *led_status;
	switch (get_led_mode(led)) {
	case ON:
		led_status = "[on] off blink";
		break;
	case OFF:
		led_status = "on [off] blink";
		break;
	case BLINK:
		led_status = "on off [blink]";
		break;
	}
	return sprintf(buf, "%s\n", led_status);
}

static ssize_t
led_green_show(struct kobject *kobj, // NOLINT(misc-unused-parameters)
	       struct kobj_attribute *attr, // NOLINT(misc-unused-parameters)
	       char *buf)
{
	return led_show(GREEN, buf);
}

static ssize_t
led_red_show(struct kobject *kobj, // NOLINT(misc-unused-parameters)
	     struct kobj_attribute *attr, // NOLINT(misc-unused-parameters)
	     char *buf)
{
	return led_show(RED, buf);
}

static ssize_t led_store(enum LED led, const char *buf, size_t count)
{
	set_led(led, str_to_mode(buf));
	return (ssize_t)count;
}

static ssize_t
led_green_store(struct kobject *kobj, // NOLINT(misc-unused-parameters)
		struct kobj_attribute *attr, // NOLINT(misc-unused-parameters)
		const char *buf, size_t count)
{
	return led_store(GREEN, buf, count);
}

static ssize_t
led_red_store(struct kobject *kobj, // NOLINT(misc-unused-parameters)
	      struct kobj_attribute *attr, // NOLINT(misc-unused-parameters)
	      const char *buf, size_t count)
{
	return led_store(RED, buf, count);
}

static ssize_t led_blink_time_show(enum LED led, char *buf)
{
	return sprintf(buf, "%lu\n", get_led_blink_timeout(led));
}

static ssize_t led_green_blink_time_show(
	struct kobject *kobj, // NOLINT(misc-unused-parameters)
	struct kobj_attribute *attr, // NOLINT(misc-unused-parameters)
	char *buf)
{
	return led_blink_time_show(GREEN, buf);
}

static ssize_t led_red_blink_time_show(
	struct kobject *kobj, // NOLINT(misc-unused-parameters)
	struct kobj_attribute *attr, // NOLINT(misc-unused-parameters)
	char *buf)
{
	return led_blink_time_show(RED, buf);
}

static ssize_t led_blink_time_store(enum LED led, const char *buf, size_t count)
{
	const short MAX_DIGITS = 10;
	unsigned long timeout;
	if (kstrtoul(buf, MAX_DIGITS, &timeout) == 0) {
		set_led_blink_timeout(led, timeout);
	}
	return (ssize_t)count;
}

static ssize_t led_green_blink_time_store(
	struct kobject *kobj, // NOLINT(misc-unused-parameters)
	struct kobj_attribute *attr, // NOLINT(misc-unused-parameters)
	const char *buf, size_t count)
{
	return led_blink_time_store(GREEN, buf, count);
}
static ssize_t led_red_blink_time_store(
	struct kobject *kobj, // NOLINT(misc-unused-parameters)
	struct kobj_attribute *attr, // NOLINT(misc-unused-parameters)
	const char *buf, size_t count)
{
	return led_blink_time_store(RED, buf, count);
}
