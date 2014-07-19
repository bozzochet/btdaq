//  $Id: tofdbc02.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author E.Choumilov 14.06.96.
#include "typedefs.h"
#include <math.h>
#include "commons.h"
#include "job.h"
#include "tofdbc02.h"
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include "tofsim02.h"
#include "tofrec02.h"
#include "tofcalib02.h"
#include "charge.h"
//
using namespace AMSChargConst;
//
TOF2Varp TOF2Varp::tofvpar; // mem.reserv. TOF general parameters 
TOF2Brcal TOF2Brcal::scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];// mem.reserv. TOF indiv.bar param. 
TOFBrcalMS TOFBrcalMS::scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];// the same for "MC Seeds" 
TOFBPeds TOFBPeds::scbrped[TOF2GC::SCLRS][TOF2GC::SCMXBR];//mem.reserv. TOF-bar pedestals/sigmas/...
TOF2Varp::TOF2Temperature TOF2Varp::tftt;
TofElosPDF TofElosPDF::TofEPDFs[MaxZTypes];
//-----------------------------------------------------------------------
//  =====> TOF2DBc class variables definition :
//
integer TOF2DBc::debug=1;
//
//
//======> memory reservation for _brtype :
// (real values are initialized from ext. geomconfig-file in amsgeom.c) 
integer TOF2DBc::_brtype[TOF2GC::SCBLMX]={
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
};
//
// Initialize TOF geometry (defaults, REAL VALUES are read FROM geomconfig-file)
//
//---> bar lengthes (cm) for each of TOF2GC::SCBTPN types :
geant TOF2DBc::_brlen[TOF2GC::SCBTPN]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> bar eff.light-guide lengthes (cm) for TOF2GC::SCBTPN types :
geant TOF2DBc::_lglen[TOF2GC::SCBTPN]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> plane rotation mask for 4 layers (1/0 -> rotated/not):
integer TOF2DBc::_plrotm[TOF2GC::SCLRS]={
  1,0,0,1
};
//---> real planes:
integer TOF2DBc::_planes=0;
//
//---> real bars/plane for 4 planes:
integer TOF2DBc::_bperpl[TOF2GC::SCLRS]={
  0,0,0,0
};
//---> outer counter parameters(1-4) for 4 planes
geant TOF2DBc::_outcp[TOF2GC::SCLRS][4]={
  0., 0., 0., 0.,
  0., 0., 0., 0.,
  0., 0., 0., 0.,
  0., 0., 0., 0.
};
//---> honeycomb supporting str. data:
geant TOF2DBc::_supstr[12]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> sc. plane design parameters:
geant TOF2DBc::_plnstr[20]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> PMT SES-params:
integer TOF2DBc::_nsespar=0;
geant TOF2DBc::_sespar[TOF2GC::SCBTPN][TOF2GC::SESPMX]={
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0
};
//---> Initialize TOF MC/RECO "time-stable" parameters :
//
  geant TOF2DBc::_edep2ph=10000.;//(was 8k for old scint)edep(Mev)-to-Photons convertion
  geant TOF2DBc::_adc2q=1.;       // not used (now taken from TFCA #21)
  geant TOF2DBc::_fladctb=0.1;    // MC "flash-ADC" internal time binning (ns)
  geant TOF2DBc::_tdcscl=50331.;  // max TDC-scale(ns)(21bit*24ps)
  geant TOF2DBc::_strflu=0.36;     // Stretcher "end-mark" time uncertainty (ns)
  geant TOF2DBc::_tdcbin[4]={
    0.0244,                        // LTtime/FTtime/SumHTtime-TDC binning(ns).
    0.024,                         // 
    0.5,                           // supl.DAQ binning for charge meas.(in ADC-chan units)
    0.                             // spare
  };
  geant TOF2DBc::_daqpwd[15]={
    250.,   // (0)PW of "z>=1(FTC)"(HT-branch) output signal(ACTEL-outp going to SPT)[ns] 
    5.,     // (1)minimal inp.pulse width(TovT) to fire the generic discriminator[its rise time, ns]
    250.,   // (2)PW of "z>=2(FTZ)"(SHT-branch) output signal(ACTEL-outp going to SPT)[ns]
    7.,     // (3)minimal output pulse width[ns] of generic discriminator [i.e. outPW=7ns if
//                    5<=inpTovT<=(5+7), and outPW=inpTovT-5 if inpTovT>(5+7)]
    11.,     // (4)input dead time of generic discr(min dist. of prev_down/next_up edges)[ns]
//                    (i neglect by fall-time of generic discr, so DT=(11+5)ns for output signals !!!)
    250.,   // (5)gate-width for z>=1 tof-trig-pattern creation(in SPT2)[ns]
    250.,   // (6)gate-width for z>=2 tof-trig-pattern creation(in SPT2)[ns]
    20.,    // (7)dead time of TDC-inputs, the same for LT-/FT-/SumHT-inputs[ns]
    10.,    // (8)dead time of "HT/SHT-trig" branch on ACTEL-outp(SPT-inp)[fall-to-rise min.dist, going to SPT, ns]
//                    (Guido: ACTEL-input is faster than Discr, so no ACTEL-inp dead time check) 
    20.,    // (9)dead time of "SumHT(SHT)" branch on ACTEL output going to TDC[ns]
    0.02,   // (10)generic discr. internal accuracy[ns]
    25.,    // (11)fixed PW of "SumHT(SHT)" branch on ACTEL-output[going to TDC-input, ns] 
    0.005,  // (12)TDC differential nonlinearity(rms,ns)(0.21bin) 
    0.017,  // (13)TDC integral nonlinearity(table-corrected,ns)(0.72bin) 
    10.36   // (14)spare 
  };
  geant TOF2DBc::_trigtb=0.5;  // MC time-bin in logic(trig) pulse handling (ns)
  geant TOF2DBc::_hagemx=6000.;  // max.TimeTDC-hit age(wrt FT,ns) accepted by Actel buffer
  geant TOF2DBc::_hagemn=-10000.;// min....................................................
  geant TOF2DBc::_ftc2cj=0.015; // FT-signal crate-to-crate jitter(ns)
  geant TOF2DBc::_fts2sj=0.005; // FT-signal slot-to-slot jitter(ns)
  geant TOF2DBc::_accdel=6000.;// "CommonStop" signal delay with respect to FT (ns)
  geant TOF2DBc::_ltagew[2]={20,640};//LTtime-hit(true) wrt FTtime age-window(ns) 
  geant TOF2DBc::_ftdelm=250.; // FT max delay (allowed by stretcher logic) (ns)
  geant TOF2DBc::_fstdcd=28.;  // spare       (because now taken from DC/DB)
  geant TOF2DBc::_clkper=40.;  // Trig.electronics clock period(ns)
  integer TOF2DBc::_pbonup=1;  // set phase-bit for leading(up) edge (yes/no->1/0) 
//
//  member functions :
//
// ---> function to read geomconfig-files 
//                  Called from TOFgeom :
  void TOF2DBc::readgconf(){
    int i,j;
    char fname[80];
    char name[80]="tof2const";
    char vers1[4]="12p";
    char vers2[4]="1";
    geant ZShift(0);
//
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02D")){
      cout <<" TOFGeom-I-AMS02D setup selected."<<endl;
      cout <<" TOFGeom-I-TOF:8/8/10/8-pads setup selected."<<endl;
      strcat(name,vers2);
      ZShift=AMSDBc::amsdext;
      cout<<" TOFGeom-I-ZShift="<<ZShift<<endl;
    }
    else if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
      cout <<" TOFGeom-I-AMS02 setup selected."<<endl;
      if(strstr(AMSJob::gethead()->getsetup(),"TOF:12PAD")){
        cout <<"      TOFGeom-I-TOF:12PAD setup selected."<<endl;
        strcat(name,vers1);
	TFCAFFKEY.cfvers=0;//reset vers.numb. to 0 for old(12pads) setup
      }
      else{
        cout <<"      TOF_Paddles_Configuration:8/8/10/8-pads/layer "<<endl;
        strcat(name,vers2);
      }
	  
    }
    else
    {
          cout <<"       Unknown setup !!!"<<endl;
	  exit(10);
    }
    strcat(name,".dat");
    if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
    if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
    strcat(fname,name);
    cout<<"      Open file : "<<fname<<'\n';
    ifstream tcfile(fname,ios::in); // open needed config-file for reading
    if(!tcfile){
      cerr <<"      missing geomconfig-file "<<fname<<endl;
      exit(1);
    }
    tcfile >> _planes;
    for(i=0;i<_planes;i++) tcfile >> _bperpl[i];
    int icm=0;
    for(i=0;i<_planes;i++)icm+=_bperpl[i];
    for(int ic=0;ic<icm;ic++) tcfile >> _brtype[ic];
    for(i=0;i<_planes;i++) tcfile >> _plrotm[i];
    for(i=0;i<TOF2GC::SCBTPN;i++) tcfile >> _brlen[i];
    for(i=0;i<TOF2GC::SCBTPN;i++) tcfile >> _lglen[i];
    for(int ipar=0;ipar<4;ipar++){
      for(i=0;i<_planes;i++) tcfile >> _outcp[i][ipar];
    }
    for(i=0;i<12;i++) tcfile >> _supstr[i];
    for(i=0;i<20;i++) tcfile >> _plnstr[i];
    tcfile >> _nsespar;//# of SES params
    for(i=0;i<_nsespar;i++){
      for(j=0;j<TOF2GC::SCBTPN;j++)tcfile >> _sespar[j][i];//SES params. vs btyp
    }
    _supstr[0]+=ZShift;//used for AMS02D-setup !!! for normal one ZShift=0 !!!
    _supstr[1]-=ZShift;
    cout<<"<---- TOF2DBc::readgconf: succsessfully done !"<<endl<<endl;
  }
//---
  integer TOF2DBc::brtype(integer ilay, integer ibar){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
        assert(ibar>=0 && ibar < _bperpl[ilay]);
      }
#endif
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=_bperpl[i];
    cnum+=ibar;
    return _brtype[cnum];
  }
//
 integer TOF2DBc::npmtps(integer ilay, integer ibar){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
        assert(ibar>=0 && ibar < _bperpl[ilay]);
      }
#endif
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=_bperpl[i];
    cnum+=ibar;
    if(_brtype[cnum]==1 || _brtype[cnum]==3)return 3;
    else return 2;
  }
//
  integer TOF2DBc::barseqn(integer ilay, integer ibar){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
        assert(ibar>=0 && ibar < _bperpl[ilay]);
      }
#endif
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=_bperpl[i];
    cnum+=ibar;
    return cnum;
  }
//
  geant TOF2DBc::brlen(integer ilay, integer ibar){
    int cnum=0;
    int btyp;
    for(int i=0;i<ilay;i++)cnum+=_bperpl[i];
    cnum+=ibar;
    btyp=_brtype[cnum];
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(btyp>0 && btyp <= TOF2GC::SCBTPN);
      }
#endif
    return _brlen[btyp-1];
  }
//
  geant TOF2DBc::lglen(integer ilay, integer ibar){
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=_bperpl[i];
    cnum+=ibar;
    int btyp=_brtype[cnum];
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(btyp>0 && btyp <= TOF2GC::SCBTPN);
      }
#endif
    return _lglen[btyp-1];
  }
//
  integer TOF2DBc::plrotm(integer ilay){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
      }
#endif
    return _plrotm[ilay];
  }
//
  integer TOF2DBc::getbppl(integer ilay){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
      }
#endif
    return _bperpl[ilay];
  }
//
  geant TOF2DBc::outcp(int ilay, int ipr){//ipr=1:4->wd/xc/yc/exwd
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
	assert(ipr>0 && ipr<=4);
      }
#endif
    return _outcp[ilay][ipr-1];
  }
//
  geant TOF2DBc::supstr(int i){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>0 && i <= 12);
      }
#endif
    return _supstr[i-1];
  }
//
  geant TOF2DBc::plnstr(int i){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>0 && i <= 20);
      }
#endif
    return _plnstr[i-1];
  }
// ===> function to get Z-position of scint. bar=ib in layer=il
//
  geant TOF2DBc::getzsc(integer il, integer ib){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(il>=0 && il < _planes);
        assert(ib>=0 && ib < _bperpl[il]);
      }
#endif
  geant dz,zc;
  dz=_plnstr[5];// counter thickness
  if(il==0)
    zc=_supstr[0]-_plnstr[0]-dz/2.;
  else if(il==1)
    zc=_supstr[0]-_plnstr[1]-dz/2.;
  else if(il==2)
    zc=_supstr[1]+_plnstr[1]+dz/2.;
  else if(il==3)
    zc=_supstr[1]+_plnstr[0]+dz/2.;
  if(il==0)zc=zc-(ib%2)*_plnstr[2];
  else if(il==3)zc=zc+(ib%2)*_plnstr[2];
  else if(il==1)zc=zc-((ib+1)%2)*_plnstr[2];
  else if(il==2)zc=zc+((ib+1)%2)*_plnstr[2];
  return(zc);
  }  
// ===> function to get transv. position of scint. bar=ib in layer=il
//
  geant TOF2DBc::gettsc(integer il, integer ib){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(il>=0 && il < _planes);
        assert(ib>=0 && ib < _bperpl[il]);
      }
#endif
  static geant dxi,dxo;
  geant x,co[2],dxti,dxto;
  dxi=_plnstr[4];//sc.paddle width(inner="normal" counters)
  dxo=_outcp[il][0];//sc.paddle width(outer="trapezoidal" counters)
  dxti=(_bperpl[il]-3)*(dxi-_plnstr[3]);//inner 1st/last sc.padd.center transv.dist 
  dxto=dxti+dxo+dxi-2*_plnstr[3];//1st/last sc.padd.center transv.distance
//
  if(il<2){
    co[0]=_supstr[2];// <--top TOF-subsystem X-shift
    co[1]=_supstr[3];// <--top TOF-subsystem Y-shift
  }
  if(il>1){
    co[0]=_supstr[4];// <--bot TOF-subsystem X-shift
    co[1]=_supstr[5];// <--bot TOF-subsystem Y-shift
  }
//
  if(ib==0)x=-0.5*dxto;
  else if(ib==(_bperpl[il]-1))x=0.5*dxto;
  else x=-0.5*dxti+(ib-1)*(dxi-_plnstr[3]);
//
  if(_plrotm[il]==0){  // <-- unrotated planes
    x=co[0]+x;//honeycomb_RS --> MRS(imply sc.padds are at "0" in Hon.RS)
  }
  if(_plrotm[il]==1){  // <-- rotated planes
    x=co[1]+x;
  }
  return(x);
  }
// functions to get MC/RECO parameters:
//
  geant TOF2DBc::edep2ph(){return _edep2ph;}
  geant TOF2DBc::fladctb(){return _fladctb;}
  geant TOF2DBc::tdcscl(){return _tdcscl;}
//
  geant TOF2DBc::hagemn(){return _hagemn;}
  geant TOF2DBc::hagemx(){return _hagemx;}
//
  geant TOF2DBc::strflu(){return _strflu;}
//
  geant TOF2DBc::ftc2cj(){return _ftc2cj;}
//
  geant TOF2DBc::fts2sj(){return _fts2sj;}
//
  geant TOF2DBc::accdel(){return _accdel;}
  geant TOF2DBc::ltagew(int i){return _ltagew[i];}
  geant TOF2DBc::ftdelm(){return _ftdelm;}
  geant TOF2DBc::fstdcd(){return _fstdcd;}
  geant TOF2DBc::clkper(){return _clkper;}
  
  int TOF2DBc::nsespar(){return _nsespar;}
  geant TOF2DBc::sespar(int ibt, int ip){
#ifdef __AMSDEBUG__
    assert(ibt>=0 && ibt< TOF2GC::SCBTPN);
//    assert(ip>0=0 && ip< TOF2GC::SESPMX);
#endif
    return _sespar[ibt][ip];
  }
  
  geant TOF2DBc::adc2q(){return _adc2q;}
  integer TOF2DBc::pbonup(){return _pbonup;}
//
  geant TOF2DBc::tdcbin(int i){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>=0 && i<4);
      }
