nilibs=-lnidaqmxbase
includes=-I/usr/local/natinst/nidaqmxbase/include
flags= -O2
cc=gcc

dio_examples = readWriteDigPort readDigPort writeDigPort readDigMultiplePorts writeDigMultiplePorts ContReadDigChan-ExtClk ContWriteDigPort-ExtClk mypulse
      
all : $(dio_examples)

% : %.c
	$(cc) $(includes) $(flags) $< $(nilibs) -o $@

clean :
	rm -f $(dio_examples)
