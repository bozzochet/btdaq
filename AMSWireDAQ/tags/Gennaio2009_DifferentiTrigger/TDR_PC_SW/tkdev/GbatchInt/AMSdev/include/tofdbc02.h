//  $Id: tofdbc02.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author E.Choumilov 13.06.96.
//
// Last edit : Jan 21, 1997 ak. !!!! put back friend class TOFDBcD
// Last edit : june 27, 1997 EC, add some DAQ-constants
// Add TOF-charge classes, E.Choumilov 19.01.2005
// Removed all TOF gain5 logic, E.Choumilov 22.08.2005
//
#ifndef __TOF2DBC__
#define __TOF2DBC__
#include "typedefs.h"
#include "cern.h"
#include "dcards.h"
#include "amsdbc.h"
#include "extC.h"
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include "job.h"
// #include <daqblock.h>
//
//
//some general(tof version-independent) constants
namespace TOFGC{
  const integer AMSDISL=500;//max length of distributions in AMSDistr class
  const integer SCWORM=256;// Max.length (in 16-bits words) in bit-stream class
  const integer SCBITM=SCWORM*16;// same in bits(-> 2.048 mks with 0.5 ns binning)
  const integer SCBADB1=128; // status bit to mark counter with bad "history"(for ANTI - NoFTCoinc.on 2sides)
  const integer SCBADB2=256; // status bit to mark counter with only one_side measurement
  const integer SCBADB3=512; // ... bad for t-meas.based on DB(don't use for tzcalibr/beta-meas)
  const integer SCBADB4=1024; // missing side number(s1->no_bit,s2->set_bit, IF B2 is set !)
  const integer SCBADB5=2048; // set if missing side was recovered (when B2 is set)
  const integer SCBADB6=4096; // set if no "best"_LT-hit/SumHT matching on, at least, one of the alive side
  const integer SCBADB7=8192; // 
}
//-----
// TOF2 global constants definition
namespace TOF2GC{
//geometry :
const integer SCMXBR=10; // max nmb of bars/layer
const integer SCMXBR2=18;//max nmb of bars/top(bot) TOF-subsystem
const integer SCLRS=4; // max nmb of layers in TOF-systems
const integer SCROTN=2; // start nmb of abs. numbering of TOF rot. matrixes
const integer SCBTPN=11; //Real nmb of sc. bar types (different by length now)
const integer SCCHMX=SCLRS*SCMXBR*2; //MAX scint. channels(layers*bars*sides)
const integer SCBLMX=SCLRS*SCMXBR;   //MAX scint. bars*layers
const integer PMTSMX=3; // MAX number of PMTs per side
//MC
const integer TOFPNMX=1000;// max integral-bins  in TOFTpoints class
const integer SCANPNT=15; //max scan points in MC/REC t/a-calibration
const integer SCANTDL=400;//scan time distribution max.length(should be SCANTDL<AMSDISL) 
const integer SCANWDS=4; //max. width-divisions in the sc.paddle scan
const integer SCTBMX=10000;//length of MC "flash-ADC" buffer(-> 1mks with 0.1ns binning)
const integer SESPMX=3;//Max number of PMT single elecr.spectrum parameters
// 
//DAQ
const int16u SCPHBP=16384; // phase bit position in Reduced-format TDC word (15th bit)
const int16u SCPHBPA=32768;// phase bit position in Raw-format address word (16th bit)
const int16u SCADCMX=4095;// MAX. value in ADC (12bits-1)
const int16u SCPUXMX=3700;// MAX. value provided by PUX-chip(adc chan)

const integer SCCRAT=4; // number of crates with TOF(+ANTI)-channels (S-crates)
const integer SCSLTM=11;// number of slots(all types) per crate(max)
const integer SCSLTY=5;// slot(card) types(real: sfet,sfea,sfec,sdr,spt) 
const integer SCRCHM=13;// number of readout channels (outputs) per slot(card)(max)
const integer SCRCMX=SCCRAT*SCSLTM*SCRCHM;//total hw-readout-channels(max)
const integer SCCRCMX=SCSLTM*SCRCHM;//readout channel per crate (max)
const integer SCPMOU=4;//  number pm-outputs per side (max)(anode+3dynodes)
const integer SCMTYP=5;// number of measurement types(max)(t,q,FTt,SumHTt,SumSHTt)
const integer SCAMTS=2;// number of Anode measurements types(actualy t,q) per normal bar/side
const integer SCHPMTS=3;// number of half-plane measurement types(actually FT,SumHT,sumSHT) per side
const integer SCDMTS=1;// number of Dynode measurement types(actualy q)
const integer SCFETA=5;// number of FFET+SFEA card per crate(having temp-sensors)(actual)
//
const integer DAQSFMX=3;// number of non-empty format types(raw,compressed,mixed)
//RECO
const integer SCTHMX1=8;//max TDC FTrigger-channel hits
const integer SCTHMX2=16;//max TDC SumHT(SHT)-channel (history) hits  
const integer SCTHMX3=16;//max TDC LTtime-channel hits
const integer SCTHMX4=1;//max adca(anode) hits  
const integer SCJSTA=40;   //size of Job-statistics array
const integer SCCSTA=25;   //size of Channel-statistics array
const integer SCPROFP=6;//max. parameters/side in A-profile(Apm<->Yloc) fit
const integer SCPDFBM=100;//max bins in TOF-eloss Prob Density Functions(need additional 2 for ovfls)
//
//      Calibration:
// TDIF
const integer SCTDBM=17; //max. number of coord. bins for TDIF-calibration
// AMPL 
const integer SCACMX=2000;// max. number of accum. events per channel(or bin)
const integer SCPRBM=17;//max.bins for Ampl-profile along the counter
const integer SCBTBN=SCBTPN*SCPRBM;// max. bar_types(i.e.ref.bars) * max.bins
const integer SCELFT=4;     // max. number of param. for dE/dX fit
const integer SCMCIEVM=2000;//max. events for MC A-integrator calibration 
// AVSD
const integer SCACHB=500;//max.bins in Qa for "Qd vs Qa" fit
const number SCACBW=10.;// bin width in Qa binning (adc-channels)
// STRR
const integer SCSRCHB=1200;// max. bin for dtout (2000-6000ns)
const integer SCSRCLB=400;// min. bin for dtout
const integer SCSRCHBMC=1200;// max. bin for dtout(MC)(2000-6000ns)
const integer SCSRCLBMC=400;// min. bin for dtout (MC)
const number SCSRCTB=5.;// time binning for dtout (ns)
}
//===========================================================================
// 
// --------------> Class for "time-stable"  parameters :
class TOF2DBc {  
//
private:
// geom. constants:
  static integer _brtype[TOF2GC::SCBLMX];  // layer-map of bar types (diff. by length)
  static integer _plrotm[TOF2GC::SCLRS]; // TOF planes rotation mask
  static integer _planes;                // real TOF planes
  static integer _bperpl[TOF2GC::SCLRS]; // real bars per plane
  static geant _brlen[TOF2GC::SCBTPN]; // bar lengthes for SCBTPN types
  static geant _lglen[TOF2GC::SCBTPN]; // eff.light-guide lengthes for SCBTPN types
  static geant _outcp[TOF2GC::SCLRS][4]; // outer counters param(wid/xc/yc/lgw) vs layer
  static geant _supstr[12]; // supporting structure parameters
  static geant _plnstr[20]; // sc. plane structure parameters
// MC/RECO constants:
  static geant _edep2ph;     // MC edep(Mev)-to-Photons convertion
  static geant _adc2q;       // not used (now taken from TFCA card #21) 
  static geant _fladctb;     // MC flash-ADC time-binning (ns)
  static geant _tdcscl;      // max TDC-scale
  static geant _tdcbin[4];   // TDC binning for Time(FT)TDC, supl.DAQ binning for ADC
  static geant _trigtb;      // MC time-binning in logic(trig) pulse handling
  static geant _strflu;      // Stretcher "end-mark" time fluctuations (ns)
  static geant _daqpwd[15];  // DAQ-system pulse_widths/dead_times/...
  static geant _hagemx;      // TimeTDC-hit max.age wrt FT(accepted by Actel buffer)
  static geant _hagemn;      // TimeTDC-hit min.age wrt FT(accepted by Actel buffer)
  static geant _ftc2cj;      // FT signal crate-to-crate jitter(ns)  
  static geant _fts2sj;      // .......... slot-to-slot  jitter(ns)
  static geant _ltagew[2];   // true LTtime-hit wrt FT age-window(ns) 
  static geant _ftdelm;      // FT max delay (allowed by stretcher logic) (ns)
  static geant _accdel;      // "Lev-1"(Common stop) signal delay wrt FT (ns)
  static geant _fstdcd;      // Same hit(up-edge) relative delay of slow- wrt hist-TDC
  static geant _clkper;      // Trig.electronics clock period(ns)
  static integer _pbonup;    // set phase bit "on" for leading(up) edge (yes/no->1/0)
  static integer _nsespar;   // number of PMT SingleElectrSpectrum(SES) parameters
  static geant _sespar[TOF2GC::SCBTPN][TOF2GC::SESPMX];//SES params for each btyp
//                       (for now: 1st par->(MostProb,mV on 50 Ohm);2nd->rms(relat to MP))
//
public:  

#ifdef __DB__
   friend class TOFDBcD;
#endif
//
//  Member functions :
//
// ---> function to read geomconf-file  :
  static void readgconf();

//---
  static integer brtype(integer ilay, integer ibar);
//
  static integer npmtps(integer ilay, integer ibar);
//
  static integer barseqn(integer ilay, integer ibar);
//
  static geant brlen(integer ilay, integer ibar);
//
  static geant lglen(integer ilay, integer ibar);
//
  static integer plrotm(integer ilay);
//
  static integer getbppl(integer ilay);
//
  static integer getnplns(){
    return _planes;
  }
//
  static geant supstr(int i);
//
  static geant plnstr(int i);
//
  static geant outcp(int i, int j);
//
//
//   function to get Z-position of scint. bar=ib in layer=il
  static geant getzsc(integer il, integer ib);
// function to get transv. position of scint. bar=ib in layer=il
  static geant gettsc(integer il, integer ib);
// functions to get MC/RECO parameters:
//
  static geant edep2ph();
  static geant fladctb();
  static geant tdcscl();
//
  static geant accdel();
  static geant ltagew(int i);
  static geant ftdelm();
  static geant fstdcd();
  static geant clkper();
  static geant sespar(int ibt, int ipar);
  static int nsespar();
  static geant adc2q();
  static integer pbonup();
//
  static geant ftc2cj();
  static geant fts2sj();
//
  static geant strjit2();
//
  static geant strflu();
//
  static geant hagemx();
  static geant hagemn();
//
  static geant tdcbin(int i);
//
  static geant daqpwd(int i);
//
  static geant trigtb();
//  
  static integer debug;
};
//===================================================================
// 
// --------------> Class for general "time-Variable"  parameters :
class TOF2Varp {  
//
private:
//
class TOF2Temperature{
 public:
  uinteger tofav[8];
  uinteger cr01[4];
  uinteger cr31[4];
  uinteger cr41[4];
  uinteger cr71[4];
  uinteger cr03[2];
  uinteger cr33[3];
  uinteger cr43[3];
  uinteger cr73[3];
};
// ---> TOF DAQ-system thresholds :
  geant _daqthr[5];   // DAQ-system thresholds (defaults)
// ---> Cuts :
  geant _cuts[10];                    
          //  (0) fstdw   -> t-window to identify same hit in LT/sumHT-tdc
          //  (1) hiscutb -> "befor"-cut in time history(sumHT-ch) (ns)
          //  (2) hiscuta -> "after"-cut in time history(sumHT-ch) (ns)
          //  (3) lcoerr  -> "err. in longit.coord. measurement
          //  (4) ftdelf  -> FT fixed delay
          //  (5) sftdcd  -> spare 
          //  (6) eclass  -> assim.cut for TOFCluster energy calc.
          //  (7) eclmat  -> internal long.coo matching cut ....
          //  (8) satdcg  -> 
          //  (9) sdtdcg  -> 
   static TOF2Temperature  tftt;
//
public:
  static TOF2Varp tofvpar;
  TOF2Varp(){};
// member functions :
//
  static TOF2Temperature * gettoftp(){return &tftt;}
  static integer gettoftsize(){return sizeof(tftt);}