#endif
    return _tdcbin[i];}
//
  geant TOF2DBc::daqpwd(int i){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>=0 && i<15);
      }
#endif
    return _daqpwd[i];}
//
  geant TOF2DBc::trigtb(){return _trigtb;}
//===============================================================================
  TOF2Brcal::TOF2Brcal(integer sid, integer npm, integer sta[2], geant gna[2], 
           geant gnd[2][TOF2GC::PMTSMX],
           geant a2d[2], geant asl, geant tth, geant stra[2][2], geant fstd,  
           geant t0, geant sl, geant sls[2], geant tdiff, geant td2p[2], geant mip,
           integer nsp, geant ysc[], geant relo[], geant upar[2*TOF2GC::SCPROFP], 
	   geant a2q){
    int i,j;
    softid=sid;
    npmts=npm;
    status[0]=sta[0];
    status[1]=sta[1];
    gaina[0]=gna[0];
    gaina[1]=gna[1];
    for(i=0;i<TOF2GC::PMTSMX;i++){
      gaind[0][i]=gnd[0][i];
      gaind[1][i]=gnd[1][i];
    }
    a2dr[0]=a2d[0];
    a2dr[1]=a2d[1];
    asatl=asl;
    tthr=tth;
    strat[0][0]=stra[0][0];
    strat[0][1]=stra[0][1];
    strat[1][0]=stra[1][0];
    strat[1][1]=stra[1][1];
    fstrd=fstd;
    tzero=t0;
    slope=sl;
    slops[0]=sls[0];
    slops[1]=sls[1];
    yctdif=tdiff;
    td2pos[0]=td2p[0];
    td2pos[1]=td2p[1];
    mip2q=mip;
    nscanp=nsp;
    for(i=0;i<nsp;i++){
      yscanp[i]=ysc[i];
      relout[i]=relo[i];
    }
    for(i=0;i<2*TOF2GC::SCPROFP;i++)unipar[i]=upar[i];
    adc2qf=a2q;
  }
