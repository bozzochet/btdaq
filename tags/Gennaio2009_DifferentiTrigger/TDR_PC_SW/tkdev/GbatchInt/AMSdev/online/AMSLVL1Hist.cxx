//  $Id: AMSLVL1Hist.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
//       v1.0/E.Choumilov/20.06.2003
#include <iostream>
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "AMSLVL1Hist.h"

char Lvl1Pars::dat1[30];
char Lvl1Pars::dat2[30];
char Lvl1Pars::dat3[30];

void AMSLVL1Hist::Book(){

  AddSet("FT/LVL1 Members_Sharing");
  
  _filled.push_back(new TH1F("trigh0","FastTriggerMembers Sharing",10,0.,10.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("trigh1","BasisTrigMembers Sharing",16,0.,16.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("trigh2","Lvl1PhysBranches Sharing",8,0.,8.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
   

  AddSet("TOF-TotMultiplicity in LVL1");
  
  _filled.push_back(new TH1F("trigh3","TOF-layers per events, Z>=1(ftc)",5,0.,5.));
  _filled[_filled.size()-1]->SetXTitle("Total layers");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("trigh4","TOF-layers per events, Z>=2(bz)",5,0.,5.));
  _filled[_filled.size()-1]->SetXTitle("Total layers");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  _filled.push_back(new TH1F("trigh5","TOF-paddles per events, Z>=1(ftc)",35,0.,35.));
  _filled[_filled.size()-1]->SetXTitle("Total paddles");
  _filled[_filled.size()-1]->SetFillColor(3);

  _filled.push_back(new TH1F("trigh6","TOF-paddles per events, Z>=2(bz)",35,0.,35.));
  _filled[_filled.size()-1]->SetXTitle("Total paddles");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  
  AddSet("TOF-patterns in LVL1");
  
  _filled.push_back(new TH1F("trigh7","TOF MissingPlaneCode, Z>=1(ftc)",10,-1.,9.));
  _filled[_filled.size()-1]->SetXTitle("MissingPlaneCode");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("trigh8","TOF-pads pattern in Z>=1(Side1,globFT)",40,1.,41.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(3);
   
  _filled.push_back(new TH1F("trigh9","TOF-pads pattern Z>=1(Side2,globFT)",40,1.,41.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(3);
   
  _filled.push_back(new TH1F("trigh10","TOF MissingPlaneCode, Z>=2(bz)",10,-1.,9.));
  _filled[_filled.size()-1]->SetXTitle("MissingPlaneCode");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  _filled.push_back(new TH1F("trigh11","TOF-pads pattern in Z>=2(Side1,globFT)",40,1.,41.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(4);
   
  _filled.push_back(new TH1F("trigh12","TOF-pads pattern in Z>=2(Side2,globFT)",40,1.,41.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(4);
   
  AddSet("ANTIpatterns in LVL1");
  
  _filled.push_back(new TH1F("trigh13","ANTI-sectors(logical) pattern(globFT)",8,1.,9.));
  _filled[_filled.size()-1]->SetXTitle("Sector number");
  _filled[_filled.size()-1]->SetFillColor(2);
  
  _filled.push_back(new TH1F("trigh14","Total Anti-sectors, Z>=1(ftc)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  _filled.push_back(new TH1F("trigh15","Total Anti-sectors, Z>=2(bz)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  _filled.push_back(new TH1F("trigh16","Total Anti-sectors, SlowZ>=2(ftz)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  _filled.push_back(new TH1F("trigh17","Total Anti-sectors, EC&TOF(fte+tof)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  _filled.push_back(new TH1F("trigh18","Total Anti-sectors, EC&!TOF(fte-tof)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  AddSet("ECAL in LVL1(1)");
  
  _filled.push_back(new TH1F("trigh19","EcalMultiplFlag(fte, noTOF) ",4,0.,4.));
  _filled[_filled.size()-1]->SetXTitle("MultiplFlag: <MipThr(0)/>MipThr(1)/>LowMult(2)/>HiMylt(3=fte)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("trigh20","EcalAngleFlag(fte, noTOF) ",3,0.,3.));
  _filled[_filled.size()-1]->SetXTitle("AngleFlag(:Unkn(0)/Bad(1)/OK(2->gamma)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("trigh21","EcalMultiplFlag(globFT) ",4,0.,4.));
  _filled[_filled.size()-1]->SetXTitle("MultiplFlag: <MipThr(0)/>MipThr(1)/>LowMult(2)/>HiMylt(3=fte)");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  _filled.push_back(new TH1F("trigh22","EcalMultiplFlag(fte, &TOF) ",4,0.,4.));
  _filled[_filled.size()-1]->SetXTitle("MultiplFlag: <MipThr(0)/>MipThr(1)/>LowMult(2)/>HiMylt(3=fte)");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  _filled.push_back(new TH1F("trigh23","EcalAngleFlag(fte, &TOF) ",3,0.,3.));
  _filled[_filled.size()-1]->SetXTitle("AngleFlag(:Unkn(0)/Bad(1)/OK(2->electr)");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  AddSet("ECAL in LVL1(2)");
  
  _filled.push_back(new TH1F("trigh24","EcalTrigPatt(X-proj:Slayers 2,4,6)",120,1.,121.));
  _filled[_filled.size()-1]->SetXTitle("PhMultiplierNumber: Npm+(Sl/2-1)*40");
  _filled[_filled.size()-1]->SetYTitle("Number of events");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("trigh25","EcalTrigPatt(Y-proj:Slayers 3,5,7)",120,1.,121.));
  _filled[_filled.size()-1]->SetXTitle("PhMultiplierNumber: Npm+((Sl-1)/2-1)*40");
  _filled[_filled.size()-1]->SetYTitle("Number of events");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  
  AddSet("LiveTime in LVL1");
  
  _filled.push_back(new TProfile("trigh26","AMSDAQLiveTime vs Time",120,0,lev1trange[0],0,1.1));
  _filled[_filled.size()-1]->SetYTitle("LiveTime fraction");
  _filled.push_back(new TProfile("trigh27","AMSDAQLiveTime vs Time",120,0,lev1trange[1],0,1.1));
  _filled[_filled.size()-1]->SetYTitle("LiveTime fraction");
  _filled.push_back(new TProfile("trigh28","AMSDAQLiveTime vs Time",120,0,lev1trange[2],0,1.1));
  _filled[_filled.size()-1]->SetYTitle("LiveTime fraction");
  
  
  AddSet("TrigRates in LVL1");
  
  _filled.push_back(new TProfile("trigh29","Lev1Trig-rate vs Time",120,0,lev1trange[0],0,10000.));
  _filled[_filled.size()-1]->SetYTitle("Rate(Hz)");
  _filled.push_back(new TProfile("trigh30","TOFplane-MaxRate vs Time",120,0,lev1trange[0],0,10000.));
  _filled[_filled.size()-1]->SetYTitle("Rate(Hz)");
  _filled.push_back(new TProfile("trigh31","ECFT-MaxRate vs Time",120,0,lev1trange[0],0,10000.));
  _filled[_filled.size()-1]->SetYTitle("Rate(Hz)");
}


void AMSLVL1Hist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave = gPad;
  int i;
  TAxis *xax;
  char name[60],dat[30];
//

  switch(Set){
case 0:
  gPad->Divide(1,3);
  gPad->cd(1);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[0]->GetXaxis();
  xax->SetLabelSize(0.1);
  xax->SetBinLabel(1,"Entries");
  xax->SetBinLabel(2,"Lvl1");
  xax->SetBinLabel(3,"globFT");
  xax->SetBinLabel(4,"Z>=1(ftc)");
  xax->SetBinLabel(5,"SlowZ>=2(ftz)");
  xax->SetBinLabel(6,"EcFT(fte)");
  xax->SetBinLabel(7,"Z>=2(bz)");
  xax->SetBinLabel(8,"VerySlowZ>=2");
  xax->SetBinLabel(9,"Tof && Ec");
  xax->SetBinLabel(10,"Ec-alone");
  _filled[0]->SetStats(kFALSE);
  _filled[0]->Draw("hbar2");//FTMembers sharing  
  gPadSave->cd();
//  
  gPad->cd(2);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[1]->GetXaxis();
  xax->SetLabelSize(0.08);
  xax->SetBinLabel(1,"Z>=1(ftc)");
  xax->SetBinLabel(2,"Lut1(ftcp0)");
  xax->SetBinLabel(3,"Lut2(ftcp1)");
  xax->SetBinLabel(4,"ftct0");
  xax->SetBinLabel(5,"ftct1");
  xax->SetBinLabel(6,"SlowZ>=2(ftz)");
  xax->SetBinLabel(7,"EcFt(fte)");
  xax->SetBinLabel(8,"NAntiS<=1(acco)");
  xax->SetBinLabel(9,"NAntiS=0(acc1)");
  xax->SetBinLabel(10,"Z>=2(bz)");
  xax->SetBinLabel(11,"EcMultAnd");
  xax->SetBinLabel(12,"EcMultOr");
  xax->SetBinLabel(13,"EcAngleAnd");
  xax->SetBinLabel(14,"EcAngleOr");
  xax->SetBinLabel(15,"ExtGate0");
  xax->SetBinLabel(16,"ExtGate1");
  _filled[1]->SetStats(kFALSE);
  _filled[1]->Draw("hbar2");//BasisTrigMembers sharing  
  gPadSave->cd();
//  
  gPad->cd(3);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[2]->GetXaxis();
  xax->SetLabelSize(0.1);
  xax->SetBinLabel(1,"UnbiasedTof");
  xax->SetBinLabel(2,"Z>=1(ftc+anti)");
  xax->SetBinLabel(3,"Z>=2(bz)");
  xax->SetBinLabel(4,"SlowZ>=2(ftz)");
  xax->SetBinLabel(5,"El(ftc+fte)");
  xax->SetBinLabel(6,"Ph(fte+EcAng)");
  xax->SetBinLabel(7,"UnbEc(fte)");
  xax->SetBinLabel(8,"Ext");
  _filled[2]->SetStats(kFALSE);
  _filled[2]->Draw("hbar2");//Lvl1PhysBranches sharing  
  gPadSave->cd();
//
  break;
case 1:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+3]->Draw();//TOF-TotMultiplicity
    gPadSave->cd();
  }
  break;
case 2:
  gPad->Divide(3,2);
  for(i=0;i<6;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+7]->Draw();//TOF-patterns
    gPadSave->cd();
  }
  break;
case 3:
  gPad->Divide(3,2);
  for(i=0;i<6;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+13]->Draw();//ANTIpatterns
    gPadSave->cd();
  }
  break;

case 4:
  gPad->Divide(3,2);
  for(i=0;i<5;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+19]->Draw();//ECAL(1) 
    gPadSave->cd();
  }
  break;
case 5:
  gPad->Divide(1,2);
  for(i=0;i<2;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(100010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+24]->Draw();//ECAL(2) 
    gPadSave->cd();
  }
  break;
case 6:
  gPad->Divide(1,3);
  for(i=0;i<3;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(100010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+26]->SetMinimum(0);
    _filled[i+26]->SetMaximum(1.05);
    _filled[i+26]->SetMarkerStyle(20);
    _filled[i+26]->SetMarkerColor(2);
    _filled[i+26]->SetMarkerSize(0.5);
    if(i==0){
      strcpy(name,"Last 120mins since ");
      strcpy(dat,Lvl1Pars::getdat1());
    }
    if(i==1){
      strcpy(name,"Last 60hours since ");
      strcpy(dat,Lvl1Pars::getdat2());
    }
    if(i==2){
      strcpy(name,"Last 360days since ");
      strcpy(dat,Lvl1Pars::getdat3());
    }
    strcat(name,dat);
    xax=_filled[i+26]->GetXaxis();
    xax->SetTitle(name);
    xax->SetTitleSize(0.05);
    _filled[i+26]->Draw("P");//LiveTime 
    gPadSave->cd();
  }
  break;
case 7:
  gPad->Divide(1,3);
  for(i=0;i<3;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(100010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
//    _filled[i+26]->SetMinimum(0);
//    _filled[i+26]->SetMaximum(1.05);
    _filled[i+29]->SetMarkerStyle(20);
    _filled[i+29]->SetMarkerColor(2);
    _filled[i+29]->SetMarkerSize(0.5);
    strcpy(name,"Last 120mins since ");
    strcpy(dat,Lvl1Pars::getdat1());
    strcat(name,dat);
    xax=_filled[i+29]->GetXaxis();
    xax->SetTitle(name);
    xax->SetTitleSize(0.05);
    _filled[i+29]->Draw("P");//Rates 
    gPadSave->cd();
  }
//
  }
//
}



void AMSLVL1Hist::Fill(AMSNtupleR *ntuple){
  int lmax(4);//tot tof-layers
  int padpl[4]={8,8,10,8};//tof-paddles per layer
  int tofflg1(-1),tofflg2(-1),tofpat1[4],tofpat2[4],npds1(0),npds2(0);
  int ecflg(-1),etflg(0),ewflg(0);
  int physbpat(0),membpat(0);
  int mflg(0),ntofl(0),ntofp(0),mlcode1(-1),mlcode2(-1),tgmode,lsbt;
  float ltime(1);
  int etime[2],evnum,runum;
  char date[30];
  bool ftc(0),ftz(0),bz(0),fte(0),glft(0);
  static float range[3],timez[3];
  static int first(1),etime0(0),evnloc;
  float time[3];
  float rates[6];
  UShort_t ecpatt[6][3]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//
//  cout<<"Start evnum:"<<evnloc<<endl;
  _filled[0]->Fill(float(0),1.);//entries
//
  etime[0]=ntuple->fHeader.Time[0];//unix-time(sec)
  etime[1]=ntuple->fHeader.Time[1];
  evnum=ntuple->fHeader.Event;
  runum=ntuple->fHeader.Run;
  strcpy(date,ntuple->GetTime());
//
  if(first==1){
    first=0;
    etime0=etime[0];
    cout<<"LVL1: 1st event Run/event:"<<runum<<" "<<evnum<<" date:"<<date<<" evloc="<<evnloc<<endl;
    for(int i=0;i<3;i++){
      timez[i]=0;
    }
    evnloc=0;
    Lvl1Pars::setdat1(ntuple->GetTime());
    Lvl1Pars::setdat2(ntuple->GetTime());
    Lvl1Pars::setdat3(ntuple->GetTime());
  }
  time[0]=(etime[0]-etime0)/60;//ev.time starting from beg.of.run(min)
  time[1]=(etime[0]-etime0)/3600;//ev.time starting from beg.of.run(hour)
  time[2]=(etime[0]-etime0)/86400;//ev.time starting from beg.of.run(day)
//
  if(ntuple->nLevel1()){
    _filled[0]->Fill(1,1.);//lvl1_ok
    ecflg=ntuple->pLevel1(0)->EcalFlag;
    etflg=ecflg/10;//ECEtot(multiplicity) flag
    ewflg=ecflg%10;//ECAngle(width) flag
//
    tofflg1=ntuple->pLevel1(0)->TofFlag1;//for z>=1(ftc)
    tofflg2=ntuple->pLevel1(0)->TofFlag2;//for z>=2(bz)
    membpat=ntuple->pLevel1(0)->JMembPatt;
    physbpat=ntuple->pLevel1(0)->PhysBPatt;
    ftc=((membpat&1)>0);//Z>=1
    ftz=((membpat&1<<5)>0);//SlowZ>=2
    fte=((membpat&1<<6)>0);//ec-FT
    bz= ((membpat&1<<9)>0);//Z>=2(when ftc)
    glft=(ftc||ftz||fte);//globFT
//---
    if(glft){//<-- glFT ok
      _filled[0]->Fill(2,1.);//globFT_ok
      if(ftc)_filled[0]->Fill(3,1.);//ftc_ok(z>=1)
      if(ftz)_filled[0]->Fill(4,1.);//ftz_ok(slowz>=2)
      if(fte)_filled[0]->Fill(5,1.);//fte_ok(ecal)
      if(bz)_filled[0]->Fill(6,1.);//bz_ok(z>=2)
      if(ftz && !ftc)_filled[0]->Fill(7,1.);//"true,very_slowZ>=2)"
      if((ftc||ftz) && fte)_filled[0]->Fill(8,1.);//"tof+ec"
      if(fte && !(ftc||ftz))_filled[0]->Fill(9,1.);//"ec alone"
    }
//---
    for(int i=0;i<16;i++){//<-- basis trig-members 
      lsbt=1<<i;
      if(membpat&lsbt)_filled[1]->Fill(i,1.);
    }
//---
    for(int i=0;i<8;i++){//<--- lvl1 phys.branches
      lsbt=1<<i;
      if((physbpat&lsbt)>0)_filled[2]->Fill(i,1.);
    }
//---
//                                       <------ TOF
    ntofl=0;
    mlcode1=tofflg1%10;//miss layers code(ftc)
    if(mlcode1>=0){
      if(mlcode1==0)ntofl=4;
      else if(mlcode1<5)ntofl=3;
      else ntofl=2;
      _filled[3]->Fill(ntofl,1.);//tot. tof-layers(z>=1)
    }
    ntofl=0;
    mlcode2=tofflg2%10;//miss layers code(bz)
    if(mlcode2>=0 && bz){//tofflg2 shows input patt., so it can be >=0 even when bz=0
      if(mlcode2==0)ntofl=4;
      else if(mlcode2<5)ntofl=3;
      else ntofl=2;
      _filled[4]->Fill(ntofl,1.);//....(z>=2)
    }
//
    npds1=0;
    npds2=0;
    for(int i=0;i<lmax;i++)tofpat1[i]=ntuple->pLevel1(0)->TofPatt1[i];
    for(int i=0;i<lmax;i++)tofpat2[i]=ntuple->pLevel1(0)->TofPatt2[i];
    for(int il=0;il<lmax;il++){
      for(int ip=0;ip<padpl[il];ip++){
        if((tofpat1[il]&1<<ip)>0 || (tofpat1[il]&1<<(16+ip))>0)npds1+=1;  
        if((tofpat2[il]&1<<ip)>0 || (tofpat2[il]&1<<(16+ip))>0)npds2+=1;
      }
    }
    if(ftc)_filled[5]->Fill(npds1,1.);//tot. tof-paddles(ORed sides)
    if(bz)_filled[6]->Fill(npds2,1.);
//
    if(ftc)_filled[7]->Fill(mlcode1,1.);//miss.layer code
    if(bz)_filled[10]->Fill(mlcode2,1.);
//
    int nch;
    for(int il=0;il<lmax;il++){
      for(int ip=0;ip<padpl[il];ip++){
        nch=il*10+ip+1;
        if((tofpat1[il]&1<<ip)>0)_filled[8]->Fill(nch,1.);//s1-patt, z>=1
	if((tofpat1[il]&1<<(16+ip))>0)_filled[9]->Fill(nch,1.);//s2-patt, z>=1
	if(bz){  
          if((tofpat2[il]&1<<ip)>0)_filled[11]->Fill(nch,1.);//s1-patt, Z>=2
	  if((tofpat2[il]&1<<(16+ip))>0)_filled[12]->Fill(nch,1.);//s2-patt, Z>=2
	}  
      }
    }
//----                                       
//                                       <------ ANTI
    int asmax(8),nast(0);
    int antipat=ntuple->pLevel1(0)->AntiPatt;
    for(int is=0;is<asmax;is++){
      if((antipat & 1<<is)>0)nast+=1;
      if((antipat & 1<<is)>0)_filled[13]->Fill(is+1,1.);//sectors-patt(globFT)
    }
    if(ftc)_filled[14]->Fill(nast,1.);//tot sectors, when Z>=1(ftc)
    if(bz)_filled[15]->Fill(nast,1.);//             ...., when Z>=2(bz)
    if(ftz)_filled[16]->Fill(nast,1.);//             ...., when slowZ>=2
    if(fte && (ftc||ftz))_filled[17]->Fill(nast,1.);// ...., when EC & TOF
    if(fte && !(ftc||ftz))_filled[18]->Fill(nast,1.);// ...., when EC & !TOF("gammas")
//----    
//                                       <------ ECAL
    for(int i=0;i<6;i++){
      for(int j=0;j<3;j++){
        ecpatt[i][j]=ntuple->pLevel1(0)->EcalPatt[i][j];
      }
    }
    if(fte){
      if(!(ftc||ftz)){//EC-drived FT
        _filled[19]->Fill(etflg,1.);//Etot-flag
        _filled[20]->Fill(ewflg,1.);//ShWidth-flag
      }
      else{//TOF-driven FT
        _filled[22]->Fill(etflg,1.);//Etot-flag
        _filled[23]->Fill(ewflg,1.);//ShWidth-flag
      }
    }
    if(glft)_filled[21]->Fill(etflg,1.);//Etot-flag
    if(fte){
      int word,bit,sset;
      for(int sl=0;sl<6;sl+=2){
        for(int pm=0;pm<36;pm++){
          word=pm/16;
          bit=pm%16;
          sset=sl/2;
          if((ecpatt[sl][word]&(1<<bit))>0)_filled[24]->Fill(float(pm+1+40*sset),1.);
        }
      }
      for(int sl=1;sl<6;sl+=2){
        for(int pm=0;pm<36;pm++){
          word=pm/16;
          bit=pm%16;
          sset=(sl-1)/2;
          if((ecpatt[sl][word]&(1<<bit))>0)_filled[25]->Fill(float(pm+1+40*sset),1.);
        }
      }
    }
//----
//                                       <------ LiveTime, Rates:
    ltime=ntuple->pLevel1(0)->LiveTime;
    for(int i=0;i<6;i++)rates[i]=ntuple->pLevel1(0)->TrigRates[i];
//    if(evnloc%100==0){
//      cout<<"Event/date="<<evnum<<" "<<date<<endl;
//      cout<<"time="<<time<<" ltime="<<ltime<<endl;
//    }
//
    if((time[0]-timez[0])>=lev1trange[0]){
      ((TProfile*)_filled[26])->Reset("");
      ((TProfile*)_filled[29])->Reset("");
      ((TProfile*)_filled[30])->Reset("");
      ((TProfile*)_filled[31])->Reset("");
      timez[0]=time[0];
      Lvl1Pars::setdat1(ntuple->GetTime());
    }
    ((TProfile*)_filled[26])->Fill(time[0]-timez[0],ltime,1.);
    ((TProfile*)_filled[29])->Fill(time[0]-timez[0],rates[2],1.);//Lev1
    ((TProfile*)_filled[30])->Fill(time[0]-timez[0],rates[3],1.);//TofPlaneMX
    ((TProfile*)_filled[31])->Fill(time[0]-timez[0],rates[4],1.);//ECFTProjMX
//
    if((time[1]-timez[1])>=lev1trange[1]){
      ((TProfile*)_filled[27])->Reset("");
      timez[1]=time[1];
      Lvl1Pars::setdat2(ntuple->GetTime());
    }
    ((TProfile*)_filled[27])->Fill(time[1]-timez[1],ltime,1.);
//
    if((time[2]-timez[2])>=lev1trange[2]){
      ((TProfile*)_filled[28])->Reset("");
      timez[2]=time[2];
      Lvl1Pars::setdat3(ntuple->GetTime());
    }
    ((TProfile*)_filled[28])->Fill(time[2]-timez[2],ltime,1.);
//
  }
  evnloc+=1; 
}


