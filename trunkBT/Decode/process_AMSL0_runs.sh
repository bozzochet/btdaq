#!/bin/bash

for i in `ls -d USBLF_PCGSC17/run_*`
do
    BASE=`basename $i`
    FIRST=`echo $BASE | awk -F- '{ print $1 }'`
    LAST=`echo $BASE | awk -F- '{ print $2 }'`
#    echo "$FIRST $LAST"
    FIRSTDIR=`echo $FIRST | awk -F_ '{ print $2 }'`
    FIRSTFILE=`echo $FIRST | awk -F_ '{ print $3 }'`
    LASTDIR=`echo $LAST | awk -F_ '{ print $1 }'`
    LASTFILE=`echo $LAST | awk -F_ '{ print $2 }'`
    echo "$BASE $FIRSTDIR $FIRSTFILE $LASTDIR $LASTFILE"
    ./AMSL0filelogger --datadir="USBLF_PCGSC17" --runstart ${FIRSTDIR}${FIRSTFILE} --runstop ${LASTDIR}${LASTFILE} > log_${BASE}.txt
done
