obj-m += mydriver.o

mydriver-y := ./src/mydriver.o ./src/led_control/led_control.o ./src/dev_entry/dev_entry.o ./src/sysfs_entry/sysfs_entry.o

KDIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) clean
