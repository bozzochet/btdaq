//  $Id: tofuser02.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "tofdbc02.h"
#include "point.h"
#include "event.h"
#include "amsgobj.h"
#include "commons.h"
#include "cern.h"
#include "mccluster.h"
#include <math.h>
#include "extC.h"
#include "tofsim02.h"
#include "trrec.h"
#include "tofrec02.h"
#include "antirec02.h"
#include "particle.h"
//#include <daqblock.h>
#include "tofcalib02.h"
//#include "ntuple.h"
#include <time.h>
//
//
//
//=======================================================================
void TOF2User::Event(){  // some processing when all subd.info is redy (+accros)
  integer i,ilay,ibar,nbrl[TOF2GC::SCLRS],brnl[TOF2GC::SCLRS],bad,status,sector;
  integer nanti(0),nantit(0);
  integer il,ib,ix,iy,chan,nbrlc[TOF2GC::SCLRS],brnlc[TOF2GC::SCLRS];
  geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cost,cosc;
  number coo[TOF2GC::SCLRS],coot[TOF2GC::SCLRS],cstr[TOF2GC::SCLRS],dx,dy;
  number ama[2],amd[2];
  number adca1[TOF2GC::SCLRS],adca2[TOF2GC::SCLRS];
  number adcd1[TOF2GC::SCLRS],adcd2[TOF2GC::SCLRS];
  geant elosa[TOF2GC::SCLRS],elosd[TOF2GC::SCLRS],elosc[TOF2GC::SCLRS];
  number am1[TOF2GC::SCLRS],am2[TOF2GC::SCLRS];
  number am[2],eanti(0),eacl;
  number am1d[TOF2GC::SCLRS],am2d[TOF2GC::SCLRS];
  geant ainp[2],dinp[2],cinp;
  number ltim[TOF2GC::SCLRS],tdif[TOF2GC::SCLRS],trle[TOF2GC::SCLRS],trlr[TOF2GC::SCLRS];
  number cltim[TOF2GC::SCLRS];
  integer clmem[TOF2GC::SCLRS];
  number fpnt,bci,sut,sul,sul2,sutl,sud,sit2,tzer,chsq,betof,lflgt;
  number sigt[4]={0.15,0.15,0.15,0.15};// time meas.accuracy 
  number cvel(29.979);// light velocity
  number eacut=0.3;// cut on E-anti (mev)
  number dscut=8.;// TOF/Tracker-coord. dist.cut (hard usage of tracker)
  int16u crat,slot,tsens;
  integer swid,hwid;
  number temper,stimes[4],strr,offs,tinp,tout;
  static number tinpp,toutp,first(0);
  TOF2RawSide *ptrt;
  TOF2RawCluster *ptr;
  AMSTOFCluster *ptrc;
  AMSAntiCluster *ptra;
  AMSPoint clco[TOF2GC::SCLRS];
  uinteger Runum(0);
  ptrt=(TOF2RawSide*)AMSEvent::gethead()->
                           getheadC("TOF2RawSide",0);
  ptr=(TOF2RawCluster*)AMSEvent::gethead()->
                           getheadC("TOF2RawCluster",0);
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
//
//----
  Runum=AMSEvent::gethead()->getrun();// current run number
  TOF2JobStat::addre(21);
  for(i=0;i<TOF2GC::SCLRS;i++)nbrl[i]=0;
  for(i=0;i<TOF2GC::SCLRS;i++)nbrlc[i]=0;
  for(i=0;i<TOF2GC::SCLRS;i++)brnl[i]=-1;
  if(first==0){//clear 1st tinp/tout-measurements
    tinpp=-9999;
    toutp=-9999;
    first+=1;
  }
//
//-----> look at TOF2RawSide-hits(test for temper.study):
  while(ptrt){ // <--- loop over TOF2RawSide hits
    swid=ptrt->getsid();//LBBS
    hwid=ptrt->gethid();//CS
    crat=hwid/10;
    slot=hwid%10;
//    ptrt->getstdc(stimes);
    temper=ptrt->gettemp();
//    tinp=stimes[0]-stimes[1];
//    tout=stimes[1]-stimes[3];
//    cout<<"swid/hwid="<<swid<<" "<<hwid<<"  tin/out="<<tinp<<" "<<tout<<"  temp="<<temper<<endl;
    if(TFREFFKEY.reprtf[2]>0){
      if(swid==1041){
//        cout<<"swidOK, dti/dto="<<fabs(tinp-tinpp)<<" "<<fabs(tinp-tinpp)<<endl;
//	if(fabs(tinp-tinpp)>5 && fabs(tinp-tinpp)<200){//curr-prev. meas. ok
//	  strr=(tout-toutp)/(tinp-tinpp);
//	  offs=tout-strr*tinp;
//          HF2(1120,geant(tinp),geant(tout),1.);
//          HF2(1121,geant(temper),geant(strr),1.);
//          HF2(1122,geant(temper),geant(offs),1.);
//	}
//	tinpp=tinp;
//	toutp=tout;
      }
    }
    ptrt=ptrt->next();
  }// --- end of hits loop --->
//
//-----> take Tof RawCluster-hits :
  while(ptr){ // <--- loop over TOF2RawCluster hits
    status=ptr->getstatus();
    ilay=(ptr->getntof())-1;
    ibar=(ptr->getplane())-1;
    if((status&TOFGC::SCBADB1)==0 && (status&TOFGC::SCBADB3)==0){ //"good_history/good_in_DB
      if((status&TOFGC::SCBADB2)==0 || (status&TOFGC::SCBADB5)!=0){// 2-sided or recovered
        ptr->getadca(ama);// Anode-ampl(ADC-ch)
        ptr->getadcd(amd);// Dynode(equiliz.sum)-ampl(ADC-ch)
        adca1[ilay]=ama[0];
        adca2[ilay]=ama[1];
        adcd1[ilay]=amd[0];
        adcd2[ilay]=amd[1];
        TOF2Brcal::scbrcal[ilay][ibar].adc2q(0,ama,am);// Anode-ADC convert to charge
        am1[ilay]=am[0];
        am2[ilay]=am[1];
        TOF2Brcal::scbrcal[ilay][ibar].adc2q(2,amd,am);// dynode(sum)-ADC convert to charge
        am1d[ilay]=am[0];
        am2d[ilay]=am[1];
        nbrl[ilay]+=1;
        brnl[ilay]=ibar;
        elosa[ilay]=ptr->getedepa();
        elosd[ilay]=ptr->getedepd();
        coo[ilay]=ptr->gettimeD();// get local Y-coord., got from time-diff
        ltim[ilay]=ptr->gettime();// get ampl-corrected time
      }
    }
    ptr=ptr->next();
  }// --- end of hits loop --->
//
//-----> take Tof Cluster-hits :
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (TofClus containers) ---
    ptrc=(AMSTOFCluster*)AMSEvent::gethead()->
                           getheadC("AMSTOFCluster",ilay,0);
    while(ptrc){ // <--- loop over AMSTofCluster hits in L=ilay
      ibar=(ptrc->getplane())-1;
      elosc[ilay]=ptrc->getedep();//continious
      clco[ilay]=ptrc->getcoo();
      cltim[ilay]=-(ptrc->gettime())*1.e+9;//(back from V.C. format) 
      clmem[ilay]=ptrc->getnmemb();
      nbrlc[ilay]+=1;
      brnlc[ilay]=ibar;
      ptrc=ptrc->next();
    }
  }
