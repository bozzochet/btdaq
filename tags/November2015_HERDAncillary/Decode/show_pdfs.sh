#!/bin/bash

current_directory=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $current_directory
#echo $current_directory

evince ./last_upstream.pdf &
evince ./last_downstream.pdf &

declare -a array=( upstream downstream )

while [ 1 ]
do
    for SUFFIX in ${array[@]}
    do
	echo $SUFFIX
	ln -sfv RootData_$SUFFIX/`ls -Flarth RootData_$SUFFIX/ | grep pdf | tail -1 | awk '{ print $9 }'` ./last_$SUFFIX.pdf
    done
    sleep 120
done