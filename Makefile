.PHONY:
	dependencies
	compile-commands
	install
	remove
	status
	test
	led_on_params
	led_off_params
	led_on_sysfs
	led_off_sysfs
	led_blink_slow
	led_blink_fast
	led_blink_params

dependencies:
	apt-get install -y build-essential dkms kmod linux-headers-$$(uname -r) linux-source bear

compile-commands:
	cd ./mydriver && make clean && bear -- make

install: remove
	mkdir -p /usr/src/mydriver-1.0
	cp -rf ./mydriver/* /usr/src/mydriver-1.0/
	dkms add -m mydriver -v 1.0
	dkms build -m mydriver -v 1.0 --force
	dkms install -m mydriver -v 1.0 --force
	modprobe mydriver

remove:
	modprobe -r mydriver || true
	dkms remove -m mydriver -v 1.0 --all || true
	modprobe -r mydriver || true
	rm -rf /usr/src/mydriver-1.0 || true
	rm -rf /var/lib/dkms/mydriver/ || true

status:
	dkms status
	modinfo mydriver
	modprobe mydriver
	lsmod | grep mydriver
	dmesg | grep "Start mydriver"

test: led_on_params led_off_params led_on_sysfs led_off_sysfs led_blink_slow led_blink_fast led_blink_params
	gcc -o ./mydriver/test/test_ioctl ./mydriver/test/test_ioctl.c
	./mydriver/test/test_read.sh
	./mydriver/test/test_ioctl

led_off_params:
	modprobe -r mydriver || true
	modprobe mydriver led_green=off led_red=off

led_off_sysfs:
	echo "off" > /sys/kernel/mydriver/led_green
	echo "off" > /sys/kernel/mydriver/led_red

led_blink_params:
	modprobe -r mydriver || true
	modprobe mydriver led_green=blink led_red=blink

led_blink_on:
	echo "blink" > /sys/kernel/mydriver/led_green

led_on_params:
	modprobe -r mydriver || true
	modprobe mydriver led_green=on led_red=on

led_on_sysfs:
	echo "on" > /sys/kernel/mydriver/led_green
	echo "on" > /sys/kernel/mydriver/led_red

led_blink_slow: led_blink_on
	echo "2000" > /sys/kernel/mydriver/led_green_blink_time

led_blink_fast: led_blink_on
	echo "200" > /sys/kernel/mydriver/led_green_blink_time
	cat /sys/kernel/mydriver/led_green_blink_time
	cat /sys/kernel/mydriver/led_green
