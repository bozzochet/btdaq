#! /bin/sh
# Shell script for make a loop commanding
 
echo "Repeat $1 times of command: <$2>"
 
N=$1
CMD=$2
 
for ((i=1; i<=$N; i++));
  do
  echo $i / $N
  $CMD
done
