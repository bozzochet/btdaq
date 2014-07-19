//  $Id: producer.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifdef __CORBA__
#ifndef __AMSPRODUCER__
#define __AMSPRODUCER__
#include "daqevt.h"
#include "client.h"
#include "node.h"
#include "timeid.h"
#include <sys/time.h>
class AMSProducer: public AMSClient, public AMSNode{
protected:

typedef list<DPS::Producer_var> PList;
PList _plist;
static AMSProducer * _Head;
static AString * _dc;
bool _OnAir;
bool _FreshMan;
bool _Local;
bool _Solo;
bool _RemoteDST;
bool _Transfer;
float _CPUMipsTimeSpent;
DPS::Producer::CurrentInfo _cinfo;
DPS::Producer::DST   _ntend[2];  // 0 - ntuple 1 -root
DPS::Producer::DST   _evtag;
DPS::Producer::RunEvInfo_var   _reinfo;
DPS::Producer::DSTInfo_var   _dstinfo;
double _ST0;
float _T0;
void _init(){};
public:

AMSProducer(int argc,  char * argv[], int debug) throw (AMSClientError);
~AMSProducer();
static AMSProducer* gethead(){return _Head;}
static  const char* GetDataCards()  {return (const char*)(*_dc);}
bool SetDataCards(); 
DPS::Producer::DST * getdst(DPS::Producer::DSTType type);
bool & OnAir(){return _OnAir;}
bool  IsLocal(){return _Local;}
bool  IsSolo(){return _Solo;}
bool getior(const char * getiorvar);
void AddEvent();
void UpdateARS();
void getRunEventInfo();
void sendCurrentRunInfo(bool force=false);
void getASL();
void sendid();
void sendSelfId();
void sendDSTInfo();
bool Progressing();
void sendNtupleStart(DPS::Producer::DSTType type,const char * name,int run, int first,time_t begin);
void sendNtupleUpdate(DPS::Producer::DSTType type);
void sendNtupleEnd(DPS::Producer::DSTType type,int entries, int last, time_t end, bool suc);
void sendRunEnd(DAQEvent::InitResult res);
void sendRunEndMC();
void sendEventTagEnd(const char *name, uinteger run,time_t insert, time_t begin,time_t end,uinteger first,uinteger last,integer nelem, bool fail);  
void sendEventTagBegin(const char * name,uinteger run,uinteger first);  
bool getTDV(AMSTimeID * tdv, int id);
bool getSplitTDV(AMSTimeID * tdv, int id);
bool sendTDV(const AMSTimeID * tdv);
virtual void Exiting(const char * message=0);
void InitTDV(uinteger run);
};

#endif
#endif
