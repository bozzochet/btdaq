//  $Id: trigger302.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMSTRIGGER302__
#define __AMSTRIGGER302__
#include "link.h"
#include "tofdbc02.h"
#include "tkdbc.h"
#include "trddbc.h"
#include "ecaldbc.h"
#include "amsdbc.h"
#include "amsstl.h"
#include "trid.h"
#include "trdid.h"
#include "commons.h"
//
namespace trigger302const{
const integer NTRHDRP=trid::ntdr;
const integer NTRHDRP2=trid::ntdr*trid::ncrt;
const integer maxtr=10000;
const integer maxtof=1000;
const integer maxtrpl=10;
const integer TRDIN=1;
const integer TOFIN=2;
const integer ECEMIN=4;
const integer ECMATIN=8;
const integer maxufe=20;
const integer maxhitstrd=12;
const integer maxtrd=maxufe*maxhitstrd;
const integer matrixsize=60;
};
const integer NDSTR=3;//tempor
//
class TriggerAuxLVL302{
 protected:
  integer _ltr;   // recorded length  
  integer _ctr;   // current pointer position while read
  int16 _ptr[trigger302const::maxtr];//storage needed info for all detectors of lvl3
 public:

  TriggerAuxLVL302();
  void sitknoise();
  void addnoisetk(integer crate);
  void filltk(integer crate);
  void filltrd(integer crate);
  void filltof();
  void fillecal();
  int16 * readtracker(integer begin=0);
  int16 * readtrd(integer begin=0);
  int16 * readtof(integer begin=0);
  int16 * readecal(integer begin=0);
};




class TriggerLVL302: public AMSlink{

// Put everything in one class due to necessity to be as close to C as possible

protected:
 static integer _flowc[15];// prog.flow counters
//
 uinteger _TriggerInputs;   //   0 Default
                            //   1  Doesnot Require TRD      
                            //   2  Doesnot Require TOF Up/Down info      
                            //   4  Doesnot Require EC EM-info      
                            //   8  Doesnot Require EC track-info      

 integer _TOFTrigger;       //  -1 No Matrix
                            //   0 Too Many Hits
                            //   1  tof 0  1 cluster or 2 cluster
                            //   2  tof 1  -----
                            //   4  tof 2  -----
                            //   8  tof 3  -----
                            //   16  tof 0  2 cluster
                            //   32  tof 1  -----
                            //   64  tof 2  -----
                            //   128  tof 3  -----

 uinteger _TRDTrigger;      //   0  Nothing found
                            //   bit 0:  Segment x found
                            //   bit 1:  segment y found
                            //   bit 2: too many hits found  
                            //   bit 3: high gamma event found 

 uinteger _TrackerTrigger;  // 0  - initial state
                            // 1  - No Tr Tracks found
                            // 2  - Too Many Hits in Tracker
                            // 3  - Positive Rigidity found
                            // 4  - Ambigious Comb (A) found 
                            // 5  - Ambigious Comb (B) found 
                            // 6  - Negative Rigidity(Momentum) found
                            // + 8   // Heavy Ion (Tracker)

 uinteger _MainTrigger;     //  0  - initial state
                            // bit  0 No Tr Tracks found
                            // bit  1 Too Many Hits in Tracker
                            // bit  2 Too Many Hits in TRD
                            // bit  3 Too Many Hits in TOF
                            // bit  4 No TRD Tracks found
                            // bit  5 Upgoing event found by TOF
                            // bit  6 TOF Up/Down Information not found
                            // bit  7 Positive Rigidity(Momentum) found
                            // bit  8 Ambigious Comb (A) found 
                            // bit  9 Ambigious Comb (B) found 
                            // bit  10  Negative Rigidity(Momentum) found
                            // bit  11  High Gamma (TRD)  
                            // bit  12   Heavy Ion (Tracker)
                            // bit  13 Prescaled event  (8192)
                            // bit  14 No EC activity (Etot<MIP)  
                            // bit  15 EC EM-object OR Etot>20gev  
                            // bit  16 EC-track found  
                            // bit  17 EC-track match TOF/TRD path 

// TOF Time Part
 integer _TOFDirection;//(-1->up;+1->down;0->unknown)
//

//
// ECAL electromagneticity/track_matching Part
//
 integer _ECemag;//(-1->NonEmag; 0->unknown(noECactivity); 1->Emag;)
 integer _ECmatc;//(0->unknown(noECTrack);-1->NoMatching;1->Matching)
 geant _ECtofcr[4];//x/y/tgx/tgy-cross. with bot.plane of Tracker


 
// Tracker Part
 integer _NTrHits;      // Total number of "good" hits
 integer _NPatFound;    // Number of "tracks" found
 integer _Pattern[2];
 number _Residual[2];      // Discriminators
 number _TrEnergyLoss;



// Tracker Aux Part

