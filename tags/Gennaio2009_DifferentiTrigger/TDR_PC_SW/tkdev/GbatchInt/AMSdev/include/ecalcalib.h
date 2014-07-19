#ifndef __AMSECCALIB__
#define __AMSECCALIB__
#include "typedefs.h"
#include "ecaldbc.h"

//  
// v1.0 E.Choumilov 08.12.2000
// class to manipulate with REUN-calib data:
//                  (REspons UNiformity)
//
// glob. conts for calibration :
//
const integer ECCLBMX=12;// max. long. bins for uniformity study(should be even)
//const integer ECLBMID=3;// Area (+-bins) from fiber center, used for PM RelGain calibr.
const integer ECLBMID=2;// Area (+-bins) from fiber center, used for PM RelGain calibr.
const integer ECCHBMX=50;// max. bins in hg-channel for h2lr study
const integer ECCADCR=100;//max. HGainADC range to use .............. 
//
class ECREUNcalib {


  private:
    static integer pxstat[ecalconst::ECPMSL][4];// PM-subcell status(-1/0/1...->unusable/ok/limited
    static number pxrgain[ecalconst::ECPMSL][4];// PM-subcell(pixel) gain(4*(relat.to averaged over PM)) 
    static number pmrgain[ecalconst::ECPMSL];   // PM gain(relative to reference PM) 
    static number pmlres[ecalconst::ECPMSL][ECCLBMX];//to store PM vs Longit.bin responce    
    static number pmlres2[ecalconst::ECPMSL][ECCLBMX];//to store PM vs Longit.bin responce err.    
    static number sbcres[ecalconst::ECPMSL][4];//to store SubCell responce    
    static number hi2lowr[ecalconst::ECPMSL][4];// h/l-gain ratios for each PM-subcell(pixel)
    static number an2dynr[ecalconst::ECPMSL];// Anode/Dynode ratios for each PM
    static number tevpml[ecalconst::ECPMSL][ECCLBMX];// tot.events/Lbin/pm_fired 
    static number tevsbf[ecalconst::ECPMSL][4];// tot.events/subcell_fired 
    static number tevsbc[ecalconst::ECPMSL][4];// tot.events/subcell_crossed 
    static number tevpmc[ecalconst::ECPMSL];// tot.events/pm_crossed 
    static number tevpmf[ecalconst::ECPMSL];// tot.events/pm_fired 
    static number tevpmm[ecalconst::ECPMSL];// tot.events/pm_crossed_at_2_central_long_bins 
    static number tevhlc[ecalconst::ECPMSL*4][ECCHBMX];// tot.events/cell/bin for Hb/Lg calibr 
    static number sbchlc[ecalconst::ECPMSL*4][ECCHBMX];// cell-resp. for Hg/Lg calibr 
    static number sbchlc2[ecalconst::ECPMSL*4][ECCHBMX];// cell-resp.**2 for Hgain/Lgain calibr 
    static number values[ECCLBMX];//working arrays for profile-fit 
    static number errors[ECCLBMX]; 
    static number coords[ECCLBMX];
    static integer nbins; 
    static number slslow;//averaged light att. slopes/%
    static number slfast;
    static number fastfr;
    static integer nfits;// counter for averaging
    static number et2momr;//aver. of Etot/Mom ratios
    static integer net2mom;// number of Etot/Mom measurements
  public:
    static void init();
    static void fill_1(integer,integer,integer,integer, number );
    static void fill_2(integer,integer,integer,number a[2]);
    static void mfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
    static void mfit();
    static void makeToyRLGAfile();
    static void select();
    static void selecte();
    static void mfite();
}; 
//---------------------------------------------------------------------

