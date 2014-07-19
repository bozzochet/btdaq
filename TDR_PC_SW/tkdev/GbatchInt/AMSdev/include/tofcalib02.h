//  $Id: tofcalib02.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#include "typedefs.h"
#include "tofdbc02.h"  
//  Some classes for calibrations. E.Choumilov
//
//  class to manipulate with TZSL-calibration data :
class TOF2TZSLcalib {
private:
  static geant slope;
  static geant tzero[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  static number s1;
  static number s3[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  static number s4;
  static number s6[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
  static number s7[TOF2GC::SCMXBR];
  static number s8;
  static number s12[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
  static number s13[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
  static number s14[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
  static number s15[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
  static number s16[TOF2GC::SCMXBR];
  static number events;
  static number resol;
public:
  static void init(){
    int i,j,il,ib;
    slope=0.;
    s1=0.;
    s4=0.;
    s8=0.;
    events=0.;
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      for(il=0;il<TOF2GC::SCLRS;il++){
        tzero[il][ib]=0.;
        s3[il][ib]=0.;
      }
      for(i=0;i<(TOF2GC::SCLRS-1);i++){
        s6[i][ib]=0.;
        s15[i][ib]=0.;
      }
      s7[ib]=0.;
      s16[ib]=0.;
      for(j=0;j<TOF2GC::SCMXBR;j++){
        s12[ib][j]=0.;
        s13[ib][j]=0.;
        s14[ib][j]=0.;
      }
    }
  };
  static void fill(int ib[4],number tdi[3],number dum[3]);
  static void mfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void mfit();
  static void select();
  static geant getslop(){return slope;};
  static void gettzer(geant arr[]){
    int cnum;
    for(int il=0;il<TOF2GC::SCLRS;il++){
    for(int ib=0;ib<TOF2GC::SCMXBR;ib++){
      cnum=il*TOF2GC::SCMXBR+ib;
      arr[cnum]=tzero[il][ib];
    }
    }
  };
};
//------------------------------------------------------------------------
//
//  class to manipulate with TDIF-calibration data :
class TOF2TDIFcalib {
private:
  static integer nbins[TOF2GC::SCBTPN];//numb.of coord-bins vs bar-type(<=TOF2GC::SCTDBM)
  static number tdiff[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// side-times differences (ns)
  static number tdif2[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// square of ...
  static number clong[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// impact longit.coordinates(cm)
  static integer nevnt[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// event counters
public:
  static void init();
  static void fill(integer il, integer ib, number td, number coo);
  static void select();
  static void fit();
};
//-----------------------------------------------------------------------
//  class to manipulate with AMPL-calibration data :
class TOF2AMPLcalib {
private:
  static number ambin1[TOF2GC::SCBTBN][TOF2GC::SCACMX];// s1-signals for each ref_bar/bin/event
  static integer nevenb1[TOF2GC::SCBTBN];// s1 events accum. per ref_bar/bin for ambin
  static number ambin2[TOF2GC::SCBTBN][TOF2GC::SCACMX];// s2-signals for each ref_bar/bin/event
  static integer nevenb2[TOF2GC::SCBTBN];// s2 events accum. per ref_bar/bin for ambin
  static number amchan[TOF2GC::SCCHMX][TOF2GC::SCACMX];// side-signals for each channel/event
  static integer nevenc[TOF2GC::SCCHMX];// numb.of events accum. per channel for amchan
  static geant gains[TOF2GC::SCCHMX];//trunc. mean ch.signals ("0" impact) relat. to ref.ones
  static geant btamp[2][TOF2GC::SCBTBN];// MostProb bar-signals for each side/bin(ref.bars) 
  static geant ebtamp[2][TOF2GC::SCBTBN];// Error in MostProb  for each side/bin(ref.bars) 
  static integer rbls[TOF2GC::SCBTPN];// list of ref. bars (diff. by bar type)(LBB)
  static geant profb[TOF2GC::SCBTPN][TOF2GC::SCPRBM];// profile bins width(counting from "-" side)
  static geant profp[TOF2GC::SCBTPN][TOF2GC::SCPRBM];// bin middle-point values
  static integer nprbn[TOF2GC::SCBTPN];// real number of prof.bins according to bar-type
  
  static number a2dr[TOF2GC::SCCHMX];//sum of An/Sum(Dyn(pmt))-ratios for each channel
  static number a2dr2[TOF2GC::SCCHMX];//sum of ratios**2 for each channel
  static integer neva2d[TOF2GC::SCCHMX];//number of events/channel for above sum
  static number d2sdr[TOF2GC::SCCHMX][TOF2GC::PMTSMX];//sum of Dyn(pm)/Sum(Dyn(pm))-gains for LBBS
  static number d2sdr2[TOF2GC::SCCHMX][TOF2GC::PMTSMX];//sum of ratios**2 for each LBBS
  static integer nevdgn[TOF2GC::SCCHMX];//number of events/channel for above sum
  
  static number ammrfc[TOF2GC::SCBTPN];// mean charge (central incidence) for ref.counters
  static integer nevrfc[TOF2GC::SCBTPN];// number of accum. events for above sum
  static number arefb[TOF2GC::SCBTPN][TOF2GC::SCACMX];// tot-signals for each ref_bar/event
  static integer nrefb[TOF2GC::SCBTPN];// numb.of events accum. per ref_bar
  static geant fpar[10];// parameters to fit
  static integer nbinr;// working var. for A-profile fit
  static integer iside;// working var. for A-profile fit
  static geant fun2nb;// working var. for  fit
  static number mcharge[TOF2GC::SCPRBM]; 
  static number mcoord[TOF2GC::SCPRBM];
  static number emchar[TOF2GC::SCPRBM];
  static number aprofp[TOF2GC::SCBTPN][2*TOF2GC::SCPROFP];// A-profile parameters(ref.bars)
  static number clent;//bar-length (to exchange with FCN)
  static integer cbtyp;//bar-type  (....................)
  static integer fitflg;//flag     (....................)
//
  static geant binsta[50];// bin-statistics arrays for dE/dX fit
  static geant bincoo[50];// bin-coordinate ...
  static integer elbt;//     tot.bins
  static geant elfitp[TOF2GC::SCELFT];// fit-parameters
  static char eltit[60];  // title for fit
public:
  static void init();
  static void fill(integer il, integer ib, geant ama[2], geant coo);
  static void fillabs(integer il, integer ib, geant ama[2], geant coo, number mom,
                                                                     number btof);
  static void filla2dg(int il, int ib, geant cin, geant ama[2], geant amd[2][TOF2GC::PMTSMX]);
  
  static void mfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void melfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void select();
  static void fit();
};
//-----------------------------------------------------------------------
//
//  class to manipulate  AvsD-calibration data :
class TOF2AVSDcalib {
private:
  static number dtdyn[TOF2GC::SCCHMX][TOF2GC::SCACHB];// to calc.mean adcd per chan/Abin 
  static number dtdyn2[TOF2GC::SCCHMX][TOF2GC::SCACHB];// to calc.mean (adcd)**2 per chan/Abin
  static integer nevdyn[TOF2GC::SCCHMX][TOF2GC::SCACHB];// events in above sums per chan/Abin
  static integer nevdynt[TOF2GC::SCCHMX];// events in above sums per chan
public:
  static void init();
  static void filla2dr(integer chan, geant adca, geant adcd);
  static void fit(number a2d[], number a2do[], number &av, number &avo);
};
//-----------------------------------------------------------------------
//  class to manipulate with STRR-calibration (StretcherRatio) data:
class TOF2STRRcalib {
private:
  static number dtin[TOF2GC::SCCHMX][TOF2GC::SCSRCHB]; // to calc. mean dtin per chan/bin (150 bins x 1ns)
  static number dtinq[TOF2GC::SCCHMX][TOF2GC::SCSRCHB];//to calc. mean square dtin per chan/bin
  static integer nevnt[TOF2GC::SCCHMX][TOF2GC::SCSRCHB];// events per chan/bin 
  static integer nevtot[TOF2GC::SCCHMX];    // total number of analized events/chan
  static integer nevnt2[TOF2GC::SCCHMX];// event counters for f/s-tdc time difference
  static number fstdif[TOF2GC::SCCHMX];// f/s-tdc time difference
  static number fstdif2[TOF2GC::SCCHMX];// squares ....
  static number sbins[TOF2GC::SCCHMX];// for "points-fit" method
  static number ssumc[TOF2GC::SCCHMX];
  static number ssumt[TOF2GC::SCCHMX];
  static number ssumid[TOF2GC::SCCHMX];
  static number ssumct[TOF2GC::SCCHMX];
  static number ssumc2[TOF2GC::SCCHMX];
  static number ssumt2[TOF2GC::SCCHMX];
public:
  static void init();
  static void fill(integer ichan, number tm[3]);
  static void fill2(integer ichan, number tdif);
  static void outp(); 
};
//-----------------------------------------------------------------------
//===============> TOF PedCalib:
//
const integer TFPCSTMX=100;// PedCalib max.values's stack size (max)
const integer TFPCEVMN=50;//min ev/ch to calc.ped/sig(also for partial average calc)
const integer TFPCEVMX=1000;//max.statistics on events/channel
const geant TFPCSIMX=6.;//max Ped-rms to accept channel as good
const geant TFPCSPIK=50.;//Anode ADC-value(adc-ch) to be considered as spike(~1mip)
//
//  class to manipulate with PedSig-calibration  data:
class TOFPedCalib {
private:
  static number adc[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];//store Anode/Dynode adc sum
  static number adc2[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];//store adc-squares sum
  static number adcm[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1][TFPCSTMX];//max. adc-values stack
  static integer nevt[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];// events in sum
  static geant peds[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
  static geant sigs[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
  static uinteger stas[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
  static integer nstacksz;//really needed stack size (ev2rem*TFPCEVMX)
  static integer hiamap[TOF2GC::SCLRS][TOF2GC::SCMXBR];//high signal Paddles map (1 event) 
  static time_t BeginTime;
  static uinteger BeginRun;
//
  class TOFPedCal_ntpl {
    public:
      integer Run;
      integer Layer;
      integer Paddle;
      geant   PedA[2];
      geant   SigA[2];
      geant   PedD[2][3];
      geant   SigD[2][3];
      integer StaA[2];
      integer StaD[2][3];
  };
  static TOFPedCal_ntpl TOFPedCalNT;
//
public:
  static void init();
  static void resetb();
  static void fill(int il, int ib, int is, int pmt, geant adc);//pmt=0/1,2,3=>anode/dynode1,2,3
  static void filltb(int il,int ib,int is,int pmt,geant pd,geant sg,int st);//to store OnBoardPedTable elems
  static void outp(int flg);
  static void outptb(int flg);//to manipulation with final OnBoardPedTable
  static void hiamreset(){
    for(int i=0;i<TOF2GC::SCLRS;i++){
      for(int j=0;j<TOF2GC::SCMXBR;j++)hiamap[i][j]=0;
    }
  } 
  static time_t & BTime(){return BeginTime;}
  static uinteger & BRun(){return BeginRun;}
  static void ntuple_close();
};
//--------------------------
