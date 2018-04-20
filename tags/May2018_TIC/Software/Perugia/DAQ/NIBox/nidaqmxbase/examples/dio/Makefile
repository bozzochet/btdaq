nilibs=-lnidaqmxbase
includes=-I../../include
flags= -O2
cc=gcc

dio_examples = readWriteDigPort readDigPort writeDigPort readDigMultiplePorts writeDigMultiplePorts ContReadDigChan-ExtClk ContWriteDigPort-ExtClk
      
all : $(dio_examples)

% : %.c
	$(cc) $(includes) $(flags) $< $(nilibs) -o $@

clean :
	rm -f $(dio_examples)
