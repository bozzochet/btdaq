#!/bin/bash
usage() { echo "Usage: $0 [OPTIONS] 
    COMMON OPTIONS :
	-d 	date_time of data run (mandatory)
	-p      date_time of pedestal run to be used for ADAMO (mandatory)
	
	-a	analyze only adamo
	-c	analyze only calocube
" 1>&2; exit 1; }

errorCheck() { if [ $? -gt 0 ]; then
    exit 1;
    fi
}

CCdatadir="/home/calocube//SPS2017/CaloKagathos/acquired_files/"
ADdatadir="/home/calocube//SPS2017/CaloKagathos/acquired_files/"
CCoutdir="/home/calocube//SPS2017/CaloKagathos/analysed_files/"
ADoutdir="/home/calocube//SPS2017/CaloKagathos/analysed_files/"
CCdir="/home/calocube//SPS2017/CaloKagathos/"
ADdir="/home/calocube//SPS2017/ADAMO/"

data="./data.dat"
pedestal="./pedestal.ped"
adamo=true
calocube=true

if [ $# -eq 0 ]; then
    usage;
fi

TEMP=`getopt -o d:p:ach \
    -n $0 -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

eval set -- "$TEMP"

while true ; do
    case "$1" in
        -d) 
            case "$2" in
		"") usage ;;
		*) data=${2}
		    shift 2 ;;
            esac ;;
        -p) 
            case "$2" in
                "") usage ;;
                *) pedestal=${2}	
	    	    shift 2 ;;
            esac ;;
        -a) 
            case "$2" in
                *) calocube=false
		    shift 1 ;;
            esac ;; 
        -c) 
            case "$2" in
                *) adamo=false
		    shift 1 ;;
            esac ;;             
	-h)
	   usage ;; 
        --) shift ; break ;;
        *) usage ;;
    esac
done

echo -e "\n================================================================================="
echo -e "Starting analysis of run $data"
echo -e "==============================================================\n"
   
if $calocube ; then
	echo -e "================================\n"
	echo -e "==========  CALOUBE  ===========\n"
	echo -e "================================\n"
	
	inCCfile="$CCdatadir/CC${data}.dat"
	outCCfile="$CCoutdir/CC${data}.root"

#	if [ -f ${outCCfile} ]; then
#	    echo -e "\n $outCCfile already present! Skipping... \n" 
#	    exit 1;
#	fi
	
	if [ ! -f ${inCCfile} ]; then
	    echo -e "\n $inCCfile not found! Exit... \n" 
	    exit 1;
	fi
	
	CCcommand="$CCdir/analyze $inCCfile $outCCfile"
	echo $CCcommand 
	$CCcommand
	errorCheck
fi
    
if $adamo ; then    
	echo -e "================================\n"
	echo -e "===========  ADAMO  ============\n"
	echo -e "================================\n"
	
        inADfile="$ADdatadir/AD${data}.dat"
	inADpedfile="$ADdatadir/AD${pedestal}.ped"
	outADpedfile="$ADdir/analisi/pedestal/data/AD${pedestal}.ped"
	outADclufile="$ADdir/analisi/cluster/data/AD${data}.clu"
	outADpawfile="$ADdir/analisi/cluster/data/AD${data}.rz"
	outADrootfile="$ADdir/analisi/analisi/data/AD${data}.root"
	
#	if [ -f ${outADrootfile} ]; then
#	    echo -e "\n $outADrootfile already present! Skipping... \n" 
#	    exit 1;
#	fi

	if [ ! -f ${inADfile} ]; then
	    echo -e "\n $inADfile not found! Exit... \n" 
	    exit 1;
	fi
	if [ ! -f ${inADpedfile} ]; then
	    echo -e "\n $inADpedfile not found! Exit... \n" 
	    exit 1;
	fi
	
	ADcommand="$ADdir/analisi/pedestal/calibra $inADpedfile $ADdir/analisi/pedestal/conf $outADpedfile"
	echo $ADcommand 
	$ADcommand
	errorCheck

	if [ -L $ADdir/analisi/pedestal/data/ped ]; then
		ln -fs $outADpedfile $ADdir/analisi/pedestal/data/ped
	else
		ln -fs $outADpedfile $ADdir/analisi/pedestal/data/ped
	fi

	ADcommand="$ADdir/analisi/cluster/clusterfind $outADpedfile $inADfile $ADdir/analisi/cluster/conf $outADclufile"
	echo $ADcommand 
	$ADcommand
	errorCheck

	if [ -L $ADdir/analisi/cluster/data/clu ]; then
		ln -fs $outADclufile $ADdir/analisi/cluster/data/clu 
	else
		ln -fs $outADclufile $ADdir/analisi/cluster/data/clu
	fi

	echo -e "$ADdir/analisi/pedestal/data/ped\n$ADdir/analisi/cluster/data/clu\n$ADdir/analisi/cluster/data/paw" > $ADdir/analisi/analisi/lancia

	ADcommand="$ADdir/analisi/analisi/analisi < $ADdir/analisi/analisi/lancia"
	echo $ADcommand 
	$ADdir/analisi/analisi/analisi < $ADdir/analisi/analisi/lancia
	errorCheck
	
	mv $ADdir/analisi/cluster/data/paw $outADpawfile
	
	ADcommand="h2root $outADpawfile $outADrootfile"
	echo $ADcommand 
	$ADcommand
	errorCheck
fi

if [[ $calocube && $adamo ]]; then
	command="hadd -f $CCoutdir/comb/run${data}.root $outADrootfile $outCCfile"
    	echo $command 
	$command
	errorCheck
fi
