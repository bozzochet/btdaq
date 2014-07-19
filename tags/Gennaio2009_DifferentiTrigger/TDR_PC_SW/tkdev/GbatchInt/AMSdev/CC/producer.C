//  $Id: producer.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include <unistd.h>
#include <stdlib.h>
#include "producer.h"
#include "cern.h"
#include "commons.h" 
#include <stdio.h>
#include <iostream.h>
#include "event.h"
#include "job.h"
#include<algorithm>
#include <sys/statfs.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/file.h>
AMSProducer * AMSProducer::_Head=0;
AString * AMSProducer::_dc=0; 
AMSProducer::AMSProducer(int argc, char* argv[], int debug) throw(AMSClientError):AMSClient(),AMSNode(AMSID("AMSProducer",0)),_RemoteDST(false),_OnAir(false),_FreshMan(true),_Local(true),_Solo(false),_Transfer(false){
DPS::Producer_var pnill=DPS::Producer::_nil();
_plist.push_back(pnill);
if(_Head){
 FMessage("AMSProducer::AMSProducer-E-AMSProducerALreadyExists",DPS::Client::CInAbort);
}
else{
 char * ior=0;
 uinteger uid=0;
 float ClockCor=1;
 for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--)){
    pchar++;
    switch (*pchar){
    case 'I':   //ior
     ior=pchar;
     break;
    case 'D':   //debug
     _debug=atoi(++pchar);
     break;
    case 'M':   //debug
     _MT=true;
     break;
    case 'U':   //uid
     uid=atol(++pchar);
     break;
    case 'C':   //correction factor to computer clock 
     ClockCor=atol(++pchar)/100.;
     if(ClockCor>0 && ClockCor<5){
      AMSFFKEY.CpuLimit*=ClockCor;
      if(ClockCor>1)GCTIME.TIMEND*=ClockCor;
      cout <<" AMSProducer::AMSProducer-I-CPULimitChangedTo "<<AMSFFKEY.CpuLimit<<endl;
     }
     break;
    case 'G':   //local
     _Local=false;
     if(pchar+1 && *(pchar+1)=='R'){
        _RemoteDST=true;
        cout<< "AMSProducer::AMSProducer-I-RemoteDSTSelected "<<endl;
     }
     break;
    case 'A': //amsdatadir
      setenv("AMSDataDir",++pchar,1);
      break;
    case 'S':   // Solo, no IOR 
      if(pchar+1 && *(pchar+1)=='Y'){
       _Solo=true;
      }
      break;
  }
 }
  if(!ior ){
    
   if(_debug){
     if(AMSJob::gethead()->isSimulation())_openLogFile("MCProducer",_Solo);
     else _openLogFile("Producer",_Solo);
   }
//   _Head=this;
// return;
   FMessage("AMSProducer::AMSProducer-F-NoIOR",DPS::Client::CInAbort);
  }
  _orb= CORBA::ORB_init(argc,argv);
  try{
   CORBA::Object_var obj=_orb->string_to_object(ior);
   if(!CORBA::is_nil(obj)){
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
    if(!CORBA::is_nil(_pvar)){
     _plist.clear();
     _plist.push_front(_pvar);
  if(!_getpidhost(uid)){
   if(_debug){
     if(AMSJob::gethead()->isSimulation())_openLogFile("MCProducer",_Solo);
     else _openLogFile("Producer",_Solo);
   }
      FMessage("AMSProducer::AMSProducer-F-UnableToGetHostName", DPS::Client::CInAbort);
 }
   if(_debug){
     if(AMSJob::gethead()->isSimulation())_openLogFile("MCProducer",_Solo);
     else _openLogFile("Producer",_Solo);
   }
     _Head=this;
     return;
}
}

}
  catch (CORBA::SystemException & a){
  FMessage("AMSProducer::AMSProducer-F-CORBA::SystemError",DPS::Client::CInAbort);
  }

}
FMessage("AMSProducer::AMSProducer-F-UnableToInitCorba",DPS::Client::CInAbort);

}

void AMSProducer::sendid(){
    _pid.Mips=AMSCommonsI::getmips();
    cout <<"  Mips:  "<<_pid.Mips<<endl;
    bool ok=SetDataCards();
if (_Solo){
      _pid.Type=DPS::Producer::Standalone;
      _pid.StatusType=DPS::Producer::OneRunOnly;
      LMessage(AMSClient::print(_pid,"JobStarted"));
      if(!ok ){
       cerr<<" AMSProducer::sendid-F-UnableToSetDataCards "<<endl;
       abort();
      } 
      else{
             LMessage(AMSClient::print(_pid,"MetaDataAdded")); 
      }
      return;
}
else if(!ok){
        AString pc="AMSProducer::sendid-F-UnableToSetDataCards ";
        pc+=(const char*)_pid.Interface;
        FMessage((const char*)pc,DPS::Client::CInAbort);
}


again:

     list<DPS::Producer_var>::iterator li = _plist.begin();
     time_t cput=5*AMSFFKEY.CpuLimit;
     time_t cpug=12*3600;
     if(!IsLocal() && cput<cpug)cput=cpug; 
     cout <<"   TimeOut sending "<<cput<<endl;
     cout <<"  Mips:  "<<_pid.Mips<<endl;
  try{
     if(!((*li)->sendId(_pid,_pid.Mips,cput))){
       if(_pid.uid)sleep(10);
       else{
        // dieHard
        AString pc="Server Requested Termination after sendID Because Of ";
        pc+=(const char*)_pid.Interface;
        FMessage((const char*)pc,DPS::Client::SInAbort);
       }
      if(!((*li)->sendId(_pid,_pid.Mips,cput))){
       // dieHard
       AString pc="Server Requested Termination after sendID Because Of ";
       pc+=(const char*)_pid.Interface;
       FMessage((const char*)pc,DPS::Client::SInAbort);
      }
     }
         cout <<"  Mips:  "<<_pid.Mips<<endl;
     IMessage(AMSClient::print(_pid,"sendID-I-Success"));
     LMessage(AMSClient::print(_pid,"JobStarted"));
            cout <<"  Mips:  "<<_pid.Mips<<endl;

      return;       
     }
   catch (CORBA::MARSHAL a){
   if(getior("GetIorExec"))goto again;
   else FMessage("AMSProducer::AMSProducer-F-CORBA::MARSHAL",DPS::Client::CInAbort);
  }
  catch (CORBA::COMM_FAILURE a){
   if(getior("GetIorExec"))goto again;
   else FMessage("AMSProducer::AMSProducer-F-CORBA::COMM_FAILURE",DPS::Client::CInAbort);
  }
  catch (CORBA::SystemException & a){
   if(getior("GetIorExec"))goto again;
   else FMessage("AMSProducer::AMSProducer-F-CORBA::SystemError",DPS::Client::CInAbort);
  }
}




