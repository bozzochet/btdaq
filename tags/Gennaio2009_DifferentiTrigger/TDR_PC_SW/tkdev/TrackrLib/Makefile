#  $Id: Makefile,v 1.34 2008/10/23 10:12:39 zuccon Exp $
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
# $Date: 2008/10/23 10:12:39 $
#
# $Revision: 1.34 $

#-------------------------------------
# Settings
#-------------------------------------
### Directories ###
SRC      = CC
DEP      = dep
OBJ      = obj
LIB      = ../lib
BIN      = ../bin

CPPFLAGS  = -I. -Iinclude -I../AMSbase/include 

include ../Make.common
DOXYGEN= /Applications/Doxygen.app/Contents/Resources/doxygen
### Classes ###
CLASSES  = TkDBc patt TkLadder TkObject TkPlane TkCoo TkSens\
	   TrCalDB TrLadCal \
	   TrRawCluster TrCluster TrRecHit  Event  TrFit\
	   TrTrack TrRecon  TrMCCluster VCon VCon_root
	   
### ROOT dictionary ###
LINK     = LinkDef
DICT     = rootElibdict

SRCS     = $(addprefix $(SRC)/,$(addsuffix .C, $(CLASSES)))
SRCS    += $(SRC)/tkdcards.C  

INCS     = $(addprefix include/, $(addsuffix .h, $(CLASSES)))
INCS     += $(addprefix include/, $(addsuffix .h, $(LINK)))
### Targets ###
DSRC     = $(addprefix $(SRC)/, $(addsuffix .C, $(DICT)))
DINC     = $(addprefix $(SRC)/, $(addsuffix .h, $(DICT)))
DEPS     = $(patsubst $(SRC)/%.C,$(DEP)/%.d,$(SRCS))
OBJS     = $(patsubst $(SRC)/%.C,$(OBJ)/%.o,$(SRCS))
OBJS     += $(patsubst $(SRC)/%.C,$(OBJ)/%.o,$(DSRC))


#-------------------------------------
# Rules
#-------------------------------------

all: lib 

test: 
	echo $(OBJS)

lib: $(LIB)/libTracker.a $(LIB)/libTracker.so

$(LIB)/libTracker.so : $(OBJS)
	@echo Bulding $@
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi
	@$(CXX) $(SOFLAGS) $(LDFLAGS) -o $@ $^

$(LIB)/libTracker.a : $(OBJS)
	@echo Bulding $@
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi
	@$(AR) r $@ $^ 

$(DSRC) : $(INCS)
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
	@rm -f $(LIB)/libTracker.a $(LIB)/libTracker.so $(DSRC) $(DINC)
	@rm -fr $(OBJ) $(DEP)

### Generate html document with doxygen ###
doxygen:
	@$(DOXYGEN) config/Doxyfile

### Check CVS status ###
status:
	@cvs status | grep 'Status:' | grep -v 'Up-to-date' | cat
