CXX= g++ 
CC= gcc 
F77= g77 
TOP= ./

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)

LIBSOURCES= $(TOP)PeCpIp/amswcom.cxx $(TOP)SlowControl/QList.cxx $(TOP)amsw/amsw_lib.cxx
LIBINCLUDE= -I $(TOP)PeCpIp/ -I  $(TOP)SlowControl/ -I $(TOP)amsw/

LIBOBJ:=$(LIBSOURCES:%.cxx=%.o)

CFLAGS= -g
CPPFLAGS+= $(LIBINCLUDE) 

OBJECTS=$(TOP)/TakeData/take.o $(TOP)/TakeData/CommClass.o $(TOP)/TakeData/TrigClass.o $(TOP)/TakeData/Jinf.cxx $(TOP)/TakeData/PUtil.cxx

OBJECTS2=$(TOP)/TakeData/takeJinj.o $(TOP)/TakeData/CommClass.o $(TOP)/TakeData/TrigClass.o $(TOP)/TakeData/Jinj.cxx $(TOP)/TakeData/Jinf.cxx $(TOP)/TakeData/PUtil.cxx
all: lib PCIcomm TakePCI SUMMARY

all: TakePCI lib TakeJPCI

pippo:
	echo $(LIBOBJ)

lib: $(TOP)lib/libamswire.a

$(TOP)lib/libamswire.a: $(TOP)lib/libamswire.a($(LIBOBJ))

PCIcomm: $(TOP)/Tests/test.o $(TOP)lib/libamswire.a
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $< -L$(TOP)lib/ -lamswire 
TakePCI: $(OBJECTS)  $(TOP)lib/libamswire.a
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(ROOTCFLAGS) $(CPPFLAGS) -o $@ $(OBJECTS) -L$(TOP)lib/ -lamswire $(ROOTLIBS)

TakeJPCI: $(OBJECTS2)  $(TOP)lib/libamswire.a
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(ROOTCFLAGS) $(CPPFLAGS) -o $@ $(OBJECTS2) -L$(TOP)lib/ -lamswire $(ROOTLIBS)
	cp -f TakeJPCI ../pyremote-18/

SUMMARY: $(TOP)/TakeData/summary.o  $(TOP)lib/libamswire.a
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(ROOTCFLAGS) $(CPPFLAGS) -o $@ $(TOP)/TakeData/summary.o -L$(TOP)lib/ -lamswire $(ROOTLIBS)

%.o : %.cxx
	@echo Compiling $< ...
	$(CXX)  $< -c  -o $@ $(CFLAGS) $(ROOTCFLAGS) $(CPPFLAGS)

clean:
	rm -fv */*.o test lib/lib*.a
