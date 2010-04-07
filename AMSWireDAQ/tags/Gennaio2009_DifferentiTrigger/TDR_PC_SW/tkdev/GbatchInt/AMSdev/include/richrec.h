//  $Id: richrec.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

#ifndef __RICHREC__
#define __RICHREC__
#include "richdbc.h"
#include <iostream>
#include "richid.h"


PROTOCCALLSFSUB6(SOLVE,solve,DOUBLE,DOUBLE,DOUBLE,DOUBLE,DOUBLEV,INT)
#define SOLVE(A1,A2,A3,A4,A5,A6) CCALLSFSUB6(SOLVE,solve,DOUBLE,DOUBLE,DOUBLE,DOUBLE,DOUBLEV,INT,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFFUN1(FLOAT,PROBKL,probkl,FLOAT)
#define PROBKL(A) CCALLSFFUN1(PROBKL,probkl,FLOAT,A) 
//+LIP
PROTOCCALLSFSUB1(RICHRECLIP,richreclip,INT)
#define RICHRECLIP(I1) CCALLSFSUB1(RICHRECLIP,richreclip,INT,I1)
//END

#include<string.h>

     /*
// First a simple template to use safe arrays. The value of should
// be always greater than 0

template<class T,int S>
class safe_array{
private:
  T *_origin;
  T _org[S];
  int _size;
public:
  T error;
  safe_array(int size=0){
    if(size<=0) {_size=-S;_origin=_org;} else {
    _origin=new T[size];
    _size=size;}
  }
  ~safe_array(){
    if(_origin!=_org && _size>0){
      delete []_origin;
    }
  };

  T& operator [](int pos){
    if(pos>=abs(_size) || pos<0) {
      cout << "RICH Safe array overflow. Request: "<<pos<<" of "<<abs(_size)<<endl;
      return error;
    }
    return *(_origin+pos);
  }
  int size(){return abs(_size);}
};

typedef safe_array<geant,3> geant_small_array;
typedef safe_array<integer,3> integer_small_array;
     */

/////////////////////////////////////////////
//         Container for hits              //
/////////////////////////////////////////////


class AMSRichRawEvent: public AMSlink{
private:
  integer _channel; // (PMT number-1)*16+window number
  integer _counts;  // 0 means true, 1 means noise
  geant   _beta_hit[3];  // Reconstructed betas
	 
public:
  AMSRichRawEvent(integer channel,integer counts,uinteger status=0):AMSlink(status),
    _channel(channel),_counts(counts){};
  ~AMSRichRawEvent(){};
  AMSRichRawEvent * next(){return (AMSRichRawEvent*)_next;}

  integer getchannel() const {return _channel;}
  inline geant getpos(integer i){
    AMSRICHIdGeom channel(_channel);
    return i<=0?channel.x():channel.y();
  }

  integer getcounts() {return _counts;}
  geant getnpe(){ 
    AMSRICHIdSoft calibration(_channel);
    return _counts/calibration.getgain(_status&gain_mode?1:0);
  }

  static void mc_build();
  static void build();
  void reconstruct(AMSPoint,AMSPoint,AMSDir,AMSDir,geant,
		   geant,
                   geant *,geant,int);

  integer reflexo(AMSPoint origin,AMSPoint *ref_point);
  geant inline dist(AMSPoint punto,AMSPoint plane){
    return -(punto[0]*plane[0]+punto[1]*plane[1]+punto[2]*plane[2]);
  }
  AMSPoint inline rpoint(AMSPoint ri,AMSPoint rf,AMSPoint plane){
    geant z1=-dist(ri,plane),z2=-dist(rf,plane);
    AMSPoint output((-2*plane[0]*z1*z2+z2*ri[0]+z1*rf[0])/(z1+z2),
		    (-2*plane[1]*z1*z2+z2*ri[1]+z1*rf[1])/(z1+z2),
		    (-2*plane[2]*z1*z2+z2*ri[2]+z1*rf[2])/(z1+z2));
    return output;		    
  }

