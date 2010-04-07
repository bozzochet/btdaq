//  $Id: trdrec.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMSTRDREC__
#define __AMSTRDREC__
#include "trdid.h"
#include "commons.h"
#include "link.h"
#include "cont.h"
#include "trdsim.h"

class AMSTRDCluster: public AMSlink{
protected:
AMSPoint _Coo;   // Global Coordinates 
integer _layer;  //  layer from 0 to 19 increasing with z 
number _ClSizeR;       // tube radius
number _ClSizeZ;      // tube 1/2 length
AMSDir   _CooDir;      // Dir Cosinuses 
uinteger _Multiplicity;
uinteger _HighMultiplicity;
float    _Edep;    // in KeV;
AMSTRDRawHit *_pmaxhit;
void _init(){}
void _printEl(ostream &o);
void _copyEl();
void _writeEl();
static AMSTRDCluster* _Head[trdconst::maxlay];
public:
AMSTRDCluster(uinteger status, uinteger layer,AMSPoint coo, number hsr, number hdz,AMSDir dir, float edep,int multip, int hmultip, AMSTRDRawHit* pmaxhit):AMSlink(status,0),_Coo(coo),_ClSizeR(hsr),_ClSizeZ(hdz),_CooDir(dir),_Edep(edep),_Multiplicity(multip),_HighMultiplicity(hmultip),_pmaxhit(pmaxhit),_layer(layer){};
static integer build(int rerun=0);
static integer Out(integer status);
number getEdep() const {return _Edep;}
integer operator < (AMSlink & o) const {
 
  AMSTRDCluster * p= dynamic_cast<AMSTRDCluster*>(&o);
  if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
  else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
  else return !p || _layer < p->_layer;
}
static AMSTRDCluster * gethead(uinteger i=0);
number getHit(uinteger i);
number getEHit(){return _ClSizeR;}
number getHitL(){return _ClSizeZ;}
uinteger getlayer()const {return _layer;}
const AMSDir & getCooDir()const {return _CooDir;}
 AMSPoint & getCoo() {return _Coo;}
inline integer Good() { 
  return ((TRFITFFKEY.FullReco!=0 || checkstatus(AMSDBc::USED)==0)
             && checkstatus(AMSDBc::GOOD));
}
uinteger getmult()const{return _Multiplicity;}
uinteger gethmult()const {return _HighMultiplicity;}

AMSTRDCluster *  next(){return (AMSTRDCluster*)_next;}
void resethash(integer i, AMSlink *head){
if(i>=0 && i <TRDDBc::nlay())_Head[i]=(AMSTRDCluster*)head;
}
~AMSTRDCluster(){if(_pos==1)_Head[_layer]=0;};

#ifdef __WRITEROOT__
 friend class TrdClusterR;
#endif
};

class AMSTRDSegment: public AMSlink{
protected:
integer _Orientation;
number _FitPar[2];
number _Chi2;
uinteger _NHits;
integer _Pattern;
integer _SuperLayer;
AMSTRDCluster * _pCl[trdconst::maxhits];
void _init(){}
void _printEl(ostream &o);
void _copyEl();
void _writeEl();
static integer pat;
static number par[2];
static AMSTRDCluster * phit[trdconst::maxhits];
static integer _TrSearcher(int icall,uinteger iseg);
static integer _addnext(integer pat, integer nhit, uinteger iseg,AMSTRDCluster* pthit[]);
void _addnextR(uinteger iseg);
public:
AMSTRDSegment():AMSlink(),_Chi2(-1),_Orientation(-1),_NHits(0),_Pattern(-1),_SuperLayer(-1){
_FitPar[0]=_FitPar[1]=0;
for(int i=0;i<trdconst::maxhits;i++){
  _pCl[i]=0;
}
}
AMSTRDSegment(integer slay,integer Pattern,uinteger nhits,integer ori,AMSTRDCluster *pcl[]):_Chi2(-1),_Orientation(ori),_NHits(nhits),_Pattern(Pattern),_SuperLayer(slay){
_FitPar[0]=_FitPar[1]=0;
for(int i=0;i<(_NHits<trdconst::maxhits?_NHits:trdconst::maxhits);i++){
 _pCl[i]=pcl[i];
}
}
integer operator < (AMSlink & o) const {
 
  AMSTRDSegment * p= dynamic_cast<AMSTRDSegment*>(&o);
  if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
  else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
  else return !p || _Pattern < p->_Pattern;
}
integer getslayer()const{return _SuperLayer;}
uinteger getNHits()const {return _NHits;}
number getFitPar(uinteger i)const {return i<2?_FitPar[i]:0;}
AMSTRDCluster *getpHit(uinteger i){return i<_NHits?_pCl[i]:0;}
static AMSTRDSegment * gethead(uinteger i=0);
static integer build(int rerun=0);
void Fit();
static integer Out(integer status);

static bool Distance1D(number par[2], AMSTRDCluster *ptr);

inline integer Good() { 
  return ((TRFITFFKEY.FullReco!=0 || checkstatus(AMSDBc::USED)==0));
}
AMSTRDSegment *  next(){return (AMSTRDSegment*)_next;}

#ifdef __WRITEROOT__
 friend class TrdSegmentR;
#endif
};



