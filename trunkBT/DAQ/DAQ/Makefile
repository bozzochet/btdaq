CXX = $(shell root-config --cxx)
CC  = $(shell root-config --cc)
F77 = $(shell root-config --f77)

TOP= ./

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --libs)

LIBSOURCES= $(TOP)PeCpIp/amswcom.cxx $(TOP)SlowControl/QList.cxx $(TOP)amsw/amsw_lib.cxx
LIBINCLUDE= -I$(ROOTSYS)/include -I $(TOP)PeCpIp/ -I  $(TOP)SlowControl/ -I $(TOP)amsw/

LIBOBJ:=$(LIBSOURCES:%.cxx=%.o)


CFLAGS= -g -Wno-int-to-pointer-cast
CPPFLAGS+= $(LIBINCLUDE) 

OBJECTS=$(TOP)/TakeData/takeJinj.o $(TOP)/TakeData/JinjSlave.o $(TOP)/TakeData/dampe.o $(TOP)/TakeData/JLV1.o $(TOP)/TakeData/CommClass.o $(TOP)/TakeData/TrigClass.o $(TOP)/TakeData/Jinj.o $(TOP)/TakeData/Jinf.o $(TOP)/TakeData/PUtil.o

all: lib TESTpci TakeJPCI SUMMARY

pippo:
	echo $(LIBOBJ)

lib: $(TOP)lib/libamswire.a

$(TOP)lib/libamswire.a: $(TOP)lib/libamswire.a($(LIBOBJ))

TESTpci: $(TOP)/Tests/test.o $(TOP)lib/libamswire.a
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $< -L$(TOP)lib/ -lamswire
	@echo "-------------- for using through EPP (it depends on the program's name) ---------------"
	rm -f ./TESTepp
	ln -s ./$@ ./TESTepp
	@echo "---------------------------------------------------------------------------------------"
	@echo

TakeJPCI: $(OBJECTS)  $(TOP)lib/libamswire.a
	@echo Linking $@ ...
	$(CXX) $(CFLAGS)  $(CPPFLAGS) -o $@ $(OBJECTS) -L$(TOP)lib/ -lamswire $(ROOTLIBS)
	@echo "------ for using through EPP and without Jinf (it depends on the program's name) ------"
	rm -f ./TakeJEPP
	ln -s ./$@ ./TakeJEPP
	rm -f ./TakePCI
	ln -s ./$@ ./TakePCI
	rm -f ./TakeEPP
	ln -s ./$@ ./TakeEPP
	@echo "---------------------------------------------------------------------------------------"
	@echo

#SUMMARY: $(TOP)/TakeData/summary.o  $(TOP)lib/libamswire.a
SUMMARY: $(TOP)/TakeData/summary.o
	@echo Linking $@ ...
#	$(CXX) $(CFLAGS) $(ROOTCFLAGS) $(CPPFLAGS) -o $@ $(TOP)/TakeData/summary.o -L$(TOP)lib/ -lamswire $(ROOTLIBS)
	$(CXX) $(CFLAGS) $(ROOTCFLAGS) $(CPPFLAGS) -o $@ $(TOP)/TakeData/summary.o $(ROOTLIBS)
	ln -fs SUMMARY SUMMARY_MUTE

%.o : %.cxx
	@echo Compiling $< ...
	$(CXX)  $< -c  -o $@ $(CFLAGS) $(CPPFLAGS) $(ROOTCFLAGS)

clean:
	rm -fv */*.o test lib/lib*.a ./TESTpci ./TESTepp ./TakeJPCI ./TakeJEPP ./TakePCI ./TakeEPP ./SUMMARY ./SUMMARY_MUTE
