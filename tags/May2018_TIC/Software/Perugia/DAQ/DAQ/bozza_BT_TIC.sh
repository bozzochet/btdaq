#!/bin/bash

#tocchera' fare come root

for i in `seq 0 100`
do
#ricopiare la logica sotto con CALIB invece che TAKEDATA
#COMPLETERUN NON se po usa
#con gli opportuni sleep NON serve il kill in questo caso
    ./TakeEPP 0 CALIB

    cd ../DAQ_clone
    ./TakeEPP 1 TAKEDATA &
    sleep 60
    PID=`ps aux | grep TakeEPP | grep -v grep | awk '{ print $2 }'`
    cd ../DAQ
    ./TakeEPP 0 TAKEDATA  10000
    kill $PID
    sleep 60
done



