//  $Id: trid.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
//
// Last edit : Mar 19, 1997. ak. add AMSTrIdSoft::getidgeom() function 
//
#ifndef __AMSTRID__
#define __AMSTRID__
#include "typedefs.h"
#include "amsdbc.h"
#include <iostream.h>
#include "amsstl.h"
//#include "commons.h"
#include "link.h"
#include "job.h"
#include "tkdbc.h"

namespace trid{
  const integer ms=4000;
  const int ncrt=8;
  const int ntdr=32;
}

class AMSTrIdGeom{
  integer _layer;    // from 1 to TKDBc::nlay()
  integer _ladder;   // from 1 to TKDBc::nlad(_layer)
  integer _sensor;   // from 1 to TKDBc::nsen(_layer,_ladder)
  integer _stripx;   // from 0 to TKDBc::NStripsSen(_layer,0)-1
  integer _stripy;   // from 0 to TKDBc::NStripsSen(_layer,1)-1
  void _check();
  integer _R2Gx(integer stripx) const;
  integer _R2Gy(integer stripy) const;
  public:
  friend ostream &operator << (ostream &o, const  AMSTrIdGeom &b )
  {return o<<" lay "<<b._layer<<" lad "<<b._ladder<<" sensor "<<b._sensor<<" strip(x,y) "<<b._stripx<<" "<<b._stripy<<endl;}
  friend class AMSTrIdSoft;
  AMSTrIdGeom():_layer(1),_ladder(1),_sensor(1),_stripx(0),_stripy(0){};
  ~AMSTrIdGeom(){};
  AMSTrIdGeom( const AMSTrIdGeom& o):_layer(o._layer),_ladder(o._ladder),
				     _sensor(o._sensor),_stripx(o._stripx),_stripy(o._stripy){}
  AMSTrIdGeom(integer layer,integer ladder,integer sensor, integer stripx,
      integer stripy);
  AMSTrIdGeom(integer idsoft, integer stripx=0, integer stripy=0);
  AMSTrIdGeom & operator = (const AMSTrIdGeom &o);
  AMSID crgid() const;
  integer FindAtt(const AMSPoint & pnt,  AMSPoint  size);
  integer operator == (const AMSTrIdGeom &o);
  void R2Gx(integer stripx);
  void R2Gy(integer stripy);
  void R2G(const AMSTrIdSoft &id);
  number  getsize(integer side)const;
  number  getcofg(integer side, integer shift, integer readoutch, integer & error)const;
  inline void upd(integer side,integer strip){
#ifdef __AMSDEBUG__
    assert(side>=0 && side<2);
    assert (strip>=0 && strip <getmaxstrips(side));
#endif
    if(side==0)_stripx=strip;
    else _stripy=strip;
  }
  inline integer getlayer() const {return _layer;}
  inline integer getladder()const {return _ladder;}
  inline integer gethalf()const {return _sensor<=TKDBc::nhalf(_layer,_ladder)?0:1;}
  inline integer getsensor()const {return _sensor;}        
  inline integer cmpt()const {return _layer+10*_ladder+1000*_sensor;}
  inline integer getstrip (integer i) const{if(i==0)return _stripx;else return _stripy;} 
  inline number getmaxsize(integer i)  {return TKDBc::ssize_active(_layer-1,i);}
  inline integer getmaxstrips(integer i) const
  {return AMSJob::gethead()->isRealData()? TKDBc::NStripsSenR(_layer,i):TKDBc::NStripsSen(_layer,i);}
  integer size2strip(integer side, number size);


  inline number strip2size(integer side) const{
#ifdef __AMSDEBUG__
    assert(side>=0 && side<2 && !AMSJob::gethead()->isRealData());
#endif
    if(side==0)return _swxyl[_layer-1][0][_stripx];
    else       return _swxyl[_layer-1][1][_stripy];
  }

  static integer debug;
  static number *  _swxy[trconst::maxlay][2] ;  // strip size x,y
  static number *  _swxyl[trconst::maxlay][2];   // integral of strip size
  //
  // we have to have yet different apporiach to real data
  //
  static number *  _swxyR[trconst::maxlay][2] ;  // strip size x,y
  static number *  _swxyRl[trconst::maxlay][2];   // integral of strip size

