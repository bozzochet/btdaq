//  $Id: io.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 2-feb-1997
//
// Feb  10, 1997.  ak.  implement friend class

// Last edit : Feb 10, 1997. ak.
//

#ifndef __AMSIO__
#define __AMSIO__
#include "point.h"
#include "cern.h"
#include "link.h"
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
class AMSIO {
private:
integer _run;
integer _event;
time_t _time;
integer _ipart;
integer _seed[2];
geant _coo[3];
geant _dir[3];
geant _mom;
geant _polephi;
geant _stationtheta;
geant _stationphi;
integer _nskip;
geant _rflight;
geant _velphi;
geant _veltheta;
geant _yaw;
geant _pitch;
geant _roll;
geant _angvel;
time_t _nsec;
static integer _Ntot;
public:
AMSIO():_run(0),_event(0){};
AMSIO(integer run, integer event, time_t time, time_t nsec, integer ipart, 
integer seed[], AMSPoint coo,
AMSDir dir, number mom, number pole, number stheta, number sphi, integer nskip,
number rad, number velt, number velp, number yaw, number roll, number pitch, number angvel);
~AMSIO(){};
 friend ostream &operator << (ostream &o, const AMSIO &b );
void convert();   
integer getrun() const { return _run;}
integer getevent() const { return _event;}
integer getpid() const {return _ipart;}
integer getskip() const{return _nskip;}
void setpid(integer pid)  {_ipart=pid;}
time_t gettime() const {return _time;}
time_t getnsec() const {return _nsec;}
geant getpolephi() const{return _polephi;}
geant getstheta() const{return _stationtheta;}
geant getsphi() const{return _stationphi;}
geant getvelphi() const{return _velphi;}
geant getveltheta() const{return _veltheta;}
geant getrad() const{return _rflight;}
geant getyaw() const{return _yaw;}
geant getroll() const{return _roll;}
geant getpitch() const{return _pitch;}
geant getangvel() const{return _angvel;}
integer read();
integer readA();
void close(){ fbin.close();}
integer getseed(integer i){return i>=0 && i<2 ? _seed[i] : 0;} 
static void setfile(char *f);
void write();
static char * fnam;
static fstream fbin;
static void init(integer mode, integer format=0);
static integer getntot(){return _Ntot;}
friend class AMSmceventg;

#ifdef __DB__
   friend class AMSIOD;
#endif
};

class AMSIOI{
public:
AMSIOI();
~AMSIOI();
private:
static integer _Count;
};
static AMSIOI ioI;
 

#endif
