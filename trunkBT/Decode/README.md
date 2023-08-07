#This is the dir for:

#/bin/bash

# AMS-LO USB-LF+LINF and version >=5
./Decode --rawdata "/Volumes/DATI/OneDrive - Istituto Nazionale di Fisica Nucleare/Work/AMS-L0/Data/L0/BLOCKS/PG/TEST_LINF" --caldata "/Volumes/DATI/OneDrive - Istituto Nazionale di Fisica Nucleare/Work/AMS-L0/Data/L0/BLOCKS/PG/TEST_LINF" --l0 -c --calrunstart 0001717 --calrunstop 0001717 0001718 0001718

./PlotCalib --l0 --caldir="/Volumes/DATI/OneDrive - Istituto Nazionale di Fisica Nucleare/Work/AMS-L0/Data/L0/BLOCKS/PG/TEST_LINF/" 0001717

# AMS-L0 LEF directly to USB-LEF (not USB-LF, I think)
./Decode --rawdata "/Volumes/DATI/OneDrive - Istituto Nazionale di Fisica Nucleare/Work/AMS-L0/Data/L0/BLOCKS/PG/202212_AMSL0_data" --caldata "/Volumes/DATI/OneDrive - Istituto Nazionale di Fisica Nucleare/Work/AMS-L0/Data/L0/BLOCKS/PG/202212_AMSL0_data" --l0old -c --calrunstart 0009870 --calrunstop 0009870 0009876 0009880

./PlotCalib --l0 --caldir="/Volumes/DATI/OneDrive - Istituto Nazionale di Fisica Nucleare/Work/AMS-L0/Data/L0/BLOCKS/PG/202212_AMSL0_data" 0009870