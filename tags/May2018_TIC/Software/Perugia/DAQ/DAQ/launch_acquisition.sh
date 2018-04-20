#!/bin/bash
nfile=$1
nevent=$2

command="./TakeEPP 0 INIT"
echo $command
$command
sleep 5

for i in `seq 0 $nfile`; do
	command="./TakeEPP 0 CALIB"
	echo $command
	$command
	sleep 5

	command="./TakeEPP 0 TAKEDATA $nevent"
	echo $command
	$command
	sleep 5
done
