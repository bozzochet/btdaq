CXX := `root-config --cxx`
ROOTCLING=rootcling
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
ROOTVIEWERLIBS := $(shell root-config --cflags --glibs)
ROOTINC := $(shell root-config --incdir)

DECODE = ../Decode/
DECODELIBS = -Wl,-rpath,$(DECODE) -L$(DECODE) -lEvent

COMMON = ../CommonTool/

INCLUDES=-I./ -I$(ROOTINC) -isystem$(ROOTINC) -I$(DECODE)/include -isystem$(DECODE)/include -I$(COMMON)

.PHONY: ViewerGUI

all: Analysis create_tree

Viewer: ./obj/viewer.o
	$(CXX) $(CFLAGS) $< $(ADDLDFLAGS) $(ROOTDYNAMICLIBS) -o$@

ViewerGUI:
	$(ROOTCLING) -f $(SRC)/guiDict.cxx $(DECODE)/Event.cxx $(DECODE)/Cluster.cxx $(DECODE)/LadderConf.cxx -c $(SRC)/viewerGUI.hh $(SRC)/guiLinkDef.h
	$(CXX) $(SRC)/viewerGUI.cxx $(SRC)/guiDict.cxx $(INCLUDES) $(ROOTVIEWERLIBS) -o$@

ViewerFOOT: ./obj/viewerFOOT.o libEvent
	$(CXX) $(CFLAGS) $< $(ADDLDFLAGS) $(ROOTDYNAMICLIBS) $(DECODELIBS) -o$@

Analysis: ./obj/Analysis.o libEvent
	$(CXX) $(CFLAGS) $< $(ADDLDFLAGS) $(ROOTDYNAMICLIBS) $(DECODELIBS) -o$@
	ln -fs $@ AnalysisFOOT
	ln -fs $@ AnalysisOCA
	ln -fs $@ AnalysisAMSL0

create_tree: ./obj/create_tree.o libEvent
	$(CXX) $(CFLAGS) $< $(ADDLDFLAGS) $(ROOTDYNAMICLIBS) $(DECODELIBS) -o$@

#AnalysisFOOT: ./obj/AnalysisFOOT.o libEvent
#	$(CXX) $(CFLAGS) $< $(ADDLDFLAGS) $(ROOTDYNAMICLIBS) $(DECODELIBS) -o$@

./obj/%.o: ./src/%.C
	if ! [ -d $(OBJ) ] ; then mkdir -p $(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

./obj/%.o: ./src/%.cxx
	if ! [ -d $(OBJ) ] ; then mkdir -p $(OBJ); fi
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

libEvent:
	make -C $(DECODE)

clean:
	rm -Rf ./obj
	rm -fv ./Analysis
	rm -fv ./AnalysisFOOT
	rm -fv ./AnalysisOCA
	rm -f ./ViewerGUI
	rm -f ./ViewerFOOT
	rm -f ./guiDict.cxx

cleanALL: clean
	make -C $(DECODE) clean

all: Analysis
