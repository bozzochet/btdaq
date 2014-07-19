//  $Id: tofsim02.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author Choumilov.E. 10.07.96.
// Removed gain5 logic, E.Choumilov 22.08.2005
#ifndef __AMSTOF2SIM__
#define __AMSTOF2SIM__
#include "upool.h"
#include "apool.h"
#include "link.h"
#include "typedefs.h"
#include "tofdbc02.h"
#include <stdlib.h>
#include <time.h>
//=======================================================================
//              <--- class for distribution manipulation
class AMSDistr{
//
private:
  int nbin; // number of bins in distribution
  geant bnl;   // first bin low edge 
  geant bnw;   // bin width
  geant distr[TOFGC::AMSDISL]; // array of bins contents (integr. distribution !)
//
public:
  AMSDistr(){};
  ~AMSDistr(){};
  AMSDistr(int _nbin, geant _bnl, geant _bnw, geant arr[]);
//external array "arr" is differential(!!!) distribution. Leading, middle 
//  or trailing zeroes are accepted.
//
geant getrand(const geant &); // to get rand. value according to distribution,
//              inp.parameter have to be uniform random value in [0,1] range.
//function to get nbin,bnl,bnw :
inline void getpar(int &_nbin, geant &_bnl, geant &_bnw)
  {
  _nbin=nbin;
  _bnl=bnl;
  _bnw=bnw;
  }
//
};
//=======================================================================
//  <--- to store time-points, corresp. to integral 0-1 with bin 1/(SCTPNMX-1)
class TOFTpoints{
//
private:
  geant bini;// bin width in integr.distr
  geant times[TOF2GC::TOFPNMX+1]; // array of time points
//
public:
  TOFTpoints(){};
 ~TOFTpoints(){};
  TOFTpoints(int _nbin, geant _bnl, geant _bnw, geant arr[]);
//
//function to get time-point for given value of integral r(0-1) :
  geant gettime(geant r){
    integer i;
    geant ri,t;
    ri=r/bini;
    i=integer(ri);
    if(i>TOF2GC::TOFPNMX)i=TOF2GC::TOFPNMX;
    if(i<TOF2GC::TOFPNMX)t=times[i]+(times[i+1]-times[i])*(ri-geant(i));
    else t=times[i];
    return t;
  }
//
  void getarr(geant arr[]){
    for(int i=0;i<=TOF2GC::TOFPNMX;i++)arr[i]=times[i];
  }
  geant getbini(){ return bini;}
};
//=====================================================================
//         <--- class for MC single width-dvision t/eff-distributions
class TOF2Scan{
//
private:
  int nscpnts;      // real number of scan points in given w-division
  int indsp[TOF2GC::SCANPNT]; //array of long.scan-points indexes(refered to wdiv=1)
  geant scanp[TOF2GC::SCANPNT]; //array of longit. scan points
  geant eff1[TOF2GC::SCANPNT];  //effic. for -X side PMT
  geant eff2[TOF2GC::SCANPNT];  //effic. for +X side PMT
  geant rgn1[TOF2GC::SCANPNT];  //PM rel.gain for -X side PMT
  geant rgn2[TOF2GC::SCANPNT];  //PM rel.gain for +X side PMT
  geant psh1[TOF2GC::SCANPNT][TOF2GC::PMTSMX];// ph.el. sharing over PMTs (s1)
  geant psh2[TOF2GC::SCANPNT][TOF2GC::PMTSMX];// ph.el. sharing over PMTs (s2)
  TOFTpoints tdist1[TOF2GC::SCANPNT];// t-points arrays for -X side
  TOFTpoints tdist2[TOF2GC::SCANPNT];// t-points arrays for +X side
public:
  TOF2Scan(){};
  TOF2Scan(const integer nscp, const int _indsp[], const geant _scanp[], 
                         const geant _eff1[], const geant _eff2[],
                         const geant _rgn1[], const geant _rgn2[],
			 const geant _psh1[][TOF2GC::PMTSMX],const geant _psh2[][TOF2GC::PMTSMX],
                         const TOFTpoints _tdist1[], const TOFTpoints _tdist2[])
  {
    nscpnts=nscp;
    int i,j,k;
    for(i=0;i<nscp;i++)indsp[i]=_indsp[i];
    for(i=0;i<nscp;i++)scanp[i]=_scanp[i];
    for( j=0;j<nscp;j++)eff1[j]=_eff1[j];
    for( k=0;k<nscp;k++)eff2[k]=_eff2[k];
    for( j=0;j<nscp;j++)rgn1[j]=_rgn1[j];
    for( k=0;k<nscp;k++)rgn2[k]=_rgn2[k];
    for( k=0;k<nscp;k++)for(i=0;i<TOF2GC::PMTSMX;i++)psh1[k][i]=_psh1[k][i];
    for( k=0;k<nscp;k++)for(i=0;i<TOF2GC::PMTSMX;i++)psh2[k][i]=_psh2[k][i];
    for(int l=0;l<nscp;l++)tdist1[l]=_tdist1[l];
    for(int m=0;m<nscp;m++)tdist2[m]=_tdist2[m];
  };
void getybin(const geant , int &, int &, geant &);// calc. indexes of Y-position
//
void getscp(geant scp[]){
  for(int i=0;i<nscpnts;i++)scp[i]=scanp[i];
}
//
void getidsp(int iscp[]){
  for(int i=0;i<nscpnts;i++)iscp[i]=indsp[i];
}
//
void getefarr(geant ef1[], geant ef2[]){
  int i;
  for(i=0;i<nscpnts;i++)ef1[i]=eff1[i];
  for(i=0;i<nscpnts;i++)ef2[i]=eff2[i];
}
//-------
integer getnscp(){return nscpnts;}
// <--- functions to get efficiency ( getxbin should be called first!!!)
geant getef1(const geant r, const int i1, const int i2){
  if(i1==i2){return eff1[i1];}
  else{return (eff1[i1]+(eff1[i2]-eff1[i1])*r);}
}
geant getgn1(const geant r, const int i1, const int i2){
  if(i1==i2){return rgn1[i1];}
  else{return (rgn1[i1]+(rgn1[i2]-rgn1[i1])*r);}
}
geant getps1(const int ipm, const geant r, const int i1, const int i2){
  if(i1==i2){return psh1[i1][ipm];}
  else{return (psh1[i1][ipm]+(psh1[i2][ipm]-psh1[i1][ipm])*r);}
}
geant getef2(const geant r, const int i1, const int i2){
  if(i1==i2){return eff2[i1];}
  else{return (eff2[i1]+(eff2[i2]-eff2[i1])*r);}
}
geant getgn2(const geant r, const int i1, const int i2){
  if(i1==i2){return rgn2[i1];}
  else{return (rgn2[i1]+(rgn2[i2]-rgn2[i1])*r);}
}
geant getps2(const int ipm, const geant r, const int i1, const int i2){
  if(i1==i2){return psh2[i1][ipm];}
  else{return (psh2[i1][ipm]+(psh2[i2][ipm]-psh2[i1][ipm])*r);}
}
//-------
//get PM time using t-distr. arrays for closest (to Y) sc.points
//    Need getybin to be called first !!!
geant gettm1(const geant r, const int i1, const int i2);    
geant gettm2(const geant r, const int i1, const int i2);    
//  
};
//=====================================================================
//        <--- general class for counter MC-scan(width+leng.scan.points)
class TOFWScan{
//
private:
  int npmts; //real number of PMTs per side
  int nwdivs; // real number of width-divisions
  geant divxl[TOF2GC::SCANWDS];//division xlow
  geant divxh[TOF2GC::SCANWDS];//division xhigh
  TOF2Scan wscan[TOF2GC::SCANWDS];//array of single-division scans
public:
  static TOFWScan scmcscan[TOF2GC::SCBTPN];
  TOFWScan(){};
  TOFWScan(const int npm, const int nwdvs, 
                         const geant _dxl[], const geant _dxh[], 
                         const TOF2Scan  _sdsc[])
  {
    npmts=npm;
    nwdivs=nwdvs;
    int i;
    for(i=0;i<nwdvs;i++){
      divxl[i]=_dxl[i];
      divxh[i]=_dxh[i];
      wscan[i]=_sdsc[i];
    }
  };
//
  int getnpmts(){
    return(npmts);
  }
  int getndivs(){
    return(nwdivs);
  }
//
  integer getnscp(int id){// get length array of scan-points for wdiv=id
    return wscan[id].getnscp();
  }
//
  void getscp(int id, geant scp[]){// get array of scan-points(wdiv=id)
    wscan[id].getscp(scp);
  }
//
  void getidsp(int id, int iscp[]){// get array of scan-points indexes(wdiv=id)
    wscan[id].getidsp(iscp);
  }
//
//   Func.to calc.X(width)-division index for given X(counter local r.s)
//
  int getwbin(const geant x){
    int div(0);
    if((divxl[0]-x)>0.01)return(-1);
    if((x-divxh[nwdivs-1])>0.01)return(-1);
    if(x<divxl[0])return(div);
    if(x>=divxl[nwdivs-1])return(nwdivs-1);
    for(int i=0;i<nwdivs-1;i++){
      if(x>=divxl[i] && x<divxl[i+1])div=i; 
    }
    return(div);
  }
//
//   Func. to calc.Y-pos indexes for division div:
//
  void getybin(const int div, const geant y, int &i1, int &i2, geant &r){
    wscan[div].getybin(y, i1, i2, r);
  }
//
//   Func. to calc. pm1/pm2 eff for division div:
//
  geant getef1(const int div, const geant r, const int i1, const int i2){
    return(wscan[div].getef1(r, i1, i2));
  }
  geant getef2(const int div, const geant r, const int i1, const int i2){
    return(wscan[div].getef2(r, i1, i2));
  }
//
//   Func. to calc. pm1/pm2 gain for division div:
//
  geant getgn1(const int div, const geant r, const int i1, const int i2){
    return(wscan[div].getgn1(r, i1, i2));
  }
  geant getgn2(const int div, const geant r, const int i1, const int i2){
    return(wscan[div].getgn2(r, i1, i2));
  }
  geant getps1(const int ipm, const int div, const geant r, const int i1, const int i2){
    return(wscan[div].getps1(ipm, r, i1, i2));
  }
  geant getps2(const int ipm, const int div, const geant r, const int i1, const int i2){
    return(wscan[div].getps2(ipm, r, i1, i2));
  }
//
//   Func. to calc. pm1/pm2 times for division div:
//
  geant gettm1(const int div, const geant r, const int i1, const int i2){
    return(wscan[div].gettm1(r, i1, i2));
  }
  geant gettm2(const int div, const geant r, const int i1, const int i2){
    return(wscan[div].gettm2(r, i1, i2));
  }
//
  static void build();
//  
};
//=========================================================================== 
//         <--- class to store time_over_threshold hits during MC-digitization:
//
class TOF2Tovt: public AMSlink{
//
private:
static number itovts[TOF2GC::SCMCIEVM];
static number icharg[TOF2GC::SCMCIEVM];
static integer ievent;
integer idsoft;  // LBBS
integer status; // channel is alive/dead/ ... --> 0/1/...
number charga;  // total anode charge (pC)
number tedep;   // total Edep(mev)
integer ntr1;
number ttr1u[TOF2GC::SCTHMX1];   // store up-times of "z>=1(FT)" trigger signals 
number ttr1d[TOF2GC::SCTHMX1];   // store down-times of "z>=1(FT)" trigger signals 
integer ntr3;
number ttr3u[TOF2GC::SCTHMX1];   // store up-times of "z>=2" trigger signals 
number ttr3d[TOF2GC::SCTHMX1];   // store down-times of "z>=2" trigger signals 
integer nstdc;
number tstdc[TOF2GC::SCTHMX3];  // store up-times of pulses of TDC LT-time channel 
number adca;  // store ADC-counts(float) for Anode channel
integer nadcd;
number adcd[TOF2GC::PMTSMX];  // store ADC-counts(float) for Dynodes channels
//
public:
static geant SumHTt[TOF2GC::SCCRAT][TOF2GC::SCFETA-1][TOF2GC::SCTHMX2];//TDC SumHT-channel time-hits(vs CRATE/SFET)
static int SumHTh[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];// number of TDC SumHT-channel time-hits 
static geant SumSHTt[TOF2GC::SCCRAT][TOF2GC::SCFETA-1][TOF2GC::SCTHMX2];//TDC SumSHT-channel time-hits(vs CRATE/SFET)
static int SumSHTh[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];// number of TDC SumSHT-channel time-hits
// 
TOF2Tovt(integer _ids, integer _sta, number _charga, number _tedep,
  integer _ntr1, number _ttr1u[], number _ttr1d[], integer _ntr3, number _ttr3u[], number _ttr3d[],
  integer _nstdc, number _tstdc[], number _adca, integer _nadcd, number _adcd[]);
//
~TOF2Tovt(){};
TOF2Tovt * next(){return (TOF2Tovt*)_next;}
integer getid() const {return idsoft;}
number getcharg(){return charga;}
number getedep(){return tedep;}
integer gettr1(number arru[], number arrd[]);
integer gettr3(number arru[], number arrd[]);
integer getstdc(number arr[]);
number  getadca();
integer getadcd(number arr[]);
integer getstat(){return status;}
static void inipsh(integer &nbn, geant arr[]);
static geant pmsatur(const geant am);
static void displ_a(char comm[], int id, int mf, const geant arr[]);
static number ftctime(int &trcode,int &cpcode);//to get FTC("z>=1") trigger time/code/lutcode 
static number ftztime(int &trcode);//to get FTZ("SlowZ>=2") trigger time/code
static void spt2patt(number ftime, integer patt1[], integer patt2[]);//to get z>=1/z>=2 tof-patterns of SPT2
static bool bztrig(number gftime, int &trcode);//to get BZ-flag for LVL1 and BZ(z>=2) layers patt. 
static void build();
static void totovt(integer id, geant edep, geant tslice[],geant shar[]);//flash_ADC_array->Tovt
//
protected:
void _printEl(ostream &stream){stream <<"TOF2Tovt: "<<idsoft<<endl;}
void _writeEl(){};
void _copyEl(){};
};
//=========================================================================== 
//         <--- class for bitstream manipulations :
//
class AMSBitstr{
//
private:
  static geant clkfas;//trig.electronics clock phase(0-1 random, but the same for whole event)
  unsigned short int bitstr[TOFGC::SCWORM]; // max. length in 16-bit words
  int bslen; // real length in 16-bits words (<=def=TOFGC::SCWORM)
//
public:
// constructors:
  AMSBitstr():bslen(TOFGC::SCWORM){for(int i=0;i<bslen;i++)bitstr[i]=0;};
  AMSBitstr(const int _bslen, const unsigned short int arr[]){
    if(_bslen>TOFGC::SCWORM){
      bslen=TOFGC::SCWORM;
      cout<<"AMSBitstr::warning: init.array is too long !!!"<<'\n';
    }
    else{
      bslen=_bslen;
    }
    for(int i=0;i<bslen;i++)bitstr[i]=arr[i];
  }
  AMSBitstr(const int _bslen){
    if(_bslen>TOFGC::SCWORM){
      bslen=TOFGC::SCWORM;
      cout<<"AMSBitstr::warning: init.array is too long !!!"<<'\n';
    }
    else{
      bslen=_bslen;
    }
    for(int i=0;i<bslen;i++)bitstr[i]=0;
  }
  void bitset(const int il, const int ih);
  void bitclr(const int il, const int ih);
  void clatch();
  void testbit(int &i1, int &i2);
  static void setclkphase();
  int getbslen() const{return (bslen);}
  inline void getbsarr(unsigned short int arr[]) const{
    for(int i=0;i<bslen;i++)arr[i]=bitstr[i];
  }
//
  void display(char arr[]);
//
  AMSBitstr(const AMSBitstr & other){ // copy constructor
    bslen=other.bslen;
    for(int i=0;i<bslen;i++)bitstr[i]=other.bitstr[i];
  }
//
  AMSBitstr operator =(const AMSBitstr & other){
    if(this == &other)return (*this);
    bslen=other.bslen;
    for(int i=0;i<bslen;i++)bitstr[i]=other.bitstr[i];
    return (*this);
  }
//
  friend AMSBitstr operator & (const AMSBitstr &, const AMSBitstr &);
  friend AMSBitstr operator | (const AMSBitstr &, const AMSBitstr &);
};
//====================================================================
//  class for TOF RawSide hits (all digital data for given id(LBBS) are groupped)
//
class TOF2RawSide: public AMSlink{
//
private:
 static uinteger StartRun;//first run of the job
 static time_t StartTime;//first run time
 static integer _trcode; //CP's layer-pattern code(<0 ->noFTC, >=0 -> as trcode, z>=1)
 static integer _trcodez; //BZ's layer-pattern code(<0-> >=0 -> as trcode, z>=2)
 static integer _trpatt[TOF2GC::SCLRS];// Fired bars pattern(z>=1)
 static integer _trpattz[TOF2GC::SCLRS];// Fired bars pattern(z>=2)
 static integer _ftpatt;//general FT components pattern(starting from lsbit:FTC/FTZ/FTE/EXT)
 static integer _cpcode;//MN-> FTCP1|FTCP0 flags for lvl1(m(n)=0/1)
 static integer _bzflag;//1/0->BZtrigOK/not
 static number _trtime; //  abs. FTrigger time(ns, input to SFET(A), fix decision-delay included) 
 int16u _swid;        // short SW-id(LBBS->Lay|BarBar|Side (as in Tovt MC-obj))
 int16u _hwid;          // short HW-id(CSS->cr|sl, slot by Anode meas.,if it is present,otherw. by Dynode)
 int16u _status;        // channel status (usable/not/ ... --> 0/1/...)
 
