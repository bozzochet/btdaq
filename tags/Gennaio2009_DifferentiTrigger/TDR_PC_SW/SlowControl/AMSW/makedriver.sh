#!/bin/sh

gcc -o amswire.mod -c -g -I. -I. -L. -L.  -O2 -fno-builtin -Wall -DLINUX   -D__KERNEL__ -DKERNEL  -DMODULE -isystem /lib/modules/`uname -r`/build/include amswire.c
