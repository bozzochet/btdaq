
CLASSES=   Cluster.hh Event.hh 

DEC_SOURCES=  DecodeData.cxx    main.cxx  rootElibdict.cxx 
DEC_SOURCES+= $(CLASSES:%.hh=%.cxx)
DEC_OBJ=$(DEC_SOURCES:%.cxx=obj/%.o)

LIB_SOURCES =    rootElibdict.cxx
LIB_SOURCES += $(CLASSES:%.hh=%.cxx)
LIB_OBJ=$(LIB_SOURCES:%.cxx=obj/%.o)

CFLAGS= -g -fPIC  
DEFINES=

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	SOFLAGS = -dynamic -shared -dynamiclib -undefined dynamic_lookup
else
        SOFLAGS = -shared
endif

all: Decode lib

lib:  libEvent.so

libEvent.so:$(LIB_OBJ)
	$(CXX) $(CFLAGS) $(DEFINES) $(SOFLAGS) -o $@ $^ 

libEvent.a:$(LIB_OBJ)
	$(CXX) $(CFLAGS) $(DEFINES) -o $@ $^ 

Decode: $(DEC_OBJ)
	$(CXX) $(CFLAGS) -o$@ $^  `$(ROOTSYS)/bin/root-config --libs`

obj/%.o:%.cxx 
	@echo Compiling $< ...; 
	@ if ! [ -d ./obj ] ; then  mkdir -p obj; fi
	@$(CXX) $(CFLAGS) $(DEFINES) -c -Wall $< -o$@  -I$(ROOTSYS)/include

rootElibdict.cxx: $(CLASSES) LinkDef.h
	@echo Creating ROOT dictionary
	rootcint -f $@  -c -p $(CFLAGS) $(DEFINES) $^

clean:
	@rm -fv libEvent.a libEvent.so Decode rootElibdict.cxx rootElibdict.h obj/*.o 
	@rm -frv obj
