#include "Event.hh"
#include "Cluster.hh"

#include "TrackSelection.hh"
#include "Utilities.hh"

#include "TTree.h"
#include "TFile.h"

#include "TH2D.h"
#include "TCanvas.h"

using namespace std;

int main(){ 
  TCanvas* c1=new TCanvas();
  TH2D* vert=new TH2D("","",80,-10,10,80,-10,10);

  TFile* mcf=new TFile("../Decode/RootData/run_000100.root","READ");
  TTree* mct=(TTree*)mcf->Get("t4");

  TString align_filename = "../Decode/alignment_mc_150.dat";
  TString gaincorrection_filename = "../Decode/gaincorrection_mc.dat";
  
  printf("---------------------------------------------\n");
  Event::ReadAlignment(align_filename.Data());
  Event::ReadGainCorrection(gaincorrection_filename.Data());
  printf("---------------------------------------------\n");
  
  int nEvs;
  Event* ev=new Event();
  //Cluster* cl;
  pair<double,double> vx,vy;

  mct->SetBranchAddress("cluster_branch",&ev);

  nEvs = mct->GetEntries();
  bool tracciato;

  for(int Ev=0; Ev<nEvs; Ev++){
    ev->Clear();
    vx=make_pair(9999.,9999.);
    vy=make_pair(9999.,9999.);
    tracciato=false;
    printf("+----------------------------+\n");
    printf("|+++ ANALISI EVENTO N° %d +++|\n",Ev);    
    printf("+----------------------------+\n");
    mct->GetEntry(Ev);
    printf("Via!!\n");
    tracciato=ev->FindTracksAndVertex();
    if(tracciato){
      printf("Quasi fatto!\n");
      vx=ev->GetVertexK();
      vy=ev->GetVertexS();
      vert->Fill(vx.second,vy.second);
    }
  }
  
  vert->Draw();
  c1->SaveAs("vert.png");

  delete ev;
  delete mct;
  delete mcf;
  delete vert;
  delete c1;

}
