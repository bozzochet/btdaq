#!/bin/sh  

if [ $# -lt 1 ]; then
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]
    exit
fi

echo STARTING $0

port=$1

./TESTjmdc $port 4000003f 49 1001 b 0
sleep 2
./TESTjmdc $port 4000013f 49 1001 b 0
sleep 2
./TESTjmdc $port 4000033f 49 1001 b 0
sleep 2
./TESTjmdc $port 4000093f 49 1001 b 0
sleep 2
./TESTjmdc $port 4000103f 49 1001 b 0
sleep 2
./TESTjmdc $port 4000113f 49 1001 b 0
sleep 2
./TESTjmdc $port 4000163f 49 1001 b 0
sleep 2
./TESTjmdc $port 4000173f 49 1001 b 0