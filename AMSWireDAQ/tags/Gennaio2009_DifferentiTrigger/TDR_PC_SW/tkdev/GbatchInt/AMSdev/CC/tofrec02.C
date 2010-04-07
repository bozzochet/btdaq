//  $Id: tofrec02.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// last modif. 10.12.96 by E.Choumilov - TOF2RawCluster::build added, 
//                                       AMSTOFCluster::build rewritten
//              16.06.97   E.Choumilov - TOF2RawSide::validate added
//                                       TOF2RawCluster::sitofdigi modified for trigg.
//              26.06.97   E.Choumilov - DAQ decoding/encoding added
//              22.08.05   E.Choumilov - gain5 logic removed
//  !!!!!  15.03.06   E.Choumilov - complete rebuild caused by new readout(new TDC)
//
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
#include "tofid.h"
#include "tofrec02.h"
#include "antirec02.h"
#include "particle.h"
#include "daqs2block.h"
#include "tofcalib02.h"
#include "ntuple.h"
#include <time.h>
//
// mem.reservation for some static arrays:
//
uinteger TOF2RawCluster::trpatt[TOF2GC::SCLRS]={0,0,0,0};//just init. of static var.
uinteger TOF2RawCluster::trpatt1[TOF2GC::SCLRS]={0,0,0,0};//just init. of static var.
integer TOF2RawCluster::trflag=0;
//
geant TOF2RawCluster::fttime[TOF2GC::SCCRAT]={0.,0.,0.,0.};
uinteger TOF2RawSide::StartRun(0);
time_t TOF2RawSide::StartTime(0);

integer TOF2RawSide::SumHTt[TOF2GC::SCCRAT][TOF2GC::SCFETA-1][TOF2GC::SCTHMX2];//TDC SumHT(SHT)-channel mem.reserv.
integer TOF2RawSide::SumHTh[TOF2GC::SCCRAT][TOF2GC::SCFETA-1]; 
integer TOF2RawSide::SumSHTt[TOF2GC::SCCRAT][TOF2GC::SCFETA-1][TOF2GC::SCTHMX2];
integer TOF2RawSide::SumSHTh[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];
integer TOF2RawSide::FTtime[TOF2GC::SCCRAT][TOF2GC::SCFETA][TOF2GC::SCTHMX1];//FT-channels(incl.ANTI) mem.reserv.
integer TOF2RawSide::FThits[TOF2GC::SCCRAT][TOF2GC::SCFETA];//number of FT-channel hits ....... 
//
AMSTOFCluster * AMSTOFCluster::_Head[4]={0,0,0,0};
integer AMSTOFCluster::_planes=0;
integer AMSTOFCluster::_padspl[TOF2GC::SCLRS]={0,0,0,0};
//
//
//-----------------------------------------------------------------------
void TOF2RawSide::validate(int &status){ //Check/correct RawSide-structure
  integer nftdc,nstdc,nadcd,nsumh,nsumsh;
  integer ftdc[TOF2GC::SCTHMX1],stdc[TOF2GC::SCTHMX3];
  integer sumht[TOF2GC::SCTHMX2],sumsht[TOF2GC::SCTHMX2];
  geant adca;
  geant adcd[TOF2GC::PMTSMX];
  integer ilay,last,ibar,isid,isds(0),pedrun(0);
  integer i,j,im,tmfound,complm,chnum,brnum;
  int16u id,idd,idN,stat,idr;
  number dt;
  geant peda,pedd;
  int16u otyp,mtyp,crat,slot,tsens;
  int bad(1);
  static int first(0);
  geant charge,temp;
  TOF2RawSide *ptr;
//
  status=1;//bad
//
  ptr=(TOF2RawSide*)AMSEvent::gethead()
                        ->getheadC("TOF2RawSide",0,1);//last 1 to sort
//
//#ifdef __AMSDEBUG__
  if(TFREFFKEY.reprtf[4]>0)
  cout<<endl<<"========> TOF::validation: for event "<<(AMSEvent::gethead()->getid())<<endl;
//#endif
//
  if(first==0){//store run/time for the first event
    first=1;
    StartRun=AMSEvent::gethead()->getrun();
    StartTime=AMSEvent::gethead()->gettime();
    TOFPedCalib::BRun()=AMSEvent::gethead()->getrun();
    TOFPedCalib::BTime()=AMSEvent::gethead()->gettime();//for possible classic/"DownScaledEvent" PedCalib
//(if 1st "DownScalEvent" come later than 1st normal event, BTime() will be owerwritten at its decoding stage)  
  }
//---- Scint.data length monitoring:
  if(TFREFFKEY.reprtf[4]>0){
    im=DAQS2Block::gettbll();//total blocks length for current format
//    im=0;
//    for(i=0;i<8;i++)im+=DAQS2Block::calcblocklength(i);
    HF1(1107,geant(im),1.);
  }
//---------------------------------------------------------------
// ====> check for PedCalib data if PedCalJob :
//
  int chan,il,ib,is;
  if(TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6){//PedCalJob
    TOF2JobStat::addre(38);
    TOFPedCalib::hiamreset();
    while(ptr){//<--RawSide-objects loop
      idd=ptr->getsid();//LBBS
      id=idd/10;// short id=LBB
      ilay=id/100-1;
      ibar=id%100-1;
      isid=idd%10-1;
      stat=ptr->getstat();//should be 1(no ped-subtr/suppr) if (PedCalJob & ClassPedCalData(or DownScaled))
      adca=ptr->getadca();
      nadcd=ptr->getadcd(adcd);
      if(stat==1){//not PedSubtractedData, fill PedCal arrays
        pedrun=1;
	if(adca>0)TOFPedCalib::fill(ilay,ibar,isid,0,adca);
	for(int pmt=0;pmt<TOF2DBc::npmtps(ilay,ibar);pmt++)
	                          if(adcd[pmt]>0)TOFPedCalib::fill(ilay,ibar,isid,pmt+1,adcd[pmt]);
      }
//
      ptr=ptr->next();// take next RawEvent hit
    }//  ---- end of RawSide hits loop ------->
    return;//PedCalJob always exit here with status=1(bad) to bypass next reco-stages !!!
  }
//---------------------------------------------------------------
//
// =============> check/combine adc/tdc/Ft data :
//
  while(ptr){// <--- loop over TOF RawSide hits
    idd=ptr->getsid();//LBBS
    id=idd/10;// short id=LBB
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
    stat=ptr->getstat();//upto now it is just ped-subtr flag(should be =0(PedSubtracted))
    if(stat>0){
      cout<<"TOF2RawSide::validate:-E- Found not PedSubtracted Data while not PedCalJob !!"<<endl;
      exit(2);
    } 
#ifdef __AMSDEBUG__
    assert(ilay>=0 && ilay<TOF2DBc::getnplns());
    assert(ibar>=0 && ibar<TOF2DBc::getbppl(ilay));
    assert(isid>=0 && isid<2);
    if(TFREFFKEY.reprtf[4]>0)ptr->_printEl(cout);
#endif
//---> set temper.for TOF2RawSide-obj using static job temper-store(not all events have temp-measur)
    mtyp=0;
    otyp=0;
    AMSSCIds tofid(ilay,ibar,isid,otyp,mtyp);//otyp=0(anode),mtyp=0(LTtime)
    crat=tofid.getcrate();//current crate#
    slot=tofid.getslot();//sequential slot#(0,1,...9)(2 last are fictitious for d-adcs)
    tsens=tofid.gettempsn();//... sensor#(1,2,...,5)(not all slots have temp-sensor!)
    ptr->settemp(TOF2JobStat::gettemp(crat,tsens-1));
//<---
    chnum=ilay*TOF2GC::SCMXBR*2+ibar*2+isid;//channels numbering
    brnum=ilay*TOF2GC::SCMXBR+ibar;//bar numbering
    temp=ptr->gettemp();//
    charge=ptr->getcharg();
//
    tmfound=0;
    complm=0;
//       fill working arrays for given channel:
      nstdc=ptr->getstdc(stdc);
      adca=ptr->getadca();
      nadcd=ptr->getadcd(adcd);
      TOF2JobStat::addch(chnum,12);
//---> check FTtime info in related crat/slot :
      nftdc=TOF2RawSide::FThits[crat][tsens-1];
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(nftdc),1.);
      if(nftdc>0){
        for(i=0;i<nftdc;i++)ftdc[i]=TOF2RawSide::FTtime[crat][tsens-1][i];
	ptr->putftdc(nftdc,ftdc);//attach FTtime info to given RawSide-object
        if(nftdc>1)TOF2JobStat::addch(chnum,14);//count multy FT cases
      }
      else{
        TOF2JobStat::addch(chnum,13);
	stat=1;//mark missing FTtime(unrecoverable case)
      }
//
//---> check LTtime info :
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(nstdc+10),1.);
      if(nstdc>0){
        if(nftdc==1 && idd==1041 && TFREFFKEY.reprtf[2]>0){
          for(i=0;i<nstdc;i++){
	    dt=(stdc[i]-ftdc[0])*TOF2DBc::tdcbin(1);//TDCch->ns
            HF1(1137,geant(dt),1.);//look at LTtime/FTtime correl in  some channel
	  }
	}
      }
      else{
        TOF2JobStat::addch(chnum,15);
	stat+=10;;//mark missing LTtime(unrecoverable case)
      }
      if(nftdc>0 && nstdc>0)tmfound=1;//found object with LTtime&FTtime OK