void AMSProducer::getRunEventInfo(){
if (_Solo){
    struct timeb  ft;
    ftime(&ft);
    _ST0=ft.time+ft.millitm/1000.;
    if(_debug)cout <<"ST0 "<<_ST0<<endl;
    TIMEX(_T0);
    if(_debug)cout <<"T0 "<<_T0<<endl;
     _reinfo =new DPS::Producer::RunEvInfo(); 
     _dstinfo =new DPS::Producer::DSTInfo(); 
     _dstinfo->UpdateFreq=1000;
     if(IOPA.WriteRoot)_dstinfo->type = DPS::Producer::RootFile;
     else _dstinfo->type = DPS::Producer::Ntuple;
     _reinfo->uid=0;
     _reinfo->Priority=0;
     time_t ct;
     time(&ct);
     _reinfo->SubmitTime=ct;
     _reinfo->cuid=_pid.uid;
//     _reinfo->pid=_pid.pid;
     _reinfo->cinfo.HostName=_pid.HostName; 
     _reinfo->cinfo.Status=DPS::Producer::Processing;
   if(AMSJob::gethead()->isSimulation()){
    _reinfo->FirstEvent=GCFLAG.IEVENT+1;
    _reinfo->LastEvent=GCFLAG.NEVENT;
    _reinfo->Run=CCFFKEY.run;
    _reinfo->Status=DPS::Producer::Allocated;
    _reinfo->History=DPS::Producer::Foreign;
    _reinfo->CounterFail=0;
   }
    SELECTFFKEY.Run=_reinfo->Run;
    //SELECTFFKEY.Event=_reinfo->FirstEvent;
    SELECTFFKEY.RunE=_reinfo->Run;
    //SELECTFFKEY.EventE=_reinfo->LastEvent;    
    _cinfo.Mips=AMSCommonsI::getmips();
    _cinfo.EventsProcessed=0;
    _cinfo.ErrorsFound=0;
    _cinfo.Status=DPS::Producer::Processing;
    _cinfo.CPUTimeSpent=0;
    _cinfo.CPUMipsTimeSpent=0;
    _CPUMipsTimeSpent=0;
    _cinfo.TimeSpent=0;
    _cinfo.HostName=_pid.HostName; 
    _cinfo.Run=_reinfo->Run;
    _reinfo->cinfo=_cinfo;
     LMessage(AMSClient::print(_reinfo,"StartingRun"));
  return;
}


UpdateARS();
bool mtry=false; 
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
    (*li)->getRunEvInfo(_pid,_reinfo,_dstinfo);
         
    if(_dstinfo->DieHard){
     if(!mtry){
      cerr <<" problem with runevinfo sleep 20 sec "<<endl;
       sleep(20);
       mtry=true;
       goto again;
    } 
      cerr <<" DieHard "<<_dstinfo->DieHard<<endl; 
      if(!_FreshMan && _dstinfo->DieHard==1)FMessage("AMSProducer::getRunEventinfo-I-ServerRequestedExit",DPS::Client::SInExit);
      else FMessage("AMSProducer::getRunEventinfo-I-ServerRequestedAbort",DPS::Client::SInAbort);
    }
    _FreshMan=false;
    _cinfo.Run=_reinfo->Run;
    _cinfo.HostName=_pid.HostName; 
    SELECTFFKEY.Run=_reinfo->Run;
    //SELECTFFKEY.Event=_reinfo->FirstEvent;
    SELECTFFKEY.RunE=_reinfo->Run;
    //SELECTFFKEY.EventE=_reinfo->LastEvent;    
    _cinfo.Mips=AMSCommonsI::getmips();
    _cinfo.EventsProcessed=(_reinfo->cinfo).EventsProcessed;
    _cinfo.ErrorsFound=(_reinfo->cinfo).ErrorsFound;
    _cinfo.CriticalErrorsFound=(_reinfo->cinfo).CriticalErrorsFound;
    _cinfo.Status=DPS::Producer::Processing;
    _cinfo.CPUTimeSpent=0;
    _cinfo.CPUMipsTimeSpent=(_reinfo->cinfo).CPUMipsTimeSpent;
    _CPUMipsTimeSpent=_cinfo.CPUMipsTimeSpent;
    cout << "  _cinfo CPUMIPS "<<_cinfo.CPUMipsTimeSpent<<" "<<_cinfo.EventsProcessed<<endl;
    _cinfo.TimeSpent=0;

   if(AMSJob::gethead()->isSimulation()){
    GCFLAG.IEVENT=_reinfo->FirstEvent;
/*
    if(GCFLAG.IEVENT>1 ){
     // should call the rndm odnako
     cerr<<"AMSProducer::getRunEventInfo-W-ChangingRNDMBecauseFirstEventNumberNotOne"<<endl;
     for(int i=1;i<GCFLAG.IEVENT;i++){
      geant dum;
      RNDM(dum);
     }
    }
*/
    if(GCFLAG.IEVENT>1 ){
    if(!_reinfo->rndm1 ||  !_reinfo->rndm2){

     cerr<<"AMSProducer::getRunEventInfo-W-FirstEventNumberNotOneButRNDMIsZero"<<endl;
     for(int i=1;i<GCFLAG.IEVENT;i++){
      geant dum;
      RNDM(dum);
     }

    }
    else{
      cout<<"AMSProducer::getRunEventInfo-W-ChangingRNDMBecauseFirstEventNumberNotOne "<<_reinfo->rndm1<<" "<<_reinfo->rndm2<<endl;
       GCFLAG.NRNDM[0]=_reinfo->rndm1;      
       GCFLAG.NRNDM[1]=_reinfo->rndm2;
       AMSmceventg::RestoreSeeds();      
    }
    }
    GCFLAG.NEVENT=_reinfo->LastEvent;
    CCFFKEY.run=_reinfo->Run;
   }
   else{

    if(_dstinfo->Mode==DPS::Producer::RILO || _dstinfo->Mode==DPS::Producer::RIRO){ 
     DAQEvent::setfile((const char *)(_reinfo->FilePath));
}
else{
     AString fpath=(const char *)_dstinfo->OutputDirPath;
     AString fmake="mkdir -p ";
     fmake+=fpath;
     system((const char*)fmake);
     fpath+="/run.";
     char tmp[80];
     sprintf(tmp,"%d",_reinfo->Run);
     fpath+=tmp;
     fpath+=".";
     sprintf(tmp,"%d",_pid.uid);
     fpath+=tmp;
     DAQEvent::setfile((const char *)(fpath));
     ofstream fbin;
     fbin.open((const char*)fpath);
     if(!fbin){
       cerr <<fpath<<endl;
       FMessage("Unable to open tmp run file ", DPS::Client::CInAbort); 
     }
     DPS::Producer::TransferStatus st=DPS::Producer::Begin;
     DPS::Producer::RUN * prun;
     DPS::Producer::FPath fp;
     fp.fname=(const char*)_reinfo->FilePath;
     fp.pos=0;   
     while(st!=DPS::Producer::End){
     try{
      int length=(*li)->getRun(_pid,fp,prun,st);
      DPS::Producer::RUN_var  vrun=prun;
      if(length)fbin.write((char*)vrun->get_buffer(),length);      
      fp.pos+=length;
     }
     catch (DPS::Producer::FailedOp & a){
      FMessage((const char *)a.message,DPS::Client::SInAbort);
     }
    }
    if(!fbin.good()){
       cerr <<fpath<<endl;
       FMessage("Unable to write tmp run file ", DPS::Client::CInAbort); 
    }
    fbin.close();
}
}
//  Check here CERN_ROOT;  put /cern/2001 if no
// check if proposed dst file is writeable
   bool writeable=false;
   if(char  *crnr=getenv("CERN_ROOT")){
   }
   else{
      cerr<<"AMSPRoducer-W-CERN_ROOTNotDefined"<<endl;
      setenv("CERN_ROOT","/cern/2001",1);
   }
  
   if(char *ntd=getenv("NtupleDir")){
     AString cmd=" mkdir -p -v  ";
     cmd+=ntd;
     system(cmd);
     cout <<"AMSProducer::getRunEvInfo-I-CreatingOutputDir "<<cmd<<endl;
     cmd=" touch ";
     cmd+=ntd;
     cmd+="/qq";
     int i=system(cmd);
     if(i==0){
      writeable=true;
      cmd=ntd;
      cmd+="/qq";
      unlink((const char*)cmd);
     }
     else{
      cerr<<"AMSProducer::getRunEventInfo-E-UnwritableDir "<<ntd<<endl;
     }     
   }

   LMessage(AMSClient::print(_reinfo,"StartingRun"));

      if(IOPA.WriteRoot)_dstinfo->type = DPS::Producer::RootFile;
      else _dstinfo->type = DPS::Producer::Ntuple;
   if(IsLocal() && !writeable){
    cout <<"AMSProducer-getRunEvInfo-S-NtupleDir "<<getenv("NtupleDir")<<" IsNotWriteable"<<endl; 
    AString ntpath=(const char *)_dstinfo->OutputDirPath;
    setenv("NtupleDir",(const char*)ntpath,1);
//  check if it is writeable
     AString cmd=" touch ";
     cmd+=ntpath;
     cmd+="/qq";
     int i=system(cmd);
     if(i==0){
      writeable=true;
      cmd=ntpath;
      cmd+="/qq";
      unlink((const char*)cmd);
     }
     else{
      cerr<<"AMSProducer::getRunEventInfo-F-UnwritableDir "<<ntpath<<endl;
      FMessage("Unable to write dstfile ", DPS::Client::CInAbort); 
     }     

    ntpath+="/";
    char tmp[80];
    sprintf(tmp,"%d",_reinfo->Run);
    ntpath+=tmp;
    ntpath+="/";
    if(_dstinfo->type == DPS::Producer::RootFile){
      AMSJob::gethead()->SetRootPath((const char *)ntpath);
    }
    else{
       AMSJob::gethead()->SetNtuplePath((const char *)ntpath);
    }
    }
    else{
     if(_dstinfo->type == DPS::Producer::RootFile)
     _dstinfo->OutputDirPath=AMSJob::gethead()->GetRootPath();
     else
     _dstinfo->OutputDirPath=AMSJob::gethead()->GetNtuplePath();
     if(_RemoteDST){
       if(_dstinfo->Mode==DPS::Producer::LILO)_dstinfo->Mode=DPS::Producer::LIRO;
       if(_dstinfo->Mode==DPS::Producer::RILO)_dstinfo->Mode=DPS::Producer::RIRO;
    }
    else{
       if(_dstinfo->Mode==DPS::Producer::LIRO)_dstinfo->Mode=DPS::Producer::LILO;
       if(_dstinfo->Mode==DPS::Producer::RIRO)_dstinfo->Mode=DPS::Producer::RILO;
    }
    }
    struct timeb  ft;
    ftime(&ft);
    _ST0=ft.time+ft.millitm/1000.;
    if(_debug)cout <<"ST0 "<<_ST0<<endl;
    TIMEX(_T0);
    if(_debug)cout <<"T0 "<<_T0<<endl;
    IMessage(AMSClient::print(_reinfo," get reinfo "));
    IMessage(AMSClient::print(_dstinfo," get dstinfo "));
    InitTDV(_reinfo->uid);
    return;
  }
  catch  (CORBA::SystemException & a){
  }
 }
 if(getior("GetIorExec"))goto again;
 else FMessage("AMSProducer::getRunEventinfo-F-UnableToGetRunEvInfo",DPS::Client::CInAbort);
}

