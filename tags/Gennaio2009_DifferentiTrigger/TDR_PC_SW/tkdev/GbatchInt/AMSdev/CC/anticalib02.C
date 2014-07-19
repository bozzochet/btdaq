//        ANTI-calib program, v1.0, 15.10.2003, E.Choumilov
#include "tofdbc02.h"
#include "tofsim02.h"
#include "point.h"
#include "typedefs.h"
#include "event.h"
#include "amsgobj.h"
#include "commons.h"
#include "cern.h"
#include "mccluster.h"
#include <math.h>
#include "extC.h"
#include "antidbc02.h"
#include "trrec.h"
#include "antirec02.h"
#include "anticalib02.h"
#include "particle.h"
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <time.h>
#include "timeid.h"
using namespace std;
//
//
//=============================================================================
//
  number AntiCalib::ambinl[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.adcs in low_bin(event-by-event)
  integer AntiCalib::nevbl[2*ANTI2C::ANTISRS];//total events, accum. in low_bin
  number AntiCalib::xcol[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.xcoords in low_bin(event-by-event)
  number AntiCalib::ambinh[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.adcs in high_bin(event-by-event)
  integer AntiCalib::nevbh[2*ANTI2C::ANTISRS];//total events, accum. in high_bin
  number AntiCalib::xcoh[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.xcoords in high_bin(event-by-event)
//
//--------------------------
void AntiCalib::init(){ // ----> initialization for AMPL-calibration 
  integer i,j,il,ib,ii,jj,id,nadd,nbnr,chan,is;
  geant blen,dd,bw,bl,bh,hll,hhl;
  char htit1[60];
  char htit2[60];
  char htit3[7];
  char inum[11];
  char in[2]="0";
  strcpy(inum,"0123456789");
//
  for(i=0;i<2*ANTI2C::ANTISRS;i++){
    nevbl[i]=0;
    nevbh[i]=0;
    for(j=0;j<ANTI2C::ANACMX;j++){
      ambinl[i][j]=0.;
      ambinh[i][j]=0.;
      xcol[i][j]=0.;
      xcoh[i][j]=0.;
    }
  }
//
//  ---> book hist. for side-signals:
//
  chan=0;
  for(int ilh=0;ilh<2;ilh++){//low/high bin loop
    if(ilh==0)strcpy(htit1,"LowBin signals for chan(BBS) ");
    if(ilh==1)strcpy(htit1,"HighBin signals for chan(BBS) ");
    for(i=0;i<2;i++){//side loop
      if(i==0){//s=1
        if(ilh==0){//close
          hll=0.;
          hhl=160.;
	}
	else{//farther
          hll=0.;
          hhl=80.;
	}
      }
      else{//s=2
        if(ilh==0){//farther
          hll=0.;
          hhl=80.;
	}
	else{//close
          hll=0.;
          hhl=160.;
	}
      }
      for(ib=0;ib<ANTI2C::ANTISRS;ib++){//phys.sect.loop
	strcpy(htit2,htit1);
        ii=(ib+1)/10;
        jj=(ib+1)%10;
        in[0]=inum[ii];
        strcat(htit2,in);
        in[0]=inum[jj];
        strcat(htit2,in);
        in[0]=inum[i+1];
        strcat(htit2,in);
        id=2561+chan;
//	cout<<"              chan/id="<<chan<<" "<<id<<endl;
//	cout<<htit2<<endl;
        HBOOK1(id,htit2,80,hll,hhl,0.);
        chan+=1;
      }
    }
  }
//
}
//--------------------------------------
//
void AntiCalib::select(){ // ------> event selection for AMPL-calibration
  bool anchok;
  integer ntdct,tdct[ANTI2C::ANTHMX],nftdc,ftdc[TOF2GC::SCTHMX1];
  geant adca;
  int16u id,idN,sta;
  number ampe[2],uptm[2];
  number am1[ANTI2C::MAXANTI],am2[ANTI2C::MAXANTI];
  integer frsecn[ANTI2C::MAXANTI],frsect;
  integer i,j,jmax,sector,isid,nsds,stat,chnum,n1,n2,i1min,i2min;
  integer status(0);
  uinteger Runum(0);
  geant ftdel[2],padlen,padrad,padth,padfi,paddfi,ped,sig,tzer;
  int nphsok;
  Anti2RawEvent *ptr;
  Anti2RawEvent *ptrN;
//
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                  ->getheadC("Anti2RawEvent",0);// already sorted after validation
//----
  ANTI2JobStat::addre(11);
  Runum=AMSEvent::gethead()->getrun();// current run number
  padlen=ANTI2DBc::scleng();//z-size
  padrad=ANTI2DBc::scradi();//int radious
  padth=ANTI2DBc::scinth();//thickness
  paddfi=360./ANTI2C::MAXANTI;//per logical sector
  frsect=0;
  nsds=0;
  ntdct=0;
  uptm[0]=0;
  uptm[1]=0;
  ampe[0]=0;
  ampe[1]=0;
  while(ptr){ // <------ RawEvent hits loop
    id=ptr->getid();//BBS
    sector=id/10-1;//Readout(logical) sector number (0-7)
    isid=id%10-1;
    chnum=sector*2+isid;//channels numbering
    adca=ptr->getadca();
    ntdct=ptr->gettdct(tdct);
    nftdc=ptr->getftdc(ftdc);
    if(ntdct==1 && nftdc==1){//select only 1 Hist/FT-hit events
//DAQ-ch-->p.e's:
      ped=ANTIPeds::anscped[sector].apeda(isid);//adc-chan
      sig=ANTIPeds::anscped[sector].asiga(isid);//adc-ch sigmas
      ampe[nsds]=number(adca)/ANTI2DBc::daqscf()       //DAQ-ch-->ADC-ch
              *ANTI2SPcal::antispcal[sector].getadc2pe(); //ADC-ch-->p.e.
//cout<<"    decoded signal="<<ampe[nsds]<<endl; 
//TDC-ch-->time(ns):
      i=1;//use up-edge(come after down)
      nphsok=ANTI2VPcal::antivpcal[sector].NPhysSecOK();
      if(nphsok==2)tzer=ANTI2SPcal::antispcal[sector].gettzerc();
      else tzer=ANTI2SPcal::antispcal[sector].gettzer(nphsok);
      uptm[nsds]=(ftdc[0]-tdct[0])*ANTI2DBc::htdcbw() + tzer;//TDC-ch-->ns + compens.tzero
//cout<<"    decoded Up-time="<<uptm[nsds]<<endl;
//
      nsds+=1;
    }//--->endof 1-hit check
//--------
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getid())/10; //next hit short_id (BBS)
//
    if(idN != id/10){//next hit is new sector: create 2-sides signal for current sect
//
      if(nsds==2 && (ampe[0]>0 && ampe[1]>0) && (uptm[0]>0 && uptm[1]>0)){//good sector ?
         ANTI2JobStat::addbr(sector,2);//count good sect
	 frsecn[frsect]=sector;//store fired sect.number
	 am1[frsect]=ampe[0];//store signals(pe's)
	 am2[frsect]=ampe[1];
	 frsect+=1;//counts fired good sectors
      }//--->endof good sector check
//
      nsds=0;
      ntdct=0;
      ampe[0]=0;
      ampe[1]=0;
      uptm[0]=0;
      uptm[1]=0;
    }//--->endof next sector check
//---
    ptr=ptr->next();// take next RawEvent hit
  }//------>endof RawEvent hits loop
//
  HF1(2530,geant(frsect),1.);
  if(frsect!=1)return;//remove empty/mult sector events
  ANTI2JobStat::addre(12);
//
  padfi=paddfi*(0.5+frsecn[0]);//fired logical-paddle phi
//
//
//------> get parameters from tracker:
//
    number pmas;
    number pmom,mom,bet,chi2,betm,pcut[2];
    number the,phi,trl,rid,err,ctran;
    integer chargeTOF,chargeTracker,trpatt;
    AMSPoint C0(0,0,0);
    AMSPoint cooCyl(0,0,0);
    AMSPoint crcCyl;
    AMSDir dirCyl(0,0,1.);
    AMSContainer *cptr;
    AMSParticle *ppart;
    AMSTrTrack *ptrack;
    AMSCharge  *pcharge;
    AMSBeta *pbeta;
    int npart,ipatt,bad;
    npart=0;
    cptr=AMSEvent::gethead()->getC("AMSParticle",0);// get pointer to part-envelop
    if(cptr)
           npart+=cptr->getnelem();
    if(npart<1)return;// require events with 1 particle at least
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    bad=1;
    while(ppart){
      ptrack=ppart->getptrack();//get pointer of the TRK-track, used in given particle
      if(ptrack){
        trpatt=ptrack->getpattern();//TRK-track pattern
	if(trpatt>=0){
          ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
          status=ptrack->getstatus();
          pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
//        pbeta=pcharge->getpbeta();
          pbeta=ppart->getpbeta();
//          betpatt=pbeta->getpattern();
          bet=pbeta->getbeta();
//          chi2t=pbeta->getchi2();
//          chi2s=pbeta->getchi2S();
          chargeTracker=pcharge->getchargeTracker();
          chargeTOF=pcharge->getchargeTOF();
          pmas=ppart->getmass();
	  bad=0;
	}
      }
      ppart=ppart->next();
    } 
//
    if(bad)return;//require part. with TRK-track
    ANTI2JobStat::addre(13);
    pmom=fabs(rid);
//
// ----->  find track crossing points/angles with sectors:
//
    number dir,cpsn,cpcs,cphi,dphi1,dphi2,phil,phih,zcut,zscr,thes,phis;
    integer nseccr(0),isphys(-1);
    phil=padfi-0.5*paddfi;//boundaries of fired sector
    phih=padfi+0.5*paddfi;
    zcut=padlen/2+1;
    for(i=0;i<2;i++){
      dir=2*i-1;
      ptrack->interpolateCyl(cooCyl,dirCyl,padrad,dir,crcCyl,the,phi,trl);
// cout<<"i/dir="<<i<<" "<<dir<<" cr="<<padrad<<" th/ph/tr="<<the<<" "<<phi<<" "<<trl<<endl;
// cout<<"cooCyl="<<cooCyl[0]<<" "<<cooCyl[1]<<" "<<cooCyl[2]<<endl; 
// cout<<"dirCyl="<<dirCyl[0]<<" "<<dirCyl[1]<<" "<<dirCyl[2]<<endl; 
// cout<<"crcCyl="<<crcCyl[0]<<" "<<crcCyl[1]<<" "<<crcCyl[2]<<endl; 
      cpsn=crcCyl[1]/sqrt(crcCyl[0]*crcCyl[0]+crcCyl[1]*crcCyl[1]);//sin(phi) of cr.point
      cpcs=crcCyl[0]/sqrt(crcCyl[0]*crcCyl[0]+crcCyl[1]*crcCyl[1]);//cos(phi) of cr.point
      cphi=atan2(cpsn,cpcs)*AMSDBc::raddeg;//phi of cr.point(degr,+-180)
      if(cphi<0)cphi=360+cphi;//0-360
      phi=phi*AMSDBc::raddeg;//inpact phi(degr,+-180)
      if(phi<0)phi=360+phi;//0-360
      HF1(2531,geant(crcCyl[2]),1.);
      if(fabs(crcCyl[2])<zcut){//match in Z (+1cm extention due to accuracy)
        if(i==0)HF1(2626,geant(frsecn[0]),1.);
	else HF1(2627,geant(frsecn[0]),1.);
        HF1(2540,geant(cphi),1.);
        HF1(2541,geant(phi),1.);
        dphi1=padfi-cphi;//PHIsect-PHIcrp
	if(dphi1>180)dphi1=360-dphi1;
	if(dphi1<-180)dphi1=-(360+dphi1);
        HF1(2532,geant(dphi1),1.);//PHIsect-PHIcrp
        if(cphi<(phih-1) && cphi>(phil+1)){//match in phi (-1degr for safety)
          if(i==0)HF1(2628,geant(frsecn[0]),1.);
	  else HF1(2629,geant(frsecn[0]),1.);
	  if(cphi<padfi)isphys=2*frsecn[0];//physical sector number
	  else isphys=2*frsecn[0]+1;
	  zscr=crcCyl[2];
	  thes=the;
	  phis=phi;
          nseccr+=1;
	  dphi2=cphi-phi;//PHIcrp-PHIimpp
	  if(dphi2>180)dphi2=360-dphi2;
	  if(dphi2<-180)dphi2=-(360+dphi2);
          HF1(2533,geant(dphi2),1.);//PHIcrp-PHIimpp
        }//phi-match ok?
      }//z-match ok?
    }//endof dir loop
//
    HF1(2530,geant(nseccr+10),1.);
    if(nseccr!=1)return;//requir. 1-sect. crossing
    HF1(2539,geant(isphys+1),1.);//phys.sector number
    ANTI2JobStat::addre(14);
//
    if(abs(chargeTOF)!=1 || abs(chargeTracker)!=1)return;//requir. Z=1
    ANTI2JobStat::addre(15);
//
    if(fabs(dphi1)>26)return;//out of logic.sector
    if(fabs(dphi2)>70)return;//too high impact phi
    ANTI2JobStat::addre(16);
//    
    HF1(2534,geant(pmom),1.);
    HF1(2535,bet,1.);
    HF1(2536,pmas,1.);
    number betg(4);
    betg=pmom/pmas;//use particle-defined betg
    HF1(2537,betg,1.);
    if(betg>6)return;//non MIP area
    ANTI2JobStat::addre(17);
//
// ------> normalize Edep(p.e) to normal 1cm incidence :
//
    geant ama[2],cinp,crlen,betnor;
    dphi2/=AMSDBc::raddeg;//degr->rad
    crlen=padth/(sin(thes)*fabs(cos(dphi2)));//pass-length in scint(ignore local curvature)
    HF1(2538,crlen,1.);
    if(crlen>5.5)return;//too high cr.length: suspicious
    ANTI2JobStat::addre(18);
    ama[0]=am1[0];//imply single sector event
    ama[1]=am2[0];
    ama[0]*=(sin(thes)*fabs(cos(dphi2))/padth);//normalize to 1cm norm.inc(ignore loc.curv.)
    ama[1]*=(sin(thes)*fabs(cos(dphi2))/padth);//...                   
    cinp=zscr;
//
// ------> normalize Edep(p.e) to mip :
//
    if(fabs(bet)<0.97)betnor=pow(fabs(bet),5./3)/pow(0.97,5./3);//norm. to MIP
    else betnor=1;
    ama[0]*=betnor;
    ama[1]*=betnor;
    if(isphys==6 || isphys==9)HF2(2625,geant(bet),geant(ama[0]),1.);//sect-7
//
// ------> fill working arrays and histograms :
//
//   cout<<"Fill:sect/ama/x="<<isphys<<" "<<ama[0]<<" "<<ama[1]<<" "<<cinp<<endl;
    AntiCalib::fill(isphys,ama,cinp);//for relat.gains,profile,abs.norm. 
}
//--------------------------------------
//   ---> program to accumulate data for relat.gains/prof/abs.nor-calibration:
void AntiCalib::fill(integer isec, geant am[2], geant coo){
//
  integer i,j,chan,iblh(-1),idh,nev,isd;
  geant ramt,r,padlen;
  geant bl,bh;
  geant cll,chl;//bin limits
//
  cll=25;
  chl=41.5;//- (4-5)cm for real data to avoid edge-effects 
//
  padlen=ANTI2DBc::scleng();//z-size
  HF1(2545+isec,coo,1.);//phys.sect longit.imp.point distribution
//
  if(fabs(coo)>=cll && fabs(coo)<=chl){
    if(coo<0)iblh=0;//low(-z)/high(+z)-bin flag
    else iblh=1;
  }
  else return;//out of needed range
//
  for(isd=0;isd<2;isd++){
    idh=2*ANTI2C::ANTISRS*iblh+ANTI2C::ANTISRS*isd+isec;
    chan=ANTI2C::ANTISRS*isd+isec;
    if(iblh==0){
      if(nevbl[chan]<ANTI2C::ANACMX){
        ambinl[chan][nevbl[chan]]=am[isd];
	xcol[chan][nevbl[chan]]=coo;
	nevbl[chan]+=1;
      }  
    }
    if(iblh==1){
      if(nevbh[chan]<ANTI2C::ANACMX){
        ambinh[chan][nevbh[chan]]=am[isd];
	xcoh[chan][nevbh[chan]]=coo;
	nevbh[chan]+=1;
      }  
    }
    HF1(2561+idh,geant(am[isd]),1.);
  }
//
}
//--------------------------------------
//            ---> fit-program to get final Anti-calibration consts:
void AntiCalib::fit(){
//
  integer i,j,k,chan,isd,idh,iblh,isec,nev,nmin,nmax,lsec;
  int ndef(0);
//  geant elos(1.4/2);//m.p. eloss/cm, norm.inc., per side
  geant elos(0.98/2);//m.p. eloss, norm.inc., per side
  geant avxl(0),avxh(0);
  geant e2pel[2],e2peh[2],aval[2],avah[2],atl[2],aver,att,e2pe[2],adc2pe;
  geant ed2pe[ANTI2C::ANTISRS][2],atlen[ANTI2C::ANTISRS],atldef(0);
  number *pntr[ANTI2C::ANACMX];
  geant padl=ANTI2DBc::scleng();//sector length
//------
  for(isec=0;isec<ANTI2C::ANTISRS;isec++){//clear outp.arrays
    atlen[isec]=0;
    for(isd=0;isd<2;isd++){
      ed2pe[isec][isd]=0;   
    }
  }
//
  for(isec=0;isec<ANTI2C::ANTISRS;isec++){//<--- PhysSector loop
    cout<<"===> AntiCalibFit:PhysSector="<<isec+1<<endl;
    lsec=integer(floor(number(isec)/2.));
    adc2pe=ANTI2SPcal::antispcal[lsec].getadc2pe();
    for(isd=0;isd<2;isd++){
      cout<<"    ===> side="<<isd+1<<endl;
      atl[isd]=0;
      e2pe[isd]=0;
      for(iblh=0;iblh<2;iblh++){
        idh=2561+2*ANTI2C::ANTISRS*iblh+ANTI2C::ANTISRS*isd+isec;
        chan=ANTI2C::ANTISRS*isd+isec;
	cout<<"       ===>bin="<<iblh<<endl;
        HPRINT(idh);
	if(iblh==0){
	  nev=nevbl[chan];
	  aval[isd]=0;
	  avxl=0;
	  if(nev>=3){
	    for(i=0;i<nev;i++){
	      avxl+=xcol[chan][i];
//	      aval[isd]+=ambinl[chan][i];
	    }
	    avxl/=geant(nev);
//	    aval[isd]/=geant(nev);
            for(k=0;k<nev;k++)pntr[k]=&ambinl[chan][k];//pointers to event-signals of chan=i 
            AMSsortNAG(pntr,nev);//sort in increasing order
            nmax=integer(floor(nev*0.98));// to keep 98% of lowest amplitudes
            nmin=integer(floor(nev*0.02));// to remove 2 % of lowest amplitudes
//            if(nmin==0)nmin=1; 
	    aver=0;
            for(j=nmin;j<nmax;j++)aver+=(*pntr[j]);//lowest event is not used
            if((nmax-nmin)>0)aval[isd]=geant(aver/(nmax-nmin));
	  }
	  cout<<"         evnts="<<nev<<" averx="<<avxl<<" averal="<<aval[isd]<<endl;
	}
	else{
	  nev=nevbh[chan];
	  avah[isd]=0;
	  avxh=0;
	  if(nev>=3){
	    for(i=0;i<nev;i++){
	      avxh+=xcoh[chan][i];
//	      avah[isd]+=ambinh[chan][i];
	    }
	    avxh/=geant(nev);
//	    avah[isd]/=geant(nev);
            for(k=0;k<nev;k++)pntr[k]=&ambinh[chan][k];//pointers to event-signals of chan=i 
            AMSsortNAG(pntr,nev);//sort in increasing order
            nmax=integer(floor(nev*0.97));// to keep 90% of lowest amplitudes
            nmin=integer(floor(nev*0.02));// to remove 2 % of lowest amplitudes
            if(nmin==0)nmin=1; 
	    aver=0;
            for(j=nmin;j<nmax;j++)aver+=(*pntr[j]);//lowest event is not used
            if((nmax-nmin)>0)avah[isd]=geant(aver/(nmax-nmin));
	  }
	  cout<<"         evnts="<<nev<<" averx="<<avxh<<" averah="<<avah[isd]<<endl;
	}
      }//--->endof iblh-loop
      if(aval[isd]>0 && avah[isd]>0){
        if(isd==0)att=aval[isd]/avah[isd];
	else att=avah[isd]/aval[isd];
	atl[isd]=(avxh-avxl)/log(att);
//
      }
//
    }//--->endof isd-loop
    att=0;
    if(atl[0]>0 && atl[1]>0)att=(atl[0]+atl[1])/2;//use average, if possible
    else if(atl[0]>0)att=atl[0];
    else if(atl[1]>0)att=atl[1];
    else att=0;
    if(att>0){  
      atlen[isec]=att;
      atldef+=att;
      ndef+=1;
      cout<<"   atl(s1/s2)="<<atl[0]<<" "<<atl[1]<<" aver="<<att<<endl;
      HF1(2542,att,1.);
      if(atl[0]>0){//s1
        e2pel[0]=aval[0]*adc2pe/elos/exp(-avxl/att);
        e2peh[0]=avah[0]*adc2pe/elos/exp(-avxh/att);
        e2pe[0]=(e2pel[0]+e2peh[0])/2;
        ed2pe[isec][0]=e2pe[0];
        cout<<"   S1:e2pel/h="<<e2pel[0]<<" "<<e2peh[0]<<" aver="<<e2pe[0]<<endl;
        HF1(2543,e2pe[0],1.);
      }	
      if(atl[1]>0){//s2
        e2pel[1]=aval[1]*adc2pe/elos/exp(avxl/att);
        e2peh[1]=avah[1]*adc2pe/elos/exp(avxh/att);
        e2pe[1]=(e2pel[1]+e2peh[1])/2;
        ed2pe[isec][1]=e2pe[1];
        cout<<"   S2:e2pel/h="<<e2pel[1]<<" "<<e2peh[1]<<" aver="<<e2pe[1]<<endl;
        HF1(2544,e2pe[1],1.);
      }	
    }  
//
  }//--->endof PhysSector-loop
//
  atldef/=geant(ndef);
  cout<<" Aver.AttLen="<<atldef<<endl;  
  HPRINT(2542);
  HPRINT(2543);
  HPRINT(2544);
//  
// ----------------> create/write ReadoutSectors arrays:
//
  integer phsec,status1[ANTI2C::MAXANTI][2],status2[ANTI2C::MAXANTI][2];
  geant mev2pe1[ANTI2C::MAXANTI][2],mev2pe2[ANTI2C::MAXANTI][2];
  geant attlen[ANTI2C::MAXANTI][2],attlendef;
  bool p1s1ok,p2s1ok,p1s2ok,p2s2ok;
//
  char fname[80];
  char frdate[30];
  char in[2]="0";
  char inum[11];
  char vers1[3]="mc";
  char vers2[3]="rl";
  integer cfvn;
  uinteger StartRun;
  time_t StartTime;
  int dig;
//
//--> create name for output file
// 
  strcpy(inum,"0123456789");
  cfvn=ATCAFFKEY.cfvers%1000;
  strcpy(fname,"/f2users/choumilo/antivpf");
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".cof");
//
//--> get run/time of the first event
//
  StartRun=TOF2RawSide::getsrun();
  StartTime=TOF2RawSide::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"AntiCalib:error opening file for output  "<<fname<<'\n';
    exit(8);
  }
  cout<<"AntiCalib: Open file for output, fname:  "<<fname<<'\n';
//
//  
  for(isec=0;isec<ANTI2C::MAXANTI;isec++){//<--- ReadoutSector loop    
    status1[isec][0]=1;//dead
    status1[isec][1]=1;//dead
    status2[isec][0]=1;//dead
    status2[isec][1]=1;//dead
    attlen[isec][0]=0;//physpad1
    attlen[isec][1]=0;//physpad2
    mev2pe1[isec][0]=0;//physpad1
    mev2pe1[isec][1]=0;
    mev2pe2[isec][0]=0;//physpad2
    mev2pe2[isec][1]=0;
    phsec=2*isec;
    p1s1ok=(ed2pe[phsec][0]>0 && atlen[phsec]>0);
    p1s2ok=(ed2pe[phsec][1]>0 && atlen[phsec]>0);
    p2s1ok=(ed2pe[phsec+1][0]>0 && atlen[phsec+1]>0);
    p2s2ok=(ed2pe[phsec+1][1]>0 && atlen[phsec+1]>0);
    if(p1s1ok && p2s1ok){// <--- side-1
      status1[isec][0]=0;//p1s1 ok
      status2[isec][0]=0;//p2s1 ok
      mev2pe1[isec][0]=ed2pe[phsec][0];//p1s1
      attlen[isec][0]=atlen[phsec];
      mev2pe2[isec][0]=ed2pe[phsec+1][0];//p2s1
      attlen[isec][1]=atlen[phsec+1];
    }
    else if(p1s1ok){
      status1[isec][0]=0;//p1s1 ok
      status2[isec][0]=1;//p2s1 bad
      mev2pe1[isec][0]=ed2pe[phsec][0];//p1s1
      attlen[isec][0]=atlen[phsec];
    }
    else if(p2s1ok){
      status1[isec][0]=1;//p1s1 bad
      status2[isec][0]=0;//p2s1 ok
      mev2pe1[isec][0]=ed2pe[phsec+1][0];//p2s1
      attlen[isec][1]=atlen[phsec+1];
    }
//    
    if(p1s2ok && p2s2ok){// <--- side-2
      status1[isec][1]=0;//p1s2 ok
      status2[isec][1]=0;//p2s2 ok
      mev2pe1[isec][1]=ed2pe[phsec][1];//p1s2
      attlen[isec][0]=atlen[phsec];
      mev2pe2[isec][1]=ed2pe[phsec+1][1];//p2s2
      attlen[isec][1]=atlen[phsec+1];
    }
    else if(p1s2ok){
      status1[isec][1]=0;//p1s2 ok
      status2[isec][1]=1;//p2s2 bad
      mev2pe1[isec][1]=ed2pe[phsec][1];//p1s2
      attlen[isec][0]=atlen[phsec];
    }
    else if(p2s2ok){
      status1[isec][1]=1;//p1s2 bad
      status2[isec][1]=0;//p2s2 ok
      mev2pe1[isec][1]=ed2pe[phsec+1][1];//p2s2
      attlen[isec][1]=atlen[phsec+1];
    }
// ----> write to file :   
    tcfile.setf(ios::fixed);
    tcfile << endl;
// s1    
    tcfile.width(1);
    tcfile.precision(1);// precision stat
    tcfile << status1[isec][0] <<" ";
    tcfile.width(6);
    tcfile.precision(2);// precision  mev2pe
    tcfile << mev2pe1[isec][0] <<" ";
    tcfile.width(1);
    tcfile.precision(1);// precision  stat
    tcfile << status2[isec][0] <<" ";
    tcfile.width(6);
    tcfile.precision(2);// precision  mev2pe
    tcfile << mev2pe2[isec][0] <<endl;
// s2    
    tcfile.width(1);
    tcfile.precision(1);// precision stat
    tcfile << status1[isec][1] <<" ";
    tcfile.width(6);
    tcfile.precision(2);// precision mev2pe
    tcfile << mev2pe1[isec][1] <<" ";
    tcfile.width(1);
    tcfile.precision(1);// precision stat
    tcfile << status2[isec][1] <<" ";
    tcfile.width(6);
    tcfile.precision(2);// precision mev2pe
    tcfile << mev2pe2[isec][1] <<endl;
// p1
    tcfile.width(6);
    tcfile.precision(1);// precision for attlen
    tcfile << attlen[isec][0] <<" ";     
// p2
    tcfile << attlen[isec][1] <<endl;     
    tcfile << endl;
//
  }// -----> endof ReadoutSector loop
//  
    tcfile << 12345 << endl;//endoffile label
    tcfile << endl;
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
    tcfile << endl<<" Date of the first event : "<<frdate<<endl;
    tcfile.close();
    cout<<" First run used for calibration is "<<StartRun<<endl;
    cout<<" Date of the first event : "<<frdate<<endl;
    cout<<"AntiCalib:output file closed !"<<endl;
//  
}
//=============================================================================
//===============================> ANTPedCalib:
//
  number ANTPedCalib::adc[ANTI2C::MAXANTI][2];//store Anode/Dynode adc sum
  number ANTPedCalib::adc2[ANTI2C::MAXANTI][2];//store adc-squares sum
  number ANTPedCalib::adcm[ANTI2C::MAXANTI][2][ATPCSTMX];//max. adc-values stack
  integer ANTPedCalib::nevt[ANTI2C::MAXANTI][2];// events in sum
  geant ANTPedCalib::peds[ANTI2C::MAXANTI][2];
  geant ANTPedCalib::sigs[ANTI2C::MAXANTI][2];
  uinteger ANTPedCalib::stas[ANTI2C::MAXANTI][2];
  integer ANTPedCalib::nstacksz;//really needed stack size (ev2rem*ANPCEVMX)
  integer ANTPedCalib::hiamap[ANTI2C::MAXANTI];//high signal Paddles map (1 event) 
  time_t ANTPedCalib::BeginTime;
  uinteger ANTPedCalib::BeginRun;
  ANTPedCalib::ANTPedCal_ntpl ANTPedCalib::ANTPedCalNT;
//
void ANTPedCalib::init(){ // ----> initialization for TofPed-calibration 
  integer i,j,k,il,ib,id,ii,jj,chan;
  char htit1[60];
  char inum[11];
  char in[2]="0";
  geant por2rem;
//
  strcpy(inum,"0123456789");
//
  cout<<endl;
//
   if(ATREFFKEY.relogic==4){//open Ntuple file (for OnBoardTable only for the moment)
     char hfile[161];
     UHTOC(IOPA.hfile,40,hfile,160);  
     char filename[256];
     strcpy(filename,hfile);
     integer iostat;
     integer rsize=1024;
     char event[80];  
     HROPEN(IOPA.hlun+1,"antpedsig",filename,"NP",rsize,iostat);
     if(iostat){
       cerr << "<==== ANTPedCalib::init: Error opening antpedsig ntuple file "<<filename<<endl;
       exit(1);
     }
     else cout <<"====> ANTPedCalib::init: Ntuple file "<<filename<<" opened..."<<endl;
     HBNT(IOPA.ntuple,"AntPedSigmas"," ");
    
     HBNAME(IOPA.ntuple,"ANTPedSig",(int*)(&ANTPedCalNT),"Run:I,Sector:I,PedA(2):R,SigA(2):R,StaA(2):I");
     return;
   }
//
  if(ATREFFKEY.relogic==2)por2rem=ATCAFFKEY.pedcpr[0];//ClassPed(random)
  else if(ATREFFKEY.relogic==3)por2rem=ATCAFFKEY.pedcpr[1];//DownScaled(in trigger)
  nstacksz=integer(floor(por2rem*ATPCEVMX+0.5));
  if(nstacksz>ATPCSTMX){
    cout<<"ANTPedCalib::init-W- Stack size too small, change truncate-value or max.events/ch !!!"<<nstacksz<<endl;
    cout<<"                Its size set back to max-possible:"<<ATPCSTMX<<endl;
    nstacksz=ATPCSTMX;
  }
  if(nstacksz<1)nstacksz=1;
  cout<<"ANTPedCalib::init: real stack-size="<<nstacksz<<endl;
//
//  ---> book hist.  :
//
  HBOOK1(2670,"Peds vs paddle for bot/top sides",20,1.,21.,0.);
  HMINIM(2670,75.);
  HMAXIM(2670,125.);
  HBOOK1(2671,"Ped-rms vs paddle for bot/top sides",20,1.,21.,0.);
  HMINIM(2671,0.);
  HMAXIM(2671,4.);
  HBOOK1(2672,"Ped-stat(1=bad) vs paddle for bot/top sides",20,1.,21.,0.);
  HMINIM(2672,0.);
  HMAXIM(2672,1.);
  HBOOK1(2673,"Ped-diff(old-new) vs paddle for bot/top sides",20,1.,21.,0.);
  HMINIM(2673,-5.);
  HMAXIM(2673,5.);
//
  for(i=0;i<ANTI2C::MAXANTI;i++){
    for(j=0;j<2;j++){
      strcpy(htit1,"Raw ADCs for Sector/Side=");
      in[0]=inum[i+1];
      strcat(htit1,in);
      in[0]=inum[j+1];
      strcat(htit1,in);
      id=2674+i*2+j;
    HBOOK1(id,htit1,80,80.,160.,0.);
    }
  }
  //
// ---> clear arrays:
//
  for(i=0;i<ANTI2C::MAXANTI;i++){
    for(j=0;j<2;j++){
      nevt[i][j]=0;
      adc[i][j]=0;
      adc2[i][j]=0;
      peds[i][j]=0;
      sigs[i][j]=0;
      stas[i][j]=1;//bad
      for(k=0;k<ATPCSTMX;k++)adcm[i][j][k]=0;
    }
  }
  cout<<"ANTPedCalib::init done..."<<endl<<endl;;
}
//-------------------------------------------
void ANTPedCalib::resetb(){ // ----> to work with OnBoardPedTables 
  integer i,j;
  char hmod[2]=" ";
  static int first(0);
//
  cout<<endl;
//
//  ---> book hist.  :
//
  if(first==0){
    HBOOK1(2670,"Peds vs paddle for bot/top sides",20,1.,21.,0.);
    HMINIM(2670,75.);
    HMAXIM(2670,125.);
    HBOOK1(2671,"Ped-rms vs paddle for bot/top sides",20,1.,21.,0.);
    HMINIM(2671,0.);
    HMAXIM(2671,4.);
    HBOOK1(2672,"Ped-stat(1=bad) vs paddle for bot/top sides",20,1.,21.,0.);
    HMINIM(2672,0.);
    HMAXIM(2672,1.);
    HBOOK1(2673,"Ped-diff(old-new) vs paddle for bot/top sides",20,1.,21.,0.);
    HMINIM(2673,-5.);
    HMAXIM(2673,5.);
    first=1;
  }
  else{
    for(i=0;i<4;i++)HRESET(2670+i,hmod);
  }
//
// ---> clear arrays:
//
  for(i=0;i<ANTI2C::MAXANTI;i++){
    for(j=0;j<2;j++){
      peds[i][j]=0;
      sigs[i][j]=0;
      stas[i][j]=1;//bad
    }
  }
  cout<<"====> ANTPedCalib::OnBoardPedTable/Histos Reset done..."<<endl<<endl;;
}
//-------------------------------------------
void ANTPedCalib::fill(int sr, int sd, geant val){//
   int i,ist,nev,evs2rem,srl,srr,id;
   geant lohil[2]={0,9999};//means no limits on val, if partial ped is bad
   geant ped,sig,sig2,spikethr;
   bool accept(true);
   geant por2rem;
//
   if(ATREFFKEY.relogic==2)por2rem=ATCAFFKEY.pedcpr[0];//ClassPed(random)
   else if(ATREFFKEY.relogic==3)por2rem=ATCAFFKEY.pedcpr[1];//DownScaled(in trigger)
//
//cout<<"--->In ANTPedCalib::fill: sect/sid="<<sr<<" "<<sd<<" val="<<val<<endl;
   nev=nevt[sr][sd];
// peds[sr][sd];//SmalSample(SS) ped (set to "0" at init)
   if(peds[sr][sd]==0 && nev==ATPCEVMN){//calc. SS-ped/sig when TFPCEVMN events is collected
     evs2rem=int(floor(por2rem*nev+0.5));
     if(evs2rem>nstacksz)evs2rem=nstacksz;
     if(evs2rem<1)evs2rem=1;
     for(i=0;i<evs2rem;i++){//remove "evs2rem" highest amplitudes
       adc[sr][sd]=adc[sr][sd]-adcm[sr][sd][i];
       adc2[sr][sd]=adc2[sr][sd]-adcm[sr][sd][i]*adcm[sr][sd][i];
     }
     ped=adc[sr][sd]/number(nev-evs2rem);//truncated average
     sig2=adc2[sr][sd]/number(nev-evs2rem);
     sig2=sig2-ped*ped;// truncated rms**2
     if(sig2>0 && sig2<=(2.25*ATPCSIMX*ATPCSIMX)){//2.25->1.5*SigMax
       sigs[sr][sd]=sqrt(sig2);
       peds[sr][sd]=ped;//is used now as flag that SS-PedS ok
     }
     adc[sr][sd]=0;//reset to start new life(with real selection limits)
     adc2[sr][sd]=0;
     nevt[sr][sd]=0;
     for(i=0;i<ATPCSTMX;i++)adcm[sr][sd][i]=0;
//     cout<<"PartialPed:sr/sd="<<sr<<" "<<sd<<endl;
//     cout<<"           ped/sig2="<<ped<<" "<<sig2<<endl;
//     cout<<"           evs2rem="<<evs2rem<<endl;
   }
   ped=peds[sr][sd];//now !=0 or =0 
   sig=sigs[sr][sd];
//
   if(ped>0){//set val-limits if partial ped OK
     lohil[0]=ped-3*sig;
     lohil[1]=ped+5*sig;
     spikethr=max(5*sig,ATPCSPIK);
     if(val>(ped+spikethr)){//spike(>~1mips in higain chan)
//       hiamap[sr][sd]=1;//put it into map
       accept=false;//mark as bad for filling
     }
//     else{//candidate for "fill" - check neigbours
//       if(sr>0)srl=sr-1;
//       else srl=0;
//       if(sr<(MAXANTI)srr=sr+1;
//       else srr=MAXANTI-1;
//       accept=(hiamap[srl]==0 && hiamap[srr]==0);//not accept if there is any neighbour
//     }
   }
//
   accept=true;//tempor to switch-off spike algorithm
//
   if(val>lohil[0] && val<lohil[1] && accept){//check "in_limits/not_spike"
     if(nev<ATPCEVMX){//limit statistics(to keep max-stack size small)
       adc[sr][sd]+=val;
       adc2[sr][sd]+=(val*val);
       nevt[sr][sd]+=1;
       for(ist=0;ist<nstacksz;ist++){//try to position val in stack of nstacksz highest max-values
          if(val>adcm[sr][sd][ist]){
	    for(i=nstacksz-1;i>ist;i--)adcm[sr][sd][i]=adcm[sr][sd][i-1];//move stack -->
	    adcm[sr][sd][ist]=val;//store max.val
	    break;
	  }
       }
     }
   }//-->endof "in limits" check
   if(accept){
     id=2674+sr*2+sd;
     HF1(id,val,1.);
   } 
}
//-------------------------------------------
void ANTPedCalib::filltb(int sr, int sd, geant ped, geant sig, int sta){
// for usage with OnBoardPedTables
  peds[sr][sd]=ped;
  sigs[sr][sd]=sig;
  stas[sr][sd]=sta;
}
//-------------------------------------------
void ANTPedCalib::outp(int flg){// very preliminary
// flg=0/1/2=>HistOnly/write2DB+file/write2file
   int i,sr,sd,statmin(9999);
   geant pdiff,por2rem,p2r;
   time_t begin=BTime();//begin time = 1st_event_time(filled at 1st "ped-block" arrival)
   uinteger runn=BRun();//1st event run# 
   time_t end,insert;
   char DataDate[30],WrtDate[30];
   strcpy(DataDate,asctime(localtime(&begin)));
   time(&insert);
   strcpy(WrtDate,asctime(localtime(&insert)));
//
   integer evs2rem;
   if(ATREFFKEY.relogic==2)por2rem=ATCAFFKEY.pedcpr[0];//ClassPed(random)
   else if(ATREFFKEY.relogic==3)por2rem=ATCAFFKEY.pedcpr[1];//DownScaled(in trigger)
//
   cout<<endl;
   cout<<"=====> ANTPedCalib-Report:"<<endl<<endl;
   for(sr=0;sr<ANTI2C::MAXANTI;sr++){
     for(sd=0;sd<2;sd++){
       if(nevt[sr][sd]>=ATPCEVMN){//statistics ok
	 evs2rem=integer(floor(por2rem*nevt[sr][sd]+0.5));
	 if(evs2rem>nstacksz)evs2rem=nstacksz;
//           if(evs2rem<1)evs2rem=1;
	 for(i=0;i<evs2rem;i++){//remove highest amplitudes
	   adc[sr][sd]=adc[sr][sd]-adcm[sr][sd][i];
	   adc2[sr][sd]=adc2[sr][sd]-adcm[sr][sd][i]*adcm[sr][sd][i];
	 }
	 adc[sr][sd]/=number(nevt[sr][sd]-evs2rem);//truncated average
	 adc2[sr][sd]/=number(nevt[sr][sd]-evs2rem);
	 adc2[sr][sd]=adc2[sr][sd]-adc[sr][sd]*adc[sr][sd];//truncated rms**2
	 if(adc2[sr][sd]>0
	                   && adc2[sr][sd]<=(ATPCSIMX*ATPCSIMX)
		                                       && adc[sr][sd]<300){//chan.OK
	   peds[sr][sd]=geant(adc[sr][sd]);
	   sigs[sr][sd]=geant(sqrt(adc2[sr][sd]));
	   stas[sr][sd]=0;//ok
//update ped-object in memory:
	   pdiff=peds[sr][sd]-ANTIPeds::anscped[sr].apeda(sd);
	   ANTIPeds::anscped[sr].apeda(sd)=peds[sr][sd];
	   ANTIPeds::anscped[sr].asiga(sd)=sigs[sr][sd];
	   ANTIPeds::anscped[sr].astaa(sd)=stas[sr][sd];
           HF1(2670,geant(sd*10+sr+1),ANTIPeds::anscped[sr].apeda(sd));
	   HF1(2671,geant(sd*10+sr+1),ANTIPeds::anscped[sr].asiga(sd));
	   HF1(2672,geant(sd*10+sr+1),geant(stas[sr][sd]));
	   HF1(2673,geant(sd*10+sr+1),pdiff);
	   if(statmin>nevt[sr][sd])statmin=nevt[sr][sd];
	 }
	 else{//bad chan
	   cout<<"      BadCh=Sector/Side="<<sr<<" "<<sd<<endl;
	   cout<<"                      Nevents="<<nevt[sr][sd]<<endl;    
	   cout<<"                      ped/sig**2="<<adc[sr][sd]<<" "<<adc2[sr][sd]<<endl;    
	 }
       }//--->endof "good statistics" check
       else{
	 cout<<"      LowStatCh=Sector/Side="<<sr<<" "<<sd<<" Nevents="<<nevt[sr][sd]<<endl;    
       }
     }//--->endof side-loop
   }//--->endof sector-loop
   cout<<"      MinAcceptableStatistics/channel was:"<<statmin<<endl; 
//   
// ---> prepare update of DB :
   if(flg==1){
     AMSTimeID *ptdv;
     ptdv = AMSJob::gethead()->gettimestructure(AMSID("Antipeds",AMSJob::gethead()->isRealData()));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     end=begin+86400*30;
     ptdv->SetTime(insert,begin,end);
   }
// ---> write MC/RD ped-file:
   if(flg==1 || flg==2){
     integer endflab(12345);
     char fname[80];
     char name[80];
     char buf[20];
//
     if(ATREFFKEY.relogic==2)strcpy(name,"antp_cl");//classic(all_events rundom trig)
     if(ATREFFKEY.relogic==3)strcpy(name,"antp_ds");//down_scaled events
     if(AMSJob::gethead()->isMCData())           // for MC-event
     {
       cout <<"      new MC peds-file will be written..."<<endl;
       strcat(name,"_mc.");
     }
     else                                       // for Real events
     {
       cout <<"      new RD peds-file will be written..."<<endl;
       strcat(name,"_rl.");
     }
     sprintf(buf,"%d",runn);
     strcat(name,buf);
     if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
     if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
     strcat(fname,name);
     cout<<"Open file : "<<fname<<'\n';
     cout<<" Date of the first used event : "<<DataDate<<endl;
     ofstream icfile(fname,ios::out|ios::trunc); // open pedestals-file for writing
     if(!icfile){
       cerr <<"<---- Problems to write new pedestals-file !!? "<<fname<<endl;
       exit(1);
     }
     icfile.setf(ios::fixed);
//
// ---> write Anodes peds/sigmas/stat:
//
     for(sr=0;sr<ANTI2C::MAXANTI;sr++){   // <-------- loop over layers
       icfile.width(2);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << stas[sr][sd]<<" ";//stat
       icfile << " ";
       icfile.width(6);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << peds[sr][sd]<<" ";//ped
       icfile << " ";
       icfile.width(5);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << sigs[sr][sd]<<" ";//sig
       icfile << endl;
     } // --- end of layer loop --->
     icfile << endl;
//
//
     icfile << endl<<"======================================================"<<endl;
     icfile << endl<<" Date of the 1st used event : "<<DataDate<<endl;
     icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
     icfile.close();
//
   }//--->endof file writing 
//
   for(i=0;i<20;i++)HPRINT(2670+i);
   cout<<endl;
   cout<<"====================== ANTPedCalib: job is completed ! ======================"<<endl;
   cout<<endl;
//
}
//-------------------------------------------
void ANTPedCalib::outptb(int flg){// very preliminary
// flg=0/1/2=>No/write2DB+file/write2file
   int i,sr,sd;
   int totch(0),goodtbch(0),goodch(0);
   geant pedo,sigo,pdiff;
   int stao;
   time_t begin=BTime();//begin time = 1st_event_time(filled at 1st "ped-block" arrival)
   uinteger runn=BRun();//1st event run# 
   time_t end,insert;
   char DataDate[30],WrtDate[30];
   strcpy(DataDate,asctime(localtime(&begin)));
   time(&insert);
   strcpy(WrtDate,asctime(localtime(&insert)));
//
   cout<<endl;
   cout<<"=====> ANTPedCalib:OnBoardTable-Report:"<<endl<<endl;
//---- fill ntuple:
   ANTPedCalNT.Run=BRun();
   for(sr=0;sr<ANTI2C::MAXANTI;sr++){
     ANTPedCalNT.Sector=sr+1;
     for(sd=0;sd<2;sd++){
       ANTPedCalNT.PedA[sd]=peds[sr][sd];
       ANTPedCalNT.SigA[sd]=sigs[sr][sd];
       ANTPedCalNT.StaA[sd]=stas[sr][sd];
     }
     HFNT(IOPA.ntuple);
   }
//----
   for(sr=0;sr<ANTI2C::MAXANTI;sr++){
     for(sd=0;sd<2;sd++){
       totch+=1;
       pedo=ANTIPeds::anscped[sr].apeda(sd);
       sigo=ANTIPeds::anscped[sr].asiga(sd);
       stao=ANTIPeds::anscped[sr].astaa(sd);
       pdiff=peds[sr][sd]-pedo;
//
       if(peds[sr][sd]>0 && stas[sr][sd]==0){//channel OK in table ? tempor: stas-definition from Kunin ?
	 goodtbch+=1;
	 if(sigs[sr][sd]>0 && sigs[sr][sd]<=ATPCSIMX
		                   && peds[sr][sd]<200 && fabs(pdiff)<10){//MyCriteria:chan.OK 
	   goodch+=1;
//update ped-object in memory:
	   ANTIPeds::anscped[sr].apeda(sd)=peds[sr][sd];
	   ANTIPeds::anscped[sr].asiga(sd)=sigs[sr][sd];
	   ANTIPeds::anscped[sr].astaa(sd)=stas[sr][sd];
           HF1(2670,geant(sd*10+sr+1),ANTIPeds::anscped[sr].apeda(sd));
	   HF1(2671,geant(sd*10+sr+1),ANTIPeds::anscped[sr].asiga(sd));
	   HF1(2672,geant(sd*10+sr+1),geant(stas[sr][sd]));
	   HF1(2673,geant(sd*10+sr+1),pdiff);
	 }
	 else{//MyCriteria: bad chan
	   cout<<"       MyCriteriaBadCh: Sector/Side="<<sr<<" "<<sd<<endl;
	   cout<<"                      ped/sig="<<peds[sr][sd]<<" "<<sigs[sr][sd]<<endl;    
	   cout<<"                      PedDiff="<<pdiff<<endl;    
	 }
       }//--->endof "channel OK in table ?" check
       else{
	 cout<<"       BadTableChan:Sector/Side="<<sr<<" "<<sd<<endl;    
	 cout<<"       ped/sig/sta="<<peds[sr][sd]<<" "<<sigs[sr][sd]<<" "<<stas[sr][sd]<<endl;    
       }
     }//--->endof side-loop
   }//--->endof sector-loop
   cout<<"       BadChannels(Table/My)="<<goodtbch<<" "<<goodch<<" from total="<<totch<<endl;  
//   
// ---> prepare update of DB :
   if(goodch==goodtbch && flg==1){//Update DB "on flight"
     AMSTimeID *ptdv;
     ptdv = AMSJob::gethead()->gettimestructure(AMSID("Antipeds",AMSJob::gethead()->isRealData()));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     end=begin+86400*30;
     ptdv->SetTime(insert,begin,end);
//
     if(AMSFFKEY.Update==2 ){
       AMSTimeID * offspring = 
         (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());//get 1st timeid instance
       while(offspring){
         if(offspring->UpdateMe())cout << "       Start update ANT-peds DB "<<*offspring; 
         if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
         cerr <<"       Problem To Update ANT-peds in DB"<<*offspring;
         offspring=(AMSTimeID*)offspring->next();//get one-by-one
       }
     }
   }
// ---> write OnBoardPedTable to ped-file:
   if(flg==1 || flg==2){
     integer endflab(12345);
     char fname[80];
     char name[80];
     char buf[20];
//
     strcpy(name,"antp_tb_rl.");//from OnBoardTable
     sprintf(buf,"%d",runn);
     strcat(name,buf);
     if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
     if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
     strcat(fname,name);
     cout<<"       Open file : "<<fname<<'\n';
     cout<<"       Date of the first used event : "<<DataDate<<endl;
     ofstream icfile(fname,ios::out|ios::trunc); // open pedestals-file for writing
     if(!icfile){
       cerr <<"<---- Problems to write new ONBT-Peds file !!? "<<fname<<endl;
       exit(1);
     }
     icfile.setf(ios::fixed);
//
// ---> write peds/sigmas/stat:
//
     for(sr=0;sr<ANTI2C::MAXANTI;sr++){   // <-------- loop over layers
       icfile.width(2);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << stas[sr][sd]<<" ";//stat
       icfile << " ";
       icfile.width(6);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << peds[sr][sd]<<" ";//ped
       icfile << " ";
       icfile.width(5);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << sigs[sr][sd]<<" ";//sig
       icfile << endl;
     } // --- end of layer loop --->
     icfile << endl;
//
//
     icfile << endl<<"======================================================"<<endl;
     icfile << endl<<" Date of the 1st used event : "<<DataDate<<endl;
     icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
     icfile.close();
//
   }//--->endof file writing 
//
   for(i=0;i<4;i++)HPRINT(2670+i);
   cout<<endl;
   cout<<"=================== ANTPedCalib:OnBoardTable job is completed ! ====================="<<endl;
   cout<<endl;
//
}
//--------------
void ANTPedCalib::ntuple_close(){
//
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//antpedsig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("antpedsig");
  CLOSEF(IOPA.hlun+1);
//
}
//
