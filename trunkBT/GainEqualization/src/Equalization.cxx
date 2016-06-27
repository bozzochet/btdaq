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

/* from the CommonTool dir */
#include "TrackSelection.hh"
#include "Utilities.hh"
/* end */

using namespace std;

//raw value. To put the real distance from the most upward AMS ladder to the desired position in the ICC
#define ZHERD 1500

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
  TString gaincorrection_filename = "gaincorrection.dat";
  TString output_filename = argv[1];

  printf("---------------------------------------------\n");
  Event::ReadAlignment(align_filename.Data());
  Event::ReadGainCorrection(gaincorrection_filename.Data());
  //  exit(1);
  printf("---------------------------------------------\n");
  
  Event *ev;
  Cluster *cl;
  
  Long64_t entries = chain->GetEntries();
  printf("This run has %lld entries\n", entries);
  
  ev = new Event();
  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  int _maxtdr = NJINF*NTDRS;
  
  if (GetRH(chain)) {
    GetRH(chain)->Print();
    _maxtdr = GetRH(chain)->ntdrCmp;
  }
  else {
    printf("Not able to find the RHClass header in the UserInfo...\n");
    return -9;
  }
  //  printf("%d\n", _maxladd);
  
  TFile* foutput = new TFile(output_filename.Data(), "RECREATE");
  foutput->cd();
  
  TH1F* occupancy[NJINF*NTDRS];
  TH1F* occupancy_posS[NJINF*NTDRS];
  TH1F* occupancy_posK[NJINF*NTDRS];
  TH1F* residual_S[NJINF*NTDRS];
  TH1F* residual_K[NJINF*NTDRS];
  TH1F* chargeS[NJINF*NTDRS];
  TH1F* chargeK[NJINF*NTDRS];
  TH2F* charge2D[NJINF*NTDRS];
  TH1F* chargeS_ave;
  TH1F* chargeK_ave;
  TH2F* charge2D_ave;
  int NSTRIPSS=640;
  int NSTRIPSK=384;
  for (int tt=0; tt<_maxtdr; tt++) {
    occupancy[tt] = new TH1F(Form("occupancy_0_%02d", GetRH(chain)->tdrCmpMap[tt]), Form("occupancy_0_%02d;Channel number;Occupancy", GetRH(chain)->tdrCmpMap[tt]), 1024, 0, 1024);
    occupancy_posS[tt] = new TH1F(Form("occupancy_posS_0_%02d", GetRH(chain)->tdrCmpMap[tt]), Form("occupancy_posS_0_%02d;Position_{S} (mm);Occupancy", GetRH(chain)->tdrCmpMap[tt]), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
    occupancy_posK[tt] = new TH1F(Form("occupancy_posK_0_%02d", GetRH(chain)->tdrCmpMap[tt]), Form("occupancy_posK_0_%02d;Position_{K} (mm);Occupancy", GetRH(chain)->tdrCmpMap[tt]), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(1), NSTRIPSK*Cluster::GetPitch(1));
    residual_S[tt] = new TH1F(Form("residual_S_0_%02d", GetRH(chain)->tdrCmpMap[tt]), Form("residual_S_0_%02d;Residual_{S} (mm);Entries", GetRH(chain)->tdrCmpMap[tt]), 
				 2*NSTRIPSS, -float(NSTRIPSS)/100.*Cluster::GetPitch(0), float(NSTRIPSS)/100.*Cluster::GetPitch(0));
    residual_K[tt] = new TH1F(Form("residual_K_0_%02d", GetRH(chain)->tdrCmpMap[tt]), Form("residual_K_0_%02d;Residual_{K} (mm);Entries", GetRH(chain)->tdrCmpMap[tt]), 
				 40*NSTRIPSK, -20*float(NSTRIPSK)/100.*Cluster::GetPitch(1), 20*float(NSTRIPSK)/100.*Cluster::GetPitch(1));
    chargeS[tt] = new TH1F(Form("chargeS_0_%02d", GetRH(chain)->tdrCmpMap[tt]), Form("chargeS_0_%02d;Q_{S} (c.u.);Entries", GetRH(chain)->tdrCmpMap[tt]), 1000, 0, 100);
    chargeK[tt] = new TH1F(Form("chargeK_0_%02d", GetRH(chain)->tdrCmpMap[tt]), Form("chargeK_0_%02d;Q_{K} (c.u.);Entries", GetRH(chain)->tdrCmpMap[tt]), 1000, 0, 100);
    charge2D[tt] = new TH2F(Form("charge_0_%02d", GetRH(chain)->tdrCmpMap[tt]), Form("charge_0_%02d;Q_{S} (c.u.);Q_{K} (c.u.);Entries", GetRH(chain)->tdrCmpMap[tt]), 1000, 0, 100, 1000, 0, 100);
  }
  chargeS_ave = new TH1F("chargeS", "chargeS;Q_{S} (c.u.);Entries", 1000, 0, 100);
  chargeK_ave = new TH1F("chargeK", "chargeK;Q_{K} (c.u.);Entries", 1000, 0, 100);
  charge2D_ave = new TH2F("charge", "charge;Q_{S} (c.u.);Q_{K} (c.u.);Entries", 1000, 0, 100, 1000, 0, 100);

  TH1F* chi = new TH1F("chi", "chi;log10(#chi^{2});Entries", 1000, -5, 10);
  TH1F* theta = new TH1F("theta", "theta;#theta (rad);Entries", 10000, -1.0, 1.0);
  TH1F* phi = new TH1F("phi", "phi;#phi (rad);Entries", 1000, -TMath::Pi(), TMath::Pi());
  TH2F* thetaphi = new TH2F("thetaphi", "thetaphi;#theta (rad);#phi (rad);Entries", 10000, -1.0, 1.0, 1000, -TMath::Pi(), TMath::Pi());
  TH1F* X0 = new TH1F("X0", "X0;X_{Z=0} (mm);Entries", 1000, -100, 100);
  TH1F* Y0 = new TH1F("Y0", "Y0;Y_{Z=0} (mm);Entries", 1000, -100, 100);
  TH2F* X0Y0 = new TH2F("X0Y0", "X0Y0;X_{Z=0} (mm);Y_{Z=0} (mm);Entries", 1000, -100, 100, 1000, -100, 100);
  TH1F* X0HERD = new TH1F("X0HERD", "X0HERD;X_{Z=HERD} (mm);Entries", 1000, -100, 100);
  TH1F* Y0HERD = new TH1F("Y0HERD", "Y0HERD;Y_{Z=HERD} (mm);Entries", 1000, -100, 100);
  TH2F* X0Y0HERD = new TH2F("X0Y0HERD", "X0Y0HERD;X_{Z=HERD} (mm);Y_{Z=HERD} (mm);Entries", 1000, -100, 100, 1000, -100, 100);

  TH1F* hclusSladd = new TH1F("hclusSladd", "hclusSladd;Ladder;Clusters_{S}", 24, 0, 24);
  TH1F* hclusSladdtrack = new TH1F("hclusSladdtrack", "hclusSladdtrack;Ladder;Clusters_{S,OnTrack}", 24, 0, 24);
  TH1F* hclusKladd = new TH1F("hclusKladd", "hclusKladd;Ladder;Clusters_{K}", 24, 0, 24);
  TH1F* hclusKladdtrack = new TH1F("hclusKladdtrack", "hclusKladdtrack;Ladder;Clusters_{K,OnTrack}", 24, 0, 24);
  TH1F* hclus = new TH1F("hclus", "hclus;Clusters;Entries", 1000, 0, 1000);

  Long64_t cleanevs=0;
  Long64_t tracks=0;
  Long64_t goodtracks=0;
  Long64_t goodStracks=0;
  Long64_t goodKtracks=0;

  TStopwatch sw;
  sw.Start();

  double perc=0;
  
  for (int index_event=0; index_event<entries; index_event++) {
    Double_t pperc=1000.0*((index_event+1.0)/entries);
    if (pperc>=perc) {
      printf("Processed %d out of %lld: %d%%\n", (index_event+1), entries, (int)(100.0*(index_event+1.0)/entries));
      perc++;
    }
    //    printf("----- new event %d/%lld = %d%%\n", index_event, entries, (int)(100.0*(index_event+1.0)/entries));
    chain->GetEntry(index_event);
    
    int NClusTot = ev->GetNClusTot();
    //    printf("\t\tnclusters = %d\n", NClusTot);

    //at least 4 clusters (if we want 2 on S and 2 on K this is really the sindacal minimum...)
    //and at most 50 (to avoid too much noise around and too much combinatorial)
    //at most 6 clusters per ladder (per side) + 0 additional clusters in total (per side)
    bool cleanevent = CleanEvent(ev, GetRH(chain), 4, 50, 6, 6, 0, 0);
    if (!cleanevent) continue;
    cleanevs++;
    
    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddK[NJINF*NTDRS];

    //at least 3 points on S, and 3 points on K, not verbose
    bool trackfitok = ev->FindTrackAndFit(3, 3, false);
    //    printf("%d\n", trackfitok);
    if (trackfitok) {
      //remove from the best fit track the worst hit if giving a residual greater than 6.0 sigmas on S and 6.0 sigmas on K
      //(but only if removing them still we'll have more or equal than 3 hits on S and 3 (2 if 'HigherCharge') hits on K)
      //and perform the fit again
      ev->RefineTrack(6.0, 2, 6.0, 2);
    }
    else {
      //let's downscale to 2 (on S) and 2 (on K) hits but even in this no-chisq case
      //let's garantee a certain relaiability of the track fitting the one with the higher charge (this method can be used also for ions)
      //and requiring an higher S/N for the cluster
      trackfitok = ev->FindHigherChargeTrackAndFit(2, 5.0, 2, 5.0, false);
    }
    if (!trackfitok) continue;
    //    printf("%f %f %f %f %f\n", ev->GetChiTrack(), ev->GetThetaTrack(), ev->GetPhiTrack(), ev->GetX0Track(), ev->GetY0Track());    
    tracks++;

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

    // if (ev->GetNHitsTrack()>5) {
    //   printf("Nhits: %u (S: %u, K: %u)\n", ev->GetNHitsTrack(), ev->GetNHitsSTrack(), ev->GetNHitsKTrack());
    // }

    chi->Fill(log10(ev->GetChiTrack()));
    theta->Fill(ev->GetThetaTrack());
    phi->Fill(ev->GetPhiTrack());
    thetaphi->Fill(ev->GetThetaTrack(), ev->GetPhiTrack());
    X0->Fill(ev->GetX0Track());
    Y0->Fill(ev->GetY0Track());
    X0Y0->Fill(ev->GetX0Track(), ev->GetY0Track());
    X0HERD->Fill(ev->ExtrapolateTrack(ZHERD, 0));
    Y0HERD->Fill(ev->ExtrapolateTrack(ZHERD, 1));
    X0Y0HERD->Fill(ev->ExtrapolateTrack(ZHERD, 0), ev->ExtrapolateTrack(ZHERD, 1));

    hclus->Fill(NClusTot);
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
	
      cl = ev->GetCluster(index_cluster);
      
      int ladder = cl->ladder;
      //      printf("%d --> %d\n", ladder, GetRH(chain)->FindPos(ladder));
      occupancy[GetRH(chain)->FindPos(ladder)]->Fill(cl->GetCoG());
      int side=cl->side;
      
      if (side==0) {
	occupancy_posS[GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddS[GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
      else {
	occupancy_posK[GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddK[GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
            
      if (!ev->IsClusterUsedInTrack(index_cluster)) continue;

      double charge=cl->GetCharge();//unused for now
      
      if (side==0) {
	if (strackok) {
	  residual_S[GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 0));
	  v_cog_laddS[GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	  hclusSladdtrack->Fill(ladder);
	}
      }
      else {
	if (ktrackok) {
	  residual_K[GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 1));
	  v_cog_laddK[GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	  hclusKladdtrack->Fill(ladder);
	}
      }
      
    }

    std::vector<std::pair<int, std::pair<int, int> > > vec_charge = ev->GetHitVector();
    for (unsigned int tt=0; tt<vec_charge.size(); tt++) {
      int ladder = vec_charge.at(tt).first;
      int index_cluster_S = vec_charge.at(tt).second.first;
      int index_cluster_K = vec_charge.at(tt).second.second;
      if (index_cluster_S>=0) {
	chargeS[GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_S)->GetCharge());
      }
      if (index_cluster_K>=0) {
	chargeK[GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_K)->GetCharge());
      }
      if (index_cluster_S>=0 && index_cluster_K>=0) {
	charge2D[GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_S)->GetCharge(), ev->GetCluster(index_cluster_K)->GetCharge());
      }
    }
    
    chargeS_ave->Fill(ev->GetChargeTrack(0));
    chargeK_ave->Fill(ev->GetChargeTrack(1));
    charge2D_ave->Fill(ev->GetChargeTrack(0), ev->GetChargeTrack(1));
    
    for (int ll=0; ll<NJINF*NTDRS; ll++) {
      hclusSladd->Fill(GetRH(chain)->tdrCmpMap[ll], v_cog_all_laddS[ll].size());
      hclusKladd->Fill(GetRH(chain)->tdrCmpMap[ll], v_cog_all_laddK[ll].size());
    }
    
    //    printf(" \n ");
  }

  sw.Stop();
  sw.Print();
  
  printf("---------------------------------------------\n");
  printf("\t%lld events analyzed\n", entries);
  printf("\t\t%lld clean events\n", cleanevs);
  printf("\t\t%lld tracks fitted\n", tracks);
  printf("\t\t%lld good tracks found\n", goodtracks);
  // printf("\t\t%lld good S tracks found\n", goodStracks);
  // printf("\t\t%lld good K tracks found\n", goodKtracks);
  printf("---------------------------------------------\n");
  
  foutput->Write();
  foutput->Close();
  
  return 0;
}
