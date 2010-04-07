#!/usr/bin/env python2.4
#  $Id: vr.py,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $

import sys, os
sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
sys.path.insert(0,'./lib')
import RemoteClient


v=1
i=0
d=0
run2p=0
h=0
for x in sys.argv:
    if x == "-h": h=1
    elif x == "-i": i=1
    elif x == "-v": v=1
    elif x == "-d": d=1
    elif x[0:2] == "-r" :
        run2p=int(x[2:len(x)])

html= RemoteClient.RemoteClient()
html.ConnectDB(1)
if(html.ServerConnect()):
    html.ValidateRuns(run2p,i,v,d,h)


