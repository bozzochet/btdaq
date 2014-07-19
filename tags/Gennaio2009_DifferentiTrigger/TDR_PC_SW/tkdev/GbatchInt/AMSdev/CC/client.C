//  $Id: client.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "client.h"
#include <stdio.h>
#include <unistd.h>
#include <iostream.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <netdb.h>
char AMSClient::_streambuffer[1024];
std::ostrstream AMSClient::_ost(AMSClient::_streambuffer,sizeof(AMSClient::_streambuffer));

void AMSClient::_openLogFile(char * prefix,bool solo){
AString fnam;
char * logdir=getenv("ProductionLogDir");
if(!logdir){
 cerr<<"AMSClient::_openLogFile-W-ProductionLogDirNotDefined:$AMSDataDir/prod.log will be used"<<endl;
 logdir=getenv("AMSDataDir");
 if(!logdir){
  AString a("AMSDataDirNotDefined ");
    FMessage((char *) a,DPS::Client::CInAbort);
 }
 fnam=logdir;
 fnam+="/prod.log";
}
else fnam=logdir;
 fnam+="/";
 fnam+=prefix;
 fnam+="/";
 AString out="mkdir -p ";
 out+=fnam;
 system(out);
 time_t timecur;
 time(&timecur);
 char time[255];
 sprintf(time,"%s.%d.%d",(const char *)_pid.HostName,_pid.uid,_pid.pid);
//
//  Journal File  
//
 AString fnam2(fnam);
 fnam+=time;
 _fbin.open(fnam,ios::out);
 if(!_fbin){
    AString a("AMSClient::_openLogFile-F-UnableOpenLogFile ");
     a+=fnam; 
    FMessage((char *) a,DPS::Client::CInAbort);
  }
 else{
    AString a("AMSClient::_openLogFile-I-opened");
     a+=fnam; 
     IMessage((const char *) a);
 }

 sprintf(time,"%010d.journal",_pid.uid);
 fnam2+=time;
  if(!solo){
   _fbin2.open(fnam2,ios::out|ios::app);
  }
  else{
   _fbin2.open(fnam2,ios::in);
   if(!_fbin2){
    _fbin2.clear();
    _fbin2.open(fnam2,ios::out);
   }
   else{
    AString a("AMSClient::_openLogFile-F-JournalFileAlreadyExists");
     a+=fnam2; 
    FMessage((char *) a,DPS::Client::CInAbort);
   }
  }
 if(!_fbin2){
    AString a("AMSClient::_openLogFile-F-UnableOpenJournalFile ");
     a+=fnam2; 
    FMessage((char *) a,DPS::Client::CInAbort);
  }
 else{
    AString a("AMSClient::_openLogFile-I-JournalFileOpened");
     a+=fnam2; 
     IMessage((const char *) a);
 }

}
/*
extern "C" pid_t getpid();
extern "C" pid_t getppid();
extern "C" int gethostname(char*, int);
*/
bool AMSClient::_getpidhost(uinteger uid, const char * iface){
 _pid.uid=uid;
 _pid.pid=getpid();
 _pid.ppid=getppid();
 char name[256];
 int len=255;
 if(gethostname(name,len))return false;
else{
   struct hostent *hent;
   hent=gethostbyname(name);
   if(hent)strcpy(name,hent->h_name);
   else cerr<<"AMSClient::_gethostname-E-UnableToGetHostbyName "<<name<<endl;
   AString as=(const char *)name;
   if(!iface || strcmp(iface,"default")){
    int newlength=as.length();
    bool isdot=false;
    for (int i=0;i<as.length();i++){
      if (as[i]=='.'){
      //  newlength=i;
          isdot=true;
      }
      if(i>newlength-1)as[i]='\0';
    }
     if(iface && !isdot){
      as+=".";
      as+=iface;
     }
   }
/*
   bool isdot=false;
   for (int i=0;i<as.length();i++){
    if (as[i]=='.'){
       isdot=true;
       break;
      }
    }
   if(!isdot && !getdomainname(name,len)){
     as+=(const char*)name;
   }
*/  
  _pid.HostName=(const char*)as;
  return true;
}
}

