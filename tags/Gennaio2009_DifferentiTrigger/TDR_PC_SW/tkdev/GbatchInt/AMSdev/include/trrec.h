//  $Id: trrec.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
 // Author V. Choutko 24-may-1996
//
// May 27, 1996. ak. add functions to AMSTrRecHit
//                   _addnext is public now (though not neccessary)
//                   AMSTrRecHit::setClusterP
//                   AMSTrTrack:: (see getParFastFit), getNumbers
//                   _Pthit[6] ??? ! it is correct
//                   default constructor for AMSTrTrack
// June, 1996. ak.   getNumbers, setNumbers, setid for AMSTrCluster&AMSTrRecHit
// July 03, 1996. ak. V1.09 + alpha code
// Aug  07, 1996. ak. V1.24 (AMSTrTrack functions are modified accord)
// Sep  10, 1996. ak. V1.25
// Oct  10, 1996. ak. implement friend class
// Mar  15, 1997. ak. there were two same functions AMSTrTrack::getHitP and 
//                    getpthit. getHitP is removed
//                    new function AMSTrREcHit::setSensorP
// Last Edit : Mar 20, 1997. ak.
//
#ifndef __AMSTRBANK__
#define __AMSTRBANK__
#include "upool.h"
#include "apool.h"
#include "gsen.h"
#include "trid.h"
#include "link.h"
#include "dcards.h"
#include "cont.h"
#include "event.h"
#include <vector>

using namespace std;

class Test: public AMSlink{
protected:
 integer _pid;
 AMSPoint _coo;
 void _copyEl(){}
 void _printEl(ostream & stream){}
 void _writeEl(){}
public:
 Test(integer pid=0):_pid(pid){}
 Test(integer pid, AMSPoint coo):_pid(pid),_coo(coo){}
  integer getpid()const {return _pid;}
  AMSPoint getcoo()const {return _coo;}
  Test *  next(){return (Test*)_next;}

};





class AMSTrCluster: public AMSlink {
protected:
  void _ErrorCalc();
  void _printEl(ostream & stream)
  { stream << _Id << " Nelem " << -_NelemL+_NelemR << " Sum " << _Sum <<endl;}
  void _copyEl();
  void _writeEl();
AMSTrIdSoft _Id; // center
integer _NelemL; // to left
integer _NelemR; // to right
number _Sum;
number _Sigma;
number _Mean;    // with respect to center
number _Rms;
number _ErrorMean; // Guessed Error
number _Eta;

number * _pValues;

static AMSTrCluster * _addnext(const AMSTrIdSoft& id, integer status, integer nelemL,
  integer nelemR, number sum,  number ssum, number pos, number rms, number eta,
  number val[]);

static number _etacalc(integer nelemL, integer nelemR, number val[]);
static number _etacor(integer side, number eta);




public:
static integer Out(integer);
number getVal(){return TRCLFFKEY.CommonGain[_Id.getside()]*(_Sum-TRCLFFKEY.CommonShift[_Id.getside()]);}
number getcofg( AMSTrIdGeom * id);
number getlcofg(AMSTrIdGeom * id);
number cfgCorFun(number s, AMSTrIdGeom * id);
number getecofg(){return _ErrorMean;}
number geteta(){return _Eta;}
//+

     integer getnelem()      {return -_NelemL+_NelemR;}
     number  getrms()        {return _Rms;}
     number  getsigma()      {return TRCLFFKEY.CommonGain[_Id.getside()]*_Sigma;}

inline void setid(const AMSTrIdSoft& id) { _Id = id;}
number getMean() { return _Mean;}
void   getValues(number* values)
       { if(getnelem() > 0) 
         UCOPY (_pValues, values, sizeof(number)*getnelem()/4); }
void   setValues(number* values)
  {
   if(getnelem()>0){
    _pValues=(number*)UPool.insert(getnelem()*sizeof(number));
    for(int i=0;i<getnelem();i++) _pValues[i] = values[i];
   }
  }
integer getSide() {return _Id.getside();}

AMSTrIdSoft getid()const {return _Id;}
AMSTrCluster(const AMSTrIdSoft& id, integer status, integer nelemL, 
integer nelemR, number sum,number ssum, number pos, number rms, number eta,
number val[]);
AMSTrCluster *  next(){return (AMSTrCluster*)_next;}

//default builder
static integer build(integer refit=0);
static integer buildWeak(integer refit=0);

static void print();
AMSTrCluster():AMSlink(){_NelemL=0; _NelemR=0;_pValues=0;};
~AMSTrCluster(){if(_pValues)UPool.udelete(_pValues);}
integer operator < (AMSlink & o) const {

 integer ithis =   (!checkstatus(AMSDBc::USED))
                 + 2*(checkstatus(AMSDBc::AwayTOF));
 integer iother =   (!o.checkstatus(AMSDBc::USED))
                  + 2*(o.checkstatus(AMSDBc::AwayTOF));

 if (ithis<iother)return 1;
 else return 0;

}
#ifdef __WRITEROOT__
 friend class TrClusterR;
#endif
};

