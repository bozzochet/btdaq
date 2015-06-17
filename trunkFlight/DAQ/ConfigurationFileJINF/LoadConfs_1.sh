#!/bin/bash
echo You will use JINJ 0 and JINJ 2 to load on JINFs A and on JINFs B
sleep 5
CMDERRORS=0
WRTERRORS=0
ERSERRORS=0
OUTPUT=`./TESTjmdc 0 093f 7 | grep 7021`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc 0 093f 47 7021 | grep rxdone`
  usleep 200000
  echo Erase Flash on node 0 093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc 0 093f 7 | grep 7021`
  if [ -n "$OUTPUT" ]
    then
    echo DSP Code 7021 Not Erased on node 0 093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 7021 not found on node 0 093f, so no erasing is needed
fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_1.dat ./file.dat
OUTPUT=`./TESTjmdc 0 093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0 093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]
 then
  let "CMDERRORS=$CMDERRORS+1"
fi
OUTPUT=`./TESTjmdc 0 093f 7 | grep 7021`
if [ -z "$OUTPUT" ]
 then
  echo DSP Code 7021 Not Loaded on node 0 093f!!!
  let "WRTERRORS=$WRTERRORS+1"
fi
OUTPUT=`./TESTjmdc 2 093f 7 | grep 7021`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc 2 093f 47 7021 | grep rxdone`
  usleep 200000
  echo Erase Flash on node 2 093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc 2 093f 7 | grep 7021`
  if [ -n "$OUTPUT" ]
    then
    echo DSP Code 7021 Not Erased on node 2 093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 7021 not found on node 2 093f, so no erasing is needed
fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_1.dat ./file.dat
OUTPUT=`./TESTjmdc 2 093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 2 093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]
 then
  let "CMDERRORS=$CMDERRORS+1"
fi
OUTPUT=`./TESTjmdc 2 093f 7 | grep 7021`
if [ -z "$OUTPUT" ]
 then
  echo DSP Code 7021 Not Loaded on node 2 093f!!!
  let "WRTERRORS=$WRTERRORS+1"
fi
echo There were $CMDERRORS erros in sending command!!!!
echo There were $ERSERRORS erros in erasing DSP files!!!!
echo There were $WRTERRORS erros in writing DSP files!!!!
