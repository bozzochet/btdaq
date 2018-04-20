#!/bin/bash

DATADIR=$1
current_directory=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $current_directory
#echo $current_directory


#-------------------------------------------------------------------------------------

    ls -lrt ${DATADIR}/Data/ | grep dat | awk '{ print $9 " " $5}' > FILELIST.txt
    
    if [ ! -f FILELIST.old.txt ]; then
        touch FILELIST.old.txt 
    fi

    for i in `diff -y --suppress-common-lines FILELIST.txt FILELIST.old.txt | awk '{ print $1 }'`;
    do
#	echo $i;
#	echo ${i:0:10}

	DECODE_ARG="${i:0:10} "
	if [[ $i == *ANC* ]]; then
#	    echo " It is with ANC ";	  
	    ARG2=${i#*ANC_*};	  
	    ARG2=${ARG2%*.*};
#	    echo $ARG2;
	    DECODE_ARG+=" $ARG2 "
	fi
#	echo " DECODE ARG " $DECODE_ARG
	COMMAND=" ./Decode $DECODE_ARG -c --cworkaround 1 --rootdata ${DATADIR}/RootData/ --rawdata ${DATADIR}/Data --caldata ${DATADIR}/Calibrations/ "
	echo $COMMAND 
	$COMMAND 
#	./Decode ${i:0:10} -c --cworkaround 1 --rootdata RootData/ --rawdata RawData --caldata Calibrations/
    done
    
    cp FILELIST.txt FILELIST.old.txt

#-------------------------------------------------------------------------------------

    ls -lrt ${DATADIR}/Calibrations/ | grep cal | grep -v ANC | grep -v pdf | grep -v $'>' | awk '{ print $9 " " $5}' > CALLIST.txt

    if [ ! -f CALLIST.old.txt ]; then
        touch CALLIST.old.txt 
    fi

#    rm Calibrations
#    ln -sv Calibrations Calibrations

    for i in `diff -y --suppress-common-lines CALLIST.txt CALLIST.old.txt | awk '{ print $1 }'`;
    do
	echo $i;
	echo ${i:0:10}
	echo ./SUMMARY ${i:0:10} 0
#	./SUMMARY ${i:0:10} 0
	break
    done
    
    cp CALLIST.txt CALLIST.old.txt


