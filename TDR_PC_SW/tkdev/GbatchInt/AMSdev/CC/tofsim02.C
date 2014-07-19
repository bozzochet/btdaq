//  $Id: tofsim02.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author Choumilov.E. 10.07.96.
// Modified to work with width-divisions by Choumilov.E. 19.06.2002
// Removed gain-5 logic, E.Choumilov 22.08.2005
//  !!!!! 15.03.06   E.Choumilov - complete rebuild caused by new readout(new TDC)
#include "tofdbc02.h"
#include <iostream.h>
#include <stdio.h>
#include "typedefs.h"
#include "cern.h"
#include "extC.h"
#include <math.h>
#include "point.h"
#include "amsgobj.h"
#include "event.h"
#include "amsstl.h"
#include "commons.h"
#include "tofsim02.h"
#include "mccluster.h"
#include "trigger102.h"
#include "trigger302.h"
#include "mceventg.h"
#include "ecalrec.h"
#include "tofid.h"
//
using namespace std;
//
TOFWScan TOFWScan::scmcscan[TOF2GC::SCBTPN];
//
integer TOF2RawSide::_trpatt[TOF2GC::SCLRS]={0,0,0,0};
integer TOF2RawSide::_trpattz[TOF2GC::SCLRS]={0,0,0,0};
integer TOF2RawSide::_trcode=0;
integer TOF2RawSide::_trcodez=0;
integer TOF2RawSide::_ftpatt=0;
integer TOF2RawSide::_cpcode=0;
integer TOF2RawSide::_bzflag=0;
number TOF2RawSide::_trtime=0.;
number TOF2Tovt::itovts[TOF2GC::SCMCIEVM];
number TOF2Tovt::icharg[TOF2GC::SCMCIEVM];
integer TOF2Tovt::ievent;
geant TOF2Tovt::SumHTt[TOF2GC::SCCRAT][TOF2GC::SCFETA-1][TOF2GC::SCTHMX2];//TDC SumHT(SHT)-channel mem.reserv.
int TOF2Tovt::SumHTh[TOF2GC::SCCRAT][TOF2GC::SCFETA-1]; 
geant TOF2Tovt::SumSHTt[TOF2GC::SCCRAT][TOF2GC::SCFETA-1][TOF2GC::SCTHMX2];
int TOF2Tovt::SumSHTh[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];
geant AMSBitstr::clkfas=0;
//============================================================
AMSDistr::AMSDistr(int _nbin, geant _bnl, geant _bnw, geant arr[]){// constructor
  nbin=_nbin;
  bnl=_bnl;
  bnw=_bnw;
  geant tot(0.);
  for(int i=0;i<nbin;i++)tot+=arr[i];// calc. integral
  distr[0]=arr[0]/tot;             // prepare cumul. spectrum
  for(int j=1;j<nbin;j++)distr[j]=distr[j-1]+arr[j]/tot;
}
//---------------------------------------------------
geant AMSDistr::getrand(const geant &rnd){
  geant val;
  integer i=AMSbins(distr,rnd,nbin);
  if(i<0){
    i=-i;
    val=bnl+bnw*i+bnw*(rnd-distr[i-1])/(distr[i]-distr[i-1]);
  }
  else if(i==0){val=bnl+bnw*rnd/distr[i];}
  else {val=bnl+bnw*i;}
  return val;
}
//===========================================================
TOFTpoints::TOFTpoints(int nbin, geant bnl, geant bnw, geant arr[]){//constructor
  int i,it,nb,nbf,fst,nbl;
  geant cint,low,hig,tot(0.);
  geant bl,bh,cdis[TOFGC::AMSDISL];
//
  fst=0;
  for(i=0;i<nbin;i++){
    if(fst==0 && arr[i]>0.){
      fst=1;
      nbf=i;// 1st nonzero bin
    }
    if(arr[i]>0.)nbl=i;// last nonzero bin
    tot+=arr[i];// calc. integral
  }
  cdis[0]=arr[0]/tot;             // prepare cumul. spectrum
  for(i=1;i<nbin;i++)cdis[i]=cdis[i-1]+arr[i]/tot;
  bini=1./geant(TOF2GC::TOFPNMX);
//
  times[0]=bnl+bnw*nbf;
  times[TOF2GC::TOFPNMX]=bnl+bnw*(nbl+1);
  nb=nbf;
  for(it=1;it<TOF2GC::TOFPNMX;it++){// <--- loop over integral bins
    cint=it*bini;
    while(nb<=nbl){// <--- loop over cumul.distr. bins
      bl=bnl+nb*bnw;
      bh=bl+bnw;
      if(nb==nbf)low=0.;
      else low=cdis[nb-1];
      hig=cdis[nb];
      if(cint>=low && cint<hig){
        times[it]=bl+bnw*(cint-low)/(hig-low);
	break;
      }
      else nb+=1;
    }
  }
}
//===========================================================
//  <--- for inp. Y define position in scan array (i1/i2) and correction ratio R.
//
void TOF2Scan::getybin(const geant y, integer &i1, integer &i2, geant &r)
{
  integer i=AMSbins(scanp,y,nscpnts);
  if(i==0){
    i1=0;
    i2=1;
    r=(y-scanp[i1])/(scanp[i2]-scanp[i1]);//r<0 -> y below 1st sc.point
  }
  else if(i<0){
    if(i==-nscpnts){// r>1 -> y above last sc.point
      i1=nscpnts-2;
      i2=nscpnts-1;
      r=(y-scanp[i1])/(scanp[i2]-scanp[i1]);
      }
    else{// <- normal case when y is inside of sc.points array
      i1=-i-1;
      i2=-i;
      r=(y-scanp[i1])/(scanp[i2]-scanp[i1]);
    }
  }
  else{ // exact matching inside sc.points array(i1=i2, r=0.)
    i1=i-1;
    i2=i1;
    r=0.;
  }
}
//------------------------------------------------------------------
geant TOF2Scan::gettm1(const geant r, const int i1, const int i2){
  geant rnd,t1,t2,dummy(-1);
//
  rnd=RNDM(dummy);
  t1=tdist1[i1].gettime(rnd);
  t2=tdist1[i2].gettime(rnd);
  return t1+(t2-t1)*r>0?t1+(t2-t1)*r:0;

};    
geant TOF2Scan::gettm2(const geant r, const int i1, const int i2){
  geant rnd,t1,t2,dummy(-1);
//
  rnd=RNDM(dummy);
  t1=tdist2[i1].gettime(rnd);
  t2=tdist2[i2].gettime(rnd);
  return t1+(t2-t1)*r>0?t1+(t2-t1)*r:0;

};    
//------------------------------------------------------------------
// function to create TOFWScan objects for all sc. bars :
void TOFWScan::build(){
//
  int i,ic,nverst,ivers,dig,nctot(0);
  int brfnam[TOF2GC::SCBTPN];
  char fname[80];
  char name[80];
  char in[2]="0";
  char inum[11];
  char tdisfn[20]="tof2smap";
//
  strcpy(inum,"0123456789");
//
// ---> read version number of "tof2smap"-file from verslist-file:
//
  integer cfvn;
  cfvn=TFCAFFKEY.cfvers%1000;
  strcpy(name,"tof2cvlist");// basic name of file for cal-files list
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
  cout<<"====> TOFWScan::build: Open verslist-file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"<---- missing verslist-file !!! "<<fname<<endl;
    exit(1);
  }
  vlfile >> nverst;// total number of files in the list
  vlfile >> ivers;// first number in first line (vers.# for tdisfmap-file)
  vlfile.close();
  ivers=(ivers%1000);
  cout<<"      use MC-TimeScanMap file version="<<ivers<<'\n';
//  
//                    <-- first read TScanFileNames vs BType (map) file:
  strcpy(name,tdisfn);
  dig=ivers/100;
  in[0]=inum[dig];
  strcat(name,in);
  dig=(ivers%100)/10;
  in[0]=inum[dig];
  strcat(name,in);
  dig=ivers%10;
  in[0]=inum[dig];
  strcat(name,in);
  strcat(name,".dat");
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"       Open file : "<<fname<<'\n';
  ifstream tcfile(fname,ios::in); // open needed tdisfmap-file for reading
  if(!tcfile){
    cerr <<"<---- missing  MC-TimeScanMap file !!! "<<fname<<endl;
    exit(1);
  }
  for(ic=0;ic<TOF2GC::SCBTPN;ic++) tcfile >> brfnam[ic];
  tcfile.close();
  cout<<"   <-- MC-TimeScanMap file succsessfully read !"<<endl;
//-------------------
//                                  <-- now read t-distr. files
 int j,ila,ibr,brt,ibrm,isp,nsp,ibt,cnum,dnum,dnumm,mult,dnumo;
 integer nb;
 int iscp[TOF2GC::SCANPNT];
 geant scp[TOF2GC::SCANPNT];
 geant nft,bl,bw;
 geant arr[TOFGC::AMSDISL];
 geant ef1[TOF2GC::SCANPNT],ef2[TOF2GC::SCANPNT];
 geant rg1[TOF2GC::SCANPNT],rg2[TOF2GC::SCANPNT];
 geant ps1[TOF2GC::SCANPNT][TOF2GC::PMTSMX],ps2[TOF2GC::SCANPNT][TOF2GC::PMTSMX];
 TOFTpoints td1[TOF2GC::SCANPNT];
 TOFTpoints td2[TOF2GC::SCANPNT];
 int nwdivs(0);
 int npmts(0);
 TOF2Scan swdscan[TOF2GC::SCANWDS];
 geant wdivxl[TOF2GC::SCANWDS];
 geant wdivxh[TOF2GC::SCANWDS];
 integer idiv;
 geant eff1,eff2;
 cout<<"      Reading MC-TimeScan files ...."<<endl;