void AMSProducer::sendCurrentRunInfo(bool force){
if (_Solo)return;
if(_OnAir){
  EMessage("AMSProducer::sendCurrentrunInfo-W-AlreadyOnAir ");
 return;
}
//cout <<" sendcurrentinfo start "<<endl;
int failure=0;
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendCurrentInfo(_pid,_cinfo,0);
    _OnAir=false;
    break;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
   failure++;
  }
}
if(failure)UpdateARS();

if(force){
  if(IOPA.hlun)sendNtupleUpdate(DPS::Producer::Ntuple);
  else if(IOPA.WriteRoot)sendNtupleUpdate(DPS::Producer::RootFile);
}
//cout <<" sendcurrentinfo end "<<endl;
}

void AMSProducer::getASL(){
}


void AMSProducer::sendNtupleEnd(DPS::Producer::DSTType type,int entries, int last, time_t end, bool success){
cout <<" sendntupleend start "<<endl;

DPS::Producer::DST *ntend=getdst(type);
if(ntend){
ntend->crc=0;
if(success){
 ntend->Status=DPS::Producer::Success;
}
else ntend->Status=DPS::Producer::Failure;
//ntend->Status=success?(DPS::Producer::Success):(DPS::Producer::Failure);
ntend->EventNumber=entries;
ntend->LastEvent=last;
ntend->End=end;
ntend->Type=type;
AMSmceventg::SaveSeeds();
ntend->rndm1=GCFLAG.NRNDM[0];
ntend->rndm2=GCFLAG.NRNDM[1];
//cout <<"  Sending Last Event " <<ntend->LastEvent<<" "<<ntend->rndm1<<" "<<ntend->rndm2<<endl;
if(ntend->End==0 || ntend->LastEvent==0)ntend->Status=DPS::Producer::Failure;
{
   AString a=(const char*)ntend->Name;
   int bstart=0;
   for (int i=0;i<a.length();i++){
    if(a[i]==':'){
     bstart=i+1;
     break;
    }
   }
   int bend=0;
   for (int i=a.length()-1;i>=0;i--){
    if(a[i]=='/'){
     bend=i+1;
     break;
    }
   }
   int bnt=bstart+strlen(getenv("NtupleDir"));
   if(a.length()>bnt && a[bnt]=='/')bnt++;
   



// Move ntuple to the dest directory

char *destdir=getenv("NtupleDestDir");
if(destdir && strcmp(destdir,getenv("NtupleDir"))){
 _Transfer=true;
 char *means=getenv("TransferBy");
 AString fmake;
 AString fcopy;
 if(means && means[0]=='r' && means[1]=='f'){
  fmake="rfmkdir -p -m 775 ";
  fcopy=means;
  fcopy+=" ";
 }
 else if(means){
  fmake+="mkdir -p ";
  fcopy=means;
  fcopy+=" ";
 } 
 else{
  fmake="mkdir -p ";
  fcopy="cp ";
 }
 fmake+=destdir;
// fmake+='/';
// for (int k=bnt;k<bend;k++)fmake+=a[k];
 system((const char*)fmake);
 fcopy+=(const char*)a(bstart);
 fcopy+="  ";
 fcopy+=destdir; 
// fcopy+='/';
// for (int k=bnt;k<bend;k++)fcopy+=a[k];
 int ntry=5;
 for(int j=0;j<ntry;j++){
  sleep(1<<(j+1));
  cout <<"SendNtupleEnd-I-StartCopyingDST "<<j<<" Try "<<(const char*)fcopy<<endl;
  if(!system((const char*)fcopy)){
   cout <<"SendNtupleEnd-I-CopiedDSTSuccesfully "<<j<<" Try "<<(const char*)fcopy<<endl;
   if((_Solo || !(_dstinfo->Mode==DPS::Producer::LIRO || _dstinfo->Mode==DPS::Producer::RIRO)) &&
     !(means && means[0]=='r' && means[1]=='f')){
    AString rm="rm -rf ";
    rm+=a(bstart);
    system((const char*)rm);
    cout <<"SendNtupleEnd-I-DeletingDSTBy "<<(const char*)rm<<endl;
     struct statfs buffer;
     int fail=statfs((const char*)destdir, &buffer);
    if(fail){
      ntend->FreeSpace=-1;
      ntend->TotalSpace=-1;
    }
    else{
     ntend->FreeSpace= (buffer.f_bavail*(buffer.f_bsize/1024.));
     ntend->TotalSpace= (buffer.f_blocks*(buffer.f_bsize/1024.));
    }
    AString b="";
    for(int k=0;k<bstart;k++)b+=a[k];
    b+=destdir;
    b+="/";
    //b+=a(bnt);
    b+=a(bend);
    a=b;
    ntend->Name=(const char*)a;
   }
   break;
  }
 }
 _Transfer=false;
}






    struct stat statbuf;
    stat((const char*)a(bstart), &statbuf);
    

ntend->Insert=statbuf.st_ctime;
ntend->size=statbuf.st_size;
ntend->ErrorNumber=0;

//add crc
   if(!AMSTimeID::_Table){
     AMSTimeID::_InitTable;
   }
   ifstream fbin;
   sleep(1);
   fbin.open((const char*)a(bstart));
   uinteger crc=0;
   if(fbin){
          unsigned int chunk[32000]; 
         int i=0;
          int fsize=statbuf.st_size;
         for(;;){
           if(!fsize) break;
           int myread=fsize>sizeof(chunk)?sizeof(chunk):fsize;
           fbin.read((char*)chunk,myread);
           fsize-=myread;
           if(fbin.good() && !fbin.eof()){
           int beg;
           if(i==0){
            crc=~chunk[0];
            beg=1;
           }
           else{
            beg=0;
           }
           for(int m=beg;m<myread/sizeof(chunk[0]);m++){
            for(int j=0;j<3;j++)crc=AMSTimeID::_Table[crc>>24]^(crc<<8);
            crc=crc^chunk[m];  
           }
           i++;
          }
          else break;
         }
         fbin.close();
         ntend->crc=~crc;
   }
// add validation
const char *exedir=getenv("ExeDir");
const char *nve=getenv("NtupleValidatorExec");
if(exedir && nve && AMSCommonsI::getosname()){
 AString systemc(exedir);
  systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  systemc+=" ";
  systemc+=a(bstart);
  systemc+=" ";
  char tmp[80];
  sprintf(tmp,"%d",ntend->EventNumber);
  systemc+=tmp;
  if(IOPA.WriteRoot)systemc+=" 1 ";
  else systemc+=" 0 ";
  sprintf(tmp,"%d",ntend->LastEvent);
   systemc+=tmp; 
  int i=system(systemc);
  if( (i == 0xff00) || (i & 0xff)){
// Unable To Check
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBy "<<systemc<<endl;
  }
  else{
    i=(i>>8);
    if(i/128){
     ntend->Status=DPS::Producer::Failure;
    }
    else{
     ntend->Status=DPS::Producer::Validated;
     ntend->ErrorNumber=int(i*ntend->EventNumber/100);
    }
  }
 }
 else if(!nve){
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBecauseNtupleValidatorExecIsNull"<<endl;
 }
 else if(!exedir){
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBecauseExeDirIsNull"<<endl;
 }
 else {
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBecauseOsNameIsNull"<<endl;
 }
}



LMessage(AMSClient::print(*ntend,"CloseDST"));

cout << " nt end " <<ntend->Insert<<" "<<ntend->Begin<<" "<<ntend->End<<endl;
if(_Solo)return;
UpdateARS();
sendDSTInfo();

if(_dstinfo->Mode==DPS::Producer::LIRO || _dstinfo->Mode==DPS::Producer::RIRO){
aga1:
for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,*ntend,DPS::Client::Delete);
    _OnAir=false;
     break;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
 if(getior("GetIorExec"))goto aga1;
 else FMessage("AMSProducer::sendNtupleEnd-F-UnableToDeleteNtuple ",DPS::Client::CInAbort);
  }
}
   AString a=(const char*)ntend->Name;
   int bstart=0;
   for (int i=0;i<a.length();i++){
    if(a[i]==':'){
     bstart=i+1;
     break;
    }
   }
   int start=0;
   for (int i=a.length()-1;i>=0;i--){
    if(a[i]=='/'){
     start=i+1;
     break;
    }
   }
    DPS::Producer::FPath fpath;
    fpath.fname=(const char*)a(start);    
    fpath.pos=0;
    cout <<" file name "<<a<<" "<<start<<endl;    
    struct stat statbuf;
    stat((const char*)a(bstart), &statbuf);
   ifstream fbin;
   fbin.open((const char*)a(bstart));
   if(fbin){
    DPS::Producer::TransferStatus st=DPS::Producer::Begin;
    const int maxs=2000000;
     DPS::Producer::RUN_var vrun=new DPS::Producer::RUN();
    while(st !=DPS::Producer::End){
     int last=statbuf.st_size-fpath.pos;
     if(last>maxs)last=maxs;
     else st=DPS::Producer::End;
     vrun->length(last);
     fbin.read(( char*)vrun->get_buffer(),last);
     if(!fbin.good()){
      FMessage("AMSProducer::sendNtupleEnd-F-UnableToReadNtuplein mode RO ",DPS::Client::CInAbort);
     }
      for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
      try{
       _OnAir=true;
       (*li)->sendFile(_pid,fpath,vrun,st);
       _OnAir=false;
        break;
       }
       catch (DPS::Producer::FailedOp & a){
        _OnAir=false;
        FMessage((const char *)a.message,DPS::Client::SInAbort);
       }
       catch  (CORBA::SystemException & a){
       _OnAir=false;
       FMessage("AMSProducer::sendNtupleEnd-F-UnableToSendNtupleBody ",DPS::Client::CInAbort);
       }
     }
     fpath.pos+=last;
      if(st==DPS::Producer::Begin)st=DPS::Producer::Continue;
    }
     fbin.close();
     unlink( ((const char*)a(bstart)));
      a=(const char*)_pid.HostName;
