#  $Id: Makefile,v 1.16 2008/11/18 10:06:51 haino Exp $
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
# $Date: 2008/11/18 10:06:51 $
#
# $Revision: 1.16 $

#-------------------------------------
# Settings
#-------------------------------------
### Directories ###
SRC      = CC
DEP      = dep
OBJ      = obj
BIN      = ../bin

### Absolute libraty path is needed for TDisplay
ifeq ($(shell basename $(PWD)),TrackerTools)
	LIB = $(PWD)/../lib
else
	LIB = $(PWD)/lib
endif

CPPFLAGS = -I. -Iinclude -I../AMSbase/include -I../TrackerLib/include
LDFLAGS  = $(shell root-config --glibs) -lRGL -lGeom -lGed

include ../Make.common

### Classes ###
CLASSES  = TrDHF RTDisplay3D RTGLViewer

### ROOT dictionary ###
LINK     = LinkDef
DICT     = dictTOOLS

SRCS     = $(addprefix $(SRC)/,$(addsuffix .C, $(CLASSES)))


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

lib: $(LIB)/libTrackerTools.a $(LIB)/libTrackerTools.so

$(LIB)/libTrackerTools.so : $(OBJS)
	@echo Bulding $@
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi
	@$(CXX) $(SOFLAGS) $(LDFLAGS) -o $@ $^

$(LIB)/libTrackerTools.a : $(OBJS)
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
	@rm -f $(LIB)/libTrackerTools.a $(LIB)/libTrackerTools.so $(DSRC) $(DINC)
	@rm -fr $(OBJ) $(DEP)

### Generate html document with doxygen ###
doxygen:
	@$(DOXYGEN) config/Doxyfile

### Check CVS status ###
status:
	@cvs status | grep 'Status:' | grep -v 'Up-to-date' | cat
