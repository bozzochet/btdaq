#!/bin/bash

DATADIR=$1
current_directory=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $current_directory
#echo $current_directory

declare -a array=( upstream downstream )

for SUFFIX in ${array[@]}
do
    echo $SUFFIX
    
#-------------------------------------------------------------------------------------

    ls -lrt Data_$SUFFIX/ | grep dat | awk '{ print $9 " " $5}' > FILELIST_$SUFFIX.txt
    
    if [ ! -f FILELIST_$SUFFIX.old.txt ]; then
        touch FILELIST_$SUFFIX.old.txt 
    fi

    for i in `diff -y --suppress-common-lines FILELIST_$SUFFIX.txt FILELIST_$SUFFIX.old.txt | awk '{ print $1 }'`;
    do
#	echo $i;
#	echo ${i:0:10}
	echo ./Decode ${i:0:10} -c --cworkaround 1 --rootdata ${DATADIR}/RootData_$SUFFIX/ --rawdata Data_$SUFFIX --caldata Calibrations_$SUFFIX/
	./Decode ${i:0:10} -c --cworkaround 1 --rootdata ${DATADIR}/RootData_$SUFFIX/ --rawdata Data_$SUFFIX --caldata Calibrations_$SUFFIX/
    done
    
    cp FILELIST_$SUFFIX.txt FILELIST_$SUFFIX.old.txt

#-------------------------------------------------------------------------------------

    ls -lrt Calibrations_$SUFFIX/ | grep cal | grep ANC | grep -v pdf | grep -v $'>' | awk '{ print $9 " " $5}' > CALLIST_$SUFFIX.txt

    if [ ! -f CALLIST_$SUFFIX.old.txt ]; then
        touch CALLIST_$SUFFIX.old.txt 
    fi

    rm Calibrations
    ln -sv Calibrations_$SUFFIX Calibrations

    for i in `diff -y --suppress-common-lines CALLIST_$SUFFIX.txt CALLIST_$SUFFIX.old.txt | awk '{ print $1 }'`;
    do
#	echo $i;
#	echo ${i:0:10}
	echo ./SUMMARY ${i:0:10} 0
	./SUMMARY ${i:0:10} 0
    done
    
    cp CALLIST_$SUFFIX.txt CALLIST_$SUFFIX.old.txt

done
