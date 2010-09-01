#!/bin/sh 

# 01/12/2009
# Substitute the defaul 2ea3 code with the one
# contained in file.dat (you have to rename it). 
# WARNING: be sure that the first line of file.dat
# is 3ea3. If is not, modify that.   

# At the end you obtain a new software loaded in the
# in all TDR setted as default with the same name
# of the previous one.

for jinf in `cat jinflist`; do
  for tdr in `cat tdrlist`; do
    address=${tdr}3f${jinf}3f
    # Delete the old code
    ./TESTjmdc 2 $address 47 2ea3
    sleep 1
    # Flash File Load (the one stored in file.dat)
    ./TESTjmdc 2 $address 45 3ea3
    sleep 1
    # Set default 
    ./TESTjmdc 2 $address 48 3ea3 1
    usleep 100000
    # Load the new code
    ./TESTjmdc 2 $address 46 2ea3 
    usleep 100000
  done
done 

