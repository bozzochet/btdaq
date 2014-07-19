SHELL=/bin/sh
OS := $(shell uname)

ifeq  "$(OS)"  "Linux" 
      RUBY := $(shell which ruby 2> /dev/null)
else
      $(error Ruby is not prepared for $(OS) yet)
endif

ifndef RUBY
	RUBY=/afs/cern.ch/user/a/alcaraz/public/$(OS)/bin/ruby
endif

MY_OS := $(shell uname | tr '[A-Z]' '[a-z]')
file= AMS

ARCH := $(shell $(ROOTSYS)/bin/root-config --arch)
ifeq ($(ARCH),linuxicc)
      $(warning RubyRoot wrapping is not prepared for the ICC compiler yet. )
      $(error Please use a Root version compiled with g++.)
      #GC=/opt/intel/compiler70/ia32/bin/icc -w
      #LD=/opt/intel/compiler70/ia32/bin/icc
else
      GC=g++
      LD=g++
endif

ROOTLIBDIR := $(shell root-config --libdir)
ROOTINCDIR := $(shell root-config --incdir)

LIB_DIR := $(shell $(RUBY) -r rbconfig -e 'puts Config::CONFIG["sitedir"]')
INC_DIR := $(shell $(RUBY) -r rbconfig -e 'puts Config::CONFIG["rubylibdir"] + "/" + Config::CONFIG["arch"]')

AMSWD := ..

ifeq  "$(OS)"  "Linux"
ifeq ($(ARCH),linuxicc)
      STATICLIB := -L$(AMSWD)/lib/$(MY_OS)/icc -lntuple
else
      STATICLIB := -L$(AMSWD)/lib/$(MY_OS) -lntuple
endif
endif

GCINCS=-I$(INC_DIR) -I$(AMSWD)/include -I. -I$(ROOTINCDIR)
LIBS= $(STATICLIB) \
-L$(ROOTLIBDIR) -lCore -lCint -lHist -lTree -lMatrix -lMinuit \
-lGraf -lGraf3d -lGpad -lGX11 -lGX11TTF -lGui -lHistPainter -lPostscript \
-lRint -lHtml -lRFIO
MYLIB=$(MY_OS)/$(file).so
RUBYLIB=$(LIB_DIR)/$(file).so

all : $(MYLIB)
      
install : $(RUBYLIB)

$(RUBYLIB) : $(MYLIB)
	cp -f $(MYLIB) $(RUBYLIB)

$(MYLIB) : $(file)_wrapnew.o
	$(LD) -shared $(file)_wrapnew.o -o $(MYLIB) $(LIBS)

$(file)_wrapnew.o : $(file)_wrapnew.cxx
	$(GC) $(GCINCS) -c $(file)_wrapnew.cxx

$(file)_wrapnew.cxx : 
	./configure

clean_wrap :
	rm -f $(file)_wrap* $(file).h

clean :
	rm -f $(file)_wrap* $(file).h
	cd $(MY_OS)
	rm -f $(MYLIB)
