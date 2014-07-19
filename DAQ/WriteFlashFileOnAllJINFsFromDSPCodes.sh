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
    cp -fv ./DSPCodes/file_tdr_$name.dat ./file.dat 
else
    echo "I can't find in ./DSPCodes/ the code named $name"
    exit
fi

for jinf in `cat jinflist`; do
    address=${jinf}3f
    # Flash file test: is there? 
    OUTPUT=`./TESTjmdc $port $address 7 | grep $name`
    if [ -z "$OUTPUT" ]
    then
	true
#	echo "No DSP Code $name on node $address (port: $port), so nothing to delete..."
    else
        # Flash File Load 
	OUTPUT=`./TESTjmdc $port $address 47 | grep rxdone` 
	usleep 20000
	echo "Delete Flash File $name on address $address (port: $port): $OUTPUT"
    fi
    # Is there room for a file? 
    OUTPUT=`./TESTjmdc $port $address 7 | grep ffff`
    if [ -z "$OUTPUT" ]
    then
	echo "No room on node $address (port: $port) for writing a new file!!!"
    fi	
done 

for jinf in `cat jinflist`; do
    address=${jinf}3f
    # Flash File Load 
    OUTPUT=`./TESTjmdc $port $address 45 $name | grep rxdone` 
    echo "Write Flash File $name on address $address (port: $port): $OUTPUT"
    usleep 20000
done 


#for jinf in `cat jinflist`; do
#    address=${jinf}3f
#    # Flash file test: is there? 
#    OUTPUT=`./TESTjmdc $port $address 7 | grep $name`
#    if [ -z "$OUTPUT" ]
#    then echo "KO: No DSP Code $name on node $address (port: $port)"
#    else
#	echo "OK: DSP Code $name present on node $address (port: $port)"
#    fi
#    usleep 20000
#done 

echo "Now you made this operation only on one side of JINF (port: $port)! Please, repeate this operation with another JINJ!!!!"