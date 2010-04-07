//  $Id: AMSAxAMSHist.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <iostream>
#include "AMSNtuple.h"
#include "AMSDisplay.h"
#include <math.h>
#include <TPaveText.h>
#include <TProfile.h>
#include <TF1.h>

#include "AMSAxAMSHist.h"





void AMSAxAMSHist::Book(){

// Sets
int color=18;

AddSet("Set 0");

_filled.push_back(new TH1F("AxAMS_0_00","Momentum",200,-200.,200.));
_filled[_filled.size()-1]->SetXTitle("Momentum (GeV)");
_filled[_filled.size()-1]->SetFillColor(color++);
_filled.push_back(new TH1F("AxAMS_0_01","Abs(Momentum)",100,0.,200.));
_filled[_filled.size()-1]->SetXTitle("Abs(Momentum) (GeV)");
_filled[_filled.size()-1]->SetFillColor(color++);
_filled.push_back(new TH1F("AxAMS_0_02","Charge",20,-0.5,19.5));
_filled[_filled.size()-1]->SetXTitle("Charge");
_filled[_filled.size()-1]->SetFillColor(color++);
_filled.push_back(new TH1F("AxAMS_0_03","Velocity",240,-1.2,1.2));
_filled[_filled.size()-1]->SetXTitle("#beta");
_filled[_filled.size()-1]->SetFillColor(color++);

AddSet("Set 1");


_filled.push_back(new TH1F("AxAMS_1_04","Mass",200,0.01,10.01));
_filled[_filled.size()-1]->SetXTitle("Mass");
_filled[_filled.size()-1]->SetFillColor(color++);
_filled.push_back(new TH2F("AxAMS_1_05","Momentum vs Velocity",100,-5.,5.,
100,0.6,1.1));
_filled[_filled.size()-1]->SetXTitle("Log(Momentum) Log(GeV)");
_filled[_filled.size()-1]->SetYTitle("Velocity");

_filled.push_back(new TProfile("AxAMS_1_06","Tracker `D#?p!/p vs p",100,0.,20.,0.,1000.));
_filled[_filled.size()-1]->SetXTitle("Momentum (GeV)");
_filled[_filled.size()-1]->SetYTitle("#sigma #frac{#Delta p}{p} (%)");
_filled[_filled.size()-1]->SetFillColor(color++);

AddSet("BeamProfile");


_filled.push_back(new TH1F("AxAMS_2_07","XCoo",400,-40.,40.));
_filled[_filled.size()-1]->SetXTitle("X Coo  (cm) at Z=72 cm");
_filled[_filled.size()-1]->SetFillColor(color++);
_filled.push_back(new TH1F("AxAMS_2_08","YCoo",400,-40.,40.));
_filled[_filled.size()-1]->SetXTitle("Y Coo  (cm) at Z=72 cm");
_filled[_filled.size()-1]->SetFillColor(color++);
_filled.push_back(new TH1F("AxAMS_2_09","Theta",360,0.,180.));
_filled[_filled.size()-1]->SetXTitle("#Theta (deg)");
_filled[_filled.size()-1]->SetFillColor(color++);
_filled.push_back(new TH1F("AxAMS_2_10","Phi",360,0.,360.));
_filled[_filled.size()-1]->SetXTitle("#Phi (deg)");
_filled[_filled.size()-1]->SetFillColor(color++);


AddSet("ReconstructionEfficiency");

_filled.push_back(new TH1F("AxAMS_3_11","NPart",10,-0.5,9.5));
_filled[_filled.size()-1]->SetXTitle("Number of Rec Particles");
_filled[_filled.size()-1]->SetFillColor(color++);



AddSet("MC");

_filled.push_back(new TH1F("AxAMS_4_12","Measured Momentum / Generated Momentum",200,0.001,4.));
_filled[_filled.size()-1]->SetXTitle("#frac{Momentum_{Measured}}{Momentum_{Generated}}");
_filled[_filled.size()-1]->SetFillColor(color++);
_filled.push_back(new TH1F("AxAMS_4_13","Measured Velocity -Generated Velocity",200,-1.,1.));
_filled[_filled.size()-1]->SetXTitle("#beta_{Measured}-#beta_{Generated}");
_filled[_filled.size()-1]->SetFillColor(color++);
_filled.push_back(new TH2F("AxAMS_4_14","Measured Charge vs Generated Charge",21,-1.5,19.5,21,-1.5,19.5));
_filled[_filled.size()-1]->SetXTitle("Q_{Measured}");
_filled[_filled.size()-1]->SetYTitle("Q_{Generated}");
_filled[_filled.size()-1]->SetFillColor(color++);

}





