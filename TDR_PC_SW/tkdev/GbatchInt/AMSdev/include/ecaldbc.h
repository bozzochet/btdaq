//  $Id: ecaldbc.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author E.Choumilov 14.07.99.
//
//
#ifndef __ECALDBC__
#define __ECALDBC__
#include "cern.h"
#include "extC.h"
#include <math.h>
#include "amsdbc.h"
#include "dcards.h"
//
// ECAL global constants definition
//
namespace ecalconst{
const integer ECRT=2;      // number of ecal crates 
const integer ECFLSMX=10; // max. fiber-layers per S-layer
const integer ECFBCMX=12; // max. fibers per layer in PMcell
const integer ECSLMX=9; // max. S(uper)-layers
const integer ECPMMX=36;//max PMTs per superLayer
const integer ECPMSMX=ECPMMX; //same for backw.compatibility
const integer ECPMSL=ECSLMX*ECPMSMX;// Max. total PM's in all S-layers
const integer ECSTYPS=2;//active(keeping pmt-data) slot-types (EDR2,ETRG)
const integer ECSLOTS=7;//total number of active slots/crate
const integer ECEDRS=6;//number of ERD2s/crate
const integer ECCONN=3;//number of connectors/EDR2
const integer ECRCMX=ECSLMX*ECPMMX*(4*2+1);// max EC readout_channels(4->pixels,2->gains,1->dynode)
const integer ECEDRC=243;//readout channels/EDR
const integer ECFBLMX=500;// max. fibers per layer
const integer ECADCMX[3]={3500,4095,4095};//max capacity of ADC(12bits)
const integer ECROTN=10000; // geant numbering of ECAL rot. matr.(starting from...)
const integer ECJSTA=15; // max size of counter-array for job statistics
const integer ECHIST=2000;// MCEcal histogram number(starting from...) 
const integer ECHISTR=2100;// REEcal histogram number(starting from...)
const integer ECHISTC=2200;// CAEcal histogram number(starting from...)
};
//
//geometry :
//
//===========================================================================
// 
// --------------> Class for "time-stable("geom")"  parameters :
class ECALDBc {  
//
private:
// geom. constants:
  static geant _gendim[10]; // ECAL general dimensions
//                            1-3-> X,Y,Z-sizes of EC-radiator;
//                            4-> eff. X(Y)-thickness of PM+electronics volume
//                            5->X-pos; 6->Y-pos; 7->Z-pos(front face);
//                            8->top(bot) honeycomb thickness
  static geant _fpitch[3]; // fiber pitch:
//                           1-> in X(Y); 2/3->Z(inside of x(y) cell and between x-y)
  static geant _rdcell[10];// readout-PMcell parameters
//                          1-4->fiber att.length,%,diam; 5->size(dx=dz) of "1/4" of PM-cathode;
//                          6->abs(x(z)-position) of "1/4" in PMT coord.syst.(not used);
//                          7/8->X(Y)-pitch of PM's; fiber glue thickness(in radious)
//                          9-> lead thickness of 1 SL
//                          10-> Al-plate thickness
//
  static integer _scalef;  // MC/Data: scale factor used for digitization in DAQ-system
// 
  static integer _nfibpl[2];// num.of fibers per odd/even(1st/2nd) elementary layer in s-layer
  static integer _slstruc[6];//1->1st super-layer projection(0->X, 1->Y);
//                             2->num. of fib.layers per super-layer; 3->num.of super-layers (X+Y);
//                             4->num. of PM's per super-layer;
//                             5-6->readout dir. in X/Y-proj (=1/-1->+/-) for the 1st PM-cell. 
//
public:  
//
//  Member functions :
//
// ---> function to read geomconf-file
  static void readgconf();
//
//

