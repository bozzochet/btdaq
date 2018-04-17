###################################
# Makefile for AMSWire PCI Board
# Kernel Version: 2.6
###################################

ifneq ($(KERNELRELEASE),)

obj-m   := amswire.o

else

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)
DRVDIR := /lib/modules/$(shell uname -r)/kernel/drivers
SECTION := ams02

TARGETS := amswire.o libamsw.a libutil.a reg_test cmd_test mem_test loopback master slave
OBJS	:= amswire.o libamsw.o libutil.o reg_test.o cmd_test.o mem_test.o loopback.o master.o slave.o
MACHDEP = $(CC_OPTIM_DRIVER) $(CC_WARN)
CFLAGS	:= -g $(MACHFLG) $(MACHDEP) $(CC_DEFINES) \
	-DDEBUG=$(DEBUG) -DPRINT=0 -DTRACE=$(TRACE) \
	-D__KERNEL__ -DKERNEL -DMODULE

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

amswire.o:	amswire.c amswire.h
	$(CC) -c amswire.c -o amswire.o
libamsw.a:	libamsw.o
	$(AR) -r libamsw.a libamsw.o
libamsw.o:	libamsw.c amswire.h amsw_func.h
	$(CC) -c libamsw.c -o libamsw.o
libutil.a:	libutil.o
	$(AR) -r libutil.a libutil.o
libutil.o:	libutil.c amswire.h util_func.h
	$(CC) -c libutil.c -o libutil.o
reg_test:	reg_test.c amswire.h amsw_func.h libamsw.a
	$(CC) -c reg_test.c -o reg_test.o
	$(CC) reg_test.o libamsw.a -o reg_test
#	-rm reg_test.o
cmd_test:	cmd_test.c amswire.h amsw_func.h libamsw.a
	$(CC) -c cmd_test.c -o cmd_test.o
	$(CC) cmd_test.o libamsw.a -o cmd_test
#	-rm cmd_test.o
mem_test:	mem_test.c amswire.h amsw_func.h libamsw.a libutil.a
	$(CC) -c mem_test.c -o mem_test.o
	$(CC) mem_test.o libamsw.a libutil.a -o mem_test
#	-rm mem_test.o
loopback:	loopback.c amswire.h amsw_func.h amsw_protocol.h libamsw.a libutil.a
	$(CC) -c loopback.c -o loopback.o
	$(CC) loopback.o libamsw.a libutil.a -o loopback
#	-rm loopback.o
master:		master.c amswire.h amsw_func.h amsw_protocol.h libamsw.a libutil.a
	$(CC) -c master.c -o master.o
	$(CC) master.o libamsw.a libutil.a -o master
#	-rm master.o
evt_req:	evt_req.c amswire.h amsw_func.h amsw_protocol.h libamsw.a libutil.a
	$(CC) -c evt_req.c -o evt_req.o
	$(CC) evt_req.o libamsw.a libutil.a -o evt_req
evt_rpl:	evt_rpl.c amswire.h amsw_func.h amsw_protocol.h libamsw.a libutil.a
	$(CC) -c evt_rpl.c -o evt_rpl.o
	$(CC) evt_rpl.o libamsw.a libutil.a -o evt_rpl
slave:		slave.c amswire.h amsw_func.h amsw_protocol.h libamsw.a libutil.a
	$(CC) -c slave.c -o slave.o
	$(CC) slave.o libamsw.a libutil.a -o slave
#	-rm slave.o

install: amswire.ko
	-mkdir -p $(DRVDIR)/$(SECTION)
	-cp -f amswire.ko $(DRVDIR)/$(SECTION)
	-depmod -a
	-cp -f amswire.init /etc/init.d/amswire
	-(chmod a+x /etc/init.d/amswire; /sbin/chkconfig --add /etc/init.d/amswire)
	-/sbin/service amswire restart

uninstall:
	-/sbin/service amswire stop
	-/sbin/chkconfig --del amswire
	-rmmod amswire
	-rm -f /etc/init.d/amswire
	-rm -f $(DRVDIR)/$(SECTION)/amswire.ko

clean:
	rm -rf *.o *~core .depend .*.cmd *.ko *.mod.c .tmp_versions Module* modules*

endif
