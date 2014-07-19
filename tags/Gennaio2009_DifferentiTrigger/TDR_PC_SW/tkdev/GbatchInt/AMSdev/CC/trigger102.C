//  $Id: trigger102.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Simple version 9.06.1997 by E.Choumilov
// deep modifications Nov.2005 by E.Choumilov
// decoding tools added dec.2006 by E.Choumilov
//
#include "commons.h" 
#include "trigger102.h"
#include "event.h"
#include "mccluster.h"
#include "tofdbc02.h"
#include "antidbc02.h"
#include "ecaldbc.h"
#include "tofrec02.h"
#include "tofsim02.h"
#include "antirec02.h"
#include "ecalrec.h"
#include "ntuple.h"
using namespace std;
using namespace ecalconst;
//
 Trigger2LVL1::Scalers Trigger2LVL1::_scaler;
 Trigger2LVL1::Lvl1TrigConfig Trigger2LVL1::l1trigconf;
 Trigger2LVL1::ScalerMon Trigger2LVL1::scalmon;
 integer TGL1JobStat::countev[20];
 integer TGL1JobStat::daqc1[15];
 int16u Trigger2LVL1::nodeids[2]={4,5};//node addr for side a/b (tempor)
//
void Trigger2LVL1::build(){
//build lvl1-obj for MC; Complete(+rebuild) lvl1 for RealData.
// 
  integer PhysBPatt(0);//Lvl1 phys.branches pattern(e,p,gamma,unbiased,...,external; 8 in total)
  integer JMembPatt(0);//Lvl1 joined members pattern(FTC,FTZ,FTE,ACC0,...,BZ,ECAL-F_and,...; 16 in total)
  int i,il,ib,ibmx,ns1,ns2;
  integer ntof=0;
  integer tofpatt1[TOF2GC::SCLRS]={0,0,0,0};//all sides,FTC(z>=1)
  integer tofpatt2[TOF2GC::SCLRS]={0,0,0,0};//all sides,BZ(z>=2)
  geant rates[6];//TrigCompRates(Hz):FT,FTC,LVL1,TOFmx,ECFTmx,ANTImx
  integer tflg(-1);
  integer toftrcode(-1),tofcpcode(0),tofbzflag(0);
  integer toftrcode1(-1),toftrcode2(-1);//0->4planes,(1-4)->3plns,(5-8)->2plns, <0->none
  integer ftpatt;
  integer nanti=0;
  integer antipatt=0;
  integer ectrigfl=0;
  integer ectrconf=0;
  int16u ectrpatt[6][3];
  geant gmaglat,gmagphi;
  geant ectrsum(0);
  geant livetime(1),ratemx;
  bool toftcp1(0);//tof.centr.area flag(1/0->centre/not),FTCP1(lut1)
  bool toftcp2(0);//tof.centr.area flag(1/0->centre/not),FTCP2(lut2)
  uinteger cbt,lsbit(1);
  int antismx[2];
  char mnames[16][4];
//
  TGL1JobStat::addev(0);
//-----------------------------------------------------------------------------------------
//=====> RealData:
  if(AMSJob::gethead()->isRealData()){// <===== add some info to existing DAQ-created lvl1-obj
    Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
    if(ptr){
      TGL1JobStat::addev(15);
      TOF2RawSide::getpatt(tofpatt1);
      TOF2RawSide::getpattz(tofpatt2);
      AMSEcalRawEvent::gettrpatt(ectrpatt);//masked trpatt[6][3]: max 3x16bits in 6 "trigger" sup.layers
      ectrsum=AMSEcalRawEvent::gettrsum();
      ptr->settofpat1(tofpatt1);
      ptr->settofpat2(tofpatt2);
      ptr->setecpat(ectrpatt);
      ptr->setectrs(ectrsum);
      TGL1JobStat::addev(16);
    }
//-------------------------------------------
/*
    if(TGL1FFKEY.RebuildLVL1==1){//<=== special RD: (Re)build lvl1 from subdet-reco objects only
// TOF :
//    (AMSEvent::gethead()->getC("TriggerLVL1",0))->eraseC();
      tofflag=1;
      TOF2RawCluster *pcl=(TOF2RawCluster*)AMSEvent::gethead()->getheadC("TOF2RawCluster",0);
      while(pcl){
       int plane=pcl->getntof()-1;
       int bar=pcl->getplane()-1;
       ttrpatt[plane]=ttrpatt[plane] | ( 1 << bar);  
       pcl=pcl->next();
      }
      for(i=0;i<TOF2GC::SCLRS;i++)if(ttrpatt[i]>0)ntof+=1;//counts coinc. planes
// ANTI :
      integer antip[2]={0,0};
      for(int k=0;k<2;k++){
        AMSAntiRawCluster *pcl=(AMSAntiRawCluster *)AMSEvent::gethead()->getheadC("AMSAntiRawCluster",k);
        while(pcl){
        int sector=pcl->getsector()-1;
         int updown=pcl->getupdown();
         antip[updown]=antip[updown] | ( 1 << sector);  
         pcl=pcl->next();
        }
      }
      antipatt=antip[0] & antip[1];
 
      integer cbt;
      for(i=0;i<MAXANTI;i++){
        cbt=1<<i;
        if((antipatt&cbt)>0)nanti+=1;//counts paddles
      }
// ECAL :
      ectrigfl=AMSEcalRawEvent::gettrfl();
      ectrsum=AMSEcalRawEvent::gettrsum();
//
      AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
          new Trigger2LVL1(PhysBPatt,JMembPatt,toftrcode1,toftrcode2,tofpatt1,tofpatt2,
	                         antipatt,ectrigfl,ectrpatt,ectrsum,livetime,rates));//new lvl1-object
    }
*/
    return;
  }
//----------------------------------------------------------------------------------------
//=====> MC:
  else{//<===== create lvl1-obj using subdet.simulated RawEvent objects
//
    strcpy(mnames[0],"FTC");
    strcpy(mnames[1],"CP0");
    strcpy(mnames[2],"CP1");
    strcpy(mnames[3],"CT0");
    strcpy(mnames[4],"CT1");
    strcpy(mnames[5],"FTZ");
    strcpy(mnames[6],"FTE");
    strcpy(mnames[7],"AC0");
    strcpy(mnames[8],"AC1");
    strcpy(mnames[9],"BZ ");
    strcpy(mnames[10],"EFa");
    strcpy(mnames[11],"EFo");
    strcpy(mnames[12],"EAa");
    strcpy(mnames[13],"EAo");
    strcpy(mnames[14],"EX0");
    strcpy(mnames[15],"EX1");
//
    if(TGL1FFKEY.antismx[0]>=0)antismx[0]=TGL1FFKEY.antismx[0];//from data-card
    else antismx[0]=Trigger2LVL1::l1trigconf.antsectmx(0);//from DB
    if(TGL1FFKEY.antismx[1]>=0)antismx[1]=TGL1FFKEY.antismx[1];//from data-card
    else antismx[1]=Trigger2LVL1::l1trigconf.antsectmx(1);//from DB
//
    rates[0]=scalmon.FTrate();
    rates[1]=scalmon.FTCrate();
    rates[2]=scalmon.LVL1rate();
    rates[3]=scalmon.TOFrateMX();
    rates[4]=scalmon.ECftrateMX();
    rates[5]=scalmon.AntirateMX();
    ratemx=max(rates[3],rates[4]);
//
//-->TOF:
    toftrcode1=TOF2RawSide::gettrcode();//<0 ->noFTC(z>=1), >=0 ->OK, masked
    toftrcode2=TOF2RawSide::gettrcodez();//<0 ->noBZ(z>=2), >=0 ->OK, masked
    ftpatt=TOF2RawSide::getftpatt();//already passed masking in tofsim02.C !!!
    tofcpcode=TOF2RawSide::getcpcode();//IJ->lut2|lut1  (i(j)=1/0->used/not)
    tofbzflag=TOF2RawSide::getbzflag();//Logically:No BZ-flag if FTC missing
    TOF2RawSide::getpatt(tofpatt1);
    TOF2RawSide::getpattz(tofpatt2);
    if(TGL1FFKEY.printfl>0){
      cout<<endl;
      cout<<"===> In TrigLev1Build: FTpatt="<<ftpatt<<" toftrcode/toftrcodez="<<toftrcode1<<" "<<toftrcode2<<endl;
      cout<<"     tofcpcode="<<tofcpcode<<" bzflag="<<tofbzflag<<endl;
    }
//
//-->ECAL:
    ectrigfl=AMSEcalRawEvent::gettrfl();//masked MN->Energy|Angle, M=<3/3->NonEm/Em,
//                                                              N=0/1/2->Undef/Bad/OK
    ectrconf=AMSEcalRawEvent::gettrconf();//masked MN->Energy|Angle, M(N)=2/1=>proj-AND/OR
//                                          created in EcalRawEvent::mc_build for convenience
    AMSEcalRawEvent::gettrpatt(ectrpatt);//masked trpatt[6][3]: max 3x16bits in 6 "trigger" sup.layers
    ectrsum=AMSEcalRawEvent::gettrsum();
//   
//-->ANTI:
//    
    antipatt=Anti2RawEvent::getpatt();//bits (1-8)->sectors in coinc.with FT
    nanti=Anti2RawEvent::getncoinc();
//
//-->Event: latitude
//
    AMSEvent::gethead()->getmag(gmaglat,gmagphi);
//
    if(TGL1FFKEY.printfl>0){
      cout<<"     ECTrigFlag/ProjConfig="<<ectrigfl<<" "<<ectrconf<<endl;
      cout<<"     TotAntiSect/Patt="<<nanti<<" "<<antipatt<<endl;
      cout<<"     MagLat/Phi="<<gmaglat<<" "<<gmagphi<<endl;
    }
//------
    if(ftpatt==0)return;// no global(FTC/FTZ/FTE) FastTrig, no ExtTrig
    TGL1JobStat::addev(1);
//
    if(ftpatt&(1)){//was FTC
      TGL1JobStat::addev(2);//was FTC-member
      if(tofbzflag==1)TGL1JobStat::addev(3);//was FTC+BZ
    }
//
    if((ftpatt&(1<<1))>0)TGL1JobStat::addev(4);//was FTZ-member
    if((ftpatt&(1<<1))>0 && (ftpatt&1)==0)TGL1JobStat::addev(5);//was FTZ-member when FTC missed
    if((ftpatt&(1<<2))>0){//was FTE-member
      TGL1JobStat::addev(6);
      HF1(1097,geant(ectrconf),1.);
      if((ftpatt&3)==1)HF1(1096,geant(ectrconf),1.);
    }
    if((ftpatt&(1<<2))>0 && (ftpatt&3)==0)TGL1JobStat::addev(7);//was FTE-member fhen TOF-FT missed
    if((ftpatt&(1<<3))>0)TGL1JobStat::addev(8);//was EXT-member
//---> create "central trigger"-like flag FTCP1(lut1)(exists in Lin's paper, useless):    
    if(ftpatt&1>0){
      if(tofcpcode%10>0){//lut1
        ns1=0;
        ns2=0;
        for(int il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
          ibmx=TOF2DBc::getbppl(il);
          cbt=lsbit<<(ibmx-1);//last(in current il) paddle bit
          if((tofpatt1[il] & 1)>0 || (tofpatt1[il] & cbt)>0)ns1+=1;//s1 fired   
          if((tofpatt1[il] & 1<<16)>0 || (tofpatt1[il] & cbt<<16)>0)ns2+=1;//s2 fired
        }
        toftcp1=(ns1==0 && ns2==0);//no outer counters, central trig, 
      }
//   
      if(tofcpcode/10>0){//lut2
        ns1=0;
        ns2=0;
        for(int il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
          ibmx=TOF2DBc::getbppl(il);
          cbt=lsbit<<(ibmx-1);//last(in current il) paddle bit
          if((tofpatt1[il] & 1)>0 || (tofpatt1[il] & cbt)>0)ns1+=1;//s1 fired   
          if((tofpatt1[il] & 1<<16)>0 || (tofpatt1[il] & cbt<<16)>0)ns2+=1;//s2 fired
        }
        toftcp2=(ns1==0 && ns2==0);//no outer counters, central trig, 
      }
    }
//
//-->Make lvl1 JoinedMembersPattern(JMembPatt includes FTC, i.e all 16 bits):
//
    if(ftpatt&1>0)JMembPatt|=1;//FTC
    if(tofcpcode%10>0)JMembPatt|=(1<<1);//FTCP0
    if(tofcpcode/10>0)JMembPatt|=(1<<2);//FTCP1
    if(toftcp1)JMembPatt|=(1<<3);//FTCT0 
    if(toftcp2)JMembPatt|=(1<<4);//FTCT1
    if((ftpatt&(1<<1)) > 0)JMembPatt|=(1<<5);//FTZ
    if((ftpatt&(1<<2)) > 0)JMembPatt|=(1<<6);//FTE
    if(nanti<=antismx[0])JMembPatt|=(1<<7);//ACC0(equat:1)
    if(nanti<=antismx[1])JMembPatt|=(1<<8);//ACC1(polar:0)
    if(tofbzflag==1)JMembPatt|=(1<<9);//BZ
    if(ectrconf/10==2)JMembPatt|=(1<<10);//ECAL=F_and
    if(ectrconf/10==1)JMembPatt|=(1<<11);//ECAL=F_or
    if(ectrconf%10==2)JMembPatt|=(1<<12);//ECAL=A_and
    if(ectrconf%10==1)JMembPatt|=(1<<13);//ECAL=A_or
    if((ftpatt&(1<<3)) > 0)JMembPatt|=(1<<14);//Ext0
//
    if(TGL1FFKEY.printfl>0){
      cout<<"     JMembPatt:"<<endl;
      for(i=0;i<16;i++){
        if((JMembPatt&(1<<i))>0)cout<<mnames[i]<<"|";
        else cout<<"000|";
      }
      cout<<endl;
    } 
//------
//
//    geant sumsc=_scaler.getsum(AMSEvent::gethead()->gettime());
//    geant livetime=_scaler.getlifetime(AMSEvent::gethead()->gettime());
//    integer tm=int(floor(TOF2Varp::tofvpar.getmeantoftemp(0)));   
//    if(livetime>1. && !MISCFFKEY.BeamTest && AMSJob::gethead()->isRealData() )
//                                                     AMSEvent::gethead()->seterror();
//------
//
    bool comtrok(0),BranchOK[8]={0,0,0,0,0,0,0,0};
//  if(fabs(gmaglat)<TGL1FFKEY.TheMagCut && nanti <= antismx[0])antiok=1;//equat.region
//  if(fabs(gmaglat)>TGL1FFKEY.TheMagCut && nanti <= antismx[1])antiok=1;//polar region
//
    integer brand[8];//ANDs of current and preset patterns of phys.branches
    integer brreq[8]; 
    for(i=0;i<8;i++){
      brreq[i]=(Trigger2LVL1::l1trigconf.physbrmemb(i)<<1);//"<<1" due to miss.FTC-bit in JMemb-request 
      brand[i]=(JMembPatt & brreq[i]);
    }
//
//===> It is implied that globFT is always present, so below requirements are complementary !!!
//
    BranchOK[0]=(brreq[0]>0 && brreq[0]==brand[0]);//unb_tof(ftc)                                
    BranchOK[1]=(brreq[1]>0 && brreq[1]==brand[1]);//Z>=1(ftc, anti)  
    BranchOK[2]=(brreq[2]>0 && brreq[2]==brand[2]);//Z>=2(ftc, bz)                              
    BranchOK[3]=(brreq[3]>0 && brreq[3]==brand[3]);//SlowZ>=2(ftz)
    BranchOK[4]=(brreq[4]>0 && brreq[4]==brand[4]);//e(ftc,fte, ecFand)
    BranchOK[5]=(brreq[5]>0 && brreq[5]==brand[5]);//gamma(fte, ecFand, ecAand)
    BranchOK[6]=(brreq[6]>0 && brreq[6]==brand[6]);//unb_ec(fte) 
    BranchOK[7]=(brreq[7]>0 && brreq[7]==brand[7]);//ext0
//
    char subln[8][7];
    if(TGL1FFKEY.printfl>0){
      strcpy(subln[0],"unbTOF");
      strcpy(subln[1],"Z>=1  ");
      strcpy(subln[2],"Z>=2  ");
      strcpy(subln[3],"slZ>=2");
      strcpy(subln[4],"electr");
      strcpy(subln[5],"gamma ");
      strcpy(subln[6],"unbEC ");
      strcpy(subln[7],"ext0  ");
      cout<<"     Fired lvl1-branches:"<<endl;
      for(int ibr=0;ibr<8;ibr++){
        if(BranchOK[ibr])cout<<subln[ibr]<<"|";
        else cout<<"000000|";
      }
      cout<<endl;
    }
//
    int nbreq(0);
    integer trtype(0);
    if(TGL1FFKEY.trtype>0)trtype=TGL1FFKEY.trtype;
    else trtype=Trigger2LVL1::l1trigconf.globl1mask();
    if(TGL1FFKEY.printfl>0)cout<<"     RequestedSubLvl1Patt(trtype)="<<trtype<<endl;
//
    HF1(1098,0.,1.);
    for(i=0;i<8;i++){
      if(BranchOK[i]){
        HF1(1098,geant(i+1),1.);
      }
      if(trtype & 1<<i)nbreq+=1;//count requested branches
    }
//
    if(nbreq==0 || trtype>128){//">128" means (ExtTr + smth else) = wrong
      cout<<"Trigger2LVL1::build Error: unknown trigger type requested ! "<<trtype<<endl;
      exit(10);
    }
//
    for(i=0;i<16;i++){
      if((JMembPatt&(1<<i))>0){
        HF1(1095,geant(i+1),1.);
      }
    }
//
//                         <---- check OR of requested branches(trigger type):
    int nbchk(0);
    if(trtype ==128){
      comtrok=1;
      PhysBPatt=128;
    }
    else{
      for(i=0;i<8;i++){
        if((trtype & 1<<i)>0 && BranchOK[i]){
          nbchk+=1;
	  PhysBPatt|=(1<<i);//create (fired&requested)-branch pattern
        }
      }
      if(nbchk>0)comtrok=1;
    }
//
    if(comtrok){
      TGL1JobStat::addev(9);
      if(ratemx<TGL1FFKEY.MaxScalersRate && livetime>TGL1FFKEY.MinLifeTime){
        TGL1JobStat::addev(10);
        AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
          new Trigger2LVL1(PhysBPatt,JMembPatt,toftrcode1,toftrcode2,tofpatt1,tofpatt2,
	                         antipatt,ectrigfl,ectrpatt,ectrsum,livetime,rates));//create lvl1trig-object
      }
      else AMSEvent::gethead()->seterror();
    }
  }//--->endof "MC-data"
}
//--------------------
void Trigger2LVL1::init(){
  HBOOK1(1095,"LVL1:16-membPatt:ftc,cp0,cp1,ct0,ct1,ftz,fte,ac0,ac1,bz,ecfa,ecfo,ecaa,ecao,ext",20,0.,20.,0.);
  HBOOK1(1096,"LVL1:EC ProjConfig(when FTE&TOF_FT, masked)",30,0.,30.,0.);
  HBOOK1(1097,"LVL1:EC ProjConfig(when FTE, masked, val: M|N=FTE|ANG, M(N)=1/2->or/end_proj)",30,0.,30.,0.);
  HBOOK1(1098,"LVL1:fired-branches(globFT,unbTOF,Z>=1,Z>=2,SlowZ>=2,elec,phot,unbEC,ext",10,0.,10.,0.);
  TGL1JobStat::resetstat();
}
//--------------------
void Trigger2LVL1::Lvl1TrigConfig::read(){//read needed lvl1trig-masks
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
  int mrfp;
  char in[2]="0";
  char inum[11];
  int mcvn,rlvn,dig,endflab(0);
//
  strcpy(inum,"0123456789");
//reset:
 _globftmask=0;
 _globl1mask=0;
 _toflut1=0;
 _toflut2=0;
 _toflutbz=0;
 _toflcsz=0;
 for(int il=0;il<TOF2GC::SCLRS;il++){
   _tofoamask[il]=0;//reset mask
   _tofoazmask[il]=0;//reset mask
   for(int ib=0;ib<TOF2GC::SCMXBR;ib++){
     _tofinmask[il][ib]=0;//reset mask
     _tofinzmask[il][ib]=0;//reset mask
   }
 }
//
 strcpy(name,"lvl1conf");//generic Lvl1ConfData file name
 if(AMSJob::gethead()->isMCData())           // for MC-data
 {
   cout <<" Trigger2LVL1::Init: LVL1TrigConf-data for MC are requested"<<endl;
   mcvn=TGL1FFKEY.Lvl1ConfMCVers%1000;
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
   cout <<" Trigger2LVL1::Init: LVL1TrigConf-data for RealData are requested"<<endl;
   rlvn=TGL1FFKEY.Lvl1ConfRDVers%1000;
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
   if(TGL1FFKEY.Lvl1ConfRead/10==0)strcpy(fname,AMSDATADIR.amsdatadir);//M
   if(TGL1FFKEY.Lvl1ConfRead/10==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"Open file : "<<fname<<'\n';
   ifstream tgfile(fname,ios::in); // open file for reading
   if(!tgfile){
     cerr <<"Trigger2LVL1::Init: Missing Lvl1ConfData file "<<fname<<endl;
     exit(1);
   }
//---->>> TOF:
  for(int il=0;il<TOF2DBc::getnplns();il++){
    for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
      tgfile >> _tofinmask[il][ib];
      if(tgfile.eof()){
        cerr<<"Trigger2LVL1::Init:Unexpected EOF reading TofInTrigMask(z>=1) !!!"<<endl;
	exit(1);
      }
    }
  }
//  
  for(int il=0;il<TOF2DBc::getnplns();il++){
    for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
      tgfile >> _tofinzmask[il][ib];
      if(tgfile.eof()){
        cerr<<"Trigger2LVL1::Init:Unexpected EOF reading TofInTrigMask(z>=2) !!!"<<endl;
	exit(1);
      }
    }
  }
//  
  for(int il=0;il<TOF2DBc::getnplns();il++){
    tgfile >> _tofoamask[il];
    if(tgfile.eof()){
      cerr<<"Trigger2LVL1::Init:Unexpected EOF reading Tof PlaneSidesOrAndMask(z>=1) !!!"<<endl;
      exit(1);
    }
  }
//  
  for(int il=0;il<TOF2DBc::getnplns();il++){
    tgfile >> _tofoazmask[il];
    if(tgfile.eof()){
      cerr<<"Trigger2LVL1::Init:Unexpected EOF reading Tof PlaneSidesOrAndMask(z>=2) !!!"<<endl;
      exit(1);
    }
  }
//  
  tgfile >> _toflut1;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading TOF LayerConfLut1(z>=1) !!!"<<endl;
    exit(1);
  }