//
//---> check Anode-adc :
      if(adca>0)im=1;
      else im=0;
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+30),1.);
      if(adca==0)TOF2JobStat::addch(chnum,16);//miss Anode
//
//---> check Dynode-adc :
      im=nadcd;
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+40),1.);
      if(adca==0 && nadcd==0)stat+=100;//mark missing Ampl.info
      ptr->updstat(stat);//set RawSide-obj status to filter at reco-stage(befor it was =0(ok))
      number pedv,peds;
      pedv=TOFBPeds::scbrped[ilay][ibar].apeda(isid);
      peds=TOFBPeds::scbrped[ilay][ibar].asiga(isid);
      if((adca+3*peds)>=number(TOF2GC::SCPUXMX)){
//                                     +3*peds to be sure in high value of Anode(close to PUX-ovfl)
        if(nadcd==0)TOF2JobStat::addch(chnum,17);//miss Dynode at Anode ovfl - something wrong
      }
//
      if(tmfound==1 && (adca>0 || nadcd>0))complm=1;//found channel with complete t+amp measurement
      if(complm==1)bad=0;//found at least one channel with t+amp measurement per event - accept event
//
//---> check SumHTtime info:
      nsumh=TOF2RawSide::SumHTh[crat][tsens-1];
      for(i=0;i<nsumh;i++)sumht[i]=TOF2RawSide::SumHTt[crat][tsens-1][i];
      ptr->putsumh(nsumh,sumht);//fill object with SumHTtime-hits
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(nsumh+50),1.);
      if(nsumh==0)TOF2JobStat::addch(chnum,18);//miss sumHT info
//
//---> check SumSHTtime info:
      nsumsh=TOF2RawSide::SumSHTh[crat][tsens-1];
      for(i=0;i<nsumsh;i++)sumht[i]=TOF2RawSide::SumSHTt[crat][tsens-1][i];
      ptr->putsumsh(nsumsh,sumsht);//fill object with SumSHTtime-hits
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(nsumsh+70),1.);
//
//-----      
//
//---------------
NextObj:
    ptr=ptr->next();// take next RawEvent hit
  }//  ---- end of RawSide hits loop ------->
  if(bad==0)status=0;// good TOF-event(at least one t+amp measurement )
