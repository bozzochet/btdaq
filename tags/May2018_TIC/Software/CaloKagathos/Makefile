CXX = g++
CXX32 = g++ -m32
CXX64 = g++ -m64
ROOTFLAGS = `root-config --libs --cflags`
QUSBFLAGS = -lusb -lquickusb
#DEFFLAGS = -DTIC -DDRIVER
DEFFLAGS = -DTIC
ACQUIRE_CC = acquire_calocube
ANALYZE_ALONE = analyze_alone
ANALYZE = analyze
DRAW_ALONE = draw_alone
VIEWER = viewer
TEST = test
DRAW = draw
INC = inc
SRC = src
BIN = bin

#AMS stuff
AMSDIR=$(PWD)/../Perugia
DECODE =$(AMSDIR)/Decode/
COMMON =$(AMSDIR)/CommonTool/
DECODELIBS = -Wl,-rpath,$(DECODE) -L$(DECODE) -lEvent

all: iofile_alone iofile acquire_calocube analyze_alone analyze draw_alone draw viewer test link

bin/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INC) $(DEFFLAGS) $(ROOTFLAGS) -c -o $@ $<

iofile_alone: $(INC)/IOfile.h $(INC)/IOfile.cc
	$(CXX) -c $(INC)/IOfile.cc -o $(BIN)/IOfile_alone.o $(DEFFLAGS) $(ROOTFLAGS) -g -Wall
	
iofile: $(INC)/IOfile.h $(INC)/IOfile.cc
	$(CXX) -c $(INC)/IOfile.cc -o $(BIN)/IOfile.o $(DEFFLAGS) -DAMS $(ROOTFLAGS) -g -Wall

acquire_calocube: $(SRC)/acquire_calocube.cpp	
	$(CXX) $(SRC)/acquire_calocube.cpp -o $(BIN)/$(ACQUIRE_CC) -I$(INC) $(DEFFLAGS) $(QUSBFLAGS) -g -Wall -lrt
		
analyze_alone: $(SRC)/analyze.cpp $(BIN)/IOfile_alone.o
	$(CXX) $(SRC)/analyze.cpp -o $(BIN)/$(ANALYZE_ALONE) $(BIN)/IOfile_alone.o -I$(INC) $(DEFFLAGS) $(ROOTFLAGS) -g -Wall

analyze: $(SRC)/analyze.cpp $(BIN)/IOfile.o
	$(CXX) $(SRC)/analyze.cpp -o $(BIN)/$(ANALYZE) $(BIN)/IOfile.o -DAMS -I$(INC) $(DEFFLAGS) $(ROOTFLAGS) -g -Wall

draw_alone: $(SRC)/draw.cpp
	$(CXX) $(SRC)/draw.cpp -o $(BIN)/$(DRAW_ALONE) -I$(INC) $(DEFFLAGS) $(ROOTFLAGS) -g -Wall

draw: $(SRC)/draw.cpp
	$(CXX) $(SRC)/draw.cpp -o $(BIN)/$(DRAW) -I$(INC) -I$(DECODE) -I$(COMMON) -DAMS $(DEFFLAGS) $(ROOTFLAGS) $(DECODELIBS) -g -Wall

viewer: $(SRC)/viewer.cpp
	$(CXX) $(SRC)/viewer.cpp -o $(BIN)/$(VIEWER) -I$(INC) -I$(DECODE) -I$(COMMON) $(DEFFLAGS) $(ROOTFLAGS) $(DECODELIBS) -g -Wall

test: $(SRC)/test.cpp
	$(CXX) $(SRC)/test.cpp -o $(BIN)/$(TEST) -I$(INC) -I$(DECODE) -I$(COMMON) $(DEFFLAGS) $(ROOTFLAGS) $(DECODELIBS) -g -Wall

link:
	@rm -f $(ACQUIRE_CC) $(ANALYZE) $(ANALYZE_ALONE) $(DRAW) $(DRAW_ALONE) $(VIEWER) $(TEST)
	ln -sf $(BIN)/$(ACQUIRE_CC) $(ACQUIRE_CC)
	ln -sf $(BIN)/$(ANALYZE_ALONE) $(ANALYZE_ALONE)		
	ln -sf $(BIN)/$(ANALYZE) $(ANALYZE)		
	ln -sf $(BIN)/$(DRAW_ALONE) $(DRAW_ALONE)
	ln -sf $(BIN)/$(DRAW) $(DRAW)
	ln -sf $(BIN)/$(VIEWER) $(VIEWER)			
	ln -sf $(BIN)/$(TEST) $(TEST)			
	
clean:
	@rm -f *.o *~ $(ACQUIRE_CC) $(ANALYZE_ALONE) $(ANALYZE) $(DRAW) $(DRAW_ALONE) $(VIEWER) $(TEST)
	@rm -f $(BIN)/$(ACQUIRE_CC) $(BIN)/$(ANALYZE_ALONE) $(BIN)/$(ANALYZE) $(BIN)/$(DRAW) $(BIN)/$(DRAW_ALONE) $(BIN)/$(VIEWER) $(BIN)/$(TEST) $(BIN)/IOfile_alone.o $(BIN)/IOfile.o
