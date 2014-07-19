//  $Id: antidbc02.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author E.Choumilov 2.07.97
//
#ifndef __ANTI2DBC__
#define __ANTI2DBC__
//
#include "typedefs.h"
//
// Constants for Anti-Counters :
namespace ANTI2C{
 const integer ANTISRS=16;// number of physical sectors 
 const integer MAXANTI=ANTISRS/2; // number of logical(readout) sectors(8)
 const integer ANCHMX=2*MAXANTI;// number of readout channels(inputs)
 const integer ANAMTS=2;// number of Anode measurements types for normal sectors(time/charge) 
 const integer ANAGMTS=1;// number of Anode measurements types for half-cylinder(FT) 
 const integer ANAHMX=1; // max. number of anode-charge hits  per chan.
 const integer ANTHMX=16; // max. number of LTtime-hits per channel
 const integer ANFADC=2000;//Flash-ADC working channels(ovfl not included)
 const integer ANJSTA=25;// size of Job-statistics array 
 const integer ANCSTA=10;// size of Channel-statistics array
 const integer ANCRAT=4; // number of crates with SFEA cards
 const integer ANSFEA=1; // number of SFEA cards per crate
 const integer ANCHSF=16;// max. number of TDC-channels per SFEA (incl. FT-channel)
 const integer ANCHCH=4;// real number of Anti-channels per TDC-chip (impl. 2chip/SFEA)
 const integer ANSLOT=5; // SFEA card position in crate (sequential slot number(1-6))
// calibr.const:
const integer ANACMX=2000;// max. number of accum. events per channel(or bin)
const integer ANPRBM=11;//max.bins for Ampl-profile along the counter
const integer ANELFT=4;     // max. number of param. for Eloss-shape(landau) fit
const integer ANPROFP=4;//max. parameters/side in A-profile(Apm<->Yloc) fit
//
}
//
class ANTI2DBc {  
//
private:
// geom. constants:
  static geant _scradi;   // internal radious of ANTI sc. cylinder 
  static geant _scinth;   // thickness of scintillator
  static geant _scleng;   // scintillator paddle length (glob. Z-dim)
  static geant _wrapth;   // wrapper thickness 
  static geant _groovr;   // groove radious (corresponds to bump on neigb. paddle)
  static geant _pdlgap;   // inter paddle gap 
  static geant _stradi;   // support tube radious (internal)
  static geant _stleng;   //              length
  static geant _stthic;   //              thickness
//MC/RECO const:
  static geant _fadcbw;   //  MC:Flash-ADC bin width
  static geant _htdcbw;   //  bin width (ns) for LT/FT-TDC.
  static integer _daqscf; //  DAQ charge-readout scaling factor 
  static geant _ftcoinw;  // not used
  static geant _dopwmn;   //  MC: LT-discr. output pulse min.duration 
  static geant _dipwmn;   //  MC: LT-discr. input pulse min.duration 
  static geant _tgpwid;   //  "trig.pattern" pulse fixed width(ns)
  static geant _tgdtim;   //  "trig.pattern" branch dead time(ns)
  static geant _didtim;   //  input dead time of generic discr(min dist. of prev_down/next_up edges)[ns]
 //                        (i neglect by fall-time of generic discr, so DT=(11+5)ns for output signals !!!)
  static geant _tdcdtm;   //  dead time of TDC-inputs, the same for LT-/FT-inputs[ns]
  static geant _daccur;   //  generic discr. internal accuracy[ns]
  static geant _pbgate;   //  TgBox FT-gate width to latch "pattern" pulses 
public:  
//
//  Member functions :
//
  static geant scradi();
  static geant scinth();
  static geant scleng();
  static geant wrapth();
  static geant groovr();
  static geant pdlgap();
  static geant stradi();
  static geant stleng();
  static geant stthic();
  static void setgeom();
//
  static geant fadcbw();
  static geant htdcbw();
  static integer daqscf();
  static geant ftcoinw();
  static geant dopwmn();
  static geant dipwmn();
  static geant tgpwid();   
  static geant tgdtim();
  static geant didtim();
  static geant tdcdtm();
  static geant daccur();   
  static geant pbgate(); 
  static void inipulsh(int & nbn, geant arr[]);
  static void inipedisp(int & nbn, geant arr[]);
  static void displ_a(char comm[], int id, int f, const geant arr[]);
#ifdef __DB__
friend class AntiDBcD;
#endif

};
//=================================================================
// class to store individual ANTI-ReadoutPaddle Stable calib-parameters:
class ANTI2SPcal{
//
private:
  integer softid;   // readout paddle number (LogicSector:1-8)
  geant hdthr;      // Hist-channel discriminator threshold (p.e)
  geant dqthr;      // DAQ-readout threshold(ADC-ch sigmas)
  geant adc2pe;     // Signal(adc-ch)->Signal(pe's) conv.factor (pe/ADCch)
  geant tzero[2];   // 2 PhysSector-signal delays(ns, side independ) due to fib.connection
  geant ftdel[2];   // spare
  
public:
  static ANTI2SPcal antispcal[ANTI2C::MAXANTI];
  ANTI2SPcal(){};
  ANTI2SPcal(integer sid, geant ad2e, geant dthr, geant hthr, 
                                 geant tzer[2], geant ftd[2]):
                                          softid(sid),adc2pe(ad2e),
                                          dqthr(dthr),hdthr(hthr){
    tzero[0]=tzer[0];
    tzero[1]=tzer[1];
    ftdel[0]=ftd[0];
    ftdel[1]=ftd[1];
  }
  integer getid(){return softid;}
  geant getadc2pe(){return adc2pe;}
  geant getdqthr(){return dqthr;}
  geant gettzer(int iphc){return tzero[iphc];}
  geant gettzerc(){return 0.5*(tzero[0]+tzero[1]);}
//
  geant gethdthr(){
    return(hdthr);
  }
  void getftd(geant ftd[2]){
    ftd[0]=ftdel[0];
    ftd[1]=ftdel[1];
  }
  static void build();
};
//=================================================================
// class to store individual ANTI-ReadoutPaddle Variable calib-parameters:
class ANTI2VPcal{
//
private:
  integer softid;   // readout paddle number (LogicSector:1-8)
  integer status1[2];// 1st PhysSector sides-status (PM+AMPL, 1/0->dead/alive)
  integer status2[2];// 2nd PhysSector sides-status (PM+AMPL, 1/0->dead/alive)
  geant attlen[2];  // 2 PhysSectors atten.length(cm)
  geant mev2pe1[2]; // 1st PhysSector Edep->pe conv.factor(1SideSignal(pe)/0.5MIP(mev))
//                     (incl.indiv. abs. PM-amplification and clear-fiber atten)
  geant mev2pe2[2]; // 2nd PhysSector Edep->pe conv.factor(1SideSignal(pe)/0.5MIP(mev))
//                     (incl.indiv. abs. PM-amplification and clear-fiber atten)
    
public:
  static ANTI2VPcal antivpcal[ANTI2C::MAXANTI];
  ANTI2VPcal(){};
  ANTI2VPcal(integer sid, integer sta1[2], integer sta2[2], 
             geant m2pe1[2],geant m2pe2[2], geant atl[2]):softid(sid){
                                          
    status1[0]=sta1[0];
    status1[1]=sta1[1];
    status2[0]=sta2[0];
    status2[1]=sta2[1];
    attlen[0]=atl[0];
    attlen[1]=atl[1];
    mev2pe1[0]=m2pe1[0];
    mev2pe1[1]=m2pe1[1];
    mev2pe2[0]=m2pe2[0];
    mev2pe2[1]=m2pe2[1];
  }
  integer getid(){return softid;}
  geant getmev2pe1(int is){return mev2pe1[is];}
  geant getmev2pe2(int is){return mev2pe2[is];}
  geant getmev2pec(int is);
  geant getatlen(int iphc){return attlen[iphc];}
  geant getatlenc();
//
  bool CalStOK(int isd){return (status1[isd]==0 || status2[isd]==0);}
  integer getstat1(int isd){
    return status1[isd];
  }
  int NPhysSecOK();
  integer getstat2(int isd){
    return status2[isd];
  }
  static void build();
};
//=================================================================
// class to store ANTI-ReadoutPaddle peds/sigmas  :
class ANTIPeds{
//
private:
  integer softid;  // S(readout-sector number 1-8)
  integer stata[2];//status for side1/2 anodes =0/1->ok/bad
  geant peda[2]; // anode peds for side1/2
  geant siga[2]; // anode ped.sigmas .............................
//
public:
  static ANTIPeds anscped[ANTI2C::MAXANTI];
  ANTIPeds(){};
  ANTIPeds(integer _sid,
           integer _stata[2],
           geant _peda[2], geant _siga[2]):softid(_sid)
  {
    for(int i=0;i<2;i++){
      stata[i]=_stata[i];
      peda[i]=_peda[i];
      siga[i]=_siga[i];
    }
  };
  bool PedStOK(int isd){return stata[isd]==0;}
  geant &apeda(int isd)  {return peda[isd];}  
  geant &asiga(int isd)  {return siga[isd];}
  integer &astaa(int isd){return stata[isd];}
  
