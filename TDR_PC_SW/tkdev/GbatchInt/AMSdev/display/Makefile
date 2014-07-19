#  $Id: Makefile,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

ARCH         := $(shell $(ROOTSYS)/bin/root-config --arch)
PLATFORM      =  Linux
PREFIX        = $(AMSWD)/display/
RH9add=
RH9adda=
ifdef RH9
 RH9add=-pthread  /usr/lib/ctype.o
 RH9adda=-pthread  /usr/lib/ctype-info.o
endif

ifneq "$(shell uname -r )" "2.4.20-24.7.cernsmp"
 RH9add=-pthread  /usr/lib/ctype.o
 RH9adda=-pthread  /usr/lib/ctype-info.o
endif

ifeq "$(wildcard /usr/lib/ctype-info.o)" ""
 RH9adda= -pthread
 RH9add=  -pthread
endif
ifeq ($(ARCH),linuxicc)
CXX           =  /opt/intel/compiler80/bin/icc
CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2  -tpp6 -mp1 -xi 
OPT           = -O2
OPTC          = -O2
NOOPT         = -g
NOOPTC         = -g
LD            = /opt/intel/compiler80/bin/icc
LDFLAGS       = 
ROOTLIBS      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lcrypt -lcrypto -L/usr/X11R6/lib  -lXpm -lXmu -lXt -lXext -lX11 $(RH9add) -lshift -lpcre
ROOTLIBSA      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lcrypt -lcrypto -L/usr/X11R6/lib  -lXpm -lXmu -lXt -lXext -lX11 $(RH9adda) -lshift -lpcre
LIBS          =  $(ROOTLIBS) -lm -ldl 
LIBSA          =  $(ROOTLIBSA) -lm -ldl 
else
CXX           =  g++ -Wno-deprecated
CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
OPT           = -O2
OPTC          = -O0
NOOPT         = -g
NOOPTC         = -g
LD            = g++
LDFLAGS       = -g 
ROOTLIBS      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lAfterImage -ljpeg -lpng -ltiff -lcrypt -lcrypto -L/usr/X11R6/lib  -lXpm -lXmu -lXt -lXext -lX11 $(RH9add) -lshift -lpcre -lXrdClient -lXrd -lXrdOuc -lXrdNet
ROOTLIBSA      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lAfterImage -ljpeg -lpng -ltiff -lcrypt -lcrypto -L/usr/X11R6/lib  -lXpm -lXmu -lXt -lXext -lX11 $(RH9adda) -lshift -lpcre  -lXrdClient -lXrd -lXrdOuc -lXrdNet
LIBS          =  $(ROOTLIBS) -lm -ldl
LIBSA          =  $(ROOTLIBSA) -lm -ldl
endif
ifeq ($(ARCH),macosx)
CXX           =  g++ -Wno-deprecated
CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
OPT           = -O2
OPTC          = -O0
NOOPT         = -g
LD            = g++
LDFLAGS       = -g 
ROOTLIBS      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lcrypto -L/usr/X11R6/lib  -lXpm -lXmu -lXt -lXext -lX11  

LIBS          =  $(ROOTLIBS) $(RH9add) -lm -ldl
LIBSA          =  $(ROOTLIBS) $(RH9adda) -lm -ldl
endif
ifeq ($(ARCH),win32gcc)
PREFIX        = $(AMSWD)/display/cygwin/
CXX           =  g++ -Wno-deprecated
CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
OPT           = -O2
OPTC          = -O0
NOOPT         = -g
NOOPTC         = -g
LD            = g++
LDFLAGS       = -g 
ROOTLIBS      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lcrypt -L/usr/X11R6/lib  -lXpm.dll -lX11.dll $(RH9add) -L/usr/lib -lshift
ROOTLIBSA      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lcrypt -L/usr/X11R6/lib  -lXpm.dll -lX11.dll $(RH9adda) -L/usr/lib -lshift
LIBS          =  $(ROOTLIBS) -lm 
LIBSA          =  $(ROOTLIBSA) -lm 
endif
ifeq ($(ARCH),win32)
PREFIX   = $(AMSWD)/display/wingdk/
CXX        =  $(ROOTSYS)/build/win/cxx.sh
CXXFLAGS =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -O2 -Z7 -G5 -GR -GX -MD -DWIN32 -DGDK_WIN32  -D_WINDOWS -DWINVER=0x0400 -nologo -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -FIw32pragma.h -D_X86_=1 -D_DLL -DVISUAL_CPLUSPLUS -I../include  -I$(ROOTSYS)/include  /TP
OPT           = 
OPTC          = 
NOOPT         = -g
NOOPTC         = -g
LD = $(ROOTSYS)/build/win/ld.sh
LDFLAGS       =  -nologo -incremental:yes   -nodefaultlib:libc.lib -nodefaultlib:dfor.lib 
LIBS          =  $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libGui.lib  $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libGeom.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libTree.lib  $(ROOTSYS)/lib/libRint.lib advapi32.lib
ALIBS          = lib/libroot.a lib/libCore.lib lib/libfreetype.lib advapi32.lib
endif
GLIBS         = -L$(ROOTSYS)/lib -lGX11TTF  -lCore -lCint -lHist -lGraf -lGraf3d -lGui -lGpad -lGX11  -lTree -lRint -lPostscript -lMatrix -lPhysics  -lHistPainter -lMinuit -L/usr/X11R6/lib  -lXmu -lXt -lXext -lX11 -lXpm -lm -ldl 