//     a+=":REMOTE:";
       a+=":";
     a+=(const char*)fpath.fname;
     ntend->Name=(const char *)a;
     if(ntend->Status !=DPS::Producer::Failure)ntend->Status=DPS::Producer::Success;
     again:
     for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
      try{
       if(!CORBA::is_nil(*li)){
        _OnAir=true;
        (*li)->sendDSTEnd(_pid,*ntend,DPS::Client::Create);
         _OnAir=false;
        return;
       }
      }
      catch  (CORBA::SystemException & a){
      _OnAir=false;
     }
    }
 if(getior("GetIorExec"))goto again;
 else FMessage("AMSProducer::sendRunEnd-F-UnableToSendNtupleEndInfo ",DPS::Client::CInAbort);
}
   else FMessage("AMSProducer::sendNtupleEnd-F-UnableToSendNtuplein mode RO ",DPS::Client::CInAbort);

  }

 




else{
 again1:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,*ntend,DPS::Client::Update);
    _OnAir=false;
     if( ntend->Status==DPS::Producer::Failure)FMessage("Ntuple Failure",DPS::Client::CInAbort);
     return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again1;
 else FMessage("AMSProducer::sendNtupleEnd-F-UnableToSendNtupleEndInfo ",DPS::Client::CInAbort);
}
}
else{
FMessage("AMSProducer::sendNtupleEnd-F-UNknownDSTType ",DPS::Client::CInAbort);
}  
}



