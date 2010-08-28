#!/bin/sh  

if [ $# -lt 2 ]; then
    echo "You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]"
    echo "You must indicate the threshold value for high occupancy channels (calib. step 4) (valid from dsp code 3fa3)"
    exit
fi

echo STARTING $0

port=$1

threshex=`echo "ibase=10; obase=16; $2" | /usr/bin/bc`

parnumber=f

echo "I will use, for the parameter 0x$parnumber the value 0x$threshex"

sleep 5

./TESTjmdc $port 4000003f 49 1001 $parnumber $threshex
usleep 100000
./TESTjmdc $port 4000013f 49 1001 $parnumber $threshex
usleep 100000
./TESTjmdc $port 4000033f 49 1001 $parnumber $threshex
usleep 100000
./TESTjmdc $port 4000093f 49 1001 $parnumber $threshex
usleep 100000
./TESTjmdc $port 4000103f 49 1001 $parnumber $threshex
usleep 100000
./TESTjmdc $port 4000113f 49 1001 $parnumber $threshex
usleep 100000
./TESTjmdc $port 4000163f 49 1001 $parnumber $threshex
usleep 100000
./TESTjmdc $port 4000173f 49 1001 $parnumber $threshex
