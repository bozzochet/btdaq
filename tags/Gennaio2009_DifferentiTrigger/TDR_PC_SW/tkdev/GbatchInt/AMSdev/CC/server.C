//  $Id: server.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
//
#include <stdlib.h>
#include "server.h"
#include <fstream.h>
#include <iostream.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include "amsdbc.h"
#include <signal.h>    
#include "dbserver.h"
#include <glib.h>
static GMainLoop* gloop[2]={0,0};
static GThread* gthread[2]={0,0};
extern "C" gpointer myrun(gpointer data);

AMSServer* AMSServer::_Head=0;
void (handler)(int);   
void (handler)(int sig){    
switch (sig){
case SIGTERM: 
AMSServer::Singleton()->FMessage("Interrupt Received",DPS::Client::CInExit);
break;
case SIGXCPU:
AMSServer::Singleton()->FMessage("Interrupt Received",DPS::Client::CInExit);
break;
case SIGINT:
AMSServer::Singleton()->FMessage("Interrupt Received",DPS::Client::CInExit);
break;

case SIGUSR1:
// stop everything;
AMSServer::Singleton()->StopEverything();
break;
case SIGUSR2:
//dump IOR in logfile
AMSServer::Singleton()->DumpIOR();
break;
case SIGHUP:
AMSServer::Singleton()->UpdateDB(true);
break;
}
}

int main(int argc, char * argv[]){
// myst thing
//     CORBA::MARSHAL a;
     *signal(SIGTERM, handler);
     *signal(SIGXCPU,handler);
     *signal(SIGINT, handler);
     *signal(SIGUSR1, handler);
     *signal(SIGUSR2, handler);
     *signal(SIGHUP, handler);
//     *signal(SIGFPE, handler);

 try{
    AMSServer::Singleton()=new AMSServer(argc,argv);
 }
  catch (CORBA::SystemException &a){
   cerr<<"CorbaSystemExceptionDuringInitialization "<<endl;
   return 1;
 }
 catch (AMSClientError a){
  cerr<<a.getMessage()<<endl;
   return 1;
  } 
   AMSServer::Singleton()->IMessage("Initialization Completed");
   AMSServer::Singleton()->DumpIOR();
    int count=AMSServer::Singleton()->MT()?-1:1;
          GError *error=NULL;
          int countt=0;
//            g_thread_init(NULL);
//         CORBA::ORB_ptr orb=AMSServer::Singleton()->getdefaultorb();
//         g_thread_create(myrun,&orb,FALSE,&error);
    AMSServer::Singleton()->Listening(count);
    cout <<"  Starting ... "<<endl;
    for(;;){
     try{
      AMSServer::Singleton()->UpdateDB();
      AMSServer::Singleton()->SystemCheck();     
     }
     catch(CORBA::SystemException &a){
      cerr <<"CorbaSystemExceptionDuringUpdateDB/SystemCheck "<<endl;
       continue;
     try{
      AMSServer::Singleton()->Listening(count);
     }
     catch(CORBA::SystemException &a){
      cerr <<"CorbaSystemExceptionDuringPerform_Work "<<endl;
       continue;
     }
     }
      catch (AMSClientError a){
      cerr<<a.getMessage()<<endl;
      break;
          }
    }
 return 0;
}




AMSServer::AMSServer(int argc, char* argv[]){
 char *dbfilename=0;
 char *iface=0;
 char *niface=0;
 char * ior=0;
 uinteger uid=1;
 char *rfile=0;
 char *nserver=0;
 char *nkiller=0;
 char *nhost=0;
 char *nproducer=0;
 char *ntuplestring=0;
 char *eventtagstring=0;
 char *amsd=0;
 char *amsp=0;
 char *amsperl=0;
 setSleepTime();
 
 for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--)){
    pchar++;
    switch (*pchar){
    case 'm':    // MC data
     _MC=true;
     break;
    case 'M':    // Multithread
     _MT=true;
     break;
    case 'o':    // OraclePerl
     _Oraperl=true;
     break;
    case 'O':    // Oracle
/*     
      _Oracle=true;
      if(pchar+1 && *(pchar+1) =='I')_OracleW=true;
      if(pchar+1 && *(pchar+1) =='W')_OracleWarm=true;
*/
      break;
    case 'D':    //debug
     _debug=atoi(++pchar);
     break;
    case 'A':   //AMSDataDir
     amsd=++pchar;
      setenv("AMSDataDir",amsd,1);
     break;
    case 'a':   //AMSProdOutputDir
     amsp=++pchar;
      setenv("AMSProdOutputDir",amsp,1);
     break;
    case 'p':   //AMSProdPerl
     amsperl=++pchar;
      setenv("AMSProdPerl",amsperl,1);
     break;
    case 'I':   //Interface
     iface=++pchar;
     break;
    case 'C':   //corba ior
     ior=++pchar;
     break;
    case 'B':   //DBfile name
     dbfilename=++pchar;
     cout <<"  DBFile found "<<dbfilename<<endl;
     break;
    case 'b':   //DBfile name
     _MaxDBProcesses=atoi(++pchar);
     break;
    case 'R':   //RunFileName
     rfile=++pchar;
     break;
    case 'H':   //NominalHostFile
     nhost=++pchar;
     break;
    case 'S':   //NominalServer
     nserver=++pchar;
     break;
    case 'K':   //NominalKiller
     nkiller=++pchar;
     break;
    case 'F':   //NominalInterface
     niface=++pchar;
     break;
    case 'P':   //NominalProducer
     nproducer=++pchar;
     break;
    case 'N':   //NominalNtuple
     ntuplestring=++pchar;
     break;
    case 'T':   //EventTagOutputDir
     eventtagstring=++pchar;
     break;
    case 'U':   //uid
     uid=atoi(++pchar);
     break;
  }
 }
if(!_MT){
// check there is no other 

  AString fout="/tmp/";
  fout+="server.sc"; 
  AString cmd="ps -elfw --cols 200  ";
  cmd+=" 1>";
  cmd+=(const char*)fout;
  cmd+=" 2>&1 ";
  int i=system((const char*)cmd);
  if(i==0 ){
   AString dc="";
   ifstream fbin;
   fbin.open((const char*)fout);
   AString fscript="";
    int count=0;
    while(fbin.good() && !fbin.eof()){
     fbin.ignore(1024,' ');
     char tmpbuf[256];
     fbin>>tmpbuf;
     if(strstr(tmpbuf,"amsprodserver.exe")){
      count++;
     } 
    }
    fbin.close();
    unlink((const char*)fout);
    if(count>1){
      cerr <<"AMSProdserver-F-FoundInstanceWhileNotInMT  "<<endl;
      abort();
    }
   }
   else{
    cerr<<"AMSProdserver-E-UnableTo "<<cmd<<endl;
   }
}
setdbfile(dbfilename);
_getpidhost(uid,iface);
if(_debug)_openLogFile("Server",false);
_pid.Type=DPS::Client::Server;
if(iface)_pid.Interface = (const char *)iface;
else _pid.Interface= (const char *)"default";
   AString amsdatadir; 
   char* gtv=getenv("AMSDataDir");
   if(gtv && strlen(gtv)>0){
     amsdatadir=gtv;
   }
    else FMessage("AMSDataDirNotDefined",DPS::Client::CInAbort);
    amsdatadir+="/DataBase/";
     AMSDBc::amsdatabase=new char[strlen((const char*)amsdatadir)+1];
     strcpy(AMSDBc::amsdatabase,(const char *)amsdatadir);
     cout <<"amsdatadir  "<<AMSDBc::amsdatabase<<endl;
if(niface){
 bool match=false;
 ifstream fbin;
 fbin.open(niface);
 if(fbin){
  while(!fbin.eof() && fbin.good()){ 
   if(fbin.get()=='#'){
      fbin.ignore(1024,'\n');
      continue;
   }
   else fbin.seekg(fbin.tellg()-sizeof(char));
   char tmpbuf[1024];
   fbin>>tmpbuf;
   if(fbin.good()){
     



  try{
   OrbitVars e;



   if(_MT){
      e._orb=CORBA::ORB_init(argc,argv,"orbit-local-mt-orb");
   }
   else{
   if(1 || strstr(tmpbuf,"default"))e._orb=CORBA::ORB_init(argc,argv);
   else {
    AString a=(const char*)_pid.HostName;
     for (int i=0;i<a.length();i++){
      if(a[i]=='.'){
       for (int j=i;j<a.length();j++)a[j]='\0';
       break;
      }
     }
    a+=".";
    a+=tmpbuf;
       e._orb=CORBA::ORB_init(argc,argv,(const char *)a);
   }

  }

   CORBA::Object_var obj=e._orb->resolve_initial_references("RootPOA");
   e._poa=PortableServer::POA::_narrow(obj);

/*
   // orbit doesnot support policies in cpp yet -> mod the sources, so root poa now has multiple_id policy
   PortableServer::IdUniquenessPolicy_var lf=e._poa->create_iduniqueness_policy(PortableServer::MULTIPLE_ID);
   CORBA::Policy pl;
   pl.length(1);
    pl[0]=lf;
   e._poa=e._poa->create_POA("AMSPOA",PortableServer::POAManager::_nil(),pl);
*/
   e._mgr=e._poa->the_POAManager();
   e._mgr->activate();


//
// tmp
//
   cout << " tmp init started "<<endl;
   Server_impl * ptr= new Server_impl();
   PortableServer::ObjectId_var oid=e._poa->activate_object(ptr);
   static DPS::Server_var _ref = reinterpret_cast<DPS::Server_ptr>(e._poa->id_to_reference(oid));
   cout <<" orb "<<&(e._orb)<<" "<<_ref<<" "<<endl;
   CORBA::Object *ppp=(void*)_ref;
   cout <<"  ppp "<<ppp<<endl;
   CORBA::String_var str=e._orb->object_to_string(ppp);
   cout << str<<endl;


   AString a(tmpbuf);
   _orbmap[a]=e;
   if(!strcmp((const char*)a,(const char *)_pid.Interface))match=true;
   }   
  catch (CORBA::SystemException &ex){
   cerr <<"AMSServer::AMSServer-E-CorbaSysExeceptionOnInit "<<tmpbuf<<endl;
  }
  }
  }
 }
 else{
 
  FMessage("AMSServer::AMSServer-F-UnableToOpenNIFile ",DPS::Client::CInAbort);
 }
 if(_orbmap.size()<1){
   FMessage("AMSServer::AMSServer-F-NoInterfacesOpen ",DPS::Client::CInAbort);
 }
 if(!match){
  FMessage("AMSServer::AMSServer-F-UnableToMatchInterace ",DPS::Client::CInAbort);
 }
}
else{
  FMessage("AMSServer::AMSServer-F-UnableToFindNIFile ",DPS::Client::CInAbort);
}

 if(ior==0){      //  Primary
  _pser= new Server_impl(_orbmap,_pid,this,nserver,nhost,nkiller);
   if(_Oracle){
    _pser->add(new DBServer_impl(_orbmap,_pid,this));
   } 
   else{
    _pser->add(new Client_impl(DPS::Client::DBServer,"DBServerPerl",this));
   }
  if(rfile){  
   _pser->add(new Producer_impl(_orbmap,_pid,this,nproducer,rfile,ntuplestring,eventtagstring));
  }  
   _pser->add(new Client_impl(DPS::Client::Monitor,"Monitor",this));
  _pser->_init();
  Server_impl * pser=dynamic_cast<Server_impl*>(_pser);
  pser->setInterface(niface);

   if(IsOracle()){
// Reread(rewrite) everything in case of db-aware program
     DPS::Server_var _svar;
     DPS::DBServer_var _dbsvar;

     CORBA::Object_var obj;
     for (AMSServerI* pcur=_pser;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
     if(pcur->getType()==DPS::Client::Server){
       CORBA::Object_var obj=pcur->getdefaultorb()->string_to_object((const char *)((pcur->getrefmap().begin())->second));
      _svar= DPS::Server::_narrow(obj);
     }
     if(pcur->getType()==DPS::Client::DBServer){
       CORBA::Object_var obj=pcur->getdefaultorb()->string_to_object((const char *)((pcur->getrefmap().begin())->second));
      _dbsvar= DPS::DBServer::_narrow(obj);
     }
   }
     if(!InitOracle())pser->StartSelf(_pid,DPS::Client::Create);
     for (AMSServerI* pcur=_pser;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
     if(pcur->getType()!=DPS::Client::DBServer){
      if(InitOracle()){
        pcur->ReWriteTables(_dbsvar);
      }
      else{
        pcur->ReReadTables(_svar);
     }
     }
     }
     if(InitOracle())pser->StartSelf(_pid,DPS::Client::Create);
   }
   else pser->StartSelf(_pid,DPS::Client::Create);

  
 }
 else{
 try{
   CORBA::Object_var obj=(_orbmap.find((const char *)_pid.Interface)->second)._orb->string_to_object(ior);
    if(!CORBA::is_nil(obj)){
     DPS::Server_var _svar=DPS::Server::_narrow(obj);
     if(!CORBA::is_nil(_svar)){
         
        if(!(_svar->sendId(_pid,_pid.Mips,90))){
         FMessage("Server Requested Termination after sendID ",DPS::Client::SInAbort);
        }
        else{
         _pser= new Server_impl(_orbmap,_svar,_pid,this);
          Producer_impl a;
          if(_svar->TypeExists(a.getType())){
              _pser->add(new Producer_impl(_orbmap,_svar,_pid,this));
          }
           _pser->add(new Client_impl(DPS::Client::Monitor,"Monitor",_svar,_pid,this));
   if(_Oracle){
           _pser->add(new DBServer_impl(_orbmap,_svar,_pid,this));
   }
   else{
           _pser->add(new Client_impl(DPS::Client::DBServer,"DBServerPerl",_svar,_pid,this));
   }
  Server_impl * pser=dynamic_cast<Server_impl*>(_pser);
  pser->setInterface(niface);
  pser->StartSelf(_pid,DPS::Client::Update);

    }
 }
 }
}
 catch(CORBA::SystemException &ex){
   FMessage("AMSServer::AMSServer-F-UnableToInitIOR ",DPS::Client::CInAbort);
  } 
}
}




void AMSServer::Listening(int sleeptime){
typedef map<AString, AMSServer::OrbitVars>::iterator MOI; 
      int ntry=sleeptime*1000000/int(AMSServer::Singleton()->getSleepTime());
      if(ntry<=0)ntry=1;
     if(AMSServer::Singleton()->getSleepTime()<20000){
       cout <<"  entering listening " << "  "<<AMSServer::Singleton()->getSleepTime() <<" "<<sleeptime<<"  "<<ntry<<endl;
   }
      for(int itry=0;itry<ntry;itry++){
       usleep(AMSServer::Singleton()->getSleepTime());
      for(MOI i=_orbmap.begin();i!=_orbmap.end();++i){
//        if(sleeptime<0)((*i).second)._orb->run();
//        else ((*i).second)._orb->perform_work();
          ((*i).second)._orb->perform_work();
      }
     }
     if(AMSServer::Singleton()->getSleepTime()<20000){
       cout <<"  exiting listening " << "  "<<AMSServer::Singleton()->getSleepTime() <<" "<<sleeptime<<endl;
   }
}

void AMSServer::UpdateDB(bool force){
// Her don't know yet
for(AMSServerI * pcur=_pser; pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
pcur->UpdateDB(force);
}
}

void AMSServer::SystemCheck(bool force){
static int check=0;
check++;
// Here run Start,Stop,Kill,Check Clients

    int count=AMSServer::Singleton()->MT()?-1:1;
for(AMSServerI * pcur=_pser; pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
/*
  Listening(count);
  if(!_GlobalError)pcur->StartClients(_pid);
  Listening(count);
  if(!_GlobalError) pcur->CheckClients(_pid);
  Server_impl* pser=dynamic_cast<Server_impl*>(pcur);
  if(pser)pser->AdvancedPing();
  Listening(count);
 if(!_GlobalError)pcur->KillClients(_pid);
 else Listening(count);
*/
  if(!force)Listening(count);
  pcur->StartClients(_pid);
  if(check<1000)cout << "  start ok "<<endl;
  if(!force)Listening(count);
  pcur->CheckClients(_pid);
  if(check<1000)cout << "  check ok "<<endl;
  if(!force)Listening(count);
  pcur->KillClients(_pid);
  if(check<1000)cout << "  kill ok "<<endl;

}
if(force)IMessage("ForceSystemCheckSuccessful");      

}

void AMSServer::Exiting(const char * message){
if(_ExitInProgress)return;
cout<< " Exiting ...."<<(message?message:" ")<<endl;
_ExitInProgress=true;
  Server_impl* pser=dynamic_cast<Server_impl*>(_pser);
    DPS::Client::ARS * pars;
    int length=pser?pser->getARS(_pid,pars):0;
    DPS::Client::ARS_var arf=pars;
  for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=(_orbmap.find((const char*)(arf[i].Interface))->second)._orb->string_to_object(arf[i].IOR);

    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->Exiting(_pid,(message?message:AMSClient::_error.getMessage()),AMSClient::_error.ExitReason());
typedef map<AString, AMSServer::OrbitVars>::iterator MOI; 
      for(MOI i=_orbmap.begin();i!=_orbmap.end();++i){
       ((*i).second)._orb->shutdown(false);
      }
     break;
   }
   catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
     
   }
}
}

void AMSServerI::_UpdateACT(const DPS::Client::CID & cid, DPS::Client::ClientStatus st){
{
    time_t tt;
    time(&tt);
    _parent->LastServiceTime()=tt;
}
     for(ACLI j=_acl.begin();j!=_acl.end();++j){
     if((*j)->id.uid==cid.uid){
       time_t tt;
       time(&tt);
       (*j)->LastUpdate=tt;
       (*j)->Status=st;
       DPS::Client::ActiveClient_var acv=*j;
       PropagateAC(acv,DPS::Client::Update);
       return;
     }   
     }
}

void AMSServerI::PropagateAC(DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc,DPS::Client::AccessType type, uinteger uid){

  PropagateACDB(ac,rc);
//         cout <<" entering Producer_impl::propagateac"<<endl;

  if(_ActivateQueue)_acqueue.push_back(ACA(ac,rc));
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=_pser->getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,type,uid);
    DPS::Client::ARS_var arf=pars;
  if(length==0){
      _parent->EMessage(AMSClient::print(ac,"PropagateAC-getARS retrurns 0"));
  }
//  cout <<"propagateAC - getars returns "<<length<<endl;
  time_t tt;
  time(&tt);
  for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
     DPS::Client::CID acid=ac.id;
    _pvar->sendAC(acid,ac,rc);
   }
   catch (CORBA::SystemException &ex){
     cerr<<"oops corba exc found for "<<i<<" "<<ctime(&tt)<< endl;
/*
     DPS::Client::ActiveClient ac;
     ac.id.uid=arf[i].uid;
     ACLI li=find_if(_pser->getacl().begin(),_pser->getacl().end(),Eqs(ac));
     if(li!=_pser->getacl().end()){
      (*li)->Status==DPS::Client::TimeOut;
      PropagateAC(*li,DPS::Client::Update,DPS::Client::AnyButSelf,ac.id.uid);
     }
     else{
      _parent->EMessage(AMSClient::print(ac," PropagateRunExceptionClientNotFou\
nd"));
     }
*/
   }
  } 
//         cout <<" exiting Producer_impl::propagateac"<<endl;
  }   

void AMSServerI::PropagateAH(const DPS::Client::CID & pid,DPS::Client::ActiveHost & ah,DPS::Client::RecordChange rc,DPS::Client::AccessType type, uinteger uid){
  PropagateAHDB(pid,ah,rc);
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=_pser->getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,type,uid);
    DPS::Client::ARS_var arf=pars;
  if(length==0){
      _parent->EMessage(AMSClient::print(ah,"PropagateAC-getARS retrurns 0"));
  }
//  cout <<"propagateAC - getars returns "<<length<<endl;
  time_t tt;
  time(&tt);
  for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->sendAH(pid,ah,rc);
   }
   catch (CORBA::SystemException &ex){
     cerr<<"oops corba exc found for "<<i<<" "<<ctime(&tt)<<endl;
   }
  } 
  }   

void AMSServerI::RegisteredClientExists(){
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
for(ACLI li= _pser->getacl().begin();li!=_pser->getacl().end();++li){
 if((*li)->Status==DPS::Client::Registered){
 if(!_ActivateQueue){
   if(_parent->Debug())_parent->IMessage("Queue Activated");
   _ActivateQueue=true;
   _RecID=((*li)->id.uid);
 }   
   return;
 }
}
if(_ActivateQueue){
   _ActivateQueue=false;
   _PurgeQueue();
   if(_parent->Debug())_parent->IMessage("Queue Purged");

}
}

bool AMSServerI::InactiveClientExists(DPS::Client::ClientType ctype){
for(ACLI li= _acl.begin();li!=_acl.end();++li){
 if((*li)->Status!=DPS::Client::Active){
//   if(_parent->Debug())_parent->IMessage(AMSClient::print(*li," Inactive Client Found"));
   return true;
  }
}
if(getType() == DPS::Client::DBServer && getType()!=ctype){
  if(_parent->DBServerExists() && !_acl.size())return true;
}
return false;
}



