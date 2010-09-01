#!/bin/sh

if [ $# -lt 3 ]; then
    echo "You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]"
    echo "You must indicate the SDR side to use [A or B]"
    echo "You must indicate the artificial dead time (in unit of musec) you want to apply"
    exit
fi

port=$1
sdr=$2
deadtimemusec=$3

let "deadtime=$deadtimemusec*50"

deadtimehex=`echo "ibase=10; obase=16; $deadtime" | /usr/bin/bc`

if [[ $sdr = "A" ]]
then
    address="063f"
    ./TESTjmdc $port $address 49 5001 010a $deadtimehex
    address="043f"
    ./TESTjmdc $port $address 49 5001 010a $deadtimehex
    address="123f"
    ./TESTjmdc $port $address 49 5001 010a $deadtimehex
    address="143f"
    ./TESTjmdc $port $address 49 5001 010a $deadtimehex
elif [[ $sdr = "B" ]]
then
    address="073f"
    ./TESTjmdc $port $address 49 5001 010a $deadtimehex
    address="053f"
    ./TESTjmdc $port $address 49 5001 010a $deadtimehex
    address="133f"
    ./TESTjmdc $port $address 49 5001 010a $deadtimehex
    address="153f"
    ./TESTjmdc $port $address 49 5001 010a $deadtimehex
fi