//
}
//-----------------------------------------------------------------------
void TOF2RawCluster::build(int &ostatus){
  integer nwftt,wftt[TOF2GC::SCTHMX1];
  integer nwltt,wltt[TOF2GC::SCTHMX3];
  integer nwsht,wsht[TOF2GC::SCTHMX2];
  integer nwssht,wssht[TOF2GC::SCTHMX2];
  number fttm;
  integer nftdc[2]={0,0};
  integer nstdc[2]={0,0};
  integer nhtdc[2]={0,0};
  integer nshtdc[2]={0,0};
  integer nttdc[2]={0,0};
  integer nadca[2]={0,0};
  integer nadcd[2]={0,0};
  number ftdc[2][TOF2GC::SCTHMX1],stdc[2][TOF2GC::SCTHMX3],ttdc[2][TOF2GC::SCTHMX3];
  number htdc[2][TOF2GC::SCTHMX2],shtdc[2][TOF2GC::SCTHMX2];
  integer itmatch[2][TOF2GC::SCTHMX3],itftcor[2][TOF2GC::SCTHMX3];
  integer ittdc[2][TOF2GC::SCTHMX3];
  integer itmbest[2];
  integer rej1,rej2,rej3,ibef,iaft;
  number dtmin[2][TOF2GC::SCTHMX3],tmbest[2],tbef,taft; 
  geant adca[2];
  geant adcd[2][TOF2GC::PMTSMX];
  integer SumHTuse;
  
  integer ilay,last,ibar,isid,isds,isd,isdsl[TOF2GC::SCLRS],slnu,tdcc;
  int16u crat,slot,tsens;
  integer i,j,k,chnum,brnum,am[2],tmi[2],itmf[2],sta,st,smty[2];
  integer nmpts;
  uinteger trpatt[TOF2GC::SCLRS];
  uinteger Runum(0);
  int statdb[2];
  int16u pbitn;
  int16u pbanti;
  int16u id,idd,idN,stat[2],amf[2];
  int16u mtyp(0),otyp(0);
  number ama[2],amd[2],amc[2];
  geant peds[2],sigs[2];
  number zc,elosa,elosal,elosd,elosdl,tmf[2],time,coo,ecoo;//input to RawCluster Constr
  number aedep,dedep;
  number tm[2],tf,tff,dt,fstd,tmr[2];
  number timeD,tamp;
  number temper[2]={0.,0.};
  number charg[2]={0.,0.};
  number t1,t2,t3,t4;
  geant blen,co,eco,point,brlm,pcorr,td2p,etd2p,clong[TOF2GC::SCLRS];
  TOF2RawSide *ptr;
  TOF2RawSide *ptrN;
  integer nbrl[TOF2GC::SCLRS],brnl[TOF2GC::SCLRS];
  int bad,tsfl(0),anchok,anlchok,dychok,dylchok,hlf;
// some variables for histogramming:
  geant gdt,tch,pch1[TOF2GC::SCLRS],pch2[TOF2GC::SCLRS],rrr[2];
  geant edepa[TOF2GC::SCLRS],edepd[TOF2GC::SCLRS];
  geant tcorr[TOF2GC::SCLRS],elosn;
  geant tuncorr[TOF2GC::SCLRS],tdiff[TOF2GC::SCLRS],td13,td24,td14;
  geant gnd,dh2l,a2d;
  number aaa,ddd;
  int dovfl,dlovfl;
  integer npmts;
  number adcdr[2][TOF2GC::PMTSMX];
  
//
  if(TFREFFKEY.reprtf[4]>0)cout<<"======> Enter TOF2RawCluster::build..."<<endl;
  ptr=(TOF2RawSide*)AMSEvent::gethead()
                                    ->getheadC("TOF2RawSide",0);
  Runum=AMSEvent::gethead()->getrun();// current run number
  pbitn=TOF2GC::SCPHBP;//phase bit position
  pbanti=pbitn-1;// mask to avoid it.
  ostatus=1;// event output status(init. as bad)
  isds=0;
  for(i=0;i<TOF2GC::SCLRS;i++)nbrl[i]=0;
//
//                             <---- loop over TOF RawEvent hits -----
  while(ptr){
    idd=ptr->getsid();
    id=idd/10;// short id=LBB, where L=1,4 BB=1,10
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
    mtyp=0;
    otyp=0;
    AMSSCIds tofid(ilay,ibar,isid,otyp,mtyp);//otyp=0(anode),mtyp=0(LTtime)
    crat=tofid.getcrate();//current crate#
    slot=tofid.getslot();//sequential slot#(0,1,...9)(2 last are fictitious for d-adcs)
//    tsens=tofid.gettsn();//... sensor#(1,2,...,5)(not all slots have temp-sensor!)
    temper[isid]=ptr->gettemp();//SFET(A)-slot temper(was set at Validation-stage from static job-store)
//
    chnum=ilay*TOF2GC::SCMXBR*2+ibar*2+isid;//channel numbering for job-stat counters
    brnum=ilay*TOF2GC::SCMXBR+ibar;//bar numbering ...
    stat[isid]=ptr->getstat();
    if(TFREFFKEY.reprtf[4]>0){
        cout<<endl;
        cout<<" --->look id="<<idd<<" cr/sl="<<crat<<" "<<slot
                                 <<" temper="<<temper[isid]<<" stat="<<stat[isid]<<endl;
    }
//
    if(stat[isid]%10==0                              //<--- validation status(FTtime is absolutely required)
      && TOF2Brcal::scbrcal[ilay][ibar].SideOK(isid) //<--- check hit DB(calibr)-status
      && TOFBPeds::scbrped[ilay][ibar].PedAchOK(isid)//<--- check hit DB(ped)-status
      && TOFBPeds::scbrped[ilay][ibar].PedDchOK(isid)
                                                     ){
      TOF2JobStat::addch(chnum,0);//statistics on input channel
//---> fill working arrays for given side:
      isds+=1;//count alive sides
      nwftt=ptr->getftdc(wftt);
      nwltt=ptr->getstdc(wltt);
      nwsht=ptr->getsumh(wsht);
      nwssht=ptr->getsumsh(wssht);
      if(TFREFFKEY.reprtf[4]>0){
        cout<<"     DAQ:nft/nlt/nsht/nssht="<<nwftt<<" "<<nwltt<<" "<<nwsht<<" "<<nwssht<<endl;
        for(int ih=0;ih<nwftt;ih++)cout<<"     FT:"<<wftt[ih]<<endl;
        for(int ih=0;ih<nwltt;ih++)cout<<"     LT:"<<wltt[ih]<<endl;
        for(int ih=0;ih<nwsht;ih++)cout<<"     sHT:"<<wsht[ih]<<endl;
        for(int ih=0;ih<nwssht;ih++)cout<<"    sSHT:"<<wssht[ih]<<endl;
      }
      for(i=0;i<nwftt;i++)ftdc[isid][i]=wftt[i]*TOF2DBc::tdcbin(1);//FTtime TDCch->ns)
      nftdc[isid]=nwftt;
      fttm=ftdc[isid][nftdc[isid]-1];// tempor use last FTtime-hit
      if(TFREFFKEY.reprtf[4]>0)cout<<"     FTtime(ns)="<<fttm<<endl;
//
      for(i=0;i<nwltt;i++)stdc[isid][i]=fttm-wltt[i]*TOF2DBc::tdcbin(1);//Rel.LTtime(+ means befor FTtime)(+TDCch->ns)
      nstdc[isid]=nwltt;
      for(i=0;i<nwsht;i++)htdc[isid][i]=fttm-wsht[i]*TOF2DBc::tdcbin(1);//Rel.SumHTtime +TDCch->ns
      nhtdc[isid]=nwsht;
      for(i=0;i<nwssht;i++)shtdc[isid][i]=fttm-wssht[i]*TOF2DBc::tdcbin(1);//Rel.SumSHTtime +TDCch->ns
      nshtdc[isid]=nwssht;
      if(TFREFFKEY.reprtf[4]>0){
        cout<<"     rel.times:"<<endl;
        for(int ih=0;ih<nstdc[isid];ih++)cout<<"     LT:"<<stdc[isid][ih]<<endl;
        for(int ih=0;ih<nhtdc[isid];ih++)cout<<"     sHT:"<<htdc[isid][ih]<<endl;
      }
//
//-----> select FT-correlated LTtime-hits(in age-window):
//
      nttdc[isid]=0;
      for(i=0;i<nstdc[isid];i++){
        itftcor[isid][i]=0;
        t1=stdc[isid][i];//t1=0 means FT-time
	if(t1>TOF2DBc::ltagew(0) && t1<TOF2DBc::ltagew(1)){//notTooOld(>40ns befFT), notTooYoung(<640 befFT)
	  ttdc[isid][nttdc[isid]]=t1;
          ittdc[isid][nttdc[isid]]=i;//position in stdc[is][]-array
	  nttdc[isid]+=1;
	  itftcor[isid][i]=1;//mark FT-correlated LTtime-hits
	}
      }
      if(TFREFFKEY.reprtf[4]>0){
        cout<<"     ttdc-array of FT-matched LT(stdc)-hits:"<<endl;
        for(int ih=0;ih<nttdc[isid];ih++)cout<<"       ttdc:"<<ttdc[isid][ih]<<endl;
      }
//
      charg[isid]=ptr->getcharg();
      adca[isid]=ptr->getadca();//here all ADCs ARE ped-subtracted/suppressed !!!
      if(adca[isid]>0)nadca[isid]+=1;
      nadcd[isid]=ptr->getadcd(adcd[isid]);
      if(nttdc[isid]>0)TOF2JobStat::addch(chnum,1);
      if(nhtdc[isid]>0)TOF2JobStat::addch(chnum,2);
      if(nshtdc[isid]>0)TOF2JobStat::addch(chnum,3);
      if(nadca[isid]>0)TOF2JobStat::addch(chnum,4);
      if(nadcd[isid]>0)TOF2JobStat::addch(chnum,5);
      if(nftdc[isid]==1)TOF2JobStat::addch(chnum,6);
      if(nttdc[isid]==1)TOF2JobStat::addch(chnum,7);
      if(nttdc[isid]>0 && (nadca[isid]>0 || nadcd[isid]>0))TOF2JobStat::addch(chnum,8);
      if(nttdc[isid]>0 && (nadca[isid]>0 || nadcd[isid]>0)
                                        && nhtdc[isid]>0)TOF2JobStat::addch(chnum,11);
    }// endof side-/DB-status check ---> 
//---
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getsid())/10; //next hit short_id (LBB)
//------------------------------------------------------
//aaa--->
    if(idN != id){ // both sides ready, next hit is NEW_counter_hit/last_hit,
//       so process CURRENT counter data : 
//---
      if(isds==2 || isds==1){ // alive sides presence check
        TOF2JobStat::addbr(brnum,0);//bar entries(at least one alive(in DB) side is present )
//
// =============> start pattern recognition for given sc.bar :
//
// --------> select bar with >= 1x4-multiplicity in stdc, >= 1x2 in ftdc and A||D-adc >0 
//(first pulse from readout (last in real time) will be used if many f/stdc are present): 
//
        smty[0]=0;
        smty[1]=0;
        SumHTuse=TFREFFKEY.relogic[1];// use/not(1/0) SumHT-channel for matching with LTtime-channel 
        if(nttdc[0]>0  
	    && (nadca[0]>0 || nadcd[0]>0)
	    && (nhtdc[0]>0 || SumHTuse==0))smty[0]=1;//side has complete t,amp,[hist] measurement
        if(nttdc[1]>0  
	    && (nadca[1]>0 || nadcd[1]>0)
	    && (nhtdc[1]>0 || SumHTuse==0))smty[1]=1;//
//----------------------
//bbb--->
        if(smty[0]==1 || smty[1]==1){ //check side-measurement completeness(accept 1-sided) 
          TOF2JobStat::addbr(brnum,1);
          isds=smty[0]+smty[1];// number of sides with complete measurements
          sta=0;
// -> add status-bits for known(from DB) counter with time-measurement problem:
//          if(!(TOF2Brcal::scbrcal[ilay][ibar].SchOK(0)&&TOF2Brcal::scbrcal[ilay][ibar].SchOK(1)))
//	                                sta|=TOFGC::SCBADB3;//mark bad t-measurement on any side(acc.to DB)

          if(isds==1){
            sta|=TOFGC::SCBADB2;// set bit for counter with only one-side measurements
            if(smty[1]==0)sta|=TOFGC::SCBADB4;// bad_side_number bit(s1->reset_bit,s2->set_bit)
          }
//
//--------------> identify "corresponding"(to SumHT-TDC) hit in LT-TDC :
//
// -----> histogr. for events with single SumHT/LT-TDC(FT-correlated) hit :
//
          if(TFREFFKEY.reprtf[2]>0){
	    if(smty[0]>0)HF1(1108,float(nhtdc[0]),1.);
	    if(smty[1]>0)HF1(1108,float(nhtdc[1]),1.);
//
            if(smty[0]>0 && nhtdc[0]==1 && nttdc[0]==1){
              dt=ttdc[0][0]-htdc[0][0];//LTtime-SumHTtime
              HF1(1100,geant(dt),1.);//histogr.the same hit LT/SumHT-TDC difference
            }
            if(smty[1]>0 && nhtdc[1]==1 && nttdc[1]==1){
              dt=ttdc[1][0]-htdc[1][0];//LTtime-SumHTtime
              HF1(1100,geant(dt),1.);//histogr. the same hit LT/SumHT-TDC difference
            }
          }
//
// -----> loop over all LT(stdc)/SumHT(htdc)-hits to find all/best matching pairs:
//
	  for(isd=0;isd<2;isd++){//side loop
            if(smty[isd]>0){//side has complete measurement
              for(i=0;i<nstdc[isd];i++){ // all LTtime-hits loop
	        dtmin[isd][i]=9999;
	        itmatch[isd][i]=-1;
	        for(j=0;j<nhtdc[isd];j++){ // all SumHTtime-hits-loop
                  dt=stdc[isd][i]-htdc[isd][j];
                  if(fabs(dt) < TOF2Varp::tofvpar.fstdw()){//check match window
		    if(fabs(dt)<dtmin[isd][i]){//find best matching(to choose optimal match window)
		      dtmin[isd][i]=fabs(dt);
		      itmatch[isd][i]=j;//mark matched LTtime-hits(stdc) with index of best matched SumHT-hit
		    }
		  }
		}//--->endof SumHT-hits loop
                if(itftcor[isd][i]==1 && TFREFFKEY.reprtf[2]>0)
		                    HF1(1115,geant(dtmin[isd][i]),1.);//hist.for FT-correlated LT-hits
              }//--->endof LTtime-hits loop
            }
	    if(TFREFFKEY.reprtf[4]>0){
	      cout<<"    LTtime(stdc) best SumHT-matched hit-indexes for side="<<isd<<":"<<endl;
	      for(i=0;i<nstdc[isd];i++)if(itmatch[isd][i]>=0)cout<<"     "<<i<<"("<<itmatch[isd][i]<<")"<<endl;  
	    }
          }                          
//
//-----> select TRUE FT-correlated LTtime-hit(in ttdc) and, if requested, SumHT-matched :
//
          number dtbest[isd];
          for(isd=0;isd<2;isd++){
	    tmbest[isd]=0;
	    itmbest[isd]=-1;
	    dtbest[isd]=9999;
	    if(smty[isd]>0){
	      if(nttdc[isd]==1){//single FT-correlated LT-hit case,just use it
	        tmbest[isd]=ttdc[isd][0];
		itmbest[isd]=ittdc[isd][0];//its index in stdc-array
              }
	      else{//many FT-correlated LT-hits(ttdc-array)
	        if(SumHTuse==1){//try to use best matched hit, if SumHTuse
		  for(i=0;i<nttdc[isd];i++){//FT-correlated LTtime-hits loop
		    dt=dtmin[isd][ittdc[isd][i]];//best matching for this hit
	            if(dt<dtbest[isd]){
		      itmbest[isd]=ittdc[isd][i];//its index in stdc-array
		      dtbest[isd]=dt;
	              tmbest[isd]=ttdc[isd][i];
		    }
		  }
	        }
	        else{//noMatch-info - use 1st(youngest) hit
	          tmbest[isd]=ttdc[isd][0];
	          itmbest[isd]=ittdc[isd][0];//its index in stdc-array
//	                 tmbest[isd]=ttdc[isd][nttdc[isd]-1];//use last(oldest)
//	                 itmbest[isd]=ittdc[isd][nttdc[isd]-1];
                }
	      }//--->endof "many FT-corr. LT-hits" case
	    }
	    if(TFREFFKEY.reprtf[4]>0){
	      cout<<"    Side:"<<isd<<endl;
	      cout<<"     TRUE FT-corr LT-hit time="<<tmbest[isd]<<" ,its stdc-index="<<itmbest[isd]<<endl;
	      cout<<"     Its matching index(best-match SumHT-hit index)="<<itmatch[isd][itmbest[isd]]<<endl;
	    }   
	  }
//------------>        
          rej1=0;
          if(SumHTuse==1){//require matching of TRUE LT-hit
            if((itmatch[0][itmbest[0]]<0 && smty[0]==1) ||
              (itmatch[1][itmbest[1]]<0 && smty[1]==1))rej1=1;//found NO_MATCHING on any of complete(3-meas) sides
	      if(TFREFFKEY.reprtf[4]>0 && rej1==1)
	                          cout<<"    No TrueLThit matching on any of complete sides"<<endl;
          }
//---
//
//-----------> check "befor"/"after"(wrt TRUE LT-hit) hits presence(clean history check)  :
//            (look into stdc[matched,if required]-hits !!!)
//
          rej2=0;
	  rej3=0;
          for(isd=0;isd<2;isd++){
	    if(smty[isd]>0){//side with complete measurement
	      if(itmbest[isd]>0){//check that "best" LT-hit is not the 1st one in stdc-arr
	        ibef=itmbest[isd]-1;//previous(to "best") LT-hit(stdc)
		for(i=ibef;i>=0;i--){//find 1st LT-hit which is: (matched,if requested) and too close to "best" 
		  tbef=stdc[isd][ibef];
		  dt=tbef-tmbest[isd];//dist befor the "best"
		  if((itmatch[isd][i]>=0 || SumHTuse==0) && dt<TOF2Varp::tofvpar.hiscutb()){
                    if(TFREFFKEY.reprtf[2]>0)HF1(1101,geant(dt),1.);
		    rej2=1;
		    break;
		  }
		}
	      }
	      if(itmbest[isd]<(nstdc[isd]-1)){//check that "best" hit is not the last in stdc-arr
	        iaft=itmbest[isd]+1;//next(to "best") LT-hit
		for(i=iaft;i<nstdc[isd];i++){//find 1st LT-hit which is: (matched,if requested) and too close to "best"
		  taft=stdc[isd][iaft];
		  dt=tmbest[isd]-taft;//dist after the "best"
		  if((itmatch[isd][i]>=0 || SumHTuse==0) && dt<TOF2Varp::tofvpar.hiscuta()){
                    if(TFREFFKEY.reprtf[2]>0)HF1(1102,geant(dt),1.);
		    rej3=1;
		    break;
		  }
		}
	      }
	    }//--->endof smty>0 check
          }//--->endof sides loop
//
//===========>>> set time-history status of sc.bar :
//
            if(rej2==1 || rej3==1)sta|=TOFGC::SCBADB1;// set bit "bad time-history" on any of complete sides
	    if(rej1==1)sta|=TOFGC::SCBADB6;// set bit "no TrueLT-hit/SumHT matching"(if required) on any of the complete sides
            if(rej2==0 && rej3==0)TOF2JobStat::addbr(brnum,2);//"good time-history" on each of complete sides
            if(rej1==0)TOF2JobStat::addbr(brnum,3);//"True LT-hit" matching OK(when requested) on each of complete sides
            if(rej1==0&&rej2==0&&rej3==0 && isds==2)TOF2JobStat::addbr(brnum,4);//all is OK on both sides
	    if(TFREFFKEY.reprtf[4]>0 && (rej2==1 || rej3==1))
	                                   cout<<"    Bad time-history on any of complete sides !"<<endl;
//
//===========>>> calculate times/Edeps of sc.bar :
//
//---> Anode-Time/Charge reconstruction:
//
            chnum=ilay*TOF2GC::SCMXBR*2+ibar*2;//s1-channel number 
            tm[0]=0;
            ama[0]=0.;
            TOFBPeds::scbrped[ilay][ibar].getsiga(sigs);// get anode-adc sig(s1/2)
            if(smty[0]==1){// good S1(3-measurement) side, but matching/history may not be good
              tm[0]=tmbest[0];// s-1 time (ns,A-noncorr)
              tmf[0]=tm[0];
	      if(nadca[0]>0){//Anode,s1
                ama[0]=number(adca[0]);//ADC-counts(float)(anode s1)
                if(TFREFFKEY.reprtf[2]>0 && id==104)HF1(1104,geant(ama[0]),1.);
		if((ama[0]+3*sigs[0])>=number(TOF2GC::SCPUXMX)){//check PUX-ovfl
		  ama[0]=0;//mark ovfl
		  TOF2JobStat::addch(chnum,9);//counts Anode-cnan. overflows
		}
	      }
            }
//
            tm[1]=0;
            ama[1]=0.;
            if(smty[1]==1){// good S2(3-measurement) side, but matching/hist may not be good
              tm[1]=tmbest[1];// s-2 time (ns,A-noncorr)
              tmf[1]=tm[1];
	      if(nadca[1]>0){//Anode,s2
                ama[1]=number(adca[1]);//ADC-counts(float)(anode s2)
		if((ama[1]+3*sigs[1])>=number(TOF2GC::SCPUXMX)){//check PUX-ovfl
		  ama[1]=0;//mark ovfl
		  TOF2JobStat::addch(chnum+1,9);//counts Anode-cnan. overflows
		}
	      }
            }
            if(smty[0]==0){ //set "=" s1&s2 times and ampl. for 1-sided case:
              tmf[0]=tmf[1];
              ama[0]=ama[1];
            }
            if(smty[1]==0){
              tmf[1]=tmf[0];
              ama[1]=ama[0];
            }
	    anchok=1;
	    if(ama[0]==0.|| ama[1]==0.){//set s1&s2->0(no Anode info) if any side-overflow
	      ama[0]=0.;
	      ama[1]=0.;
	      anchok=0;
	    }
//
//---> Dynode-charge reconstruction:
//
            npmts=TOF2Brcal::scbrcal[ilay][ibar].getnpm();
	    for(int is=0;is<2;is++){
	      for(int ip=0;ip<TOF2GC::PMTSMX;ip++){
		adcdr[is][ip]=0;
	      }
	    }
//
            amd[0]=0.;
	    dovfl=0;
            if(smty[0]>0){// <===S1
	      if(nadcd[0]>0){//combine Dynode pm-signals into S1 joined Dyn-signal
	        for(int ip=0;ip<npmts;ip++){//<--s1 pm-loop
		  if(adcd[0][ip]>0){
                    TOFBPeds::scbrped[ilay][ibar].getsigd(ip,sigs);//Dyn sig(s1/2)
		    gnd=TOF2Brcal::scbrcal[ilay][ibar].getgnd(0,ip);
		    aaa=number(adcd[0][ip]);//ADC-counts(float, vs pmt)
		    if((aaa+3*sigs[0])>=number(TOF2GC::SCPUXMX))dovfl=1;//Dyn(pm) s1-overflow
		    amd[0]+=(aaa/gnd);//Dyn(equilised)
                    if(TFREFFKEY.reprtf[2]>0 && id==104)HF1(1105,geant(aaa),1.);
		  }
		}//-->endof pm-loop
		if(dovfl>0)TOF2JobStat::addch(chnum,10);//counts s1 Dyn-cnan. overflows
	      }
	    }
//
            amd[1]=0.;
	    dovfl=0;
            if(smty[1]>0){// <===S2
	      if(nadcd[1]>0){//combine Dynode pm-signals into S2 joined Dyn-signal
	        for(int ip=0;ip<npmts;ip++){//<--s2 pm-loop
		  if(adcd[1][ip]>0){
                    TOFBPeds::scbrped[ilay][ibar].getsigd(ip,sigs);//Dyn sig(s1/2)
		    gnd=TOF2Brcal::scbrcal[ilay][ibar].getgnd(1,ip);
		    aaa=number(adcd[1][ip]);//ADC-counts(float, vs pmt)
		    if((aaa+3*sigs[1])>=number(TOF2GC::SCPUXMX))dovfl=1;//Dyn(pm) s2-overflow
		    amd[1]+=(aaa/gnd);//Dyn(equilised)
		  }
		}//-->endof pm-loop
		if(dovfl>0)TOF2JobStat::addch(chnum+1,10);//counts s2 Dyn-cnan. overflows
	      }
            }
//
            if(smty[0]==0)amd[0]=amd[1];//set "=" s1&s2 D-ampl. for 1-sided case:
            if(smty[1]==0)amd[1]=amd[0];
	    dychok=1;
	    if(amd[0]==0.|| amd[1]==0.){//missing Dynode info
	      amd[0]=0.;
	      amd[1]=0.;
	      dychok=0;
	    }
//----
            zc=TOF2DBc::getzsc(ilay,ibar);
            blen=TOF2DBc::brlen(ilay,ibar);
//ccc------->
            if(TFREFFKEY.relogic[0] != 1
	              && (anchok+dychok)>0){//<--require any signal>0(nonovfl) and NOT SRS-calib
	      if(anchok){
	        amc[0]=ama[0];
	        amc[1]=ama[1];
		hlf=0;
	      }
	      else{
	        amc[0]=amd[0];
	        amc[1]=amd[1];
		hlf=1;
	      }
//
//--> Calc. longit. coord/err and position correction to charge-signal :
//
              TOF2Brcal::scbrcal[ilay][ibar].tmd2p(tmf,amc,hlf,co,eco);// get A-corrected Local(!).coo/err
	      dt=-0.5*(tmf[0]-tmf[1]);
//   cout<<"l/b="<<ilay<<" "<<ibar<<" Td="<<dt<<" co="<<co<<endl; 
              brlm=0.5*TOF2DBc::brlen(ilay,ibar)+3.*eco;//limit on max. coord
              if(fabs(co) > brlm){   //means: "coord. is more than counter half length"
                pcorr=TOF2Brcal::scbrcal[ilay][ibar].poscor(0.);// take position corr. as for "0"
              }
              else{
                pcorr=TOF2Brcal::scbrcal[ilay][ibar].poscor(co);
              }
              if(isds==1)pcorr=1.;// no position correction for one-sided counters
//
//--> Find counter-time, corrected for slewing(according to ADC-signal) :
//
              time=TOF2Brcal::scbrcal[ilay][ibar].tm2t(tmf,amc,hlf); // time with corrections
//
//--> Find Eloss from Anode-adc(if present) :
//
              elosa=0.;
              if(anchok)elosa=TOF2Brcal::scbrcal[ilay][ibar].adc2mip(0,ama)
                                             /pcorr; //Anode Edep(mev)(normalized to center)
              aedep=elosa;
//
//--> Find Eloss from combined Dynode(h+l)-adc :
//
              elosd=0.;
              if(dychok)elosd=TOF2Brcal::scbrcal[ilay][ibar].adc2mip(1,amd)
                                            /pcorr;//Dynode Edep(mev)(normalized to center)
              dedep=elosd;
//---> store raw(adc-counts, but float) ADCs in RawCl-obj for calibr.purpose:
	      for(int ip=0;ip<npmts;ip++){
		if(nadcd[0]>0)adcdr[0][ip]=number(adcd[0][ip]);
		if(nadcd[1]>0)adcdr[1][ip]=number(adcd[1][ip]);
              }
//
//-->
              nbrl[ilay]+=1;
              isdsl[ilay]=isds;
              edepa[ilay]=aedep;
              edepd[ilay]=dedep;
              tcorr[ilay]=time;
              tuncorr[ilay]=0.5*(tmf[0]+tmf[1]);// layer A-non.cor. time
              tdiff[ilay]=0.5*(tmf[0]-tmf[1]);// layer A-non.cor. time-diff.(ns) 
              pch1[ilay]=geant(charg[0]);
              pch2[ilay]=geant(charg[1]);
              brnl[ilay]=ibar;
              clong[ilay]=co;
              st=0;
              coo=co;// Local coord.!!!
              ecoo=eco;
              if(isds==1)ecoo=blen/sqrt(12.);//for single-sided counters
              if(AMSEvent::gethead()->addnext(AMSID("TOF2RawCluster",0)
                        ,new TOF2RawCluster(sta,ilay+1,ibar+1,zc,ama,amd,adcdr,
                              aedep,dedep,tm,time,coo,ecoo)))st=1;;//store value
//	      cout<<"StoreNewRawCl(il/ib)="<<ilay+1<<" "<<ibar+1<<" time="<<time<<endl;
//	      cout<<"      sta/Aedep/coo="<<sta<<" "<<aedep<<" "<<coo<<endl;
            } // ccc------> end of SRS-calib bypass check
//-----------
        } // bbb---> end of "side measurement-completenes" check(smty[0] || smty[1] >0)
//---------
      } //---> end of side presence check
//--------
      isds=0;// clear side-hits counters befor next bar processing
      nftdc[0]=0;
      nftdc[1]=0;
      nstdc[0]=0;
      nstdc[1]=0;
      nadca[0]=0;
      nadca[1]=0;
      nadcd[0]=0;
      nadcd[1]=0;
      nhtdc[0]=0;
      nhtdc[1]=0;
      nshtdc[0]=0;
      nshtdc[1]=0;
      nttdc[0]=0;
      nttdc[1]=0;
//
    } // aaa---> end of "next COUNTER" or "last hit" check
//------------------------------------------------------
    ptr=ptr->next();// take next RawSide hit
//
  } //                ---- end of RawSide hits loop ------->