Server_impl::Server_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient* parent, char* NS, char * NH, char *NK): POA_DPS::Server(),AMSServerI(AMSID("Server",0),parent,DPS::Client::Server){
typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;
Server_impl * pcur =0;
for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new Server_impl());
  PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
   static DPS::Server_ptr _ref = reinterpret_cast<DPS::Server_ptr>((i->second)._poa->id_to_reference(oid));
  CORBA::Object *ppp=(void*)_ref; 
   _refmap[i->first]=(i->second)._orb->object_to_string(ppp);
   if(!strcmp((const char *)(i->first),(const char*)cid.Interface)){
    _defaultorb=(i->second)._orb;
    
   }
}


if(NK){
 ifstream fbin;
 fbin.open(NK);
 if(fbin){
   unsigned int uid=0;
   DPS::Client::NominalClient_var ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>ncl->MaxClients>>ncl->CPUNeeded>>ncl->MemoryNeeded;
    char tmpbuf[1024];
    fbin>>tmpbuf;
    ncl->Type=DPS::Client::Killer;
    ncl->WholeScriptPath=(const char*)tmpbuf;
   fbin.ignore(1024,'\n');
  while(!fbin.eof() && fbin.good()){ 
   fbin>>tmpbuf;
   ncl->HostName=(const char*)tmpbuf;
   fbin>>ncl->LogInTheEnd;
   fbin.ignore(1024,'\n');
   fbin.getline(tmpbuf,1024);
   ncl->LogPath= (const char*)tmpbuf;
   fbin.getline(tmpbuf,1024);
    ncl->SubmitCommand=(const char*)tmpbuf;
    ncl->uid=++uid;
    if(fbin.good())_nki.push_back(ncl);
  }
 fbin.close();
 }
 else{
  cerr<<"Server_impl::Server_impl-F-UnableToOpenNKFile "<<NK<<endl;
  _parent->FMessage("Server_impl::Server_impl-F-UnableToOpenNKFile ",DPS::Client::CInAbort);
}
}
else _parent->FMessage("Server_impl::Server_impl-F-NoNKFile",DPS::Client::CInAbort);




if(NS){
 ifstream fbin;
 fbin.open(NS);
 if(fbin){
   unsigned int uid=0;
   DPS::Client::NominalClient_var ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>ncl->MaxClients>>ncl->CPUNeeded>>ncl->MemoryNeeded;
    char tmpbuf[1024];
    fbin>>tmpbuf;
    ncl->WholeScriptPath=(const char*)tmpbuf;
    ncl->Type=DPS::Client::Server;
   fbin.ignore(1024,'\n');
  while(!fbin.eof() && fbin.good()){ 
   fbin>>tmpbuf;
   ncl->HostName=(const char*)tmpbuf;
   fbin>>ncl->LogInTheEnd;
   fbin.ignore(1024,'\n');
   fbin.getline(tmpbuf,1024);
   ncl->LogPath= (const char*)tmpbuf;
   fbin.getline(tmpbuf,1024);
    ncl->SubmitCommand=(const char*)tmpbuf;
    ncl->uid=++uid;
    if(fbin.good())_ncl.push_back(ncl);
 }
 fbin.close();
 }
 else{
  cerr<<"Server_impl::Server_impl-F-UnableToOpenNSFile "<<NS<<endl;
  _parent->FMessage("Server_impl::Server_impl-F-UnableToOpenNSFile ",DPS::Client::CInAbort);
}
}
else _parent->FMessage("Server_impl::Server_impl-F-NoNSFile",DPS::Client::CInAbort);
if(NH){
 ifstream fbin;
 fbin.open(NH);
 if(fbin){
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
  while(!fbin.eof() && fbin.good()){ 
   DPS::Server::NominalHost nh;
   char tmpbuf[1024];
   fbin>>tmpbuf;
   nh.HostName=(const char*)tmpbuf;
   fbin>>tmpbuf;
   nh.Interface=(const char*)tmpbuf;
   AString hn((const char*)(nh.HostName));
   if(strcmp((const char*)nh.Interface,"default")){
    hn+=".";
    hn+=(const char*)nh.Interface;
   }
   nh.HostName=(const char *)hn;
   fbin>>tmpbuf;
   nh.OS= (const char*)tmpbuf;
   fbin>>nh.CPUNumber>>nh.Memory>>nh.Clock;
   DPS::Client::NominalHost_var vnh= new  DPS::Client::NominalHost(nh);
   if(fbin.good())_nhl.push_back(vnh);  
 }
 fbin.close();
 }
 else{
  cerr<<"Server_impl::Server_impl-F-UnableToOpenNHFile "<<NH<<endl;
  _parent->FMessage("Server_impl::Server_impl-F-UnableToOpenNHFile ",DPS::Client::CInAbort);
}
}
else _parent->FMessage("Server_impl::Server_impl-F-NoNHFile",DPS::Client::CInAbort);




}


Server_impl::Server_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _pvar, const DPS::Client::CID & cid,AMSClient* parent): POA_DPS::Server(),AMSServerI(AMSID("Server",0),parent,DPS::Client::Server){
typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;

Server_impl * pcur =0;

for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new Server_impl());
  PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  DPS::Server_ptr _ref = reinterpret_cast<DPS::Server_ptr>((i->second)._poa->id_to_reference(oid));
   _refmap[i->first]=(i->second)._orb->object_to_string(_ref);
   if(!strcmp((const char *)(i->first),(const char*)cid.Interface)){
    _defaultorb=(i->second)._orb;
   }
}

     ReReadTables(_pvar);

}


void AMSServerI::_UpdateHosts(){
         for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
            int runcl=0;
           for (ACLI j=_acl.begin();j!=_acl.end();++j){
            if(!strcmp((const char *)(*i)->HostName, (const char *)(((*j)->id).HostName))){
              runcl++;
            }
           }
           if(runcl!=(*i)->ClientsRunning){
            _parent->EMessage(AMSClient::print(*i,"ResettinHost "));
            (*i)->ClientsRunning=runcl;
             DPS::Client::CID cid=_parent->getcid();      
             cid.Type=getType();
             cid.Interface= (const char *) " "; 
             PropagateAH(cid,(*i),DPS::Client::Update);
           }
         }
}

void Server_impl::StartClients(const DPS::Client::CID & pid){
{
    time_t tt;
    time(&tt);
    _parent->LastServiceTime()=tt;
}

if(!Master() || _parent->MT())return;
RegisteredClientExists();
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
if(pcur->InactiveClientExists(getType()))return;
 
}
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 


 _UpdateHosts();

 // Check if there are some hosts to run on
 DPS::Client::ActiveHost_var ahlv=new DPS::Client::ActiveHost();
 bool suc=false;
 if(_parent->DBServerExists() && getHostInfoSDB(_parent->getcid(),ahlv)){
  suc=true;
 }
 else{
 _ahl.sort(Less());
if(_acl.size()<(*_ncl.begin())->MaxClients ){
 for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
  if((*i)->Status!=DPS::Server::NoResponse && (*i)->Status!=DPS::Server::InActive){
    if((*i)->ClientsRunning>=(*i)->ClientsAllowed)continue;
    // HereStartClient
    ahlv=*i;
    suc=true;
    break;
   }
  }
 }
 }
  if(suc){
  if(!_pser->Lock(pid,DPS::Server::StartClient,getType(),_StartTimeOut))return;
   CORBA::String_var _refstring=_refmap.find((const char *)((ahlv)->Interface))->second;
  if(! (const char*)(_refstring)){
       _parent->EMessage(AMSClient::print(ahlv, " Could not find refstring for the host "));
     _refstring=_refmap.find((const char *)("default"))->second;
    
  }
    cout <<" interface :"<<((ahlv)->Interface)<<" "<<_refstring<<endl;
    NCLI cli=find_if(_ncl.begin(),_ncl.end(),NCL_find((const char *)(ahlv)->HostName)); 
    if(cli==_ncl.end())cli=_ncl.begin();
    AString submit;
    submit+=(const char*)((*cli)->SubmitCommand);  
    submit+=" ";
    submit+=(const char*)((ahlv)->HostName);
     char tmp[80];
     sprintf(tmp,"%d",_Submit+1);
     submit+=" ";
    if(!(*cli)->LogInTheEnd){
     submit+=(const char*)((*cli)->LogPath);  
     submit+="Server.";
     submit+=tmp;
     submit+=".log ";
    }
     char* gtv=getenv((const char*)((*cli)->WholeScriptPath));
     if(!gtv){
        AString a("-E-CouldNot getenv for ");
        a+=(const char*)((*cli)->WholeScriptPath);
        a+=" ContinueWithFingersCrossing";
       _parent->EMessage((const char *)a);
       submit+=(const char*)((*cli)->WholeScriptPath);  
     }
     else submit+=(const char*)gtv;
    submit+=" -C";
    submit+=(const char*) _refstring;
    submit+=" -U";
    submit+=tmp;
    submit+=" -F";
    submit+= _iface;
    submit+=" -I";
    submit+=(const char*)(ahlv)->Interface;
    if(_parent->IsOracle())submit+=" -O";
    if(_parent->Debug())submit+=" -D1";
    if(_parent->MT())submit+=" -M";
    UpdateDBFileName(); 
    if(_parent->getdbfile()){
     submit+=" -B";
     submit+=_parent->getdbfile();
    }
    submit+=" -b";
    sprintf(tmp,"%d",_parent->getmaxdb());
    submit+=tmp;
    submit+=" -A";
     submit+=getenv("AMSDataDir");
     if(getenv("AMSProdOutputDir")){
      submit+=" -a";
      submit+=getenv("AMSProdOutputDir");
     }
    if((*cli)->LogInTheEnd){
     submit+=" ";
     submit+=(const char*)((*cli)->LogPath);  
     submit+="Server.";
     sprintf(tmp,"%d",_Submit+1);
     submit+=tmp;
     submit+=".log ";
    }
    if(_parent->Debug())_parent->IMessage(submit);
    submit+=" &";
    int out=systemC(submit);
     time_t tt;
     time(&tt);
     (ahlv)->LastUpdate=tt;     
    if(out==0){
     // Add New Active Client
     DPS::Client::ActiveClient ac;
     (ac.id).HostName=CORBA::string_dup((ahlv)->HostName);
     ac.id.Interface=(ahlv)->Interface;
     (ac.id).uid=_Submit+1;
     ac.id.Type=getType();
     ac.id.pid=0;
     ac.id.ppid=0;
     ac.id.Status=DPS::Client::NOP;
     ac.Status=DPS::Client::Submitted;
     ac.id.StatusType=DPS::Client::Permanent;
     ac.StatusType=DPS::Client::Permanent;
     ac.TimeOut=_KillTimeOut;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
     ((ac.ars)[0]).Interface=(const char *)("Dummy");
     ((ac.ars)[0]).IOR=(const char *)(" ");
     ((ac.ars)[0]).Type=DPS::Client::Generic;
     ((ac.ars)[0]).uid=0;
/*
     _acl.push_back(ac);
*/
//     ((ahlv)->ClientsRunning)++;
    (ahlv)->Status=DPS::Client::OK; 
     DPS::Client::CID cid=_parent->getcid();      
     cid.Type=getType();
     cid.Interface= (const char *) " "; 
    PropagateAH(cid,(ahlv),DPS::Client::Update);
    DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(ac);
    PropagateAC(acv,DPS::Client::Create);
//      Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
//      _pser->MonInfo("Started client ",DPS::Client::Info);
    }
    else{
     HostClientFailed(ahlv); 
      
     DPS::Client::CID cid=_parent->getcid();      
     cid.Type=getType();
     cid.Interface= (const char *) " "; 
     PropagateAH(cid,(ahlv),DPS::Client::Update);

     AString a="StartClients-E-UnableToStartClient ";
     a+=submit;
     _parent->EMessage((const char *) a);
      Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
      _pser->MonInfo((const char *) a,DPS::Client::Error);

     
    }
    _pser->Lock(pid,DPS::Server::ClearStartClient,getType(),_StartTimeOut);
  }
}

#include <signal.h>
void Server_impl::KillClients(const DPS::Client::CID & pid){
{
    time_t tt;
    time(&tt);
    _parent->LastServiceTime()=tt;
}
if(!Master())return;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(pid,DPS::Server::KillClient,getType(),_KillTimeOut))return;

ACLI li=find_if(_acl.begin(),_acl.end(),find(DPS::Client::Killed));
if(li!=_acl.end()){
   if(_pser->MonDialog(AMSClient::print(*li,"Asking To Kill Client: "),DPS::Client::Error)){
 //kill by -9 here
   if(_parent->Debug())_parent->EMessage(AMSClient::print(*li, " KILL -9"));
    int iret=_pser->Kill((*li),SIGKILL,true);
    if(iret){
     _parent->EMessage(AMSClient::print(*li,"Server::Unable To Kill Client"));
    }
    DPS::Client::ActiveClient_var acv=*li;
    PropagateAC(acv,DPS::Client::Delete,DPS::Client::AnyButSelf,acv->id.uid);
  }
  else{
    _UpdateACT((*li)->id,DPS::Client::Active);
  }

}
 li=find_if(_acl.begin(),_acl.end(),find(DPS::Client::TimeOut));
if(li!=_acl.end()){
 
 if(!_pser->pingHost((const char*)((*li)->id.HostName))){
    for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
      if(!strcmp((const char *)(*i)->HostName, (const char *)((*li)->id).HostName)){
       (*i)->Status=DPS::Server::NoResponse;
       _parent->EMessage(AMSClient::print(*i,"No Response From: "));
       _pser->MonInfo(AMSClient::print(*i,"No Response From: "),DPS::Client::Warning);
    }
}  
   (*li)->id.Status=DPS::Client::SInKill;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete);
 }
  else{
   if(1 || _pser->MonDialog(AMSClient::print(*li,"Asking To Kill Client: "),DPS::Client::Error)){
   (*li)->id.Status=DPS::Client::SInKill;
   (*li)->Status=DPS::Client::Killed;
   DPS::Client::ActiveClient_var acv=*li;
    PropagateAC(acv,DPS::Client::Update,DPS::Client::AnyButSelf,acv->id.uid);
//    _pser->Kill((*li),SIGTERM,true);
  }
  else{
    _UpdateACT((*li)->id,DPS::Client::Active);
  }
 }
}

_pser->Lock(pid,DPS::Server::ClearKillClient,getType(),_KillTimeOut);


}

void Server_impl::CheckClients(const DPS::Client::CID & cid){
{
    time_t tt;
    time(&tt);
    _parent->LastServiceTime()=tt;
}
if(!Master())return;
static int iorder=0;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(cid,DPS::Server::CheckClient,getType(),_KillTimeOut))return;
for(AHLI li=_ahl.begin();li!=_ahl.end();++li){
 if((*li)->Status==DPS::Server::NoResponse){
 iorder=(iorder+1)%60;
    if(iorder==1 && _pser->pingHost((const char*)((*li)->HostName))){
        (*li)->Status=DPS::Client::OK;
     DPS::Client::CID cid=_parent->getcid();      
     cid.Type=getType();
     cid.Interface= (const char *) " "; 
     PropagateAH(cid,*li,DPS::Client::Update,DPS::Client::AnyButSelf);

    }
 }
}
time_t tt;
time(&tt);
for(ACLI li=_acl.begin();li!=_acl.end();++li){
 // find clients with timeout
 if((*li)->Status!=DPS::Client::Killed && (*li)->LastUpdate+(*li)->TimeOut<tt){
   ;
   if(PingServer(*li)){
     
    _UpdateACT((*li)->id,DPS::Client::Active);
//    (*li)->LastUpdate=tt;
   }
   else{
    DPS::Client::ActiveClient_var acv=*li;
    acv->Status=DPS::Client::TimeOut;
    if(_parent->Debug())_parent->EMessage(AMSClient::print(acv,"Client TIMEOUT"));
    PropagateAC(acv,DPS::Client::Update,DPS::Client::AnyButSelf,acv->id.uid);
 }
 }
}

_pser->Lock(cid,DPS::Server::ClearCheckClient,getType(),_KillTimeOut);
}


void Server_impl::_init(){

// here active host list
if(_ahl.size())return;


 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  


 if(_pser){
  for(NHLI i=(_pser->getnhl()).begin();i!=(_pser->getnhl()).end();++i){
   DPS::Client::ActiveHost ah;
   ah.HostName=CORBA::string_dup((*i)->HostName);
   ah.Interface=CORBA::string_dup((*i)->Interface);
   if(_pser->pingHost((const char*)(ah.HostName)))ah.Status=DPS::Client::OK; 
   else ah.Status=DPS::Client::NoResponse; 
   ah.ClientsProcessed=0;
   ah.ClientsFailed=0;
   ah.LastFailed=0;
   ah.ClientsKilled=0;
   ah.ClientsRunning=0;
   ah.Clock=(*i)->Clock;  
   ah.ClientsAllowed=min((*i)->CPUNumber/(*_ncl.begin())->CPUNeeded,(*i)->Memory/float((*_ncl.begin())->MemoryNeeded));
 
   time_t tt;
   time(&tt);
   ah.LastUpdate=tt;
//Check Interface
   if(_refmap.find((const char *)(ah.Interface))==_refmap.end()){
   ah.Interface=(const char *)((_refmap.begin())->first);
#ifdef __AMSDEBUG__
       if((_refmap.begin())->first != "default"){
        cerr<<"Server_impl::_init-S-FirstRefmapEntry!=default "<<(_refmap.begin())->first<<endl;
   }
#endif
   }
   DPS::Client::ActiveHost_var vah= new DPS::Client::ActiveHost(ah);
   _ahl.push_back(vah);
  }
 }
 else{
  _parent->FMessage("Server_impl::_init-F-UnableToConnectToServer",DPS::Client::CInAbort);
 }

}



  CORBA::Boolean Server_impl::sendId(DPS::Client::CID& cid, float Mips,uinteger timeout) throw (CORBA::SystemException){
//cout <<" entering Server_impl::sendId"<<endl;
      cid.Mips=Mips;
if(cid.Type==DPS::Client::Server){
     for(ACLI j=_acl.begin();j!=_acl.end();++j){
      if(((*j)->id).uid==cid.uid){
       ((*j)->id).pid=cid.pid;
       ((*j)->id).ppid=cid.ppid;
       cid.Interface=CORBA::string_dup(((*j)->id).Interface);
       cid.Type=((*j)->id).Type;
       (*j)->Status=DPS::Client::Registered;
       time_t tt;
       time(&tt);
       (*j)->LastUpdate=tt;
       (*j)->TimeOut=timeout;
       DPS::Client::ActiveClient_var acv=*j;
         PropagateAC(acv,DPS::Client::Update );
#ifdef __AMSDEBUG__
        _parent->IMessage(AMSClient::print(cid,"Server_impl::sendId-I-RegClient") );
#endif 
//cout <<" exiting Server_impl::sendId 1"<<endl;
       return true;
      }
     }
//cout <<" exiting Server_impl::sendId 2"<<endl;
     return false;
}
else if(cid.Type==DPS::Client::Monitor){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      return (dynamic_cast<Client_impl*>(pcur))->sendId(cid,cid.Mips,timeout);
      break;
    }
}
}
else if(cid.Type==DPS::Client::DBServer){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      return (dynamic_cast<Client_impl*>(pcur))->sendId(cid,cid.Mips,timeout);
      break;
    }
}
}
//cout <<" exiting Server_impl::sendId 3"<<endl;

}

int Server_impl::getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out acs)throw (CORBA::SystemException){

//cout <<" entering Server_impl::getNC"<<endl;

 if(_parent->DBServerExists()){
        Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(unsigned int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getNC(cid,acs);
       }
       catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
        _parent->EMessage("dbserver corba error during getnc" );   
       }
     }
 }


DPS::Client::NCS_var acv= new DPS::Client::NCS();
unsigned int length=0;
for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
if(pser->getType()==cid.Type){
length=pser->getncl().size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(NCLI li=pser->getncl().begin();li!=pser->getncl().end();++li){
 acv[length++]=*li;
}
}
acs=acv._retn();
//cout <<" exiting Server_impl::getNC"<<endl;

return length;
}
}


}

int Server_impl::getNK(const DPS::Client::CID &cid, DPS::Client::NCS_out acs)throw (CORBA::SystemException){
 if(_parent->DBServerExists()){
        Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(int unsigned i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getNK(cid,acs);
       }
       catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
        _parent->EMessage("dbserver corba error during getnk" );   
       }
     }
 }

DPS::Client::NCS_var acv= new DPS::Client::NCS();
unsigned int length=0;
length=_nki.size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(NCLI li=_nki.begin();li!=_nki.end();++li){
 acv[length++]=*li;
}
}
acs=acv._retn();
return length;
}



bool Server_impl::ARSaux(DPS::Client::AccessType type,uint id, uint compare){
 switch (type){
  case DPS::Client::Any:
  return true;
  case DPS::Client::LessThan:
   return compare<id;
  case DPS::Client::Self:
   return compare==id;
  case DPS::Client::AnyButSelf:
   return compare!=id;
  case DPS::Client::GreaterThan:
   return compare>id;
  default:
   return false;
 }
}


int Server_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out  arf,DPS::Client::AccessType type,uinteger maxcid, int selffirst)throw (CORBA::SystemException){
DPS::Client::ARS_var arv=new DPS::Client::ARS();
unsigned int length=0;
AMSServerI * _pser=getServer();
AMSServerI *  pser=0;
if(cid.Type == DPS::Client::Server || cid.Type == DPS::Client::Producer){
 pser=_pser;
}
else{
 for(AMSServerI * pcur=_pser; pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
  if(pcur->getType() == cid.Type){
  //_parent->IMessage(AMSClient::print(cid," got ars  "));
    pser=pcur;
  }
}
}
if(!pser)pser=_pser;
   pser->getacl().sort(Less(cid,selffirst));
for (ACLI li=pser->getacl().begin();li!=pser->getacl().end();++li){
    bool pred=ARSaux(type,maxcid,(*li)->id.uid);
  if(pred){
   for (int i=0;i<((*li)->ars).length();i++){
    if( (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)cid.Interface)){
      length++;
    }
    else if( !strcmp(" ", (const char *)cid.Interface) && (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)((*li)->id).Interface)){
      length++;
    }
  }
 }
}