 integer _nftdc;//number of FastTrig(FT)-TDC hits, =1 in MC(filled at validation stage !!)
 integer _ftdc[TOF2GC::SCTHMX1]; // FastTrig(FT)-TDC hits(TDC-chan),half-plane specific, but stored for each pad.
 integer _nstdc;         // number of LTTime-TDC hits
 integer _stdc[TOF2GC::SCTHMX3]; // Time-TDC hits ("LT-time",TDC channels)
 integer _nsumh;//number of SumHT time-hits(filled at validation stage !!)
 integer _sumht[TOF2GC::SCTHMX2];//SumHT time-hits(TDC-channelss) 
 integer _nsumsh;//number of SumSHT time hits(filled at validation stage !!)
 integer _sumsht[TOF2GC::SCTHMX2];//SumSHT time-hits(TDC-channelss)
  
 geant _adca; // Anode ADC-hit (ped-subtracted if not PedCalib job)
 integer _nadcd;// number of NONZERO Dynode-channels(max PMTSMX)
 geant _adcd[TOF2GC::PMTSMX];// ALL Dynodes PMTs ADC-hits(positional, ped-subtracted if not PedCalib job)
 
 geant _charge;         // for MC : tot. anode charge (pC)
 geant _temp;           // SFET(A) temperature corresponding to LBBS(filled at validation stage !!)
//
public:
 static integer FTtime[TOF2GC::SCCRAT][TOF2GC::SCFETA][TOF2GC::SCTHMX1];//FT-channel time-hits(incl.ANTI)
 static integer FThits[TOF2GC::SCCRAT][TOF2GC::SCFETA];//number of FT-channel hits ....... 
 static integer SumHTt[TOF2GC::SCCRAT][TOF2GC::SCFETA-1][TOF2GC::SCTHMX2];//TOF SumHT-channel time-hits
 static integer SumHTh[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];// number of SumHT-channel time-hits 
 static integer SumSHTt[TOF2GC::SCCRAT][TOF2GC::SCFETA-1][TOF2GC::SCTHMX2];//TOF SumSHT-channel time-hits
 static integer SumSHTh[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];// number of SumSHT-channel time-hits
 static integer Out(integer);
// 
 TOF2RawSide(int16u swid, int16u hwid, int16u sta, geant charge, geant temp,
   integer nftdc, integer ftdc[],
   integer nstdc, integer stdc[],
   integer nsumh, integer sumht[],
   integer nsumsh, integer sumsht[],
   geant adca,
   integer nadcd, geant adcd[TOF2GC::PMTSMX]);
 ~TOF2RawSide(){};
 TOF2RawSide * next(){return (TOF2RawSide*)_next;}

#ifdef __WRITEROOT__
   friend class TofRawSideR;
#endif

