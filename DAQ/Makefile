CXX= g++ 
CC= gcc 
F77= g77 
TOP= ./

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)

AMSWIRESOURCES= $(TOP)SlowControl/QList.cxx $(TOP)amsw/amsw_lib.cxx

AMSWCOMSOURCES= $(TOP)PeCpIp/amswcom.cxx 

LIBINCLUDE= -I$(ROOTSYS)/include -I$(TOP)SlowControl/ -I$(TOP)amsw/ -I$(TOP)amswLeb/ -I$(TOP)PeCpIp/ -I$(TOP)TrackerLadders/ -I$(TOP)Control/ -I/usr/include/freetype2

AMSWIREOBJ:=$(AMSWIRESOURCES:%.cxx=%.o)
AMSWCOMOBJ:=$(AMSWCOMSOURCES:%.cxx=%.o)

CFLAGS= -g 
CPPFLAGS+= $(LIBINCLUDE) 

OBJECTS=$(TOP)/TakeData/takeJinj.o $(TOP)/TakeData/JinjSlave.o $(TOP)/TakeData/JLV1.o $(TOP)/TakeData/CommClass.o $(TOP)/TakeData/TrigClass.o $(TOP)/TakeData/Jinj.o $(TOP)/TakeData/Jinf.o $(TOP)/TakeData/PUtil.o

#TKCLASTSTO = $(TOP)/TrackerLadders/TrackerLadders.o $(TOP)/TrackerLadders/testclass.o

SLOWGUIO =  $(TOP)/slowgui/slowguiDict.o $(TOP)/SlowControl/SlowControl.o $(TOP)/SlowControl/QList.o $(TOP)/TrackerLadders/TrackerLadders.o $(TOP)/slowgui/HELPgui.o $(TOP)/slowgui/S9011ATctl.o $(TOP)/slowgui/S9011ATgui.o $(TOP)/slowgui/TPSFEctl.o $(TOP)/slowgui/TPSFEgui.o $(TOP)/slowgui/TBSctl.o $(TOP)/slowgui/TBSgui.o $(TOP)/slowgui/Settings.o $(TOP)/slowgui/PLOTgui.o $(TOP)/slowgui/slowgui.o $(TOP)/slowgui/TrackerMonitorGui.o  $(TOP)/slowgui/main.o

SLOWGUI2O =  $(TOP)/slowgui2/slowgui2Dict.o $(TOP)/SlowControl/SlowControl.o $(TOP)/SlowControl/QList.o $(TOP)/TrackerLadders/TrackerLadders.o $(TOP)/slowgui2/HELPgui2.o $(TOP)/slowgui2/S9011ATctl2.o $(TOP)/slowgui2/S9011ATgui2.o $(TOP)/slowgui2/TPSFEctl2.o $(TOP)/slowgui2/TPSFEgui2.o $(TOP)/slowgui2/TBSctl2.o $(TOP)/slowgui2/TBSgui2.o $(TOP)/slowgui2/Settings2.o $(TOP)/slowgui2/PLOTgui2.o $(TOP)/slowgui2/slowgui2.o $(TOP)/slowgui2/TrackerMonitorGui2.o  $(TOP)/slowgui2/main2.o

CONTROLO = $(TOP)/Control/Queue.o $(TOP)/Control/Share.o $(TOP)/TrackerLadders/TrackerLadders.o $(TOP)/Control/Control.o 

LOGO = $(TOP)/Control/Queue.o $(TOP)/Control/Share.o $(TOP)/Log/Log.o 

GUIO  = $(TOP)/Control/Queue.o $(TOP)/Control/Share.o $(TOP)/Gui/GUITimer.o \
	$(TOP)/Gui/NewLadder.o $(TOP)/Gui/SetCalPar.o $(TOP)/Gui/SetShapingTime.o \
        $(TOP)/Gui/SetLeakTemp.o $(TOP)/Gui/SetGainPar.o $(TOP)/Gui/SetPrinter.o \
	$(TOP)/Gui/SelectLadder.o $(TOP)/Gui/ShowFile.o \
        $(TOP)/Gui/DefineFonts.o $(TOP)/Gui/AMSWireWindow.o\
        $(TOP)/Gui/MenuBar.o $(TOP)/Gui/Title.o $(TOP)/Gui/Work.o \
        $(TOP)/Gui/MainFrame.o $(TOP)/Gui/Actions.o $(TOP)/Gui/FileDialog.o \
	$(TOP)/Gui/ProcessGUI.o $(TOP)/Gui/ProcessMessage.o $(TOP)/Gui/GUI.o  

