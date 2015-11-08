
CLASSES = Cluster.hh Event.hh 

DEC_SOURCES = DecodeData.cxx main.cxx rootElibdict.cxx 
DEC_SOURCES+= $(CLASSES:%.hh=%.cxx)
DEC_OBJ=$(DEC_SOURCES:%.cxx=obj/%.o)

LIB_SOURCES = rootElibdict.cxx
LIB_SOURCES += $(CLASSES:%.hh=%.cxx)
LIB_OBJ=$(LIB_SOURCES:%.cxx=obj/%.o)

ANYOPTION=AnyOption

ROOTDYNAMICLIBS := $(shell root-config --glibs) -lMinuit

CFLAGS= -g -fPIC
DEFINES=

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	SOFLAGS = -dynamic -shared -dynamiclib -undefined dynamic_lookup
else
        SOFLAGS = -shared
endif

all: Decode lib

lib: libEvent.so libEventa.a

libEvent.so:$(LIB_OBJ)
	$(CXX) $(CFLAGS) $(DEFINES) $(SOFLAGS) -o $@ $^ 

libEventa.a:$(LIB_OBJ)
	ar rv $@ $^ 

Decode: $(ANYOPTION)/anyoption.o $(DEC_OBJ)
	$(CXX) $(CFLAGS) -o$@ $^ -I$(ROOTSYS)/include -I$(ANYOPTION) $(ROOTDYNAMICLIBS)

obj/%.o:%.cxx 
	@echo Compiling $< ...; 
	@ if ! [ -d ./obj ] ; then  mkdir -p obj; fi
	@$(CXX) $(CFLAGS) $(DEFINES) -c -Wall -Wno-overloaded-virtual $< -o$@  -I$(ROOTSYS)/include -I$(ANYOPTION)

$(ANYOPTION)/anyoption.o :
	make -C $(ANYOPTION)

rootElibdict.cxx: $(CLASSES) LinkDef.h
	@echo Creating ROOT dictionary
	rootcint -f $@  -c -p $(CFLAGS) $(DEFINES) $^

clean:
	@rm -fv libEventa.a libEvent.so Decode rootElibdict.cxx rootElibdict.h obj/*.o 
	@rm -frv obj
	make -C $(ANYOPTION) clean
