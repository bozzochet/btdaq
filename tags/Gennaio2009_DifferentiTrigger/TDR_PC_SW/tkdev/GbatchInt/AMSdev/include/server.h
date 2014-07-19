//  $Id: server.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMSPRODSERVER__
#define __AMSPRODSERVER__
#include "typedefs.h"
#include <time.h>
#include <iostream.h>
#include <unistd.h>
#include <map>
#include <list>
#include <queue>
#include <server-cpp-common.h> 
#include <server-cpp-skels.h>
#include "node.h"
#include "astring.h"
#include "client.h"
#include "timeid.h"

class AMSServerI : public AMSNode{
public:
typedef list<DPS::Server::CriticalOps> COL;
typedef list<DPS::Server::CriticalOps>::iterator COLI;
typedef list<DPS::Client::ActiveHost_var> AHL;
typedef list<DPS::Client::ActiveHost_var>::iterator AHLI;
typedef list<DPS::Client::ActiveClient_var> ACL;
typedef list<DPS::Client::ActiveClient_var>::iterator ACLI;

protected:
uinteger _SubmitTime;
uinteger _Submit;    // in fact active client ID
uinteger  _KillTimeOut;  // Kill Client Timeout
uinteger  _StartTimeOut;  // Start Client Timeout
bool _ActivateQueue;
uinteger _RecID;
CORBA::ORB_ptr  _defaultorb;    
AMSClient * _parent;

COL _col;

DPS::Client::ClientType _Type;

AHL  _ahl;

ACL  _acl;     // Active Server List
class ACA{
private:
DPS::Client::ActiveClient _a;
DPS::Client::RecordChange _rc;
public:
DPS::Client::ActiveClient & getacv(){return _a;}
DPS::Client::RecordChange getaction(){return _rc;}
ACA(const DPS::Client::ActiveClient & a,DPS::Client::RecordChange rc):_a(a),_rc(rc){}

};
typedef list<ACA> ACAL;
typedef list<ACA>::iterator ACALI;
ACAL _acqueue;

typedef list<DPS::Client::NominalClient_var> NCL;
typedef list<DPS::Client::NominalClient_var>::iterator NCLI;
NCL _ncl;

typedef list<DPS::Client::NominalHost_var> NHL;
typedef list<DPS::Client::NominalHost_var>::iterator NHLI;
NHL  _nhl;

typedef map<AString,CORBA::String_var> MS;
typedef map<AString,CORBA::String_var>::iterator MSI;
 MS _refmap;
virtual void _PurgeQueue()=0;

  void _UpdateACT(const DPS::Client::CID & cid, DPS::Client::ClientStatus status);
  void _UpdateHosts();
 public:
// class Eqs :public unary_function<DPS::Client::ActiveClient,bool>{
 class Eqs{
 DPS::Client::ActiveClient _a;
 public:
 

 explicit Eqs( const  DPS::Client::ActiveClient & a):_a(a){}
  bool operator () (const DPS::Client::ActiveClient_var & a){return _a.id.uid==a->id.uid;}
};
 class Eqs_n{
 DPS::Client::NominalClient _c;
 public:
 explicit Eqs_n( const  DPS::Client::NominalClient & c):_c(c){}
  bool operator () (const DPS::Client::NominalClient_var & a){return _c.uid==a->uid;}
};
 class Eqs_h{
 DPS::Client::ActiveHost _b;
 public:
 explicit Eqs_h( const  DPS::Client::ActiveHost & b):_b(b){}
  bool operator () (const DPS::Client::ActiveHost_var & b){return strstr((const char*)(b->HostName),(const char *)(_b.HostName));}
};
 class Eqs_nh{
 DPS::Client::NominalHost _b;
 public:
 explicit Eqs_nh( const  DPS::Client::NominalHost & b):_b(b){}
  bool operator () (const DPS::Client::NominalHost_var & b){return strstr((const char*)(b->HostName),(const char *)(_b.HostName));}
};
 class find :public unary_function<DPS::Client::ActiveClient,bool>{
 DPS::Client::ClientStatus _st;
 public:
 explicit find( const  DPS::Client::ClientStatus st):_st(st){}
  bool operator () (const DPS::Client::ActiveClient_var & a){return a->Status==_st;}
};
class NCL_find: public unary_function<DPS::Client::NominalClient,bool>{
AString _a;
public:
 explicit NCL_find(const char * s){_a=s;}
 bool operator()(const DPS::Client::NominalClient & a){
  return strstr((const char *) _a, (const char *) a.HostName);
}



};


CORBA::ORB_ptr  getdefaultorb()const {return _defaultorb;}
  uinteger getmaxcl() const {return _Submit;}
  uinteger getSubmitTime() const {return _SubmitTime;}
  void setmaxcl(uinteger maxcl)  {_Submit=maxcl;}
  void addone(){++_Submit;}
 COL & getcol(){return _col;}
  ACL & getacl(){return _acl;}
  AHL & getahl(){return _ahl;}
  NHL & getnhl(){return _nhl;}
  NCL & getncl(){return _ncl;}
  MS & getrefmap(){return _refmap;}
  DPS::Client::ClientType getType()const {return _Type;}