  void getpeda(geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=peda[i];
  };
  void getsiga(geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=siga[i];
  };
  
  static void build();
  static void mcbuild();
};
//=================================================================
// class to store ANTI-ReadoutPaddle MC-seed peds/sigmas  :
class ANTIPedsMS{
//
private:
  integer softid;  // S(readout-sector number 1-8)
  integer stata[2];//status for side1/2 anodes =0/1->ok/bad
  geant peda[2]; // anode peds for side1/2
  geant siga[2]; // anode ped.sigmas .............................
//
public:
  static ANTIPedsMS anscped[ANTI2C::MAXANTI];
  ANTIPedsMS(){};
  ANTIPedsMS(integer _sid,
           integer _stata[2],
           geant _peda[2], geant _siga[2]):softid(_sid)
  {
    for(int i=0;i<2;i++){
      stata[i]=_stata[i];
      peda[i]=_peda[i];
      siga[i]=_siga[i];
    }
  };
  bool PedStOK(int isd){return stata[isd]==0;}
  geant &apeda(int isd)  {return peda[isd];}  
  geant &asiga(int isd)  {return siga[isd];}
  integer &astaa(int isd){return stata[isd];}
  
  void getpeda(geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=peda[i];
  };
  void getsiga(geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=siga[i];
  };
  