//
//
// ---> now check min. multiplicity and make some histograms :
//
  int nbrch[TOF2GC::SCLRS],conf(-1),isum(0);
  int il,ib,ix,iy;
  geant x[2],y[2],zx[2],zy[2],zcb[TOF2GC::SCLRS],tgx,tgy,cosi;
  number trlen13,trlen24;
//
  for(i=0;i<TOF2GC::SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]>0)nbrch[i]=1;
  }
  for(i=0;i<TOF2GC::SCLRS;i++)isum+=nbrch[i];
  HF1(1110,geant(isum),1.);// tot.number of layers
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){

    for(i=0;i<TOF2GC::SCLRS;i++)if(nbrch[i]>0)HF1(1111,geant(i+1),1.);// layer appear. freq.
  }
  if(isum>=2)conf=0;
  if(isum>=3){
    for(i=0;i<TOF2GC::SCLRS;i++)if(nbrch[i]==0)conf=i+1;
  }
  if(isum==4)conf=5;
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
    HF1(1112,geant(conf),1.);
  }
//
  if((nbrch[0]+nbrch[1]) + (nbrch[2]+nbrch[3])>=1)ostatus=0; // good event
// 
// ---> multtipl. checks for single bar layers:
//
  for(i=0;i<TOF2GC::SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]==1)nbrch[i]=1;
  }
  isum=0;
  conf=-1;
  for(i=0;i<TOF2GC::SCLRS;i++)isum+=nbrch[i];
  if(isum>=2)conf=0;
  if(isum>=3){
    for(i=0;i<TOF2GC::SCLRS;i++)if(nbrch[i]==0)conf=i+1;
  }
  if(isum==4)conf=5;
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
    HF1(1113,geant(conf),1.);
  }
