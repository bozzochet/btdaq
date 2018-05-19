#!/bin/bash
usage() { echo "Usage: $0 [NUMBERofFile] [NUMBERofEVENTS]" 1>&2; exit 1; }

datediff() {
	d1=$(date -d "$1" +%s)
	d2=$(date -d "$2" +%s)
	echo $(( (d1 - d2) / 1 )) #sec
}

log_dir="log"
master_dir="."
#slave_dir="."
master_port=0
#slave_port=1
acq_exe="TakeEPP"
gen_log="launch_acq_log.txt"

if [ ! -d $log_dir ]; then
	echo "Log directory $log_dir not existing!"
	exit 1;
fi
if [ ! -d $master_dir ]; then
	echo "Master directory $master_dir not existing!"
	exit 1;
fi
#if [ ! -d $slave_dir ]; then
#	echo "Slave directory $slave_dir not existing!"
#	exit 1;
#fi

if [ $# -ne 2 ]; then
	usage
	exit 1;
fi 

nfile=$1
nevent=$2

date >> $gen_log
echo "Starting Master INIT: " >> $gen_log
cd $master_dir
command="$acq_exe $master_port INIT"
echo $command
$command
date >> $gen_log
echo "Master INIT is completed: " >> $gen_log
sleep 5

#date >> $gen_log
#echo "Starting Slave INIT: " >> $gen_log
#cd $slave_dir
#command="$acq_exe $slave_port INIT"
#echo $command
#$command
#date >> $gen_log
#echo "Slave INIT is completed: " >> $gen_log
#sleep 5

calibration_time=$(date -d "Thu Jan  1 01:00:00 CET 1970")

for i in `seq 0 $nfile`; do

	iteration_time=$(date)
	
	echo "Begin iteration $i at $iteration_time"
	nsec=$(datediff "$iteration_time" "$calibration_time")

	if [ $nsec -ge 3600 ]; then
		echo "Elapsed $nsec since last calibration: Calibration forced"

	        date >> $gen_log
	        echo "Starting Master CALIB: " >> $gen_log
		cd $master_dir
		command="$acq_exe $master_port CALIB 1 0"
		echo $command
		$command
	        date >> $gen_log
       		echo "Master CALIB is completed: " >> $gen_log
		sleep 30

#		date >> $gen_log
#	        echo "Starting Slave CALIB: " >> $gen_log
#		cd $slave_dir
#		command="$acq_exe $slave_port CALIB 1 0"
#		echo $command
#		$command
#      		date >> $gen_log
#		echo "Master Slave is completed: " >> $gen_log
#		sleep 30
		
		calibration_time=$(date)
	fi 

	unixtime=$(date +%s)

#	date >> $gen_log
#       echo "Starting Slave TAKEDATA: " >> $gen_log
#	cd $slave_dir
#	command="$acq_exe $slave_port TAKEDATA 1000000 0 0 $unixtime"
#	echo $command
#	$command |& tee $log_dir/slave_$unixtime.log &
#       sleep 30
#    
#	PID=`ps aux | grep $acq_exe | grep -v grep | awk '{ print $2 }'`
	
	cd $master_dir
	date >> $gen_log
        echo "Starting Master TAKEDATA: " >> $gen_log
	cd $master_dir
	command="$acq_exe $master_port TAKEDATA $nevent 1 0 $unixtime"
	echo $command
	$command |& tee $log_dir/master_$unixtime.log
        
	date >> $gen_log
        echo "Master TAKEDATA is completed: " >> $gen_log
	sleep 30

#       kill $PID
# 
#	date >> $gen_log
#       echo "Slave TAKEDATA is completed: " >> $gen_log
#	#should we wait time here?
done
