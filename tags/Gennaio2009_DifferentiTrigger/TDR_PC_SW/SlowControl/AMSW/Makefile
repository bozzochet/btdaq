# file Makefile for SlowControl/AMSW directory
#
# A.Lebedev - Mar 2005...

OS        = OS_LINUX
CC        = gcc
AR        = ar

PWD       = $(shell pwd)
EXEDIR    = $(shell if `pwd | grep -sq gss`; then pwd | sed -e 's/gss\/.*/gss\/bin/'; else echo "/usr/local/bin"; fi)

INCDEFS    = nodedef.h jlv1def.h sdr2def.h spt2def.h sfet2def.h
INCSYSLIB  = mylib.h xformslib.h template.h
INCAMSWLIB = epplib.h amswlib.h
INCTCPLIB  = tcplib.h easslib.h
INCDEVLIB  = daqlib.h bricklib.h

INC        = $(INCDEFS) $(INCSYSLIB) $(INCAMSWLIB) $(INCTCPLIB) $(INCDEVLIB)
#            $(INCRECLIB)

OBJSYSLIB  = mylib.o xformslib.o template.o
OBJAMSWLIB = epplib.o amswlib.o
OBJTCPLIB  = tcplib.o easslib.o
OBJDEVLIB  = daqlib.o bricklib.o

OBJ       = $(OBJSYSLIB) $(OBJAMSWLIB) $(OBJTCPLIB) $(OBJDEVLIB)
#           $(OBJRECLIB)

LIB       = AMSW.a

CFLAGS    = -D$(OS) -Wall -O2 -fno-strict-aliasing \
            -I/usr/X11R6/include/ -I/usr/X11R6/include/X11 -DUSE_AMSW
LDFLAGS   = -L/usr/X11R6/lib
LOADLIBES = $(LIB) -lm -lX11 -lXpm -lpthread -Xlinker -Bstatic -lforms -Xlinker -Bdynamic
#LOADLIBES = $(LIB) -lm -lX11 -lforms

INT     = b e rfs rns lecroy-1 lecroy-2 path \
          lddr-2 lin-1 lin-2 lin-3 lin-4 \
          read_and_print_jlv1_scalers
EXT     = 3 ping-amsw s9011at \
          LeCroy HV U \
          Brick Brick-2 Brick-3 Brick-4 \
          JINF JLV1-1 JLV1-2 EDR2-1 \
          SDR2 SPT2 SFET2 SFET2-2 SFET2-3 \
          JINJ-1 \
          load_all_jlv1_registers convert_jlv1_registers_configuration_file \
          basili-3 basili-4 basili-5 basili-5a basili-6 basili-6a basili-6b \
          basili-7 philip-1 philip-2 read-events
#         blocktest

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
	chown root:root $(EXE)
	chmod +s        $(EXE)

#-------------------------------------------------------------------------------------

export:
	cp -p -f $(EXT) /usr/local/bin

#-------------------------------------------------------------------------------------

purge:
	(cd /usr/local/bin; rm -f -v $(EXT) )

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
