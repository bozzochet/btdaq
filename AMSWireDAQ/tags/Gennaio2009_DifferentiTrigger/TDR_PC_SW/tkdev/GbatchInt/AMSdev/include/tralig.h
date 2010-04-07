//  $Id: tralig.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 13-nov-1998

#ifndef __AMSTRALIG__
#define __AMSTRALIG__
#include "typedefs.h"
#include  "commons.h"
#include "link.h"
#include "point.h"
#include "particle.h"
#include "trid.h"
#include "mceventg.h"
using namespace amsprotected;
class AMSTrAligDB;
class AMSTrAligPar{
public:
class AMSTrAligDBEntry{
public:
 uinteger address;
 uinteger status;  // 0 if implicit
                   // 1++ if explicit
 
 geant coo[6][3];
 geant angle[6][3];
 geant chi2b; // before
 geant chi2a; // after
 geant pav;       // rel pmom
 geant pav2;      // pmom sigma 
 int operator < (const AMSTrAligDBEntry&o) const{ return address<o.address;}
 int operator == (const AMSTrAligDBEntry&o) const{ return address==o.address;}
 AMSTrAligDBEntry(uinteger _address=0):address(_address){};
 AMSTrAligDBEntry(uinteger _address,  uinteger _status, AMSTrAligPar o[6], number _fcnb, number _fcna, number _pav, number _pav2):address(_address),status(_status),chi2b(_fcnb), chi2a(_fcna),pav(_pav),pav2(_pav2){
  for(int i=0;i<6;i++){
   for(int k=0;k<3;k++){
    coo[i][k]=o[i].getcoo()[k];
    angle[i][k]=o[i].getang()[k];
   }
  }
}
};

class AMSTrAligDB{
 public:
 uinteger Nentries;
 AMSTrAligPar::AMSTrAligDBEntry arr[15000];
AMSTrAligDB():Nentries(0){};
};

protected:
integer  _NEntries;
AMSPoint _Coo;
AMSPoint _Angles;
AMSDir _Dir[3];
void _a2m();
void _m2a();
static AMSTrAligDB _traldb;
static AMSTrAligPar par[8];
public:
AMSTrAligPar():_Coo(0,0,0),_Angles(0,0,0),_NEntries(0){};
AMSTrAligPar(const AMSPoint & coo, const AMSPoint & angles);
AMSTrAligPar(const AMSTrAligDBEntry * ptr, integer i);
AMSPoint  getcoo()const {return _Coo;}
AMSPoint  getang()const {return _Angles;}
void setcoo(const AMSPoint & o) {_Coo=o;}
void setpar(const AMSPoint & coo, const AMSPoint & angle);
integer& NEntries(){return _NEntries;}
integer      AddOne();
void         MinusOne();
AMSDir   getmtx(integer i){assert(i>=0 && i<3);return _Dir[i];}
AMSDir &  setmtx(integer i){assert(i>=0 && i<3);return _Dir[i];}
void updmtx(){_a2m();}
void updangles(){_m2a();}
static AMSID getTDVDB();
AMSTrAligPar  operator +(const AMSTrAligPar &o){
return AMSTrAligPar(_Coo+o._Coo,_Angles+o._Angles);
}         
AMSTrAligPar  operator -(const AMSTrAligPar &o){
return AMSTrAligPar(_Coo-o._Coo,_Angles-o._Angles);
}         
AMSTrAligPar  operator *(const AMSTrAligPar &o){
return AMSTrAligPar(_Coo*o._Coo,_Angles*o._Angles);
}         
AMSTrAligPar  operator /(number o){
return AMSTrAligPar(_Coo/o,_Angles/o);
}
static integer  getdbentries(){return  _traldb.Nentries;}
static void  incdbentries(){  _traldb.Nentries++;}
static integer maxdbentries(){return 15000;}
static AMSTrAligDBEntry * getdbtopp(){return _traldb.arr;}
static AMSTrAligPar * getparp(){return par;}
static void InitDB(){_traldb.Nentries=0;}
static AMSTrAligPar * SearchDB(uinteger address, integer & found, number DB[2]);
static void UpdateDB(uinteger address,  AMSTrAligPar o[], number _fcnb, number _fcna, number _pav, number _pav2  );
static void LockDB();
static void UnlockDB();
static void _lockunlock(integer lock);
static integer DbIsNotLocked(integer sleep=2);
static AMSTrAligDB * gettraligdbp(){ return & _traldb;}
static integer gettraligdbsize(){ return sizeof(_traldb);}




friend ostream &operator << (ostream &o, const  AMSTrAligPar &b )
  {return o<<" "<<b._Coo<<" "<<b._Angles<<" "<<b._Dir[0]<<" "<<b._Dir[1]<<" "<<b._Dir[2];}
};