 integer operator < (AMSlink & o)const{
  return _swid<((TOF2RawSide*)(&o))->_swid;}

 int16u getsid(){return _swid;}
 int16u gethid(){return _hwid;}
 int16u getstat(){return _status;}
 void updstat(int16u sta){_status=sta;}
 geant getcharg(){return _charge;}
 geant gettemp(){return _temp;}
 void settemp(geant tmp){_temp=tmp;}
 integer gettoth(){return integer(_nftdc+_nstdc+_nsumh+_nsumsh+1+_nadcd);}


 integer getnztdc();
 
 integer getftdc(integer arr[]);
 void putftdc(integer nelem, integer arr[]);
 
 integer getstdc(integer arr[]);
 integer getnstdc(){return _nstdc;}
 void putstdc(integer nelem, integer arr[]);
 
 integer getsumh(integer arr[]);
 void putsumh(integer nelem, integer arr[]);
 integer getsumsh(integer arr[]);
 void putsumsh(integer nelem, integer arr[]);
  
 geant getadca(){return _adca;}
 void putadca(geant adca){_adca=adca;}
 integer getadcd(geant arr[]);
 integer getnadcd(){return _nadcd;}
 void putadcd(integer nelem, geant arr[]);
 integer lvl3format(int16 * ptr, integer rest);