//------> check  clust/layer=1 :
  bad=0;
  for(i=0;i<TOF2GC::SCLRS;i++)if(nbrlc[i] != 1)bad=1;
//  for(i=0;i<2;i++)if(nbrlc[i] != 1)bad=1;//tempor for l=1,2
//  for(i=2;i<4;i++)if(nbrlc[i] > 2)bad=1;//tempor for l=3,4
  
//
// -----> check Anti-counter :
  eanti=0;
  nanti=0;
  while (ptra){ // <--- loop over AMSANTIRawCluster hits
    status=ptra->getstatus();
    if((status & TOFGC::SCBADB1)==0){ //select only important(=FTCoincident) hits
      sector=(ptra->getsector())-1;
      eacl=ptra->getedep();
      eanti=eanti+(ptra->getedep());
      if(eacl>eacut)nanti+=1;
      if(TFREFFKEY.reprtf[2]>0)HF1(1503,geant(eacl),1.);
    }
    ptra=ptra->next();
  }// --- end of hits loop --->
  if(TFREFFKEY.reprtf[2]>0){
    HF1(1505,geant(nanti),1.);
    if(bad==0)HF1(1514,geant(nanti),1.);
  }
  nantit=Anti2RawEvent::getncoinc();//same from trigger
  if(TFREFFKEY.reprtf[2]>0)HF1(1517,geant(nantit),1.);