//  
  tgfile >> _toflut2;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading TOF LayerConfLut2(z>=1) !!!"<<endl;
    exit(1);
  }
//
  tgfile >> _toflutbz;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading TOF LayerConfLutbz(z>=2) !!!"<<endl;
    exit(1);
  }
//
  tgfile >> _toflcsz;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading TOF LayerConfSlowZ(z>=2) !!!"<<endl;
    exit(1);
  }
//
  tgfile >> _tofextwid;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading TOF SlowZExtWidth(z>=2) !!!"<<endl;
    exit(1);
  }
//----->>> ANTI:  
  for(int is=0;is<ANTI2C::MAXANTI;is++){
    tgfile >> _antinmask[is];
    if(tgfile.eof()){
      cerr<<"Trigger2LVL1::Init:Unexpected EOF reading Anti InTrigMask !!!"<<endl;
      exit(1);
    }
  }
//-----
  for(int is=0;is<ANTI2C::MAXANTI;is++){
    tgfile >> _antoamask[is];
    if(tgfile.eof()){
      cerr<<"Trigger2LVL1::Init:Unexpected EOF reading Anti OrAndMask !!!"<<endl;
      exit(1);
    }
  }
//-----
  for(int ip=0;ip<2;ip++){
    tgfile >> _antsectmx[ip];
    if(tgfile.eof()){
      cerr<<"Trigger2LVL1::Init:Unexpected EOF reading ANTI-sectors number limits !!!"<<endl;
      exit(1);
    }
  }