void AMSClient::FMessage(const char * message, DPS::Client::ClientExiting res){

cerr<<message<<endl;
   if(_fbin){
     time_t tcur;
     time(&tcur);
     _fbin<<endl<<" -F-  "<<ctime(&tcur)<<endl<<message << endl;
   }
   AMSClientError a(message,res);
   Error()=a;
   Exiting();
   throw a;
}

void AMSClient::EMessage(const char * message){

cerr<<message<<endl;
   if(_fbin){
     time_t tcur;
     time(&tcur);
     _fbin <<endl<<" -E- "<<ctime(&tcur)<<endl<<message <<endl;
   }
}


void AMSClient::IMessage(const char * message){

cout<<message<<endl;
   if(_fbin){
     time_t tcur;
     time(&tcur);
     _fbin <<endl<<" -I- "<<ctime(&tcur)<<endl<<message <<endl;
   }
}

void AMSClient::LMessage(const char * message){

cout<<message<<endl;
   if(_fbin2){
     time_t tcur;
     time(&tcur);
     _fbin2 <<endl<<"-I-TimeStamp "<<tcur<<" "<<ctime(&tcur)<<", "<<message <<endl;
   }
}


char * AMSClient::print(const DPS::Client::ActiveClient & a,const char * mes){

_ost.clear();
_ost.seekp(0);
_ost<<mes <<" AC ";
print(a.id,_ost);
_ost<<" ARSLength "<<a.ars.length()<<" LastUpdate "<<ctime((const time_t *)&a.LastUpdate)<<" Start "<<ctime((const time_t *)&a.Start)<< "  Status " <<CS2string(a.Status)<<ends;
return _streambuffer;
}

ostream & AMSClient::print(const DPS::Producer::TDVTableEntry & a, ostream & o){
return o<<" id "<<a.id<<" Insert "<<a.Insert<<" Begin "<<a.Begin<<" End "<<a.End;
}


char * AMSClient::print(const DPS::Client::NominalClient & a,const char * mes){

_ost.clear();
_ost.seekp(0);
_ost<<mes <<" NC ";
_ost<<a.uid<<" Type "<<CT2string(a.Type)<<" MaxCl "<<a.MaxClients<<ends;
return _streambuffer;
}



char * AMSClient::print(const DPS::Producer::TDVTableEntry & a,const char * mes){
_ost.clear();
_ost.seekp(0);
_ost<<mes <<" TDVTableEntry ";
print(a,_ost);
_ost<<ends;
}

char * AMSClient::print(const DPS::Producer::TDVName & a,const char * mes){

_ost.clear();
_ost.seekp(0);
_ost<<mes <<" TDV ";
_ost<<a.Name<<" Size "<<a.Size<<" CRC "<< a.CRC<< " DataMC " <<a.DataMC<<" Success "<<a.Success;
print(a.Entry,_ost);
_ost<<ends;
return _streambuffer;
}


char * AMSClient::print(const DPS::Client::CID & a,const char * mes){
_ost.clear();
_ost.seekp(0);
_ost<<mes<<" HostName ";
print(a,_ost);
_ost<<ends;
return _streambuffer;
}


ostream & AMSClient::print(const DPS::Client::CID & a, ostream & o){
return o<<a.HostName<<" "<<(a.Interface?a.Interface:" ")<<" , UID "<<a.uid<<" , PID "<<a.pid<<" "<<a.ppid<<" , Type "<<CT2string(a.Type)<<" , ExitStatus "<<CSE2string(a.Status)<<" , StatusType "<<CST2string(a.StatusType)<<" , Mips "<<a.Mips;
}


char * AMSClient::print(const DPS::Client::ActiveHost & a,const char * mes){
_ost.clear();
_ost.seekp(0);
_ost<<mes <<" AH "<<a.HostName<<" "<<(a.Interface?a.Interface:" ")<<" Status  "<<HS2string(a.Status)<< " CRun "<<a.ClientsRunning<<" CAll "<<a.ClientsAllowed<<" CProcessed "<<a.ClientsProcessed<<" CFailed "<<a.ClientsFailed<<" CKilled "<<a.ClientsKilled<<" LastUpdate "<<ctime((const time_t *)&a.LastUpdate)<<" Clock "<<a.Clock<<ends;
return _streambuffer;
}