//
  if(nanti>1)return;// remove events with >1 sector(e>ecut) in Anti
  TOF2JobStat::addre(22);
  if(bad==1)return; // remove events with >1 clusters/layer 
  TOF2JobStat::addre(23);
//
  for(i=0;i<TOF2GC::SCLRS;i++){
    if(brnl[i]>=0)HF1(5040+i,geant(brnl[i]+1),1.);
  }
//
//------> get parameters from tracker:
//
    static number pmas(0.938),mumas(0.1057),imass;
    number pmom,mom,bet,chi2,betm,pcut[2],massq,beta,chi2t,chi2s;
    number the,phi,trl,rid,err,ctran,charge,partq;
    integer chargeTOF,chargeTracker,betpatt,trpatt,trhits(0);
    uinteger traddr(0);
    integer ilad[2][8]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    AMSPoint C0,Cout;
    AMSDir dir(0,0,1.);
    AMSContainer *cptr;
    AMSParticle *ppart;
    AMSTrTrack *ptrack;
    AMSCharge  *pcharge;
    AMSBeta * pbeta;
    int npart,ipatt;
    npart=0;
    cptr=AMSEvent::gethead()->getC("AMSParticle",0);// get pointer to part-envelop
    if(cptr)
           npart+=cptr->getnelem();
    if(TFREFFKEY.reprtf[2]>0)HF1(1518,geant(npart),1.);
    if(npart<1)return;// require events with 1 particle at least 
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    bad=1;			      
    while(ppart){
      partq=ppart->getcharge();
      ptrack=ppart->getptrack();//get pointer of the TRK-track, used in given particle
      if(ptrack){
        trpatt=ptrack->getpattern();//TRK-track pattern
	traddr=ptrack->getaddress();//TRK-track ladders combination id
	trhits=ptrack->getnhits();
	if(trpatt>=0){
          ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
          status=ptrack->getstatus();
          pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
          pbeta=ppart->getpbeta();
          betpatt=pbeta->getpattern();
          beta=pbeta->getbeta();
          chi2t=pbeta->getchi2();
          chi2s=pbeta->getchi2S();
          chargeTracker=pcharge->getchargeTracker();
          chargeTOF=pcharge->getchargeTOF();
	  charge=partq;
	  bad=0;
//	  if(trhits==8){
//	    AMSTrTrack::decodeaddress(ilad,traddr);
//	    cout<<"TofUser:TRK-address/pattern="<<traddr<<" "<<trpatt<<endl;
//	    cout<<"        Address(decoded):"<<endl;
//	    for(i=0;i<8;i++)cout<<ilad[0][i]<<" ";
//	    cout<<endl;
//	    for(i=0;i<8;i++)cout<<ilad[1][i]<<" ";
//	    cout<<endl;
//	  }
	}
      }
      if(!bad)break;//use 1st particle with good TRK-track
      ppart=ppart->next();
    }
     
//
    if(bad)return;//require part. with TRK-track
    TOF2JobStat::addre(24);
    pmom=fabs(rid);
