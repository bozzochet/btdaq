//  $Id: trrawcluster.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __AMSTRRAWCLUSTER__
#define __AMSTRRAWCLUSTER__
#include "upool.h"
#include "apool.h"
#include "gsen.h"
#include "event.h"
#include "cont.h"
#include "amsstl.h"
#include "dcards.h"
#include "trid.h"

// Tracker RAW bank

class AMSTrRawCluster : public AMSlink{
protected:
integer _address;   // as for idsoft
integer _strip;
integer _nelem;
geant _s2n;      // signal to noise for triggered strip
integer * _array;
void _init(){};
AMSTrRawCluster(const AMSTrRawCluster &){_next=0;};
void _copyEl();
void _printEl(ostream & stream);
void _writeEl();
integer _matched();
public:
    static const integer MATCHED;

static integer Out(integer);
static void lvl3CompatibilityAddress(int16u address, 
integer & strip, integer & va, integer & side,  integer &drp){
  drp=(address>>10)&31;va=(address>>6)&15;
  strip=((address)&63)+(va%10)*64; side=va>9 ? 0 : 1;
}

integer getid()const {return _address;}

integer operator < (AMSlink & o) const {
// No RTTI - take a "risk" here
  AMSTrRawCluster * p= (AMSTrRawCluster*)(&o);
  return getid() < p->getid();
}
~AMSTrRawCluster();
AMSTrRawCluster(integer id=0):AMSlink(),_address(id),_strip(0),_nelem(0),
_array(0),_s2n(0){}
AMSTrRawCluster(integer id,  integer nleft, integer nright,geant *p, geant s2n);
AMSTrRawCluster(integer id,  integer nleft, integer nright,int16 *p, geant s2n);
void expand( number *p) const ;
integer lvl3format(int16 * ptr, integer nmax,  integer matchedonly=0);
static void sitkdigi();
  AMSTrRawCluster *  next(){return (AMSTrRawCluster*)_next;}

  static number ADC2KeV(){return 0.5e6/TRMCFFKEY.beta/TRMCFFKEY.dedx2nprel;}


 // Interface with DAQ

 static int16u getdaqid(int i);
 static integer checkdaqid(int16u id);
 static integer calcdaqlength(integer i);
 static int16u getdaqidRaw(int i);
 static int16u getdaqidMixed(int i);
 static int16u getdaqidParameters(int i);
 static int16u getdaqidCompressed(int i);
 static integer checkdaqidRaw(int16u id);
 static integer GetTrCrate(int16u id);
 static integer checkdaqidMixed(int16u id);
 static integer checkdaqidParameters(int16u id);
 static integer checkdaqidCompressed(int16u id);
 static integer calcdaqlengthRaw(integer i);
 static integer getmaxblocks(){return AMSTrIdSoft::ncrates();}
 static integer getmaxblocksRaw(){return AMSTrIdSoft::ncrates();}
 static void builddaq(integer i, integer n, int16u *p);
 static void buildraw(integer n, int16u *p);
 static void matchKS();
 static void builddaqRaw(integer i, integer n, int16u *p);
 static void buildrawRaw(integer n, int16u *p);
 static void buildrawRawA(integer n, int16u *p);
 static void buildrawRawB(integer n, int16u *p);
 static void buildrawMixed(integer n, int16u *p);
 static void buildrawParameters(integer n, int16u *p);
 static void buildrawCompressed(integer n, int16u *p);
 static void buildpreclusters( AMSTrIdSoft & idd, integer len, geant id[]);
 integer TestRawMode();
  // H/K data read
  static integer checkstatusSid(int16u id) ;
  static integer checkpedSid(int16u id) ;
  static integer checkpedSRawid(int16u id) ;
  static integer checkcmnSRawid(int16u id) ;
  static integer checksigSRawid(int16u id) ;
  static integer checkstatusSRawid(int16u id) ;
  static integer checksigmaSid(int16u id);
  static void updstatusS(integer length,int16u *p); 
  static void updpedSRaw(integer length,int16u *p); 
  static void updcmnSRaw(integer length,int16u *p); 
  static void updsigSRaw(integer length,int16u *p); 
  static void updstatusSRaw(integer length,int16u *p); 
  static void updpedS(integer length,int16u *p); 
  static void updsigmaS(integer length,int16u *p); 
  // H/K data simulate
  static int16u getstatusSid(int i) ;
  static int16u getpedSid(int i) ;
  static int16u getpedSRawid(int i) ;
  static int16u getcmnSRawid(int i) ;
  static int16u getsigSRawid(int i) ;
  static int16u getstatusSRawid(int i) ;
  static int16u getsigmaSid(int i);
  static integer getmaxblockS(){return  AMSTrIdSoft::ncrates();}
  static void writestatusS(integer i, integer length,int16u *p); 
  static void writepedS(integer i, integer length,int16u *p); 
  static void writesigmaS(integer i, integer length,int16u *p); 
  static integer calcstatusSl(int i);
  static integer calcpedSl(int i);
  static integer calcsigmaSl(int i);
  // H/K TDV correspondance
  static AMSID getTDVstatus(int i);
  static AMSID getTDVCmnNoise();
  static AMSID getTDVIndNoise();
  static AMSID getTDVped(int i);
  static AMSID getTDVsigma(int i);
  static AMSID getTDVrawsigma(int i);
  static AMSID getTDVrhomatrix(int i);
#ifdef __WRITEROOT__
  friend class TrRawClusterR;
#endif
};

#endif
