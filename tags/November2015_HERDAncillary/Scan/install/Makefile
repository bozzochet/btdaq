INCLUDE = ../include/
SOURCE = ../source/
EXE = ../exe/
OBJ = ../obj/
# SROOT = /amssw/root_v5.14.00f
# SROOT = /amssw/root_v5.15.08
SROOT = $(ROOTSYS)

LD_LIBRARY_PATH = $(ROOTSYS)/lib
DYLD_LIBRARY_PATH = $(ROOTSYS)/lib

CLASSES=  DHF.hh RHClass.hh Event.hh Cluster.hh Align.hh Trace.hh Calib.hh
DICTIONARY= $(INCLUDE)DHF.hh  $(INCLUDE)Cluster.hh $(INCLUDE)Event.hh $(INCLUDE)Align.hh $(INCLUDE)Trace.hh $(INCLUDE)Calib.hh $(INCLUDE)LinkDef.h
DEC_SOURCES=   main.cxx  DecodeData.cxx  rootElibdict.cxx
DEC_SOURCES+=  $(CLASSES:%.hh=%.cxx)
DEC_OBJ=$(DEC_SOURCES:%.cxx=$(OBJ)%.o)

SCA_SOURCES=    mainScan.cxx  rootElibdict.cxx 
SCA_SOURCES+= $(CLASSES:%.hh=%.cxx)
SCA_OBJ=$(SCA_SOURCES:%.cxx=$(OBJ)%.o)

LIB_SOURCES =  rootElibdict.cxx
LIB_OBJ=$(LIB_SOURCES:%.cxx=$(OBJ)%.o)
LIB_SOURCES += $(CLASSES:%.hh=%.cxx)
LIB_SOURCES += $(CLASSES:%.h=%.cxx)

RLIBS = -L$(LD_LIBRARY_PATH) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -pthread -lm -ldl -rdynamic
RDLIBS = -L$(ROOTSYS)/lib -lCore.so -lCint.so -lHist.so -lGraf.so -lGraf3d.so -lGpad.so -lTree.so -lRint.so -lPostscript.so -lMatrix.so -lPhysics.so -pthread -lm -ldl -rdynamic

CFLAGS= -g -v -mtune=pentium4 
DEFINES= -I$(SROOT)/include 

type:
	@echo $(SCA_OBJ); 
	@echo $(SCA_SOURCES); 

all: $(EXE)Scan $(EXE)Decode lib

LinearF: $(SOURCE)fitLinear2.cxx
	@$(CXX) $(CFLAGS) -Wall -o$@ $<  -I$(SROOT)/include -I$(INCLUDE) `$(SROOT)/bin/root-config --libs` -L$(ROOTSYS)/lib -lMinuit

lib:  ../libEvent.so

../libEvent.so:$(LIB_OBJ)
	$(CXX) --shared $(CFLAGS) $(DEFINES)  -o$@ $^ 

libEvent.a:$(LIB_OBJ)
	$(CXX) $(CFLAGS) $(DEFINES)  -o$@ $^ 

$(EXE)Decode: $(DEC_OBJ)
	@echo linking ...  $(DEC_SOURCES) into $@
	$(CXX) $(CFLAGS) $(DEFINES) -I$(INCLUDE) -o$(EXE)$@ $^  `$(SROOT)/bin/root-config --libs`
	cp -f $@ ./$@FAST

$(EXE)Scan: $(SCA_OBJ)
	@$(CXX) $(CFLAGS)   -o$(EXE)$@ $^ `$(SROOT)/bin/root-config --libs` -L$(ROOTSYS)/lib -lMinuit

$(OBJ)%.o:$(SOURCE)%.cxx
	@echo Compiling $< ...; 
	@$(CXX) -v $(CFLAGS) $(DEFINES) -c -Wall $< -o$@ -I$(SROOT)/include -I$(INCLUDE)  

$(SOURCE)rootElibdict.cxx: $(DICTIONARY)
	@echo Creating ROOT dictionary
	@$(SROOT)/bin/rootcint -f $@  -c $^

clean:
	@rm -fv libEvent.a libEvent.so Decode CalDecode  $(SOURCE)rootElibdict.cxx $(SOURCE)rootElibdict.h $(OBJ)*.o
