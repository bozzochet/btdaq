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
#include "TF1.h"
#include "TMath.h"

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

  Event::ReadAlignment("alignment.dat");
  
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
    residual_posS[tt] = new TH1F(Form("residual_posS_%02d", tt), Form("residual_posS_%02d", tt), 2*640, -6.4*Cluster::GetPitch(0), 6.4*Cluster::GetPitch(0));
    residual_posK[tt] = new TH1F(Form("residual_posK_%02d", tt), Form("residual_posK_%02d", tt), 2*384, -3.8*Cluster::GetPitch(1), 3.8*Cluster::GetPitch(1));
  }

  TH1F* htheta = new TH1F("htheta", "htheta", 1000, -0.005*TMath::Pi(), 0.005*TMath::Pi());
  TH1F* htphi = new TH1F("hphi", "hphi", 1000, TMath::Pi(), TMath::Pi());
  TH1F* hX0 = new TH1F("hX0", "hX0", 1000, -100, 100);
  TH1F* hY0 = new TH1F("hY0", "hY0", 1000, -100, 100);
  TH1F* hchi = new TH1F("hchi", "hchi", 1000, -5, 10);
  
  //  for (int index_event=405; index_event<406; index_event++) {
  for (int index_event=0; index_event<entries; index_event++) {
    //    printf("----- new event %d\n", index_event);
    chain->GetEntry(index_event);
    
    int NClusTot = ev->NClusTot;

    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];

    bool trackfitok = ev->FindTrackAndFit(3, 3, false);//at least 2 points on S, and 2 points on K, not verbose
    //    printf("%d\n", trackfitok);
    if (!trackfitok) continue;
    //    printf("%f %f %f %f %f\n", ev->GetChiBestTrack(), ev->GetThetaBestTrack(), ev->GetPhiBestTrack(), ev->GetX0BestTrack(), ev->GetY0BestTrack());

    htheta->Fill(ev->GetThetaBestTrack());
    htphi->Fill(ev->GetPhiBestTrack());
    hX0->Fill(ev->GetX0BestTrack());
    hY0->Fill(ev->GetY0BestTrack());
    hchi->Fill(log10(ev->GetChiBestTrack()));
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
      if (!ev->IsClusterUsedInBestTrack(index_cluster)) continue;
      
      cl = ev->GetCluster(index_cluster);
      int ladder = cl->ladder;
      
      // printf("%d --> %d\n", ladder, rh->tdrCmpMap[ladder]);
      // printf("%d --> %d\n", ladder, rh->FindPos(ladder));
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

  for (int tt=0; tt<_maxtdr; tt++) {
    TF1* gauss = new TF1("gauss", "gaus", -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));
    residual_posS[tt]->Fit("gauss", "Q", "", -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));
    residual_posS[tt]->Fit("gauss", "Q", "", -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));
    double Smean = /*ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0) +*/ gauss->GetParameter(1);
    //    printf("S align par = %f\n", ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0));
    residual_posK[tt]->Fit("gauss", "Q", "", -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));
    residual_posK[tt]->Fit("gauss", "Q", "", -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));
    double Kmean = /*ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1) +*/ gauss->GetParameter(1);
    //    printf("K align par = %f\n", ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1));
    printf("%d -> %d) %f %f\n", rh->tdrCmpMap[tt], tt, Smean, Kmean);
  }

  foutput->Write();
  foutput->Close();
  
  return 0;
}
