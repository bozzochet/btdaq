CXX= g++ 
CC= gcc 
F77= g77 
TOP= ./

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)

AMSWIRESOURCES= $(TOP)SlowControl/QList.cxx $(TOP)amsw/amsw_lib.cxx

AMSWCOMSOURCES= $(TOP)PeCpIp/amswcom.cxx 

LIBINCLUDE= -I$(ROOTSYS)/include -I$(TOP)SlowControl/ -I$(TOP)amsw/ -I$(TOP)amswLeb/ -I$(TOP)PeCpIp/

AMSWIREOBJ:=$(AMSWIRESOURCES:%.cxx=%.o)
AMSWCOMOBJ:=$(AMSWCOMSOURCES:%.cxx=%.o)

CFLAGS= -g 
CPPFLAGS+= $(LIBINCLUDE) 

OBJECTS=$(TOP)/TakeData/takeJinj.o $(TOP)/TakeData/JinjSlave.o $(TOP)/TakeData/JLV1.o $(TOP)/TakeData/CommClass.o $(TOP)/TakeData/TrigClass.o $(TOP)/TakeData/Jinj.o $(TOP)/TakeData/Jinf.o $(TOP)/TakeData/PUtil.o

all: lib TESTpci TakePCI SUMMARY


lib: $(TOP)lib/libamswire.a $(TOP)lib/libAMSslow.a $(TOP)lib/libamswcom.a


$(TOP)lib/libamswire.a: $(TOP)lib/libamswire.a($(AMSWIREOBJ))

$(TOP)lib/libAMSslow.a:
	$(MAKE) -C $(TOP)amswLeb

$(TOP)lib/libamswcom.a: $(TOP)lib/libamswcom.a($(AMSWCOMOBJ))


TESTpci: $(TOP)/Tests/test.o $(TOP)/SlowControl/QList.o lib
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $< -L$(TOP)lib/ -lamswcom -lAMSslow -lamswire
	@echo "----------- for using through EPP and JMDC (it depends on the program's name) ---------------"
	rm -f ./TESTepp
	ln -s ./$@ ./TESTepp
	rm -f ./TESTjmdc
	ln -s ./$@ ./TESTjmdc
	@echo "---------------------------------------------------------------------------------------------"
	@echo

TakePCI: $(OBJECTS) lib
	@echo Linking $@ ...
	$(CXX) $(CFLAGS)  $(CPPFLAGS) -o $@ $(OBJECTS) -L$(TOP)lib/ -lamswcom -lAMSslow -lamswire `root-config --libs`
	@echo "-- for using through EPP and JMDC and with a Jinj (it depends on the program's name) --"
	rm -f ./TakeEPP
	ln -s ./$@ ./TakeEPP
	rm -f ./TakeJMDC
	ln -s ./$@ ./TakeJMDC
	rm -f ./TakeJinjEPP
	ln -s ./$@ ./TakeJinjEPP
	rm -f ./TakeJinjPCI
	ln -s ./$@ ./TakeJinjPCI
	rm -f ./TakeJinjJMDC
	ln -s ./$@ ./TakeJinjJMDC
	@echo "---------------------------------------------------------------------------------------------"
	@echo

SUMMARY: $(TOP)/TakeData/summary.o
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(ROOTCFLAGS) $(CPPFLAGS) -o $@ $(TOP)/TakeData/summary.o $(ROOTLIBS)

%.o : %.cxx
	@echo Compiling $< ...
	$(CXX)  $< -c  -o $@ $(CFLAGS)  $(CPPFLAGS)

clean:
	rm -fv */*.o test lib/lib*.a ./TESTpci ./TESTepp ./TESTjmdc ./TakeJinjPCI ./TakeJinjEPP ./TakeJinjJMDC ./TakePCI ./TakeEPP  ./TakeJMDC ./SUMMARY
