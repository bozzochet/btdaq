#!/bin/sh  

if [ $# -lt 2 ]; then
    echo "You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]"
    echo "You must indicate the correction to apply at each event in unit of 1/8 ADC (valid from dsp code 3fb3)"
    exit
fi

echo STARTING $0

port=$1
par=$2

echo "./TESTjmdc $port 4000003f 49 1001 b 1"
./TESTjmdc $port 4000003f 49 1001 b $par
usleep 100000
echo "./TESTjmdc $port 4000013f 49 1001 b 1"
./TESTjmdc $port 4000013f 49 1001 b $par
usleep 100000
echo "./TESTjmdc $port 4000033f 49 1001 b 1"
./TESTjmdc $port 4000033f 49 1001 b $par
usleep 100000
echo "./TESTjmdc $port 4000093f 49 1001 b 1"
./TESTjmdc $port 4000093f 49 1001 b $par
usleep 100000
echo "./TESTjmdc $port 4000103f 49 1001 b 1"
./TESTjmdc $port 4000103f 49 1001 b $par
usleep 100000
echo "./TESTjmdc $port 4000113f 49 1001 b 1"
./TESTjmdc $port 4000113f 49 1001 b $par
usleep 100000
echo "./TESTjmdc $port 4000163f 49 1001 b 1"
./TESTjmdc $port 4000163f 49 1001 b $par
usleep 100000
echo "./TESTjmdc $port 4000173f 49 1001 b 1"
./TESTjmdc $port 4000173f 49 1001 b $par