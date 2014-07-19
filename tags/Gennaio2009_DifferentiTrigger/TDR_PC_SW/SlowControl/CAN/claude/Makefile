# file Makefile for claude directory
#
# A.Lebedev - Jun 2008...

OS        = OS_LINUX
CC        = gcc
AR        = ar

STEAL     = /home/ams/AAL/SlowControl/CAN
LIB       = $(STEAL)/CAN.a

CFLAGS    = -D$(OS) -Wall -O2 -fno-strict-aliasing \
            -I/usr/X11R6/include/ -I/usr/X11R6/include/X11 -I$(STEAL) -DUSE_CAN
LDFLAGS   = -L/usr/X11R6/lib
LOADLIBES = $(LIB) -lm -lX11 -Xlinker -Bstatic -lforms -Xlinker -Bdynamic

EXE       = 

#------------------------------------------------------------------------------

all:	$(EXE)

#------------------------------------------------------------------------------

%:	%.c $(INC) $(LIB)
	$(CC) -o $* $*.c $(CFLAGS) $(LDFLAGS) $(LOADLIBES)

#------------------------------------------------------------------------------

install:
	chown root:root $(EXE)
	chmod +s        $(EXE)

#------------------------------------------------------------------------------

export:
	cp -p -f $(EXE) /usr/local/bin

#------------------------------------------------------------------------------

clean:
	rm -f *~ *.bak *.o $(EXE) core

#------------------------------------------------------------------------------

release: clean
	 (cd ..; tar cvzf claude.20`date +%g%m%d`.tgz claude/*)

#==============================================================================
