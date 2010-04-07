//  $Id: antidbc02.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author E.Choumilov 2.06.97
//    18.03.03 changed to be compatible with AMS02 design.
//
#include "typedefs.h"
#include "commons.h"
#include "job.h"
#include "amsdbc.h"
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include "tofdbc02.h"
#include "antidbc02.h"
#include "antirec02.h"
#include "anticalib02.h"
//
ANTI2SPcal ANTI2SPcal::antispcal[ANTI2C::MAXANTI];//mem.resrv. for ANTI-ReadoutPaddles stab.calib.objects
ANTI2VPcal ANTI2VPcal::antivpcal[ANTI2C::MAXANTI];//mem.resrv. for ANTI-ReadoutPaddles variab.calib.objects
ANTIPeds ANTIPeds::anscped[ANTI2C::MAXANTI];//mem.reserv. for ANTI-ReadoutPaddles pedestals/sigmas
ANTIPedsMS ANTIPedsMS::anscped[ANTI2C::MAXANTI];//mem.reserv. for ANTI-ReadoutPaddles MC-seeds pedestals/sigmas
//
//======> just memory reservation for ANTI2DBc class variables:
// (real values are initialized at run-time from data cards in setgeom() or in...)
// 
  geant ANTI2DBc::_scradi=0.;
  geant ANTI2DBc::_scinth=0.;
  geant ANTI2DBc::_scleng=0.;
  geant ANTI2DBc::_wrapth=0.;
  geant ANTI2DBc::_groovr=0.;
  geant ANTI2DBc::_pdlgap=0.;
  geant ANTI2DBc::_stradi=0.;
  geant ANTI2DBc::_stleng=0.;
  geant ANTI2DBc::_stthic=0.;
//
  geant ANTI2DBc::_fadcbw=0.5; // Flash-ADC bin width(ns)
  geant ANTI2DBc::_htdcbw=0.0244;// bin width in Hist/FT-TDC (ns)
  integer ANTI2DBc::_daqscf=2; //daq scaling factor for charge(not used in new raw mode readout)
  geant ANTI2DBc::_ftcoinw=50.;//not used
  geant ANTI2DBc::_dopwmn=7.; // min.outPW of discr.(outPW=inpTovT-dipwmn when outPW>dipwmn)
  geant ANTI2DBc::_dipwmn=5.; // minimal inp.pulse width(TovT) to fire discr.(its "rise time")
  geant ANTI2DBc::_tgpwid=120.;// "Trig.patt" pulses width(Actel,ns) used for coinc.with FT(confirmed 30.04.2005)
  geant ANTI2DBc::_tgdtim=5.;  //dead time of "TrigPatt"-branch on ACTEL-output(going to SPT-inpts)
  geant ANTI2DBc::_didtim=11.;//input dead time of generic discr(min dist. of prev_down/next_up edges)[ns]
//                                     (i neglect by fall-time of generic discr, so DT=(11+5)ns for output signals !!!)
  geant ANTI2DBc::_tdcdtm=20.;//dead time of TDC-inputs, the same for LT-/FT-inputs[ns]
  geant ANTI2DBc::_daccur=0.02;//generic discr. internal accuracy[ns]
  geant ANTI2DBc::_pbgate=240.;//TgBox FT-gate width to latch "pattern" pulses
//
//  ANTI2DBc class member functions :
//
  geant ANTI2DBc::scradi(){return _scradi;}
  geant ANTI2DBc::scinth(){return _scinth;}
  geant ANTI2DBc::scleng(){return _scleng;}
  geant ANTI2DBc::wrapth(){return _wrapth;}
  geant ANTI2DBc::groovr(){return _groovr;}
  geant ANTI2DBc::pdlgap(){return _pdlgap;}
  geant ANTI2DBc::stradi(){return _stradi;};
  geant ANTI2DBc::stleng(){return _stleng;};
  geant ANTI2DBc::stthic(){return _stthic;}
  geant ANTI2DBc::fadcbw(){return _fadcbw;}
  geant ANTI2DBc::htdcbw(){return _htdcbw;}
  integer ANTI2DBc::daqscf(){return _daqscf;}
  geant ANTI2DBc::ftcoinw(){return _ftcoinw;}
  geant ANTI2DBc::dopwmn(){return _dopwmn;}
  geant ANTI2DBc::dipwmn(){return _dipwmn;}
  geant ANTI2DBc::tgpwid(){return _tgpwid;}
  geant ANTI2DBc::tgdtim(){return _tgdtim;}
  geant ANTI2DBc::didtim(){return _didtim;}
  geant ANTI2DBc::tdcdtm(){return _tdcdtm;}
  geant ANTI2DBc::daccur(){return _daccur;}
  geant ANTI2DBc::pbgate(){return _pbgate;}
