#!/bin/sh  

if [ $# -lt 3 ]; then
    echo "You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]"
    echo "You must indicate the threshold to be applied for the exclusion on S side (valid from dsp code 3fb3). In units of ADC count."
    echo "You must indicate the threshold to be applied for the exclusion on K side (valid from dsp code 3fb3). In units of ADC count."
    exit
fi

echo STARTING $0

port=$1
let "Sside=$2*8"
let "Kside=$3*8"

echo "The Threshold on S side in units of 1/8 ADC is $Sside"
echo "The Threshold on K side in units of 1/8 ADC is $Kside"

lsb=`echo "ibase=10; obase=16; $Sside" | /usr/bin/bc`
msb=`echo "ibase=10; obase=16; $Kside" | /usr/bin/bc`

echo "The Threshold on S side in units of 1/8 ADC in hex is 0x$lsb"
echo "The Threshold on K side in units of 1/8 ADC is hex is 0x$msb"

parnumber=1c

echo "I will use, for the parameter 0x$parnumber the value 0x$msb$lsb"

sleep 5

./TESTjmdc $port 4000003f 49 1001 $parnumber $msb$lsb
usleep 100000
./TESTjmdc $port 4000013f 49 1001 $parnumber $msb$lsb
usleep 100000
./TESTjmdc $port 4000033f 49 1001 $parnumber $msb$lsb
usleep 100000
./TESTjmdc $port 4000093f 49 1001 $parnumber $msb$lsb
usleep 100000
./TESTjmdc $port 4000103f 49 1001 $parnumber $msb$lsb
usleep 100000
./TESTjmdc $port 4000113f 49 1001 $parnumber $msb$lsb
usleep 100000
./TESTjmdc $port 4000163f 49 1001 $parnumber $msb$lsb
usleep 100000
./TESTjmdc $port 4000173f 49 1001 $parnumber $msb$lsb