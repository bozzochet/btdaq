//  $Id: trigger102.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMS2TRIGGER__
#define __AMS2TRIGGER__
#include "link.h"
#include "tofdbc02.h"
#include "antidbc02.h"
//
class Trigger2LVL1: public AMSlink{
protected:
//
 class ScalerMon{
  protected:
    number _FTtrig[5];//Glob,FTC,FTZ,FTE,NonPhys
    number _LVL1trig[9];//LVL1, sub1:sub8
    number _SPtrig[5];//LA0,LA1,Ext,DSP,Internal
    number _TrigTimeT;// trigger time tap
    number _CPside1[4];//4 tof-layers
    number _CPside2[4];
    number _CTside1[4];
    number _CTside2[4];
    number _BZside1[4];//4 tof-layers
    number _BZside2[4];
    number _AntiBot[8];//8 anti-sectors(logical)
    number _AntiTop[8];
    number _ECftProj[2];//EC-ft rate in x,y-proj
    number _ECl1Proj[2];//EC-lev1(angle) rate in x,y-proj
    number _ScalTimeT;//scalers time tap
    number _LiveTime[2];
    uinteger _TrigFPGAid;
    uinteger _ScalFPGAid;
  public:
    number &FTtrig(int i){return _FTtrig[i];}
    number &LVL1trig(int i){return _LVL1trig[i];}
    number &SPtrig(int i){return _SPtrig[i];}
    number &TrigTimeT(){return _TrigTimeT;}
    number &CPside1(int i){return _CPside1[i];}
    number &CPside2(int i){return _CPside2[i];}
    number &CTside1(int i){return _CTside1[i];}
    number &CTside2(int i){return _CTside2[i];}
    number &BZside1(int i){return _BZside1[i];}
    number &BZside2(int i){return _BZside2[i];}
    number &AntiTop(int i){return _AntiTop[i];}
    number &AntiBot(int i){return _AntiBot[i];}
    number &ECftProj(int i){return _ECftProj[i];}
    number &ECl1Proj(int i){return _ECl1Proj[i];}
    number &ScalTimeT(){return _ScalTimeT;}
    number &LiveTime(int i){return _LiveTime[i];}
    uinteger &TrigFPGAid(){return _TrigFPGAid;}
    uinteger &ScalFPGAid(){return _ScalFPGAid;}
    geant FTrate(){return geant(_FTtrig[0]);}
    geant FTCrate(){return geant(_FTtrig[1]);}
    geant LVL1rate(){return geant(_LVL1trig[0]);}
    geant TOFrateMX(){return _CPside1[2]>_CPside2[2]?geant(_CPside1[2]):geant(_CPside2[2]);}//imply layer3 has max rate due to its biggest size
    geant ECftrateMX(){return _ECftProj[0]>_ECftProj[1]?geant(_ECftProj[0]):geant(_ECftProj[1]);}
    geant AntirateMX(){
      number mx=0;
      for(int i=0;i<8;i++){
        number mr=_AntiTop[i]>_AntiBot[i]?_AntiTop[i]:_AntiBot[i];
	if(mr>mx)mx=mr;
      }
      return geant(mx);
    }
    void setdefs();
 };
//
 class Lvl1TrigConfig {
  protected:
   integer _tofinmask[TOF2GC::SCLRS][TOF2GC::SCMXBR];//=0/1/2/3=>Not_in_trig/both/s1/s2, z>=1 trigger
   integer _tofinzmask[TOF2GC::SCLRS][TOF2GC::SCMXBR];//=0/1/2/3=>Not_in_trig/both/s1/s2, z>=2 trigger
   integer _tofoamask[TOF2GC::SCLRS];//=0/1=>And/Or of two Plane-sides, FTC(z>=1)
   integer _tofoazmask[TOF2GC::SCLRS];//=0/1=>And/Or of two Plane-sides, BZ(z>=2)
   integer _toflut1;//accepted FT tof-layers config-1 (LookUpTabel,z>=1)
   integer _toflut2;//accepted FT tof-layers config-2 (2nd LookUpTabel, z>=1)
   integer _toflutbz;//accepted LVL1 tof-layers config(z>=2 lvl1)
   integer _toflcsz;//MN, layer-logic for FTZ(slowZ>=2),M=0/1->and/or of top and bot logic, N->top(bot)logic
   integer _tofextwid;//5_lowsignbits/next5moresignbits=>widthExtentionCode for TopTOF/BotTOF 
   integer _antinmask[ANTI2C::MAXANTI];//=0/1/2/3=>Not_in_trig/both/s1(bot)/s2(top), in_trigger mask
   integer _antoamask[ANTI2C::MAXANTI];//=0/1=>And/Or of two LogicSector-sides mask
   integer _antsectmx[2];//two maxlimits on acceptable numb. of fired ANTI-sectors(logical),equat/polar reg.
   integer _ecorand;//Ecal or/and(=1/2)->  of two projections requirements on min. numb. of fired layers
   integer _ecprjmask;//Ecal proj.mask(lkji: ij=1/0->XYproj active/disabled in FT; kl=same for LVL1(angle)
//                      i don't need here ecinmask because already have dynode bad/good status in calib. 
   integer _globftmask;//global FT sub-triggers mask(i|j|k->FTE|FTZ|FTC)
   integer _globl1mask;//global LVL1 trigger(phys.branches) mask
   integer _physbrmemb[8];//Memb.setting for each phys.branche 
   integer _phbrprescf[8];//Prescaling factors for each phys.branche
  public:
   integer &tofinmask(int il, int ib){return _tofinmask[il][ib];}
   integer &tofinzmask(int il, int ib){return _tofinzmask[il][ib];}
   integer &tofoamask(int il){return _tofoamask[il];}
   integer &tofoazmask(int il){return _tofoazmask[il];}
   integer &toflut1(){return _toflut1;}
   integer &toflut2(){return _toflut2;}
   integer &toflutbz(){return _toflutbz;}
   integer &toflcsz(){return _toflcsz;}
   integer &tofextwid(){return _tofextwid;}
   integer &antinmask(int is){return _antinmask[is];}
   integer &antoamask(int is){return _antoamask[is];}
   integer &antsectmx(int i){return _antsectmx[i];}
   integer &ecorand(){return _ecorand;}
   integer &ecprjmask(){return _ecprjmask;}
   integer &globftmask(){return _globftmask;}
   integer &globl1mask(){return _globl1mask;}
   integer &physbrmemb(int br){return _physbrmemb[br];}
   integer &phbrprescf(int br){return _phbrprescf[br];}
   void read();
 };
//
 class Scalers{//old ones !
  protected:
   uinteger _Nentries;
   uinteger _Tls[3][32];
   uinteger _GetIndex(time_t time);
  public:
   geant getsum(time_t time) {return _Tls[2][_GetIndex(time)]/96.;}
   geant getlifetime(time_t time)   {return _Tls[1][_GetIndex(time)]/16384.;}
 };
// 
 integer _PhysBPatt;//LVL1 Phys. Branches Pattern(bits 0-7)(unbiased,p,Ion,SlowIon,e,gamma, etc)
 integer _JMembPatt;//LVL1 Joined Members Pattern(bits 1-16)(FTC,FTZ,FTE,ACC0,ACC1,BZ,ECAL-F_and,...)
 integer _tofflag1;//Tof-layers contrib. in FTC(z>=1) (<0/0/1/2...=>none/all4/miss.layer#)  
 integer _tofflag2;//Tof-layers contrib. in BZ(z>=2) (<0/0/1/2...=>none/all4/miss.layer#)  
 integer _tofpatt1[TOF2GC::SCLRS];// TOF:  triggered paddles/layer pattern for z>=1(when globFT)
 integer _tofpatt2[TOF2GC::SCLRS];// TOF:  triggered paddles/layer pattern for z>=2(when globFT)
 integer _antipatt; //ANTI: pattern of FT-coincided sectors(logical)
 integer _ecalflag; //ECAL: =<0-> noTrig
 int16u _ectrpatt[6][3];//PM(dyn) trig.patt for 6"trigger"-SupLayers(36of3x16bits for 36 dynodes) 
 geant   _ectrsum;//"EC tot.energy"(total sum of all dynode channels used for trigger,gev)
 geant _LiveTime;//Live time fraction
 geant _TrigRates[6];//some TrigComponentsRates(Hz):FT,FTC,LVL1,TOFmx,ECFTmx,ANTImx
 static Scalers _scaler;
 void _copyEl(){}
 void _printEl(ostream & stream){ stream << " LiveTime " << float(_LiveTime)/1000.<<endl;}
 void _writeEl();
public:
 static Lvl1TrigConfig l1trigconf;//current TrigSystemConfiguration
 static ScalerMon scalmon;//current scalers values
 static int16u nodeids[2];//LVL1 node IDs(side a/b)
 