void AMSProducer::sendNtupleStart(DPS::Producer::DSTType type,const char * name, int run, int first,time_t begin){
DPS::Producer::DST *ntend=getdst(type);
if(ntend){
AString a=(const char*)_pid.HostName;
a+=":";
a+=name;
ntend->Name=(const char *)a;
a=AMSCommonsI::getversion();
char tmp[80];
sprintf(tmp,"%d",AMSCommonsI::getbuildno());
a+="/build";
a+=tmp;
sprintf(tmp,"%d",AMSCommonsI::getosno());
a+="/os";
a+=tmp;
ntend->Version=(const char*)a;
ntend->Run=run;
ntend->crc=0;
ntend->FirstEvent=first;
ntend->Begin=begin;
time_t tt;
time(&tt);
ntend->Insert=tt;
ntend->End=0;
ntend->LastEvent=0;
ntend->EventNumber=0;
ntend->ErrorNumber=0;
ntend->Status=DPS::Producer::InProgress;
ntend->Type=type;
ntend->size=0;
     struct statfs buffer;
     int fail=statfs((const char *)name, &buffer);
    if(fail){
      ntend->FreeSpace=-1;
      ntend->TotalSpace=-1;
    }
    else{
     ntend->FreeSpace= (buffer.f_bavail*(buffer.f_bsize/1024.));
     ntend->TotalSpace= (buffer.f_blocks*(buffer.f_bsize/1024.));
    }
//cout <<" sendntuplestart start "<<name<<" "<<ntend->FreeSpace<<" "<<ntend->TotalSpace<<endl;



LMessage(AMSClient::print(*ntend,"OpenDST"));
if(_Solo)return;
UpdateARS();


sendDSTInfo();
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,*ntend,DPS::Client::Create);
    _OnAir=false;
     goto checkdd;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendNtupleStart-F-UnableToSendNtupleStartInfo ",DPS::Client::CInAbort);
}
else{
FMessage("AMSProducer::sendNtupleEnd-F-UNknownDSTType ",DPS::Client::CInAbort);
}


checkdd:
{
   AString a=(const char*)ntend->Name;
   int bstart=0;
   for (int i=0;i<a.length();i++){
    if(a[i]==':'){
     bstart=i+1;
     break;
    }
   }
   int bend=0;
   for (int i=a.length()-1;i>=0;i--){
    if(a[i]=='/'){
     bend=i+1;
     break;
    }
   }
   int bnt=bstart+strlen(getenv("NtupleDir"));
   if(a.length()>bnt && a[bnt]=='/')bnt++;
   



// check if dd writeable 

char *destdir=getenv("NtupleDestDir");
if(destdir && strcmp(destdir,getenv("NtupleDir"))){

   AString a=(const char*)ntend->Name;
   int bstart=0;
   for (int i=0;i<a.length();i++){
    if(a[i]==':'){
     bstart=i+1;
     break;
    }
   }
   int bend=0;
   for (int i=a.length()-1;i>=0;i--){
    if(a[i]=='/'){
     bend=i+1;
     break;
    }
   }
   int bnt=bstart+strlen(getenv("NtupleDir"));
   if(a.length()>bnt && a[bnt]=='/')bnt++;
   

 char *means=getenv("TransferBy");
 AString fmake;
 if(means && means[0]=='r' && means[1]=='f'){
  fmake="rfmkdir -p -m 775 ";
 }
 else{
  fmake="mkdir -p ";
 }
 fmake+=destdir;
// fmake+='/';
// for (int k=bnt;k<bend;k++)fmake+=a[k];
   cout <<"SendNtupleStart-I-MakingDestDir "<<(const char*)fmake<<endl;

 if(system((const char*)fmake)){
       FMessage("Unable create dst dest  dir ", DPS::Client::CInAbort); 
 }
}
}
}

void AMSProducer::sendNtupleUpdate(DPS::Producer::DSTType type){
if(_Solo)return;
DPS::Producer::DST *ntend=getdst(type);
if(ntend){
ntend->Status=DPS::Producer::InProgress;
   AString a=(const char*)ntend->Name;
   int bstart=0;
   for (int i=0;i<a.length();i++){
    if(a[i]==':'){
     bstart=i+1;
     break;
    }
   }
    struct stat statbuf;
    stat((const char*)a(bstart), &statbuf);
    

ntend->Insert=statbuf.st_ctime;
ntend->size=statbuf.st_size/1024./1024.+0.5;


     struct statfs buffer;
     int fail=statfs((const char*)a(bstart), &buffer);
    if(fail){
      ntend->FreeSpace=-1;
      ntend->TotalSpace=-1;
    }
    else{
     ntend->FreeSpace= (buffer.f_bavail*(buffer.f_bsize/1024.));
     ntend->TotalSpace= (buffer.f_blocks*(buffer.f_bsize/1024.));
    }

//cout <<" sendntupleupdate start "<<a(bstart)<<" "<<ntend->FreeSpace<<" "<<ntend->TotalSpace<<endl;

UpdateARS();


sendDSTInfo();
cout <<" sendntupleupdate middle "<<endl;
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,*ntend,DPS::Client::Update);
    _OnAir=false;
    cout <<" sendntupleupdate end "<<endl;
    return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendNtupleUpdate-F-UnableToSendNtupleStartInfo ",DPS::Client::CInAbort);
}
else{
FMessage("AMSProducer::sendNtupleUpdate-F-UNknownDSTType ",DPS::Client::CInAbort);
}  

}

void AMSProducer::Exiting(const char * message){
if(_Solo)return;
if(_ExitInProgress)return;
cout<< " Exiting ...."<<(message?message:" ")<<endl;
_ExitInProgress=true;
_pid.Status=AMSClient::_error.ExitReason();
for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
try{
if(!CORBA::is_nil(*li)){
    _OnAir=true;
(*li)->Exiting(_pid,(message?message:AMSClient::_error.getMessage()),AMSClient::_error.ExitReason());
    _OnAir=false;
cout <<" exiting ok"<<endl;
break;
}
}
catch  (CORBA::SystemException & a){}
    _OnAir=false;
}
}