  static geant getmeantoftemp(int crate);
  void init(geant daqthr[5], geant cuts[10]);
//
  geant fstdw(){return _cuts[0];}
  geant hiscutb(){return _cuts[1];}
  geant hiscuta(){return _cuts[2];}
  geant lcoerr(){return _cuts[3];}
  geant ftdelf(){return _cuts[4];}
  geant sftdcd(){return _cuts[5];}
  geant eclass(){return _cuts[6];}
  geant eclmat(){return _cuts[7];}
  geant satdcg(){return _cuts[8];}
  geant sdtdcg(){return _cuts[9];}
// 
  geant daqthr(int i){;
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>=0 && i<5);
      }
#endif
    return _daqthr[i];}
//
};
//===================================================================
//
class TOF2JobStat{
//
private:
  static integer mccount[TOF2GC::SCJSTA];// event passed MC-cut "i"
//          i=0 -> entries
//          i=1 => TovT->RawEvent OK
//          i=2 => Ghits->RawCluster OK
//          i=3 => Out-of-volume hit
//          i=4 => Fired TOF-bars
//          i=5 => MC flash-ADC overflows
//          i=9 => MC fTDC stack overflows
//          i=6 => MC stretch-TDC overflows
//          i=7 => MC anode-ADC overflows
//          i=8 => MC dynode-ADC overflows
//          i=10=> spare
//          i=11=> MC GHitT out of FADC
//          i=12=> MC GHitT total
//          i=13=> spare
//          i=14=> OK in MCgen fast selection
//          i=15-22=> FT-trig.finder statistics
  static integer recount[TOF2GC::SCJSTA];// event passed RECO-cut "i"
//          i=0 -> entries
//          i=1 -> H/W TOF-trigger OK
//          i=2 -> RawEv-validate OK
//           =3 -> RawEvent->RawCluster OK
//           =4 -> RawCluster->Cluster OK
//           =5 -> spare
//          i=6 -> entries to TZSl-calibration 
//           =7 -> TZSl: multiplicity OK
//           =8 -> TZSl: no interaction
//           =9 -> TZSl: Tracker mom. OK
//          =10 -> TZSl: TOF-Tracker matching OK
//          =20 -> TZSL: TOF self-matching OK
//
//          =11 -> entries to AMPL-calibration
//          =12 -> AMPL: multiplicity OK
//          =13 -> AMPL: no interaction 
//          =14 -> AMPL: Track mom. OK
//          =15 -> AMPL: TOF-TRK match OK
//          =30 -> AMPL: TOF beta-fit OK
//          =31 -> AMPL: Final check OK
//
//          =16 -> entries to STRR-calibration
//           
//          =17 -> entries to TDIF-calibration
//          =18 -> TDIF: multiplicity OK
//          =19 -> TDIF: tracker OK
//           21-25 TOF-user event-counters
//          =33 -> TOF reco with TOF in LVL1
//          =34 -> TOF reco with EC in LVL1
//
//          =38 -> entries to PEDS-calibration
//------
  static integer chcount[TOF2GC::SCCHMX][TOF2GC::SCCSTA];//channel statistics
//                              [0] -> RawSide channel entries(with FT-time!)  
//                              [1] -> "LTtdc-ON"     (Nstdc>0)
//                              [2] -> "SumHTtdc-ON"  (Nhtdc>0)
//                              [3] -> "SumSHTtdc-ON" (Nshtdc>0)
//                              [4] -> "AnodeADC-ON"  (Nadca>0)
//                              [5] -> "DynodeADC-ON" (Nadcd>0)
//                              [6] -> "FTtdc-1hit"   (Nftdc=1)
//                              [7] -> "LTtdc-1hit"   (Nstdc=1)
//                              [8] -> "(LTtdc&(Aadc|Dadc)-ON "
//                              [9] -> "Anode-adc overflow "
//                              [10]-> "Dynode-adc overflow "
//                              [11]-> "Miss.SumHT when LT and Aadc"
//
//                             [12] -> "Validate entries/channel"
//                             [13] -> "Missing FTtdc(unrecoverable)"
//                             [14] -> "Multy FTtdc-hits case"
//                             [15] -> "Missing LTtdc"
//                             [16] -> "Missing Anode"
//                             [17] -> "Missing Dynode when Anode-ovfl"
//                             [18] -> "Missing SumHT info"
//------
  static integer brcount[TOF2GC::SCBLMX][TOF2GC::SCCSTA];// bar statistics
//                               [0] -> RawEvent Bar(at least 1side) entries
//                               [1] -> "multipl-OK"
//                               [2] -> "history-OK"
//                               [3] -> "2-sided history-OK"
//------
  static integer daqsf[30];//fragment statistics
  static integer cratr[TOF2GC::SCCRAT][20];//raw fmt
  static integer cratp[TOF2GC::SCCRAT][20];//processed fmt
  static integer cratc[TOF2GC::SCCRAT][20];//on-board-pedcal fmt
  static integer cratm[TOF2GC::SCCRAT][20];//mixFMT raw/comp blocks comparison
  static integer sltr[TOF2GC::SCCRAT][TOF2GC::SCSLTM][20];
  static integer sltp[TOF2GC::SCCRAT][TOF2GC::SCSLTM][20];
  static integer rdcr1[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcr2[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcr3[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcr4[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcp1[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcp2[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcp3[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcp4[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
//
  static geant tofantemp[TOF2GC::SCCRAT][TOF2GC::SCFETA];//TOF+ANTI temperatures in crates/temp_slots 
//
public:
  static void clear();
  inline static void puttemp(int16u crt, int16u sen, geant t){
    assert(crt<TOF2GC::SCCRAT);
    assert(sen<TOF2GC::SCFETA);
    tofantemp[crt][sen]=t;
  }
  inline static geant gettemp(int16u crt, int16u sen){
    assert(crt<TOF2GC::SCCRAT);
    assert(sen<TOF2GC::SCFETA);
    return tofantemp[crt][sen];
  }
  inline static void addmc(int i){
    assert(i>=0 && i< TOF2GC::SCJSTA);
    mccount[i]+=1;
  }
  inline static void addre(int i){
    assert(i>=0 && i< TOF2GC::SCJSTA);
    recount[i]+=1;
  }
  inline static void addch(int chnum, int i){
    assert(chnum>=0 && i>=0);
    assert(chnum < TOF2GC::SCCHMX && i < TOF2GC::SCCSTA);
    chcount[chnum][i]+=1;
  }
  inline static void addbr(int brnum, int i){
    assert(brnum>=0 && i>=0);
    assert(brnum < TOF2GC::SCBLMX && i < TOF2GC::SCCSTA);
    brcount[brnum][i]+=1;
  }
  static void daqsfr(int16u ie);
  static void daqscr(int16u df, int16u crat, int16u ie);
  static void daqssl(int16u df, int16u crat, int16u slot, int16u ie);
  static void daqsch(int16u df, int16u crat, int16u slot, int16u rdch, int16u ie);
  static void printstat();
  static void bookhist();
  static void bookhistmc();
  static void outp();
  static void outpmc();
};
//===================================================================
// class to store TOF2_bar calibration constants :
class TOF2Brcal{
//
private:
  integer softid;  // LBB
  integer npmts;   // Npmts per side
  integer status[2]; //2-sides calib.status F|S|A|D -> Anode(sumHT/LTchan)/
//                                                  Anode/Dynode ADC, dec.bit=0/1->OK/Bad
  geant gaina[2]; // Anode-gain(PMT mainly)(S1/2; relative to some ref. bar of given type)
  geant gaind[2][TOF2GC::PMTSMX];//Dynode rel.gain(S1/2; for each PMT wrt aver. side-signal)       
  geant a2dr[2];  // A to D(equilized sum) ratio (in unsatur. region)
  geant asatl;  // anode_chain saturation limit(mev)(incl. PM,Shaper,...)
//                  (i.e. below use anode data, above  - dinode data)
  geant tthr;   // Time-discr. threshold (mV)
  geant strat[2][2];  // Stretcher param.[side 1/2][par.1(ratio)/2(offs)]
  geant fstrd;  // <===Don't need now ! same hit time difference between fast/slow TDC (ns)
  geant tzero;  // T0 (ns)
  geant slope; // slope for T vs (1/sqrt(Qa1)+1/sqrt(Qa2))
  geant slops[2];// indiv.slopes for Ts vs 1/Qs
  geant yctdif;// two ends time difference at counter center (ns)
  geant td2pos[2];// t_diff->position conv. factors(=Vlight,cm/ns) and coo-error(cm))
// for (at least) reference bar (in each bar type group) :
  geant mip2q;// 2-sides A-signal(pC/Mev) (at long(Y) coo=0(center))
  integer nscanp;// real number of scant points(long wdiv)
  geant yscanp[TOF2GC::SCANPNT]; // Y-positions of the scan points(bar type depend)
  geant relout[TOF2GC::SCANPNT]; // Relative(to Y=0) 2-Sides(sum) Light output
//(if some PMTs in some bar are dead, curve should be MC-calc. or measured)  
//
  geant adc2qf; // Average(tempor) Anode ADCch->Q conv.factor(pC/ch)
//  (for dynodes this factor is not required because included into a2dr)
//
  geant unipar[2*TOF2GC::SCPROFP];// responce uniformity parameters (for ref.bars)
//
//
public:
  static TOF2Brcal scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOF2Brcal(){};
  TOF2Brcal(integer sid, integer npm, integer sta[2], geant gna[2],
           geant gnd[2][TOF2GC::PMTSMX], 
           geant a2d[2], geant asl, geant tth, geant stra[2][2], geant fstd, 
           geant t0, geant sl, geant sls[2], geant tdiff, geant td2p[2],
           geant mip, integer nscp, geant ysc[], geant relo[], geant upar[], 
           geant a2q);
  void getbstat(int sta[]){
    sta[0]=status[0];
    sta[1]=status[1];
  }
// status(is)=F*100000+S*10000+A*1000+D(kji); i/j/k=1->PMDyn1/2/3 bad
//                              
  bool FchOK(int isd){
    return(status[isd]/100000==0);// means good sumHT-chan(anode)
  }
  bool SchOK(int isd){
    return((status[isd]%100000)/10000==0);// means good LTtime-chan(anode)
  }
  bool AchOK(int isd){
    return((status[isd]%10000)/1000==0);// means good Anode channel
  }
  bool DchOK(int isd, int ipm){//ipm=0-2
    int nml=(status[isd]%1000);
    if(ipm==0)return(nml%10==0);// means good Dynode ipm-chan(1)
    if(ipm==1)return((nml%100)/10==0);// means good Dynode ipm-chan(2)
    if(ipm==2)return(nml/1000==0);// means good Dynode ipm-chan(3)
    return(0);
  }
  bool SideOK(int isd){return(
                     ((status[isd]/100000)==0 || TFREFFKEY.relogic[1]>=1)
                   && (status[isd]%100000)/10000==0
		   && (status[isd]%10000)/1000==0
		   && (status[isd]%1000)==0
		                         );}
//
  void gtstrat(geant str[2][2]){
    str[0][0]=strat[0][0];
    str[0][1]=strat[0][1];
    str[1][0]=strat[1][0];
    str[1][1]=strat[1][1];
  }
  integer getnpm(){ return npmts;}
  geant gtfstrd(){return fstrd;}
  geant getasatl(){return asatl;}
  geant gettthr(){return tthr;}
  geant gettdif(){return yctdif;}
  geant getslope(){return slope;}
  geant gettzero(){return tzero;}
  geant getadc2q(){return adc2qf;}
  void getslops(geant sls[2]){
    sls[0]=slops[0];
    sls[1]=slops[1];
  }
  void getgaina(geant gn[2]){
    gn[0]=gaina[0];
    gn[1]=gaina[1];
  }
  geant getagain(int isd){
    return gaina[isd];
  }
  geant getgnd(int isd, int ipm){
    return gaind[isd][ipm];
  }
  void geta2dr(geant a2d[2]){
    a2d[0]=a2dr[0];
    a2d[1]=a2dr[1];
  }
  void getupar(geant upar[]){
    for(int i=0;i<2*TOF2GC::SCPROFP;i++){upar[i]=unipar[i];}
  }
  
  void q2a2q(int cof, int sdf, int hlf, number &adc, number &q); //Q(pC)<->ADC(chan)
  geant adc2mip(int hlf, number am[2]); // A-measured s1+s2 ampl.(ADCch) -> (mev)
  geant adcd2mip(number am[2][TOF2GC::PMTSMX]); // D-measured s1+s2 ampl.(ADCch) -> (mev)
  void  adc2q(int hlf, number am[2],number qs[2]);// meas. side ampl.(ADCch)->Q(pC)
  geant poscor(geant point); // position correction 
  geant tm2t(number tm[2], number am[2], int hlf); // raw times(ns)/ampl(ns) -> Time (ns)
  geant tm2tr(number tm[2]); // tempor(lv3-games), raw times(ns) -> Non_ampl_corrected_Time (ns)
  void tmd2p(number tm[2], number am[2], int hlf,
       geant &pos, geant &epos); // raw times/ampl -> Position/err(cm) along counter
  void td2ctd(number tdo, number am[2], int hlf, number &tdn);// time_diff(ns)->corrected(by ampl) one 
  void getd2p(geant &td2p, geant &etd2p){
    td2p=td2pos[0];
    etd2p=td2pos[1];
  }
  static void build();
};
//===================================================================
// class to store another set of MC TOF2_bar calibration constants
//  to use them as "MC Seeds" for simulation   :
class TOFBrcalMS{
//
private:
  integer softid;  // LBB
  integer status[2]; //2-sides cal.status as befor
  geant gaina[2]; // A-chain gain(PMT mainly)| (side-1/2; relative to some 
//                                                     reference bar of given type)
  geant gaind[2][TOF2GC::PMTSMX];//D(pm) rel.gain(S1/2; for each PMT wrt aver. side-signal)       
  geant a2dr[2];  // anode-to-dynode signal ratio (in unsatur. region)
  geant strat[2][2];  // Stretcher param.[side 1/2][par.1(ratio)/2(offs)]
  geant adc2qf; // Average(tempor) Anode ADCch->Q conv.factor(pC/ch)
//  (for dynodes this factor is not required because included into a2dr)
//
//
public:
  static TOFBrcalMS scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOFBrcalMS(){};
  TOFBrcalMS(integer sid, integer sta[2], geant gna[2], 
           geant gnd[2][TOF2GC::PMTSMX],
           geant a2d[2], geant stra[2][2],  geant a2q);
//
  void getbstat(int sta[]){
    sta[0]=status[0];
    sta[1]=status[1];
  }
//
// status(is)=F*100000+S*10000+A*1000+D(kji); i/j/k=1->PMDyn1/2/3 bad
//                              
  bool FchOK(int isd){
    return(status[isd]/100000==0);// means good FastTDC chan(anode)
  }
  bool SchOK(int isd){
    return((status[isd]%100000)/10000==0);// means good SlowTDC chan(anode)
  }
  bool AchOK(int isd){
    return((status[isd]%10000)/1000==0);// means good Anode channel
  }
  bool DchOK(int isd, int ipm){//ipm=0-2
    int nml=(status[isd]%1000);
    if(ipm==0)return(nml%10==0);// means good Dynode ipm-chan(1)
    if(ipm==1)return((nml%100)/10==0);// means good Dynode ipm-chan(2)
    if(ipm==2)return(nml/1000==0);// means good Dynode ipm-chan(3)
    return(0);
  }
  bool SideOK(int isd){return(
                     ((status[isd]/100000)==0 || TFREFFKEY.relogic[1]>=1)
                   && (status[isd]%100000)/10000==0
		   && (status[isd]%10000)/1000==0
		   && (status[isd]%1000)==0
		                         );}
//
  void gtstrat(geant str[2][2]){
    str[0][0]=strat[0][0];
    str[0][1]=strat[0][1];
    str[1][0]=strat[1][0];
    str[1][1]=strat[1][1];
  }
  geant getadc2q(){return adc2qf;}
  void getgaina(geant gn[2]){
    gn[0]=gaina[0];
    gn[1]=gaina[1];
  }
//  void getgaind(int ipm, geant gn[2]){
//    gn[0]=gaind[0][ipm];
//    gn[1]=gaind[1][ipm];
//  }
  geant getagain(int isd){
    return gaina[isd];
  }
  geant getgnd(int isd, int ipm){
    return gaind[isd][ipm];
  }
  void geta2dr(geant a2d[2]){
    a2d[0]=a2dr[0];
    a2d[1]=a2dr[1];
  }
  
  void q2a2q(int cof, int sdf,int hlf, number &adc, number &q); //Q(pC)<->ADC(chan)
  static void build();
};
//===================================================================
// class to store TOF_bar anode/dynode peds/sigmas  :
class TOFBPeds{
//
private:
  integer softid;  // LBB
  integer stata[2];//status for side1/2 anodes =0/1->ok/bad
  integer statd[2][TOF2GC::PMTSMX];//Dh-pmts statuses( =0/1->ok/bad) 
  geant peda[2]; // anode peds for side1/2
  geant siga[2]; // anode ped.sigmas .............................
  geant pedd[2][TOF2GC::PMTSMX]; // D-pmts peds for side1/2
  geant sigd[2][TOF2GC::PMTSMX]; // D-pmts ped.sigmas .............................
//
public:
  static TOFBPeds scbrped[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOFBPeds(){};
  TOFBPeds(integer _sid,
           integer _stata[2],
           geant _peda[2], geant _siga[2],
           integer _statd[2][TOF2GC::PMTSMX],
           geant _pedd[2][TOF2GC::PMTSMX], geant _sigd[2][TOF2GC::PMTSMX]
  ):softid(_sid){
    for(int i=0;i<2;i++){
      stata[i]=_stata[i];
      peda[i]=_peda[i];
      siga[i]=_siga[i];
      for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
        statd[i][ipm]=_statd[i][ipm];
        pedd[i][ipm]=_pedd[i][ipm];
        sigd[i][ipm]=_sigd[i][ipm];
      }
    }
  };
  bool PedAchOK(int isd){return stata[isd]==0;}
  bool PedDchOK(int isd, int ipm){return (statd[isd][ipm]==0);}
  bool PedDchOK(int isd){return (statd[isd][0]==0 && statd[isd][1]==0 && statd[isd][2]==0);}
  geant &apeda(int isd)  {return peda[isd];}  
  geant &asiga(int isd)  {return siga[isd];}
  integer &astaa(int isd)  {return stata[isd];}
  geant &apedd(int isd, int ipm)  {return pedd[isd][ipm];}  
  geant &asigd(int isd, int ipm)  {return sigd[isd][ipm];}
  integer &astad(int isd, int ipm)  {return statd[isd][ipm];}  
  
  void getpeda(geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=peda[i];
  };
  void getsiga(geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=siga[i];
  };
  
  
  void getpedd(int ipm, geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=pedd[i][ipm];
  };
  void getsigd(int ipm, geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=sigd[i][ipm];
  };
  
  static void build();
  static void mcbuild();
};
//-------------------------------------------------
class TofElosPDF{
//TOF-eloss prob.density function for particular charge 
private:
  int ichar;//0,1,...index of particle(e,p,he,..)
  int charge;//charge(1,1,2,...)
  int nbins;//distribution_length(without 2ovfl)
  geant stpx;//bin width(MeV)
  geant xmin;//1st bin low edge(MeV)
  geant slope;//exp.slope to calc. pdf in ovfl-region
  geant unpdf;//const.level of pdf in undf-region 
  geant elpdf[TOF2GC::SCPDFBM];//PDF-array
  geant bpow;//beta-dependence power 
public:
  static TofElosPDF TofEPDFs[AMSChargConst::MaxZTypes];
  TofElosPDF(){};
  TofElosPDF(int ich, int ch, geant bp, int nb, geant stp, geant bnl, geant undf, geant ovfl, geant distr[]);
  int getnbins(){return nbins;}
  int getcharge(){return charge;}
  geant getstep(){return stpx;}
  geant getbpow(){return bpow;}
  number getlkhd(int nhits, int hstat[], number ehit[], number beta);
  static void build();  
}; 
#endif