if(length==0){ 
  //arv->length(1);
//  cerr<<"***ERROR*** getars-length== 0"<<endl;
}
else arv->length(length);
length=0;
for (ACLI li=pser->getacl().begin();li!=pser->getacl().end();++li){
    bool pred=ARSaux(type,maxcid,(*li)->id.uid);
  if(pred){
  for (int i=0;i<((*li)->ars).length();i++){
    if( (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)cid.Interface)){
      arv[length++]=((*li)->ars)[i];
    }
    else if( !strcmp(" ", (const char *)cid.Interface) && (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)((*li)->id).Interface)){
      arv[length++]=((*li)->ars)[i];
    }
  }
 }
}

arf=arv._retn();
return length;
}




int Server_impl::getACS(const DPS::Client::CID &cid, DPS::Server::ACS_out acs, unsigned int & maxc)throw (CORBA::SystemException){
 if(_parent->DBServerExists()){
        Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(unsigned int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getACS(cid,acs,maxc);
       }
       catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
        _parent->EMessage("dbserver corba error during getacs" );   
       }
     }
 }


//_parent->IMessage(AMSClient::print(cid," type "));
DPS::Server::ACS_var acv= new DPS::Server::ACS();
unsigned int length=0;
for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
if(pser->getType()==cid.Type){
maxc=pser->getmaxcl();
length=pser->getacl().size();
if(length==0){
//acv->length(1);
}
else{
acv->length(length);
length=0;
for(ACLI li=pser->getacl().begin();li!=pser->getacl().end();++li){
 acv[length++]=*li;
 _parent->IMessage(AMSClient::print(acv[length-1],"getacs"));
}
}
acs=acv._retn();
return length;
}
}
return 0;

}



   void Server_impl::sendAC(const DPS::Client::CID &  cid,   DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc)throw (CORBA::SystemException){

//cout <<" entering Server_impl::sendAC"<<endl;

  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      ACLI li=find_if(pcur->getacl().begin(),pcur->getacl().end(),Eqs(ac));
      switch (rc){
       case DPS::Client::Update:
       if(li==pcur->getacl().end())_parent->EMessage(AMSClient::print(cid,"Client not found for editing"));
       else{
        DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(ac);
        replace_if(li,pcur->getacl().end(),Eqs(ac),vac);
       }
       break;
       case DPS::Client::Delete:
       if(li==pcur->getacl().end())_parent->EMessage(AMSClient::print(cid,"Client not found for deleting"));
       else{
        if(_parent->Debug())_parent->IMessage(AMSClient::print(ac,"Deleting Client "));
        Producer_impl * pprod=dynamic_cast<Producer_impl*>(pcur);
        if(pprod)pprod->RunFailed(ac);
        bool hostfound=false;
//      Here find the corr Ahost and update it
         for(AHLI i=pcur->getahl().begin();i!=pcur->getahl().end();++i){
           cout <<" host: "<<(const char *)(*i)->HostName<<endl;
            if(!strcmp((const char *)(*i)->HostName, (const char *)(ac.id).HostName)){
       
         hostfound=true;
         cout << " host found for deleteing "<<endl;
        ((*i)->ClientsRunning)--;
        if((*i)->ClientsRunning<0){
          _parent->EMessage("Server_impl::sendAC-F-NegativeNumberClientRunning ");
         (*i)->ClientsRunning=0;
        }
     time_t tt;
     time(&tt);
     (*i)->LastUpdate=tt;     

       switch ((ac.id).Status){
       case DPS::Client::CInExit: 
          ((*i)->ClientsProcessed)++; 
            (*i)->Status=DPS::Client::OK; 
        break;
        case DPS::Client::SInExit:
           ((*i)->ClientsProcessed)++; 
           (*i)->Status=DPS::Client::OK; 
            break;
       case DPS::Client::CInAbort:
        HostClientFailed(*i); 
        break;
       case DPS::Client::SInAbort:
        HostClientFailed(*i);
        break;
       case DPS::Client::SInKill:
        HostClientFailed(*i); 
        ((*i)->ClientsKilled)++; 
        break;
       }
       DPS::Client::CID cid=_parent->getcid();      
       cid.Type=getType();
       cid.Interface= (const char *) " "; 
        PropagateAH(cid,*i,DPS::Client::Update);
  
        break;
       }
       }
       if (!hostfound){
         _parent->EMessage(AMSClient::print(*li," No Host found while deleting client"));
       }
        pcur->getacl().erase(li);
       }
       break;
       case DPS::Client::Create:
       if(li !=pcur->getacl().end()){
         _parent->EMessage(AMSClient::print(*li,"Client alreadyexists"));
       }
       else{
        pcur->addone();
        if(_parent->Debug())_parent->IMessage(AMSClient::print(ac,"added client "));
        DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(ac);
       if(ac.Status == DPS::Client::Killed){
         vac->Status= DPS::Client::TimeOut;
        }
       pcur->getacl().push_back(vac);
       cout <<pcur->getacl().size()<<endl;
//     Here find the corr ahost and update it
         for(AHLI i=pcur->getahl().begin();i!=pcur->getahl().end();++i){
            if(!strcmp((const char *)(*i)->HostName, (const char *)(vac->id).HostName)){
//          cout << " host found for creating "<<endl;
        ((*i)->ClientsRunning)++;
        break;
}
         }
       }       
       break;
      }
    break;
    }
}
//cout <<" exiting Server_impl::sendAC"<<endl;
}

   void Server_impl::sendAH(const DPS::Client::CID &  cid,  const DPS::Client::ActiveHost & ah, DPS::Client::RecordChange rc)throw (CORBA::SystemException){

//cout <<" entering Server_impl::sendAH"<<endl;

  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      AHLI li=find_if(pcur->getahl().begin(),pcur->getahl().end(),Eqs_h(ah));
      switch (rc){
       case DPS::Client::Update:
       if(li==pcur->getahl().end())_parent->EMessage(AMSClient::print(ah,"Host not found for editing"));
       else{
        DPS::Client::ActiveHost_var vac= new DPS::Client::ActiveHost(ah);
        cout <<" SendAH-I-"<<" "<<cid.Type<<" "<<ah.HostName<<" "<<(*li)->HostName<<endl;
        replace_if(li,pcur->getahl().end(),Eqs_h(ah),vac);
       
       }
       break;
       case DPS::Client::Delete:
       if(li==pcur->getahl().end())_parent->EMessage(AMSClient::print(cid,"Host not found for deleting"));
       else{
        if(_parent->Debug())_parent->IMessage(AMSClient::print(ah,"Deleting Host "));
        pcur->getahl().erase(li);
       }
       break;
       case DPS::Client::Create:
       if(li!=pcur->getahl().end())_parent->EMessage(AMSClient::print(ah,"Host already exists"));
       else{
        DPS::Client::ActiveHost_var vac= new DPS::Client::ActiveHost(ah);
         pcur->getahl().push_back(vac);
       }
       break;
       default:
        _parent->EMessage(AMSClient::print(ah,"sendAH-Unknown rc value"));
      }
    break;
    }
}
cout <<" exiting Server_impl::sendAH"<<endl;
}


   void Server_impl::sendNH(const DPS::Client::CID &  cid, const DPS::Client::NominalHost & ah, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      NHLI li=find_if(pcur->getnhl().begin(),pcur->getnhl().end(),Eqs_nh(ah));
      switch (rc){
       case DPS::Client::Update:
       if(li==pcur->getnhl().end())_parent->EMessage(AMSClient::print(cid,"Host not found for editing"));
       else{
        DPS::Client::NominalHost_var vac= new DPS::Client::NominalHost(ah);
        replace_if(li,pcur->getnhl().end(),Eqs_nh(ah),vac);
       }
       break;
       case DPS::Client::Delete:
       if(li==pcur->getnhl().end())_parent->EMessage(AMSClient::print(cid,"Host not found for deleting"));
       else{
        if(_parent->Debug())_parent->IMessage(AMSClient::print(ah,"Deleting Host "));
        pcur->getnhl().erase(li);
       }
       break;
       case DPS::Client::Create:
       if(li!=pcur->getnhl().end())_parent->EMessage(AMSClient::print(ah,"Host already exists"));
       else{
        DPS::Client::NominalHost_var vac= new DPS::Client::NominalHost(ah);
         pcur->getnhl().push_back(vac);
       }
       break;
       default:
        _parent->EMessage(AMSClient::print(ah,"sendNH-Unknown rc value"));
      }
    break;
    }
}
}



void Server_impl::Exiting(const DPS::Client::CID & cid, const char * message, DPS::Client::ClientExiting status)throw (CORBA::SystemException){
//cout <<" entering Server_impl::Exiting"<<endl;
// find and remove client
if(cid.Type==DPS::Client::Server){
_parent->IMessage(AMSClient::print(cid,message?message:"Server exiting"));
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
_pser->MonInfo(AMSClient::print(cid,message?message:" Server Exiting "),DPS::Client::Info);
for( ACLI li=_acl.begin();li!=_acl.end();++li){
 if (cid.uid==((*li)->id).uid){
   (*li)->id.Status=status;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete,DPS::Client::AnyButSelf,cid.uid);
//cout <<" exiting Server_impl::Exiting"<<endl;
   return;
 }

}
 _parent->EMessage(AMSClient::print(cid,"Server_impl::Exiting::No Such Client"));
}
else{
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      (dynamic_cast<Client_impl*>(pcur))->Exiting(cid,message,status);
      break;
    }
}
}


}

int Server_impl::getNHS(const DPS::Client::CID &cid, DPS::Client::NHS_out acs)throw (CORBA::SystemException){

 if(_parent->DBServerExists()){
        Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(unsigned int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getNHS(cid,acs);
       }
       catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
        _parent->EMessage("dbserver corba error during getnhs" );   
       }
     }
 }



DPS::Client::NHS_var acv= new DPS::Client::NHS();
unsigned int length=0;
for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
if(pser->getType()==cid.Type){
length=pser->getnhl().size();
if(length==0){
//acv->length(1);
}
else{
acv->length(length);
length=0;
for(NHLI li=pser->getnhl().begin();li!=pser->getnhl().end();++li){
 acv[length++]=*li;
}
}
acs=acv._retn();
return length;
}
}


}

int Server_impl::getAHS(const DPS::Client::CID &cid, DPS::Client::AHS_out acs)throw (CORBA::SystemException){



 if(_parent->DBServerExists()){
        Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(unsigned int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getAHS(cid,acs);
       }
       catch(DPS::DBProblem &dbl){
        _parent->EMessage((const char*)dbl.message); 
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
        _parent->EMessage("dbserver corba error during getahs" );   
       }

     }
 }


DPS::Client::AHS_var acv= new DPS::Client::AHS();
unsigned int length=0;

for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
if(pser->getType()==cid.Type){
length=pser->getahl().size();
if(length==0){
//acv->length(1);
}
else{
acv->length(length);
length=0;
for(AHLI li=pser->getahl().begin();li!=pser->getahl().end();++li){
 acv[length++]=*li;
}
}
acs=acv._retn();
return length;
}
}


}


  CORBA::Boolean Server_impl::AdvancedPing()throw (CORBA::SystemException){
    time_t tt;
    time(&tt);
    if(  tt-_parent->LastServiceTime()>_KillTimeOut+2 && Master(false)){
     _parent->EMessage("Server_impl::AdvancedPing-MasterProblems-StickInListening");
    _parent->GlobalError()=true;
    return false;
    }
    else {
    _parent->GlobalError()=false;
     return true;
    }
  }

  void Server_impl::ping()throw (CORBA::SystemException){
  }
  void Producer_impl::pingp()throw (CORBA::SystemException){
  }

  void Server_impl::SystemCheck()throw (CORBA::SystemException){
   _parent->GlobalError()=false;
   time_t tt;
   time(&tt);
   if(  tt-_parent->LastServiceTime()>_KillTimeOut+3 && _parent->MT()){
     _parent->EMessage("Server_impl::SystemCheck-MainThreadNotRespondingWillBeSimulatedBySecondaryOne");
    for(;;){
     ((AMSServer*)_parent)->SystemCheck();
     sleep(1);
    }
   }
   else ((AMSServer*)_parent)->SystemCheck(true);
  }


   void Server_impl::sendCriticalOps(const DPS::Client::CID & cid, const DPS::Server::CriticalOps & op)throw (CORBA::SystemException){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==op.Type){
//      case (DPS::Server::StartClient): (DPS::Server::KillClient): (DPS::Server::CheckClient):
     if(op.Action == DPS::Server::StartClient || op.Action == DPS::Server::KillClient || op.Action == DPS::Server::CheckClient){
          DPS::Server::CriticalOps op1;
        op1.TimeOut=op.TimeOut;
        op1.Action=op.Action;
        op1.TimeStamp=op.TimeStamp;
        op1.Type=op.Type;
        op1.id=op.id;
        if(op.Action==DPS::Server::StartClient)op1.Action=DPS::Server::ClearStartClient;        
        if(op.Action==DPS::Server::KillClient)op1.Action=DPS::Server::ClearKillClient;        
        if(op.Action==DPS::Server::CheckClient)op1.Action=DPS::Server::ClearCheckClient;        
        pcur->getcol().push_back(op1);
     }
     else{
//      case DPS::Server::ClearStartClient: DPS::Server::ClearKillClient: DPS::Server::ClearCheckClient:
        for (COLI li=pcur->getcol().begin(); li!=pcur->getcol().end();++li){
         if(op.Action == (*li).Action){
          pcur->getcol().erase(li);
//         cout << "remove lock "<<cid.uid<<" "<<op.Type<<endl;
          break;
         }
        }
        break;
     }     
     break;
    }
  }
}

  bool Server_impl::pingHost(const char * host){
   char tmpbuf[1024];
   strcpy(tmpbuf,"ping -c 1 -w 9 ");
   strcat(tmpbuf,host);
   return systemC(tmpbuf)==0;


}



CORBA::Boolean Server_impl::TypeExists(DPS::Client::ClientType type)throw (CORBA::SystemException){
for(AMSServerI * pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
 if(pcur->getType()==type)return true;
}
return false;
}



void Server_impl::StartSelf(const DPS::Client::CID & cid, DPS::Client::RecordChange rc){

     time_t tt;
     time(&tt);

     _SubmitTime=tt;


 // Registered itself in _acl
     DPS::Client::ActiveClient as;
     as.id= cid;
     as.Status=DPS::Client::Active;
     as.id.StatusType=DPS::Client::Permanent;
     as.StatusType=DPS::Client::Permanent;
     as.LastUpdate=tt;     
     as.TimeOut=_KillTimeOut;
     as.Start=tt;
    int length=0;
    for (AMSServerI * pser=this;pser; pser=pser->down()?pser->down():pser->next())length+=pser->getrefmap().size();
//       cout <<"  length "<<length <<endl;
    if(length){
     (as.ars).length(length);
    length=0;
     for (AMSServerI * pser=this;pser; pser=pser->down()?pser->down():pser->next()){
      for(  map<AString,CORBA::String_var>::iterator mi=pser->getrefmap().begin();mi!=pser->getrefmap().end();++mi){
        ((as.ars)[length]).Interface=(const char *)(mi->first);
        ((as.ars)[length]).IOR=(const char *)(mi->second);
        ((as.ars)[length]).Type=pser->getType();
        ((as.ars)[length]).uid=as.id.uid;
        length++;
       }
     }
    }
     else {
      (as.ars).length(1);
      ((as.ars)[0]).Interface=(const char *)("Dummy");
      ((as.ars)[0]).IOR=(const char *)(" ");
      ((as.ars)[0]).Type=DPS::Client::Generic;
      ((as.ars)[0]).uid=0;
     }
   DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(as);
//   cout << "  pac 0  "<<endl;
   PropagateAC(acv,rc);
//   cout << "  pac 1  "<<endl;
   DPS::Client::CID asid=as.id;
   sendAC(asid,as,rc);
//   cout << "  pac 2  "<<endl;

   if(rc ==DPS::Client::Create){
         for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
            if(!strcmp((const char *)(*i)->HostName, (const char *)(as.id).HostName)){
            as.id.Mips=(*i)->Clock;
            cout << " host found for creating "<<endl;
            PropagateAH(asid,*i,DPS::Client::Update);
            break;
        }
       }
}
}


//------------------------------Producer_impl--------------------------------


Producer_impl::Producer_impl(const map<AString, AMSServer::OrbitVars> & mo, const DPS::Client::CID & cid,AMSClient* parent,char * NC, char *RF, char *NS, char *TS): POA_DPS::Producer(),AMSServerI(AMSID("Producer",0),parent,DPS::Client::Producer),_RunID(0){
typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;
Producer_impl * pcur =0;
for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new Producer_impl());
 PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  static DPS::Producer_ptr _ref = reinterpret_cast<DPS::Producer_ptr>((i->second)._poa->id_to_reference(oid));
     
   CORBA::Object *ppp=(void*)_ref;
   _refmap[i->first]=((i->second)._orb)->object_to_string(ppp);
   if(!strcmp((const char *)(i->first),(const char*)cid.Interface)){
    _defaultorb=(i->second)._orb;
   }
}
// Here read nominalclients
   
if(NC){
 ifstream fbin;
 fbin.open(NC);
 unsigned int uid=0;
 if(fbin){
   DPS::Client::NominalClient_var ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>ncl->MaxClients>>ncl->CPUNeeded>>ncl->MemoryNeeded;
    char tmpbuf[1024];
    fbin>>tmpbuf;
    ncl->WholeScriptPath=(const char*)tmpbuf;
    ncl->Type=DPS::Client::Producer;
   fbin.ignore(1024,'\n');
  while(!fbin.eof() && fbin.good()){ 
   fbin>>tmpbuf;
   ncl->HostName=(const char*)tmpbuf;
   fbin>>ncl->LogInTheEnd;
   fbin.ignore(1024,'\n');
   fbin.getline(tmpbuf,1024);
   ncl->LogPath= (const char*)tmpbuf;
   fbin.getline(tmpbuf,1024);
    ncl->SubmitCommand=(const char*)tmpbuf;
    ncl->uid=++uid;
    if(fbin.good())_ncl.push_back(ncl);
  }
 }
 else{
 _parent->FMessage("Producer_impl::Producer_impl-F-UnableToOpenNCFile ",DPS::Client::CInAbort);
}
}
else{
 _parent->FMessage("Producer_impl::Producer_impl-F-UnableToFindNCFile ",DPS::Client::CInAbort);
}

if(NS){
 ifstream fbin;
 fbin.open(NS);
 if(fbin){
   unsigned int uid=0;
   DPS::Producer::DSTInfo_var ncl= new DPS::Producer::DSTInfo();
    char tmpbuf[1024];
  while(!fbin.eof() && fbin.good()){ 
   if(fbin.get()=='#'){
      fbin.ignore(1024,'\n');
      continue;
   }
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>tmpbuf;
   ncl->HostName=(const char*)tmpbuf;
   ncl->uid=++uid;
   ncl->FreeSpace=-1;
   ncl->TotalSpace=-1;
   int imode;
   fbin>>imode;
   switch (imode){
     case 0:
     ncl->Mode=DPS::Producer::RILO;
     break;
     case 1:
     ncl->Mode=DPS::Producer::LILO;
     break;
     case 2:
     ncl->Mode=DPS::Producer::RIRO;
     break;
     case 3:
     ncl->Mode=DPS::Producer::LIRO;
     break;
     default:
     ncl->Mode=DPS::Producer::RILO;
     break;
   }
   fbin>>ncl->UpdateFreq;
   fbin>>imode;
   switch (imode){
     case 0:
     ncl->type=DPS::Producer::Ntuple;
     break;
     case 1:
     ncl->type=DPS::Producer::RootFile;
     break;
     default:
     ncl->type=DPS::Producer::Ntuple;
     break;
   }
   fbin>>tmpbuf;
    ncl->OutputDirPath=(const char*)tmpbuf;
    ncl->DieHard=0;
    
    if(fbin.good())_dstinfo.push_back(ncl);
  }
 }
 else{
 _parent->FMessage("Producer_impl::Producer_impl-F-UnableToOpenNtupeFile ",DPS::Client::CInAbort);
}
}
else{
 _parent->FMessage("Producer_impl::Producer_impl-F-UnableToFindNtupeFile ",DPS::Client::CInAbort);
}

