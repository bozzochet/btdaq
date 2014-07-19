#! /bin/sh
# stupied loop to rename files

PREFIX=$1

for ((i=1; i<10; i++));
  do
  cp 0$i.jpg $PREFIX\0$i.jpg
done

for ((i=10; i<99; i++));
  do
  cp $i.jpg $PREFIX$i.jpg
done
