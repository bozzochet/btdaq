BIN = $(shell pwd)/../bin

CXX = g++ -Wl,-rpath=$(BIN) 
ROOTFLAGS = $(shell root-config --libs --cflags --glibs)
AMS-CCVIEWER-DEP= AMS-CCviewer.cpp src/ViewerGUIDict.cpp $(BIN)/libCalo3DViewer.so $(BIN)/libCaloAMS2DViewer.so $(BIN)/libViewerGUI.so $(BIN)/libDataFileHandler.so $(BIN)/libViewerCore.so 
DEFFLAGS = -DSPS2017Nov

#AMS stuff
AMSDIR=$(PWD)/../../Perugia
DECODE =$(AMSDIR)/Decode/
COMMON =$(AMSDIR)/CommonTool/
DECODELIBS = -Wl,-rpath,$(DECODE) -L$(DECODE) -lEvent
INCLUDEGLOBAL = -I../inc -Iinclude -I$(DECODE) -I$(COMMON)

all: $(BIN)/AMS-CCviewer $(BIN)/PedMonitor 

$(BIN)/libCalo3DViewer.so: src/Calo3DViewer.cpp include/Calo3DViewer.h
	$(CXX) -fPIC -c src/Calo3DViewer.cpp -o $(BIN)/Calo3DViewer.o $(INCLUDEGLOBAL) $(DEFFLAGS) $(ROOTFLAGS) -g -Wall
	$(CXX) -shared -o $(BIN)/libCalo3DViewer.so $(BIN)/Calo3DViewer.o

$(BIN)/libDataFileHandler.so: src/DataFileHandler.cpp include/DataFileHandler.h
	$(CXX) -fPIC -c src/DataFileHandler.cpp -o $(BIN)/DataFileHandler.o $(INCLUDEGLOBAL) $(DEFFLAGS) $(ROOTFLAGS) -g -Wall
	$(CXX) -shared -o $(BIN)/libDataFileHandler.so $(BIN)/DataFileHandler.o

$(BIN)/libCaloAMS2DViewer.so: src/CaloAMS2DViewer.cpp include/CaloAMS2DViewer.h
	$(CXX) -fPIC -c src/CaloAMS2DViewer.cpp -o $(BIN)/CaloAMS2DViewer.o $(INCLUDEGLOBAL) $(DEFFLAGS) $(ROOTFLAGS) -g -Wall
	$(CXX) -shared -o $(BIN)/libCaloAMS2DViewer.so $(BIN)/CaloAMS2DViewer.o

$(BIN)/libViewerGUI.so: src/ViewerGUI.cpp include/ViewerGUI.h
	$(CXX) -fPIC -c src/ViewerGUI.cpp -o $(BIN)/ViewerGUI.o $(INCLUDEGLOBAL) $(DEFFLAGS) $(ROOTFLAGS) -g -Wall
	rootcint -f src/ViewerGUIDict.cpp -c $(shell pwd)/include/ViewerGUI.h $(shell pwd)/include/ViewerLinkDef.h
	$(CXX) -shared -o $(BIN)/libViewerGUI.so $(BIN)/ViewerGUI.o

$(BIN)/libViewerCore.so: src/ViewerCore.cpp include/ViewerCore.h
	$(CXX) -fPIC -c src/ViewerCore.cpp -o $(BIN)/ViewerCore.o $(INCLUDEGLOBAL) $(DEFFLAGS) $(ROOTFLAGS) -g -Wall
	#$(CXX) -shared $(BIN)/ViewerCore.o -o $(BIN)/libViewerCore.so -l:$(BIN)/libCalo3DViewer.so -l:$(BIN)/libCaloAMS2DViewer.so
	$(CXX) -shared $(BIN)/ViewerCore.o -o $(BIN)/libViewerCore.so -L$(BIN)/ -lCalo3DViewer -lCaloAMS2DViewer

$(BIN)/AMS-CCviewer: $(AMS-CCVIEWER-DEP)
	#$(CXX) AMS-CCviewer.cpp src/ViewerGUIDict.cpp -o $(BIN)/AMS-CCviewer -l:$(BIN)/libCalo3DViewer.so -l:$(BIN)/libCaloAMS2DViewer.so -l:$(BIN)/libDataFileHandler.so -l:$(BIN)/libViewerGUI.so  -l:$(BIN)/libViewerCore.so $(INCLUDEGLOBAL) $(COMMON)/obj/TrackSelection.o $(COMMON)/obj/Utilities.o $(ROOTFLAGS) $(DECODELIBS) -g -Wall $(DEFFLAGS) 
	$(CXX) AMS-CCviewer.cpp src/ViewerGUIDict.cpp -o $(BIN)/AMS-CCviewer -L$(BIN)/ -lCalo3DViewer -lCaloAMS2DViewer -lDataFileHandler -lViewerGUI  -lViewerCore $(INCLUDEGLOBAL) $(COMMON)/obj/TrackSelection.o $(COMMON)/obj/Utilities.o $(ROOTFLAGS) $(DECODELIBS) -g -Wall $(DEFFLAGS)

$(BIN)/PedMonitor:  PedMonitor.cpp  $(BIN)/libDataFileHandler.so
	$(CXX) PedMonitor.cpp -o $(BIN)/PedMonitor -L$(BIN)/ -lDataFileHandler $(INCLUDEGLOBAL) $(COMMON)/obj/TrackSelection.o $(COMMON)/obj/Utilities.o $(ROOTFLAGS) $(DECODELIBS) -g -Wall $(DEFFLAGS) 

clean:
	@rm -f  $(BIN)/AMS-CCviewer $(BIN)/PedMonitor $(BIN)/Calo3DViewer.o $(BIN)/libCalo3DViewer.so $(BIN)/CaloAMS2DViewer.o $(BIN)/libCaloAMS2DViewer.so $(BIN)/libCaloAMS2DViewer.so $(BIN)/ViewerGUI.o $(BIN)/libViewerGUI.so $(BIN)/DataFileHandler.o $(BIN)/libDataFileHandler.so $(BIN)/ViewerCore.o $(BIN)/libViewerCore.so
