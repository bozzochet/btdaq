#!/bin/bash
# Automatic execution of AMS Decode.
# To be launched from the Software/Perugia/Decode/ folder

usage() { echo "Usage: $0
" 1>&2; exit 1; }

errorCheck() { if [ $? -gt 0 ]; then
    exit 1;
    fi
}

if [ $# -ne 0 ]; then
    usage;
fi

indir=$1       
outdir=$2       
sleep_time=30

filenumber=0

currDir=$PWD
inputDir=$currDir/RawData/
outputDir=$currDir/RootData/

while true
do

	echo "Check if there is some new file to process..."
	filenumber=0

	for file in $inputDir/*;
	do
		file_name=$(basename $file)
		if [[ $file_name == ??????????.dat ]]; then
		    unixTime=${file_name%.dat}
                    output_name=$outputDir/run_${unixTime}.root
		    dateRaw=`date -r $file +%s`
                    dateProcessed=`date -r $output_name +%s 2>/dev/null`
                    if [[ $? != 0 ]]; then
                      dateProcessed=0
                    fi
                    if [[ $dateRaw -gt $(($dateProcessed - 180)) ]]; then  # Reprocess if a data file is newer than its output file
		    	let "filenumber += 1"
			
			echo "Processing file n°$filenumber = $file"
		    	com="./Decode -c $unixTime"
	    		echo "$com"
	    		$com
	    	    fi
		fi
	done

	if [ $filenumber -eq 0 ]; then
		echo "No recent file to be processed now"
	else 
		echo "Processed $filenumber files"
	fi

	echo "Now sleeping $sleep_time minute"
	sleep ${sleep_time}m;

done
