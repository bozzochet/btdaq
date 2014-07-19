#  $Id: Makefile,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $

CFLAGS=-I/usr/local/include  -g

#CLFLAGS = -g1
LDFLAGS=-L/usr/local/lib

LOADLIBES=-lm -lamstcp

CC=cxx

OBJS =	Coo.c

recalc: Coordinates.o
	$(CC) $(CFLAGS) -g -o Coo_file Coo_file.c Coordinates.o $(LDFLAGS) $(LOADLIBES)

coo_recalc.o: Coordinates.h
	$(CC)  $(CFLAGS) -g -c Coordinates.c  $(LDFLAGS) $(LOADLIBES)





