#!/bin/bash
current_directory=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $current_directory
for i in `ls ./Calibrations/*.cal`; do
	real_name=`basename $i`
	#echo $real_name
	numeric_value=${real_name:0:10}
	real_root_name="./Calibrations/summary-00_"$numeric_value".cal.pdf";
	#echo $real_root_name
	exists=0
	if [ -e $real_root_name ]; then
	    exists=1
	fi
	if [ "$exists" -eq "0" ]; then
		echo "running on $real_name ..."
		./SUMMARY_MUTE $numeric_value &> /dev/null
	fi
done
