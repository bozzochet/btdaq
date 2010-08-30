#!/bin/sh  

if [ $# -lt 1 ]; then
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]
    exit
fi

dspcode=29f3
confile=6001

echo STARTING $0

port=$1

./LoadDSPCode.sh $port $dspcode
usleep 100000
./LoadDSPCode.sh $port $confile
usleep 100000
./EnableDynamicPedestals.sh $port