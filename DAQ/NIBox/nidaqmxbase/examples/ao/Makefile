nilibs=-lnidaqmxbase
includes=-I../../include
flags= -O2
cc=gcc

ao_examples = contGeneration multVoltUpdates contGen-ExtClk voltUpdate
      
all : $(ao_examples)

% : %.c
	$(cc) $(includes) $(flags) $< $(nilibs) -o $@

clean :
	rm -f $(ao_examples)
