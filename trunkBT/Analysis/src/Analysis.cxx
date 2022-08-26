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
#include <bitset>

/* from the 'Decode' API */
#include "GenericEvent.hpp"
/* end */

/* from the CommonTool dir */
#include "TrackSelection.hpp"
#include "Utilities.hpp"
/* end */

using namespace std;

//raw value. To put the real distance from the most upward AMS ladder to the desired position in the target detector
#define ZTARGETDET 0

using EventOCA = GenericEvent<1, 24, 64, 5, 10, 0>;
using calibOCA = calib<EventOCA::GetNCHAVA() * EventOCA::GetNVAS()>;
using RHClassOCA = RHClass<EventOCA::GetNJINF(), EventOCA::GetNTDRS()>;
//---
using EventFOOT = GenericEvent<1, 24, 64, 5, 10, 0>;
using calibFOOT = calib<EventFOOT::GetNCHAVA() * EventFOOT::GetNVAS()>;
using RHClassFOOT = RHClass<EventFOOT::GetNJINF(), EventFOOT::GetNTDRS()>;
//---
using EventAMS = GenericEvent<1, 24, 64, 3, 16, 10>;
using calibAMS = calib<EventAMS::GetNCHAVA() * EventAMS::GetNVAS()>;
using RHClassAMS = RHClass<EventAMS::GetNJINF(), EventAMS::GetNTDRS()>;

/*
template <>
bool TrackSelection::CleanEvent<EventOCA, RHClassOCA>(EventOCA* ev, RHClassOCA* rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS, int safetyK);
template <>
bool TrackSelection::CleanEvent<EventFOOT, RHClassFOOT>(EventFOOT* ev, RHClassFOOT* rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS, int safetyK);
template <>
bool TrackSelection::CleanEvent<EventAMS, RHClassAMS>(EventAMS* ev, RHClassAMS* rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS, int safetyK);
*/

template <class Event, class RH>
int ProcessChain(TChain* ch, TString output_filename);

//----------------------------------------

int main(int argc, char* argv[]) {
  
  if (argc<3) {
    printf("Usage:\n");
    printf("%s <output root-filename> <first input root-filename> [second input root-filename] ...\n", argv[0]);
    return 1;
  }

  bool kOca = false;
  bool kFoot = false;

  TString exename = argv[0];
  
  if (exename.Contains("OCA")) {
    kOca = true;
  }
  else if (exename.Contains("FOOT")) {
    kFoot = true;
  }
  
  TChain *chain = new TChain("t4");
     
  for (int ii=2; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }

  TString output_filename = argv[1];

  printf("---------------------------------------------\n");
  if (kOca) {
    EventOCA::ReadAlignment("alignment_OCA.dat");
    EventOCA::ReadGainCorrection("gaincorrection_OCA.dat");
    return ProcessChain<EventOCA, RHClassOCA>(chain, output_filename);
  }
  else if (kFoot) {
    EventFOOT::ReadAlignment("alignment_FOOT.dat");
    EventFOOT::ReadGainCorrection("gaincorrection_FOOT.dat");
    return ProcessChain<EventFOOT, RHClassFOOT>(chain, output_filename);
  }
  else {
    EventAMS::ReadAlignment("alignment.dat");
    EventAMS::ReadGainCorrection("gaincorrection.dat");
    return ProcessChain<EventAMS, RHClassAMS>(chain, output_filename);
  }
  printf("---------------------------------------------\n");
  
  return 0;
}