 Trigger2LVL1(integer PhysBPatt, integer JMembPatt, integer toffl1,integer toffl2, 
              integer tofpatt1[],integer tofpatt2[], integer antipatt, integer ecflg,
                    int16u ectrpatt[6][3], geant ectrsum, geant LiveTime, geant rates[]):
      _PhysBPatt(PhysBPatt),_JMembPatt(JMembPatt),_tofflag1(toffl1),_tofflag2(toffl2),
               _antipatt(antipatt),_ecalflag(ecflg),_ectrsum(ectrsum),_LiveTime(LiveTime){
   int i,j;
   for(i=0;i<TOF2GC::SCLRS;i++)_tofpatt1[i]=tofpatt1[i];
   for( i=0;i<TOF2GC::SCLRS;i++)_tofpatt2[i]=tofpatt2[i];
   for(i=0;i<6;i++)for(j=0;j<3;j++)_ectrpatt[i][j]=ectrpatt[i][j];
   for(i=0;i<6;i++)_TrigRates[i]=rates[i];
 }
 bool IsECHighEnergy()const {return _ecalflag/10>2;}
 bool IsECEMagEnergy()const {return _ecalflag%10==2;}
 bool GlobFasTrigOK(){return ((_JMembPatt&1)>0 || (_JMembPatt&(1<<5))>0 || (_JMembPatt&(1<<6))>0);}
 bool TofFasTrigOK(){return ((_JMembPatt&1)>0 || (_JMembPatt&(1<<5))>0);}
 bool EcalFasTrigOK(){return ((_JMembPatt&(1<<6))>0);}
 bool ExternTrigOK(){return ((_JMembPatt&(1<<14))>0);}
  static Scalers * getscalersp(){return &_scaler;}
  static integer getscalerssize(){return sizeof(_scaler);}
  geant getlivetime () const {return _LiveTime;}
 number gettrrates(int i){return _TrigRates[i];}
 integer gettoflag1() {return _tofflag1;}
 integer gettoflag2() {return _tofflag2;}
 integer getecflag() {return _ecalflag;}
 void getectrpatt(int16u patt[6][3]){for(int i=0;i<6;i++)for(int j=0;j<3;j++)patt[i][j]=_ectrpatt[i][j];}
 bool EcalDynON(int sl, int pm){//sl=0-5, pm=0-35
   int word,bit;
   word=pm/16;
   bit=pm%16;
   return((_ectrpatt[sl][word]&(1<<bit))>0);
 }
 void settofpat1(int patt[]){ for(int i=0;i<TOF2GC::SCLRS;i++)_tofpatt1[i]=patt[i];}
 void settofpat2(int patt[]){ for(int i=0;i<TOF2GC::SCLRS;i++)_tofpatt2[i]=patt[i];}
 void setecpat(int16u patt[6][3]){for(int i=0;i<6;i++)for(int j=0;j<3;j++)_ectrpatt[i][j]=patt[i][j];}
 void setectrs(geant trs){_ectrsum=trs;}
 integer getl1strpatt(){return _PhysBPatt;}
 integer checktofpattor(integer tofc, integer paddle);
 integer checktofpattand(integer tofc, integer paddle);
 integer checkantipatt(integer counter){return _antipatt & (1<<counter);}
 static void build();
 static void init();