//
  if(ostatus!=0)return;//remove bad events
//
  if(conf != 5)return;//use only 1bar/layer,4-layer events for next processing
//
//--------------> try to recover 1-sided counters
//
  integer status;
  number crc;
  geant cry[TOF2GC::SCLRS];
  TOF2RawCluster *ptrc;
  ptrc=(TOF2RawCluster*)AMSEvent::gethead()->
                                     getheadC("TOF2RawCluster",0);
//
// --> find track-counter crossing points(longit) using counter transv.coord :
//
  ix=0;
  iy=0;
  for(il=0;il<TOF2GC::SCLRS;il++){
    ib=brnl[il];
    zcb[il]=TOF2DBc::getzsc(il,ib);
    if(TOF2DBc::plrotm(il)==0){// unrotated (X-meas) planes
      x[ix]=TOF2DBc::gettsc(il,ib);
      zx[ix]=zcb[il];
      ix+=1;
    }
    else{                    // rotated (Y-meas) planes
      y[iy]=TOF2DBc::gettsc(il,ib);
      zy[iy]=zcb[il];
      iy+=1;
    }
  }
  tgx=(x[0]-x[1])/(zx[0]-zx[1]);// track slopes in 2 projections
  tgy=(y[0]-y[1])/(zy[0]-zy[1]);
//
  for(il=0;il<TOF2GC::SCLRS;il++){
    ib=brnl[il];
    if(TOF2DBc::plrotm(il)==0){// unrotated (X-meas) planes
      crc=y[1]+tgy*(zcb[il]-zy[1]);// long. crossing coord. in abs. ref.syst.
      cry[il]=crc;//x-crossing in local syst. 
    }
    else{                     // rotated (Y-meas) planes
      crc=x[1]+tgx*(zcb[il]-zx[1]);// long. crossing coord. in abs. ref.syst.
      cry[il]=-crc;//y-crossing in local syst. 
    }
  }
//
// ----------> recover missing sides
//
  number stin[2],stout[2],timin,timout,edin,edout,clin,clout;//tempor for test
  integer isdb(1),isdg;// bad/good side
  number ddt;
  if(TFREFFKEY.relogic[3]){// do recovering of missing side
    while(ptrc){ // loop over counters(raw clusters)
      il=ptrc->getntof()-1;
      ib=ptrc->getplane()-1;
      status=ptrc->getstatus();
      if((status&TOFGC::SCBADB2)>0){ // 1-sided counter found -> try to recover it
        ptrc->recovers(cry[il]);// recovering
        status=ptrc->getstatus();//new status
        if((status & TOFGC::SCBADB5)>0){
          isdsl[il]+=1;//add recovered side
          tcorr[il]=ptrc->gettime();
        }
      }
      ptrc=ptrc->next();
    }
  }
//--------------
// 
// ---> multtipl. checks for single 2-sided bar layers:
//
  for(i=0;i<TOF2GC::SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]==1 && isdsl[i]==2)nbrch[i]=1;
  }
  isum=0;
  conf=-1;
  for(i=0;i<TOF2GC::SCLRS;i++)isum+=nbrch[i];
  if(isum>=2)conf=0;
  if(isum>=3){
    for(i=0;i<TOF2GC::SCLRS;i++)if(nbrch[i]==0)conf=i+1;
  }
  if(isum==4)conf=5;
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
    HF1(1114,geant(conf),1.);
  }
//
// -> make hist. only for 4layer x 1bar(2-sided) events:
//
  if(conf != 5)return;
//  
  cosi=sqrt(1.+tgx*tgx+tgy*tgy);// this is 1/cos(theta) !!!
  trlen13=(zcb[0]-zcb[2])*cosi;//1->3
  trlen24=(zcb[1]-zcb[3])*cosi;//2->4
  td13=(tcorr[0]-tcorr[2]);
  td24=(tcorr[1]-tcorr[3]);
  td14=tuncorr[0]-tuncorr[3];