  static uinteger GetLayersNo(){return _slstruc[2]*2;}
  static geant gendim(integer i);
  static geant fpitch(integer i);
  static geant rdcell(integer i);
  static integer scalef(){return _scalef;}
  static integer nfibpl(integer i);
  static integer slstruc(integer i);
  static void fid2cida(integer fid, integer cid[4], number w[4]);
  static void getscinfoa(integer i, integer j, integer k,
         integer &pr, integer &pl, integer &ce, number &ct, number &cl, number &cz);
  static number segarea(number r, number ds);
  static number CellCoo(integer plane, integer cell, integer icoo);
  static number CellSize(integer plane, integer cell, integer icoo);
	 
	 
//  
  static integer debug;
//
};
//===========================================================================
//
// ---------------> Class for JobControlStatistics  :
//
class EcalJobStat{
//
private:
  static integer mccount[ecalconst::ECJSTA];// event passed MC-cut "i"
//          i=0 -> entries
//          i=1 => MCHits->RawEvent OK
  static integer recount[ecalconst::ECJSTA];// event passed RECO-cut "i"
//          i=0 -> entries
//          i=1 ->
//          i=5 -> CatastrRearLeak detected 
  static integer cacount[ecalconst::ECJSTA];// event passed CALIB-cut "i"
//          i=0 -> entries
//          i=1 ->
  static integer srcount[20];// service counters
// 
  static number zprmc1[ecalconst::ECSLMX];// mc-hit average Z-profile(SL-layers) 
  static number zprmc2[ecalconst::ECSLMX];// mc-hit(+att) average Z-profile(SL(PM-assigned)-layers)
//
  static integer daqc1[ecalconst::ECJSTA];//daq-decoding counters
//            i=0 -> JINF-level entries
//             =1 -> ...with rawData type
//             =2 -> ........comprData type
//             =3 -> ...no_assembl_err (raw-type) 
//             =4 -> ...no_assembl_err (com-type)
//             =5 -> ...for Crate_1/Side_1(any type) 
//             =6 -> ...for Crate_1/Side_2(any type) 
//             =7 -> ...for Crate_2/Side_1(any type) 
//             =8 -> ...for Crate_2/Side_2(any type)
//            ..............................
//             =ECJSTA-1 -> Rejected(bad) JINF-entries
  static integer daqc2[ecalconst::ECRT][ecalconst::ECJSTA];
//            i=0 -> EDR-level RawFormat entries per crate
//             =1 -> .........CompFormat .................
//             =2 -> ....no_assembl_err (raw)  
//             =3 -> ....no_assembl_err (comp)  
//             =4 -> .... IllegalCardId(EDR/ETRG blockID)(any type)  
  static integer daqc3[ecalconst::ECRT][ecalconst::ECSLOTS][ecalconst::ECJSTA];
//            i=0 -> EDR/ETRG-level entrs per crate/slot
//            i=1 -> EDR length OK per crate/slot
//            i=2 -> 
public:
  static number zprofa[2*ecalconst::ECSLMX];//  SubCellPlanes  profile
  static number zprofapm[ecalconst::ECSLMX];// SL profile
  static number zprofac[ecalconst::ECSLMX];// SuperLayers Edep profile for calib.events(punch-through)
  static geant nprofac[ecalconst::ECSLMX];// SuperLayers profile for calib.events(punch-through)
  
  static void daqs1(integer info){
#ifdef __AMSDEBUG__
      assert(info>=0 && info<ecalconst::ECJSTA );
#endif
    daqc1[info]+=1;
  }
  static void daqs2(int16u crat, integer info){
#ifdef __AMSDEBUG__
      assert(info>=0 && info< ecalconst::ECJSTA);
      assert(crat>=0 && crat< ecalconst::ECRT);
#endif
    daqc2[crat][info]+=1;
  }
  static void daqs3(int16u crat, int16u slot, integer info){
#ifdef __AMSDEBUG__
      assert(info>=0 && info< ecalconst::ECJSTA);
      assert(crat>=0 && crat< ecalconst::ECRT);
      assert(slot>=0 && slot< ecalconst::ECSLOTS);
#endif
    daqc3[crat][slot][info]+=1;
  }
  