class AMSTrRecHit: public AMSlink{
public:
protected:
AMSgSen * _pSen;
AMSTrCluster *_Xcl;
AMSTrCluster *_Ycl;
integer      _Layer;
AMSPoint     _Hit;
AMSPoint     _EHit;
number       _Sum;
number       _DifoSum;
number       _cofgx;
number       _cofgy;
AMSPoint     _Bfield;

static AMSTrRecHit* _Head[trconst::maxlay];
static void _addnext(AMSgSen * p, integer ,integer ,number, number, AMSTrCluster *, 
            AMSTrCluster *,  const AMSPoint &, const AMSPoint &);
  void _copyEl();
  void _writeEl();

public:
  void _printEl(ostream & stream){ stream << " Status " << _status << " Layer " << 
  _Layer <<" Coo " << _Hit<< endl;}
integer operator < (AMSlink & o) const {
  AMSTrRecHit * p= (AMSTrRecHit*)(&o);

  integer ithis =   (!checkstatus(AMSDBc::USED))
                  + 2*(checkstatus(AMSDBc::AwayTOF));
  integer iother =   (!p->checkstatus(AMSDBc::USED))
                   + 2*(p->checkstatus(AMSDBc::AwayTOF));

  if (ithis<iother)return 1;
  else if(ithis==iother){
   if(_Sum > p->_Sum)return 1;
   else return 0;
  }
  else return 0;



} 

inline integer Good() { 
  return ((TRFITFFKEY.FullReco!=0 || checkstatus(AMSDBc::USED)==0) 
            && checkstatus(AMSDBc::AwayTOF )==0) && checkstatus(AMSDBc::GOOD);
}
inline integer Good_PI() { 
  return ( (checkstatus(AMSDBc::AwayTOF)==0) && checkstatus(AMSDBc::GOOD));
}
static AMSTrRecHit* firstgood(integer pattern, integer index);
AMSTrRecHit* nextgood();
static AMSTrRecHit* firstgood_path(integer pattern, integer index, number par[2][3]);
AMSTrRecHit* nextgood_path(number par[2][3]);
static AMSTrRecHit* firstgood_WEAK(integer pattern, integer index);
AMSTrRecHit* nextgood_WEAK();
static AMSTrRecHit* firstgood_WEAK_path(integer pattern, integer index, number par[2][3]);
AMSTrRecHit* nextgood_WEAK_path(number par[2][3]);
static integer markAwayTOFHits();
integer is_in_path(number par[2][3]){
   return fabs(par[0][1]+par[0][0]*_Hit[2]-_Hit[0]) < TRFITFFKEY.SearchRegStrLine*par[0][2]
       && fabs(par[1][1]+par[1][0]*_Hit[2]-_Hit[1]) < TRFITFFKEY.SearchRegCircle*par[1][2];
}

static void _markDouble(vector<double>& , integer );

AMSTrRecHit *  next(){return (AMSTrRecHit*)_next;}

void resethash(integer i, AMSlink *head){
if(i>=0 && i <TKDBc::nlay())_Head[i]=(AMSTrRecHit*)head;
}


static AMSTrRecHit * gethead(integer i=0){
   if(i>=0 && i<TKDBc::nlay()){
    if(!_Head[i])_Head[i]=(AMSTrRecHit*)AMSEvent::gethead()->getheadC("AMSTrRecHit",i,1);
    return _Head[i];
   }
   else {
#ifdef __AMSDEBUG__
    cerr <<"AMSTrRecHit:gethead-S-Wrong Head "<<i;
#endif
    return 0;
   }   
}





AMSTrRecHit(AMSgSen *p, integer good,integer layer, number cofgx, number cofgy,AMSTrCluster * xcl, AMSTrCluster * ycl,
            const AMSPoint & hit, const AMSPoint & ehit, number sum, number dfs, const AMSPoint & bfield): AMSlink(good,0),
            _pSen(p), _Layer(layer),_Xcl(xcl),_cofgx(cofgx),_cofgy(cofgy),
            _Ycl(ycl), _Hit(hit), _EHit(ehit),_Sum(sum),_DifoSum(dfs),_Bfield(bfield){};
AMSTrRecHit(): AMSlink(),_pSen(0),_Xcl(0),_Ycl(0),_cofgx(0),_cofgy(0){};
static integer build(integer refit=0);
static integer buildWeak(integer refit=0);
static void print();
static integer Out(integer);
number getsum()const{return _Sum;}
number getsonly()const {return _Sum*(1-_DifoSum);}
number getkonly()const {return _Sum*(1+_DifoSum);}
AMSgSen * getpsen()const{return _pSen;}
inline  AMSPoint  getHit(){return _Hit;}
AMSPoint  getlocHit(){ return (_pSen->up())->gl2loc(_Hit);}
inline  AMSPoint  getEHit(){return _EHit;}
inline  AMSPoint  getBfield(){return _Bfield;}
integer       getLayer() const            {return _Layer;}
AMSTrCluster* getClusterP(integer n) const {
                                              if(n==0)return _Xcl;
                                              else if(n==1)return _Ycl;
                                              else return 0;}
//+
inline  number        getSum()               { return _Sum;}
inline  number        getDSum()              { return _DifoSum;}
void          setClusterP(AMSTrCluster* p,integer n) {
              if (n == 0) _Xcl = p;
              if (n == 1) _Ycl = p;
}
void          setSensorP(AMSgSen* p) { _pSen = p;}

//-
~AMSTrRecHit(){int i;for( i=0;i<trconst::maxlay;i++)_Head[i]=0;};
   friend class AMSTrTrack;
#ifdef __WRITEROOT__
   friend class TrRecHitR;
#endif
};

