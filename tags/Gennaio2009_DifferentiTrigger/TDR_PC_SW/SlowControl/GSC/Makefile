# file Makefile for SlowControl/GSC directory
#
# A.Lebedev - Oct 2007...

OS        = OS_LINUX
CC        = gcc
AR        = ar

PWD       = $(shell pwd)
EXEDIR    = $(shell if `pwd | grep -sq gss`; then pwd | sed -e 's/gss\/.*/gss\/bin/'; else echo "/usr/local/bin"; fi)

INCDEFS    = nodedef.h treedef.h
INCSYSLIB  = mylib.h gsclib.h hrdllib.h xformslib.h template.h
INCCANLIB  = epplib.h canlib.h uscmlib.h gcanlib.h
INCJMDCLIB = jmdclib.h
INCTCPLIB  = tcplib.h easslib.h

INC        = $(INCDEFS) $(INCSYSLIB) $(INCCANLIB) $(INCJMDCLIB) $(INCTCPLIB)

OBJSYSLIB  = mylib.o gsclib.o hrdllib.o xformslib.o template.o
OBJCANLIB = epplib.o canlib.o uscmlib.o gcanlib.o
OBJJMDCLIB = jmdclib.o
OBJTCPLIB  = tcplib.o easslib.o

OBJ       = $(OBJSYSLIB) $(OBJCANLIB) $(OBJJMDCLIB) $(OBJTCPLIB)

LIB       = GSC.a

CFLAGS    = -D$(OS) -Wall -O2 -fno-strict-aliasing \
            -I/usr/X11R6/include/ -I/usr/X11R6/include/X11 -DUSE_JMDC
LDFLAGS   = -L/usr/X11R6/lib
LOADLIBES = $(LIB) -lm -lX11 -lXpm -lpthread -Xlinker -Bstatic -lforms -Xlinker -Bdynamic

INT     = make_run_list make_data_type_list read_block_files
EXT     = write_frames deframing

EXE     = $(INT) $(EXT)

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
	chown root:root $(EXT)
	chmod +s        $(EXT)

#-------------------------------------------------------------------------------------

export:
	cp -p -f $(EXT) /usr/local/bin

#-------------------------------------------------------------------------------------

purge:
	(cd /usr/local/bin; rm -f -v $(EXT) )

#-------------------------------------------------------------------------------------

links:
	(if [ "$(EXEDIR)" != "/usr/local/bin" ]; then mkdir -p $(EXEDIR); fi)
	(for file in $(EXT); do ln -sf --target-directory=$(EXEDIR) $(PWD)/$$file; done);

#-------------------------------------------------------------------------------------

unlinks:
	(for file in $(EXT); do rm -f $(EXEDIR)/$$file; done)

#-------------------------------------------------------------------------------------

clean:
	rm -f *~ *.bak *.o $(LIB) $(EXE) core

#=====================================================================================
