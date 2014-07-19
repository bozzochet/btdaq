//  $Id: mccluster.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
//
// June 12, 1996. ak. add set/getnumbers function to AMSTrMCCluster
// July 22, 1996. ak. the same for AMSTOFMCCluster
// Aug  07, 1996. ak. V124
// Sep  30, 1996. ak. V127
// Oct  04, 1996. ak _ContPos is moved to AMSLink
//
//

#ifndef __AMSMCCLUSTER__
#define __AMSMCCLUSTER__
#include "upool.h"
#include "apool.h"
#include "gsen.h"
#include "trid.h"
#include "link.h"
#include "commons.h"
#include <stdlib.h>
#include "trdid.h"
//========================================================  
class AMSTOFMCCluster: public AMSlink{
public:

 integer  idsoft;
 AMSPoint xcoo;
 number   tof;
 number   edep;
 integer getid() const {return idsoft;}
 static integer Out(integer);

 AMSTOFMCCluster(integer _idsoft,AMSPoint _xcoo,number _edep, number _tof) :
 idsoft(_idsoft), xcoo(_xcoo),edep(_edep),tof(_tof){_next=0;};
 AMSTOFMCCluster(){_next=0;};
 ~AMSTOFMCCluster(){};
 void _printEl(ostream &stream){stream <<"AMSTOFMCCluster "<<idsoft<<" "<<edep<<" "<<xcoo<<endl;}
 void _writeEl();
 void _copyEl(){};
 integer operator < (AMSlink & o)const{
   return idsoft < ((AMSTOFMCCluster*)(&o)) ->idsoft ;
 }
 static void sitofhits(integer idsoft , geant vect[],geant edep, geant tofg);
  AMSTOFMCCluster *  next(){return (AMSTOFMCCluster*)_next;}
#ifdef __WRITEROOT__
   friend class TofMCClusterR;
#endif
};

//=====================================================
class AMSAntiMCCluster: public AMSlink{
protected:
 integer  _idsoft;
 AMSPoint _xcoo;
 number _tof;
 number _edep;

public:
 
 AMSAntiMCCluster(integer idsoft,AMSPoint xcoo,number edep, number tof) :
 _idsoft(idsoft), _xcoo(xcoo),_edep(edep),_tof(tof){_next=0;};
 AMSAntiMCCluster(){_next=0;};
 ~AMSAntiMCCluster(){};
 void _printEl(ostream &stream){stream <<"AMSAntiMCCluster "<<_idsoft<<" "<<_edep<<endl;}
 void _writeEl();
 void _copyEl(){};
 static void siantihits(integer idsoft , geant vect[],geant edep, geant tofg);
 AMSAntiMCCluster *  next(){return (AMSAntiMCCluster*)_next;}
 integer operator < (AMSlink & o)const{
 return _idsoft < ((AMSAntiMCCluster*)(&o)) ->_idsoft;}

 integer getid() const {return _idsoft;}
 number getedep() const {return _edep;}
 number gettime() const {return _tof;}
 number getcoo(integer i) {return i>=0 && i<3 ? _xcoo[i]:0;}
 static integer Out(integer);

#ifdef __WRITEROOT__
   friend class AntiMCClusterR;
#endif

};
//=====================================================
class AMSEcalMCHit: public AMSlink{
protected:
 integer  _idsoft;// SSLLFFF (SS->s-layer,LL->f-layer in SS, FFF->fiber in LL)
 AMSPoint _xcoo;
 number _tof;
 number _edep;

public:
 
 AMSEcalMCHit(integer idsoft,AMSPoint xcoo,number edep, number tof) :
 _idsoft(idsoft), _xcoo(xcoo),_edep(edep),_tof(tof){_next=0;};
 AMSEcalMCHit(){_next=0;};
 ~AMSEcalMCHit(){};
 void _printEl(ostream &stream){stream <<"AMSEcalMCHit "<<_idsoft<<" "<<_edep<<endl;}
 void _writeEl();
 void _copyEl(){};
 static void siecalhits(integer idsoft , geant vect[],geant edep, geant tofg);
 AMSEcalMCHit *  next(){return (AMSEcalMCHit*)_next;}
 integer operator < (AMSlink & o)const{
 return _idsoft < ((AMSEcalMCHit*)(&o)) ->_idsoft;}