  static void init();
};
class AMSTrIdSoft{
  protected:
    integer _dead;    // dead   if 1  ; alive otherwise
    integer _layer;   // from 1 to TKDBc::nlay()
    integer _drp;     // from 1 to TKDBc::nlad(_layer)
    integer _side;    // 0=x 1=y
    integer _half;    // from 0 to 1
    integer _strip;   // from 0 to getmaxstrips()-1
    integer _addr;    // Calculated from the above
    int16u _haddr;    // Hardware address
    int16u _crate;    // Crate no
    integer _VANumber; // from 0 to 5 (9)
    AMSTrIdGeom * _pid;
    static uinteger _ncrates;
    static char *   _TSig[2];
    static char *   _TRSig[2];
    static char *   _TPed[2];
    static char *   _TGa[2];
    static char *   _TSt[2];
    static char *   _TRM[2];
    static char *   _TCm;
    static uinteger _CrateNo[trid::ncrt];
    static geant laser[trconst::maxlay][2];
    static integer idsoft2linear[trid::ms];
    static integer *status;
    static geant *peds;
    static geant *gains;
    static geant *sigmas;
    static geant *sigmaraws;
    static geant cmnnoise[10][trid::ms];
    static geant *indnoise;
    static uinteger * rhomatrix;
    static integer _numel;
    static integer _numell;
    const static integer _VAChannels;
    void _check();
    void _mkhaddr();
    void _mkcrate();
    static integer _GetGeo[trid::ncrt][trid::ntdr][2][3];   // crate,tdrs,side ->
    //layer,ladder,half
    static integer _GetHard[trconst::maxlay][trconst::maxlad][2][3];     // layer,ladder,half ->
    // tdrs(0&1), crate


