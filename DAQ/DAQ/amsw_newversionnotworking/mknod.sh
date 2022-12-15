#!/bin/bash

rm -f /dev/amsw?
mknod /dev/amsw0 c 240 0
mknod /dev/amsw1 c 240 1
mknod /dev/amsw2 c 240 2
mknod /dev/amsw3 c 240 3

AMSW_IOC_POLL_TX