  static void clear();
  static void addzprmc1(int i, number ed){
    zprmc1[i]+=ed;
  }
  static number getzprmc1(int i){
    return zprmc1[i];
  }
  static void addzprmc2(int i, number ed){
    zprmc2[i]+=ed;
  }
  static void addmc(int i){
#ifdef __AMSDEBUG__
      assert(i>=0 && i< ecalconst::ECJSTA);
#endif
    mccount[i]+=1;
  }
  static void addre(int i){
#ifdef __AMSDEBUG__
      assert(i>=0 && i< ecalconst::ECJSTA);
#endif
    recount[i]+=1;
  }
  static void addca(int i){
#ifdef __AMSDEBUG__
      assert(i>=0 && i<  ecalconst::ECJSTA);
#endif
    cacount[i]+=1;
  }
  static void addsr(int i){
#ifdef __AMSDEBUG__
      assert(i>=0 && i< 20);
#endif
    srcount[i]+=1;
  }
  static integer getca(int i){
#ifdef __AMSDEBUG__
      assert(i>=0 && i<  ecalconst::ECJSTA);
#endif
    return cacount[i];
  }
  static void printstat();
  static void bookhist();
  static void bookhistmc();
  static void outp();
  static void outpmc();
};
//
//===========================================================================
// ---------------> Class to store ECAL calibration(MC/RealData) constants :
//
class ECcalib{
//
private:
  integer _softid;  // SSPP (SS->S-layer number, PP->PMcell number)
  integer _status[4];  //4-SubCells calib.status(MN->Hch/Lch, M(N)=0/1 -> OK/BAD)
  integer _statusd;//  calib.status of Dynode(=0/1->OK/BAD) 
  geant _pmrgain;    // PM relative(to ref.PM) gain (if A=(sum of 4 SubCells) pmrgain = A/Aref)
  geant _scgain[4]; // relative(to averaged) gain of 4 SubCells(highGain chain)(average_of_four=1 !!!)
  geant _hi2lowr[4]; // ratio of gains of high- and low-chains (for each of 4 SubCells)
  geant _an2dyr;    // 4xAnode_pixel/dynode signal ratio
  geant _adc2mev;   // Global(hope) Signal(ADCchannel)->Emeas(MeV) conv. factor (MeV/ADCch)
  geant _lfast;// att.length(short comp.)
  geant _lslow;// att.length(long comp.)
  geant _fastf;// percentage of short comp.
public:
  static ECcalib ecpmcal[ecalconst::ECSLMX][ecalconst::ECPMSMX];
  ECcalib(){};
  ECcalib(integer sid, integer sta[4], integer stad, geant pmg, geant scg[4], geant h2lr[4], 
       geant a2dr, geant lfs, geant lsl, geant fsf, geant conv):
       _softid(sid),_statusd(stad), _pmrgain(pmg),_an2dyr(a2dr),_lfast(lfs),
       _lslow(lsl),_fastf(fsf),_adc2mev(conv){
    for(int i=0;i<4;i++){
      _status[i]=sta[i];
      _scgain[i]=scg[i];
      _hi2lowr[i]=h2lr[i];
    }
  };
  integer & getstat(int i){return _status[i];}
  geant &pmrgain(){return _pmrgain;}
  geant &pmscgain(int i){return _scgain[i];}
  bool HCHisBad(int i){ return (_status[i]%100)/10>0;}
  bool LCHisBad(int i){ return _status[i]%10>0;}
  bool DCHisBad(){ return _statusd%10>0;}
  geant &hi2lowr(integer subc){return _hi2lowr[subc];}
  geant & adc2mev(){return _adc2mev;}
  geant &an2dyr(){return _an2dyr;}
  geant& alfast(){return _lfast;}
  geant& alslow(){return _lslow;}
  geant& fastfr(){return _fastf;}
  geant attfdir(geant pmd){ return((1-_fastf)*exp(-pmd/_lslow)+_fastf*exp(-pmd/_lfast));}
  geant attfrfl(geant pmd, geant hflen){
    return ((1-_fastf)*exp(-(2*hflen-pmd)/_lslow)+_fastf*exp(-(2*hflen-pmd)/_lfast))
	  *((1-_fastf)*exp(-2*hflen/_lslow)+_fastf*exp(-2*hflen/_lfast))*ECMCFFKEY.fendrf;
  }
  static void build();
  static number pmsatf1(int dir, number q);
};
//===========================================================================
// ---------------> Class to store ECAL calibration(MC-Seeds) constants :
//
class ECcalibMS{
//
private:
  integer _softid;  // SSPP (SS->S-layer number, PP->PMcell number)
  integer _status[4];  //4-SubCells calib.status(MN->Hch/Lch, M(N)=0/1 -> OK/BAD)
  integer _statusd;//  calib.status of Dynode(=0/1->OK/BAD) 
  geant _pmrgain;    // PM relative(to ref.PM) gain (if A=(sum of 4 SubCells) pmrgain = A/Aref)
  geant _scgain[4]; // relative(to averaged) gain of 4 SubCells(highGain chain)(average_of_four=1 !!!)
  geant _hi2lowr[4]; // ratio of gains of high- and low-chains (for each of 4 SubCells)
  geant _an2dyr;    // 4xAnode_pixel/dynode signal ratio
  geant _lfast;// att.length(short comp.)
  geant _lslow;// att.length(long comp.)
  geant _fastf;// percentage of short comp.
public:
  static ECcalibMS ecpmcal[ecalconst::ECSLMX][ecalconst::ECPMSMX];
  ECcalibMS(){};
  ECcalibMS(integer sid, integer sta[4], integer stad, geant pmg, geant scg[4], geant h2lr[4], 
       geant a2dr, geant lfs, geant lsl, geant fsf):
       _softid(sid),_statusd(stad), _pmrgain(pmg),_an2dyr(a2dr),_lfast(lfs),
       _lslow(lsl),_fastf(fsf){
    for(int i=0;i<4;i++){
      _status[i]=sta[i];
      _scgain[i]=scg[i];
      _hi2lowr[i]=h2lr[i];
    }
  };
  integer & getstat(int i){return _status[i];}
  geant &pmrgain(){return _pmrgain;}
  geant &pmscgain(int i){return _scgain[i];}
  bool HCHisBad(int i){ return (_status[i]%100)/10>0;}
  bool LCHisBad(int i){ return _status[i]%10>0;}
  bool DCHisBad(){ return _statusd%10>0;}
  geant &hi2lowr(integer subc){return _hi2lowr[subc];}
  geant &an2dyr(){return _an2dyr;}
  geant& alfast(){return _lfast;}
  geant& alslow(){return _lslow;}
  geant& fastfr(){return _fastf;}
  geant attf(geant pmd){ return((1-_fastf)*exp(-pmd/_lslow)+_fastf*exp(-pmd/_lfast));}
  static void build();
};
//
//===========================================================================
// --------------> Class for general "time-Variable"  parameters :
class ECALVarp {  
//
private:
// ---> ECAL DAQ-system thresholds :
  geant _daqthr[15];   // DAQ-system thresholds
          // (0) -> anode readout DAQ-threshold(ped sigmas)
	  // (1) -> Dynode Etot cut for "MIP"-trigger(mev tempor)  
	  // (2) ->                     
	  // (3) -> 
	  // (4) -> Dynode readout DAQ-threshold(ped.sigmas)  
          // (5-13)-> LayerTrigThresholds
	  // (14)-> spare 
//   
// ---> RECO Run-Time Cuts :
  geant _cuts[10];                    
          //  (0)  -> FT-algorithm cuts
          //  (1)  -> ................. 
          //  (2)  -> ................. 
          //  (3)  -> ................. 
          //  (4)  -> ................. 
          //  (5)  -> spare 
          //  (9)  -> LVL3-trig. EC-algorithm: "peak"/"average" methode boundary 
public:
  static ECALVarp ecalvpar;
  ECALVarp(){};
// member functions :
//
  void init(geant daqthr[15], geant cuts[10]);
//
  geant daqthr(int i){;
#ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>=0 && i<15);
      }
#endif
    return _daqthr[i];}
