#!/bin/sh  

if [ $# -lt 1 ]; then
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]
    exit
fi

dspcode=3a33
confile=7011

echo STARTING $0

port=$1

./LoadDSPCode.sh $port $dspcode
usleep 100000
./LoadDSPCode.sh $port $confile
usleep 100000
#./EnableDynamicPedestalsWithPar.sh $port 1
#usleep 100000
# ./EnableSingleChannelClusterExclusion.sh $port 12 10
#./EnableSingleChannelClusterExclusion.sh $port 0 0 
#./GaussianStep4.sh $port 2048