template <class Event, class RH>
int ProcessChain(TChain* chain, TString output_filename){

  using TS = TrackSelection<Event, RH>;
  TS* ts = new TS();
  using UT = Utilities<Event, RH>;
  UT* ut = new UT();
  
  static int NJINF = Event::GetNJINF();
  static int NTDRS = Event::GetNTDRS();
  static int NVAS = Event::GetNVAS();
  static int NCHAVA = Event::GetNCHAVA();
  static int NADCS = Event::GetNADCS();

  Event* ev = new Event();
  Cluster *cl;
  
  Long64_t entries = chain->GetEntries();
  //  entries=10000;
  printf("This run has %lld entries\n", entries);

  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);
  
  int _maxtdr = NJINF*NTDRS;
  
  if (ut->GetRH(chain)) {
    ut->GetRH(chain)->Print();
    _maxtdr = ut->GetRH(chain)->GetNTdrs();
  }
  else {
    printf("Not able to find the RHClass header in the UserInfo...\n");
    return -9;
  }
  //  printf("%d\n", _maxladd);
  
  // for (int tt=0; tt<_maxtdr; tt++) {
  //   printf("%d %d %f %f\n", tt, ut->GetRH(chain)->GetTdrNum(tt), Cluster::GetPitch(0, ut->GetRH(chain)->GetTdrNum(tt), 0), Cluster::GetPitch(0, ut->GetRH(chain)->GetTdrNum(tt), 1));
  // }

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
    int jinfnum = 0;
    int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
    double pitchS = std::max(Cluster::GetPitch(jinfnum, tdrnum, 0), 0.01);
    double pitchK = std::max(Cluster::GetPitch(jinfnum, tdrnum, 1), 0.01);
    occupancy[tt] = new TH1F(Form("occupancy_0_%02d", tdrnum), Form("occupancy_0_%02d;Channel number;Occupancy", tdrnum), 1024, 0, 1024);
    occupancy_posS[tt] = new TH1F(Form("occupancy_posS_0_%02d", tdrnum), Form("occupancy_posS_0_%02d;Position_{S} (mm);Occupancy", tdrnum), 2*NSTRIPSS, -NSTRIPSS*pitchS, NSTRIPSS*pitchS);
    occupancy_posK[tt] = new TH1F(Form("occupancy_posK_0_%02d", tdrnum), Form("occupancy_posK_0_%02d;Position_{K} (mm);Occupancy", tdrnum), 2*NSTRIPSK, -NSTRIPSK*pitchK, NSTRIPSK*pitchK);
    residual_S[tt] = new TH1F(Form("residual_S_0_%02d", tdrnum), Form("residual_S_0_%02d;Residual_{S} (mm);Entries", tdrnum), 
			      2*NSTRIPSS, -float(NSTRIPSS)/100.*pitchS, float(NSTRIPSS)/100.*pitchS);
    residual_K[tt] = new TH1F(Form("residual_K_0_%02d", tdrnum), Form("residual_K_0_%02d;Residual_{K} (mm);Entries", tdrnum), 
			      40*NSTRIPSK, -20*float(NSTRIPSK)/100.*pitchK, 20*float(NSTRIPSK)/100.*pitchK);
    chargeS[tt] = new TH1F(Form("chargeS_0_%02d", tdrnum), Form("chargeS_0_%02d;Q_{S} (c.u.);Entries", tdrnum), 1000, 0, 100);
    chargeK[tt] = new TH1F(Form("chargeK_0_%02d", tdrnum), Form("chargeK_0_%02d;Q_{K} (c.u.);Entries", tdrnum), 1000, 0, 100);
    charge2D[tt] = new TH2F(Form("charge_0_%02d", tdrnum), Form("charge_0_%02d;Q_{S} (c.u.);Q_{K} (c.u.);Entries", tdrnum), 1000, 0, 100, 1000, 0, 100);
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
  TH1F* X0TARGETDET = new TH1F("X0TARGETDET", "X0TARGETDET;X_{Z=TARGETDET} (mm);Entries", 1000, -100, 100);
  TH1F* Y0TARGETDET = new TH1F("Y0TARGETDET", "Y0TARGETDET;Y_{Z=TARGETDET} (mm);Entries", 1000, -100, 100);
  TH2F* X0Y0TARGETDET = new TH2F("X0Y0TARGETDET", "X0Y0TARGETDET;X_{Z=TARGETDET} (mm);Y_{Z=TARGETDET} (mm);Entries", 1000, -100, 100, 1000, -100, 100);

  TH1F* hclusSladd = new TH1F("hclusSladd", "hclusSladd;Ladder;Clusters_{S}", 24, 0, 24);
  TH1F* hclusSladdtrack = new TH1F("hclusSladdtrack", "hclusSladdtrack;Ladder;Clusters_{S,OnTrack}", 24, 0, 24);
  TH1F* hclusKladd = new TH1F("hclusKladd", "hclusKladd;Ladder;Clusters_{K}", 24, 0, 24);
  TH1F* hclusKladdtrack = new TH1F("hclusKladdtrack", "hclusKladdtrack;Ladder;Clusters_{K,OnTrack}", 24, 0, 24);
  TH1F* hclus = new TH1F("hclus", "hclus;Clusters;Entries", 1000, 0, 1000);
  TH1F* hclus_aftersel = new TH1F("hclus_aftersel", "hclus_aftersel;Clusters;Entries", 1000, 0, 1000);

  Long64_t cleanevs=0;
  Long64_t preselevs=0;
  Long64_t tracks=0;
  Long64_t goodtracks=0;
  Long64_t goldtracks=0;
  Long64_t goldStracks=0;
  Long64_t goldKtracks=0;

  TStopwatch sw;
  sw.Start();

  double perc=0;

  /* valid for TIC BT?
  ts->ExcludeTDR(ev, 0, 2, 0);
  ts->ExcludeTDR(ev, 0, 2, 1);
  ts->ExcludeTDR(ev, 0, 3, 0);
  ts->ExcludeTDR(ev, 0, 3, 1);
  ts->ExcludeTDR(ev, 0, 6, 0);
  ts->ExcludeTDR(ev, 0, 6, 1);
  ts->ExcludeTDR(ev, 0, 7, 0);
  ts->ExcludeTDR(ev, 0, 7, 1);
  ts->ExcludeTDR(ev, 0, 10, 0);
  ts->ExcludeTDR(ev, 0, 10, 1);
  ts->ExcludeTDR(ev, 0, 11, 0);
  ts->ExcludeTDR(ev, 0, 11, 1);
  ts->ExcludeTDR(ev, 0, 14, 0);
  ts->ExcludeTDR(ev, 0, 14, 1);
  ts->ExcludeTDR(ev, 0, 15, 0);
  ts->ExcludeTDR(ev, 0, 15, 1);
  ts->ExcludeTDR(ev, 0, 18, 0);
  ts->ExcludeTDR(ev, 0, 18, 1);
  ts->ExcludeTDR(ev, 0, 19, 0);
  ts->ExcludeTDR(ev, 0, 19, 1);
  */
  
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

    hclus->Fill(NClusTot);

    //at least 4 clusters (if we want 2 on S and 2 on K this is really the sindacal minimum...)
    //and at most 100 (to avoid too much noise around and too much combinatorial)
    //at most 6 clusters per ladder (per side) + 0 additional clusters in total (per side)
    // ts->CleanEvent(ev, ut->GetRH(chain), 4, 50, 6, 6, 0, 0);
    
    bool cleanevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 9999, 9999, 9999, 9999, 9999);
    if (!cleanevent) continue;
    cleanevs++;//in this way this number is giving the "complete reasonable sample"
    
    bool preselevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 100, 3, 3, 0, 0);
    if (!preselevent) continue;
    preselevs++;
    
    //at least 4 points on S, and 4 points on K, not verbose
    // ev->FindTrackAndFit(4, 4, false);
    
    bool trackfitok = ev->FindTrackAndFit(2, 2, false);
    //    printf("%d\n", trackfitok);
    if (trackfitok) {
      //      ev->RefineTrack(999999.0, 2, 999999.0, 2);
      //remove from the best fit track the worst hit if giving a residual greater than 6.0 sigmas on S and 6.0 sigmas on K
      //(but only if removing them still we'll have more or equal than 3 hits on S and 3 (2 if 'HigherCharge') hits on K)
      //and perform the fit again
      int minSclus=3;
      int minKclus=3;
      for (int ii=0; ii<_maxtdr-std::min(minSclus, minKclus); ii++) {
	ev->RefineTrack(6.0, minSclus, 6.0, minKclus);
      }
      //      ev->RefineTrack(1.0, 2, 1.0, 2);
    }
    else {
      //let's downscale to 2 (on S) and 2 (on K) hits but even in this no-chisq case
      //let's garantee a certain relaiability of the track fitting the one with the higher charge (this method can be used also for ions)
      //and requiring an higher S/N for the cluster
      //      trackfitok = ev->FindHigherChargeTrackAndFit(2, 5.0, 2, 5.0, false);
    }
    if (!trackfitok) continue;
    //    printf("%f %f %f %f %f\n", ev->GetChiTrack(), ev->GetThetaTrack(), ev->GetPhiTrack(), ev->GetS0Track(), ev->GetS0Track());    
    tracks++;

    //    printf("S %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(0), ev->IsTDRInTrack(0, 0), ev->IsTDRInTrack(0, 4), ev->IsTDRInTrack(0, 8), ev->IsTDRInTrack(0, 12), ev->IsTDRInTrack(0, 14));
    //    printf("K %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(1), ev->IsTDRInTrack(1, 0), ev->IsTDRInTrack(1, 4), ev->IsTDRInTrack(1, 8), ev->IsTDRInTrack(1, 12), ev->IsTDRInTrack(1, 14));
    
    // //                              321098765432109876543210
    // if (ev->GetTrackHitPattern(0) <                100010001) continue;
    // if (ev->GetTrackHitPattern(1) <                100010001) continue;

    // printf("S %s\n", std::bitset<NTDRS>(ev->GetTrackHitPattern(0)).to_string().c_str());
    // printf("K %s\n", std::bitset<NTDRS>(ev->GetTrackHitPattern(1)).to_string().c_str());

    // printf("S %d %d %d %d\n", ev->IsTDRInTrack(0, 20)?1:0, ev->IsTDRInTrack(0, 22)?1:0, ev->IsTDRInTrack(0, 22)?1:0, ev->IsTDRInTrack(0, 23)?1:0);
    // printf("K %d %d %d %d\n", ev->IsTDRInTrack(1, 20)?1:0, ev->IsTDRInTrack(1, 22)?1:0, ev->IsTDRInTrack(1, 22)?1:0, ev->IsTDRInTrack(1, 23)?1:0);

    double logchi = log10(ev->GetChiTrack());
    if (logchi>6) continue;
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
      goldStracks++;
    }

    if (
	ev->IsTDRInTrack(1, 0) &&
	ev->IsTDRInTrack(1, 4) &&
	ev->IsTDRInTrack(1, 8) &&
	(ev->IsTDRInTrack(1, 12) || ev->IsTDRInTrack(1, 14)) ) {
      ktrackok=true;
      goldKtracks++;
    }
    */
    int snhits=ev->GetNHitsSTrack();
    if (snhits>=3) {
      strackok=true;
      goldStracks++;
    }
    
    int knhits= ev->GetNHitsKTrack();
    if (knhits>=3) {
      ktrackok=true;
      goldKtracks++;
    }

    // printf("S -> %d %d\n", snhits, strackok);
    // printf("K -> %d %d\n", knhits, ktrackok);

    if (strackok & ktrackok) goldtracks++;

    // printf("S %d %d %d %d -> %d %d\n", ev->IsTDRInTrack(0, 20)?1:0, ev->IsTDRInTrack(0, 22)?1:0, ev->IsTDRInTrack(0, 22)?1:0, ev->IsTDRInTrack(0, 23)?1:0, snhits, strackok);
    // printf("K %d %d %d %d -> %d %d\n", ev->IsTDRInTrack(1, 20)?1:0, ev->IsTDRInTrack(1, 22)?1:0, ev->IsTDRInTrack(1, 22)?1:0, ev->IsTDRInTrack(1, 23)?1:0, knhits, ktrackok);

    // if (ev->GetNHitsTrack()>5) {
    //   printf("Nhits: %u (S: %u, K: %u)\n", ev->GetNHitsTrack(), ev->GetNHitsSTrack(), ev->GetNHitsKTrack());
    // }

    chi->Fill(log10(ev->GetChiTrack()));
    theta->Fill(ev->GetThetaTrack());
    phi->Fill(ev->GetPhiTrack());
    thetaphi->Fill(ev->GetThetaTrack(), ev->GetPhiTrack());
    X0->Fill(ev->GetS0Track());
    Y0->Fill(ev->GetK0Track());
    X0Y0->Fill(ev->GetS0Track(), ev->GetK0Track());
    X0TARGETDET->Fill(ev->ExtrapolateTrack(ZTARGETDET, 0));
    Y0TARGETDET->Fill(ev->ExtrapolateTrack(ZTARGETDET, 1));
    X0Y0TARGETDET->Fill(ev->ExtrapolateTrack(ZTARGETDET, 0), ev->ExtrapolateTrack(ZTARGETDET, 1));

    hclus_aftersel->Fill(NClusTot);

    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddK[NJINF*NTDRS];
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
      
      cl = ev->GetCluster(index_cluster);
      
      int ladder = cl->ladder;
      //      printf("%d --> %d\n", ladder, ut->GetRH(chain)->FindPos(ladder));
      occupancy[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetCoG());
      int side=cl->side;
      
      if (side==0) {
	occupancy_posS[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddS[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
      else {
	occupancy_posK[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddK[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
            
      if (!ev->IsClusterUsedInTrack(index_cluster)) continue;

      double charge=cl->GetCharge();//unused for now
      
      if (side==0) {
	if (strackok) {
	  residual_S[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 0));
	  v_cog_laddS[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	  hclusSladdtrack->Fill(ladder);
	}
      }
      else {
	if (ktrackok) {
	  residual_K[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 1));
	  v_cog_laddK[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
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
	chargeS[ut->GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_S)->GetCharge());
      }
      if (index_cluster_K>=0) {
	chargeK[ut->GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_K)->GetCharge());
      }
      if (index_cluster_S>=0 && index_cluster_K>=0) {
	charge2D[ut->GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_S)->GetCharge(), ev->GetCluster(index_cluster_K)->GetCharge());
      }
    }
    
    chargeS_ave->Fill(ev->GetChargeTrack(0));
    chargeK_ave->Fill(ev->GetChargeTrack(1));
    charge2D_ave->Fill(ev->GetChargeTrack(0), ev->GetChargeTrack(1));
    
    for (int ll=0; ll<NJINF*NTDRS; ll++) {
      hclusSladd->Fill(ut->GetRH(chain)->GetTdrNum(ll), v_cog_all_laddS[ll].size());
      hclusKladd->Fill(ut->GetRH(chain)->GetTdrNum(ll), v_cog_all_laddK[ll].size());
    }
    
    //    printf(" \n ");
  }

  sw.Stop();
  sw.Print();
  
  printf("---------------------------------------------\n");
  printf("\t%lld events analyzed\n", entries);
  printf("\t\t%lld clean events\n", cleanevs);
  printf("\t\t%lld preselected events\n", preselevs);
  printf("\t\t%lld tracks fitted\n", tracks);
  printf("\t\t%lld good tracks found\n", goodtracks);
  printf("\t\t%lld gold S tracks found\n", goldStracks);
  printf("\t\t%lld gold K tracks found\n", goldKtracks);
  printf("\t\t%lld gold tracks found\n", goldtracks);
  printf("---------------------------------------------\n");
  
  foutput->Write();
  foutput->Close();
  
  return 0;
}
