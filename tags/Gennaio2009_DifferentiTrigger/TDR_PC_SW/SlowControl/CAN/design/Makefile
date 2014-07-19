# file Makefile for SlowControl/CAN/design directory
#
# A.Lebedev - Mar 2005 ...

CC        = gcc
AR        = ar

INCDEFS   = nodedef.h uscmdef.h pdsdef.h trddef.h ccebdef.h jpddef.h ttcedef.h gpsdef.h
INCSYSLIB = mylib.h xformslib.h template.h
INCCANLIB = epplib.h canlib.h uscmlib.h
INCTCPLIB = tcplib.h
INCDEVLIB = ugbslib.h mcalib.h cceblib.h gpslib.h

INC       = $(INCDEFS) $(INCSYSLIB) $(INCCANLIB) $(INCTCPLIB) $(INCDEVLIB)

OBJSYSLIB = mylib.o xformslib.o template.o
OBJCANLIB = canlib-fake.o
OBJDEVLIB = ugbslib.o mcalib.o cceblib.o gpslib.o

OBJ       = $(OBJSYSLIB) $(OBJCANLIB) $(OBJDEVLIB)

LIB       = fake.a

CFLAGS    = -Wall -O2 -fno-strict-aliasing \
            -I/usr/X11R6/include/ -I/usr/X11R6/include/X11 -DUSE_CAN \
            -DSIMULATION
LDFLAGS   = -L/usr/X11R6/lib
LOADLIBES = $(LIB) -lm -lX11 -lXpm -Xlinker -Bstatic -lforms -Xlinker -Bdynamic
#LOADLIBES = $(LIB) -lm -lX11 -lforms

#EXE     = HV JPD DS PDS PDS-4 PDS-5 LDDR LDDR-2 \
#          U USCM LVDS CCEB 1616 config \
#          lddr-test-1 

EXE      = gps-tc

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