//
  for(brt=0;brt<TOF2GC::SCBTPN;brt++){   // <-------- loop bar-types
      dnum=brfnam[brt];// 4-digits t-distr. file name (VLBB)
      mult=1000;
      strcpy(name,"tof2c");
      dnumm=dnum;
      for(i=3;i>=0;i--){//create 4-letters file name
        j=dnumm/mult;
        in[0]=inum[j];
        strcat(name,in);
        dnumm=dnumm%mult;
        mult=mult/10;
      }
      strcat(name,".tsf");
      if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
      if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
      strcat(fname,name);
      cout<<"      Open file : "<<fname<<'\n';
      ifstream tcfile(fname,ios::in); // open needed t-calib. file for reading
      if(!tcfile){
        cerr <<"<---- missing MC-TimeScan file !!! "<<fname<<endl;
        exit(1);
      }
      tcfile >> npmts; // read numb.of PMTs per side
//
      tcfile >> nwdivs; // read tot.number of X(width) divisions
//
      for(int div=0;div<nwdivs;div++){// <--- divisions loop
//
        tcfile >> idiv; // read numb.of current div.
//
        tcfile >> nsp; // read tot.number of scan-point in this div.
        if(nsp>TOF2GC::SCANPNT){
          cerr<<"<---- wrong number of MC-TimeScan Y-points ! "<<nsp<<'\n';
          exit(1);
        } 
        tcfile >> wdivxl[div]; // read xl
        tcfile >> wdivxh[div]; // read xh
//
// <-- fill errays scp,ef1,ef2,rg1,rg2 for given division
//
        for(isp=0;isp<nsp;isp++){ // <--- sp. points loop to prepare arr. of t-distr
          tcfile >> iscp[isp];
          tcfile >> scp[isp];
          tcfile >> nft;   // for PM-1
          tcfile >> nb;
          tcfile >> bl;
          tcfile >> bw;
          tcfile >> ef1[isp];
          tcfile >> rg1[isp];
	  for(i=0;i<TOF2GC::PMTSMX;i++)tcfile >> ps1[isp][i];
          for(i=0;i<nb;i++){arr[i]=0.;}
          for(i=0;i<nb;i++){tcfile >> arr[i];}
          td1[isp]=TOFTpoints(nb,bl,bw,arr);
          tcfile >> nft;   // for PM-2
          tcfile >> nb;
          tcfile >> bl;
          tcfile >> bw;
          tcfile >> ef2[isp];
          tcfile >> rg2[isp];
	  for(i=0;i<TOF2GC::PMTSMX;i++)tcfile >> ps2[isp][i];
          for(i=0;i<nb;i++){arr[i]=0.;}
          for(i=0;i<nb;i++){tcfile >> arr[i];}
          td2[isp]=TOFTpoints(nb,bl,bw,arr);
//
        } // <--- end of scan points loop -----
//
        swdscan[div]=TOF2Scan(nsp,iscp,scp,ef1,ef2,rg1,rg2,ps1,ps2,td1,td2);// create single div. scan obj
//
      } // <--- end of divisions loop -----
//
      tcfile.close(); // close file
      scmcscan[brt]=TOFWScan(npmts,nwdivs,wdivxl,wdivxh,swdscan);// create complete scan obj
  } // --- end of bar-types loop --->
  cout<<"<---- TOFWScan::build: succsessfully done !"<<endl<<endl;
}
//===================================================================
//
void TOF2Tovt::build()
{
//      <--- prepare PM single electron pulse height distribution:
  static geant arr[19]={50.,20.,39.7,46.5,52.3,54.6,52.7,47.6,40.7,
                          32.5,23.9,16.7,11.,7.,4.5,3.,1.5,1.,0.5};
  static AMSDistr scpmsesp(19,0.,0.279,arr);// p/h spectrum ready
//                   ( scale in mV@50ohm to have 1.5mV as m.p. value!!!)
//
  integer id,idN,idd,ibar,ilay,ibtyp,cnum,is;
  static integer cnumo;
  geant tslice1[TOF2GC::SCTBMX+1]; //  flash ADC array for side-1
  geant tslice2[TOF2GC::SCTBMX+1]; //  flash ADC array for side-2
  geant tslice[TOF2GC::SCTBMX+1]; //  flash ADC array
  geant bnw,bwid; 
  integer warr[TOFGC::AMSDISL]; 
  int i,i0,j,jj,ij,jm,k,stat(0),nelec,ierr(0),lbn,nbm;
  int status[2]={0,0};
  integer nhitl[TOF2GC::SCLRS];
  geant edepl[TOF2GC::SCLRS],edepb;
  geant dummy(-1);
  geant time,qtime,x,y,z,am,am0,am2,sig,r,rnd,eff,rgn,de,tm,eps(0.002);
  geant vl,vh;
  geant nel0,nel,nelb,nesig,nphot;
  geant shar1[TOF2GC::PMTSMX],shar2[TOF2GC::PMTSMX];
  geant wshar1[TOF2GC::PMTSMX],wshar2[TOF2GC::PMTSMX];
  AMSPoint cloc(999.,999.,999.);
  AMSgvolume *p;
  AMSTOFMCCluster *ptr;
  AMSTOFMCCluster *ptrN;
  int i1,i2,idivx,nwdivs,npmts;
  static geant bthick;
//
  static integer first=0;
  static integer npshbn;
  static geant pmplsh[1000];  // PM s.e. pulse shape
  static geant fadcb,ifadcb,trts,tmax,zlmx,convr;
  static geant sesmp[TOF2GC::SCBTPN],sesig[TOF2GC::SCBTPN];
  static geant sesav[TOF2GC::SCBTPN],sesas[TOF2GC::SCBTPN],sesrat;
  int nsebnd(30);//use asimptotic spectrum if Nse/bin >= nsebnd
  geant amm,amm2;
  geant corrg;
  int npess[2];
//
  int ii,kk;
  char inum[11];
  char in[2]="0";
  char vname[5];
  strcpy(inum,"0123456789");
  char hmod[2]=" ";
//
  if(first++==0){//<---prepare some time-stable params
    AMSmceventg::SaveSeeds();
//    AMSmceventg::PrintSeeds(cout);
    AMSmceventg::SetSeed(0);
//    cout <<" first in"<<endl;
    bthick=0.5*TOF2DBc::plnstr(6);//half bar-thickness
    cnumo=0;
    TOF2Tovt::inipsh(npshbn,pmplsh); // <---prepare PM sing.electron PulseShape array
    HBOOK1(1099,"Single electron spectrum,mV",65,0.,13.,0.);
    HBOOK1(1094,"Spectrum of Sum(Ntest_el) ,mV",80,0.,80.,0.);
    cout<<"====> TOF2Tovt::build: Prepare some SE-spectrum related data..."<<endl;
    bool bprint(0);
    for(int ibt=0;ibt<TOF2GC::SCBTPN;ibt++){//<---prepare SES-params vs Btyp
      bprint=(((ibt+1)==2 || (ibt+1)==6 || (ibt+1)==7) && (TFMCFFKEY.mcprtf[0]==1));
      sesmp[ibt]=TOF2DBc::sespar(ibt,0);
      sesig[ibt]=sesmp[ibt]*TOF2DBc::sespar(ibt,1);//SigRelat->SigAbs(mv)
      if(bprint){
        cout<<"  BarType="<<ibt+1<<"  SE-spectrum: gaussian mp/sig="<<sesmp[ibt]
                                                                  <<" "<<sesig[ibt]<<endl;
      }
      am=0.;
      am2=0.;
      for(i=0;i<5000;i++){
//      rnd=RNDM(dummy);
//      am0=scpmsesp.getrand(rnd);//amplitude from "Live" single elect. spectrum
        am0=sesmp[ibt]+sesig[ibt]*rnormx();// tempor use simple gaussian
        if(am0<0.)am0=0.;
        am+=am0;
        am2+=am0*am0;
        if(bprint)HF1(1099,am0,1.);
      }
      if(bprint)HPRINT(1099);
      am/=5000.;
      am2/=5000.;
      sesav[ibt]=am;//SE-spectrum average(Ase)
      sesas[ibt]=sqrt(am2-am*am);//SE-spectrum rms(Sse)
      sesrat=sesas[ibt]/sesav[ibt];
      if(bprint)cout<<"      Original SE-specrtum Aver/Sigm="<<sesav[ibt]<<" "<<sesas[ibt]<<" ratio="<<sesrat<<endl;
      if(bprint)HRESET(1099,hmod);
      if(bprint)cout<<"      Asimpt.spectrum for "<<nsebnd<<" electrons has Aver/Sig="<<
                            (nsebnd*sesav[ibt])<<" "<<(sqrt(geant(nsebnd))*sesas[ibt])<<endl;
      amm=0;
      amm2=0;
      for(int j=0;j<2000;j++){
        am=0.;
        for(i=0;i<nsebnd;i++){
          am0=sesmp[ibt]+sesig[ibt]*rnormx();
          if(am0<0.)am0=0.;
          am+=am0;
        }
        if(bprint)HF1(1094,am,1.);
        amm+=am;
        amm2+=am*am;
      }
      if(bprint)HPRINT(1094);
      amm/=2000;
      amm2/=2000;
      amm2=sqrt(amm2-amm*amm);
      if(bprint)cout<<"      True spectrum for "<<nsebnd<<" electrond has Aver/Sig="<<amm<<" "<<amm2<<endl;
      if(bprint){
        HRESET(1094,hmod);
        cout<<"--------------"<<endl;
      }
    }//--->endof bar-type loop
//
    fadcb=TOF2DBc::fladctb();
    ifadcb=1./fadcb;
    tmax=TOF2GC::SCTBMX*fadcb-1.;// ns
    zlmx=TOF2DBc::plnstr(6)/2.+eps;
    convr=1000.*TOF2DBc::edep2ph();
    AMSmceventg::RestoreSeeds();
    cout<<"<---- TOF2Tovt::build: SE-spectrum related data prepared !"<<endl;
    
//    cout <<"first out "<<endl;
  }
//
  ptr=(AMSTOFMCCluster*)AMSEvent::gethead()->
                                      getheadC("AMSTOFMCCluster",0,1);
//                                        ( sort by idsoft done)
//
  for(i=0;i<TOF2GC::SCLRS;i++){
    nhitl[i]=0;
    edepl[i]=0.;
  }
  for(i=0;i<TOF2GC::PMTSMX;i++){
    shar1[i]=0;
    shar2[i]=0;
    wshar1[i]=0;
    wshar2[i]=0;
  }
  for(i=0;i<TOF2GC::SCTBMX+1;i++){
    tslice1[i]=0.;// clear s1/s2 flash ADC arrays
    tslice2[i]=0.;
  }
//
  edepb=0.;// Edep in given bar
  npess[0]=0;
  npess[1]=0;
  while(ptr){//       <------------- loop over geant hits
    id=ptr->idsoft; 
    ilay=id/100-1;
    nhitl[ilay]+=1;
    ibar=id%100-1;
    ibtyp=TOF2DBc::brtype(ilay,ibar)-1;//0->10
    if((ilay==1 && ibtyp!=4) ||
       (ilay==2 && ibtyp!=8))corrg=1.065;//tempor fix to compensate higher gain(1/0.939) for B+T counters set in 
// LTRANS because now i want to equilize count.signals in tofsim02-part only (using SingleElectrSpectrum MostProb value)
//, by this trick i still keep (patrially) effect of HV influence on PM transition time (really missing in current
// setup with equal HV/gains on PMs of one side, difference in TTS between sides/counters is not important due to
// offline T0-calibration) 
    else corrg=1;
    de=ptr->edep;
    edepl[ilay]+=de;
    edepb+=de;
    time=(1.e+9)*(ptr->tof); // conv. to "ns"
    x=ptr->xcoo[0];
    y=ptr->xcoo[1];
    z=ptr->xcoo[2];
//  if(TOF2DBc::debug)cout<<"MChit id="<<id<<" edep="<<de<<" /x/y/z="<<x<<" "<<y<<" "<<z<<" time="<<time<<endl;
    AMSPoint cglo(x,y,z);
    strcpy(vname,"TF");
    kk=ilay*TOF2GC::SCMXBR+ibar+1;//bar ID used in the volume name
    ii=kk/10;
    in[0]=inum[ii];
    strcat(vname,in);
    ii=kk%10;
    in[0]=inum[ii];
    strcat(vname,in);
    p=AMSJob::gethead()->
           getgeomvolume(AMSID(vname,id));// pointer to volume "TFnn",id
#ifdef __AMSDEBUG__
    assert(p != NULL);
#endif
    cloc=p->gl2loc(cglo);// convert global coord. to local
    x=cloc[0];
    y=cloc[1];
    z=cloc[2];
    TOF2JobStat::addmc(12);
//    HF1(1080,geant(time),1.);
    if((time>=tmax) || (time<0.)){
      TOF2JobStat::addmc(11);
//            cout<<"TOF: Bad G-hit, id="<<id<<" t/de="<<time<<" "<<de*1000<<'\n';
      ptr=ptr->next(); // take next hit
      continue;
    }
    qtime=time*ifadcb;//G-hit abs.time in FADC-channel units
    cnum=TOF2DBc::barseqn(ilay,ibar);// solid sequential numbering(0->33)
    nwdivs=TOFWScan::scmcscan[ibtyp].getndivs();//real # of wdivs
    npmts=TOFWScan::scmcscan[ibtyp].getnpmts();//real # of pmts per side
    idivx=TOFWScan::scmcscan[ibtyp].getwbin(x);// x-div #
    if(idivx<0 || (fabs(z)-bthick)>0.01){
      if(cnum!=cnumo){//do not count repeated bad hits from the same volume  
#ifdef __AMSDEBUG__
        cout<<"---> TOF2Tovt::build: OutOfVolError:ID="<<id<<" cnum="<<cnum<<" Vol.name:"<<vname<<endl;
        if(idivx<0){
          cout<<"     Out-of-width hit !"<<endl;
          nbm=TOF2DBc::getbppl(ilay);
	  if(ibar==0 || ibar==(nbm-1))bwid=0.5*TOF2DBc::outcp(ilay,1);
	  else bwid=0.5*TOF2DBc::plnstr(5);                      
	  HF1(1078,geant(fabs(x)-bwid),1.);
        }
        if((fabs(z)-bthick)>0.01){
          cout<<"     Out-of-thickness hit !"<<endl;
	  HF1(1079,geant(fabs(z)-bthick),1.);
        }    
        cout<<"   xyzloc="<<x<<" "<<y<<" "<<z<<endl;
        cout<<"   xyzglo="<<cglo[0]<<" "<<cglo[1]<<" "<<cglo[2]<<" de="<<de<<endl;
        cout<<"   Evnum="<<GCFLAG.IEVENT<<endl;
#endif
        TOF2JobStat::addmc(3);
        cnumo=cnum;
      }
      ptr=ptr->next(); // take next hit
      continue;
    }
    TOFWScan::scmcscan[ibtyp].getybin(idivx,y,i1,i2,r);//y-bin #
    nel0=de*convr;// -> photons
//
// PM(side=1) actions --->
//
    eff=TOFWScan::scmcscan[ibtyp].getef1(idivx,r,i1,i2);//eff for PM-1
    rgn=TOFWScan::scmcscan[ibtyp].getgn1(idivx,r,i1,i2);//gain for PM-1
    rgn/=corrg;//remove B+T Lg efficiency compensation(by gain) made in LTRANS
    for(i=0;i<npmts;i++){//to calc.later average(hit-energy-weighted) Dpms signals sharing
      shar1[i]+=(de*TOFWScan::scmcscan[ibtyp].getps1(i,idivx,r,i1,i2));//dyn.sharing
      wshar1[i]+=de;
    }
    nel=nel0*eff;// mean total number of photoelectrons
//cout<<" PMsid=1"<<" eff/gn="<<eff<<" "<<rgn<<" nel="<<nel<<endl;
    POISSN(nel,nelec,ierr);// fluctuations
    npess[0]+=nelec;
    for(i=0;i<TOFGC::AMSDISL;i++)warr[i]=0;
//    <-------- create phel. arrival-time distribution(PM-1) ---<<<
    for(i=0;i<nelec;i++){
      tm=TOFWScan::scmcscan[ibtyp].gettm1(idivx,r,i1,i2);//phel.arrival time from interpol.distr.
      uinteger ii=uinteger(floor(tm*ifadcb));
      if(ii<TOFGC::AMSDISL)warr[ii]+=1;
    }
    i0=integer(floor(qtime));// 1st bin pos. in abs. scale (for tslice1)
    if((qtime-geant(i0))>0.5)i0+=1;// to compensate(partially) binning influence
//    <-------- Loop over distr.bins ---<<<
    for(i=0;i<TOFGC::AMSDISL;i++){
      uinteger ii=i0+i;
      if(ii>TOF2GC::SCTBMX)break;//ignore "out of FADC time-range" 
      nelec=warr[i];
      if(nelec!=0){
        if(nelec<nsebnd){//direct simulation of Atot of nelec(if low statistics)
          for(k=0;k<nelec;k++){//<-- summing of all amplitudes from nelec photoelectrons 
            am=sesmp[ibtyp]+sesig[ibtyp]*rnormx();//amplitude from SE-spectrum
            if(am>0.)tslice1[ii]+=am*rgn;
          }
        }
        else{//asimptotics(suppose gaussian:Atot=nelec*Ase; Stot=sqrt(nelec)*Sse)
          am=sesav[ibtyp]*nelec;
          sig=sesas[ibtyp]*sqrt(geant(nelec));
          tslice1[ii]+=(am+sig*rnormx())*rgn;//now rgn=1 if all PMs of 1 side are OK
        }
      }
    } // >>>----- end of PM-1 loop ------>
//
// PM(side=2) actions --->
//
    eff=TOFWScan::scmcscan[ibtyp].getef2(idivx,r,i1,i2);//eff for PM-2
    rgn=TOFWScan::scmcscan[ibtyp].getgn2(idivx,r,i1,i2);//gain for PM-2
    rgn/=corrg;
    for(i=0;i<npmts;i++){//to calc.later average(hit-energy-weighted) Dpms signals sharing
      shar2[i]+=(de*TOFWScan::scmcscan[ibtyp].getps2(i,idivx,r,i1,i2));//dyn.sharing
      wshar2[i]+=de;
    } 
    nel=nel0*eff;// mean total number of photoelectrons
//cout<<" PMsid=2"<<" eff/gn="<<eff<<" "<<rgn<<" nel="<<nel<<endl;
    POISSN(nel,nelec,ierr);// fluctuations
    npess[1]+=nelec;
    for(i=0;i<TOFGC::AMSDISL;i++)warr[i]=0;
//    <-------- create phel. arrival-time distribution(PM-2) ---<<<
    for(i=0;i<nelec;i++){
      tm=TOFWScan::scmcscan[ibtyp].gettm2(idivx,r,i1,i2);//phel.arrival time from interpol.distr.
      uinteger ii=uinteger(floor(tm*ifadcb));
      if(ii<TOFGC::AMSDISL)warr[ii]+=1;
    }
    i0=integer(floor(qtime));// 1st bin pos. in abs. scale (for tslice2)
    if((qtime-geant(i0))>0.5)i0+=1;// to compensate(partially) binning influence
//    <-------- Loop over distr.bins ---<<<
    for(i=0;i<TOFGC::AMSDISL;i++){
      uinteger ii=i0+i;
      if(ii>TOF2GC::SCTBMX)break;//ignore "out of FADC time-range"
      nelec=warr[i];
      if(nelec!=0){
        if(nelec<nsebnd){
          for(k=0;k<nelec;k++){//<-- summing of all amplitudes from photoelectrons 
            am=sesmp[ibtyp]+sesig[ibtyp]*rnormx();//amplitude from SE-spectrum
            if(am>0.)tslice2[ii]+=am*rgn;
          }
        }
        else{
          am=sesav[ibtyp]*nelec;
          sig=sesas[ibtyp]*sqrt(geant(nelec));
          tslice2[ii]+=(am+sig*rnormx())*rgn;
        }
      }
    } // >>>----- end of PM-2 loop ------>
//-----------------------------------
      if(ptr->testlast()){ // <---- last or new bar -> create Tovt-objects :
        edepb*=1000.;// --->MeV
	if(edepb>0.)TOF2JobStat::addmc(4);//count fired bars
        if(edepb>TFMCFFKEY.Thr){// process only bar with Edep>Ethr
// PM(side=1) loop to apply pulse shape to each tslice-bin :
          idd=id*10+1;//LBBS
          if(TFMCFFKEY.mcprtf[2]!=0){
            if(idd==1011)HF1(1031,geant(npess[0]),1.);// pe's per side for ref.bar
            if(idd==1041)HF1(1032,geant(npess[0]),1.);
            if(id==104 && npess[0]>10 && npess[1]>10){
	      HF1(1042,geant((npess[0]-npess[1]))/geant((npess[0]+npess[1])),1.);
              HF1(1043,geant(npess[0]+npess[1]),1.);
	    }
            if(idd==4011)HF1(1033,geant(npess[0]),1.);
            if(idd==4041)HF1(1034,geant(npess[0]),1.);
            if(idd==2011)HF1(1035,geant(npess[0]),1.);
            if(idd==2021)HF1(1036,geant(npess[0]),1.);
            if(idd==2041)HF1(1037,geant(npess[0]),1.);
            if(idd==3011)HF1(1038,geant(npess[0]),1.);
            if(idd==3021)HF1(1039,geant(npess[0]),1.);
            if(idd==3031)HF1(1040,geant(npess[0]),1.);
            if(idd==3051)HF1(1041,geant(npess[0]),1.);
	  }
          am0=fabs(TFMCFFKEY.blshift*rnormx());//base line shift simulation
          for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice[i]=am0;
          for(i=0;i<=TOF2GC::SCTBMX;i++){
            am=tslice1[i];
            if(am>0){
	      for(j=0;j<npshbn;j++){
	        ij=i+j;
                if(ij>TOF2GC::SCTBMX)break;//ignore FADC time-range ovfl
                tslice[ij]+=am*pmplsh[j];
	      }
            }
          }        
          for(i=0;i<npmts;i++)shar1[i]/=wshar1[i];//calc. average Dpms-signals sharing
          TOF2Tovt::totovt(idd,edepb,tslice,shar1);// Tovt-obj for side(PM)-1
          if(TFMCFFKEY.mcprtf[1] > 0){
                   TOF2Tovt::displ_a("Side-1 PM pulse, id=",idd,20,tslice);//print PMT pulse
	  }
//        
// PM(side=2) loop to apply pulse shape :
          am0=fabs(TFMCFFKEY.blshift*rnormx());//base line shift simulation
          idd=id*10+2;
          for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice[i]=am0;
          for(i=0;i<=TOF2GC::SCTBMX;i++){
            am=tslice2[i];
            if(am>0){
	      for(j=0;j<npshbn;j++){
	        ij=i+j;
                if(ij>TOF2GC::SCTBMX)break;//ignore FADC time-range ovfl
                tslice[ij]+=am*pmplsh[j];
	      }
            }
          }        
          for(i=0;i<npmts;i++)shar2[i]/=wshar2[i];//calc. average Dpms-signals sharing
          TOF2Tovt::totovt(idd,edepb,tslice,shar2);// Tovt-obj for side(PM)-2
          if(TFMCFFKEY.mcprtf[1] > 0)
                   TOF2Tovt::displ_a("Side-2 PM pulse, id=",idd,20,tslice);//print PMT pulse
        }// ---> end of counter Edep check
//
        for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice1[i]=0.;//clear flash ADC arrays for next bar
        for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice2[i]=0.;
        edepb=0.;// clear Edep
        npess[0]=0;
        npess[1]=0;
        for(i=0;i<TOF2GC::PMTSMX;i++){//clear Dpms-sharing arrays
          shar1[i]=0;
          shar2[i]=0;
          wshar1[i]=0;
          wshar2[i]=0;
        }
      }// ---> end of next/last bar check
//
    ptr=ptr->next();
  }// ------ end of geant hits loop ---->
//-------
//
// <--- arrange in incr.order TOF2Tovt::SumHT/SumSHT-arrays,created by all calls to TOF2Tovt::totovt(...)-routine:
  int nhth;
  for(int ic=0;ic<TOF2GC::SCCRAT;ic++){
    for(int sl=0;sl<TOF2GC::SCFETA-1;sl++){
      nhth=TOF2Tovt::SumHTh[ic][sl];
      if(nhth>0){//<-- non-empty slot with SumHTt-hits
	AMSsortNAGa(TOF2Tovt::SumHTt[ic][sl],nhth);
	if(TFMCFFKEY.mcprtf[3]>0){
          cout<<"     TOF2TovT:SumHT>0:cr/sl_seq="<<ic<<" "<<sl<<" Nhits="<<nhth<<endl;
          for(int ih=0;ih<nhth;ih++)cout<<"       "<<TOF2Tovt::SumHTt[ic][sl][ih]<<endl;
	}
      }
      nhth=TOF2Tovt::SumSHTh[ic][sl];
      if(nhth>0){//<-- non-empty slot with SumSHTt-hits
	AMSsortNAGa(TOF2Tovt::SumSHTt[ic][sl],nhth);
	if(TFMCFFKEY.mcprtf[3]>0){
          cout<<"     TOF2TovT:SumSHT>0:cr/ssl="<<ic<<" "<<sl<<" Nhits="<<nhth<<endl;
          for(int ih=0;ih<nhth;ih++)cout<<"       "<<TOF2Tovt::SumSHTt[ic][sl][ih]<<endl;
	}
      }
    }
  }

//
  if(TFMCFFKEY.mcprtf[2]!=0){
    for(i=0;i<TOF2GC::SCLRS;i++)HF1(1050+i,geant(nhitl[i]),1.);
    HF1(1060,1000.*edepl[0],1.);
    HF1(1061,1000.*edepl[1],1.);
    HF1(1062,1000.*edepl[2],1.);
    HF1(1063,1000.*edepl[3],1.);
  }

//
}
//=========================================================================
//
// function below creates PMT pulse shape array arr[] with binning fladctb :
//  peak value = 1.(e.g. 1mV@50ohm)
void TOF2Tovt::inipsh(integer &nbn, geant arr[])
{
  static integer tbins=12;
//  static geant pmpsh[15]={0.,0.026,0.175,0.422,0.714,0.942,1.,0.935,
//       0.753,0.584,0.442,0.305,0.182,0.078,0.};// pulse heights at time points
//  static geant pmpsb[15]={0.,0.5,1.,1.5,2.,2.5,3.,3.5,4.,4.5,5.,
//       5.5,6.,6.5,7.}; // time points (min. step should be above fladctb)
  static geant pmpsh[12]={0.,0.132,0.85,1.,0.83,0.68,0.35,0.15,0.078,
       0.059,0.029,0.};// pulse heights at time points
  static geant pmpsb[12]={0.,1.,2.25,3.5,4.75,6.,8.5,11.,13.5,16.,18.5,
                      21.}; // time points (min. step should be above fladctb)
  integer i,io,ib,lastiob(0);
  geant bl,bh,bol,boh,ao1,ao2,tgo,al,ah,tota;
  tota=0.;
  io=0;
  nbn=0;
  for(ib=0;ib<TOF2GC::SCTBMX;ib++){
    bl=ib*TOF2DBc::fladctb();
    bh=bl+TOF2DBc::fladctb();
    bol=pmpsb[io];
    boh=pmpsb[io+1];
    arr[ib]=0.;
    if(bl>=bol && bh<=boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      ah=ao1+tgo*(bh-bol);
      arr[ib]=(al+ah)/2.;
      tota+=arr[ib];
    }
    else if(bl<boh && bh>boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      io+=1;//next io-bin
      if((io+1)<tbins){
        bol=pmpsb[io];
        boh=pmpsb[io+1];
        ao1=pmpsh[io];
        ao2=pmpsh[io+1];
        tgo=(ao2-ao1)/(boh-bol);
        ah=ao1+tgo*(bh-bol);
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
      }
      else{                 // last io-bin
        lastiob=1;
        ah=0.;
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
	nbn=ib+1;
      }
    }
    else{//bl>=boh
      io+=1;//next io-bin
      if((io+1)<tbins)ib-=1;//to continue loop with the same ib
      else{                // last io-bin
        lastiob=1;
	nbn=ib;
      }
    }
    if(lastiob){//<-- last iobin test
      if(TFMCFFKEY.mcprtf[0] == 2){
        tota=tota*TOF2DBc::fladctb()/50.;
        printf("TOF-PM pulse shape:nbins=% 3d, Integral(pC) = %4.2e\n",nbn,tota);
        for(i=1;i<=nbn;i++){
          if(i%10 != 0)
                       printf("% 4.2e",arr[i-1]);
          else
                       printf("% 4.2e\n",arr[i-1]);
        }
        if(nbn%10 !=0)printf("\n");
      }
      return;
    }//--->endof last iobin test
  }//--->endof ib loop
//
  cerr<<"TOF2Tovt:inipsh-Err: PM-pulse length exeeds Flash-ADC range !?"<<'\n';
  cerr<<"                 nbn="<<nbn<<endl;
  exit(1);
}
//--------------------------------------------------------------------------
//
// function below simulate PMT saturation, it returns nonsat. factor <=1. :
//
geant TOF2Tovt::pmsatur(const geant am){
  geant gain=1.; //tempor. no saturation
  return gain;
}
//--------------------------------------------------------------------------
//
// function to display PMT pulse (flash-ADC array arr[]) :
//
void TOF2Tovt::displ_a(char comm[], int id, int mf, const geant arr[]){
  integer i;
  geant tm,a(0.);
  geant tb,tbi;
  char name[80], buf[10];
  sprintf(buf, "%4d\n",id);
  strcpy(name,comm);
  strcat(name,buf);
  tb=geant(mf)*TOF2DBc::fladctb();
  tbi=TOF2DBc::fladctb();
  HBOOK1(1000,name,100,0.,100*tb,0.);
  for(i=1;i<=TOF2GC::SCTBMX;i++){
    if(i%mf==0){
      a+=arr[i-1];
      tm=i*tbi-0.5*tb;
      HF1(1000,tm,a/geant(mf));
      a=0.;
    }
    else{
      a+=arr[i-1];
    }
  }
  HPRINT(1000);
  HDELET(1000);
  return ;
}
//--------------------------------------------------------------------------
//
void TOF2Tovt::totovt(integer idd, geant edepb, geant tslice[], geant shar[])
{
//!!! Logic is preliminary:wait for detailed SFET slecrtrical logic from Diego(Guido)
//
  integer i,ii,j,ij,ilay,ibar,isid,id,_sta,stat(0);
  geant tm,a,am,am0,amd,tmp,amp,amx,amxq;
  geant iq0,it0,itau;
  integer _ntr1,_ntr2,_ntr3,_nftdc,_nstdc,_nadca,_nadcd;
  number _ttr1u[TOF2GC::SCTHMX1],_ttr2u[TOF2GC::SCTHMX1],_ttr3u[TOF2GC::SCTHMX1];
  number _ttr1d[TOF2GC::SCTHMX1],_ttr2d[TOF2GC::SCTHMX1],_ttr3d[TOF2GC::SCTHMX1];
  number _tftdc[TOF2GC::SCTHMX2],_tftdcd[TOF2GC::SCTHMX2];
  number _tstdc[TOF2GC::SCTHMX3];
  number _adca;
  number _adcd[TOF2GC::PMTSMX];
  number tovt,aqin;
  int updsh;
  int imax,imin;
  geant a2dr[2],adc2q;
  static geant a2ds[2],adc2qs;
  geant tshd,tshup,charge,saturf;
  geant tbn,w,bo1,bo2,bn1,bn2,tmark;
  static integer first=0;
  static integer nshbn,mxcon,mxshc,mxshcg;
  static geant fladcb,cconv;
  geant daqt0,daqt1,daqt2,daqt3,daqt4,fdaqt0;
  static geant daqp0,daqp1,daqp2,daqp3,daqp4,daqp5,daqp6,daqp7,daqp8,daqp9,daqp10;
  static geant daqp11,daqp12;
  number adcs;
  int16u otyp,mtyp,crat,slot,tsens;
//
//cout<<"======>Enter TOF2Tovt::totovt with id="<<idd<<endl;
  id=idd/10;// LBB
  isid=idd%10-1;// side
  ilay=id/100-1;
  ibar=id%100-1;
  mtyp=0;
  otyp=0;
  AMSSCIds tofid(ilay,ibar,isid,otyp,mtyp);//otyp=0(anode),mtyp=0(LT-time)
  crat=tofid.getcrate();//current crate#
  slot=tofid.getslot();//sequential slot#(0,1,...9)(2 last are fictitious for d-adcs)
  tsens=tofid.gettempsn();//... sensor#(1,2,...,5 == sequential SFET(A)'s number !!!)
//
  if(first++==0){
    fladcb=TOF2DBc::fladctb();          //prepare some time-stable parameters
    cconv=fladcb/50.; // for mV->pC (50 Ohm load)
    daqp0=TOF2DBc::daqpwd(0);// fixed PW of "FTC(HT)"-branch output pulse(ACTEL-outp going to SPT)
    daqp1=TOF2DBc::daqpwd(1);// minimal inp.pulse width(TovT) to fire discr.(its rise time)
    daqp2=TOF2DBc::daqpwd(2);// fixed PW of "FTZ(SHT)"-branch output pulse(ACTEL-outp going to SPT) 
    daqp3=TOF2DBc::daqpwd(3);// min.outPW of discr.(outPW=inpTovT-daqp1 when outPW>daqp3)
    daqp4=TOF2DBc::daqpwd(4);// inp dead time of generic discr(outpDT=daqp4+daqp1)
    daqp7=TOF2DBc::daqpwd(7);// dead time of TDC-inputs(ns,the same for LT-/FT-/Sum-inputs) 
    daqp8=TOF2DBc::daqpwd(8);// dead time of "HT/SHT-trig"-branch on ACTEL-output(going to SPT-inp)
//                                      (Guido: ACTEL-inp is faster than Discr, so no ACTEL-inp DT check)
    daqp9=TOF2DBc::daqpwd(9);// dead time of "SumHT(SHT)"-branch on ACTEL-output(going to TDC-inp) 
//                                      (Guido: ACTEL-inp is faster than Discr, so no ACTEL-inp DT check)
    daqp10=TOF2DBc::daqpwd(10);// generic discr. intrinsic accuracy
    daqp11=TOF2DBc::daqpwd(11);// fixed PW of "SumHT(SHT)"-branch on ACTEL-output(going to TDC)
  }
// time-dependent parameters(thresholds mainly) !!! :
  daqt0=TOF2Varp::tofvpar.daqthr(0); //discr-1 thresh(Low) for LT-channel 
  fdaqt0=0.1*daqt0;// lowered threshold to select "working" part of pulse(m.b. loose some charge !!!)
  daqt1=TOF2Varp::tofvpar.daqthr(1); //discr-2 threshold(High) for HT-channel(FTC)
  daqt2=TOF2Varp::tofvpar.daqthr(2); //discr-3 threshold(VeryHigh) for SHT-channel(FTZ)  
//
// -----> create/fill summary Tovt-object for idsoft=idd :
//
        if(tslice[TOF2GC::SCTBMX]>daqt0){//test last bin of flash-ADC ("FADC")
          TOF2JobStat::addmc(5);
//#ifdef __AMSDEBUG__
          cout<<"SITOFTovt-W: MC_FADC time-range overflow, id="<<idd<<
             "  AmplOfLastBin(mV)= "<<tslice[TOF2GC::SCTBMX]<<'\n';
          if(TFMCFFKEY.mcprtf[1]>1)TOF2Tovt::displ_a("FADC-ovfl, PM-pulse id=",idd,100,tslice);//print PMT pulse
//#endif
        }
        for(i=TOF2GC::SCTBMX;i>0;i--){
	  imax=i;
          if(tslice[i]>fdaqt0 && tslice[i-1]>fdaqt0)break;//find high limit as highest 2bins above thresh.
	}
        for(i=0;i<TOF2GC::SCTBMX;i++){
	  imin=i;
	  if(tslice[i]>fdaqt0)break;//find low limit
	}
// max possible index range: 0,1,...,TOF2GC::SCTBMX !!!
//
//cout<<"---->InTovT:id/imax="<<idd<<" "<<imax<<endl;
        charge=0.;
        _ntr1=0;
        _ntr2=0;
        _ntr3=0;
        _nftdc=0;
        _nstdc=0;
//--->
//   Based on available from Guido information, i created the following logic of
//      SFET front part(simple and fast but may be wrong a little):
//    1) for each logical channel(LT,HT,SHT,SumHT) there is a generic discriminator(fast core)
//      which require: min input pulse duration(~5ns) to go UP, have small dead time(~10ns),
//      have extending (proportional to input TovT) outp. pulse duration with some small 
//      minimum pulse width(~7ns)
//    2) the core is followed by slower output branch(s) where its own dead time(buzy) state
//      is controlled, core up-times are stored in the FIFO buffer. Branch up-setting is driven
//      by discriminator, but is branch's own dead time controlled !. Branch may be self-resetted to
//      prowide nesessary outp.pulse width(independently on discr. state !). 
//<---
     int upd1=0; //up-flag for discr-1(LowThr, followed by 1 branch: LT-time)
     geant tmd1u=-9999.;//up-time of discr-1 
     geant tmd1d=-9999.;// down-time of discr-1
     int pupd1=0;//pre_up-flag (to manage minimal inp.pulse duration(rise time) requirement)
     geant tpupd1=0;//time of the 1st pre_up (.................................)
     geant td1ref=-9999;
     
     int upd11=0;//up-flag for branche-1
     geant tmd11u=-9999.;//branche-1 up-time
     geant tmd11d=-9999.;//branche-1 down-time
//
     int upd2=0; //up-flag for discr-2(HiThr, followed by 1 branche: Z>=1(FTC)-trig)
     geant tmd2u=-9999.;//up-time of discr-2 
     geant tmd2d=-9999.;//down-time of discr-2 
     int pupd2=0;//pre_up-flag (to manage minimal inp.pulse duration(rise time) requirement)
     geant tpupd2=0;//time of the 1st pre_up (...........................................)
     
     int upd21=0;// up-flag for FTC(z>=1) branch(#1) of discr-2
     geant tmd21u=-9999;//branche-1 up-time
     geant tmd21d=-9999;//branche-1 down-time
//
     int upd3=0;//up-flag for discr-3(SHiThr,  followed by 1 branch: Z>=2 trig(FTZ))
     geant tmd3u=-9999.;//up-time of discr-3 
     geant tmd3d=-9999.;//down-time of discr-3
     int pupd3=0;//pre_up-flag (to manage minimal inp.pulse duration requirement)
     geant tpupd3=0;//time of the 1st pre_up (.................................)
     
     int upd31=0;// up-flag for FTZ(z>=2) branch of discr.3
     geant tmd31u=-9999.;// branch-1 up-time
     geant tmd31d=-9999.;// branch-1 up-time
     geant maxtu;
//	 
//      _
//    _| |_
//___|     |___  PMT-pulse
//   ^tmp
//     ^tm
//
        amx=0.;
        amp=0.;
        tmp=fladcb*imin;//low edge of imin-bin
        tm=tmp;
        for(i=imin;i<=imax;i++){  //  <--- time bin loop to simulate time measurements ---
          tm+=fladcb;//high edge of the current bin
          am=tslice[i];
          if(am>amx)amx=am;//to find max amplitude
          am=am+TFMCFFKEY.hfnoise*rnormx();//tempor high freq. noise
          charge+=am;
//-------------------          
// generic discr-1 up/down setting (used by LT-time branch):
          if(am>=daqt0){// <=== Am>LTthr
	    if(pupd1==0 && (tm-tmd1d)>daqp4){// try set D1 pre-up state(inp. dead time check)
	      pupd1=1;
	      tpupd1=tm;
	      tmark=tmp+fladcb*(daqt0-amp)/(am-amp);//precise pre-up time (bin-width compencated)
	    }
	    if(pupd1==1 && upd1==0){// try set d1 "up" if it is "down" and pre-up state OK 
              if((tm-tpupd1)>daqp1){//min inpTovT check
                upd1=1;
                tmd1u=tm;// up time of discr.1(delayed wrt tpupd1 according to min inpTovT (rise time))
		td1ref=tmark+daqp1;//D1 "precise time"(when pre-up state was 1st time UP + FIXED rise-time)
              }
	    }
          }
          else{//Am<LTthr
	    pupd1=0;//reset pre-up state
            if(upd1!=0 && (tm-tmd1u)>daqp3){ //try reset D1 (min. outPW check)
              upd1=0;
              tmd1d=tm;//down time of discr.1
            }
          }
//------------------
// generic discr-2 up/down setting (used by z>=1(FTC)-trig branch):
          if(am>=daqt1){// <=== Am>HTthr
	    if(pupd2==0 && (tm-tmd2d)>daqp4){// try set D2 pre-up state(inp dead time check)
	      pupd2=1;
	      tpupd2=tm;
	    }
	    if(pupd2==1 && upd2==0){// try set D2 "up" if it is "down" and pre-up state OK 
              if((tm-tpupd2)>daqp1){// min inpTovT check
                upd2=1;
                tmd2u=tm;// up time of discr-2(delayed wrt tpupd2 according to min inpTovT (rise time))
//		cout<<"D2up:tmd2u/d="<<tmd2u<<" "<<tmd2d<<endl;
              }
	    }
          }
          else{//Am<HTthr
	    pupd2=0;//reset pre-up state
            if(upd2!=0 && (tm-tmd2u)>daqp3){ // try reset D2 (min. outPW check)
              upd2=0;
              tmd2d=tm;
//		cout<<"D2down:tmd2u/d="<<tmd2u<<" "<<tmd2d<<endl;
            }
          }
//------------------
// generic discr-3 up/down setting (used by "z>=2(FTZ)" branch:
          if(am>=daqt2){// <=== Am>SHTthr
	    if(pupd3==0 && (tm-tmd3d)>daqp4){// try set D3 pre-up state(inp dead time check)
	      pupd3=1;
	      tpupd3=tm;
	    }
	    if(pupd3==1 && upd3==0){// try set D3 "up" if it is "down" and pre-up state OK 
              if((tm-tpupd3)>daqp1){// min inpTovT check
                upd3=1;
                tmd3u=tm;// up time of discr-3(delayed wrt tpupd3 according to min inpTovT (rise time))
              }
	    }
          }
          else{//Am<SHTthr
	    pupd3=0;//reset pre-up state
            if(upd3!=0 && (tm-tmd2u)>daqp3){ // try reset D3 (min. outPW check)
              upd3=0;
              tmd3d=tm;
            }
          }
//----------
          amp=am;// store ampl to use as "previous" one in next i-loop(for interpolation)
          tmp=tm;// ......time .........................................................
//
//------------------------------------
//
// =========> try set branch-1 of discr-1(LT)  just on discr. up-edge: 
          if(upd1>0 && tm==tmd1u){//<-- D1-up moment
// ---> D1,branch-1(LT-time TDC) :        
            if(upd11==0){//try to set branch up
              if((tm-tmd11d)>daqp7){ // branch-1(TDC-input itself) dead time(buzy) check
                upd11=1;  // set up-state 
                tmd11u=tm;//store up-time 
                if(_nstdc<TOF2GC::SCTHMX3){//store upto SCTHMX3 up-edges
                  _tstdc[_nstdc]=td1ref+daqp10*rnormx();//store precise up-time + intrinsic fluct.
                  _nstdc+=1;
                }
                else{
                  TOF2JobStat::addmc(25);
                  cout<<"TOF2Tovt::build-W: LT-time buffer ovfl, nhits="<<_nstdc<<endl;
                  upd11=2;//to block buffer input
                }
              } 
            }
          }//<-- end of discr-1 up-check
//
// =========> try set branch-1 of discr-2(HT)  when it is up:
// 
          if(upd2>0 && tm==tmd2u){//<-- D2-up moment
            if(TOF2Tovt::SumHTh[crat][tsens-1]<TOF2GC::SCTHMX2){//store upto SCTHMX2 D2 up-edges
	      TOF2Tovt::SumHTt[crat][tsens-1][TOF2Tovt::SumHTh[crat][tsens-1]]=tmd2u;//store D2 up-times
	      TOF2Tovt::SumHTh[crat][tsens-1]+=1;// for later simulation of SumHT-time channel
	    }
            else{
              TOF2JobStat::addmc(26);
              cout<<"TOF2Tovt::build-W: SumHT-time buffer ovfl, nhits="<<TOF2Tovt::SumHTh[crat][tsens-1]<<endl;
            }
// ---> D2,branch-1(FTC-trig) :
            if(upd21==0){
              if((tm-tmd21d)>daqp8){//ACTELoutp=SPTinp dead time check, imply that Actel can go UP only on D2 front edge !
                upd21=1;  // set up-state
		tmd21u=tm;//store up-time to use at self-reset(outpPW) check
//		cout<<"D2B1up:tmd21u/d="<<tmd21u<<" "<<tmd21d<<endl;
              } 
            }
	  }
//--------
// ========> try set branch-1 of discr-3(SHT)  when it is up: 
          if(upd3>0 && tm==tmd3u){//<-- D3-up moment
            if(TOF2Tovt::SumSHTh[crat][tsens-1]<TOF2GC::SCTHMX2){//store upto SCTHMX2 D3 up-edges
	      TOF2Tovt::SumSHTt[crat][tsens-1][TOF2Tovt::SumSHTh[crat][tsens-1]]=tmd3u;//store D3 up-times
	      TOF2Tovt::SumSHTh[crat][tsens-1]+=1;// for later simulation of SumSHT-time channel
	    }
            else{
              TOF2JobStat::addmc(27);
              cout<<"TOF2Tovt::build-W: SumSHT-time buffer ovfl, nhits="<<TOF2Tovt::SumSHTh[crat][tsens-1]<<endl;
            }
// ---> D3,branch-1(FTZ-trig) :        
            if(upd31==0){// try set branch up
              if((tm-tmd31d)>daqp8){//ACTELoutp=SPTinp dead time check 
                upd31=1;  // set up state
		tmd31u=tm;//store up-time to use at self-reset(outpPW) check
              } 
            }
          }
//------------------------------------------
// try reset all branches:
//--------
// "LT-time TDC"-branche(#1) of Discr1
          if(upd11==1){ // branch is up - try reset it
            if(tm==tmd1d || i==imax){//reset(go-to-ready) on D1 "down"-edge or on "time-out"
	      upd11=0;
	      tmd11d=tm;//store down-time to use in next i-loop set-up (DT) check
	    }
          }
//--------
// "FTC-trig"-branch(#1) of Discr2(imply fixed outp.pulse width made by ACTEL)
          if(upd21==1){// branch is up - try reset it(check also discr-2 extra pulse during branch "up"-state)
	    maxtu=max(tmd21u,tmd2u);//latest between br21-up and d2-up
            if((tm-maxtu)>daqp0 || i==imax){//self-reset in daqp0 after the latest event: d2-up or br21-up
              upd21=0;
	      tmd21d=tm;//store down-time to use in next i-loop set-up stage
              if(_ntr1<TOF2GC::SCTHMX1){//store upto SCTHMX1 up/down-edges pairs
                _ttr1u[_ntr1]=tmd21u;//don't need accurate up-time for trigger
                if(i<imax)_ttr1d[_ntr1]=tmd21d;
		else _ttr1d[_ntr1]=maxtu+daqp0;//"internal time-out" case, use fixed pwid starting from
//                                         max(tmd21u,tmd2u)(if there is exra D2up during bran21 "up"state, tmd2u > tmd21u)
                _ntr1+=1;
              }
              else{
                cerr<<"TOF2Tovt::build-W: HT(FTC)-buffer ovfl, nhits="<<_ntr1<<endl;
                upd21=2;//to block buffer input
              }
            }
          }
//--------
// "FTZ-trig"-branch(#1) of Discr3(imply fixed outp.pulse width made by ACTEL)
          if(upd31==1){// branch is up - try reset it(check also discr-3 extra pulse during branch "up"-state)
	    maxtu=max(tmd31u,tmd3u);//latest between br21-up and d2-up
            if((tm-maxtu)>daqp2 || i==imax){//self-reset in daqp0 after the latest event: d3-up or br31-up
              upd31=0;
	      tmd31d=tm;//store down-time to use in next i-loop set-up stage
              if(_ntr3<TOF2GC::SCTHMX1){//store upto SCTHMX1(=8) up/down-edges pairs
                _ttr3u[_ntr3]=tmd31u;//up-time
                if(i<imax)_ttr3d[_ntr3]=tmd31d;//down-time
		else _ttr3d[_ntr3]=maxtu+daqp2;//"internal time-out" case, use fixed pwid starting from
//                                         max(tmd31u,tmd3u)(if there is exra D3up during bran31 "up"state, tmd3u > tmd31u)
                _ntr3+=1;
              }
              else{
                cerr<<"TOF2Tovt::build-W: FTZ-buffer ovfl, nhits="<<_ntr3<<endl;
                upd31=2;//to block buffer input
              }
            }
          }
//---------------------------        
        } //      --- end of time bin loop for time measurements --->
//---------------------------         
        charge=charge*fladcb/50.; // get total charge (pC)
        saturf=TOF2Tovt::pmsatur(charge);//true charge reduction fact. due to satur.
        if(TFMCFFKEY.mcprtf[2]!=0){
          if(idd==1011)HF1(1081,amx,1.);// ampl.spectrum for ref.bar
          if(idd==1041)HF1(1082,amx,1.);// ampl.spectrum for ref.bar
          if(idd==4011)HF1(1083,amx,1.);// ampl.spectrum for ref.bar
          if(idd==4041)HF1(1084,amx,1.);// ampl.spectrum for ref.bar
          if(idd==2011)HF1(1085,amx,1.);// ampl.spectrum for ref.bar
          if(idd==2021)HF1(1086,amx,1.);// ampl.spectrum for ref.bar
          if(idd==2041)HF1(1087,amx,1.);// ampl.spectrum for ref.bar
          if(idd==3011)HF1(1088,amx,1.);// ampl.spectrum for ref.bar
          if(idd==3021)HF1(1089,amx,1.);// ampl.spectrum for ref.bar
          if(idd==3031)HF1(1090,amx,1.);// ampl.spectrum for ref.bar
          if(idd==3051)HF1(1091,amx,1.);// ampl.spectrum for ref.bar
	  if(idd==1041)HF1(1070,float(charge),1.);
	  for(i=0;i<_ntr1;i++){
	    HF1(1072,geant(_ttr1d[i]-_ttr1u[i]),1.);
	  }
	  for(i=0;i<_ntr3;i++){
	    HF1(1073,geant(_ttr3d[i]-_ttr3u[i]),1.);
	  }
	  HF1(1071,geant(_ntr1),1.);
	  HF1(1071,geant(_ntr3+10),1.);
	}
//**************************************************************************
        number ped,sig,gain,eqs;
	geant pmgn,dsum(0),asum(0),rrr;
	integer brtyp,npmts;
	brtyp=TOF2DBc::brtype(ilay,ibar)-1;//0-10
        npmts=TOFWScan::scmcscan[brtyp].getnpmts();//real # of pmts per side
        _adca=0;
        _nadcd=0;
	for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
	  _adcd[ipm]=0.;
	}
        aqin=number(charge);// anode signal in pC (ref.bar !)
	aqin*=TOFBrcalMS::scbrcal[ilay][ibar].getagain(isid);//in current bars("seed" gains)