//
  geant zpl1,zpl2,trlnor;
  zpl1=TOF2DBc::supstr(1)-
        (TOF2DBc::plnstr(1)+TOF2DBc::plnstr(6)/2+TOF2DBc::plnstr(3)/2);//z-l1-middl
  zpl2=TOF2DBc::supstr(1)-
        (TOF2DBc::plnstr(2)+TOF2DBc::plnstr(6)/2+TOF2DBc::plnstr(3)/2);//z-l2-middl
  trlnor=zpl1+zpl2;//z-dist. L1-L3(L2-L4)
//
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
      HF1(1532,td13*trlnor/trlen13,1.);//ToF for L0->L2, normalized to trlnor
      HF1(1534,td24*trlnor/trlen24,1.);//ToF for L1->L3, ....................
      gdt=(td13/trlen13-td24/trlen24)*trlnor;// normalized to fix(~127cm) dist. for AMS02
//                                                           (130cm for AMS01)
      HF1(1544,gdt,1.);
      if(TFREFFKEY.reprtf[2]>1){ 
        for(il=0;il<TOF2GC::SCLRS;il++){// fill de/dx hist. for each bar
          ib=brnl[il];
          brnum=il*TOF2GC::SCMXBR+ib;//sequential bar numbering
          elosn=edepa[il]/cosi;//normalized  to norm.inc.
          HF1(1140+brnum,elosn,1.);
        }
        HF1(1526,edepa[0],1.); //layer=0 Anode-reconstructed Edep
        HF1(1527,edepa[2],1.); //layer=3 Anode-reconstructed Edep
        HF1(1528,edepd[0],1.); //layer=0 Dinode-reconstructed Edep
        HF1(1529,edepd[2],1.); //layer=3 Dinode-reconstructed Edep
        if(AMSJob::gethead()->isSimulation()){
          geant tch;
          charg[0]=pch1[0];
          charg[1]=pch2[0];
          tch=charg[0]+charg[1];
          HF1(1071,tch,1.);
          HF1(1072,tch,1.);
        }
      }
  }
}
//-----------------------------------------------------------------------
void TOF2RawCluster::recovers(number x){ // function to recover missing side
  geant gn[2],gnd[2],csl,sl[2],dt0,upr[2*TOF2GC::SCPROFP],vel,xerr,sqr,hclen,co,eco,pcorr;
  number q[2],tm[2],tcor;
  number tot[2],adc[2];
  int il,ib,hlf;
  int isg(1),isb(0);
//
  if(_status&TOFGC::SCBADB4){
    isg=0;
    isb=1;
  }
  il=_ntof-1;
  ib=_plane-1;
  TOF2Brcal::scbrcal[il][ib].getgaina(gn);
  TOF2Brcal::scbrcal[il][ib].getupar(upr);
  TOF2Brcal::scbrcal[il][ib].getd2p(vel,xerr);
  TOF2Brcal::scbrcal[il][ib].getslops(sl);
  csl=TOF2Brcal::scbrcal[il][ib].getslope();
  dt0=TOF2Brcal::scbrcal[il][ib].gettdif();
  hclen=0.5*TOF2DBc::brlen(il,ib);
  if(fabs(x) > (hclen+5.)){// out of range by more than 5cm
    pcorr=TOF2Brcal::scbrcal[il][ib].poscor(0.);// no correction (==1)
  }
  else{
    pcorr=TOF2Brcal::scbrcal[il][ib].poscor(geant(x));
  }
// ---> recover Anode-signal for bad side :
  if(_edepa>0. && _adca[isg]>0){
    hlf=0;
    tot[isg]=_adca[isg];
    TOF2Brcal::scbrcal[il][ib].q2a2q(1,isg,hlf,tot[isg],q[isg]);//ADC->Q
    q[isg]/=gn[isg];// Q->Qref
    sqr=(upr[0]*((1-upr[2])*exp(-(hclen+x)/upr[1])+upr[2]*exp(-(hclen+x)/upr[3]))+
         upr[4]*exp(-(hclen-x)/upr[5]))/
	(upr[6]*((1-upr[8])*exp(-(hclen-x)/upr[7])+upr[8]*exp(-(hclen-x)/upr[9]))+ 
         upr[10]*exp(-(hclen+x)/upr[11]));//Qr1/Qr2
    if(isb==1)sqr=1./sqr;
    q[isb]=q[isg]*sqr;//s1/s2 ratio correction(vs imp.point)
    q[isb]*=gn[isb];//Qref->Q
    TOF2Brcal::scbrcal[il][ib].q2a2q(0,isb,hlf,tot[isb],q[isb]);//Q->ADC
    _adca[isb]=tot[isb]; // recovered ADC for bad side
    _edepa=TOF2Brcal::scbrcal[il][ib].adc2mip(hlf,tot)/pcorr; //recowered anode-Eloss
    tcor=2.*csl*(sl[0]/sqrt(q[0])-sl[1]/sqrt(q[1]));//slewing corrections for time
    adc[0]=tot[0];
    adc[1]=tot[1];
  }
//
// ---> recover Dynode(pm-combined)-signal for bad side :
  if(_edepd>0 && _adcd[isg]>0){
    tot[isg]=_adcd[isg];
    hlf=1;
    TOF2Brcal::scbrcal[il][ib].q2a2q(1,isg,hlf,tot[isg],q[isg]);//ADC(isg)->Q(pmt-comb,equilized)
    q[isg]/=gn[isg];// Q->Qref
    sqr=(upr[0]*((1-upr[2])*exp(-(hclen+x)/upr[1])+upr[2]*exp(-(hclen+x)/upr[3]))+
         upr[4]*exp(-(hclen-x)/upr[5]))/
	(upr[6]*((1-upr[8])*exp(-(hclen-x)/upr[7])+upr[8]*exp(-(hclen-x)/upr[9]))+ 
         upr[10]*exp(-(hclen+x)/upr[11]));//Qr1/Qr2
    if(isb==1)sqr=1./sqr;
    q[isb]=q[isg]*sqr;//s1/s2 ratio correction(vs imp.point)
    q[isb]*=gn[isb];//Qref->Q
    TOF2Brcal::scbrcal[il][ib].q2a2q(0,isb,hlf,tot[isb],q[isb]);//Q->ADC(isb)
    _adcd[isb]=tot[isb]; // recovered ADC for bad side
    _edepd=TOF2Brcal::scbrcal[il][ib].adc2mip(hlf,tot)/pcorr; //recowered dinode-Eloss
    if(_edepa==0.){//slew.cor. from dynode-chan., if anode-chan. is missing
      tcor=2.*csl*(sl[0]/sqrt(q[0])-sl[1]/sqrt(q[1]));//slewing corrections for time
      adc[0]=tot[0];
      adc[1]=tot[1];
    }
  }
//
// ---> recover time :
  tm[isg]=_sdtm[isg];
  tm[isb]=tm[isg]-(1-2*isg)*(2.*dt0-2.*x/vel-tcor);
  _sdtm[isb]=tm[isb]; // recowered side time(raw)
  _time=TOF2Brcal::scbrcal[il][ib].tm2t(tm,tot,hlf); // recovered time(with corr)
  TOF2Brcal::scbrcal[il][ib].tmd2p(tm,adc,hlf,co,eco);// get A-corrected Local(!).coord/err
  _timeD=co;// recovered loc. coordinate (chould be = x)
  _etimeD=3.5;// tempor err.on longit.coord(cm) for 1-sided,recovered counters
//
  _status|=TOFGC::SCBADB5;//add bit for recovered counters
}
//-----------------------------------------------------------------------
void TOF2RawCluster::_writeEl(){

  if(TOF2RawCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
// Fill the ntuple
    TOFRawClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_tofraw();
    if (TN->Ntofraw>=MAXTOFRAW) return;
    TN->Status[TN->Ntofraw]=_status;
    TN->Layer[TN->Ntofraw]=_ntof;
    TN->Bar[TN->Ntofraw]=_plane;
    for(int i=0;i<2;i++){
      TN->adca[TN->Ntofraw][i]=_adca[i];
      TN->adcd[TN->Ntofraw][i]=_adcd[i];
      for(int ip=0;ip<TOF2GC::PMTSMX;ip++)TN->adcdr[TN->Ntofraw][i][ip]=_adcdr[i][ip];
      TN->sdtm[TN->Ntofraw][i]=_sdtm[i];
    }
    TN->edepa[TN->Ntofraw]=_edepa;
    TN->edepd[TN->Ntofraw]=_edepd;
    TN->time[TN->Ntofraw]=_time;
    TN->cool[TN->Ntofraw]=_timeD;
    TN->Ntofraw++;
  }
}
//------

void TOF2RawCluster::_copyEl(){
}

//------
void TOF2RawCluster::_printEl(ostream & stream){
  stream <<"TOF2RawCluster "<<_ntof<<" "<<_edepa<<" "<<_plane<<endl;
}
//----------------------------------
void TOF2RawSide::_copyEl(){
}