//
    if(TFCAFFKEY.caltyp==0){ // space calibration
      bet=pmom/sqrt(pmom*pmom+pmas*pmas);
      imass=pmas;
      pcut[0]=TFCAFFKEY.plhc[0];
      pcut[1]=TFCAFFKEY.plhc[1];
    }
    else{                     // earth calibration
      bet=pmom/sqrt(pmom*pmom+mumas*mumas);
      imass=mumas;
      pcut[0]=TFCAFFKEY.plhec[0];
      pcut[1]=TFCAFFKEY.plhec[1];
    }
    if(TFREFFKEY.reprtf[2]>0){
      HF1(1500,geant(pmom),1.);
      HF1(1516,geant(pmom),1.);
    }
//
    bad=0;
//    if(pmom<=pcut[0] || pmom>=pcut[1])bad=1;// out of needed mom.range
    if((status&16384)!=0)bad=1;// remove FalseTOFX tracks
    if(bad==1)return;
//
    TOF2JobStat::addre(25);
//
    if(TFREFFKEY.reprtf[2]>0)HF1(1501,beta,1.);
    if(TFREFFKEY.reprtf[2]>0)HF1(1511,chi2t,1.);
    if(TFREFFKEY.reprtf[2]>0)HF1(1512,chi2s,1.);
    if(TFREFFKEY.reprtf[2]>0)HF1(1513,chi2,1.);
    if(TFREFFKEY.reprtf[2]>0)HF1(1515,geant(nanti),1.);
//
    bad=0;
//
// ----->  find track crossing points/angles with counters:
//
    C0[0]=0.;
    C0[1]=0.;
    AMSPoint crd;
    number cllc[TOF2GC::SCLRS];//cluster long.coo
    number cltc[TOF2GC::SCLRS];//cluster tran.coo
    for(il=0;il<TOF2GC::SCLRS;il++){
      crd=clco[il];
      ib=brnl[il];
      if(ib<0)continue;
      zc[il]=TOF2DBc::getzsc(il,ib);
      C0[2]=zc[il];
      ptrack->interpolate(C0,dir,Cout,the,phi,trl);
      cstr[il]=cos(the);
      trlr[il]=fabs(trl);
      if(TOF2DBc::plrotm(il)==0){
        coot[il]=Cout[1];// unrot. (X-meas) planes -> take trk Y-cross as long.c
        ctran=Cout[0];// TRK transv. coord.(abs. r.s.)(X-cross)
	cltc[il]=crd[0];//Cl.tran.coo(XCl,abs)
	cllc[il]=crd[1];//Cl.long.coo(YCl,abs) 
      }
      else {
        coot[il]=Cout[0];// rot. (Y-meas) planes -> take trk X-cross as long.c.
        ctran=Cout[1];// TRK transv. coord.(abs. r.s.)(Y-cross) 
	cllc[il]=crd[0];//cl.long.coo(XCl,abs) 
	cltc[il]=crd[1];//Cl.tran.coo(YCl,abs)
      }
      dy=coot[il]-cllc[il];//Long.coo_track-Long.coo_TofClust
      if(TFREFFKEY.reprtf[2]>0){
        if(clmem[il]==1)HF1(1200+il,geant(dy),1.);
        if(clmem[il]==2)HF1(1204+il,geant(dy),1.);
      }
      dx=ctran-cltc[il];//Transv.coo_tracker-Transv.coo_TofClust
      if(TFREFFKEY.reprtf[2]>0){
        if(clmem[il]==1)HF1(1210+il,geant(dx),1.);
        if(clmem[il]==2)HF1(1214+il,geant(dy),1.);
      }
      if(fabs(dx)>dscut || fabs(dy)>dscut)bad=1;//too big dist. of tof-tracker
//
      elosc[il]*=geant(fabs(cos(the)));// angle correction for dE/dX(to norm.inc)
      adca1[il]*=geant(fabs(cos(the)));// .....................adca
      adca2[il]*=geant(fabs(cos(the)));
    }//--> endof tof-layer loop
    cost=geant((fabs(cstr[0])+fabs(cstr[1])+fabs(cstr[2])+fabs(cstr[3]))/4);//average cos from track
