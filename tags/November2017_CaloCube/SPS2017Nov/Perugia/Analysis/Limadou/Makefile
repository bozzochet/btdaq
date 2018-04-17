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

CFLAGS += $(shell root-config --auxcflags) -fPIC

ROOTDYNAMICLIBS := $(shell root-config --glibs) -lMinuit -lSpectrum
ROOTINC := $(shell root-config --incdir)

DECODE = ../../Decode/
DECODELIBS = -Wl,-rpath,$(DECODE) -L$(DECODE) -lEvent

COMMON = ../../CommonTool/

INCLUDES=-I./ -I$(ROOTINC) -isystem$(ROOTINC) -I$(DECODE) -isystem$(DECODE) -I$(COMMON)

default: SigPos

SigPos: ./obj/SigPos.o $(COMMON)/obj/TrackSelection.o $(COMMON)/obj/Utilities.o libEvent
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
	rm -f ./obj/*.o
	rm -f ./SigPos

cleanALL: clean
	make -C ../Decode clean
	rm -f $(COMMON)/$(OBJ)/*.o