 // Interface with DAQ
      static int16u getdaqid(int sid){return(nodeids[sid]);}      
      static integer checkdaqid(int16u id);
      static void node2side(int16u nodeid, int16u &sid);
      static integer calcdaqlength(int i){return 19;}
      static integer getmaxblocks(){return 1;}
      static void builddaq(integer i, integer n, int16u *p);
      static void buildraw(integer n, int16u *p);
#ifdef __WRITEROOT__
      friend class Level1R;
#endif
};
//-----------------------------
class TGL1JobStat{
//
private:
  static integer countev[20];
//          i=0 -> entries(MC/RD)
//          i=1 -> MC: Common FT OK
//          i=2 -> 
//          i=3 -> 
//          i=4 -> 
//          i=5 -> 
//          i=6 => 
//          i=7 =>
//         i=15 => HW-created LVL1 found
// 
  static integer daqc1[15];//daq-decoding counters
//            i=0 -> LVL1-segment entries
//             =1 -> ............ non empty
//             =2 -> ............ no assembl_errors
//             =3 -> ............ with a-side 
//             =4 -> ............ with b-side
//             =5 -> TrigPattBlock entries 
//             =6 -> ............. length OK 
//             =7 -> ScalersBlock entries 
//             =8 -> ............ length OK
//             =9 -> TrigSetupBlock entries 
//            =10 -> .............. length OK
//            =11 -> total LVL1-segment errors
//
public:
  inline static void resetstat(){
    for(int i=0;i<20;i++)countev[i]=0;
    for(int i=0;i<15;i++)daqc1[i]=0;
  }
  inline static void addev(int i){
    assert(i>=0 && i< 20);
    countev[i]+=1;
  }
  static void daqs1(integer info){
#ifdef __AMSDEBUG__
      assert(info>=0 && info<15 );
#endif
    daqc1[info]+=1;
  }
  static void printstat();
};
//---------------------
//
#endif
