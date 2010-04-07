//  $Id: AMSRICHHist.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <iostream>
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "AMSRICHHist.h"




void AMSRICHHist::Book(){ 

AddSet("RICH Raw");       // About occupancy and so on

  _filled.push_back(new TH1F("RICHOccupancy","RICH Occupancy",16*680,0,16*680));
  _filled[_filled.size()-1]->SetXTitle("Rich hits");
  _filled[_filled.size()-1]->SetYTitle("Number of events");
  _filled[_filled.size()-1]->SetFillColor(41);



  _filled.push_back(new TH1F("RICHOccupancy2","RICH Occupancy2",16*680,0,16*680));
  _filled[_filled.size()-1]->SetXTitle("Rich soft Channel");
  _filled[_filled.size()-1]->SetYTitle("Number of hits");
  _filled[_filled.size()-1]->SetFillColor(41);


  AddSet("RICH Hits");      // About hits information (signals and so on)

  _filled.push_back(new TH1F("RICHSignal","RICH Signal",500,0,100));
  _filled[_filled.size()-1]->SetXTitle("Number of photoelectrons");
  _filled[_filled.size()-1]->SetFillColor(41);


  _filled.push_back(new TH1F("RICHADC1","RICH ADC gainx1",1024,0,4096));
  _filled[_filled.size()-1]->SetXTitle("ADC_{#times 1} count");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TH1F("RICHADC5","RICH ADC gainx5",1024,0,4096));
  _filled[_filled.size()-1]->SetXTitle("ADC_{#times 5} count");
  _filled[_filled.size()-1]->SetFillColor(41);


  AddSet("RICH Rings");     // About reconstructed rings
  
  _filled.push_back(new TH1F("RICHRings","RICH Rings",30,0,30));
  _filled[_filled.size()-1]->SetXTitle("RICH rings");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TH1F("RICHRingProb","RICH Ring probability",50,0.,1.));
  _filled[_filled.size()-1]->SetXTitle("Ring probability");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TH1F("RICHBeta","RICH Beta ",50,0.97,1.01));
  _filled[_filled.size()-1]->SetXTitle("#beta_{RICH}(prob>0.01)");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TH1F("RICHCharge","RICH Charge ",100,0.,10.));
  _filled[_filled.size()-1]->SetXTitle("#Z_{RICH}(prob>0.01)");
  _filled[_filled.size()-1]->SetFillColor(41);


  //  AddSet("RICH/TRACKER"); // Mass reconstruction

  //  _filled.push_back(new TH1F("RICHMass","RICH Mass ",100,0.,10.));
  //  _filled[_filled.size()-1]->SetXTitle("Mass (prob>0.01)");
  //  _filled[_filled.size()-1]->SetFillColor(41);

}


void AMSRICHHist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave=gPad;
  switch(Set){
  case 0:
    gPad->Divide(1,2);
    for(int i=0;i<2;i++){
      gPad->cd(i+1);
      gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(gAMSDisplay->IsLogY());
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      _filled[i]->Draw();
      gPadSave->cd();
    }
    break;
  case 1:
    gPad->Divide(1,2);
    gPad->cd(2);
    gPad->Divide(2,1);
    gPadSave->cd();
    gPad->cd(1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[2]->Draw();
    gPadSave->cd();
    for(int i=0;i<2;i++){
      gPad->cd(2);
      gPad->cd(i+1);
      gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(gAMSDisplay->IsLogY());
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      _filled[i+3]->Draw();
      gPadSave->cd();
    }
    break;
  case 2:
    gPad->Divide(2,2);
    for(int i=0;i<4;i++){
      gPad->cd(i+1);
      gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(gAMSDisplay->IsLogY());
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      _filled[i+5]->Draw();
      gPadSave->cd();
    }
    break;
    //  case 3:
    //    for(int i=0;i<1;i++){
    //      gPad->cd(i+1);
    //      gPad->SetLogx(gAMSDisplay->IsLogX());
    //      gPad->SetLogy(gAMSDisplay->IsLogY());
    //      gPad->SetLogz(gAMSDisplay->IsLogZ());
    //      _filled[i+9]->Draw();
    //      gPadSave->cd();
    //  }
    //    break;
  }
}



void AMSRICHHist::Fill(AMSNtupleR *ntuple){ 
  if(ntuple->nLevel3()){
    _filled[0]->Fill(ntuple->nRichHit(),1.);
    for(int i=0;i<ntuple->nRichHit();i++){
      RichHitR current=ntuple->RichHit(i);
      _filled[1]->Fill(current.Channel,1.);

      _filled[2]->Fill(current.Npe,1.);
      if(! (current.Status&(1<<30)))
	_filled[3]->Fill(current.Counts,1.);
      else
	_filled[4]->Fill(current.Counts,1.);
    }
    _filled[5]->Fill(ntuple->nRichRing(),1.);
    if(ntuple->nParticle()==1){
      ParticleR current=ntuple->Particle(0);
      if(current.pRichRing()){
      	_filled[6]->Fill(current.pRichRing()->Prob,1.);
	if(current.pRichRing()->Prob>1e-2){
	  _filled[7]->Fill(current.pRichRing()->BetaRefit,1.);
	  if(current.pRichRing()->NpExp>0)
	    _filled[8]->Fill(sqrt((current.pRichRing()->NpCol)/
				  (current.pRichRing()->NpExp)),1.);
      	}
      }
    
    }
    
  }
}







