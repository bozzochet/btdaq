# This Makefile is essentially useless. These two files are compiled by Align, Analysis, etc...
# You can use it to clean the dir tree from here

CXX := `root-config --cxx`
MARCH := `root-config --arch`
LD:=$(CXX)

OBJ=./obj/
SRC=./
INC=./

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

CFLAGS += $(shell root-config --auxcflags) -O -std=c++11 -fPIC

ROOTDYNAMICLIBS := $(shell root-config --libs) -lMinuit -lSpectrum
ROOTINC := $(shell root-config --incdir)

DECODE = ../Decode/
DECODELIBS = -Wl,-rpath,$(DECODE) -L$(DECODE) -lEvent

COMMON = ./

INCLUDES=-I./ -I$(ROOTINC) -isystem$(ROOTINC) -I$(DECODE) -isystem$(DECODE)

default: $(COMMON)/obj/TrackSelection.o $(COMMON)/obj/Utilities.o

$(COMMON)/obj/%.o: $(COMMON)/%.C
	@if ! [ -d $(COMMON)/$(OBJ) ] ; then mkdir -p $(COMMON)/$(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(COMMON)/obj/%.o: $(COMMON)/%.cxx
	@if ! [ -d $(COMMON)/$(OBJ) ] ; then mkdir -p $(COMMON)/$(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean: 
	rm -Rf ./obj