//  ================= TOF2Brcal class functions ======================= :
//
void TOF2Brcal::build(){// create scbrcal-objects for each sc.bar
//
 int lsflg(1);//0/1->use common/individual values for Lspeed 
 integer i,j,k,ila,ibr,ip,ibrm,isd,isp,nsp,ibt,cnum,dnum,mult;
 geant scp[TOF2GC::SCANPNT];
 geant rlo[TOF2GC::SCANPNT];// relat.(to Y=0) light output
 integer lps=1000;
 geant ef1[TOF2GC::SCANPNT],ef2[TOF2GC::SCANPNT];
 integer i1,i2,sta[2],stat[TOF2GC::SCBLMX][2],npm;
 geant r,eff1,eff2;
 integer sid,brt,endflab;
 geant gna[2],qath,qdth,a2dr[2],tth,strat[2][2],ah2l[2];
 geant slope,slpf,fstrd,tzer,tdif,mip2q,speedl,lspeeda[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 geant tzerf[TOF2GC::SCLRS][TOF2GC::SCMXBR],tdiff[TOF2GC::SCBLMX];
 geant slops[2],slops1[TOF2GC::SCLRS][TOF2GC::SCMXBR],slops2[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 geant strf[TOF2GC::SCBLMX][2],strof[TOF2GC::SCBLMX][2];
 geant an2di[TOF2GC::SCBLMX][2],gaina[TOF2GC::SCBLMX][2],m2q[TOF2GC::SCBTPN];
 geant aprofp[TOF2GC::SCBTPN][2*TOF2GC::SCPROFP],apr[2*TOF2GC::SCPROFP],hblen;
 geant a2drf[TOF2GC::SCBLMX][2];
 geant p1s1,p2s1,p3s1,p4s1,p5s1,p6s1,p1s2,p2s2,p3s2,p4s2,p5s2,p6s2,nom,denom; 
 char fname[80];
 char name[80];
 geant a2q,td2p[2];
 char vers1[3]="mc";
 char vers2[3]="rl";
 int mrfp;
//
 geant asatl=20.;//(mev,~20MIPs),if E-dinode(1-end) higher - use it instead
//                                 of anode measurements
//
 geant gaind[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];//buff.for dyn.pmts relat.gains
 geant gnd[2][TOF2GC::PMTSMX];
 int ipm; 
//------------------------------
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  strcpy(inum,"0123456789");
//
// ---> read file with the list of version numbers for all needed calib.files :
//
  integer cfvn;
  cfvn=TFCAFFKEY.cfvers%1000;//vers.list-file version number 
  strcpy(name,"tof2cvlist");// basic name for vers.list-file  
  dig=cfvn/100;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
//
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> TOF2Brcal::build: Opening vers.list-file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in);
  if(!vlfile){
    cerr <<"<---- missing vers.list-file !!! "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
    vlfile >> rlvern[i];// second number - for real
  }
  vlfile.close();
//------------------------------------------------
//
//   --->  Prepare to read tof-channels calib-status file :
//
 ctyp=2;//line# corresponding calib-status parameters file
 strcpy(name,"tof2stcf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData())           // for MC-data
 {
   cout <<"      TOF-channels Calib-Status for MC-data is requested..."<<endl;
   dig=mcvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(mcvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real-data
 {
   cout <<"      TOF-channels Calib-Status for RealData is requested..."<<endl;
   dig=rlvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(rlvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
   strcat(name,".dat");
   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"      Opening file : "<<fname<<'\n';
   ifstream stfile(fname,ios::in); // open file for reading
   if(!stfile){
     cerr <<"<---- missing TOF-channels Calib-Status file: "<<fname<<endl;
     exit(1);
   }
//------------------------------
//   --->  Read TOF-channels status values:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
     for(int ipr=0;ipr<2;ipr++){
       stfile >> stat[cnum][ipr];//stat(s1),stat(s2)
     }
     cnum+=1; // sequential counter numbering(0-...)
   } // --- end of bar loop --->
   } // --- end of layer loop --->
//
   stfile >> endflab;//read endfile-label
//
   stfile.close();
//
   if(endflab==12345){
     cout<<"   <--TOF-calib status file is successfully read !"<<endl;
   }
   else{cout<<"<---- problems with TOF-channels Calib-Status file !!!"<<endl;
     exit(1);
   }
//
//------------------------------------------------- 
//
//   --->  Prepare to read stretcher_ratios/offs file :
//
// ctyp=3;
// strcpy(name,"tof2srcf");
// mcvn=mcvern[ctyp-1]%1000;
// rlvn=rlvern[ctyp-1]%1000;
// if(AMSJob::gethead()->isMCData())           // for MC-data
// {
//   cout <<" TOF2Brcal_build: str_ratio/offs params for MC-data are requested"<<endl;
//   dig=mcvn/100;
//   in[0]=inum[dig];
//   strcat(name,in);
//   dig=(mcvn%100)/10;
//   in[0]=inum[dig];
//   strcat(name,in);
//   dig=mcvn%10;
//   in[0]=inum[dig];
//   strcat(name,in);
//   strcat(name,vers1);
// }
// else                                       // for Real-data
// {
//   cout <<" TOF2Brcal_build: str_ratio/offs params for Real-data are requested"<<endl;
//   dig=rlvn/100;
//   in[0]=inum[dig];
//   strcat(name,in);
//   dig=(rlvn%100)/10;
//   in[0]=inum[dig];
//   strcat(name,in);
//   dig=rlvn%10;
//   in[0]=inum[dig];
//   strcat(name,in);
//   strcat(name,vers2);
// }
//   strcat(name,".dat");
//   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
//   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
//   strcat(fname,name);
//   cout<<"Open file : "<<fname<<'\n';
//   ifstream scfile(fname,ios::in); // open file for reading
//   if(!scfile){
//     cerr <<"TOF2Brcal_build: missing str_ratio/offsets file "<<fname<<endl;
//     exit(1);
//   }
//-----------------> read str_ratios/offsets:
//
//   cnum=0;
//   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
//   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
//     scfile >> strf[cnum][0];
//     scfile >> strof[cnum][0];
//     scfile >> strf[cnum][1];
//     scfile >> strof[cnum][1];
//     cnum+=1; // sequential counter numbering(0-...)
//   } // --- end of bar loop --->
//   } // --- end of layer loop --->
//
//   scfile >> endflab;//read endfile-label
//
//   scfile.close();
//
//   if(endflab==12345){
//     cout<<"TOF2Brcal_build: TOF-str_ratio/offs file is successfully read !"<<endl;
//   }
//   else{cout<<"TOF2Brcal_build: ERROR(problems with TOF-str_ratio/offs file)"<<endl;
//     exit(1);
//   }
//
//--------------------------------------------------
//
//   --->  Prepare to read lspeed/tdiffs calibration file :
//
 ctyp=4;
 strcpy(name,"tof2tdcf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData()) //      for MC-data
 {
       cout <<"      TOF Lspeed/Tdiffs for MC-data are requested..."<<endl;
       dig=mcvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(mcvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
 }
 else                              //      for Real-data
 {
       cout <<"      TOF Lspeed/Tdiffs for Real-data are requested..."<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
 }
//
 strcat(name,".dat");
 if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"      Open file : "<<fname<<'\n';
 ifstream tdcfile(fname,ios::in); // open  file for reading
 if(!tdcfile){
   cerr <<"<---- missing Lspeed/Tdif-file !!! "<<fname<<endl;
   exit(1);
 }
//
// ------------------------> read Lspeed/Tdiffs:
//
 if(lsflg){// read bar indiv.Lspeed
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  
       tdcfile >> lspeeda[ila][ibr];
     }
   }
 }
 else tdcfile >> speedl;// read average Lspeed
//
 cnum=0;
 for(ila=0;ila<TOF2DBc::getnplns();ila++){   
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  
     tdcfile >> tdiff[cnum];
     cnum+=1; // sequential counters numbering(0-...)
   }
 }
//
   tdcfile >> endflab;//read endfile-label
//
   tdcfile.close();
//
   if(endflab==12345){
     cout<<"   <--TOF Lspeed/Tdiffs file is successfully read !"<<endl;
   }
   else{cout<<"<---- problems with TOF Lspeed/Tdiffs file !!!"<<endl;
     exit(1);
   }
//-----------------------------------------------------
//
//   --->  Prepare to read slewing_slope/tzeros calibration file :
//
 ctyp=5;
 strcpy(name,"tof2tzcf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData()) //      for MC-data
 {
       cout <<"      Slewing/T0-params for MC-data are requested..."<<endl;
       dig=mcvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(mcvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
 }
 else                              //      for Real-data
 {
       cout <<"      Slewing/T0-params for Real-data are requested..."<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
 }
//
 strcat(name,".dat");
 if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"      Open file : "<<fname<<'\n';
 ifstream tzcfile(fname,ios::in); // open  file for reading
 if(!tzcfile){
   cerr <<"<---- missing TOF Slewing/T0-params file !!! "<<fname<<endl;
   exit(1);
 }
//
// --------------------------> read Slewing/Tzero's:
 tzcfile >> slpf;
 for(ila=0;ila<TOF2DBc::getnplns();ila++){ 
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){
     tzcfile >> tzerf[ila][ibr];
   } 
 }
//
   tzcfile >> endflab;//read endfile-label
//
   tzcfile.close();
//
   if(endflab==12345){
     cout<<"   <--TOF Slewing/T0-params file is successfully read !"<<endl;
   }
   else{cout<<"<---- problems with TOF Slewing/T0-params file !!!"<<endl;
     exit(1);
   }
//-------------------------------------------------------
//
//   --->Prepare to read anodes/dynode relat.gains, anode/dynode ratios,
//       mip2q and A-profile param. calib.file :
//
 ctyp=6;
 strcpy(name,"tof2chcf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData())           // for MC-data
 {
   cout <<"      EnergyDeposit-calib params for MC-data are requested..."<<endl;
   dig=mcvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(mcvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real-data
 {
   cout <<"      EnergyDeposit-calib params for Real-data are requested..."<<endl;
   dig=rlvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(rlvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
   strcat(name,".dat");
   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"      Open file : "<<fname<<'\n';
   ifstream gcfile(fname,ios::in); // open file for reading
   if(!gcfile){
     cerr <<"<---- missing EnergyDeposit-calib params file !!! "<<fname<<endl;
     exit(1);
   }
//
// ----------------> read anode relative(wrt ref.counter) gains:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-1
       gcfile >> gaina[cnum+ibr][0];
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-2
       gcfile >> gaina[cnum+ibr][1];
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read anode/dynode ratios:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-1
       gcfile >> a2drf[cnum+ibr][0];
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-2
       gcfile >> a2drf[cnum+ibr][1];
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read dynode-pmts gains(relat to side average):
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){// pm-loop
       for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  //padles loop s-1
         gcfile >> gaind[cnum+ibr][0][ipm]; // read PM=ipm
       }
     }
     for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){// pm-loop
       for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  //padles loop s-2 
         gcfile >> gaind[cnum+ibr][1][ipm];// read PM=ipm
       }
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read mip2q's:
//
   for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){  // <-------- loop over bar-types
     gcfile >> m2q[ibt];
   }
//
// ----------------> read A-prof. parameters:
//
   for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){  // <-------- loop over bar-types
     for(i=0;i<2*TOF2GC::SCPROFP;i++)gcfile >> aprofp[ibt][i];
   }
// ---------------->
   gcfile >> endflab;//read endfile-label
//
   gcfile.close();
//
   if(endflab==12345){
     cout<<"   <--EnergyDeposit-calib params file is successfully read !"<<endl;
   }
   else{cout<<"<---- problems with EnergyDeposit-calib params file !!!"<<endl;
     exit(1);
   }
//   
//---------------------------------------------
//   ===> fill TOFBrcal bank :
//
  a2q=TFCAFFKEY.adc2q;//tempor (if variation are high - read special indiv.a2q's file)
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);//1->11
    hblen=0.5*TOF2DBc::brlen(ila,ibr);
    nsp=TOFWScan::scmcscan[brt-1].getnscp(0);//get scan-points number for wdiv=1(most long)
    npm=TOFWScan::scmcscan[brt-1].getnpmts();//get pmts/side 
    TOFWScan::scmcscan[brt-1].getscp(0,scp);//get scan-points for wdiv=1(most long)
// read from file or DB:
    gna[0]=gaina[cnum][0];
    gna[1]=gaina[cnum][1];
    tth=TOF2Varp::tofvpar.daqthr(0); // (mV), time-discr. threshold
    mip2q=m2q[brt-1];//(pC/mev),dE(mev)_at_counter_center->Q(pC)_at_PM_anode(2x3-sum)
    fstrd=TOF2Varp::tofvpar.sftdcd();//(ns),same hit(up-edge)delay in f/sTDC(const. for now)
//
//-->prepare position correction array (valid for local !!! r.s.):
//
      for(i=0;i<2*TOF2GC::SCPROFP;i++)apr[i]=aprofp[brt-1][i];
      p1s1=aprofp[brt-1][0];
      p2s1=aprofp[brt-1][1];
      p3s1=aprofp[brt-1][2];
      p4s1=aprofp[brt-1][3];
      p5s1=aprofp[brt-1][4];
      p6s1=aprofp[brt-1][5];
      p1s2=aprofp[brt-1][6];
      p2s2=aprofp[brt-1][7];
      p3s2=aprofp[brt-1][8];
      p4s2=aprofp[brt-1][9];
      p5s2=aprofp[brt-1][10];
      p6s2=aprofp[brt-1][11];
      mrfp=nsp/2;//central point (coo=0.)
      if(brt==1 || brt==3 || brt==5 || brt==8){//trapez.counters
        denom=p1s1*((1-p3s1)*exp(-(hblen+scp[mrfp])/p2s1)+p3s1*exp(-(hblen+scp[mrfp])/p4s1))
	     +p5s1*exp(-(hblen-scp[mrfp])/p6s1)
             +p1s2*((1-p3s2)*exp(-(hblen-scp[mrfp])/p2s2)+p3s2*exp(-(hblen-scp[mrfp])/p4s2))
	     +p5s2*exp(-(hblen+scp[mrfp])/p6s2);//s1+s2 signal at center(long.coo=0)
        for(isp=0;isp<nsp;isp++){ // fill 2-ends rel. l.output at scan-points
          nom=p1s1*((1-p3s1)*exp(-(hblen+scp[isp])/p2s1)+p3s1*exp(-(hblen+scp[isp])/p4s1))
	     +p5s1*exp(-(hblen-scp[isp])/p6s1)
             +p1s2*((1-p3s2)*exp(-(hblen-scp[isp])/p2s2)+p3s2*exp(-(hblen-scp[isp])/p4s2))
	     +p5s2*exp(-(hblen+scp[isp])/p6s2);//s1+s2 signal at long.coo=scp[isp] 
          rlo[isp]=nom/denom;
        }
      }
      else{//normal counters
        denom=p1s1*((1-p3s1)*exp(-(hblen+scp[mrfp])/p2s1)+p3s1*exp(-(hblen+scp[mrfp])/p4s1))
             +p1s2*((1-p3s2)*exp(-(hblen-scp[mrfp])/p2s2)+p3s2*exp(-(hblen-scp[mrfp])/p4s2));
        for(isp=0;isp<nsp;isp++){ // fill 2-ends rel. l.output at scan-points
          nom=p1s1*((1-p3s1)*exp(-(hblen+scp[isp])/p2s1)+p3s1*exp(-(hblen+scp[isp])/p4s1))
             +p1s2*((1-p3s2)*exp(-(hblen-scp[isp])/p2s2)+p3s2*exp(-(hblen-scp[isp])/p4s2));
          rlo[isp]=nom/denom;
        }
      }
//
    sid=100*(ila+1)+(ibr+1);
//    strat[0][0]=strf[cnum][0];//stretcher param. from ext.file
//    strat[1][0]=strf[cnum][1];
    strat[0][0]=25;//tempor: put dummy pars(not used now, keep for possible future appl)
    strat[1][0]=25;
//    strat[0][1]=strof[cnum][0];
//    strat[1][1]=strof[cnum][1];
    strat[0][1]=1100;
    strat[1][1]=1100;
    sta[0]=stat[cnum][0];
    sta[1]=stat[cnum][1];
    slope=slpf;// common slope from ext. file
//    slops[0]=slops1[ila][ibr];// indiv.slopes from ext.file
//    slops[1]=slops2[ila][ibr];
    slops[0]=1.;// default indiv.slopes
    slops[1]=1.;
    tzer=tzerf[ila][ibr];//was read from ext. file
    tdif=tdiff[cnum];//was read from ext. file
    if(lsflg){
      td2p[0]=lspeeda[ila][ibr];//indiv.bar speed of the light from external file
    } 
    else td2p[0]=speedl;//average speed of the light from external file
    td2p[1]=TOF2Varp::tofvpar.lcoerr();//error on longit. coord. measurement(cm)
    a2dr[0]=a2drf[cnum][0];//an/dyn ratios from ext.file
    a2dr[1]=a2drf[cnum][1];
    for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
      gnd[0][ipm]=gaind[cnum][0][ipm];// dyn-pms rel.gains from ext.file
      gnd[1][ipm]=gaind[cnum][1][ipm];// dyn-pms rel.gains from ext.file
    }
    scbrcal[ila][ibr]=TOF2Brcal(sid,npm,sta,gna,gnd,a2dr,asatl,tth,strat,fstrd,tzer,
                      slope,slops,tdif,td2p,mip2q,nsp,scp,rlo,apr,a2q);
//
    cnum+=1;// solid sequential numbering of all counter(0->33)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
  cout<<"<---- TOF2Brcal::build: succsessfully done !"<<endl<<endl;
}
//-----------------------------------------------------------------------
//
void TOF2Brcal::q2a2q(int cof, int sdf, int hlf, number &adc, number &q){  
// Q(pC) <-> ADC(ch,float) to use in sim./rec. programs (cof=0/1-> Q->ADC/ADC->Q)
//                                                      (sdf=0/1-> bar side 1/2 )
//                                                       hlf=0/1-> for A/D)
//
// WARNING : for dynode Side-signal(adc or q) is the SUM of PMTs Gain-equilized signals
// 
  if(cof==0){ // <=== Q->ADCch
    if(hlf==0){//      <-- for anode
      adc=q/adc2qf;
    }
    else{// <-- for Dynode
      adc=q/adc2qf/a2dr[sdf];//q->Aadc->Dadc(equilized sum)
    }
  }
  else{       // <=== ADCch->Q
    if(hlf==0){//      <-- for anode
      q=adc*adc2qf;
    }
    else{// <-- for Dynode
      q=adc*a2dr[sdf]*adc2qf;//Dadc(equil.sum)->Aadc(hi)->q
    }
  }
}
//------
geant TOF2Brcal::adc2mip(int hlf, number amf[2]){ //A/D side1+2 ADC(ch) -> Etot(Mev)
// for dynodes amf[2] are side equilized sums !!!
  number q(0),qt(0);
  for(int isd=0;isd<2;isd++){
    q2a2q(1,isd,hlf,amf[isd],q);//ADC->Qa
    qt+=(q/gaina[isd]);// Qa->Qa_gain_corrected
  }
  qt=qt/mip2q; // Qa(pC)->Mev
  return geant(qt);
}
//------
void TOF2Brcal::adc2q(int hlf, number amf[2], number qs[2]){// side ADC(ch) -> Q(pC)
//                                      to use in calibr. program only for anodes
  for(int isd=0;isd<2;isd++){
    qs[isd]=0.;
    q2a2q(1,isd,hlf,amf[isd],qs[isd]);//ADC->Q
  }
}
//-----
geant TOF2Brcal::poscor(geant point){
//(return light-out corr.factor, input 'point' is Y-coord. in bar loc.r.s.)
  integer nmx=nscanp-1;
  int i;
  geant corr;
  if(point >= yscanp[nmx])corr=relout[nmx]+(relout[nmx]-relout[nmx-1])
                         *(point-yscanp[nmx])/(yscanp[nmx]-yscanp[nmx-1]);
  if(point < yscanp[0])corr=relout[0]+(relout[1]-relout[0])
                         *(point-yscanp[0])/(yscanp[1]-yscanp[0]);
  for(i=0;i<nmx;i++){
    if(point>=yscanp[i] && point<yscanp[i+1])
      corr=relout[i]+(relout[i+1]-relout[i])*(point-yscanp[i])
          /(yscanp[i+1]-yscanp[i]);
  }
  return corr;//you should devide signal by this value later
}
//------
geant TOF2Brcal::tm2t(number tmf[2], number amf[2], int hlf){//(2-sides_times/ADC)->Time (ns)
// for dynodes amf[2] are side equilized sums !!!
  number time,qs,uv(0);
    for(int isd=0;isd<2;isd++){
      q2a2q(1,isd,hlf,amf[isd],qs);// ADCch->Q
      uv+=slops[isd]/sqrt(qs);// summing slops/sqrt(Q), works sl.better
    }
  time=0.5*(tmf[0]+tmf[1])+tzero+slope*uv;
  return geant(time); 
}
//-----
geant TOF2Brcal::tm2tr(number tmf[2]){//tempor, same for raw times(lv3-games)
  number time(0);
  time=0.5*(tmf[0]+tmf[1])+tzero;
  return geant(time); 
}
//-----
void TOF2Brcal::tmd2p(number tmf[2], number amf[2], int hlf,
                              geant &co, geant &eco){//(time-diff)->loc.coord/err(cm)
// for dynodes amf[2] are side equilized sums !!!
  number coo,qs,uv(0);
  for(int isd=0;isd<2;isd++){
    q2a2q(1,isd,hlf,amf[isd],qs);// ADCch->Q
    uv+=(1-2*isd)*slops[isd]/sqrt(qs);// subtr slops/sqrt(Q)
  }
//  uv=0.;//tempor
  coo=-(0.5*(tmf[0]-tmf[1])+slope*uv-yctdif);
//common "-" is due to the fact that Tmeas=Ttrig-Tabs and coo-loc is prop. to Tabs1-Tabs2
  co=td2pos[0]*geant(coo);//coo(ns)->cm                    
  eco=td2pos[1];
}
//-----
void TOF2Brcal::td2ctd(number tdo, number amf[2], int hlf,
                              number &tdc){//td0=0.5*(t[0]-t[1])->tdc (A-corrected)
  number qs,uv(0);
  for(int isd=0;isd<2;isd++){
    q2a2q(1,isd,hlf,amf[isd],qs);// ADCch->Q
    uv+=(1-2*isd)*slops[isd]/sqrt(qs);// subtr slops/sqrt(Q)
  }
  tdc=tdo+slope*uv;
}
//===============================================================================
  TOFBrcalMS::TOFBrcalMS(integer sid, integer sta[2], geant gna[2], 
           geant gnd[2][TOF2GC::PMTSMX], geant a2d[2], geant stra[2][2],
           geant a2q){
    int i,j;
    softid=sid;
    status[0]=sta[0];
    status[1]=sta[1];
    gaina[0]=gna[0];
    gaina[1]=gna[1];
    for(i=0;i<TOF2GC::PMTSMX;i++){
      gaind[0][i]=gnd[0][i];
      gaind[1][i]=gnd[1][i];
    }
    a2dr[0]=a2d[0];
    a2dr[1]=a2d[1];
    strat[0][0]=stra[0][0];
    strat[0][1]=stra[0][1];
    strat[1][0]=stra[1][0];
    strat[1][1]=stra[1][1];
    adc2qf=a2q;
  }
//  ================= TOFBrcalMS class("MC Seeds") functions ======================= :
//
void TOFBrcalMS::build(){// create scbrcal-objects for each sc.bar
//
 integer i,j,k,ila,ibr,ip,ibrm,isd,isp,nsp,ibt,cnum,dnum,mult;
 integer lps=1000;
 integer i1,i2,sta[2],stat[TOF2GC::SCBLMX][2],npm;
 geant r,eff1,eff2;
 integer sid,brt,endflab;
 geant gna[2],a2dr[2],strat[2][2],ah2l[2];
 geant strf[TOF2GC::SCBLMX][2],strof[TOF2GC::SCBLMX][2];
 geant an2di[TOF2GC::SCBLMX][2],gaina[TOF2GC::SCBLMX][2],m2q[TOF2GC::SCBTPN];
 geant aprofp[TOF2GC::SCBTPN][2*TOF2GC::SCPROFP],hblen;
 geant a2drf[TOF2GC::SCBLMX][2];
//
 geant gaind[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];//buff.for dyn.pmts relat.gains
 geant gnd[2][TOF2GC::PMTSMX];
 int ipm; 
 char fname[80];
 char name[80];
 geant a2q,td2p[2];
 char vers1[3]="mc";
 char vers2[3]="sd";
 int mrfp;
//------------------------------
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  strcpy(inum,"0123456789");
//
// ---> read file with the list of version numbers for all needed calib.files :
//
  integer cfvn;
  cfvn=TFCAFFKEY.cfvers%1000;//vers.list-file version number 
  strcpy(name,"tof2cvlist");// basic name for vers.list-file  
  dig=cfvn/100;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
//
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> TOFBrcalMS::build: Opening vers.list-file...  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in);
  if(!vlfile){
    cerr <<"<---- missing vers.list-file !!! "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
    vlfile >> rlvern[i];// second number - for real
  }
  vlfile.close();
//------------------------------------------------
//
//   ---> Prepare to read tof-chan MCSeed calib-status file(used as "MC Seed") :
//
 ctyp=2;//line# corresponding calib-status parameters file
 strcpy(name,"tof2stcf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
   cout <<"      MC_Seed_Calib-status params file is requested..."<<endl;
   dig=rlvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(rlvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   if(TFMCFFKEY.mcseedo==0)strcat(name,vers1);//mc
   else strcat(name,vers2);//sd = copy of rl
//
   strcat(name,".dat");
   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"      Opening file : "<<fname<<'\n';
   ifstream stfile(fname,ios::in); // open file for reading
   if(!stfile){
     cerr <<"<---- missing MC_Seed_Calib-status params file !!! "<<fname<<endl;
     exit(1);
   }
//------------------------------
//   --->  Read MCSeed TOF-channels status values:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
     for(int ipr=0;ipr<2;ipr++){
       stfile >> stat[cnum][ipr];//stat(s1),stat(s2)
     }
     cnum+=1; // sequential counter numbering(0-...)
   } // --- end of bar loop --->
   } // --- end of layer loop --->
//
   stfile >> endflab;//read endfile-label
//
   stfile.close();
//
   if(endflab==12345){
     cout<<"   <--MC_Seed_Calib-status params file successfully read !"<<endl;
   }
   else{cout<<"<---- problems with MC_Seed_Calib-status params file !!!"<<endl;
     exit(1);
   }
//
//------------------------------------------------- 
//
//   ---> Prepare to read MCSeed stretcher_ratios/offs file :
//
// ctyp=3;
// strcpy(name,"tof2srcf");
// mcvn=mcvern[ctyp-1]%1000;
// rlvn=rlvern[ctyp-1]%1000;
//   cout <<" TOFBrcalMS_build: MCSeeds str_ratio/offs params are requested"<<endl;
//   dig=rlvn/100;
//   in[0]=inum[dig];
//   strcat(name,in);
//   dig=(rlvn%100)/10;
//   in[0]=inum[dig];
//   strcat(name,in);
//   dig=rlvn%10;
//   in[0]=inum[dig];
//   strcat(name,in);
//   if(TFMCFFKEY.mcseedo==0)strcat(name,vers1);//mc
//   else strcat(name,vers2);//sd = copy of rl
//
//   strcat(name,".dat");
//   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
//   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
//   strcat(fname,name);
//   cout<<"Open file : "<<fname<<'\n';
//   ifstream scfile(fname,ios::in); // open file for reading
//   if(!scfile){
//     cerr <<"TOFBrcalMS_build: missing TOF MCSeed str_ratio/offsets file "<<fname<<endl;
//     exit(1);
//   }
//-----------------> read str_ratios/offsets:
//
//   cnum=0;
//   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
//   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
//     scfile >> strf[cnum][0];
//     scfile >> strof[cnum][0];
//     scfile >> strf[cnum][1];
//     scfile >> strof[cnum][1];
//     cnum+=1; // sequential counter numbering(0-...)
//   } // --- end of bar loop --->
//   } // --- end of layer loop --->
//
//   scfile >> endflab;//read endfile-label
//
//   scfile.close();
//
//   if(endflab==12345){
//     cout<<"TOFBrcal_buildMS: MCSeed str_ratio/offs file is successfully read !"<<endl;
//   }
//   else{cout<<"TOFBrcalMS_build: ERROR(problems with MCSeed str_ratio/offs file)"<<endl;
//     exit(1);
//   }
//
//-------------------------------------------------------
//
//   --->Prepare to read MCSeed anodes/dynodes relat.gains, anode/dynode ratios,
//       mip2q's, A-profile param.  calib.file :
//
 ctyp=6;
 strcpy(name,"tof2chcf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
   cout <<"      EnergyDeposit-calib MC-Seeds params are requested..."<<endl;
   dig=rlvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(rlvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   if(TFMCFFKEY.mcseedo==0)strcat(name,vers1);//mc
   else strcat(name,vers2);//sd = copy of rl
//
   strcat(name,".dat");
   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"      Open file : "<<fname<<'\n';
   ifstream gcfile(fname,ios::in); // open file for reading
   if(!gcfile){
     cerr <<"<---- missing EnergyDeposit-calib MC-Seeds params file !!! "<<fname<<endl;
     exit(1);
   }
//
// ----------------> read anode relat.gains:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-1
       gcfile >> gaina[cnum+ibr][0];
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-2
       gcfile >> gaina[cnum+ibr][1];
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read anode/dynode ratios:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-1
       gcfile >> a2drf[cnum+ibr][0];
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-2
       gcfile >> a2drf[cnum+ibr][1];
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read dynode-pmts gains(relat to side average):
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){// pm-loop
       for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  //padles loop s-1
         gcfile >> gaind[cnum+ibr][0][ipm]; // read PM=ipm
       }
     }
     for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){// pm-loop
       for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  //padles loop s-2 
         gcfile >> gaind[cnum+ibr][1][ipm];// read PM=ipm
       }
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read mip2q's:
//
   for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){  // <-------- loop over bar-types
     gcfile >> m2q[ibt];
   }