//anode:
	ped=TOFBPeds::scbrped[ilay][ibar].apeda(isid);// aver.ped in adc-chann. units(float)
	sig=TOFBPeds::scbrped[ilay][ibar].asiga(isid);// .... sig
//	sig=0.1;
        TOFBrcalMS::scbrcal[ilay][ibar].q2a2q(0,isid,0,adcs,aqin);// Qa(pC)->Anode(adc,float) 
	if(adcs>TOF2GC::SCPUXMX)adcs=TOF2GC::SCPUXMX;//PUX-chip saturation
	if(TFMCFFKEY.mcprtf[2]!=0)
	                          if(idd==1041)HF1(1074,float(adcs),1.);
        _adca=adcs+ped+sig*rnormx();// Anode adc+ped (float)
//	cout<<"  Aped/sig="<<ped<<" "<<sig<<" _adca="<<_adca<<endl;
//
//
//dynode:
        TOFBrcalMS::scbrcal[ilay][ibar].q2a2q(0,isid,1,eqs,aqin);//Qa(pC)->Dynode(adc,equil.sum,float)
	if(TFMCFFKEY.mcprtf[2]!=0)
	                          if(idd==1041)HF1(1075,float(eqs/npmts),1.);
	for(int ipm=0;ipm<npmts;ipm++){
	  pmgn=TOFBrcalMS::scbrcal[ilay][ibar].getgnd(isid,ipm);//Dynode(pm) rel.gain(wrt side aver)
	  adcs=eqs*shar[ipm]*pmgn;//Dyn(eqs)->Dyn(pm)
	  if(adcs>TOF2GC::SCPUXMX)adcs=TOF2GC::SCPUXMX;//PUX-chip saturation
	  ped=TOFBPeds::scbrped[ilay][ibar].apedd(isid,ipm);
	  sig=TOFBPeds::scbrped[ilay][ibar].asigd(isid,ipm);
          _adcd[_nadcd]=adcs+ped+sig*rnormx();// Dynode(pm) adc+ped (float)
//	  cout<<"  Dped/sig="<<ped<<" "<<sig<<" adcd="<<_adcd[_nadcd]<<" nadcd="<<_nadcd<<endl;
          _nadcd+=1;//really it is number of PMTs/side because all adcd's accepted
//if(adcs>2500){
//  cout<<"---> TofSimDynode:L/B/S="<<ilay<<" "<<ibar<<" "<<isid<<" pm="<<ipm<<" eqs="<<eqs<<endl;
//  cout<<" adc/adc+ped="<<adcs<<" "<<_adcd[_nadcd]<<" shar="<<shar[ipm]<<" pmgn="<<pmgn<<endl;
//}
	}
