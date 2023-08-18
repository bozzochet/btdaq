# This is the dir with the core of the packet: the tools to decode binary files, clusterize raw file and output ROOT files. The code contain also the code to create high level objects like tracks and charge measurements

Some decoding examples:

## AMS-L0

- ### AMS-LO USB-LF+LINF and version >=5
```
./Decode --rawdata "/path/to/BLOCKS/PG/TEST_LINF" --caldata "/path/to/BLOCKS/PG/TEST_LINF" --l0 -c --calrunstart 0001717 --calrunstop 0001717 0001718 0001718
```
```
./PlotCalib --l0 --caldir="path/to/BLOCKS/PG/TEST_LINF/" 0001717
```

- ### AMS-L0 LEF directly to USB-LEF (not USB-LF, I think)
```
./Decode --rawdata "/path/to/BLOCKS/PG/202212_AMSL0_data" --caldata "/path/to/BLOCKS/PG/202212_AMSL0_data" --l0old -c --calrunstart 0009870 --calrunstop 0009870 0009876 0009880
```
```
./PlotCalib --l0 --caldir="/path/to/BLOCKS/PG/202212_AMSL0_data" 0009870
```

## AMS

## OCA

## FOOT

Some logger examples:

## AMS-L0

- ### AMS-LO USB-LF+LINF and version >=5
```
./AMSL0filelogger --datadir="/Volumes/DATI/OneDrive - Istituto Nazionale di Fisica Nucleare/Work/AMS-L0/Data/L0/BLOCKS/CH/USBLF_PCGSC17" --runstart 0000435 --runstop 0000479
```
