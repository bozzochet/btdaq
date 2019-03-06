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

if [ $# -eq 0 ]; then
    usage;
fi

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

cc_folder="/home/calocube/SPS2017Nov/CaloKagathos/"
ams_folder="/home/calocube/SPS2017Nov/Perugia/DAQ/DAQ/RootData/"
sha_folder="$cc_folder/sha/"

normal=true
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

sleep_time=5 

while true
do

	echo "Check if there is some new file to process..."
	cc_number=0

	for file in $indir/*;
	do
		if [[ $(basename $file) == CC*_*dat ]]; then

			today=$(date +%Y%m%d)
			if [[ ! -z "${file##*$today*}" ]]; then
				continue
			fi

	   	        cc_in_name=$(basename $file)
     		        ams_in_name=${cc_in_name#CC}
				
			cc_out_name=${cc_in_name%.dat}.root	
			ams_out_name=${ams_in_name%.dat}.root	

			rehadd=""
			if [[ $calocube == true ]] ; then	
		  		 sha_cc_in=$(cat $sha_folder/$cc_out_name.sha)
				 sha_cc_out=$(sha256sum $indir/$cc_in_name)
				 if [ ! -f $sha_folder/$cc_out_name.sha ]; then
					let "cc_number += 1"

					echo "Processing file n°$cc_number = $indir/$cc_in_name"
					com="./analyze $indir/$cc_in_name $outdir/$cc_out_name"
					echo "$com"
					$com
					rehadd=true;
				 else
					 if [ "$sha_cc_in" != "$sha_cc_out" ]; then
						let "cc_number += 1"

						echo "Processing file n°$cc_number = $indir/$cc_in_name"
				    		com="./analyze $indir/$cc_in_name $outdir/$cc_out_name"
		    				echo "$com"
						$com
						rehadd=true;
				  	 fi
   				fi
			 	echo "$sha_cc_out" > $sha_folder/$cc_out_name.sha
				#echo "Created file $sha_folder/$cc_out_name.sha"
			fi
			ams_link=$(readlink $indir/$ams_in_name)	
			ams_number=${ams_link%.dat}
			if [[ $ams == true ]] ; then
		  		sha_ams_in=$(cat $sha_folder/run_${ams_number}.sha)
				sha_ams_out=$(sha256sum $ams_folder/run_${ams_number}.root)
				if [ "$sha_ams_in" != "$sha_ams_out" ]; then
					rehadd=true;	
				fi
			 	echo "$sha_ams_out" > $sha_folder/run_${ams_number}.sha
				#echo "Created file $sha_folder/run_${ams_number}.sha"
			   	if [[ -f $outdir/$cc_out_name && -f $ams_folder/run_${ams_number}.root ]]; then
					#if [ ! -f $outdir/combined_$ams_out_name ]; then
						if [[ $rehadd == true ]] ; then	
							com="hadd -f $outdir/combined_$ams_out_name $outdir/$cc_out_name $ams_folder/run_${ams_number}.root"
			   	 			echo $com 
							$com
							#echo "Plotting file n°$cc_number = $outdir/$cc_out_name"
							#com="./draw -i $outdir/$cc_out_name -p -l -A -1000 -Z 1000 -d -R -a 0 -z 50000 -v -V -g"
		    					echo "Plotting file n°$cc_number = $outdir/combined_$ams_out_name"
							com="./draw -i $outdir/combined_$ams_out_name -p -l -A -1000 -Z 1000 -d -R -a 0 -z 50000 -v -V -g"
		    					echo "$com"
		    					$com
						fi
#					fi
				fi
			fi
		fi
	done

	if [[ $cc_number -eq 0 && $ams_number -eq 0 ]]; then
		echo "No recent file to be processed now"
	else 
		echo "Processed $cc_number files"
	fi

	echo "Now sleeping $sleep_time minute"
	sleep ${sleep_time}m;

done
