nilibs=-lnidaqmxbase
includes=-I./nidaqmxbase/include
flags= -g

 
SOURCES= main.cxx SmallNetServer.cxx NIDaqClass.cxx
OBJ= main.o SmallNetServer.o NIDaqClass.o

all : server test


%.o : %.cxx
	$(CXX) $(includes) $(flags) -c -o $@ $< 

server : $(OBJ)
	$(CXX)   $(flags) $^ $(nilibs) -o $@

test : main2.o NIDaqClass.o
	$(CXX)   $(flags) $^ $(nilibs) -o $@


clean :
	rm -f *.o server