//
//------------------------------
// now create/fill TOF2Tovt object (id=idd) with above digi-data:
//      if(_ntr1>0){// if counter_side FT-signal(HT) exists->create object
      if(_nstdc>0){//if counter_side LT-signal exists->create object 
        _sta=0;    
        stat=0;
	if(TFMCFFKEY.mcprtf[3]>0){
          cout<<"  --->TOFTovT-obj created for id="<<idd<<endl;
          cout<<"      nLThits="<<_nstdc<<endl;
          for(int ih=0;ih<_nstdc;ih++)cout<<"      "<<_tstdc[ih]<<endl;;
          cout<<"      nFThits="<<_ntr1<<endl;
          for(int ih=0;ih<_ntr1;ih++)cout<<"Tup/down="<<_ttr1u[ih]<<" "<<_ttr1d[ih]<<endl;
          cout<<"      nFTZhits="<<_ntr3<<endl;
          for(int ih=0;ih<_ntr3;ih++)cout<<"Tup/down="<<_ttr3u[ih]<<" "<<_ttr1d[ih]<<endl;
          cout<<"      adca="<<_adca<<" nadcd="<<_nadcd<<" "<<_adcd[0]<<" "<<_adcd[1]<<" "<<_adcd[2]<<endl;
	}
        if(AMSEvent::gethead()->addnext(AMSID("TOF2Tovt",ilay), new
             TOF2Tovt(idd,_sta,charge,edepb,
             _ntr1,_ttr1u,_ttr1d,_ntr3,_ttr3u,_ttr3d,
             _nstdc,_tstdc,
             _adca,
	     _nadcd,_adcd)))stat=1;
      }
//
      return;
