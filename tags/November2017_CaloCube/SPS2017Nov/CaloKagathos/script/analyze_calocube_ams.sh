#!/bin/bash
usage() { echo "Usage: $0 [OPTIONS]
    COMMON OPTIONS :
	-d 	input folder (mandatory)
	-p      output folder (mandatory)

	-a	analyze only ams
	-c	analyze only calocube
" 1>&2; exit 1; }

errorCheck() { if [ $? -gt 0 ]; then
    exit 1;
    fi
}

get_abs_sec(){
	file=$1
	date_hour=$(basename $file)
        date_hour=${date_hour#AD}
        date_hour=${date_hour#CC}
        date_hour=${date_hour%.ped}               
        date_hour=${date_hour%.dat}        
			
	ymd=${date_hour:0:8}
	h=${date_hour:9:2}
	m=${date_hour:11:2}
	s=${date_hour:13:2}

	abs_sec=$(date -d "$ymd $h:$m:$s" +%s)
	echo $abs_sec
}

normal=true
ams_folder=""

ams=true
calocube=true

TEMP=`getopt -o d:p:ach \
    -n $0 -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

eval set -- "$TEMP"

while true ; do
    case "$1" in
        -d) 
            case "$2" in
		"") usage ;;
		*) indir=${2}
		    shift 2 ;;
            esac ;;
        -p) 
            case "$2" in
                "") usage ;;
                *) outdir=${2}	
	    	    shift 2 ;;
            esac ;;
        -a) 
            case "$2" in
                *) calocube=false
		    shift 1 ;;
            esac ;; 
        -c) 
            case "$2" in
                *) ams=false
		    shift 1 ;;
            esac ;;             
	-h)
	   usage ;; 
        --) shift ; break ;;
        *) usage ;;
    esac
done

sleep_time=1 
cc_number=0
ams_number=0

while true
do

	echo "Check if there is some new file to process..."

	for file in $indir/*;
	do
		if [[ $file == CC*dat ]]; then
	   	        cc_in_name=$(basename $file)
     		        ams_in_name=${input_name#.CC}
				
			cc_out_name=${cc_in_name%.dat}.root	
			ams_out_name=${ams_in_name%.dat}.root	

			if $calocube ; then	
			    if [ ! -f $outdir/$cc_out_name ]; then
				let "cc_number += 1"

				if $normal ; then
					echo "Processing file n°$cc_number = $indir/$cc_in_name"
			    		com="./analyze $indir/$cc_in_name $outdir/$cc_out_name"
		    			echo "$com"
		    			$com					
				else
					ref_sec=$(get_abs_sec "$cc_in_name")

    					diff_sec=1000000
				        file_pedestal=""

					for elif in $indir/*;
					do
					   if [[ $elif == *ped ]]; then
	   				        elif_name=$(basename $elif)
	   				        cmp_sec=$(get_abs_sec "$elif_name")
						
						if [ $((cmp_sec - ref_sec)) -gt 0 ]; then
							cmp_diff=$((cmp_sec - ref_sec))
						else
							cmp_diff=$((ref_sec - cmp_sec))
						fi

						if [ "$cmp_diff" -lt "$diff_sec" ]; then
							file_pedestal=$elif_name
							diff_sec=$cmp_diff
						fi
					   fi
					done

	    			        elif_out_name=${cc_name%.dat}.root	

					echo "Processing file n°$cc_number = $indir/$cc_in_name"
					echo -e "\twith  $indir/$elif_name"

			    		com="./analyze $indir/$elif_name $outdir/$elif_out_name"
		    			echo "$com"
		    			$com	
			    		com="./analyze $indir/$cc_in_name -c $outdir/$elif_out_name"
		    			echo "$com"
		    			$com					
				fi
			   fi
			fi

			   if $ams ; then
			   	if [ ! -f $outdir/$ams_out_name ]; then
			   	 	let "ams_number += 1"

					ams_link=$(readlink $indir/$ams_in_name)
					ams_number=${ams_link%.dat}	

					echo "Processing file n°$ams_number = $indir/$ams_in_name"
        				com=" ./Decode $ams_number --rootdata $outdir --rawdata ${ams_folder}/Data --caldata ${ams_folder}/Calibrations/"
		    			echo "$com"
		    			$com			
				fi
			   fi

			   if [[ $calocube && $ams ]]; then
			   	if [[ -f $outdir/$cc_out_name && -f $outdir/$ams_out_name ]]; then
					command="hadd -f $outdir/combined_$ams_out_name $outdir/$cc_out_name $outdir/$ams_out_name"
			   	 	echo $command 
					$command
					errorCheck
		    			echo "Plotting file n°$cc_number = $outdir/$cc_out_name"
			   	 	com="./draw -i $outdir/$cc_out_name -p -d -M -g"
		    			echo "$com"
		    			$com
				fi
			   fi
		fi
	done

	if [[ $cc_number -eq 0 && $ams_number -eq 0 ]]; then
		echo "No recent file to be processed now"
	else 
		echo "Processed $cc_number Calocube and $ams_number AMS files"
	fi

	echo "Now sleeping $sleep_time minute"
	sleep ${sleep_time};

done
