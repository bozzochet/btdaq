SHELL=/bin/sh
OS := $(shell uname)

ifeq  "$(OS)"  "Linux" 
      PREF := $(shell python2.2 -c "import sys; print sys.prefix")
else
      $(error Python is not prepared for $(OS) yet)
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

AMSWD := ..

ifeq  "$(OS)"  "Linux"
ifeq ($(ARCH),linuxicc)
      STATICLIB := -L$(AMSWD)/lib/$(MY_OS)/icc -lntuple
else
      STATICLIB := -L$(AMSWD)/lib/$(MY_OS) -lntuple
endif
endif

GCINCS=-I$(PREF)/include/python2.2 -I$(AMSWD)/include -I. -I$(ROOTINCDIR)
LIBS= $(STATICLIB) \
-L$(ROOTLIBDIR) -lCore -lCint -lHist -lTree -lMatrix -lMinuit \
-lGraf -lGraf3d -lGpad -lGX11 -lGX11TTF -lGui -lHistPainter -lPostscript \
-lRint -lHtml -lRFIO
MYLIB=$(MY_OS)/_$(file).so
MYPY=$(MY_OS)/$(file).py
MYPYC=$(MY_OS)/$(file).pyc
PYTHONLIB=$(PREF)/lib/python2.2/lib-dynload/_$(file).so
PYTHONPY=$(PREF)/lib/python2.2/$(file).py
PYTHONPYC=$(PREF)/lib/python2.2/$(file).pyc

all : $(MYPY) $(MYLIB)
      
install : $(PYTHONLIB) $(PYTHONPY)
	cp -f $(MYPYC) $(PYTHONPYC)

$(PYTHONLIB) : $(MYLIB)
	cp -f $(MYLIB) $(PYTHONLIB)

$(PYTHONPY) : $(MYPY)
	cp -f $(MYPY) $(PYTHONPY)

$(MYPY) : 
	./configure
	mv -f $(file).py $(MYPY)
	cd $(MY_OS)
	python2.2 -c "import py_compile; py_compile.compile('$(MYPY)')"

$(MYLIB) : $(file)_wrapnew.o
	$(LD) -shared $(file)_wrapnew.o -o $(MYLIB) $(LIBS)

$(file)_wrapnew.o : $(file)_wrapnew.cxx
	$(GC) $(GCINCS) -c $(file)_wrapnew.cxx
      
$(file)_wrapnew.cxx :
	./configure
	mv -f $(file).py $(MYPY)

clean_wrap :
	rm -f $(file)_wrap* $(file).h

clean :
	rm -f $(file)_wrap* $(file).h
	cd $(MY_OS)
	rm -f $(MYLIB) $(MYPY)*
