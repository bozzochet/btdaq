#!/bin/bash

cd /home/testsys/amsdaq-code/trunkBT/DAQ/DAQ

line=$(head -n 1 "./log/PID.txt")
kill -15 $line
