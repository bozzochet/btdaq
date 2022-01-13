CXX := $(shell root-config --cxx)

CLASSES = Cluster.hh LadderConf.hh EventUtils.hh 
TEMPLATE_CLASSES = GenericEvent.hh

DEC_SOURCES = DecodeData.cxx DecodeDataAMS.cxx DecodeDataOCA.cxx DecodeDataFOOT.cxx EventAMS.cxx EventOCA.cxx FSUtils.cxx main.cxx rootElibdict.cxx
DEC_SOURCES+= $(CLASSES:%.hh=%.cxx)
DEC_OBJ=$(DEC_SOURCES:%.cxx=obj/%.o)

LIB_SOURCES = rootElibdict.cxx
LIB_SOURCES += $(CLASSES:%.hh=%.cxx)
LIB_OBJ=$(LIB_SOURCES:%.cxx=obj/%.o)

ANYOPTION=AnyOption

ROOTVERSION      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-1)

ROOTINC = $(shell root-config --incdir)
ROOTDYNAMICLIBS := $(shell root-config --libs) -lMinuit

LDFLAGS = $(shell root-config --ldflags)
#CFLAGS = $(shell root-config --cflags) -g -fPIC

### For Release:
CFLAGS:= -g -O3 -fPIC -Wno-deprecated

### For Debug:
#CFLAGS:= -g -O0 -fPIC -Wno-deprecated
#CFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address

CFLAGS += $(shell $(ROOTSYS)/bin/root-config --auxcflags)

ifneq ($(ROOTVERSION),6)
        CFLAGSFORCINT	:= -c -p $(CFLAGS)
endif

SOFLAGS=
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	SOFLAGS = -dynamic -shared -dynamiclib -undefined dynamic_lookup
#	CFLAGSFORCINT=$(subst -stdlib=libc++ -std=c++11,,$(CFLAGS))
else
	SOFLAGS = -shared
endif

all: Decode lib

lib: libEvent.so libEventa.a

libEvent.so:$(LIB_OBJ)
	@$(CXX) $(LDFLAGS) $(SOFLAGS) -o $@ $^ 

libEventa.a:$(LIB_OBJ)
	@ar rv $@ $^

Decode: $(ANYOPTION)/anyoption.o $(DEC_OBJ)
	@$(CXX) $(LDFLAGS) -o$@ $^ -I$(ANYOPTION) $(ROOTDYNAMICLIBS)

obj/%.o:%.cxx
	@echo Compiling $< ...;
	@ if ! [ -d ./obj ] ; then  mkdir -p obj; fi
	@$(CXX) $(CFLAGS) -c -Wall -Wno-overloaded-virtual $< -o$@ -I$(ROOTINC) -I$(ANYOPTION)

$(ANYOPTION)/anyoption.o :
	make -C $(ANYOPTION)

rootElibdict.cxx: $(CLASSES) $(TEMPLATE_CLASSES) LinkDef.h
	@echo Creating ROOT dictionary
	@$(ROOTSYS)/bin/rootcint -f $@ $(CFLAGSFORCINT) $^

clean:
	@rm -Rfv libEventa.a libEvent.so Decode rootElibdict.cxx rootElibdict.h obj
	make -C $(ANYOPTION) clean
	@rm -f rootElibdict_rdict.pcm
