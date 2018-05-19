CXX := $(shell root-config --cxx)

CLASSES = Cluster.hh LadderConf.hh Event.hh

DEC_SOURCES = DecodeData.cxx main.cxx rootElibdict.cxx
DEC_SOURCES+= $(CLASSES:%.hh=%.cxx)
DEC_OBJ=$(DEC_SOURCES:%.cxx=obj/%.o)

LIB_SOURCES = rootElibdict.cxx
LIB_SOURCES += $(CLASSES:%.hh=%.cxx)
LIB_OBJ=$(LIB_SOURCES:%.cxx=obj/%.o)

ANYOPTION=AnyOption

ROOTINC = $(shell root-config --incdir)
ROOTDYNAMICLIBS := $(shell root-config --libs) -lMinuit

#CFLAGS= -O -std=c++11 -g -fPIC
CFLAGS = $(shell root-config --cflags) -g -fPIC

LDFLAGS = $(shell root-config --ldflags)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	SOFLAGS = -dynamic -shared -dynamiclib -undefined dynamic_lookup
        CFLAGSFORCINT=$(subst -stdlib=libc++ -std=c++11,,$(CFLAGS))	
else
        SOFLAGS = -shared
        CFLAGSFORCINT=$(CFLAGS)
endif

all: Decode lib

lib: libEvent.so libEventa.a

libEvent.so:$(LIB_OBJ)
	$(CXX) $(LDFLAGS) $(SOFLAGS) -o $@ $^ 

libEventa.a:$(LIB_OBJ)
	ar rv $@ $^

Decode: $(ANYOPTION)/anyoption.o $(DEC_OBJ)
	$(CXX) $(LDFLAGS) -o$@ $^ -I$(ANYOPTION) $(ROOTDYNAMICLIBS)

obj/%.o:%.cxx
	@echo Compiling $< ...;
	@ if ! [ -d ./obj ] ; then  mkdir -p obj; fi
	@$(CXX) $(CFLAGS) -c -Wall -Wno-overloaded-virtual $< -o$@ -I$(ROOTINC) -I$(ANYOPTION)

$(ANYOPTION)/anyoption.o :
	make -C $(ANYOPTION)

rootElibdict.cxx: $(CLASSES) LinkDef.h
	@echo Creating ROOT dictionary
	rootcint -f $@ -c -p $(CFLAGSFORCINT) $^

clean:
	@rm -Rfv libEventa.a libEvent.so Decode rootElibdict.cxx rootElibdict.h obj
	make -C $(ANYOPTION) clean
