//  $Id: mceventg.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
// 
// Oct 02, 1996. ak. add set/getNumbers, rearrange class member functions
//                   add next()
// Oct  04, 1996.  ak _ContPos is moved to AMSLink
//                    implement friend class
// Feb  10, 1997. ak. AMSIOD - friend class
//
// Last edit : Feb 10, 1997. ak. 
//

#ifndef __AMSMCEVENTG__
#define __AMSMCEVENTG__
#include "point.h"
#include "cern.h"
#include "link.h"
#include "io.h"
#include <time.h>
#include "astro.h"
class orbit{
public:
integer Nskip;
integer Nskip2;
integer Ntot;
number AlphaTanThetaMax;
number AlphaSpeed;
number AlphaAltitude;
number ThetaI;
number PhiI;
number PhiZero;
number PolePhi;
number PolePhiStatic;
number PoleTheta;
number EarthSpeed;
number NodeSpeed; //ISN  
number FlightTime;
number EarthR;
number DipoleR;
number DipoleTheta;
number DipolePhi;
AMSDir Axis;
tm Begin;
tm End;
integer UpdateOrbit(number curtime, geant & ThetaS, geant & PhiS, geant & PolePhi, number & RaS, number & DecS, number & GLatS, number & GLongS, time_t & time); 
void UpdateOrbit(number theta,number phi, integer dir);
void UpdateAxis(number veltheta, number velphi, number theta, number phi);
orbit(geant ThetaI,geant PhiI, geant PolePhi, integer Dir);
orbit(){};
~orbit(){cout <<"Nskip "<<Nskip<<" Nskip2 "<<Nskip2<<endl;}
};

class AMSmctrack: public AMSlink{
protected:
number _radl;
number _absl;
number _pos[3];
char _vname[4];
void _copyEl(){};
void _writeEl();
void _printEl(ostream & stream){};
public:
AMSmctrack(number radl,number absl, float pos[3], char name[]):AMSlink(),_radl(radl),_absl(absl){
 for (int i=0;i<3;i++)_pos[i]=pos[i];
 for (int i=0;i<4;i++)_vname[i]=name[i];
}
#ifdef __WRITEROOT__
 friend class MCTrackR;
#endif
};

class AMSmceventg: public AMSlink {
private:

AMSPoint _coo;
AMSDir   _dir;

number _mom;
number _mass;
number _charge;
number _delay;
integer _ipart;
integer _seed[2];
integer _nskip;
static AMSPoint _coorange[2];
static AMSPoint _dirrange[2];
static number _momrange[2];
static integer _fixedpoint;
static integer _fixedplane;
static integer _fixeddir;
static integer _fixedmom;
static number _albedorate;
static number _albedocz;
static number _planesw[6];
static number _flux[30];
static geant* _spectra[30];
static int _particle[30];
static int _nucleons[30];
integer operator < (AMSlink & o) const {
 return dynamic_cast<AMSmceventg*>(&o) ? _mom> ((AMSmceventg*)(&o))->_mom:0;
}
void _copyEl();
void _writeEl();
void _printEl(ostream & stream);
public:
static void SaveSeeds();
static void RestoreSeeds();
static void PrintSeeds(ostream & o);
static void SetSeed(int seed);
static orbit Orbit;
integer getseed(integer i)const{return (i>=0 && i<2) ? _seed[i]: 0;}
void setseed(integer seed[2]){_seed[0]=seed[0];_seed[1]=seed[1];}
static integer fixedmom(){return _fixedmom;}
static integer fixeddir(){return _fixeddir;}
static void FillMCInfo();
static integer debug;
static integer Out(integer status=0);
AMSmceventg(integer ip, geant mom, const AMSPoint & coo, const AMSDir & dir, integer nskip=0);
AMSmceventg(integer seed[2]){_next=0;_nskip=0;setseed(seed);}
AMSmceventg( const AMSIO & io);
~AMSmceventg(){}
void init(integer);
void run(integer ipart);
void run(integer ipart, integer ipat);
void run();
#ifdef __G4AMS__
void runG4(integer ipart=0);
#endif
void InitSeed();
void gener();
integer fastcheck(geant x, geant y, geant z, geant the, geant phi);
number getcharge()const {return _charge;}
number getdelay()const {return _delay;}
void setdelay(number delay){_delay=delay;}
void gendelay();
AMSDir getdir()const {return _dir;}
number getmom()const {return _mom;}
void setmom(number mom) { _mom=mom;}
integer & pid() {return _ipart;}
bool Primary();
number getmass() const {return _mass;}
integer accept();
bool SpecialCuts(integer cut);
integer acceptio();
integer EarthModulation();
static void setcuts( geant [],geant[],geant[],integer,geant,geant);
static void setspectra(integer begind, integer begint, integer endd, 
integer endt, integer ipart, integer low);
static void lookupsourcesp(integer sourceid,number & constant,number & index); //ISN 
static void lookupsourcecoo(integer sourceid, geant & rasource, geant & decsource); //ISN
static integer _hid;
static void endjob();
AMSmceventg *  next(){return (AMSmceventg*)_next;}
 // Interface with DAQ

 static int16u getdaqid(){return (14 <<9);}
 static integer checkdaqid(int16u id);
 static integer calcdaqlength(integer i);
 static integer getmaxblocks(){return 1;}
 static void builddaq(integer i, integer n, int16u *p);
 static void buildraw(integer n, int16u *p);
AMSmceventg(){_next=0;}
#ifdef __WRITEROOT__
   friend class MCEventgR;
#endif
};
#endif
