

CPPFLAGS= -DOS_LINUX -DUSE_AMSW

CFLAGS= -Wall -g  -fno-strict-aliasing 
CFLAGS+=$(CPPFLAGS)

SRCS=  uscmlib.c canlib.c amswlib.c easslib.c epplib.c mylib.c tcplib.c pzlib.c

OBJS=$(patsubst %.c,%.o,$(SRCS))

../lib/libAMSslow.a: ../lib/libAMSslow.a($(OBJS))
#	$(AR) r $@ $^
	ranlib $@

doc:
	doxygen Doxyfile

clean:
	rm -fv *.o ../lib/libAMSslow.a
distclean: clean
	rm -fvr doc