class AMSTrAligDB{
 public:
 uinteger Nentries;
 AMSTrAligPar::AMSTrAligDBEntry arr[10000];
AMSTrAligDB():Nentries(0){};
friend class  AMSTrAligPar;
};



class AMSTrAligData{
protected:
  integer _NHits;
  integer _Pattern;
  uinteger _Address;
  AMSPoint * _Hits;
  AMSPoint * _EHits;
  integer _Pid;
  geant _InvRigidity;
  geant _ErrInvRigidity;
  
public:
AMSTrAligData():_NHits(0),_Hits(0),_EHits(0),_Pid(0),
_InvRigidity(0),_ErrInvRigidity(0), _Pattern(0), _Address(0){};
void Init(AMSParticle * ptr, AMSmceventg * pgen);
friend class AMSTrAligFit;
~AMSTrAligData(){ delete [] _Hits; delete[] _EHits;}
};


class AMSTrAligFit: public AMSNode{



class TrAlig_def{
public:
integer Pattern;
integer Alg;
integer Address;
geant FCN;
geant FCNI;
geant Pfit;
geant Pfitsig;
geant Coo[8][3];
geant Angle[8][3];
};
class gldb_def{
public:
 integer nentries;
 geant coo[3];
 geant ang[3];
};
class TrAligg_def{
public:
integer Alg;
int Layer;
int Ladder;
int Side;
geant FCN;
geant FCNI;
geant CHI2[1000];
geant CHI2I[1000];
int ndata;
geant Pfit;
geant Pfitsig;
geant Coo[3];
geant Angle[3];
integer Stat;
};
protected:
uinteger _Address;
integer  _Pattern; 
integer* _PlaneNo;
integer* _ParNo;
integer* _LadderNo;
integer* _HalfNo;
integer _NoActivePar;
integer _PositionData;
integer _NData;
integer _Algorithm;
AMSTrAligData * _pData;
integer _flag;    // 
number _tmp;
number _tmppav;
number _tmppsi;
number _fcn;   // pointer to fcns;
number _fcnI;   // pointer to fcns;
number _pfit;  //pointer to fitterd mom
number _pfitbefore;  //pointer to fitterd mom before
number _pfits;  //pointer to fitterd mom sigma
number chi2[1000];
number chi2i[1000];
AMSTrAligPar _pParC[8];
static AMSTrAligPar _pPargl[17][2][8];
static gldb_def _gldb[trconst::maxlad][2][8];
static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall)
{};
static void alfun(integer & n, number xc[], number & fc, AMSTrAligFit * ptr);
static void alfungl(integer & n, number xc[], number & fc, AMSTrAligFit * ptr);
static void UpdateDBgl();
void _init(){};
public:
  AMSTrAligFit *  next(){return (AMSTrAligFit*)_next;}           
AMSTrAligFit();
AMSTrAligFit(uinteger _Address, integer pattern, integer data, integer alg, integer nodeno);
static integer glDBOK(uinteger add);
static gldb_def * gettraliggldbp(){ return &(_gldb[0][0][0]);}
static integer gettraliggldbsize(){return sizeof(_gldb);}
static void InitDB();
static void Test(int i=0);
static void Testgl(int i=0);
static AMSID getTDVGLDB();
static integer Select(AMSParticle * & ptr, AMSmceventg * & mcg, integer alg);
integer AddressOK(uinteger address, integer strict=0);
void Fit();
void Fitgl();
void RebuildNoActivePar();
void Anal();
void Analgl();

uinteger getaddress(){ return _Address;}
~AMSTrAligFit();

};





#endif