//==============================> "On-Data" Pedestal/sigma Calibration:
//
// this logic use REAL events when both (raw and compressed) formats are
// red out. The peds/sigmas are calculated at the end of the job
// and can be saved into DB and/or ped-file.
//
const integer ECPCSTMX=50;// PedCalib max.values's stack size (max)
const integer ECPCEVMN=50;//min ev/ch to calc.ped/sig(also for partial average calc)
const integer ECPCEVMX=1000;//max.statistics on events/channel
const number ECPCSIMX=2.5;//max Ped-rms to accept channel as good
const geant ECPCSPIK=10.;//signal threshold to be the "Spike"(~>1mips in Hgain chan)
//
class ECPedCalib {
  private:
    static time_t BeginTime;
    static uinteger BeginRun;
    static number adc[ecalconst::ECPMSL][5][2];
    static number adc2[ecalconst::ECPMSL][5][2];//square
    static number adcm[ecalconst::ECPMSL][5][2][ECPCSTMX];//max's  stack
    static integer nevt[ecalconst::ECPMSL][5][2];
    static integer hiamap[ecalconst::ECSLMX][ecalconst::ECPMSMX];//high signal PMTs map (1 event) 
    static geant peds[ecalconst::ECPMSL][5][2];
    static geant sigs[ecalconst::ECPMSL][5][2];
    static uinteger stas[ecalconst::ECPMSL][5][2];
    static integer nstacksz;//really needed stack size (ev2rem*ECPCEVMX)
//
    class ECPedCalib_ntpl {//to store PedTable-event(i.e.particular Run,SL,PM) in ntuple
      public:
        integer Run;
        integer SLayer;
        integer PmtNo;
        geant Pedh[5];
        geant Sigh[5];
        geant Pedl[5];
        geant Sigl[5];
        integer Stah[5];
        integer Stal[5];
    };
    static ECPedCalib_ntpl ECPedCalNT;
//
  public:
    static void init();
    static void resetb();
    static void hiamreset(){for(int i=0;i<ecalconst::ECSLMX;i++)
                                          for(int j=0;j<ecalconst::ECPMSMX;j++)hiamap[i][j]=0;
                           }
    static void fill(integer swid, geant val);
    static void filltb(integer swid, geant ped, geant sig, int16u sta);
    static void outp(int flg);
    static void outptb(int flg);
    static time_t & BTime(){return BeginTime;}
    static uinteger & BRun(){return BeginRun;}
    static void ntuple_close();
};
//-----------------------------------------------------------------
 
//----------------------------------------------------------------- 



#include "ecid.h"


class AMSECIdCalib: public AMSECIds{
protected:

static integer  _Count[ecalconst::ECPMSMX][ecalconst::ECSLMX][4][3];
static integer  _BadCh[ecalconst::ECPMSMX][ecalconst::ECSLMX][4][3];
static number   _ADC[ecalconst::ECPMSMX][ecalconst::ECSLMX][4][3];
static number   _ADCMax[ecalconst::ECPMSMX][ecalconst::ECSLMX][4][3];
static number  _ADC2[ecalconst::ECPMSMX][ecalconst::ECSLMX][4][3];
static time_t _BeginTime;
static uinteger _CurRun;

class ECCalib_def {
public:
integer Run;
integer SLayer;
integer PmtNo;
integer Channel;
integer Gain;  //high low dynode
geant   Ped;
geant   ADCMax;
geant   Sigma;
integer BadCh;
};

static ECCalib_def ECCALIB;

public:
AMSECIdCalib():AMSECIds(){};
AMSECIdCalib(const AMSECIds &o):AMSECIds(o){};
static void getaverage();
static void write();
static void clear();
static void init();
void updADC(uinteger adc,uinteger gain);
static uinteger & Run(){return _CurRun;}
static time_t & Time(){return _BeginTime;}
static void buildSigmaPed(integer n, int16u *p);
static void buildPedDiff(integer n, int16u *p);
number getADC(uinteger gain) const{return gain<3?_ADC[_pmt][_slay][_pixel][gain]:0;}
number getADCMax(uinteger gain) const{return gain<3?_ADCMax[_pmt][_slay][_pixel][gain]:0;}
number getADC2(uinteger gain) const{return gain<3?_ADC2[_pmt][_slay][_pixel][gain]:0;}
number & setADC(uinteger gain) {return _ADC[_pmt][_slay][_pixel][gain<3?gain:0];}
number & setADC2(uinteger gain) {return _ADC2[_pmt][_slay][_pixel][gain<3?gain:0];}
uinteger getcount(uinteger gain) const{return gain<3?_Count[_pmt][_slay][_pixel][gain]:0;}






};


#endif