void AMSAxAMSHist::ShowSet(Int_t Set){
   TF1 f1("myg","[0]*exp(-(x-[1])/[2]*(x-[1])/[2]/2)",0,2);
   f1.SetParameter(1,1.);
   f1.SetParameter(2,0.05);
   _filled[3]->Fit("myg","Q");
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
 if(i==3){
    char text[80];
    TPaveText *lf=new TPaveText(0.2,0.5,0.4,0.8,"NDC");
    lf->SetFillColor(18);
    sprintf(text,"Mean %f",f1.GetParameter(1));    
    lf->AddText(text);
    sprintf(text,"Sigma %f",f1.GetParameter(2));    
    lf->AddText(text);
    lf->Draw();
 }
gPadSave->cd();
}

break;
case 1:
{
gPad->Divide(1,2);
 gPad->cd(1);
TVirtualPad * gp1=gPad; 
 gPad->Divide(2,1);
 gPad->cd(1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled[4]->Draw();
 gp1->cd();
 gPad->cd(2);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled[6]->Draw();
 gPadSave->cd();
 gPad->cd(2);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 _filled[5]->Draw();
break;
}
case 2:
gPad->Divide(2,2);
for(i=0;i<4;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled[i+7]->Draw();
gPadSave->cd();
}
break;
case 3:
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled[11]->Draw();
break;
case 4:
gPad->Divide(2,1);
 gPad->cd(1);
TVirtualPad * gp1=gPad; 
 gPad->Divide(1,2);
 gPad->cd(1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled[12]->Draw();
 gp1->cd();
 gPad->cd(2);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled[13]->Draw();
 gPadSave->cd();
 gPad->cd(2);
// gPad->SetLogx(gAMSDisplay->IsLogX());
// gPad->SetLogy(gAMSDisplay->IsLogY());
 _filled[14]->Draw("COL");
break;

}


}

void AMSAxAMSHist::Fill(AMSNtupleR * ntuple){
    _filled[11]->Fill(ntuple->nParticle(),1.);
  if(ntuple->nParticle() && ntuple->pParticle(0)){
    _filled[0]->Fill(ntuple->pParticle(0)->Momentum,1.);
    _filled[1]->Fill(fabs(ntuple->pParticle(0)->Momentum),1.);
    _filled[2]->Fill(ntuple->pParticle(0)->Charge,1.);
    _filled[3]->Fill(ntuple->pParticle(0)->Beta,1.);
    _filled[4]->Fill(ntuple->pParticle(0)->Mass,1.);
    ((TH2F*)_filled[5])->Fill(log(fabs(ntuple->pParticle(0)->Momentum)),ntuple->pParticle(0)->Beta,1.);
    float err=ntuple->pParticle(0)->ErrMomentum;
    if(err<100000){
      err=err*100;
     if (fabs(ntuple->pParticle(0)->Momentum) !=0)err=err/fabs(ntuple->pParticle(0)->Momentum);
    } 
    ((TProfile*)_filled[6])->Fill(fabs(ntuple->pParticle(0)->Momentum),err,1.);
    _filled[7]->Fill(ntuple->pParticle(0)->TOFCoo[0][0],1.);
    _filled[8]->Fill(ntuple->pParticle(0)->TOFCoo[0][1],1.);
    _filled[9]->Fill(ntuple->pParticle(0)->Theta*180./3.1415926,1.);
    _filled[10]->Fill(ntuple->pParticle(0)->Phi*180./3.1415926,1.);
     if(ntuple->nMCEventg()>0){		
        MCEventgR mc_ev=ntuple->MCEventg(0);
        double xm = log(mc_ev.Momentum);
        double beta=1/sqrt(1-(mc_ev.Mass)/(mc_ev.Momentum)*(mc_ev.Mass)/(mc_ev.Momentum));
         _filled[12]->Fill(mc_ev.Momentum/(fabs(ntuple->pParticle(0)->Momentum)+1e-20),1);
         _filled[13]->Fill(ntuple->pParticle(0)->Beta-beta,1);
        double charge=ntuple->pParticle(0)->Charge;
        if(ntuple->pParticle(0)->Momentum<0)charge=-charge;
        ((TH2F*)_filled[14])->Fill(charge,mc_ev.Charge,1);
      }  
  }
}
