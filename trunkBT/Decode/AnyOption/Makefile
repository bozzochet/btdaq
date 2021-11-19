SRC=anyoption.cxx

CXX := $(shell root-config --cxx)
#MAKEFLAGS = --no-print-directory -r -s
LIBS    = $(shell root-config --libs)
CPPFLAGS = $(shell root-config --cflags) -I. #-I$(ANYOPTION)
CXXFLAGS= -O3 -Wno-write-strings $(CPPFLAGS) -fstack-protector -D_FORTIFY_SOURCE=2

AnyOption:
	@echo "Building $@ ... "
	@$(CXX) $(CXXFLAGS) -c $(SRC) -o anyoption.o
#	@echo "Done"

clean:
	rm -f anyoption.o
