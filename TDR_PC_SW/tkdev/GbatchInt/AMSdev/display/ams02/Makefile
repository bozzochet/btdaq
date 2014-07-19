#  $Id: Makefile,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
# ------------------------------
# Makefile for AMS Event Display
# ------------------------------
#
# Requirements:
#                1) Gnu-make should be used
#                2) ROOTSYS should be defined for location of ROOT
#****************************************************************

#--gcc used to produce dependencies
 GCC  = gcc 

#################
# Common MACRO  #
#################
#ROOTLIBS      = -L$(ROOTSYS)/lib -lNew -lBase -lCint -lClib -lCont -lFunc \
#                -lGraf -lGraf3d -lHist -lHtml -lMatrix -lMeta -lMinuit -lNet \
#                -lPostscript -lProof -lTree -lUnix -lZip

ROOTLIBS      = -L$(ROOTSYS)/lib  -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lTreePlayer -lProof 

#ROOTLIBS      =  -L$(ROOTSYS)/lib  -lRoot

ROOTGLIBS     = -lRint -lGpad -lGui -lGX11 -lX3d
#ROOTGLIBS     = 
CERNLIBS      = -L$(CERN)/2001/lib -lmathlib -lpacklib -lkernlib
#OPT           = -g -DROOT10308
OPT           = -g 
#OPT           = -O 
NOOPT         =
ARLD          = ar vr

####################
#                  #
# operating system #
#                  #
####################

#--List of supported platforms so far
SUPPORTED = AIX OSF1 HP-UX Linux

OS    = $(shell uname)
FOUND = $(findstring $(OS),$(SUPPORTED))

#--Directory to put object files, executive and library produced
OBJDIR = ./$(OS)/bin

##############
#            #
#  IBM/AIX   #
#  =======   #
##############
ifeq  "$(OS)"  "AIX"
 CXX           = xlC
 INCLUDES      = -I$(ROOTSYS)/include -I/usr/lpp/xlC/include
 CXXFLAGS      = -O -qnoro -qnoroconst -qmaxmem=-1  $(INCLUDES) 
 FF            = xlf
 FFLAGS        = -g -qextname
 LD            = xlC
 LDFLAGS       = -g
# SOFLAGS       = -b
 SOFLAGS       =
 SOGFLAGS      = $(SOFLAGS)

 FORLIBS       = -lxlf90 -lxlf $(CERNLIBS)
 ADDLIBS       =
 ROOTLIBS      = -L$(ROOTSYS)/lib -lRoot -lCint
 ROOTGLIBS     = -lGpad -lGX11 -lMotif -lWidgets -lX3d
 LIBS          = $(ROOTLIBS) -lm -lcurses
 GLIBS         = $(ROOTLIBS) -lXm -lXt -lX11 -lm -lPW -lld -lcurses
 SHLD          = /usr/lpp/xlC/bin/makeC++SharedLib $(LIBS) $(FORLIBS) -p 0
endif

##############
#            #
#  DEC/OSF1  #
#  =======   #
##############
ifeq  "$(OS)"  "OSF1"
 GCC  = cxx 
 CXX           = cxx
 INCLUDES      = -I$(ROOTSYS)/include -I/usr/include -I/usr/include/cxx 
 CXXFLAGS      = -D__osf__  -D__alpha -D_ALPHA_  $(INCLUDES) 
 FF            = f77
 FFLAGS        = -g
# LD            = cxx -non_shared
 LD            = cxx 
 LDFLAGS       = -g
 SOFLAGS       =
 SOGFLAGS      = $(SOFLAGS)
# SOFLAGS       = -Wl,-expect_unresolved,* -call_shared
# SOGFLAGS      = -Wl,-expect_unresolved,* -call_shared

 FORLIBS       = $(CERNLIBS) -lUfor -lfor -lFutil
 ADDLIBS       = $(FORLIBS)
 LIBS          = $(ROOTLIBS) -lm
 GLIBS         = $(ROOTLIBS) $(ROOTGLIBS) -lXpm -lX11 -lm -lPW
 SHLD          = ld -L/usr/lib/cmplrs/cxx -rpath /usr/lib/cmplrs/cxx \
                 -expect_unresolved -g -shared /usr/lib/cmplrs/cc/crt0.o \
                 /usr/lib/cmplrs/cxx/_main.o -lcxxstd -lcxx -lexc -lots -lc
endif


##############
#            #
#   HP-UX    #
#   =====    #
##############
ifeq "$(OS)"  "HP-UX"
 CXX           = CC
 INCLUDES      = -I$(ROOTSYS)/include 
 CXXFLAGS      = -O +a1 +Z  $(INCLUDES) 
 FF            = f77
 FFLAGS        = -g +ppu +Z
 LD            = CC
 LDFLAGS       = -g +a1 -z
 SOFLAGS       = -b
 SOGFLAGS      = $(SOFLAGS)

 FORLIBS       = $(CERNLIBS) -L/usr/local/lib -lF77 -lI77 -lc
 ADDLIBS       =
 LIBS          = $(ROOTLIBS) -lm $(FORLIBS)
 GLIBS         = $(ROOTLIBS) $(ROOTGLIBS) -L/usr/lib/X11R5 \
                 -lXpm -lX11 -lm -ldld $(FORLIBS)
 SHLD          = $(LD) $(SOFLAGS) $(LDFLAGS)
endif

##############
#            #
#  PC/Linux  #
#  ========  #
##############
ifeq "$(OS)"  "Linux"
 CXX           = g++
 INCLUDES      = -I$(ROOTSYS)/include 
 CXXFLAGS      =  -Wall -fPIC  $(INCLUDES) 
 FF            = g77
 FFLAGS        =
 LD            = g++
 LDFLAGS       = -g
# SOFLAGS       = -Wl,-soname,$(AMSR_SHAREDLIB) -shared
# SOGFLAGS      = -Wl,-soname,$(AMSR_GLIB) -shared
 SOFLAGS       = -shared
 SOGFLAGS      = $(SOFLAGS)

 FORLIBS       = $(CERNLIBS) -lm -lnsl -lg2c
# FORLIBS       = $(CERNLIBS) -lg2c -lF77 -lI77 -lc
# FORLIBS       = $(CERNLIBS) -L/usr/local/lib -lf2c /usr/lib/libstdc++.so
 ADDLIBS       = $(FORLIBS)
 LIBS          = $(ROOTLIBS)  -lm -ldl -rdynamic
 GLIBS         = $(ROOTLIBS) $(ROOTGLIBS) -L/usr/X11R6/lib \
                 -lXpm -lX11  -lm -lcrypt -ldl -rdynamic
 SHLD          = $(LD) $(SOFLAGS) $(LDFLAGS)
endif

##### RULES #####

.SUFFIXES:
.SUFFIXES: .cxx .f

$(OBJDIR)/%.o:    %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o:    %.f
	$(FF)  $(OPT) $(FFLAGS) -c $< -o $@


##### MACROS and TARGETS #####

include Make-bin


##### DEPENDENCIES #####

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),touch_depend)
ifneq ($(MAKECMDGOALS),dist)
#ifneq "$(FOUND)" ""
 include $(OBJDIR)/Make-depend
#endif
endif
endif
endif