##### MACROS #####

ROOTH         = TTree.h TFile.h TH1.h

SRCS          = TGRunEventDialog.cxx \
                ControlFrame.cxx \
                AMSNtupleV.cxx \
                Help.cxx

SRCSN          = AMSR_GeometrySetter.cxx \
                 AMSDisplay.cxx \
                TMarker3DCl.cxx \
                AMSR_Axis.cxx 

HDRS          =  ../include/root_RVS.h\
		ControlFrame.h \
		AMSDisplay.h \
		AMSNtupleV.h \
                AMSR_GeometrySetter.h \
                AMSR_Axis.h \
                TGRunEventDialog.h \
                TMarker3DCl.h \
                ../include/id.h \
                LinkDef.h
DICT          = AMSCint_$(ARCH).cxx
DICTH         = $(DICT:.cxx=.h)
DICTO         = $(DICT:%.cxx=$(PREFIX)%.o)


MAIN          = main.cxx
MAINH         = $(MAIN:.cxx=.h)
MAINO         = $(MAIN:%.cxx=$(PREFIX)%.o)

MAINC          = mainc.cxx
MAINOC         = $(MAINC:.cxx=.o)

OBJS          = $(SRCS:%.cxx=$(PREFIX)%.o) 
OBJSR          = $(PREFIX)root.o $(PREFIX)id.o
OBJSN          = $(SRCSN:%.cxx=$(PREFIX)%.o) 
OBJSC         =   $(PREFIX)AMSNtuple.o

ALLDICT       = $(DICT) $(DICTH) 
ALLSRCS       = $(SRCS)  $(HDRS)  $(ALLDICT) $(MAIN) $(MAINH) $(MAINC)
ALLOBJS       = $(OBJS) $(MAINO) 



AMSPRG       = $(PREFIX)amsed
AMSPRGC       = $(PREFIX)amsedc
ifeq ($(ARCH),win32)
AMSPRG       = $(PREFIX)amsed.exe
endif
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic
ifeq ($(ARCH),macosx)
AMSPRG       = nothing
LDS =  @echo "  Static Linking not supported in macosx!!! Use 'make osdepend' instead"
LDD	= 	$(LD) -dynamic
endif



##### TARGETS #####
standalone:  $(AMSPRG)

osdepend:  $(AMSPRGC)



$(AMSPRG):      $(ALLOBJS) $(DICTO) $(MAINH) $(OBJSR) $(OBJSN)  $(OBJSC)  
		$(LDS)    $(LDFLAGS) $(ALLOBJS) $(OBJSR) $(OBJSN)  $(OBJSC)  $(DICTO)  $(LIBSA)  -o $(@) 
		@echo "$(AMSPRG) done"

$(AMSPRGC):	$(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSN) $(OBJSC)  
		$(LDD)  $(LDFLAGS) $(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSN) $(OBJSC)     $(LIBS) -o $(@)  


$(DICT):        $(HDRS)
		@echo "Generating dictionary ..."
		$(ROOTSYS)/bin/rootcint -f $(DICT) -c $(ROOTH) $(HDRS)




depend:		$(PREFIX)Make-depend 
touch_depend:
		touch $(PREFIX)Make-depend
$(PREFIX)Make-depend:	
ifneq "$(shell du -a  $(PREFIX)Make-depend | grep -c  ake)" "1"
		touch $(PREFIX)Make-depend
endif
	makedepend -f$(PREFIX)Make-depend -- $(CXXFLAGS) -- $(SRCS) $(SRCSG) -p$(PREFIX) \
		$(MAIN)
		cat  $(PREFIX)Make-depend | sed -e 'sx/../CC/x/x1' > $(PREFIX)Make-depend.new
		rm -f $(PREFIX)Make-depend
		mv $(PREFIX)Make-depend.new $(PREFIX)Make-depend

dist:
		rm -f $(AMSPRG).tar.gz ; \
		tar cvf $(AMSPRG).tar $(ALLSRCS) 00index.txt Make* ; \
		gzip $(AMSPRG).tar

clean:
		@rm -f $(ALLOBJS) $(ALLDICT)


##### RULES #####

$(ALLOBJS): $(PREFIX)%.o : %.cxx 
	$(CXX) $(OPTC) $(CXXFLAGS) -c $(<)  -o $(@)

$(OBJSR): $(PREFIX)%.o : ../CC/%.C 
	$(CXX) $(OPT) $(CXXFLAGS) -c $(<)  -o $(@)

$(OBJSN): $(PREFIX)%.o : %.cxx 
	$(CXX) $(OPT) $(CXXFLAGS) -c $(<)  -o $(@)

$(OBJSC): $(PREFIX)%.o : ../online/%.cxx 
	$(CXX) $(OPT) $(CXXFLAGS) -c $(<)  -o $(@)

$(DICTO): $(PREFIX)%.o : %.cxx 
	$(CXX) $(NOOPT) $(CXXFLAGS)  -c $(<)  -o $(@)


##### DEPENDENCIES #####

ifneq ($(MAKECMDGOALS),touch_depend)
include $(PREFIX)Make-depend
endif