RUNO =  $(TOP)/Control/Queue.o $(TOP)/Control/Share.o $(TOP)/Control/Time.o $(TOP)/Run/Mclib.o $(TOP)/Run/Run.o 

DSPO = $(TOP)/Control/Queue.o $(TOP)/Control/Share.o  $(TOP)/Dsp/Ladder.o $(TOP)/Dsp/Cluster.o $(TOP)/Dsp/DSP.o 


all:	lib TESTpci TakePCI SUMMARY SLOWGUIDICT SLOWGUI SLOWGUI2DICT SLOWGUI2 CONTROL LOG GUI RUN DSP


lib: $(TOP)lib/libamswire.a $(TOP)lib/libAMSslow.a $(TOP)lib/libamswcom.a


$(TOP)lib/libamswire.a: $(TOP)lib/libamswire.a($(AMSWIREOBJ))

$(TOP)lib/libAMSslow.a:
	$(MAKE) -C $(TOP)amswLeb

$(TOP)lib/libamswcom.a: $(TOP)lib/libamswcom.a($(AMSWCOMOBJ))


TESTpci: $(TOP)/Tests/test.o $(TOP)/SlowControl/QList.o lib
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $< -L$(TOP)lib/ -lamswcom -lAMSslow -lamswire
	@echo "----------- for using through EPP and JMDC (it depends on the program's name) ---------------"
	chmod +s ./$@
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


SLOWGUIDICT: 
	$(MAKE) -C slowgui

SLOWGUI: $(SLOWGUIO)
	@echo Linking $@ ...
	$(CXX) $(CFLAGS)  $(CPPFLAGS) -o $@ $(SLOWGUIO) -L$(TOP)lib/ -lamswcom -lAMSslow -lamswire `root-config --glibs`
	chmod +s ./$@
	@echo "---------------------------------------------------------------------------------------------"
	@echo

SLOWGUI2DICT: 
	$(MAKE) -C slowgui2

SLOWGUI2: $(SLOWGUI2O)
	@echo Linking $@ ...
	$(CXX) $(CFLAGS)  $(CPPFLAGS) -o $@ $(SLOWGUI2O) -L$(TOP)lib/ -lamswcom -lAMSslow -lamswire `root-config --glibs`
	chmod +s ./$@
	@echo "---------------------------------------------------------------------------------------------"
	@echo


CONTROL: $(CONTROLO)
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $(CONTROLO) -L$(TOP)lib/ -lamswcom -lAMSslow -lamswire `root-config --libs`
	@echo "---------------------------------------------------------------------------------------------"
	@echo

LOG: $(LOGO)
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $(LOGO) 
	@echo "---------------------------------------------------------------------------------------------"
	@echo

GUI: $(GUIO)
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $(GUIO) `root-config --glibs` 
	@echo "---------------------------------------------------------------------------------------------"
	@echo

RUN: $(RUNO)
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $(RUNO) -L$(TOP)lib/ -lamswcom -lAMSslow -lamswire
	chmod +s ./$@
	@echo "---------------------------------------------------------------------------------------------"
	@echo

DSP: $(DSPO)
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $(DSPO) -lm -lfftw3
	@echo "---------------------------------------------------------------------------------------------"
	@echo

SUMMARY: $(TOP)/TakeData/summary.o
	@echo Linking $@ ...
	$(CXX) $(CFLAGS) $(ROOTCFLAGS) $(CPPFLAGS) -o $@ $(TOP)/TakeData/summary.o $(ROOTLIBS)

%.o : %.cxx
	@echo Compiling $< ...
	$(CXX)  $< -c  -o $@ $(CFLAGS)  $(CPPFLAGS)

clean:
	rm -fv */*.o test lib/lib*.a ./TESTpci ./TESTepp ./TESTjmdc ./TakeJinjPCI ./TakeJinjEPP ./TakeJinjJMDC ./TakePCI ./TakeEPP  ./TakeJMDC ./SUMMARY ./Tests/test.o ./SlowControl/QList.o ./DSP ./RUN ./GUI ./LOG ./CONTROL ./SLOWGUI ./SLOWGUI2