//
//    if(bad)return;//too big difference of TOF-Tracker coord.
    TOF2JobStat::addre(26);
//
//--------> find beta TOF-times and Tracker track lengthes :
//
//
    trle[0]=0.;
    trle[1]=trlr[0]-trlr[1];//1->2
    trle[2]=trlr[0]+trlr[2];//1->3
    trle[3]=trlr[0]+trlr[3];//1->4
    tdif[0]=0;
    tdif[1]=cltim[0]-cltim[1];
    tdif[2]=cltim[0]-cltim[2];
    tdif[3]=cltim[0]-cltim[3];
    fpnt=0;
    sul=0;
    sut=0;
    sutl=0;
    sul2=0;
    sud=0.;
    for(il=0;il<TOF2GC::SCLRS;il++){
      sit2=pow(sigt[il],2);
      fpnt+=1;
      sud+=1./sit2;
      sut+=tdif[il]/sit2;
      sul+=trle[il]/sit2;
      sul2+=(trle[il]*trle[il])/sit2;
      sutl+=(tdif[il]*trle[il])/sit2;
    }
    bci = (sud*sutl-sut*sul)/(sud*sul2-sul*sul);
    tzer=(sut*sul2-sutl*sul)/(sud*sul2-sul*sul);
    chsq=0;
    for(il=0;il<TOF2GC::SCLRS;il++)chsq+=pow((tzer+bci*trle[il]-tdif[il])/sigt[il],2);
    chsq/=number(fpnt-2);
    betof=1./bci/cvel;
    if(TFREFFKEY.reprtf[2]>0)HF1(1502,betof,1.);
    if(TFREFFKEY.reprtf[2]>0)HF1(1208,chsq,1.);
    if(TFREFFKEY.reprtf[2]>0)HF1(1209,tzer,1.);
//    if(chsq>6. || betof<0.3)return;//cut on chi2/beta
//
    geant td13,td24;
    geant zpl1,zpl2,trlnor;
    zpl1=TOF2DBc::supstr(1)-
        (TOF2DBc::plnstr(1)+TOF2DBc::plnstr(6)/2+TOF2DBc::plnstr(3)/2);//z-l1-middl
    zpl2=TOF2DBc::supstr(1)-
        (TOF2DBc::plnstr(2)+TOF2DBc::plnstr(6)/2+TOF2DBc::plnstr(3)/2);//z-l2-middl
     
    trlnor=zpl1+zpl2;//z-dist. L1-L3(L2-L4)
    td13=tdif[2]*trlnor/trle[2];// normalized to fix(~127cm) distance
    td24=(cltim[1]-cltim[3])*trlnor/(trle[3]-trle[1]);// normalized to fix(~127cm) distance
    if(TFREFFKEY.reprtf[2]>0)HF1(1504,(td13-td24),1.);
//
    if(TFREFFKEY.reprtf[2]>0){
      bad=0;
      for(il=0;il<TOF2GC::SCLRS;il++){
        if(nbrl[il]>1)bad=1;
	if((il==0) && (brnl[il]==0 || brnl[il]==7))bad+=1;
	if((il==3) && (brnl[il]==0 || brnl[il]==7))bad+=1;
	if(il==1 && (brnl[il]==0 || brnl[il]==7))bad+=1;
	if(il==2 && (brnl[il]==0 || brnl[il]==9))bad+=1;
      }
      if(bad<2){
        HF1(1507,geant(chargeTOF),1.);//accept max one trapez.counter
        HF1(1508,geant(chargeTracker),1.);
        HF1(1506,geant(charge),1.);
        HF2(1509,geant(chargeTracker),geant(chargeTOF),1.);
      }
    }
