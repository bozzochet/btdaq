### Directories ###
INC      = include
SRC      = CC
DEP      = dep
OBJ      = obj
LIB      = ../lib
BIN      = ../bin

CPPFLAGS = -I.  -I../TrackerTools/include -I../TrackerLib/include -I../AMSbase/include -I../AMSGeom/include -I../FORTInterface/include
include ../Make.common

DEC_SOURCES= main_calib3.C DeBlock.C  POutF.C SaveCalToDb.C
DEC_OBJ=$(DEC_SOURCES:%.C=$(OBJ)/%.o)

DEC2_SOURCES= main_data.C DeBlock.C POutF.C  myholder.C 
DEC2_OBJ=$(DEC2_SOURCES:%.C=$(OBJ)/%.o)

DEC3_SOURCES= main_slow.C DeBlock.C POutF.C
DEC3_OBJ=$(DEC3_SOURCES:%.C=$(OBJ)/%.o)

DEC4_SOURCES= main_data_std.C DeBlock.C POutF.C
DEC4_OBJ=$(DEC4_SOURCES:%.C=$(OBJ)/%.o)

DEF2_SOURCES = main_framesdata.C Deframe.C
DEF2_OBJ=$(DEF2_SOURCES:%.C=$(OBJ)/%.o)


DEF3_SOURCES = ScanDST.C
DEF3_OBJ=$(DEF3_SOURCES:%.C=$(OBJ)/%.o)


CERNLIB = `cernlib geant grafX11 mathlib`

SCC += $(DEC_SOURCES)
SCC += $(DEC2_SOURCES)
SCC += $(DEC3_SOURCES)
SCC += $(DEC4_SOURCES)
SCC += $(DEF_SOURCES)

SCC += $(DEF2_SOURCES)
SCC += $(DEF3_SOURCES)

DEPS:=$(SCC:%.C=$(OBJ)%.d) 


DEFINES += -I../TrackerLib/include  -I../TrackerTools/include  -I ../AMSbase/include/ -I../prova2/src -I../FORTinterface/include


all: $(BIN)/ScanDST $(BIN)/DecodeStd  $(BIN)/Decode  $(BIN)/CalDecode $(BIN)/Deframe $(BIN)/SlowDecode

#all:   $(BIN)/Decode $(BIN)/CalDecode $(BIN)/SlowDecode
#all:   $(BIN)/Decode 
Decodea: $(BIN)/Decodea
$(LIB)/libTrackerTools.a:
	$(MAKE) -C ../TrackerTools

$(LIB)/libTracker.a:
	$(MAKE) -C ../TrackerLib

$(LIB)/libAMSbase.a:
	$(MAKE) -C ../AMSbase

$(BIN)/DecodeStd: $(DEC4_OBJ) $(LIB)/libTracker.a $(LIB)/libAMSbase.a 
	@ if ! [ -d $(BIN) ] ; then  mkdir -p $(BIN); fi
	$(CXX) $(CXXFLAGS)  -o $@ $^    $(ROOTLIBS) $(CERNLIB) $(FLIB) -L/usr/X11R6/lib/ -lX11


$(BIN)/Decodea: $(DEC2_OBJ) $(LIB)/libTracker.a $(LIB)/libTrackerTools.a $(LIB)/libAMSbase.a 
	@ if ! [ -d $(BIN) ] ; then  mkdir -p $(BIN); fi
	$(CXX) $(CXXFLAGS) -static  -o $@ $^ -L$(ROOTSYS)/lib -lRoot -ldl -lpcre 


$(BIN)/Decode: $(DEC2_OBJ) $(LIB)/libTracker.a $(LIB)/libTrackerTools.a $(LIB)/libAMSbase.a 
	@ if ! [ -d $(BIN) ] ; then  mkdir -p $(BIN); fi
	$(CXX) $(CXXFLAGS)  -o $@ $^    $(ROOTLIBS) $(CERNLIB) $(FLIB) -L/usr/X11R6/lib/ -lX11

$(BIN)/SlowDecode: $(DEC3_OBJ) $(LIB)/libTracker.a $(LIB)/libTrackerTools.a $(LIB)/libAMSbase.a
	@ if ! [ -d $(BIN) ] ; then  mkdir -p $(BIN); fi
	$(CXX) $(CXXFLAGS)  -o $@ $^   $(ROOTLIBS) `cernlib` $(FLIB)


$(BIN)/CalDecode: $(DEC_OBJ) $(LIB)/libTracker.a $(LIB)/libAMSbase.a  
	@ if ! [ -d $(BIN) ] ; then  mkdir -p $(BIN); fi
	$(CXX) $(CXXFLAGS)  -o $@ $^    $(ROOTLIBS) $(CERNLIB) $(FLIB) 


$(BIN)/Deframe: $(DEF2_OBJ) 
	@ if ! [ -d $(BIN) ] ; then  mkdir -p $(BIN); fi
	$(CXX) $(CXXFLAGS) -o $@ $^  


$(BIN)/ScanDST: $(DEF3_OBJ) $(LIB)/libTracker.a $(LIB)/libTrackerTools.a $(LIB)/libAMSbase.a
	@ if ! [ -d $(BIN) ] ; then  mkdir -p $(BIN); fi
	$(CXX) $(CXXFLAGS)  -o $@ $^    $(ROOTLIBS) $(CERNLIB) $(FLIB)






clean:
	@rm -fv $(BIN)/Deframe $(BIN)/Decode $(BIN)/CalDecode *.so *.d  *.a  obj/*.o 
	@rm -frv obj

### Include dependency files ###
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),status)
ifneq ($(MAKECMDGOALS),doxygen)
-include $(DEPS)
endif
endif
endif

