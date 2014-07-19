# file Makefile for SlowControl/MISC directory
#
# A.Lebedev - Aug-2006...

CC        = gcc
AR        = ar

INCDEFS   = nodedef.h
INCSYSLIB = mylib.h xformslib.h template.h

INC       = $(INCDEFS) $(INCSYSLIB)

OBJSYSLIB = mylib.o xformslib.o template.o

OBJ       = $(OBJSYSLIB)

LIB       = MISC.a

CFLAGS    = -Wall -O2 -fno-strict-aliasing \
            -I/usr/X11R6/include/ -I/usr/X11R6/include/X11
LDFLAGS   = -L/usr/X11R6/lib
LOADLIBES = $(LIB) -lm -lX11 -lXpm -Xlinker -Bstatic -lforms -Xlinker -Bdynamic
#LOADLIBES = $(LIB) -lm -lX11 -lforms

EXE    = TEMPERATURE

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

clean:
	rm -f *~ *.bak *.o $(LIB) $(EXE) core

#-------------------------------------------------------------------------------------

%.hex: %.cfg 
	./cfg_gen $@ < $< 2> $*.log

#=====================================================================================