  void inline setbit(int bit_number){_status|=1<<bit_number;}
  void inline unsetbit(int bit_number){_status&=~(1<<bit_number);}
  integer inline getbit(int bit_number){return (_status&(1<<bit_number))>>bit_number;}


  // Get betas from last call to reconstruct
  inline geant getbeta(integer i){
    if(i<=0) return _beta_hit[0];
    if(i>=2) return _beta_hit[2];
    return _beta_hit[1];
  } 
  
  static int _npart;
  static int Npart();
  static double RichRandom();
  static void Select(int howmany,int size,int lista[]);
  


// interface with DAQ :

protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSRichRawEvent: id="<<_channel<<endl;
    stream <<" Adc="<<_counts<<endl;
    stream <<" Status="<<_status<<endl;
  }
  void _writeEl();
  void _copyEl(){};
#ifdef __WRITEROOT__
friend class RichHitR;
#endif
};



// Container for rings. Each ring is a reconstructed track

class AMSRichRing: public AMSlink{

#ifdef __WRITEROOT__
friend class AMSTrTrack;
#endif
private:

static void _Start(){TIMEX(_Time);}
static geant _CheckTime(){geant tt1;TIMEX(tt1);return tt1-_Time;}
static bool _NoMoreTime(){return _CheckTime()>AMSFFKEY.CpuLimit;}
static geant _Time;

  AMSTrTrack* _ptrack;
  integer _used;        // number of hits used
  integer _mused;       // number of mirrored hits used
  number  _beta;        // ring beta
  number  _wbeta;       // weighted beta using the number of p.e. 
  number  _beta_blind;  // Unused: in the future reconstruction without using the track direction
  number  _errorbeta;   // Estimated error in beta
  number  _quality;     // Chi2  
  number  _npexp;       // Number of expected photons for Z=1
  number  _collected_npe;  // Number of collected photoelectrons: the rich charge is estimated as sqrt(npexp/collected_npe)
  number  _probkl;      // Kolgomorov test probability of the reconstructed ring azimuthal distribution being correct
  number  _kdist;       // Leibler-Kullback distance between the reconstructed ring and the hits for the azimuthal marginal distribution
  number _phi_spread;   //(\sum_i (phi_i-\phi_0)^2)/N_{hits} for used hits
  number _unused_dist;   //(\sum_i 1/\dist_i^2) for unused hits which do not belong to PMTs crossed by a charged particle

  number _theta;
  number _errortheta;
  number _radpos[3];
  number _pmtpos[3];


  number  _npexpg;       // Number of expected photons for Z=1
  number  _npexpr;       // Number of expected photons for Z=1
  number  _npexpb;       // Number of expected photons for Z=1

  //+LIP
  // variables
  integer _liphused;     //nr of hits used=10000 + nr hits mirror
  number  _lipthc;       // rec. Cerenkov angle
  number  _lipbeta;      // rec. beta
  number  _lipebeta;     // error on rec. beta
  number  _liplikep;     // likelihood prob.
  number  _lipchi2;      // chi2 of the fit
  number  _liprprob;     // ring prob.

  static integer _lipdummy;
  //ENDofLIP
  // All these guys can be obtained asking to the class RichRadiatorTile

  static number _index;  // refractive index used in the recontruction
                  // intended for using with two radiators
  static number _height; // Radiator height
  static AMSPoint _entrance_p;  // Entrance point in the radiator 
  static AMSDir   _entrance_d;  // Entrance direction

  static AMSPoint _emission_p;
  static AMSDir   _emission_d;

  // Those guys necessary to compute the number of 
  // expected photons for Z=1
  static geant   _clarity;
  static geant *_abs_len;
  static geant *_index_tbl;
  static int _kind_of_tile;
  

  // ALl the routines from Elisa