//----->>> EcalOrAndFlag and ActiveProjMask:
  tgfile >> _ecorand;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading EcalProjOrAndFlag !!!"<<endl;
    exit(1);
  }
//-----
  tgfile >> _ecprjmask;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading EcalActiveProjMask !!!"<<endl;
    exit(1);
  }
//----->>> globalFastTrig:  
  tgfile >> _globftmask;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading GlobalFTrigMask !!!"<<endl;
    exit(1);
  }
//----->>> globalLVL1:  
  tgfile >> _globl1mask;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading lLvl1SubTrigMask !!!"<<endl;
    exit(1);
  }
//----->>> TrigMembSetting each of 8 PhysBranches:
  for(int ip=0;ip<8;ip++){
    tgfile >> _physbrmemb[ip];
    if(tgfile.eof()){
      cerr<<"Trigger2LVL1::Init:Unexpected EOF reading  !!!"<<endl;
      exit(1);
    }
  }
//----->>> Phys.branches prescale-factors:  
  for(int ip=0;ip<8;ip++){
    tgfile >> _phbrprescf[ip];
    if(tgfile.eof()){
      cerr<<"Trigger2LVL1::Init:Unexpected EOF reading PhysBranchesPrescFactors !!!"<<endl;
      exit(1);
    }
  }
