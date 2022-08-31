CXX := `root-config --cxx`
MARCH := `root-config --arch`
LD:=$(CXX)

OBJ=./obj/
SRC=./src/
INC=./include/

#CFLAGS:= -g -Wno-deprecated
CFLAGS:= -O3 -Wno-deprecated

VERSION      := $(shell root-config --version | cut -b1-4)
ifeq ($(VERSION),5.27)
VERSIONP=
else
VERSIONP = $(VERSION)
endif

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

INCLUDES=-I./ -I$(ROOTINC) -isystem$(ROOTINC) -I$(DECODE)/include -isystem$(DECODE)/include -I$(COMMON)

default: linkAlignAMS linkAlignOCA linkAlignFOOT

Align: ./obj/Align.o libEvent
	$(CXX) $(CFLAGS) $< $(ADDLDFLAGS) $(ROOTDYNAMICLIBS) $(DECODELIBS) -o$@

AMS: AlignAMS

OCA: AlignOCA

FOOT: AlignFOOT

AlignAMS: Align
	@ln -sfv Align AlignAMS

AlignOCA: Align
	@ln -sfv Align AlignOCA

AlignFOOT: Align
	@ln -sfv Align AlignFOOT

linkAlign%: %
#	echo $(<)
	@ln -sfv Align$(<) NoFirstStepAlign$(<)
	@ln -sfv Align$(<) FirstPassAlign$(<)
	@ln -sfv Align$(<) NoLastStepAlign$(<)
	@ln -sfv Align$(<) DryRunResidual$(<)
	@ln -sfv Align$(<) DryRunBruna$(<)
	@ln -sfv Align$(<) OnlyFinalRun$(<)

./obj/%.o: ./src/%.C
	@if ! [ -d $(OBJ) ]; then mkdir -p $(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

./obj/%.o: ./src/%.cxx
	@if ! [ -d $(OBJ) ]; then mkdir -p $(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

libEvent:
	make -C $(DECODE)

clean: 
	rm -Rf ./obj
	rm -f ./Align*
	rm -f ./NoFirst*
	rm -f ./FirstPass*
	rm -f ./NoLast*
	rm -f ./DryRun*
	rm -f ./OnlyFinal*

cleanALL: clean
	make -C $(DECODE)/ clean
