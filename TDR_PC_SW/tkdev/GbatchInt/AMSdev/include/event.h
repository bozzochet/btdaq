//  $Id: event.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

// Author V. Choutko 24-may-1996
// June 12, 1996. ak. add getEvent function
// Oct   1, 1997. ak. add TDV related functions
// May  23, 2002. ak. AMSEventHeaderRoot
//
// Last Edit : May 23, 2002. ak.
//

#ifndef __AMSEVENT__
#define __AMSEVENT__
#include <fstream.h>
#include "job.h"
#include "typedefs.h"
#include "upool.h"
#include "node.h"
#include "snode.h"
#include "link.h"
#include "cont.h"
#include "daqevt.h"
#include "astro.h" // ISN 
class AMSEvent: public AMSNode{
private:
  class ShuttlePar{
  public:
   geant StationR;
   geant StationTheta;
   geant StationPhi;
   // geant StationEqAsc; // ISN these variables will be
   // geant StationEqDec; // ISN used in DAQ with a data base
   //geant StationGalLat;
   //geant StationGalLong;
   geant GrMedPhi;
   geant StationYaw;
   geant StationPitch;
   geant StationRoll;
   geant StationSpeed;
   geant SunR;
   geant VelTheta;
   geant VelPhi;
   time_t Time;
  };
  class BeamPar{
   public:
    uinteger RunTag;   // runTag
    time_t Time;     // UNIX sec, of the manipulator PC
    float X;     //cm, the beam cross in AMS coo system
    float Y;     //cm
    float Z;     //cm
    float Theta; //rad [0-Pi], direction of the beam (Pi- from z-direction)
    float Phi;   //rad [0-2Pi]
    uinteger  Pid;   //Beam particle ID  == Geant One
    float Mom;   //Beam particle momentum (GeV/c) - 0-default
    float Intens;//Beam intensity (part/sec), 0-default
    uinteger  Cond;  //  Position no
    float BeamSize;  //beam size in cm;
 };
 class EventId{
    public:
    uinteger Run;
    uinteger Event;
  EventId(uinteger run=0, uinteger event=0):Run(run),Event(event){};
 int operator !=(const EventId & o)const{
   return Run!=o.Run || Event!=o.Event ;
 }
 int operator < (const EventId & o) const{
    if(Run<o.Run)return 1;
    else if(o.Run<Run)return 0;
    else{
     if(Event<o.Event && Event!=0)return 1;
     else return 0;
    }
 }
 };

uinteger _status[2];
uinteger _run;
uinteger _runtype;
uinteger _Error;
geant _StationRad;    //cm 
geant _StationTheta; 
geant _StationPhi;   
geant _NorthPolePhi;
number _StationEqAsc; //ISN  
number _StationEqDec; //ISN 
number _StationGalLat; //ISN 
number _StationGalLong; //ISN 
number _AMSEqAsc; //ISN 
number _AMSEqDec; //ISN 
number _AMSGalLat; //ISN 
number _AMSGalLong; //ISN
geant _Yaw;
geant _Pitch;
geant _Roll;
geant _StationSpeed;
geant _SunRad; 
geant _VelTheta; 
geant _VelPhi; 
time_t _time;
uinteger _usec;
static integer SRun;
static integer PosInRun;
static integer PosGlobal;
static AMSEvent * _Head;
static AMSNodeMap EventMap;
static ShuttlePar Array[60];
static BeamPar ArrayB[60];
static EventId * _SelectedEvents;
void _copyEl();
void _writeEl();
void _init();
void _siamsinitrun();
void _signinitrun();
void _sitkinitrun();
void _sitrdinitrun();
void _sitofinitrun();
void _siantiinitrun();
void _siecalinitrun();
void _sirichinitrun();

void _reamsinitrun();
void _retkinitrun();
void _retrdinitrun();
void _retofinitrun();
void _reantiinitrun();
void _reecalinitrun();
void _rerichinitrun();
void _reaxinitrun();
void _siamsinitevent();
void _signinitevent();
void _sitkinitevent();
void _sitrdinitevent();
void _sitofinitevent();
void _siantiinitevent();
void _siecalinitevent();
void _sirichinitevent();
void _reamsinitevent();
void _retkinitevent();
void _retrdinitevent();
void _retof2initevent();
void _reantiinitevent();
void _reecalinitevent();
void _rerichinitevent();
void _reaxinitevent();
void _retriginitevent();
void _siamsevent();
void _redaqinitevent();
void _reamsevent();
void _regnevent();
void _sitrigevent();
void _sitkevent();
void _sitof2event(int &cftr);
void _sianti2event();
void _sitrdevent();
void _siecalevent();
void _sirichevent();
void _sidaqevent();
void _redaqevent();
void _retkevent(integer refit=0);
void _retof2event();
void _reanti2event();
void _reecalevent();
void _retrigevent();
void _rerichevent();
void _retrdevent();
void _reaxevent();
void _caamsinitevent();
void _catkinitevent();
void _catrdinitevent();
void _catofinitevent();
void _cantinitevent();
void _caaxinitevent();
void _caamsevent();
void _catkevent();
void _catofevent();
void _cantievent();
void _caecevent();
void _catrdevent();
void _caaxevent();
void _validate(int i=0);
void _collectstatus();

void _startofrun();
static void _endofrun();

//+
void _validateDB();
//-
static const int16u _HDID;
AMSlink * _getheadC( AMSID id, integer sorted=0);
AMSlink * _getlastC( AMSID id);
integer _setheadC( AMSID id, AMSlink * p);
void _findC(AMSID & id);
AMSContainer * _getC(AMSID id);
public:
AMSEvent(AMSID id, integer run, integer runtype,time_t time,
uinteger usec,geant pole, geant stationT, geant stationP, geant VelT, geant VelP, geant StationR=666000000,geant yaw=0,geant pitch=0,geant roll=0,geant StationS=1.16e-3, geant SunR=0):AMSNode(id),_run(run),_VelTheta(VelT),_VelPhi(VelP),
  _time(time), _usec(usec),_runtype(runtype),_NorthPolePhi(pole),_StationPhi(stationP),_Roll(roll),_Yaw(yaw),_StationRad(StationR),_Pitch(pitch),_StationSpeed(StationS),_StationTheta(stationT),_SunRad(SunR),_Error(0),_StationEqAsc(0),_StationEqDec(0),_StationGalLat(0),_StationGalLong(0),_AMSEqAsc(0),_AMSEqDec(0),_AMSGalLat(0),_AMSGalLong(0){_Head=this;_status[0]=0;_status[1]=0;} //ISN
AMSEvent(AMSID id, integer run, integer runtype, time_t time, uinteger usec):AMSNode(id),_run(run),
   _runtype(runtype), _time(time), _usec(usec),_Error(0){
   _Head=this;
   _status[0]=0;
   _status[1]=0;
}
~AMSEvent(){_Head=0;}
void _printEl(ostream & stream);
void loc2gl();
void getmag(float & thetam, float & phim);
static void setfile(char file[]);
static integer IsTest();
static void SetShuttlePar();
static AMSEvent * gethead()  {return _Head;}
static integer debug;
AMSlink * getheadC(char name[], integer id, integer sorted=0)
{return _getheadC(AMSID(name,id),sorted);}
integer buildC(char name[], integer par=0);
integer rebuildC(char name[], integer par=0);
static integer getSRun() {return SRun;}
 integer setbuilderC(char name[], pBuilder pb);
AMSlink * getlastC(char name[], integer id)
{return _getlastC(AMSID(name,id));}
AMSlink * getheadC( AMSID id, integer sorted=0){return _getheadC(id,sorted);}
integer setheadC(char name[], integer id, AMSlink *p)
{return _setheadC(AMSID(name,id),p);}
integer setheadC( AMSID id, AMSlink *p){return _setheadC(id,p);}
AMSContainer * getC(char name[], integer id){return _getC(AMSID(name,id));}
AMSContainer * getC( AMSID id){return _getC(id);}
integer getnC (char name[]);
void SetTimeCoo(integer rec=0);
void GetGeographicCoo(number & pole, number & theta, number &phi){
pole=_NorthPolePhi;theta=_StationTheta;phi=_StationPhi;}
geant GetStationRad() const{return _StationRad;}
void GetISSCoo(number & ra, number & dec){
  ra=_StationEqAsc;dec=_StationEqDec;} // ISN 
void GetAMSCoo(number & ra, number & dec){
  ra=_AMSEqAsc;dec=_AMSEqDec;} // ISN 
static void  sethead(AMSEvent* head) 
{ _Head=head;if(_Head)AMSEvent::EventMap.map(*_Head);}
integer removeC();
void Recovery();
void write(int trig);
void copy();
void printA(integer debugl=0);
void event();
void seterror(uinteger error=1){_Error=error;}
integer HasNoErrors(){return _Error==0;}
integer HasNoCriticalErrors(){return _Error<2;}
integer HasFatalErrors(){return _Error>=3;}
//+
uinteger getEvent() {return uinteger(_id);}
static integer _checkUpdate();
//-
AMSlink * addnext(AMSID id, AMSlink * p);
integer replace(AMSID id, AMSlink * p, AMSlink *prev);
uinteger getrun() const{return _run;}
uinteger  * getstatus() {return _status;}
void setstatus(uinteger   * status){_status[0]=status[0];_status[1]=status[1];}
geant getpitch() const{return _Pitch;}
geant getveltheta() const{return _VelTheta;}
geant getvelphi() const{return _VelPhi;}
geant getroll() const{return _Roll;}
geant getyaw() const{return _Yaw;}
geant getangvel() const {return _StationSpeed;}
uinteger& setrun() {return _run;}
uinteger getruntype() const{return _runtype;}
uinteger& setruntype() {return _runtype;}
time_t gettime(){return _time;}
time_t getusec()const {return _usec;}
time_t& settime(){return _time;}
//void * operator new(size_t t, void *p) {return p;}
void * operator new(size_t t) {return UPool.insert(t);}
void operator delete(void *p)
  {if(p){((AMSEvent*)p)->~AMSEvent();p=0;UPool.udelete(p);}}

inline AMSNode * getp(const AMSID & id, char hint=0) const{
  return AMSEvent::EventMap.getp(id,hint);
}
inline void printH(){
  AMSEvent::EventMap.print();
}
static AMSID getTDVStatus();
// Interface with DAQ
static int16u  getdaqid(int btype){return 0x200 | btype <<13;}
static integer checkdaqid(int16u id);
static int16u  getdaqidSh(){return 1 | 14<<9;}
static integer checkdaqidSh(int16u id);
static void buildrawSh(integer length, int16u *p);
static void buildraw(integer length, int16u *p, uinteger &run, uinteger &event,
uinteger & runtype, time_t & time, uinteger & usec); 
static integer getmaxblocks(){return 1;}
static integer getmaxblocksSh(){return 1;}
static integer calcdaqlength(integer i){return 1+2+2+2+4;}
static integer calcdaqlengthSh(integer i){return 21;}
static integer calcTrackerHKl(integer i);
static void builddaq(integer i, integer length, int16u *p);
static void builddaqSh(integer i, integer length, int16u *p);
static void buildTrackerHKdaq(integer i, integer length, int16u *p);
static integer getselrun(integer i=0);
friend class AMSJob;
#ifdef __WRITEROOT__
friend class AMSEventHeaderRoot;
friend class AMSShuttleParRoot;
friend class EventRoot02;
#endif
};
#endif
