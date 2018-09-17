#!/bin/bash
# Automatic execution of CaloCube analyze
# To be launched from the Software/CaloKagathos/ folder

usage() { echo "Usage: $0 [inDIR] [outDIR]
" 1>&2; exit 1; }

errorCheck() { if [ $? -gt 0 ]; then
    exit 1;
    fi
}

if [ $# -ne 2 ]; then
    usage;
fi

indir=$1       
outdir=$2       
sleep_time=30

filenumber=0

while true
do

	echo "Check if there is some new file to process..."
	filenumber=0

	for file in $indir/*;
	do
		file_name=$(basename $file)
		if [[ $file_name == CC*_*.dat ]]; then
		    output_name=$outdir/${file_name%.dat}.root
                    dateRaw=`date -r $file +%s`
                    dateProcessed=`date -r $output_name +%s 2>/dev/null`
                    if [[ $? != 0 ]]; then
                      dateProcessed=0
                    fi		    
                    if [[ $dateRaw -gt $(($dateProcessed - 180)) ]]; then  # Reprocess if a data file is newer than its output file
		    #if [ ! -f $output_name ]]; then	
		    	let "filenumber += 1"
			
			echo "Processing file n°$filenumber = $file"
			#com="./analyze $input_name $output_name"
		    	com="./analyze $file $output_name"
	    		echo "$com"
	    		$com
			#echo "Plotting file n°$filenumber = $output_name"
			#com="./draw_alone -i $output_name -p -d -M -g"
			#echo "$com"
			#$com
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
