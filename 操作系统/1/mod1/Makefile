obj-m:=module1.o

KDIR:= /lib/modules/$(shell uname -r)/build
 
PWD:= $(shell pwd)
 
all:default test
 
default:
	
	$(MAKE) -C $(KDIR) M=$(PWD) modules 

clean:

	@sudo rmmod module1
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	@dmesg | tail -10

test:

	@sudo dmesg -C
	@sudo insmod module1.ko
	@dmesg
