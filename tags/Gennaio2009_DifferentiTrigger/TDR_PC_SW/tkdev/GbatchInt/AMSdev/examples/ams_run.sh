#!/bin/bash

if [ "$#" == "1" ]; then
  file=$1
else
  echo "You have to provide the name of a C++ file. For instance: \"ana.C\"; EXIT"
  exit
fi

if [ -e "${file}" ]; then
    suffix2=`echo $file | awk '{print substr($1,length($1)-1,2)}'`
    suffix3=`echo $file | awk '{print substr($1,length($1)-2,3)}'`
    suffix4=`echo $file | awk '{print substr($1,length($1)-3,4)}'`
    if [ "$suffix2" = ".C" ]; then
      FILE_EXE=`echo $file | awk '{print substr($1,1,length($1)-2)}'`.exe
    elif [ "$suffix3" = ".cc" ]; then
      FILE_EXE=`echo $file | awk '{print substr($1,1,length($1)-3)}'`.exe
    elif [ "$suffix4" = ".cxx" ]; then
      FILE_EXE=`echo $file | awk '{print substr($1,1,length($1)-4)}'`.exe
    else
      echo "Wrong input file \""${file}\""; EXIT "
      exit
    fi
else
    echo "Wrong input file \""${file}\""; EXIT "
    exit
fi

OS=`uname`
ARCH=`$ROOTSYS/bin/root-config --arch`

if [ ! -n "$AMSWD" ]; then
  AMSWD="/afs/cern.ch/exp/ams/Offline/vdev"
fi

if [ "$ARCH" = "linuxicc" ]; then
   if [ -e "/opt/intel/compiler70/ia32/bin/icc" ]; then
      GC="/opt/intel/compiler70/ia32/bin/icc -static -w"
   else
      echo "You have no access to $GC."
      echo "Please use a Root version compiled with g++."
      exit
   fi
   EXTRALIBS="-lshift -ldl -lcrypt"
   AMSLIB="${AMSWD}/lib/linux/icc/libntuple.a"
elif [ "${OS}" = "Linux" ]; then
   GC="g++ -static -w"
   EXTRALIBS="-lshift -ldl -lcrypt"
   AMSLIB="${AMSWD}/lib/linux/libntuple.a"
elif [ "${OS}" = "OSF1" ]; then
   GC="cxx -non_shared -w"
   EXTRALIBS="-lshift -lcxx -lm"
   AMSLIB="${AMSWD}/lib/osf1/libntuple.a"
else
  echo "This script only runs on Linux and on OSF1; EXIT"
  exit
fi

ROOTLIBDIR=`$ROOTSYS/bin/root-config --libdir`
ROOTLIBS="-L${ROOTLIBDIR} -lRoot"

ROOTINCDIR=`$ROOTSYS/bin/root-config --incdir`
INCS="-I${AMSWD}/include -I${ROOTINCDIR}"

echo -e "\n>>> COMPILING and LINKING: ${file}"

/usr/bin/make -f - <<!
SHELL=/bin/sh

${FILE_EXE}:	${file} ${AMSLIB}
	${GC} ${INCS} -o ${FILE_EXE} ${file} ${AMSLIB} ${ROOTLIBS} ${EXTRALIBS}
	chmod 755 ${FILE_EXE}

${AMSLIB}: 
	cd $AMSWD/install; gmake static

!

if [ -x "${FILE_EXE}" ]; then
      echo -e "\n>>> EXECUTING file: ${FILE_EXE}"
      ${FILE_EXE}
fi