//
}
//
//---------------------------------------------------------------------
//
// function to get absolute time of the FIRST trigger(coincidence) "z>=1";
// (it is FTC-time in Lin's paper)
// trcode - trigger code (L-pattern) =-1/n, n>=0 -> MissingPlaneCode(=missing
// planeN if <=4), =-1 -> no trigger.
// cpcode - LookUpTables pattern MN, N(M)=1/0->lut1(lut2)=satisfied/not
// (It is LVL1 FTCP0/FTCP1 flags in Lin's paper)
//
number TOF2Tovt::ftctime(int &trcode, int &cpcode){
  integer i1,i2,isd,isds(0),first(0);
  integer i,j,ilay,ibar,ntr,idd,id,idN,stat,intrig,sorand,toflc(-1);
  integer lut1,lut2;
  uinteger sbt,lsbit(1);
  number ftime(-1),ttru[TOF2GC::SCTHMX1],ttrd[TOF2GC::SCTHMX1];
  geant t1,t2;
  AMSBitstr trbs[2];
  AMSBitstr trbi;
  AMSBitstr trbl[TOF2GC::SCLRS];
  TOF2Tovt *ptr;
//
  geant trigb=TOF2DBc::trigtb();
  geant cgate=TOF2DBc::daqpwd(5);//gate for tof-pattern creation(z>=1)(i.e. FTC-pulse width)
  trcode=-1;
  cpcode=0;
  lut1=Trigger2LVL1::l1trigconf.toflut1();//lut from DB(file)
  lut2=Trigger2LVL1::l1trigconf.toflut2();
  if(TGL1FFKEY.toflc>=0){//overwrite lut's settings by request from data card
    toflc=TGL1FFKEY.toflc;
    if(toflc==0){//3of4
      lut1=1<<7+1<<11+1<<13+1<<14;//3 layers combinations
      lut2=1<<15;//4 layers combination
    }
    else if(toflc==1){//4of4
      lut1=1<<15;//4 layers combination
      lut2=0;//none
    }
    else if(toflc==2){//2of4
      lut1=1<<5+1<<6;// 13+23 comb.
      lut2=1<<9+1<<10;// 14+24 comb
    }
    else if(toflc==3){//>=3of4 + 1&2
//      lut1=(lut1|(1<<3));//3of4 + 1&2 (add 1&2 to default >=3of4)
//      lut2=1<<15;// 4of4
      lut1=(1<<3);
      lut2=0;//none
    }
    else{
      cout<<"FatalError: TOF2Tovt::ftctime: wrong layer conf. was requested from data card !!!"<<endl;
      exit(1);
    }
  }
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    isds=0;
    trbl[ilay].bitclr(1,0);
    trbs[0].bitclr(1,0);
    trbs[1].bitclr(1,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      trbi.bitclr(1,0);
      idd=ptr->getid();//LBBS
      isd=idd%10-1;
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      ilay=id/100-1;
      intrig=Trigger2LVL1::l1trigconf.tofinmask(int(ilay),int(ibar));//TofInTrig from DB
      if(intrig==1 || (intrig>1 && (intrig-2)!=isd)){//<--bar/side in trigger(not masked)
//
        ntr=ptr->gettr1(ttru,ttrd);// get number and up/down times of "z>=1"==HT branch 
        for(j=0;j<ntr;j++){// <--- trig-hits loop ---
          t1=geant(ttru[j]);
          t2=geant(ttrd[j]);
          i1=integer(t1/trigb);
          i2=integer(t2/trigb);
          if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
          if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
          trbi.bitset(i1,i2);//set bits according to hit-time and pulse width
	  trbs[isd]=trbs[isd] | trbi;//make side-OR
        }// --- end of trig-hits loop --->
//
      }//--> endof "in trig" check
//
      ptr=ptr->next();// take next Tovt-hit
// 
    }//--- end of Tovt-hits loop in layer --->
// Now create both side  bit pattern for CURRENT layer
// (trbs[isd]-pulses are not width-formatted)
    if((TGL1FFKEY.tofsc%10)>=0)sorand=TGL1FFKEY.tofsc;//TofPlaneSidesOrAnd from data-card(z>=1)
    else sorand=Trigger2LVL1::l1trigconf.tofoamask(int(ilay));//TofPlaneSidesOrAnd from DB (indivL)
    if(sorand == 0)
                                trbl[ilay]=trbs[0] & trbs[1];// Plane 2-sides AND
    else
                                trbl[ilay]=trbs[0] | trbs[1];// Plane 2-sides OR
    trbl[ilay].clatch();//25MHz-clock latch of 2-sides OR(AND) signal trbl
//
  } //--- end of layer loop --->
//
//---> find earliest coinc. which satisfy lut1:
// 
  int imin;
  t1=-1;
  imin=9999;
// check 4-fold coincidence:
  AMSBitstr coinc1234,rr1,rr2;
  rr1=trbl[0]&trbl[1];
  rr2=trbl[2]&trbl[3];
  coinc1234=rr1&rr2;
  coinc1234.testbit(i1,i2);
  if(i2>=i1 && (lut1 & 1<<15)){//found 4-fold coinc.(requested in lut1)
    if(i1<imin)imin=i1;//store time
  }
// check 3-fold coincidence:
  AMSBitstr coinc0234=trbl[1]&trbl[2]&trbl[3];
  AMSBitstr coinc1034=trbl[0]&trbl[2]&trbl[3];
  AMSBitstr coinc1204=trbl[0]&trbl[1]&trbl[3];
  AMSBitstr coinc1230=trbl[0]&trbl[1]&trbl[2];
  coinc0234.testbit(i1,i2);
  if((i2>=i1) && (lut1 & 1<<14)){//found 3-fold coinc.(requested in lut1)
    if(i1<imin)imin=i1;
  }
  coinc1034.testbit(i1,i2);
  if((i2>=i1) && (lut1 & 1<<13)){//found 3-fold coinc.(requested in lut1)
    if(i1<imin)imin=i1;
  }
  coinc1204.testbit(i1,i2);
  if((i2>=i1) && (lut1 & 1<<11)){//found 3-fold coinc.(requested in lut1)
    if(i1<imin)imin=i1;
  }
  coinc1230.testbit(i1,i2);
  if((i2>=i1) && (lut1 & 1<<7)){//found 3-fold coinc.(requested in lut1)
    if(i1<imin)imin=i1;
  }
// check 2-fold coincidence(only tof-measurement suitable !!):
  AMSBitstr coinc13=trbl[0]&trbl[2];
  AMSBitstr coinc14=trbl[0]&trbl[3];
  AMSBitstr coinc23=trbl[1]&trbl[2];
  AMSBitstr coinc24=trbl[1]&trbl[3];
  AMSBitstr coinc12=trbl[0]&trbl[1];//special request
  coinc13.testbit(i1,i2);
  if((i2>=i1) && (lut1 & 1<<5)){//found 2-fold coinc.(requested in lut1)
    if(i1<imin)imin=i1;
  }
  coinc14.testbit(i1,i2);
  if((i2>=i1) && (lut1 & 1<<9)){//found 2-fold coinc.(requested in lut1)
    if(i1<imin)imin=i1;
  }
  coinc23.testbit(i1,i2);
  if((i2>=i1) && (lut1 & 1<<6)){//found 2-fold coinc.(requested in lut1)
    if(i1<imin)imin=i1;
  }
  coinc24.testbit(i1,i2);
  if((i2>=i1) && (lut1 & 1<<10)){//found 2-fold coinc.(requested in lut1)
    if(i1<imin)imin=i1;
  }
  coinc12.testbit(i1,i2);
  if((i2>=i1) && (lut1 & 1<<3)){//found special 2-fold coinc.L=1&2(requested in lut1)
    if(i1<imin)imin=i1;
  }
  if(imin!=9999){ // lut1-ok
    t1=imin*trigb;
  }
//
//---> find earliest coinc. which satisfy lut2:
// 
  imin=9999;
  t2=-1;
// check 4-fold coincidence:
  coinc1234.testbit(i1,i2);
  if(i2>=i1 && (lut2 & 1<<15)){//found 4-fold coinc.(requested in lut2)
    if(i1<imin)imin=i1;
  }
// check 3-fold coincidence:
  coinc0234.testbit(i1,i2);
  if((i2>=i1) && (lut2 & 1<<14)){//found 3-fold coinc.(requested in lut2)
    if(i1<imin)imin=i1;
  }
  coinc1034.testbit(i1,i2);
  if((i2>=i1) && (lut2 & 1<<13)){//found 3-fold coinc.(requested in lut2)
    if(i1<imin)imin=i1;
  }
  coinc1204.testbit(i1,i2);
  if((i2>=i1) && (lut2 & 1<<11)){//found 3-fold coinc.(requested in lut2)
    if(i1<imin)imin=i1;
  }
  coinc1230.testbit(i1,i2);
  if((i2>=i1) && (lut2 & 1<<7)){//found 3-fold coinc.(requested in lut2)
    if(i1<imin)imin=i1;
  }
// check 2-fold coincidence:
  coinc13.testbit(i1,i2);
  if((i2>=i1) && (lut2 & 1<<5)){//found 2-fold coinc.(requested in lut2)
    if(i1<imin)imin=i1;
  }
  coinc14.testbit(i1,i2);
  if((i2>=i1) && (lut2 & 1<<9)){//found 2-fold coinc.(requested in lut2)
    if(i1<imin)imin=i1;
  }
  coinc23.testbit(i1,i2);
  if((i2>=i1) && (lut2 & 1<<6)){//found 2-fold coinc.(requested in lut2)
    if(i1<imin)imin=i1;
  }
  coinc24.testbit(i1,i2);
  if((i2>=i1) && (lut2 & 1<<10)){//found 2-fold coinc.(requested in lut2)
    if(i1<imin)imin=i1;
  }
  coinc12.testbit(i1,i2);
  if((i2>=i1) && (lut2 & 1<<3)){//found special 2-fold coinc.L=1&2(requested in lut1)
    if(i1<imin)imin=i1;
  }
  if(imin!=9999){ // lut2-ok
    t2=imin*trigb;
  }
//----> check FT :
  if(t1<0 && t2<0)return ftime;//====> NoFT: fail of any LUT-request(trcode=-99, t<0)
  else{
    if(t1>=0)ftime=t1;
    if(t2>=0)ftime=t2;
    if(t1>=0 && t2>=0 && t1<t2)ftime=t1;//earliest time(FT-time) found
  }
//
//----> create layers pattern by latching with FTC-pulse :
//
  AMSBitstr ftpuls,lcoinc;
  ftpuls.bitclr(1,0);
  t1=ftime;//imply no delay in LUT itself !!!
  t2=t1+cgate;
  i1=integer(t1/trigb);
  i2=integer(t2/trigb);
  if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
  if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
  ftpuls.bitset(i1,i2);//set bits according to FT_time/pulse_width(FTC-signal in Lin's paper)
  int16u lpat(0);
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){
    lcoinc.bitclr(1,0);
    lcoinc=ftpuls&trbl[ilay];
    lcoinc.testbit(i1,i2);
    if(i2>=i1)lpat=lpat|(1<<ilay);//coinc. of layer-signal and FTC found
  }
//----> trcode coding(analog of CP layer-pattern in Lin's lvl1 description):
  if(lpat==15)trcode=0;//all 4
  else{
    if(lpat==14)trcode=1;//3of4,miss=1
    if(lpat==13)trcode=2;//3of4,miss=2
    if(lpat==11)trcode=3;//3of4,miss=3
    if(lpat==7)trcode=4; //3of4,miss=4
    if(lpat==5)trcode=5;//2of4,miss=2,4
    if(lpat==9)trcode=6;//2of4,miss=2,3
    if(lpat==6)trcode=7;//2of4,miss=1,4
    if(lpat==10)trcode=8;//2of4,miss=1,3
    if(lpat==3)trcode=9;//2of4,miss=3,4
  }
//----> create CP1,CP2 flags(cpcode) for LVL1:
  cpcode=0;
  if(lut1&(1<<lpat))cpcode+=1;
  if(lut2&(1<<lpat))cpcode+=10;
//
  return ftime;
}
//---------------------------------------------------------------------
//
// function to get absolute time of the "SlowZ>=2" coincidence;
// (it is FTZ-signal in Lin's paper)
// trcode - trigger code  = -1/toflcsz("-" if toflcsz conditions
// was not satisfied)
//
number TOF2Tovt::ftztime(int &trcode){
  integer i1,i2,isd,isds(0),first(0);
  integer i,j,ilay,ibar,ntr,idd,id,idN,stat,intrig,sorand,toflcsz(-1);
  uinteger sbt,lsbit(1);
  integer ewcode,lutbz;
  number ftime(-1),ttru[TOF2GC::SCTHMX1],ttrd[TOF2GC::SCTHMX1];
  geant t1,t2;
  geant pwextt,pwextb;
  AMSBitstr trbs[2];
  AMSBitstr trbi;
  AMSBitstr trbl[TOF2GC::SCLRS];
  TOF2Tovt *ptr;
//
  geant trigb=TOF2DBc::trigtb();
  ewcode=Trigger2LVL1::l1trigconf.tofextwid();//5bits|5bits code for top/bot ext.width
  if(TGL1FFKEY.tofextwid>0){
    pwextt=geant(TGL1FFKEY.tofextwid);
    pwextb=geant(TGL1FFKEY.tofextwid);
  }
  else{
    pwextt=geant(20*(1+(ewcode&31)));//ext.width for top-coinc. signal
    pwextb=geant(20*(1+(ewcode&(31<<5))>>5));//ext.width for bot-coins. signal
  }
  geant cgate=TOF2DBc::daqpwd(6);//gate(FTZ-pulse width)(SlowZ>=2)
  trcode=-1;
//
  toflcsz=Trigger2LVL1::l1trigconf.toflcsz();//FTZ layers config(MN) from DB
  if(TGL1FFKEY.toflcsz>=0)toflcsz=TGL1FFKEY.toflcsz;//overwrite FTZ-config by data card
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    isds=0;
    trbl[ilay].bitclr(1,0);
    trbs[0].bitclr(1,0);
    trbs[1].bitclr(1,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      trbi.bitclr(1,0);
      idd=ptr->getid();//LBBS
      isd=idd%10-1;
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      ilay=id/100-1;
      intrig=Trigger2LVL1::l1trigconf.tofinzmask(int(ilay),int(ibar));//TofInTrig from DB 
      if(intrig==1 || (intrig>1 && (intrig-2)!=isd)){//<--bar/side in trigger(not masked)
//
        ntr=ptr->gettr3(ttru,ttrd);// get number and times of trig3 ("z>=2")
        for(j=0;j<ntr;j++){// <--- trig-hits loop ---
          t1=geant(ttru[j]);
          t2=geant(ttrd[j]);
          i1=integer(t1/trigb);
          i2=integer(t2/trigb);
          if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
          if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
          trbi.bitset(i1,i2);//set bits according to hit-time and pulse width
	  trbs[isd]=trbs[isd] | trbi;//make 1plane/1side-OR
        }// --- end of trig-hits loop --->
//
      }//--> endof "in trig" check
//
      ptr=ptr->next();// take next Tovt-hit
// 
    }//--- end of Tovt-hits loop in layer --->
//
// Now create both side  bit pattern for CURRENT layer
// (trbs[isd]-pulses are not width-formatted)
    if(TGL1FFKEY.tofsc/10>=0)sorand=TGL1FFKEY.tofsc;//TofSidesOrAnd from data-card (z>=2)
    else sorand=Trigger2LVL1::l1trigconf.tofoazmask(int(ilay));//TofSidesOrAnd from DB (indiv, z>=2)
    if(sorand == 0)
                                trbl[ilay]=trbs[0] & trbs[1];// 2-sides AND
    else
                                trbl[ilay]=trbs[0] | trbs[1];// 2-sides OR
    trbl[ilay].clatch();//25MHz-clock latch of 2-sides OR(AND) signal trbl
//
  } //--- end of layer loop --->
//
  trcode=-1;
  t1=0.;
//
//---> create top and bot signals:
//
  AMSBitstr top,bot,etop,ebot;
  etop.bitclr(1,0);
  ebot.bitclr(1,0);
  if(toflcsz%10==0 || toflcsz%10==1)top=(trbl[0]&trbl[1]);//topAND
  else top=(trbl[0]|trbl[1]);//topOR
  top.testbit(i1,i2);
  if(i2>=i1){
    t1=i1*trigb;
    t2=t1+pwextt;
    i2=integer(t2/trigb);
    if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
    etop.bitset(i1,i2);//extended top-coinc
  }
  if(toflcsz%10==0 || toflcsz%10==2)bot=(trbl[2]&trbl[3]);//botAND
  else bot=(trbl[2]|trbl[3]);//botOR
  bot.testbit(i1,i2);
  if(i2>=i1){
    t1=i1*trigb;
    t2=t1+pwextb;
    i2=integer(t2/trigb);
    if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
    ebot.bitset(i1,i2);//extended bot-coinc
  }
//---> check top-bot coinc:
//
  AMSBitstr ctopbot;
  if(toflcsz/10==0)ctopbot=(etop&ebot);//top/bot-AND
  else ctopbot=(etop|ebot);//top/bot-OR
  ctopbot.testbit(i1,i2); 
  if(i2>=i1){
    t1=i1*trigb;
    ftime=t1;//>=0
    trcode=toflcsz;//>=0
  }
