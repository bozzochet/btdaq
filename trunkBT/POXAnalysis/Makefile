CXX := `root-config --cxx`
ROOTCLING=rootcling
MARCH := `root-config --arch`
LD:=$(CXX)

OBJ=./obj/
SRC=./src/
INC=./include/

#CFLAGS:= -g -Wno-deprecated
CFLAGS:= -O3 -g -Wno-deprecated

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
ADDLDFLAGS = -Wl,--no-as-needed
endif
ifeq ($(UNAME), Darwin)
ADDLDFLAGS =
endif

#CFLAGS += $(shell root-config --auxcflags) -O -std=c++11 -fPIC
CFLAGS += $(shell root-config --auxcflags) -O -fPIC

ROOTDYNAMICLIBS := $(shell root-config --libs) -lMinuit -lSpectrum
ROOTINC := $(shell root-config --incdir)

DECODE = ../Decode/
DECODELIBS = -Wl,-rpath,$(DECODE) -L$(DECODE) -lEvent

COMMON = ../CommonTool/

INCLUDES=-I./ -I$(ROOTINC) -isystem$(ROOTINC) -I$(DECODE) -isystem$(DECODE) -I$(COMMON)

default: Analysis

Analysis: ./obj/Analysis.o $(COMMON)/obj/TrackSelection.o $(COMMON)/obj/Utilities.o libEvent
	$(CXX) $(CFLAGS) $< $(ADDLDFLAGS) $(ROOTDYNAMICLIBS) $(COMMON)/obj/TrackSelection.o $(COMMON)/obj/Utilities.o $(DECODELIBS) -o$@

./obj/%.o: ./src/%.C
	@if ! [ -d $(OBJ) ] ; then mkdir -p $(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

./obj/%.o: ./src/%.cxx
	@if ! [ -d $(OBJ) ] ; then mkdir -p $(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(COMMON)/obj/%.o: $(COMMON)/%.C
	@if ! [ -d $(COMMON)/$(OBJ) ] ; then mkdir -p $(COMMON)/$(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(COMMON)/obj/%.o: $(COMMON)/%.cxx
	@if ! [ -d $(COMMON)/$(OBJ) ] ; then mkdir -p $(COMMON)/$(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

libEvent:
	make -C $(DECODE)

clean:
	rm -Rf ./obj
	rm -f ./Analysis

cleanALL: clean
	make -C $(DECODE) clean
	make -C $(COMMON) clean