 virtual void UpdateDB(bool force)=0;
 virtual void StartClients(const DPS::Client::CID &cid)=0;
 virtual void CheckClients(const DPS::Client::CID &cid)=0; 
 virtual void KillClients(const DPS::Client::CID &cid)=0;
 virtual AMSServerI * getServer()=0;
 virtual void ReReadTables(DPS::Server_ptr pvar);
 virtual void ReWriteTables(DPS::DBServer_ptr pvar);
  void HostClientFailed(DPS::Client::ActiveHost_var & ahlv);
  void PropagateAC( DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc, DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0);
  bool getHostInfoSDB(const DPS::Client::CID & cid, DPS::Client::ActiveHost_var & ac);
  bool PropagateACDB( DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc);
  void PropagateAH(const DPS::Client::CID & cid, DPS::Client::ActiveHost & ah, DPS::Client::RecordChange rc, DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0);
  bool PropagateAHDB(const DPS::Client::CID & cid, DPS::Client::ActiveHost & ah, DPS::Client::RecordChange rc);
  int systemC(const char * cmd);
  bool InactiveClientExists(DPS::Client::ClientType type);
  void RegisteredClientExists();
  AMSServerI * next(){return dynamic_cast<AMSServerI*>(AMSNode::next());}
  AMSServerI * prev(){return dynamic_cast<AMSServerI*>(AMSNode::prev());}
  AMSServerI * up(){return   dynamic_cast<AMSServerI*>(AMSNode::up());}
  AMSServerI * down(){return dynamic_cast<AMSServerI*>(AMSNode::down());}

  AMSServerI(AMSID id, AMSClient * parent=0,DPS::Client::ClientType type=DPS::Client::Generic):_Type(type),AMSNode(id),_Submit(0),_StartTimeOut(60),_KillTimeOut(300),_parent(parent),_ActivateQueue(false),_RecID(0),_SubmitTime(0){};
};


class AMSServer : public AMSClient{
public:
class OrbitVars{
public:
CORBA::ORB_ptr  _orb;    
PortableServer::POA_ptr _poa;
PortableServer::POAManager_ptr _mgr;

};
protected:
static AMSServer * _Head;
typedef map<AString, AMSServer::OrbitVars> MO;  
MO _orbmap; 
AMSServerI *   _pser;
uinteger _SleepTime;   //  server sleeptime in us
public:
AMSServer(int argc, char *argv[]);
void StopEverything();
void DumpIOR();
static AMSServer* & Singleton(){return _Head;};
void Listening(int sleeptime=0);
void UpdateDB(bool force=false);
void SystemCheck(bool force=false);
void Exiting(const char * message=0);
CORBA::ORB_ptr  getdefaultorb()const {return ((*_orbmap.begin()).second)._orb;}
~AMSServer(){Exiting();if(_pser)delete _pser;}
  uinteger  getSleepTime() const {return _SleepTime;}
  uinteger  setSleepTime(){
   const char *ch=getenv("AMSServerSleepTime");
   if(ch){
   _SleepTime=atol(ch);
   cout <<"  AMSServerI::setSleepTime-I-SleepTimeSetTo "<<_SleepTime<<endl;
   }
   else{
   cout <<"  AMSServerI::setSleepTime-I-SleepTimeSetToDefault "<<_SleepTime<<endl;
   }
  }
};