  geant trace(AMSPoint r, AMSDir u, 
	      geant phi, geant *xb, geant *yb, 
	      geant *lentr, geant *lfoil, 
	      geant *lguide, geant *geff, 
	      geant *reff, geant *beff, 
              integer *tflag,float beta_gen=0.9999);
  int tile(AMSPoint r);
  float generated(geant length,
		  geant lfoil,
		  geant lguide,
		  geant *fg,
		  geant *fr,
		  geant *fb);
  void refract(geant r1,
	       geant r2, 
	       float *u,
	       geant *v);
  geant lgeff(AMSPoint r, 
	      float u[3],
	      geant *lguide);
  int locsmpl(int id,
	      int *iw, 
	      geant *u, 
	      geant *v);
  


// A parametrisation
  static inline geant Sigma(geant beta,geant A,geant B){
      return beta*(B*beta+A)*1.e-2;
    }

  static inline integer closest(geant beta,
                                geant *betas){

    geant a=fabs(beta-betas[0]);
    geant b=fabs(beta-betas[1]);
    geant c=fabs(beta-betas[2]);
    if(b<c) {if(a<b) return 0; else return 1;}
    else    {if(a<c) return 0; else return 2;}
  }

protected:
  void _printEl(ostream &stream){stream<<" Beta "<<_beta<<" Error "<<_errorbeta<<endl;}
  void _writeEl();
  void _copyEl();
  void CalcBetaError();
  void ReconRingNpexp(geant window_size=3.,int cleanup=0);
public:
  //+LIP
  AMSRichRing(AMSTrTrack* track,int used,int mused,geant beta,geant quality,geant wbeta,int liphused, geant lipthc, geant lipbeta,geant lipebeta, geant liplikep,geant lipchi2, geant liprprob,uinteger status=0,integer build_charge=0):AMSlink(status),
    _ptrack(track),_used(used),_mused(mused),_beta(beta),_quality(quality),_wbeta(wbeta),_liphused(liphused),_lipthc(lipthc),_lipbeta(lipbeta),_lipebeta(lipebeta),_liplikep(liplikep),_lipchi2(lipchi2),_liprprob(liprprob){
    CalcBetaError();

    _npexp=0;
    _collected_npe=0;
    _probkl=0;
    _kdist=1e6;
    _phi_spread=1e6;
    _unused_dist=-1;

    if(build_charge){
      if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.start("RERICHZ");
      ReconRingNpexp(3.,!checkstatus(dirty_ring));
      if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.stop("RERICHZ");
    }

    AMSPoint pnt;
    number theta,phi,length;
    
    _radpos[0]=_emission_p[0];
    _radpos[1]=_emission_p[1];
    _radpos[2]=_emission_p[2];
    
    _theta=acos(1/_beta/_index);
    _errortheta=_errorbeta/_beta/tan(_theta);

    track->interpolate(AMSPoint(0,0,RICHDB::RICradpos()-RICHDB::pmt_pos()+RICHDB::pmtb_height()/2.),
		       AMSDir(0.,0.,-1.),pnt,theta,phi,length);
    
    _pmtpos[0]=pnt[0];
    _pmtpos[1]=pnt[1];
    _pmtpos[2]=pnt[2];

    if(fabs(RICHDB::RICradpos()-RICHDB::pmt_pos()+RICHDB::pmtb_height()/2.-_pmtpos[2])>0.01){
      _pmtpos[0]=0;
      _pmtpos[1]=0;
      _pmtpos[2]=0;
    }

  };
  ~AMSRichRing(){};
  AMSRichRing * next(){return (AMSRichRing*)_next;}

  static void build();
  static AMSRichRing* build(AMSTrTrack *track,int cleanup=1);
  static AMSRichRing* rebuild(AMSTrTrack *ptrack);
  //+LIP
  static void buildlip(AMSTrTrack *track);
  //ENDofLIP

  AMSTrTrack* gettrack(){return _ptrack;}
  integer getused(){return _used;}
  bool IsGood(){return !checkstatus(dirty_ring);}
  number getbeta(){return _beta;}
  number geterrorbeta(){return _errorbeta;}
  number getquality(){return _quality;}
  number getnpexp(){return _npexp;}
  number getprob(){return _probkl;}
  number getcollnpe(){return _collected_npe;}

  // lipaccesors
  integer getlipused(){return _liphused;}
  number getlipprob(){return _liprprob;}

#ifdef __WRITEROOT__
friend class RichRingR;
#endif
};





#endif










