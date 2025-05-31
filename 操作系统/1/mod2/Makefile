obj-m := module2.o

KDIR := /lib/modules/`uname -r`/build

PWD := $(shell pwd)

all:default

default: 
	sudo make -C $(KDIR) M=$(PWD) modules
	@sudo dmesg -C
 
clean: 
	@sudo rmmod module2
	sudo make -C $(KDIR) M=$(PWD) clean
	@dmesg | tail -10
