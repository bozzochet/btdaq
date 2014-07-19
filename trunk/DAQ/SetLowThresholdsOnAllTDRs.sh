#!/bin/sh
if [ $# -lt 4 ]; then
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]
    echo You must indicate the S1 low threshold
    echo You must indicate the S2 low threshold
    echo You must indicate the K low threshold
    exit
fi

echo STARTING $0

port=$1
s1=$2
s2=$3
k=$4

echo "./TESTjmdc $port 4000003f 49 1003 1 $s1 3 $s2 5 $k"
./TESTjmdc $port 4000003f 49 1003 1 $s1 3 $s2 5 $k
usleep 100000
echo "./TESTjmdc $port 4000013f 49 1003 1 $s1 3 $s2 5 $k"
./TESTjmdc $port 4000013f 49 1003 1 $s1 3 $s2 5 $k
usleep 100000
echo "./TESTjmdc $port 4000033f 49 1003 1 $s1 3 $s2 5 $k"
./TESTjmdc $port 4000033f 49 1003 1 $s1 3 $s2 5 $k
usleep 100000
echo "./TESTjmdc $port 4000093f 49 1003 1 $s1 3 $s2 5 $k"
./TESTjmdc $port 4000093f 49 1003 1 $s1 3 $s2 5 $k
usleep 100000
echo "./TESTjmdc $port 4000103f 49 1003 1 $s1 3 $s2 5 $k"
./TESTjmdc $port 4000103f 49 1003 1 $s1 3 $s2 5 $k
usleep 100000
echo "./TESTjmdc $port 4000113f 49 1003 1 $s1 3 $s2 5 $k"
./TESTjmdc $port 4000113f 49 1003 1 $s1 3 $s2 5 $k
usleep 100000
echo "./TESTjmdc $port 4000163f 49 1003 1 $s1 3 $s2 5 $k"
./TESTjmdc $port 4000163f 49 1003 1 $s1 3 $s2 5 $k
usleep 100000
echo "./TESTjmdc $port 4000173f 49 1003 1 $s1 3 $s2 5 $k"
./TESTjmdc $port 4000173f 49 1003 1 $s1 3 $s2 5 $k