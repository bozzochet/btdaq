#! /bin/bash

cd /Users/claudio/App/TrunkBT/Decode
make clean
make
cp libEvent.so ../QuickAnalysis
cp rootElibdict_rdict.pcm ../QuickAnalysis
cd ../QuickAnalysis
make clean
make