class Client_impl : public virtual POA_DPS::Client, public AMSServerI{
protected:
  void _init();
  void _PurgeQueue(){};
public:
  Client_impl(DPS::Client::ClientType type=DPS::Client::Generic, const char* ctype="Generic",AMSClient * parent=0);
  Client_impl(DPS::Client::ClientType type, const char* ctype,DPS::Server_ptr _svar, DPS::Client::CID  cid,AMSClient * parent);
  void UpdateDB(bool force){};
  void StartClients(const DPS::Client::CID &cid);
  void CheckClients(const DPS::Client::CID &cid);
  void KillClients(const DPS::Client::CID &cid);
  bool PingClient(const DPS::Client::ActiveClient & ac);
  bool Master(bool advanced=true);
  CORBA::Boolean sendId(DPS::Client::CID & cid, float mips,uinteger timeout) throw (CORBA::SystemException);
  void getId(DPS::Client::CID_out cid) throw (CORBA::SystemException);
   int getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars, DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0, int selffirst=0)throw (CORBA::SystemException);
 AMSServerI * getServer(){return up();}
  void Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status)throw (CORBA::SystemException);
};





class Server_impl : public virtual POA_DPS::Server, public AMSServerI{
protected:
AString _iface;
NCL _nki;
DPS::Server::OpType _clear(DPS::Server::OpType type){ if(type==DPS::Server::StartClient)return DPS::Server::ClearStartClient;else if (type==DPS::Server::KillClient)return DPS::Server::ClearKillClient;else return DPS::Server::ClearCheckClient;}
public:
  Server_impl(uinteger i=0):POA_DPS::Server(),AMSServerI(AMSID("Server",++i),0,DPS::Client::Server){};
  Server_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient * parent, char * NS=0, char* NH=0, char *NK=0);
  Server_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _cvar, const DPS::Client::CID & cid, AMSClient * parent);
  ~Server_impl(){if(_down)_down->remove();}
 bool Master(bool advanced=true);
 integer Kill(const DPS::Client::ActiveClient & ac, int signal, bool self);
 bool PingServer(const DPS::Client::ActiveClient & ac);
 bool Lock(const DPS::Client::CID & cid, DPS::Server::OpType op, DPS::Client::ClientType type, int Time);
 void UpdateDBFileName();
 void setInterface(const char * iface){_iface=iface;}
 AMSServerI * getServer(){return this;}
 virtual void ReReadTables(DPS::Server_ptr pvar);
 virtual void ReWriteTables(DPS::DBServer_ptr pvar);
 virtual void UpdateDB(bool force=false);
 virtual void StartClients(const DPS::Client::CID &cid);
 virtual void CheckClients(const DPS::Client::CID &cid); 
 virtual void KillClients(const DPS::Client::CID &cid);
