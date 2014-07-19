# file Makefile for SlowControl/TTCP directory
#
# A.Lebedev - Mar-2007...

CC        = gcc
AR        = ar

INCDEFS   = 
INCSYSLIB = mylib.h
INCCANLIB = 
INCDEVLIB = 

INC       = $(INCDEFS) $(INCSYSLIB) $(INCCANLIB) $(INCDEVLIB)

OBJSYSLIB = mylib.o
OBJCANLIB = 
OBJDEVLIB = 

OBJ       = $(OBJSYSLIB) $(OBJCANLIB) $(OBJDEVLIB)

LIB       = TTCP.a

CFLAGS    = -Wall -O2 -fno-strict-aliasing \
            -I/usr/X11R6/include/ -I/usr/X11R6/include/X11 -DUSE_CAN
LDFLAGS   = -L/usr/X11R6/lib
LOADLIBES = $(LIB) -lm -lX11 -Xlinker -Bstatic -lforms -Xlinker -Bdynamic
#LOADLIBES = $(LIB) -lm -lX11 -lforms

INT    = client server master slave simple-client \
         ttcp-client ttcp-server

EXT    = 
         
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

export:
	cp -p -f $(EXT) /usr/local/bin

#-------------------------------------------------------------------------------------

clean:
	rm -f *~ *.bak *.o $(LIB) $(EXE) core

#=====================================================================================