//
// ----------------> read A-prof. parameters:
//
   for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){  // <-------- loop over bar-types
     for(i=0;i<2*TOF2GC::SCPROFP;i++)gcfile >> aprofp[ibt][i];
   }
// ---------------->
   gcfile >> endflab;//read endfile-label
//
   gcfile.close();
//
   if(endflab==12345){
     cout<<"   <--EnergyDeposit-calib MC-Seeds params file is successfully read !"<<endl;
   }
   else{cout<<"<---- problem with EnergyDeposit-calib MC-Seeds params file !!!"<<endl;
     exit(1);
   }
//   
//---------------------------------------------
//   ===> fill TOFBrcal bank :
//
  a2q=TFCAFFKEY.adc2q;//tempor (if variation are high - read special indiv.adc2q's file)
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);
    npm=TOFWScan::scmcscan[brt-1].getnpmts();//get pmts/side 
    hblen=0.5*TOF2DBc::brlen(ila,ibr);
    gna[0]=gaina[cnum][0];
    gna[1]=gaina[cnum][1];
//
    sid=100*(ila+1)+(ibr+1);
//    strat[0][0]=strf[cnum][0];//stretcher param. from ext.file
//    strat[1][0]=strf[cnum][1];
//    strat[0][1]=strof[cnum][0];
//    strat[1][1]=strof[cnum][1];
    strat[0][0]=25;//tempor dummy
    strat[1][0]=25;
    strat[0][1]=1100;
    strat[1][1]=1100;
    sta[0]=stat[cnum][0];
    sta[1]=stat[cnum][1];
    a2dr[0]=a2drf[cnum][0];//an/dyn(hichan) ratios from ext.file
    a2dr[1]=a2drf[cnum][1];
    for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
      gnd[0][ipm]=gaind[cnum][0][ipm];// dyn-pms rel.gains from ext.file
      gnd[1][ipm]=gaind[cnum][1][ipm];// dyn-pms rel.gains from ext.file
    }
    scbrcal[ila][ibr]=TOFBrcalMS(sid,sta,gna,gnd,a2dr,strat,a2q);
//
    cnum+=1;// solid sequential numbering of all counter(0->33)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
  cout<<"<---- TOF2BrcalMS::build: succsessfully done !"<<endl<<endl;
}
//-----------------------------------------------------------------------
//
void TOFBrcalMS::q2a2q(int cof, int sdf, int hlf, number &adc, number &q){  
// Q(pC) <-> ADC(ch,float) to use in sim./rec. programs (cof=0/1-> Q->ADC/ADC->Q)
//                                                      (sdf=0/1-> bar side 1/2 )
//                                                      (hlf=0/1-> for A/D)
//
// WARNING : for dynode(h) Side-signal(adc or q) is the SUM of PMTs Gain-equilized signals
//
  if(cof==0){ // <=== Q->ADCch
    if(hlf==0){//      <-- for anode
      adc=q/adc2qf;
    }
    else{// <-- for Dynode(high)
      adc=q/adc2qf/a2dr[sdf];//q->Aadc->Dadc(equilized sum)
    }
  }
  else{       // <=== ADCch->Q
    if(hlf==0){//      <-- for anode
      q=adc*adc2qf;
    }
    else{// <-- for Dynode
      q=adc*a2dr[sdf]*adc2qf;//Dadc(equil.sum)->Aadc->q
    }
  }
}
//==========================================================================
//
void TOFBPeds::mcbuild(){// create MC TOFBPeds-objects for each sc.bar
//
  int i,j,ila,ibr,cnum,brt;
  integer sid;
  char fname[80];
  char name[80];
  integer asta[TOF2GC::SCBLMX][2];// array of counter stat
  geant aped[TOF2GC::SCBLMX][2];// array of counter peds
  geant asig[TOF2GC::SCBLMX][2];// array of counter sigmas
  integer dsta[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  geant dped[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  geant dsig[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  integer stata[2];
  geant peda[2],siga[2];
  integer statd[2][TOF2GC::PMTSMX];
  geant pedd[2][TOF2GC::PMTSMX],sigd[2][TOF2GC::PMTSMX];
  int ipm;
//
//
//   --->  Read  default MC-pedestals file :
//
  strcpy(name,"tofp_df_mc");
  cout <<"====> TOFBPeds_mcbuild: default MC-peds file will be used..."<<endl;
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<"      AMS02 TOF:8/8/10/8-pads/layer setup selected."<<endl;
  }
  else
  {
        cout <<"      Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"      missing default pedestals-file !!! "<<fname<<endl;
    exit(1);
  }
//---> Read anode:
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    for(i=0;i<2;i++){// sequence: side1,side2
      icfile >> asta[cnum][i];
      icfile >> aped[cnum][i];
      icfile >> asig[cnum][i];
    }
    cnum+=1;// sequential counter numbering(0-...)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
//---> Read dynode:
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    for(i=0;i<2;i++){//<-- side-loop
      for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){//<-- pm-loop
        icfile >> dsta[cnum][i][ipm];
        icfile >> dped[cnum][i][ipm];
        icfile >> dsig[cnum][i][ipm];
      }
    }
    cnum+=1;// sequential counter numbering(0-...)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
  icfile.close();
//---------------------------------------------
//   ===> fill TOFBPeds bank :
//
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);
    sid=100*(ila+1)+(ibr+1);
    for(i=0;i<2;i++){
      stata[i]=asta[cnum][i];
      peda[i]=aped[cnum][i];
      siga[i]=asig[cnum][i];

      for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){	
        statd[i][ipm]=dsta[cnum][i][ipm];
        pedd[i][ipm]=dped[cnum][i][ipm];
        sigd[i][ipm]=dsig[cnum][i][ipm];
      }
    }
//
    scbrped[ila][ibr]=TOFBPeds(sid,stata,peda,siga,statd,pedd,sigd);
//
    cnum+=1;
  } // --- end of bar loop --->
  } // --- end of layer loop --->
  cout<<"<---- TOFBPeds_mcbuild: succsessfully done !"<<endl<<endl;
}
//==========================================================================
//
void TOFBPeds::build(){// tempor solution for RealData peds.
//
  int i,j,ila,ibr,cnum,brt;
  integer sid;
  char fname[80];
  char name[80];
  integer asta[TOF2GC::SCBLMX][2];// array of counter stat
  geant aped[TOF2GC::SCBLMX][2];// array of counter peds
  geant asig[TOF2GC::SCBLMX][2];// array of counter sigmas
  integer dsta[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  geant dped[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  geant dsig[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  integer stata[2];
  geant peda[2],siga[2];
  integer statd[2][TOF2GC::PMTSMX];
  geant pedd[2][TOF2GC::PMTSMX],sigd[2][TOF2GC::PMTSMX];
  int ipm;
//
//
//   --->  Read pedestals file :
//
  strcpy(name,"tofp_df_rl");
  cout <<"====> TOFBPeds_build: default RD-peds file will be used..."<<endl;
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<"      AMS02 TOF:8/8/10/8-pads/layer setup selected..."<<endl;
  }
  else
  {
        cout <<"      Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"      missing default pedestals-file !!! "<<fname<<endl;
    exit(1);
  }
//---> Read anode:
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    for(i=0;i<2;i++){// sequence: side1,side2
      icfile >> asta[cnum][i];
      icfile >> aped[cnum][i];
      icfile >> asig[cnum][i];
    }
    cnum+=1;// sequential counter numbering(0-...)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
//---> Read dynode:
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    for(i=0;i<2;i++){//<-- side-loop
      for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){//<-- pm-loop
        icfile >> dsta[cnum][i][ipm];
        icfile >> dped[cnum][i][ipm];
        icfile >> dsig[cnum][i][ipm];
      }
    }
    cnum+=1;// sequential counter numbering(0-...)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
  icfile.close();
//---------------------------------------------
//   ===> fill TOFBPeds bank :
//
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);
    sid=100*(ila+1)+(ibr+1);
    for(i=0;i<2;i++){
      stata[i]=asta[cnum][i];
      peda[i]=aped[cnum][i];
      siga[i]=asig[cnum][i];
	
      for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){	
        statd[i][ipm]=dsta[cnum][i][ipm];
        pedd[i][ipm]=dped[cnum][i][ipm];
        sigd[i][ipm]=dsig[cnum][i][ipm];
      }
    }
//
    scbrped[ila][ibr]=TOFBPeds(sid,stata,peda,siga,statd,pedd,sigd);
//
    cnum+=1;
  } // --- end of bar loop --->
  } // --- end of layer loop --->
  cout<<"<---- TOFBPeds_build: succsessfully done !"<<endl<<endl;
}
//
//==========================================================================
//
//   TOFJobStat static variables definition (memory reservation):
//
integer TOF2JobStat::mccount[TOF2GC::SCJSTA];
integer TOF2JobStat::recount[TOF2GC::SCJSTA];
integer TOF2JobStat::chcount[TOF2GC::SCCHMX][TOF2GC::SCCSTA];
integer TOF2JobStat::brcount[TOF2GC::SCBLMX][TOF2GC::SCCSTA];
integer TOF2JobStat::daqsf[30];
integer TOF2JobStat::cratr[TOF2GC::SCCRAT][20];
integer TOF2JobStat::cratp[TOF2GC::SCCRAT][20];
integer TOF2JobStat::cratc[TOF2GC::SCCRAT][20];
integer TOF2JobStat::cratm[TOF2GC::SCCRAT][20];
integer TOF2JobStat::sltr[TOF2GC::SCCRAT][TOF2GC::SCSLTM][20];
integer TOF2JobStat::sltp[TOF2GC::SCCRAT][TOF2GC::SCSLTM][20];
integer TOF2JobStat::rdcr1[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcr2[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcr3[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcr4[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcp1[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcp2[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcp3[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcp4[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
geant TOF2JobStat::tofantemp[TOF2GC::SCCRAT][TOF2GC::SCFETA];
//
void TOF2JobStat::daqsfr(int16u ie){
  assert(ie<30);
  daqsf[ie]+=1;
}
void TOF2JobStat::daqscr(int16u df, int16u crat, int16u ie){
  assert(crat<TOF2GC::SCCRAT && ie<20);
  if(df==0)cratr[crat][ie]+=1;//raw
  else if(df==1)cratp[crat][ie]+=1;//processed
  else if(df==2)cratc[crat][ie]+=1;//pedcal
  else cratm[crat][ie]+=1;//mixed: raw/comp comparison
}
void TOF2JobStat::daqssl(int16u df, int16u crat, int16u slot, int16u ie){
  assert(crat<TOF2GC::SCCRAT && slot<TOF2GC::SCSLTM && ie<20);
  if(df==0)sltr[crat][slot][ie]+=1;
  else sltp[crat][slot][ie]+=1;
}
void TOF2JobStat::daqsch(int16u df, int16u crat, int16u slot, int16u rdch, int16u ie){
  assert(crat<TOF2GC::SCCRAT && slot<TOF2GC::SCSLTM && rdch<TOF2GC::SCRCHM && ie<20);
  if(df==0)
  {
    if(crat==0)rdcr1[slot][rdch][ie]+=1;
    if(crat==1)rdcr2[slot][rdch][ie]+=1;
    if(crat==2)rdcr3[slot][rdch][ie]+=1;
    if(crat==3)rdcr4[slot][rdch][ie]+=1;
  }
  else
  {
    if(crat==0)rdcp1[slot][rdch][ie]+=1;
    if(crat==1)rdcp2[slot][rdch][ie]+=1;
    if(crat==2)rdcp3[slot][rdch][ie]+=1;
    if(crat==3)rdcp4[slot][rdch][ie]+=1;
  }
}

//
// function to print Job-statistics at the end of JOB(RUN):
//
void TOF2JobStat::printstat(){
  int il,ib,ic,icr,isl,ie;
  integer rdcr[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  char slnames[TOF2GC::SCSLTM][8];
  strcpy(slnames[0],"SDR    ");
  strcpy(slnames[1],"SFET_01");
  strcpy(slnames[2],"SFET_02");
  strcpy(slnames[3],"SPT    ");
  strcpy(slnames[4],"SFET_03");
  strcpy(slnames[5],"SFET_04");
  strcpy(slnames[6],"SFEA_01");
  strcpy(slnames[7],"SFEC_01");
  strcpy(slnames[8],"SFEC_02");
  strcpy(slnames[9],"SFEC_03");
  strcpy(slnames[10],"SFEC_04");
  geant rc;
//
  if(daqsf[0]>0 && MISCFFKEY.dbwrbeg==0){//not empty, not dwwriter job
  printf("\n");
  printf("    ======================= JOB DAQ-decoding statistics ====================\n");
  printf("\n");
  printf("Total calls to SDRsegment-decoding : %7d\n",daqsf[0]);
  printf("Rejected as empty or errored       : %7d\n",daqsf[1]);
  printf("NonEmpty segments (raw/com/mix/ped): %7d %7d %7d %7d\n",daqsf[2],daqsf[3],daqsf[4],daqsf[8]);
  printf(" + No SegmentBuild ErrFlags found  : %7d %7d %7d %7d\n",daqsf[5],daqsf[6],daqsf[7],daqsf[9]);
  printf("\n");
//
  if(daqsf[2]>0 || daqsf[4]>0){
  printf("----------------> RawFMT Segment Decoding statistics: \n\n");
  printf("     Crate:                    1         2         3         4\n");
  printf("Total Segments         :  %7d   %7d   %7d   %7d\n",cratr[0][0],cratr[1][0],cratr[2][0],cratr[3][0]);
  printf("length ovfl(build)     :  %7d   %7d   %7d   %7d\n",cratr[0][1],cratr[1][1],cratr[2][1],cratr[3][1]);
  printf("length error           :  %7d   %7d   %7d   %7d\n",cratr[0][2],cratr[1][2],cratr[2][2],cratr[3][2]);
  printf("LinkErr in Q-blk       :  %7d   %7d   %7d   %7d\n",cratr[0][3],cratr[1][3],cratr[2][3],cratr[3][3]);
  printf("SlotErr in Q-blk       :  %7d   %7d   %7d   %7d\n",cratr[0][4],cratr[1][4],cratr[2][4],cratr[3][4]);
  printf("Wrong words in TP-blk  :  %7d   %7d   %7d   %7d\n",cratr[0][5],cratr[1][5],cratr[2][5],cratr[3][5]);
  printf("Time-out   in TP-blk   :  %7d   %7d   %7d   %7d\n",cratr[0][6],cratr[1][6],cratr[2][6],cratr[3][6]);
  printf("LinkErr in Time-blk    :  %7d   %7d   %7d   %7d\n",cratr[0][7],cratr[1][7],cratr[2][7],cratr[3][7]);
  printf("SlotErr in Time-blk    :  %7d   %7d   %7d   %7d\n",cratr[0][8],cratr[1][8],cratr[2][8],cratr[3][8]);
  printf("\n");
  printf("-----> Slots statistics(words counting) :\n\n");
  printf("SFET_1  Q-entries      :  %7d   %7d   %7d   %7d\n",sltr[0][1][0],sltr[1][1][0],sltr[2][1][0],sltr[3][1][0]);
  printf("SFET_2  Q-entries      :  %7d   %7d   %7d   %7d\n",sltr[0][2][0],sltr[1][2][0],sltr[2][2][0],sltr[3][2][0]);
  printf("SFET_3  Q-entries      :  %7d   %7d   %7d   %7d\n",sltr[0][4][0],sltr[1][4][0],sltr[2][4][0],sltr[3][4][0]);
  printf("SFET_4  Q-entries      :  %7d   %7d   %7d   %7d\n",sltr[0][5][0],sltr[1][5][0],sltr[2][5][0],sltr[3][5][0]);
  printf("SFEA_1  Q-entries      :  %7d   %7d   %7d   %7d\n",sltr[0][6][0],sltr[1][6][0],sltr[2][6][0],sltr[3][6][0]);
  printf("SFEC_1    entries      :  %7d   %7d   %7d   %7d\n",sltr[0][7][0],sltr[1][7][0],sltr[2][7][0],sltr[3][7][0]);
  printf("SFEC_2    entries      :  %7d   %7d   %7d   %7d\n",sltr[0][8][0],sltr[1][8][0],sltr[2][8][0],sltr[3][8][0]);
  printf("SFEC_3    entries      :  %7d   %7d   %7d   %7d\n",sltr[0][9][0],sltr[1][9][0],sltr[2][9][0],sltr[3][9][0]);
  printf("SFEC_4    entries      :  %7d   %7d   %7d   %7d\n",sltr[0][10][0],sltr[1][10][0],sltr[2][10][0],sltr[3][10][0]);
  printf("\n");
  printf("SFET_1 TDCbuff-entries :  %7d   %7d   %7d   %7d\n",sltr[0][1][1],sltr[1][1][1],sltr[2][1][1],sltr[3][1][1]);
  printf("SFET_2   ...........   :  %7d   %7d   %7d   %7d\n",sltr[0][2][1],sltr[1][2][1],sltr[2][2][1],sltr[3][2][1]);
  printf("SFET_3   ...........   :  %7d   %7d   %7d   %7d\n",sltr[0][4][1],sltr[1][4][1],sltr[2][4][1],sltr[3][4][1]);
  printf("SFET_4   ...........   :  %7d   %7d   %7d   %7d\n",sltr[0][5][1],sltr[1][5][1],sltr[2][5][1],sltr[3][5][1]);
  printf("SFEA_1   ...........   :  %7d   %7d   %7d   %7d\n",sltr[0][6][1],sltr[1][6][1],sltr[2][6][1],sltr[3][6][1]);
  printf("\n");
  printf("SFET_1  TDCbuff-ovfl   :  %7d   %7d   %7d   %7d\n",sltr[0][1][2],sltr[1][1][2],sltr[2][1][2],sltr[3][1][2]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][2],sltr[1][2][2],sltr[2][2][2],sltr[3][2][2]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][2],sltr[1][4][2],sltr[2][4][2],sltr[3][4][2]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][2],sltr[1][5][2],sltr[2][5][2],sltr[3][5][2]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][2],sltr[1][6][2],sltr[2][6][2],sltr[3][6][2]);
  printf("\n");
  printf("SFET_1  NonTDC_Wtype   :  %7d   %7d   %7d   %7d\n",sltr[0][1][3],sltr[1][1][3],sltr[2][1][3],sltr[3][1][3]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][3],sltr[1][2][3],sltr[2][2][3],sltr[3][2][3]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][3],sltr[1][4][3],sltr[2][4][3],sltr[3][4][3]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][3],sltr[1][5][3],sltr[2][5][3],sltr[3][5][3]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][3],sltr[1][6][3],sltr[2][6][3],sltr[3][6][3]);
  printf("\n");
  printf("SFET_1 TDC_BadStr/Tout :  %7d   %7d   %7d   %7d\n",sltr[0][1][4],sltr[1][1][4],sltr[2][1][4],sltr[3][1][4]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][4],sltr[1][2][4],sltr[2][2][4],sltr[3][2][4]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][4],sltr[1][4][4],sltr[2][4][4],sltr[3][4][4]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][4],sltr[1][5][4],sltr[2][5][4],sltr[3][5][4]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][4],sltr[1][6][4],sltr[2][6][4],sltr[3][6][4]);
  printf("\n");
  printf("SFET_1 TDC_InternErr   :  %7d   %7d   %7d   %7d\n",sltr[0][1][5],sltr[1][1][5],sltr[2][1][5],sltr[3][1][5]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][5],sltr[1][2][5],sltr[2][2][5],sltr[3][2][5]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][5],sltr[1][4][5],sltr[2][4][5],sltr[3][4][5]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][5],sltr[1][5][5],sltr[2][5][5],sltr[3][5][5]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][5],sltr[1][6][5],sltr[2][6][5],sltr[3][6][5]);
  printf("\n");
  printf("SFET_1  TDC_Data_OK    :  %7d   %7d   %7d   %7d\n",sltr[0][1][6],sltr[1][1][6],sltr[2][1][6],sltr[3][1][6]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][6],sltr[1][2][6],sltr[2][2][6],sltr[3][2][6]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][6],sltr[1][4][6],sltr[2][4][6],sltr[3][4][6]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][6],sltr[1][5][6],sltr[2][5][6],sltr[3][5][6]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][6],sltr[1][6][6],sltr[2][6][6],sltr[3][6][6]);
  printf("\n");
  printf("-----> Charge-channels statistics :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcr1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcr2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcr3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcr4[isl][ic][ie];
	}
      }
      cout<<"     entrs/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][0];
      cout<<endl;
    }
  }
  printf("\n");
  printf("-----> Time-channels statistics :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3 || isl>=7)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcr1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcr2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcr3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcr4[isl][ic][ie];
	}
      }
      cout<<"     entrs/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][1];
      cout<<endl;
    }
  }
  printf("\n");
  printf("----->        Found Nhits overflows in Time-channel :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3 || isl>=7)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcr1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcr2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcr3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcr4[isl][ic][ie];
	}
      }
      cout<<"     overfls/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][2];
      cout<<endl;
    }
  }
  printf("\n");
  }