//Here read runfiletable
if(RF){
 ifstream fbin;
 fbin.open(RF);
 if(fbin){
   int cur=0;
  while(!fbin.eof() && fbin.good()){ 
   if(fbin.get() =='#'){
      fbin.ignore(1024,'\n');
      continue;
   }
   else fbin.seekg(fbin.tellg()-sizeof(char));
   DPS::Producer::RunEvInfo  re;
   re.uid=_RunID++;
    char tmpbuf[1024];
   fbin>>re.Run>>re.FirstEvent>>re.LastEvent>>re.TFEvent>>re.TLEvent>>re.Priority>>tmpbuf;
    re.TLEvent+=60;
   if( fbin.eof() || !fbin.good())break;
      fbin.ignore(1024,'\n');
   re.FilePath=(const char*)tmpbuf;
   if(re.LastEvent<re.FirstEvent)re.LastEvent=2000000000;
   if(!_parent->IsMC() || strstr((const char*)tmpbuf,"cern")){
      re.Status=DPS::Producer::ToBeRerun;
      re.History=DPS::Producer::ToBeRerun;
      re.cuid=0;
      //re.pid=0;
      re.CounterFail=0;
   }
   else{
     re.Status=DPS::Producer::Foreign;
     re.History=DPS::Producer::Foreign;
     re.cuid=re.Run;
     //re.pid=0;
     re.CounterFail=0;
   }
   time_t tt;
   time(&tt);
   re.SubmitTime=tt; 
   re.cinfo.Run=re.Run;
   re.cinfo.EventsProcessed=0;
   re.cinfo.LastEventProcessed=0;
   re.cinfo.ErrorsFound=0;
   re.cinfo.CriticalErrorsFound=0;
   re.cinfo.CPUTimeSpent=0;
   re.cinfo.TimeSpent=0;
   re.cinfo.Status=re.Status;
   re.cinfo.HostName=" ";
   DPS::Producer::RunEvInfo_var vre= new DPS::Producer::RunEvInfo(re);
   _rl.push_back(vre); 
    cout <<++cur<<" "<<re.Run<<endl;
  }
 }
else{
  cerr<<"Producer_impl::Producer_impl-F-UnableToOpenRunFile "<<RF<<endl;
  _parent->FMessage("Producer_impl::Producer_impl-F-UnableToOpenRunFile ",DPS::Client::CInAbort);
}
 cout <<"RQsize "<<_rl.size()<<endl;
 _rl.sort(Less());
}
}



Producer_impl::Producer_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _svar, DPS::Client::CID  cid,AMSClient* parent): POA_DPS::Producer(),AMSServerI(AMSID("Producer",0),parent,DPS::Client::Producer),_RunID(0){

typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;
Producer_impl * pcur =0;
for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new Producer_impl());
 PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  DPS::Producer_ptr _ref = reinterpret_cast<DPS::Producer_ptr>((i->second)._poa->id_to_reference(oid));
   _refmap[i->first]=((i->second)._orb)->object_to_string(_ref);
   if(!strcmp((const char *)(i->first),(const char*)cid.Interface)){
    _defaultorb=(i->second)._orb;
   }
}


   AMSServerI::ReReadTables(_svar);

}




void Producer_impl::_init(){
 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
 if(!_pser){
  _parent->FMessage("Producer_impl::_init-F-UnableToConnectToServer",DPS::Client::CInAbort);
 }
if(_ahl.size()){
//Get producer_var and read producer specifics
    DPS::Client::CID cid=_parent->getcid();
    cid.Type=getType();
    cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars);
    DPS::Client::ARS_var arf=pars;
    DPS::Producer_var _pvar=DPS::Producer::_nil();
  for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    _pvar=DPS::Producer::_narrow(obj);
     if(!CORBA::is_nil(_pvar))break;
   }
   catch (CORBA::SystemException &ex){
   }
  }
   if(CORBA::is_nil(_pvar))_parent->FMessage("Producer_impl::ctor-UnableToGetpvar",DPS::Client::CInAbort);  

//Read dstinfo

//Here read runfiletable
DPS::Producer::DSTIS * pdstis;
length=_pvar->getDSTInfoS(cid, pdstis);
DPS::Producer::DSTIS_var dstis=pdstis; 

for(unsigned int i=0;i<length;i++){
DPS::Producer::DSTInfo_var vre= new DPS::Producer::DSTInfo(dstis[i]);
 _dstinfo.push_back(vre);
}
 cout <<"DSTinfosize "<<_dstinfo.size()<<endl;



//Here read runfiletable
DPS::Producer::RES * pres;
length=_pvar->getRunEvInfoS(cid, pres,_RunID);
DPS::Producer::RES_var res=pres; 

for(unsigned int i=0;i<length;i++){
DPS::Producer::RunEvInfo_var vre= new DPS::Producer::RunEvInfo(res[i]);
 _rl.push_back(vre);
}
 cout <<"RQsize "<<_rl.size()<<endl;



//Now Read Ntuple

DPS::Producer::DSTS * pdsts;
length=_pvar->getDSTS(cid,pdsts);
DPS::Producer::DSTS_var dsts=pdsts;
for(unsigned int i=0;i<length;i++){
DPS::Producer::DST_var vdst= new DPS::Producer::DST(dsts[i]);
 _dst.insert(make_pair(vdst->Type,vdst));
}

}

else{
// here init active host list
  for(NHLI i=(_pser->getnhl()).begin();i!=(_pser->getnhl()).end();++i){
   DPS::Client::ActiveHost ah;
   ah.HostName=CORBA::string_dup((*i)->HostName);
   ah.Interface=CORBA::string_dup((*i)->Interface);
   if(_pser->pingHost((const char*)(ah.HostName)))ah.Status=DPS::Client::OK;
   else ah.Status=DPS::Client::NoResponse;
   ah.ClientsProcessed=0;
   ah.ClientsFailed=0;
   ah.LastFailed=0;
   ah.ClientsKilled=0;
   ah.ClientsRunning=0;
   ah.Clock=(*i)->Clock;  
   
   ah.ClientsAllowed=min((*i)->CPUNumber/(*_ncl.begin())->CPUNeeded+0.5f,(*i)->Memory/float((*_ncl.begin())->MemoryNeeded));

   time_t tt;
   time(&tt);
   ah.LastUpdate=tt;
//Check Interface
   if(_refmap.find((const char *)(ah.Interface))==_refmap.end()){
   ah.Interface=(const char *)((_refmap.begin())->first);
#ifdef __AMSDEBUG__
       if((_refmap.begin())->first != "default"){
        cerr<<"Producer_impl::_init-S-FirstRefmapEntry!=default "<<(_refmap.begin())->first<<endl;
   }
#endif
   }
   DPS::Client::ActiveHost_var vah= new DPS::Client::ActiveHost(ah);
   _ahl.push_back(vah);
  }

}
}





void Producer_impl::StartClients(const DPS::Client::CID & pid){
if(!Master())return;
RegisteredClientExists();
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
if(pcur->InactiveClientExists(getType()))return;
 
}

 _UpdateHosts();
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
 // Check if there are some hosts to run on
 DPS::Client::ActiveHost_var ahlv=new DPS::Client::ActiveHost();
 bool suc=false;
 if(_parent->DBServerExists() && getHostInfoSDB(_parent->getcid(),ahlv)){
  suc=true;
 }
 else{
 _ahl.sort(Less());
  if(_acl.size()<(*_ncl.begin())->MaxClients && _acl.size()<count_if(_rl.begin(),_rl.end(),REInfo_Count())){
 for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
  if((*i)->Status!=DPS::Server::NoResponse && (*i)->Status!=DPS::Server::InActive){
    if((*i)->ClientsRunning>=(*i)->ClientsAllowed)continue;
     ahlv=*i;
     suc=true;
     break;
    }
   }
  }
 }
  if(suc){ 
  if(!_pser->Lock(pid,DPS::Server::StartClient,getType(),_StartTimeOut))return;
   // HereStartClient
  CORBA::String_var _refstring;
   _refstring=_refmap.find((const char *)((ahlv)->Interface))->second;
   if(_refmap.find((const char *)((ahlv)->Interface))->second){
    _refstring=_refmap.find((const char *)((ahlv)->Interface))->second;
   }
   else{
       _parent->EMessage(AMSClient::print(ahlv, " Could not find refstring for the host "));
     _refstring=_refmap.find((const char *)("default"))->second;
    
  }
#ifdef __AMSDEBUG__
     cout <<((ahlv)->Interface)<<" "<<_refstring<<endl;
#endif
    NCLI cli=find_if(_ncl.begin(),_ncl.end(),NCL_find((const char *)(ahlv)->HostName)); 
    if(cli==_ncl.end())cli=_ncl.begin();


     
     // Add New Active Client
     DPS::Client::ActiveClient ac;
     (ac.id).HostName=CORBA::string_dup((ahlv)->HostName);
     ac.id.Interface=(ahlv)->Interface;
     (ac.id).uid=0;
     ac.id.pid=0;
     ac.id.ppid=0;
     ac.id.Type=getType();
     ac.id.Status=DPS::Client::NOP;
     ac.Status=DPS::Client::Submitted;
     ac.id.StatusType=DPS::Client::Permanent;
     ac.StatusType=DPS::Client::Permanent;
     ac.TimeOut=_KillTimeOut;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
     ((ac.ars)[0]).Interface=(const char *)("Dummy");
     ((ac.ars)[0]).IOR=(const char *)(" ");
     ((ac.ars)[0]).Type=DPS::Client::Generic;
     ((ac.ars)[0]).uid=0;
    if(_parent->IsMC()){
      // find run from very beginning, as whole script path depend on it
        DPS::Producer::RunEvInfo_var   reinfo;
        DPS::Producer::DSTInfo_var   dstinfo;
        getRunEvInfo(ac.id,reinfo,dstinfo);     
        ac.id.StatusType=DPS::Client::OneRunOnly;  
        if(dstinfo->DieHard){
           cout << " failed "<<dstinfo->DieHard<<endl;
 
              //  run not found, aborting client
         HostClientFailed(ahlv); 
         _parent->EMessage(AMSClient::print(ac,"Server_impl::RunNotfounfFor "));
          _pser->Lock(pid,DPS::Server::ClearStartClient,getType(),_StartTimeOut);  
         return;
        }
        else{
          _Submit=reinfo->Run-1;
          ac.StatusType=DPS::Client::OneRunOnly;
          ac.id.StatusType=DPS::Client::OneRunOnly;
          (*cli)->WholeScriptPath=CORBA::string_dup(reinfo->FilePath);
        }
    }
       (ac.id).uid=_Submit+1;
    AString submit;
    char uid[80];
     sprintf(uid,"%d",_Submit+1);
    submit+=(const char *)(*cli)->SubmitCommand;  
    submit+=" ";
    submit+=(const char*)((ahlv)->HostName);
    submit+=" ";
    if(!(*cli)->LogInTheEnd){
     submit+=(const char*)((*cli)->LogPath);  
     if(_parent->IsMC())submit+="MC";
     submit+="Producer.";
     submit+=uid;
     submit+=".log ";
    }
     if(_parent->IsMC()){
       AString fnam;
       char * logdir=getenv("ProductionLogDir");
       if(!logdir){
         logdir=getenv("AMSDataDir");
         if(!logdir){
          AString a("AMSDataDirNotDefined ");
          _parent->EMessage((const char*) a);
          HostClientFailed(ahlv); 
          
          _pser->Lock(pid,DPS::Server::ClearStartClient,getType(),_StartTimeOut);  
          return;

         }
         fnam=logdir;
         fnam+="/prod.log";
       }
       else fnam=logdir;
        fnam+="/scripts/";
        submit+=(const char*)fnam;
        submit+=(const char*)((*cli)->WholeScriptPath);  
     }
     else{
      char *gtv=getenv((const char*)((*cli)->WholeScriptPath));
      if(!gtv){
         AString a("CouldNot getenv for ");
         a+=(const char*)((*cli)->WholeScriptPath);
         a+=" ContinueWithFingersCrossing";
        _parent->EMessage((const char *)a);
        submit+=(const char*)((*cli)->WholeScriptPath);  
      }
      else submit+=(const char*)gtv;
    }
    submit+=" -";
    submit+=(const char*) _refstring;
    submit+=" -U";
    submit+=uid;
    if(_parent->MT())submit+=" -M";
    if(_parent->Debug())submit+=" -D1";
    int corfac=70000/((ahlv)->Clock>0?(ahlv)->Clock:1000);
    char cuid[80];
    sprintf(cuid," -C%d",corfac);
    submit+=cuid;
    submit+=" -A";
     submit+=getenv("AMSDataDir");
    if((*cli)->LogInTheEnd){
     submit+=" ";
     submit+=(const char*)((*cli)->LogPath);  
     if(_parent->IsMC())submit+="MC";
     submit+="Producer.";
     submit+=uid;
     submit+=".log ";
    }
    if(_parent->Debug())_parent->IMessage(submit);
    submit+=" &";
    int out=systemC(submit);
     time(&tt);
     (ahlv)->LastUpdate=tt;     
    if(out==0){
     (ahlv)->Status=DPS::Client::OK; 
      DPS::Client::CID cid=_parent->getcid();      
      cid.Type=getType();
      cid.Interface= (const char *) " "; 
    PropagateAH(cid,(ahlv),DPS::Client::Update);
    DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(ac);
    PropagateAC(acv,DPS::Client::Create);
//     ((ahlv)->ClientsRunning)++;
    }
    else{
        HostClientFailed(ahlv); 
     
      DPS::Client::CID cid=_parent->getcid();      
      cid.Type=getType();
      cid.Interface= (const char *) " "; 
    PropagateAH(cid,(ahlv),DPS::Client::Update);
     AString a="StartClients-E-UnableToStartClient ";
     a+=submit;
     _parent->EMessage((const char *)a);
      Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
      _pser->MonInfo((const char *) a,DPS::Client::Error);
    }
    _pser->Lock(pid,DPS::Server::ClearStartClient,getType(),_StartTimeOut);  
   }

}



#include <signal.h>
void Producer_impl::KillClients(const DPS::Client::CID & pid){
if(!Master())return;


Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(pid,DPS::Server::KillClient,getType(),_KillTimeOut))return;


time_t tt;
time(&tt);


ACLI li=find_if(_acl.begin(),_acl.end(),find(DPS::Client::Killed));
 uinteger TimeCheckValue=0;
 if(li!=_acl.end())TimeCheckValue=(*li)->LastUpdate>getSubmitTime()?(*li)->LastUpdate+1.41*((*li)->TimeOut):getSubmitTime()+((*li)->TimeOut);
if(li!=_acl.end() && TimeCheckValue<tt){

 if(!_pser->pingHost((const char*)((*li)->id.HostName))){
    for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
      if(!strcmp((const char *)(*i)->HostName, (const char *)((*li)->id).HostName)){
       (*i)->Status=DPS::Server::NoResponse;

       DPS::Client::CID cid=_parent->getcid();      
       cid.Type=getType();
       cid.Interface= (const char *) " "; 
       PropagateAH(cid,*i,DPS::Client::Update,DPS::Client::AnyButSelf);


       _pser->MonInfo(AMSClient::print(*i,"No Response From: "),DPS::Client::Warning);
      _parent->EMessage(AMSClient::print(*i,"  No Response from "));
      break;
    }
} 
      (*li)->id.Status=DPS::Client::SInKill;
      (*li)->Status=DPS::Client::Killed;
      DPS::Client::ActiveClient_var acv=*li;
      PropagateAC(acv,DPS::Client::Delete);

 }
 else{
   if(_pser->MonDialog(AMSClient::print(*li,"Asking To Kill Client: "),DPS::Client::Error)){
 //kill by -9 here
 
   if(_parent->Debug())_parent->EMessage(AMSClient::print(*li, " KILL -9"));
    int iret=_pser->Kill((*li),SIGKILL,true);
    if(iret){
     _parent->EMessage(AMSClient::print(*li,"Producer::Unable To Kill Client"));
    }
    
    DPS::Client::ActiveClient_var acv=*li;
    PropagateAC(acv,DPS::Client::Delete);
  }
  else{
    _UpdateACT((*li)->id,DPS::Client::Active);
  }

}
}
 li=find_if(_acl.begin(),_acl.end(),find(DPS::Client::TimeOut));
if(li!=_acl.end()){
 
 if(!_pser->pingHost((const char*)((*li)->id.HostName))){
    for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
      if(!strcmp((const char *)(*i)->HostName, (const char *)((*li)->id).HostName)){
       (*i)->Status=DPS::Server::NoResponse;
       _pser->MonInfo(AMSClient::print(*i,"No Response From: "),DPS::Client::Warning);
      _parent->EMessage(AMSClient::print(*i,"  No Response from "));
      break;
    }
} 
      (*li)->id.Status=DPS::Client::SInKill;
      (*li)->Status=DPS::Client::Killed;
      DPS::Client::ActiveClient_var acv=*li;
      PropagateAC(acv,DPS::Client::Update);
//    PropagateAC(acv,DPS::Client::Delete);

 }
 else {
   if(1 || _pser->MonDialog(AMSClient::print(*li,"Asking To Kill Client: "),DPS::Client::Error)){
   (*li)->id.Status=DPS::Client::SInKill;
   (*li)->Status=DPS::Client::Killed;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Update);
   int iret=_pser->Kill((*li),SIGHUP,true);
    if(iret){
     _parent->EMessage(AMSClient::print(*li,"Producer::Unable To SigHup Client"));
         DPS::Client::ActiveClient_var acv=*li;
         PropagateAC(acv,DPS::Client::Delete);

    }

  }
  else{
    _UpdateACT((*li)->id,DPS::Client::Active);
  }
 }
}

_pser->Lock(pid,DPS::Server::ClearKillClient,getType(),_KillTimeOut);
}





void Producer_impl::CheckClients(const DPS::Client::CID & cid){
if(!Master())return;
static int iorder=0;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(cid,DPS::Server::CheckClient,getType(),_KillTimeOut))return;
for(AHLI li=_ahl.begin();li!=_ahl.end();++li){
 if((*li)->Status==DPS::Server::NoResponse){
 iorder=(iorder+1)%100;
    if(iorder==1 && _pser->pingHost((const char*)((*li)->HostName))){
        (*li)->Status=DPS::Client::OK;
     DPS::Client::CID cid=_parent->getcid();      
     cid.Type=getType();
     cid.Interface= (const char *) " "; 
         PropagateAH(cid,*li,DPS::Client::Update,DPS::Client::AnyButSelf);
    } 
}
}
time_t tt;
time(&tt);
for(ACLI li=_acl.begin();li!=_acl.end();++li){
 // find clients with timeout
 if((*li)->Status!=DPS::Client::Killed && ((*li)->LastUpdate+(*li)->TimeOut<tt && ((*li)->Status!=DPS::Client::Submitted || (*li)->LastUpdate+100<tt))){
   DPS::Client::ActiveClient_var acv=*li;
   if(acv->Status==DPS::Client::Submitted){
    acv->LastUpdate=(*li)->LastUpdate-2*((*li)->TimeOut);
   } 
   acv->Status=DPS::Client::TimeOut;
   if(_parent->Debug())_parent->EMessage(AMSClient::print(acv,"Client TIMEOUT"));
   PropagateAC(acv,DPS::Client::Update);
 }
}

_pser->Lock(cid,DPS::Server::ClearCheckClient,getType(),_KillTimeOut);
}









CORBA::Boolean Producer_impl::sendId(DPS::Client::CID & cid, float Mips, uinteger timeout) throw (CORBA::SystemException){
//cout <<" entering Producer_impl::sendId"<<endl;
     cid.Mips=Mips;
     for(ACLI j=_acl.begin();j!=_acl.end();++j){
      if(((*j)->id).uid==cid.uid && (*j)->Status ==DPS::Client::Submitted){
       ((*j)->id).pid=cid.pid;
       ((*j)->id).ppid=cid.ppid;
        ((*j)->id).Mips=Mips;
       cid.Interface=CORBA::string_dup(((*j)->id).Interface);
       cid.StatusType=((*j)->id).StatusType;
       cid.Type=((*j)->id).Type;
//          cout <<"  timeout was  "<<(*j)->TimeOut<<" now "<<timeout<<endl;
         if(timeout>(*j)->TimeOut)(*j)->TimeOut=timeout;
       (*j)->Status=DPS::Client::Registered;
       time_t tt;
       time(&tt);
       (*j)->LastUpdate=tt;
#ifdef __AMSDEBUG__
       _parent->IMessage(AMSClient::print(cid,"Producer_impl::sendId-I-RegClient "));
#endif 
       DPS::Client::ActiveClient_var acv=*j;
       PropagateAC(acv, DPS::Client::Update);
//       cout <<" exiting Producer_impl::sendId 1"<<endl;
       return true;
      }
     }
     if(_parent->IsMC() && cid.uid!=0){
      // Check if it is a foreign guy


     DPS::Client::ActiveClient ac;
     (ac.id).HostName=CORBA::string_dup(cid.HostName);
     cid.Interface=(const char*)"default";
     ac.id.Interface=CORBA::string_dup(cid.Interface);
     (ac.id).uid=cid.uid;
     ac.id.Mips=Mips;
     ac.id.pid=cid.pid;
     ac.id.ppid=cid.ppid;
     cid.Type=DPS::Client::Producer;
     ac.id.Type=cid.Type;
     ac.id.Status=cid.Status;
     cid.StatusType=DPS::Client::OneRunOnly;
     ac.id.StatusType=cid.StatusType;
     ac.Status=DPS::Client::Registered;
     ac.StatusType=cid.StatusType;
     ac.TimeOut=timeout;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
     ((ac.ars)[0]).Interface=(const char *)("Dummy");
     ((ac.ars)[0]).IOR=(const char *)(" ");
     ((ac.ars)[0]).Type=DPS::Client::Generic;
     ((ac.ars)[0]).uid=0;

         for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
            if(!strcmp((const char *)(*i)->HostName, (const char *)(ac.id).HostName)){
            (*i)->Clock=ac.id.Mips;
             _parent->IMessage(AMSClient::print (*i,"Setting Clock to "));  
             _parent->IMessage(AMSClient::print (ac,"Setting Clock to "));  
             PropagateAH(cid,(*i),DPS::Client::Update);
            break;
     }
     }

        DPS::Producer::RunEvInfo_var   reinfo;
        DPS::Producer::DSTInfo_var   dstinfo;
        getRunEvInfo(ac.id,reinfo,dstinfo);      
        if(dstinfo->DieHard){
         //  run not found, aborting client
        _parent->EMessage(AMSClient::print(cid,"Producer_impl::sendId-E-RegClientNotFoundAndForeignClientTestFailed "));
         cid.uid=0;
         if(dstinfo->DieHard==1)cid.Interface=(const char*)"RunNotFoundOrAnotherInstanceOfMCProducerActiveWaitForTimeOutIfCrashed";
         else cid.Interface=(const char*)"DBProblemFound";
         return false;
        }
        else {
          DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(ac);
          PropagateAC(acv, DPS::Client::Create);
          return true;
       } 
       }     
       else _parent->EMessage(AMSClient::print(cid,"Producer_impl::sendId-E-RegClientNotFound "));
