classes = Cluster.hh Event.hh
objects = Viewer.o rootElibdict.o Cluster.o Event.o
cpp = g++ -g
cflags = -Wall -I/usr/local/include `root-config --cflags` -Wno-long-long -Wno-variadic-macros -Wno-missing-braces -Wno-overloaded-virtual -c -pedantic 
lflags = -Wall -fPIC
llinks = -L/usr/lib `root-config --libs`
exec = Viewer

all: $(objects)
	$(cpp) $(lflags) $(objects) -o $(exec) $(llinks)

rootElibdict.cxx: $(classes) LinkDef.hh
	@echo Creating ROOT dictionary
	@rootcint -f $@ -c $^

Viewer.o: Viewer.cxx
	$(cpp) $(cflags) Viewer.cxx

rootElibdict.o: rootElibdict.cxx
	$(cpp) $(cflags) rootElibdict.cxx

Cluster.o: Cluster.cxx Cluster.hh
	$(cpp) $(cflags) Cluster.cxx

Event.o: Event.cxx Event.hh Cluster.hh
	$(cpp) $(cflags) Event.cxx

clean:
	rm -f *.o
	rm -f rootElibdict.*
	rm -f $(exec)