 static void setpatt(integer patt[]){
   for(int i=0;i<TOF2GC::SCLRS;i++)_trpatt[i]=patt[i];
 }
 static void setpattz(integer patt[]){
   for(int i=0;i<TOF2GC::SCLRS;i++)_trpattz[i]=patt[i];
 }
 static void getpatt(integer patt[]){
   for(int i=0;i<TOF2GC::SCLRS;i++)patt[i]=_trpatt[i];
 }
 static void getpattz(integer patt[]){
   for(int i=0;i<TOF2GC::SCLRS;i++)patt[i]=_trpattz[i];
 }
 static void addtpb(integer il, integer ib, integer is){//il=0-3,ib=0->,is=0,1
   integer bit=(ib+is*16);
   _trpatt[il]|=(1<<bit);
 }
 static void addtpzb(integer il, integer ib, integer is){//il=0-3,ib=0->,is=0,1
   integer bit=(ib+is*16);
   _trpattz[il]|=(1<<bit);
 }
 static void settrcode(integer trfl){_trcode=trfl;}
 static void settrcodez(integer trfl){_trcodez=trfl;}
 static void setftpatt(integer ftp){_ftpatt=ftp;}
 static void setftpbit(integer ftp){_ftpatt|=ftp;}
 static void setcpcode(integer cpc){_cpcode=cpc;}
 static void setbzflag(integer flg){_bzflag=flg;}
 static integer gettrcode(){return _trcode;}
 static integer gettrcodez(){return _trcodez;}
 static integer getftpatt(){return _ftpatt;}
 static integer getcpcode(){return _cpcode;}
 static integer getbzflag(){return _bzflag;}
 static bool GlobFasTrigOK(){return _ftpatt>0;}
 static number gettrtime(){return _trtime;}
 static void settrtime(number trt){_trtime=trt;}
 static uinteger getsrun(){return StartRun;}
 static time_t getstime(){return StartTime;}
//
 static void mc_build(int &);// Tovt-->raw_event  
 static void validate(int &);// RawEvent->RawEvent
//
// interface with DAQ :
// static integer calcdaqlength(int16u blid);
// static void builddaq(int16u blid, integer &len, int16u *p);
// static void buildraw(int16u blid, integer &len, int16u *p);
// static int16u hw2swid(int16u a1, int16u a2, int16u a3);
// static int16u sw2hwid(int16u a1, int16u a2, int16u a3); 
//
protected:
 void _printEl(ostream &stream){
  int i;
  stream <<"TOF2RawSide: swid="<<dec<<_swid<<endl;
  stream <<"hwid="<<dec<<_hwid<<endl;
  stream <<"stat="<<dec<<_status<<endl;
  stream <<"nftdc="<<dec<<_nftdc<<endl;
  for(i=0;i<_nftdc;i++)stream <<hex<<_ftdc[i]<<endl;
  stream <<"nstdc="<<dec<<_nstdc<<endl;
  for(i=0;i<_nstdc;i++)stream <<hex<<_stdc[i]<<endl;
  stream <<"nsumh="<<dec<<_nsumh<<endl;
  for(i=0;i<_nsumh;i++)stream <<hex<<_sumht[i]<<endl;
  stream <<"nsumsh="<<dec<<_nsumsh<<endl;
  for(i=0;i<_nsumsh;i++)stream <<hex<<_sumsht[i]<<endl;
  stream <<"adca="<<dec<<_adca<<endl;
  stream <<"nadcd="<<dec<<_nadcd<<endl;
  for(i=0;i<_nadcd;i++)stream <<dec<<_adcd[i]<<endl;
  stream <<"temper="<<dec<<_temp<<endl;
  stream <<dec<<endl<<endl;
 }
 void _writeEl();
 void _copyEl();
};
//

#endif
