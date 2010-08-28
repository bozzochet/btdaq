#!/bin/sh

if [ $# -lt 3 ]; then
    echo "You must indicate the crate to use [0...7]"
    echo "You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T] for the JINF-A (1 or 2)"
    echo "You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T] for the JINF-B (0 or 3)"
    exit
fi

crate=$1
apart=$2
bpart=$3

if [[ $crate = "3" ]]
then
    address="013f"
elif [[ $crate = "2" ]]
then
    address="003f"
elif [[ $crate = "7" ]]
then
    address="173f"
elif [[ $crate = "6" ]]
then
    address="163f"
elif [[ $crate = "0" ]]
then
    address="033f"
elif [[ $crate = "1" ]]
then
    address="093f"
elif [[ $crate = "4" ]]
then
    address="103f"
elif [[ $crate = "5" ]]
then
    address="113f"
fi

./TESTjmdc $apart $address 40                                  # Boot the tracker JINFs Primary
sleep 1
./TESTjmdc $bpart $address 40                                  # Boot the tracker JINFs Secondary
sleep 1
./TESTjmdc $apart $address 46 29F2                             # Load the program
sleep 1
./TESTjmdc $bpart $address 46 29F2                             # Load the program Secondary
sleep 1
./TESTjmdc $apart $address ffff 1D 0001 0000 0040 E18A 0065    # switch off 9053_1 hot
sleep 1
./TESTjmdc $apart $address ffff 1D 0001 0000 0040 E18A 00A5    # switch on 9053_1 cold
sleep 1
./TESTjmdc $apart $address ffff 1D 0001 0000 0050 E18A 00A5    # copy conf to Actel brother
sleep 1
./TESTjmdc $apart $address 46 7011                             # Load the parameters 
sleep 1
./TESTjmdc $bpart $address ffff 1D 0001 0000 0050 E18A 00A6    # switch off 9053_0 hot
sleep 1
./TESTjmdc $bpart $address ffff 1D 0001 0000 0050 E18A 00AA    # switch on 9053_0 cold
sleep 1
./TESTjmdc $bpart $address ffff 1D 0001 0000 0040 E18A 00AA    # copy conf to Actel brother
sleep 1
./TESTjmdc $bpart $address 46 7011                             # Load the parameters 
sleep 1
./TESTjmdc $bpart $address 54 1003                             # Apply parameters to power supplies
sleep 1
./TESTjmdc $bpart $address 5A 0000                             # Switch off brother