void AMSProducer::UpdateARS(){
if(AMSJob::gethead()->isSimulation()){
 return;
}
for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
try{
if(!CORBA::is_nil(*li)){
     DPS::Client::ARS * pars;
    _OnAir=true;
     int length=(*li)->getARS(_pid,pars,DPS::Client::Any,0,true);
    _OnAir=false;
     DPS::Client::ARS_var ars=pars;
     if(length==0){
      FMessage("getARS-S-UnableToGetARS ",DPS::Client::CInAbort);
     }   
     if(_MT){
     for(int i=0;i<length;i++){
        CORBA::Object_var obj=_orb->string_to_object((ars[i]).IOR);
        if(!CORBA::is_nil(obj)){
        DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
        if(!CORBA::is_nil(_pvar)){
         if(i==0)_plist.clear();
         _plist.push_back(_pvar);
        }
        }
       }
     }
     else{
      bool LocalHostFound=false;
     for(int i=0;i<length;i++){
        CORBA::Object_var obj=_orb->string_to_object((ars[i]).IOR);
        if(!CORBA::is_nil(obj)){
        DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
        if(!CORBA::is_nil(_pvar)){
         if(i==0)_plist.clear();
         double r=double(rand())/RAND_MAX;
          try{
           DPS::Client::CID * pcid;       
             _OnAir=true;
             _pvar->getId(pcid);
             _OnAir=false;
           DPS::Client::CID_var cid=pcid;
            if(strstr((const char *)cid->HostName, (const char *)_pid.HostName)){
              if(!LocalHostFound){
#ifdef __AMSDEBUG__
              cout <<" LocalHostFound "<<cid->HostName<<endl;
#endif
                r=1;
                LocalHostFound=true;
              }
            }
            else if(LocalHostFound)r=0;
          }
          catch  (CORBA::SystemException & a){
            _OnAir=false;
            r=0;
          }
         if(r>0.5)_plist.push_front(_pvar);
         else _plist.push_back(_pvar);
        }
        }
       }
     }
      break;
}
}
catch  (CORBA::SystemException & a){}
    _OnAir=false;
}

}


void AMSProducer::sendRunEnd(DAQEvent::InitResult res){
if(_dstinfo->Mode ==DPS::Producer::LILO || _dstinfo->Mode==DPS::Producer::LIRO){
unlink( DAQEvent::getfile());
}
     if(res!=DAQEvent::OK)FMessage("AMSProducer::sendRunEnd-F-RunFailed ",DPS::Client::CInAbort);
//_cinfo.Status= (res==DAQEvent::OK)?DPS::Producer::Finished: DPS::Producer::Failed;
if(res==DAQEvent::OK){
 _cinfo.Status=DPS::Producer::Finished;
}
else _cinfo.Status=DPS::Producer::Failed;

    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
_cinfo.TimeSpent=st-_ST0;

TIMEX(_cinfo.CPUTimeSpent);
_cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;
_cinfo.CPUMipsTimeSpent=_CPUMipsTimeSpent+(_cinfo.CPUTimeSpent)*_cinfo.Mips/1000;

UpdateARS();
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendCurrentInfo(_pid,_cinfo,0);
    _OnAir=false;

     return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendRunEnd-F-UnableToSendRunEndInfo ",DPS::Client::CInAbort);


}
void AMSProducer::sendRunEndMC(){
if (!_Head)return;
double error=5./sqrt(double(GCFLAG.IDEVT+1));
if (error<0.01)error=0.01;
if(error>0.5)error=0.5;
if(GCFLAG.NEVENT*(1-error) > GCFLAG.IEVENT+1 || GCFLAG.NEVENT==0){
_cinfo.Status= DPS::Producer::Failed;
}
else _cinfo.Status= DPS::Producer::Finished;
    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
_cinfo.TimeSpent=st-_ST0;

TIMEX(_cinfo.CPUTimeSpent);
_cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;
_cinfo.CPUMipsTimeSpent=_CPUMipsTimeSpent+(_cinfo.CPUTimeSpent)*_cinfo.Mips/1000;

        if(_cinfo.Status!= DPS::Producer::Finished){
         LMessage(AMSClient::print(_cinfo," RunIncomplete"));
         FMessage("RunIncomplete ", DPS::Client::CInAbort); 
        }
        else LMessage(AMSClient::print(_cinfo," RunFinished"));
        if(_Solo)return;
UpdateARS();
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendCurrentInfo(_pid,_cinfo,0);
    _OnAir=false;

     return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendRunEndMC-F-UnableToSendRunEndInfo ",DPS::Client::CInAbort);


}

void AMSProducer::AddEvent(){
if(_cinfo.Run == AMSEvent::gethead()->getrun()){
 _cinfo.EventsProcessed++;
 _cinfo.LastEventProcessed=AMSEvent::gethead()->getid();
  if(!AMSEvent::gethead()->HasNoErrors())_cinfo.ErrorsFound++;
  if(!AMSEvent::gethead()->HasNoCriticalErrors())_cinfo.CriticalErrorsFound++;
}
    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;

if(AMSEvent::gethead()->HasFatalErrors()){
 FMessage("AMSProducer::AddEvent-F-EventHasFatalError ",DPS::Client::CInAbort);
}
else if(!(AMSEvent::gethead()->HasNoCriticalErrors())){
  TIMEX(_cinfo.CPUTimeSpent);
  _cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;
_cinfo.CPUMipsTimeSpent=_CPUMipsTimeSpent+(_cinfo.CPUTimeSpent)*_cinfo.Mips/1000;

    _cinfo.TimeSpent=st-_ST0;

  sendCurrentRunInfo();
}
else if(_cinfo.EventsProcessed%_dstinfo->UpdateFreq==1 || st-_ST0-_cinfo.TimeSpent>AMSFFKEY.CpuLimit){
  TIMEX(_cinfo.CPUTimeSpent);
  _cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;
_cinfo.CPUMipsTimeSpent=_CPUMipsTimeSpent+(_cinfo.CPUTimeSpent)*_cinfo.Mips/1000;

    _cinfo.TimeSpent=st-_ST0;

  sendCurrentRunInfo(true);
}
}

AMSProducer::~AMSProducer(){
const char * a=0;
cout <<" ams producer destructor called "<<endl;
Exiting(_up?_up->getMessage():a);
}