//
    geant *pntr[TOF2GC::SCLRS];
    geant avera[4];
    if(betof>0.9){ // dE/dX only relativistic particles
//                  ---> look at truncated averages :
      for(il=0;il<TOF2GC::SCLRS;il++)pntr[il]=&elosc[il];//pointers to layer edep's 
      AMSsortNAG(pntr,TOF2GC::SCLRS);//sort in increasing order
      avera[0]=(*pntr[0]);// lowest
      avera[1]=avera[0]+(*pntr[1]);// sum of 2 lowest
      avera[2]=avera[1]+(*pntr[2]);//        3 lowest
      avera[3]=avera[2]+(*pntr[3]);// average
      avera[1]/=2.;
      avera[2]/=3.;
      avera[3]/=4.;
      if(TFREFFKEY.reprtf[2]>0){
        for(il=0;il<TOF2GC::SCLRS;il++)HF1(5001+il,avera[il],1.);
        HF1(5010,sqrt(avera[0]/1.8),1.);// eff.Z
        HF1(5011,sqrt(avera[1]/1.8),1.);
        HF1(5012,sqrt(avera[2]/1.8),1.);
        HF1(5013,sqrt(avera[3]/1.8),1.);
      }
    }
//
//-----> look at angle-corrected anode amplitudes(adc-chan) for all bar-types:
//
    integer ibtyp,nbrs;
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      if(ib<0)continue;
      nbrs=nbrl[il];
      if(nbrl[il]==1 && fabs(coot[il])<10){//select counter center crossing
        ibtyp=TOF2DBc::brtype(il,ib)-1;
	HF1(1220+ibtyp,geant(adca1[il]),1.);
      }
    } 
//
//-----> simulate TOF-group Npe measurement procedure for some counter (used in Bologna DB)
    number varr,amp1,amp2,sig,noise;
    if(TFREFFKEY.reprtf[2]>0 && brnl[0]>=2 && brnl[0]<=5){//use counters 103-106(type=2) for the moment
//    if(TFREFFKEY.reprtf[2]>0 && brnl[0]==3){//use counter 104(type=2) for the moment
      if(fabs(coot[0])<2.5){//use narrow area at the counter center
//        sig=3.5;//(adc-ch, 10% of of prot mprob.ampl=35)
	sig=0;
        noise=sig*rnormx(); 
        amp1=adca1[0]+noise;
        noise=sig*rnormx(); 
	amp2=adca2[0]+noise;
        varr=(amp1-amp2)/(amp1+amp2);
        HF1(1519,geant(varr),1.);
      }
    }
//
//
    return;