void _PurgeQueue();
  void _init();
  CORBA::Boolean sendId(DPS::Client::CID & cid, float mips,uinteger timeout) throw (CORBA::SystemException);
  void getId(DPS::Client::CID_out cid) throw (CORBA::SystemException);
  int getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out nc)throw (CORBA::SystemException);
  void sendNCS(const DPS::Client::CID &cid, const DPS::Client::NCS & nc)throw (CORBA::SystemException);
  void sendNKS(const DPS::Client::CID &cid, const DPS::Client::NCS & nc)throw (CORBA::SystemException);
  int getNK(const DPS::Client::CID &cid, DPS::Client::NCS_out nc)throw (CORBA::SystemException);
   int getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars, DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0, int selffirst=0)throw (CORBA::SystemException);
   bool ARSaux(DPS::Client::AccessType type,uinteger id,uinteger compare);
   int getACS(const DPS::Client::CID &cid, DPS::Server::ACS_out acs, unsigned int & maxc)throw (CORBA::SystemException);
   void sendAC(const DPS::Client::CID &cid,  DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   void sendAH(const DPS::Client::CID &cid,  const DPS::Client::ActiveHost & ah,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   void sendNH(const DPS::Client::CID &cid,  const DPS::Client::NominalHost & ah,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   void sendNC(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   void sendNK(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   CORBA::Boolean getDBSpace(const DPS::Client::CID &cid, const char * path, const char * addpath,DPS::Server::DB_out db)throw (CORBA::SystemException);
  void Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status)throw (CORBA::SystemException);
   int getNHS(const DPS::Client::CID &cid,DPS::Client::NHS_out nhl)throw (CORBA::SystemException);
   void sendNHS(const DPS::Client::CID &cid,const DPS::Client::NHS & nhl)throw (CORBA::SystemException);
   int getAHS(const DPS::Client::CID &cid,DPS::Client::AHS_out ahl)throw (CORBA::SystemException);
   void sendAHS(const DPS::Client::CID &cid, const DPS::Client::AHS & ahl)throw (CORBA::SystemException);
   int getEnv(const DPS::Client::CID &cid, DPS::Server::SS_out ss)throw (CORBA::SystemException);
   void setEnv(const DPS::Client::CID &cid,const char * env, const char *path)throw (CORBA::SystemException);
   CORBA::Boolean AdvancedPing()throw (CORBA::SystemException);
   void ping()throw (CORBA::SystemException);
   void SystemCheck()throw (CORBA::SystemException);
   void sendCriticalOps(const DPS::Client::CID &cid, const DPS::Server::CriticalOps & op)throw (CORBA::SystemException);
  bool pingHost(const char * host);
  CORBA::Boolean TypeExists(DPS::Client::ClientType type)throw (CORBA::SystemException);

  void StartSelf(const DPS::Client::CID & cid, DPS::Client::RecordChange rc);
  void MonInfo(const char * message, DPS::Client::ErrorType error);
  bool MonDialog(const char * message, DPS::Client::ErrorType error);
};




class Producer_impl : public virtual POA_DPS::Producer, public AMSServerI{
protected:

uinteger _RunID;

bool _DBInitialized;

typedef map<AMSID,AMSTimeID*> TID;
typedef map<AMSID,AMSTimeID*>::iterator TIDI;
TID _tid; 


typedef list<DPS::Producer::RunEvInfo_var> RL;
typedef list<DPS::Producer::RunEvInfo_var>::iterator RLI;
RL _rl;

typedef multimap<DPS::Producer::DSTType,DPS::Producer::DST_var> DSTL;
typedef multimap<DPS::Producer::DSTType,DPS::Producer::DST_var>::iterator DSTLI;
DSTL _dst;

typedef list<DPS::Producer::DSTInfo_var> DSTIL;
typedef list<DPS::Producer::DSTInfo_var>::iterator DSTILI;
DSTIL _dstinfo;

bool LastTry(const DPS::Producer::RunEvInfo_var & rv){
 unsigned int maxtry=1;
  char* gtv=getenv("AMSMaxRunFailRate");
   if(gtv && strlen(gtv)>0){
    maxtry=atol(gtv);
    if(maxtry>10)maxtry=10;
   }
 if(rv->CounterFail>=maxtry)return true;
 else return false;
}

class RA{
private:
DPS::Producer::RunEvInfo _a;
DPS::Client::RecordChange _rc;
public:
DPS::Producer::RunEvInfo & getacv(){return _a;}
DPS::Client::RecordChange getaction(){return _rc;}
RA(const DPS::Producer::RunEvInfo & a,DPS::Client::RecordChange rc):_a(a),_rc(rc){}

};
typedef list<RA> RAL;
typedef list<RA>::iterator RALI;
RAL _runqueue;

class DSTA{
private:
DPS::Producer::DST _a;
DPS::Client::RecordChange _rc;
public:
DPS::Producer::DST & getacv(){return _a;}
DPS::Client::RecordChange getaction(){return _rc;}
DSTA(const DPS::Producer::DST & a,DPS::Client::RecordChange rc):_a(a),_rc(rc){}

};
typedef list<DSTA> DSTAL;
typedef list<DSTA>::iterator DSTALI;
DSTAL _dstqueue;


public:
 bool getRunEvInfoSDB(const DPS::Client::CID & cid, DPS::Producer::RunEvInfo_var & rv, DPS::Producer::DSTInfo_var &dv);
 bool NotAllServersAlive();
 bool Master(bool advanced=true);
 AMSServerI * getServer(){return up();}

  Producer_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient * parent,char * NC=0, char* RF=0, char* NS=0, char * TS=0);
  Producer_impl(uinteger i=0):POA_DPS::Producer(),AMSServerI(AMSID("Producer",++i),0,DPS::Client::Producer){};
  Producer_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _cvar,DPS::Client::CID  cid, AMSClient * parent);
  void _PurgeQueue();
  void _init();
 void pingp()throw (CORBA::SystemException);
 virtual void ReReadTables(DPS::Server_ptr pvar);
 virtual void ReWriteTables(DPS::DBServer_ptr pvar);
 virtual void UpdateDB(bool force=false);
 virtual void StartClients(const DPS::Client::CID &cid);
 virtual void CheckClients(const DPS::Client::CID &cid); 
 virtual void KillClients(const DPS::Client::CID &cid);
  void RunFailed(const DPS::Client::ActiveClient & ac);
  CORBA::Boolean sendId(DPS::Client::CID & cid, float mips,uinteger timeout) throw (CORBA::SystemException);
   int getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars,  DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0, int selffirst=0)throw (CORBA::SystemException);
  void Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status)throw (CORBA::SystemException);
  int getTDV(const DPS::Client::CID & cid,  DPS::Producer::TDVName & tdvname, DPS::Producer::TDVbody_out body)throw (CORBA::SystemException);
  int getSplitTDV(const DPS::Client::CID & cid, unsigned int & pos, DPS::Producer::TDVName & tdvname, DPS::Producer::TDVbody_out body, DPS::Producer::TransferStatus &st)throw (CORBA::SystemException);
  void sendTDV(const DPS::Client::CID & cid, const DPS::Producer::TDVbody & tdv, DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException);
  void sendTDVUpdate(const DPS::Client::CID & cid, const DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException);
  void sendTDVUpdate(const DPS::Client::CID & cid,  DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException);

  TIDI & _findTDV(const DPS::Producer::TDVName & tdv);  
  int getTDVTable(const DPS::Client::CID & cid,DPS::Producer::TDVName & tdvname, unsigned int run, DPS::Producer::TDVTable_out table)throw (CORBA::SystemException);


  int getDSTInfoS(const DPS::Client::CID &cid, DPS::Producer::DSTIS_out res)throw (CORBA::SystemException);
  void sendDSTInfoS(const DPS::Client::CID &cid, const DPS::Producer::DSTIS & res)throw (CORBA::SystemException);
  void getId(DPS::Client::CID_out cid) throw (CORBA::SystemException);
  int getRun(const DPS::Client::CID &cid, const DPS::Producer::FPath & fpath, DPS::Producer::RUN_out run,DPS::Producer::TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp);
  int sendFile(const DPS::Client::CID &cid,  DPS::Producer::FPath & fpath, const  DPS::Producer::RUN & file,DPS::Producer::TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp);
  int getRunEvInfoS(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int & maxrun)throw (CORBA::SystemException);
  void sendRunEvInfoS(const DPS::Client::CID &cid, const DPS::Producer::RES & res, unsigned int  maxrun)throw (CORBA::SystemException);
   void getRunEvInfo(const DPS::Client::CID &cid, DPS::Producer::RunEvInfo_out rv, DPS::Producer::DSTInfo_out dv)throw (CORBA::SystemException);

  void sendRunEvInfo(const  DPS::Producer::RunEvInfo & ne,DPS::Client::RecordChange rc)throw (CORBA::SystemException);

  void sendDSTInfo(const  DPS::Producer::DSTInfo & ne,DPS::Client::RecordChange rc)throw (CORBA::SystemException);

  int getDSTS(const DPS::Client::CID & ci, DPS::Producer::DSTS_out dsts)throw (CORBA::SystemException);
  void sendDSTS(const DPS::Client::CID & ci, const DPS::Producer::DSTS & dsts)throw (CORBA::SystemException);

  void sendCurrentInfo(const DPS::Client::CID & ci, const  DPS::Producer::CurrentInfo &cii, int propagate)throw (CORBA::SystemException);

  void sendDSTEnd(const DPS::Client::CID & ci, const  DPS::Producer::DST & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException);

