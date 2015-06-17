#!/bin/bash
current_directory=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $current_directory
for i in `ls ./Calibrations/*02_ANC_*.cal`; do
	real_name=${i:13}
	numeric_value=${real_name:0:10}
	current_string=${real_name%.*}
	ancillary_value=${current_string:20}
	real_root_names[0]="./Calibrations/summary-00_"$numeric_value"_ANC_"$ancillary_value".cal.pdf";
	real_root_names[1]="./Calibrations/summary-01_"$numeric_value"_ANC_"$ancillary_value".cal.pdf";
	exists=0
	for real_root_name in ${real_root_names[@]}; do
		if [ -e $real_root_name ]; then
			exists=1
			break
		fi
	done
	if [ "$exists" -eq "0" ]; then
		echo "running on $real_name ..."
		./SUMMARY $numeric_value $ancillary_value &> /dev/null
	fi
done
