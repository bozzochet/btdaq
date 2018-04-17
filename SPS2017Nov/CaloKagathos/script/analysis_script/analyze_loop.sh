#!/bin/bash
usage() { echo "Usage: $0 [DATADIR] [PEDFILE] 
" 1>&2; exit 1; }

errorCheck() { if [ $? -gt 0 ]; then
    exit 1;
    fi
}

if [ $# -ne 2 ]; then
    usage;
fi

datadir=$1       
pedfile=$2

for f in $datadir/*;
do
	    
	inPath=$f
	
#	if [[ $inPath == *dat ]]; then
#	    let "filenumber += 1"
#
#	    echo "Processing file n°$filenumber = $f"
#
#	    date=$(basename $inPath)
#	    date=${date%.dat}	
# 
#	    pedfile=$(basename $pedfile)
#	    pedfile=${pedfile%.dat}	
#	    
#            echo
#	    com="./analyze.sh -d $date -p $pedfile -c"
#	    echo "$com"
#	    $com
#	    #errorCheck
#	    echo
#	fi
	if [[ $inPath == *AD*dat ]]; then
	    let "filenumber += 1"

	    echo "Processing file n°$filenumber = $f"

	    date=$(basename $inPath)
	    date=${date#AD}	
	    date=${date%.dat}	
 
	    pedfile=$(basename $pedfile)
	    pedfile=${pedfile#AD}	
	    pedfile=${pedfile%.dat}	
	    
            echo
	    com="./analyze.sh -d $date -p $pedfile -a"
	    echo "$com"
	    $com
	    #errorCheck
	    echo
	fi
#	if [[ $inPath == *AD*dat ]]; then
#	    let "filenumber += 1"
#
#	    echo "Processing file n°$filenumber = $f"
#
#	    date=$(basename $inPath)
#	    date=${dateAD}	
#	    date=${date%.dat}	
# 
#	    pedfile=$(basename $pedfile)
#	    pedfile=${pedfileAD}	
#	    pedfile=${pedfile%.dat}	
#	    
#            echo
#	    com="./analyze.sh -d $date -p $pedfile"
#	    echo "$com"
#	    $com
#	    #errorCheck
#	    echo
#	fi

done


