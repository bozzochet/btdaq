#  $Id: Makefile,v 1.2 2008/09/18 16:53:54 zuccon Exp $
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
# $Revision: 1.2 $

#-------------------------------------
# Settings
#-------------------------------------
### Directories ###
SRC      = F
DEP      = dep
OBJ      = obj
LIB      = ../lib
BIN      = ../bin

CPPFLAGS      = -Iinclude 

include ../Make.common

FFLAGS = -g -I/cern/pro/include $(CPPFLAGS) 

### Classes ###
FSRC     = trd.F rqmd.F call_fluka.F call_rqmd.F

SRCS =  $(addprefix $(SRC)/,$(FSRC))

OBJS     = $(patsubst F/%.F,$(OBJ)/%.o,$(SRCS))

DEPS      = $(patsubst F/%.F,$(DEP)/%.d,$(SRCS))



#-------------------------------------
# Rules
#-------------------------------------




../lib/libfort.a: $(OBJS)
	@echo Bulding $@
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB);fi 
	$(AR) r $@ $^


### Include dependency files ###
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),test)
-include $(DEPS)
endif
endif

.PHONY: clean doxygen status

### Cleanup ###
clean:
	@rm -fr ../lib/libfort.a $(OBJ) $(DEP)

