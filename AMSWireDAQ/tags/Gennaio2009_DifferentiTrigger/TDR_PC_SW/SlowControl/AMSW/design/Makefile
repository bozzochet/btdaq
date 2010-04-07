# file Makefile for SlowControl/AMSW/design directory
#
# A.Lebedev - Mar 2005 ...

CC        = gcc
AR        = ar

INCDEFS    = nodedef.h jlv1def.h sdr2def.h
INCSYSLIB  = mylib.h xformslib.h template.h
INCAMSWLIB = epplib.h amswlib.h daqlib.h
INCDEVLIB  = bricklib.h

INC       = $(INCDEFS) $(INCSYSLIB) $(INCAMSWLIB) $(INCDEVLIB)

OBJSYSLIB  = mylib.o xformslib.o template.o
OBJAMSWLIB = amswlib-fake.o
OBJDEVLIB  = bricklib.o

OBJ       = $(OBJSYSLIB) $(OBJAMSWLIB) $(OBJDEVLIB)

LIB       = fake.a

CFLAGS    = -Wall -O2 -fno-strict-aliasing \
            -I/usr/X11R6/include/ -I/usr/X11R6/include/X11 -DUSE_AMSW \
            -DSIMULATION
LDFLAGS   = -L/usr/X11R6/lib
LOADLIBES = $(LIB) -lm -lX11 -lXpm -Xlinker -Bstatic -lforms -Xlinker -Bdynamic
#LOADLIBES = $(LIB) -lm -lX11 -lforms

#EXE    = Brick Brick-2 LeCroy HV U Brick Brick-2 Brick-3 JINF JLV1-1 JLV1-2 EDR2-1 \
#         load_all_jlv1_registers
EXE     = Brick-4

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

#=====================================================================================
