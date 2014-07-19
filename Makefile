CXX= g++ 
CC= gcc 
F77= g77 
TOP= ./

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)

LIBSOURCES= $(TOP)PeCpIp/amswcom.cxx $(TOP)SlowControl/QList.cxx $(TOP)amsw/amsw_lib.cxx
LIBINCLUDE= -I$(ROOTSYS)/include -I $(TOP)PeCpIp/ -I  $(TOP)SlowControl/ -I $(TOP)amsw/

LIBOBJ:=$(LIBSOURCES:%.cxx=%.o)


CFLAGS= -g
CPPFLAGS+= $(LIBINCLUDE) 

OBJECTS=$(TOP)/TakeData/takeJinj.o $(TOP)/TakeData/JinjSlave.o $(TOP)/TakeData/JLV1.o $(TOP)/TakeData/TrigClass.o $(TOP)/TakeData/Jinj.o $(TOP)/TakeData/Jinf.o $(TOP)/TakeData/PUtil.o

all: lib TESTpci TakeJPCI

pippo:
	echo $(LIBOBJ)

lib: $(TOP)lib/libamswire.a

$(TOP)lib/libamswire.a: $(TOP)lib/libamswire.a($(LIBOBJ))

TESTpci: $(TOP)/Tests/test.o $(TOP)lib/libamswire.a
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $< -L$(TOP)lib/ -lamswire
	cp ./$@ ./TESTepp

TakeJPCI: $(OBJECTS)  $(TOP)lib/libamswire.a
	@echo Linking $@ ...
	$(CXX) $(CFLAGS)  $(CPPFLAGS) -o $@ $(OBJECTS) -L$(TOP)lib/ -lamswire `root-config --libs`
	cp ./$@ ./TakeJEPP

%.o : %.cxx
	@echo Compiling $< ...
	$(CXX)  $< -c  -o $@ $(CFLAGS)  $(CPPFLAGS)

clean:
	rm -fv */*.o test lib/lib*.a ./TESTpci ./TESTepp ./TakeJPCI ./TakeJEPP
