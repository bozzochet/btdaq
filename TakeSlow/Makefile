



all: TakeSlowEPP

TakeSlowEPP: PUtil.cxx TakeSlow.cxx
	$(CXX) -o $@ $^ -I../amsw/ -I../PeCpIp/ -I ../SlowControl/ -L ../lib/ -lamswire

clean:
	rm -fv PUtil.o TakeSlow.o TakeSlowEPP
