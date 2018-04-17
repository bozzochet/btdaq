#!/bin/bash

QUEUE="wizardq"
MACHINE="a44 a45 a51 a52 a61 a62 a63 a64"

usage() { echo "Usage: $0 [OPTIONS] 
    COMMON OPTIONS :
	-l 	input list data
	-r	input list pedestal

	-i	input directory
	-o	output directory

	-a	analyze only adamo
	-c	analyze only calocube
" 1>&2; exit 1; }

errorCheck() { if [ $? -gt 0 ]; then
    exit 1;
    fi
}

get_file_name(){
	  directory=$1
	  detector=$2
 	  line=$3
 	  suffix=$4

	  day=""
	  hour=""

	  counter=0

	  for word in $line
	  do
	    if [ $counter -eq 0 ]; then
		    day=$word
	    fi
	    if [ $counter -eq 1 ]; then
		    hour=$word
	    fi
	    counter=$((counter + 1));
	  done

	  if [ $counter -ne 2 ]; then
	    echo "$line $counter bbbbb" 1>&2;
	    echo "Incomplete file name: Exit..." 1>&2; exit 1;
 	  fi
  
	  echo "${directory}${detector}${day}_${hour}${suffix}"
}

get_abs_sec(){
	file=$1
	date_hour=$(basename $file)
        date_hour=${date_hour#AD}
        date_hour=${date_hour#CC}
        date_hour=${date_hour%.ped}
			
	ymd=${date_hour:0:8}
	h=${date_hour:9:2}
	m=${date_hour:11:2}
	s=${date_hour:13:2}

	abs_sec=$(date -d "$ymd $h:$m:$s" +%s)
	echo $abs_sec
}

ANALYSIS_DIR="/home/berti/calocube/SPS2017Aug/CaloKagathos/"
ANALYZE_SCRIPT="$ANALYSIS_DIR/analyze.sh"
CCdir="$ANALYSIS_DIR/CaloKagathos/"
ADdir="$ANALYSIS_DIR/ADAMO/"

list_data=""
list_pedestal=""
inDir=""
outDir=""

data="./data.dat"
pedestal="./pedestal.ped"
adamo="true"
calocube="true"

if [ $# -eq 0 ]; then
    usage;
fi

TEMP=`getopt -o l:r:i:o:ach \
    -n $0 -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

eval set -- "$TEMP"

while true ; do
    case "$1" in
        -l) 
            case "$2" in
		"") usage ;;
		*) list_data=${2}
		    shift 2 ;;
            esac ;;
        -r) 
            case "$2" in
		"") usage ;;
		*) list_pedestal=${2}
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
	usage
elif [[ -z "$outDir" ]]; then
	usage
elif [[ -z "$list_data" ]]; then
	usage
elif [[ -z "$list_pedestal" ]]; then
	usage
fi

echo -e "\n================================================================================="
echo -e "Submitting run from list $list_data"
echo -e "==============================================================\n"

detector=0

if [ "$calocube" == "true" ] ; then
	detector=$((detector + 1));
	echo "Analyzing Calocube"
fi
if [ "$adamo" == "true" ] ; then
	detector=$((detector + 2));
	echo "Analyzing ADAMO"
fi

if [ $detector -eq 0 ]; then
	echo "Error selecting detectors: Exit..."
	exit 1;
fi

if [ $detector -eq 1 ]; then
	option="-c"
elif [ $detector -eq 2 ]; then
	option="-a"
else
	option=""
fi

echo -e "\n=====================================\n"

counter=0
listDir=""
listLog=""

while IFS='' read -r line || [[ -n "$line" ]]; do
	if [ $counter -eq 0 ]; then
		listDir="$outDir/$line/"
		listLog="$outDir/$line/log"

		mkdir -p $listDir
		mkdir -p $listLog
	else
		if [ "$calocube" == "true" ] ; then
			file_name=$(get_file_name "$inDir" "CC" "$line" ".dat")
			if [ ! -f ${filename} ]; then
				echo -e "\n ${filename} not found! Exit... \n" 
				exit 1;
			fi
		fi
		if [ "$adamo" == "true" ] ; then
			file_name=$(get_file_name "$inDir" "AD" "$line" ".dat")
			if [ ! -f ${filename} ]; then
				echo -e "\n ${filename} not found! Exit... \n" 
				exit 1;
			fi
		fi

		array[$((counter - 1))]="${line}";
	fi
	
	counter=$((counter + 1));
done < "$list_data"

if [[ ! -d "$listDir" ]]; then
	echo "Null output directory : Exit..."
	exit 1;
fi
if [[ ! -d "$listLog" ]]; then
	echo "Null log directory : Exit..."
	exit 1;
fi

for index in ${!array[*]}
do
    item=${array[$index]}

    file_data=$(get_file_name "" "" "$item" "")
    name_data=$(get_file_name "$inDir" "CC" "$item" ".dat")
    ref_sec=$(get_abs_sec "$name_data")

    file_pedestal=""
    name_pedestal=""
    diff_sec=1000000

    while IFS='' read -r enil || [[ -n "$enil" ]]; do
	   inPath=$(get_file_name "$inDir" "AD" "$enil" ".ped")
	   if [[ $inPath == *ped ]]; then
	        cmp_sec=$(get_abs_sec "$inPath")
		if [ $((cmp_sec - ref_sec)) -gt 0 ]; then
			cmp_diff=$((cmp_sec - ref_sec))
		else
			cmp_diff=$((ref_sec - cmp_sec))
		fi

		if [ "$cmp_diff" -lt "$diff_sec" ]; then
			file_pedestal=$(get_file_name "" "" "$enil" "")
			name_pedestal=$inPath
			diff_sec=$cmp_diff
		fi
	   fi
     done < "$list_pedestal"

     printf "Submitting File %2d: %s\n" $index $file_data
     printf "\twith Pedestal %s\n" $file_pedestal
     
     logfile=$listLog/${file_data%.dat}.log
     bsub -q $QUEUE -m "$MACHINE" -o $logfile "$ANALYZE_SCRIPT -d $file_data -p $file_pedestal -i $inDir -o $listDir $option"
     echo
done
