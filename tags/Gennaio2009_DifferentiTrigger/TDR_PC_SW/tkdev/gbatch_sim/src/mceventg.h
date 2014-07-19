//  $Id: mceventg.h,v 1.1 2008/09/11 17:19:22 zuccon Exp $
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

#include <time.h>

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
  static AMSPoint *_r_c;
  static AMSDir *_dir_c;
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


public:
  void _copyEl();
  void _writeEl();
  void _printEl(ostream & stream);
  static void SaveSeeds();
  static void RestoreSeeds();
  static void PrintSeeds(ostream & o);
  static void SetSeed(int seed);

  integer getseed(integer i)const{return (i>=0 && i<2) ? _seed[i]: 0;}
  void setseed(integer seed[2]){_seed[0]=seed[0];_seed[1]=seed[1];}
  static integer fixedmom(){return _fixedmom;}
  static integer fixeddir(){return _fixeddir;}
  static void FillMCInfo();
  static integer debug;
  static integer Out(integer status=0);
  AMSmceventg(integer ip, geant mom, const AMSPoint & coo, const AMSDir & dir, integer nskip=0);
  AMSmceventg(integer seed[2]){_next=0;_nskip=0;setseed(seed);}

  ~AMSmceventg(){}
  void init(integer);
  void run(integer ipart);
  void run(integer ipart, integer ipat);
  void run();

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

  static integer _hid;
  static void endjob();
  AMSmceventg *  next(){return (AMSmceventg*)_next;}
  // Interface with DAQ
  
  AMSmceventg(){_next=0;}

#ifdef __WRITEROOT__
  friend class MCEventgR;
#endif
};
#endif