//-----
  tgfile>>endflab;
//
  tgfile.close();
//
  if(endflab==12345){
    cout<<"Trigger2LVL1::Init ======> Lvl1FTrigConfig-data are successfully read !"<<endl;
  }
  else{cout<<"Trigger2LVL1::Init ======> Error reading Lvl1FTrigConfig-data !"<<endl;
    exit(1);
  }
//
}
//---------
void Trigger2LVL1::ScalerMon::setdefs(){
// called by sitrig2initjob(for real data also)
//
  if(AMSJob::gethead()->isMCData()){//arbitrary mc-values(just to debugg)
    _FTtrig[0]=10;
    _FTtrig[1]=10;
    _FTtrig[2]=10;
    _FTtrig[3]=10;
    _FTtrig[4]=0;
    _LVL1trig[0]=1;
    _LVL1trig[1]=1;
    _LVL1trig[2]=1;
    _LVL1trig[3]=1;
    _LVL1trig[4]=1;
    _LVL1trig[5]=1;
    _LVL1trig[6]=1;
    _LVL1trig[7]=1;
    _LVL1trig[8]=0;
    for(int i=0;i<5;i++)_SPtrig[i]=0;
    for(int i=0;i<4;i++){
      _CPside1[i]=100;
      _CPside2[i]=100;
      _CTside1[i]=50;
      _CTside2[i]=50;
      _BZside1[i]=15;
      _BZside2[i]=15;
    }
    for(int i=0;i<8;i++){
      _AntiBot[i]=20;
      _AntiTop[i]=20;
    }
    _ECftProj[0]=60;
    _ECftProj[1]=60;
    _ECl1Proj[0]=30;
    _ECl1Proj[1]=30;
    _TrigTimeT=9999;
    _ScalTimeT=7777;
    _LiveTime[0]=6666;
    _LiveTime[1]=5555;
    _TrigFPGAid=11;
    _ScalFPGAid=9;
  }
  else if(AMSJob::gethead()->isRealData()){//set "-1" as "undefined"
    for(int i=0;i<5;i++)_FTtrig[i]=-1;
    for(int i=0;i<9;i++)_LVL1trig[i]=-1;
    for(int i=0;i<5;i++)_SPtrig[i]=-1;
    for(int i=0;i<4;i++){
      _CPside1[i]=-1;
      _CPside2[i]=-1;
      _CTside1[i]=-1;
      _CTside2[i]=-1;
      _BZside1[i]=-1;
      _BZside2[i]=-1;
    }
    for(int i=0;i<8;i++){
      _AntiBot[i]=-1;
      _AntiTop[i]=-1;
    }
    _ECftProj[0]=-1;
    _ECftProj[1]=-1;
    _ECl1Proj[0]=-1;
    _ECl1Proj[1]=-1;
    _TrigTimeT=-1;
    _ScalTimeT=-1;
    _LiveTime[0]=0;
    _LiveTime[1]=0;
    _TrigFPGAid=0;
    _ScalFPGAid=0;
  }
}  
//---------
void TGL1JobStat::printstat(){
  HPRINT(1097);
  HPRINT(1096);
  HPRINT(1095);
  HPRINT(1098);
  printf("\n");
  printf("    ============ LVL1-RDdecoding statistics =============\n");
  printf("\n");
  printf(" LVL1-segment entries                : % 8d\n",daqc1[0]);
  if(daqc1[0]>0){
    printf(" LVL1-segments non empty             : % 8d\n",daqc1[1]);
    printf(" ............. no_assembl_errors     : % 8d\n",daqc1[2]);
    printf(" ............. from A-side           : % 8d\n",daqc1[3]);
    printf(" ............. from B-side           : % 8d\n",daqc1[4]);
    printf(" TrigPatternBlock entries            : % 8d\n",daqc1[5]);
    printf(" ................ length OK          : % 8d\n",daqc1[6]);
    printf(" ScalersBlock entries                : % 8d\n",daqc1[7]);
    printf(" ............ length OK              : % 8d\n",daqc1[8]);
    printf(" TrigSetupBlock entries              : % 8d\n",daqc1[9]);
    printf(" .............. length OK            : % 8d\n",daqc1[10]);
    printf(" Total LVL1-segment bad exits        : % 8d\n",daqc1[11]);
  }
//
  printf("\n");
  printf("    ============ LVL1-RECO statistics =============\n");
  printf("\n");
  printf(" entries(MC/RD)                      : % 6d\n",countev[0]);
  if(AMSJob::gethead()->isRealData()){
    printf(" RD: DAQ-created lvl1-obj found      : % 6d\n",countev[15]);
    printf(" RD: missing patterns added          : % 6d\n",countev[16]);
  }
  else{
    printf(" MC: globFTriger(incl ext) OK      : % 6d\n",countev[1]);
    printf(" MC: with FTC(tofZ>=1) included    : % 6d\n",countev[2]);
    printf(" MC: with FTC&BZ(Z>=2) included    : % 6d\n",countev[3]);
    printf(" MC: with FTZ(tofSlowZ>=2) included: % 6d\n",countev[4]);
    printf(" MC: with FTZ at missing FTC       : % 6d\n",countev[5]);
    printf(" MC: with FTE(EcalEnergy) included : % 6d\n",countev[6]);
    printf(" MC: with FTE at missing TOF-FT    : % 6d\n",countev[7]);
    printf(" MC: with EXT(Ext.trig) included   : % 6d\n",countev[8]);
    printf(" MC: LVL1 conditions satisfied     : % 6d\n",countev[9]);
    printf(" MC:   + ScalerRate/LiveTime OK    : % 6d\n",countev[10]);
  }
  printf("--------\n");
}
//------
integer Trigger2LVL1::checktofpattor(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <TOF2GC::SCLRS);
#endif
 return ((_tofpatt1[tof])&(1 << paddle))||((_tofpatt1[tof])&(1 << 16+paddle));
}