//
  return ftime;
}
//---------------------------------------------------------------------
//
// function to create BZ("z>=2") sub-trigger info for LVL1;
// trcode - BZ-trig. code(L-pattern) =-1/n, n>=0 -> MissingPlaneCode(=missing
// planeN if <=4);
// On input require TOF-FT(z>=1)-trigger time(FTC in Lin's paper)
// BZ may be "false" when trcode>=0, but don't match lutbz 
//
bool TOF2Tovt::bztrig(number ftime, int &trcode){
  integer i1,i2,j,isd,isds(0),first(0);
  integer ilay,ibar,ntr,idd,id,stat,intrig,sorand,toflcz(-1);
  integer lutbz,lut(0);
  number ttru[TOF2GC::SCTHMX1],ttrd[TOF2GC::SCTHMX1];
  geant t1,t2;
  AMSBitstr trbs[2];
  AMSBitstr trbi;
  AMSBitstr trbl[TOF2GC::SCLRS];
  TOF2Tovt *ptr;
//
  geant trigb=TOF2DBc::trigtb();
  geant cgate=TOF2DBc::daqpwd(5);//gate(FTC-pulse width)
  trcode=-1;
  lutbz=Trigger2LVL1::l1trigconf.toflutbz();//lvl1 lutbz from DB
  if(TGL1FFKEY.toflcz>=0){//overwrite lutbz by request from data card
    toflcz=TGL1FFKEY.toflcz;
    if(toflcz==0){//>=3of4)
      lutbz=1<<7+1<<11+1<<13+1<<14+1<<15;
    }
    else if(toflcz==1){//4of4
      lutbz=1<<15;
    }
    else if(toflcz==2){//>=2of4(at least 1top and 1bot)
      lutbz=1<<5+1<<6+1<<7+1<<9+1<<10+1<<11+1<<13+1<<14+1<<15;
    }
    else{
      cout<<"FatalError: TOF2Tovt::tr2time: wrong LVL1(BZ)layer conf. was requested from data card !!!"<<endl;
      exit(1);
    }
  }
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    isds=0;
    trbl[ilay].bitclr(1,0);
    trbs[0].bitclr(1,0);
    trbs[1].bitclr(1,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      trbi.bitclr(1,0);
      idd=ptr->getid();//LBBS
      isd=idd%10-1;
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      ilay=id/100-1;
      intrig=Trigger2LVL1::l1trigconf.tofinzmask(int(ilay),int(ibar));//TofInTrigBZ from DB 
      if(intrig==1 || (intrig>1 && (intrig-2)!=isd)){//<--bar/side in trigger(not masked)
//
        ntr=ptr->gettr3(ttru,ttrd);// get number and times of trig3 ("z>=2")
        for(j=0;j<ntr;j++){// <--- trig-hits loop ---
          t1=geant(ttru[j]);
          t2=geant(ttrd[j]);
          i1=integer(t1/trigb);
          i2=integer(t2/trigb);
          if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
          if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
          trbi.bitset(i1,i2);//set bits according to hit-time and pulse width
	  trbs[isd]=trbs[isd] | trbi;//make 1plane/1side-OR
        }// --- end of trig-hits loop --->
//
      }//--> endof "in trig" check
//
      ptr=ptr->next();// take next Tovt-hit
// 
    }//--- end of Tovt-hits loop in layer --->
//
// Now create both side  bit pattern for CURRENT layer
// (trbs[isd]-pulses are not width-formatted)
    if(TGL1FFKEY.tofsc/10>=0)sorand=TGL1FFKEY.tofsc;//TofSidesOrAnd from data-card (z>=2)
    else sorand=Trigger2LVL1::l1trigconf.tofoazmask(int(ilay));//TofSidesOrAnd from DB (indiv, z>=2)
    if(sorand == 0)
                                trbl[ilay]=trbs[0] & trbs[1];// 2-sides AND
    else
                                trbl[ilay]=trbs[0] | trbs[1];// 2-sides OR
    trbl[ilay].clatch();//25MHz-clock latch of 2-sides OR(AND) signal trbl(TOFBZ(L)-signals)
//
  } //--- end of layer loop --->
//
//----> create layers pattern by latching with FT-pulse(got from z>=1, =FTC) :
//
  trcode=-1;
  AMSBitstr ftpuls,lcoinc;
  ftpuls.bitclr(1,0);
  t1=ftime;//imply no delay in LUT-FT itself !!!
  t2=t1+cgate;
  i1=integer(t1/trigb);
  i2=integer(t2/trigb);
  if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
  if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
  ftpuls.bitset(i1,i2);//set bits according to FT-time/pulse width(FTC-signal)
  int16u lpat(0);
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){
    lcoinc.bitclr(1,0);
    lcoinc=ftpuls&trbl[ilay];
    lcoinc.testbit(i1,i2);
    if(i2>=i1)lpat=lpat|(1<<ilay);//coinc. of layer-signal and FT-gate found
  }
//----> trcode coding(like CP layer-pattern in lvl1, but need to apply lutbz here !):
  if(lpat==15)trcode=0;//all 4
  else{
    if(lpat==14)trcode=1;//3of4,miss=1
    if(lpat==13)trcode=2;//3of4,miss=2
    if(lpat==11)trcode=3;//3of4,miss=3
    if(lpat==7)trcode=4; //3of4,miss=4
    if(lpat==5)trcode=5;//2of4,miss=2,4
    if(lpat==9)trcode=6;//2of4,miss=2,3
    if(lpat==6)trcode=7;//2of4,miss=1,4
    if(lpat==10)trcode=8;//2of4,miss=1,3
    if(lpat==1)trcode=9;//
    if(lpat==2)trcode=10;//
    if(lpat==3)trcode=11;//
    if(lpat==4)trcode=12;//
    if(lpat==8)trcode=13;//
    if(lpat==12)trcode=14;//
  }
//here we have input BZ-signals pattern(lpat<=>trcode) ready, now BZ-trig decision :
  bool bz(0);
  if(lpat>0)bz=(lutbz&(1<<lpat))>0;//given L-combination(lpat) is found in LUT-BZ ?
  return bz;
//
}
//---------------------------------------------------------------
void TOF2Tovt::spt2patt(number gftime, integer toftrp1[], integer toftrp2[]){
//
//create TOF HT,SHT-signals patterns on the arrival of globFT(as in SPT2)
//on input gftime includes delay from LVL1 to SPT2 !
//  
  integer ilay,ibar,j,ntr,idd,id,isd,intrig,ncoins;
  uinteger sbt,lsbit(1);
  number ttru[TOF2GC::SCTHMX1],ttrd[TOF2GC::SCTHMX1];
  geant cgate,t1,t2,tg1,tg2;
  geant trigb=TOF2DBc::trigtb();
  TOF2Tovt *ptr;
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){
    toftrp1[ilay]=0;
    toftrp2[ilay]=0;
  }
//
//-----> create counters sides pattern(HT(z>=1):
//
  cgate=TOF2DBc::daqpwd(5);//gate for tof-pattern creation, tempor use FTC-pulse width
  tg1=gftime;//gate_start_time=FTime+fix.delay(from lvl1 to SPT2)
  tg2=tg1+cgate;//gate_end_time
// 
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();//LBBS
      isd=idd%10-1;
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      ilay=id/100-1;
      ncoins=0;
      intrig=Trigger2LVL1::l1trigconf.tofinmask(int(ilay),int(ibar));
      if(intrig==1 || (intrig>1 && (intrig-2)!=isd)){//bar/side in trigger(not masked)
        ntr=ptr->gettr1(ttru,ttrd);// get number and times of "z>=1"==HT hits
        for(j=0;j<ntr;j++){// <--- trig-hits loop ---
          t1=geant(ttru[j]);
          t2=geant(ttrd[j]);
	  if(tg2<=t1 || tg1>=t2)continue;
	  ncoins+=1;
        }// --- end of trig-hits loop --->
	if(ncoins>0){
          if(isd==0)sbt=lsbit<<ibar;
	  else sbt=lsbit<<(16+ibar);
          toftrp1[ilay]|=sbt;//add bit of coinsided side(toftrp:bit(1-16)->s1;bit(17-32)->s2)
	}
      }//endof "in trig" test
//  
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
// 
// ---> create counters sides pattern(SHT(z>=2)):
//
  cgate=TOF2DBc::daqpwd(6);//gate for tof-pattern creation(z>=2)
  tg1=gftime;//gate_start_time=FTime+fix.delay(from lvl1 to SPT2)
  tg2=tg1+cgate;//gate_end_time
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();//LBBS
      isd=idd%10-1;
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      ilay=id/100-1;
      ncoins=0;
      intrig=Trigger2LVL1::l1trigconf.tofinzmask(int(ilay),int(ibar));//TofInTrig from DB
      if(intrig==1 || (intrig>1 && (intrig-2)!=isd)){//bar/side in trigger(not masked)
        ntr=ptr->gettr3(ttru,ttrd);// get number and times of "z>=2"==SHT hits
        for(j=0;j<ntr;j++){// <--- trig-hits loop ---
          t1=geant(ttru[j]);
          t2=geant(ttrd[j]);
	  if(tg2<=t1 || tg1>=t2)continue;
	  ncoins+=1;
        }// --- end of trig-hits loop --->
	if(ncoins>0){
          if(isd==0)sbt=lsbit<<ibar;
	  else sbt=lsbit<<(16+ibar);
          toftrp2[ilay]|=sbt;//add bit of coinsided side(toftrp:bit(1-16)->s1;bit(17-32)->s2)
	}
      }//endof "in trig" test
//  
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
//
}
//=====================================================================
//
//  function to set bits in AMSBitstr objects (from bit il till bit ih):
//   (bits are counting starting from 0 (left edge of bit-array))
//   (if il>ih, function sets all available bits)
//
void AMSBitstr::bitset(const int il, const int ih){
//
  static unsigned short int allzer(0x0000);
  static unsigned short int allone(0xFFFF);
  static unsigned short int setone[16]={
       0x8000,0x4000,0x2000,0x1000,0x0800,0x0400,0x0200,0x0100,
       0x0080,0x0040,0x0020,0x0010,0x0008,0x0004,0x0002,0x0001};
  int i,iw1,iw2,i1,i2,nw;
#ifdef __AMSDEBUG__
  assert(il>=0 && ih>=0);
  assert(il<TOFGC::SCBITM && ih<TOFGC::SCBITM);
#endif
  iw1=il/16;
  i1=il-16*iw1;
  if(il==ih){
    bitstr[iw1]|=setone[i1];
    return;
  }
  if(il>ih){
    for(i=0;i<bslen;i++)bitstr[i]=allone;//set all bits
    return;
  }
  iw2=ih/16;
  i2=ih-16*iw2;
  nw=iw2-iw1;
  if(nw==0){
    for(i=i1;i<=i2;i++)bitstr[iw1]|=setone[i];
  }
  else{
    for(i=i1;i<=15;i++)bitstr[iw1]|=setone[i];
    for(i=0;i<=i2;i++)bitstr[iw2]|=setone[i];
    for(i=iw1+1;i<iw2;i++){bitstr[i]=allone;}
  }
  return;
}
//----------------------------------------------------------------------
//  function to clear bits in AMSBitstr objects (from bit il upto bit ih):
//   (bits are counting starting from 0 (left edge of bit-array))
//   (if il>ih, function clear all available bits)
//
void AMSBitstr::bitclr(const int il, const int ih){
//
  static unsigned short int allzer(0x0000);
  static unsigned short int setzer[16]={
        0x7FFF,0xBFFF,0xDFFF,0xEFFF,0xF7FF,0xFBFF,0xFDFF,0xFEFF,
        0xFF7F,0xFFBF,0xFFDF,0xFFEF,0xFFF7,0xFFFB,0xFFFD,0xFFFE};
  int i,iw1,iw2,i1,i2,nw;
#ifdef __AMSDEBUG__
  assert(il>=0 && ih>=0);
  assert(il<TOFGC::SCBITM && ih<TOFGC::SCBITM);
#endif
  iw1=il/16;
  i1=il-16*iw1;
  if(il==ih){
    bitstr[iw1]&=setzer[i1];
    return;
  }
  if(il>ih){
    for(i=0;i<bslen;i++)bitstr[i]=allzer;//clear all bits
    return;
  }
  iw2=ih/16;
  i2=ih-16*iw2;
  nw=iw2-iw1;
  if(nw==0){
    for(i=i1;i<=i2;i++)bitstr[iw1]&=setzer[i];
  }
  else{
    for(i=i1;i<=15;i++)bitstr[iw1]&=setzer[i];
    for(i=0;i<=i2;i++)bitstr[iw2]&=setzer[i];
    for(i=iw1+1;i<iw2;i++){bitstr[i]=allzer;}
  }
  return;
}
//-----------------------------------------------------------------------
// function to test "up" bits in AMSBitstr object. It returns i1/i2 as
// first/last "up" bits of the FIRST continious bunch of "up" bits 
// in the object.  i1>i2 (1>0) if no "up" bits.
//
void AMSBitstr::testbit(int &i1, int &i2){
  static unsigned short int setone[16]={
       0x8000,0x4000,0x2000,0x1000,0x0800,0x0400,0x0200,0x0100,
       0x0080,0x0040,0x0020,0x0010,0x0008,0x0004,0x0002,0x0001};
  int i,iw;
  int ifl(0);
  unsigned short int ia;
  i1=1;
  i2=0;
  for(iw=0;iw<bslen;iw++){
    for(i=0;i<16;i++){
      ia=bitstr[iw] & setone[i];
      if((ia != 0) && (ifl == 0)){
        i1=iw*16+i;
        ifl=1;
      }
      if((ia == 0) && (ifl == 1)){
        i2=iw*16+i-1;
        ifl=0;
        return;
      }
    }
  }
  if(ifl==1){// case when bits are "up" till the end of bitstream
    i2=16*bslen-1;
    return;
  }
}
//-----------------------------------------------------------------------
AMSBitstr operator &(const AMSBitstr &b, const AMSBitstr &c){
  int lenb=b.bslen;
  int lenc=c.bslen;
  int len=lenb;
  unsigned short int arr[TOFGC::SCWORM];
  if(lenb != lenc){
    cout<<"AMSBitstr:AND-warning:len1 != len2 "<<lenb<<" "<<lenc<<'\n';
    if(lenc<lenb && lenb<TOFGC::SCWORM)len=lenc;
    else if(lenb<lenc && lenc<TOFGC::SCWORM)len=lenb;
    else exit(1);
  }
  for(int i=0;i<len;i++)arr[i]= b.bitstr[i] & c.bitstr[i];
  return AMSBitstr(len,arr);
} 
//-----------------------------------------------------------------------
AMSBitstr operator |(const AMSBitstr &b, const AMSBitstr &c){
  int lenb=b.bslen;
  int lenc=c.bslen;
  int len=lenb;
  unsigned short int arr[TOFGC::SCWORM];
  if(lenb != lenc){
    cout<<"AMSBitstr:OR-warning:len1 != len2 "<<lenb<<" "<<lenc<<'\n';
    if(lenc<lenb && lenb<TOFGC::SCWORM)len=lenc;
    else if(lenb<lenc && lenc<TOFGC::SCWORM)len=lenb;
    else exit(1);
  }
  for(int i=0;i<len;i++)arr[i] = b.bitstr[i] | c.bitstr[i];
  return AMSBitstr(len,arr);
} 
//-----------------------------------------------------------------------
void AMSBitstr::display(char comment[]){
    printf("AMSBitstr: %s ;  length=%d\n",comment,bslen);
    for(int i=1;i<=bslen;i++){
      if(i%10 != 0)
                  printf(" %#x",bitstr[i-1]);
      else 
                  printf(" %#x\n",bitstr[i-1]);
    }
    if(bslen%10 !=0)printf("\n");
  }
