##Compiling module
obj-m := charDr.o
KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o UA_dev1 UA_dev1.c
	gcc -o UA_dev2 UA_dev2.c

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf UA_dev1 UA_dev2


