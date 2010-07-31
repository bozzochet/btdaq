#!/bin/sh

if [ $# -lt 2 ]; then
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T] for the JINF-A (1 or 2)
    echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T] for the JINF-B (0 or 3)
    exit
fi

apart=$1
bpart=$2

./TESTjmdc $apart 033f 40                             # Boot the tracker JINFs Primary
sleep 1
./TESTjmdc $bpart 033f 40                             # Boot the tracker JINFs Secondary
sleep 1
./TESTjmdc $apart 033f 46 29F2                        # Load the program
sleep 1
./TESTjmdc $bpart 033f 46 29F2                        # Load the program Secondary
sleep 1
./TESTjmdc $apart 033f ffff 1D 0001 0000 0040 E18A 0065    # switch off 9053_1 hot
sleep 1
./TESTjmdc $apart 033f ffff 1D 0001 0000 0040 E18A 00A5    # switch on 9053_1 cold
sleep 1
./TESTjmdc $apart 033f ffff 1D 0001 0000 0050 E18A 00A5    # copy conf to Actel brother
sleep 1
./TESTjmdc $apart 033f 46 7011                        # Load the parameters 
sleep 1
./TESTjmdc $bpart 033f ffff 1D 0001 0000 0050 E18A 00A6    # switch off 9053_0 hot
sleep 1
./TESTjmdc $bpart 033f ffff 1D 0001 0000 0050 E18A 00AA    # switch on 9053_0 cold
sleep 1
./TESTjmdc $bpart 033f ffff 1D 0001 0000 0040 E18A 00AA    # copy conf to Actel brother
sleep 1
./TESTjmdc $bpart 033f 46 7011                        # Load the parameters 
sleep 1
./TESTjmdc $bpart 033f 54 1003                        # Apply parameters to power supplies
sleep 1
./TESTjmdc $bpart 033f 5A 0000                        # Switch off brother