integer Trigger2LVL1::checktofpattand(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <TOF2GC::SCLRS);
#endif
 return ((_tofpatt1[tof])&(1 << paddle))&&((_tofpatt1[tof])&(1 << 16+paddle));
}


void Trigger2LVL1::_writeEl(){

  LVL1Ntuple02* lvl1N = AMSJob::gethead()->getntuple()->Get_lvl102();

  if (lvl1N->Nlvl1>=MAXLVL1) return;

// Fill the ntuple
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  lvl1N->PhysBPatt[lvl1N->Nlvl1]=_PhysBPatt;
  lvl1N->JMembPatt[lvl1N->Nlvl1]=_JMembPatt;
  lvl1N->LiveTime[lvl1N->Nlvl1]=_LiveTime;
  lvl1N->TOFlag1[lvl1N->Nlvl1]=_tofflag1;
  lvl1N->TOFlag2[lvl1N->Nlvl1]=_tofflag2;
  int i,j;
  for(i=0;i<4;i++)lvl1N->TOFPatt1[lvl1N->Nlvl1][i]=_tofpatt1[i];
  for(i=0;i<4;i++)lvl1N->TOFPatt2[lvl1N->Nlvl1][i]=_tofpatt2[i];
  lvl1N->AntiPatt[lvl1N->Nlvl1]=_antipatt;
  lvl1N->ECALflag[lvl1N->Nlvl1]=_ecalflag;
  for(i=0;i<6;i++)for(j=0;j<3;j++)lvl1N->ECALpatt[lvl1N->Nlvl1][i][j]=_ectrpatt[i][j];
  lvl1N->ECALtrsum[lvl1N->Nlvl1]=_ectrsum;
  for(i=0;i<6;i++)lvl1N->TrigRates[lvl1N->Nlvl1][i]=_TrigRates[i];
  lvl1N->Nlvl1++;
}



