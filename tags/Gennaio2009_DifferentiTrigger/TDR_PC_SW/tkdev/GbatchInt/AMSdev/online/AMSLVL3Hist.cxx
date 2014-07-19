//  $Id: AMSLVL3Hist.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <iostream>
#include "AMSDisplay.h"
#include "AMSNtuple.h"
#include "AMSLVL3Hist.h"



void AMSLVL3Hist::Book(){

AddSet("LVL3 Distributions");
_filled.push_back(new TH1F("LVL3Output","LVL3 Output",17,-0.5,16.5));
_filled[_filled.size()-1]->SetXTitle("LVL3 Output");
_filled[_filled.size()-1]->SetFillColor(41);
_filled.push_back(new TH1F("LVL3Hits","LVL3 Hits",60,-0.5,59.5));
_filled[_filled.size()-1]->SetXTitle("Number Of Tracker Hits");
_filled[_filled.size()-1]->SetFillColor(43);
_filled.push_back(new TH1F("LVL3Residual","LVL3 Residual",50,-2.,2.));
_filled[_filled.size()-1]->SetXTitle("Average Residual (mm)");
_filled[_filled.size()-1]->SetFillColor(42);
_filled.push_back(new TH1F("LVL3ELoss","LVL3 ELoss",500,0.,5000.));
_filled[_filled.size()-1]->SetXTitle("Tracker Energy Loss (ADC ch *8)");
_filled[_filled.size()-1]->SetFillColor(44);

AddSet("LVL3/AxAMS  Comparison");

_filled.push_back(new TH1F("LVL3Output1","LVL3 Output Positive Rigidity",17,-0.5,16.5));
_filled[_filled.size()-1]->SetXTitle("LVL3 Output");
_filled[_filled.size()-1]->SetFillColor(41);
_filled.push_back(new TH1F("LVL3Output2","LVL3 Output Negative Rigidity",17,-0.5,16.5));
_filled[_filled.size()-1]->SetXTitle("LVL3 Output");
_filled[_filled.size()-1]->SetFillColor(42);

}





void AMSLVL3Hist::ShowSet(Int_t Set){
gPad->Clear();
TVirtualPad * gPadSave = gPad;
int i;
switch(Set){
case 0:
gPad->Divide(2,2);
for(i=0;i<4;i++){
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
for(i=4;i<6;i++){
 gPad->cd(i+1-4);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled[i]->Draw();
gPadSave->cd();
}

}


}

void AMSLVL3Hist::Fill(AMSNtupleR * ntuple){
  if(ntuple->nLevel3() ){
    _filled[0]->Fill((ntuple->pLevel3(0)->MainTr),1.);
    if(ntuple->pLevel3(0)->TOFTr>0){
     _filled[1]->Fill(ntuple->pLevel3(0)->NTrHits,1.);
     if(ntuple->nParticle()>0 && ntuple->pParticle(0)->pTrTrack()){
      _filled[2]->Fill(ntuple->pLevel3(0)->Residual[0],1.);
      _filled[3]->Fill(ntuple->pLevel3(0)->ELoss,1.);
        } 
    }
    if(ntuple->nParticle()>0 && ntuple->pParticle(0)->pTrTrack()){
     if(ntuple->pParticle(0)->Momentum*ntuple->pParticle(0)->Beta > 0)_filled[4]->Fill((ntuple->pLevel3(0)->TrackerTr%32),1.);
     else _filled[5]->Fill(ntuple->pLevel3(0)->TrackerTr%32,1.);
    }
  }
}