void AMSProducer::InitTDV( uinteger run){
AMSTimeID * head=AMSJob::gethead()->gettimestructure();
for (AMSTimeID * pser=dynamic_cast<AMSTimeID*>(head->down());pser;pser=dynamic_cast<AMSTimeID*>(pser->next())){
DPS::Producer::TDVName a;
a.Name=pser->getname();
a.DataMC=pser->getid();
a.Size=pser->GetNbytes();
a.CRC=pser->getCRC();
time_t i,b,e;
pser->gettime(i,b,e);
a.Entry.id=0;
a.Entry.Insert=i;
a.Entry.Begin=b;
a.Entry.End=e;
DPS::Producer::TDVTable * ptdv;
//IMessage(AMSClient::print(a," INITDV "));
 int suc=0;
 int length;
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    _OnAir=true;
    length=(*li)->getTDVTable(_pid,a,run,ptdv);
    _OnAir=false;
    suc++;
    break;
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
 }
if(!suc){
 if(getior("GetIorExec"))goto again;
 else FMessage("AMSProducer::getinitTDV-F-UnableTogetTDVTable",DPS::Client::CInAbort);
}
DPS::Producer::TDVTable_var tvar=ptdv;
uinteger *ibe[5];

if(a.Success){
for(int i=0;i<5;i++){
 ibe[i]=new uinteger[length];
}
 for(int j=0;j<length;j++){
    ibe[0][j]=tvar[j].id;
    ibe[1][j]=tvar[j].Insert;
    ibe[2][j]=tvar[j].Begin;
    ibe[3][j]=tvar[j].End;
  }
  pser->fillDB(length,ibe);
}
else{
FMessage("AMSProducer::getinitTDV-F-tdvgetFailed",DPS::Client::CInAbort);
}
}
}

bool AMSProducer::getTDV(AMSTimeID * tdv, int id){
cout <<" ttrying to get tdv "<<tdv->getname()<<endl;
DPS::Producer::TDVbody * pbody;
DPS::Producer::TDVName name;
name.Name=tdv->getname();
name.DataMC=tdv->getid();
name.CRC=tdv->getCRC();
name.Size=tdv->GetNbytes();
name.Entry.id=id;
time_t i,b,e;
tdv->gettime(i,b,e);
name.Entry.Insert=i;
name.Entry.Begin=b;
name.Entry.End=e;
 int length=0;
 int suc=0;
 bool oncemore=false;
 bool exhausted=false;
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    _OnAir=true;
    length=(*li)->getTDV(_pid,name,pbody);
    _OnAir=false;
    suc++;
    break;
  }
  catch  (const CORBA::SystemException &  a){
     cerr<< "Problems with TDV "<<endl;
//     cerr<< "Problems with TDV "<< a._orbitcpp_get_repoid()<<endl;
endl;
    _OnAir=false;
  }
 }
if(!suc){
 if(oncemore){
 if(getior("GetIorExec") && !exhausted){
    exhausted=true;
    goto again;
 }
  else FMessage("AMSProducer::getTDV-F-UnableTogetTDV",DPS::Client::CInAbort);
  return false;
 }
 else{
  oncemore=true;
  sleep(5);
   UpdateARS();
  goto again;
 }
}
DPS::Producer::TDVbody_var vbody=pbody;
if(name.Success){
 int nb=tdv->CopyIn(vbody->get_buffer());
 if(nb){
  tdv->SetTime(name.Entry.Insert,name.Entry.Begin,name.Entry.End);
  return true;
 }
}
return false;
}
bool AMSProducer::getSplitTDV(AMSTimeID * tdv, int id){
cout <<" trying to get tdv "<<tdv->getname()<<endl;
DPS::Producer::TDVbody * pbody;
DPS::Producer::TDVName name;
name.Name=tdv->getname();
name.DataMC=tdv->getid();
name.CRC=tdv->getCRC();
name.Size=tdv->GetNbytes();
name.Entry.id=id;
time_t i,b,e;
tdv->gettime(i,b,e);
name.Entry.Insert=i;
name.Entry.Begin=b;
name.Entry.End=e;
 int length=0;
 uinteger pos=0;
 DPS::Producer::TransferStatus st=DPS::Producer::Begin;
 DPS::Producer::TDVbody_var vb2=new DPS::Producer::TDVbody();
 uinteger totallength=0;
 while (st!=DPS::Producer::End){
 int suc=0;
 bool exausted=false;
 again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    _OnAir=true;
    length=(*li)->getSplitTDV(_pid,pos,name,pbody,st);
    _OnAir=false;
    suc++;
    break;
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
if(!suc){
 if(getior("GetIorExec")&& !exausted){
   exausted=true;
   goto again;
}
 else FMessage("AMSProducer::getSplitTDV-F-UnableTogetTDV",DPS::Client::CInAbort);
 return false;
}
if(!totallength){
  vb2=pbody;
}
else{
 DPS::Producer::TDVbody_var vbody=pbody;
 vb2->length(totallength+length);
 for(int i=0;i<length;i++){
  vb2[i+totallength]=vbody[i];
 }
}
totallength+=length;
}
if(name.Success){
 int nb=tdv->CopyIn(vb2->get_buffer());
 if(nb){
  tdv->SetTime(name.Entry.Insert,name.Entry.Begin,name.Entry.End);
  cout <<"  gettdv success "<<endl;
  return true;
 }
}
  cout <<"  gettdv failure "<<endl;
return false;
}

bool AMSProducer::sendTDV(const AMSTimeID * tdv){
cout <<" Send tdv get for "<<tdv->getname()<<endl;

DPS::Producer::TDVName name;
name.Name=tdv->getname();
name.DataMC=tdv->getid();
name.CRC=tdv->getCRC();
name.Size=tdv->GetNbytes();
time_t i,b,e;
tdv->gettime(i,b,e);
name.Entry.Insert=i;
name.Entry.Begin=b;
name.Entry.End=e;
 int suc=0;
DPS::Producer::TDVbody_var vbody=new DPS::Producer::TDVbody();
vbody->length(name.Size/sizeof(integer));
tdv->CopyOut(vbody->get_buffer());
 bool transienterror=false;
 bool again=false;
AGAIN:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    _OnAir=true;
    (*li)->sendTDV(_pid,vbody,name);
    _OnAir=false;
    suc++;
    break;
  }
   catch (CORBA::TRANSIENT &tr){
    _OnAir=false;
    if(!again){
      transienterror=true;
      again=true;
      EMessage("AMSProducer::sendTDV-E-TransientErrorOccurs");
    }
    sleep(1);    
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
 }
if(!suc && !transienterror){
 FMessage("AMSProducer::sendTDV-F-UnableTosendTDV",DPS::Client::CInAbort);
 return false;
}
else if(!suc){
transienterror=false;
goto AGAIN;
}
return true;
}

void AMSProducer::sendEventTagEnd(const char * name,uinteger run,time_t insert, time_t begin,time_t end,uinteger first,uinteger last,integer nelem, bool fail){
//_evtag.Status=fail?DPS::Producer::Failure:DPS::Producer::Success;
if(fail){
 _evtag.Status=DPS::Producer::Failure;
}
else  _evtag.Status=DPS::Producer::Success;
_evtag.EventNumber=nelem;
_evtag.FirstEvent=first;
_evtag.LastEvent=last;
_evtag.Insert=insert;
_evtag.Begin=begin;
_evtag.End=end;
_evtag.Run=run;
_evtag.Type=DPS::Producer::EventTag;
_evtag.size=0;

UpdateARS();
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,_evtag,DPS::Client::Update);
    _OnAir=false;
    if( _evtag.Status==DPS::Producer::Failure)FMessage("EventTag Failure",DPS::Client::CInAbort);
    return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendRunEnd-F-UnableToSendEventTagEndInfo ",DPS::Client::CInAbort);



}


