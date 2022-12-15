#!/bin/bash

MOTHERDIR="./Data"
SUBDIR="L0/BLOCKS/PG/USBL0_PG_LEFV2BEAM1/0000"

FIRST=627
LAST=999

FIRSTSTOP=750
SECONDSTOP=850

outfile="${MOTHERDIR}_hacked/$SUBDIR/${FIRST}_${LAST}"
echo $outfile
touch $outfile

for i in `seq $FIRST $LAST`
do
    ls -lh $MOTHERDIR/$SUBDIR/$i
    cat $MOTHERDIR/$SUBDIR/$i >> $outfile
    if [[ "$i" == "$FIRSTSTOP" ]]
    then
	echo "you can free the space of the files from $FIRST to $FIRSTSTOP"
	sleep 60
    elif [[ "$i" == "$SECONDSTOP" ]]
    then
	echo "you can free the space of the files from $FIRST to $SECONDSTOP"
	sleep 60
    fi
done