//----
  if(daqsf[3]>0 || daqsf[4]>0){
  printf("---------------> Compressed(Processed)FMT Segment Decoding statistics: \n\n");
  printf("     Crate:                       1       2       3       4\n");
  printf(" Entries(alone||inMixtFMT):    %7d %7d %7d %7d\n",cratp[0][0],cratp[1][0],cratp[2][0],cratp[3][0]);
  printf(" ...inMix when RawSubs OK :    %7d %7d %7d %7d\n",cratp[0][1],cratp[1][1],cratp[2][1],cratp[3][1]);
  printf(" trunc_len(in MixtFMT,rej):    %7d %7d %7d %7d\n",cratp[0][2],cratp[1][2],cratp[2][2],cratp[3][2]);
//  printf(" evn_mism(in MixtFMT,rej) :    %7d %7d %7d %7d\n",cratp[0][3],cratp[1][3],cratp[2][3],cratp[3][3]);
  printf(" len_mism(inside Mixt,rej):    %7d %7d %7d %7d\n",cratp[0][4],cratp[1][4],cratp[2][4],cratp[3][4]);
  printf(" len_mism(stand-alone,rej):    %7d %7d %7d %7d\n",cratp[0][5],cratp[1][5],cratp[2][5],cratp[3][5]);
  printf("\n");
  printf(" TrPatt-block IllegalWord :    %7d %7d %7d %7d\n",cratp[0][6],cratp[1][6],cratp[2][6],cratp[3][6]);
  printf(" ................Timeout  :    %7d %7d %7d %7d\n",cratp[0][7],cratp[1][7],cratp[2][7],cratp[3][7]);
  printf(" ...StatVerifMask problems:    %7d %7d %7d %7d\n",cratp[0][8],cratp[1][8],cratp[2][8],cratp[3][8]);
  printf(" Q-block LinkHeaderErr    :    %7d %7d %7d %7d\n",cratp[0][9],cratp[1][9],cratp[2][9],cratp[3][9]);
  printf(" IllegSlotNumb in Q-block :    %7d %7d %7d %7d\n",cratp[0][10],cratp[1][10],cratp[2][10],cratp[3][10]);
  printf("\n");
  printf(" T-Blk Raw->Comp linkErr  :    %7d %7d %7d %7d\n",cratp[0][11],cratp[1][11],cratp[2][11],cratp[3][11]);
  printf(" Raw->Comp FatalErrLink0  :    %7d %7d %7d %7d\n",cratp[0][12],cratp[1][12],cratp[2][12],cratp[3][12]);
  printf(" Raw->Comp FatalErrLink1  :    %7d %7d %7d %7d\n",cratp[0][13],cratp[1][13],cratp[2][13],cratp[3][13]);
  printf(" Raw->Comp FatalErrLink2  :    %7d %7d %7d %7d\n",cratp[0][14],cratp[1][14],cratp[2][14],cratp[3][14]);
  printf(" Raw->Comp FatalErrLink3  :    %7d %7d %7d %7d\n",cratp[0][15],cratp[1][15],cratp[2][15],cratp[3][15]);
  printf(" Raw->Comp FatalErrLink4  :    %7d %7d %7d %7d\n",cratp[0][16],cratp[1][16],cratp[2][16],cratp[3][16]);
  printf(" Invalid slot number      :    %7d %7d %7d %7d\n",cratp[0][17],cratp[1][17],cratp[2][17],cratp[3][17]);
  printf("\n");
  printf("-----> Slots statistics(words counting) :\n\n");
  printf("SFET_1  Q-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][1][0],sltp[1][1][0],sltp[2][1][0],sltp[3][1][0]);
  printf("SFET_2  Q-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][2][0],sltp[1][2][0],sltp[2][2][0],sltp[3][2][0]);
  printf("SFET_3  Q-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][4][0],sltp[1][4][0],sltp[2][4][0],sltp[3][4][0]);
  printf("SFET_4  Q-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][5][0],sltp[1][5][0],sltp[2][5][0],sltp[3][5][0]);
  printf("SFEA_1  Q-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][6][0],sltp[1][6][0],sltp[2][6][0],sltp[3][6][0]);
  printf("SFEC_1    entries      :  %7d   %7d   %7d   %7d\n",sltp[0][7][0],sltp[1][7][0],sltp[2][7][0],sltp[3][7][0]);
  printf("SFEC_2    entries      :  %7d   %7d   %7d   %7d\n",sltp[0][8][0],sltp[1][8][0],sltp[2][8][0],sltp[3][8][0]);
  printf("SFEC_3    entries      :  %7d   %7d   %7d   %7d\n",sltp[0][9][0],sltp[1][9][0],sltp[2][9][0],sltp[3][9][0]);
  printf("SFEC_4    entries      :  %7d   %7d   %7d   %7d\n",sltp[0][10][0],sltp[1][10][0],sltp[2][10][0],sltp[3][10][0]);
  printf("\n");
  printf("SFET_1 too low adc-ped :  %7d   %7d   %7d   %7d\n",sltp[0][1][1],sltp[1][1][1],sltp[2][1][1],sltp[3][1][1]);
  printf("SFET_2     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][2][1],sltp[1][2][1],sltp[2][2][1],sltp[3][2][1]);
  printf("SFET_3     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][4][1],sltp[1][4][1],sltp[2][4][1],sltp[3][4][1]);
  printf("SFET_4     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][5][1],sltp[1][5][1],sltp[2][5][1],sltp[3][5][1]);
  printf("SFEA_1     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][6][1],sltp[1][6][1],sltp[2][6][1],sltp[3][6][1]);
  printf("SFEC_1     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][7][1],sltp[1][7][1],sltp[2][7][1],sltp[3][7][1]);
  printf("SFEC_2     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][8][1],sltp[1][8][1],sltp[2][8][1],sltp[3][8][1]);
  printf("SFEC_3     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][9][1],sltp[1][9][1],sltp[2][9][1],sltp[3][9][1]);
  printf("SFEC_4     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][10][1],sltp[1][10][1],sltp[2][10][1],sltp[3][10][1]);
  printf("\n");
  printf("SFET_1  T-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][1][2],sltp[1][1][2],sltp[2][1][2],sltp[3][1][2]);
  printf("SFET_2  T-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][2][2],sltp[1][2][2],sltp[2][2][2],sltp[3][2][2]);
  printf("SFET_3  T-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][4][2],sltp[1][4][2],sltp[2][4][2],sltp[3][4][2]);
  printf("SFET_4  T-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][5][2],sltp[1][5][2],sltp[2][5][2],sltp[3][5][2]);
  printf("SFEA_1  T-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][6][2],sltp[1][6][2],sltp[2][6][2],sltp[3][6][2]);
  printf("\n");
  printf("SFET_1  with no temper :  %7d   %7d   %7d   %7d\n",sltp[0][1][3],sltp[1][1][3],sltp[2][1][3],sltp[3][1][3]);
  printf("SFET_2     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][2][3],sltp[1][2][3],sltp[2][2][3],sltp[3][2][3]);
  printf("SFET_3     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][4][3],sltp[1][4][3],sltp[2][4][3],sltp[3][4][3]);
  printf("SFET_4     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][5][3],sltp[1][5][3],sltp[2][5][3],sltp[3][5][3]);
  printf("SFEA_1     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][6][3],sltp[1][6][3],sltp[2][6][3],sltp[3][6][3]);
  printf("\n");
  printf("SFET_1  with err-word  :  %7d   %7d   %7d   %7d\n",sltp[0][1][4],sltp[1][1][4],sltp[2][1][4],sltp[3][1][4]);
  printf("SFET_2     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][2][4],sltp[1][2][4],sltp[2][2][4],sltp[3][2][4]);
  printf("SFET_3     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][4][4],sltp[1][4][4],sltp[2][4][4],sltp[3][4][4]);
  printf("SFET_4     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][5][4],sltp[1][5][4],sltp[2][5][4],sltp[3][5][4]);
  printf("SFEA_1     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][6][4],sltp[1][6][4],sltp[2][6][4],sltp[3][6][4]);
  printf("\n");
  printf("-----> Charge-channels statistics :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcp1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcp2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcp3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcp4[isl][ic][ie];
	}
      }
      cout<<"     entrs/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][0];
      cout<<endl;
    }
  }
  printf("\n");
  printf("-----> Time-channels statistics :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3 || isl>=7)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcp1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcp2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcp3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcp4[isl][ic][ie];
	}
      }
      cout<<"     entrs/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][1];
      cout<<endl;
    }
  }
  printf("\n");
  printf("----->        Found Nhits overflows in Time-channel :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3 || isl>=7)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcp1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcp2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcp3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcp4[isl][ic][ie];
	}
      }
      cout<<"     overfls/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][2];
      cout<<endl;
    }
  }
  printf("\n");
  }
//---
  if(daqsf[8]>0){
  printf("---------------> ONBoardPedCalFMT Segment Decoding statistics: \n\n");
  printf("     Crate:                  1       2       3       4\n");
  printf("TofAnt PedCal blocks :    %7d %7d %7d %7d\n",cratc[0][0],cratc[1][0],cratc[2][0],cratc[3][0]);
  printf("       wrong length  :    %7d %7d %7d %7d\n",cratc[0][1],cratc[1][1],cratc[2][1],cratc[3][1]);
  printf("       invalid slot# :    %7d %7d %7d %7d\n",cratc[0][2],cratc[1][2],cratc[2][2],cratc[3][2]);
  printf("\n");
  }
//---
  if(daqsf[4]>0){
  printf("---------------> MixtFMT: Raw/Compr-SubSegment comparison statistics: \n\n");
  printf("     Crate:                        1       2       3       4\n");
  printf("Crate-entries(Raw/Com OK):    %7d %7d %7d %7d\n",cratm[0][0],cratm[1][0],cratm[2][0],cratm[3][0]);
  printf("sw-ch with Nh/Id mismatch:    %7d %7d %7d %7d\n",cratm[0][1],cratm[1][1],cratm[2][1],cratm[3][1]);
  printf("sw-ch Hit-value mismatch :    %7d %7d %7d %7d\n",cratm[0][2],cratm[1][2],cratm[2][2],cratm[3][2]);
  printf("Errorless Crate-entries  :    %7d %7d %7d %7d\n",cratm[0][3],cratm[1][3],cratm[2][3],cratm[3][3]);
  printf("\n");
  }
//---
  }
//
//
  printf("\n");
  if(MISCFFKEY.dbwrbeg>0)return;//dbwritwr job, don't need any statistics
//
  printf("    ====================== JOB TOF-statistics ======================\n");
  printf("\n");
  printf(" MC: entries                                : % 6d\n",mccount[0]);
  printf("   MCGen FastSel OK                         : % 6d\n",mccount[14]);
  printf("   Ghits->TovT OK(err.stat.follow)          : % 6d\n",mccount[1]);
  printf("        OutOfTime GHits                     : % 6d\n",mccount[11]);
  printf("        out of total GHits                  : % 6d\n",mccount[12]);
  printf("        Bars with OutOfSizeHits             : % 6d\n",mccount[3]);
  printf("        out of bars fired                   : % 6d\n",mccount[4]);
  printf("        FlashADC time-scale overflows       : % 6d\n",mccount[5]);
  printf("        LTtime-hits buffer overflows        : % 6d\n",mccount[25]);
  printf("        SumHTtime-hits buffer overflows     : % 6d\n",mccount[26]);
  printf("        SumSHTtime-hits buffer overflows    : % 6d\n",mccount[27]);
  printf("   TovT->RawEvent(FT) OK(err.stat.follow)   : % 6d\n",mccount[2]);
  printf("        TooOld Time-hits                    : % 6d\n",mccount[7]);
  printf("        TooYoung Time-hits                  : % 6d\n",mccount[8]);
  printf("        Anode-ADC overflows                 : % 6d\n",mccount[9]);
  printf("        Dynode-ADC overflows                : % 6d\n",mccount[10]);
  printf("   FastTrigStatistics :\n");
  printf("        ParticleTrigger requests             : % 6d\n",mccount[15]);
  printf("        found TOF-FTC(Z>=1)                  : % 6d\n",mccount[16]);
  printf("        found TOF-BZ(Z>=2 when FTC)          : % 6d\n",mccount[17]);
  printf("        found TOF-FTZ(SlowZ>=2)              : % 6d\n",mccount[18]);
  printf("        found TOF-FTZ when FTC missing       : % 6d\n",mccount[23]);
  printf("        found EC-FTE                         : % 6d\n",mccount[19]);
  printf("        found EC-FTE when TOF-FT missing     : % 6d\n",mccount[20]);
  printf("        found globFT(after masking)          : % 6d\n",mccount[21]);
  printf("        ExternalTrigger requests             : % 6d\n",mccount[22]);
  
  
  printf(" RECO-entries                               : % 6d\n",recount[0]);
  printf("   LVL1-trig OK                             : % 6d\n",recount[1]);
  printf("   LVL1 with TOF flag                       : % 6d\n",recount[33]);
  printf("   LVL1 with only ECAL flag                 : % 6d\n",recount[34]);
  printf("   RawSide-validation OK                    : % 6d\n",recount[2]);
  printf("   RawSide->RawCluster  OK                  : % 6d\n",recount[3]);
  printf("   RawCluster->Cluster OK                   : % 6d\n",recount[4]);
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTOF){
    printf(" Entries to TZSl-calibr.   : % 6d\n",recount[6]);
    printf(" TZSl: multiplicity OK     : % 6d\n",recount[7]);
    printf(" TZSl: Anti,Bet,Spks,Qtr OK: % 6d\n",recount[8]);
    printf(" TZSl: Tracker mom. OK     : % 6d\n",recount[9]);
    printf(" TZSl: TOF-Tr.match. OK    : % 6d\n",recount[10]);
    printf(" TZSl: TOF-track quality OK: % 6d\n",recount[20]);
    printf(" Entries to AMPL-calibr.   : % 6d\n",recount[11]);
    printf(" AMPL: multiplicity OK     : % 6d\n",recount[12]);
    printf(" AMPL: noANTI,no albedo    : % 6d\n",recount[13]);
    printf(" AMPL: Track momentum OK   : % 6d\n",recount[14]);
    printf(" AMPL: TOF-TRK match OK    : % 6d\n",recount[15]);
    printf(" AMPL: TOF beta-fit OK     : % 6d\n",recount[30]);
    printf(" AMPL: Mom.range OK        : % 6d\n",recount[31]);
    printf(" AMPL: Eloss match MIP     : % 6d\n",recount[32]);
    printf(" Entr to STRR-calibr       : % 6d\n",recount[16]);
    printf(" Entries to TDIF-calibr.   : % 6d\n",recount[17]);
    printf(" TDIF: multiplicity OK     : % 6d\n",recount[18]);
    printf(" TDIF: Tracker OK          : % 6d\n",recount[19]);
    printf(" Entr to PEDS-calibr       : % 6d\n",recount[38]);
  }
  else{
    printf(" TOFUser entries            : % 6d\n",recount[21]);
    printf("   TOFU: NofANTIsectors Low : % 6d\n",recount[22]);
    printf("   TOFU: Clusters/layer Low : % 6d\n",recount[23]);
    printf("   TOFU: ParticleWithTrack  : % 6d\n",recount[24]);
    printf("   TOFU: TrkMom/FalsTofX OK : % 6d\n",recount[25]);
    printf("   TOFU: TOF-TRK match OK   : % 6d\n",recount[26]);
  }
  printf("\n\n");
//
  if(TFREFFKEY.reprtf[0]==0)return;
//
//----------------------------------------------------------
//
  printf("===========> Channels validation report :\n\n");
//
  printf("Validation entries per channel:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      printf(" % 6d",chcount[ic][12]);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      printf(" % 6d",chcount[ic][12]);
    }
    printf("\n\n");
  }
//
  printf("Missing FTtime info(fatal case)[%]  :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][13])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][13])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Multiple FTtime info[%]  :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][14])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][14])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Missing LTtime info(fatal case)[%] :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][15])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][15])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Missing Anode info[%] :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][16])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][16])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Missing Dynode when Anode is PUX-saturated[%] :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][17])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][17])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Missing History(SumHT) info [%] :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][18])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][18])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
//
//----------------------------------------------------------
  if(TFREFFKEY.relogic[0]==1)return;// no reco for strr-calibr
//
  printf("==========> Bars reconstruction report :\n\n");
//
  printf("Bar entries with at least one ALIVE side(having [FTtime+smth] + DBok:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      printf(" % 6d",brcount[ic][0]);
    }
    printf("\n\n");
  }
//
  printf("Bars fraction with at least 1 side with COMPLETE(LT & Q & [SumHT]) measurement:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][1])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Bars fraction with 'HISTORY-OK' on COMPLETE sides:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][2])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Bars fraction with 'bestLT-hit MATCHING'(if required)' on COMPLETE sides :\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][3])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Bar fraction with COMPLETE and HISTORY/MATCHING 'OK' measurements on BOTH sides:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][4])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
//---------------------------------------------------------
  printf("============> Channels reconstruction report :\n\n");
//
  printf("RawSide channel entries(FTtime is mandatory) :\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n\n");
  }
//
  printf("FT-correlated LTtime-hits found :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("SumHTtime-hits found :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("SumSHTtime-hits found :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("AnodeADC found :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("DynodeADC found :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][5])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][5])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("FTtdc has '1 hit' :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Single FT-correlated LTtime-hit found :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("FT-correlated LTtime-hits AND (Anode|Dynode)-hits found :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("As above, but history(SumHT)-channel is also required :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][11])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][11])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Anode PUX-chip overflows :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][9])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][9])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Dynode PUX-chip overflows(in >= 1pm/side) :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][10])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][10])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
//
}
//------------------------------------------
void TOF2JobStat::bookhist(){
  int i,j,k,ich,il,ib,ii,jj,ic,is;
  char htit1[60];
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
//
  if(TFREFFKEY.reprtf[2]!=0){// Reconstruction histograms
// Book reco-hist
    HBOOK1(1100,"LT/SumHT-time difference(SINGLE LT/SumHT-hit events)",80,-10.,10.,0.);
    HBOOK1(1107,"TOF+CTC+ANTI data length (16-bit words)",80,1.,1001.,0.);
    HBOOK1(1101,"Time_history:befor_hit dist(ns)",80,0.,2400.,0.);
    HBOOK1(1102,"Time_history:after_hit dist(ns)",80,0.,400.,0.);
//    HBOOK1(1103,"Time_history: TovT(ns)",80,0.,120.,0.);
    HBOOK1(1104,"Anode-adc signals(id=104,s1)",100,0.,1000.,0.);
    HBOOK1(1105,"Dynode(pm)-adc signals(id=104,s1)",100,0.,200.,0.);
    HBOOK1(1108,"SumHTtime-channel hits multiplicity(all chan)",20,0.,20.,0.);
    HBOOK1(1110,"RawCluster:Total fired layers per event",5,0.,5.,0.);
    HBOOK1(1111,"RawCluster:Layer appearence frequency",4,0.5,4.5,0.);
    HBOOK1(1112,"RawCluster:Layer-Config(-1/0/1.../5--> <2/2/missLay#/All4)",10,-1.,9.,0.);
    HBOOK1(1113,"RawCluster:OneBarPerLayer Layer-Config(<2/2/missLay/All4)",10,-1.,9.,0.);
    HBOOK1(1114,"RawCluster:One2SidedBarPerLayer Layer-Config(<2/2/missLay/All4)",10,-1.,9.,0.);
    HBOOK1(1115,"BestMatch LT/SumHT-time diff(FTmatched, any multipl.events)",80,-10.,10.,0.);
    HBOOK1(1116,"dEdX vs bar (norm.inc.,L=1)",10,0.,10.,0.);
    HBOOK1(1117,"dEdX vs bar (norm.inc.,L=2)",10,0.,10.,0.);
    HBOOK1(1118,"dEdX vs bar (norm.inc.,L=3)",10,0.,10.,0.);
    HBOOK1(1119,"dEdX vs bar (norm.inc.,L=4)",10,0.,10.,0.);
//    HBOOK1(1095-1098 are used for trigger-hists in trigger102.C  !!!!)
//    HBOOK1(1099,...    reserved for tofsim02.C internal use !!!!
//    HBOOK1(1094,...    reserved for tofsim02.C internal use !!!!
    HBOOK1(1092,"TOF:Ttop-Tbot(LVL3)",50,-12.5,12.5,0.);
    if(TFREFFKEY.reprtf[2]>1){
      HBOOK1(1526,"L=1,Ed_anode(mev),pcorr,1b/lay evnt",80,0.,24.,0.);
      HBOOK1(1527,"L=3,Ed_anode(mev),pcorr,1b/lay evnt",80,0.,24.,0.);
      HBOOK1(1528,"L=1,Ed_dynode(mev),pcorr,1b/lay evnt",80,0.,24.,0.);
      HBOOK1(1529,"L=3,Ed_dynode(mev),pcorr,1b/lay evnt",80,0.,24.,0.);
// spare     HBOOK1(1530,"L=1,Ed_dynodeL(mev),pcorr,1b/lay evnt",80,0.,24.,0.);
// spare     HBOOK1(1531,"L=3,Ed_dynodeL(mev),pcorr,1b/lay evnt",80,0.,24.,0.);
      for(il=0;il<TOF2GC::SCLRS;il++){
        for(ib=0;ib<TOF2GC::SCMXBR;ib++){
    	  strcpy(htit1,"dE/dX (norm.inc) for bar(LBB) ");
  	  in[0]=inum[il+1];
  	  strcat(htit1,in);
  	  ii=(ib+1)/10;
	  jj=(ib+1)%10;
	  in[0]=inum[ii];
	  strcat(htit1,in);
	  in[0]=inum[jj];
	  strcat(htit1,in);
	  ich=TOF2GC::SCMXBR*il+ib;
	  HBOOK1(1140+ich,htit1,50,0.,15.,0.);
        }
      }
    }
    
    HBOOK1(1137,"LTime-FTtime for LBBS=1041",70,-640.,60.,0.);
//
    HBOOK1(1532,"T1-T3(ns,NormIncidence,1b/L evnt)",80,1.,9.,0.);
//

    HBOOK1(1534,"T2-T4(ns,NormIncidence,1b/L evnt)",80,1.,9.,0.);
    HBOOK1(1544,"(T1-T3)-(T2-T4),(ns,1b/L evnt)",80,-4.,4.,0.);
    HBOOK1(1535,"L=1,TOFClus Edep(mev)",80,0.,24.,0.);
    HBOOK1(1536,"L=3,TOFClus Edep(mev)",80,0.,24.,0.);
    HBOOK1(1537,"L=1,TOFClus Edep(10Xscaled,mev)",80,0.,240.,0.);
    HBOOK1(1538,"L=3,TOFClus Edep(10Xscaled,mev)",80,0.,240.,0.);
    HBOOK1(1539,"L=2,TOFClus Edep(mev)",80,0.,24.,0.);
    HBOOK1(1540,"L=4,TOFClus Edep(mev)",80,0.,24.,0.);
    HBOOK1(1541,"TOFClus,L1XCoo(long)",100,-50.,50.,0.);
    HBOOK1(1542,"TOFClus,L1YCoo(tran)",100,-50.,50.,0.);
    HBOOK1(1543,"TOFClus,L2XCoo(tran)",100,-50.,50.,0.);
    HBOOK1(1547,"TOFClus,L2YCoo(long)",100,-50.,50.,0.);
    HBOOK1(1545,"L=1,TOFClus SQRT(Edep(mev))",100,0.,25.,0.);
    HBOOK1(1546,"L=3,TOFClus SQRT(Edep(mev))",100,0.,25.,0.);
    HBOOK1(1548,"TOFClus 2 bars E-ass(dt,dc ok)",44,-1.1,1.1,0.);
    HBOOK1(1549,"TOFClus cand. Y-match(cm, dt ok)",80,-40.,40.,0.);
    HBOOK1(1550,"TOFClus cand. T-match(ns)",80,-10.,10.,0.);
//
//    if(TFREFFKEY.relogic[0]==1){ // <==================== STRR-calibration
//      HBOOK1(1200,"Stretcher-ratio for indiv. channel,bin.meth",80,35.,55.,0.);
//      HBOOK1(1201,"Offsets for indiv. channels,bin.meth",80,-100.,2300.,0.);
//      HBOOK1(1202,"Chi2 for indiv. channel,bin.meth",50,0.,10.,0.);
//      HBOOK1(1204,"Bin Tin-RMS in Tin-Tout fit,bin.meth",50,0.,10.,0.);
//      HBOOK1(1205,"Stretcher-ratio for indiv. channel,pnt.meth",80,35.,55.,0.);
//      HBOOK1(1206,"Offsets for indiv. channels,pnt.meth",80,-100.,2300.,0.);
//      HBOOK1(1207,"Chi2 for indiv. channel,pnt.meth",50,0.,10.,0.);
//      HBOOK1(1208,"Stretcher-ratio for indiv. channel,comb.meth",80,35.,55.,0.);
//      HBOOK1(1209,"Offsets for indiv. channels,comb.meth",80,-100.,2300.,0.);
//      HBOOK1(1210,"Chi2 for indiv. channel,comb.meth",50,0.,10.,0.);
      
// hist.1600-1711 are booked in init-function for Tin vs Tout correl.!!!(TDLV)
// hist.1720-1781 are booked in init-function for BarRawTime histogr.!!!(TDLV)
//    }
    if(TFREFFKEY.relogic[0]==1){// <==================== TofPed-calibr. runs
//   hist. 1790-1999 are booked in init.function
    }
    if(TFREFFKEY.relogic[0]==2){// <==================== TDIF-calibr. runs
      HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
    }
    if(TFREFFKEY.relogic[0]==3){ // <==================== TZSL-calibration
      HBOOK1(1500,"Part.rigidity from tracker(gv)",100,0.,25.,0.);
      HBOOK1(1501,"TZSL:Beta(tracker,protmass)",100,0.95,1.,0.);
//      HBOOK1(1504,"Beta(tracker,prot)",80,0.2,1.,0.);// spare
      HBOOK1(1502,"TZSL:Beta(TOFprev.calib,no angle-corr)",80,0.4,1.2,0.);
      HBOOK1(1220,"Chisq (tof-beta-fit)",50,0.,10.,0.);
      HBOOK1(1221,"TZSL:Beta(TOFprev.calib,mom-cut,angle-corr)",80,0.4,1.2,0.);
      HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
      HBOOK1(1200,"Res_long.coo(track-sc),L=1",50,-10.,10.,0.);
      HBOOK1(1201,"Res_long.coo(track-sc),L=2",50,-10.,10.,0.);
      HBOOK1(1202,"Res_long.coo(track-sc),L=3",50,-10.,10.,0.);
      HBOOK1(1203,"Res_long.coo(track-sc),L=4",50,-10.,10.,0.);
      HBOOK2(1204,"Res_long. vs track coord.,L1",70,-70.,70.,60,-30.,30.,0.);
      HBOOK2(1205,"Res_long. vs track coord.,L2",70,-70.,70.,60,-30.,30.,0.);
      HBOOK2(1206,"Res_long. vs track coord.,L3",70,-70.,70.,60,-30.,30.,0.);
      HBOOK2(1207,"Res_long. vs track coord.,L4",70,-70.,70.,60,-30.,30.,0.);
      HBOOK1(1210,"Res_transv.coo(track-sc),L=1",50,-20.,20.,0.);
      HBOOK1(1211,"Res_transv.coo(track-sc),L=2",50,-20.,20.,0.);
      HBOOK1(1212,"Res_transv.coo(track-sc),L=3",50,-20.,20.,0.);
      HBOOK1(1213,"Res_transv.coo(track-sc),L=4",50,-20.,20.,0.);
      HBOOK1(1215,"(Cos_tr-Cos_sc)/Cos_tr",50,-1.,1.,0.);
      HBOOK1(1217,"Cos_sc",50,0.5,1.,0.);
      HBOOK1(1218,"TOF track-fit chi2-x",50,0.,5.,0.);
      HBOOK1(1219,"TOF track-fit chi2-y",50,0.,5.,0.);
      HBOOK1(1503,"Anticounter cluster-energy(mev)",80,0.,20.,0.);
      HBOOK1(1505,"Qmax ratio",80,0.,16.,0.);
      HBOOK1(1509,"Aver.Q(pC,truncated)",60,0.,600.,0.);
      HBOOK1(1507,"T0-difference inside bar-types 2",80,-0.4,0.4,0.);
      HBOOK1(1508,"T0-difference betw. ref. and bar-types 4",80,-0.4,0.4,0.);
      HBOOK2(1514,"Layer-1,T vs SUM(1/sqrt(Q))",50,0.,0.5,50,1.,101.,0.);
      HBOOK1(1524,"TRlen13-TRlen24",80,-4.,4.,0.);
    }
    if(TFREFFKEY.relogic[0]==4){ // <==================== AMPL-calibration
      HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
      HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
      HBOOK1(1501,"Particle beta(tracker)",80,0.5,1.,0.);
      HBOOK1(1502,"Particle beta(tof-fit)",80,0.7,1.2,0.);
      HBOOK1(1503,"ANTI-cluster energy(4Lx1bar events)(mev)",80,0.,40.,0.);
      HBOOK1(1200,"Res_long.coo(track-sc),L=1",50,-10.,10.,0.);
      HBOOK1(1201,"Res_long.coo(track-sc),L=2",50,-10.,10.,0.);
      HBOOK1(1202,"Res_long.coo(track-sc),L=3",50,-10.,10.,0.);
      HBOOK1(1203,"Res_long.coo(track-sc),L=4",50,-10.,10.,0.);
      HBOOK1(1204,"Mass**2",80,-1.,19.,0.);
      HBOOK1(1207,"Mass**2 for beta<0.92",80,-1.,19.,0.);
      HBOOK1(1208,"Ptr/impl.mass",80,0.,24.,0.);
      HBOOK1(1205,"Chisq (tof-beta-fit)",50,0.,10.,0.);
      HBOOK1(1206,"Tzer (tof-beta-fit)",50,-2.5,2.5,0.);
      HBOOK1(1210,"Res_transv.coo(track-sc),L=1",50,-20.,20.,0.);
      HBOOK1(1211,"Res_transv.coo(track-sc),L=2",50,-20.,20.,0.);
      HBOOK1(1212,"Res_transv.coo(track-sc),L=3",50,-20.,20.,0.);
      HBOOK1(1213,"Res_transv.coo(track-sc),L=4",50,-20.,20.,0.);
      HBOOK1(1215,"(Cos_tr-Cos_sc)/Cos_tr",50,-1.,1.,0.);
      HBOOK1(1217,"Cos_sc",50,0.5,1.,0.);
      HBOOK2(1218,"TOF-beta vs TRACKER-momentum",80,0.,4.,60,0.5,1.1,0.);
      HBOOK2(1219,"Q(ref.btyp=2) vs (beta*gamma)",80,0.,40.,60,0.,300.,0.);
// hist.# 1220-1239 are reserved for imp.point distr.(later in TOFAMPLcalib.init()
//
      HBOOK1(1240,"Instant Ah/Dl(pm-sum)(LBBS=1041,midd.bin)",50,5.,15.,0.);
      HBOOK1(1241,"Instant Dh(pm) rel.gain(LBBS=1041,PM=1,midd.bin)",50,0.5,1.5,0.);
      HBOOK1(1242,"Instant Dh(pm) rel.gain(LBBS=1041,PM=2,midd.bin)",50,0.5,1.5,0.);
      HBOOK1(1244,"Instant Ah/Al for LBBS=1041",60,2.,8.,0.);
      HBOOK1(1245,"Instant Dh(pm)/Dl(pm) for LBBS=1041,PM=1",50,0.,10.,0.);
      HBOOK1(1243,"Instant Dh(pm)/Dl(pm) for LBBS=1042,PM=1",50,0.,10.,0.);
      HBOOK1(1246,"Average Ah/Dh(pm-sum) (all chan, midd.bin)",50,7.5,12.5,0.);
      HBOOK1(1247,"RelatRMS of  aver. Ah/Dh(pm-sum) (all chan, midd.bin)",50,0.,1.,0.);
      HBOOK1(1248,"Average Dh(pm) rel.gains(all chan/pm,m.bin)",50,0.5,1.5,0.);
      HBOOK1(1249,"RelatRMS of aver. Dh(pm) rel.gains(all chan/pm,m.bin)",50,0.,1.,0.);
      HBOOK1(1276,"Average Ah/Al(all chan, midd.bin)",50,2.5,7.5,0.);
      HBOOK1(1277,"RelatRMS of aver. Ah/Al(all chan, midd.bin)",50,0.,5.,0.);
      HBOOK1(1278,"Average Dh/Dl(all chan/pm, midd.bin)",50,2.5,7.5,0.);
      HBOOK1(1279,"RelatRMS of aver. Dh/Dl(all chan/pm, midd.bin)",50,0.,2.5,0.);
// hist.1800-1911 are booked in init-function for D(h) vs A(h) correlation!!!
//
      HBOOK1(1250,"Ref.bar(type=2) Q-distr.(s=1,centre)",80,0.,160.,0.);        
      HBOOK1(1251,"Ref.bar(type=2) Q-distr.(s=2,centre)",80,0.,160.,0.);
      HBOOK1(1252,"Relative anode-gains(all channels)",80,0.5,2.,0.);
      HBOOK1(1254,"Ref.bar A-profile (s1,type-1)",70,-70.,70.,0.);        
      HBOOK1(1255,"Ref.bar A-profile (s2,type-1)",70,-70.,70.,0.);        
      HBOOK1(1256,"Ref.bar A-profile (s1,type-2)",70,-70.,70.,0.);        
      HBOOK1(1257,"Ref.bar A-profile (s2,type-2)",70,-70.,70.,0.);        
      HBOOK1(1258,"Ref.bar A-profile (s1,type-3)",70,-70.,70.,0.);        
      HBOOK1(1259,"Ref.bar A-profile (s2,type-3)",70,-70.,70.,0.);        
      HBOOK1(1260,"Ref.bar A-profile (s1,type-4)",70,-70.,70.,0.);        
      HBOOK1(1261,"Ref.bar A-profile (s2,type-4)",70,-70.,70.,0.);        
      HBOOK1(1262,"Ref.bar A-profile (s1,type-5)",70,-70.,70.,0.);        
      HBOOK1(1263,"Ref.bar A-profile (s2,type-5)",70,-70.,70.,0.);        
      HBOOK1(1264,"Ref.bar A-profile (s1,type-6)",70,-70.,70.,0.);        
      HBOOK1(1265,"Ref.bar A-profile (s2,type-6)",70,-70.,70.,0.);        
      HBOOK1(1266,"Ref.bar A-profile (s1,type-7)",70,-70.,70.,0.);        
      HBOOK1(1267,"Ref.bar A-profile (s2,type-7)",70,-70.,70.,0.);        
      HBOOK1(1268,"Ref.bar A-profile (s1,type-8)",70,-70.,70.,0.);        
      HBOOK1(1269,"Ref.bar A-profile (s2,type-8)",70,-70.,70.,0.);        
      HBOOK1(1270,"Ref.bar A-profile (s1,type-9)",70,-70.,70.,0.);        
      HBOOK1(1271,"Ref.bar A-profile (s2,type-9)",70,-70.,70.,0.);        
      HBOOK1(1272,"Ref.bar A-profile (s1,type-10)",70,-70.,70.,0.);        
      HBOOK1(1273,"Ref.bar A-profile (s2,type-10)",70,-70.,70.,0.);        
      HBOOK1(1274,"Ref.bar A-profile (s1,type-11)",70,-70.,70.,0.);        
      HBOOK1(1275,"Ref.bar A-profile (s2,type-11)",70,-70.,70.,0.);        
    }
    if(TFREFFKEY.reprtf[3]!=0){//Validation-stage multiplicity histograms
      for(il=0;il<TOF2GC::SCLRS;il++){
	for(ib=0;ib<TOF2GC::SCMXBR;ib++){
	  for(i=0;i<2;i++){
	    strcpy(htit1,"Validation::FTtdc/LTtdc/Aadc/Dadc/SumHT/SumSHT multipl. for chan(LBBS) ");
	    in[0]=inum[il+1];
	    strcat(htit1,in);
	    ii=(ib+1)/10;
	    jj=(ib+1)%10;
	    in[0]=inum[ii];
	    strcat(htit1,in);
	    in[0]=inum[jj];
	    strcat(htit1,in);
	    in[0]=inum[i+1];
	    strcat(htit1,in);
	    ich=2*TOF2GC::SCMXBR*il+2*ib+i;
	    HBOOK1(1300+ich,htit1,100,0.,100.,0.);
	  }
	}
      }
    }
  }
}
//-----------------------------
void TOF2JobStat::bookhistmc(){
    if(TFMCFFKEY.mcprtf[2]!=0){ // Book mc-hist
      HBOOK1(1050,"SIMU: GHits in layer-1",50,0.,50.,0.);
      HBOOK1(1051,"SIMU: GHits in layer-2",50,0.,50.,0.);
      HBOOK1(1052,"SIMU: GHits in layer-3",50,0.,50.,0.);
      HBOOK1(1053,"SIMU: GHits in layer-4",50,0.,50.,0.);
      HBOOK1(1060,"SIMU: GHit Edeps(mev) in layer-1",80,0.,24.,0.);
      HBOOK1(1061,"SIMU: GHit Edeps(mev) in layer-2",80,0.,24.,0.);
      HBOOK1(1062,"SIMU: GHit Edeps(mev) in layer-3",80,0.,24.,0.);
      HBOOK1(1063,"SIMU: GHit Edeps(mev) in layer-4",80,0.,24.,0.);
      HBOOK1(1070,"SIMU: PM-1 charge(pC,id=104,s1)",80,0.,400.,0.);
      HBOOK1(1071,"SIMU: Number of HT(SHT+10)-trigger pulses(SPT-inp)",20,0.,20.,0.);
      HBOOK1(1072,"SIMU: HT-trig pulse width(SPT-inp, ns)",80,200.,600.,0.);
      HBOOK1(1073,"SIMU: SHT-trig pulse width(SPT-inp, ns)",80,200.,600.,0.);
      HBOOK1(1074,"SIMU: Anode-adc(id=104,s1,NoPeds)",100,0.,500.,0.);
      HBOOK1(1075,"SIMU: Dynode-adc(eq.sum/npm, id=104,s1,NoPeds)",100,0.,100.,0.);
      HBOOK1(1065,"SIMU: FTCTrigPat(z>=1):S1-frequence(L=1,4)",80,0.,80.,0.);
      HBOOK1(1066,"SIMU: FTCTrigPat(z>=1):S2-frequence(L=1,4)",80,0.,80.,0.);
      HBOOK1(1067,"SIMU: BZTrigPat(z>=2):S1-frequence(L=1,4)",80,0.,80.,0.);
      HBOOK1(1068,"SIMU: BZTrigPat(z>=2):S2-frequence(L=1,4)",80,0.,80.,0.);
      HBOOK1(1069,"SIMU: TofFtCodes(/0-14/+20/+40->FTC/BZ-inp/BZ-accepted)",60,0.,60.,0.);
      HBOOK1(1076,"SIMU: ECTrigFlag when TOFTrflag OK",40,0.,40.,0.);
      HBOOK1(1077,"SIMU: TOFFTTime-ECFTTime(when FTC&FTE)",80,-80.,80.,0.);
      HBOOK1(1078,"SIMU: Out-of-width-hit X-excess",50,0.,5.,0.);
      HBOOK1(1079,"SIMU: Out-of-thickness-hit Z-excess",50,0.,5.,0.);
      HBOOK1(1080,"SIMU: GHitTime",100,0.,1000.,0.);
      HBOOK1(1081,"SIMU: Anode pulse-hight(mV,id=101(bt1),s1)",80,0.,1600.,0.);
      HBOOK1(1082,"SIMU: Anode pulse-hight(mV,id=104(bt2),s1)",80,0.,1600.,0.);
      HBOOK1(1083,"SIMU: Anode pulse-hight(mV,id=401(bt3),s1)",80,0.,1600.,0.);
      HBOOK1(1084,"SIMU: Anode pulse-hight(mV,id=404(bt4),s1)",80,0.,1600.,0.);
      HBOOK1(1085,"SIMU: Anode pulse-hight(mV,id=201(bt5),s1)",80,0.,1600.,0.);
      HBOOK1(1086,"SIMU: Anode pulse-hight(mV,id=202(bt6),s1)",80,0.,1600.,0.);
      HBOOK1(1087,"SIMU: Anode pulse-hight(mV,id=204(bt7),s1)",80,0.,1600.,0.);
      HBOOK1(1088,"SIMU: Anode pulse-hight(mV,id=301(bt8),s1)",80,0.,1600.,0.);
      HBOOK1(1089,"SIMU: Anode pulse-hight(mV,id=302(bt9),s1)",80,0.,1600.,0.);
      HBOOK1(1090,"SIMU: Anode pulse-hight(mV,id=303(bt10),s1)",80,0.,1600.,0.);
      HBOOK1(1091,"SIMU: Anode pulse-hight(mV,id=305(bt11),s1)",80,0.,1600.,0.);
      HBOOK1(1031,"SIMU: Nphelectrons/side(id=101(bt1),s1)",80,0.,1600.,0.);
      HBOOK1(1032,"SIMU: Nphelectrons/side(id=104(bt2),s1)",80,0.,3200.,0.);
      HBOOK1(1033,"SIMU: Nphelectrons/side(id=401(bt3),s1)",80,0.,1600.,0.);
      HBOOK1(1034,"SIMU: Nphelectrons/side(id=404(bt4),s1)",80,0.,1600.,0.);
      HBOOK1(1035,"SIMU: Nphelectrons/side(id=201(bt5),s1)",80,0.,1200.,0.);
      HBOOK1(1036,"SIMU: Nphelectrons/side(id=202(bt6),s1)",80,0.,1200.,0.);
      HBOOK1(1037,"SIMU: Nphelectrons/side(id=204(bt7),s1)",80,0.,1200.,0.);
      HBOOK1(1038,"SIMU: Nphelectrons/side(id=301(bt8),s1)",80,0.,1200.,0.);
      HBOOK1(1039,"SIMU: Nphelectrons/side(id=302(bt9),s1)",80,0.,1200.,0.);
      HBOOK1(1040,"SIMU: Nphelectrons/side(id=303(bt10),s1)",80,0.,1200.,0.);
      HBOOK1(1041,"SIMU: Nphelectrons/side(id=305(bt11),s1)",80,0.,1200.,0.);
      HBOOK1(1042,"SIMU: Npe1-Npe2/Npe1+Npe2(id=104(bt2))",80,-0.6,0.6,0.);
      HBOOK1(1043,"SIMU: Nphelectrons/counter(id=104(bt2))",80,0.,6400.,0.);
    }
}
//----------------------------
void TOF2JobStat::outp(){
  int i,j,k,ich;
  geant dedx[TOF2GC::SCMXBR],dedxe[TOF2GC::SCMXBR];
       if(TFREFFKEY.reprtf[2]!=0){ // print RECO-hists
         HPRINT(1535);
         HPRINT(1536);
         HPRINT(1537);
         HPRINT(1538);
	 HPRINT(1545);
	 HPRINT(1546);
         HPRINT(1541);
         HPRINT(1542);
         HPRINT(1543);
         HPRINT(1547);
         HPRINT(1539);
         HPRINT(1540);
         HPRINT(1548);
         HPRINT(1549);
         HPRINT(1550);
         HPRINT(1100);
         HPRINT(1115);
         HPRINT(1101);
         HPRINT(1102);
//         HPRINT(1103);
         HPRINT(1108);
         HPRINT(1104);
         HPRINT(1105);
         HPRINT(1110);
         HPRINT(1111);
         HPRINT(1112);
         HPRINT(1113);
         HPRINT(1114);
         HPRINT(1137);
	 
	 
//         HPRINT(1095);
//         HPRINT(1096);
//         HPRINT(1097);
//         HPRINT(1098);
         HPRINT(1092);
         if(TFREFFKEY.reprtf[2]>1){
           HPRINT(1526);
           HPRINT(1527);
           HPRINT(1528);
           HPRINT(1529);
// spare          HPRINT(1530);
//           HPRINT(1531);
           for(i=0;i<TOF2GC::SCLRS;i++){
             for(j=0;j<TOF2GC::SCMXBR;j++){
               ich=TOF2GC::SCMXBR*i+j;
               HPRINT(1140+ich);
               dedx[j]=0.;
               dedxe[j]=0.;
               dedx[j]=HSTATI(1140+ich,1," ",0);
               dedxe[j]=HSTATI(1140+ich,2," ",0);
             }
             HPAK(1116+i,dedx);
             HPAKE(1116+i,dedxe);
             HPRINT(1116+i);
           }
         }
           HPRINT(1532);
           HPRINT(1544);
           HPRINT(1534);
         if(TFREFFKEY.reprtf[3]!=0){//TDC-hit multiplicity histograms
           for(i=0;i<TOF2GC::SCLRS;i++){
             for(j=0;j<TOF2GC::SCMXBR;j++){
               for(k=0;k<2;k++){
                 ich=2*TOF2GC::SCMXBR*i+2*j+k;
                 HPRINT(1300+ich);
               }
             }
           }
         }
       }
// ---> calibration specific :
       if(TFREFFKEY.relogic[0]==3){// for TZSL-calibr. runs
         HPRINT(1500);
         HPRINT(1501);
//         HPRINT(1504);
         HPRINT(1502);
         HPRINT(1220);
         HPRINT(1221);
         HPRINT(1506);
           HPRINT(1514);
           HPRINT(1503);
           HPRINT(1505);
           HPRINT(1509);
           HPRINT(1200);
           HPRINT(1201);
           HPRINT(1202);
           HPRINT(1203);
           HPRINT(1204);
           HPRINT(1205);
           HPRINT(1206);
           HPRINT(1207);
           HPRINT(1210);
           HPRINT(1211);
           HPRINT(1212);
           HPRINT(1213);
           HPRINT(1215);
           HPRINT(1217);
           HPRINT(1218);
           HPRINT(1219);
           HPRINT(1524);
//           HPRINT(1550);
//           HPRINT(1551);
//           HPRINT(1552);
//           HPRINT(1553);
           TOF2TZSLcalib::mfit();
           HPRINT(1507);
           HPRINT(1508);
       }
//
       if(TFREFFKEY.relogic[0]==4){// for AMPL-calibr. runs
           HPRINT(1506);
           HPRINT(1500);
           HPRINT(1501);
           HPRINT(1502);
           HPRINT(1205);
           HPRINT(1206);
           HPRINT(1503);
           HPRINT(1200);
           HPRINT(1201);
           HPRINT(1202);
           HPRINT(1203);
           HPRINT(1210);
           HPRINT(1211);
           HPRINT(1212);
           HPRINT(1213);
           HPRINT(1215);
           HPRINT(1217);
           HPRINT(1218);
           HPRINT(1219);
           HPRINT(1204);
           HPRINT(1207);
           HPRINT(1208);
           HPRINT(1250);
           HPRINT(1251);
            TOF2AMPLcalib::fit();//call h#1240-1249
	    HPRINT(1240);
	    HPRINT(1241);
	    HPRINT(1242);
	    HPRINT(1244);
	    HPRINT(1245);
	    HPRINT(1243);
	    HPRINT(1246);
	    HPRINT(1247);
	    HPRINT(1248);
	    HPRINT(1249);
	    HPRINT(1276);
	    HPRINT(1277);
	    HPRINT(1278);
	    HPRINT(1279);
           HPRINT(1252);
           HPRINT(1254);
           HPRINT(1255);
           HPRINT(1256);
           HPRINT(1257);
           HPRINT(1258);
           HPRINT(1259);
           HPRINT(1260);
           HPRINT(1261);
           HPRINT(1262);
           HPRINT(1263);
           HPRINT(1264);
           HPRINT(1265);
           HPRINT(1266);
           HPRINT(1267);
           HPRINT(1268);
           HPRINT(1269);
           HPRINT(1270);
           HPRINT(1271);
           HPRINT(1272);
           HPRINT(1273);
           HPRINT(1274);
           HPRINT(1275);
       }
//
//
       if(TFREFFKEY.relogic[0]==1){// for STRR-calibr. runs
           TOF2STRRcalib::outp();
           HPRINT(1200);
           HPRINT(1201);
           HPRINT(1202);
           HPRINT(1204);
           HPRINT(1205);
           HPRINT(1206);
           HPRINT(1207);
           HPRINT(1208);
           HPRINT(1209);
           HPRINT(1210);
       }
//
       if(TFREFFKEY.relogic[0]==2){// for TDIF-calibr. runs
           HPRINT(1500);
           TOF2TDIFcalib::fit();
       }
//
}
//----------------------------
void TOF2JobStat::outpmc(){
       if(TFMCFFKEY.mcprtf[2]!=0){ // print MC-hists
         HPRINT(1050);
         HPRINT(1051);
         HPRINT(1052);
         HPRINT(1053);
         HPRINT(1060);
         HPRINT(1061);
         HPRINT(1062);
         HPRINT(1063);
         HPRINT(1070);
         HPRINT(1071);
         HPRINT(1072);
         HPRINT(1073);
	 HPRINT(1031);
	 HPRINT(1032);
	 HPRINT(1043);
	 HPRINT(1033);
	 HPRINT(1034);
	 HPRINT(1035);
	 HPRINT(1036);
	 HPRINT(1037);
	 HPRINT(1038);
	 HPRINT(1039);
	 HPRINT(1040);
	 HPRINT(1041);
	 HPRINT(1042);
         HPRINT(1081);
         HPRINT(1082);
         HPRINT(1083);
         HPRINT(1084);
         HPRINT(1085);
         HPRINT(1086);
         HPRINT(1087);
         HPRINT(1088);
         HPRINT(1089);
         HPRINT(1090);
         HPRINT(1091);
//         HPRINT(1054);
         HPRINT(1075);
//         HPRINT(1064);
         HPRINT(1065);
         HPRINT(1066);
         HPRINT(1067);
         HPRINT(1068);
         HPRINT(1069);
         HPRINT(1076);
         HPRINT(1077);
         HPRINT(1078);
         HPRINT(1079);
         HPRINT(1080);
//         if(TFCAFFKEY.mcainc)TOF2Tovt::aintfit();
       }
}
//==========================================================================


geant TOF2Varp::getmeantoftemp(int crate){
geant tsum=0;
switch (crate){
case 1:
return float(tftt.tofav[0])/10.;
case 31:
return float(tftt.tofav[1])/10.;
case 41:
return float(tftt.tofav[2])/10.;
case 71:
return float(tftt.tofav[3])/10.;
case 3:
return float(tftt.tofav[4])/10.;
case 33:
return float(tftt.tofav[5])/10.;
case 43:
return float(tftt.tofav[6])/10.;
case 73:
return float(tftt.tofav[7])/10.;
case 0:
int i;
for(i=0;i<8;i++){
 tsum+=tftt.tofav[i]/10.;
}
return tsum;
default:
cerr <<"TOF2Varp::getmeantoftemp-E-NoCrateFound "<<crate<<endl;
return 0.;
}

}

void TOF2Varp::init(geant daqth[5], geant cuts[10]){

    int i;
    for(i=0;i<5;i++)_daqthr[i]=daqth[i];
    for(i=0;i<10;i++)_cuts[i]=cuts[i];
}


  void TOF2JobStat::clear(){
    int i,j;
    for(i=0;i<TOF2GC::SCJSTA;i++)mccount[i]=0;
    for(i=0;i<TOF2GC::SCJSTA;i++)recount[i]=0;
    for(i=0;i<TOF2GC::SCCHMX;i++)
                  for(j=0;j<TOF2GC::SCCSTA;j++)
                                       chcount[i][j]=0;
    for(i=0;i<TOF2GC::SCBLMX;i++)
                  for(j=0;j<TOF2GC::SCCSTA;j++)
                                       brcount[i][j]=0;
    for(int ie=0;ie<30;ie++)daqsf[ie]=0;
    for(int ie=0;ie<20;ie++){
      for(i=0;i<TOF2GC::SCCRAT;i++){
        cratr[i][ie]=0;
        cratp[i][ie]=0;
        cratc[i][ie]=0;
        cratm[i][ie]=0;
        for(j=0;j<TOF2GC::SCSLTM;j++){
	  sltr[i][j][ie]=0;
	  sltp[i][j][ie]=0;
          for(int ic=0;ic<TOF2GC::SCRCHM;ic++){
	    if(i==0){
	      rdcr1[j][ic][ie]=0;
	      rdcp1[j][ic][ie]=0;
	    }
	    if(i==1){
	      rdcr2[j][ic][ie]=0;
	      rdcp2[j][ic][ie]=0;
	    }
	    if(i==2){
	      rdcr3[j][ic][ie]=0;
	      rdcp3[j][ic][ie]=0;
	    }
	    if(i==3){
	      rdcr4[j][ic][ie]=0;
	      rdcp4[j][ic][ie]=0;
	    }
	  }
	}
      }
    }
    for(i=0;i<TOF2GC::SCCRAT;i++)
                for(j=0;j<TOF2GC::SCFETA;j++)tofantemp[i][j]=20;//tempor default temperature(degrees)
  }
//--------------------------------------------------
TofElosPDF::TofElosPDF(int ich, int ch, geant bp, int nb, geant stp, geant bnl, geant undf, geant ovfl, geant distr[]){
  ichar=ich;
  charge=ch;
  bpow=bp;
  nbins=nb;
  stpx=stp;
  xmin=bnl;
//cout<<"iZ/Z="<<ichar<<" "<<charge<<"  beta_pow="<<bpow<<endl;
//cout<<"Nbns/binw/xmin="<<nbins<<" "<<stpx<<" "<<xmin<<endl;
//cout<<"Undfl/Ovfl="<<undf<<" "<<ovfl<<" dis(0)/dis(n-1)="<<distr[0]<<" "<<distr[nb-1]<<endl;
  number norm(0),suml,sumr,hend;
  for(int i=0;i<nb;i++){
    elpdf[i]=number(distr[i]);
    norm+=elpdf[i];
  }
  suml=number(undf);
  if(suml==0)suml+=1;
  sumr=number(ovfl);
  norm+=(suml+sumr);
//  cout<<"suml/sumr="<<suml<<" "<<sumr<<endl;
  if(sumr==0 || elpdf[nb-1]==0){
    cout<<"TofElosPDF::-E-:Original distribition high-end is badly defined !"<<" iZ="<<ich<<endl;
    exit(1);
  }
  hend=elpdf[nbins-1];
  slope=hend/sumr/stpx;//exp.slope in ovfl-region
  unpdf=suml/xmin/norm;//const.PDF-value in underfl-region(already normalized correctly !!!)
  for(int i=0;i<nb;i++)elpdf[i]/=(norm*stpx);//normalized PDF in central region
}
//---
void TofElosPDF::build(){// create TofElosPDF-objects array for real/mc data
//
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
  int mrfp;
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
  integer ic,charge,chref,nbns,endflab;
  geant betap,lovfl,rovfl,xmin,binw,distr[TOF2GC::SCPDFBM];
//
  strcpy(inum,"0123456789");
//
// ---> read file with the list of version numbers for all needed calib.files :
//
  integer cfvn;
  cfvn=TFCAFFKEY.cfvers%1000;//vers.list-file version number 
  strcpy(name,"tof2cvlist");// basic name for vers.list-file  
  dig=cfvn/100;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
//
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> TofElosPDF::build: Opening vers.list-file "<<fname<<'\n';
  ifstream vlfile(fname,ios::in);
  if(!vlfile){
    cerr <<"<---- TofElosPDF::build:Error: missing vers.list-file !!? "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(int i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
    vlfile >> rlvern[i];// second number - for real
  }
  vlfile.close();
//------------------------------------------------
 ctyp=7;
 strcpy(name,"tof2pdff");//part.charge calib.file
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData())           // for MC-data
 {
   cout <<"      dE/dX<->Charge distr. for MC-data are requested..."<<endl;
   dig=mcvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(mcvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real-data
 {
   cout <<"      dE/dX<->Charge distr. for Real-data are requested..."<<endl;
   dig=rlvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(rlvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
   strcat(name,".dat");
   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"      Open file : "<<fname<<'\n';
   ifstream pdfile(fname,ios::in); // open file for reading
   if(!pdfile){
     cerr <<"<----- TofElosPDF::build:Error: Missing dE/dX<->Charge distr. file !!? "<<fname<<endl;
     exit(1);
   }
//
  for(int ich=0;ich<MaxZTypes;ich++){
    pdfile >> ic;
    pdfile >> charge;
    chref=AMSCharge::ind2charge(0,ich+1);//charge from ctandard list("0"->TOF)
    if(charge!=chref || ic!=(ich+1)){
      cout<<"<---- TofElosPDF::build:Error: wrong Z-structure of dEdX<->Charge distr. file,id="<<ic<<endl;
      exit(1);
    }
    pdfile >> betap;
    pdfile >> nbns;
    pdfile >> lovfl;
    pdfile >> rovfl;
    pdfile >> xmin;
    pdfile >> binw;
    for(int ib=0;ib<nbns;ib++)pdfile >> distr[ib];
    TofEPDFs[ich]=TofElosPDF(ic, charge, betap, nbns, binw, xmin, lovfl, rovfl, distr);
  }
//
  pdfile >> endflab;//read endfile-label
//
  pdfile.close();
//
  if(endflab==12345){
    cout<<"<----- TofElosPDF::build: dE/dXCharge distr. file is successfully read !"<<endl<<endl;
  }
  else{cout<<"<----- TofElosPDF::build:Error: problem with dEdX<-<Charge distr. file !!?"<<endl<<endl;
    exit(1);
  }
//
}
//---
number TofElosPDF::getlkhd(int nhits, int hstat[], number ehit[], number beta){
  number eh,xmax,hend,betapow,betacor,betamax(0.94),lkhd(0);
  int ia;
  betapow=bpow;//now depends on Z-index
  betacor=ichar?pow(min(fabs(beta/betamax),1.),betapow):1;//corr to "mip"(=1 for ichar=0(electrons))
  xmax=xmin+nbins*stpx;
  hend=elpdf[nbins-1];
  for(int ih=0;ih<nhits;ih++){//hits loop
    if(hstat[ih]>=0){
      eh=ehit[ih]*betacor;
      if(eh<xmin)lkhd+=-log(unpdf);//undfl-region
      else if(eh>=xmax)lkhd+=(-log(hend)+slope*(eh-xmax));//ovfl-region
      else{//central region
        ia=int(floor((eh-xmin)/stpx));
        lkhd+=-log(elpdf[ia]);
      }
    }
  }
  return lkhd;
}

//-----------------------------------------------------------------------


