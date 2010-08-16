#!/bin/sh  

if [ $# -lt 2 ]; then
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]
    echo You must indicate the parameter to use (which flags to erase)
    exit
fi

echo STARTING $0

port=$1
par=$2

echo "./TESTjmdc $port 4000003f 54 3 $par"
./TESTjmdc $port 4000003f 54 3 $par
usleep 100000
echo "./TESTjmdc $port 4000013f 54 3 $par"
./TESTjmdc $port 4000013f 54 3 $par
usleep 100000
echo "./TESTjmdc $port 4000033f 54 3 $par"
./TESTjmdc $port 4000033f 54 3 $par
usleep 100000
echo "./TESTjmdc $port 4000093f 54 3 $par"
./TESTjmdc $port 4000093f 54 3 $par
usleep 100000
echo "./TESTjmdc $port 4000103f 54 3 $par"
./TESTjmdc $port 4000103f 54 3 $par
usleep 100000
echo "./TESTjmdc $port 4000113f 54 3 $par"
./TESTjmdc $port 4000113f 54 3 $par
usleep 100000
echo "./TESTjmdc $port 4000163f 54 3 $par"
./TESTjmdc $port 4000163f 54 3 $par
usleep 100000
echo "./TESTjmdc $port 4000173f 54 3 $par"
./TESTjmdc $port 4000173f 54 3 $par

