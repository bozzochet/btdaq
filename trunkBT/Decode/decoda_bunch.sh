#!/bin/bash

containsElement () {
    local e match="$1"
    shift
    for e; do [[ "$e" == "$match" ]] && return 0; done
    return 1
}

RAWPATH="~/NASAMSPGZYXEL_exports/BTData/202110_HERD_BT/rawdata"

declare -a array=( "41" "42" "43" "44" "45" "46" "47" "48" "49" "50" "53" "55" "56" )

for i in `seq 39 74`
do
    containsElement $i "${array[@]}"
    retVal=$?
    if [ $retVal -ne 0 ]
    then
	eval "./Decode --rawdata ${RAWPATH} -c --oca ${i}"
    fi
done
