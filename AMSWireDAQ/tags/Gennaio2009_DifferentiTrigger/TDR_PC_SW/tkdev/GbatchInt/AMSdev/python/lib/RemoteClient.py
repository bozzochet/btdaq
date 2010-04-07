import sys,os,time,string,re,thread,smtplib
from stat import *
from DBSQLServer import DBSQLServer
from DBServer import DBServer
class RemoteClient:
    env={}
    dbclient=""
    sqlserver=""
    sqlconnected=-1
    def getior(self):
        if self.sqlconnected==-1:
            self.ConnectDB()
        if(self.sqlconnected==1):
            ret=self.sqlserver.Query("select IORS,IORP,dbfilename from Servers where status='Active' order by lastupdate desc")
            if(len(ret)>0):
                return ret[0][0]
            else: return ""
    def ConnectDB(self,one=0):
        self.sqlserver=DBSQLServer(sys.argv,one)
        self.sqlconnected=self.sqlserver.Connect()
        self.setenv()
        return self.sqlconnected
    def CheckCRC(self,v,irm,upd,run2p,force,dir,nocastoronly):
        #
        #  check crc of files on disks
        #  copy from castor if bad crc found
        #  remove from disk if castor copy is eq damaged
        #  change ntuple status to 'Bad'
        #  Updates catalogs
        #
        # input par: 
        #                                     /dir are optional ones
        #  $verbose   verbose if 1
        #  $irm    
        #  $update    do sql/file rm  if 1
        #  $run2p   only process run $run2p if not 0
        #  $dir:   path to  files like /s0dat1
        #  $nocastoronly  only check crc for files without castor
        #  output par:
        #   1 if ok  0 otherwise
        #
        # 

        if irm==1: rm="rm -i "
        else: rm ="rm -f "
        # to be changed
        address="vitali.choutko@cern.ch"
        castorPrefix='/castor/cern.ch/ams'
        delimiter='MC'
        rfcp="/usr/local/bin/rfcp"
        whoami=os.getlogin()
        if not (whoami == None or whoami =='ams' or whoami=='casadmva' or whoami=='choutko'):
            print "castorPath -ERROR- script cannot be run from account : ",whoami 
            return 0
        sql="select ntuples.path,ntuples.crc,ntuples.castortime,ntuples.run,ntuples.fevent,ntuples.levent,ntuples.sizemb from ntuples where  ntuples.path not like  '"+castorPrefix+"%' ";
        if force==0 :
            sql=sql+"  and ( ntuples.status='OK' or ntuples.status='Validated') "
        if dir != None:
            sql= sql+" and ntuples.path like '%"+dir+"%' "
        if run2p>0 :
            sql= sql+" and ntuples.run="+run2p+" ";
        if nocastoronly !=None and nocastoronly==1:
            sql=sql+" and ntuples.castortime=0 "
        sql=sql+" order by ntuples.run "
        ret_nt=self.sqlserver.Query(sql)
        totmb=0
        ntna=0
        badfs=[]
        cmb=0
        runs=0
        ntpf=0
        ntpb=0
        ntp=0
        for ntuple in ret_nt:
            totmb=totmb+ntuple[6]
        times=time.time()
        #print len(ret_nt)
        run=0
        self.CheckFS(1)
        for ntuple in ret_nt:
            if run != ntuple[3]:
                junk=string.split(ntuple[0],delimiter)
                if len(junk)<2:
                    print "fatal problem with ",delimiter," for ",ntuple[0]," do nothing "
                    ntna=ntna+1
                    continue
                disk=(junk[0])[0:len(junk[0])-1];
                sql="select disk from filesystems where disk='"+disk+"' and isonline=1"
                ret_fs=self.sqlserver.Query(sql)
                if len(ret_fs) == 0:
                    found=0
                    for bd in badfs:
                        if bd == disk:
                            found=1
                            break
                    if not found:
                        badfs.append(disk)
                        if v:
                            print disk," is not online"
                    ntna=ntna+1
                    continue
                run=ntuple[3]
                timec=time.time()-times
                speed=cmb/(timec+1)
                timest=(totmb-cmb)/(speed+0.001)/3600
                if speed==0:
                    timest=0
                if v:
                    tn=len(ret_nt)
                    print "New run ",run,ntp," ntuples processed out of ",tn," for", timec," min ",speed," mbsec out of ",timest," hrs"
                runs=runs+1
            ntp=ntp+1
            cmb=cmb+ntuple[6]
            crccmd="/Offline/AMSDataDir/DataManagement/exe/linux/crc "+ntuple[0]+" "+str(ntuple[1])
            rstatus=os.system(crccmd)
            rstatus=(rstatus>>8)
            if rstatus !=1:
                if v:
                    print ntuple[0]," crc error: ",rstatus
                ntpb=ntpb+1
                if ntuple[2]>0:
                    #copy from castor
                    a=0
                else:
                    #no castor file found and bad crc remove ntuple
                    ntpf=ntpf+1
        if v and ntp>0:
            print "Total of ",runs,"  runs, ",ntp," ntuples  processed. \n ",ntpb," bad ntuples found. \n ",ntpf,"  ntuples could not be repared\n ",ntna," ntuples could not be verified"

    def CheckCRCT(self,v,irm,upd,run2p,force,dir,nocastoronly):
        #
        #  threaded version one thread per fs
        #  check crc of files on disks
        #  copy from castor if bad crc found
        #  remove from disk if castor copy is eq damaged
        #  change ntuple status to 'Bad'
        #  Updates catalogs
        #
        # input par: 
        #                                     /dir are optional ones
        #  $verbose   verbose if 1
        #  $irm    
        #  $update    do sql/file rm  if 1
        #  $run2p   only process run $run2p if not 0
        #  $dir:   path to  files like /s0dat1
        #  $nocastoronly  only check crc for files without castor
        #  output par:
        #   1 if ok  0 otherwise
        #
        # 
        if irm==1: rm="rm -i "
        else: rm ="rm -f "
        # to be changed
        global address,castorPrefix,delimiter,rfcp
        sql ="select address from mails where rserver=1"
        ret_a=self.sqlserver.Query(sql)
        address=ret_a[0][0]
        castorPrefix='/castor/cern.ch/ams'
        delimiter='MC'
        rfcp="/usr/bin/rfcp"
        whoami=os.getlogin()
        if not (whoami == None or whoami =='ams' or whoami=='casadmva' or whoami=='choutko'):
            print "castorPath -ERROR- script cannot be run from account : ",whoami 
            return 0
        sql="select ntuples.path,ntuples.crc,ntuples.castortime,ntuples.run,ntuples.fevent,ntuples.levent,ntuples.sizemb from ntuples where  ntuples.path not like  '"+castorPrefix+"%' ";
        if force==0 :
            sql=sql+"  and ( ntuples.status='OK' or ntuples.status='Validated') "
        if dir != None:
            sql= sql+" and ntuples.path like '%"+dir+"%' "
        if run2p>0 :
            sql= sql+" and ntuples.run="+run2p+" ";
        if nocastoronly !=None and nocastoronly==1:
            sql=sql+" and ntuples.castortime=0 "
        sql=sql+" order by ntuples.run "
        ret_nt=self.sqlserver.Query(sql)
        global totmb
        totmb=0
        global badfs
        self.ntna=0
        badfs=[]
        self.update=upd
        self.cmb=0
        self.runs=0
        self.ntpf=0
        self.ntpb=0
        self.ntp=0
        self.v=v
        for ntuple in ret_nt:
            totmb=totmb+ntuple[6]
        self.times=time.time()
        self.CheckFS(1,300)
        sql="select disk from filesystems where isonline=1"
        ret_fs=self.sqlserver.Query(sql)
        global exitmutexes
        exitmutexes = {}
        global mutex
        mutex=thread.allocate_lock()
        for fs in ret_fs:
            exitmutexes[fs[0]]=thread.allocate_lock()
            thread.start_new(self.checkcrct,(fs[0],ret_nt))
            #self.checkcrct(fs[0],ret_nt)
        for key in exitmutexes.keys():
            while not exitmutexes[key].locked(): pass
        time.sleep(1)
        for key in exitmutexes.keys():
            while not exitmutexes[key].locked(): pass
        if v and self.ntp>0:
            print "Total of ",self.runs,"  runs, ",self.ntp," ntuples  processed. \n ",self.ntpb," bad ntuples found. \n ",self.ntpf,"  ntuples could not be repared\n ",self.ntna," ntuples could not be verified"

    def checkcrct(self,fs,ret_nt):
        mutex.acquire()
        print fs
        run=0
        mutex.release()
        for ntuple in ret_nt:
            mutex.acquire()
            if run != ntuple[3]:
                junk=string.split(ntuple[0],delimiter)
                if len(junk)<2:
                    print "fatal problem with ",delimiter," for ",ntuple[0]," do nothing "
                    self.ntna=self.ntna+1
                    mutex.release()
                    continue
                disk=(junk[0])[0:len(junk[0])-1];
                if disk != fs:
                    mutex.release()
                    continue
                sql="select disk from filesystems where disk='"+disk+"' and isonline=1"
                ret_fs=self.sqlserver.Query(sql)
                if len(ret_fs) == 0:
                    found=0
                    for bd in badfs:
                        if bd == disk:
                            found=1
                            break
                    if not found:
                        badfs.append(disk)
                        if self.v:
                            print disk," is not online"
                    self.ntna=self.ntna+1
                    mutex.release()
                    continue
                run=ntuple[3]
                timec=time.time()-self.times
                speed=self.cmb/(timec+1)
                timest=(totmb-self.cmb)/(speed+0.001)/3600
                if speed==0:
                    timest=0
                if self.v:
                    tn=len(ret_nt)
                    print "New run ",run,self.ntp," ntuples processed out of ",tn," for", timec," min ",speed," mbsec out of ",timest," hrs"
                self.runs=self.runs+1
            self.ntp=self.ntp+1
            self.cmb=self.cmb+ntuple[6]
            crccmd="/Offline/AMSDataDir/DataManagement/exe/linux/crc "+ntuple[0]+" "+str(ntuple[1])
            mutex.release()
            rstatus=os.system(crccmd)
            mutex.acquire()
            rstatus=(rstatus>>8)
            if rstatus !=1:
                if self.v:
                    print ntuple[0]," crc error: ",rstatus
                    self.sendmailmessage(address,"crc failed"+ntuple[0]," ")
                    self.ntpb=self.ntpb+1
                if ntuple[2]>0:
                    #copy from castor
                    suc=1
                    junk=ntuple[0].split(delimiter)
                    if len(junk)>1:
                        castornt=castorPrefix+"/"+delimiter+junk[1]
                        sys=rfcp+" "+castornt+" "+ntuple[0]+".castor"
                        mutex.release()
                        i=os.system(sys)
                        mutex.acquire()
                        if(i):
                            suc=0
                            if(self.v):
                                print sys," failed for ",castornt
                            self.ntpf=self.ntpf+1
                            os.system("rm "+ntuple[0]+".castor")
                            continue
                        else:
                            crccmd="/Offline/AMSDataDir/DataManagement/exe/linux/crc "+ntuple[0]+".castor "+str(ntuple[1])
                            mutex.release()
                            rstatus=os.system(crccmd)
                            mutex.acquire()
                            rstatus=(rstatus>>8)
                            if rstatus !=1:
                                if self.v:
                                    self.sendmailmessage(address,"crc castor failed"+castornt," ")
                                    print castornt," castor crc error: ",rstatus
                            if(suc):
                                if(self.update):
                                    os.system("mv "+ntuple[0]+".castor "+ntuple[0])
                                else: os.system("rm "+ntuple[0]+".castor")
                            else:
                                os.system("rm "+ntuple[0]+".castor")
                                self.ntpf=self.ntpf+1
                                sql="update ntuples set ntuples.status='BAD' where ntuples.path='"+ntuple[0]+"' "
                                self.sqlserver.Update(sql)
                                sql="update ntuples set ntuples.crcflag=0 where ntuples.path='"+ntuple[0]+"' "
                                self.sqlserver.Update(sql)
                                sql="update ntuples set ntuples.path='"+castornt+"' where ntuples.path='"+ntuple[0]+"' ";
                                self.sqlserver.Update(sql)
                              
                                sql=" update jobs set realtriggers=realtriggers-"+str(ntuple[5])+"+"+str(ntuple[4])+"-1 where jid="+str(ntuple[3])
                                self.sqlserver.Update(sql)
                                if(self.update):
                                    fname=ntuple[0]
                                    res=self.sqlserver.Commit()
                                    if(res==0):
                                       if(self.v): print " commit failed for ",ntuple[0]
                                    else:
                                      sys=rm+" "+fname
                                      i=os.system(sys)
                                      if(i): self.sendmailmessage(address,"unable to "+sys," ")
                                else: self.sqlserver.Commit(0)
                    else:
                      print "fatal error with delimiter for "+ntuple[0]+" do nothing"
                      self.ntna=self.ntna+1
                                    
                else:
                    #no castor file found and bad crc remove ntuple
                    self.ntpf=self.ntpf+1
                    sql="insert into ntuples_deleted select * from ntuples where ntuples.path='"+ntuple[0]+"'"
                    self.sqlserver.Update(sql)
                    timenow=int(time.time())
                    sql="update ntuples_deleted set timestamp="+str(timenow)+"  where path='"+ntuple[0]+"'"
                    self.sqlserver.Update(sql)
                    sql="delete from ntuples where ntuples.path='"+ntuple[0]+"' "
                    self.sqlserver.Update(sql)
                    sql=" update jobs set realtriggers=realtriggers-"+str(ntuple[5])+"+"+str(ntuple[4])+"-1 where jid="+str(ntuple[3])
                    self.sqlserver.Update(sql)
                    sql="select path from ntuples where run="+str(ntuple[3])
                    r2=self.sqlserver.Query(sql)
                    sql="select realtriggers from jobs where jid="+str(ntuple[3])
                    r3=self.sqlserver.Query(sql)
                    if(self.update):
                                    fname=ntuple[0]
                                    res=self.sqlserver.Commit()
                                    if(res==0):
                                       if(self.v): print " commit failed for ",ntuple[0]
                                    else:
                                      sys=rm+" "+fname
                                      i=os.system(sys)
                                      if(i): self.sendmailmessage(address,"unable to "+sys," ")
                    else: self.sqlserver.Commit(0)

            mutex.release()
        exitmutexes[fs].acquire()

    def CheckFS(self,updatedb=0,cachetime=60):
        #
        #  check  filesystems, update the tables accordingly if $updatedb is set
        #  status: Active  :  May be used 
        #          Reserved :  Set manually, may not be reseting
        #          Full     :  if < 3 GB left
        #         
        #  isonline: 1/0 
        #  return disk with highest available and online=1
        #
           sql="select disk from filesystems where isonline=1 and status='Active' order by available desc"
           ret=self.sqlserver.Query(sql);
           if(time.time()-cachetime < self.dbfsupdate() and len(ret)>0):
               return ret[0][0]
           sql="select disk,host,status,allowed  from filesystems"
           ret=self.sqlserver.Query(sql);
           for fs in ret:
               #
               #          check to see if it is can be readed
               #
               stf=fs[0]+".o"
               stf=re.sub(r'/','',stf)
               stf="/tmp/"+stf
               if(self.v):
                   print stf
               os.system("ls "+fs[0]+" 1>"+stf+" 2>&1 &")
               time.sleep(1)
               stat =os.stat(stf)
               os.unlink(stf)
               if stat[ST_SIZE]==0:
                   sql="update filesystems set isonline=0 where disk='"+str(fs[0])+"'"
                   if updatedb!=0:
                       self.sqlserver.Update(sql)
                   continue
               res=os.statvfs(fs[0])
               if len(res) == 0:
                   isonline=0
               else:
                   timestamp=int(time.time())
                   bsize=res[1]
                   blocks=res[2]
                   bfree=res[3]
                   bavail=res[4]
                   files=res[5]
                   ffree=res[6]
                   isonline=1
                   fac=float(bsize)/1024/1024
                   tot=blocks*fac
                   if(tot>1000000000):
                       status="Unknown"
                       isonline=0
                   if isonline:
                	status="Active"
                        occ=(blocks-bfree)*fac
                 	sql = "SELECT SUM(sizemb) FROM ntuples WHERE  PATH like '"+fs[0]+"%'"
                 	sizemb=self.sqlserver.Query(sql)
                 	rused=0
                        if len(sizemb)>0 and sizemb[0][0] != None:
                            rused=sizemb[0][0]
                        ava=bavail*fac
                        ava1=tot*float(fs[3])/100-rused
                        if (fs[2].find('Reserved')>=0):
                            status='Reserved'
                        elif ava1<0 or ava<3000:
                            status='Full'
                        if ava1< ava: ava=ava1;
                        if ava<0: ava=0
                        sql="update filesystems set isonline=1, totalsize="+str(tot)+", status='"+status+"',occupied="+str(occ)+",available="+str(ava)+",timestamp="+str(timestamp) +" where disk='"+fs[0]+"'"
                   else:
                    sql="update filesystems set isonline=0 where disk='"+fs[0]+"'"
                   if updatedb>0:
                    self.sqlserver.Update(sql)
                    self.sqlserver.Commit()
           sql="select disk from filesystems where isonline=1 and status='Active' order by available desc"
           ret=self.sqlserver.Query(sql)
           return ret[0][0]

    def  dblupdate(self):
           part=0
           sql="select sessiontimezone from dual"
           got=self.sqlserver.Query(sql)
           if len(got)>0:
              sh1=string.split(got[0][0], ':')
              if( len(sh1)>1):
                  part=(float(sh1[0])+float(sh1[1])/60.)/24.
                        
           sql="select max(date_to_unixts(modtime-"+str(part)+")) as u_modtime from mods where tabname != 'fs'"
           rdt=self.sqlserver.Query(sql)
           lupdate=-1
           if len(rdt)>0: lupdate=rdt[0][0]
           return lupdate

    def dbfsupdate(self):
           part=0
           sql="select sessiontimezone from dual"
           got=self.sqlserver.Query(sql)
           if len(got)>0:
              sh1=string.split(got[0][0], ':')
              if( len(sh1)>1):
                  part=(float(sh1[0])+float(sh1[1])/60.)/24.
           sql="select max(date_to_unixts(modtime-"+str(part)+")) as u_modtime from mods where tabname = 'fs'"
           rdt=self.sqlserver.Query(sql)
           lupdate=-1
           if len(rdt)>0: lupdate=rdt[0][0]
           return lupdate

    def DiskIsOnline(self,disk):
           self.CheckFS(1,300)
           sql="select disk from filesystems where isonline=1 and disk like '"+disk+"'"
           ret=self.sqlserver.Query(sql)
           if len(ret)>0: return 1
           return 0
    def sendmailmessage(self,add,sub,mes="",att=""):
        import mimetypes
        from email import encoders
        from email.message import Message
        from email.mime.base import MIMEBase
        from email.mime.image import MIMEImage
        from email.mime.multipart import MIMEMultipart
        from email.mime.text import MIMEText
        if(att == ""):
               message=MIMEText(mes)
               message['Subject']=sub
               message['To']=add
               message['From']='ams@mail.cern.ch'
               s=smtplib.SMTP()
               s.connect()
               s.sendmail(message['From'],message['To'],message.as_string())
               s.quit()
        else:
               message=MIMEMultipart()
               message['Subject']=sub
               message['To']=add
               message['From']='apache@pcamss0.cern.ch'
               ctype='text/plain'
               maintype, subtype = ctype.split('/', 1)
               msg=MIMEText(mes,_subtype=subtype)
               message.attach(msg)
               files=att.split(';')
               ctype='application/octet-stream'
               maintype, subtype = ctype.split('/', 1)
               for file in files:
                   path,name=file.split(',',1)
                   msg=MIMEBase(maintype,subtype)
                   fp = open(path)
                   msg.set_payload(fp.read())
                   fp.close()
                   encoders.encode_base64(msg)
                   msg.add_header('Content-Disposition', 'attachment', filename=name)
                   message.attach(msg)
               s=smtplib.SMTP()
               s.connect()
               s.sendmail(message['From'],message['To'],message.as_string())
               s.quit()
               
    def ValidateRuns(self,run2p,i,v,d,h):
        self.failedcp=0
        self.thrusted=0
        self.copied=0
        self.failedcp=0
        self.bad=0
        self.unchecked=0
        self.gbDST=0
        timenow=int(time.time())
        global rflag
        rglag=0
        self.validated=0
        HelpText="""
             validateRuns gets list of runs from production server
                  validates DSTs and copies them to final destination
                  update Runs and NTuples DB tables
     -h    - print help
     -i    - prompt before files removal
     -v    - verbose mode
     -rRun    -  perfom op only for run  Run;
     -d     -  delete completed runs/ntuiples from server
     ./validateRuns.py  -v
     """
        if(i ==1):
            self.rm="rm -i "
        else: self.rm="rm -f "
        if(d==1):
            self.delete=1
        else: self.delete=0 
        if(h==1):
            print HelpText
            return 1
        if(v==1):
            self.v=1
        else: self.v=0 
        self.BadCRC=[0]
        self.CheckedDSTs=[0]
        self.BadDSTs=[0]
        self.GoodDSTs=[0]
        self.BadDSTCopy=[0]
        self.doCopyTime=0
        self.doCopyCalls=0
        self.fastntTime=0
        self.fastntCalls=0
        self.crcTime=0
        self.crcCalls=0
        self.copyTime=0
        self.copyCalls=0
        self.nCheckedCite=0
        self.nBadCopiesInRow=0
        self.CheckedRuns=[0]
        self.GoodRuns=[0]
        self.BadRuns=[0]
        # first check evr is ok
        timenow=int(time.time())
        self.valStTime=timenow
        sql = "SELECT flag, timestamp from FilesProcessing"
        ret = self.sqlserver.Query(sql)
        if(ret[0][0]==1):
            print "ValidateRuns-E-ProcessingFlagSet on ",ret[0][1]," exiting"
            return 0
        else: self.setprocessingflag(1,timenow)
        global vdir,vlog,output
        vdir=self.getValidationDir()
        vlog=vdir+"validateRuns.log."+str(timenow)
        try:
            output=open(vlog,'w')
            print "ValidateRuns-I-Open ",vlog
        except IOError,e:
            print e
            self.setprocessingflag(0,timenow)
            return 0
        global vst
        vst=timenow
        ret=self.sqlserver.Query("SELECT min(begin) FROM productionset WHERE STATUS='Active' ORDER BY begin")
        if (len(ret)==0):
            print "ValidateRuns=E-CannotFindActiveProductionSet"
            self.setprocessingflag(0,timenow)
            return 0
        firstjobtime=ret[0][0]-24*60*60
        ret=self.sqlserver.Query("SELECT run,submit FROM Runs WHERE submit > "+str(firstjobtime))
        global exitmutexes
        exitmutexes = {}
        global mutex
        mutex=thread.allocate_lock()
        global fsmutexes
        fsmutexes = {}
        for run in self.dbclient.rtb:
            if(run2p!=0 and run.Run != run2p):
                continue
            exitmutexes[run.Run]=thread.allocate_lock()
            self.CheckedRuns[0]=self.CheckedRuns[0]+1
            thread.start_new(self.validaterun,(run,))
            #self.validaterun(run)
        for key in exitmutexes.keys():
            while not exitmutexes[key].locked():
                time.sleep(0.001)
                pass
        time.sleep(1)
        for key in exitmutexes:
            while not exitmutexes[key].locked():
                time.sleep(0.001)
                pass

        warn = """%s Ntuple(s) Successfully Validated.
                  %s Ntuple(s) Copied.
                  %s Ntuple(s) Not Copied.
                  %s Ntuple(s) Turned Bad.
                  %s Ntuples(s) Could Not Be Validated.
                  %s Ntuples Could Not be Validated But Assumed  OK.
              """ %(self.validated,self.copied,self.failedcp,self.bad,self.unchecked,self.thrusted)
        output.write(warn)
        output.close()
        self.printValidateStat()
        timenow=int(time.time())
        self.setprocessingflag(0,timenow)
        if(self.delete):
            for run in self.dbclient.rtb:
                status=self.dbclient.cr(run.Status)
                if(status=='Finished' or status=='Foreign' or status == 'Canceled'):
                    sql=" select ntuples.path from ntuples,runs where ntuples.run=runs.run and runs.status='Completed' and runs.jid=%d " %(run.Run)
                    ret=self.sqlserver.Query(sql)
                    if(len(ret)>0):
                        print " deleting ",run.Run
                        self.dbclient.iorp.sendRunEvInfo(run,self.dbclient.tm.Delete)
                    
              
                
        return 1
    def getValidationDir(self):
        sql="SELECT myvalue FROM Environment WHERE mykey='ValidationDirPath'"
        ret=self.sqlserver.Query(sql)
        if(len(ret)>0):
            return ret[0][0]
        else:
            return None
        
    def printValidateStat(self):
        timenow=int(time.time())
        vdir=self.getValidationDir()
        if(vdir == None):
            vdir="/tmp"
        vlog=vdir+"/validationRunsSummary.log."+str(timenow)
        try:
            output=open(vlog,'w')
        except IOError,e:
            print e
            return 0
        fl="------------- ValidateRuns ------------- "
        print fl
        output.write(fl)
        stime=time.asctime(time.localtime(self.valStTime))
        ltime=time.asctime(time.localtime(timenow))
        t0="Start Time : "+stime
        t1="End Time : "+ltime
        print t0,t1
        output.write(t0)
        output.write(t1)
        l0="   Runs (Listed, Completed, Bad, Failed) : %d,  %d, %d " %(self.CheckedRuns[0],self.GoodRuns[0],self.BadRuns[0])
        l1=" DSTs (Checked, Good, Bad, CRCi, CopyFail/CRC ) : %d, %d, %d, %d, %d) " %(self.CheckedDSTs[0], self.GoodDSTs[0], self.BadDSTs[0], self.BadCRC[0], self.BadDSTCopy[0])
        print l0
        print l1
        output.write(l0)
        output.write(l1)
        totalgb=self.gbDST/1024./1024./1024.
        chgb="Total GB %3.1f " %(totalgb)
        print chgb
        hours   = float(timenow - self.valStTime)/60./60.
        ch0="Total Time %3.1f hours " %(hours)
        ch1="doCopy (calls, time): %5d %3.1fh [cp file :%5d %3.1f] " %(self.doCopyCalls,float(self.doCopyTime)/60./60., self.copyCalls,float(self.copyTime)/3600.)
        ch2="CRC (calls,time) : %5d %3.1fh; Validate (calls,time) : : %5d %3.1fh " %(self.crcCalls,float(self.crcTime)/3600.,self.fastntCalls, float(self.fastntTime)/3600.)
        print ch0
        print ch1
        print ch2
        output.write(chgb)
        output.write(ch0)
        output.write(ch1)
        output.write(ch2)
        ll="-------------     Thats It          -------------"
        print ll
        output.write(ll)
        output.close()
        
    def validaterun(self,run):
        mutex.acquire()
        rmcmd=[]
        rmbad=[]
        cpntuples=[]
        mvntuples=[]
        timestamp=int(time.time())
        outputpath=None
	copyfailed=0
	runupdate="update runs set "
        ro=self.sqlserver.Query("select run, status from runs where run="+str(run.Run))
        if(len(ro)==0):
            self.InsertRun(run)
        ro=self.sqlserver.Query("select run, status from runs where run="+str(run.Run))
        r1=self.sqlserver.Query("select count(path)  from ntuples where run="+str(run.Run))
        status=ro[0][1]
        if(status== 'Completed' and r1[0][0]==0):
            status="Unchecked"
        if(status != 'Completed' and status != self.dbclient.cr(run.Status)):
            sql="update runs set status='%s' where run=%d" %(self.dbclient.cr(run.Status),run.Run)
            self.sqlserver.Update(sql)
        if(self.dbclient.cr(run.Status) == "Finished" and status != "TimeOut" and status !="Completed"):
            print "Check Run %d Status %s DBStatus %s" %(run.Run,self.dbclient.cr(run.Status),status)
            output.write("Check Run %d Status %s DBStatus %s" %(run.Run,self.dbclient.cr(run.Status),status))
            sql="select runs.status, jobs.content, cites.status from runs,jobs,cites where jobs.jid=%d and runs.jid=jobs.jid and cites.cid=jobs.cid" %(run.Run)
            r1=self.sqlserver.Query(sql)
            if(len(r1)==0):
                self.sqlserver.Update("update runs set status='Failed' where run="+str(run.Run))
                print "cannot find status, content in Jobs for JID=%d" %(run.Run)
                output.write( "cannot find status, content in Jobs for JID=%d" %(run.Run))
            else:
                jobstatus=r1[0][0]
                jobcontent=r1[0][1]
                citestatus=r1[0][2]
                # '-GR' runs
                if(jobcontent.find("-GR")>=0  or citestatus == "local"):
                    mips=run.cinfo.Mips
                    events=run.cinfo.EventsProcessed
                    errors=run.cinfo.CriticalErrorsFound
                    cputime=run.cinfo.CPUMipsTimeSpent
                    elapsed="%.2f" %(run.cinfo.TimeSpent)
                    host=run.cinfo.HostName
                    if(mips<=0):
                        print "Mips Problem for %d %s" %(mips, host)
                        mips=1000
                    cputime=cputime/mips*1000
                    cputime="%.2f" %(cputime)
                    if(events==0 and errors==0 and self.dbclient.cr(run.Status) == "Finished"):
                        print "run events 0 "
                        self.sqlserver.Update("update runs set status='Unchecked' where run="+str(run.Run))
                    else:
                        sql="update jobs set events=%d, errors=%d, cputime=%s,elapsed=%s,host='%s',mips=%d,timestamp=%d where jid=%d" %(events,errors,cputime,elapsed,host,int(mips),timestamp,run.Run)
                        self.sqlserver.Update(sql)
                        output.write(sql)
                        self.sqlserver.Update("delete ntuples where run="+str(run.Run))
                        ntuplelist=[]
                        for ntuple in self.dbclient.dsts:
                            #print ntuple.Run,run.Run,self.dbclient.cn(ntuple.Status)
                            if( (self.dbclient.cn(ntuple.Status) == "Success" or  self.dbclient.cn(ntuple.Status) == "Validated") and ntuple.Run == run.Run):
                                ntuplelist.append(ntuple)
                        ntuplelist.sort(lambda x,y: cmp(y.Insert,x.Insert))
                        fevt=-1
                        dat0=0
                        for ntuple in ntuplelist:
                            if(fevt>=0 and ntuple.LastEvent>fevt):
                                ntuple.EventNumber=-1
                                print " problems with %d %s " %(run.Run,ntuple.Name)
                            else:
                                fevt=ntuple.FirstEvent
                                if(str(ntuple.Name).find(':/dat0')>=0):
                                    dat0=1
                        fevent=1
                        levent=0
                        for ntuple in ntuplelist:
                            if(ntuple.EventNumber==-1):
                                fpatha=str(ntuple.Name).split(':')
                                fpath=fpatha[len(fpatha)-1]
                                rmbad.append("rm "+fpath)
                            else:
                                if(str(ntuple.Name).find(':/dat0')>=0):
                                    ntuple.Status=self.dbclient.iorp.Success
                                self.CheckedDSTs[0]=self.CheckedDSTs[0]+1
                                levent=levent+ntuple.LastEvent-ntuple.FirstEvent+1
                                fpatha=str(ntuple.Name).split(':')
                                fpath=fpatha[len(fpatha)-1]
                                badevents=ntuple.ErrorNumber
                                events=ntuple.EventNumber
                                status="OK"
                                rcp=1
                                if( not os.path.isfile(fpath)):
                                        # try to copy to local dir
                                        parser=str(ntuple.Name).split(':')
                                        if(len(parser)>1):
                                           host=parser[0]
                                           hostok=0
                                           if(host.find('.cern.ch')>=0):
                                               hostok=1
                                           else:
                                               hparser=host.split('.')
                                               if(len(hparser)>1 and hparser[1]=='om'):
                                                   host=hparser[0]+".cern.ch"
                                                   hostok=1
                                               else:
                                                   print "host bizarre ",host
                                           if(hostok):
                                               dir=fpath.split(str(run.Run))
                                               cmd="mkdir -p "+dir[0]
                                               i=os.system(cmd)
                                               cmd="scp -2 %s:%s %s " %(host,fpath,fpath)
                                               mutex.release()
                                               i=os.system(cmd)
                                               mutex.acquire()
                                               if(i):
                                                   print cmd," failed"
                                                   rcp=0
                                                   continue
                                               else:
                                                   cmd="ssh -x -2 "+host+" rm "+fpath
                                                   rmcmd.append(cmd)
                                        if(not os.path.isfile(fpath)):
                                            print "unable to open file ",fpath
                                            continue
                                retcrc=self.calculateCRC(fpath,ntuple.crc)
                                if(retcrc !=1):
                                            print "ValidateRuns-E_Error-CRC status ",retcrc
                                            output.write("ValidateRuns-E_Error-CRC status "+str(retcrc))
                                            self.BadCRC[0]=self.BadCRC[0]+1
                                            self.BadDSTs[0]=self.BadDSTs[0]+1
                                            self.bad=self.bad+1
                                            levent=levent-(ntuple.LastEvent-ntuple.FirstEvent+1)
                                            rmbad.append("rm "+fpath)
                                            mvntuples.append(fpath)
                                else:
                                            (ret,i)=self.validateDST(fpath,ntuple.EventNumber,self.dbclient.ct(ntuple.Type),ntuple.LastEvent)
                                            if( i == 0xff00 or (i & 0xff)):
                                                if(ntuple.Status != self.dbclient.iorp.Validated):
                                                    status="Unchecked"
                                                    events=ntuple.EventNumber
                                                    self.unchecked=self.unchecked+1
                                                    copyfailed=1
                                                    break
                                                else:
                                                    self.thrusted=self.thrusted+1
                                            else:
                                                i= i>>8
                                                if(i/128):
                                                    events=0
                                                    status="Bad"+str(i-128)
                                                    self.bad=self.bad+1
                                                    levent=levent-(ntuple.LastEvent-ntuple.FirstEvent+1)
                                                    rmbad.append("rm "+fpath)
                                                else:
                                                    status="OK"
                                                    events=ntuple.EventNumber
                                                    badevents=(i*events/100)
                                                    self.validated=self.validated+1
                                                    (outputpatha,rstatus,odisk)=self.doCopy(run.Run,fpath,ntuple.crc,ntuple.Version,outputpath)
						    outputpath=outputpatha[:]
                                                    if(outputpath != None):
                                                        mvntuples.append(outputpath)
                                                    if(rstatus==1):
                                                        self.GoodDSTs[0]=self.GoodDSTs[0]+1
                                                        self.nBadCopiesInRow=0
                                                        self.InsertNtuple(run.Run,ntuple.Version,self.dbclient.ct(ntuple.Type),run.Run,ntuple.FirstEvent,ntuple.LastEvent,events,badevents,ntuple.Insert,ntuple.size,status,outputpath,ntuple.crc,ntuple.Insert,1,0)
                                                        output.write("insert %d %s %s %d" %(run.Run, outputpath, status,int(ntuple.size)))
                                                        self.copied=self.copied+1
                                                        self.gbDST=self.gbDST+ntuple.size
                                                        cpntuples.append(fpath)
                                                        runupdate="update runs set fevent="+str(fevent)+", levent="+str(levent)+", "
                                                    else:
                                                        self.BadDSTs[0]=self.BadDSTs[0]+1
                                                        self.BadDSTCopy[0]=self.BadDSTCopy[0]+1
                                                        output.write("failed to copy or wrong crc for %s" %(fpath))
                                                        copyfailed=1
                                                        self.nBadCopiesInRow=self.nBadCopiesInRow+1
                                                        if(self.nBadCopiesInRow>3):
                                                            output.write("too many docopy failures")
                                                            print "too many docopy failurs"
                                                            os.exit()
                                                        levent=levent-(ntuple.LastEvent-ntuple.FirstEvent+1)
                                                        self.bad=self.bad+1
                                                        if(outputpath != None):
                                                            cmd="rm "+outputpath
                                                            rstat=os.system(cmd)
                                                            output.write("remove bad file "+cmd)

                        status="Failed"
                        fsmutexes[odisk].release()
                        if(copyfailed==0):
                            warn="Validation done Run %d " %(run.Run)
                            print warn
                            output.write(warn)
                        if(len(cpntuples)>0 and len(mvntuples)>0):
                               status='Completed'
                        elif(len(mvntuples)>0):
                             warn="Validation/copy failed run %d " %(run.Run)
                             print warn
                             output.write(warn)
                             status="Unchecked"
                             for ntuple in mvntuples:
                                 cmd="rm "+ntuple
                                 os.system(cmd)
                                 print "Validation failed "+cmd
                                 output.write("Validation failed "+cmd)
                                 self.failedcp=self.failedcp+1
                                 self.copied=self.copied-1
                             sql="delete ntuples where run=%d" %(run.Run)
                             self.sqlserver.Update(sql)
                             runupdate="update runs set "
                             
                        else:
                            status="Completed"
                        if(status == "Completed"):
                            self.GoodRuns[0]=self.GoodRuns[0]+1
                        elif (status =="Failed"):
                            self.BadRuns[0]=self.BadRuns[0]+1
                        sql= "update runs set status='%s' where run=%d" %(status,run.Run)
                        self.sqlserver.Update(sql)
                        sql="select sum(ntuples.levent-ntuples.fevent+1),min(ntuples.fevent)  from ntuples,runs where ntuples.run=runs.run and runs.run="+str(run.Run)
                        r4=self.sqlserver.Query(sql)
                        if(len(r4)==0):
                            ntevt=0
                            fevt=0
                        else:
                            ntevt=r4[0][0]
                            fevt=r4[0][1]
                        if(ntevt != run.LastEvent-run.FirstEvent+1):
                            print "  ntuples/run mismatch %d %d %d " %(ntevt,run.LastEvent-run.FirstEvent+1,run.Run)
                        sql="update runs set fevent=%d, levent=%d where jid=%d" %(fevt,ntevt-1+fevt,run.Run)
                        self.sqlserver.Update(sql)
                        sql="update jobs set realtriggers=%d,timekill=0 where jid=%d" %(ntevt,run.Run)
                        self.sqlserver.Update(sql)
                        for ntuple in cpntuples:
                            cmd="rm "+ntuple
                            for mn in mvntuples:
                                if(ntuple == mn):
                                    cmd=" "
                                    break
                            os.system(cmd)
                            warn=" validation done "+cmd
                            output.write(warn)
                            print warn
                        
                        if(status =="Completed"):
                            self.updateRunCatalog(run.Run)
                            warn=" Update RunCatalog table "+str(run.Run)
                            for cmd in rmcmd:
                                i=os.system(cmd)
                                if(i):
                                    print "Remote command ",cmd," failed"
                            del rmcmd[:]
                            for cmd in rmbad:
                                i=os.system(cmd)
                                if(i):
                                    print " remove bad ntuples command failed ",cmd
                            del rmbad [:]
        del cpntuples[:]
        del mvntuples[:]
	self.sqlserver.Commit()
        mutex.release()
        exitmutexes[run.Run].acquire()

    def setprocessingflag(self,flag,timenow):
        sql="Update FilesProcessing set flag="+str(flag)+",timestamp="+str(timenow)
        self.sqlserver.Update(sql)
        self.sqlserver.Commit()


    def getValidationDir(self):
        sql="SELECT myvalue FROM Environment WHERE mykey='ValidationDirPath'"
        return self.sqlserver.Query(sql)[0][0]

    def ServerConnect(self):
        try:
            ior=self.getior()
            self.dbclient=DBServer(ior)
            return 1
        except:
            print "Problem to ConnectServer "
            return 0
    
    def doCopy(self,run,inputfile,crc,version,outputpath):
       time0=time.time()
       time00=0
       (dbv,gbv,osv)=self.getDSTVersion(version)
       (period,prodstarttime,periodid)=self.getActiveProductionPeriodByVersion(dbv)
       if(period == None or prodstarttime==0):
           sys.exit("Cannot find Active Production Period for DB version "+str(dbv))
       self.doCopyCalls=self.doCopyCalls+1
       junk=inputfile.split('/')
       file=junk[len(junk)-1]
       filesize=os.stat(inputfile)[ST_SIZE]
       if(filesize>0):
           #get output disk
           if(outputpath == None):
               stime=100
               odisk=None
               while(stime>60):
                   if(odisk!=None):
                       fsmutexes[odisk].release()
                   (outputpatha,gb,odisk,stime)=self.getOutputPath(period)
                   print "acquired:  ",outputpatha,gb,odisk,stime
               outputpath=outputpatha[:]
               if(outputpath.find('xyz')>=0 or gb==0):
                   sys.exit("doCopy-F-CannotFindAnyDisk Exit")
           else:
               junk=outputpath.split('/')
               odisk="/"+junk[1]
               outputpath=""
               for i in range (0,len(junk)-4):
                   if(i>0):
                       outputpath=outputpath+"/"
                   outputpath=outputpath+junk[i]
           #find job
           sql="SELECT cites.name,jobname  FROM jobs,cites WHERE jid =%d AND cites.cid=jobs.cid" %(run)
           r1=self.sqlserver.Query(sql)
           if(len(r1)>0):
                   cite=r1[0][0]
                   jobname=r1[0][1]
                   dataset="unknown"
                   sql = "SELECT jobs.jid, datasets.name  FROM datasets, jobs WHERE jid =%d AND jobs.did=datasets.did" %(run)
                   r2=self.sqlserver.Query(sql)
                   if(len(r2)>0):
                       dataset=r2[0][1]
                       jobname=re.sub(cite+".",'',jobname,1)
                       jobname=re.sub(str(run)+".",'',jobname,1)
                       jobname=re.sub('.job','',jobname,1)
                       outputpath=outputpath+'/'+dataset+'/'+jobname
                       cmd="mkdir -p "+outputpath
                       cmdstatus=os.system(cmd)
                       if(cmdstatus==0):
                           outputpath=outputpath+"/"+file
                           cmdstatus=self.copyFile(inputfile,outputpath)
                           time00=time.time()
                           self.doCopyTime=self.doCopyTime+time00-time0
                           if(cmdstatus==0):
                               rstatus=self.calculateCRC(outputpath,crc)
                               if(rstatus==1):
                                   return outputpath,1,odisk
                               else:
                                   print "doCopy-E-ErorrCRC ",rstatus
                                   BadCRC[self.nCheckedCite]=BadCRC[self.nCheckedCite]+1
                                   return outputpath,0,odisk
                           BadDSTCopy[self.nCheckedCite]=BadDSTCopy[self.nCheckedCite]+1
                           print "docopy-E-cannot ",cmd
                           return outputpath,0,odisk
       else:
           print "doCopy-E-cannot stat",inputfile
           BadDSTs[self.nCheckedCite]=BadDSTs[self.nCheckedCite]+1
           return None,0,odisk
    
    def getActiveProductionPeriodByVersion(self,dbv):
       ret=self.sqlserver.Query("SELECT NAME, BEGIN, DID  FROM ProductionSet WHERE STATUS='Active' and vdb='"+dbv+"'")
       if(len(ret)>0):
           return self.trimblanks(ret[0][0]),ret[0][1],ret[0][2]
       else:
          return None,0,0
      
    def trimblanks(self,expr):
        return expr.lstrip().rstrip()
       
                              
    def getDSTVersion(self,version):
        junk=version.split('/')
        if(len(junk)>2):
            return self.trimblanks(junk[0]),self.trimblanks(junk[1]),self.trimblanks(junk[2])
        else:
            return self.trimblanks(junk[0]),None,None
       
    def getOutputPath(self,period):
        #
        # select disk to be used to store ntuples
        #
        self.CheckFS(1)
        tme=int(time.time())
        if(tme%2 ==0):
            sql="SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 ORDER BY priority DESC, available "
        else:
            sql = "SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 ORDER BY priority DESC, available DESC"
        ret=self.sqlserver.Query(sql)
        for disk in ret:
            outputdisk=self.trimblanks(disk[0])
            outputpath=self.trimblanks(disk[1])
            if(outputdisk.find('vice')>=0):
                outputpath=outputpath+"/"+period
            else:
                outputpath=outputdisk+outputpath+"/"+period
            os.system("mkdir -p "+outputpath)
            mtime=os.stat(outputpath)[9]
            if(mtime != None):
                if(mtime!=0):
                    gb=disk[2]
                    break
        timew=time.time()
        if(fsmutexes.has_key(outputdisk)):
            print "acquirng fs mutex for ",outputdisk
            mutex.release()
            fsmutexes[outputdisk].acquire()
            mutex.acquire()
            print "got fs mutex for ",outputdisk
        else:
            fsmutexes[outputdisk]=thread.allocate_lock()
            mutex.release()
            print "acquirng first fs mutex for ",outputdisk
            fsmutexes[outputdisk].acquire()
            mutex.acquire()
            print "got first fs mutex for ",outputdisk
        return outputpath,gb,outputdisk,time.time()-timew
           
           
    def copyFile(self,input,output):
        mutex.release()
        if(input == output):
            print "acquirung  mutex in copyfile ups"
            mutex.acquire()
            return 0
        time0=time.time()
        cmd="cp -pi -d -v "+input+" "+output
        cmdstatus=os.system(cmd)
        print "acquirung  mutex in copyfile ", cmd
        mutex.acquire()
        print "got  mutex in copyfile ", cmd
        if ( self.v ):
            print "docopy-I-",cmd
        self.copyCalls=self.copyCalls+1
        self.copyTime=self.copyTime+time.time()-time0
        return cmdstatus

    def updateRunCatalog(self,run):
        timestamp=int(time.time())
        sql = "SELECT runs.status, jobs.content FROM Runs, Jobs WHERE runs.jid=jobs.jid and jobs.jid="+str(run)
        r0=self.sqlserver.Query(sql)
        if(len(r0)>0):
            runstatus=r0[0][0]
            jobcontent=r0[0][1]
            if(runstatus == "Completed"):
                sql="delete runcatalog where run=%d" %(run)
                if(self.v):
                    print sql
                self.sqlserver.Update(sql)
                sbuf=jobcontent.split("\n")
                sql0="insert into runcatalog(run"
                sql1="   values(%d" %(run)
                farray=("PART","PMIN","PMAX","TRIGGER","SPECTRUM","SETUP","FOCUS","COSMAX","PLANENO","GEOCUTOFF","COOCUB1","COOCUB2","COOCUB3","COOCUB4","COOCUB5","COOCUB6")
                narray=(1,1,1,0,0,0,0,1,0,0,1,1,1,1,1,1)
                i=0
                for ent in farray:
                    for line in sbuf:
                        if(line.find(ent+"=")>=0):
                            junk=line.split(ent+"=",1)
                            if(junk[0]!="" and junk[1]!=""):
                                if(ent=="TRIGGER"):
                                    sql0=sql0+", TRTYPE"
                                else:
                                    sql0=sql0+ent
                                if(narray[i]==0):
                                    sql1=sql1+", '"+junk[len(junk)-1]+"'"
                                else:
                                    sql1=sql1+", "+junk[len(junk)-1]
                            else:
                                if(ent=="SETUP"):
                                    sql0=sql0+", SETUP"
                                    sql1=sql1+", 'AMS02'"
                                if(ent=="TRIGGER"):
                                    sql0=sql0+", TRTYPE"
                                    sql1=sql1+", 'TriggerLVL1'"
                            break
                    i=i+1
                for line in sbuf:
                    if(line.find("generate")>=0):
                        line=line.replace("C ","",1)
                        line=line.replace("\\","",1)
                        line=line.replace("\\","",1)
                        line=self.trimblanks(line)
                        sql0=sql0+", jobname"
                        sql1=sql1+", '"+line+"'"
                        break
                sql0=sql0+", TIMESTAMP) "
                sql1=sql1+", "+str(timestamp)+")"
                sql=sql0+sql1
                self.sqlserver.Update(sql)
            else:
                print "UpdateRunCatalog-E-CannitFindRunJobContent with jid=",run
    
              
    def InsertNtuple(self,run,version,type,jid,fevent,levent,events,errors,timestamp,size,status,path,crc,crctime,crcflag,castortime):
        junk=path.split("/")
        filename=self.trimblanks(junk[len(junk)-1])
        sp1=version.split('build')
        sp2=sp1[1].split('/')
        buildno=sp2[0]
        sql="SELECT run, path FROM ntuples WHERE run=%d AND path like '%%%s%%'" %(run,filename)
        ret=self.sqlserver.Query(sql)
        if(len(ret)>0):
            sql="DELETE ntuples WHERE run=%d AND path like  '%%%s%%'" %(run,filename)
            print sql,ret[0][1]
        ntsize=float(size)/1024/1024
        sizemb="%.f" %(ntsize)
        sql = "INSERT INTO ntuples VALUES( %d, '%s','%s',%d,%d,%d,%d,%d,%d,%s,'%s','%s',%d,%d,%d,%d,%s)" %(run,version,type,jid,fevent,levent,events,errors,timestamp,sizemb,status,path,crc,crctime,crcflag,castortime,buildno)
        self.sqlserver.Update(sql)
   
    def setenv(self):
        if(os.environ.has_key('AMSDataDir')):
            dir=os.environ['AMSDataDir']
        else:
            dir="/afs/ams.cern.ch/AMSDataDir"
            os.environ['AMSDataDir']=dir
        self.env['AMSDataDir']=dir
        key='AMSSoftwareDir'
        sql="select myvalue from Environment where mykey='"+key+"'";
        ret=self.sqlserver.Query(sql)
        if( len(ret)>0):
            self.env[key]=self.env['AMSDataDir']+"/"+ret[0][0]
        else:
            print "AMSSoftwareSir Not Defined "
            return 0
        return 1
   
    def validateDST(self,fname,nevents,ftype,levent):
        time0=time.time()
        ret=0
        dtype=None
        if(ftype == "Ntuple"):
            dtype=0
        elif(ftype=="RootFile"):
            dtype=1
        if(dtype!=None):
            mutex.release()
            validatecmd="/exe/linux/fastntrd.exe %s %d %d %d " %(fname,nevents,dtype,levent)
            validatecmd=self.env['AMSSoftwareDir']+validatecmd
            vcode=os.system(validatecmd)
            print "acquirung  mutex in validate", validatecmd
            mutex.acquire()
            print "got  mutex in validate", validatecmd
            ret=1
        self.fastntCalls=self.fastntCalls+1
        self.fastntTime=self.fastntTime+time.time()-time0
        return ret,vcode
    
    def InsertRun(self,Run):
            run=Run.Run
            jid=Run.Run
            fevent=Run.FirstEvent
            levent=Run.LastEvent
            fetime=Run.TFEvent
            letime=Run.TLEvent
            submit=Run.SubmitTime
            status=self.dbclient.cr(Run.Status)
            sql="SELECT run, jid, fevent, levent, status FROM Runs WHERE run=%d" %(run)
            ret=self.sqlserver.Query(sql)
            doinsert=0
            if(len(ret)>0):
                dbrun=ret[0][0]
                dbjid=ret[0][1]
                dbfevent=ret[0][2]
                dblevent=ret[0][3]
                dbststus=ret[0][4]
                if(dbjid==run and dbfevent==fevent and dblevent==levent and dbstatus==status):
                    print "InsertRun-E-",run,"AlreadyExists"
                else:
                    sql="DELETE runs WHERE run=%d" %(run)
                    self.sqlserver.Update(sql)
                    doinsert=1
            else:
                doinsert=1
            if doinsert==1:
                sql="INSERT INTO Runs VALUES(%d,%d,%d,%d,%d,%d,%d,'%s')" %(run,jid,fevent,levent,fetime,letime,submit,status)
                self.sqlserver.Update(sql)
                if(self.v):
                    print sql
                if(status=="Completed"):
                    if(self.v):
                        print "Update RunCatalog ",run

       
    def calculateCRC(self,filename,crc):
        self.crcCalls=self.crcCalls+1
        time0=time.time()
        mutex.release()
        crccmd=self.env['AMSSoftwareDir']+"/exe/linux/crc "+filename+" "+str(crc)
        rstatus=os.system(crccmd)
        rstatus=rstatus>>8
        print "acquirung  mutex in calccrc", filename
        mutex.acquire()
        print "got  mutex in calccrc", filename
        if(self.v):
            print crccmd,rstatus
        self.crcTime=self.crcTime+time.time()-time0
        return rstatus
        
        
        