  public:
    friend ostream &operator << (ostream &o, const  AMSTrIdSoft &b )
    {return o<<" lay "<<b._layer<<" lad "<<b._drp<<" side "<<b._side<<" half "<<b._half<<" "<<b._strip<<" "<<b._dead<<endl;}
    static void init();
    static void inittable(int setup);
    integer operator < (const AMSTrIdSoft & o)const;
    static uinteger ncrates() {return _ncrates;}
    static uinteger CrateNo(int i){return i>=0 && i<_ncrates? _CrateNo[i]:0;}
    static uinteger ndrp() {return _ncrates*trid::ntdr;}
    static inline integer getnchan() {return _numel;}
    inline integer dead() const {return _dead==1;}
    integer match(AMSTrIdGeom * pid);
    void kill(); 
    static char * TrackerSigmas(int i){return i>=0 && i<2?_TSig[i]:0;}
    static char * TrackerPedestals(int i){return i>=0 && i<2?_TPed[i]:0;}
    static char * TrackerStatus(int i){return i>=0 && i<2?_TSt[i]:0;}
    static char * TrackerGains(int i){return i>=0 && i<2?_TGa[i]:0;}
    static char * TrackerRhoMatrix(int i){return i>=0 && i<2?_TRM[i]:0;}
    static char * TrackerRawSigmas(int i){return i>=0 && i<2?_TRSig[i]:0;}
    static char * TrackerCmnNoise(){return _TCm;}
    inline int16u gethaddr()const { return _haddr;}
    inline int16u getcrate() const {return _crate;}
    inline integer gettdr() const {return (_haddr>>10)&31;}
    inline integer getaddr() const {return _addr;}
    inline integer getchannel() const { return idsoft2linear[_addr]+_strip;}
    inline integer checkstatus(integer checker) const 
    {return status[idsoft2linear[_addr]+_strip] & checker;}
    inline geant getped() const {return peds[idsoft2linear[_addr]+_strip];}
    inline integer getavgain() const {return 8;}
    inline geant getsig() const {return sigmas[idsoft2linear[_addr]+_strip];}
    inline geant getsigraw() const {return sigmaraws[idsoft2linear[_addr]+_strip];}
    inline geant getsignsigraw() const {return sigmaraws[idsoft2linear[_addr]+_strip]>0?1:0;}
    inline geant getgain() const {return gains[idsoft2linear[_addr]+_strip];}
    inline geant getlaser() const {return laser[_layer-1][_side];}
    inline geant getindnoise() const{return indnoise[idsoft2linear[_addr]+_strip];}
    integer getprob(geant r);
    virtual  geant getcmnnoise() const {return cmnnoise[_VANumber][_addr];}
    void  setrhomatrix(integer bit);
    void  clearrhomatrix(integer bit);
    integer  getrhomatrix(integer bit);
    geant & setped()  {return peds[idsoft2linear[_addr]+_strip];}
    geant & setsig()  {return sigmas[idsoft2linear[_addr]+_strip];}
    geant & setsigraw()  {return sigmaraws[idsoft2linear[_addr]+_strip];}
    geant & setgain() {return gains[idsoft2linear[_addr]+_strip];}
    geant & setindnoise() {return indnoise[idsoft2linear[_addr]+_strip];}
    geant & setcmnnoise()  {return cmnnoise[_VANumber][_addr];}
    void  setstatus(integer changer)  
    {status[idsoft2linear[_addr]+_strip]=status[idsoft2linear[_addr]+_strip] | changer;}
    void  clearstatus(integer changer)  
    {status[idsoft2linear[_addr]+_strip]=status[idsoft2linear[_addr]+_strip] & ~changer;}
    friend class AMSTrIdGeom;
    friend class AMSJob;
    AMSTrIdSoft(int16u crate, int16u haddr);
    AMSTrIdSoft(const AMSTrIdGeom &,integer side);
    AMSTrIdSoft():_pid(0),_dead(0){};
    AMSTrIdSoft( const AMSTrIdSoft& o):_layer(o._layer),_drp(o._drp),
				       _half(o._half),_side(o._side),_strip(o._strip),_addr(o._addr),
				       _VANumber(o._VANumber),_pid(0),_dead(o._dead),_haddr(o._haddr),_crate(o._crate){}
    AMSTrIdSoft(integer idsoft);
    AMSTrIdSoft(integer layer,integer drp,integer half,integer side,integer strip);
    AMSTrIdSoft(int16u crate,int16u tdrs, int16u side, int16u strip);
    AMSTrIdSoft & operator = (const AMSTrIdSoft &o);
    integer operator == (const AMSTrIdSoft &o);
    inline integer cmpt() const {return _layer+10*_drp+1000*_half+2000*_side+10000*_strip;}
    inline integer getmaxstrips(){return TKDBc::NStripsDrp(_layer,_side);}
    inline integer teststrip(integer strip){
      return strip>=0 && strip <getmaxstrips();
    }
    inline void upd(integer strip){
#ifdef __AMSDEBUG__
      if(_side==1)assert (strip>=0 && strip <getmaxstrips());
      else assert (strip>=-TRCLFFKEY.ThrClNEl[_side]/2 
	  && strip <getmaxstrips()+TRCLFFKEY.ThrClNEl[_side]/2);
#endif
      if(strip >=0 && strip < getmaxstrips())_strip=strip;
      else if(strip < 0)_strip=getmaxstrips()+strip;
      else              _strip=strip-getmaxstrips();
      _VANumber=_strip/_VAChannels;
      _mkhaddr();
    }
    inline integer getlayer() const {return _layer;}
    inline integer getdrp() const {return _drp;}
    inline integer getside()const {return _side;}        
    inline integer gethalf()const {return _half;}        
    inline integer getstrip()const {return _strip;}
    inline integer getstripa()const {return _strip%_VAChannels;}
    inline integer getva()const {return _side==0? _VANumber+10:_VANumber;}
    AMSTrIdGeom *  getidgeom()     {return _pid;}
    AMSTrIdGeom * ambig(const AMSTrIdSoft &, integer & nambig) ;

    static integer debug;
};
class AMSTrIdSoftI{
  public:
    AMSTrIdSoftI();
  private:
    static integer _Count;
};
static AMSTrIdSoftI rI;






#endif