uinteger getSmartFirst(uinteger run,uinteger rndm[2]);

void PropagateRun(const DPS::Producer::RunEvInfo & ri, DPS::Client::RecordChange rc,  DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0);
bool PropagateRunDB(const DPS::Producer::RunEvInfo & ri, DPS::Client::RecordChange rc);
void PropagateDST(const DPS::Producer::DST & ri, DPS::Client::RecordChange rc,  DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0);
bool PropagateDSTDB(const DPS::Producer::DST & ri, DPS::Client::RecordChange rc);
bool PropagateDSTInfoDB(const DPS::Producer::DSTInfo & ri, DPS::Client::RecordChange rc);

class DSTInfo_find: public unary_function<DPS::Producer::DSTInfo,bool>{
DPS::Client::CID _cid;
public:
 explicit DSTInfo_find(const DPS::Client::CID & cid):_cid(cid){};
 bool operator()(const DPS::Producer::DSTInfo & a){
  return  strstr((const char *) _cid.HostName, (const char *) a.HostName);
}
};
class DSTInfo_Eqs: public unary_function<DPS::Producer::DSTInfo,bool>{
DPS::Producer::DSTInfo _cid;
public:
 explicit DSTInfo_Eqs(const DPS::Producer::DSTInfo & cid):_cid(cid){};
 bool operator()(const DPS::Producer::DSTInfo & a){
  return  a.uid==_cid.uid;
}
};
class REInfo_find: public unary_function<DPS::Producer::RunEvInfo,bool>{
DPS::Client::CID _cid;
DPS::Producer::RunStatus _rs;
bool _status;
public:
 explicit REInfo_find(const DPS::Client::CID & cid, DPS::Producer::RunStatus rs, bool permanent=true):_cid(cid),_rs(rs),_status(permanent){}
 bool operator()(const DPS::Producer::RunEvInfo & a){
  return (a.Status==DPS::Producer::ToBeRerun ||  !_status) && (a.History==_rs || a.History==DPS::Producer::Foreign) && (_rs==DPS::Producer::ToBeRerun || strcmp((const char *) _cid.HostName, (const char *) a.cinfo.HostName)) && (_status ||  (_cid.uid==a.Run && (a.Status==DPS::Producer::Allocated || a.Status==DPS::Producer::Foreign)) );
}
};
class REInfo_process: public unary_function<DPS::Producer::RunEvInfo,bool>{
public:
 explicit REInfo_process(){}
 bool operator()(const DPS::Producer::RunEvInfo & a){
  return a.Status!=DPS::Producer::Finished && a.Status!=DPS::Producer::Failed;
}
};

