# file Makefile for .../SlowControl/PCI-AMSW/ directory
#
# A.Lebedev - Feb 2007...

CC        = gcc
AR        = ar

INCDEFS    = 
INCSYSLIB  = mylib.h
INCAMSWLIB = amsw_dev.h amsw_lib.h pciamswlib.h
INCAMSWLIB = pciamswlib.h
INCDEVLIB  = 

INC        = $(INCDEFS) $(INCSYSLIB) $(INCAMSWLIB) $(INCDEVLIB)

OBJSYSLIB  = mylib.o
OBJAMSWLIB = amsw_lib.o pciamswlib.o
OBJDEVLIB  = 

OBJ       = $(OBJSYSLIB) $(OBJAMSWLIB) $(OBJDEVLIB)

LIB       = pciamsw.a

CFLAGS    = -Wall -O2 -fno-strict-aliasing \
            -I/usr/X11R6/include/ -I/usr/X11R6/include/X11 -DUSE_PCIAMSW
LDFLAGS   = -L/usr/X11R6/lib
LOADLIBES = $(LIB) -lm -lX11 -Xlinker -Bstatic -lforms -Xlinker -Bdynamic
#LOADLIBES = $(LIB) -lm -lX11 -lforms

INT     = al-test-1 al-test-2 kill \
          al-test-3 al-test-4
EXT     = 
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

driver:
	$(CC) -o amswire.mod -c -g -I. -I. -L. -L.  -O2 -fno-builtin -Wall -DLINUX \
  -D__KERNEL__ -DKERNEL  -DMODULE -isystem /lib/modules/`uname \
  -r`/build/include amswire.c

#-------------------------------------------------------------------------------------

install:
	chown root:root $(EXE)
	chmod +s        $(EXE)

#-------------------------------------------------------------------------------------

export:
	cp -p -f $(EXT) /usr/local/bin

#-------------------------------------------------------------------------------------

clean:
	rm -f *~ *.bak *.o $(LIB) $(EXE) core

#=====================================================================================
