TARGETS := abiTest frmTest abiFileTest ssptimeTest ringbufferTest

EXTRAINCS :=  -I../grs
LDFLAGS =  -shared

LIBRARY := aallib.a

OSTYPE  := $(shell echo $$OSTYPE)

ifeq "$(OSTYPE)" "msys"
OTHERSRCS := commreport.c pipeexec.c ptyopen.c
endif

include ../make.inc

abiTest: abiTest.c $(LIBRARY)
	$(CC) $(GBLCFLAGS) abiTest.c -o abiTest $(LIBRARY) -lpthread

frmTest: frmTest.c $(LIBRARY) 
	$(CC) $(GBLCFLAGS) frmTest.c -o frmTest $(LIBRARY) -lpthread

abiFileTest: abiFile.c $(LIBRARY) 
	$(CC) $(GBLCFLAGS) -D DEBUG_abiFile abiFile.c -o abiFileTest $(LIBRARY) -lpthread

ssptimeTest: ssptime.c $(LIBRARY) 
	$(CC) $(GBLCFLAGS) -D TEST_MAIN ssptime.c -o ssptimeTest $(LIBRARY) -lpthread

ringbufferTest: ringbuffer.c $(LIBRARY) 
	$(CC) $(GBLCFLAGS) -D RINGBUFFER_UNIT_TEST ringbuffer.c -o ringbufferTest $(LIBRARY) -lpthread

-include $(DEPINCS)
