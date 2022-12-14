CXX := $(shell root-config --cxx)

CLASSES = Cluster.hh LadderConf.hh EventUtils.hh
TEMPLATE_CLASSES = GenericEvent.hh GenericEvent.hpp

DEC_SOURCES = DecodeData.cxx DecodeDataAMS.cxx DecodeDataOCA.cxx DecodeDataFOOT.cxx EventAMS.cxx EventOCA.cxx FSUtils.cxx main.cxx rootElibdict.cxx
DEC_SOURCES += DecodeDataAMSL0.cxx EventAMSL0.cxx
DEC_SOURCES+= $(CLASSES:%.hh=%.cxx)
DEC_OBJ=$(DEC_SOURCES:%.cxx=obj/%.o)

LIB_SOURCES = rootElibdict.cxx
LIB_SOURCES += $(CLASSES:%.hh=%.cxx)
LIB_OBJ=$(LIB_SOURCES:%.cxx=obj/%.o)

CLASSES_HEADERS = $(CLASSES:%=include/%)
TEMPLATE_CLASSES_HEADERS = $(TEMPLATE_CLASSES:%=include/%)

ANYOPTION=AnyOption

ROOTVERSION      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-1)

ROOTINC = $(shell root-config --incdir) -I./ -I./include
ROOTDYNAMICLIBS := $(shell root-config --libs) -lMinuit

LDFLAGS = $(shell root-config --ldflags)
#CFLAGS = $(shell root-config --cflags) -g -fPIC

### For Debug:
# CFLAGS:= -g -O0 -fPIC -Wno-deprecated
# CFLAGS += -fsanitize=address
# LDFLAGS += -fsanitize=address

### For Release:
CFLAGS:= -g -O3 -fPIC -Wall -Wno-overloaded-virtual -Wno-deprecated -Wno-unused-but-set-variable -Wno-unused-variable

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

all: Decode lib PlotCalib

lib: libEvent.so libEventa.a

libEvent.so:$(LIB_OBJ)
	@$(CXX) $(LDFLAGS) $(SOFLAGS) -o $@ $^ 

libEventa.a:$(LIB_OBJ)
	@ar rv $@ $^

Decode: $(ANYOPTION)/anyoption.o $(DEC_OBJ)
	@echo Linking $@ ...;
	@$(CXX) $(LDFLAGS) -o$@ $^ -I$(ANYOPTION) $(ROOTDYNAMICLIBS)

obj/%.o:src/%.cxx
	@echo Compiling $< ...;
	@ if ! [ -d ./obj ] ; then  mkdir -p obj; fi
	@$(CXX) $(CFLAGS) -c $< -o$@ -I$(ROOTINC) -I$(ANYOPTION)

$(ANYOPTION)/anyoption.o :
	@make -C $(ANYOPTION)

src/rootElibdict.cxx: $(CLASSES_HEADERS) $(TEMPLATE_CLASSES_HEADERS) include/LinkDef.h
	@echo Creating ROOT dictionary
	@$(ROOTSYS)/bin/rootcint -f $@ $(CFLAGSFORCINT) $^
	ln -fs src/rootElibdict_rdict.pcm ./

PlotCalib: obj/summary.o
	@echo Linking $@ ...;
	@$(CXX) $(LDFLAGS) -o$@ $^ -I$(ANYOPTION) $(ROOTDYNAMICLIBS)
	@ln -fs $@ $@Mute

clean:
	@rm -Rfv libEventa.a libEvent.so Decode src/rootElibdict.cxx include/rootElibdict.h obj PlotCalib PlotCalibMute
	@make -C $(ANYOPTION) clean
	@rm -f rootElibdict_rdict.pcm

print:
	@echo $(DEC_OBJ)
	@echo $(LIB_OBJ)
	@echo $(LIB_SOURCES)
	@echo $(CLASSES_HEADERS)
	@echo $(TEMPLATE_CLASSES_HEADERS)
