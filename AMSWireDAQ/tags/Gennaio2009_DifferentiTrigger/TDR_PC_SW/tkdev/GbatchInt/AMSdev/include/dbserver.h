//  $Id: dbserver.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMSDBSERVER__
#define __AMSDBSERVER__
#include "server.h"

class DBServer_impl : public virtual POA_DPS::DBServer, public AMSServerI{
protected:
  void _PurgeQueue(){};
  void _init();
public:
  DBServer_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient * parent);
  DBServer_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _cvar,DPS::Client::CID  cid, AMSClient * parent);
 bool Master(bool advanced=true);
 AMSServerI * getServer(){return up();}
 virtual void UpdateDB(bool force=false);
 virtual void StartClients(const DPS::Client::CID &cid);
 virtual void CheckClients(const DPS::Client::CID &cid); 
 virtual void KillClients(const DPS::Client::CID &cid);
  CORBA::Boolean sendId(DPS::Client::CID & cid, float mips,uinteger timeout)throw (CORBA::SystemException);
  void getId(DPS::Client::CID_out cid)throw (CORBA::SystemException);
  //  int getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out nc) throw (CORBA::SystemException,DPS::DBServer::DBProblem)throw (CORBA::SystemException);
  int getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out nc)throw (CORBA::SystemException);
  int getNK(const DPS::Client::CID &cid, DPS::Client::NCS_out nc)throw (CORBA::SystemException);
   int getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars, DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0, int selffirst=0)throw (CORBA::SystemException);
   int getACS(const DPS::Client::CID &cid, DPS::Server::ACS_out acs, unsigned int & maxc)throw (CORBA::SystemException);
   void sendAC(const DPS::Client::CID &cid,  DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   void sendACPerl(const DPS::Client::CID &cid,  const DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   void sendAH(const DPS::Client::CID &cid,  const DPS::Client::ActiveHost & ah,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   void sendNH(const DPS::Client::CID &cid,  const DPS::Client::NominalHost & ah,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   void sendNC(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   void sendNK(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   CORBA::Boolean getDBSpace(const DPS::Client::CID &cid, const char * path, const char * addpath,DPS::Server::DB_out db)throw (CORBA::SystemException);
  void Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status)throw (CORBA::SystemException);
   int getNHS(const DPS::Client::CID &cid,DPS::Client::NHS_out nhl)throw (CORBA::SystemException);
   int getAHS(const DPS::Client::CID &cid,DPS::Client::AHS_out ahl)throw (CORBA::SystemException);
   int getEnv(const DPS::Client::CID &cid, DPS::Server::SS_out ss)throw (CORBA::SystemException);
   void setEnv(const DPS::Client::CID &cid,const char * env, const char *path)throw (CORBA::SystemException);
   void ping()throw (CORBA::SystemException);
   void pingp()throw (CORBA::SystemException){};
   void SystemCheck()throw (CORBA::SystemException){};
   CORBA::Boolean AdvancedPing()throw (CORBA::SystemException){};
   void sendCriticalOps(const DPS::Client::CID &cid, const DPS::Server::CriticalOps & op)throw (CORBA::SystemException){};
  CORBA::Boolean TypeExists(DPS::Client::ClientType type)throw (CORBA::SystemException);

  int getTDV(const DPS::Client::CID & cid,  DPS::Producer::TDVName & tdvname, DPS::Producer::TDVbody_out body)throw (CORBA::SystemException);
  int getSplitTDV(const DPS::Client::CID & cid, unsigned int & pos, DPS::Producer::TDVName & tdvname, DPS::Producer::TDVbody_out body, DPS::Producer::TransferStatus &st)throw (CORBA::SystemException);
  void sendTDV(const DPS::Client::CID & cid, const DPS::Producer::TDVbody & tdv, DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException);
  void sendTDVUpdate(const DPS::Client::CID & cid, const DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException);

  int getTDVTable(const DPS::Client::CID & cid,DPS::Producer::TDVName & tdvname, unsigned int run, DPS::Producer::TDVTable_out table)throw (CORBA::SystemException);

  DPS::Client::ClientType    unsi2CT(unsigned int a);
  DPS::Client::ClientStatus  unsi2CS(unsigned int a);
  DPS::Client::ClientExiting unsi2CSE(unsigned int a);
  DPS::Client::HostStatus    unsi2HS(unsigned int a);
  DPS::Producer::RunStatus   unsi2RS(unsigned int a);
  DPS::Producer::RunMode     unsi2RM(unsigned int a);
  DPS::Producer::DSTType     unsi2DSTT(unsigned int a);
  DPS::Producer::DSTStatus   unsi2DSTS(unsigned int a);
  int getDSTInfoS(const DPS::Client::CID &cid, DPS::Producer::DSTIS_out res)throw (CORBA::SystemException);
  int getRun(const DPS::Client::CID &cid, const DPS::Producer::FPath & fpath, DPS::Producer::RUN_out run,DPS::Producer::TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp);
  int sendFile(const DPS::Client::CID &cid,  DPS::Producer::FPath & fpath, const  DPS::Producer::RUN & file,DPS::Producer::TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp);
  int getRunEvInfoS(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int & maxrun)throw (CORBA::SystemException);
  int getRunEvInfoSPerl(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int  maxrun, unsigned  int &maxrun1)throw (CORBA::SystemException);
  char * getDBFilePath(const DPS::Client::CID &cid)throw (CORBA::SystemException);
   void getRunEvInfo(const DPS::Client::CID &cid, DPS::Producer::RunEvInfo_out rv, DPS::Producer::DSTInfo_out dv)throw (CORBA::SystemException);
  void sendRunEvInfo(const  DPS::Producer::RunEvInfo & ne,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
  void sendDSTInfo(const  DPS::Producer::DSTInfo & ne,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
  int getDSTS(const DPS::Client::CID & ci, DPS::Producer::DSTS_out dsts)throw (CORBA::SystemException);
  void sendCurrentInfo(const DPS::Client::CID & ci, const  DPS::Producer::CurrentInfo &cii, int propagate)throw (CORBA::SystemException);
  void sendDSTEnd(const DPS::Client::CID & ci, const  DPS::Producer::DST & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException);
   int  getFreeHost(const DPS::Client::CID & ci, DPS::Client::ActiveHost_out host)throw (CORBA::SystemException);
   int  getFreeHostN(const DPS::Client::CID & ci)throw (CORBA::SystemException);
    void clearACS( DPS::Client::ClientType type)throw (CORBA::SystemException);
    void clearNCS( DPS::Client::ClientType type)throw (CORBA::SystemException);
    void clearAHS( DPS::Client::ClientType type)throw (CORBA::SystemException);
    void clearNHS()throw (CORBA::SystemException);
    void clearDST( DPS::Producer::DSTType type)throw (CORBA::SystemException);
    void clearDSTI()throw (CORBA::SystemException);
    void clearRunEvInfo( DPS::Producer::RunStatus status)throw (CORBA::SystemException);
   void sendNHS(const DPS::Client::CID &cid,const DPS::Client::NHS & nhl)throw (CORBA::SystemException){};
   void sendAHS(const DPS::Client::CID &cid,const DPS::Client::AHS & ahl)throw (CORBA::SystemException){};
  void sendNCS(const DPS::Client::CID &cid, const DPS::Client::NCS & nc)throw (CORBA::SystemException){};
  void sendNKS(const DPS::Client::CID &cid, const DPS::Client::NCS & nc)throw (CORBA::SystemException){};
  void sendRunEvInfoS(const DPS::Client::CID &cid, const DPS::Producer::RES & res, unsigned int  maxrun)throw (CORBA::SystemException){};
  void sendDSTInfoS(const DPS::Client::CID &cid, const DPS::Producer::DSTIS & res)throw (CORBA::SystemException){};
  void sendDSTS(const DPS::Client::CID & ci, const DPS::Producer::DSTS & dsts)throw (CORBA::SystemException){};

  }; 











#endif