void Trigger2LVL1::builddaq(integer i, integer n, int16u *p){
  Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->
  getheadC("TriggerLVL1",i);
  *p=getdaqid(0);
  //  if(ptr){
    //   *(p+1)=int16u(ptr->_tofpatt[0]);
    //   *(p+2)=int16u(ptr->_tofpatt[1]);
    //   *(p+3)=int16u(ptr->_tofpatt[2]);
    //   *(p+4)=int16u(ptr->_tofpatt[3]);
    //   *(p+5)=int16u(ptr->_antipatt);
    //   *(p+6)= ptr->_tofflag | ptr->_TriggerMode <<8;
  //  }
  for( i=1;i<n;i++)*(p+i)=0;
  if(ptr){
  // first anti
  // -x  -z ; -x +z ; +x -z ; +x +z
  // 5-12             1-4   13-16

   int16u anti8=  (ptr->_antipatt >>16) & 255;
   *(p+1)=*(p+1) | (anti8 <<8); 
/*
  {  
    integer cbt,lsbit(1);
    integer antipatt=ptr->_antipatt;
    integer nanti=0;
    for(i=4;i<12;i++){
     cbt=lsbit<<i;
     if((antipatt&cbt)>0)nanti++;
    }
    if(nanti>3)nanti=3;
    *(p+1)=*(p+1) | (nanti<<8) | (nanti<<10);
  }
  {  
    integer cbt,lsbit(1);
    integer antipatt=ptr->_antipatt;
    integer nanti=0;
    for(i=0;i<4;i++){
     cbt=lsbit<<i;
     if((antipatt&cbt)>0)nanti++;
    }
    for(i=12;i<16;i++){
     cbt=lsbit<<i;
     if((antipatt&cbt)>0)nanti++;
    }
    if(nanti>3)nanti=3;
    *(p+1)=*(p+1) | (nanti<<12) | (nanti<<14);
   }
*/

  // Word 2 Z > 1
  if((ptr->_tofflag1)%10==3)*(p+2)=*(p+2) | (1<<4);
  if(ptr->_tofflag1>10)*(p+2)=*(p+2) | (1<<5);

  // Words 3-10  
       *(p+3)=int16u(ptr->_tofpatt1[3]);
       *(p+4)=int16u(ptr->_tofpatt1[2]);
       *(p+5)=int16u(ptr->_tofpatt1[1]);
       *(p+6)=int16u(ptr->_tofpatt1[0]);

       *(p+7)=int16u((ptr->_tofpatt1[3])>>16);
       *(p+8)=int16u((ptr->_tofpatt1[2])>>16);
       *(p+9)=int16u((ptr->_tofpatt1[1])>>16);
       *(p+10)=int16u((ptr->_tofpatt1[0])>>16);
       for(i=3;i<11;i++){
         //swap bits
         int16u tmp=0;
         int16u tag=*(p+i);
         for(int k=0;k<16;k++)tmp=tmp | (( (tag >> k) & 1) << (13-k));
         *(p+i)=tmp;
       }
       // Words 11-18

       *(p+11)=int16u(ptr->_tofpatt2[3]);
       *(p+12)=int16u(ptr->_tofpatt2[2]);
       *(p+13)=int16u(ptr->_tofpatt2[1]);
       *(p+14)=int16u(ptr->_tofpatt2[0]);

       *(p+15)=int16u((ptr->_tofpatt2[3])>>16);
       *(p+16)=int16u((ptr->_tofpatt2[2])>>16);
       *(p+17)=int16u((ptr->_tofpatt2[1])>>16);
       *(p+18)=int16u((ptr->_tofpatt2[0])>>16);
       for(i=11;i<18;i++){
         //swap bits
         int16u tmp=0;
         int16u tag=*(p+i);
         for(int k=0;k<16;k++)tmp=tmp | (( (tag >> k) & 1) << (13-k));
         *(p+i)=tmp;
       }

  }   
}
//----------------------------------------------------
void Trigger2LVL1::buildraw(integer len, int16u *p){
//
// on input: len=tot_block_length as given at the beginning of block
//           *p=pointer_to_beggining_of_block_data (to the length word)
//
  {
   AMSContainer *ptr=AMSEvent::gethead()->getC("TriggerLVL1",0);
   if(ptr)ptr->eraseC();
   else cerr <<"Trigger2LVL1::buildraw-S-NoContainer"<<endl;

  }
//
  integer JMembPatt(0),PhysBPatt(0),TofFlag1(0),TofFlag2(0),AntiPatt(0),EcalFlag(0);
  integer TofPatt1[TOF2GC::SCLRS],TofPatt2[TOF2GC::SCLRS];
  int16u EcTrPatt[6][3];//PM(dyn) trig.patt for 6"trigger"-SupLayers(36of3x16bits for 36 dynodes)
  integer gftmsk(0),ftzlmsk(0);
  int16u ftzwdcode(0);
  static geant LiveTime1,LiveTime2,TrigRates[6];//static to save them if blocks come not simultaneously
  uinteger ntrst(0),timcal(0);
  uinteger time[2]={0,0};
  uinteger busypat[2]={0,0};//1st word->bits 0-31, 2nd word-> bits 32-39 of 40-bits busy patt.word
  bool busyerr(0);
//
  integer tofpat1[4]={0,0,0,0};//will be added to ready lev1-object later(after decoding of TOF-data)
  integer tofpat2[4]={0,0,0,0};//..................................................
  int16u ecpat[6][3]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//.....(after decoding of EC-data)
  geant ectrs=0;//........................................................................
//
  integer i,j,iw,il,ib,is,ic,lent,dlen,wvar;
  int16u blid,btyp,naddr,word;
  uinteger lword;
  integer ltim(0);
  geant tgate;
  int16u timgid;
//  
  int16u datyp,formt,evnum;
  int16u jbias,jblid,jleng,jaddr,csid,psfcode;
  int16u prescfcode[7]={0x3FF,0x3FE,0x3FD,0x3FC,0x3FB,0x3FA,0x3F9};//prescale factors codes
  int16u prescfvals[7]={1,2,5,10,20,50,100};//presc.factors(N->N:1)
  int16u cpmask(0),ctmask(0),bzmask(0),trstmsk(0);
  uinteger febusymsk[2]={0,0};//to store 24+16 bits of FEbusy-mask
//
  int ltimbias(49);//tempor bias to live-time data(in "trig"-block)
  int trgsbias(15);//bias to trig.setup/status block
  int scaltgbs(1);//tempor bias to scalers time-gate(in scaler's block)
  int scalbias(2);//tempor bias to scalers data(...)
//
  TGL1JobStat::daqs1(0);//count entries
  jleng=*p;//fragment's 1st word(block length excluding length-word itself)
  jblid=*(p+jleng);// JLV1 fragment's last word: Status+slaveID(its id)
//
  bool dataf=((jblid&(0x8000))>0);//data-fragment
  bool crcer=((jblid&(0x4000))>0);//CRC-error
  bool asser=((jblid&(0x2000))>0);//assembly-error
  bool amswer=((jblid&(0x1000))>0);//amsw-error   
  bool timoer=((jblid&(0x0800))>0);//timeout-error   
  bool fpower=((jblid&(0x0400))>0);//FEpower-error   
  bool seqer=((jblid&(0x0400))>0);//sequencer-error
  bool cdpnod=((jblid&(0x0020))>0);//CDP-node(like EDR2-node with no futher fragmentation)
  bool noerr;
  jaddr=jblid&(0x001F);//slaveID(="NodeAddr"=JLV1addr here)(one of 2 permitted(sides a/b))
//  cout<<"====> InBuildRaw: JLV1_length="<<jleng<<" JLV1-addr:"<<jaddr<<endl;
  if(jleng>1)TGL1JobStat::daqs1(1);//<=== count non-empty fragments
  else goto BadExit;
  noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
  if(noerr)TGL1JobStat::daqs1(2);//<=== count no_ass_errors JLV1-fragments for given DATA-type     
  else goto BadExit;
  node2side(jaddr,csid);//card_side(1-2<->a-b)
//  cout<<"     side="<<csid<<endl;
  if(csid>0)TGL1JobStat::daqs1(3+csid-1);//sides sharing of "no_ass_err" entries
  else goto BadExit;
// 
//
  if(btyp==7){//tempor ---> "Event-by-event" trig-info(patterns,livetime,...)
    TGL1JobStat::daqs1(5);//"TrigPatternsBlock" entries
    if(len!=20){
      cout<<"Trigger2LVL1::buildraw:TriPattBlock length error, len="<<jleng<<" addr="<<jaddr<<endl;
      goto BadExit;
    }
    TGL1JobStat::daqs1(6);//"TrigPatternsBlock" length OK
    word=*(p+1);//JMembPatt(FTC,FTCP0,...FTZ,...EXT-GATE1
    JMembPatt=integer(word);
    word=*(p+2);//Lev1Patt
    PhysBPatt=integer(word&0x1BFF);
    word=*(p+3);//AntiPatt
    AntiPatt=integer(word&0x00FF);
    word=*(p+4);//Tof CP,CT,BZ layers pattern
    if((word&0x000F)==0x000F)TofFlag1=0;//all4
    else if((word&0x000F)==0x000E)TofFlag1=1;//2,3,4(miss1)
    else if((word&0x000F)==0x000D)TofFlag1=2;//1,3,4(miss2)
    else if((word&0x000F)==0x000B)TofFlag1=3;//1,2,4(miss3)
    else if((word&0x000F)==0x0007)TofFlag1=4;//1,2,3(miss4)
    else if((word&0x000F)==0x0005)TofFlag1=5;//1,3
    else if((word&0x000F)==0x0009)TofFlag1=6;//1,4
    else if((word&0x000F)==0x0006)TofFlag1=7;//2,3
    else if((word&0x000F)==0x000A)TofFlag1=8;//2,4
    else TofFlag1=-1;
//
    word=word>>8;//to select BZ
    if((word&0x000F)==0x000F)TofFlag2=0;
    else if((word&0x000F)==0x000E)TofFlag2=1;
    else if((word&0x000F)==0x000D)TofFlag2=2;
    else if((word&0x000F)==0x000B)TofFlag2=3;
    else if((word&0x000F)==0x0007)TofFlag2=4;
    else if((word&0x000F)==0x0005)TofFlag2=5;
    else if((word&0x000F)==0x0009)TofFlag2=6;
    else if((word&0x000F)==0x0006)TofFlag2=7;
    else if((word&0x000F)==0x000A)TofFlag2=8;
    else TofFlag2=-1;
// 
    word=*(p+5);//1st 16bits of time
    time[0]|=word;
    lword=uinteger(*(p+6));//2nd 16bits of time
    time[0]|=lword<<16;
    word=*(p+7);//last 8bits of time +1st 8bits of ntrst
    time[1]=(word&0x00FF);
    ntrst|=((word&0xFF00)>>8);
    lword=uinteger(*(p+8));//last 16bits of ntrst
    ntrst|=(lword<<8);
//
    word=*(p+9);//busy-patt,err
    busyerr=((word&1)>0);
    if(!busyerr){
      busypat[0]|=((word&0xFF00)>>8);//1st 8bits of patt
      lword=uinteger(*(p+10));//next 16bits of patt
      busypat[0]|=(lword<<8);
      lword=uinteger(*(p+11));//last 16bits of patt
      busypat[0]|=(lword<<24);
      busypat[1]|=(lword>>8);
    }
//---> attention: for the moment i suppose that 0x0C and 0x0D words do not exist !!! tempor
    word=*(p+12);//1st 16bits of time-calib word
    timcal|=word;
    lword=uinteger(*(p+13));//last 16bits of time-calib word
    timcal|=(lword<<16);
//                    --->LiveTime "all busy":
    word=*(p+ltimbias);//1st 16bits of live-time
    ltim|=word;
    lword=uinteger(*(p+ltimbias+1));//last 11bits of live_time + time_gate.id
    ltim|=((lword&0x07FF)<<16);
    timgid|=((lword&0x3000)>>12);//2bits of time_gate.id
    if(timgid==3)tgate=2;//time-gate(sec)
    else if(timgid==0)tgate=0.25;
    else if(timgid==1)tgate=0.5;
    else tgate=1;
    LiveTime1=ltim*(2.e-8)/tgate;//livetime fraction(imply 20ns pulses period)
    if(LiveTime1>1){
      cout<<"Trigger2LVL1::buildraw:Warning - LiveTime1 fraction >1!, tg/lt="<<tgate<<" "<<ltim<<endl;
      LiveTime1=1; 
    } 
//                    --->LiveTime "FE busy":
    word=*(p+ltimbias+2);//1st 16bits of live-time
    ltim|=word;
    lword=uinteger(*(p+ltimbias+3));//last 11bits of live_time + time_gate.id
    ltim|=((lword&0x07FF)<<16);
    timgid|=((lword&0x3000)>>12);//2bits of time_gate.id
    if(timgid==3)tgate=2;//time-gate(sec)
    else if(timgid==0)tgate=0.25;
    else if(timgid==1)tgate=0.5;
    else tgate=1;
    LiveTime2=ltim*(2.e-8)/tgate;//livetime fraction(imply 20ns pulses period)
    if(LiveTime2>1){
      cout<<"Trigger2LVL1::buildraw:Warning - LiveTime2 fraction >1!, tg/lt="<<tgate<<" "<<ltim<<endl;
      LiveTime2=1; 
    }
  }//--->endof "Event trig.block" 
//-----------
  if(btyp==8){//tempor <--- update scalers "from-time-to-time" block
    TGL1JobStat::daqs1(7);//"ScalersBlock" entries
    if(len!=78){//77+1(blid)
      cout<<"Trigger2LVL1::buildraw:ScalersBlock length, len="<<len<<" addr="<<jaddr<<endl;
      goto BadExit;
    }
    TGL1JobStat::daqs1(8);//"ScalersBlock" length OK
    word=*(p+scaltgbs);
    timgid=((word&0x3000)>>12);//time-gate for scalers
    if(timgid==3)tgate=2;//time-gate(sec)
    else if(timgid==0)tgate=0.25;
    else if(timgid==1)tgate=0.5;
    else tgate=1;
    for(i=0;i<5;i++){
      word=*(p+scalbias+i);
      scalmon.FTtrig(i)=number(word)/tgate;//set FTs rates
    }
    
    for(i=0;i<9;i++){
      word=*(p+scalbias+5+i);
      scalmon.LVL1trig(i)=number(word)/tgate;//Lev1,subLev1 rates
    }
    
    for(i=0;i<5;i++){
      word=*(p+scalbias+14+i);
      scalmon.SPtrig(i)=number(word)/tgate;//LA0/1,Ext,DSP,Int rates
    }
    
    word=*(p+scalbias+23);
    scalmon.CPside1(0)=number(word)/tgate;//CP L1S1 rate
    word=*(p+scalbias+28);
    scalmon.CPside1(1)=number(word)/tgate;//CP L2S1 rate
    word=*(p+scalbias+29);
    scalmon.CPside1(2)=number(word)/tgate;//CP L3S1 rate
    word=*(p+scalbias+30);
    scalmon.CPside1(3)=number(word)/tgate;//CP L4S1 rate
    word=*(p+scalbias+27);
    scalmon.CPside2(0)=number(word)/tgate;//CP L1S2 rate
    word=*(p+scalbias+24);
    scalmon.CPside2(1)=number(word)/tgate;//CP L2S2 rate
    word=*(p+scalbias+25);
    scalmon.CPside2(2)=number(word)/tgate;//CP L3S2 rate
    word=*(p+scalbias+26);
    scalmon.CPside2(3)=number(word)/tgate;//CP L4S2 rate
    
    word=*(p+scalbias+31);
    scalmon.CTside1(0)=number(word)/tgate;//CT L1S1 rate
    word=*(p+scalbias+36);
    scalmon.CTside1(1)=number(word)/tgate;//CT L2S1 rate
    word=*(p+scalbias+37);
    scalmon.CTside1(2)=number(word)/tgate;//CT L3S1 rate
    word=*(p+scalbias+38);
    scalmon.CTside1(3)=number(word)/tgate;//CT L4S1 rate
    word=*(p+scalbias+35);
    scalmon.CTside2(0)=number(word)/tgate;//CT L1S2 rate
    word=*(p+scalbias+32);
    scalmon.CTside2(1)=number(word)/tgate;//CT L2S2 rate
    word=*(p+scalbias+33);
    scalmon.CTside2(2)=number(word)/tgate;//CT L3S2 rate
    word=*(p+scalbias+34);
    scalmon.CTside2(3)=number(word)/tgate;//CT L4S2 rate
    
    word=*(p+scalbias+39);
    scalmon.BZside1(0)=number(word)/tgate;//BZ L1S1 rate
    word=*(p+scalbias+44);
    scalmon.BZside1(1)=number(word)/tgate;//BZ L2S1 rate
    word=*(p+scalbias+45);
    scalmon.BZside1(2)=number(word)/tgate;//BZ L3S1 rate
    word=*(p+scalbias+46);
    scalmon.BZside1(3)=number(word)/tgate;//BZ L4S1 rate
    word=*(p+scalbias+43);
    scalmon.BZside2(0)=number(word)/tgate;//BZ L1S2 rate
    word=*(p+scalbias+40);
    scalmon.BZside2(1)=number(word)/tgate;//BZ L2S2 rate
    word=*(p+scalbias+41);
    scalmon.BZside2(2)=number(word)/tgate;//BZ L3S2 rate
    word=*(p+scalbias+42);
    scalmon.BZside2(3)=number(word)/tgate;//BZ L4S2 rate
    
    for(i=0;i<8;i++){
      word=*(p+scalbias+47+i);
      scalmon.AntiTop(i)=number(word)/tgate;//Anti TopSectors rates
      word=*(p+scalbias+55+i);
      scalmon.AntiBot(i)=number(word)/tgate;//Anti BotSectors rates
    }
    
    for(i=0;i<2;i++){
      word=*(p+scalbias+63+i);
      scalmon.ECftProj(i)=number(word)/tgate;//Ecal FTproj rates
      word=*(p+scalbias+65+i);
      scalmon.ECl1Proj(i)=number(word)/tgate;//Ecal Lev1(angle)proj rates
    }
    scalmon.LiveTime(0)=LiveTime1;//set already known(from "trig"-block) livetimes
    scalmon.LiveTime(1)=LiveTime2;//............
//  ---> store some rates for lnl1-object creation(need for online monitoring):
    TrigRates[0]=scalmon.FTrate();
    TrigRates[1]=scalmon.FTCrate();
    TrigRates[2]=scalmon.LVL1rate();
    TrigRates[3]=scalmon.TOFrateMX();
    TrigRates[4]=scalmon.ECftrateMX();
    TrigRates[5]=scalmon.AntirateMX();
  }//---> endof scalers block
//------------
  if(btyp==9){//tempor ---> trig-config info(masks,...)
    TGL1JobStat::daqs1(9);//"TrigSetupBlock" entries
    if(len!=45){
      cout<<"Trigger2LVL1::buildraw:TrigSetupBlock length error, len="<<len<<" addr="<<jaddr<<endl;
      goto BadExit;
    }
    TGL1JobStat::daqs1(10);//"TrigSetupBlock" length OK
    word=*(p+trgsbias);//Anti-mask
    for(i=0;i<ANTI2C::MAXANTI;i++){//update antinmask
      if((word&1<<(8+i))==0 && (word&1<<i)==0)l1trigconf.antinmask(i)=1;//s1&s2 "in"
      else if((word&1<<(8+i))==0)l1trigconf.antinmask(i)=2;//s1 "in"
      else if((word&1<<i)==0)l1trigconf.antinmask(i)=3;//s2 "in"
      else l1trigconf.antinmask(i)=0;
    }
//---   CP/CT/BZ-masks are not used for the moment(just saved)
    word=*(p+trgsbias+1);//cp/ct-masks
    cpmask=(word&0x00FF);//cp-mask(original, not convinient bits order)
    ctmask=((word&0xFF00)>>8);//ct-mask(original, not convinient bits order)
    bzmask=(word&0x00FF);//bz-mask(original, not convinient bits order)
//
    wvar=0;
    word=*(p+trgsbias+2);
    if((word&1<<8)==0)wvar+=1;//ECft x-proj active
    if((word&1<<9)==0)wvar+=10;//ECft y-proj active
    if((word&1<<10)==0)wvar+=100;//ECangl x-proj active
    if((word&1<<11)==0)wvar+=1000;//ECangl y-proj active
    l1trigconf.ecprjmask()=wvar;//update ecprjmask
//---   TRST/FEbusy-masks are not used for the moment(just stored)
    word=*(p+trgsbias+3);
    trstmsk=(word&0xFF);//raw TRST-mask(bits(0-7) positions unchanged)
    febusymsk[0]|=((word&0xFF00)>>8);//FE-busy bits 0-7
    lword=uinteger(*(p+trgsbias+4));
    febusymsk[0]|=(lword<<8);//add FE-busy bits 8-23
    word=*(p+trgsbias+5);
    febusymsk[1]=uinteger(word);//last 16 FEbusy bits(24-39) are stored as lsbits of febusymsk[1]
//---
    word=*(p+trgsbias+6);
    for(i=0;i<TOF2GC::SCLRS;i++){//update tofoamask and tofoazmask
      if((word&1<<i)>0)l1trigconf.tofoamask(i)=1;//or for CP-sides(FTC)
      else l1trigconf.tofoamask(i)=0;//and
      if((word&1<<(i+8))>0)l1trigconf.tofoazmask(i)=1;//or for BZ-sides
      else l1trigconf.tofoazmask(i)=0;//and
    }
//---
    word=*(p+trgsbias+7);
    for(i=0;i<ANTI2C::MAXANTI;i++){//update antioamask
      if((word&1<<i)>0)l1trigconf.antoamask(i)=1;//or
    }
//---          ignore 2words with InternTrig period, LA-trig
//---
    word=*(p+trgsbias+10);//settings for FT
    if((word&1)==0)gftmsk+=1;
    if((word&2)==0)gftmsk+=10;
    if((word&4)==0)gftmsk+=100;
    l1trigconf.globftmask()=gftmsk;//update globFTmask(FTE|FTZ|FTC)
    if((word&(1<<7))>0)ftzlmsk+=10;//OR of top/bot in FTZ-trig
    if((word&(1<<5))>0 & (word&(1<<6))>0)ftzlmsk+=3;//(1|2)+(3|4)
    else if((word&(1<<5))>0 & (word&(1<<6))==0)ftzlmsk+=2;//(1|2)+(3&4)
    else if((word&(1<<5))==0 & (word&(1<<6))>0)ftzlmsk+=1;//(1&2)+(3|4)
    l1trigconf.toflcsz()=ftzlmsk;//update FTZ layers or/and mask
    if((word&(1<<8))>0)l1trigconf.ecorand()=1;//update FTE-proj or/and(1->or)
    else l1trigconf.ecorand()=2;//update FTE-proj or/and(2->and)
//---
    word=*(p+trgsbias+11);//settings for TOF-lut0
    l1trigconf.toflut1()=integer(word);//update TOF lut0
    word=*(p+trgsbias+12);//settings for TOF-lut1
    l1trigconf.toflut2()=integer(word);//update TOF lut1
    word=*(p+trgsbias+13);//settings for TOF-lutBZ
    l1trigconf.toflutbz()=integer(word);//update TOF lutBZ
//---
    word=*(p+trgsbias+14);//settings for FTZ top/bot-signal's extention width
    ftzwdcode=(word&0x1F);//topTOF p.width extention code
    ftzwdcode|=((word&0x1F00)>>8);//botTOF p.width extention code
    l1trigconf.tofextwid()=integer(ftzwdcode);
//---
    word=*(p+trgsbias+15);//settings for Lev1-phys.branches mask and Acc0/1-thresholds
    l1trigconf.globl1mask()=integer((~(word&0xFF))&0xFF);//update Lev1-phys.branches mask
    l1trigconf.antsectmx(0)=integer((word&0xF00)>>8);//Max. Anti-sectors(equat. region)=1 
    l1trigconf.antsectmx(1)=integer((word&0xF000)>>12);//Max. Anti-sectors(polar region)=0 
//---
    for(i=0;i<8;i++){//update PhysBranche's members(composition)
      word=*(p+trgsbias+16+i);//particulal PhysBranch composition mask(disable bits)
      l1trigconf.physbrmemb(i)=integer(~(word&0x7FFF));//Lin(disable)->Me(anable),except bit15 !
    }
//---
    for(i=0;i<8;i++){//PhysBranches loop
      word=*(p+trgsbias+24+i);
      psfcode=(word&0x3FF);//presc.factor code
      for(j=0;j<7;j++){
        if(psfcode==prescfcode[j])break;
      }
      if(j<7)l1trigconf.phbrprescf(i)=prescfvals[j];//update presc.factor's values for PhysBranches
      else{
        l1trigconf.phbrprescf(i)=1;//set presc.fact.=1 for unknown codes
        cout<<"Trigger2LVL1::buildraw:Unknown prescFactor code="<<psfcode<<" PhBr="<<i<<endl;
      }
    }
  }//---> endof trig-config block
//
//---------> create Lev1-object:
// 
//  integer tm=int(floor(TOF2Varp::tofvpar.getmeantoftemp(0)));
     
  if(scalmon.TOFrateMX()<TGL1FFKEY.MaxScalersRate && LiveTime1>TGL1FFKEY.MinLifeTime)
      AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0), new Trigger2LVL1(PhysBPatt,JMembPatt,
         TofFlag1,TofFlag2,tofpat1,tofpat2,AntiPatt,EcalFlag,ecpat,ectrs,LiveTime1,TrigRates));
  else if(AMSJob::gethead()->isRealData()
                                         && scalmon.TOFrateMX()>=TGL1FFKEY.MaxScalersRate
			                 && LiveTime1<=TGL1FFKEY.MinLifeTime)AMSEvent::gethead()->seterror();
  return;