 integer getid() const {return _idsoft;}
 number getedep() const {return _edep;}
 number gettime() const {return _tof;}
 number getcoo(integer i) {return i>=0 && i<3 ? _xcoo[i]:0;}
 static number impoint[2];
// static integer Out(integer);

};

//------------------------------------------------------
// MC bank for RICH: Preliminary

class AMSRichMCHit: public AMSlink{
protected:

  integer _id;        // Number of geant particle or -666 for noise
  integer _channel;   // Channel number (presently 16*pmt#+window#)
  geant _counts;      // ADC counts for this hits
  AMSPoint _origin;   // Particle origin (if applicable)
  AMSPoint _direction;// Particle direction in the origin (if applicable)
  geant _angle;       // Entrance angle in the LG
  integer _status;    // photon status
  integer _hit;       // associated hit

public:

  AMSRichMCHit(integer id,integer channel,geant counts,AMSPoint origin,AMSPoint direction,integer status) :
    AMSlink(),_id(id),_channel(channel),_counts(counts),_origin(origin),_direction(direction),
    _status(status),_hit(-1){};
  AMSRichMCHit():AMSlink(),_counts(0){};
  ~AMSRichMCHit(){};

  void _printEl(ostream &stream){stream <<"AMSRichMCHit "<<_channel<<endl;}
  void _writeEl();
  void _copyEl(){};
  AMSRichMCHit *  next(){return (AMSRichMCHit*)_next;}
  integer operator < (AMSlink & o)const{
    return _channel < ((AMSRichMCHit*)(&o)) ->_channel;}

  static void sirichhits(integer id, integer pmt,geant position[],geant origin[],geant momentum[],integer status);
  static void noisyhit(integer channel,integer mode=1); // Add noise
  static geant adc_hit(integer n,integer channel=0,integer mode=1);   // Compute the adc counts
  static geant adc_empty(integer channel=0,integer mode=1);
  static geant noise(int channel,integer mode=1);
  integer getid() const {return _id;}
  integer getchannel() const {return _channel;}
  geant getcounts() const {return _counts;}
  const AMSPoint & getorigin(){return _origin;}
  const AMSPoint & getdirection(){return  _direction;}
  integer getstatus() const {return _status;}
  integer gethit() const {return _hit;}
  void puthit(integer n) {_hit=n;};
#ifdef __WRITEROOT__
  friend class RichMCClusterR;
#endif
};



//=======================================================



// MC  bank for tracker

const integer _nel=5;
class AMSTrMCCluster: public AMSlink{
protected:
 integer _idsoft;
 integer _itra;
 integer _left[2];
 integer _center[2];
 integer _right[2];
 geant _ss[2][5];
 AMSPoint _xca;
 AMSPoint _xcb;
 AMSPoint _xgl;
 number   _sum;


 void _shower();
 void _printEl(ostream & stream);
 void _writeEl();
 void _copyEl();

 static integer Out(integer);
 static integer _hid[2];
 static integer _ncha;
 static geant _step; 
 static integer _NoiseMarker;
public:
 // Constructor for noise &crosstalk
 AMSTrMCCluster(const AMSTrIdGeom & id,integer side,integer nel, number ss[],
 integer itra);
 // Constructor for geant track
 AMSTrMCCluster
 (integer idsoft, AMSPoint xca, AMSPoint xcb, AMSPoint xgl, geant sum,integer itra):
  _idsoft(idsoft),_xca(xca),_xcb(xcb),_xgl(xgl),_sum(sum),_itra(itra)
 {_shower();_next=0;}

 // Constructor for daq
 AMSTrMCCluster
 (AMSPoint xgl, integer itra):
  _idsoft(0),_xca(0,0,0),_xcb(0,0,0),_xgl(xgl),_sum(0),_itra(itra)
 {_next=0;
   for(int i=0;i<2;i++){
    _left[i]=0;
    _center[i]=0;
    _right[i]=0;
    for(int k=0;k<5;k++)_ss[i][k]=0;
   }
  }
 AMSTrMCCluster(){_next=0;};
 ~AMSTrMCCluster(){};
  AMSTrIdGeom getid();
  integer IsNoise(){return _itra==_NoiseMarker;}
  AMSPoint getHit(){return _xgl;}
  static integer noisemarker(){return _NoiseMarker;}
  static geant step(){return _step;}
  static integer ncha(){return _ncha;}
  static integer hid(integer l){return l<2?_hid[l]:0;}
  
