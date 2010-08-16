#!/bin/sh  

if [ $# -lt 4 ]; then
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]
    echo You must indicate the S1 high threshold
    echo You must indicate the S2 high threshold
    echo You must indicate the K high threshold
    exit
fi

echo STARTING $0

port=$1
s1=$2
s2=$3
k=$4

echo "./TESTjmdc $port 4000003f 49 1003 2 $s1 4 $s2 6 $k"
./TESTjmdc $port 4000003f 49 1003 2 $s1 4 $s2 6 $k
usleep 100000
echo "./TESTjmdc $port 4000013f 49 1003 2 $s1 4 $s2 6 $k"
./TESTjmdc $port 4000013f 49 1003 2 $s1 4 $s2 6 $k
usleep 100000
echo "./TESTjmdc $port 4000033f 49 1003 2 $s1 4 $s2 6 $k"
./TESTjmdc $port 4000033f 49 1003 2 $s1 4 $s2 6 $k
usleep 100000
echo "./TESTjmdc $port 4000093f 49 1003 2 $s1 4 $s2 6 $k"
./TESTjmdc $port 4000093f 49 1003 2 $s1 4 $s2 6 $k
usleep 100000
echo "./TESTjmdc $port 4000103f 49 1003 2 $s1 4 $s2 6 $k"
./TESTjmdc $port 4000103f 49 1003 2 $s1 4 $s2 6 $k
usleep 100000
echo "./TESTjmdc $port 4000113f 49 1003 2 $s1 4 $s2 6 $k"
./TESTjmdc $port 4000113f 49 1003 2 $s1 4 $s2 6 $k
usleep 100000
echo "./TESTjmdc $port 4000163f 49 1003 2 $s1 4 $s2 6 $k"
./TESTjmdc $port 4000163f 49 1003 2 $s1 4 $s2 6 $k
usleep 100000
echo "./TESTjmdc $port 4000173f 49 1003 2 $s1 4 $s2 6 $k"
./TESTjmdc $port 4000173f 49 1003 2 $s1 4 $s2 6 $k