//
BadExit:
  TGL1JobStat::daqs1(11);//count rejected LVL1-entries(segments)    
}
//------------------------------------------------------------

uinteger Trigger2LVL1::Scalers::_GetIndex(time_t time ){
static integer k=0;
// first check k; then either k+1 or run binary search

  if(time>=_Tls[0][k] && time < (k<_Nentries-1?_Tls[0][k+1]:time+1)){
   return k;
  }
  else if(k++<_Nentries-1 ){
   if(time>=_Tls[0][k] && time < (k<_Nentries-1?_Tls[0][k+1]:time+1)){
   return k;
   }
  }
  k=abs(AMSbins(_Tls[0], uinteger(time), _Nentries))-1;   
  if(k<0){
   cerr<<" Trigger2LVL1::Scalers::_GetIndex-S-IndexLessZero "<<k<<" "<<time<<endl;
   k=0;
   AMSEvent::gethead()->seterror();   
  }   
return k;
}
//------------- DAQ interface: --------------
integer Trigger2LVL1::checkdaqid(int16u blid){
  int valid(0);
  for(int i=0;i<2;i++)if((blid & (0x001F)) == nodeids[i])valid=1;
  return valid;
}
void Trigger2LVL1::node2side(int16u nodeid, int16u &side){
//  on input: nodeid=(blid&0x001F)
//  on output: side=(1,2) -> (a,b), or 0 if nodeid is missing
  side=0;
  for(int i=0;i<2;i++)if(nodeid == nodeids[i])side=i+1;
}




