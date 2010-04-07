nilibs=-lnidaqmxbase
includes=-I../../include
flags= -O2
cc=gcc

ai_examples = acquireNScans acquireNScans-ExtClk acquireNScans-AnlgStart \
              acquireNScans-DigRef contAcquire-ExtClk-DigStart \
              contAcquireNChan acquire1Scan loadAITask \
              acquireNScansThermocouple
      
all : $(ai_examples)

% : %.c
	$(cc) $(includes) $(flags) $< $(nilibs) -o $@

clean :
	rm -f $(ai_examples)