//
  geant rtcuts(int i){;
#ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>=0 && i<10);
      }
#endif
    return _cuts[i];}
//
//
};
//===================================================================
// class to store ECAL PM-peds/sigmas  :
class ECPMPeds{
//
private:
  integer _softid;  // SSPP (SS->S-layer number, PP->PMtube number)
  uinteger _staH[4];//Anode HighGainChannel  status
  uinteger _staL[4];//Anode LowGainChannel status
  uinteger _stad;//Dynode channel status
  geant _pedh[4]; // ped for high-channel of 4 SubCells(pixels)(in ADCchannels)
  geant _pedl[4]; // ped for low-channel of 4 SubCells(pixels)
  geant _sigh[4]; // sigma for high-channel of 4 SubCells(pixels)
  geant _sigl[4]; // sigma for high-channel of 4 SubCells(pixels)
  geant _pedd;
  geant _sigd;
//
public:
  static ECPMPeds pmpeds[ecalconst::ECSLMX][ecalconst::ECPMSMX];
  ECPMPeds(){};
  ECPMPeds(integer sid, uinteger stah[4], uinteger stal[4], uinteger stad,
                           geant pedh[4], geant sigh[4],
                           geant pedl[4], geant sigl[4],
			   geant pedd, geant sigd)
			   :_softid(sid),_stad(stad),_pedd(pedd),_sigd(sigd){
    for(int i=0;i<4;i++){
      _pedh[i]=pedh[i];
      _sigh[i]=sigh[i];
      _pedl[i]=pedl[i];
      _sigl[i]=sigl[i];
      _staH[i]=stah[i];
      _staL[i]=stal[i]; 
    }
  };
  geant &ped(uinteger chan, uinteger gain)  {return gain==0?_pedh[chan<4?chan:0]:_pedl[chan<4?chan:0];}  
  geant & sig(uinteger chan, uinteger gain)  {return gain==0?_sigh[chan<4?chan:0]:_sigl[chan<4?chan:0];}
  uinteger &sta(uinteger chan, uinteger gain){return gain==0?_staH[chan<4?chan:0]:_staL[chan<4?chan:0];}
  geant &pedd()  {return _pedd;}
  geant &sigd()  {return _sigd;}
  uinteger &stad()  {return _stad;}
  bool HCHisBad(uinteger chan){return (_staH[chan]!=0);}
  bool LCHisBad(uinteger chan){return (_staL[chan]!=0);}
  bool DCHisBad(){return (_stad!=0);}
  void getpedh(geant pedh[4]){
    for(int i=0;i<4;i++)pedh[i]=_pedh[i];
  }
  void getpedl(geant pedl[4]){
    for(int i=0;i<4;i++)pedl[i]=_pedl[i];
  }
  void getsigh(geant sigh[4]){
    for(int i=0;i<4;i++)sigh[i]=_sigh[i];
  }
  void getsigl(geant sigl[4]){
    for(int i=0;i<4;i++)sigl[i]=_sigl[i];
  }
  integer getsid(){return _softid;}
  static void build();
  static void mcbuild();
};
//
#endif

