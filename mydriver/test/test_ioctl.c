#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVICE_FILE "/dev/mydriver"

#define IOCTL_MAGIC 'L'
#define IOCTL_SET_LED _IOW(IOCTL_MAGIC, 0, bool)
#define IOCTL_GET_LED_STATUS _IOR(IOCTL_MAGIC, 1, bool)

int main(void)
{
	int fd, ret;
	bool led_status;

	fd = open(DEVICE_FILE, O_RDWR);
	if (fd < 0) {
		perror("Failed to open the device");
		return EXIT_FAILURE;
	}

	bool on = true;
	printf("Turning LED ON\n");
	ret = ioctl(fd, IOCTL_SET_LED, &on);
	if (ret < 0) {
		perror("Failed to turn LED on");
		close(fd);
		return EXIT_FAILURE;
	}

	printf("Getting LED status\n");
	ret = ioctl(fd, IOCTL_GET_LED_STATUS, &led_status);
	if (ret < 0) {
		perror("Failed to get LED status");
		close(fd);
		return EXIT_FAILURE;
	}
	printf("LED status: %s\n", (led_status ? "ON" : "OFF"));

	bool off = false;
	printf("Turning LED OFF\n");
	ret = ioctl(fd, IOCTL_SET_LED, &off);
	if (ret < 0) {
		perror("Failed to turn LED off");
		close(fd);
		return EXIT_FAILURE;
	}

	printf("Getting LED status\n");
	ret = ioctl(fd, IOCTL_GET_LED_STATUS, &led_status);
	if (ret < 0) {
		perror("Failed to get LED status");
		close(fd);
		return EXIT_FAILURE;
	}
	printf("LED status: %s\n", (led_status ? "ON" : "OFF"));

	close(fd);
	return EXIT_SUCCESS;
}