char * AMSClient::print(const DPS::Client::NominalHost & a,const char * mes){
_ost.clear();
_ost.seekp(0);
_ost<<mes <<" NH "<<a.HostName<<" "<<(a.Interface?a.Interface:" ")<<"OS  "<<a.OS<< " CPU "<<a.CPUNumber<<" Memory "<<a.Memory<<" Clock "<<a.Clock<<ends;
return _streambuffer;
}


char * AMSClient::print(const DPS::Server::CriticalOps & a,const char * mes){
_ost.clear();
_ost.seekp(0);
_ost<<mes<< "COp Action "<<OPS2string(a.Action) << " ClientType "<<CT2string(a.Type)<<" ClientID "<<a.id<<" Time  "<<ctime((const time_t *)&a.TimeStamp)<<" TimeOut "<<a.TimeOut<<ends;
return _streambuffer;
}

char * AMSClient::print(const DPS::Producer::RunEvInfo & a,const char * mes){
_ost.clear();
_ost.seekp(0);
_ost<<mes<<" REI, ID "<<a.uid<<" , Run "<<a.Run<<" , FirstEvent "<<a.FirstEvent<<" , LastEvent "<<a.LastEvent<<" , Prio "<<a.Priority<<" , Path "<<(a.FilePath?a.FilePath:" ")<< " , Status "<<RS2string(a.Status)<<" , History "<<RS2string(a.History)<<" Failed "<<a.CounterFail<<" , ClientID "<<a.cuid<<" , SubmitTimeU "<<a.SubmitTime<<" , SubmitTime "<<ctime((const time_t *)&a.SubmitTime);
print(a.cinfo,_ost);
_ost<<ends;
return _streambuffer;
}

char * AMSClient::print(const DPS::Producer::DSTInfo & a,const char * mes){
_ost.clear();
_ost.seekp(0);
_ost<<mes<<" "<<a.uid<<" Host "<<a.HostName<<" Mode "<<RunMode2string(a.Mode)<<" UpdateFreq "<<a.UpdateFreq<<" DSType "<<DSTT2string(a.type)<<" NtupleOutput "<<(a.OutputDirPath?a.OutputDirPath:" ")<<" FreeKb "<<a.FreeSpace<<" TotalKb "<<a.TotalSpace<<ends;
return _streambuffer;
}


char * AMSClient::print(const DPS::Producer::DST & a,const char * mes){
_ost.clear();
_ost.seekp(0);
_ost<<mes<<" "<<" , Status "<<DSTS2string(a.Status)<<" , Type "<<DSTT2string(a.Type)<<" , Name "<<(a.Name?a.Name:" ")<<" , Version "<<a.Version<<" , Size "<<a.size<<" , crc "<<a.crc<<" , Insert "<<a.Insert<<" , Begin "<<a.Begin<<" , End "<<a.End<<" , Run "<<a.Run<<" , FirstEvent "<<a.FirstEvent<<" , LastEvent "<<a.LastEvent<<" , EventNumber "<<a.EventNumber<<" , ErrorNumber "<<a.ErrorNumber<<" , FrreSpace "<<a.FreeSpace<<" , TotalSpace "<<a.TotalSpace<<ends;
return _streambuffer;
}


char * AMSClient::print(const DPS::Producer::CurrentInfo & a,const char * mes){
_ost.clear();
_ost.seekp(0);
_ost<<mes<<" CInfo ";
print(a,_ost);
_ost<<ends;
return _streambuffer;
}


ostream & AMSClient::print(const DPS::Producer::CurrentInfo & a, ostream & o){
return o<<" , Host " <<a.HostName <<" , Run "<<a.Run<<" , EventsProcessed "<<a.EventsProcessed<<" , LastEvent "<<a.LastEventProcessed<<" , Errors "<<a.ErrorsFound<<" , CPU "<<a.CPUTimeSpent<<" , Elapsed "<<a.TimeSpent<<" , CPU/Event "<<a.CPUTimeSpent/(a.EventsProcessed+1)<<" , Status "<<RS2string(a.Status);
}