//         cid.uid=0;
//         cout <<" exiting Producer_impl::sendId 3"<<endl;
           cid.Interface=(const char*)" RegisteredClientNotFound";
     return false;


}


int Producer_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out arf, DPS::Client::AccessType type, uinteger id, int selffirst)throw (CORBA::SystemException){

         //cout <<" entering Producer_impl::getARS"<<endl;
_UpdateACT(cid,DPS::Client::Active);

 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
 int iret=_pser->getARS(cid, arf,type,id,selffirst);
         //cout <<" exiting Producer_impl::getARS"<<endl;
 return iret; 

}





void Producer_impl::Exiting(const DPS::Client::CID & cid, const char * message, DPS::Client::ClientExiting status)throw (CORBA::SystemException){
_parent->IMessage(AMSClient::print(cid,message?message:" Producer Exiting"));
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
_pser->MonInfo(AMSClient::print(cid,message?message:" Producer Exiting "),DPS::Client::Info);
// find and remove client
for( ACLI li=_acl.begin();li!=_acl.end();++li){
 if (cid.uid==((*li)->id).uid){
   (*li)->id.Status=status;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete);
   RLI li=find_if(_rl.begin(),_rl.end(),REInfo_EqsClient(cid));
    if(li!=_rl.end()){
     if((*li)->Status == DPS::Producer::Processing){
       DPS::Producer::RunEvInfo_var rv=*li;
       rv->cinfo.HostName=cid.HostName;
      if(rv->History !=DPS::Producer::Failed){
        rv->Status=rv->History;
        rv->CounterFail++;
        if(LastTry(rv)){
         rv->History=DPS::Producer::Failed;
        }
      }
      else      rv->Status=DPS::Producer::Failed;
       rv->cuid=0;
       //rv->pid=0;
       _parent->EMessage(AMSClient::print(rv, " run Failed "));
       Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
       _pser->MonInfo(AMSClient::print(rv, " Run Failed: "),DPS::Client::Error);
       PropagateRun(rv,DPS::Client::Update);
     }
    }
  
   return;
 }

}
 _parent->EMessage(AMSClient::print(cid,"Producer_impl::Exiting::No Such Client"));
}





int Producer_impl::getTDV(const DPS::Client::CID & cid,  DPS::Producer::TDVName & tdvname, DPS::Producer::TDVbody_out body)throw (CORBA::SystemException){
//         cout <<" entering Producer_impl::getTDV"<<endl;
_UpdateACT(cid,DPS::Client::Active);

 if( _parent->IsOracle()){
     Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     for (AMSServerI* pcur=_pser;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
     if(DBServer_impl* pdb=dynamic_cast<DBServer_impl*>(pcur)){
          return pdb->getTDV(cid,tdvname,body);
       }
    }

 }
 else{
 int length=0;
 TIDI li=_findTDV(tdvname);
 tdvname.Success=false;
 if(li!=_tid.end()){
  time_t b=tdvname.Entry.Begin;
  tdvname.Success=li->second->read((const char*)AMSDBc::amsdatabase,tdvname.Entry.id,b);
  time_t i,e;
 li->second->gettime(i,b,e);
  tdvname.Entry.Insert=i;
  tdvname.Entry.Begin=b;
  tdvname.Entry.End=e;
 }
  DPS::Producer::TDVbody_var vbody=new DPS::Producer::TDVbody();
  if(tdvname.Success){
   length=li->second->GetNbytes()/sizeof(uinteger);
   vbody->length(length);
   li->second->CopyOut(vbody->get_buffer());
  }
  else{
   vbody->length(0);
  }
  body=vbody._retn();
  return length;
 }
}

int Producer_impl::getSplitTDV(const DPS::Client::CID & cid,  unsigned int & pos,DPS::Producer::TDVName & tdvname, DPS::Producer::TDVbody_out body, DPS::Producer::TransferStatus & st)throw (CORBA::SystemException){
//         cout <<" entering Producer_impl::getSplitTDV"<<endl;
_UpdateACT(cid,DPS::Client::Active);


 if( _parent->IsOracle()){
     Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     for (AMSServerI* pcur=_pser;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
     if(DBServer_impl* pdb=dynamic_cast<DBServer_impl*>(pcur)){
          return pdb->getSplitTDV(cid,pos,tdvname,body,st);
       }
    }

 }
 else{




st=DPS::Producer::Continue;
int length=0;
TIDI li=_findTDV(tdvname);
tdvname.Success=false;
if(li!=_tid.end()){
 time_t b=tdvname.Entry.Begin;
 tdvname.Success=li->second->read((const char*)AMSDBc::amsdatabase,tdvname.Entry.id,b);
time_t i,e;
li->second->gettime(i,b,e);
 tdvname.Entry.Insert=i;
 tdvname.Entry.Begin=b;
 tdvname.Entry.End=e;
}
 DPS::Producer::TDVbody_var vbody=new DPS::Producer::TDVbody();
 if(tdvname.Success){
  length=li->second->GetNbytes()/sizeof(uinteger);
  const int maxs=2000000;
  vbody->length(length);
  li->second->CopyOut(vbody->get_buffer());
  length-=pos;
  if(length>maxs)length=maxs;
  else st=DPS::Producer::End;
   if(pos){
    for (uinteger i=0;i<length;i++){
      vbody[i]=vbody[i+pos];
    }        
   }
   vbody->length(length);
   pos+=length;
 }
 else{
  st=DPS::Producer::End;
  vbody->length(0);
 }
 body=vbody._retn();
//         cout <<" exiting Producer_impl::getSplitTDV"<<endl;
 return length;
}
}


void Producer_impl::sendTDV(const DPS::Client::CID & cid, const DPS::Producer::TDVbody & tdv, DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException){
_UpdateACT(cid,DPS::Client::Active);



 if( _parent->IsOracle()){
     Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     for (AMSServerI* pcur=_pser;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
     if(DBServer_impl* pdb=dynamic_cast<DBServer_impl*>(pcur)){
          return pdb->sendTDV(cid,tdv,tdvname);
       }
    }

 }
 else{



TIDI li=_findTDV(tdvname);
tdvname.Success=false;
if(li!=_tid.end()){
time_t i,b,e;
i=tdvname.Entry.Insert;
b=tdvname.Entry.Begin;
e=tdvname.Entry.End;
li->second->SetTime(i,b,e);
li->second->CopyIn(tdv.get_buffer()); 
 tdvname.Success=li->second->write((const char*)AMSDBc::amsdatabase,1);

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID pid;
  pid.Type=getType();
  pid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
    DPS::Client::ARS_var arf=pars;
  for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
    _pvar->sendTDVUpdate(cid,tdvname);
   }
   catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
   }
  }



}
}
}

void Producer_impl::sendTDVUpdate(const DPS::Client::CID & cid,  const DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException){
TIDI li=_findTDV(tdvname);
if(li!=_tid.end()){
time_t i,b,e;
i=tdvname.Entry.Insert;
b=tdvname.Entry.Begin;
e=tdvname.Entry.End;
li->second->SetTime(i,b,e);
try{
li->second->updatedb();
}
catch (bad_alloc aba){
 _parent->EMessage("sendTDVUpdate::NoMemoryAvailable");
}
}
}

  
int Producer_impl::getTDVTable(const DPS::Client::CID & cid, DPS::Producer::TDVName & tdvname, unsigned int id, DPS::Producer::TDVTable_out table)throw (CORBA::SystemException){
//         cout <<" entering Producer_impl::getTDVTable"<<endl;
_UpdateACT(cid,DPS::Client::Active);
 if( _parent->IsOracle()){
     Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     for (AMSServerI* pcur=_pser;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
     if(DBServer_impl* pdb=dynamic_cast<DBServer_impl*>(pcur)){
          return pdb->getTDVTable(cid,tdvname,id,table);
       }
    }

 }
 else{

TIDI li=_findTDV(tdvname);
tdvname.Success=false;
unsigned int length=0;
DPS::Producer::TDVTable_var vtable=new DPS::Producer::TDVTable();
if(li!=_tid.end()){
  RLI ri=find_if(_rl.begin(),_rl.end(),REInfo_Eqs(id));
  if(ri!=_rl.end()){
    li->second->rereaddb();
    tdvname.Success=true;
    AMSTimeID::IBE ibe=li->second->findsubtable((*ri)->TFEvent,(*ri)->TLEvent);
    vtable->length(ibe.size());
    for( AMSTimeID::IBEI ti=ibe.begin();ti!=ibe.end();++ti){
      vtable[length].id=ti->id;
      vtable[length].Insert=ti->insert;
      vtable[length].Begin=ti->begin;
      vtable[length].End=ti->end;
      length++; 
    }
  }
  else{
    AString a("getTDVTable-S-RunNotFound ");
    a+=(const char*)tdvname.Name;
    _parent->EMessage((const char*)a);
  }
}
else{
    char tmp[80];
    sprintf(tmp,"%d",id);
    AString a("getTDVTable-S-TDVNameNotFound ");
    a+=(const char*)tmp;
    _parent->EMessage((const char*)a);
}
if(length==0){
 vtable->length(0);
}
table= vtable._retn();
//         cout <<" exiting Producer_impl::getTDVTable"<<endl;
return length;
}
}
#include <new.h>
Producer_impl::TIDI & Producer_impl::_findTDV(const DPS::Producer::TDVName & tdv){
AMSID id((const char*)tdv.Name,tdv.DataMC);
TIDI li=_tid.find(id);
if(li==_tid.end()){
try{
 uinteger *pdata = new uinteger[tdv.Size/sizeof(uinteger)-1];
 if(pdata){
  time_t b=tdv.Entry.Begin;
  time_t e=tdv.Entry.End;
  _tid[id]=new AMSTimeID(id,(*(localtime(&b))),(*(localtime(&e))),tdv.Size-sizeof(uinteger),pdata,AMSTimeID::Server);
 li=_tid.find(id);
 }
}
catch (bad_alloc aba){
 _parent->EMessage("_findTDV::NoMemoryAvailable");
}

}
return li;
}



void Producer_impl::getId(DPS::Client::CID_out  cid)throw (CORBA::SystemException){

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
   _pser->getId(cid);
}
void Server_impl::getId(DPS::Client::CID_out  cid)throw (CORBA::SystemException){
DPS::Client::CID_var cvar=new DPS::Client::CID(_parent->getcid());
cid=cvar._retn();
}

 int Producer_impl::getRunEvInfoS(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int & maxrun)throw (CORBA::SystemException){
//         cout <<" entering Producer_impl::getRunEvInfoS"<<endl;
 
DPS::Producer::RES_var acv= new DPS::Producer::RES();
unsigned int length=0;
maxrun=_RunID;
length=_rl.size();
if(length==0){
//acv->length(1);
}
else{
acv->length(length);
length=0;
for(RLI li=_rl.begin();li!=_rl.end();++li){
 acv[length++]=*li;
}
}
res=acv._retn();
//         cout <<" exiting Producer_impl::getRunEvInfoS"<<endl;
return length;
}

 int Producer_impl::getDSTInfoS(const DPS::Client::CID &cid, DPS::Producer::DSTIS_out res)throw (CORBA::SystemException){
 
//         cout <<" entering Producer_impl::getDSTInfoS"<<endl;
DPS::Producer::DSTIS_var acv= new DPS::Producer::DSTIS();
unsigned int length=0;
length=_dstinfo.size();
if(length==0){
//acv->length(1);
}
else{
acv->length(length);
length=0;
for(DSTILI li=_dstinfo.begin();li!=_dstinfo.end();++li){
 acv[length++]=*li;
}
}
res=acv._retn();
//         cout <<" exiting Producer_impl::getDSTInfoS"<<endl;
return length;
}




void Producer_impl::getRunEvInfo(const DPS::Client::CID &cid, DPS::Producer::RunEvInfo_out ro,DPS::Producer::DSTInfo_out dso)throw (CORBA::SystemException){



 DPS::Client::CID pid;
 pid.Interface=(const char *)" ";
 pid.Type=getType();
 DPS::Client::ARS * pars;      
 int length=getARS(pid,pars,DPS::Client::LessThan,_parent->getcid().uid);
 DPS::Client::ARS_var arf=pars; 
 for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
     cout <<" transferred to master "<<_parent->getcid().uid<<endl;
    _pvar->getRunEvInfo(cid,ro,dso);
    return;
  }

   catch (CORBA::TRANSIENT &tr){
    _parent->EMessage(AMSClient::print(cid,"Transient Error Occurs"));
    sleep(1);    
    try{
     CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
     DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
     _pvar->getRunEvInfo(cid,ro,dso);
    }
    catch (CORBA::SystemException &ex){
     _parent->EMessage(AMSClient::print(cid,"Transient Error Persists"));
     // Have to Kill Servers Here
    }
   }

    catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
   }
 }

 cout <<" Master getrunevinfo "<<_parent->getcid().uid<<endl;


  bool permanent=true;
  if(cid.StatusType==DPS::Client::OneRunOnly)permanent=false;
  DPS::Producer::DSTInfo_var dv =new DPS::Producer::DSTInfo();
  dv->DieHard=0;
  DPS::Producer::RunEvInfo_var rv=new DPS::Producer::RunEvInfo(); 
     for(ACLI j=_acl.begin();j!=_acl.end();++j){
     if((*j)->id.uid==cid.uid){
       if((*j)->StatusType == DPS::Client::OneRunOnly){
          cout <<"  onerunonly client detected "<<endl;
          if(permanent==true)_parent->EMessage(AMSClient::print(cid,"getrunevinf-cid and ac disagrees"));
          permanent=false;
//        dv->DieHard=1;
       }
       break;  
     }
    }
  if(dv->DieHard==0){
//   cout << " dbserver ? " <<endl;
 if(_parent->DBServerExists()){
//   cout << " dbserver !!! "<<endl;
  DPS::Client::CID tcid(cid);
   if(!permanent)tcid.StatusType=DPS::Client::OneRunOnly;
  bool succ=getRunEvInfoSDB(tcid,rv,dv);
  if(! succ){
     _parent->EMessage(AMSClient::print(_parent->getcid(),"getRunEvInfoSDBFailed"));
   dv->DieHard=2;
   ro=rv._retn();
   dso=dv._retn();
   cout <<" die hard !!!!!1 "<<endl;
   return;
 }
 
}
else{

  cout << "  No DNb server !!!"<<endl;
{
 DSTILI li=find_if(_dstinfo.begin(),_dstinfo.end(),DSTInfo_find(cid));
 if(li==_dstinfo.end())li=_dstinfo.begin();
 dv= *li;
 }
 dv->DieHard=0;
_rl.sort(Less());
RLI li=find_if(_rl.begin(),_rl.end(),REInfo_find(cid,DPS::Producer::ToBeRerun,permanent));
if(li==_rl.end()){
 dv->DieHard=1;
 _parent->IMessage("NoRunsToBeReRunAnyMore");
 li=find_if(_rl.begin(),_rl.end(),REInfo_find(cid,DPS::Producer::Failed,permanent));
 if(li==_rl.end())dv->DieHard=2;
 else{
  dv->DieHard=0;
 if(_parent->Debug()){
   cout <<  "****FAILED RUN RERUN "<<(*li)->Run<<" was "<<(*li)->cinfo.HostName <<" by "<<cid.HostName<<endl;
 }
 }
}
if(li==_rl.end()){
 
}
else if( find_if(_rl.begin(),_rl.end(),REInfo_EqsClient2(cid))!=_rl.end()){
 dv->DieHard=2;
 if(_parent->Debug()){
  _parent->EMessage(AMSClient::print(cid,"Die HARD  !!!!!!!"));
  RLI rvi=find_if(_rl.begin(),_rl.end(),REInfo_EqsClient2(cid));
  _parent->EMessage(AMSClient::print(*li,"Run/Client Logic Problem: Found:"));
  _parent->EMessage(AMSClient::print(*rvi,"Run/Client Logic Problem: Was:"));
 }
}
else {
 rv=*li;
}

 if(dv->DieHard ==0){
  if((rv->Status==DPS::Producer::Allocated || cid.uid) && rv->Status!=DPS::Producer::Foreign) rv->Status=DPS::Producer::Processing;
  else rv->Status=DPS::Producer::Allocated;
  if(cid.uid)rv->cuid=cid.uid;
  else rv->cuid=rv->Run;
  //rv->pid=cid.pid;
 }
}

if(dv->DieHard ==0){
 time_t tt;
 time(&tt);
 rv->SubmitTime=tt; 
  if(_parent->Debug()){
   _parent->IMessage(AMSClient::print (cid,"New Run Asked by"));  
   _parent->IMessage(AMSClient::print(rv));
  }
  PropagateRun(rv, DPS::Client::Update);
  uinteger rndm[2]={0,0};
  uinteger smartfirst=getSmartFirst(rv->Run,rndm);
 if( smartfirst>rv->FirstEvent){
   rv->FirstEvent=smartfirst;
   if(rndm[0] && rndm[1]){
    rv->rndm1=rndm[0];
    rv->rndm2=rndm[1];
   }
   _parent->IMessage(AMSClient::print(rv,"Run First Event Modified"));
   if(smartfirst>rv->LastEvent){
   _parent->EMessage(AMSClient::print(rv,"***SMART PROBLEM***"));
   dv->DieHard=3;
   }
  }
 }
}
ro=rv._retn();
dso=dv._retn();

_UpdateACT(cid,DPS::Client::Active);

}



void Producer_impl::sendRunEvInfo(const  DPS::Producer::RunEvInfo & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException){

//         cout <<" entering Producer_impl::sendRunEvInfo"<<endl;
 RLI li=find_if(_rl.begin(),_rl.end(),REInfo_Eqs(ne));
 switch (rc){
 case DPS::Client::Update:
  if(li==_rl.end())_parent->EMessage(AMSClient::print(ne,"RunEv not found for editing"));
  else {
    DPS::Producer::RunEvInfo * pinfo=new DPS::Producer::RunEvInfo(ne);
    (pinfo->cinfo).CPUMipsTimeSpent=((*li)->cinfo).CPUMipsTimeSpent;
    (pinfo->cinfo).EventsProcessed=((*li)->cinfo).EventsProcessed;
(pinfo->cinfo).CriticalErrorsFound=((*li)->cinfo).CriticalErrorsFound;
(pinfo->cinfo).ErrorsFound=((*li)->cinfo).ErrorsFound;
   *li=pinfo;
   cout <<"  replaCING run with "<< (pinfo->cinfo).EventsProcessed<<endl;
  }
  break;
 case DPS::Client::Delete:
  if(li==_rl.end())_parent->EMessage(AMSClient::print(ne,"runEv not found for deleting"));
  else _rl.erase(li);
  break;
 case DPS::Client::Create:
  if(li != _rl.end()){
     _parent->EMessage(AMSClient::print(ne,"Run already exists"));
  }
  else {
   _RunID++;
   DPS::Producer::RunEvInfo_var rv=new DPS::Producer::RunEvInfo(ne);
    (rv->cinfo).CPUMipsTimeSpent=0;
(rv->cinfo).EventsProcessed=0;
(rv->cinfo).CriticalErrorsFound=0;
(rv->cinfo).ErrorsFound=0;

  _rl.push_back(rv); 
  }
  break;
}
//         cout <<" exiting Producer_impl::sendRunEvInfo"<<endl;
}

void Producer_impl::sendDSTInfo(const  DPS::Producer::DSTInfo & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException){

//         cout <<" entering Producer_impl::sendDSTInfo"<<endl;

 PropagateDSTInfoDB(ne,rc);
 DSTILI li=find_if(_dstinfo.begin(),_dstinfo.end(),DSTInfo_Eqs(ne));
 switch (rc){
 case DPS::Client::Update:
  if(li==_dstinfo.end())_parent->EMessage(AMSClient::print(ne,"DSTInfo not found for editing"));
  else {
        DPS::Producer::DSTInfo_var vac= new DPS::Producer::DSTInfo(ne);
        replace_if(li,_dstinfo.end(),DSTInfo_Eqs(ne),vac);
//        _parent->IMessage(AMSClient::print(*li," Updated DSTInfo " ));
  }
  break;
 case DPS::Client::Create:
       if(li!=_dstinfo.end())_parent->EMessage(AMSClient::print(ne,"DSTInfo already exists"));
       else{
        DPS::Producer::DSTInfo_var vac= new DPS::Producer::DSTInfo(ne);
         _dstinfo.push_back(vac);
       }
  break;
 case DPS::Client::Delete:
       if(li==_dstinfo.end())_parent->EMessage(AMSClient::print(ne,"DSTInfo not found for deleting"));
       else{
         _dstinfo.erase(li);
       }
  break;
 default:
        _parent->EMessage(AMSClient::print(ne,"sendDSTInfo-Unknown rc value"));

}
//         cout <<" exiting Producer_impl::sendDSTInfo"<<endl;
}


