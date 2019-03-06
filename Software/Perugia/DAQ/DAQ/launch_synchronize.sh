#!/bin/bash

teststring=`ps ux | grep synchronize | grep -v grep | grep -v launch`
if [ -z "$teststring" ]; then
    ./synchronize.sh
else
    echo "synchronize.sh is already running..."
fi
