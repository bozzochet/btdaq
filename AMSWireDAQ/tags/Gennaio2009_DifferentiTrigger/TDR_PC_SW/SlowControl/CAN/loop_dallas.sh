#!/bin/csh

set Fname  = $1
set Nwait  = $2

set nn = 1

LOOP:
./dallas-1ts 1a1 > data/$Fname.1a1.dat_$nn
./dallas-1ts 1a2 > data/$Fname.1a2.dat_$nn

@ nn = $nn + 1

sleep $Nwait
goto LOOP

exit 0




