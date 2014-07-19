#  $Id: Makefile,v 1.4 2008/09/18 16:53:54 zuccon Exp $
#
# Makefile to build tkdev/TrckerLib - Tracker library
#
# This Makefile is confirmed to work on 
#  SLC 4.5 with root-5.10.00
#  Mac OS 10.5.2 (Darwin 9.2.1) with root-5.19.02
#
# doxygen ver 1.5 or higer is recomended for generating documentation
#
#   17/Jan/2008 PZ Rev. 1.1  First version
#   21/Jan/2008 SH Rev. 1.2  Structure changed, some comments and utils added
#   04/Apr/2008 SH Rev. 1.19 Mac OSX support
#
# $Date: 2008/09/18 16:53:54 $
#
# $Revision: 1.4 $

#-------------------------------------
# Settings
#-------------------------------------
### Directories ###
SRC      = CC
DEP      = dep
OBJ      = obj
LIB      = ../lib
BIN      = ../bin

CPPFLAGS := -I.  -Iinclude -I../gbatch_sim/src -I../AMSbase/include  -I../TrackerLib/include -I../FORTinterface/include
include ../Make.common

LINK     = LinkDef
DICT     = dictAMSGeom

CLASSES =  gmat gvolume  
NOCINT  = tkgeom
CINCS    = $(addprefix include/,$(addsuffix .h,$(CLASSES)))
SRCS     = $(addprefix $(SRC)/,$(addsuffix .C,$(CLASSES)))
SRCS     += $(addprefix $(SRC)/,$(addsuffix .C,$(NOCINT)))
CINCS    +=  $(addprefix include/,$(addsuffix .h,$(LINK)))
INCS     = $(wildcard $(SRC)/*.h)
### Targets ###
DSRC     = $(addprefix $(SRC)/, $(addsuffix .C, $(DICT)))
DINC     = $(addprefix $(SRC)/, $(addsuffix .h, $(DICT)))
DEPS     = $(patsubst $(SRC)/%.C,$(DEP)/%.d,$(SRCS))
OBJS     = $(patsubst $(SRC)/%.C,$(OBJ)/%.o,$(SRCS))
//OBJS     += $(patsubst $(SRC)/%.C,$(OBJ)/%.o,$(DSRC))


#-------------------------------------
# Rules
#-------------------------------------

all: lib


lib: $(LIB)/libAMSGeom.a $(LIB)/libAMSGeom.so


$(LIB)/libAMSGeom.a : $(OBJS)
	@echo Bulding $@
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi
	@$(AR) r $@ $^ 

$(LIB)/libAMSGeom.so : $(OBJS)
	@echo Bulding $@
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi
	@$(CXX) $(SOFLAGS) $(LDFLAGS) -o $@ $^

$(DSRC) : $(CINCS)
	@echo Creating ROOT dictionary
	@if ! [ -d $(SRC)/tmp ] ; then mkdir -p $(SRC)/tmp; fi
	$(CINT) -f $@ -c $(CPPFLAGS) $^ 

### Include dependency files ###
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),status)
ifneq ($(MAKECMDGOALS),doxygen)
-include $(DEPS)
endif
endif
endif

.PHONY: clean doxygen status

### Cleanup ###
clean:
	@rm -fv $(LIB)/libAMSGeom.a $(LIB)/libAMSGeom.so $(DSRC) $(DINC)
	@rm -fr $(OBJ) $(DEP)

### Generate html document with doxygen ###
doxygen:
	@$(DOXYGEN) config/Doxyfile

### Check CVS status ###
status:
	@cvs status | grep 'Status:' | grep -v 'Up-to-date' | cat
