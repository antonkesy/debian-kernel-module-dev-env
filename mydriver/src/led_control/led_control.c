#include "./led_control.h"

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/kthread.h>
#include <linux/module.h>

#define MODE_ON_STR "on"
#define MODE_OFF_STR "off"
#define MODE_BLINK_STR "blink"

static char *led_green = MODE_OFF_STR;
module_param(led_green, charp, S_IRUGO);
MODULE_PARM_DESC(led_green_param, "Set Green LED on/off/blink");

static char *led_red = MODE_OFF_STR;
module_param(led_red, charp, S_IRUGO);
MODULE_PARM_DESC(led_red_param, "Set Red LED on/off/blink");

struct LEDInfo {
	enum LED gpio;
	enum Mode mode;
	unsigned long blink_interval_ms;
	struct task_struct *task;
	bool running;
};

#define DEFAULT_BLINK_INTERVAL_MS 1000
static struct LEDInfo led_green_info = { .gpio = GREEN,
					 .mode = OFF,
					 .blink_interval_ms =
						 DEFAULT_BLINK_INTERVAL_MS,
					 .task = NULL,
					 .running = false };
static struct LEDInfo led_red_info = { .gpio = RED,
				       .mode = OFF,
				       .blink_interval_ms =
					       DEFAULT_BLINK_INTERVAL_MS,
				       .task = NULL,
				       .running = false };

static bool init_led_gpio(int gpio, const char *name);
static void exit_led_info(struct LEDInfo *led_info);
static int blink_led_thread(void *arg);
static struct LEDInfo *get_led_info(enum LED led);

int led_init(void)
{
	printk(KERN_INFO "Green LED: %s\n", led_green);
	printk(KERN_INFO "Red LED: %s\n", led_red);

	init_led_gpio(GREEN, "led_green");
	init_led_gpio(GREEN, "led_red");
	set_led(RED, str_to_mode(led_green));
	set_led(RED, str_to_mode(led_red));

	return 0;
}

void led_exit(void)
{
	exit_led_info(&led_green_info);
	exit_led_info(&led_red_info);
}

static bool init_led_gpio(int gpio, const char *name)
{
	int ret;
	ret = gpio_request(gpio, name);
	if (ret) {
		printk(KERN_ERR "Failed to request GPIO for %s\n", name);
		return false;
	}

	ret = gpio_direction_output(gpio, 0);
	if (ret) {
		printk(KERN_ERR "Failed to set GPIO direction for %s\n", name);
		gpio_free(gpio);
		return false;
	}

	return true;
}

void exit_led_info(struct LEDInfo *led_info)
{
	if (led_info->task && led_info->running) {
		led_info->running = false;
		kthread_stop(led_info->task);
	}
	set_led(led_info->gpio, OFF);
	gpio_free(led_info->gpio);
}

bool is_led_on(enum LED led)
{
	// low active
	return !gpio_get_value(led);
}

enum Mode get_led_mode(enum LED led)
{
	return get_led_info(led)->mode;
}

void set_led(enum LED led, enum Mode mode)
{
	printk(KERN_INFO "Setting LED %d to %s\n", led, mode_to_str(mode));
	struct LEDInfo *led_info = get_led_info(led);
	led_info->mode = mode;

	// always stop the blink thread
	if (led_info->task && led_info->running) {
		led_info->running = false;
		kthread_stop(led_info->task);
		led_info->task = NULL;
	}

	switch (mode) {
	case ON:
		gpio_set_value(led, false); // low active
		return;
	case OFF:
		gpio_set_value(led, true); // low active
		return;
	case BLINK:
		if (!led_info->task) {
			led_info->running = true;
			led_info->task = kthread_run(blink_led_thread, led_info,
						     "blink_led_thread_%d",
						     led);
			if (IS_ERR(led_info->task)) {
				printk(KERN_ERR "Failed to create "
						"thread for LED %d\n",
				       led);
				led_info->task = NULL;
			}
		}
		return;
	}
}

static int blink_led_thread(void *arg)
{
	struct LEDInfo *led_info = (struct LEDInfo *)arg;
	while (!kthread_should_stop() && led_info->running) {
		// low active
		gpio_set_value(led_info->gpio, is_led_on(led_info->gpio));
		msleep(led_info->blink_interval_ms / 2);
	}
	led_info->running = false;
	return 0;
}

enum Mode bool_to_mode(bool set_on)
{
	return set_on ? ON : OFF;
}

/***
 * Convert string to Mode
 * Cuts off str and checks only the minimal required characters
 * "on" == "online" == "onasdf"
 * @param str
 * @return
 */
enum Mode str_to_mode(const char *str)
{
	if (str == NULL) {
		return OFF;
	}
	if (strncmp(str, MODE_ON_STR, strlen(MODE_ON_STR)) == 0) {
		return ON;
	}
	if (strncmp(str, MODE_OFF_STR, strlen(MODE_OFF_STR)) == 0) {
		return OFF;
	}
	if (strncmp(str, MODE_BLINK_STR, strlen(MODE_BLINK_STR)) == 0) {
		return BLINK;
	}
	return OFF;
}

char *mode_to_str(enum Mode mode)
{
	switch (mode) {
	case ON:
		return MODE_ON_STR;
	case OFF:
		return MODE_OFF_STR;
	case BLINK:
		return MODE_BLINK_STR;
	}
	printk(KERN_ERR "mode_to_str: Unknown mode %d\n", mode);
	return "unknown";
}

static struct LEDInfo *get_led_info(enum LED led)
{
	switch (led) {
	case GREEN:
		return &led_green_info;
	case RED:
		return &led_red_info;
	}
	printk(KERN_ERR "get_led_info: Unknown LED %d\n", led);
	return NULL;
}

void set_led_blink_timeout(enum LED led, unsigned long timeout)
{
	get_led_info(led)->blink_interval_ms = timeout;
}

unsigned long get_led_blink_timeout(enum LED led)
{
	return get_led_info(led)->blink_interval_ms;
}
