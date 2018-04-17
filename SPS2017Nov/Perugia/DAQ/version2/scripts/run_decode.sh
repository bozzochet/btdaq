DDIR=/home/sysuser/AMS/Decode/RawData
CDIR=/home/sysuser/AMS/Decode/CalData
RDIR=/home/sysuser/AMS/Decode/RootData

while [ 1 ]
do

for f in `ls ${DDIR}`
do
    run=(${f//_/ })
    echo "${run}"

    frootdata=${RDIR}/run_${run}_*.root
    if [ ! -f ${frootdata} ]
    then
	#the file has not been decoded
	echo "Run Decode"
    fi

done

exit
done