class AMSTRDTrack: public AMSlink{
protected:
class TrackPar{
public:
bool _FitDone;
AMSPoint _Coo;
AMSPoint _ErCoo;
number  _InvRigidity;
number _ErrInvRigidity;
number _Phi;
number _Theta;
number _ErPhi;
number _ErTheta;
number _Chi2;
TrackPar():_FitDone(false),_Coo(0,0,0),_ErCoo(0,0,0),_InvRigidity(0),_ErrInvRigidity(0),_Phi(0),_Theta(0),_ErPhi(0),_ErTheta(0),_Chi2(0){};
TrackPar(const TrackPar & o):_FitDone(o._FitDone),_Coo(o._Coo),_ErCoo(o._ErCoo),_InvRigidity(o._InvRigidity),_ErrInvRigidity(o._ErrInvRigidity),_Phi(o._Phi),_Theta(o._Theta),_ErPhi(o._ErPhi),_ErTheta(o._ErTheta),_Chi2(o._Chi2){};
};
class TrackBase{
public:
integer _NHits;
integer _Pattern;
AMSTRDCluster * _PCluster[trdconst::maxlay];
AMSPoint _Hit[trdconst::maxlay];
AMSPoint _EHit[trdconst::maxlay];
AMSDir _HDir[trdconst::maxlay];
TrackBase(const TrackBase & o):_NHits(o._NHits),_Pattern(o._Pattern){
 for (int i=0;i<trdconst::maxlay;i++){
  _PCluster[i]=o._PCluster[i];
  _Hit[i]=o._Hit[i];
  _EHit[i]=o._EHit[i];
  _HDir[i]=o._HDir[i];
 }
}
TrackBase():_NHits(0),_Pattern(-1){
 for (int i=0;i<trdconst::maxlay;i++){
  _PCluster[i]=0;
  _Hit[i]=0;
  _EHit[i]=0;
  _HDir[i]=AMSDir(0,0,1);
 }
}
};
class TrackBaseS{
public:
integer _NSeg;
integer _Pattern;
AMSTRDSegment * _PSeg[trdconst::maxseg];
TrackBaseS(const TrackBaseS & o):_NSeg(o._NSeg),_Pattern(o._Pattern){
 for (int i=0;i<trdconst::maxseg;i++){
  _PSeg[i]=o._PSeg[i];
 }
}
TrackBaseS(int pat, int nhit, AMSTRDSegment * ps[]):_NSeg(nhit),_Pattern(pat){
 for (int i=0;i<nhit;i++){
  _PSeg[i]=ps[i];
 }
}
TrackBaseS():_NSeg(0),_Pattern(-1){
 for (int i=0;i<trdconst::maxseg;i++){
  _PSeg[i]=0;
 }
}
};
TrackPar _StrLine;
TrackPar _Real;
TrackBase _Base;
TrackBaseS _BaseS;
bool _update;
void _init(){};
void _printEl(ostream &o);
void _copyEl();
void _writeEl();
static integer pat;
static number Distance3D(AMSPoint p, AMSDir d, AMSTRDCluster *ptr);
static AMSTRDSegment * phit[trdconst::maxseg];
static integer _TrSearcher(int icall);
static integer _addnext(integer pat, integer nhit, AMSTRDSegment* pthit[]);
void _addnextR();
public:
AMSTRDTrack():AMSlink(),_Base(),_BaseS(),_StrLine(),_Real(),_update(false){};
AMSTRDTrack(const AMSTRDTrack::TrackBase & Base, const AMSTRDTrack::TrackBaseS & BaseS, const AMSTRDTrack::TrackPar & StrLine):AMSlink(),_Base(Base),_BaseS(BaseS),_StrLine(StrLine),_Real(),_update(false){};
AMSTRDTrack(const AMSTRDTrack::TrackBaseS & BaseS):AMSlink(),_Base(),_BaseS(BaseS),_StrLine(),_Real(),_update(false){};
static integer build(int rerun=0);
void StrLineFit();
void RealFit();
static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall){};
static void alfun(integer & n, number xc[], number & fc, AMSTRDTrack * ptr);
static integer Out(integer status);
AMSPoint getCooStr()const {return _StrLine._Coo;}
AMSDir getCooDirStr()const {return AMSDir(_StrLine._Theta,_StrLine._Phi);}
AMSPoint getECooStr()const {return _StrLine._ErCoo;}
bool IsEcalCandidate();
bool Veto(int last);
AMSTRDTrack *  next(){return (AMSTRDTrack*)_next;}
integer operator < (AMSlink & o) const {
 
  AMSTRDTrack * p= dynamic_cast<AMSTRDTrack*>(&o);
  if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
  else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
  else return !p || _BaseS._Pattern < p->_BaseS._Pattern;
}

bool IsHighGammaTrack();
static void _Start(){TIMEX(_Time);}
static geant _CheckTime(){geant tt1;TIMEX(tt1);return tt1-_Time;}
static bool _NoMoreTime();
static geant _Time;
static geant _TimeLimit;

number getphi(){return _StrLine._Phi;}
number gettheta(){return _StrLine._Theta;}
AMSPoint getcoo(){return _StrLine._Coo;}

#ifdef __WRITEROOT__
 friend class  TrdTrackR;
#endif
};

#endif