 integer _nhits[trconst::maxlay];
 integer _drp[trconst::maxlay][trigger302const::maxtrpl];
 geant _coo[trconst::maxlay][trigger302const::maxtrpl];
 geant _eloss[trconst::maxlay][trigger302const::maxtrpl];
 geant _lowlimitY[trconst::maxlay];
 geant _upperlimitY[trconst::maxlay];
 geant _lowlimitX[trconst::maxlay];
 geant _upperlimitX[trconst::maxlay];
 geant _upperlimitTOF[2][trconst::maxlay];
 geant _lowlimitTOF[2][trconst::maxlay];
 geant _upperlimitTRD[2][trconst::maxlay];
 geant _lowlimitTRD[2][trconst::maxlay];
 geant coou, dscr,cood,a,b,s,r;
 geant resid[trconst::maxlay-2],zmean,factor,amp[trconst::maxlay];
 static integer _TOFPattern[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
 static integer _TOFOr[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 static integer _TrackerStatus[trigger302const::NTRHDRP2];
 static integer _TrackerAux[trigger302const::NTRHDRP][trid::ncrt];
 static integer _TOFAux[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 static integer _NTOF[TOF2GC::SCLRS];
 static geant _TOFCoo[TOF2GC::SCLRS][TOF2GC::SCMXBR][3];
 static geant _TrackerCoo[trigger302const::NTRHDRP][trid::ncrt][3];
 static geant _TrackerDir[trigger302const::NTRHDRP][trid::ncrt];
 static geant _TrackerCooZ[trconst::maxlay];
 static integer _TrackerDRP2Layer[trigger302const::NTRHDRP][trid::ncrt];
 static number _stripsize;
 static integer _TrackerOtherTDR[trigger302const::NTRHDRP][trid::ncrt];

 integer _UpdateOK(geant s, geant res[], geant amp[],integer pat);
 integer _Level3Searcher(int call, int pat);


// Trd Aux Part
  class TRDAux_DEF{
   public:
    number _Par[2][6];    // fit parameters: t=par[0]*(z-par[2])+par[1]
                          //                   par[3] == error(par[0])
   bool _SegmentFound[2];
   integer _NHits[2]; // number of good hits
   integer _HMult;   // number of clusters with energy > thr 

    integer _nufe;
    integer  _lasthaddr;
    uinteger _haddrs[trigger302const::maxufe];
    uinteger _nhits[trigger302const::maxufe];
    int16u _coo[trigger302const::maxufe][trigger302const::maxhitstrd][3];  // ute,tube,amp  ( X 8 ?)
    geant   _DistrMatrix[2][trigger302const::matrixsize][trigger302const::matrixsize];
static    geant _CooLimits[2];
static    geant _CooBinSize;
static    geant _TanLimits[2];
static    geant _TanBinSize;
static uinteger _Dir[trdid::nufe][trdid::nudr][trdid::ncrt];
static geant _Coo[trdid::nufe][trdid::nudr][trdid::ncrt][3];
static geant _CooZ[trdid::nute][trdconst::maxtube];
static geant _CooT[trdid::nute][trdconst::maxtube];

static geant _IncMatrix[trdid::nute][trdconst::maxtube][trdid::nute-1][trdconst::maxtube];
static geant _CooMatrix[trdid::nute][trdconst::maxtube][trdid::nute-1][trdconst::maxtube];
   TRDAux_DEF():_HMult(0),_nufe(-1),_lasthaddr(-1){
    for(int i=0;i<2;i++){_SegmentFound[i]=false;_NHits[i]=0;}
    for(int i=0;i<trigger302const::matrixsize;i++){
     for(int j=0;j<trigger302const::matrixsize;j++){
     _DistrMatrix[0][i][j]=0;
     _DistrMatrix[1][i][j]=0;
    }
   }
   for(int i=0;i<2;i++){
    for(int j=0;j<6;j++)_Par[i][j]=0;
   }
}
  integer addnewhit(uinteger crate, uinteger udr, uinteger ufe,uinteger ute, uinteger tube, int16u amp);
  void build();
  };  
  TRDAux_DEF TRDAux;



//  Tof Aux Part

 static integer _TOFStatus[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 static geant _TOFTzero[TOF2GC::SCLRS][TOF2GC::SCMXBR];

//  ECAL Aux part
//
 static geant _ECgains[ecalconst::ECSLMX][ecalconst::ECPMSMX];
 static geant _ECadc2mev;
 static geant _ECh2lrat;
 static geant _ECpedsig;
 static geant _ECpmdx;
 static geant _ECpmx0;
 static geant _ECpmy0;
 static int   _ECpmpsl;
 static geant _ECpmdz; 
 static geant _ECpmz;
 static geant _ECcrz3;
 static geant _ECcrz4;



 void _copyEl(){}
 void _printEl(ostream & stream);
 void _writeEl();
 number _Time;          // Time to process event

public:
 TriggerLVL302(bool tofin=true, bool trdin=true);

 void Finalize();   // mke the MainTriggerOutput

 static void printfc();// print prog.flow counters
 
// Tracker Part


 integer toftrdok();
 integer TOFOK(){return _TOFTrigger==1;}
 uinteger & TrackerTrigger(){ return _TrackerTrigger;}
 uinteger & TRDTrigger(){ return _TRDTrigger;}
 integer MainTrigger(); 
 integer LVL3OK();
 integer LVL3HeavyIon(){return (_TrackerTrigger & 8) || (_TrEnergyLoss>600);}
 static integer TOFOr(uinteger paddle,uinteger plane);
 static integer TOFInFastTrigger(uinteger paddle, uinteger plane);
 void preparetracker();
 void addtof(int16 plane, int16 paddle);
 void settofdir(int d);
 geant getlowlimitY(integer layer){return _lowlimitY[layer];} 
 geant getupperlimitY(integer layer){return _upperlimitY[layer];}
 geant getlowlimitX(integer layer){return _lowlimitX[layer];} 
 geant getupperlimitX(integer layer){return _upperlimitX[layer];}
 void settime(number time);
 void fit(integer idum);
 integer addnewhit(geant coo, geant amp, integer layer, integer drp);
 static void build();
 static void init();
 static geant Discriminator(integer nht);

 //TRD Part

 bool UseTRD(){return (_TriggerInputs&trigger302const::TRDIN)==0;}


 //TOFTime Part
   
 bool UseTOFTime(){return (_TriggerInputs&trigger302const::TOFIN) ==0;}

// ECAL
 bool UseECEMinfo(){return (_TriggerInputs&trigger302const::ECEMIN) ==0;}
 bool UseECMATinfo(){return (_TriggerInputs&trigger302const::ECMATIN) ==0;}
 void setecemag(integer d);
 integer getecemag(){return _ECemag;}
 void setecmatc(integer d);
 integer getecmatc(){return _ECmatc;}
 void setectofcr(geant c[2], geant tg[2]);
 int eccrosscheck(geant ec);


 // Interface with DAQ

       static integer checkdaqid(int16u id);
       static integer calcdaqlength(integer i);
       static integer getmaxblocks(){return 1;}
       static int16u getdaqid(int i){return (i>=0 && i<getmaxblocks())?
       ( 9<<9 | i<<6 ):0;}
       static void builddaq(integer i, integer n, int16u *p);
       static void buildraw(integer n, int16u *p);


    friend class TriggerExpertLVL3;

 integer skip(){
   if ( _MainTrigger%16 )return 1;
   else return 0;
 }
#ifdef __WRITEROOT__
 friend class Level3R;
#endif
};


class TriggerExpertLVL3{
protected:
number _Mean[NDSTR];
number _Sigma[NDSTR];
number _XCount[NDSTR];
integer _Counter;              // current event in seq
integer _CounterMax;           // max no events in seq
integer _ToBadSwitch;          // max seq to switch off
integer _TryAgainSwitch;       // max seq to switch on
  // first index:
  //  0 - Reject
  //  1 - Fails if too many hits/comb
  //  2 - Accept
  //  3 - Sum
geant  _Distributions[NDSTR+1][trigger302const::NTRHDRP2]; 
integer  _Relative[NDSTR];      // 0 - take abs 1 - take relative    
integer _DRPBad[trigger302const::NTRHDRP2];       // how man sequences the drp is bad
integer _DRPOff[trigger302const::NTRHDRP2];       // how many seq the drp was off
geant _BadRange[NDSTR][2];
public:
TriggerExpertLVL3(integer countermax, integer tobadswitch, integer tryagain,
                  geant badrange[NDSTR][2]);

void update(const TriggerLVL302  * plvl3);
void analyse(const TriggerLVL302  * plvl3);
void print();
 static TriggerExpertLVL3 * pExpert;
};

class AMSLVL3Error{
private:
 char msg[256];
public:
 AMSLVL3Error(char * name);
 char * getmessage();
};



#endif

