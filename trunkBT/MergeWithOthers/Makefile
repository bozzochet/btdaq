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

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
ADDLDFLAGS = -Wl,--no-as-needed
endif
ifeq ($(UNAME), Darwin)
ADDLDFLAGS =
endif

CFLAGS += $(shell $(ROOTSYS)/bin/root-config --auxcflags) -fPIC

ROOTDYNAMICLIBS := $(shell root-config --glibs)

DECODE = ../Decode/
DECODELIBS = -Wl,-rpath,$(DECODE) -L$(DECODE) -lEvent

INCLUDES=-I./ -I$(ROOTSYS)/include -isystem$(ROOTSYS)/include -I$(DECODE) -isystem$(DECODE)

default: MergeWithOthers

MergeWithOthers: ./obj/MergeWithOthers.o libEvent
#	$(CXX) $(CFLAGS) ./obj/MergeWithOthers.o $(ADDLDFLAGS) $(ROOTDYNAMICLIBS) $(DECODELIBS) -o ./MergeWithOthers
	$(CXX) $(CFLAGS) $< $(ADDLDFLAGS) $(ROOTDYNAMICLIBS) $(DECODELIBS) -o $@ 

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

cleanALL: clean
	make -C ../Decode clean