//-----------------------------------------------------------------------
void AMSBitstr::clatch(){
//---> make latching with trig.electronics clock: 
  static unsigned short int allzer(0x0000);
  static unsigned short int allone(0xFFFF);
  static unsigned short int setone[16]={
       0x8000,0x4000,0x2000,0x1000,0x0800,0x0400,0x0200,0x0100,
       0x0080,0x0040,0x0020,0x0010,0x0008,0x0004,0x0002,0x0001};
  int cbl=int(floor(TOF2DBc::clkper()/TOF2DBc::trigtb()));//clock periond length (bits)
  unsigned short int bstr[TOFGC::SCWORM]; // max. length in 16-bit words
  unsigned short int bv;
  int i,iw,iwn,jw,nw,ibln,ib,ibl,blmn,blmx(16*bslen);
//
  for(iw=0;iw<TOFGC::SCWORM;iw++)bstr[iw]=0;
  blmn=int(floor(clkfas*TOF2DBc::clkper()/TOF2DBc::trigtb()));//1st clock-pulse position
  for(ib=blmn;ib<blmx;ib+=cbl){
    iw=ib/16;//current word(begg.of period)
    ibl=ib-16*iw;//local bit(in current word)
    bv=bitstr[iw]&setone[ibl];//local bit value of original bitstream
    if(bv!=0){//set next cbl bits starting from current
      iwn=(ib+cbl)/16;//next word(where the new clock period starts)
      ibln=(ib+cbl)-16*iwn;//next loc.bit(in next period word)
      if(iwn>=bslen){
        iwn=bslen-1;
	ibln=15;
      }
      nw=iwn-iw+1;//words to fill(incl.uncomplete ones) bef the start of the next period
      if(nw==0){
        for(i=ibl;i<ibln;i++)bstr[iw]|=setone[i];//set needed 1's in current word
      } 
      else{
        for(i=ibl;i<16;i++)bstr[iw]|=setone[i];//set 1's upto the end of the 1st word of curr. period
	for(jw=0;jw<nw-2;jw++)bstr[iw+1+jw]=allone;//fill middle nw-2 complete words of curr.period
	for(i=0;i<ibln;i++)bstr[iw+nw-1]|=setone[i];//set 1's at the beg. of the last word of curr.period  
      }
    }
  }
  for(iw=0;iw<bslen;iw++)bitstr[iw]=bstr[iw];//refill original bitstream
}
//-----------------------------------------------------------------------
void AMSBitstr::setclkphase(){
//randomly sets trig.electronics clock-pulse phase(called once per event !!!)
  geant dummy(-1);
  clkfas=RNDM(dummy);
}
//
//=====================================================================
//
//  function to build RawEvent-objects from MC Tovt-objects
//               
void TOF2RawSide::mc_build(int &status)
{
  integer i,j,jj,ilay,last,ibar,isd,stat(0);
  int16u id,idd,_sta,hid;
  integer nftdc,nstdc,nsumh,nsumsh,nadcd,itt,ntstdc,ntadcd;
  number tstdc[TOF2GC::SCTHMX3];
  number tadca,tadcd[TOF2GC::PMTSMX];
  integer  ftdc[TOF2GC::SCTHMX1],stdc[TOF2GC::SCTHMX3],sumht[TOF2GC::SCTHMX2],sumsht[TOF2GC::SCTHMX2];
  geant adca,adcd[TOF2GC::PMTSMX];
  number t,t1,t2,t3,t4,ttrig,dt,cstopt,tl1d,ftrig,ecftrig;
  number ftctime,ftztime,ftetime,ftmin;
  geant charge,edep,strr[2][2],str,offs,temp;
  geant daqta,daqtd,rrr;
  number pedv,peds,amp;
  integer iamp;
  integer trcode,trcodez,cpcode(0),ftzcode(-1);
  uinteger ectrfl(0);
  bool bztr(0);
  integer trpatt[TOF2GC::SCLRS]={0,0,0,0};
  integer trpattz[TOF2GC::SCLRS]={0,0,0,0};
  integer it,it1,it2,it3,it4,it0;
  integer phbit,maxv;
  integer ftpatt(0),ftpatreq(0);
  integer trtype(0);
  integer cftmask(0);
  geant trtmax=TOF2DBc::trigtb()*(1+TOFGC::SCBITM);//max possible abs.trig-time(=myTrigTimeScale)
  TOF2Tovt *ptr;
  TOF2Tovt *ptrN;
  int fmask[TOF2GC::SCLRS][TOF2GC::SCMXBR][2];//mask of fired sides
  status=1;// bad(=> no_globFT)
  phbit=TOF2GC::SCPHBP;
  maxv=phbit-1;//max possible TDC value (16383)
  daqta=TOF2Varp::tofvpar.daqthr(3);//daq readout thr (anode, ped sigmas)
  daqtd=TOF2Varp::tofvpar.daqthr(4);//daq readout thr (dynode, ped sigmas)
//  if(TFMCFFKEY.mcprtf[3]>0)cout<<"======>Enter TOF2RawSide::mc_build"<<endl;
//  cout<<"======>Enter TOF2RawSide::mc_build"<<endl;
//
  for(int il=0;il<TOF2GC::SCLRS;il++){
    for(int ib=0;ib<TOF2GC::SCMXBR;ib++){
      for(int is=0;is<2;is++){
        fmask[il][ib][is]=0;//reset mask
      }
    }
  }
//
  trcode=-1;
  trcodez=-1;
  TOF2RawSide::settrcode(trcode);// reset  TOF-layers CP-code(z>=1)
  TOF2RawSide::settrcodez(trcodez);// reset  TOF-layers BZ-code(z>=2)
  TOF2RawSide::setftpatt(ftpatt);// reset  globFT members pattern
  TOF2RawSide::setcpcode(cpcode);// reset  CP0/CP1 flags
  TOF2RawSide::setpatt(trpatt);// reset  SPT2 TOF(HT) pattern(z>=1)
  TOF2RawSide::setpattz(trpattz);// reset  SPT2 TOF(SHT) pattern(z>=2)
  TOF2RawSide::setbzflag(0);// reset  BZ-flag
//
  if(TGL1FFKEY.trtype>0)trtype=TGL1FFKEY.trtype;//active lvl1trig-branches
  else trtype=Trigger2LVL1::l1trigconf.globl1mask();
//
  if(TGL1FFKEY.cftmask>=0)cftmask=TGL1FFKEY.cftmask;//active commonFTrig  branches
  else cftmask=Trigger2LVL1::l1trigconf.globftmask();
  if(cftmask%10>0)ftpatreq|=1;//create binary analog of cftmask
  if((cftmask%100)/10>0)ftpatreq|=(1<<1);
  if(cftmask/100>0)ftpatreq|=(1<<2);
//
  if(TGL1FFKEY.printfl>0){
    cout<<endl;
    cout<<"===> In TOFRawEventMC:globFTmask="<<cftmask<<endl;
  }
//-----
  if(trtype<128){//<=== internal trigger, create combined FT :
    TOF2JobStat::addmc(15);
//====> find globFT time:
// FTC:
    ftctime=TOF2Tovt::ftctime(trcode,cpcode);//get abs.FTC("z>=1") signal time/L-patt/cpcode
    if(trcode>=0){
      TOF2JobStat::addmc(16);
      TOF2RawSide::settrcode(trcode);// set tof-layers CP-code(z>=1)
      TOF2RawSide::setcpcode(cpcode);// set tof-layers CP0/CP1-flag(z>=1)
      ftpatt|=1;
      if(TFMCFFKEY.mcprtf[2]!=0)HF1(1069,geant(trcode),1.);
      bztr=TOF2Tovt::bztrig(ftctime,trcodez);//get BZ-flag/Lpatt
      if(trcodez>=0)TOF2RawSide::settrcodez(trcodez);//set TOF-layers BZ-code(z>=2), bztr may be even fals!!!
      if(bztr){
        TOF2JobStat::addmc(17);
        TOF2RawSide::setbzflag(1);//set  BZ-flag
      }
      if(TFMCFFKEY.mcprtf[2]!=0){
        if(trcodez>=0)HF1(1069,geant(trcodez)+20,1.);
        if(bztr && trcodez>=0)HF1(1069,geant(trcodez)+40,1.);
      }
      if(TGL1FFKEY.printfl>0){
        cout<<"    foundFTC: trcode/cpcode="<<trcode<<" "<<cpcode<<" ftctime="<<ftctime<<endl;
        cout<<"    BZflag/code="<<bztr<<" "<<trcodez<<endl;
      }
    }
//
// FTZ:
    ftztime=TOF2Tovt::ftztime(ftzcode);//get abs.FTZ("SlowZ>=2") signal time/L-conf. request code
    if(ftzcode>=0){
      TOF2JobStat::addmc(18);
      ftpatt|=(1<<1);
      if((ftpatt&1)==0)TOF2JobStat::addmc(23);
      if(TGL1FFKEY.printfl>0)cout<<"    foundFTZ:ftzcode/ftztime="<<ftzcode<<" "<<ftztime<<endl;
    }
// FTE:
    ectrfl=AMSEcalRawEvent::gettrfl();//masked MN->Energy/Angle(width), M=0/1,2/3->non/NonEm/Em,
//                                                              N=0/1/2->Undef/Bad/OK
    if(ftpatt>0)if(TFMCFFKEY.mcprtf[2]!=0)HF1(1076,geant(ectrfl),1.);//hist.EC-flag when was TOF
    if(ectrfl/10>2){//EmEnergy found (Angle flag is used only in LVL1-logic)
      TOF2JobStat::addmc(19);
      ftetime=AMSEcalRawEvent::gettrtm();//(includes delay from EC to LVL1-crate (tempor = TOF-delay)
      if(ftpatt&1==1)if(TFMCFFKEY.mcprtf[2]!=0)HF1(1077,geant(ftctime-ftetime),1.);//when FTC ok
      if(ftpatt==0)TOF2JobStat::addmc(20);// <--- count "no FT from TOF" when EC ok
      ftpatt|=(1<<2);
      if(TGL1FFKEY.printfl>0)cout<<"   foundFTE:ectrfl/ftetime="<<ectrfl<<" "<<ftetime<<endl;
    }
//----
    if(TGL1FFKEY.printfl>0)cout<<"    foundFTpatt:"<<ftpatt<<" masked:"<<(ftpatt & ftpatreq)<<endl;
    if((ftpatt & ftpatreq)==0)return;//===> No globFT or masked
    TOF2JobStat::addmc(21);//<=== found non-masked globFT
//
    ftpatt=(ftpatt&ftpatreq);//<=== globFT-members pattern (masked)
    TOF2RawSide::setftpatt(ftpatt);//set  globFT members pattern
    ftmin=trtmax;
    if((ftpatt&1)>0 && ftctime<ftmin)ftmin=ftctime;
    if((ftpatt&(1<<1))>0 && ftztime<ftmin)ftmin=ftztime;
    if((ftpatt&(1<<2))>0 && ftetime<ftmin)ftmin=ftetime;
    ttrig=ftmin;//<--- this is globFT time in LVL1-crate
    ftrig=ttrig+TOF2Varp::tofvpar.ftdelf();//globFT abs time(+fixed delay) as it came to SFET from "JLV1"
    TOF2Tovt::spt2patt(ftrig,trpatt,trpattz);//<=== create TOF(HT/SHT-signals in SPT2) padd/side patterns
//
    if(TFMCFFKEY.mcprtf[2]!=0){
      for(int il=0;il<TOF2GC::SCLRS;il++){// pattern histogr
      for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
        if((trpatt[il]&(1<<ib)) > 0)HF1(1065,geant(il*20+ib),1.);
        if((trpatt[il]&(1<<(16+ib))) > 0)HF1(1066,geant(il*20+ib),1.);
        if((trpattz[il]&(1<<ib)) > 0)HF1(1067,geant(il*20+ib),1.);
        if((trpattz[il]&(1<<(16+ib))) > 0)HF1(1068,geant(il*20+ib),1.);
      }
      }
    }
//
    TOF2RawSide::setpatt(trpatt);// set SPT2 TOF(HT) pattern(z>=1)
    TOF2RawSide::setpattz(trpattz);// set SPT2 TOF(SHT) pattern(z>=2)
    TOF2RawSide::settrtime(ftrig);// store abs FTrigger time (at SPT2 !) 
//    cstopt=ftrig+TOF2DBc::accdel();// "common stop"-signal abs.time
    if(TGL1FFKEY.printfl>0)cout<<"   FTAbsTime="<<ttrig<<"(inSPT2:"<<ftrig<<")"<<endl;
  }//===> endof "NotExtTrigger" check
//-----
  else{//<==== external trigger
    ftpatt|=(1<<3);
    ttrig=0.;//tempor (true ExtTrigSignal-time)
    ftrig=ttrig+TOF2Varp::tofvpar.ftdelf();// FTrigger abs time (fixed delay added)
    TOF2RawSide::settrtime(ftrig);// store abs FTrigger time (at SPT2 !) 
    TOF2RawSide::setftpatt(ftpatt);//set  globFT members pattern
    cstopt=ftrig+TOF2DBc::accdel();// "common stop"-signal abs.time
    TOF2JobStat::addmc(22);
  }
//
  status=0;//<============ OK because globFT conditions was found above
//
  if(TFMCFFKEY.mcprtf[3]>0)cout<<"    FTabsTime="<<ttrig<<" inSPT2(delayed)="<<ftrig<<endl;
//  
//<---- create TOFRawSide static FTtime-channels arrays(digitization of FT-time, INCLUDING Anti-slots ):
  number ftcrat,ftslot;
  if(TFMCFFKEY.mcprtf[3]>0)cout<<"    Generated FT-signals in crates/slots:"<<endl;
  for(int cr=0;cr<TOF2GC::SCCRAT;cr++){
    ftcrat=ftrig+rnormx()*TOF2DBc::ftc2cj();//actual FT-time in crate(cr-2-cr jitter)
    if(TFMCFFKEY.mcprtf[3]>0)cout<<"      cr="<<cr<<" CrateFTtime="<<ftcrat<<endl;
    for(int sl=0;sl<TOF2GC::SCFETA;sl++){
      ftslot=ftcrat+rnormx()*TOF2DBc::fts2sj();//actual FT-time in slot(sl-2-sl jitter)
      if(TFMCFFKEY.mcprtf[3]>0)cout<<"      ssl="<<sl<<" SlotFTtime="<<ftslot<<endl;
      TOF2RawSide::FTtime[cr][sl][0]=integer(floor(ftslot/TOF2DBc::tdcbin(1)));//digitization
      TOF2RawSide::FThits[cr][sl]=1;//only one hit for MC
    }
  }
//
//----> create LT-time, charge hits scanning TOF2Tovt-objects :
//
  int16u mtyp(0),otyp(0),crat,slot;
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();// LBBS
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      isd=idd%10-1;
      mtyp=0;
      otyp=0;
      AMSSCIds tofid(ilay,ibar,isd,otyp,mtyp);//otyp=0(anode),mtyp=0(TimeTDC))
      crat=tofid.getcrate();//current crate#
      slot=tofid.getslot();//sequential slot#(0,1,...9)(2 last are fictitious for d-adcs)
      hid=tofid.gethwid()/100;//CSS(Crate|Slot=>shorthwid) 
      _sta=ptr->getstat();
      charge=ptr->getcharg();
      edep=ptr->getedep();
//----------------
//AnodeTime(LTchan):
      ntstdc=ptr->getstdc(tstdc);// get number and times of TDC LTtime-hits
      nstdc=0;
      if(TOFBrcalMS::scbrcal[ilay][ibar].SchOK(isd)){//LTtime-ch alive in "MC Seeds" DB
      if(TFMCFFKEY.mcprtf[3]>0)cout<<"    Check/digitize LT-hits for id="<<idd<<" cr/sl="<<crat
                                                             <<" "<<slot<<" hid="<<hid<<endl;
// 0---- young_hits......FT...old_hits ----->time
        for(j=0;j<ntstdc;j++){//       <--- stdc-hits loop ---
          t1=tstdc[j];//LTtime-channle abs.time
	  it1=integer(floor(t1/TOF2DBc::tdcbin(1)));//ns->TDCch
          dt=t1-ftrig;// Time-hit(t1) age wrt FT("-"==>younger, "+"==>older)
          if(TFMCFFKEY.mcprtf[3]>0)cout<<"      Hit="<<j+1<<" AbsTime/dig="<<t1<<" "<<it1
                                                    <<" t-ttrig="<<dt<<" lims="<<
                                                    TOF2DBc::hagemn()<<" "<<TOF2DBc::hagemx()<<endl;
          if(dt<TOF2DBc::hagemn())TOF2JobStat::addmc(7);//too young hit	    
          else if(dt>TOF2DBc::hagemx())TOF2JobStat::addmc(8);//too old	    
	  else{//ok(no check on maxv because of age check)unirun.job
            stdc[nstdc]=it1;
            nstdc+=1;
          }//--endof hit-age check-->
        }//--- end of stdc-hits loop --->
      }//--> end of alive check
//----------------
//Qanode:
    adca=0;
    if(TOFBrcalMS::scbrcal[ilay][ibar].AchOK(isd)){//A(h)-ch alive in "MC Seeds" DB
      pedv=TOFBPeds::scbrped[ilay][ibar].apeda(isd);
      peds=TOFBPeds::scbrped[ilay][ibar].asiga(isd);
      tadca=ptr->getadca();//get ADC-counts of Anode (contains at least ped)
      amp=tadca;// here charge is quantized by "adc2q" but not "integerized"
      iamp=integer(floor(amp));//go to real ADC-counts(on board ADC "integerization")
      if(iamp>TOF2GC::SCADCMX){//ADC overflow
        TOF2JobStat::addmc(9);
	iamp=TOF2GC::SCADCMX;
#ifdef __AMSDEBUG__
        cout<<"TOF2RawSide_mc-W: Anode-ADC overflow,id="<<idd<<'\n';
#endif
      }
      if(TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6){//to test RD PedCalib logic
        if(iamp>0)adca=geant(iamp)+0.5;//No ped-suppression for Ped-run, "+0.5" to be at ADC-bin middle
      }
      else{//normal run
        if(iamp>(pedv+daqta*peds))adca=geant(iamp)+0.5-pedv;//OnBoard_PedSuppression + OfflinePedSubtraction
	else adca=0;
      }
//cout<<"    Anode: ped/sig="<<pedv<<" "<<peds<<" iamp="<<iamp<<" adca(ped-subtracted)="<<adca<<endl;
    }//--> end of alive check
//--------------------
//Qdynode:
    nadcd=0;
    for(j=0;j<TOF2GC::PMTSMX;j++)adcd[j]=0;
    ntadcd=ptr->getadcd(tadcd);// get number of Dynode-pmts(upto PMTSMX) and its ADCs(at least ped)
    for(j=0;j<ntadcd;j++){// <--- Dyn-pmts loop ---
      if(TOFBrcalMS::scbrcal[ilay][ibar].DchOK(isd,j)){//Dyn-PMch alive in "MC Seeds" DB
	amp=tadcd[j];// here charge is quantized by "adc2q" but not "integerized"
        iamp=integer(floor(amp));//DAQ: go to real ADC-channels(on board "integerization")
        if(iamp>TOF2GC::SCADCMX){
          TOF2JobStat::addmc(10);
	  iamp=TOF2GC::SCADCMX;
#ifdef __AMSDEBUG__
          cout<<"TOF2RawSide::mc_build warning: Dynode-ADC overflow,id="<<idd<<'\n';
#endif
        }
        pedv=TOFBPeds::scbrped[ilay][ibar].apedd(isd,j);
        peds=TOFBPeds::scbrped[ilay][ibar].asigd(isd,j);
        if(TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6){//to test RD PedCalib logic
          if(iamp>0){
	    adcd[j]=geant(iamp)+0.5;//No ped-suppression for Ped-run, "+0.5" to be at ADC-bin middle
            nadcd+=1;//number of nonzero pmts !!!
	  }
        }
        else{//normal run
          if(iamp>(pedv+daqtd*peds)){//OnBoard_PedSuppression + OfflinePedSubtraction
	    adcd[j]=geant(iamp)+0.5-pedv;
            nadcd+=1;//number of nonzero(>thr) pmts !!!
	  }
	  else adcd[j]=0;
        }
      }//--> end of alive check
//cout<<"    Dynode: pm/ped/sig="<<j<<" "<<pedv<<" "<<peds<<" iamp="<<iamp<<" adcd(ped-subtr)="<<adcd[j]<<endl;
    }//--- end of Dyn-pm loop --->
