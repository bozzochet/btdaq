#!/bin/bash

cd /home/testsys/amsdaq-code/trunkBT/DAQ/DAQ

rm -fr  ./log/launch_acq_log.txt
date >> ./log/launch_acq_log.txt
rm -fr ./log/PID.txt

        echo "Starting TAKEDATA complete run " >> ./log/launch_acq_log.txt
	command="./TakeEPP 0 COMPLETERUN"
	echo $command
	$command &
	echo $! >> ./log/PID.txt
