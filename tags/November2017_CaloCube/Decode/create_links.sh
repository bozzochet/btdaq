#!/bin/bash
cd RawData
for i in `ls`; do
	ln -fs $i ${i:4:10}.dat
done
cd ..