char * AMSClient::CS2string(DPS::Client::ClientStatus a){
switch (a){
case DPS::Client::Lost:
return "Lost";
case DPS::Client::Unknown:
return "Unknown";
case DPS::Client::Submitted:
return "Submitted";
case DPS::Client::Registered:
return "Registered";
case DPS::Client::Active:
return "Active";
case DPS::Client::TimeOut:
return "TimeOut";
case DPS::Client::Killed:
return "Killed";
}
return " ";
}



char * AMSClient::CT2string(DPS::Client::ClientType a){
switch (a){
case DPS::Client::Generic:
return "Generic";
case DPS::Client::Standalone:
return "Standalone";
case DPS::Client::Server:
return "Server";
case DPS::Client::Consumer:
return "Consumer";
case DPS::Client::Producer:
return "Producer";
case DPS::Client::Monitor:
return "Monitor";
case DPS::Client::DBServer:
return "DBServer";
case DPS::Client::ODBServer:
return "ODBServer";
case DPS::Client::Killer:
return "Killer";
}
return " ";
}


char * AMSClient::CST2string(DPS::Client::ClientStatusType a){
switch (a){
case DPS::Client::Permanent:
return "Permanent ";
case DPS::Client::OneRunOnly:
return "OneRunOnly";
}
return " ";
}



char * AMSClient::HS2string(DPS::Client::HostStatus a){
switch (a){
case DPS::Client::NoResponse:
return "NoResponse";
case DPS::Client::LastClientFailed:
return "LastClientFailed";
case DPS::Client::OK:
return "OK";
case DPS::Client::InActive:
return "InActive";
}
return " ";
}

char * AMSClient::RunMode2string(DPS::Producer::RunMode a){
switch (a){
case DPS::Producer::RILO:
return "RILO";
case DPS::Producer::LILO:
return "LILO";
case DPS::Producer::RIRO:
return "RIRO";
case DPS::Producer::LIRO:
return "LIRO";
}
return " ";
}


char * AMSClient::CSE2string(DPS::Client::ClientExiting a){
switch (a){
case DPS::Client::NOP:
return "NOP";
case DPS::Client::CInExit:
return "CInExit";
case DPS::Client::SInExit:
return "SInExit";
case DPS::Client::CInAbort:
return "CInAbort";
case DPS::Client::SInAbort:
return "SInAbort";
case DPS::Client::SInKill:
return "SInKill";
}
return " ";
}


char * AMSClient::RS2string(DPS::Producer::RunStatus a){
switch (a){
case DPS::Producer::ToBeRerun:
return "ToBeRerun";
case DPS::Producer::Failed:
return "Failed";
case DPS::Producer::Processing:
return "Processing";
case DPS::Producer::Allocated:
return "Allocated";
case DPS::Producer::Foreign:
return "Foreign";
case DPS::Producer::Finished:
return "Finished";
}
return " ";
}

char * AMSClient::DSTS2string(DPS::Producer::DSTStatus a){
switch (a){
case DPS::Producer::Success:
return "Success";
case DPS::Producer::InProgress:
return "InProgress";
case DPS::Producer::Failure:
return "Failure";
case DPS::Producer::Validated:
return "Validated";
}
return " ";
}

char * AMSClient::DSTT2string(DPS::Producer::DSTType a){
switch (a){
case DPS::Producer::Ntuple:
return "Ntuple";
case DPS::Producer::EventTag:
return "EventTag";
case DPS::Producer::RootFile:
return "RootFile";
}
return " ";
}

char * AMSClient::OPS2string(DPS::Server::OpType a){
switch (a){
case DPS::Server::StartClient:
return "StartClient";
case DPS::Server::KillClient:
return "KillClient";
case DPS::Server::ClearStartClient:
return "ClearStartClient";
case DPS::Server::ClearKillClient:
return "ClearKillClient";
}
return " ";
}


void AMSClient::setdbfile(const char * dbf){
 if(dbf && strlen(dbf)>0){
  if(_DBFileName)delete [] _DBFileName;
  _DBFileName= new char[strlen(dbf)+1];
  strcpy(_DBFileName,dbf);
 }
}

void AMSClient::resetdbfile(){
  if(_DBFileName)delete [] _DBFileName;
  _DBFileName= 0;
}





