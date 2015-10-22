CXX := `$(ROOTSYS)/bin/root-config --cxx`
MARCH := `$(ROOTSYS)/bin/root-config --arch`
LD:=$(CXX)

OBJ=./obj/
SRC=./src/
INC=./include/

#CFLAGS:= -g -Wno-deprecated
CFLAGS:= -O3 -Wno-deprecated

VERSION      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)
ifeq ($(VERSION),5.27)
VERSIONP=
else
VERSIONP = $(VERSION)
endif

CFLAGS += $(shell $(ROOTSYS)/bin/root-config --auxcflags) -fPIC

ROOTDYNAMICLIBS := $(shell root-config --glibs)
ROOTDYNAMICLIBS += -lMinuit

EXTRALIBS= -ldl -lssl -lTreePlayer -lProof -lProofPlayer -lNetx -lgfortran
EXTRALIBS += -lTMVA -lXMLIO -lMLP

DECODE = ../Decode/
DECODELIBS = -Wl,-rpath,$(DECODE) -L$(DECODE) -lEvent

INCLUDES=-I./ -I$(ROOTSYS)/include -isystem$(ROOTSYS)/include -I$(DECODE) -isystem$(DECODE)

default: MergeWithOthers

MergeWithOthers: ./obj/MergeWithOthers.o libEvent
	$(CXX) $(CFLAGS) ./obj/MergeWithOthers.o $(ROOTDYNAMICLIBS) $(EXTRALIBS) $(DECODELIBS) -o ./MergeWithOthers

./obj/%.o: ./src/%.C
	@if ! [ -d $(OBJ) ] ; then mkdir -p $(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

./obj/%.o: ./src/%.cxx
	@if ! [ -d $(OBJ) ] ; then mkdir -p $(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

libEvent:
	make -C $(DECODE)

clean: 
	rm -f ./obj/*.o
	rm -f ./MergeWithOthers
