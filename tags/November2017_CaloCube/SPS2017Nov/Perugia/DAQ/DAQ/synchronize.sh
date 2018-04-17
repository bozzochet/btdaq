#!/bin/bash
echo "[WAIT!] - generating ROOT FILES for binary data"
./Decode/generate_roots.sh &>/dev/null
rsync -Pavhz ./Decode/RootData/ herd@durantivm2.cern.ch:/mnt/shared/rwall/HERD_BT2017/RootData/
rsync -Pavhz ./Decode/RawData/  herd@durantivm2.cern.ch:/mnt/shared/rwall/HERD_BT2017/RawData/
echo "[WAIT!] - generating PDF for calibrations"
./generate_roots.sh &>/dev/null
rsync -Pavhz ./Calibrations/ herd@durantivm2.cern.ch:/mnt/shared/rwall/HERD_BT2017/CalData/
echo "[OK!] - DONE!"
