#  $Id: Makefile,v 1.8 2008/04/18 19:41:58 haino Exp $
#
# Makefile to build tkdev/GbatchInt - Gbatch interface library
#
# This Makefile is confirmed to work on 
#  SLC 4.5 with cernlib/2004, root-5.10.00, and Gbatch v4.00/189/2
#
#   21/Nov/2007 SH  First test version
#   26/Nov/2007 SH  First stable version
#   03/Dec/2007 SH  First import of tkrec module
#   21/Jan/2008 SH  Imported to tkdev (test version)
#
# $Date: 2008/04/18 19:41:58 $
#
# $Revision: 1.8 $

### Set local system parameters ###
include config/Makefile.local

### CERNLIB ###
G3LIB    = lib/libgeant321.a
CERNLIBS = $(G3LIB) $(shell $(CERNCFG) mathlib graflib pawlib)

### ROOT ###
ROOTFLGS = $(shell $(ROOTCFG) --cflags)
ROOTLIBS = $(shell $(ROOTCFG) --libs)

### AMS/Gbatch libs ###
AMSLIB   = $(AMSSW)/AMSlib/$(ARCH)/libamsut.a $(AMSSW)/AMSlib/$(ARCH)/libnag.a
AMSGBLIB = lib/libAMSGB.a
AMSGBOBJ = $(AMSGB)/bin/$(ARCH)
OBJLAST  = $(shell ls -tr $(AMSGBOBJ)/*.o | tail -1)

### Tracker lib ###
TRLIBREF = ../../TrackerLib
TRLIB    = $(shell if [ ! -d $(INC)/trlib ]; then\
	     ln -s $(TRLIBREF)/include $(INC)/trlib; fi;\
	     if [ ! -d $(CSRC)/trlib ]; then\
	     ln -s $(TRLIBREF)/CC $(CSRC)/trlib; fi;\
	     echo trlib)

### Fortran compiler flags ###
FCFLAGS  = -g -O -Wno-globals\
	   -I$(AMSGB)/include -I$(AMSGB)/F -I$(CERNDIR)/include

### C++ compiler flags ###
INCDIRS  = -I. $(addprefix -I$(INC)/, $(SUBDIR)) -I$(AMSGB)/include
CXXFLAGS = $(INCDIRS) $(ROOTFLGS) $(AMSFLAGS)

### Linker flags  ###
LDFLAGS  = $(AMSLIB) $(CERNLIBS) $(ROOTLIBS) -lg2c

### Scripts and config. files ###
MAKELIBS = config/makelibs
INCDFILE = config/incdirs

### ROOT check ###
ROOTCHK  = $(ROOT) -q -l -b | grep '(int)(-1)' | wc -l


### Project name ###
PROJ     = GbInt

### Directories ###
INC      = include
FSRC     = F
CSRC     = CC
DEP      = dep
OBJ      = obj

### Sub directories ###
GBINT    = gbint
TRSIM    = trsim
TEMP     = tmp
SUBDIR   = $(GBINT) $(TRLIB) $(TRSIM)
INCDIR   = $(addprefix $(INC)/, $(SUBDIR))
OBJDIR   = $(OBJ) $(addprefix $(OBJ)/, $(SUBDIR) $(TEMP))
DEPDIR   = $(DEP) $(addprefix $(DEP)/, $(SUBDIR))
CCTMP    = $(CSRC)/$(TEMP)


### Fortran codes ###
CODEF    = gffgoi gffgof ffone alignfit

### Gbatch interface codes (ROOT based)
CODEGS   = GbatchInterface GbatchAMS GbatchTRK AlignFitGB
CODEG    = $(addprefix $(GBINT)/, $(CODEGS))

### Gbatch interface codes (non ROOT based)
CODENS   = GbatchDefs tkinits tkgeom tkcalls 
CODEN    = $(addprefix $(GBINT)/, $(CODENS))

### TrackerLib codes ###
CODETS   = point TkDBc TkObject TkPlane TkLadder TkCoo TkSens\
	   TrCalDB TrLadCal TrBase\
	   TrRawCluster TrCluster TrRecHit TrTrack Event\
	   TrReconstruction TrField TrFit AlignFit

CODET    = $(addprefix $(TRLIB)/, $(CODETS))

### Tracker Simulation codes ###
CODESS   = TrGeom TrMCCluster TrDigi
CODES    = $(addprefix $(TRSIM)/, $(CODESS))

### ROOT dictionary ###
LINK     = LinkDef
DICT     = $(TEMP)/$(PROJ)Dict

### C++ codes ###
CODEC    = $(CODEG) $(CODEN) $(CODET) $(CODES)

### All the codes ###
CODEA    = $(CODEC) $(CODEF) $(DICT)


### Targets ###
SLIB     = lib/lib$(PROJ).so
DSRC     = $(addprefix $(CSRC)/, $(addsuffix .C, $(DICT)))
DINC     = $(addprefix $(CSRC)/, $(addsuffix .h, $(DICT)))
INCS     = $(addprefix $(INC)/,  $(addsuffix .h, $(CODEG) $(CODET) $(LINK)))
DEPS     = $(addprefix $(DEP)/,  $(addsuffix .d, $(CODEC)))
OBJS     = $(addprefix $(OBJ)/,  $(addsuffix .o, $(CODEA)))

#-------------------------------------
# Rules
#-------------------------------------

all: $(SLIB)

### Librarys build rules ###
$(SLIB): $(OBJS) $(OBJ)/$(TEMP)/geant3.o $(AMSGBLIB) 
	@if [ ! -d lib ]; then \
	 echo "Creating   lib/"; mkdir lib; fi
	@echo "Creating   $@"
	@$(LD) -shared $^ $(LDFLAGS) -o $@
	@echo "Checking   $@"
	@if [ `$(ROOTCHK)` = 1 ]; then rm -f $@; /dev/null 2> /dev/null; fi

$(AMSGBLIB): $(OBJLAST) $(MAKELIBS)
	@$(MAKELIBS)

### ROOT dictionary generation ###
$(DSRC) : $(INCS)
	@if [ ! -d $(CCTMP) ]; then \
	 echo "Creating   $(CCTMP)"; mkdir $(CCTMP); fi
	@echo "Generating $@"
	@$(CINT) -f $@ -c $^

### Source compile rules ###
$(OBJ)/%.o : $(FSRC)/%.F
	@for dir in $(OBJ); do \
         if [ ! -d $$dir ]; then \
	 echo "Creating   $$dir/"; mkdir $$dir; fi; done
	@echo "Compiling  $<"
	@$(FC) $(FCFLAGS) $< -c -o $@

$(OBJ)/%.o : $(CSRC)/%.C
	@for dir in $(OBJDIR); do \
         if [ ! -d $$dir ]; then \
	 echo "Creating   $$dir/"; mkdir $$dir; fi; done
	@if [ -f $(INCDFILE) ]; then rm -f $(INCDFILE); fi
	@echo $(INCDIRS) > $(INCDFILE)
	@echo "Compiling  $<"
	@$(CXX) $(CXXFLAGS) $< -c -o $@

$(DEP)/%.d : $(CSRC)/%.C
	@for dir in $(DEPDIR); do \
	 if [ ! -d $$dir ]; then \
	 echo "Creating   $$dir/"; mkdir $$dir; fi; done
	@echo "Creating   $@"
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(OBJ)/$*.o -MF $@

### Special rules ###
$(CCTMP)/geant3.C: $(AMSGB)/CC/geant3.C
	@if [ ! -d $(CCTMP) ]; then \
	 echo "Creating   $(CCTMP)"; mkdir $(CCTMP); fi
	@echo "Generating $@"
	@cat $^ | sed 's/gukine/gbkine/g'\
	        | sed 's/gustep/gbstep/g'\
	        | sed 's/guout/gbout/g'\
	        | sed 's/islog=floor/islog=(integer)floor/'\
	        > $@

### Include dependency files ###
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),status)
ifneq ($(MAKECMDGOALS),doxygen)
-include $(DEPS)
endif
endif
endif

### Generate html document with doxygen ###
doxygen:
	@$(DOXYGEN) config/Doxyfile

### CVS status ###
status:
	@cvs status | grep 'Status:' | grep -v 'Up-to-date' | cat

### Cleanup ###
clean:
	@echo "Cleaning up"
	@rm -f $(SLIB) $(G3LIB) $(AMSGBLIB) $(DEPS)\
               $(CCTMP)/* $(OBJ)/*.o $(OBJ)/*/*.o
