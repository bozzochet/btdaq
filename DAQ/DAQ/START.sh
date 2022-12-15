#!/bin/bash

cd /home/testsys/amsdaq-code/trunkBT/DAQ/DAQ

nohup ./launch_acquisition.sh > log/log_nohup.txt 2>&1 &

sleep 5

exit