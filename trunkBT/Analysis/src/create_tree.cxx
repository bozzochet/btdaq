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
  
  static constexpr int NJINF = Event::GetNJINF();
  static constexpr int NTDRS = Event::GetNTDRS();
  static constexpr int NVAS = Event::GetNVAS();
  static constexpr int NCHAVA = Event::GetNCHAVA();
  static constexpr int NADCS = Event::GetNADCS();

  Float_t charge2D_m;
  Int_t nhits2D[2];
  Float_t InterceptXY[2];
  Float_t InterceptXY_err[2];
  Float_t SlopeXY[2];
  Float_t SlopeXY_err[2];
  Float_t chiXY[3];
  
  
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

  TTree *T = new TTree("T","T");
  T->Branch("charge2D_m",&charge2D_m,"charge2D_m/F");
  T->Branch("InterceptXY",&InterceptXY,"InterceptXY/F");
  T->Branch("InterceptXY_err",&InterceptXY_err,"InterceptXY_err/F");
  T->Branch("SlopeXY",&SlopeXY,"SlopeXY/F");
  T->Branch("SlopeXY_err",&SlopeXY_err,"SlopeXY_err/F");
  T->Branch("chiXY",&chiXY,"chiXY/F");

  int NSTRIPSS=640;
  int NSTRIPSK=384;
  
  //---------------------------------------------------
  
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

    charge2D_m = -9999.;
    InterceptXY[0] = -9999.;
    InterceptXY[1] = -9999.;
    SlopeXY[0] = -9999. ;
    SlopeXY[1] = -9999. ;
    InterceptXY_err[0] = -9999.;
    InterceptXY_err[1] = -9999.;
    SlopeXY_err[0] = -9999. ;
    SlopeXY_err[1] = -9999. ;
    chiXY[0] = -9999.;
    chiXY[1] = -9999.;
    chiXY[2] = -9999.;
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
    //and at most 100 (to avoid too much noise around and too much combinatorial)
    //at most 6 clusters per ladder (per side) + 0 additional clusters in total (per side)
    // ts->CleanEvent(ev, ut->GetRH(chain), 4, 100, 6, 6, 0, 0);
  
    bool cleanevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 9999, 9999, 9999, 9999, 9999);
    if (!cleanevent) continue;
    cleanevs++;//in this way this number is giving the "complete reasonable sample"
    
    bool preselevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 200, 50, 50, 0, 0);//ts->CleanEvent(ev, ut->GetRH(chain), 4, 100, 3, 3, 0, 0);//valid for TIC TB?
    if (!preselevent) continue;
    preselevs++;
    
    //at least 4 points on S, and 4 points on K, not verbose
    // ev->FindTrackAndFit(4, 4, false);
    
    bool trackfitok = ev->FindTrackAndFit(2,2,false);
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
  
    chiXY[0] = ev->GetChiTrackS();
    chiXY[1] = ev->GetChiTrackK();
    chiXY[2] = ev->GetChiTrack();

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
    nhits2D[0] = snhits;
    nhits2D[1] = knhits;
    
    // printf("S -> %d %d\n", snhits, strackok);
    // printf("K -> %d %d\n", knhits, ktrackok);

    if (strackok & ktrackok) goldtracks++;

    // printf("S %d %d %d %d -> %d %d\n", ev->IsTDRInTrack(0, 20)?1:0, ev->IsTDRInTrack(0, 22)?1:0, ev->IsTDRInTrack(0, 22)?1:0, ev->IsTDRInTrack(0, 23)?1:0, snhits, strackok);
    // printf("K %d %d %d %d -> %d %d\n", ev->IsTDRInTrack(1, 20)?1:0, ev->IsTDRInTrack(1, 22)?1:0, ev->IsTDRInTrack(1, 22)?1:0, ev->IsTDRInTrack(1, 23)?1:0, knhits, ktrackok);

    // if (ev->GetNHitsTrack()>5) {
    //   printf("Nhits: %u (S: %u, K: %u)\n", ev->GetNHitsTrack(), ev->GetNHitsSTrack(), ev->GetNHitsKTrack());
    // }

    //chi = log10(ev->GetChiTrack());
    SlopeXY[0] = ev->GetSlopeSTrack();
    SlopeXY_err[0] = ev->GetSlopeSerrTrack();
    SlopeXY[1] = ev->GetSlopeKTrack();
    SlopeXY_err[1] = ev->GetSlopeKerrTrack();
    InterceptXY[0]=ev->GetInterceptSTrack();
    InterceptXY_err[0]=ev->GetInterceptSerrTrack();
    InterceptXY[1]=ev->GetInterceptKTrack();
    InterceptXY_err[1]=ev->GetInterceptKerrTrack();
    
    std::vector<double> v_cog_laddS[NJINF*NTDRS];
     std::vector<double> v_cog_laddK[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddK[NJINF*NTDRS];
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
      
      cl = ev->GetCluster(index_cluster);
      
      int ladder = cl->ladder;
      //      printf("%d --> %d\n", ladder, ut->GetRH(chain)->FindPos(ladder));
      int side=cl->side;
      
      if (side==0) {
	v_cog_all_laddS[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
      else {
	v_cog_all_laddK[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
            
      if (!ev->IsClusterUsedInTrack(index_cluster)) continue;

      double charge=cl->GetCharge();//unused for now
      
      if (side==0) {
	if (strackok) {
	  v_cog_laddS[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	}
      }
      else {
	if (ktrackok) {
	  v_cog_laddK[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	}
      }
      
    }
  
    std::vector<std::pair<int, std::pair<int, int> > > vec_charge = ev->GetHitVector();

    charge2D_m = ev->GetChargeTrack(0);
    T->Fill();
    
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
  // printf("%d volte \n", fitfatto);
  printf("---------------------------------------------\n");
  //T->Write();
  foutput->Write();
  foutput->Close();
  
  return 0;
}
