#
SHELL=/bin/sh
#
MY_OS=$(shell uname)
ifeq ($(origin local_makefile), undefined)
        local_makefile = ../install/Makefile.$(MY_OS)
endif
include $(shell echo ${local_makefile})

FFLAGS+= -I. -I../F -I../include
INCS= $(wildcard *.h)    
FILES= rqmd.F  call_rqmd.f call_fluka.F trd.F


DEPS= $(BIN)rqmd.d  $(BIN)call_rqmd.d $(BIN)call_fluka.d $(BIN)trd.d
OBJECTS= $(BIN)rqmd.o  $(BIN)call_rqmd.o $(BIN)call_fluka.o $(BIN)trd.o



$(BIN)%.o : $(SFF)%.F
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $< -o $@

$(BIN)%.o : $(SFF)%.f
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $< -o $@

# rules to generate dependencies files


$(BIN)%.d: %.F
	@echo generating dependencies for $<
	@ if ! [ -d $(BIN) ] ; then  mkdir -p $(BIN); fi
	@$(SHELL) -ec '$(FF) -MM $(FFLAGS) -I$(CERNDIR)/include $< \
	|sed '\''s@\($*\)\.o[ :]*@$(@D)\1.o $@ : @g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

$(BIN)%.d: %.f
	@echo generating dependencies for $<
	@ if ! [ -d $(BIN) ] ; then  mkdir -p $(BIN); fi
	@$(SHELL) -ec '$(FF) -MM $(FFLAGS)-I$(CERNDIR)/include  $< \
	|sed '\''s@\($*\)\.o[ :]*@$(@D)\1.o $@ : @g'\'' > $@; '


all: $(LIB)libamsut.a

$(LIB)libamsut.a: $(OBJECTS)
	@ if ! [ -d $(LIB) ] ; then  mkdir -p $(LIB); fi
	@$(AR) -rv $@ $?

clean:
	rm -fv $(LIB)libamsut.a
	rm -fv $(OBJECTS) $(DEPS)
dep:

###### DEPENDENCIES #####
include $(DEPS)
