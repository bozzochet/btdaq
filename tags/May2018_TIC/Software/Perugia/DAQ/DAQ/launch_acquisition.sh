#!/bin/bash
nfile=$1
nevent=$2

rm -fr  launch_acq_log.txt
date >> launch_acq_log.txt
echo "Starting INIT: " >> launch_acq_log.txt
command="./TakeEPP 0 INIT"
echo $command
$command
date >> launch_acq_log.txt
echo "INIT is completed: " >> launch_acq_log.txt
sleep 5

for i in `seq 0 $nfile`; do
        date >> launch_acq_log.txt
        echo "Starting CALIB: " >> launch_acq_log.txt
	command="./TakeEPP 0 CALIB"
	echo $command
	#$command
        date >> launch_acq_log.txt
        echo "CALIB is completed: " >> launch_acq_log.txt
	sleep 30

        date >> launch_acq_log.txt
        echo "Starting TAKEDATA: " >> launch_acq_log.txt
	command="./TakeEPP 0 TAKEDATA $nevent"
	echo $command
	$command
        date >> launch_acq_log.txt
        echo "TAKEDATA is completed: " >> launch_acq_log.txt
	sleep 30
done
