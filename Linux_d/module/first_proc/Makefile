obj-m += proc_test.o
$(info "target: ${obj-m}")
All:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
.PHONY:clean
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
