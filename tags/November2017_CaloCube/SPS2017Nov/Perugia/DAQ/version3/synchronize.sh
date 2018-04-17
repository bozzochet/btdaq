#!/bin/bash
echo "[WAIT!] - generating ROOT FILES for binary data"
./Decode/generate_roots.sh &>/dev/null
echo "[WAIT!] - generating PDF for calibrations"
./generate_roots.sh &>/dev/null
echo "[OK!] - DONE!"
