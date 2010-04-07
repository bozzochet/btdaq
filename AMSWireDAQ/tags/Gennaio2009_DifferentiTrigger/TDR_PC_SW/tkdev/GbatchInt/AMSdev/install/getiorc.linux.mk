include $(ORACLE_HOME)/precomp/lib/env_precomp.mk
#
PCINCLUDE=/usr/local/include/g++
OBJS=getiorc.o
EXE=../exe/linux/getiorc.exe
BIN=../bin/linux/getiorc.c
PCCSRC=../CC/getiorc.pc

cppbuild: 
	$(PROC) $(PROCPPFLAGS) INCLUDE=$(PCINCLUDE) iname=$(PCCSRC) oname=$(BIN)
	$(CPLUSPLUS) -w -g -c -fpermissive -I/usr/include  -I../include  -I/usr/local/include/g++ $(INCLUDE) $(BIN)
	$(CPLUSPLUS) -static -w -o $(EXE) $(OBJS) -L$(LIBHOME) $(STATICCPPLDLIBS)

$(CPPSAMPLES): cppdemo2
	$(MAKE) -f $(MAKEFILE) OBJS=$@.o EXE=$@ cppbuild
#
# The macro definition fill in some details or override some defaults from 
# other files.
#
OTTFLAGS=$(PCCFLAGS)
CLIBS= $(TTLIBS_QA) $(STATICCPPLDLIBS)
PRODUCT_LIBHOME=
MAKEFILE=./getiorc.linux.mk
PROCPLSFLAGS= sqlcheck=full userid=$(USERID) 
PROCPPFLAGS= code=cpp $(CPLUS_SYS_INCLUDE)
USERID=scott/tiger
NETWORKHOME=$(ORACLE_HOME)/network/
PLSQLHOME=$(ORACLE_HOME)/plsql/
INCLUDE_PC=$(I_SYM). $(I_SYM)$(PRECOMPHOME)syshdr $(I_SYM)$(PRECOMPHOME)public $(I_SYM)$(RDBMSHOME)public $(I_SYM)$(RDBMSHOME)demo $(I_SYM)$(PLSQLHOME)public $(I_SYM)$(NETWORKHOME)public
INCLUDE=$(I_SYM). $(I_SYM)$(PRECOMPHOME)public $(I_SYM)$(RDBMSHOME)public $(I_SYM)$(RDBMSHOME)demo $(I_SYM)$(PLSQLHOME)public $(I_SYM)$(NETWORKHOME)public