  AMSTrMCCluster *  next(){return (AMSTrMCCluster*)_next;}
  void addcontent(char xy, geant ** p, integer noise=0);
  void addcontent(const AMSTrIdSoft & id, geant *adc);
  static integer debug;
 static void 
 sitkhits(integer idsoft , geant vect[],geant edep, geant step, 
 integer itra );
 static void sitkcrosstalk();
 static void sitknoise();
 static number sitknoiseprob(const AMSTrIdSoft & id, number threshold);
 static number sitknoiseprobU(number threshold, number step);
 static void sitknoisespectrum(const AMSTrIdSoft & id, number ss[], number prob);


 // Interface with DAQ


 static int16u getdaqid(){return (15 <<9);}
 static integer checkdaqid(int16u id);
 static integer calcdaqlength(integer i);
 static integer getmaxblocks(){return 1;}
 static void builddaq(integer i, integer n, int16u *p);
 static void buildraw(integer n, int16u *p);

#ifdef __WRITEROOT__
 friend class  TrMCClusterR;
#endif


};







class AMSTRDMCCluster: public AMSlink{
protected:
 AMSTRDIdGeom _idsoft;   // geant stray id
 integer _itra;     // geant itra
 integer _ipart;     // geant itra
 AMSPoint _xgl;     // global coo (cm)
 number _step;      // track length (cm)
 number   _edep;      // energy deposition (GeV)
 number   _ekin;      // total particle energy
 void _printEl(ostream & stream);
 void _writeEl();
 void _copyEl();
 static integer _NoiseMarker;
 static integer Out(integer);
public:
 // Constructor for noise and crosstalk
 AMSTRDMCCluster(const AMSTRDIdGeom & id ,geant energy, integer itra):AMSlink(),_idsoft(id),_xgl(0,0,0),_step(0),_edep(energy),_itra(itra),_ekin(0){};
 // Constructor for geant track
 AMSTRDMCCluster(integer idsoft , AMSPoint xgl, AMSDir xdir, geant step,geant energy, geant edep, integer ipart, integer itra):AMSlink(),
_idsoft(idsoft),_ekin(energy),_edep(edep),_ipart(ipart),_itra(itra),_step(step),_xgl(xgl){}

 static void    sitrdhits(integer idsoft ,geant vect[],
        geant destep, geant ekin, geant step,integer ipart, integer itra);   

 // Constructor for daq
 AMSTRDMCCluster (AMSPoint xgl, integer itra): AMSlink(),
  _idsoft(0),_xgl(xgl),_edep(0),_ekin(0),_step(0),_itra(itra){}

 AMSTRDMCCluster():AMSlink(){};
 ~AMSTRDMCCluster(){};
  integer IsNoise(){return _itra==_NoiseMarker;}
  AMSPoint getHit(){return _xgl;}
  static integer noisemarker(){return _NoiseMarker;}
  geant getedep()const {return _edep;}
  geant getitra()const {return _itra;}
  AMSTRDIdGeom getid()const {return _idsoft;}
  AMSTRDMCCluster *  next(){return (AMSTRDMCCluster*)_next;}
 static void sitrdnoise();
 static void init();
 integer operator < (AMSlink & o)const{
 return _idsoft.cmpt() < (((AMSTRDMCCluster*)(&o)) ->_idsoft).cmpt();}

 // Interface with DAQ

/*
 static int16u getdaqid(){return (15 <<9);}
 static integer checkdaqid(int16u id);
 static integer calcdaqlength(integer i);
 static integer getmaxblocks(){return 1;}
 static void builddaq(integer i, integer n, int16u *p);
 static void buildraw(integer n, int16u *p);
*/

#ifdef __WRITEROOT__
 friend class TrdMCClusterR;
#endif

};



#endif