int Producer_impl::getDSTS(const DPS::Client::CID & ci, DPS::Producer::DSTS_out dsts)throw (CORBA::SystemException){

DPS::Producer::DSTS_var acv= new DPS::Producer::DSTS();
unsigned int length=0;
length=_dst.size();
if(length==0){
//acv->length(1);
}
else{
acv->length(length);
length=0;
for(DSTLI li=_dst.begin();li!=_dst.end();++li){
 acv[length++]=(*li).second;
}
}
dsts=acv._retn();
return length;
}


void Producer_impl::sendCurrentInfo(const DPS::Client::CID & cid, const  DPS::Producer::CurrentInfo &ci, int propagate)throw (CORBA::SystemException){

//         cout <<" entering Producer_impl::sendCurrentInfo"<<endl;

RLI li=find_if(_rl.begin(),_rl.end(),REInfo_EqsClient(cid));
if(li !=_rl.end()){
   DPS::Producer::RunEvInfo_var rv=*li; 
   rv->cinfo=ci;
   rv->Status=ci.Status;
   (*li)->cinfo=ci;
    if(ci.Status ==DPS::Producer::Finished || ci.Status==DPS::Producer::Failed){
//  lets check it is finished normally
     if(!_parent->IsMC() && rv->LastEvent && rv->LastEvent != ci.LastEventProcessed){
       ci.Status==DPS::Producer::Failed;
       _parent->EMessage(AMSClient::print(rv, " run Failed "));
     }
   if(ci.Status==DPS::Producer::Failed){
       _parent->EMessage(AMSClient::print(ci, " run Failed "));
   }

/*
      if(_parent->Debug()){
       _parent->IMessage(AMSClient::print(cid,"sendCurrentInfo from "));
       _parent->IMessage(AMSClient::print(ci));
      }
*/
      rv->cuid=0;
      //rv->pid=0;
      rv->cinfo.HostName=cid.HostName;
      if(rv->Status==DPS::Producer::Failed && rv->History !=DPS::Producer::Failed){
        rv->Status=DPS::Producer::ToBeRerun;
        rv->History=DPS::Producer::Failed;
      }
    }
   PropagateRun(rv,DPS::Client::Update);
   _UpdateACT(cid,DPS::Client::Active);
}
else{
 _parent->EMessage(AMSClient::print(cid,"sendCurrentInfo-EUnable to find Run Record for Client"));
 _parent->EMessage(AMSClient::print(ci));

// try to kill nasty client
//
     DPS::Client::ActiveClient_var vac=new DPS::Client::ActiveClient();
     vac->id=cid;
     Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     int iret=_pser->Kill(vac,SIGKILL,true);
    if(iret){
     _parent->EMessage(AMSClient::print(cid,"Producer::Unable To Kill Client"));
    }
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
    ACLI li=find_if(pcur->getacl().begin(),pcur->getacl().end(),Eqs(vac));
    if(li!=pcur->getacl().end()){
         DPS::Client::ActiveClient_var acv=*li;
         PropagateAC(acv,DPS::Client::Delete);
    }
     break;
    }
  }
}
//         cout <<" exiting Producer_impl::sendCurrentInfo"<<endl;
}





void Producer_impl::sendDSTEnd(const DPS::Client::CID & ci, const  DPS::Producer::DST & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
//         cout <<" exiting Producer_impl::sendDSTEnd"<<endl;
_UpdateACT(ci,DPS::Client::Active);
if(_parent->Debug() && ne.Status!=DPS::Producer::InProgress){
  _parent->IMessage(AMSClient::print(ci,"senddstinfo get from "));
  _parent->IMessage(AMSClient::print(ne));
}
 DPS::Producer::DST_var vne= new DPS::Producer::DST(ne);
 pair<DSTLI,DSTLI>b=_dst.equal_range(ne.Type);
 switch (rc){
 case DPS::Client::Create:
 for(DSTLI li=b.first;li!=b.second;++li){
  if(!strcmp((const char *)(li->second)->Name,(const char *)ne.Name)){
   _parent->EMessage(AMSClient::print(li->second,"Create:DST Already exists"));
     _dst.erase(li);
//   return;
  }
 }
 _dst.insert(make_pair(ne.Type,vne));
 _parent->EMessage(AMSClient::print(vne,"Creating DST"));
  if(ci.Type!=DPS::Client::Server)PropagateDST(ne,DPS::Client::Create,DPS::Client::AnyButSelf,_parent->getcid().uid);
//         cout <<" exiting Producer_impl::sendDSTEnd create"<<endl;
 break;
 case DPS::Client::Update:
 for(DSTLI li=b.first;li!=b.second;++li){
//  if(!strcmp((const char *)(li->second)->Name,(const char *)ne.Name)){
//
// Change cmp vy comp name and file name only
// 
//
  AString a=(const char*)ne.Name;
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
    AString b="";
    for(int k=0;k<bstart;k++)b+=a[k];
  if(strstr((const char *)(li->second)->Name,(const char*)a(bend)) && 
     strstr((const char *)(li->second)->Name,(const char*)b)){
     switch ((li->second)->Status){
    case DPS::Producer::InProgress:
     (li->second)=vne;
      if(ci.Type!=DPS::Client::Server)PropagateDST(ne,DPS::Client::Update,DPS::Client::AnyButSelf,_parent->getcid().uid);
//         cout <<" exiting Producer_impl::sendDSTEnd update"<<endl;
      return;
    default:
    _parent->EMessage(AMSClient::print(vne,"Update:DST Already Exists "));
     (li->second)=vne;
      if(ci.Type!=DPS::Client::Server)PropagateDST(ne,DPS::Client::Update,DPS::Client::AnyButSelf,_parent->getcid().uid);
    return;
   }
   break;
  }
 }
    _parent->EMessage(AMSClient::print(vne,"Update:DST Not Found "));
 _dst.insert(make_pair(ne.Type,vne));
  if(ci.Type!=DPS::Client::Server)PropagateDST(ne,DPS::Client::Create,DPS::Client::AnyButSelf,_parent->getcid().uid);
 break;
 case DPS::Client::Delete:
 for(DSTLI li=b.first;li!=b.second;++li){
  if(!strcmp((const char *)(li->second)->Name,(const char *)ne.Name)){
      if(ci.Type!=DPS::Client::Server)PropagateDST((*li).second,rc,DPS::Client::AnyButSelf,_parent->getcid().uid);
     _dst.erase(li);
//         cout <<" exiting Producer_impl::sendDSTEnd delete"<<endl;
     return;       
      
  }
 } 
    _parent->EMessage(AMSClient::print(vne,"Delete:DST Not Found "));
 
}
}



void Producer_impl::PropagateRun(const DPS::Producer::RunEvInfo & ri, DPS::Client::RecordChange rc, DPS::Client::AccessType type, uinteger uid){

//         cout <<" entering Producer_impl::propagaterun"<<endl;
  PropagateRunDB(ri,rc);

  if(_ActivateQueue)_runqueue.push_back(RA(ri,rc));
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 


  DPS::Client::CID cid;
  cid.Type=getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,type,uid);
    DPS::Client::ARS_var arf=pars;
  for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
    _pvar->sendRunEvInfo(ri,rc);
   }
   catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
   }
  } 

//         cout <<" exiting Producer_impl::propagaterun"<<endl;
}

void Producer_impl::PropagateDST(const DPS::Producer::DST & ri, DPS::Client::RecordChange rc, DPS::Client::AccessType type,uinteger uid){

//         cout <<" entering Producer_impl::propagatedst"<<endl;
  PropagateDSTDB(ri,rc);

  if(_ActivateQueue)_dstqueue.push_back(DSTA(ri,rc));
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,type,uid);
    DPS::Client::ARS_var arf=pars;
  for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
    _pvar->sendDSTEnd(_parent->getcid(),ri,rc);
   }
   catch (CORBA::SystemException &ex){
   }
  } 
//         cout <<" exiting Producer_impl::propagatedst"<<endl;

}


void Server_impl::UpdateDB(bool force){
}

void Producer_impl::UpdateDB(bool force){
//just to do something




static bool resultdone=false;
if(!resultdone){
 if(force || !count_if(_rl.begin(),_rl.end(),REInfo_process())){
  if(!force)resultdone=true;

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  float dbfree,dbtotal;
  DPS::Server::DB* db;
  _pser->getDBSpace(_parent->getcid(),"AMSDataDir","/DataBase/",db);
  DPS::Server::DB_var dbv=db;
  cout <<" DBSpace Free "<<dbv->dbfree <<" Mb out of Total "<<dbv->dbtotal<<endl;
{
  for(DSTILI li= _dstinfo.begin();li!=_dstinfo.end();++li){
    _parent->IMessage(AMSClient::print((*li),"DiskSpace Info"));
  }
}

  for(RLI li= _rl.begin();li!=_rl.end();++li){
    if((*li)->Status==DPS::Producer::Finished)_parent->IMessage(AMSClient::print((*li),"Finished :"));
  }
  for(RLI li= _rl.begin();li!=_rl.end();++li){
   if((*li)->Status==DPS::Producer::Failed)_parent->IMessage(AMSClient::print((*li),"Failed :"));
  }
  for(RLI li= _rl.begin();li!=_rl.end();++li){
   if((*li)->Status==DPS::Producer::Processing)_parent->IMessage(AMSClient::print((*li),"Processing :"));
  }
cout << "Host Status **********"<<endl;
for (AHLI li=_ahl.begin();li!=_ahl.end();++li){
_parent->IMessage(AMSClient::print((*li),"Host "));
}

cout <<"DST status ******" <<endl;
{
 DPS::Producer::DSTType ntuple=DPS::Producer::Ntuple;
 pair<DSTLI,DSTLI>b=_dst.equal_range(ntuple);

for (DSTLI li=b.first;li!=b.second;++li){
_parent->IMessage(AMSClient::print((li->second),"DST "));
}
}
{
DPS::Producer::DSTType ntuple=DPS::Producer::RootFile; 
pair<DSTLI,DSTLI>b=_dst.equal_range(ntuple);

for (DSTLI li=b.first;li!=b.second;++li){
_parent->IMessage(AMSClient::print((li->second),"DST "));
}
}
{
DPS::Producer::DSTType ntuple=DPS::Producer::EventTag;
 pair<DSTLI,DSTLI>b=_dst.equal_range(ntuple);

for (DSTLI li=b.first;li!=b.second;++li){
_parent->IMessage(AMSClient::print((li->second),"DST "));
}
}
 }
//
}


}

bool Server_impl::Master(bool advanced){

DPS::Client::CID cid;
cid.Interface=(const char *)" ";
cid.Type=getType();
DPS::Client::ARS * pars;
int length=getARS(cid,pars,DPS::Client::LessThan,_parent->getcid().uid);
DPS::Client::ARS_var arf=pars;
  time_t tt;
  time(&tt);
 for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    if(!CORBA::is_nil(_pvar)){
     if(advanced){
      if(_pvar->AdvancedPing()){
       return false;
      }
     }
     else{
       _pvar->ping();
       return false;
     }
   }
  }
    catch (CORBA::SystemException &ex){
     cerr<<" Master oops corba exc for "<<i<<" "<<length<<" "<<ctime(&tt)<<endl;
   }
 }
return true;
}


bool Producer_impl::Master(bool advanced){
 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

return _pser->Master(advanced);
}


integer Server_impl::Kill(const DPS::Client::ActiveClient & ac, int signal, bool self){
  //start  killer here
   if(ac.id.pid==0)return 0;
   AString submit;
    NCLI cli=find_if(_nki.begin(),_nki.end(),NCL_find((const char *)(ac.id.HostName))); 
    if(cli==_nki.end())cli=_nki.begin();
    char tmp[80];
     sprintf(tmp,"%d",ac.id.uid);
    submit+=(const char*)((*cli)->SubmitCommand);
    submit+=" ";
    submit+=(const char*)(ac.id.HostName);
    submit+=" ";
    if(!(*cli)->LogInTheEnd){
     submit+=(const char*)((*cli)->LogPath);
     submit+="Killer.";
     submit+=tmp;
     submit+=".log ";
    }
    submit+=(const char*)((*cli)->WholeScriptPath);
    submit+=" -";
    sprintf(tmp,"%d",signal);
    submit+=tmp;
    submit+=" ";
    sprintf(tmp,"%d",(self?ac.id.pid:ac.id.ppid));
    submit+=tmp;
    if((*cli)->LogInTheEnd){
     submit+=" ";
     submit+=(const char*)((*cli)->LogPath);
     submit+="Killer.";
     sprintf(tmp,"%d",ac.id.uid);
     submit+=tmp;
     submit+=".log ";
    }
    cout << "  kill: " <<submit<<endl;
    int i=systemC(submit);
    if(signal != SIGKILL and !i){
       system("sleep 10");
     }

    return i;
}


bool Server_impl::Lock(const DPS::Client::CID & pid, DPS::Server::OpType optype, DPS::Client::ClientType type,int TimeOut){ 
time_t tt;
time(&tt);
switch (optype){


case DPS::Server::StartClient:
case DPS::Server::KillClient: 
case DPS::Server::CheckClient:
// Check if no lock
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==type){
    for ( list<DPS::Server::CriticalOps>::iterator li=pcur->getcol().begin();li!=pcur->getcol().end();++li){
      if (li->Action == _clear(optype)){
        if(tt<(*li).TimeStamp+(*li).TimeOut){
          if(_parent->Debug())_parent->IMessage(AMSClient::print(*li,"Lock found"));
          return false;
      }
      else{
       _col.erase(li);        
      }
     }
   }
  }
 }
break;
}

  DPS::Client::CID cid;
  cid.Type=getType();
  cid.Interface= (const char *) " ";
  DPS::Server::CriticalOps op;
  op.TimeStamp=tt;
  op.TimeOut=TimeOut;
  op.Type=type;
  op.Action=optype;
  op.id=pid.uid;
  DPS::Client::ARS * pars;
  int length=getARS(cid,pars);
  DPS::Client::ARS_var arf=pars;
  for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->sendCriticalOps(pid,op);
  }
   catch (CORBA::SystemException &ex){
   }
  }
 return true;
} 


void Producer_impl::RunFailed(const DPS::Client::ActiveClient & acv){
   uinteger Run=0;
   RLI li=find_if(_rl.begin(),_rl.end(),REInfo_EqsClient((acv.id)));
    if(li!=_rl.end()){
     if((*li)->Status == DPS::Producer::Processing || (*li)->Status == DPS::Producer::Allocated ){
       DPS::Producer::RunEvInfo_var rv=*li;
       rv->cinfo.HostName=(acv.id).HostName;
       rv->Status=rv->History;
        rv->CounterFail++;
        if(LastTry(rv)){
         rv->History=DPS::Producer::Failed;
        }
       if(rv->Status!=DPS::Producer::Foreign){
           rv->cuid=0;
           //rv->pid=0;
           rv->cinfo.HostName=(acv.id).HostName;
        }
        else rv->cinfo.HostName=(const char *)"  ";
       Run=rv->Run;
       _parent->EMessage(AMSClient::print(rv, " run Failed "));
       Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
       _pser->MonInfo(AMSClient::print(rv, " Run Failed: "),DPS::Client::Error);
       PropagateRun(rv,DPS::Client::Update);
     }
    }

for(DSTLI ni=_dst.begin();ni!=_dst.end();++ni){
 if( ((*ni).second)->Run==Run && ((*ni).second)->Status==DPS::Producer::InProgress ){
  ((*ni).second)->Status=DPS::Producer::Failure;
   
  PropagateDST((*ni).second,DPS::Client::Update,DPS::Client::AnyButSelf,_parent->getcid().uid);

 }
}



}


bool Server_impl::PingServer(const DPS::Client::ActiveClient & ac){
    DPS::Client::CID cid=_parent->getcid();
    cid.Type=getType();
    cid.Interface= (const char *)" ";
    DPS::Client::ARS * pars;
    int length=getARS(cid,pars,DPS::Client::Self,ac.id.uid);
    DPS::Client::ARS_var arf=pars;
  for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
     if(!CORBA::is_nil(_pvar)){
     return _pvar->AdvancedPing();
     }
   }

   catch ( CORBA::TRANSIENT  a){
    _parent->EMessage(AMSClient::print(ac,"Transient Error Occurs"));
    return true;    
   }

   catch (CORBA::SystemException &ex){
   }
  } 
  return false;
}

bool Producer_impl::NotAllServersAlive(){
    DPS::Client::CID cid=_parent->getcid();
    cid.Type=getServer()->getType();
    cid.Interface= (const char *)" ";
    DPS::Client::ARS * pars;
    int length=getARS(cid,pars);
    DPS::Client::ARS_var arf=pars;
  for(unsigned int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
     if(!CORBA::is_nil(_pvar)){
     _pvar->ping();
     }
   }
   catch (CORBA::SystemException &ex){
     return false;
   }
  } 
  return true;
}


void Server_impl::_PurgeQueue(){
for(ACALI li=_acqueue.begin();li!=_acqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingACQueue"));
 PropagateAC(li->getacv(),li->getaction(),DPS::Client::Self,_RecID);
}
_acqueue.clear();
}

void Producer_impl::_PurgeQueue(){
for(ACALI li=_acqueue.begin();li!=_acqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingACQueue"));
 PropagateAC(li->getacv(),li->getaction(),DPS::Client::Self,_RecID);
}
_acqueue.clear();
for(RALI li=_runqueue.begin();li!=_runqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingRunQueue"));
 PropagateRun(li->getacv(),li->getaction(),DPS::Client::Self,_RecID);
}
_runqueue.clear();
for(DSTALI li=_dstqueue.begin();li!=_dstqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingDSTQueue"));
 PropagateDST(li->getacv(),li->getaction(),DPS::Client::Self,_RecID);
}
_dstqueue.clear();

}

void AMSServer::StopEverything(){
for (AMSServerI * pser=_pser;pser;pser=pser->down()?pser->down():pser->next()){
   Server_impl *ptrue=dynamic_cast<Server_impl*>(pser->getServer());
  if(ptrue){
    for (AMSServerI::AHLI li=pser->getahl().begin();li!=pser->getahl().end();++li){
     (*li)->Status=DPS::Client::InActive;
   }
   for (AMSServerI::ACLI li=pser->getacl().begin();li!=pser->getacl().end();++li){
    if((*li)->id.uid!=_pid.uid || (*li)->id.pid!=_pid.pid)ptrue->Kill(*li,SIGTERM,true);
  }
 }
}
IMessage("Raising SIGTERM ");
raise(SIGTERM);
}


void AMSServer::DumpIOR(){
AString dmp("DumpIOR");
if(IsMC())dmp+=".0";
else dmp+=".1";
IMessage((const char*)dmp);
 for(map<AString,CORBA::String_var>::iterator li=_pser->getrefmap().begin();li!=_pser->getrefmap().end();++li){
IMessage((const char*)li->first);
IMessage((const char*)li->second);
}
{
 ofstream fbin;
 AString fnam("/tmp/DumpIOR");
 if(IsMC())fnam+=".0";
 else fnam+=".1";
 fbin.open((const char*)fnam);
 if(fbin){
 for(map<AString,CORBA::String_var>::iterator li=_pser->getrefmap().begin();li!=_pser->getrefmap().end();++li){
 fbin <<((const char*)li->first)<<endl;
 fbin<<((const char*)li->second)<<endl;
 }
 fbin.close();
 }
 else EMessage(" Unable To Open /tmp/DumpIOR File");
}
{
       ofstream fbin;
       AString fnam;
         char * logdir=getenv("AMSDataDir");
         if(!logdir){
            EMessage(" Unable To Open $AMSDataDir");
            return;
         }
         fnam=logdir;
         fnam+="/prod.log/DumpIOR";
 if(IsMC())fnam+=".0";
 else fnam+=".1";
 fbin.open((const char*)fnam);
 if(fbin){
 for(map<AString,CORBA::String_var>::iterator li=_pser->getrefmap().begin();li!=_pser->getrefmap().end();++li){
 fbin <<((const char*)li->first)<<endl;
 fbin<<((const char*)li->second)<<endl;
 }
 fbin.close();
 }
 else EMessage(" Unable To Open $ProductionLogDir/DumpIOR File");
}

}


uinteger Producer_impl::getSmartFirst(uinteger run, uinteger rndm[2]){
 pair<DSTLI,DSTLI>dst[3];
 bool present[3];
 DPS::Producer::DSTType Ntuple=DPS::Producer::Ntuple;
 DPS::Producer::DSTType RootFile=DPS::Producer::RootFile;
 DPS::Producer::DSTType EventTag=DPS::Producer::EventTag;
 dst[0]=_dst.equal_range( Ntuple);
 dst[1]=_dst.equal_range(RootFile);
 dst[2]=_dst.equal_range(EventTag);
  bool check=false;
 for (int i=0;i<3;i++){
  if(dst[i].first==dst[i].second)present[i]=false;
  else present[i]=true;
   check=check || present[i];
 }
  if(!check)return 0;
  uinteger rndmm[3][2];
  uinteger first[3]={INT_MAX,INT_MAX,INT_MAX};
 for(int i=0;i<3;i++){
  if(present[i]){
   first[i]=0;
   for(DSTLI li=dst[i].first;li!=dst[i].second;++li){
    if(((li->second)->Status ==DPS::Producer::Success  || (li->second)->Status ==DPS::Producer::Validated) && (li->second)->Run==run){
     if(first[i]<((li->second)->LastEvent)+1){
       first[i]=((li->second)->LastEvent)+1;
       rndmm[i][0]=(li->second)->rndm1;
       rndmm[i][1]=(li->second)->rndm2;
     }
    }
   }
  }
 }
 
 uinteger veryfirst=INT_MAX;
 for(int i=0;i<3;i++){
  if(veryfirst>first[i] && first[i]>0){
    veryfirst=first[i];
    rndm[0]=rndmm[i][0];
    rndm[1]=rndmm[i][1];
  }
 }
if(veryfirst== INT_MAX)veryfirst=0;
 return veryfirst;
}

