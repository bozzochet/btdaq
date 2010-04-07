# file Makefile for SlowControl/CAN directory
#
# A.Lebedev - Mar-2005...

OS        = OS_LINUX
CC        = gcc
AR        = ar

PWD       = $(shell pwd)
EXEDIR    = $(shell if `pwd | grep -sq gss`; then pwd | sed -e 's/gss\/.*/gss\/bin/'; else echo "/usr/local/bin"; fi)

INCDEFS   = nodedef.h uscmdef.h pdsdef.h trddef.h \
            cceb.h ccebdef.h cabdef.h\
            jpddef.h gpsdef.h ttcedef.h
INCSYSLIB = mylib.h gsclib.h xformslib.h template.h
INCCANLIB = epplib.h canlib.h uscmlib.h gcanlib.h
INCTCPLIB = tcplib.h easslib.h
INCDEVLIB = ugbslib.h mcalib.h cceblib.h ccebtasklib.h gpslib.h cablib.h crisalib.h

INC       = $(INCDEFS) $(INCSYSLIB) $(INCCANLIB) $(INCTCPLIB) $(INCDEVLIB)

OBJSYSLIB = mylib.o gsclib.o xformslib.o template.o
OBJCANLIB = epplib.o canlib.o uscmlib.o gcanlib.o
OBJTCPLIB = tcplib.o easslib.o
OBJDEVLIB = ugbslib.o mcalib.o cceblib.o ccebtasklib.o gpslib.o cablib.o crisalib.o

OBJ       = $(OBJSYSLIB) $(OBJCANLIB) $(OBJTCPLIB) $(OBJDEVLIB)

LIB       = CAN.a

CFLAGS    = -D$(OS) -Wall -O2 -fno-strict-aliasing \
            -I/usr/X11R6/include/ -I/usr/X11R6/include/X11 -DUSE_CAN
LDFLAGS   = -L/usr/X11R6/lib
LOADLIBES = $(LIB) -lm -lX11 -lXpm -lpthread -Xlinker -Bstatic -lforms -Xlinker -Bdynamic
#LOADLIBES = $(LIB) -lm -lX11 -lforms

INT    = vers t s l e c serial serial-2 serial-3 \
         g gg mca fill_gas \
         m mem-1 ttce-1 ttce-2 \
         ds1 ds2 ds3 ds4 ds5 ds6 \
         dallas-model dallas-4 \
         CGS pds30 pds31 pds-2 pds-3 pds-4 PDS \
         aste-1 aste-1a aste-1b aste-2 aste-3 aste-4 \
         cab-slave cab-slave-2 cab-master \
         cab-dlcm-read-slice cab-ptm-study \
         aps \
         lvds lvds-2 lecroy lecroy-2 lecroy-3 lecroy-4 \
         uscm-2 uscm-3 uscm-killer uscm-killer-2 uscm-killer-3 otp \
         ccbt-1 ccbt-2 ccbt-3 ccbt-4 16 \
         m-crate-1

EXT    = 6 66 \
         load wla scan config wdsid debug \
         LeCroy LVDS USCM U UGPD HV UG66 JPD JPD-2 ndd DS DEBUG \
         PDS-4 PDS-5 PDS-4-OLD PDS-5-OLD PDS-4-NEW PDS-5-NEW \
         TTCE TTCE-2 TTCE-3 TTCE-4 TTCE-4P TTCE-4S \
         LDDR LDDR-2 LDDR-3 lddr-test-1 lddr-test-2 \
         dallas-1ts dallas-1 dallas-2 dallas-3 dallas-5 \
         SERIAL \
         CCEB CCEB-2 CCEB-3 1616 \
         CCEB-TASK ccebtask-command-write ccebtask-command-read \
         ping-can can-killer \
         can-server \
         gps-sim gps-sim-2 232-422-converter \
         tvt \
         cab-crisa cab-crisa-2 \
         cab-get-essential-status cab-get-essential-status-2 \
         cab-get-digital-status cab-get-digital-status-2 \
         CAB-0 CAB-1 CAB-2 CAB-CCS CAB-STM CAB-PS CAB-PTM \
         gpse-1 gpse-2 gps-sim-loopback gps-sim-loopback-2 \
         GPSE GPSE-2 GPS \
         gps-setup gps-tm gps-gmess gps-wmode \
         gps-coa   gps-galeph gps-gdump gps-linit gps-lrpack gps-neco \
         gps-ninit gps-nmano  gps-norbit gps-ratt  gps-rcacq  gps-rccs \
         gps-rcem  gps-rcflag gps-rctr  gps-rnav  gps-rpseud gps-rtime \
         gps-tc

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

#-------------------------------------------------------------------------------------

%.hex: %.cfg
	./cfg_gen $@ < $< 2> $*.log

#=====================================================================================
