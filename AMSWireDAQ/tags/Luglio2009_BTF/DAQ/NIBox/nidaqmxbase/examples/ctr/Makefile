nilibs=-lnidaqmxbase
includes=-I../../include
flags= -O2
cc=gcc

ctr_examples = cntDigEvents cntBufCont buffPeriodFinite pulseWidth \
               genDigPulse genDigPulseTrainCont genDigPulseTrainCont-DigStart
      
all : $(ctr_examples)

% : %.c
	$(cc) $(includes) $(flags) $< $(nilibs) -o $@

clean :
	rm -f $(ctr_examples)
