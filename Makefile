obj-m := cled.o
KDIR := /project/som-rk3399/kernel-rockchip
PWD := $(shell pwd)
all:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux- -C $(KDIR) M=$(PWD) modules
clean:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux- -C $(KDIR) M=$(PWD) clean



