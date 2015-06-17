#!/bin/bash
current_directory=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $current_directory
for i in `ls -1rt RawData`; do
	numeric_value=${i:0:10}
	current_string=${i%.*}
	ancillary_value=${current_string:15}
	real_root_names[0]="./RootData/run_$current_string.root";
	exists=0
	for real_root_name in ${real_root_names[@]}; do
		if [ -e $real_root_name ]; then
			exists=1
			break
		fi
	done
	if [ "$exists" -eq "0" ]; then
		echo "running on $i ..."
		./Decode $numeric_value $ancillary_value
	fi
done
