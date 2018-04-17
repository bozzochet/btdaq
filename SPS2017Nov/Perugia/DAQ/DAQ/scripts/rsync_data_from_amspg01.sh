SERVER=128.141.105.111
SDDIR=/home/testsys/AMS/Data
DDDIR=/home/sysuser/AMS/Data
SCDIR=/home/testsys/AMS/Calibrations
DCDIR=/home/sysuser/AMS/Calibrations

while [ 1 ]
do
    rsync -av testsys@${SERVER}:${SCDIR}/*.cal ${DCDIR}
    rsync -av testsys@${SERVER}:${SDDIR}/*.dat ${DDDIR}
    sleep 120
done