#!/bin/bash

cd RawData
for i in `ls`; do
	mv -v $i ${i:4:10}.dat
done
cd ..
