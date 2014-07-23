
CLASSES=   Cluster.hh Event.hh 

DEC_SOURCES=  DecodeData.cxx    main.cxx  rootElibdict.cxx 
DEC_SOURCES+= $(CLASSES:%.hh=%.cxx)
DEC_OBJ=$(DEC_SOURCES:%.cxx=obj/%.o)

LIB_SOURCES =    rootElibdict.cxx
LIB_SOURCES += $(CLASSES:%.hh=%.cxx)
LIB_OBJ=$(LIB_SOURCES:%.cxx=obj/%.o)


CFLAGS= -g -fPIC  
DEFINES=

all: Decode lib

lib:  libEvent.so

libEvent.so:$(LIB_OBJ)
	$(CXX) --shared $(CFLAGS) $(DEFINES)  -o$@ $^ 

libEvent.a:$(LIB_OBJ)
	$(CXX) $(CFLAGS) $(DEFINES)  -o$@ $^ 


Decode: $(DEC_OBJ)
	$(CXX) $(CFLAGS)  -o$@ $^  `$(ROOTSYS)/bin/root-config --libs`



obj/%.o:%.cxx 
	@echo Compiling $< ...; 
	@ if ! [ -d ./obj ] ; then  mkdir -p obj; fi
	@$(CXX) $(CFLAGS) $(DEFINES) -c -Wall $< -o$@  -I$(ROOTSYS)/include

rootElibdict.cxx: $(CLASSES) LinkDef.h
	@echo Creating ROOT dictionary
	@$(ROOTSYS)/bin/rootcint -f $@  -c $^


clean:
	@rm -fv libEvent.a libEvent.so Decode CalDecode  rootElibdict.cxx rootElibdict.h obj/*.o 
	@rm -frv obj
