# FILE: Makefile
ifneq ($(KERNELRELEASE),)
	obj-m := i2c_test.o
	ccflags-y := -std=gnu99 -Wno-declaration-after-statement -Werror
else
	# set dir for source code libraries
	KERNELDIR ?= ~/sources/rpi-4.14/
	# set the current dir
	PWD := $(shell pwd)
default:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-poky-linux-gnueabi- -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers
endif