#include <sys/stat.h>
#include <sys/file.h>

int Producer_impl::getRun(const DPS::Client::CID &cid, const DPS::Producer::FPath & fpath ,DPS::Producer::RUN_out run, DPS::Producer::TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp){

const int maxs=8000000;
_UpdateACT(cid,DPS::Client::Active);
 ifstream fbin;
 struct stat statbuf;
 st=DPS::Producer::Continue;
 stat((const char*)fpath.fname, &statbuf);
 fbin.open((const char*)fpath.fname);
 if(!fbin){
  AString a("Server-F-Unable to open file ");
  a+=(const char*)fpath.fname;
  throw DPS::Producer::FailedOp((const char *) a);
 }
 int last=statbuf.st_size-fpath.pos;
 if(last>maxs)last=maxs;
 else st=DPS::Producer::End;
DPS::Producer::RUN_var vrun=new DPS::Producer::RUN();
vrun->length(last);
 fbin.seekg(fpath.pos);
 fbin.read(( char*)vrun->get_buffer(),last);
 if(!fbin.good()){
   throw DPS::Producer::FailedOp((const char*)"Server-F-Unable to read file");
 }
 fbin.close();
 run=vrun._retn();
 _UpdateACT(cid,DPS::Client::Active);
 return last;
}


int Producer_impl::sendFile(const DPS::Client::CID &cid,  DPS::Producer::FPath & fpath ,const  DPS::Producer::RUN & run,DPS::Producer::TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp){
const int maxs=2000000;
_UpdateACT(cid,DPS::Client::Active);
   AString fname;
   char* gtv=getenv("AMSProdOutputDir");
   if(gtv && strlen(gtv)>0){
     fname=gtv;
     fname+="/";
     char tmp[80];
     sprintf(tmp,"%d",cid.uid);
     fname+=tmp;
     if(st!=DPS::Producer::Continue){
      AString fmake="mkdir -p ";
      fmake+=fname;
      int i=system((const char*)fmake);
      cout <<" Producer_impl::sendFile-I-MakingDirectory "<<fmake<<" result "<<i<<endl;
     }
     fname+="/";
   }
   else throw DPS::Producer::FailedOp((const char*)"Server-F-AMSProdOutputDir NotDefined");
   fname+=(const char*)fpath.fname;
 ofstream fbin;
 fbin.open((const char*)fname,ios::out|ios::app);
 if(!fbin){
  AString a("Server-F-Unable to open file ");
  a+=(const char*)fname;
  throw DPS::Producer::FailedOp((const char *) a);
 }
 fbin.write(( char*)run.get_buffer(),run.length());
 if(!fbin.good()){
   throw DPS::Producer::FailedOp((const char*)"Server-F-Unable to write file");
 }
 fbin.close();
  _UpdateACT(cid,DPS::Client::Active);
  if(st==DPS::Producer::End)fpath.fname=(const char*)fname;
 return 1;
}



void Server_impl::sendNC(const DPS::Client::CID &  cid, const  DPS::Client::NominalClient & nc, DPS::Client::RecordChange rc)throw (CORBA::SystemException){


  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      NCLI li=find_if(pcur->getncl().begin(),pcur->getncl().end(),Eqs_n(nc));
      switch (rc){
       case DPS::Client::Update:
       if(li==pcur->getncl().end())_parent->EMessage(AMSClient::print(nc,"Client not found for editing"));
       else{
        DPS::Client::NominalClient_var vac= new DPS::Client::NominalClient(nc);
        replace_if(li,pcur->getncl().end(),Eqs_n(nc),vac);
       }
       break;
       case DPS::Client::Create:
       if(li!=pcur->getncl().end())_parent->EMessage(AMSClient::print(nc,"Client already exists"));
       else{
        DPS::Client::NominalClient_var vac= new DPS::Client::NominalClient(nc);
         pcur->getncl().push_back(vac);
       }
       break;
       case DPS::Client::Delete:
       if(li==pcur->getncl().end())_parent->EMessage(AMSClient::print(nc,"Client not found for deleting"));
       else{
         pcur->getncl().erase(li);
       }
       break;
       default:
        _parent->EMessage(AMSClient::print(nc,"sendNC-Unknown rc value"));
     }

   }
 }
}

void Server_impl::sendNK(const DPS::Client::CID &  cid, const  DPS::Client::NominalClient & nc, DPS::Client::RecordChange rc)throw (CORBA::SystemException){


      NCLI li=find_if(_nki.begin(),_nki.end(),Eqs_n(nc));
      switch (rc){
       case DPS::Client::Update:
       if(li==_nki.end())_parent->EMessage(AMSClient::print(nc,"sendNK-Client not found for editing"));
       else{
        DPS::Client::NominalClient_var vac= new DPS::Client::NominalClient(nc);
        replace_if(li,_nki.end(),Eqs_n(nc),vac);
       }
       break;
       case DPS::Client::Create:
       if(li!=_nki.end())_parent->EMessage(AMSClient::print(nc,"sendNK-Client already exists"));
       else{
        DPS::Client::NominalClient_var vac= new DPS::Client::NominalClient(nc);
         _nki.push_back(vac);
       }
       break;
       case DPS::Client::Delete:
       if(li==_nki.end())_parent->EMessage(AMSClient::print(nc,"sendNK-Client not found for deleting"));
       else{
         _nki.erase(li);
       }
       break;
       default:
        _parent->EMessage(AMSClient::print(nc,"sendNK-Unknown rc value"));
     }

   }

#include <sys/statfs.h>
CORBA::Boolean Server_impl::getDBSpace(const DPS::Client::CID &cid, const char * path, const char * addpath, DPS::Server::DB_out dbo)throw (CORBA::SystemException){




 if( _parent->IsOracle()){
     Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     for (AMSServerI* pcur=_pser;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
     if(DBServer_impl* pdb=dynamic_cast<DBServer_impl*>(pcur)){
          return pdb->getDBSpace(cid,path,addpath,dbo);
       }
    }

 }
 else{




   DPS::Server::DB_var dbv= new DPS::Server::DB();
   AString amsdatadir; 

   char* gtv=getenv(path);
   if(gtv && strlen(gtv)>0){
     amsdatadir=gtv;
   }
   else{
      AString a("UnknownENVingetDBSpace ");
      a+=path;
      a+=" ";
      a+=addpath;
    _parent->EMessage(a);
   dbv->fs=(const char *)" Unknown";
   dbv->dbfree=-1;
   dbv->dbtotal=-1;
   dbv->bs=-1;
   dbo=dbv._retn();
   return false;
  }
    
    amsdatadir+=addpath;
  struct statfs buffer;
  int fail=statfs((const char*)amsdatadir,&buffer); 
  if(fail){
   dbv->fs=(const char *)amsdatadir;
   dbv->dbfree=-1;
   dbv->dbtotal=-1;
   dbv->bs=-1;
   dbo=dbv._retn();
   return false;
  }
  else{
   dbv->fs=(const char *)amsdatadir;
   dbv->dbfree= (buffer.f_bavail*(buffer.f_bsize/1024./1024.));
   dbv->dbtotal= (buffer.f_blocks*(buffer.f_bsize/1024/1024.));
   dbv->bs=1024*1024;
   dbo=dbv._retn();
  }
  return true;

 }

}



int Client_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out arf, DPS::Client::AccessType type, uinteger id, int selffirst)throw (CORBA::SystemException){



 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

return _pser->getARS(cid, arf,type,id,selffirst);

}
  CORBA::Boolean Client_impl::sendId(DPS::Client::CID& cid, float Mips,uinteger timeout) throw (CORBA::SystemException){
    if(cid.Type==DPS::Client::Monitor){
     DPS::Client::ActiveClient_var vac=new DPS::Client::ActiveClient();
      if(_acl.size()){
       _Submit=((*(_acl.begin()))->id).uid;
      }
      else _Submit=0;
     if(cid.uid==0){
      cid.uid=++_Submit;
     } 
       vac->id=cid;
       vac->Status=DPS::Client::Registered;
       time_t tt;
       time(&tt);
       vac->LastUpdate=tt;
       vac->TimeOut=timeout;
       vac->Start=tt;
     (vac->ars).length(0);
        _parent->IMessage(AMSClient::print(cid,"Client_impl::sendId-I-RegClient") );
//         PropagateAC(vac,DPS::Client::Create);
         _acl.push_front(vac);
       return true;
     }
    else if(cid.Type==DPS::Client::DBServer){
     for(ACLI j=_acl.begin();j!=_acl.end();++j){
      if(((*j)->id).uid==cid.uid){
        cid.ppid=((*j)->id).pid;
       ((*j)->id).pid=cid.pid;
       ((*j)->id).ppid=cid.ppid;
       cid.Interface=CORBA::string_dup(((*j)->id).Interface);
       cid.Type=((*j)->id).Type;
       (*j)->Status=DPS::Client::Registered;
       time_t tt;
       time(&tt);
       (*j)->LastUpdate=tt;
#ifdef __AMSDEBUG__
       _parent->IMessage(AMSClient::print(cid,"Client_impl::sendId-I-RegClient "));
#endif 
//       DPS::Client::ActiveClient_var acv=*j;
//       PropagateAC(acv,DPS::Client::Update);
       return true;
      }
      }
       _parent->EMessage(AMSClient::print(cid,"Client_impl::sendId-E-RegClientNotFound "));
     return false;

    }
  }

void Client_impl::getId(DPS::Client::CID_out  cid)throw (CORBA::SystemException){

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
   _pser->getId(cid);
}

  Client_impl::Client_impl(DPS::Client::ClientType type, const char* ctype,AMSClient * parent):POA_DPS::Client(),AMSServerI(AMSID(ctype,0),parent,type){
}

Client_impl::Client_impl(DPS::Client::ClientType type, const char* ctype,DPS::Server_ptr _svar,DPS::Client::CID  cid,AMSClient * parent):POA_DPS::Client(),AMSServerI(AMSID(ctype,0),parent,type){

ReReadTables(_svar);

}

void Client_impl::_init(){
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  _defaultorb=_pser->getdefaultorb();
 if(getType()==DPS::Client::DBServer && _parent->getdbfile()){
 }

}


void Client_impl::StartClients(const DPS::Client::CID & pid){
 if(getType() == DPS::Client::DBServer){
  RegisteredClientExists();
    for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
    if(pcur->InactiveClientExists(getType()))return;
 
}
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  bool freeslot=true;
    for(ACLI li=_acl.begin();li!=_acl.end();++li){
     if(strstr((const char*)_parent->getcid().HostName,(const char*)(*li)->id.HostName)){
       freeslot=false;
       break;
     }
    } 
//   cout <<"  dbserver start "<<freeslot<<" "<<_acl.size()<<" "<<_parent->getmaxdb()<<endl;
  if(_acl.size()<_parent->getmaxdb() && freeslot){
   if(_pser->Lock(pid,DPS::Server::StartClient,getType(),_StartTimeOut)){
   char * perldir=getenv("AMSProdPerl");
     if(!perldir){
      _parent->EMessage("AMSProdPerl not defined");
     }
     else{
               AString submit(perldir);
               submit+="/dbserver.start -U";
               char tmp[80];
               sprintf(tmp,"%d",_Submit+1);
               submit+=tmp;
               _pser->UpdateDBFileName();
               if(_parent->getdbfile()){
                 submit+=" -F";
                 submit+=_parent->getdbfile();
               }
               if(_parent->IsOraperl()){
                 submit+=" -o";
               }
                submit+=" -A";
                submit+=getenv("AMSDataDir");
                submit+="/prod.log/";
               submit+=" -I";
               submit+=perldir;
               submit+=" -D1";
               submit+=" -i";
   CORBA::String_var _refstring=_pser->getrefmap().find((const char*)_parent->getcid().Interface)->second;
               submit+=(const char*) _refstring;
           cout <<" submit "<<submit<<endl;
      int pid=fork();
//      int pid=1000;
      if(pid==-1){
       _parent->EMessage("Unable to fork DBServer");
      }
      else if(  pid==0){
        //Child
               char *argv[4];
               argv[0] = "";
               argv[1] = "-c";
               char *arg = new char[submit.length()+1];
               strcpy(arg,(const char*)submit);
               argv[2] = arg;
               argv[3] = 0;
               execve("/bin/sh", argv, __environ);
               exit(127);
      }
      else{
     // Add New Active Client
       DPS::Client::ActiveClient ac;
       ac.id=_parent->getcid();
       ac.id.uid=_Submit+1;
     ac.id.Type=getType();
     ac.id.ppid=ac.id.pid;
     ac.id.pid=pid;;
     ac.id.Status=DPS::Client::NOP;
     ac.Status=DPS::Client::Submitted;
     ac.id.StatusType=DPS::Client::Permanent;
     ac.StatusType=DPS::Client::Permanent;
     ac.TimeOut=_KillTimeOut;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
     ((ac.ars)[0]).Interface=(const char *)("Dummy");
     ((ac.ars)[0]).IOR=(const char *)(" ");
     ((ac.ars)[0]).Type=DPS::Client::Generic;
     ((ac.ars)[0]).uid=0;
     DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(ac);
     PropagateAC(acv,DPS::Client::Create);

      }
     }
     _pser->Lock(pid,DPS::Server::ClearStartClient,getType(),_StartTimeOut);
    }
   }
  }

 }

void Client_impl::CheckClients(const DPS::Client::CID & cid){
if(!Master())return;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(cid,DPS::Server::CheckClient,getType(),_KillTimeOut))return;
time_t tt;
time(&tt);
for(ACLI li=_acl.begin();li!=_acl.end();++li){
 // find clients with timeout
 if((*li)->Status!=DPS::Client::Killed && (*li)->LastUpdate+(*li)->TimeOut<tt){
   if(PingClient(*li)){
    _UpdateACT((*li)->id,DPS::Client::Active);
   }
   else{
    DPS::Client::ActiveClient_var acv=*li;
    acv->Status=DPS::Client::TimeOut;
    if(_parent->Debug())_parent->EMessage(AMSClient::print(acv,"Client TIMEOUT"));
    PropagateAC(acv,DPS::Client::Update);
 }
 }
}

_pser->Lock(cid,DPS::Server::ClearCheckClient,getType(),_KillTimeOut);

}

void Client_impl::KillClients(const DPS::Client::CID & pid){

if(!Master())return;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(pid,DPS::Server::KillClient,getType(),_KillTimeOut))return;

 ACLI li=find_if(_acl.begin(),_acl.end(),find(DPS::Client::TimeOut)); 
 if(li!=_acl.end()){
    DPS::Client::ActiveClient_var acv=*li;
    PropagateAC(acv,DPS::Client::Delete);
 }

_pser->Lock(pid,DPS::Server::ClearKillClient,getType(),_KillTimeOut);


}



void Client_impl::Exiting(const DPS::Client::CID & cid, const char * message, DPS::Client::ClientExiting status)throw (CORBA::SystemException){
_parent->IMessage(AMSClient::print(cid,message?message:"Client exiting"));
// find and remove client
for( ACLI li=_acl.begin();li!=_acl.end();++li){
 if (cid.uid==((*li)->id).uid){
        if(_parent->Debug())_parent->IMessage(AMSClient::print((*li),"Deleting Client "));
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete);
   return;

}
 _parent->EMessage(AMSClient::print(cid,"Server_impl::Exiting::No Such Client"));
}

}
bool Server_impl::MonDialog(const char * message, DPS::Client::ErrorType error){
  unsigned long timeout=20;
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==DPS::Client::Monitor){
     for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[0].IOR);
       if(!CORBA::is_nil(obj)){
        DPS::Monitor_var _mvar=DPS::Monitor::_narrow(obj);
       if(!CORBA::is_nil(_mvar)){
        try{
         CORBA::Boolean kill=_mvar->MonDialog(_parent->getcid(),message,error,timeout);
         return kill;
        }
        catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc talking monitor"<<endl;
        }
       }
       }
       }
         catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc init mondialog"<<endl;
         }
     }

     break;
    }
  }
  return true;
}

void Server_impl::MonInfo(const char * message, DPS::Client::ErrorType error){
  unsigned long timeout=20;
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==DPS::Client::Monitor){
     for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[0].IOR);
       if(!CORBA::is_nil(obj)){
        DPS::Monitor_var _mvar=DPS::Monitor::_narrow(obj);
       if(!CORBA::is_nil(_mvar)){
        try{
         _mvar->MonInfo(_parent->getcid(),message,error,timeout);
         return;
        }
        catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc talking monitor"<<endl;
        }
       }
       }
       }
         catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc init moninfo"<<endl;
         }
     }
     break;
    }
  }
  
}

bool Client_impl::PingClient(const DPS::Client::ActiveClient & ac){
       if(ac.id.Type==DPS::Client::Monitor ){
        try{
        CORBA::Object_var obj=_defaultorb->string_to_object((ac.ars)[0].IOR);
        if(!CORBA::is_nil(obj)){
        DPS::Monitor_var _mvar=DPS::Monitor::_narrow(obj);
        if(!CORBA::is_nil(_mvar)){
         try{
          _mvar->ping();
          return true;
         }
         catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc talking monitor"<<endl;
         }
        }
       }
      }
      catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc init monitor"<<endl;
      }
      return false;
   }
       if(ac.id.Type==DPS::Client::DBServer ){
        try{
        CORBA::Object_var obj=_defaultorb->string_to_object((ac.ars)[0].IOR);
        if(!CORBA::is_nil(obj)){
        DPS::DBServer_var _mvar=DPS::DBServer::_narrow(obj);
        if(!CORBA::is_nil(_mvar)){
         try{
          _mvar->ping();
          return true;
         }
         catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc talking monitor"<<endl;
         }
        }
       }
      }
      catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc init monitor"<<endl;
      }
      return false;
   }
   return true;
}



bool Client_impl::Master(bool advanced){
 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

return _pser->Master(advanced);
}

void Server_impl::setEnv(const DPS::Client::CID & cid, const char * env, const char *path)throw (CORBA::SystemException){
  setenv(env,path,1);
if(!strcmp(env,"AMSServerSleepTime"))AMSServer::Singleton()->setSleepTime();
if(!strcmp(env,"AMSDataDir")){
   AString amsdatadir(path); 
   amsdatadir+="/DataBase/";
     if(AMSDBc::amsdatabase)delete[] AMSDBc::amsdatabase;
     AMSDBc::amsdatabase=new char[strlen((const char*)amsdatadir)+1];
     strcpy(AMSDBc::amsdatabase,(const char *)amsdatadir);
     _parent->IMessage("Server_impl::setEnv-I-DataBaseRedefined");
}
     AString message("Server_impl::setEnv-I-Redefined ");
     message+=env;
     message+=" to ";
     message+=path;
     _parent->IMessage((const char*)message);
}

int Server_impl::getEnv(const DPS::Client::CID & cid, DPS::Server::SS_out ss)throw (CORBA::SystemException){
char ** e1;
unsigned int length=0;
for (e1=__environ;*e1;e1++){
 if((*e1)[0]=='A' && (*e1)[1]=='M' &&(*e1)[2]=='S'){
  length++;
 }
}
DPS::Server::SS_var vss= new DPS::Server::SS();
vss->length(length);
length=0;
for (e1=__environ;*e1;e1++){
 if((*e1)[0]=='A' && (*e1)[1]=='M'  &&(*e1)[2]=='S'){
  vss[length]=(const char*)(*e1);
  length++;
 }
}
 ss=vss._retn();
 return length;
}

void Server_impl::UpdateDBFileName(){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      CORBA::String_var filepath=dvar->getDBFilePath(_parent->getcid());
      _parent->setdbfile(filepath);
      return;
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during updatadbfile" <<endl;
     }
    }
   }
//   _parent->resetdbfile();
   return;
 }
}
}

bool AMSServerI::getHostInfoSDB(const DPS::Client::CID & cid, DPS::Client::ActiveHost_var & prv){
//return false;
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (unsigned int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      DPS::Client::CID tcid=cid;
      tcid.Type=getType();
      int ret=dvar->getFreeHostN(tcid);     
      if(ret){
      CORBA::String_var filepath=dvar->getDBFilePath(_parent->getcid());
      _parent->setdbfile(filepath);
      DPS::Client::AHS * pres;
      int length=dvar->getAHS(tcid, pres);
      DPS::Client::AHS_var res=pres; 
      if(length){
       _ahl.clear();
       for(unsigned int i=0;i<length;i++){
        DPS::Client::ActiveHost_var vre= new DPS::Client::ActiveHost(res[i]);
        _ahl.push_back(vre);
       }
      }
      else{
       _parent->EMessage(" UpdateHostTable-UnableToUpdate"); 
        return 0;
      }
       DPS::Client::ActiveHost *pre;
       ret=dvar->getFreeHost(tcid,pre);     
       cout <<"  get free host "<<AMSClient::print(*pre," ");
       prv=pre;
       return ret;
      }
      else return 0;
     }
     catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during getfreehost" <<endl;
      return 0;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}
