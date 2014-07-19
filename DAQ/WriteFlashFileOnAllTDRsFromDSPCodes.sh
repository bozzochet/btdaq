#!/bin/sh  

if [ $# -lt 2 ]; then
    echo "You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]"
    echo "You must indicate a Flash file code name [the file will be searched into ./DSPCodes/] [es. 3ea3]"
    exit
fi

echo "STARTING $0" 

# 0. Prepare the file to be copied (symbolic link)
# 1. Load the new code and test the load

port=$1
name=$2

echo "Flash file name is $name"

if [ -a "./DSPCodes/file_tdr_$name.dat" ]; then
    echo "Preparing to load" 
    rm -rf file.dat
    cp -vf ./DSPCodes/file_tdr_$name.dat ./file.dat 
else
    echo "I can't find in ./DSPCodes/ the code named $name"
    exit
fi


for jinf in `cat jinflist`; do
    for tdr in `cat tdrlist`; do
	address=${tdr}3f${jinf}3f
        # Flash file test: is there? 
	OUTPUT=`./TESTjmdc $port $address 7 | grep $name`
	if [ -z "$OUTPUT" ];
	then
	    true
#	    echo "No DSP Code $name on node $address, so nothing to delete...";
	else
            # Flash File Delete 
	    OUTPUT=`./TESTjmdc $port $address 47 $name | grep rxdone` 
	    usleep 20000
	    echo "Delete Flash File $name on address $address: $OUTPUT"
	fi
        # Is there room for a new file? 
	OUTPUT=`./TESTjmdc $port $address 7 | grep ffff`
	if [ -z "$OUTPUT" ];
	then
	    echo "No room on node $address for writing a new file!!!";
	fi
    done
done


for jinf in `cat jinflist`; do
    for tdr in `cat tdrlist`; do
	address=${tdr}3f${jinf}3f
        # Flash File Load 
	OUTPUT=`./TESTjmdc $port $address 45 | grep rxdone` 
	echo "Write Flash File $name on address $address: $OUTPUT"
	usleep 20000
    done
done


for jinf in `cat jinflist`; do
    for tdr in `cat tdrlist`; do
	address=${tdr}3f${jinf}3f
        # Flash file test: is there? 
	OUTPUT=`./TESTjmdc $port $address 7 | grep $name`
	if [ -z "$OUTPUT" ]
	then echo "KO: No DSP Code $name on node $address"
	else
	    echo "OK: DSP Code $name present on node $address"
	fi
	usleep 20000
    done
done