# file Makefile for SlowControl/CAN/FS directory
#
# A.Lebedev - Jul-2008...

CC        = gcc
AR        = ar

PWD       = $(shell pwd)
EXEDIR    = $(shell if `pwd | grep -sq gss`; then pwd | sed -e 's/gss\/.*/gss\/bin/'; else echo "/usr/local/bin"; fi)

INCDEFS   = nodedef.h
INCSYSLIB = mylib.h xformslib.h template.h fslib.h
INCCANLIB = epplib.h canlib.h uscmlib.h
INCTCPLIB = tcplib.h easslib.h
INCDEVLIB =

INC       = $(INCDEFS) $(INCSYSLIB) $(INCCANLIB) $(INCTCPLIB) $(INCDEVLIB)

OBJSYSLIB = mylib.o xformslib.o template.o fslib.o
OBJCANLIB = epplib.o canlib.o uscmlib.o
OBJTCPLIB = tcplib.o easslib.o
OBJDEVLIB =

OBJ       = $(OBJSYSLIB) $(OBJCANLIB) $(OBJTCPLIB) $(OBJDEVLIB)

LIB       = FS.a

CFLAGS    = -Wall -O2 -fno-strict-aliasing \
            -I/usr/X11R6/include/ -I/usr/X11R6/include/X11 -DUSE_CAN
LDFLAGS   = -L/usr/X11R6/lib
LOADLIBES = $(LIB) -lm -lX11 -lXpm -Xlinker -Bstatic -lforms -Xlinker -Bdynamic
#LOADLIBES = $(LIB) -lm -lX11 -lforms

INT    = serialread serialwrite

EXT    = boxC boxS segments\
         test_gassys

EXE    = $(INT) $(EXT)

#-------------------------------------------------------------------------------------

all:	$(OBJ) $(LIB) $(EXE)

#-------------------------------------------------------------------------------------

%.o:	%.c $(INC)
	$(CC) -c -o $*.o $*.c $(CFLAGS)

$(LIB):	$(OBJ)
	$(AR) cru $(LIB) $(OBJ)

%:	%.c $(LIB)
	$(CC) -o $* $*.c $(CFLAGS) $(LDFLAGS) $(LOADLIBES)

#-------------------------------------------------------------------------------------

install:
	chown root:root $(EXE)
	chmod +s        $(EXE)

#-------------------------------------------------------------------------------------

purge:
	(cd /usr/local/bin; rm -f -v $(EXT) )

#-------------------------------------------------------------------------------------

export:
	cp -p -f $(EXT) /usr/local/bin

#-------------------------------------------------------------------------------------

links:
	(if [ "$(EXEDIR)" != "/usr/local/bin" ]; then mkdir -p $(EXEDIR); fi)
	(for file in $(EXT); do ln -sf --target=$(EXEDIR) $(PWD)/$$file; done);

#-------------------------------------------------------------------------------------

unlinks:
	(for file in $(EXT); do rm -f $(EXEDIR)/$$file; done)

#-------------------------------------------------------------------------------------

clean:
	rm -f *~ *.bak *.o $(LIB) $(EXE) core

#=====================================================================================
