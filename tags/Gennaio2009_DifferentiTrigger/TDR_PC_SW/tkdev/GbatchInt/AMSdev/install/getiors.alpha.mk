include $(ORACLE_HOME)/precomp/env_precomp.mk

.SUFFIXES: .pc .c .o

LDSTRING=
PRODUCT_LIBHOME=
MAKEFILE=proc.mk
PROCPLSFLAGS= dbms=v6_char
PROCPPFLAGS= code=cpp include=/usr/include include=/usr/include/cxx
USERID=scott/tiger
INCLUDE=$(I_SYM). $(PRECOMPPUBLIC)
SRC=../CC/
BIN=../bin/osf1/
OBJS=../bin/osf1/getior.o
EXE=../exe/osf1/getior.exe


cppbuild:
	$(PROC) $(PROCPPFLAGS) iname=$(SRC)getior.pc oname=$(BIN)getior.c
	g++ -c -DA_OSF $(INCLUDE) $(BIN)getior.c -o $(BIN)getior.o
	g++ -static -o $(EXE) $(OBJS) -L$(LIBHOME) $(PROLDLIBS) $(STATICCPPLDLIBS)