//----
  void ANTI2DBc::setgeom(){ //get parameters from data cards (for now)
    geant ZShift(0);
    _scradi=ATGEFFKEY.scradi;
    _scinth=ATGEFFKEY.scinth;
    _scleng=ATGEFFKEY.scleng;
    _wrapth=ATGEFFKEY.wrapth;
    _groovr=ATGEFFKEY.groovr;
    _pdlgap=ATGEFFKEY.pdlgap;
    _stradi=ATGEFFKEY.stradi;
    _stleng=ATGEFFKEY.stleng;
    _stthic=ATGEFFKEY.stthic;
//
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02D")){
          cout <<" ANTIGeom-I-AMS02D setup selected."<<endl;
	  ZShift=AMSDBc::amsdext;
	  cout<<"      ZShift="<<ZShift<<endl;
    }
    else if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
          cout <<" ANTIGeom-I-AMS02 setup selected."<<endl;
    }
    else
    {
          cout <<"<---- ANTI2DBc::setgeom: UNKNOWN setup !!!!"<<endl;
    }
    _scleng+=2*ZShift;
    _stleng+=2*ZShift;
  }
//----
// function below creates PMT pulse shape array arr[] with binning flash-adc :
//  peak value = 1.(e.g. 1mV@50ohm, or 1pe if everything is in pe's)
void ANTI2DBc::inipulsh(integer & nbn, geant arr[])
{
//  integer tbins=20;
//  geant pmpsh[20]={0.,.08,.37,.74,.99, 1., .94,.84, .54, .34, .25,
//       .16,.14,.12,.08, .1, .12,.08, .04,  0.};// pulse heights at time points
//       .16,.14,.12,.09, .08, .06,.05, .03,  0.};// pulse heights at time points, changed by hends to remove afterpulse
//
//  geant pmpsb[20]={0.,2.5, 5., 7.5,10.,12.5,15.,17.5,22.5,27.5,32.5,
//        40.,45.,50.,55.,60.,67.5,77.5,85.,100.};//time points
// below i took SE pulse shape from TOF(because the same PMT); what is above is ANTI real(multi-pes) pulse shape
  integer tbins=12;
  geant pmpsh[12]={0.,0.132,0.85,1.,0.83,0.68,0.35,0.15,0.078,
       0.059,0.029,0.};// pulse heights at time points
  geant pmpsb[12]={0.,1.,2.25,3.5,4.75,6.,8.5,11.,13.5,16.,18.5,
                      21.}; // time points (min. step should be above fladctb)
//
  integer i,io,ib,lastiob(0);
  geant bl,bh,bol,boh,ao1,ao2,tgo,al,ah,tota;
  tota=0.;
  io=0;
  for(ib=0;ib<ANTI2C::ANFADC;ib++){
    bl=ib*ANTI2DBc::fadcbw();
    bh=bl+ANTI2DBc::fadcbw();
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
      if(ATMCFFKEY.mcprtf >1){
        tota=tota*ANTI2DBc::fadcbw()/50.;
	tota*=51;//set peak value as on Aahen-picture(51mV)just to compare integral
        printf("ANTI-PM pulse shape:nbins=% 3d, Integral(pC) = %4.2e\n",nbn,tota);
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
  cerr<<"ANTI2DBc-inipulsh-Fatal: PM-pulse length exeeds Flash-ADC range !?"<<'\n';
  cerr<<"                 nbn="<<nbn<<endl;
  exit(1);
}
//----
// function below creates PhElectrons dispersion function array arr[] with binning flash-adc :
//  Integral of interpolated distribution(arr[] sum) is 1
void ANTI2DBc::inipedisp(integer & nbn, geant arr[])
{
  integer tbins=24;
  geant pmpsh[24]={0., 224., 691., 880., 856., 692., 516., 370., 262., 183.,
                 125.,  90.,  70.,  61.,  50.,  38.,  23.,  14.,  7.3,  6.1,
		  5.5,  2.5,   1.,   0.
                                       };//diff.prob at given time point(not normalized)
  geant pmpsb[24]={0., .8, 1.6, 2.4, 3.2, 4., 4.8, 5.6, 6.4, 7.2,
                   8., 8.8, 9.6, 10.8, 12., 13.2, 14.4, 16., 17.6, 20.,
		   22.4, 24.8, 27.2, 29.
                                        };//time points
//  (min. step should be >= fadcbw)
  integer i,io,ib,lastiob(0);
  geant bl,bh,bol,boh,ao1,ao2,tgo,al,ah,tota;
  tota=0.;
  io=0;
  for(ib=0;ib<ANTI2C::ANFADC;ib++){
    bl=ib*ANTI2DBc::fadcbw();
    bh=bl+ANTI2DBc::fadcbw();
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
      for(i=0;i<nbn;i++)arr[i]/=tota;//normalization
      tota/=tota;
      if(ATMCFFKEY.mcprtf > 1){
        printf("ANTI:PEs time dispersion function:nbins=% 3d, Integral = %4.2e\n",nbn,tota);
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
  cerr<<"ANTI2DBc-inipedisp-Fatal: PE-disp distr.length exeeds Flash-ADC range !?"<<'\n';
  cerr<<"                 nbn="<<nbn<<endl;
  exit(1);
}
//----
// function to display PMT pulse (flash-ADC array arr[]) :
//
void ANTI2DBc::displ_a(char comm[], int id, int mf, const geant arr[]){
  integer i;
  geant tm,a(0.);
  geant tb,tbi;
  char name[80], buf[10];
  sprintf(buf, "%4d\n",id);
  strcpy(name,comm);
  strcat(name,buf);
  tb=geant(mf)*ANTI2DBc::fadcbw();
  tbi=ANTI2DBc::fadcbw();
  HBOOK1(2638,name,100,0.,100*tb,0.);
  for(i=1;i<=ANTI2C::ANFADC;i++){
    if(i%mf==0){
      a+=arr[i-1];
      tm=i*tbi-0.5*tb;
      HF1(2638,tm,a/geant(mf));
      a=0.;
    }
    else{
      a+=arr[i-1];
    }
  }
  HPRINT(2638);
  HDELET(2638);
  return ;
}
//======================================================================
// ANTI2SPcal class member functions:
//
void ANTI2SPcal::build(){ // fill array of objects with data
  integer i,j,k,ip,cnum,ibr,isd,endflab;
  geant hthr,hdthr[ANTI2C::MAXANTI]; // hist-discr threshold(p.e.)
  geant dqthr,daqthr[ANTI2C::MAXANTI];//daq-readout thresh(adc-ch)
  geant ftdl[2],ftdel[ANTI2C::MAXANTI][2];
  geant t0[2],tzer[ANTI2C::MAXANTI][2],a2p,adc2pe[ANTI2C::MAXANTI];
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
//------------------------------
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  strcpy(inum,"0123456789");
//
// ---> read cal.file-versions file :
//
  integer cfvn;
  cfvn=ATCAFFKEY.cfvers%1000;
  strcpy(name,"antiverlist");
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
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"ANTI2Pcal::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed verslist-file for reading
  if(!vlfile){
    cerr <<"ANTI2SPcal_build:: missing verslist-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
    vlfile >> rlvern[i];// second number - for real
  }
  vlfile.close();
//
//---------------------------------------------
//
//   --->  Read stable param(adc2pe/daqthr/hdthr/tzer/ftdel) calib. file:
//
 ctyp=1;
 strcpy(name,"antispf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" ANTI2SPcal_build: adc2pe/daqthr/hdthr/tzer/ftdel const for MC-events "<<endl;
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
 else                                       // for Real events
 {
   cout <<" ANTI2SPcal_build: adc2pe/daqthr/hdthr/tzer/ftdel for Real-events"<<endl;
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
 if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"ANTI2SPcal_build:Open file : "<<fname<<'\n';
 ifstream asfile(fname,ios::in); // open StableParam-file for reading
 if(!asfile){
   cerr <<"ANTI2SPcal_build: missing StableParams-file "<<fname<<endl;
   exit(1);
 }
//
 for(i=0;i<ANTI2C::MAXANTI;i++){
   asfile >> adc2pe[i];// read adc->pe factor for ReadoutPaddle(p.e/adc_ch)(side independ)
   asfile >> daqthr[i];// read DAQ-thresh for ReadoutPaddle(adc-ch)(side independ)
   asfile >> hdthr[i];// read history-discr thresh for ReadoutPaddle(p.e)(side independ)
   asfile >> tzer[i][0];// read PhysPaddle-1 time delay(ns, mainly due to clear fiber length)
   asfile >> tzer[i][1];// read PhysPaddle-2 time delay(ns, mainly due to clear fiber length)
   asfile >> ftdel[i][0];// FT-delay for ReadoutPaddle side-1 
   asfile >> ftdel[i][1];// FT-delay for ReadoutPaddle side-2
 }
//
 asfile >> endflab;//read endfile-label
//
 asfile.close();
//
 if(endflab==12345){
   cout<<"ANTI2SPcal_build: stable-params file is successfully read !"<<endl;
 }
 else{cout<<"ANTI2SPcal_build: ERROR(problems with stable-params file)"<<endl;
   exit(1);
 }
//---------------------------------------------
// create ANTI2SPcal objects:
//
    for(i=0;i<ANTI2C::MAXANTI;i++){
      a2p=adc2pe[i];//adc->pe conv.factor 
      dqthr=daqthr[i];//DAQ-readout-threshold (adc-ch)
      hthr=hdthr[i]; // History-discr threshold (pe)
      t0[0]=tzer[i][0];//ClfFiber-delay for PhysPaddle-1
      t0[1]=tzer[i][1];//ClfFiber-delay for PhysPaddle-2
      ftdl[0]=ftdel[i][0];//S1 true hist-hit delay wrt FT (not used now)
      ftdl[1]=ftdel[i][1];//S2
//
      antispcal[i]=ANTI2SPcal(i,a2p,dqthr,hthr,t0,ftdl);// create ANTI2SPcal object
    }
//
}
//==================================================================
//
// ANTI2VPcal class member functions:
//
void ANTI2VPcal::build(){ // fill array of objects with data
  integer i,j,k,ip,cnum,ibr,isd,endflab;
  integer sta1[2]={0,0}; // all  are alive as default
  integer sta2[2]={0,0}; // all  are alive as default
  integer status1[ANTI2C::MAXANTI][2],status2[ANTI2C::MAXANTI][2];
  geant m2p1[2],m2p2[2],mev2pe1[ANTI2C::MAXANTI][2],mev2pe2[ANTI2C::MAXANTI][2];
  geant attlen[ANTI2C::MAXANTI][2],atl[2];
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
//------------------------------
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  strcpy(inum,"0123456789");
//
// ---> read cal.file-versions file :
//
  integer cfvn;
  cfvn=ATCAFFKEY.cfvers%1000;
  strcpy(name,"antiverlist");
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
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"ANTI2VPcal::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed verslist-file for reading
  if(!vlfile){
    cerr <<"ANTI2VPcal_build:: missing verslist-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
    vlfile >> rlvern[i];// second number - for real
  }
  vlfile.close();
//
//---------------------------------------------
//
//
 ctyp=2;
 strcpy(name,"antivpf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" ANTI2VPcal_build: start read stat/mev2pe/attlen const for MC-events "<<endl;
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
 else                                       // for Real events
 {
   cout <<" ANTI2VPcal_build: start read stat/mev2pe/attlen for Real-events"<<endl;
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
 if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"ANTI2VPcal_build:Open file : "<<fname<<'\n';
 ifstream avfile(fname,ios::in); // open VariableParam-file for reading
 if(!avfile){
   cerr <<"ANTI2VPcal_build: missing VariableParams-file "<<fname<<endl;
   exit(1);
 }
//
 for(i=0;i<ANTI2C::MAXANTI;i++){
   avfile >> status1[i][0];// read Side-1 stat for PhysPaddle-1
   avfile >> mev2pe1[i][0];// read Side-1 PhysPaddle-1  mev2pe
   avfile >> status2[i][0];// read Side-1 stat for PhysPaddle-2
   avfile >> mev2pe2[i][0];// read Side-1 PhysPaddle-2  mev2pe
   avfile >> status1[i][1];// read Side-2 stat for PhysPaddle-1
   avfile >> mev2pe1[i][1];// read Side-2 PhysPaddle-1  mev2pe
   avfile >> status2[i][1];// read Side-2 stat for PhysPaddle-2
   avfile >> mev2pe2[i][1];// read Side-2 PhysPaddle-2  mev2pe
   avfile >> attlen[i][0];// read PhysPaddle-1 att.length 
   avfile >> attlen[i][1];// read PhysPaddle-2 att.length 
 }
 avfile >> endflab;//read endfile-label
//
 avfile.close();
//
 if(endflab==12345){
   cout<<"ANTI2VPcal_build: variab-params file is successfully read !"<<endl;
 }
 else{cout<<"ANTI2VPcal_build: ERROR(problems with variab-params file)"<<endl;
   exit(1);
 }
//
//----------------------------------------------------------------------
// create ANTI2VPcal objects:
//
    for(i=0;i<ANTI2C::MAXANTI;i++){
      sta1[0]=status1[i][0];// alive status from CalibOutput-file
      sta1[1]=status1[i][1];
      sta2[0]=status2[i][0];// alive status from CalibOutput-file
      sta2[1]=status2[i][1];
      m2p1[0]=mev2pe1[i][0];// S1 mev->pe conv.factor(incl.clfib.atten.,PM-gain),PhysPaddle-1
      m2p1[1]=mev2pe1[i][1];// S2 ......
      m2p2[0]=mev2pe2[i][0];// S1 mev->pe conv.factor(incl.clfib.atten.,PM-gain),PhysPaddle-2
      m2p2[1]=mev2pe2[i][1];// S2 ......
      atl[0]=attlen[i][0];//att.length for PhysPaddle-1
      atl[1]=attlen[i][1];//att.length for PhysPaddle-2
//
      antivpcal[i]=ANTI2VPcal(i,sta1,sta2,m2p1,m2p2,atl);// create ANTI2VPcal object
    }
//
}
//
  geant ANTI2VPcal::getmev2pec(int is){
    if(status1[is]==0 && status2[is]==0)return 0.5*(mev2pe1[is]+mev2pe2[is]);
    else if(status1[is]==0)return mev2pe1[is];
    else if(status2[is]==0)return mev2pe2[is];
    else return(-1);
  }
  geant ANTI2VPcal::getatlenc(){
    if((status1[0]==0 || status1[1]==0)
        && (status2[0]==0 || status2[1]==0))return 0.5*(attlen[0]+attlen[1]);
    else if(status1[0]==0 || status1[1]==0)return attlen[0];
    else if(status2[0]==0 || status2[1]==0)return attlen[1];
    else return(-1);
  }
  int ANTI2VPcal::NPhysSecOK(){//return 2 if BOTH phys.sectors OK, alive phys.sect#(0,1) if not
    if((status1[0]==0 || status1[1]==0) && (status2[0]==0 || status2[1]==0))return(2);
    else if(status1[0]==0 || status1[1]==0)return(0);
    else if(status2[0]==0 || status2[1]==0)return(1);
    else return(-1);
  }
//
//==========================================================================
//
void ANTIPeds::mcbuild(){// create MC ANTI-Peds-objects for each ReadoutPaddle
//
  int i,j,is;
  integer sid;
  char fname[80];
  char name[80];
  integer asta[ANTI2C::MAXANTI][2];// array of counter stat
  geant aped[ANTI2C::MAXANTI][2];// array of counter peds
  geant asig[ANTI2C::MAXANTI][2];// array of counter sigmas
  integer stata[2];
  geant peda[2],siga[2];
//
//
//   --->  Read  pedestals file :
//
  strcpy(name,"antp_df_mc");
  cout <<"====> ANTIPeds::mcbuild: Default Real_MC-peds file is used..."<<endl;
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<"      AMS02 setup selected."<<endl;
  }
  else
  {
        cout <<"      Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"      missing Default Real_MC-peds file !!!"<<fname<<endl;
    exit(1);
  }
//
//---> Read anode stats/peds/sigs:
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <---- loop over sectors
    for(i=0;i<2;i++)icfile >> asta[is][i];// sequence: side1,side2
    for(i=0;i<2;i++)icfile >> aped[is][i];
    for(i=0;i<2;i++)icfile >> asig[is][i];
  } // ---> endof sectors loop
//
//
  icfile.close();
//---------------------------------------------
//   ===> fill ANTIPeds bank :
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <--- loop over sectors
    sid=(is+1);
    for(i=0;i<2;i++){
      stata[i]=asta[is][i];
      peda[i]=aped[is][i];
      siga[i]=asig[is][i];
    }
//
    anscped[is]=ANTIPeds(sid,stata,peda,siga);
//
  } // ---> endof sector loop
  cout <<"<---- ANTIPeds::mcbuild: succsessfully done !"<<endl<<endl;
}
//==========================================================================
//
void ANTIPeds::build(){//tempor solution for RealData peds. 
//
  int i,j,is;
  integer sid;
  char fname[80];
  char name[80];
  integer asta[ANTI2C::MAXANTI][2];// array of counter stat
  geant aped[ANTI2C::MAXANTI][2];// array of counter peds
  geant asig[ANTI2C::MAXANTI][2];// array of counter sigmas
  integer stata[2];
  geant peda[2],siga[2];
//
//
//   --->  Read  pedestals file :
//
  strcpy(name,"antp_df_rl");
  cout <<"====> ANTIPeds::build: Default Real_Data-Peds file is used..."<<endl;
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<"      AMS02 setup selected."<<endl;
  }
  else
  {
        cout <<"      Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"      missing Default Real_Data-Peds file !!! "<<fname<<endl;
    exit(1);
  }
//
//---> Read anode stats/peds/sigs:
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <---- loop over sectors
    for(i=0;i<2;i++)icfile >> asta[is][i];// sequence: side1,side2
    for(i=0;i<2;i++)icfile >> aped[is][i];
    for(i=0;i<2;i++)icfile >> asig[is][i];
  } // ---> endof sectors loop
//
//
  icfile.close();
//---------------------------------------------
//   ===> fill ANTIPeds bank :
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <--- loop over sectors
    sid=(is+1);
    for(i=0;i<2;i++){
      stata[i]=asta[is][i];
      peda[i]=aped[is][i];
      siga[i]=asig[is][i];
    }
//
    anscped[is]=ANTIPeds(sid,stata,peda,siga);
//
  } // ---> endof sector loop
  cout <<"<---- ANTIPeds::build: succsessfully done !"<<endl<<endl;
}
//==========================================================================
//
void ANTIPedsMS::build(){//tempor solution for RealData peds. 
//
  int i,j,is;
  integer sid;
  char fname[80];
  char name[80];
  integer asta[ANTI2C::MAXANTI][2];// array of counter stat
  geant aped[ANTI2C::MAXANTI][2];// array of counter peds
  geant asig[ANTI2C::MAXANTI][2];// array of counter sigmas
  integer stata[2];
  geant peda[2],siga[2];
//
//
//   --->  Read  pedestals file :
//
  strcpy(name,"antp_df_sd");
  cout <<"====> ANTIPedsMS::build: Default Seed_MC-Peds file is used..."<<endl;
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<"      AMS02 setup selected."<<endl;
  }
  else
  {
        cout <<"      Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"      missing Default Seed_MC-Peds file !!! "<<fname<<endl;
    exit(1);
  }
//
//---> Read anode stats/peds/sigs:
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <---- loop over sectors
    for(i=0;i<2;i++)icfile >> asta[is][i];// sequence: side1,side2
    for(i=0;i<2;i++)icfile >> aped[is][i];
    for(i=0;i<2;i++)icfile >> asig[is][i];
  } // ---> endof sectors loop
//
//
  icfile.close();
//---------------------------------------------
//   ===> fill ANTIPeds bank :
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <--- loop over sectors
    sid=(is+1);
    for(i=0;i<2;i++){
      stata[i]=asta[is][i];
      peda[i]=aped[is][i];
      siga[i]=asig[is][i];
    }
//
    anscped[is]=ANTIPedsMS(sid,stata,peda,siga);
//
  } // ---> endof sector loop
  cout <<"<---- ANTIPedsMS::build: succsessfully done !"<<endl<<endl;
}
//=====================================================================  
//
//   ANTIJobStat static variables definition (memory reservation):
//
integer ANTI2JobStat::mccount[ANTI2C::ANJSTA];
integer ANTI2JobStat::recount[ANTI2C::ANJSTA];
integer ANTI2JobStat::chcount[ANTI2C::ANCHMX][ANTI2C::ANCSTA];
integer ANTI2JobStat::brcount[ANTI2C::MAXANTI][ANTI2C::ANCSTA];
//
// function to print Job-statistics at the end of JOB(RUN):
//
void ANTI2JobStat::printstat(){
  int il,ib,ic;
  geant rc;
  printf("\n");
//
  if(MISCFFKEY.dbwrbeg>0)return;//dbwriter job, don't need statistics
//
  printf("    ============ JOB ANTI2-statistics =============\n");
  printf("\n");
  printf(" MC: entries                           : % 6d\n",mccount[0]);
  printf(" MC: Have FastTrig to digitize anti    : % 6d\n",mccount[7]);
  printf(" MC: RawEvent created(err.report below): % 6d\n",mccount[6]);
  printf(" MC:     Flash-ADC buff.ovfl           : % 6d\n",mccount[1]);
  printf(" MC:     TimeHist-TDC stack ovfl       : % 6d\n",mccount[2]);
  printf(" MC:     TrigTime-TDC stack ovfl       : % 6d\n",mccount[8]);
  printf(" MC:     ADC range ovfl                : % 6d\n",mccount[3]);
  printf(" MC:     Hist-hits out of search window: % 6d\n",mccount[4]);
  printf(" MC: FT/Anti coincidence               : % 6d\n",mccount[5]);
  printf("--------\n");
  printf(" RECO-entries            : % 6d\n",recount[0]);
  printf(" Lev-1 trigger OK        : % 6d\n",recount[1]);
  printf(" Usage of TOF in LVL1    : % 6d\n",recount[5]);
  printf(" Usage of EC  in LVL1    : % 6d\n",recount[6]);
  printf(" RawEvent-validation OK  : % 6d\n",recount[2]);
  printf(" RawEvent->Cluster OK    : % 6d\n",recount[3]);
  printf("--------\n");
  printf(" AmplCalib entries      : % 6d\n",recount[11]);
  printf(" Nsectors OK             : % 6d\n",recount[12]);
  printf(" TRK-track found         : % 6d\n",recount[13]);
  printf(" One TRK-Sector crossing : % 6d\n",recount[14]);
  printf(" TOF/TRK Z=1             : % 6d\n",recount[15]);
  printf(" Good matching/impact    : % 6d\n",recount[16]);
  printf(" P/m in MIP range        : % 6d\n",recount[17]);
  printf(" Cross.Length OK         : % 6d\n",recount[18]);
  printf("--------\n");
  printf(" PedCalib entries      : % 6d\n",recount[19]);
  printf(" PedCalib-events found : % 6d\n",recount[20]);
  printf("\n\n");
//
  if(ATREFFKEY.reprtf[0]==0)return;
  printf("==========> Bars reconstruction report :\n\n");
//
  printf("Have signal on any side :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      printf(" % 6d",brcount[ib][0]);
    }
    printf("\n\n");
//
  printf("Have tot.signal > threshold :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      rc=geant(brcount[ib][0]);
      if(rc>0.)rc=geant(brcount[ib][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  printf("Good for calibration :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      rc=geant(brcount[ib][0]);
      if(rc>0.)rc=geant(brcount[ib][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
//
  printf("===========> Channels validation report :\n\n");
//
  printf("Validation entries :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      printf(" % 6d",chcount[ic][5]);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      printf(" % 6d",chcount[ic][5]);
    }
    printf("\n\n");
  printf("Missing FTtime info (percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
//
  printf("Multiple FTtime info (percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("Missing Time info (percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("Missing Ampl info (percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][9])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][9])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("============> Channels reconstruction report :\n\n");
//
  printf("DB-statusOK hit found :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n\n");
//
  printf("ADC-ch 'ON' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
  printf("ADC-ch 'AbovePed' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
  printf("TDC-ch 'ON' (wrt AbovePed):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][2]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][2]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
//
  printf("TDC-ch '1 hit' (wrt AbovePed):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][2]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][2]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
}
//======================================================================
//
void ANTI2JobStat::bookmch(){
//
  HBOOK1(2630,"ANTI-MC: Counters Etot (geant,Mev)",60,0.,30.,0.); 
  HBOOK1(2631,"ANTI-MC(lvl1): AbsGlobFTime-SideTrigPattSignalTime(ns)",80,-160.,160.,0.); 
  HBOOK1(2632,"ANTI-MC: DownSideEdep (pe,FillRawEvent)",80,0.,80.,0.); 
  HBOOK1(2633,"ANTI-MC: UpSideEdep (pe,FillRawEvent)",80,0.,80.,0.); 
  HBOOK1(2634,"ANTI-MC: DownSideEdep (ADCch-ped,FillRawEvent)",80,0.,80.,0.); 
  HBOOK1(2635,"ANTI-MC: UpSideEdep (ADCch-ped,FillRawEvent)",80,0.,80.,0.); 
  HBOOK1(2636,"ANTI-MC: TimeHistory hits per side ",16,0.,16.,0.); 
  HBOOK1(2637,"ANTI-MC: NumbOfGeant-hits per event ",80,0.,320.,0.);
  HBOOK1(2639,"ANTI-MC: LogSector(1+8) GeantEloss(mev)",100,0.,10.,0.);
  HBOOK1(2640,"ANTI-MC: FTchannel PulseWidth",90,0.,450.,0.);
  
//  HBOOK1(2638,... reserved for PM-pulse  
}
void ANTI2JobStat::bookh(){
//
  HBOOK1(2520,"ANTI-VAL:HistTime-FTtime(all chann)",80,-40.,40.,0.);
//
  HBOOK1(2500,"ANTI-REC: EtotSectors(Mev,Esect>thr)",80,0.,40.,0.);
  HBOOK1(2501,"ANTI-REC: NumbOfSectors(Esect>thr)",16,0.,16.,0.);
  HBOOK1(2512,"ANTI-REC: NumbOfSectors(Esect>thr, + FT-coinc)",16,0.,16.,0.);
  HBOOK1(2502,"ANTI-REC: Total Time-hits per sector",16,0.,16.,0.);
  HBOOK1(2503,"ANTI-REC: MadeOfPair-TimeHits per sector",16,0.,16.,0.);
  HBOOK1(2504,"ANTI-REC: Sector Z-coo(cm,1Pair)",60,-60.,60.,0.);
  HBOOK1(2505,"ANTI-REC: Sector Z-coo(cm, 2sided but pairs!=1)",60,-60.,60.,0.);
  HBOOK1(2506,"ANTI-REC: Sector appearance frequency(Esect>Thr)",16,1.,17.,0.);
  HBOOK1(2507,"ANTI-REC: NumbOfPairedSectors(Esect>thr)",16,0.,16.,0.);
  HBOOK1(2508,"ANTI-REC: Edep per sector(mev,2sided)",80,0.,20.,0.);
  HBOOK1(2509,"ANTI-REC: FTtime-SectorSideTime",80,0.,240.,0.);
  HBOOK1(2510,"ANTI-REC: Total FTCoincSectors(fromTrig)",10,0.,10.,0.);
  HBOOK1(2511,"ANTI-REC: FTCoincSector frequency(0 when patt=0)",10,1.,11.,0.);
  if(ATREFFKEY.relogic==1){//book calib.hist
    HBOOK1(2530,"AntiCalib:Nfired/Nmatched sectors",20,0.,20.,0.);//spare
    HBOOK1(2531,"AntiCalib:Cyl-track Zcross(noCuts,both dirs)",75,-75.,75.,0.);
    HBOOK1(2532,"AntiCalib:PHIsect-PHIcros(Zcross OK)",91,-182.,182.,0.);
    HBOOK1(2533,"AntiCalib:PHIcros-PHIimp(Zcross OK)",91,-182.,182.,0.);
    HBOOK1(2534,"AntiCalib:PartMomentum(gev)",100,0.,50.,0.);
    HBOOK1(2535,"AntiCalib:PartBeta",50,-1.,1.,0.);
    HBOOK1(2536,"AntiCalib:PartMass(gev)",80,0.,8.,0.);
    HBOOK1(2537,"AntiCalib:Part mom/mass",80,0.,20.,0.);
    HBOOK1(2538,"AntiCalib:PartPassLength in scint",80,0.,20.,0.);
    HBOOK2(2625,"AntiCalib:Ampl1(trlen/beta-norm) vs bet",50,0.2,1.2,50,0.,50.,0.);
    HBOOK1(2539,"AntiCalib:Matched Physical sector number",16,1.,17.,0.);
    HBOOK1(2540,"AntiCalib:TRK-track CrossPointPhi(Zcr OK)",91,0.,364.,0.);
    HBOOK1(2541,"AntiCalib:TRK-track ImpactPhi(Zcr OK)",91,0.,364.,0.);
    
    HBOOK1(2542,"AntiCalib:PhysSectorAttLength",80,0.,320.,0.);
    HBOOK1(2543,"AntiCalib:PhysSectE2PE(s1)",80,0.,40.,0.);
    HBOOK1(2544,"AntiCalib:PhysSectE2PE(s2)",80,0.,40.,0.);
    
    HBOOK1(2545,"AntiCalib:Cyl-track Zcross(sector-1)",84,-42.,42.,0.);
    HBOOK1(2546,"AntiCalib:Cyl-track Zcross(sector-2)",84,-42.,42.,0.);
    HBOOK1(2547,"AntiCalib:Cyl-track Zcross(sector-3)",84,-42.,42.,0.);
    HBOOK1(2548,"AntiCalib:Cyl-track Zcross(sector-4)",84,-42.,42.,0.);
    HBOOK1(2549,"AntiCalib:Cyl-track Zcross(sector-5)",84,-42.,42.,0.);
    HBOOK1(2550,"AntiCalib:Cyl-track Zcross(sector-6)",84,-42.,42.,0.);
    HBOOK1(2551,"AntiCalib:Cyl-track Zcross(sector-7)",84,-42.,42.,0.);
    HBOOK1(2552,"AntiCalib:Cyl-track Zcross(sector-8)",84,-42.,42.,0.);
    HBOOK1(2553,"AntiCalib:Cyl-track Zcross(sector-9)",84,-42.,42.,0.);
    HBOOK1(2554,"AntiCalib:Cyl-track Zcross(sector-10)",84,-42.,42.,0.);
    HBOOK1(2555,"AntiCalib:Cyl-track Zcross(sector-11)",84,-42.,42.,0.);
    HBOOK1(2556,"AntiCalib:Cyl-track Zcross(sector-12)",84,-42.,42.,0.);
    HBOOK1(2557,"AntiCalib:Cyl-track Zcross(sector-13)",84,-42.,42.,0.);
    HBOOK1(2558,"AntiCalib:Cyl-track Zcross(sector-14)",84,-42.,42.,0.);
    HBOOK1(2559,"AntiCalib:Cyl-track Zcross(sector-15)",84,-42.,42.,0.);
    HBOOK1(2560,"AntiCalib:Cyl-track Zcross(sector-16)",84,-42.,42.,0.);
    HBOOK1(2626,"FiredLogSectNumb(zcrOK,dir=-1)",8,0.,8.,0.);
    HBOOK1(2627,"FiredLogSectNumb(zcrOK,dir=1)",8,0.,8.,0.);
    HBOOK1(2628,"FiredLogSectNumb(zcrOK,phi-mat,dir=-1)",8,0.,8.,0.);
    HBOOK1(2629,"FiredLogSectNumb(zcrOK,phi-mat,dir=1)",8,0.,8.,0.);
  }
//   Hist.2570-2599 reserved for PedCalib needs in ANTPedCalib::init()
}
void ANTI2JobStat::outpmc(){
//
  HPRINT(2639);
  HPRINT(2630);
  HPRINT(2631);
  HPRINT(2632);
  HPRINT(2633);
  HPRINT(2634);
  HPRINT(2635);
  HPRINT(2636);
  HPRINT(2637);
  HPRINT(2640);
}
//
void ANTI2JobStat::outp(){
//
  if(ATREFFKEY.reprtf[0]>0){
    HPRINT(2500);
    HPRINT(2501);
    HPRINT(2512);
    HPRINT(2507);
    HPRINT(2502);
    HPRINT(2503);
    HPRINT(2504);
    HPRINT(2505);
    HPRINT(2506);
    HPRINT(2508);
    HPRINT(2509);
    HPRINT(2510);
    HPRINT(2511);
  }
  if(ATREFFKEY.relogic==1){
    HPRINT(2626);
    HPRINT(2627);
    HPRINT(2628);
    HPRINT(2629);
    HPRINT(2530);
    HPRINT(2531);
    HPRINT(2540);
    HPRINT(2541);
    HPRINT(2532);
    HPRINT(2533);
    HPRINT(2534);
    HPRINT(2535);
    HPRINT(2536);
    HPRINT(2537);
    HPRINT(2538);
    HPRINT(2625);
    HPRINT(2539);
    HPRINT(2545);
    HPRINT(2546);
    HPRINT(2547);
    HPRINT(2548);
    HPRINT(2549);
    HPRINT(2550);
    HPRINT(2551);
    HPRINT(2552);
    HPRINT(2553);
    HPRINT(2554);
    HPRINT(2555);
    HPRINT(2556);
    HPRINT(2557);
    HPRINT(2558);
    HPRINT(2559);
    HPRINT(2560);
//
    AntiCalib::fit();
  }
}
//======================================================================