void TOF2RawSide::_writeEl(){
  if(TOF2RawSide::Out( IOPA.WriteAll%10==1 || TFREFFKEY.relogic[4]==1)){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
// Fill the ntuple
    TofRawSideNtuple* TN = AMSJob::gethead()->getntuple()->Get_tofraws();
    if (TN->Ntofraws>=MAXTOFRAWS) return;
    TN->swid[TN->Ntofraws]=_swid;
    TN->hwid[TN->Ntofraws]=_hwid;
    TN->stat[TN->Ntofraws]=_status;
    TN->nftdc[TN->Ntofraws]=_nftdc;
    for(int i=0;i<_nftdc;i++)TN->ftdc[TN->Ntofraws][i]=_ftdc[i];
    TN->nstdc[TN->Ntofraws]=_nstdc;
    for(int i=0;i<_nstdc;i++)TN->stdc[TN->Ntofraws][i]=_stdc[i];
    TN->nsumh[TN->Ntofraws]=_nsumh;
    for(int i=0;i<_nsumh;i++)TN->sumht[TN->Ntofraws][i]=_sumht[i];
    TN->nsumsh[TN->Ntofraws]=_nsumsh;
    for(int i=0;i<_nsumsh;i++)TN->sumsht[TN->Ntofraws][i]=_sumsht[i];
    TN->adca[TN->Ntofraws]=_adca;
    TN->nadcd[TN->Ntofraws]=_nadcd;
    for(int ip=0;ip<TOF2GC::PMTSMX;ip++)TN->adcd[TN->Ntofraws][ip]=_adcd[ip];
    TN->temp[TN->Ntofraws]=_temp;
    TN->Ntofraws++;
  }
}
//----------------------------------
void AMSTOFCluster::_writeEl(){

  // p2memb for Root
  if(AMSTOFCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  TOFClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_tof();

  if (TN->Ntof>=MAXTOF) return;

// Fill the ntuple
    TN->Status[TN->Ntof]=_status;
    TN->Layer[TN->Ntof]=_ntof;
    TN->Bar[TN->Ntof]=_plane;
    TN->Nmemb[TN->Ntof]=_nmemb;
    TN->Edep[TN->Ntof]=_edep;
    TN->Edepd[TN->Ntof]=_edepd;
    TN->Time[TN->Ntof]=_time;
    TN->ErrTime[TN->Ntof]=_etime;
    int i;
    for(i=0;i<3;i++)TN->Coo[TN->Ntof][i]=_Coo[i];
    for(i=0;i<3;i++)TN->ErrorCoo[TN->Ntof][i]=_ErrorCoo[i];
    if(TOF2RawCluster::Out(IOPA.WriteAll%10==1)){//WriteAll
     for(int i=0;i<3;i++)TN->P2memb[TN->Ntof][i]=0;
      for(int i=0;i<_nmemb;i++) {
        TN->P2memb[TN->Ntof][i]=_mptr[i]->getpos();
      }
    }
    else{// Used
      integer mpos;
      TOF2RawCluster *ptr;
      for(int i=0;i<_nmemb;i++) {
        mpos=_mptr[i]->getpos();
	ptr=(TOF2RawCluster*)AMSEvent::gethead()->getheadC("TOF2RawCluster",0);
        for(int j=0;j<mpos;j++){
          if(ptr && ptr->checkstatus(AMSDBc::USED))TN->P2memb[TN->Ntof][i]++;
          ptr=ptr->next();
        }
      }
    }
    TN->Ntof++;
  }
}



void AMSTOFCluster::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
 TofClusterR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->TofCluster(_vpos);
      for(int i=0;i<_nmemb;i++)(ptr.fTofRawCluster).push_back(_mptr[i]->GetClonePointer());
#endif
}


void AMSTOFCluster::_printEl(ostream & stream){
  stream <<"AMSTOFCluster "<<_status<<" "<<_ntof<<" "<<_plane<<" "<<_edep<<" "
//  <<_Coo<<  " "<<_ErrorCoo<<" "<<_time<<" " <<_etime<<endl;
  <<_edepd<<" "<<_Coo<<  " "<<_ErrorCoo<<" "<<_time<<" " <<_etime<<endl;
}


 
void AMSTOFCluster::print(){
for(int i=0;i<4;i++){
AMSContainer *p =AMSEvent::gethead()->getC("AMSTOFCluster",i);
 if(p)p->printC(cout);
}
}

