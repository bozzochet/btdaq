//  $Id: AMSAntiHist.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
// By V. Choutko & D. Casadei
// Last changes: 27 Feb 1998 by D.C.
#include <iostream>
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "AMSAntiHist.h"

char AntiPars::dat1[30];
char AntiPars::dat2[30];
char AntiPars::dat3[30];
Int_t AntiPars::evcount[20];


void AMSAntiHist::Book(){

  AddSet("Anti-Visibility");
  
  _filled.push_back(new TH1F("antih0","NAntiSectors/event(when found)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total fired sectors");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih1","NAntiSectors/event(FTcoincEvent)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total fired sectors");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  _filled.push_back(new TH1F("antih2","AntiSectorsVisibility",30,0.,30.));
  _filled[_filled.size()-1]->SetXTitle("AnySideConfig                   S2-miss                                 S1-miss                                 ");
  _filled[_filled.size()-1]->SetFillColor(3);
  _filled[_filled.size()-1]->SetTitleSize(0.05,"X");

  _filled.push_back(new TH1F("antih3","AntiSectorsVisibility(FTcoincEvent)",30,0.,30.));
  _filled[_filled.size()-1]->SetXTitle("AnySideConfig                   S2-miss                                 S1-miss                                 ");
  _filled[_filled.size()-1]->SetFillColor(3);
  _filled[_filled.size()-1]->SetTitleSize(0.05,"X");

  _filled.push_back(new TH1F("antih4","AntiSectorsVisibility(FTcoinc+PairedEvents)",30,0.,30.));
  _filled[_filled.size()-1]->SetXTitle("AnySideConfig                   S2-miss                                 S1-miss                                 ");
  _filled[_filled.size()-1]->SetFillColor(3);
  _filled[_filled.size()-1]->SetTitleSize(0.05,"X");
  

  AddSet("Anti-TimeHitsMultipl");
  
  _filled.push_back(new TH1F("antih5","Sectors 1-4 TimeHitsMult(FTcoincEvents)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Sect1                 Sect2                   Sect3                  Sect4                       ");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih6","Sectors 5-8 TimeHitsMult(FTcoincEvents)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Sect5                 Sect6                   Sect7                  Sect8                       ");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih7","Sectors 1-4 TimeHitsMult(FTcoinc+PairedEvents)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Sect1                 Sect2                   Sect3                  Sect4                       ");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih8","Sectors 5-8 TimeHitsMult(FTcoinc+PairedEvents)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Sect5                 Sect6                   Sect7                  Sect8                       ");
  _filled[_filled.size()-1]->SetFillColor(3);
  

  AddSet("Anti-Edep");
  
  _filled.push_back(new TH1F("antih9","Sector-1 Edep(FTcoincEvents,1paired t-hit)",100,0.,25.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih10","Sector-2 Edep(FTcoincEvents,1paired t-hit)",100,0.,25.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih11","Sector-3 Edep(FTcoincEvents,1paired t-hit)",100,0.,25.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih12","Sector-4 Edep(FTcoincEvents,1paired t-hit)",100,0.,25.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih13","Sector-5 Edep(FTcoincEvents,1paired t-hit)",100,0.,25.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih14","Sector-6 Edep(FTcoincEvents,1paired t-hit)",100,0.,25.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih15","Sector-7 Edep(FTcoincEvents,1paired t-hit)",100,0.,25.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih16","Sector-8 Edep(FTcoincEvents,1paired t-hit)",100,0.,25.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  
  AddSet("Anti-Events Rates");
  
  _filled.push_back(new TH1F("antih17","Anti-events rates",6,0.,6.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("antih18","Anti-event FT-correlations",12,0.,12.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
  
}



void AMSAntiHist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave = gPad;
  int i;
  TAxis *xax;
  char name[60],dat[30];
//

  switch(Set){
case 0:
  gPad->Divide(2,3);
  for(i=0;i<5;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    if(i>1)_filled[i]->SetStats(kFALSE);
    _filled[i]->Draw();//Anti-visibility
    gPadSave->cd();
  }
  break;
case 1:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+5]->SetStats(kFALSE);
    _filled[i+5]->Draw();//Anti-TimeHitsMult
    gPadSave->cd();
  }
  break;
case 2:
  gPad->Divide(4,2);
  for(i=0;i<8;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+9]->Draw();//Anti-Edep
    gPadSave->cd();
  }
  break;
case 3:
  gPad->Divide(1,2);
  gPad->cd(1);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[17]->GetXaxis();
  xax->SetLabelSize(0.04);
  xax->SetBinLabel(1,"Entries");
  _filled[17]->SetBinContent(1,AntiPars::getstat(0));
  xax->SetBinLabel(2,"Lvl1OK");
  _filled[17]->SetBinContent(2,AntiPars::getstat(1));
  xax->SetBinLabel(3,"+AntiEvent");
  _filled[17]->SetBinContent(3,AntiPars::getstat(2));
  xax->SetBinLabel(4,"+FTcoinc");
  _filled[17]->SetBinContent(4,AntiPars::getstat(3));
  xax->SetBinLabel(5,"+Paired");
  _filled[17]->SetBinContent(5,AntiPars::getstat(4));
  xax->SetBinLabel(6," ");
  _filled[17]->SetBinContent(6,0);
  
  _filled[17]->SetStats(kFALSE);
  _filled[17]->Draw("hbar2");//Anti-events Rates  
  gPadSave->cd();
  
  
  gPad->cd(2);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[18]->GetXaxis();
  xax->SetLabelSize(0.04);
  xax->SetBinLabel(1,"GlobFTOK");
  _filled[18]->SetBinContent(1,AntiPars::getstat(6));
  xax->SetBinLabel(2,"ChargedEM");
  _filled[18]->SetBinContent(2,AntiPars::getstat(7));
  xax->SetBinLabel(3,"+ AntiCoinc");
  _filled[18]->SetBinContent(3,AntiPars::getstat(8));
  xax->SetBinLabel(4,"CharNotEM");
  _filled[18]->SetBinContent(4,AntiPars::getstat(9));
  xax->SetBinLabel(5,"+ AntiCoinc");
  _filled[18]->SetBinContent(5,AntiPars::getstat(10));
  xax->SetBinLabel(6,"NeutrEM");
  _filled[18]->SetBinContent(6,AntiPars::getstat(11));
  xax->SetBinLabel(7,"+ AntiCoinc");
  _filled[18]->SetBinContent(7,AntiPars::getstat(12));
  xax->SetBinLabel(8,"Z>=2");
  _filled[18]->SetBinContent(8,AntiPars::getstat(13));
  xax->SetBinLabel(9,"+ AntiCoinc");
  _filled[18]->SetBinContent(9,AntiPars::getstat(14));
  xax->SetBinLabel(10,"SlowZ>=2");
  _filled[18]->SetBinContent(10,AntiPars::getstat(15));
  xax->SetBinLabel(11,"+ AntiCoinc");
  _filled[18]->SetBinContent(11,AntiPars::getstat(16));
  
  xax->SetBinLabel(12," ");
  _filled[18]->SetBinContent(12,0);
  
  _filled[18]->SetStats(kFALSE);
  _filled[18]->Draw("hbar2");//Anti-event Correlations  
  gPadSave->cd();
//  
  }
}



void AMSAntiHist::Fill(AMSNtupleR *ntuple){
// 
  AntiPars::addstat(0);//<--- Entries
//
  Level1R *p2lev1(0);
  Bool_t LVL1OK(0);
  Int_t ECTrigFl(0);
  Int_t TOFTrigFl1(0),TOFTrigFl2(0);
  bool ftc(0),ftz(0),bz(0),fte(0),glft(0);
  int physbpat(0),membpat(0);
//
  if(ntuple->nLevel1()>0){
    p2lev1=ntuple->pLevel1(0);
    LVL1OK=1;
    TOFTrigFl1=p2lev1->TofFlag1;
    TOFTrigFl2=p2lev1->TofFlag2;
    ECTrigFl=p2lev1->EcalFlag;
    membpat=ntuple->pLevel1(0)->JMembPatt;
    ftc=((membpat&1)>0);//Z>=1
    ftz=((membpat&1<<5)>0);//SlowZ>=2
    fte=((membpat&1<<6)>0);//ec-FT
    bz= ((membpat&1<<9)>0);//Z>=2(when ftc)
    glft=(ftc||ftz||fte);//globFT
  }
  else return;
  AntiPars::addstat(1);//<--- Lev1 OK
//
  Float_t eantit(0),eanti;
  Int_t nasects=ntuple->NAntiCluster();
  Int_t nparts=ntuple->nParticle();
  Int_t ntrktrs=ntuple->nTrTrack();
  Int_t necshow=ntuple->nEcalShower();
  Int_t nasftc(0),nasprd(0),nasftpr(0);
//
  if(nasects>0){//Anti-event
    AntiPars::addstat(2);//<--- Found any fired Anti
//
    AntiClusterR *p;
    Int_t sect,nthits,ntpairs;
    UInt_t status;
    Float_t htime[16];
    Bool_t ftcok(0),miside(0),s1ok(0),s2ok(0);
//
    for(int is=0;is<nasects;is++){//<-- loop over Clusters(fired sectors)
      p=ntuple->pAntiCluster(is);
      sect=p->Sector;//1-8
      status=p->Status;// Bit"128"->No FT-coinc. on 2 sides;"256"->1sideSector;"1024"->miss.side#
      ftcok=((status&1<<7)==0);//FTcoins ok (on any side)
      miside=((status&1<<8)==1);//1-sided sector
      if(miside){
        s1ok=((status&1<<10)==1);
        s2ok=((status&1<<10)==0);
      }
      if(ftcok)nasftc+=1;//count sectors with FTcoinc
      eanti=p->Edep;
      nthits=p->Ntimes;
      ntpairs=p->Npairs;
      if(ntpairs>0)nasprd+=1;//count sectors with t-paired sides
      if(ftcok && ntpairs>0)nasftpr+=1;//count sectors with FTcoins+paired 
      for(int i=0;i<nthits;i++)htime[i]=p->Times[i];
      eantit+=eanti;
    
      _filled[2]->Fill(sect,1.);
      if(s1ok)_filled[2]->Fill(sect+10,1.);
      if(s2ok)_filled[2]->Fill(sect+20,1.);
    
      if(ftcok){
        _filled[3]->Fill(sect,1.);
        if(s1ok)_filled[3]->Fill(sect+10,1.);
        if(s2ok)_filled[3]->Fill(sect+20,1.);
        if(sect<5)_filled[5]->Fill(nthits+(sect-1)*10,1.);//time-hits multipl.(FTcoinc)
        else _filled[6]->Fill(nthits+(sect-5)*10,1.);
      }
    
      if(ftcok && ntpairs>0){
        _filled[4]->Fill(sect,1.);
        if(s1ok)_filled[4]->Fill(sect+10,1.);
        if(s2ok)_filled[4]->Fill(sect+20,1.);
        if(sect<5)_filled[7]->Fill(nthits+(sect-1)*10,1.);//time-hits multipl.(FTcoinc+paired)
        else _filled[8]->Fill(nthits+(sect-5)*10,1.);
      }
    
      if(ftcok && ntpairs==1)_filled[9+sect-1]->Fill(eanti,1.);
    }//--->endof cluster loop
//
    _filled[0]->Fill(nasects,1.);
    _filled[1]->Fill(nasftc,1.);
    if(nasftc>0)AntiPars::addstat(3);//<--- event with at least 1 fired+FTcoinc sector
    if(nasftc>0 && nasprd>0)AntiPars::addstat(4);//<--- event with at least 1 fired+FTcoinc+Paired sector
  }//--->endof "anti-event" check
//
  if(glft){
    AntiPars::addstat(6);//globFT OK
    if(ftc && fte)AntiPars::addstat(7);//ChargEMTrig
    if(ftc && fte && nasftc>0)AntiPars::addstat(8);//ChargEMTrig+Anti(coinc)
    if(ftc && !fte)AntiPars::addstat(9);//ChargNotEMTrig
    if(ftc && !fte && nasftc>0)AntiPars::addstat(10);//ChargNotEMTrig+Anti(coinc)
    if(fte && !ftc)AntiPars::addstat(11);//NeutrEMTrig
    if(fte && !ftc && nasftc>0)AntiPars::addstat(12);//NeutrEMTrig+Anti(coinc)
    if(ftc && bz)AntiPars::addstat(13);//HiZTrig
    if(ftc && bz && nasftc>0)AntiPars::addstat(14);//HiZTrig+Anti(coinc)
    if(ftz && !bz)AntiPars::addstat(15);//SlowZTrig
    if(ftz && !bz && nasftc>0)AntiPars::addstat(16);//SlowZTrig+Anti(coinc)
  }
}