class AMSTrTrackError{
private:
 char msg[256];
public:
 AMSTrTrackError(char * name);
 char * getmessage();
};


class AMSTrTrack: public AMSlink{
protected:
AMSTrRecHit * _Pthit[trconst::maxlay];
uinteger _Address;
integer _Pattern;
integer _NHits;
integer _FastFitDone;
integer _GeaneFitDone;
integer _AdvancedFitDone;
AMSPoint _Hit[trconst::maxlay];
AMSPoint _EHit[trconst::maxlay];
number _Dbase[2];
number _Chi2StrLine;
number _Chi2WithoutMS;
number _RigidityWithoutMS;
number _Chi2FastFit;
number _Ridgidity;
number _ErrRidgidity;
number _Theta;
number _Phi;
AMSPoint _P0;
number _GChi2;
number _GRidgidity;    //  Ridgidity or momentum if (geanefitdone != 14)
number _GErrRidgidity; //  err of  see above
number _GTheta;
number _GPhi;
AMSPoint _GP0;
number _HChi2[2];
number _HRidgidity[2];
number _HErrRidgidity[2];
number _HTheta[2];
number _HPhi[2];
AMSPoint _HP0[2];
number _Chi2MS;
number _PIErrRigidity;
number _RidgidityMS;
number _PIRigidity;
number _PITheta;
number _PIPhi;
AMSPoint _PIP0;
number _PIChi2;

void SimpleFit(AMSPoint err);
void VerySimpleFit(AMSPoint err);
static void _Start(){TIMEX(_Time);time(&__Time);}
static geant _CheckTime(){geant tt1;TIMEX(tt1);return tt1-_Time;}
static int _CheckTimeE(){time_t tt1;time(&tt1);return tt1-__Time;}

static bool _NoMoreTime();
static geant _Time;
static time_t __Time;
static geant _TimeLimit;
  void _printEl(ostream & stream){ stream << " Pattern " << _Pattern << " Rigidity (no MS)" << 
  _RigidityWithoutMS <<" Rigidity (Fast) "<<_Ridgidity <<" Chi2Fast " << 
  _Chi2FastFit << " ThetaFast "<<_Theta<<" PhiFast "<<_Phi<<endl;}
  void _copyEl();
  void _writeEl();
static integer _TrSearcher(int icall);
static integer _TrSearcherFalseX(int icall);
static integer _TrSearcherFalseTOFX(int icall);
static integer pat;
static AMSTrRecHit * phit[trconst::maxlay];
static number par[2][3];
static integer _addnext(integer pat, integer nhits, AMSTrRecHit* phit[]);
static void   _addnextR(AMSTrTrack* ptr, integer pat, integer nhits, AMSTrRecHit* phit[]);
static integer _addnextFalseX(integer pat, integer nhits, AMSTrRecHit* phit[]);
static integer Distance(number par[2][3], AMSTrRecHit *ptr){
   return fabs(par[0][1]+par[0][0]*ptr->getHit()[2]-ptr->getHit()[0]) > TRFITFFKEY.SearchRegStrLine*par[0][2] ||
          fabs(par[1][1]+par[1][0]*ptr->getHit()[2]-ptr->getHit()[1])
           > TRFITFFKEY.SearchRegCircle*par[1][2];

}
static integer DistanceTOF(number par[2][3], AMSTrRecHit *ptr);
static integer _RefitIsNeeded;
void _crHit();
inline  AMSPoint  getHit(int i, int dir=0){return _Hit[dir==0?i:_NHits-1-i];}
inline  AMSPoint  getEHit(int i){return _EHit[i];}
void _buildaddress();
static void decodeaddress(integer ladder[2][trconst::maxlay], uinteger address);
static uinteger  encodeaddress(integer lad[2][trconst::maxlay]);
static uinteger * getchild(uinteger address, uinteger &nchild);
public:
  static geant & TimeLimit(){return _TimeLimit;}
  integer intercept(AMSPoint &P1, integer layer, number &theta, number &phi, number &local, integer icase=0);
static integer & RefitIsNeeded(){return _RefitIsNeeded;}
bool GenuineTrack(){return _Pattern>=0;}
integer operator < (AMSlink & o) const {
  AMSTrTrack * p= (AMSTrTrack*)(&o);

// Check if one of them is USED
  bool used= (bool)checkstatus(AMSDBc::USED);
  bool used_other= (bool)(p->checkstatus(AMSDBc::USED));
  if (used && (!used_other)) return 1;
  if ((!used) && used_other) return 0;

// Check if one of them is a FalseTOFX track
  bool falsetofx= (bool)checkstatus(AMSDBc::FalseTOFX);
  bool falsetofx_other= (bool)(p->checkstatus(AMSDBc::FalseTOFX));
  if (falsetofx && (!falsetofx_other)) return 0;
  if ((!falsetofx) && falsetofx_other) return 1;

// Check if one of them has more hits than the other
   if (_Pattern != p->_Pattern) return (_Pattern<p->_Pattern);

// If same number of hits then check the Chi2
   return (_Chi2FastFit < p->getchi2());
}
  friend class AMSTrAligFit;
  friend class AMSTrAligData;
  friend class AMSTrAligPar;
  friend class TOF2User;

uinteger getaddress(){return _Address;}
void   AdvancedFit();
integer getpattern()const{return _Pattern;}
AMSTrTrack(integer nht, AMSTrRecHit * pht[], int FFD, int GFD, number chi2FF, number rigFF, number erigFF, number thetaFF, number phiFF, AMSPoint P0FF, number chi2G, number rigG, number erigG, number thetag, number phig, AMSPoint p0g, number chi2MS, number jchi2MS, number rigFMS, number grigms);


static integer Out(integer);
number Fit(integer i=0, integer ipart=14);
void SetPar(number rig, number theta, number phi, AMSPoint P0,integer icase=0){
 if(icase==0){_Ridgidity=rig;_Theta=theta;_Phi=phi;_P0=P0;}
 else if(icase==1){_GRidgidity=rig;_GTheta=theta;_GPhi=phi;_GP0=P0;}
 else {_PIRigidity=rig;_PITheta=theta;_PIPhi=phi;_PIP0=P0;}
}
~AMSTrTrack(){};
AMSTrTrack *  next(){return (AMSTrTrack*)_next;}
AMSTrTrack (integer pattern, integer nhits, AMSTrRecHit * phit[]): 
AMSlink(0,0),_Pattern(pattern), _NHits(nhits),_GeaneFitDone(0), _AdvancedFitDone(0),_FastFitDone(0)
  {init(  phit);}
AMSTrTrack(AMSDir dir, AMSPoint point,number rig=10000000,number errig=10000000);
AMSTrTrack(number theta, number phi, AMSPoint point);
void init( AMSTrRecHit * phit[]);
static integer build(integer refit=0);
static integer buildPathIntegral(integer refit=0);
static integer buildWeak(integer refit=0);
static integer buildWeakPathIntegral(integer refit=0);
static integer buildFalseX(integer refit=0);
static integer makeFalseTOFXHits();
static integer buildFalseTOFX(integer refit=0);
static integer _MarginPatternsNeeded;
static integer _max_ambigous_hits;
static void setMargin(int margin){_MarginPatternsNeeded= margin>0?1:0;}
static void print();
AMSTrRecHit * getphit(integer i){return i>=0 && i<trconst::maxlay? _Pthit[i]:0;}
void SimpleFit();
void interpolate(AMSPoint  pnt, AMSDir  dir,  AMSPoint & P1, 
                 number & theta, number & phi, number & length, int icase=0);
void interpolateCyl(AMSPoint  pnt,  AMSDir dir, number rad, number idir, 
                    AMSPoint & P1, number & theta, number & phi, 
                    number & length);
void getParFastFit(number&  Chi2, number& Rid, number&  Err, 
number&  Theta, number & Phi, AMSPoint&  X0)const ; 

void getParSimpleFit(number & Chi2xy, number &Chi2sz, number & Rid)const;
integer AdvancedFitDone()const{return _AdvancedFitDone;}
integer GeaneFitDone()const{return _GeaneFitDone;}
bool FastFitDone()const{return _FastFitDone!=0;}
void getParAdvancedFit(number&   GChi2, number&  GRid, number&  GErr,
number&  GTheta, number&  GPhi, AMSPoint&  GP0,
number HChi2[2], number HRid[2], number HErr[2], number HTheta[2], 
number HPhi[2], AMSPoint  HP0[2] ) const;

AMSTrTrack() {_Pattern = -1; _NHits   = -1; 
   for (int i=0; i<trconst::maxlay; i++) _Pthit[i] = NULL; }
void   setHitP(AMSTrRecHit* p, integer n) {if (n< trconst::maxlay)  _Pthit[n] = p;}
//-
integer TOFOK();
integer getnhits() const {return _NHits;}
number getgrid() const {return _GRidgidity;}
number getpirid() const {return _PIRigidity;}
number getepirid() const {return _PIErrRigidity;}
number getegrid() const {return _GErrRidgidity;}
number geterid() const {return _ErrRidgidity;}
number getrid(int method=0) const {return method==0?_Ridgidity:(method==1?_GRidgidity:_PIRigidity);}
number getchi2()const {return _Chi2FastFit;}
number gettheta(int icase=0) const {return (icase==0?_Theta:(icase==1?_GTheta:_PITheta));}
number getphi(int icase=0) const {return (icase==0?_Phi:(icase==1?_GPhi:_PIPhi));}
number getpichi2() const {return _PIChi2;}
number getchi2StrLine() const {return _Chi2StrLine;}
number getchi2withoutMS() const {return _Chi2WithoutMS;}
AMSPoint getpiP0() const {return _PIP0;}
AMSTrTrack* CloneIt();
static void remove_track(AMSTrTrack* &ptrack);
integer next_combination(int index_min, int index_max, number par[2][3]);
integer next_combination_WEAK(int index_min, int index_max, number par[2][3]);
static void flag_golden_tracks();
friend class AMSVtx;
friend class AMSTrCalibFit;
#ifdef __WRITEROOT__
 friend class TrTrackR;
#endif
};

#endif
