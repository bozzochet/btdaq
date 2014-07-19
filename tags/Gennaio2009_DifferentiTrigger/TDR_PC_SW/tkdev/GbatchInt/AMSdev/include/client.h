//  $Id: client.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMSCLIENT__
#define __AMSCLIENT__
#include "typedefs.h"
#include "astring.h"
#include <list>
#include <fstream.h>
#include <server-cpp-stubs.h>
//#include <orb/orbitcpp_exception.h>
#include <strstream>
class AMSClientError{
protected:
 DPS::Client::ClientExiting _exit;
AString _message;
public:
const char* getMessage() const {return (const char*)_message;}
DPS::Client::ClientExiting ExitReason() const {return _exit;}
AMSClientError(const char * message=" Normal Exit",DPS::Client::ClientExiting exit=DPS::Client::CInExit):
_message(message),_exit(exit){};
};
class AMSClient{
protected:
 bool _ExitInProgress;
bool _GlobalError;
 AMSClientError  _error;
 int _debug;
 bool _Oracle;
 bool _Oraperl;
 bool _MT;
 bool _OracleW;
 bool _OracleWarm;
 bool _MC;
uinteger _LastServiceTime;
 char * _DBFileName;
 int _MaxDBProcesses;
 CORBA::ORB_var _orb;
 fstream _fbin;
 fstream _fbin2;
 DPS::Client::CID _pid;
 DPS::Client::ClientExiting _exit;
 void _openLogFile(char * prefix,bool solo);
 bool _getpidhost(uinteger pid, const char* iface=0);
 static char _streambuffer[1024]; 
 static std::ostrstream _ost;
public:
AMSClient(int debug=0):_debug(debug),_Oracle(false),_Oraperl(false),_MT(false),_MC(false),_OracleW(false),_OracleWarm(false),_DBFileName(0),_MaxDBProcesses(0),_error(" "),_ExitInProgress(false),_GlobalError(false),_LastServiceTime(0)
{_pid.Status=DPS::Client::NOP;_pid.StatusType=DPS::Client::Permanent;};
virtual ~AMSClient(){};
const char * getdbfile() const {return _DBFileName;}
void setdbfile(const char * db);
void resetdbfile();
int getmaxdb()const{return _MaxDBProcesses;}
bool IsOracle() const {return _Oracle;}
bool IsOraperl() const {return _Oraperl;}
bool MT() const { return _MT;} 
bool IsMC() const { return _MC;}
bool & GlobalError(){return _GlobalError;}
uinteger & LastServiceTime(){return _LastServiceTime;}
bool InitOracle()     const {return _OracleW;}
bool WarmOracleInit() const {return _OracleWarm;}
bool DBServerExists() const{return _MaxDBProcesses!=0 || _Oracle;}
AMSClientError & Error(){return _error;}
int  Debug() const{return _debug;}
const DPS::Client::CID & getcid()const {return _pid;}
DPS::Client::ClientExiting & ExitReason(){return _exit;}
void FMessage(const char * ch, DPS::Client::ClientExiting res);
void EMessage(const char * ch);
void IMessage(const char * ch);
void LMessage(const char * ch);
virtual void Exiting(const char * message=0)=0;
static char * print(const DPS::Producer::TDVTableEntry & a, const char *m=" ");
static char * print(const DPS::Producer::TDVName & a, const char *m=" ");
static char * print(const DPS::Client::ActiveClient & a, const char *m=" ");
static char * print(const DPS::Client::NominalClient & a, const char *m=" ");
static char * print(const DPS::Client::CID & a, const char *m=" ");
static ostream &  print(const DPS::Client::CID & a, ostream & o);
static char * print(const DPS::Client::ActiveHost & a, const char *m=" ");
static char * print(const DPS::Client::NominalHost & a, const char *m=" ");
static char * print(const DPS::Producer::RunEvInfo & a, const char *m=" ");
static char * print(const DPS::Producer::DSTInfo & a, const char *m=" ");
static char * print(const DPS::Server::CriticalOps & a,const  char *m=" ");
static char * print(const DPS::Producer::CurrentInfo & a, const char *m=" ");
static ostream & print(const DPS::Producer::CurrentInfo & a, ostream & o);
static ostream & print(const DPS::Producer::TDVTableEntry & a, ostream & o);
static char * print(const DPS::Producer::DST & a, const char *m=" ");
static char * CS2string(DPS::Client::ClientStatus a);
static char * CT2string(DPS::Client::ClientType a);
static char * CST2string(DPS::Client::ClientStatusType a);
static char * CSE2string(DPS::Client::ClientExiting);
static char * HS2string(DPS::Client::HostStatus a);
static char * RS2string(DPS::Producer::RunStatus a);
static char * DSTS2string(DPS::Producer::DSTStatus a);
static char * DSTT2string(DPS::Producer::DSTType a);
static char * RunMode2string(DPS::Producer::RunMode a);
static char * OPS2string(DPS::Server::OpType a);
};

class Less{
protected:
DPS::Client::CID _cid;
int _selffirst;
public:

Less(){};
Less(DPS::Client::CID cid, int selffirst=0):_cid(cid),_selffirst(selffirst){};
bool operator () (const DPS::Client::ActiveClient & a,const DPS::Client::ActiveClient & b){
if(_selffirst){
if(!strstr((const char*)_cid.HostName,(const char *)a.id.HostName) && strstr((const char*)_cid.HostName,(const char *)b.id.HostName))return 0;
}
return (a.id.uid<b.id.uid);
}

bool operator()(const DPS::Producer::RunEvInfo &a,const DPS::Producer::RunEvInfo &b){
if(a.Priority!=b.Priority)return a.Priority>b.Priority;
else return a.Status<b.Status;
}

bool operator ()(const DPS::Client::ActiveHost &a,const DPS::Client::ActiveHost &b){
if(a.Status ==b.Status)return a.Clock>b.Clock;
else if( b.Status==DPS::Client::OK)return false;
else if(a.Status != DPS::Client::NoResponse)return true;
else return false;


}


};


#endif
