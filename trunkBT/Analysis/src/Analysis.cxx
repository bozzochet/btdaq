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
#include "TSpectrum.h"
#include "TStopwatch.h"
#include <fstream>
#include <vector>

/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

using namespace std;

bool CleanEvent(Event* ev, RHClass *rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS=0, int safetyK=0);

int main(int argc, char* argv[]) {
  
  if (argc<3) {
    printf("Usage:\n");
    printf("%s <output root-filename> <first input root-filename> [second input root-filename] ...\n", argv[0]);
    return 1;
  }
  
  TChain *chain = new TChain("t4");
     
  for (int ii=2; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }

  TString align_filename = "alignment.dat";
  TString output_filename = argv[1];

  printf("---------------------------------------------\n");
  Event::ReadAlignment(align_filename.Data());
  printf("---------------------------------------------\n");
  
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
  
  TFile* foutput = new TFile(output_filename.Data(), "RECREATE");
  foutput->cd();
  
  TH1F* occupancy[NJINF*NTDRS];
  TH1F* occupancy_posS[NJINF*NTDRS];
  TH1F* occupancy_posK[NJINF*NTDRS];
  TH1F* residual_posS[NJINF*NTDRS];
  TH1F* residual_posK[NJINF*NTDRS];
  TH1F* chargeS[NJINF*NTDRS];
  TH1F* chargeK[NJINF*NTDRS];
  TH2F* charge2D[NJINF*NTDRS];
  int NSTRIPSS=640;
  int NSTRIPSK=384;
  for (int tt=0; tt<_maxtdr; tt++) {
    occupancy[tt] = new TH1F(Form("occupancy_0_%02d", rh->tdrCmpMap[tt]), Form("occupancy_0_%02d", rh->tdrCmpMap[tt]), 1024, 0, 1024);
    occupancy_posS[tt] = new TH1F(Form("occupancy_posS_0_%02d", rh->tdrCmpMap[tt]), Form("occupancy_posS_0_%02d", rh->tdrCmpMap[tt]), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
    occupancy_posK[tt] = new TH1F(Form("occupancy_posK_0_%02d", rh->tdrCmpMap[tt]), Form("occupancy_posK_0_%02d", rh->tdrCmpMap[tt]), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(1), NSTRIPSK*Cluster::GetPitch(1));
    residual_posS[tt] = new TH1F(Form("residual_posS_0_%02d", rh->tdrCmpMap[tt]), Form("residual_posS_0_%02d", rh->tdrCmpMap[tt]), 
				 2*NSTRIPSS, -float(NSTRIPSS)/100.*Cluster::GetPitch(0), float(NSTRIPSS)/100.*Cluster::GetPitch(0));
    residual_posK[tt] = new TH1F(Form("residual_posK_0_%02d", rh->tdrCmpMap[tt]), Form("residual_posK_0_%02d", rh->tdrCmpMap[tt]), 
				 40*NSTRIPSK, -20*float(NSTRIPSK)/100.*Cluster::GetPitch(1), 20*float(NSTRIPSK)/100.*Cluster::GetPitch(1));
    chargeS[tt] = new TH1F(Form("chargeS_0_%02d", rh->tdrCmpMap[tt]), Form("chargeS_0_%02d", rh->tdrCmpMap[tt]), 1000, 0, 100);
    chargeK[tt] = new TH1F(Form("chargeK_0_%02d", rh->tdrCmpMap[tt]), Form("chargeK_0_%02d", rh->tdrCmpMap[tt]), 1000, 0, 100);
    charge2D[tt] = new TH2F(Form("charge_0_%02d", rh->tdrCmpMap[tt]), Form("charge_0_%02d", rh->tdrCmpMap[tt]), 1000, 0, 100, 1000, 0, 100);
  }
  
  TH1F* htheta = new TH1F("htheta", "htheta", 1000, -45.0, 45.0);
  TH1F* htphi = new TH1F("hphi", "hphi", 1000, -180.0, 180.0);
  TH1F* hX0 = new TH1F("hX0", "hX0", 1000, -100, 100);
  TH1F* hY0 = new TH1F("hY0", "hY0", 1000, -100, 100);
  TH1F* hchi = new TH1F("hchi", "hchi", 1000, -5, 10);

  TH1F* hclusSladd = new TH1F("hclusSladd", "hclusSladd;Ladder;Clusters", 24, 0, 24);
  TH1F* hclusSladdtrack = new TH1F("hclusSladdtrack", "hclusSladdtrack;Ladder;Clusters", 24, 0, 24);
  TH1F* hclusKladd = new TH1F("hclusKladd", "hclusKladd;Ladder;Clusters", 24, 0, 24);
  TH1F* hclusKladdtrack = new TH1F("hclusKladdtrack", "hclusKladdtrack;Ladder;Clusters", 24, 0, 24);
  TH1F* hclus = new TH1F("hclus", "hclus;Clusters", 1000, 0, 1000);

  Long64_t tracks=0;
  Long64_t goodtracks=0;
  Long64_t goodStracks=0;
  Long64_t goodKtracks=0;

  TStopwatch sw;
  sw.Start();
  
  for (int index_event=0; index_event<entries; index_event++) {
    //    printf("----- new event %d\n", index_event);
    chain->GetEntry(index_event);
    
    int NClusTot = ev->NClusTot;
    //    printf("\t\tnclusters = %d\n", NClusTot);

    //at least 6 clusters and at most 12
    //at most 3 clusters per ladder (per side) + 1 additional clusters in total (per side)
    bool cleanevent = CleanEvent(ev, rh, 6, 30, 3, 3, 0, 0);
    if (!cleanevent) continue;
    
    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddK[NJINF*NTDRS];
    std::pair<double,double> qpair[NJINF*NTDRS];
    for (int pp=0; pp<_maxtdr; pp++) {
      qpair[pp].first=-9999.9;
      qpair[pp].second=-9999.9;
    }
    
    bool trackfitok = ev->FindTrackAndFit(3, 3, false);//at least 3 points on 3, and 2 points on K, not verbose

    //    printf("trackfit: %d\n", trackfitok);
    if (!trackfitok) continue;
    tracks++;
    //    printf("%f %f %f %f %f\n", ev->GetChiTrack(), ev->GetThetaTrack(), ev->GetPhiTrack(), ev->GetX0Track(), ev->GetY0Track());

    //    printf("S %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(0), ev->IsTDRInTrack(0, 0), ev->IsTDRInTrack(0, 4), ev->IsTDRInTrack(0, 8), ev->IsTDRInTrack(0, 12), ev->IsTDRInTrack(0, 14));
    //    printf("K %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(1), ev->IsTDRInTrack(1, 0), ev->IsTDRInTrack(1, 4), ev->IsTDRInTrack(1, 8), ev->IsTDRInTrack(1, 12), ev->IsTDRInTrack(1, 14));
    
    // //                              321098765432109876543210
    // if (ev->GetTrackHitPattern(0) <                100010001) continue;
    // if (ev->GetTrackHitPattern(1) <                100010001) continue;

    double logchi = log10(ev->GetChiTrack());
    if (logchi>2) continue;
    goodtracks++;
    
    bool strackok = false;
    bool ktrackok = false;

    /*
    if (
	ev->IsTDRInTrack(0, 0) &&
	ev->IsTDRInTrack(0, 4) &&
	ev->IsTDRInTrack(0, 8) &&
	(ev->IsTDRInTrack(0, 12) || ev->IsTDRInTrack(0, 14)) ) {
      strackok=true;
      goodStracks++;
    }

    if (
	ev->IsTDRInTrack(1, 0) &&
	ev->IsTDRInTrack(1, 4) &&
	ev->IsTDRInTrack(1, 8) &&
	(ev->IsTDRInTrack(1, 12) || ev->IsTDRInTrack(1, 14)) ) {
      ktrackok=true;
      goodKtracks++;
    }
    */
    strackok=true;
    ktrackok=true;
    
    htheta->Fill(ev->GetThetaTrack()*180.0/TMath::Pi());
    htphi->Fill(ev->GetPhiTrack()*180.0/TMath::Pi());
    hX0->Fill(ev->GetX0Track());
    hY0->Fill(ev->GetY0Track());
    hchi->Fill(log10(ev->GetChiTrack()));

    hclus->Fill(NClusTot);
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
	
      cl = ev->GetCluster(index_cluster);
      
      int ladder = cl->ladder;
      //      printf("%d --> %d\n", ladder, rh->FindPos(ladder));
      occupancy[rh->FindPos(ladder)]->Fill(cl->GetCoG());
      int side=cl->side;
      
      if (side==0) {
	occupancy_posS[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddS[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
      else {
	occupancy_posK[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddK[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
            
      if (!ev->IsClusterUsedInTrack(index_cluster)) continue;

      double charge=cl->GetCharge();
      
      if (side==0) {
	if (strackok) {
	  residual_posS[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 0));
	  v_cog_laddS[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	  hclusSladdtrack->Fill(ladder);
	  chargeS[rh->FindPos(ladder)]->Fill(charge);
	  qpair[rh->FindPos(ladder)].first=charge;
	}
      }
      else {
	if (ktrackok) {
	  residual_posK[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 1));
	  v_cog_laddK[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	  hclusKladdtrack->Fill(ladder);
	  chargeK[rh->FindPos(ladder)]->Fill(charge);
	  qpair[rh->FindPos(ladder)].second=charge;
	}
      }
      
    }

    for (int tt=0; tt<_maxtdr; tt++) {
      //      printf("%d\n", tt);
      if (qpair[tt].first>-9000.0 && qpair[tt].second>-9000.0) {
	charge2D[tt]->Fill(qpair[tt].first, qpair[tt].second);
      }
    }
    
    for (int ll=0; ll<NJINF*NTDRS; ll++) {
      hclusSladd->Fill(rh->tdrCmpMap[ll], v_cog_all_laddS[ll].size());
      hclusKladd->Fill(rh->tdrCmpMap[ll], v_cog_all_laddK[ll].size());
    }
    
    //    printf(" \n ");
  }

  sw.Stop();
  sw.Print();
  
  printf("---------------------------------------------\n");
  printf("\t%lld events analyzed\n", entries);
  printf("\t\t%lld tracks fitted\n", tracks);
  printf("\t\t%lld good tracks found\n", goodtracks);
  // printf("\t\t%lld good S tracks found\n", goodStracks);
  // printf("\t\t%lld good K tracks found\n", goodKtracks);
  printf("---------------------------------------------\n");
  
  foutput->Write();
  foutput->Close();
  
  return 0;
}

bool CleanEvent(Event* ev, RHClass *rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS, int safetyK){

  int NClusTot = ev->NClusTot;
  if(NClusTot<(minclus-1) ||  NClusTot>(maxclus+1)) return false;
  
  int nclusS[NJINF*NTDRS];
  int nclusK[NJINF*NTDRS];
  memset(nclusS, 0, sizeof(nclusS[0])*NJINF*NTDRS);
  memset(nclusK, 0, sizeof(nclusK[0])*NJINF*NTDRS);

  int safetySspent = safetyS;
  int safetyKspent = safetyK;
  
  for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
    Cluster *cl = ev->GetCluster(index_cluster);
    int ladder = cl->ladder;
    //      printf("%d --> %d\n", ladder, rh->FindPos(ladder));
    int side=cl->side;
    if (side==0) {
      nclusS[rh->FindPos(ladder)]++;
      if (nclusS[rh->FindPos(ladder)]>=(perladdS+safetyS)) {
	safetySspent--;
	if (safetySspent<0) return false;
      }
    }
    else {
      nclusK[rh->FindPos(ladder)]++;
      if (nclusK[rh->FindPos(ladder)]>=(perladdK+safetyK)) {
	safetyKspent--;
	if (safetyKspent<0) return false;
      }
    }
  }

  /*
  for (int jj=0; jj<NJINF*NTDRS; jj++) {
    printf("%d %d\n", nclusS[jj], nclusK[jj]);
  }
  */
  
  return true;
}