bool Producer_impl::getRunEvInfoSDB(const DPS::Client::CID & cid, DPS::Producer::RunEvInfo_var & prv, DPS::Producer::DSTInfo_var & pdv){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      CORBA::String_var filepath=dvar->getDBFilePath(_parent->getcid());
      _parent->setdbfile(filepath);
      DPS::Producer::RES * pres;
      int length;
      if(strstr(pcur->getname(),"Perl")){
       length=dvar->getRunEvInfoSPerl(_parent->getcid(), pres,_RunID,_RunID);
      }
      else{
       length=dvar->getRunEvInfoS(_parent->getcid(), pres,_RunID);
      }
      DPS::Producer::RES_var res=pres; 
      if(length){
       _rl.clear();
       for(unsigned int i=0;i<length;i++){
        DPS::Producer::RunEvInfo_var vre= new DPS::Producer::RunEvInfo(res[i]);
        _rl.push_back(vre);
       }
      }
      else{
       _parent->EMessage(" UpdateRunTable-UnableToUpdateInfoS"); 
        return 0;
      }
      DPS::Producer::RunEvInfo *pre;
      DPS::Producer::DSTInfo *pde;
      dvar->getRunEvInfo(cid,pre,pde);     
      prv=pre;
      pdv=pde;
      return 1;
     }
     catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during getrunevinfo" <<endl;
      return 0;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}

bool Producer_impl::PropagateRunDB(const DPS::Producer::RunEvInfo & ri, DPS::Client::RecordChange rc){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));

   // cout<<"Producer_impl::PropagateRunDB -I- size : "<<pcur->getacl().size()<<endl;

   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
     // cout<<"Producer_impl::PropagateRunDB -I- ((*li)->ars).length() : "
     // <<((*li)->ars).length()<<endl;
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      dvar->sendRunEvInfo(ri,rc);
      return 1;
     }
     catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during sendrunevinfo" <<endl;
      return 0; 
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}

bool Producer_impl::PropagateDSTDB(const DPS::Producer::DST & ri, DPS::Client::RecordChange rc){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      dvar->sendDSTEnd(_parent->getcid(),ri,rc);
      return 1;
     }
     catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during senddstend" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}
bool Producer_impl::PropagateDSTInfoDB(const DPS::Producer::DSTInfo & ri, DPS::Client::RecordChange rc){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      dvar->sendDSTInfo(ri,rc);
      return 1;
     }
     catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during senddstinfo" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}

bool AMSServerI::PropagateACDB(DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer ){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      DPS::Client::CID acid=ac.id;
      if(strstr(pcur->getname(),"Perl")){
//       cout << " sending acperl "<<endl;
       dvar->sendACPerl(ac.id,ac,rc);
//       cout << " sent acperl "<<endl;
      }
      else{
       dvar->sendAC(acid,ac,rc);
      }
      if(rc == DPS::Client::Delete){
//      Here find the corr Ahost and update it
        for(AMSServerI * curp=getServer(); curp; curp=(curp->down())?curp->down():curp->next()){
          if(curp->getType()==ac.id.Type){
             for(AHLI i=curp->getahl().begin();i!=curp->getahl().end();++i){
               cout<<(const char *)(*i)->HostName<<endl;
               if(!strcmp((const char *)(*i)->HostName, (const char *)((ac.id).HostName))){
               DPS::Client::ActiveHost_var ahlv= *i;
               (ahlv->ClientsRunning)--;
        if(ahlv->ClientsRunning<0){
          _parent->EMessage("Server_impl::propagateacdb-F-NegativeNumberClientRunning ");
         ahlv->ClientsRunning=0;
        }
               time_t tt;
               time(&tt);
               (ahlv)->LastUpdate=tt;     
               switch ((ac.id).Status){
               case DPS::Client::CInExit: 
                (ahlv)->Status=DPS::Client::OK; 
                (ahlv->ClientsProcessed)++; 
                break;
               case DPS::Client::SInExit:
                (ahlv)->Status=DPS::Client::OK; 
                (ahlv->ClientsProcessed)++; 
                break;
               case DPS::Client::CInAbort:
                HostClientFailed(ahlv);
                break;
               case DPS::Client::SInKill:
                HostClientFailed(ahlv);
                (ahlv->ClientsKilled)++; 
                break;
               }
//              cout << " sending ah "<<endl;
               dvar->sendAH(acid,ahlv,DPS::Client::Update);
//              cout << " sent ah "<<endl;
               break;
              }  
             }
            }
           }
       }
       else if(rc == DPS::Client::Create){
        for(AMSServerI * curp=getServer(); curp; curp=(curp->down())?curp->down():curp->next()){
          if(curp->getType()==ac.id.Type){
             for(AHLI i=curp->getahl().begin();i!=curp->getahl().end();++i){
            if(!strcmp((const char *)(*i)->HostName, (const char *)(ac.id).HostName)){
            DPS::Client::ActiveHost_var ahlv= *i;
            ((ahlv)->ClientsRunning)++;
            time_t tt;
            time(&tt);
            (ahlv)->LastUpdate=tt;     
            (ahlv)->Status=DPS::Client::OK;
             cout <<" sending sendah"<<endl;
             dvar->sendAH(acid,ahlv,DPS::Client::Update);
            break;
        }
         }
       }
      }
      }
      return 1;

     }
     catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during sendac" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}



bool AMSServerI::PropagateAHDB(const DPS::Client::CID & pid,DPS::Client::ActiveHost & ah,DPS::Client::RecordChange rc){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      cout <<"  modifying db "<<endl; 
      dvar->sendAH(pid,ah,rc);
      cout <<"   database modified "<<endl; 
      return 1;
     }
     catch(DPS::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during sendah" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}



void AMSServerI::HostClientFailed(DPS::Client::ActiveHost_var &ahlv){
      if(ahlv->Status==DPS::Client::OK){
        (ahlv)->Status=DPS::Client::LastClientFailed;
      }
     ((ahlv)->ClientsFailed)++; 
     ((ahlv)->ClientsProcessed)++; 
     time_t tt;
     time(&tt);
     cout <<" HClF ::- Info: - "<<ahlv->ClientsProcessed<<" "<<ahlv->ClientsFailed<<" "<<tt-ahlv->LastFailed<<endl;
     if(ahlv->Status!=DPS::Client::NoResponse && (ahlv)->ClientsProcessed>3 && (ahlv)->ClientsFailed>((ahlv)->ClientsProcessed+1)/2 && tt-ahlv->LastFailed<86400){
     (ahlv)->Status=DPS::Client::InActive;
     cout <<" settting host "<<ahlv->HostName <<" to inactive "<<endl;
}
      ahlv->LastFailed=tt;

}



void AMSServerI::ReReadTables( DPS::Server_ptr _pvar){

DPS::Client::CID cid=_parent->getcid();
cid.Type=getType();
//Get NominalClient
DPS::Client::NCS * pncs;
int length=_pvar->getNC(cid,pncs);
DPS::Client::NCS_var ncs=pncs;
_ncl.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::NominalClient_var vnh= new DPS::Client::NominalClient(ncs[i]);
 _ncl.push_back(vnh);
}


//Now Read ActiveHost

DPS::Client::AHS * pahs;
length=_pvar->getAHS(cid,pahs);
DPS::Client::AHS_var ahs=pahs;
_ahl.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::ActiveHost_var vah= new DPS::Client::ActiveHost(ahs[i]);
 _parent->IMessage(AMSClient::print(vah, "getAHS: "));
 _ahl.push_back(vah);
}

//Now Read ActiveClient

DPS::Client::ACS * pacs;
length=_pvar->getACS(cid,pacs,_Submit);
DPS::Client::ACS_var acs=pacs;
_acl.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(acs[i]);
 _acl.push_back(vac);
}
 cout <<"ACL size"<<_acl.size()<<" "<<_Submit<<" "<<endl;


}





void AMSServerI::ReWriteTables( DPS::DBServer_ptr _pvar){

//NominalClient

_pvar->clearNCS(getType());

for (NCLI li=_ncl.begin();li!=_ncl.end();++li){
 DPS::Client::RecordChange rc= DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 cid.Type=getType();
 _pvar->sendNC(cid,*li,rc);
}  

//ActiveClient

_pvar->clearACS(getType());
for (ACLI li=_acl.begin();li!=_acl.end();++li){
 DPS::Client::RecordChange rc= DPS::Client::Create;
 DPS::Client::CID cid=(*li)->id;
 _pvar->sendAC(cid,*li,rc);
}  

//ActiveHost

_pvar->clearAHS(getType());
for (AHLI li=_ahl.begin();li!=_ahl.end();++li){
 DPS::Client::RecordChange rc= DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 cid.Type=getType();
 _pvar->sendAH(cid,*li,rc);
}  


}




void Server_impl::ReReadTables( DPS::Server_ptr _pvar){

DPS::Client::CID cid=_parent->getcid();
cid.Type=getType();
//Get NominalServer
{
DPS::Client::NCS * pncs;
int length=_pvar->getNC(cid,pncs);
DPS::Client::NCS_var ncs=pncs;
_ncl.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::NominalClient_var vnh= new DPS::Client::NominalClient(ncs[i]);
 _ncl.push_back(vnh);
}
}
//Get NominalKiller

DPS::Client::NCS * pncs;
int length=_pvar->getNK(cid,pncs);
DPS::Client::NCS_var ncs=pncs;
_nki.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::NominalClient_var vnh= new DPS::Client::NominalClient(ncs[i]);
 _nki.push_back(vnh);
}

//Now Read NominalHost

DPS::Client::NHS * pnhs;
length=_pvar->getNHS(cid,pnhs);
DPS::Client::NHS_var nhs=pnhs;
_nhl.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::NominalHost_var vnh= new DPS::Client::NominalHost(nhs[i]);
 _nhl.push_back(vnh);
}

//Now Read ActiveHost

DPS::Client::AHS * pahs;
length=_pvar->getAHS(cid,pahs);
DPS::Client::AHS_var ahs=pahs;
_ahl.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::ActiveHost_var vah= new DPS::Client::ActiveHost(ahs[i]);
 _parent->IMessage(AMSClient::print(vah, "getAHS: "));
 _ahl.push_back(vah);
}

//Now Read ActiveClient

DPS::Client::ACS * pacs;
length=_pvar->getACS(cid,pacs,_Submit);
DPS::Client::ACS_var acs=pacs;
_acl.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(acs[i]);
 _acl.push_back(vac);
}
 cout <<"ACL size"<<_acl.size()<<" "<<(*_ncl.begin())->MaxClients<<" "<<_Submit<<" "<<endl;


}






void Server_impl::ReWriteTables( DPS::DBServer_ptr _pvar){

//NominalHost
_pvar->clearNHS();

for (NHLI li=_nhl.begin();li!=_nhl.end();++li){
 DPS::Client::RecordChange rc= DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 cid.Type=getType();
 _pvar->sendNH(cid,*li,rc);
}

//NominalClient
_pvar->clearNCS(getType());
for (NCLI li=_ncl.begin();li!=_ncl.end();++li){
 DPS::Client::RecordChange rc= DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 cid.Type=getType();
 _pvar->sendNC(cid,*li,rc);
}  
//NominalKiller

_pvar->clearNCS(DPS::Client::Killer);
for (NCLI li=_nki.begin();li!=_nki.end();++li){
 DPS::Client::RecordChange rc= DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 cid.Type=getType();
 _pvar->sendNK(cid,*li,rc);
}  

//ActiveClient
_pvar->clearACS(getType());

for (ACLI li=_acl.begin();li!=_acl.end();++li){
 DPS::Client::RecordChange rc= DPS::Client::Create;
 DPS::Client::CID cid=(*li)->id;
 _pvar->sendAC(cid,*li,rc);
}  

//ActiveHost

_pvar->clearAHS(getType());
for (AHLI li=_ahl.begin();li!=_ahl.end();++li){
 DPS::Client::RecordChange rc= DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 cid.Type=getType();
 _pvar->sendAH(cid,*li,rc);
}  


}





void Producer_impl::ReReadTables( DPS::Server_ptr _svar){

DPS::Client::CID cid=_parent->getcid();
cid.Type=getType();
//Get NominalClient
DPS::Client::NCS * pncs;
int length=_svar->getNC(cid,pncs);
DPS::Client::NCS_var ncs=pncs;
_ncl.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::NominalClient_var vnh= new DPS::Client::NominalClient(ncs[i]);
 _ncl.push_back(vnh);
}


//Now Read ActiveHost

DPS::Client::AHS * pahs;
length=_svar->getAHS(cid,pahs);
DPS::Client::AHS_var ahs=pahs;
_ahl.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::ActiveHost_var vah= new DPS::Client::ActiveHost(ahs[i]);
 _parent->IMessage(AMSClient::print(vah, "getAHS: "));
 _ahl.push_back(vah);
}

//Now Read ActiveClient

DPS::Client::ACS * pacs;
length=_svar->getACS(cid,pacs,_Submit);
DPS::Client::ACS_var acs=pacs;
_acl.clear();
for(unsigned int i=0;i<length;i++){
 DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(acs[i]);
 _acl.push_back(vac);
}
 cout <<"ACL size"<<_acl.size()<<" "<<_Submit<<" "<<endl;




     CORBA::Object_var obj=_defaultorb->string_to_object((const char *)((_refmap.begin())->second));
    DPS::Producer_ptr _pvar=DPS::Producer::_narrow(obj);



//Read dstinfo

//Here read runfiletable
DPS::Producer::DSTIS * pdstis;
length=_pvar->getDSTInfoS(cid, pdstis);
DPS::Producer::DSTIS_var dstis=pdstis; 
_dstinfo.clear();
for(unsigned int i=0;i<length;i++){
DPS::Producer::DSTInfo_var vre= new DPS::Producer::DSTInfo(dstis[i]);
 _dstinfo.push_back(vre);
}
 cout <<"DSTinfosize "<<_dstinfo.size()<<endl;



//Here read runfiletable
DPS::Producer::RES * pres;
length=_pvar->getRunEvInfoS(cid, pres,_RunID);
DPS::Producer::RES_var res=pres; 
_rl.clear();
for(unsigned int i=0;i<length;i++){
DPS::Producer::RunEvInfo_var vre= new DPS::Producer::RunEvInfo(res[i]);
 _rl.push_back(vre);
}
 cout <<"RQsize "<<_rl.size()<<endl;



//Now Read Ntuple

DPS::Producer::DSTS * pdsts;
length=_pvar->getDSTS(cid,pdsts);
DPS::Producer::DSTS_var dsts=pdsts;
_dst.clear();
for(unsigned int i=0;i<length;i++){
DPS::Producer::DST_var vdst= new DPS::Producer::DST(dsts[i]);
 _dst.insert(make_pair(vdst->Type,vdst));
}

}




void Producer_impl::ReWriteTables( DPS::DBServer_ptr _pvar){

//NominalClient

_pvar->clearNCS(getType());
for (NCLI li=_ncl.begin();li!=_ncl.end();++li){
 DPS::Client::RecordChange rc=DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 cid.Type=getType();
 _pvar->sendNC(cid,*li,rc);
}  

//ActiveClient
_pvar->clearACS(getType());

for (ACLI li=_acl.begin();li!=_acl.end();++li){
 DPS::Client::RecordChange rc=DPS::Client::Create;
 DPS::Client::CID cid=(*li)->id;
 _pvar->sendAC(cid,*li,rc);
}  

//ActiveHost
_pvar->clearAHS(getType());

for (AHLI li=_ahl.begin();li!=_ahl.end();++li){
 DPS::Client::RecordChange rc=DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 cid.Type=getType();
 _pvar->sendAH(cid,*li,rc);
}  

//DSTInfo

_pvar->clearDSTI();
for (DSTILI li=_dstinfo.begin();li!=_dstinfo.end();++li){
 DPS::Client::RecordChange rc=DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 cid.Type=getType();
 _pvar->sendDSTInfo(*li,rc);
}  

//DST

_pvar->clearDST(DPS::Producer::Ntuple);
_pvar->clearDST(DPS::Producer::EventTag);
_pvar->clearDST(DPS::Producer::RootFile);
for (DSTLI li=_dst.begin();li!=_dst.end();++li){
 DPS::Client::RecordChange rc=DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 cid.Type=getType();
 _pvar->sendDSTEnd(cid,(*li).second,rc);
}  

//REInfo

_pvar->clearRunEvInfo(DPS::Producer::ToBeRerun);
_pvar->clearRunEvInfo(DPS::Producer::Failed);
_pvar->clearRunEvInfo(DPS::Producer::Processing);
_pvar->clearRunEvInfo(DPS::Producer::Finished);
for (RLI li=_rl.begin();li!=_rl.end();++li){
 DPS::Client::RecordChange rc=DPS::Client::Create;
 DPS::Client::CID cid=_parent->getcid();
 _pvar->sendRunEvInfo(*li,rc);
}  


}





//  newly defined send routines


void Server_impl::sendAHS(const DPS::Client::CID &cid,  const DPS::Client::AHS &  acs)throw (CORBA::SystemException){

 for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
  if(pser->getType()==cid.Type){
   pser->getahl().clear();
   int length =acs.length();
   cout <<"sendahs -I - length "<<length<<endl;
   for(int i=0;i<length;i++){
     DPS::Client::ActiveHost_var anh= new DPS::Client::ActiveHost(acs[i]);
    pser->getahl().push_back(anh);
   }
   return;
  }
 }
}

void Server_impl::sendNHS(const DPS::Client::CID &cid, const DPS::Client::NHS & acs)throw (CORBA::SystemException){

 for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
  if(pser->getType()==cid.Type){
   pser->getnhl().clear();
   int length =acs.length();
   for(int i=0;i<length;i++){
     DPS::Client::NominalHost_var anh= new DPS::Client::NominalHost(acs[i]);
    pser->getnhl().push_back(anh);
   }
   return;
  }
 }
}



void Server_impl::sendNCS(const DPS::Client::CID &cid, const DPS::Client::NCS & acs)throw (CORBA::SystemException){

 for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
  if(pser->getType()==cid.Type){
   pser->getncl().clear();
   int length =acs.length();
   for(int i=0;i<length;i++){
     DPS::Client::NominalClient_var anh= new DPS::Client::NominalClient(acs[i]);
    pser->getncl().push_back(anh);
   }
   return;
  }
 }
}

void Server_impl::sendNKS(const DPS::Client::CID &cid, const DPS::Client::NCS & acs)throw (CORBA::SystemException){

 for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
  if(pser->getType()==cid.Type){
   _nki.clear();
   int length =acs.length();
   for(int i=0;i<length;i++){
     DPS::Client::NominalClient_var anh= new DPS::Client::NominalClient(acs[i]);
    _nki.push_back(anh);
   }
   return;
  }
 }
}



void Producer_impl::sendRunEvInfoS(const DPS::Client::CID &cid, const DPS::Producer::RES & acs, unsigned int maxc)throw (CORBA::SystemException){

   _rl.clear();
   int length =acs.length();
   for(int i=0;i<length;i++){
     DPS::Producer::RunEvInfo_var anh= new DPS::Producer::RunEvInfo(acs[i]);
    _rl.push_back(anh);
   }
   _RunID=maxc;
   return;
  }




void Producer_impl::sendDSTS(const DPS::Client::CID & ci, const DPS::Producer::DSTS & acs)throw (CORBA::SystemException){

_dst.clear();
   int length =acs.length();
   for(int i=0;i<length;i++){
    DPS::Producer::DST_var anh= new DPS::Producer::DST(acs[i]);
    _dst.insert(make_pair(anh->Type,anh));
   }
   return;
  }


void Producer_impl::sendDSTInfoS(const DPS::Client::CID & ci, const DPS::Producer::DSTIS & acs)throw (CORBA::SystemException){

_dstinfo.clear();
   int length =acs.length();
   for(int i=0;i<length;i++){
    DPS::Producer::DSTInfo_var anh= new DPS::Producer::DSTInfo(acs[i]);
    _dstinfo.push_back(anh);
   }
   return;
  }
/*
extern "C" gpointer myrun(gpointer data){
//((CORBA::ORB_ptr)data)->run();
cout << "  count " <<*((int*)data)<<endl;;
gloop[*((int*)data)]=g_main_loop_new(NULL,FALSE);
g_main_loop_run(gloop[*((int*)data)]);
cout <<"  returning "<<*((int*)data)<<endl;
return NULL;
}
*/
extern "C" gpointer myrun(gpointer data){
//g_main_loop_new(NULL,TRUE);
((CORBA::ORB_ptr)data)->run();
return NULL;
}

int AMSServerI::systemC(const char * cmd){
static char *dir=0;
if(!dir){
AString fnam;
 char *logdir=getenv("AMSDataDir");
 if(!logdir){
   cerr<<":AMSServerI::systemC-E-AMSDataDirNotDefined"<<endl;
   return 1;
 }
 fnam=logdir;
 fnam+="/prod.log/systemC";

dir=new char[fnam.length()+1];
strcpy(dir,(const char *)fnam);
}
ofstream fbin;
fbin.open(dir);
if(fbin){
 DPS::Client::CID cid=_parent->getcid();
time_t ct;
time(&ct);
 fbin <<ct<<" ; "<<cid.pid<<" ; "<<cmd;
 fbin.close();
}
 int i=system(cmd);
 unlink(dir);
 return i;
}
