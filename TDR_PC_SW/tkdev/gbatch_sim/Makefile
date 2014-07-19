#  $Id: Makefile,v 1.4 2008/10/23 10:12:39 zuccon Exp $
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
# $Revision: 1.4 $

#-------------------------------------
# Settings
#-------------------------------------
### Directories ###
SRC      = src
DEP      = dep
OBJ      = obj
LIB      = ../lib
BIN      = ../bin

CPPFLAGS = -I.  -Isrc -I../TrackerLib/include -I../AMSbase/include -I../FORTinterface/include -I../AMSGeom/include

include ../Make.common

FFLAGS = -g -I/cern/pro/include

### Classes ###
CLASSES  = AMSJob.C AMSEvent.C mceventg.C geant3.C cern.C amsgeom.C gbatch.C geant.C gufld.C VCon_gbatch.C commons.C
FSRC     =  Geant3_utils.F

SRCS     = $(addprefix $(SRC)/, $(CLASSES))
SRCS     += $(addprefix $(SRC)/,$(FSRC))

OBJS1     = $(patsubst $(SRC)/%.C,$(OBJ)/%.o,$(SRCS))
OBJS     += $(patsubst $(SRC)/%.F,$(OBJ)/%.o,$(OBJS1))

DEPS1     = $(patsubst $(SRC)/%.C,$(DEP)/%.d,$(SRCS))
DEPS      = $(patsubst $(SRC)/%.F,$(DEP)/%.d,$(DEPS1))

CERNLIBS= `cernlib geant grafX11 mathlib`



AMSLIBS=$(LIB)/libTracker.a  $(LIB)/libAMSGeom.a $(LIB)/libfort.a
#-------------------------------------
# Rules
#-------------------------------------

all: main


../lib/libAMSbase.a:
	$(MAKE) -C ../AMSbase

../lib/libTracker.a:
	$(MAKE) -C ../TrackerLib

../lib/libfort.a:
	$(MAKE) -C ../FORTinterface
			
../lib/libAMSGeom.a:
	$(MAKE) -C ../AMSGeom

lib:  ../lib/libAMSGeom.a ../lib/libTracker.a ../lib/libfort.a ../lib/libAMSbase.a
	rm -fr tmp
	mkdir tmp
	cd tmp && ar x ../../lib/libAMSbase.a
	cd tmp && ar x ../../lib/libTracker.a
	cd tmp && ar x ../../lib/libAMSGeom.a
	cd tmp && ar x ../../lib/libfort.a

mylib: $(OBJS)
	ar r ../lib/libAll.a $(OBJS)
	ar d ../lib/libAll.a gbatch.o

main: $(OBJS) lib
	$(CXX) -g -m32 -o $@  $(OBJS)  tmp/*.o   \
	$(CERNLIBS)  -lg2c -L/usr/X11R6/lib/ -lX11  \
	`root-config --libs`



### Include dependency files ###
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),test)
-include $(DEPS)
endif
endif

.PHONY: clean doxygen status

### Cleanup ###
clean:
	@rm -fr tmp main $(OBJ) $(DEP)

