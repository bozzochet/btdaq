#!/bin/bash

containsElement () {
    local e match="$1"
    shift
    for e; do [[ "$e" == "$match" ]] && return 0; done
    return 1
}

RAWPATH="~/NASAMSPGZYXEL_exports/BTData/202110_HERD_BT/rawdata"

#declare -a array=( "41" "42" "43" "44" "45" "46" "47" "48" "49" "50" "53" "55" "56" ) #muons
declare -a array=( "245" "243" "241" "239" "237" "235" "233" "230" "228" "226" "225" "224" "223" "221" "218" "205" "203" "201" "199" "197" "195" "194" "189" "183" "181" "179" "177" "176" ) #protons

#for i in `seq 39 74`
#for i in `seq 175 247`
for i in `seq 175 210`
do
    containsElement $i "${array[@]}"
    retVal=$?
    if [ $retVal -ne 0 ]
    then
	TRECIFRE=`printf %03d $i`
	FILE="RootData/run_000${TRECIFRE}.root"
	if [ -e $FILE ]
	then
	    SIZE=`ls -l $FILE | awk '{ print $5 }'`
	    SIZEH=`ls -lh $FILE | awk '{ print $5 }'`
#	    echo $SIZE
	    if [[ $SIZE -gt 100000000 ]]
	    then
		echo "$FILE: good file $SIZEH"
		TODECODE=0
	    else
		echo "$FILE: not-good file $SIZEH"
		TODECODE=1
	    fi
	else
	    TODECODE=1
	fi
	if [[ $TODECODE == 1 ]]
	then
	    eval "./Decode --rawdata ${RAWPATH} -c --oca ${i}"
	fi
    fi
done
