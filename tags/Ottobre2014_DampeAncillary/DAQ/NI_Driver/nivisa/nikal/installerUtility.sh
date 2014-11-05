#!/bin/sh
#
# installerUtility.sh
# This is the installation utility script for NI-KAL for Linux.
# This script is used by the nikal INSTALL script and can be sourced
# by other installers to call the functions it provides.
#
#  © Copyright 2005,
#  National Instruments Corporation.
#  All Rights reserved.
#
#  originated:  4 February 2005
#

statusFail=2
statusSuccess=0
statusSuccessReboot=1
nikalVersion=1.4.0f0

# findutil gets the path to a utility
# Input: $1: name of utility to find
# Output: $FINDUTIL: path of found utility or empty string
# Returns: 0=success, nonzero=failure
notFound="Not found in current path"
FINDUTIL=""
findutil()
{
   # Validate inputs
   if [ -z $1 ]; then
      return $statusFail
   fi
   # Use which to locate the executable
   FINDUTIL=`which $1 --skip-alias 2>/dev/null`
   # If not found, display an error
   if [ -z "$FINDUTIL" ]; then
      STRING="$1"
      # Pad spaces to account for aesthetics
      while [ ${#STRING} -lt 10 ]; do
         STRING="$STRING "
      done
      echo "$STRING $notFound"
      return $statusFail
   fi
   return $statusSuccess
}

#
# Verify tools used during install
#
RETURN=0

findutil "awk"
let "RETURN+=$?"
AWK=$FINDUTIL

findutil "cat"
let "RETURN+=$?"
CAT=$FINDUTIL

findutil "chmod"
let "RETURN+=$?"
CHMOD=$FINDUTIL

findutil "cp"
let "RETURN+=$?"
CP=$FINDUTIL

findutil "cut"
let "RETURN+=$?"
CUT=$FINDUTIL

findutil "expr"
let "RETURN+=$?"
EXPR=$FINDUTIL

findutil "gcc"
let "RETURN+=$?"
CC=$FINDUTIL

findutil "grep"
let "RETURN+=$?"
GREP=$FINDUTIL

findutil "id"
let "RETURN+=$?"
ID=$FINDUTIL

findutil "install"
let "RETURN+=$?"
INSTALL=$FINDUTIL

findutil "mkdir"
let "RETURN+=$?"
MKDIR=$FINDUTIL

findutil "mknod"
let "RETURN+=$?"
MKNOD=$FINDUTIL

findutil "mv"
let "RETURN+=$?"
MV=$FINDUTIL

findutil "objcopy"
let "RETURN+=$?"
OBJCOPY=$FINDUTIL

findutil "rm"
let "RETURN+=$?"
RM=$FINDUTIL

findutil "rmdir"
let "RETURN+=$?"
RMDIR=$FINDUTIL

findutil "sed"
let "RETURN+=$?"
SED=$FINDUTIL

findutil "strings"
let "RETURN+=$?"
STRINGS=$FINDUTIL

findutil "uname"
let "RETURN+=$?"
UNAME=$FINDUTIL

findutil "wc"
let "RETURN+=$?"
WC=$FINDUTIL

if [ $RETURN -ne 0 ]; then
   echo "*** ERROR: Some required tools are missing or were not found. ***"
   exit $RETURN
fi


# Output tools found
dumpTools()
{
echo "AWK=$AWK
CAT=$CAT
CC=$CC
CHMOD=$CHMOD
CP=$CP
CUT=$CUT
EXPR=$EXPR
GREP=$GREP
ID=$ID
INSTALL=$INSTALL
MKDIR=$MKDIR
MKNOD=$MKNOD
MV=$MV
OBJCOPY=$OBJCOPY
RM=$RM
RMDIR=$RMDIR
SED=$SED
STRINGS=$STRINGS
UNAME=$UNAME
WC=$WC"
}

niDistributionCheck()
{
   echo "National Instruments products support the following Linux distributions:"
   echo "   Mandriva Linux Official"
   echo "   SUSE Linux"
   echo "   Red Hat Enterprise Linux WS"
   echo "Refer to README.txt for the latest information at the time of release."
   echo "Refer to www.ni.com/linux for the most recent information about Linux."
   echo "support at National Instruments."
   return $statusSuccess
}

# versionCompare() - compares two RPM-style version numbers and returns -1, 0, 1 
# to indicate if the first verison is less than, equal to, or greater than
# the second.  Version numbers can be specified with or without release
# suffixes.  Alpha, beta, etc. suffixes should always specified be in the
# release portion.
#
# Examples:
#  versionCompare 1.0  1.0.1
#  versionCompare 1.0-a1 1.0-a12
#  versionCompare 1.0.1-b1 1.0-1
#  versionCompare 2.2.3.4-99  2.2.4
#  versionCompare 8.0-b129 8.0-rc0
#  versionCompare 8.0-rc0 8.0-1
versionCompare()
{
   canonicalizeVersion()
   {
      canonicalVersion=`echo "$1" |
         $SED -e 's,\([0-9]\)\([A-Za-z]\),\1.000.\2,g' \
             -e 's,\([A-Za-z]\)\([0-9]\),\1.\2,g' \
             -e 's,^\([A-Za-z]\),---.\1,' \
             -e 's,\b\([0-9]\)\b,00\1,g' \
             -e 's,\b\([0-9][0-9]\)\b,0\1,g'`
   }

   local arg1 arg2 nResult firstVersion secondVersion resultVersion
   if [ "$1" = "$2" ]; then
      return 0;
   fi
   arg1=`echo "$1" | $SED 's,-.*$,,'`
   arg2=`echo "$2" | $SED 's,-.*$,,'`
   if [ "$1" != "$arg1" -o "$2" != "$arg2" ]; then
      versionCompare "$arg1" "$arg2"
      nResult=$?
      if [ $nResult -ne 0 ]; then
         return $nResult
      fi
   fi
   canonicalizeVersion `echo "$1" | $SED 's,^.*-,,'`
   firstVersion="$canonicalVersion"
   canonicalizeVersion `echo "$2" | $SED 's,^.*-,,'`
   secondVersion="$canonicalVersion"
   resultVersion=`(echo "$firstVersion"; echo "$secondVersion") |
      LC_ALL=C sort -t. +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 | head -1`
   if [ "$firstVersion" = "$resultVersion" ]; then
      return -1
   else
      return 1
   fi
}

# check the system for pre-requisites for installation.  This function will
# return successful if it doesn't find any problems or will print a message
# and return an error if it finds something wrong.
nikalInstallationCheck()
{
   # check for gcc since we need that to successfully install
   if ! which gcc &> /dev/null; then
      echo "***************************** ERROR ******************************"
      echo "*          ERROR: gcc does not appear to be installed!           *"
      echo "* Installation of gcc is required to continue this installation. *"
      echo "***************************** ERROR ******************************"
      return $statusFail
   fi

   if [ $KERNELTARGET ]; then
      kernelVersion=$KERNELTARGET
   else
      kernelVersion=`$UNAME -r`
   fi
   kernelMajor=`echo $kernelVersion | $CUT -d "-" -f 1 | $CUT -d "." -f 1`
   kernelMinor=`echo $kernelVersion | $CUT -d "-" -f 1 | $CUT -d "." -f 2`
   kernelPatchLevel=`echo $kernelVersion | $CUT -d "-" -f 1 | $CUT -d "." -f 3`
   if [ "$kernelMinor" != "4" ] && [ "$kernelMinor" != "6" ]; then
      echo "*********************************** ERROR ************************************"
      echo "*                   ERROR: $kernelMajor.$kernelMinor kernels are not supported!                    *"
      echo "* Running a 2.4.x or 2.6.x kernel is required to continue this installation. *"
      echo "*********************************** ERROR ************************************"
      return $statusFail
   fi

   # check that this isn't a 64-bit system.  We only support 32-bit
   if [ "`$UNAME -m`" = "x86_64" ]; then
      echo "******************************** ERROR ****************************************"
      echo "* It appears that your system is running a 64-bit version of Linux.  This     *"
      echo "* product only supports Linux x86 32-bit systems.                             *"
      echo "******************************** ERROR ****************************************"
      return $statusFail
   fi

   # find modules directory
   if [ ! -d /lib/modules/$kernelVersion ]; then
      echo "*** ERROR: /lib/modules/$kernelVersion directory not found. ***"
      exit $statusFail
   fi
   
   # find the kernel headers/source dir
   if [ $KERNELHEADERS ]; then
      headersDir=$KERNELHEADERS
   elif [ -d "/lib/modules/$kernelVersion/build/include/linux" ]; then
      if [ -d "/lib/modules/$kernelVersion/source/include/linux" ]; then
         headersDir=/lib/modules/$kernelVersion/source
      else
         headersDir=/lib/modules/$kernelVersion/build
      fi
   elif [ -d "/usr/src/linux-$kernelVersion/include/linux" ]; then
      headersDir=/usr/src/linux-$kernelVersion
   elif [ -d "/usr/src/linux-$kernelMajor.$kernelMinor/include/linux" ]; then
      headersDir=/usr/src/linux-$kernelMajor.$kernelMinor
   elif [ -d "/usr/src/kernels/$kernelVersion/include/linux" ]; then
      headersDir=/usr/src/kernels/$kernelVersion
   elif [ -d "/usr/src/linux/include/linux" ]; then
      headersDir=/usr/src/linux
   else
      echo "************************************ ERROR *************************************"
      echo "* Kernel source does not appear to be installed for the $kernelVersion kernel."
      echo "*    Installation of the kernel-source package for kernel $kernelVersion is"
      echo "*                   required to continue this installation."
      echo "************************************ ERROR *************************************"
      return $statusFail
   fi

   # check that the headers dir that we found is configured
   configuredStatus=$statusSuccess
   if [ ! -e $headersDir/include/asm ]; then
      configuredStatus=$statusFail
   else
      if [ "$kernelMinor" = "4" ]; then
         if [ ! -e $headersDir/include/linux/modversions.h ]; then
            configuredStatus=$statusFail
         fi
      else
         if [ ! -e $headersDir/include/asm/asm_offsets.h ] && [ ! -e $headersDir/include/asm/asm-offsets.h ]; then
            configuredStatus=$statusFail
         fi
      fi
   fi

   if [ $configuredStatus -ne $statusSuccess ]; then
      echo "******************************** ERROR ****************************************"
      echo "* Kernel source in $headersDir does not appear to be"
      echo "* configured for the $kernelVersion kernel."
      echo "* Configuration of kernel source is required to continue installation.        *"
      echo "* Refer to the README file for the product you are installing for information *"
      echo "* about configuring your kernel source.                                       *"
      echo "******************************** ERROR ****************************************"
      return $statusFail
   fi

   # check that the headers dir that we found is for the correct version of
   # the kernel
   if [ "`$GREP UTS_RELEASE $headersDir/include/linux/version.h | $GREP -c \"$kernelVersion\"`" = "0" ]; then
      echo "******************************** ERROR ****************************************"
      echo "* Kernel source in $headersDir does not appear to be"
      echo "* for the $kernelVersion kernel."
      echo "* Ensure that kernel source for the $kernelVersion kernel is installed"
      echo "* and configured.  Refer to the README file for the product you are           *"
      echo "* installing for information about configuring your kernel source.            *"
      echo "******************************** ERROR ****************************************"
      return $statusFail
   fi

   # check the version of gcc that is in the path and ensure that it is the
   # same version of gcc that was used to compile the currently running kernel
   currentGCCVersion=`$CC --version | head -n 1 | $AWK '{print $3}'`
   kernelGCCVersion=`$CAT /proc/version | $SED 's/.*gcc version \([0-9][^ )]*\).*/\1/'`
   if [ "$currentGCCVersion" != "$kernelGCCVersion" ]; then
      if [ "${currentGCCVersion%\.*}" != "${kernelGCCVersion%\.*}" ]; then
         echo "******************************** ERROR ****************************************"
         echo "* The version of gcc in the path does not match the version of gcc used to    *"
         echo "* compile the currently running kernel.  This can cause unpredictable         *"
         echo "* behavior in kernel drivers and should be fixed.                             *"
         echo "* gcc version: $currentGCCVersion                                                          *"
         echo "* kernel compiled with: $kernelGCCVersion                                                 *"
         echo "******************************** ERROR ****************************************"
         return $statusFail
      else
         echo "******************************** WARNING **************************************"
         echo "* The version of gcc in the path does not match the version of gcc used to    *"
         echo "* compile the currently running kernel.  This can cause unpredictable         *"
         echo "* behavior in kernel drivers and should be fixed.                             *"
         echo "* gcc version: $currentGCCVersion                                                          *"
         echo "* kernel compiled with: $kernelGCCVersion                                                 *"
         echo "******************************** WARNING **************************************"
      fi
   fi

   return $statusSuccess
}

configuredForCurrentKernel()
{
   # shell scripts are evil and reverse the boolean logic
   configuredForCurrent=0
   notConfiguredForCurrent=1

   nikalDir=`$CAT /etc/natinst/nikal/nikal.dir`
   nikalMakefile=$nikalDir/src/Makefile.in
   currentKernel=`$UNAME -r`

   if [ -f $nikalMakefile ]; then
      configuredKernelTarget=`$GREP KERNELTARGET $nikalMakefile | $AWK -F = '{ print $2 }'`

      if [ "$configuredKernelTarget" == "$currentKernel" ]; then
         return $configureForCurrent
      else
         return $notConfiguredForCurrent
      fi
   elif [ $KERNELTARGET ]; then
      if [ "$KERNELTARGET" == "$currentKernel" ]; then
         return $configureForCurrent
      else
         return $notConfiguredForCurrent
      fi
   else
      return $configuredForCurrent
   fi
}

# testKernelDriverLoaded and recursiveUnloadKernelDriver from palmodmgr
testKernelDriverLoaded()
{
   /sbin/lsmod | $GREP "^$1" &> /dev/null
   return $?
}

recursiveUnloadKernelDriver()
{
   # default returnStatus is success
   returnStatus=0

   if testKernelDriverLoaded $1
   then
      kernelMinorVer=`$UNAME -r | $AWK -F. '{ print $2 }'`
      if [ "$kernelMinorVer" == "4" ] || [ "$kernelMinorVer" == "6" ]
      then
         recursiveUnloadKernelDriver2${kernelMinorVer} $1
         returnStatus=$?
      else
         echo "recursiveUnloadKernelDriver: `$UNAME -r` kernel not supported"
         return 1
      fi
   fi
   return $returnStatus
}

recursiveUnloadKernelDriver24()
{
   # if this module is depended upon by other modules, then stop those
   # first
   if /sbin/lsmod | $GREP "^$1" | $GREP '\[' &> /dev/null
   then
      for i in `/sbin/lsmod | $GREP "^$1" | $SED 's/.*\[\(.*\)\]/\1/'`
      do
         recursiveUnloadKernelDriver24 $i
         if [[ $? != "0" ]]
         then
            return 1
         fi
      done
   fi
   /sbin/rmmod $1 &> /dev/null
   rmmodStatus=$?
   if [ "$rmmodStatus" != "0" ]; then
      echo "WARNING: could not unload module $1 from memory"
   fi

   return $rmmodStatus
}

recursiveUnloadKernelDriver26()
{
   # if this module is depended upon by other modules, then stop those
   # first
   if [ `/sbin/lsmod | $GREP "^$1" | $WC -w` == 4 ]
   then
      for i in `/sbin/lsmod | $GREP "^$1" | $AWK '{ print $4 }' | $SED 's/,/ /g'`
      do
         recursiveUnloadKernelDriver26 $i
         if [[ $? != "0" ]]
         then
            return 1
         fi
      done
   fi
   /sbin/rmmod $1 &> /dev/null
   rmmodStatus=$?
   if [ "$rmmodStatus" != "0" ]; then
      echo "WARNING: could not unload module $1 from memory"
   fi

   return $rmmodStatus
}

# Determine if a newer version of installerUtility.sh is already installed
if [ -f /etc/natinst/nikal/nikal.dir ]; then
   read nikalDir < /etc/natinst/nikal/nikal.dir
   if [ -f $nikalDir/bin/installerUtility.sh ]; then
      thisVersion=$nikalVersion
      installedVersion=`grep -m 1 nikalVersion $nikalDir/bin/installerUtility.sh | awk -F = '{print $2}'`
      versionCompare $installedVersion $thisVersion
      if [ $? -eq 1 ]; then
         . $nikalDir/bin/installerUtility.sh
      fi
   fi
fi
