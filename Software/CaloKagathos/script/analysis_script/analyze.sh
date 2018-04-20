#!/bin/bash
usage() { echo "Usage: $0 [OPTIONS] 
    COMMON OPTIONS :
	-d 	date_time of data run (mandatory)
	-p      date_time of pedestal run to be used for ADAMO (mandatory)
	
	-i	input directory
	-o	output directory

	-a	analyze only adamo
	-c	analyze only calocube
" 1>&2; exit 1; }

errorCheck() { if [ $? -gt 0 ]; then
    exit 1;
    fi
}

ANALYSIS_DIR="/home/berti/calocube/SPS2017Aug/"

inDir=""
outDir=""
CCdir="$ANALYSIS_DIR/CaloKagathos/"
ADdir="$ANALYSIS_DIR/ADAMO/"

data="./data.dat"
pedestal="./pedestal.ped"
adamo="true"
calocube="true"

if [ $# -eq 0 ]; then
    usage;
fi

TEMP=`getopt -o d:p:i:o:ach \
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
        -i) 
            case "$2" in
		"") usage ;;
		*) inDir=${2}
		    shift 2 ;;
            esac ;;
        -o) 
            case "$2" in
                "") usage ;;
                *) outDir=${2}	
	    	    shift 2 ;;
            esac ;;
        -a) 
            case "$2" in
                *) calocube="false"
		    shift 1 ;;
            esac ;; 
        -c) 
            case "$2" in
                *) adamo="false"
		shift 1 ;;
            esac ;;             
	-h)
	   usage ;; 
        --) shift ; break ;;
        *) usage ;;
    esac
done

if [[ -z "$inDir" ]]; then
	echo "Empty input directory : Exit..."
fi

if [[ -z "$outDir" ]]; then
	echo "Empty output directory : Exit..."
fi

CCdatadir=$inDir
ADdatadir=$inDir
CCoutdir=$outDir
ADoutdir=$outDir
AuxDir="/gpfs/home/berti/Aux"

echo -e "\n================================================================================="
echo -e "Starting analysis of run $data"
echo -e "==============================================================\n"
   
if [ "$calocube" == "true" ]; then
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
    
if [[ $adamo == "true" ]] ; then    
	echo -e "================================\n"
	echo -e "===========  ADAMO  ============\n"
	echo -e "================================\n"
	
        inADfile="$ADdatadir/AD${data}.dat"
	inADpedfile="$ADdatadir/AD${pedestal}.ped"
	outADpedfile="$ADdir/analisi/pedestal/data/AD${pedestal}-${data}.ped"
	outADclufile="$ADdir/analisi/cluster/data/AD${data}.clu"
	outADpawfile="$ADdir/analisi/cluster/data/AD${data}.rz"
	outADrootfile="$CCoutdir/AD${data}.root"
	auxADpedfile="$AuxDir/AD${pedestal}-${data}.ped"
	auxADclufile="$AuxDir/AD${data}.clu"
	auxADpawfile="$AuxDir/AD${data}.rz"
	auxADtempfile="$CCoutdir/AD${data}-RawFile.root"
	cmdADfile="$ADdir/analisi/analisi/cmd_${data}"

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

#	if [ -L $ADdir/analisi/pedestal/data/ped ]; then
#		ln -fs $outADpedfile $ADdir/analisi/pedestal/data/ped
#	else
#		ln -fs $outADpedfile $ADdir/analisi/pedestal/data/ped
#	fi
	ln -fs $outADpedfile $auxADpedfile
	echo "$auxADpedfile" > $cmdADfile

	ADcommand="$ADdir/analisi/cluster/clusterfind $outADpedfile $inADfile $ADdir/analisi/cluster/conf $outADclufile"
	echo $ADcommand 
	$ADcommand
	errorCheck

#	if [ -L $ADdir/analisi/cluster/data/clu ]; then
#		ln -fs $outADclufile $ADdir/analisi/cluster/data/clu 
#	else
#		ln -fs $outADclufile $ADdir/analisi/cluster/data/clu
#	fi
	ln -fs $outADclufile $auxADclufile
	echo "$auxADclufile" >> $cmdADfile

#	echo -e "$ADdir/analisi/pedestal/data/ped\n$ADdir/analisi/cluster/data/clu\n$ADdir/analisi/cluster/data/paw" > $ADdir/analisi/analisi/lancia
	echo "$auxADpawfile" >> $cmdADfile

	ADcommand="$ADdir/analisi/analisi/analisi < $cmdADfile"
	echo $ADcommand 
	$ADdir/analisi/analisi/analisi < $cmdADfile
	errorCheck
	
#	mv $ADdir/analisi/cluster/data/paw $outADpawfile
	mv $auxADpawfile $outADpawfile
	rm -f $cmdADfile
	rm -f $auxADpedfile $auxADclufile

	ADcommand="h2root $outADpawfile $auxADtempfile"
	echo $ADcommand 
	$ADcommand
	errorCheck

	ADcommand="$ADdir/analisi/fit/fit_tracks -i $auxADtempfile -o $outADrootfile"
	echo $ADcommand 
	$ADcommand
	errorCheck
fi

if [ "$calocube" == "true" ] && [ "$adamo" == "true" ] ; then
	mkdir -p $CCoutdir/combined
	command="hadd -f $CCoutdir/combined/run${data}.root $outADrootfile $outCCfile"
    	echo $command 
	$command
	errorCheck
fi
