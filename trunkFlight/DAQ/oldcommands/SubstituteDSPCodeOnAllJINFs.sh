#!/bin/sh -x

# WARNING: NEVER TRIED

codename=3952 
oldcodename=2922

for jinf in `cat jinflist`; do
  address=${jinf}3f
  # Switch on Brothers
  echo ./TESTjmdc 2 $address 5A 1 
  usleep 100000
  # Delete the old code (2922)
  echo ./TESTjmdc 2 $address 47 $oldcodename
  sleep 1
  echo ./TESTjmdc 3 $address 47 $oldcodename
  sleep 1
  # Flash File Load of the new code (2e...)
  echo ./TESTjmdc 2 $address 45 $codename
  sleep 1
  echo ./TESTjmdc 3 $address 45 $codename
  sleep 1
  # Set default bit
  echo ./TESTjmdc 2 $address 48 $codename 1
  usleep 100000
  echo ./TESTjmdc 3 $address 48 $codename 1
  usleep 100000
  # Load code 
  echo ./TESTjmdc 2 $address 46 2${codename:1:3}
  usleep 100000
  echo ./TESTjmdc 3 $address 46 2${codename:1:3}
  usleep 100000
  # Switch off Brothers
  echo ./TESTjmdc 2 $address 5A 0
  usleep 100000
done 

