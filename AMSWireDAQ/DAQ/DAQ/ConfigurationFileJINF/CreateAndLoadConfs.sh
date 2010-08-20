#!/bin/bash
if [ $# -lt 4 ]; then
    echo "You must indicate the two port of JMDC to use, i.e. the two JINJs to use"
    echo "You must indicate the name of the old configuration file to erase"
    echo "You must indicate the name of the new configuration file to write"
    exit
fi

jinj1=$1
jinj2=$2
oldconfname=$3
newconfname=$4

echo "This Script run \"./ConfigFile JINFParamFile_X.txt X $jinj1 $jinj2 $oldconfname $newconfname\" to create \"./dotdats/configfile_X.dat\" and \"LoadConfs_X.sh\""
echo "            then run ./LoadConfs_X.sh"
echo "            for all X's in [0-8]" 

sleep 5

for (( ii=0; ii<8; ii++ ))
do
    echo "./ConfigFile JINFParamFile_$ii.txt $ii $jinj1 $jinj2 $oldconfname $newconfname"
    ./ConfigFile JINFParamFile_$ii.txt $ii $jinj1 $jinj2 $oldconfname $newconfname
done
for (( ii=0; ii<8; ii++ ))
do
    echo "./LoadConfs_$ii.sh"
     ./LoadConfs_$ii.sh
done