//---------------------
//     ---> create/fill RawEvent-object :
// tempor no check on "empty object" because globFT-conditions were found ("status" was set to 0 above)
      temp=0;//now dummy(will be set at validation stage using static JobStat temp-store !!!)
      nftdc=0;//now dummy(will be set at validation stage using static RawEvent FTtime-store !!!)
      nsumh=0;//now dummy(will be set at validation stage using static RawEvent SumHTtime-store !!!)
      nsumsh=0;//now dummy(will be set at validation stage using static RawEvent SumSHTtime-store !!!)
      if(TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6)_sta=1;//for RD PedCal test (no "ped" subtraction/suppression)
      fmask[ilay][ibar][isd]=1;//mark fired side
//
      AMSEvent::gethead()->addnext(AMSID("TOF2RawSide",0), new TOF2RawSide(idd,hid,_sta,charge,temp,
	                           nftdc,ftdc,nstdc,stdc,nsumh,sumht,nsumsh,sumsht,adca,nadcd,adcd));
//
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
//-----------------------------
// <--- create static TOF2RawSide::SumHTt/SumSHTt-arrays(selection of good(out-of-tdcDT) hits in TOF2Tovt):
//      (TOF2TovT::sumHTt/sumSHT-hits are already arranged in incr.order at the end of TOF2TovT::build()) 
  int nhits,phn,nhn,nhnmin;
  geant tprev,tnext;
  integer itprev;
  geant apw=TOF2DBc::daqpwd(11);//PW>=tdcDT for SumHT-ch on ACTEL-output(= TDC-input) 
  for(int ic=0;ic<TOF2GC::SCCRAT;ic++){
    for(int sl=0;sl<TOF2GC::SCFETA-1;sl++){
      nhits=TOF2Tovt::SumHTh[ic][sl];
      if(nhits>0){//<====== non-empty slot with SumHTt-hits
        for(phn=0;phn<nhits;phn++){//<---TOF2Tovt prev.hit loop
	  tprev=TOF2Tovt::SumHTt[ic][sl][phn];
	  itprev=integer(floor(tprev/TOF2DBc::tdcbin(1)));//digitization
	  TOF2RawSide::SumHTt[ic][sl][TOF2RawSide::SumHTh[ic][sl]]=itprev;//store digitized current "prev"
	  TOF2RawSide::SumHTh[ic][sl]+=1;
	  nhnmin=phn+1;
	  for(nhn=nhnmin;nhn<nhits;nhn++){//<---TOF2Tovt next hit loop
	    tnext=TOF2Tovt::SumHTt[ic][sl][nhn];
	    if((tnext-tprev-apw)<=0)continue;//bad(fall in tdcDT) "next"-hit, try next one
	    break;//good next hit
	  }
	  if(nhn>=nhits)break;//no good "next"-hit found - stop selection
	  phn=nhn-1;//use found good "next"-hit as "prev" in prev-loop
	}//endof "prev"-loop--->
      }//endof non-empty slot check--->
//
      nhits=TOF2Tovt::SumSHTh[ic][sl];
      if(nhits>0){//<-- non-empty slot with SumSHTt-hits
        for(phn=0;phn<nhits;phn++){//<---TOF2Tovt prev.hit loop
	  tprev=TOF2Tovt::SumSHTt[ic][sl][phn];
	  itprev=integer(floor(tprev/TOF2DBc::tdcbin(1)));//digitization
	  TOF2RawSide::SumSHTt[ic][sl][TOF2RawSide::SumSHTh[ic][sl]]=itprev;//store digitized current "prev"
	  TOF2RawSide::SumSHTh[ic][sl]+=1;
	  nhnmin=phn+1;
	  for(nhn=nhnmin;nhn<nhits;nhn++){//<---TOF2Tovt next hit loop
	    tnext=TOF2Tovt::SumSHTt[ic][sl][nhn];
	    if((tnext-tprev-apw)<=0)continue;//bad(fall in tdcDT) "next"-hit, try next one
	    break;//good next hit
	  }
	  if(nhn>=nhits)break;//no good "next"-hit found - stop selection
	  phn=nhn-1;//use found good "next"-hit as "prev" in prev-loop
	}//endof "prev"-loop--->
      }//endof non-empty slot check--->
//
    }//endof sfet-loop--->
  }//endof crate-loop--->
//
// ============> fill empty sides with peds (if requested by TFMC 20=1):
//
  if(TFMCFFKEY.addpeds){
    for(int il=0;il<TOF2DBc::getnplns();il++){
      for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
        for(int is=0;is<2;is++){
	  if(fmask[il][ib][is]==0){//empty side
	    idd=1000*(il+1)+10*(ib+1)+is+1;//LBBS
            mtyp=1;
            otyp=0;
            AMSSCIds tofid(il,ib,is,otyp,mtyp);//otyp=0(anode),mtyp=1(q)
            crat=tofid.getcrate();//current crate#
            slot=tofid.getslot();//sequential slot#(0,1,...9)(2 last are fictitious for d-adcs)
            hid=tofid.gethwid()/100;//CSS(Crate|Slot=>shorthwid) 
            adca=0;
            if(TOFBrcalMS::scbrcal[il][ib].AchOK(is)){//Anode-ch alive in "MC Seeds" DB
              pedv=TOFBPeds::scbrped[il][ib].apeda(is);
              peds=TOFBPeds::scbrped[il][ib].asiga(is);
	      amp=pedv+peds*rnormx();//float
              iamp=integer(floor(amp));//DAQ: go to real ADC-channels(on board "integerization")
              if(TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6){//to test RD PedCalib logic
                if(iamp>0)adca=geant(iamp)+0.5;//No ped-suppression for Ped-run, "+0.5" to be at ADC-bin middle
              }
              else{//normal run
                if(iamp>(pedv+daqta*peds))adca=geant(iamp)+0.5-pedv;//OnBoard_PedSuppression + OfflinePedSubtraction
	        else adca=0;
              }
	    }//--->endof alive check
            nadcd=0;
            for(j=0;j<TOF2GC::PMTSMX;j++)adcd[j]=0;
            for(j=0;j<TOF2DBc::npmtps(il,ib);j++){// <--- Dyn-pmts loop ---
              if(TOFBrcalMS::scbrcal[il][ib].DchOK(is,j)){//Dyn-PMch alive in "MC Seeds" DB
                pedv=TOFBPeds::scbrped[il][ib].apedd(is,j);
                peds=TOFBPeds::scbrped[il][ib].asigd(is,j);
	        amp=pedv+peds*rnormx();//float
                iamp=integer(floor(amp));//DAQ: go to real ADC-channels(on board "integerization")
                if(TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6){//to test RD PedCalib logic
                  if(iamp>0){
		    adcd[j]=geant(iamp)+0.5;//No ped-suppression for Ped-run, "+0.5" to be at ADC-bin middle
                    nadcd+=1;//number of nonzero pmts !!!
		  }
                }
                else{//normal run
                  if(iamp>(pedv+daqtd*peds)){//OnBoard_PedSuppression + OfflinePedSubtraction
	            adcd[j]=geant(iamp)+0.5-pedv;
                    nadcd+=1;//number of nonzero(>thr) pmts !!!
	          }
	          else adcd[j]=0;
		}
              }//--->endof alive check
            }//--->endof Dyn-pmt-loop
//     ---> create/fill "ped" RawEvent-object :
            if(adca>0 || nadcd>0){
              temp=0;
              nftdc=0;
	      nstdc=0;
	      edep=0;
              nsumh=0;
              nsumsh=0;
//
              if(TFREFFKEY.relogic[0]==5||TFREFFKEY.relogic[0]==6)_sta=1;//for RD PedCal test (no "ped" subtr/suppr)
              AMSEvent::gethead()->addnext(AMSID("TOF2RawSide",0), new TOF2RawSide(idd,hid,_sta,charge,temp,
	                                 nftdc,ftdc,nstdc,stdc,nsumh,sumht,nsumsh,sumsht,adca,nadcd,adcd));
	    }
	  }//--->endof emty side check
        }//--->endof side-loop
      }//--->endod bar-loop
    }//--->endof layer-loop
  }//--->endof addpeds check
//  
}
//================================================================





TOF2Tovt::TOF2Tovt(integer _ids, integer _sta, number _charga, number _tedep,
  integer _ntr1, number _ttr1u[], number _ttr1d[],
  integer _ntr3, number _ttr3u[], number _ttr3d[],
  integer _nstdc, number _tstdc[],
  number _adca,
  integer _nadcd, number _adcd[]):idsoft(_ids),status(_sta)
  {
    int i;
    ntr1=_ntr1;
    for(i=0;i<ntr1;i++){
      ttr1u[i]=_ttr1u[i];
      ttr1d[i]=_ttr1d[i];
    }
    ntr3=_ntr3;
    for(i=0;i<ntr3;i++){
      ttr3u[i]=_ttr3u[i];
      ttr3d[i]=_ttr3d[i];
    }
    charga=_charga;
    tedep=_tedep;
    nstdc=_nstdc;
    for(i=0;i<nstdc;i++)tstdc[i]=_tstdc[i];
    adca=_adca;
    nadcd=_nadcd;
    for(i=0;i<nadcd;i++){
      adcd[i]=_adcd[i];
    }
  }

integer TOF2Tovt::gettr1(number arru[], number arrd[]){
  for(int i=0;i<ntr1;i++){
    arru[i]=ttr1u[i];
    arrd[i]=ttr1d[i];
  }
  return ntr1;
}
integer TOF2Tovt::gettr3(number arru[], number arrd[]){
  for(int i=0;i<ntr3;i++){
    arru[i]=ttr3u[i];
    arrd[i]=ttr3d[i];
  }
  return ntr3;
}
integer TOF2Tovt::getstdc(number arr[]){
  for(int i=0;i<nstdc;i++)arr[i]=tstdc[i];
  return nstdc;
}
number TOF2Tovt::getadca(){
  return adca;
}
integer TOF2Tovt::getadcd(number arr[]){
  for(int i=0;i<nadcd;i++){
    arr[i]=adcd[i];
  }
  return nadcd;
}





TOF2RawSide::TOF2RawSide(int16u sid, int16u hid, int16u sta, geant charge, geant temp,
   integer nftdc, integer ftdc[],
   integer nstdc, integer stdc[],
   integer nsumh, integer sumht[],
   integer nsumsh, integer sumsht[],
   geant adca,
   integer nadcd, geant adcd[]):_swid(sid),_hwid(hid),_status(sta)
   {
     integer i;
     _nftdc=nftdc;
     for(i=0;i<nftdc;i++)_ftdc[i]=ftdc[i];
     _nstdc=nstdc;
     for(i=0;i<nstdc;i++)_stdc[i]=stdc[i];
     _nsumh=nsumh;
     for(i=0;i<nsumh;i++)_sumht[i]=sumht[i];
     _nsumsh=nsumsh;
     for(i=0;i<nsumsh;i++)_sumsht[i]=sumsht[i];
     _adca=adca;
     _nadcd=nadcd;
     for(i=0;i<TOF2GC::PMTSMX;i++){
       _adcd[i]=adcd[i];
     }
     _charge=charge;
     _temp=temp;
   }

integer TOF2RawSide::getnztdc(){
  integer nz(0);
  if(_nftdc>0)nz+=_nftdc;
  if(_nstdc>0)nz+=_nstdc;
  if(_nsumh>0)nz+=_nsumh;
  if(_nsumsh>0)nz+=_nsumsh;
  if(_adca>0)nz+=1;
  if(_nadcd>0)nz+=_nadcd;
  return nz+1;//+temper.measurement
}


integer TOF2RawSide::getstdc(integer arr[]){
  for(int i=0;i<_nstdc;i++)arr[i]=_stdc[i];
  return _nstdc;
}
void TOF2RawSide::putstdc(integer nelem, integer arr[]){
  _nstdc=nelem;
  for(int i=0;i<_nstdc;i++)_stdc[i]=arr[i];
}

integer TOF2RawSide::getftdc(integer arr[]){
  for(int i=0;i<_nftdc;i++)arr[i]=_ftdc[i];
  return _nftdc;
}
void TOF2RawSide::putftdc(integer nelem, integer arr[]){
  _nftdc=nelem;
  for(int i=0;i<_nftdc;i++)_ftdc[i]=arr[i];
}

integer TOF2RawSide::getsumh(integer arr[]){
  for(int i=0;i<_nsumh;i++)arr[i]=_sumht[i];
  return _nsumh;
}
void TOF2RawSide::putsumh(integer nelem, integer arr[]){
  _nsumh=nelem;
  for(int i=0;i<_nsumh;i++)_sumht[i]=arr[i];
}

integer TOF2RawSide::getsumsh(integer arr[]){
  for(int i=0;i<_nsumsh;i++)arr[i]=_sumsht[i];
  return _nsumsh;
}
void TOF2RawSide::putsumsh(integer nelem, integer arr[]){
  _nsumsh=nelem;
  for(int i=0;i<_nsumsh;i++)_sumsht[i]=arr[i];
}


integer TOF2RawSide::getadcd(geant arr[]){
  for(int i=0;i<TOF2GC::PMTSMX;i++)arr[i]=_adcd[i];
  return _nadcd;
}

void TOF2RawSide::putadcd(integer nelem, geant arr[]){
  _nadcd=nelem;
  for(int i=0;i<_nadcd;i++)_adcd[i]=arr[i];
}

//--------------
integer TOF2RawSide::lvl3format(int16 *ptr, integer rest){
// Note: at the moment of call FT-time is still not attached to RawSide-obj, so i need to take FT
// from static store, where FT-times stored after decoding as crat/slot arrays !!!
  integer ilay,ibar,isid;
  int i,j,nrwt;
  int statdb[2];
  int16u hwid,crat,otyp(0),mtyp(0),slot,tsens;
  int16u id,rwt[10];
  int16 rawt;
  integer nftt,ftt[TOF2GC::SCTHMX1];
  number dt;
//
  id=_swid/10;// short id=LBB, where L=1,4 BB=1,8(10)
  ilay=id/100-1;
  ibar=id%100-1;
  isid=_swid%10-1;
  AMSSCIds tofid(ilay,ibar,isid,otyp,mtyp);//mtyp=0 to find crate/slot for needed slowTDC
  nrwt=0;
  if(TOF2Brcal::scbrcal[ilay][ibar].SideOK(isid)){ // side OK(F&S&A), read out it
    hwid=tofid.gethwid();//CSRR(Crate|Slot|Readout_channel)
    crat=tofid.getcrate();//current crate#(0,1,..)
    slot=tofid.getslot();//sequential slot#(0,1,...9)(2 last are fictitious for d-adcs)
    tsens=tofid.gettempsn();//... sensor#(1,2,...,5),coinsides with SFET(A) seq.slot number!
    nftt=TOF2RawSide::FThits[crat][tsens-1];
    if(nftt>0)for(i=0;i<nftt;i++)ftt[i]=TOF2RawSide::FTtime[crat][tsens-1][i];
    if(nftt==1){//require single FT-hit
      for(i=0;i<_nstdc;i++){//<---LT-hits loop
        dt=(ftt[0]-_stdc[i])*TOF2DBc::tdcbin(1);//LT-time wrt FT-time[ns]
	if(dt>TOF2DBc::ltagew(0) && dt<TOF2DBc::ltagew(1)){//notTooOld(>40ns befFT), notTooYoung(<640 befFT)
	  if(nrwt<10){
            rwt[nrwt]=int16u(ftt[0]-_stdc[i]);//store all LT-time hits(calc. wrt FT-time)  [tdc-channels]
            nrwt+=1;
	  }
	}//--->endof "FT-correlation" check
      }//--->endof LT-hits loop
    }//--->endof single FT-hit check
//---  
  }// --->endof DBstat check
//---
  if(nrwt>0){
    if (rest < 3) return 0;
    rawt=rwt[nrwt-1];//tempor choose last LT-hit
    *(ptr)=hwid;
    *(ptr+1)=_swid;
    *(ptr+2)=rawt;
    return 3; 
  }
  else return 0; 
}










