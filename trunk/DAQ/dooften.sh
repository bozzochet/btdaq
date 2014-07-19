
i=0
while [ $i -lt 10000 ];  do
  #echo $i
  OUTPUT=`./TESTjmdc 2 0d3f103f c | grep crc`
  set $OUTPUT
  #echo $3 $6
  if [ $3 != $6 ]; then
    echo $OUTPUT
  fi
  ((i++)) 
done