  static void build();
};
//=================================================================
//
class ANTI2JobStat{
//
private:
  static integer mccount[ANTI2C::ANJSTA];// event passed MC-cut "i"
//          i=0 -> entries
//          i=1 -> Flash-adc buffer ovfl
//          i=2 -> Hist-tdc stack ovfl
//          i=3 -> ADC range ovfl
//          i=4 -> Hist-hit is out of search window(wrt FT)
//          i=5 -> FT/Anti-trig Coinc
//          i=6 => Ghits->RawEvent OK
//          i=7 => Common FT exist
//          i=8 -> trig-tdc stack ovfl
  static integer recount[ANTI2C::ANJSTA];// event passed RECO-cut "i"
//          i=0 -> entries
//          i=1 -> Lev-1 trigger OK
//          i=2 -> RawEv-validate OK
//           =3 -> RawEvent->RawCluster OK
//           =4 -> RawCluster->Cluster OK
//           =5 -> Using TOF in LVL1
//           =6 -> Using EC  in LVL1
//       ....=7-10 -> reco spare
//           =11-> AntiCalib entries
//           =12-> Nsectors OK
//           =13-> TRK-track found
//           =14-> Track-sector matching OK
//           =15-> TOF/TRK Z=1 
//           =16-> good matching/impact 
//           =17-> In MIP range 
//           =18-> CrossLength OK
//           =19-> PedCalib entries 
//           =20-> PedCalib-events found 
//
//------
  static integer chcount[ANTI2C::ANCHMX][ANTI2C::ANCSTA];//channel statistics
//                              [0] -> h/w-status="ON" frequency 
//                              [1] -> "ADCA-ON"  (Nadch>0)
//                              [2] -> spare
//
//                              [5] -> "h/w-ok frequency (validate)"
//                              [6] -> "No ADC hit"
//                              [7] -> spare
//                              [8] -> "bad up/down sequence in TDC"
//------
  static integer brcount[ANTI2C::MAXANTI][ANTI2C::ANCSTA];// bar statistics
//                               [0] -> h/w-status="ON" frequency
//                               [1] -> "multipl-OK"
//                               [2] -> "Good for calibr"
public:
  inline static void clear(){
    int i,j;
    for(i=0;i<ANTI2C::ANJSTA;i++)mccount[i]=0;
    for(i=0;i<ANTI2C::ANJSTA;i++)recount[i]=0;
    for (i=0;i<ANTI2C::ANCHMX;i++)
                  for(j=0;j<ANTI2C::ANCSTA;j++)
                                       chcount[i][j]=0;
    for (i=0;i<ANTI2C::MAXANTI;i++)
                  for(j=0;j<ANTI2C::ANCSTA;j++)
                                       brcount[i][j]=0;
  }
  inline static void addmc(int i){
    assert(i>=0 && i< ANTI2C::ANJSTA);
    mccount[i]+=1;
  }
  inline static void addre(int i){
    assert(i>=0 && i< ANTI2C::ANJSTA);
    recount[i]+=1;
  }
  inline static void addch(int chnum, int i){
    assert(chnum>=0 && i>=0);
    assert(chnum < ANTI2C::ANCHMX && i < ANTI2C::ANCSTA);
    chcount[chnum][i]+=1;
  }
  inline static void addbr(int brnum, int i){
    assert(brnum>=0 && i>=0);
    assert(brnum < ANTI2C::MAXANTI && i < ANTI2C::ANCSTA);
    brcount[brnum][i]+=1;
  }
  static void bookh();
  static void bookmch();
  static void printstat();
  static void outpmc();
  static void outp();
};
//===================================================================
#endif
