#!/bin/sh  

if [ $# -lt 3 ]; then
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]
    echo You must indicate a Flash file name [es. 3ea3] and a default state [0/1]
    exit
fi

echo STARTING $0

port=$1
name=$2
default=$3

echo "Flash file name is $name"

for jinf in `cat jinflist`; do
    address=${jinf}3f
    # Flash file test: is there? 
    OUTPUT=`./TESTjmdc $port $address 7 | grep $name`
    if [ -z "$OUTPUT" ]; 
    then echo "No DSP Code $name on node $address (port: $port)"
    else 
      # Set default 
	OUTPUT=`./TESTjmdc $port $address 48 $name $default | grep rxdone`
	echo "Set default to $default of $name on address $address (port: $port) $OUTPUT"
	usleep 100000
      # (If unsuccessful try again not implemented)
    fi
done

echo "Now you setted as default the file $name only on one side of JINF (port: $port)! Please, repeate this operation with another JINJ!!!!"