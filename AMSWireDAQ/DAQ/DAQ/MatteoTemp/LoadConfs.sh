#!/bin/bash
if [ $# -lt 1 ]
 then
  echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]
  exit
fi
port=$1
CMDERRORS=0
WRTERRORS=0
ERSERRORS=0
OUTPUT=`./TESTjmdc $port 003f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 003f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 003f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 003f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 003f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 003f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 013f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 013f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 013f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 013f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 013f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 013f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 023f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 023f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 023f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 023f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 023f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 023f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 033f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 033f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 033f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 033f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 033f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 033f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 043f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 043f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 043f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 043f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 043f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 043f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 053f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 053f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 053f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 053f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 053f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 053f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 063f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 063f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 063f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 063f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 063f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 063f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 073f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 073f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 073f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 073f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 073f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 073f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 083f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 083f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 083f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 083f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 083f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 083f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 093f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 093f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 093f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 093f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 093f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 093f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0a3f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0a3f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0a3f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0a3f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0a3f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0a3f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0b3f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0b3f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0b3f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0b3f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0b3f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0b3f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0c3f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0c3f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0c3f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0c3f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0c3f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0c3f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0d3f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0d3f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0d3f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0d3f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0d3f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0d3f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0e3f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0e3f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0e3f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0e3f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0e3f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0e3f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0f3f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0f3f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0f3f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0f3f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0f3f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0f3f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 103f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 103f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 103f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 103f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 103f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 103f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 113f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 113f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 113f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 113f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 113f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 113f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 123f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 123f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 123f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 123f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 123f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 123f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 133f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 133f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 133f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 133f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 133f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 133f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 143f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 143f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 143f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 143f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 143f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 143f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 153f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 153f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 153f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 153f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 153f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 153f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 163f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 163f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 163f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 163f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 163f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 163f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 173f033f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 173f033f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 173f033f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 173f033f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 173f033f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 173f033f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 003f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 003f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 003f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 003f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 003f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 003f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 013f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 013f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 013f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 013f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 013f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 013f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 023f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 023f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 023f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 023f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 023f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 023f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 033f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 033f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 033f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 033f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 033f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 033f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 043f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 043f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 043f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 043f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 043f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 043f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 053f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 053f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 053f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 053f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 053f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 053f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 063f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 063f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 063f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 063f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 063f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 063f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 073f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 073f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 073f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 073f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 073f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 073f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 083f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 083f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 083f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 083f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 083f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 083f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 093f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 093f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 093f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 093f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 093f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 093f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0a3f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0a3f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0a3f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0a3f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0a3f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0a3f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0b3f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0b3f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0b3f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0b3f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0b3f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0b3f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0c3f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0c3f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0c3f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0c3f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0c3f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0c3f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0d3f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0d3f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0d3f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0d3f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0d3f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0d3f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0e3f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0e3f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0e3f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0e3f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0e3f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0e3f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0f3f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0f3f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0f3f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0f3f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0f3f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0f3f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 103f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 103f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 103f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 103f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 103f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 103f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 113f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 113f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 113f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 113f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 113f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 113f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 123f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 123f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 123f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 123f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 123f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 123f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 133f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 133f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 133f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 133f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 133f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 133f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 143f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 143f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 143f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 143f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 143f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 143f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 153f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 153f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 153f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 153f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 153f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 153f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 163f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 163f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 163f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 163f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 163f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 163f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 173f093f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 173f093f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 173f093f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 173f093f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 173f093f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 173f093f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 003f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 003f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 003f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 003f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 003f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 003f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 013f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 013f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 013f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 013f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 013f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 013f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 023f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 023f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 023f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 023f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 023f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 023f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 033f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 033f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 033f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 033f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 033f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 033f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 043f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 043f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 043f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 043f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 043f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 043f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 053f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 053f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 053f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 053f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 053f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 053f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 063f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 063f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 063f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 063f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 063f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 063f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 073f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 073f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 073f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 073f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 073f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 073f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 083f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 083f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 083f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 083f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 083f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 083f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 093f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 093f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 093f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 093f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 093f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 093f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0a3f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0a3f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0a3f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0a3f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0a3f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0a3f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0b3f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0b3f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0b3f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0b3f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0b3f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0b3f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0c3f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0c3f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0c3f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0c3f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0c3f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0c3f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0d3f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0d3f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0d3f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0d3f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0d3f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0d3f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0e3f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0e3f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0e3f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0e3f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0e3f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0e3f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0f3f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0f3f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0f3f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0f3f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0f3f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0f3f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 103f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 103f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 103f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 103f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 103f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 103f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 113f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 113f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 113f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 113f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 113f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 113f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 123f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 123f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 123f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 123f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 123f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 123f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 133f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 133f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 133f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 133f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 133f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 133f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 143f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 143f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 143f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 143f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 143f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 143f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 153f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 153f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 153f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 153f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 153f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 153f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 163f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 163f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 163f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 163f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 163f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 163f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 173f003f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 173f003f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 173f003f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 173f003f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 173f003f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 173f003f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 003f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 003f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 003f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 003f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 003f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 003f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 013f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 013f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 013f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 013f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 013f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 013f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 023f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 023f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 023f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 023f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 023f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 023f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 033f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 033f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 033f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 033f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 033f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 033f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 043f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 043f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 043f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 043f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 043f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 043f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 053f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 053f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 053f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 053f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 053f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 053f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 063f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 063f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 063f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 063f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 063f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 063f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 073f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 073f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 073f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 073f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 073f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 073f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 083f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 083f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 083f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 083f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 083f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 083f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 093f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 093f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 093f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 093f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 093f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 093f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0a3f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0a3f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0a3f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0a3f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0a3f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0a3f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0b3f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0b3f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0b3f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0b3f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0b3f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0b3f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0c3f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0c3f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0c3f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0c3f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0c3f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0c3f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0d3f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0d3f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0d3f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0d3f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0d3f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0d3f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0e3f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0e3f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0e3f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0e3f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0e3f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0e3f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0f3f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0f3f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0f3f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0f3f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0f3f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0f3f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 103f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 103f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 103f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 103f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 103f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 103f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 113f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 113f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 113f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 113f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 113f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 113f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 123f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 123f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 123f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 123f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 123f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 123f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 133f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 133f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 133f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 133f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 133f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 133f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 143f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 143f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 143f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 143f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 143f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 143f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 153f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 153f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 153f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 153f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 153f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 153f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 163f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 163f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 163f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 163f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 163f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 163f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 173f013f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 173f013f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 173f013f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 173f013f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 173f013f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 173f013f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 003f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 003f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 003f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 003f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 003f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 003f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 013f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 013f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 013f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 013f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 013f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 013f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 023f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 023f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 023f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 023f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 023f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 023f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 033f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 033f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 033f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 033f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 033f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 033f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 043f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 043f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 043f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 043f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 043f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 043f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 053f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 053f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 053f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 053f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 053f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 053f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 063f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 063f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 063f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 063f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 063f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 063f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 073f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 073f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 073f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 073f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 073f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 073f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 083f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 083f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 083f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 083f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 083f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 083f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 093f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 093f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 093f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 093f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 093f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 093f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0a3f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0a3f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0a3f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0a3f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0a3f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0a3f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0b3f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0b3f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0b3f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0b3f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0b3f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0b3f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0c3f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0c3f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0c3f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0c3f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0c3f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0c3f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0d3f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0d3f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0d3f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0d3f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0d3f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0d3f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0e3f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0e3f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0e3f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0e3f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0e3f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0e3f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0f3f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0f3f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0f3f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0f3f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0f3f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0f3f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 103f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 103f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 103f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 103f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 103f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 103f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 113f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 113f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 113f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 113f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 113f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 113f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 123f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 123f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 123f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 123f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 123f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 123f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 133f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 133f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 133f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 133f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 133f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 133f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 143f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 143f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 143f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 143f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 143f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 143f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 153f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 153f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 153f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 153f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 153f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 153f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 163f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 163f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 163f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 163f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 163f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 163f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 173f103f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 173f103f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 173f103f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 173f103f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 173f103f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 173f103f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 003f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 003f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 003f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 003f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 003f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 003f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 013f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 013f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 013f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 013f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 013f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 013f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 023f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 023f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 023f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 023f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 023f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 023f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 033f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 033f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 033f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 033f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 033f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 033f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 043f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 043f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 043f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 043f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 043f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 043f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 053f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 053f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 053f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 053f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 053f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 053f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 063f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 063f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 063f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 063f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 063f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 063f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 073f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 073f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 073f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 073f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 073f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 073f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 083f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 083f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 083f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 083f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 083f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 083f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 093f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 093f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 093f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 093f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 093f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 093f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0a3f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0a3f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0a3f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0a3f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0a3f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0a3f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0b3f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0b3f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0b3f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0b3f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0b3f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0b3f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0c3f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0c3f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0c3f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0c3f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0c3f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0c3f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0d3f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0d3f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0d3f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0d3f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0d3f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0d3f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0e3f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0e3f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0e3f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0e3f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0e3f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0e3f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0f3f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0f3f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0f3f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0f3f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0f3f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0f3f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 103f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 103f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 103f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 103f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 103f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 103f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 113f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 113f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 113f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 113f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 113f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 113f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 123f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 123f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 123f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 123f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 123f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 123f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 133f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 133f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 133f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 133f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 133f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 133f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 143f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 143f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 143f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 143f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 143f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 143f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 153f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 153f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 153f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 153f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 153f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 153f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 163f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 163f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 163f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 163f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 163f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 163f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 173f113f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 173f113f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 173f113f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 173f113f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 173f113f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 173f113f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 003f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 003f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 003f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 003f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 003f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 003f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 013f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 013f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 013f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 013f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 013f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 013f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 023f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 023f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 023f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 023f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 023f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 023f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 033f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 033f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 033f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 033f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 033f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 033f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 043f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 043f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 043f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 043f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 043f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 043f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 053f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 053f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 053f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 053f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 053f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 053f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 063f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 063f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 063f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 063f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 063f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 063f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 073f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 073f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 073f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 073f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 073f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 073f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 083f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 083f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 083f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 083f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 083f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 083f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 093f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 093f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 093f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 093f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 093f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 093f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0a3f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0a3f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0a3f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0a3f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0a3f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0a3f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0b3f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0b3f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0b3f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0b3f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0b3f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0b3f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0c3f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0c3f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0c3f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0c3f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0c3f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0c3f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0d3f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0d3f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0d3f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0d3f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0d3f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0d3f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0e3f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0e3f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0e3f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0e3f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0e3f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0e3f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0f3f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0f3f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0f3f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0f3f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0f3f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0f3f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 103f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 103f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 103f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 103f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 103f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 103f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 113f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 113f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 113f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 113f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 113f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 113f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 123f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 123f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 123f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 123f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 123f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 123f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 133f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 133f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 133f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 133f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 133f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 133f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 143f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 143f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 143f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 143f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 143f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 143f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 153f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 153f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 153f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 153f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 153f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 153f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 163f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 163f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 163f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 163f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 163f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 163f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 173f163f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 173f163f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 173f163f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 173f163f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 173f163f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 173f163f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 003f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 003f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 003f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 003f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 003f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 003f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 013f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 013f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 013f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 013f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 013f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 013f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 023f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 023f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 023f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 023f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 023f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 023f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 033f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 033f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 033f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 033f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 033f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 033f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 043f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 043f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 043f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 043f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 043f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 043f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 053f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 053f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 053f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 053f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 053f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 053f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 063f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 063f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 063f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 063f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 063f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 063f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 073f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 073f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 073f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 073f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 073f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 073f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 083f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 083f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 083f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 083f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 083f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 083f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 093f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 093f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 093f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 093f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 093f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 093f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0a3f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0a3f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0a3f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0a3f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0a3f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0a3f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0b3f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0b3f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0b3f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0b3f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0b3f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0b3f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0c3f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0c3f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0c3f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0c3f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0c3f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0c3f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0d3f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0d3f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0d3f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0d3f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0d3f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0d3f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0e3f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0e3f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0e3f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0e3f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0e3f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0e3f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 0f3f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 0f3f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 0f3f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 0f3f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 0f3f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 0f3f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 103f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 103f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 103f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 103f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 103f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 103f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 113f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 113f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 113f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 113f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 113f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 113f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 123f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 123f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 123f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 123f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 123f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 123f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 133f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 133f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 133f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 133f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 133f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 133f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 143f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 143f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 143f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 143f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 143f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 143f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 153f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 153f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 153f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 153f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 153f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 153f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 163f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 163f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 163f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 163f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 163f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 163f173f, so no erasing is needed
fi
OUTPUT=`./TESTjmdc $port 173f173f 7 | grep 6001`
if [ -n "$OUTPUT" ]
 then
  OUTPUT=`./TESTjmdc $port 173f173f 47 6001 | grep rxdone`
  usleep 100000
  echo Erase Flash on node 173f173f: $OUTPUT
  if [ "$OUTPUT" != "rxdone = 0000" ]
   then
    let "CMDERRORS=$CMDERRORS+1"
  fi
  OUTPUT=`./TESTjmdc $port 173f173f 7 | grep 6001`
  if [ -n "$OUTPUT" ]
   then
    echo DSP Code 6001 Not Erased on node 173f173f!!!
    let "ERSERRORS=$ERSERRORS+1"
  fi
 else
  echo DSP Code 6001 not found on node 173f173f, so no erasing is needed
fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 003f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 003f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 013f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 013f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_23.dat ./file.dat
OUTPUT=`./TESTjmdc $port 023f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 023f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_23.dat ./file.dat
OUTPUT=`./TESTjmdc $port 033f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 033f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 043f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 043f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 053f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 053f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_24.dat ./file.dat
OUTPUT=`./TESTjmdc $port 063f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 063f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_22.dat ./file.dat
OUTPUT=`./TESTjmdc $port 073f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 073f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 083f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 083f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 093f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 093f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0a3f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0a3f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0b3f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0b3f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_31.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0c3f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0c3f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0d3f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0d3f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_32.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0e3f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0e3f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_32.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0f3f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0f3f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 103f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 103f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 113f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 113f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 123f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 123f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_33.dat ./file.dat
OUTPUT=`./TESTjmdc $port 133f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 133f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 143f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 143f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 153f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 153f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 163f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 163f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 173f033f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 173f033f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 003f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 003f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 013f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 013f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 023f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 023f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 033f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 033f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 043f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 043f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_43.dat ./file.dat
OUTPUT=`./TESTjmdc $port 053f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 053f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 063f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 063f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 073f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 073f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_42.dat ./file.dat
OUTPUT=`./TESTjmdc $port 083f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 083f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_42.dat ./file.dat
OUTPUT=`./TESTjmdc $port 093f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 093f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0a3f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0a3f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0b3f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0b3f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0c3f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0c3f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0d3f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0d3f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0e3f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0e3f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0f3f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0f3f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 103f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 103f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 113f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 113f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 123f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 123f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 133f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 133f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 143f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 143f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 153f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 153f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 163f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 163f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 173f093f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 173f093f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 003f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 003f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 013f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 013f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_52.dat ./file.dat
OUTPUT=`./TESTjmdc $port 023f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 023f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_52.dat ./file.dat
OUTPUT=`./TESTjmdc $port 033f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 033f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 043f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 043f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 053f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 053f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 063f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 063f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_53.dat ./file.dat
OUTPUT=`./TESTjmdc $port 073f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 073f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 083f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 083f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 093f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 093f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_41.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0a3f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0a3f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_51.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0b3f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0b3f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0c3f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0c3f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0d3f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0d3f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0e3f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0e3f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0f3f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0f3f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 103f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 103f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 113f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 113f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 123f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 123f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 133f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 133f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 143f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 143f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 153f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 153f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 163f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 163f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 173f003f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 173f003f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 003f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 003f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 013f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 013f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 023f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 023f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 033f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 033f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 043f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 043f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_12.dat ./file.dat
OUTPUT=`./TESTjmdc $port 053f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 053f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 063f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 063f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 073f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 073f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_13.dat ./file.dat
OUTPUT=`./TESTjmdc $port 083f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 083f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_13.dat ./file.dat
OUTPUT=`./TESTjmdc $port 093f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 093f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0a3f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0a3f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0b3f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0b3f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_14.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0c3f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0c3f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0d3f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0d3f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0e3f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0e3f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0f3f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0f3f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 103f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 103f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 113f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 113f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 123f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 123f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 133f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 133f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 143f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 143f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 153f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 153f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 163f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 163f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 173f013f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 173f013f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 003f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 003f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 013f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 013f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 023f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 023f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 033f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 033f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_21.dat ./file.dat
OUTPUT=`./TESTjmdc $port 043f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 043f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_23.dat ./file.dat
OUTPUT=`./TESTjmdc $port 053f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 053f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 063f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 063f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 073f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 073f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_22.dat ./file.dat
OUTPUT=`./TESTjmdc $port 083f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 083f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_22.dat ./file.dat
OUTPUT=`./TESTjmdc $port 093f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 093f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0a3f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0a3f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0b3f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0b3f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0c3f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0c3f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0d3f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0d3f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0e3f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0e3f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0f3f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0f3f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 103f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 103f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_32.dat ./file.dat
OUTPUT=`./TESTjmdc $port 113f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 113f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 123f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 123f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 133f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 133f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_33.dat ./file.dat
OUTPUT=`./TESTjmdc $port 143f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 143f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_33.dat ./file.dat
OUTPUT=`./TESTjmdc $port 153f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 153f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_34.dat ./file.dat
OUTPUT=`./TESTjmdc $port 163f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 163f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 173f103f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 173f103f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 003f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 003f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 013f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 013f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_43.dat ./file.dat
OUTPUT=`./TESTjmdc $port 023f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 023f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_43.dat ./file.dat
OUTPUT=`./TESTjmdc $port 033f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 033f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 043f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 043f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 053f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 053f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 063f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 063f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_42.dat ./file.dat
OUTPUT=`./TESTjmdc $port 073f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 073f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 083f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 083f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 093f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 093f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0a3f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0a3f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0b3f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0b3f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0c3f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0c3f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0d3f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0d3f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0e3f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0e3f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0f3f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0f3f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 103f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 103f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 113f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 113f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 123f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 123f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 133f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 133f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 143f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 143f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 153f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 153f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 163f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 163f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 173f113f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 173f113f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_44.dat ./file.dat
OUTPUT=`./TESTjmdc $port 003f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 003f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_54.dat ./file.dat
OUTPUT=`./TESTjmdc $port 013f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 013f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 023f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 023f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 033f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 033f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 043f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 043f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_52.dat ./file.dat
OUTPUT=`./TESTjmdc $port 053f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 053f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 063f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 063f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 073f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 073f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_53.dat ./file.dat
OUTPUT=`./TESTjmdc $port 083f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 083f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_53.dat ./file.dat
OUTPUT=`./TESTjmdc $port 093f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 093f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0a3f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0a3f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0b3f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0b3f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0c3f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0c3f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0d3f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0d3f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0e3f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0e3f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0f3f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0f3f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 103f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 103f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 113f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 113f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 123f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 123f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 133f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 133f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 143f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 143f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 153f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 153f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 163f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 163f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 173f163f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 173f163f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 003f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 003f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 013f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 013f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_12.dat ./file.dat
OUTPUT=`./TESTjmdc $port 023f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 023f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_12.dat ./file.dat
OUTPUT=`./TESTjmdc $port 033f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 033f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 043f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 043f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 053f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 053f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 063f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 063f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_13.dat ./file.dat
OUTPUT=`./TESTjmdc $port 073f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 073f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 083f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 083f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 093f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 093f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0a3f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0a3f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0b3f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0b3f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0c3f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0c3f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0d3f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0d3f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0e3f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0e3f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 0f3f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 0f3f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 103f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 103f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 113f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 113f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 123f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 123f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 133f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 133f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 143f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 143f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 153f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 153f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_11.dat ./file.dat
OUTPUT=`./TESTjmdc $port 163f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 163f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
rm -f ./file.dat
cp -fv ./dotdats/configfile_extpl_00.dat ./file.dat
OUTPUT=`./TESTjmdc $port 173f173f 45 | grep rxdone`
usleep 100000
echo Write Flash on node 173f173f: $OUTPUT
if [ "$OUTPUT" != "rxdone = 0000" ]; then let "CMDERRORS=$CMDERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 003f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 003f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 013f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 013f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 023f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 023f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 033f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 033f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 043f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 043f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 053f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 053f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 063f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 063f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 073f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 073f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 083f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 083f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 093f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 093f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0a3f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0a3f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0b3f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0b3f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0c3f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0c3f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0d3f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0d3f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0e3f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0e3f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0f3f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0f3f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 103f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 103f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 113f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 113f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 123f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 123f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 133f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 133f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 143f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 143f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 153f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 153f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 163f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 163f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 173f033f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 173f033f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 003f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 003f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 013f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 013f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 023f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 023f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 033f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 033f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 043f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 043f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 053f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 053f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 063f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 063f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 073f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 073f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 083f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 083f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 093f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 093f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0a3f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0a3f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0b3f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0b3f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0c3f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0c3f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0d3f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0d3f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0e3f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0e3f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0f3f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0f3f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 103f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 103f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 113f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 113f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 123f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 123f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 133f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 133f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 143f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 143f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 153f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 153f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 163f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 163f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 173f093f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 173f093f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 003f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 003f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 013f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 013f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 023f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 023f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 033f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 033f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 043f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 043f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 053f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 053f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 063f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 063f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 073f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 073f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 083f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 083f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 093f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 093f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0a3f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0a3f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0b3f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0b3f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0c3f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0c3f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0d3f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0d3f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0e3f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0e3f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0f3f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0f3f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 103f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 103f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 113f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 113f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 123f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 123f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 133f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 133f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 143f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 143f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 153f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 153f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 163f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 163f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 173f003f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 173f003f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 003f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 003f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 013f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 013f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 023f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 023f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 033f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 033f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 043f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 043f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 053f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 053f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 063f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 063f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 073f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 073f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 083f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 083f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 093f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 093f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0a3f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0a3f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0b3f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0b3f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0c3f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0c3f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0d3f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0d3f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0e3f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0e3f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0f3f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0f3f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 103f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 103f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 113f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 113f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 123f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 123f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 133f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 133f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 143f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 143f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 153f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 153f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 163f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 163f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 173f013f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 173f013f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 003f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 003f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 013f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 013f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 023f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 023f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 033f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 033f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 043f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 043f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 053f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 053f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 063f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 063f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 073f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 073f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 083f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 083f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 093f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 093f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0a3f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0a3f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0b3f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0b3f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0c3f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0c3f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0d3f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0d3f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0e3f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0e3f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0f3f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0f3f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 103f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 103f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 113f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 113f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 123f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 123f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 133f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 133f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 143f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 143f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 153f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 153f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 163f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 163f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 173f103f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 173f103f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 003f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 003f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 013f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 013f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 023f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 023f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 033f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 033f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 043f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 043f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 053f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 053f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 063f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 063f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 073f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 073f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 083f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 083f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 093f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 093f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0a3f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0a3f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0b3f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0b3f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0c3f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0c3f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0d3f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0d3f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0e3f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0e3f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0f3f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0f3f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 103f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 103f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 113f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 113f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 123f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 123f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 133f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 133f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 143f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 143f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 153f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 153f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 163f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 163f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 173f113f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 173f113f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 003f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 003f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 013f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 013f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 023f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 023f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 033f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 033f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 043f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 043f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 053f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 053f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 063f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 063f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 073f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 073f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 083f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 083f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 093f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 093f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0a3f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0a3f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0b3f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0b3f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0c3f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0c3f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0d3f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0d3f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0e3f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0e3f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0f3f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0f3f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 103f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 103f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 113f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 113f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 123f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 123f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 133f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 133f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 143f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 143f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 153f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 153f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 163f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 163f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 173f163f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 173f163f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 003f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 003f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 013f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 013f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 023f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 023f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 033f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 033f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 043f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 043f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 053f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 053f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 063f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 063f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 073f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 073f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 083f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 083f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 093f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 093f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0a3f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0a3f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0b3f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0b3f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0c3f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0c3f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0d3f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0d3f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0e3f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0e3f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 0f3f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 0f3f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 103f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 103f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 113f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 113f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 123f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 123f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 133f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 133f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 143f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 143f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 153f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 153f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 163f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 163f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
usleep 100000
OUTPUT=`./TESTjmdc $port 173f173f 7 | grep 6001`
if [ -z "$OUTPUT" ]; then echo DSP Code 6001 Not Loaded on node 173f173f!!!; let "WRTERRORS=$WRTERRORS+1"; fi
echo There were $CMDERRORS erros in sending command!!!!
echo There were $ERSERRORS erros in erasing DSP files!!!!
echo There were $WRTERRORS erros in writing DSP files!!!!
