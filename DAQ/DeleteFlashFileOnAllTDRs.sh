#!/bin/sh  

if [ $# -lt 2 ]; then
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]
    echo You must indicate a Flash file name [es. 3ea3 or 7f25] 
    exit
fi

echo STARTING $0

port=$1
name=$2

echo Flash file name is $name

for jinf in `cat jinflist`; do
  for tdr in `cat tdrlist`; do
    address=${tdr}3f${jinf}3f
    # Flash file test: is there? 
    OUTPUT=`./TESTjmdc $port $address 7 | grep $name`
    if [ -z "$OUTPUT" ]; 
      then echo "No Flash file $name on node $address"
    else 
      # Delete 
      OUTPUT=`./TESTjmdc $port $address 47 $name | grep rxdone`
      echo "Delete $name on address $address: $OUTPUT"
      usleep 100000
      # (If unsuccessful try again not implemented)
    fi
  done
done 

for jinf in `cat jinflist`; do
  for tdr in `cat tdrlist`; do
    address=${tdr}3f${jinf}3f
    # Flash file test: is there? 
    OUTPUT=`./TESTjmdc $port $address 7 | grep $name`
    if [ -z "$OUTPUT" ]; 
      then echo "OK: No Flash file $name on node $address"
    else 
	echo "KO: Flash file $name yet present on node $address"
    fi
  done
done 