//===========================================================================
//
void AMSTOFCluster::init(){
  _planes=TOF2DBc::getnplns();
  for(int i=0;i<_planes;i++)_padspl[i]=TOF2DBc::getbppl(i);
}
//---------------------------------------------------------------------------
void AMSTOFCluster::build2(int &stat){
//(search for true dublets(overlaping counter events))
//
  TOF2RawCluster *ptr; 
  TOF2RawCluster *ptrn; 
  static TOF2RawCluster * xptr[TOF2GC::SCMXBR];
  static number eplane[TOF2GC::SCMXBR],edplane[TOF2GC::SCMXBR];
  static AMSlink * membp[2];
  int nclust,cllay[TOF2GC::SCLRS],nmemb;
  AMSPoint coo,ecoo;
  int i,j,il,ib,bmax;
  integer ntof,barn,status,statusn,plrot,ok;
  geant barl,barw,cl,cle,ct,cte,cln,clne,ctn,ctne,cz,czn,clm,clnm;
  geant edep,edepn,edepa,edepd,edepdl,edepdn,edass;
  geant time,etime,timen,etimen,speedl,err;
//-----
  stat=1; // bad
  for(i=0;i<TOF2GC::SCLRS;i++)cllay[i]=0;
//
  for(il=0;il<TOF2GC::SCLRS;il++){// <-------- TOF layers loop -----
    ptr=(TOF2RawCluster*)AMSEvent::gethead()->
                                     getheadC("TOF2RawCluster",0);
    VZERO(eplane,(TOF2GC::SCMXBR)*sizeof(number)/4);
    VZERO(edplane,(TOF2GC::SCMXBR)*sizeof(number)/4);
    VZERO(xptr,(TOF2GC::SCMXBR)*sizeof(TOF2RawCluster*)/4);
//
    while (ptr){// scan to put all fired bars of layer "il" in buffer
      if(ptr->getntof()==il+1){
        ib=ptr->getplane();
#ifdef __AMSDEBUG__
        assert(ib>0 && ib <= TOF2GC::SCMXBR);
#endif
        edepa=ptr->getedepa();
        edepd=ptr->getedepd();
//    select between anode and dynode measurements:
	if(edepa>0.)edep=edepa;// no overflow of anode --> use it
	else edep=edepd; //  --> use Dynode otherwise
//
        eplane[ib-1]=edep;//store full range Edep(An/Dyn-combined)
        if(edepd>0)edplane[ib-1]=edepd;//store separately dynode Edep
        xptr[ib-1]=ptr;
      }
      ptr=ptr->next();
    }// ---> end of scan
//------
  nclust=0;
  bmax=TOF2DBc::getbppl(il);//bars per given plane
  for (ib=0;ib<bmax;ib++){// <---- loop over buffer content (clust. search)
    if(eplane[ib]> TFREFFKEY.Thr1){//<--- thresh. check
      ok=1;
      nmemb=0;
      ptr=xptr[ib];
      ntof=il+1;
      barn=ib+1;
      barl=TOF2DBc::brlen(il,ib);// bar length
      barw=TOF2DBc::plnstr(5);//bar width
      TOF2Brcal::scbrcal[il][ib].getd2p(speedl,err);//get light speed
      plrot=TOF2DBc::plrotm(il);     // =0/1-unrotated/rotated TOF-plane
      cz=ptr->getz();          //bar(raw clust) Z-coord.
      cl=ptr->gettimeD();// get yloc/err for ib bar
      cle=ptr->getetimeD();
      clm=0.5*barl;// limit on max. long. coord.
      ct=TOF2DBc::gettsc(il,ib);   //transv.pos. of  bar
      cte=barw/sqrt(12.);
      status=ptr->getstatus();//may be !=0(bad history/t_meas or single-sided)
      edep=eplane[ib];
      edepd=edplane[ib];
      if(fabs(cl) > clm+2*cle){//bad l-coord.measurement(out of bar size)
        status|=AMSDBc::BAD; // bad coord. means also bad for t-measurement !!!
        cle=barl/sqrt(12.);
        if(cl>0.)cl=clm;//at bar edge
        else cl=-clm;
	ok=0;//means bar with suspicious time measurement 
      }
      membp[nmemb]=ptr;//store ib cluster pointer
      nmemb+=1;
      time=ptr->gettime();// time from ib bar(ns) 
      etime=TFMCFFKEY.TimeSigma/sqrt(2.);//tempor(0.15ns,later put in TOFBrcal needed data!)
      if((status&TOFGC::SCBADB2)>0){//1-sided
        if(status & TOFGC::SCBADB5){ // recovered 1-sided counter
          etime=2.05*TFMCFFKEY.TimeSigma/sqrt(2.);//tempor(still no Tracker info)
        }
        else{
          etime=(1.e-9)*barl/speedl/sqrt(12.);//(sec !!!)for single-sided counters
        }
	ok=0;//means bar with time meas.problems 
      }
      if((status&TOFGC::SCBADB3)>0)ok=0;//bar "ib" is bad for t-meas. according to DB
//
      if(ok==1 && ib+1<bmax && eplane[ib+1]> TFREFFKEY.Thr1){// try next(adjacent) bar if prev.OK
        ptrn=xptr[ib+1];
        barl=TOF2DBc::brlen(il,ib+1);// peak bar length
        czn=ptrn->getz();          //next bar(raw clust) Z-coord.
        cln=ptrn->gettimeD();// get yloc/err for "peak" bar
        clne=ptrn->getetimeD();
        clnm=0.5*barl;// limit on max. long. coord.
        ctn=TOF2DBc::gettsc(il,ib+1);   //transv.pos. of  bar
        ctne=barw/sqrt(12.);
        statusn=ptrn->getstatus();//may be !=0(bad history/t_meas or single-sided)
        edepn=eplane[ib+1];
        edepdn=edplane[ib+1];
        if(fabs(cln) > clnm+2*clne){//bad l-coord.measurement(out of bar size)
          clne=barl/sqrt(12.);
          if(cln>0.)cln=clnm;//at bar edge
          else cln=-clnm;
	  ok=0;//means bar with suspicious time measurement 
        }
	if(ok==1 && (statusn&TOFGC::SCBADB2)==0
                 && (statusn&TOFGC::SCBADB3)==0	
	                                       ){//<--- next bar good for gluing ?
          timen=ptrn->gettime();// time from ib+1 bar(ns) 
          etimen=TFMCFFKEY.TimeSigma/sqrt(2.);//tempor(0.15ns,later put in TOFBrcal needed data!)
	  if(TFREFFKEY.reprtf[2]>0){
	    if(fabs(time-timen)<3*etime*sqrt(2.))HF1(1549,cl-cln,1.);
	    HF1(1550,time-timen,1.);
	  }
	  if(fabs(time-timen)<3*etime*sqrt(2.)
	             && fabs(cl-cln)<3*clne*sqrt(2.)){//t+coo match -> create cluster(glue "next")
	    etime=etime/sqrt(2.);//recalc. parameters using glued bar
	    time=0.5*(time+timen);
	    cle=cle/sqrt(2.);
	    cl=0.5*(cl+cln);
	    cte=(TOF2DBc::plnstr(3)+TOF2DBc::plnstr(4)
	                           +TOF2DBc::plnstr(6))/sqrt(12.);//max.estim(overlap+v.gap+thickn)
	    ct=0.5*(ct+ctn);
	    cz=0.5*(cz+czn);
	    if(edep<edepn)barn=ib+2;//use bar-number for cluster from highest edep bar 
	    edass=(edep-edepn)/(edep+edepn);//edep,edepn>thresh>0 
	    if(TFREFFKEY.reprtf[2]>0)HF1(1548,edass,1.);
            if(fabs(edass)<TOF2Varp::tofvpar.eclass()){//tempor(need real data to clarify algor.)
	      edep=0.5*(edep+edepn);
	      if(edepd>0 && edepdn>0)edepd=0.5*(edepd+edepdn);
	      else{
	        if(edepdn>0)edepd=edepdn;
	      }
	    }
	    else{
	      if(edepn>edep){
	        edep=edepn;//use highest when big assimetry in edep's
		edepd=edepdn;
	      }
	    }
            membp[nmemb]=ptrn;//store glued cluster pointer
            nmemb+=1;
            if((statusn&TOFGC::SCBADB1)!=0)status|=TOFGC::SCBADB1;//bad hist OR-ed for members 
	    ib+=1;//to skip already glued "next" bar in further buffer scan 
	  }//---> endof time/coo-match check
	}//endof check for  "next" bar gluing
      }//---> endof next bar check
//
      coo[2]=cz;
      ecoo[2]=TOF2DBc::plnstr(6)/sqrt(12.);//bar thickness/...   
      if(plrot==0){ // non-rotated plane
        coo[0]=ct;//clust. X-coord.
        ecoo[0]=cte;
        coo[1]=cl;//clust. abs.Y-coord.(yabs=yloc and neglect counter's long.shift)
        ecoo[1]=cle;
      }
      else{ // rotated plane
        coo[0]=cl;//clust. abs.X-coord.(xabs=yloc and neglect counter's long.shift)
        ecoo[0]=cle;
        coo[1]=ct;
        ecoo[1]=cte;
      }
//
      if(TFREFFKEY.reprtf[2]>0){// some histograms for clusters(1/2 members):
        if(il==0){
          HF1(1535,edep,1.);//Cluster energy distr.,L=1
          HF1(1537,edep,1.);
	  HF1(1545,sqrt(edep),1.);
          HF1(1541,geant(coo[0]),1.);
          HF1(1542,geant(coo[1]),1.);
        }
        if(il==1){
          HF1(1539,edep,1.);//Cluster energy distr.,L=2
          HF1(1543,geant(coo[0]),1.);
          HF1(1547,geant(coo[1]),1.);
        }
        if(il==2){
          HF1(1536,edep,1.);//Cluster energy distr.,L=3
          HF1(1538,edep,1.);
	  HF1(1546,sqrt(edep),1.);
        }
        if(il==3){
          HF1(1540,edep,1.);//Cluster energy distr.,L=4
        }
      }
//
      time=-time*1.e-9;// ( ns -> sec ,"-" for V.Shoutko fit)
      etime*=1.e-9;// ( ns -> sec for V.Shoutko fit)
      for(j=0;j<nmemb;j++)membp[j]->setstatus(AMSDBc::USED);// set "used" bit for members
      if((status & TOFGC::SCBADB3)!=0)status|=AMSDBc::BAD;//bad for t-meas. according to DB 
//
      if((status & TOFGC::SCBADB2)!=0 && (status & TOFGC::SCBADB5)==0)status|=AMSDBc::BAD;
//          bad=(bar is 1-sided and not recovered -> bad for t-measur)
//
      AMSEvent::gethead()->addnext(AMSID("AMSTOFCluster",il), new
        AMSTOFCluster(status,ntof,barn,edep,edepd,coo,ecoo,time,etime,nmemb,membp));
//
      nclust+=1;
//------
    }// ---> end of ib thresh. check    
//------
  }// ---> end of buffer (ib) loop (clust.search)
  if(nclust>0)cllay[il]=1;
//------
  }// ---> end of TOF layers (il) loop
  if((cllay[0]+cllay[1]+cllay[2]+cllay[3])>0)stat=0;//at least one layer with cluster
// 
}
//
//--------------------------------------------
//
void AMSTOFCluster::recovers2(AMSTrTrack * ptr){ // recreate TOFCluster
//  time through recovering(using tracker) of 1-sided TOFRawCluster-members
// (only for 1-member clusters: 2-memb.clusters are 2-sided by definition(creation))
// called from beta.C
  integer status,nm,mib,mil;
  AMSDir dir(0,0,1.);
  AMSPoint coo;
  AMSPoint outp;
  number theta,phi,sleng,ctr,clo,newt;
//
  if(_nmemb==1){// only for 1-memb clusters
    nm=0;
    status=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->getstatus();
    mil=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->getntof()-1;
    mib=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->getplane()-1;
    if(nm==0){// 
      coo[0]=0.;
      coo[1]=0.;
      coo[2]=TOF2DBc::getzsc(mil,mib);
      ptr->interpolate(coo,dir,outp,theta,phi,sleng);
      if(TOF2DBc::plrotm(mil)==0){
        clo=outp[1];// unrot.(X-meas) -> Y-cross = long.c(loc.r.s.=abs.r.s.)
        ctr=outp[0];// transv. coord.(abs. r.s.)(X-cross) 
      }
      else {
        clo=outp[0];// rot.(Y-meas) -> take X-cross for long.c.
//        clo=-clo;// go to bar local r.s.
        ctr=outp[1];// transv. coord.(abs. r.s.)(Y-cross) 
      }
    }
      dynamic_cast<TOF2RawCluster*>(_mptr[nm])->recovers(clo);//missing side recovering
      newt=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->gettime();//new time of raw cluster (ns)
      _time=-newt*1.e-9;//new time of cluster(-sec for Vitali)
      _etime=1.323*TFMCFFKEY.TimeSigma/sqrt(2.);//tempor(sec-> 160ps, trecker recovered)
  }
}
//-----------------------------------------------------------------------------
integer TOF2RawSide::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
   init=1;
   integer ntrig=AMSJob::gethead()->gettriggerN();
   for(int n=0;n<ntrig;n++){
     if(strcmp("TOF2RawSide",AMSJob::gethead()->gettriggerC(n))==0){
       WriteAll=1;
       break;
     }
   }
  }
  return (WriteAll || status);
}
//---
integer TOF2RawCluster::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("TOF2RawCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}
//---
integer AMSTOFCluster::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
   init=1;
   integer ntrig=AMSJob::gethead()->gettriggerN();
   for(int n=0;n<ntrig;n++){
     if(strcmp("AMSTOFCluster",AMSJob::gethead()->gettriggerC(n))==0){
       WriteAll=1;
       break;
     }
   }
  }
  return (WriteAll || status);
}
//=======================================================================
//  DAQ-interface functions :
//
// function returns number of TOF_data-words for given block/format
//
/*integer TOF2RawSide::calcdaqlength(int16u blid){
  return(0);
}
//--------------------------------------------------------------------
void TOF2RawSide::builddaq(int16u blid, integer &len, int16u *p){
//
}
//----------------------------------------------------------------------
// function to decode DAQ and build RawEvent: 
//  
void TOF2RawSide::buildraw(int16u blid, integer &len, int16u *p){
//
}
//-------------------------------------------------------------------------------
//  function to get sofrware-id (LBBS) for given hardware-chan (crate,sfet,tofch):
//                                                               0-7   0-3  0-3
int16u TOF2RawSide::hw2swid(int16u a1, int16u a2, int16u a3){
  return(200);
}

*/

AMSID AMSTOFCluster::crgid(int k){
  int ii,kk;
  char inum[11];
  char in[2]="0";
  char vname[5];
  strcpy(inum,"0123456789");
  strcpy(vname,"TF");
  kk=(_ntof-1)*TOF2GC::SCMXBR+_plane;//counter ID used in volume name
  ii=kk/10;
  in[0]=inum[ii];
  strcat(vname,in);
  ii=kk%10;
  in[0]=inum[ii];
  strcat(vname,in);
  return AMSID(vname,_ntof*100+_plane);
}
