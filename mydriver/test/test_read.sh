#!/bin/bash

DEVICE_PATH="/dev/mydriver"
READ_SIZE_1=50
READ_SIZE_2=60

# Test cat
if [ ! -e $DEVICE_PATH ]; then
    echo "Device file $DEVICE_PATH not found. Make sure the driver is loaded."
    exit 1
fi

# Open the device file
exec 3< $DEVICE_PATH

# Read the first 50 characters
echo -n "Reading first $READ_SIZE_1 characters: "
head -c $READ_SIZE_1 <&3
echo

# Read the next 60 characters
echo -n "Reading next $READ_SIZE_2 characters: "
head -c $READ_SIZE_2 <&3
echo

exec 3<&-

# Test echo
echo "Hello World!" > $DEVICE_PATH


# Test sysfs
cat /sys/kernel/mydriver/led_green
echo "on" > /sys/kernel/mydriver/led_green
cat /sys/kernel/mydriver/led_green
echo "off" > /sys/kernel/mydriver/led_green
cat /sys/kernel/mydriver/led_green


exit 0
