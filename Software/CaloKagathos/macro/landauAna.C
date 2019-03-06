
#include <vector>;
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include <iostream.h>

#define scaleOverMPV 1
//#define scaleOverMPV 0.063; // for 400 micron Silicon
//#define scaleOverMPV 0.064; // for 300 micron Silicon

Double_t clangaufun(Double_t *x, Double_t *par) {

  // by Helge!

//     Fit parameters:
//     par[0]=Width (scale) parameter of Landau density
//     par[1]=Most Probable (MP, location) parameter of Landau density
//     par[2]=Total area (integral -inf to inf, normalization constant)
//     par[3]=Width (sigma) of convoluted Gaussian function
//
//     In the Landau distribution (represented by the CERLIB approximation),
//     the maximum is located at x=-0.22278298 with the location parameter=0.
//     This shift is corrected within this function, so that the actual
//     maximum is identical to the MP parameter.

  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  Double_t mpshift  = -0.22278298;       // Landau maximum location

  // Control constants
  Double_t np = 200.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;

  Double_t fitfun;

  Double_t scale = scaleOverMPV*par[0];


  // MP shift correction
  mpc = par[0] - mpshift * scale;

  // Range of convolution integral
  xlow = x[0] - sc * par[2];
  xupp = x[0] + sc * par[2];

  step = (xupp-xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for(i=1.0; i<=np/2; i++)

    {
      xx = xlow + (i-.5) * step;
      fland = TMath::Landau(xx,mpc, scale) / scale;
      sum += fland * TMath::Gaus(x[0],xx,par[2]);

      xx = xupp - (i-.5) * step;
      fland = TMath::Landau(xx,mpc,scale) / scale;
      sum += fland * TMath::Gaus(x[0],xx,par[2]);
    }

  fitfun = (par[1] * step * sum * invsq2pi / par[2]);

// now just add another gaussion for the noise peak centered at zero
  // fitfun = fitfun + par[4]*exp(-x[0]*x[0]/par[5]/par[5]);

  return fitfun;

}

void landauAna (const char* filename, int row, int col, int layer){

  TFile* aFile = new TFile(filename);
  TTree * tree = (TTree*) aFile->Get("data");
  aFile->ls();
  TString plotname = Form("RoW%dCol%dLayer%d", row, col, layer);
  TH1F* aHisto = new TH1F(plotname, plotname, 100, 0, 1000);
  TString selection = Form("hitRow==%d && hitCol==%d && hitLayer==%d", row, col, layer);
  tree->Draw(TString("hitSignal>>")+plotname, selection);
 
  TF1* langau = new TF1("langau",clangaufun, 0., 10000., 3);
                            
  langau->SetParName(0,"MPV");
  langau->SetParName(1,"integral");
  langau->SetParName(2,"sigma of gauss");

  langau->SetParameter(0,400);
  langau->SetParameter(1,aHisto->GetEntries());
  langau->SetParameter(2,1);

  //  langau->SetParLimits(2,1.,10.0); 
  langau->SetParLimits(0,300.,500.); 
  
  aHisto->Fit("langau","" ,"",0.,300.);
  aHisto->DrawCopy("HISTO");
  langau->Draw("LSAME");

}


Double_t clangaufun(Double_t *x, Double_t *par) {

  // by Helge!

//     Fit parameters:
//     par[0]=Width (scale) parameter of Landau density
//     par[1]=Most Probable (MP, location) parameter of Landau density
//     par[2]=Total area (integral -inf to inf, normalization constant)
//     par[3]=Width (sigma) of convoluted Gaussian function
//
//     In the Landau distribution (represented by the CERLIB approximation),
//     the maximum is located at x=-0.22278298 with the location parameter=0.
//     This shift is corrected within this function, so that the actual
//     maximum is identical to the MP parameter.

  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  Double_t mpshift  = -0.22278298;       // Landau maximum location

  // Control constants
  Double_t np = 200.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;

  Double_t fitfun;

  Double_t scale = scaleOverMPV*par[0];


  // MP shift correction
  mpc = par[0] - mpshift * scale;

  // Range of convolution integral
  xlow = x[0] - sc * par[2];
  xupp = x[0] + sc * par[2];

  step = (xupp-xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for(i=1.0; i<=np/2; i++)

    {
      xx = xlow + (i-.5) * step;
      fland = TMath::Landau(xx,mpc, scale) / scale;
      sum += fland * TMath::Gaus(x[0],xx,par[2]);

      xx = xupp - (i-.5) * step;
      fland = TMath::Landau(xx,mpc,scale) / scale;
      sum += fland * TMath::Gaus(x[0],xx,par[2]);
    }

  fitfun = (par[1] * step * sum * invsq2pi / par[2]);

// now just add another gaussion for the noise peak centered at zero
  // fitfun = fitfun + par[4]*exp(-x[0]*x[0]/par[5]/par[5]);

  return fitfun;

}
