#include "typedefs.h"
#include "antidbc02.h"  
//  Some classes for calibrations. E.Choumilov
//-----------------------------------------------------------------------
//  class to manipulate with ANTI-calibration data :
class AntiCalib {
private:
  static number ambinl[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.adcs in low_bin(event-by-event)
  static integer nevbl[2*ANTI2C::ANTISRS];//total events, accum. in low_bin
  static number xcol[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.xcoords in low_bin(event-by-event)
  static number ambinh[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.adcs in high_bin(event-by-event)
  static integer nevbh[2*ANTI2C::ANTISRS];//total events, accum. in high_bin
  static number xcoh[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.xcoords in high_bin(event-by-event)
// for attlen calc. using r=A(is=2)/A(is=1)  
  static integer nebl[ANTI2C::ANTISRS];//low(-z) bin
  static integer nebh[ANTI2C::ANTISRS];//high(+z) bin
  static number attll[ANTI2C::ANTISRS];
  static number attlh[ANTI2C::ANTISRS];
  static number zcol[ANTI2C::ANTISRS];
  static number zcoh[ANTI2C::ANTISRS];
public:
  static void init();
  static void fill(integer secn, geant ama[2], geant coo);
//  static void mfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
//  static void melfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void select();
  static void fit();
};
//--------------------------
//
const integer ATPCSTMX=100;// PedCalib max.values's stack size (max)
const integer ATPCEVMN=50;//min ev/ch to calc.ped/sig(also for partial average calc)
const integer ATPCEVMX=1000;//max.statistics on events/channel
const geant ATPCSIMX=6.;//max Ped-rms to accept channel as good
const geant ATPCSPIK=20.;//Anode ADC-value(adc-ch) to be considered as spike(~1mip)
//
//  class to manipulate with PedSig-calibration  data:
class ANTPedCalib {
private:
  static number adc[ANTI2C::MAXANTI][2];//store Anode/Dynode adc sum
  static number adc2[ANTI2C::MAXANTI][2];//store adc-squares sum
  static number adcm[ANTI2C::MAXANTI][2][ATPCSTMX];//max. adc-values stack
  static integer nevt[ANTI2C::MAXANTI][2];// events in sum
  static geant peds[ANTI2C::MAXANTI][2];
  static geant sigs[ANTI2C::MAXANTI][2];
  static uinteger stas[ANTI2C::MAXANTI][2];
  static integer nstacksz;//really needed stack size (ev2rem*ANPCEVMX)
  static integer hiamap[ANTI2C::MAXANTI];//high signal Paddles map (1 event) 
  static time_t BeginTime;
  static uinteger BeginRun;
//
  class ANTPedCal_ntpl {
    public:
      integer Run;
      integer Sector;
      geant   PedA[2];
      geant   SigA[2];
      integer StaA[2];
  };
  static ANTPedCal_ntpl ANTPedCalNT;
//
public:
  static void init();
  static void resetb();
  static void fill(int ls, int is, geant adc);//ls->logical sector(1of8),is->side(bot/top)
  static void filltb(int ls, int is, geant ped, geant sig, int sta);//to use OnBoardTable
  static void outp(int flg);
  static void outptb(int flg);
  static void hiamreset(){
    for(int i=0;i<ANTI2C::MAXANTI;i++)hiamap[i]=0;
  } 
  static time_t & BTime(){return BeginTime;}
  static uinteger & BRun(){return BeginRun;}
  static void ntuple_close();
};