//
}
//----------------------------
void TOF2User::InitJob(){
  int i;
  if(TFREFFKEY.reprtf[2]>0){
    HBOOK1(1500,"TofUser:Part.rigidity from tracker(gv)",100,0.,40.,0.);
    HBOOK1(1501,"TofUser:Particle beta",80,-1.2,1.2,0.);
    HBOOK1(1511,"TofUser:Particle betachi2",80,0.,16.,0.);
    HBOOK1(1512,"TofUser:Particle betachi2S",80,0.,16.,0.);
    HBOOK1(1513,"TofUser:Particle trackchi2",80,0.,16.,0.);
    HBOOK1(1514,"TofUser:Number of Sectors(FTCoinc,E>Thr,TOFmultOK)",20,0.,20.,0.);
    HBOOK1(1515,"TofUser:Number of Sectors(FTCoinc,E>Thr,TOF/ANTImultOK,TrkOK)",20,0.,20.,0.);
    HBOOK1(1502,"TofUser:MyBeta(tof)",80,0.7,1.2,0.);
    HBOOK1(1504,"TofUser:TofClust T13-T24(ns,high momentum)",80,-4.,4.,0.);
    HBOOK1(1503,"TofUser:Sector energy(mev,FTCoinc)",80,0.,20.,0.);
    HBOOK1(1505,"TofUser:Number of Sectors(FTCoinc,E>Thr)",20,0.,20.,0.);
    HBOOK1(1506,"TofUser:Part.charge(trapez.c <=1)",20,0.,20.,0.);
    HBOOK1(1507,"TofUser:TOF-charge(trapez.c <=1)",20,0.,20.,0.);
    HBOOK1(1508,"TofUser:Tracker-charge(trapez.c <=1)",20,0.,20.,0.);
    HBOOK2(1509,"TofUser:TOF-ch vs Tracker-ch(trapez.c <=1)",10,0.,10.,10,0.,10.,0.);
    HBOOK1(1510,"TofUser:Anti-hit part.index",50,0.,50.,0.);
    HBOOK1(1516,"TofUser:Part.rigidity from tracker(gv)",100,0.,1500.,0.);
    HBOOK1(1517,"TofUser:Number of Sectors(FTCoincAccordingToTrigPatt)",20,0.,20.,0.);
    HBOOK1(1518,"TofUser:Number of particles",20,0.,20.,0.);
    HBOOK1(1519,"TofUser:(As1-As2)/(As1+As2)(Ampl.in adc-ch, cid=104)",80,-0.6,0.6,0.);
    
    HBOOK1(1200,"TofUser:LongCooDiff(Track-TofCl),L=1,Nmem=1",50,-10.,10.,0.);
    HBOOK1(1201,"TofUser:LongCooDiff(Track-TofCl),L=2,Nmem=1",50,-10.,10.,0.);
    HBOOK1(1202,"TofUser:LongCooDiff(Track-TofCl),L=3,Nmem=1",50,-10.,10.,0.);
    HBOOK1(1203,"TofUser:LongCooDiff(Track-TofCl),L=4,Nmem=1",50,-10.,10.,0.);
    HBOOK1(1204,"TofUser:LongCooDiff(Track-TofCl),L=1,Nmem=2",50,-10.,10.,0.);
    HBOOK1(1205,"TofUser:LongCooDiff(Track-TofCl),L=2,Nmem=2",50,-10.,10.,0.);
    HBOOK1(1206,"TofUser:LongCooDiff(Track-TofCl),L=3,Nmem=2",50,-10.,10.,0.);
    HBOOK1(1207,"TofUser:LongCooDiff(Track-TofCl),L=4,Nmem=2",50,-10.,10.,0.);
    
    HBOOK1(1210,"TofUser:TranCooDiff(Track-TofCl),L=1,Nmem=1",50,-20.,20.,0.);
    HBOOK1(1211,"TofUser:TranCooDiff(Track-TofCl),L=2,Nmem=1",50,-20.,20.,0.);
    HBOOK1(1212,"TofUser:TranCooDiff(Track-TofCl),L=3,Nmem=1",50,-20.,20.,0.);
    HBOOK1(1213,"TofUser:TranCooDiff(Track-TofCl),L=4,Nmem=1",50,-20.,20.,0.);
    HBOOK1(1214,"TofUser:TranCooDiff(Track-TofCl),L=1,Nmem=2",50,-20.,20.,0.);
    HBOOK1(1215,"TofUser:TranCooDiff(Track-TofCl),L=2,Nmem=2",50,-20.,20.,0.);
    HBOOK1(1216,"TofUser:TranCooDiff(Track-TofCl),L=3,Nmem=2",50,-20.,20.,0.);
    HBOOK1(1217,"TofUser:TranCooDiff(Track-TofCl),L=4,Nmem=2",50,-20.,20.,0.);
    
    HBOOK1(1208,"TofUser:Chisq(beta-fit with TofCl)",50,0.,10.,0.);
    HBOOK1(1209,"TofUser:Tzer (beta-fit with TofCl)",50,-2.5,2.5,0.);
    
    HBOOK1(1220,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-1)",80,0.,160.,0.);
    HBOOK1(1221,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-2)",80,0.,160.,0.);
    HBOOK1(1222,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-3)",80,0.,160.,0.);
    HBOOK1(1223,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-4)",80,0.,160.,0.);
    HBOOK1(1224,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-5)",80,0.,160.,0.);
    HBOOK1(1225,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-6)",80,0.,160.,0.);
    HBOOK1(1226,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-7)",80,0.,160.,0.);
    HBOOK1(1227,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-8)",80,0.,160.,0.);
    HBOOK1(1228,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-9)",80,0.,160.,0.);
    HBOOK1(1229,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-10)",80,0.,160.,0.);
    HBOOK1(1230,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-11)",80,0.,160.,0.);
    
    HBOOK1(5001,"TofUser:TofCl EdepNormInc(mev),AverTrunc1of4",80,0.,32.,0.);
    HBOOK1(5002,"TofUser:TofCl EdepNormInc(mev),AverTrunc2of4",80,0.,32.,0.);
    HBOOK1(5003,"TofUser:TofCl EdepNormInc(mev),AverTrunc3of4",80,0.,32.,0.);
    HBOOK1(5004,"TofUser:TofCl EdepNormInc(mev),AverTrunc4of4",80,0.,32.,0.);
    
    HBOOK1(5010,"TofUser:TofCl Sqrt(AverTrunc1of4/MIP)",80,0.,32.,0.);
    HBOOK1(5011,"TofUser:TofCl Sqrt(AverTrunc2of4/MIP)",80,0.,32.,0.);
    HBOOK1(5012,"TofUser:TofCl Sqrt(AverTrunc3of4/MIP)",80,0.,32.,0.);
    HBOOK1(5013,"TofUser:TofCl Sqrt(AverTrunc4of4/MIP)",80,0.,32.,0.);
    
    HBOOK1(5040,"TofUser:L=1,Fired bar number",14,1.,15.,0.);
    HBOOK1(5041,"TofUser:L=2,Fired bar number",14,1.,15.,0.);
    HBOOK1(5042,"TofUser:L=3,Fired bar number",14,1.,15.,0.);
    HBOOK1(5043,"TofUser:L=4,Fired bar number",14,1.,15.,0.);
  }
  return;
}
//===================================================================
void TOF2User::EndJob(){
  int i;
  char chopt[6]="qb";
  char chfun[6]="g";
  char chopt1[5]="LOGY";
  geant par[3],step[3],pmin[3],pmax[3],sigp[3],chi2;
//
  if(TFREFFKEY.reprtf[2]==0)return;
  par[0]=200.;
  par[1]=1.;
  par[2]=0.03;
  step[0]=50.;
  step[1]=0.005;
  step[2]=0.005;
  pmin[0]=10.;
  pmin[1]=0.8;
  pmin[2]=0.01;
  pmax[0]=10000.;
  pmax[1]=1.2;
  pmax[2]=0.1;
  HFITHN(1502,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" TOFbeta-fit: Mp/resol="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(1502);
//
  par[0]=200.;
  par[1]=0.;
  par[2]=0.3;
  step[0]=50.;
  step[1]=0.1;
  step[2]=0.05;
  pmin[0]=10.;
  pmin[1]=-2.;
  pmin[2]=0.05;
  pmax[0]=10000.;
  pmax[1]=2.;
  pmax[2]=0.5;
  HFITHN(1504,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" TOFTresol-fit: Mp/resol="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(1504);
//
  HPRINT(1500);
  HPRINT(1516);
  HPRINT(1501);
  HPRINT(1511);
  HPRINT(1512);
  HPRINT(1513);
  
  HPRINT(1503);
  HPRINT(1505);
  HPRINT(1514);
  HPRINT(1517);
  HPRINT(1515);
  
  HPRINT(1518);
  HPRINT(1519);
  HPRINT(1507);
  HPRINT(1508);
  HPRINT(1506);
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
  HPRINT(1214);
  HPRINT(1215);
  HPRINT(1216);
  HPRINT(1217);
  HPRINT(1208);
  HPRINT(1209);
  HPRINT(1510);
  
  HPRINT(1220);
  HPRINT(1221);
  HPRINT(1222);
  HPRINT(1223);
  HPRINT(1224);
  HPRINT(1225);
  HPRINT(1226);
  HPRINT(1227);
  HPRINT(1228);
  HPRINT(1229);
  HPRINT(1230);
  
  for(i=0;i<TOF2GC::SCLRS;i++)HPRINT(5040+i);
  for(i=0;i<TOF2GC::SCLRS;i++)HPRINT(5001+i);
  for(i=0;i<TOF2GC::SCLRS;i++)HPRINT(5010+i);
//  HIDOPT(5020,chopt1);
//  HPRINT(5020);
  return;
}
