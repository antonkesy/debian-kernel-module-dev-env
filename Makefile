dependencies:
	sudo apt-get install -y build-essential dkms kmod linux-headers-$$(uname -r)

install: remove
	sudo mkdir -p /usr/src/mydriver-1.0
	sudo cp -rf ./mydriver/* /usr/src/mydriver-1.0/
	sudo dkms add -m mydriver -v 1.0
	sudo dkms build -m mydriver -v 1.0
	sudo dkms install -m mydriver -v 1.0 --force
	sudo modprobe mydriver

remove:
	sudo modprobe -r mydriver || true
	sudo dkms remove -m mydriver -v 1.0 --all || true
	sudo modprobe -r mydriver || true
	sudo rm -rf /usr/src/mydriver-1.0 || true

status:
	sudo dkms status
	sudo modprobe mydriver
	lsmod | grep mydriver
	sudo dmesg | grep "Start mydriver"
