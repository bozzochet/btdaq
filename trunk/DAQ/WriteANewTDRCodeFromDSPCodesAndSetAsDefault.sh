#!/bin/sh  

if [ $# -lt 3 ]; then
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]
    echo You must indicate a Flash file code name [es. 3ea3] for the DSP code to be Written on Flash and to be Setted as Default [1th parameter]
    echo You must indicate a Flash file code name [es. 2ee3] for the DSP code to be Setted as NoMoreDefault [2nd parameter]
    exit
fi

echo STARTING $0 

port=$1
newname=$2
oldname=$3

echo New Flash file name is $newname
echo Old Flash file name is $oldname

WriteFlashFileOnAllTDRsFromDSPCodes.sh $port $newname
SetDefaultFlashFileOnAllTDRs.sh $port $oldname 0
SetDefaultFlashFileOnAllTDRs.sh $port $newname 1
LoadFlashFileOnAllTDRs.sh $port $newname