void AMSProducer::sendEventTagBegin(const char * name,uinteger run,uinteger first){
AString a=(const char*)_pid.HostName;
a+=":";
a+=name;
a+=".";
char tmp[80];
sprintf(tmp,"%d",run);
a+=tmp;
a+=".";
sprintf(tmp,"%d",first);
a+=tmp;
_evtag.Name=(const char *)a;
_evtag.Status=DPS::Producer::InProgress;
_evtag.EventNumber=1;
_evtag.FirstEvent=first;
_evtag.LastEvent=first;
_evtag.Insert=0;
_evtag.Begin=0;
_evtag.End=0;
_evtag.Run=run;
_evtag.Type=DPS::Producer::EventTag;


UpdateARS();
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,_evtag,DPS::Client::Create);
    _OnAir=false;
    if( _evtag.Status==DPS::Producer::Failure)FMessage("EventTagBegin Failure",DPS::Client::CInAbort);
    return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendRunEnd-F-UnableToSendEventTagBeginInfo ",DPS::Client::CInAbort);



}



void AMSProducer::sendDSTInfo(){

     struct statfs buffer;
     int fail=statfs((const char *)_dstinfo->OutputDirPath, &buffer);
    if(fail){
      _dstinfo->FreeSpace=-1;
      _dstinfo->TotalSpace=-1;
    }
    else{
     _dstinfo->FreeSpace= (buffer.f_bavail*(buffer.f_bsize/1024.));
     _dstinfo->TotalSpace= (buffer.f_blocks*(buffer.f_bsize/1024.));
    }
    for( list<DPS::Producer_var>::iterator ni = _plist.begin();ni!=_plist.end();++ni){
      try{
    _OnAir=true;
       (*ni)->sendDSTInfo(_dstinfo,DPS::Client::Update);
    _OnAir=false;
      }
      catch  (CORBA::SystemException & a){
      cerr<<" sendDSTInfo-E-UnableToSend "<<endl; 
    _OnAir=false;
      }
    }



}

bool AMSProducer::Progressing(){
static integer total=-1;
static float xt=0;
static double xte=0;
float lt;
    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
TIMEX(lt);
cerr <<"AMSPRoducer::Progressing "<<total<<" "<<lt-xt<<" "<<AMSFFKEY.CpuLimit*2<<" "<<_cinfo.EventsProcessed<<" "<<st-xte<<endl;
 if(_Transfer)return true;
 if(total>=0 && total == _cinfo.EventsProcessed && (lt-xt>AMSFFKEY.CpuLimit*2.5 || st-xte>AMSFFKEY.CpuLimit*100)){
   total=-1;   
   return false;
 }
 else if(total<0){
  total=_cinfo.EventsProcessed;
  TIMEX(xt);
    struct timeb  ft;
    ftime(&ft);
    xte=ft.time+ft.millitm/1000.;
  return true;
 }
 else if(total != _cinfo.EventsProcessed){
  total=-1;
  return true;
 }
 else{
  return true;
 }
}

DPS::Producer::DST* AMSProducer::getdst(DPS::Producer::DSTType type){
switch (type){
case DPS::Producer::Ntuple:
return &(_ntend[0]);
break;
case DPS::Producer::RootFile:
return &(_ntend[1]);
break;
default:
return 0;
}
}

bool AMSProducer::getior(const char * getiorvar){
_OnAir=false;
char iort[1024];
const char *exedir=getenv("ExeDir");
const char *nve=getenv(getiorvar);
const char *nvr=AMSCommonsI::getosversion(); 
int maxtries=6;
int delay=1;
if(exedir && nve && AMSCommonsI::getosname()){
 char t1[1024];
 strcpy(t1,exedir);
 strcat(t1,"/../prod");
 setenv("TNS_ADMIN",t1,1);
 for (int tries=0;tries<maxtries;tries++){
  sleep(delay);
  delay+=3600*(tries+1);
  AString systemc(exedir);
  systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  if(strstr(nvr,"2.6")){
   systemc+=".6";
  }
  systemc+=" > /tmp/getior.";
  char tmp[80];
  sprintf(tmp,"%d",getpid());
  systemc+=tmp;
  int i=system(systemc);
  if(i){
   cerr <<" AMSProducer::getenv-E-UnableTo "<<systemc<<endl;
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
//   system(systemc);
   continue;
  }
  else{
   systemc="/tmp/getior."; 
   systemc+=tmp;
   ifstream fbin;
   fbin.open(systemc);
   iort[0]='\0';
   fbin>>iort;
   fbin.close();
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
   system(systemc);
   if(iort[0]=='-'){
    cerr <<" AMSProducer::getenv-E-UnableToGetIOR "<<iort<<endl;
    continue;
   }
   else{
   try{
    cout <<" AMSProducer::getenv-I-GetIOR "<<iort<<endl;
    CORBA::Object_var obj=_orb->string_to_object(iort);
    if(!CORBA::is_nil(obj)){
     DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
     _pvar->pingp();
     _plist.clear();
     _plist.push_front(_pvar);
     return true;
    }
   }
  catch (CORBA::SystemException & a){
    EMessage("AMSProducer::getior-E-UnableToUpdateIOR");
    continue;
  }
   }
  }
}
}
else{
    EMessage("AMSProducer::getior-E-UnableToTryToGetIORBecauseSomeVarAreNull");
}
return false;
}

bool AMSProducer::SetDataCards(){
  _dc=new AString("");
  char tmp[80];
  char tmpp[80];
  char tmpu[80];
  sprintf(tmpp,"%d",_pid.ppid);
  sprintf(tmp,"%d",_pid.pid);
  sprintf(tmpu,"%d",_pid.uid);
  AString fout="/tmp/";
  fout+=tmp;
  fout+=".dc"; 
  AString cmd="ps -elfw --cols 200 | grep ";
  cmd+=tmpp;
  cmd+=" 1>";
  cmd+=(const char*)fout;
  cmd+=" 2>&1 ";
  AString dc="";
  int i=system((const char*)cmd);
  if(i==0 ){
   ifstream fbin;
   fbin.open((const char*)fout);
   AString fscript="";
    while(fbin.good() && !fbin.eof()){
     fbin.ignore(1024,' ');
     char tmpbuf[256];
     fbin>>tmpbuf;
     if(strstr(tmpbuf,tmpu) && strstr(tmpbuf,".job")){
      cout <<"   Found job "<<tmpbuf<<endl;
      fscript+=tmpbuf;
      break;
     } 
    }
    fbin.close();
    unlink((const char*)fout);
    ifstream f1;
    f1.open((const char *)fscript);
    if(f1){
    char tmpbuf[1024];
     (*_dc)+="ScriptName=";
     (*_dc)+=(const char *)fscript;
     (*_dc)+='\n';
    while(f1.good() && !f1.eof()){ 
     f1.getline(tmpbuf,1023);
     (*_dc)+=tmpbuf;
     (*_dc)+='\n';
     //cout <<"  *************************8  ******************8"<<endl;
     //cout<<(const char *)(*_dc)<<endl;
    }
    }
    else{
     cerr<<" AMSProducer::SetDatacards-S-UnableToOpenFile "<<(const char *)fscript<<endl;
     return false; 
    }
  }
  else{
   cerr <<" AMSProducer::SetDataCards-S-UnableToSys "<<(const char *)cmd<<endl;  
    return false;
  }
  return true;
} 
