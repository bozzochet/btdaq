CC = cc
CFLAGS    = -O4 -Dthis_is_ALPHA_computer -I$(HOME)/AMS -I/usr/local/include
LDFLAGS   = -L$(HOME)/AMS -L/usr/lib/X11
LOADLIBES = -lm -lX11 -lXpm -lforms


all:	m b f k


m:		m.o p_data.o pgsc.o \
		psb.o htc.o can.o cnt.o mon.o  \
		mcc.o daq.o \
		main.o mcds.o ehts.o ihts.o tplt.o \
		dialog.o \
		graph.o
		$(CC) -o m m.o p_data.o pgsc.o \
		psb.o htc.o can.o cnt.o mon.o  \
		mcc.o daq.o \
		main.o mcds.o ehts.o ihts.o tplt.o \
		dialog.o \
		graph.o -lm

w:		w.o pp_alpha.o pgsc.o \
		psb.o htc.o can.o cnt.o mon.o  \
		main.o mcds.o ehts.o ihts.o tplt.o \
		mcc.o daq.o \
		graph.o
		$(CC) -o w w.o pp_alpha.o pgsc.o \
		psb.o htc.o can.o cnt.o mon.o  \
		mcc.o daq.o \
		main.o mcds.o ehts.o ihts.o tplt.o \
		graph.o -lm

1:		1.o
		$(CC) -o 1 1.o -lm

1.o:		1.c makefile

2:		2.o
		$(CC) -o 2 2.o -lm

2.o:		2.c makefile

3:		3.o
		$(CC) -o 3 3.o \
		$(HOME)/AMS/Deframing.../SRDL_frames.o \
		$(HOME)/AMS/Deframing.../all_frames.o \
		$(HOME)/AMS/Deframing.../status_files.o \
		-lm

3.o:		3.c makefile

33:		33.o
		$(CC) -o 33 33.o -lm

33.o:		33.c makefile

33a:		33a.o
		$(CC) -o 33a 33a.o myftplib.o -lm

33a.o:		33a.c makefile

33b:		33b.o
		$(CC) -o 33b 33b.o myftplib.o -lm

33b.o:		33b.c makefile

33c:		33c.o
		$(CC) -o 33c 33c.o myftplib.o -lm

33c.o:		33c.c makefile

h:		h.o
		$(CC) -o h h.o myftplib.o -lm

h.o:		h.c makefile

333:		333.o
		$(CC) -o 333 333.o -lm

333.o:		333.c makefile

b:		b.o bus_550_alpha.o \
		$(HOME)/AMS/send_command.o \
		$(HOME)/AMS/decode_AMS_block_ID.o
		$(CC) -o b b.o bus_550_alpha.o \
		$(HOME)/AMS/send_command.o \
		$(HOME)/AMS/decode_AMS_block_ID.o \
		$(CFLAGS) $(LDFLAGS) $(LOADLIBES) -lamstcp -lm

f:		b.o bus_550_alpha_fast.o \
		$(HOME)/AMS/send_command.o \
		$(HOME)/AMS/decode_AMS_block_ID.o
		$(CC) -o f b.o bus_550_alpha_fast.o \
		$(HOME)/AMS/send_command.o \
		$(HOME)/AMS/decode_AMS_block_ID.o \
		$(CFLAGS) $(LDFLAGS) $(LOADLIBES) -lamstcp -lm

b.o:		b.c buscon.h makefile

bus_550_alpha.o:	bus_550_alpha.c buscon.h makefile

bus_550_alpha_fast.o:	bus_550_alpha.c buscon.h makefile
			$(CC) -o bus_550_alpha_fast.o \
			-c bus_550_alpha.c $(CFLAGS) -Dsend_commands_fast

k:		k.o
		$(CC) -o k k.o \
		$(CFLAGS) $(LDFLAGS) $(LOADLIBES) -lamstcp -lm

k.o:		k.c makefile

m.o:		m.c pgsc.h makefile
w.o:		w.c pgsc.h makefile

p_data.o:	p_data.c pgsc.h graph.c graph.h makefile
pp_alpha.o:	pp_alpha.c pgsc.h graph.c graph.h makefile
pgsc.o:		pgsc.o pgsc.h graph.c graph.h makefile
psb.o:		psb.c pgsc.h graph.c graph.h makefile
htc.o:		htc.c pgsc.h graph.c graph.h makefile
can.o:		can.c pgsc.h graph.c graph.h makefile
cnt.o:		cnt.c pgsc.h graph.c graph.h makefile
mon.o:		mon.c pgsc.h graph.c graph.h makefile
mcc.o:		mcc.c pgsc.h graph.c graph.h makefile
daq.o:		daq.c pgsc.h graph.c graph.h makefile
main.o:		main.o pgsc.h graph.c graph.h makefile
mcds.o:		mcds.o pgsc.h graph.c graph.h makefile
ehts.o:		ehts.o pgsc.h graph.c graph.h makefile
ihts.o:		ihts.o pgsc.h graph.c graph.h makefile
tplt.o:		tplt.o pgsc.h graph.c graph.h makefile
dialog.o:	dialog.o pgsc.h graph.c graph.h makefile

graph.o:	graph.c graph.h makefile

$(HOME)/AMS/decode_AMS_block_ID.o:	\
		$(HOME)/AMS/decode_AMS_block_ID.c \
		$(HOME)/AMS/AMS_commanding_language.h \
		makefile
		$(CC) \
		-o $(HOME)/AMS/decode_AMS_block_ID.o \
		-c $(HOME)/AMS/decode_AMS_block_ID.c \
		$(CFLAGS)

$(HOME)/AMS/send_command.o:	\
		$(HOME)/AMS/send_command.c \
		$(HOME)/AMS/commander.h \
		makefile
		$(CC) \
		-o $(HOME)/AMS/send_command.o \
		-c $(HOME)/AMS/send_command.c \
		$(CFLAGS)

#------------------------------------------------------------------

clean:
	rm -f *.o core m w 1 2 3 33 33a 33b 33c h 333 b f k 

#------------------------------------------------------------------
