#!/bin/bash
usage() { echo "Usage: $0 [DATADIR]
" 1>&2; exit 1; }

errorCheck() { if [ $? -gt 0 ]; then
    exit 1;
    fi
}

if [ $# -ne 1 ]; then
    usage;
fi

datadir=$1       
sleep_time=1 

filenumber=0

while true
do

	echo "Check if there is some new file to process..."

	for file in $datadir/*;
	do
		if [[ $file == CC*dat ]]; then
		    let "filenumber += 1"

		    input_name=$file
		    complete_path=$(basename $input_name)
		    output_name=${complete_path%.dat}.root	

		    if [ ! -f $output_name ]; then	
			echo "Processing file n°$filenumber = $file"
		    	com="./analyze $input_name $output_name"
	    		echo "$com"
	    		$com
	    		echo "Plotting file n°$filenumber = $output_name"
		    	com="./draw -i $output_name -p -d -M -g"
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
