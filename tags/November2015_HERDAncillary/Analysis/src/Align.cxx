#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TROOT.h"

/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

#include <vector>

using namespace std;

int main(int argc, char* argv[]) {

  if (argc<1) {
    printf("No ROOT file passed...\n");
    return 1;
  }

  TChain *chain = new TChain("t4");
     
  for (int ii=1; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }

  TString filename = argv[1];

  if (filename=="rootfiles/run_1416121279_ANC_374_S0.root") Event::ReadAlignment("alignment_S0.dat");
  else if (filename=="rootfiles/run_1416121279_ANC_374_S1.root") Event::ReadAlignment("alignment_S1.dat");
  else Event::ReadAlignment("alignment.dat");
  
  Event *ev;
  Cluster *cl;
  RHClass *rh;
  
  Long64_t entries = chain->GetEntries();
  printf("This run has %lld entries\n", entries);
  
  ev = new Event();
  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  int _maxtdr = NJINF*NTDRS;
  
  //  chain->GetTree()->GetUserInfo()->Print();
  rh = (RHClass*)chain->GetTree()->GetUserInfo()->At(0);
  //  printf("%p\n", rh);
  if (rh) {
    rh->Print();
    _maxtdr = rh->ntdrCmp;
  }
  //  printf("%d\n", _maxladd);
  
  TFile* foutput = new TFile("foutput.root", "RECREATE");
  foutput->cd();
  
  TH1F* occupancy[NJINF*NTDRS];
  TH1F* occupancy_posS[NJINF*NTDRS];
  TH1F* occupancy_posK[NJINF*NTDRS];
  TH1F* residual_posS[NJINF*NTDRS];
  TH1F* residual_posK[NJINF*NTDRS];
  for (int tt=0; tt<_maxtdr; tt++) {
    occupancy[tt] = new TH1F(Form("occupancy_%02d", tt), Form("occupancy_%02d", tt), 1024, 0, 1024);
    occupancy_posS[tt] = new TH1F(Form("occupancy_posS_%02d", tt), Form("occupancy_posS_%02d", tt), 2*640, -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));
    occupancy_posK[tt] = new TH1F(Form("occupancy_posK_%02d", tt), Form("occupancy_posK_%02d", tt), 2*384, -384*Cluster::GetPitch(1), 384*Cluster::GetPitch(1));
    residual_posS[tt] = new TH1F(Form("residual_posS_%02d", tt), Form("residual_posS_%02d", tt), 2*640, -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));
    residual_posK[tt] = new TH1F(Form("residual_posK_%02d", tt), Form("residual_posK_%02d", tt), 2*384, -384*Cluster::GetPitch(1), 384*Cluster::GetPitch(1));
  }
  
  for (int index_event=7; index_event<8; index_event++) {
    //  for (int index_event=0; index_event<entries; index_event++) {
    printf("----- new event %d\n", index_event);
    chain->GetEntry(index_event);
    
    int NClusTot = ev->NClusTot;

    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];

    ev->FindTrackAndFit(3, 3, false);
    printf("%f %f %f %f %f\n", ev->GetChiBestTrack(), ev->GetThetaBestTrack(), ev->GetPhiBestTrack(), ev->GetX0BestTrack(), ev->GetY0BestTrack());
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {

      cl = ev->GetCluster(index_cluster);
      int ladder = cl->ladder;

      printf("%d --> %d\n", ladder, rh->tdrCmpMap[ladder]);
      printf("%d --> %d\n", ladder, rh->FindPos(ladder));
      occupancy[rh->FindPos(ladder)]->Fill(cl->GetCoG());

      int side=cl->side;
      if (side==0) {
	occupancy_posS[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	residual_posS[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateBestTrack(cl->GetZPosition(), 0));
	v_cog_laddS[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
      else {
	occupancy_posK[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	residual_posK[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateBestTrack(cl->GetZPosition(), 1));
	v_cog_laddK[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
    }

  }

  foutput->Write();
  foutput->Close();
  
  return 0;
}
