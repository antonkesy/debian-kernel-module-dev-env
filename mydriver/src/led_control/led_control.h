#include <linux/types.h>

// Matches GPIO pin numbers
enum LED {
	RED = 13,
	GREEN = 14,
};

enum Mode {
	OFF = 0,
	ON = 1,
	BLINK = 2,
};

int led_init(void);
void led_exit(void);

bool is_led_on(enum LED led);
void set_led(enum LED led, enum Mode mode);
enum Mode get_led_mode(enum LED led);

void set_led_blink_timeout(enum LED led, unsigned long timeout);
unsigned long get_led_blink_timeout(enum LED led);

enum Mode bool_to_mode(bool set_on);
enum Mode str_to_mode(const char *str);
char *mode_to_str(enum Mode mode);