class REInfo_Eqs: public unary_function<DPS::Producer::RunEvInfo,bool>{
DPS::Producer::RunEvInfo _re;
public:
 explicit REInfo_Eqs(const DPS::Producer::RunEvInfo & re):_re(re){}
 explicit REInfo_Eqs(unsigned int uid){_re.uid=uid;}
 bool operator()(const DPS::Producer::RunEvInfo & a){return a.uid== _re.uid;}
};

class REInfo_Count: public unary_function<DPS::Producer::RunEvInfo,bool>{
public:
 explicit REInfo_Count(){}
 bool operator()(const DPS::Producer::RunEvInfo & a){return a.Status== DPS::Producer::ToBeRerun || a.Status==DPS::Producer::Processing;}
};

class REInfo_EqsClient: public unary_function<DPS::Producer::RunEvInfo,bool>{
DPS::Client::CID _cid;
public:
 explicit REInfo_EqsClient(const DPS::Client::CID & cid):_cid(cid){}
 bool operator()(const DPS::Producer::RunEvInfo & a){return a.cuid== _cid.uid;}
};

class REInfo_EqsClient2: public unary_function<DPS::Producer::RunEvInfo,bool>{
DPS::Client::CID _cid;
public:
 explicit REInfo_EqsClient2(const DPS::Client::CID & cid):_cid(cid){}
 bool operator()(const DPS::Producer::RunEvInfo & a){return _cid.uid!=0 && a.Status != DPS::Producer::Allocated && a.Status!=DPS::Producer::Foreign && a.cuid== _cid.uid;}
};




};












#endif

