#!/usr/bin/env python2.4
#  $Id: checkcrct.py,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $

import sys, os
sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
sys.path.insert(0,'./lib')
import RemoteClient
force=0
v=1
i=0
upd=0
run2p=0
dir=None
HelpText="""
     RemoteClient::CheckCRC
#
#
#  check crc of files on disk
#  if wrong upload from castor
#  if still wrong delete  from disk and update corr db entry  
#  put status as 'bad'
#  input paramaters
#  -d :   path  to files like MC/AMS02/2005A/dir or /s0dah1 
#           if missed all the files will be checked
#                                   
#  -v   verbose if 1 
#  -u     do sql  if 1
#  -r   only process run $run2p if not 0
#  -i    will ask confirmation for rm command
#  -f    to recheck bad ntuples
#  output par:
#   1 if ok  0 otherwise
#
# 
#     example
     ./checkcrc.py -v -u -d/s0dah1  
"""
for x in sys.argv:
    if x == "-h": print HelpText
    elif x == "-f": force=1
    elif x == "-u":  upd=1
    elif x == "-i": i=1
    elif x == "-v": v=1
    elif x[0:2] == "-d" :
        dir=x[2:len(x)]
    elif x[0:2] == "-r" :
        run2p=x[2:len(x)]

html= RemoteClient.RemoteClient()
html.ConnectDB(1)
html.CheckCRCT(v,i,upd,run2p,force,dir,None)
