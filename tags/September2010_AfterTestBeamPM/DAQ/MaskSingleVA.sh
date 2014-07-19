#!/bin/sh 

if [ $# -lt 4 ]; then
    echo $0 [JMDCport=0, ..., 3] [CrateNumber=0, ..., 7] [TDRNumber=0, ..., 23] [VANumber=0, ..., 16]
    exit
fi

# JMDC port
jmdc=$1

# CRATE
param=$2

if   [[ $param = "3" ]]; then 
  crate=1
elif [[ $param = "2" ]]; then
  crate=0
elif [[ $param = "7" ]]; then
  crate=23
elif [[ $param = "6" ]]; then
  crate=22
elif [[ $param = "0" ]]; then
  crate=3
elif [[ $param = "1" ]]; then
  crate=9
elif [[ $param = "4" ]]; then
  crate=16
elif [[ $param = "5" ]]; then
  crate=17
fi

# TDR number     
tdr=$3
     
# VA number
va=$4
nstrips=64
firststrip=$(($va*64))

command=`printf "./TESTjmdc %1hx ffff fffe %02hx3f %02hx3f 2e54 1 %hx %hx\n" $jmdc $crate $tdr $firststrip $nstrips`

echo $command
$command

