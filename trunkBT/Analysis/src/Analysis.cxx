#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TMath.h"
#include "TPad.h"
#include "TROOT.h"
#include "TSpectrum.h"
#include "TStopwatch.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TTree.h"
#include <bitset>
#include <fstream>
#include <vector>
#include "TGraph.h"
#include "TGraphErrors.h"

/* from the 'Decode' API */
#include "GenericEvent.hpp"
#include "LadderConf.hh"
/* end */

/* from the CommonTool dir */
#include "TrackSelection.hpp"
#include "Utilities.hpp"
/* end */

using namespace std;

// raw value. To put the real distance from the most upward AMS ladder to the desired position in the target detector
#define ZTARGETDET 0

using EventOCA = GenericEvent<1, 24, 64, 5, 10, 0>;
using calibOCA = calib<EventOCA::GetNCHAVA() * EventOCA::GetNVAS()>;
using RHClassOCA = RHClass<EventOCA::GetNJINF(), EventOCA::GetNTDRS()>;
//---
using EventFOOT = GenericEvent<1, 24, 64, 5, 10, 0>;
using calibFOOT = calib<EventFOOT::GetNCHAVA() * EventFOOT::GetNVAS()>;
using RHClassFOOT = RHClass<EventFOOT::GetNJINF(), EventFOOT::GetNTDRS()>;
//---
using EventAMSL0 = GenericEvent<2, 9, 64, 8, 16, 0>;
using calibAMSL0 = calib<EventAMSL0::GetNCHAVA() * EventAMSL0::GetNVAS()>;
using RHClassAMSL0 = RHClass<EventAMSL0::GetNJINF(), EventAMSL0::GetNTDRS()>;
//---
using EventAMS = GenericEvent<1, 24, 64, 3, 16, 10>;
using calibAMS = calib<EventAMS::GetNCHAVA() * EventAMS::GetNVAS()>;
using RHClassAMS = RHClass<EventAMS::GetNJINF(), EventAMS::GetNTDRS()>;

template <class Event, class RH> int ProcessChain(TChain *ch, TString output_filename);
template <class Event, class RH> void BookHistos(TObjArray *histos, Long64_t entries, int _maxtdr, TChain *chain);
template <class Event, class RH> void FillAllHistos(TObjArray *histos, int NClusTot, Event *ev, int index_event);
template <class Event, class RH> void FillCleanHistos(TObjArray *histos, int NClusTot, Event *ev, int index_event, TFile *f);
template <class Event, class RH> void FillPreselHistos(TObjArray *histos, int NClusTot, Event *ev, int index_event);
template <class Event, class RH>
void FillGoodHistos(TObjArray *histos, int NClusTot, Event *ev, int _maxtdr, TChain *chain, bool strackok,
                    bool ktrackok);
void NormalizePlots(TObjArray *histos, Long64_t cleanevs, Long64_t preselevs, Long64_t goodtracks, Long64_t goldStracks,
                    Long64_t goldKtracks);
void CleanEmptyHistos(TObjArray *histos);
//new
double ch[9][7]; // ch[va][ch]
double ch_err[9][7];
double y[9][6];
double y_err[9][6];
const Int_t Size_z = 6;
TGraphErrors *va[9];
const Int_t nLadder = 11;
std::vector<TString> name_cog_maxCl;
std::vector<TString> title_cog_maxCl;
std::vector<TString> name_1half_sig_vs_eta_maxCl;
std::vector<TString> title_1half_sig_vs_eta_maxCl;
std::vector<TString> name_2half_sig_vs_eta_maxCl;
std::vector<TString> title_2half_sig_vs_eta_maxCl;
std::vector<TString> name_TotSig_vs_cog;
std::vector<TString> title_TotSig_vs_cog;
std::vector<TString> name_TotSig_vs_eta_maxCl;
std::vector<TString> title_TotSig_vs_eta_maxCl;
std::vector<TString> name_TotSig_vs_eta_AllCl;
std::vector<TString> title_TotSig_vs_eta_AllCl;
std::vector<TString> name_1stStrip_vs_eta_maxCl;
std::vector<TString> title_1stStip_vs_eta_maxCl;
std::vector<TString> name_2ndStrip_vs_eta_maxCl;
std::vector<TString> title_2ndStip_vs_eta_maxCl;
std::vector<TString> name_3rdStrip_vs_eta_maxCl;
std::vector<TString> title_3rdStip_vs_eta_maxCl;
std::vector<TString> name_4thStrip_vs_eta_maxCl;
std::vector<TString> title_4thStip_vs_eta_maxCl;
std::vector<TString> name_ADC_vs_Strips_maxCl;
std::vector<TString> title_ADC_vs_Strips_maxCl;
std::vector<TString> name_strip1_vs_strip2_maxCl;
std::vector<TString> title_strip1_vs_strip2_maxCl;
std::vector<TString> name_strip1_vs_strip2_All;
std::vector<TString> title_strip1_vs_strip2_All;
std::vector<TString> name_strip1_All;
std::vector<TString> title_strip1_All;
std::vector<TString> name_strip2_All;
std::vector<TString> title_strip2_All;
std::vector<TString> name_st1over2m1_All;
std::vector<TString> title_st1over2m1_All;
std::vector<TString> name_sigMin1_vs_eta23_All;
std::vector<TString> title_sigMin1_vs_eta23_All;
std::vector<TString> name_2vs3_All;
std::vector<TString> title_2vs3_All;
std::vector<TString> name_1vs3_All;
std::vector<TString> title_1vs3_All;
std::vector<TString> name_2vs4_All;
std::vector<TString> title_2vs4_All;
std::vector<TString> name_1vs3Corr_All;
std::vector<TString> title_1vs3Corr_All;
std::vector<TString> name_1vs2vsEta_All;
std::vector<TString> title_1vs2vsEta_All;
std::vector<TString> name_2vsEta_All;
std::vector<TString> title_2vsEta_All;

std::vector<TString> name_1vsEta12_All;
std::vector<TString> title_1vsEta12_All;

std::vector<TString> name_2vsEta23_All;
std::vector<TString> title_2vsEta23_All;

std::vector<TString> name_3vsEta34_All;
std::vector<TString> title_3vsEta34_All;

std::vector<TString> name_4vsEta45_All;
std::vector<TString> title_4vsEta45_All;

std::vector<TString> name_2vsEta23Corr_All;
std::vector<TString> title_2vsEta23Corr_All;

std::vector<TString> name_1And2vsEta_All;
std::vector<TString> title_1And2vsEta_All;

std::vector<TString> name_2vsEta12_After6000;
std::vector<TString> title_2vsEta12_After6000;

std::vector<TString> name_2vsEta12_Before6000;
std::vector<TString> title_2vsEta12_Before6000;

std::vector<TString> name_2vsEta23_After6000;
std::vector<TString> title_2vsEta23_After6000;

std::vector<TString> name_2vsEta23_Before6000;
std::vector<TString> title_2vsEta23_Before6000;

std::vector<TString> name_1vsEta23;
std::vector<TString> title_1vsEta23;

std::vector<TString> name_12vsEta23;
std::vector<TString> title_12vsEta23;

std::vector<TString> name_123vsEta34;
std::vector<TString> title_123vsEta34;

std::vector<TString> name_1vsEta34;
std::vector<TString> title_1vsEta34;

std::vector<TString> name_1vsEta45;
std::vector<TString> title_1vsEta45;






//----------------------------------------

void CreateTH3(TObjArray &hist, std::vector<TString> &vecName, std::vector<TString> &vecTitle  , TString name, TString title, int nxbin, int xmin, int xmax,int nybin, int ymin, int ymax, int nzbin,
	int zmin, int zmax) {
	for (int ii=0; ii<nLadder; ii++) {
  		vecTitle.push_back(Form("%d_%s", ii, title.Data() ));
    	vecName.push_back(Form("%d_%s", ii, name.Data() ));
    	TH3F *h = new TH3F (vecName[ii].Data(), vecTitle[ii].Data(), nxbin, xmin, xmax, nybin, ymin, ymax, nzbin, zmin, zmax);
    	hist.Add(h);
    	h->Reset();
  	}
}


void CreateTH2(TObjArray &hist, std::vector<TString> &vecName, std::vector<TString> &vecTitle  , TString name, TString title, int nxbin, int xmin, int xmax,int nybin, int ymin, int ymax) {
	for (int ii=0; ii<nLadder; ii++) {
  		vecTitle.push_back(Form("%d_%s", ii, title.Data() ));
    	vecName.push_back(Form("%d_%s", ii, name.Data() ));
    	TH2F *h = new TH2F (vecName[ii].Data(), vecTitle[ii].Data(), nxbin, xmin, xmax, nybin, ymin, ymax);
    	hist.Add(h);
    	h->Reset();
  	}
}

void CreateTH1(TObjArray &hist, std::vector<TString> &vecName, std::vector<TString> &vecTitle , TString name, TString title, int nbin, int xmin, int xmax) {
	for (int ii=0; ii<nLadder; ii++) {
		vecTitle.push_back(Form("%d_%s", ii, title.Data() ));
	    vecName.push_back(Form("%d_%s",ii, name.Data() ));
    	TH1F *h = new TH1F (vecName[ii].Data(), vecTitle[ii].Data(), nbin, xmin, xmax);
    	hist.Add(h);
    	h->Reset();
  	}
}


//new, added by Alessio
int cLen = 30;

int main(int argc, char *argv[]) {

  if (argc < 3) {
    printf("Usage:\n");
    printf("%s <output root-filename> <first input root-filename> [second input root-filename] ...\n", argv[0]);
    return 1;
  }

  bool kOca = false;
  bool kFoot = false;
  bool kAMSL0 = true;

  TString exename = argv[0];

  if (exename.Contains("OCA")) {
    kOca = true;
  } else if (exename.Contains("AMSL0")) {
    kAMSL0 = true;
  } else if (exename.Contains("FOOT")) {
    kFoot = true;
  }

  TChain *chain = new TChain("t4");

  for (int ii = 2; ii < argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }

  TString output_filename = argv[1];

  printf("---------------------------------------------\n");
  if (kOca) {
    EventOCA::ReadAlignment("alignment_OCA.dat");
    EventOCA::ReadGainCorrection("gaincorrection_OCA.dat");
    return ProcessChain<EventOCA, RHClassOCA>(chain, output_filename);
  } else if (kFoot) {
    EventFOOT::ReadAlignment("alignment_FOOT.dat");
    EventFOOT::ReadGainCorrection("gaincorrection_FOOT.dat");
    return ProcessChain<EventFOOT, RHClassFOOT>(chain, output_filename);
  } else if (kAMSL0) {
    EventAMSL0::ReadAlignment("alignment_L0.dat");
    EventAMSL0::ReadGainCorrection("gaincorrection_L0.dat");
    //new, added by Alessio
    //Cluster::SetMip(260,260);
    return ProcessChain<EventAMSL0, RHClassFOOT>(chain, output_filename);
  } else {
    EventAMS::ReadAlignment("alignment.dat");
    EventAMS::ReadGainCorrection("gaincorrection.dat");
    return ProcessChain<EventAMS, RHClassAMS>(chain, output_filename);
  }
  printf("---------------------------------------------\n");

  return 0;
}

template <class Event, class RH> int ProcessChain(TChain *chain, TString output_filename) {

  using TS = TrackSelection<Event, RH>;
  TS *ts = new TS();
  using UT = Utilities<Event, RH>;
  UT *ut = new UT();

  static constexpr int NJINF = Event::GetNJINF();
  static constexpr int NTDRS = Event::GetNTDRS();
  static constexpr int NVAS = Event::GetNVAS();
  static constexpr int NCHAVA = Event::GetNCHAVA();
  static constexpr int NADCS = Event::GetNADCS();

  Event *ev = NULL;
  //  Cluster *cl;

  Long64_t entries = chain->GetEntries();
  //  entries=10000;
  printf("This run has %lld entries\n", entries);

  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);
  printf("ev->GetEventKind() = %d\n", ev->GetEventKind());
  LadderConf *ladderconf = ut->GetLadderConf(chain);

  int _maxtdr = NJINF * NTDRS;

  if (ut->GetRH(chain)) {
    ut->GetRH(chain)->Print();
    _maxtdr = ut->GetRH(chain)->GetNTdrs();
  } else {
    printf("Not able to find the RHClass header in the UserInfo...\n");
    return -9;
  }
  //  printf("%d\n", _maxladd);

  // for (int tt=0; tt<_maxtdr; tt++) {
  //   printf("%d %d %f %f\n", tt, ut->GetRH(chain)->GetTdrNum(tt), Cluster::GetPitch(0,
  //   ut->GetRH(chain)->GetTdrNum(tt), 0), Cluster::GetPitch(0, ut->GetRH(chain)->GetTdrNum(tt), 1));
  // }

  TFile *foutput = new TFile(output_filename.Data(), "RECREATE");
  
  //--------------------------------------------------------------------------------------------------------------

  TObjArray *histos = new TObjArray();
  BookHistos<Event, RH>(histos, entries, _maxtdr, chain);

  //---------------------------------------------------

  Long64_t cleanevs = 0;
  Long64_t preselevs = 0;
  Long64_t tracks = 0;
  Long64_t goodtracks = 0;
  Long64_t goldtracks = 0;
  Long64_t goldStracks = 0;
  Long64_t goldKtracks = 0;

  TStopwatch sw;
  sw.Start();

  double perc = 0;

  // valid for TIC BT?
  //  ts->ExcludeTDR(ev, 0, 2, 0);
  //  ts->ExcludeTDR(ev, 0, 2, 1);
  //  ts->ExcludeTDR(ev, 0, 3, 0);
  //  ts->ExcludeTDR(ev, 0, 3, 1);
  //  ts->ExcludeTDR(ev, 0, 6, 0);
  //  ts->ExcludeTDR(ev, 0, 6, 1);
  //  ts->ExcludeTDR(ev, 0, 7, 0);
  //  ts->ExcludeTDR(ev, 0, 7, 1);
  // valid for 2021 SPS HERD BT
  //  ts->ExcludeTDR(ev, 0, 8, 0);
  //  ts->ExcludeTDR(ev, 0, 8, 1);
  //  ts->ExcludeTDR(ev, 0, 9, 0);
  //  ts->ExcludeTDR(ev, 0, 9, 1);
  //  ts->ExcludeTDR(ev, 0, 10, 0);
  //  ts->ExcludeTDR(ev, 0, 10, 1);
  //  ts->ExcludeTDR(ev, 0, 11, 0);
  //  ts->ExcludeTDR(ev, 0, 11, 1);
  //  ts->ExcludeTDR(ev, 0, 14, 0);
  //  ts->ExcludeTDR(ev, 0, 14, 1);
  //  ts->ExcludeTDR(ev, 0, 15, 0);
  //  ts->ExcludeTDR(ev, 0, 15, 1);
  //  ts->ExcludeTDR(ev, 0, 18, 0);
  //  ts->ExcludeTDR(ev, 0, 18, 1);
  //  ts->ExcludeTDR(ev, 0, 19, 0);
  //  ts->ExcludeTDR(ev, 0, 19, 1);
  //  ts->ExcludeTDR(ev, 0, 20, 0);
  //  ts->ExcludeTDR(ev, 0, 20, 1);
  //  ts->ExcludeTDR(ev, 0, 21, 0);
  //  ts->ExcludeTDR(ev, 0, 21, 1);
  //  ts->ExcludeTDR(ev, 0, 22, 0);
  //  ts->ExcludeTDR(ev, 0, 22, 1);
  //  ts->ExcludeTDR(ev, 0, 23, 0);
  //  ts->ExcludeTDR(ev, 0, 23, 1);


  for (int index_event = 0; index_event < entries; index_event++) {
    Double_t pperc = 100.0 * ((index_event + 1.0) / entries);
    if (pperc >= perc) {
      printf("\rProcessed %d out of %lld: %d%%", (index_event + 1), entries,
             (int)(100.0 * (index_event + 1.0) / entries));
      std::cout << std::flush;
      perc++;
    }
    //    printf("----- new event %d/%lld = %d%%\n", index_event, entries, (int)(100.0*(index_event+1.0)/entries));
    chain->GetEntry(index_event);
    // std::cout << "TimeStamp: " << ev->GetTimeStamp() << "." << ev->GetTimeStamp_ns() << "   Board clock: " <<
    // ev->GetBoardClock() << '\n';

    int NClusTot = ev->GetNClusTot();
    //    printf("\t\tnclusters = %d\n", NClusTot);

    //FillAllHistos<Event, RH>(histos, NClusTot, ev, index_event);

    // at least 4 clusters (if we want 2 on S and 2 on K this is really the sindacal minimum...)
    // and at most 100 (to avoid too much noise around and too much combinatorial)
    // at most 6 clusters per ladder (per side) + 0 additional clusters in total (per side)
    //  ts->CleanEvent(ev, ut->GetRH(chain), 4, 100, 6, 6, 0, 0);

    bool cleanevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 9999, 9999, 9999, 9999, 9999);
    if (!cleanevent)
      continue;
    cleanevs++; // in this way this number is giving the "complete reasonable sample"

    FillCleanHistos<Event, RH>(histos, NClusTot, ev, index_event, foutput);

    //    bool preselevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 100, 3, 3, 0, 0);//valid for TIC TB?
    bool preselevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 100, 10, 10, 0, 0);
    if (!preselevent)
      continue;
    preselevs++;

   // FillPreselHistos<Event, RH>(histos, NClusTot, ev, index_event);

    // at least 4 points on S, and 4 points on K, not verbose
    //  ev->FindTrackAndFit(4, 4, false);

    bool trackfitok = ev->FindTrackAndFit(2, 2, false); // later on we fill histos only for views with a gold<view>track
    //    printf("%d\n", trackfitok);
    if (trackfitok) {
      //      ev->RefineTrack(999999.0, 2, 999999.0, 2);
      // remove from the best fit track the worst hit if giving a residual greater than 6.0 sigmas on S and 6.0 sigmas
      // on K (but only if removing them still we'll have more or equal than 3 hits on S and 3 (2 if 'HigherCharge')
      // hits on K) and perform the fit again
      int minSclus = 3;
      int minKclus = 3;
      for (int ii = 0; ii < _maxtdr - std::min(minSclus, minKclus); ii++) {
        ev->RefineTrack(6.0, minSclus, 6.0, minKclus);
      }
      //      ev->RefineTrack(1.0, 2, 1.0, 2);
    } else {
      // let's downscale to 2 (on S) and 2 (on K) hits but even in this no-chisq case
      // let's garantee a certain relaiability of the track fitting the one with the higher charge (this method can be
      // used also for ions) and requiring an higher S/N for the cluster
      trackfitok = ev->FindHigherChargeTrackAndFit(2, 5.0, 2, 5.0, false);
    }
    if (!trackfitok)
      continue;
    //    printf("%f %f %f %f %f\n", ev->GetChiTrack(), ev->GetThetaTrack(), ev->GetPhiTrack(), ev->GetS0Track(),
    //    ev->GetS0Track());
    tracks++;

    //    printf("S %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(0), ev->IsTDRInTrack(0, 0), ev->IsTDRInTrack(0,
    //    4), ev->IsTDRInTrack(0, 8), ev->IsTDRInTrack(0, 12), ev->IsTDRInTrack(0, 14)); printf("K %024lld: %d %d %d %d
    //    %d\n", ev->GetTrackHitPattern(1), ev->IsTDRInTrack(1, 0), ev->IsTDRInTrack(1, 4), ev->IsTDRInTrack(1, 8),
    //    ev->IsTDRInTrack(1, 12), ev->IsTDRInTrack(1, 14));

    // //                              321098765432109876543210
    // if (ev->GetTrackHitPattern(0) <                100010001) continue;
    // if (ev->GetTrackHitPattern(1) <                100010001) continue;

    // printf("S %s\n", std::bitset<NTDRS>(ev->GetTrackHitPattern(0)).to_string().c_str());
    // printf("K %s\n", std::bitset<NTDRS>(ev->GetTrackHitPattern(1)).to_string().c_str());

    // printf("S %d %d %d %d\n", ev->IsTDRInTrack(0, 20)?1:0, ev->IsTDRInTrack(0, 22)?1:0, ev->IsTDRInTrack(0, 22)?1:0,
    // ev->IsTDRInTrack(0, 23)?1:0); printf("K %d %d %d %d\n", ev->IsTDRInTrack(1, 20)?1:0, ev->IsTDRInTrack(1, 22)?1:0,
    // ev->IsTDRInTrack(1, 22)?1:0, ev->IsTDRInTrack(1, 23)?1:0);

    double logchi = log10(ev->GetChiTrack());
    if (logchi > 10)
      continue;
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
    int snhits = ev->GetNHitsSTrack();
    if (snhits >= 3) {
      strackok = true;
      goldStracks++;
    }

    int knhits = ev->GetNHitsKTrack();
    if (knhits >= 3) {
      ktrackok = true;
      goldKtracks++;
    }

    // printf("S -> %d %d\n", snhits, strackok);
    // printf("K -> %d %d\n", knhits, ktrackok);

    if (strackok & ktrackok)
      goldtracks++;

    // printf("S %d %d %d %d -> %d %d\n", ev->IsTDRInTrack(0, 20)?1:0, ev->IsTDRInTrack(0, 22)?1:0, ev->IsTDRInTrack(0,
    // 22)?1:0, ev->IsTDRInTrack(0, 23)?1:0, snhits, strackok); printf("K %d %d %d %d -> %d %d\n", ev->IsTDRInTrack(1,
    // 20)?1:0, ev->IsTDRInTrack(1, 22)?1:0, ev->IsTDRInTrack(1, 22)?1:0, ev->IsTDRInTrack(1, 23)?1:0, knhits,
    // ktrackok);

    // if (ev->GetNHitsTrack()>5) {
    //   printf("Nhits: %u (S: %u, K: %u)\n", ev->GetNHitsTrack(), ev->GetNHitsSTrack(), ev->GetNHitsKTrack());
    // }

    //FillGoodHistos<Event, RH>(histos, NClusTot, ev, _maxtdr, chain, strackok, ktrackok);

    //    printf(" \n ");
  }
  std::cout << '\n';

  NormalizePlots(histos, cleanevs, preselevs, goodtracks, goldStracks, goldKtracks);
  
  CleanEmptyHistos(histos);

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

template <class Event, class RH> void BookHistos(TObjArray *histos, Long64_t entries, int _maxtdr, TChain *chain) {
  
  //new
  fstream file;
  file.open("/home/alessio/ams/macros/fitValueVA5to13.txt", ios::in);

  //read fit values
  for (int i=0; i<9; i++)
    for (int k=0; k<7; k++)
      file >> ch[i][k] >> ch_err[i][k];
  file.close();

  //creates ratios for every VA with respect to VA 10, except for Z=1 (kk=0)
  for (int ii=0; ii<9; ii++)
    for (int kk=0; kk<7; kk++)
      for (int jj=0; jj<6; jj++) {
            y[ii][jj] = ch[5][jj+1]/ch[ii][jj+1];
            y_err[ii][jj] = ch_err[5][jj+1]/ch[ii][jj+1];
      }

  //ridimensiona ch[ii]
  double x[9][6],x_err[9][6];
  for (int ii=0; ii<9; ii++)
    for (int kk=0; kk<7; kk++)
      for (int jj=0; jj<6; jj++) {
          x[ii][jj] = ch[ii][jj+1];
          x_err[ii][jj] = ch_err[ii][jj+1];
        }

  //create the gain curve for every VA (TGraph)
  for (int ii=0; ii<9; ii++) {
    va[ii] = new TGraphErrors (Size_z,x[ii],y[ii],x_err[ii],y_err[ii]);
  }


  using UT = Utilities<Event, RH>;
  UT *ut = new UT();

  static constexpr int NJINF = Event::GetNJINF();
  static constexpr int NTDRS = Event::GetNTDRS();
  static constexpr int NVAS = Event::GetNVAS();
  static constexpr int NVASS = Event::GetNVASS();
  static constexpr int NVASK = Event::GetNVASK();
  static constexpr int NCHAVA = Event::GetNCHAVA();
  static constexpr int NADCS = Event::GetNADCS();

  // int NSTRIPSS=Cluster::GetNChannels(0);
  // int NSTRIPSK=Cluster::GetNChannels(1);
  int NSTRIPSS = NCHAVA * NVASS;
  int NSTRIPSK = NCHAVA * NVASK;

  //for BT 10/2023 (11 ladder)
  
  CreateTH2(*histos, name_TotSig_vs_cog, title_TotSig_vs_cog , "TotSig_vs_cog", "TotSig_vs_cog; Cog; TotSig(ADC)", 1024, 0, 1024, 2000, -500, 15000);
  CreateTH2(*histos, name_TotSig_vs_eta_maxCl, title_TotSig_vs_eta_maxCl , "TotSig_vs_eta_maxCl", "TotSig_vs_eta_maxCl; eta; TotSig(ADC)", 1024, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_TotSig_vs_eta_AllCl, title_TotSig_vs_eta_AllCl , "TotSig_vs_eta_All", "TotSig_vs_eta_Alll; eta; TotSig(ADC)", 1024, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_1stStrip_vs_eta_maxCl, title_1stStip_vs_eta_maxCl , "1stStrip_vs_eta_maxCl", "1stStrip_vs_eta_maxCl; eta; 1stStrip(ADC)", 1024, 0, 1, 2000, 0, 20000);
  CreateTH2(*histos, name_2ndStrip_vs_eta_maxCl, title_2ndStip_vs_eta_maxCl , "2ndStrip_vs_eta_maxCl", "2ndStrip_vs_eta_maxCl; eta; 2ndStrip(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_2vsEta_All, title_2vsEta_All , "2vsEta_All", "2vsEta_All; eta; strip2(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_2vsEta23_All, title_2vsEta23_All , "2vsEta23_All", "2vsEta23_All; eta23; strip2(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_2vsEta23Corr_All, title_2vsEta23Corr_All , "2vsEta23Corr_All", "2vsEta23Corr_All; eta23; strip2(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_3rdStrip_vs_eta_maxCl, title_3rdStip_vs_eta_maxCl , "3rdStrip_vs_eta_maxCl", "3rdStrip_vs_eta_maxCl; eta; 3rdStrip(ADC)", 1024, 0, 1, 1500, 0, 5000);
  CreateTH2(*histos, name_4thStrip_vs_eta_maxCl, title_4thStip_vs_eta_maxCl , "4thStrip_vs_eta_maxCl", "4thStrip_vs_eta_maxCl; eta; 4thStrip(ADC)", 1024, 0, 1, 1500, 0, 5000);
  CreateTH2(*histos, name_ADC_vs_Strips_maxCl, title_ADC_vs_Strips_maxCl, "ADC_vs_Strips_maxCl", "Strip/TotSig_vs_Strips_maxCl; #StripsOfCl; Ratio", 20, -0.5, 19.5, 1000, 0, 10);
  //CreateTH2(*histos, name_1half_sig_vs_eta_maxCl, title_1half_sig_vs_eta_maxCl, "1half_sig_vs_eta_maxCl", "1half_sig_vs_eta_maxCl; Eta; 1half_sig",1024,0,1,2000,0,12000);
  //CreateTH2(*histos, name_2half_sig_vs_eta_maxCl, title_2half_sig_vs_eta_maxCl, "2half_sig_vs_eta_maxCl", "2half_sig_vs_eta_maxCl; Eta; 2half_sig",1024,0,1,2000,0,12000);
  CreateTH2(*histos, name_strip1_vs_strip2_All, title_strip1_vs_strip2_All, "strip1_vs_strip2_All", "strip1_vs_strip2_All; strip1 (ADC); strip2 (ADC)", 2000,0,40000, 2000, 0, 20000);
  CreateTH2(*histos, name_strip1_vs_strip2_maxCl, title_strip1_vs_strip2_maxCl, "strip1_vs_strip2_maxCl", "strip1_vs_strip2_maxCl, strip1 (ADC); strip2 (ADC)", 2000,0,40000, 2000, 0, 20000);
  //CreateTH2(*histos, name_st1over2m1_All, title_st1over2m1_All, "st1over2m1_All", "st1over2m1_All; strip1 (ADC); ratio-1", 2000,0,40000, 2000, 0, 10000);
  CreateTH2(*histos, name_2vs3_All, title_2vs3_All, "2vs3_All", "2vs3_All; strip3 (ADC); strip2 (ADC)", 2000,0,12000, 2000, 0, 12000);
  CreateTH2(*histos, name_1vs3_All, title_1vs3_All, "1vs3_All", "1vs3_All; strip3 (ADC); strip1 (ADC)", 2000,0,12000, 2000, 0, 15000);
  CreateTH2(*histos, name_2vs4_All, title_2vs4_All, "2vs4_All", "2vs4_All; strip4 (ADC); strip2 (ADC)", 2000,0,12000, 2000, 0, 12000);
  //CreateTH2(*histos, name_1vs3Corr_All, title_1vs3Corr_All, "1vs3Corr_All", "1vs3Corr_All; s3Corr (ADC); s1Corr (ADC)", 2000,0,12000, 4000, 0, 30000);
  CreateTH2(*histos, name_1And2vsEta_All, title_1And2vsEta_All, "1And2vsEta_All", "1nd2vsEta_All; 1and2 (ADC); eta", 0,0,1, 3000, 0, 20000);
  CreateTH2(*histos, name_1vsEta12_All, title_1vsEta12_All , "1vsEta12_All", "1vsEta12_All; eta12; strip1(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_2vsEta12_After6000, title_2vsEta12_After6000 , "2vsEta12_After6000", "2vsEta12_After6000; eta12; strip2(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_2vsEta12_Before6000, title_2vsEta12_Before6000 , "2vsEta12_Before6000", "2vsEta12_Before6000; eta12; strip2(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_2vsEta23_After6000, title_2vsEta23_After6000 , "2vsEta23_After6000", "2vsEta23_After6000; eta23; strip2(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_2vsEta23_Before6000, title_2vsEta23_Before6000 , "2vsEta23_Before6000", "2vsEta23_Before6000; eta23; strip2(ADC)", 2000, 0, 1, 3000, -500, 20000);
  
  CreateTH2(*histos, name_3vsEta34_All, title_3vsEta34_All , "3vsEta34_All", "3vsEta34_All; eta34; strip3(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_4vsEta45_All, title_4vsEta45_All , "4vsEta45_All", "4vsEta45; eta45; strip4(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_1vsEta23, title_1vsEta23, "1vsEta23", "1vsEta23; eta23; strip1(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_1vsEta34, title_1vsEta34, "1vsEta34", "1vsEta34; eta34; strip1(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_1vsEta45, title_1vsEta45, "1vsEta45", "1vsEta45; eta45; strip1(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_12vsEta23, title_12vsEta23, "12vsEta23", "12vsEta23; eta23; strip1+2(ADC)", 2000, 0, 1, 3000, -500, 20000);
  CreateTH2(*histos, name_123vsEta34, title_123vsEta34, "123vsEta34", "123vsEta34; eta34; strip1+2+3(ADC)", 2500, 0, 1, 3500, -500, 30000);
  
  CreateTH1(*histos, name_cog_maxCl, title_cog_maxCl, "cog_maxCl", "Cog_maxCl; Cog; Entries", 1024, 0, 1024);
  CreateTH1(*histos, name_strip1_All, title_strip1_All, "strip1_All", "strip1_All; strip1 (ADC); Entries", 2000, 0,40000);
  CreateTH1(*histos, name_strip2_All, title_strip2_All, "strip2_All", "strip2_All; strip2 (ADC); Entries", 2000, 0,20000);
  //CreateTH3(*histos, name_1vs2vsEta_All, title_1vs2vsEta_All, "1vs2vsEta_All", "1vs2vsEta_All; strip1 (ADC); strip2 (ADC); Eta", 1500, 0, 12000, 1500, 0, 15000, 1000, 0, 1);



  // all events
  TH1F *hclus_vs_event = new TH1F("hclus_vs_event", "hclus_vs_event;Event Number;Clusters", entries, 0, entries);
  histos->Add(hclus_vs_event);
  TH1F *hclus = new TH1F("hclus", "hclus;Clusters;Entries", 1000, 0, 1000);
  histos->Add(hclus);
  TH2F *hclusSladd_vs_event = new TH2F("hclusSladd_vs_event", "hclusSladd_vs_event;Event Number;Ladder;Clusters",
                                       entries, 0, entries, NJINF * NTDRS, 0, NJINF * NTDRS);
  histos->Add(hclusSladd_vs_event);
  TH2F *hclusKladd_vs_event = new TH2F("hclusKladd_vs_event", "hclusKladd_vs_event;Event Number;Ladder;Clusters",
                                       entries, 0, entries, NJINF * NTDRS, 0, NJINF * NTDRS);
  histos->Add(hclusKladd_vs_event);

  //new, added by alessio:
  TH1F *hlength = new TH1F("hlength", "hlength; Cluster length; Entries",128,-0.5,128.5);
  histos->Add(hlength);
  TH1F *hcog = new TH1F("hcog", "hcog; Cog; Entries",1024,0,1024);
  histos->Add(hcog);
  TH1F *hTotSigCorr = new TH1F("hTotSigCorr","hTotSigCorr; TotSigCorr; Entries",5000,0,1000);
  histos->Add(hTotSigCorr);
  TH1F *hTotSig = new TH1F("hTotSig","hTotSig; TotSig; Entries",5000,0,1000);
  histos->Add(hTotSig);
  TH1F *hTotSigMaxCl = new TH1F("hTotSigMaxCl","hTotSigMaxCl; TotSig; Entries",2000,0,10000);
  histos->Add(hTotSigMaxCl);
  TH1F *heta = new TH1F("heta","heta; eta; Entries",250,0,1);
  histos->Add(heta);

  //TH2F *hTotSignal_vs_cog = new TH2F("hTotSignal_vs_cog", "hTotSignal_vs_cog (Cl with highest TotSig); Cog; TotSig(ADC)", NVAS * NCHAVA, 0, NVAS * NCHAVA, 4200, -100, 25000);

  TH2F *hTotSignal_vs_cog = new TH2F("hTotSignal_vs_cog", "hTotSignal_vs_cog (Cl with highest TotSig); Cog; TotSig(ADC)", NVAS * NCHAVA, 0, NVAS * NCHAVA, 4200, -100, 15000);
  histos->Add(hTotSignal_vs_cog);
  TH2F *hSeedSignal_vs_cog = new TH2F("hSeedSignal_vs_cog", "hSeedSignal_vs_cog (Cl with highest TotSig); Cog; SeedSig(ADC)", NVAS * NCHAVA, 0, NVAS * NCHAVA, 4200, -100, 15000);
  histos->Add(hSeedSignal_vs_cog);
  TH2F *hSecSignal_vs_cog = new TH2F("hSecSignal_vs_cog", "hSecSignal_vs_cog (Cl with highest TotSig); Cog; SecSig(ADC)", NVAS * NCHAVA, 0, NVAS * NCHAVA, 4200, -100, 15000);
  histos->Add(hSecSignal_vs_cog);
  TH2F *hTotSignalCorr_vs_cog = new TH2F("hTotSignalCorr_vs_cog", "hTotSignalCorr_vs_cog (Cl with highest TotSig);Cog; TotSig Corrected(ADC)", NVAS * NCHAVA, 0, NVAS * NCHAVA, 4200, -100, 15000);
  histos->Add(hTotSignalCorr_vs_cog);
  TH2F *hTotSigCorr_vs_cog_readout = new TH2F("hTotSigCorr_vs_cog_readout","hTotSigCorr_vs_cog_readout (max CL); cog; TotSig(ADC)",NVAS * NCHAVA, 0, NVAS * NCHAVA, 4200, -100, 15000);
  histos->Add(hTotSigCorr_vs_cog_readout);


  //TH2F *hTotSignalCorr_vs_cog = new TH2F("hTotSignalCorr_vs_cog", "hTotSignalCorr_vs_cog (Cl with highest TotSig); Cog (Length<=30); TotSig Corrected(ADC)", NVAS * NCHAVA, 0, NVAS * NCHAVA, 4200, -100, 15000);
  //histos->Add(hTotSignalCorr_vs_cog);

  TH2F *hTotSignal_vs_eta = new TH2F("hTotSignal_vs_eta", "hTotSignal_vs_eta (Cl with highest TotSig); Eta; TotSig(ADC)",250,0,1,4200,-100,30000);
  histos->Add(hTotSignal_vs_eta);
  TH2F *hSeedSignal_vs_eta = new TH2F("hSeedSignal_vs_eta", "hSeedSignal_vs_eta (Cl with highest TotSig); Eta; SeedSig(ADC)",250,0,1,4200,-100,15000);
  histos->Add(hSeedSignal_vs_eta);
  TH2F *hSecSignal_vs_eta = new TH2F("hSecSignal_vs_eta", "hSecSignal_vs_eta (Cl with highest TotSig); Eta; SecSig(ADC)",250,0,1,4200,-100,15000);
  histos->Add(hSecSignal_vs_eta);
  TH2F *hTotSignalCorr_vs_eta= new TH2F("hTotSignalCorr_vs_eta", "hTotSignalCorr_vs_eta(Cl with highest TotSig); Eta; TotSig Corrected(ADC)",250,0,1,4200,-100,40000);
  histos->Add(hTotSignalCorr_vs_eta);

  TH2F *htotQ_vs_cog = new TH2F("htotQ_vs_cog","htotQ_vs_cog (Cl with highest TotSig); Cog; TotCharge",NVAS * NCHAVA, 0, NVAS * NCHAVA,4200,0,15);
  histos->Add(htotQ_vs_cog);
  //TH2F *htotQCorr_vs_cog = new TH2F("htotQCorr_vs_cog","htotQCorr_vs_cog (Cl with highest TotSig); Cog (Length<=30); TotChargeCorr",10,0.5,10.5,2100,-100,15000);
  //histos->Add(htotQCorr_vs_cog);
  TH2F *htotQ_vs_eta = new TH2F("htotQ_vs_eta","htotQ_vs_eta (Cl with highest TotSig); Eta; TotCharge",500,0,1,4200,0,15);
  histos->Add(htotQ_vs_eta);
  TH2F *htotQCorr_vs_eta = new TH2F("htotQCorr_vs_eta","htotQCorr_vs_eta (Cl with highest TotSig); eta ; TotChargeCorr",500,0,1,4200,0,15);
  histos->Add(htotQCorr_vs_eta);
  //TH2F *sqrtTotSig_vs_eta = new TH2F("sqrtTotSig_vs_eta", "sqrtTotSig_vs_eta (Cl with highest TotSig); eta; sqrt(TotSig) (ADC) ",500,0,1,1500,0,500);
  TH2F *sqrtTotSig_vs_eta = new TH2F("sqrtTotSig_vs_eta", "sqrtTotSig_vs_eta (Cl with highest TotSig); eta; sqrt(TotSig) (ADC) ",500,0,1,250,0,500);
  histos->Add(sqrtTotSig_vs_eta);

  TH2F *htotQ_vs_eta_noCut = new TH2F("htotQ_vs_eta_noCut","htotQ_vs_eta_noCut; eta; TotCharge",500,0,1,4200,0,15);
  histos->Add(htotQ_vs_eta_noCut);
  TH2F *hlength_vs_cog = new TH2F("hlength_vs_cog", "hlength_vs_cog; cog; Cluster Length",1024,0,1024,128,0,128);
  histos->Add(hlength_vs_cog);
  TH2F *hlength_vs_totSig = new TH2F("hlength_vs_totSig","hlength_vs_totSig; totSig (ADC); Cl length",4200,0,15000,45,0,45);
  histos->Add(hlength_vs_totSig);
  TH2F *hlength_vs_seedSig = new TH2F("hlength_vs_seedSig","hlength_vs_seedSig; seedSig (ADC); Cl length",4200,0,15000,45,0,45);
  histos->Add(hlength_vs_seedSig);
  TH2F *hlength_vs_secSig = new TH2F("hlength_vs_secSig","hlength_vs_secSig; secSig (ADC); Cl length",4200,0,15000,45,0,45);
  histos->Add(hlength_vs_secSig);

  TH2F *hseedOsec_vs_eta = new TH2F("hseedOsec_vs_eta","hseedOsec_vs_eta; eta; Seed/Sec",500,0,1,250,0,200);
  histos->Add(hseedOsec_vs_eta);
  TH2F *hsecOthird_vs_eta = new TH2F("hsecOthird_vs_eta","hsecOthird_vs_eta; eta; Sec/Third",500,0,1,250,0,1000);
  histos->Add(hsecOthird_vs_eta);
  TH2F *hSigMinusSeed_vs_eta = new TH2F("hSigMinusSeed_vs_eta","hSigMinusSeed_vs_eta; eta; TotSig-Seed",500,0,1,250,0,10000);
  histos->Add(hSigMinusSeed_vs_eta);
  TH2F *hSigMinusSeed_Sec_vs_eta = new TH2F("SigMinusSeed_Sec_vs_eta","SigMinusSeed_Sec_vs_eta; eta; TotSig-Seed-Sec",500,0,1,5200,0,40000);
  histos->Add(hSigMinusSeed_Sec_vs_eta);
  TH2F *sqrt_vs_eta_corrected = new TH2F("sqrt_vs_eta_corrected","sqrt_vs_eta_corrected; eta; sqrt corrected",500,0,1,4200,0,500);
  histos->Add(sqrt_vs_eta_corrected);
  TH2F *sqrt_vs_cog_raw = new TH2F("sqrt_vs_cog_raw","sqrt_vs_cog_raw; cog; sqrt(adc)",1024,0,1024,4200,0,500);
  histos->Add(sqrt_vs_cog_raw);
  TH2F *sqrt_vs_cog_equalized = new TH2F("sqrt_vs_cog_equalized","sqrt_vs_cog_equalized;cog; sqrt equalied",1024,0,1024,4200,0,500);
  histos->Add(sqrt_vs_cog_equalized);
  TH2F *sqrt_vs_cog_corrected = new TH2F("sqrt_vs_cog_corrected","sqrt_vs_cog_corrected;cog; sqrt equalied",1024,0,1024,4200,0,500);
  histos->Add(sqrt_vs_cog_corrected);
  TH1F *sqrt_raw = new TH1F("sqrt_raw","sqrt_raw; sqrt(ADC); Entries",2000,0,500);
  histos->Add(sqrt_raw);
  TH1F *sqrt_corr = new TH1F("sqrt_corr","sqrt_corr; sqrt_corr(ADC); Entries",2000,0,500);
  histos->Add(sqrt_corr);
  TH1F *sqrt_eq= new TH1F("sqrt_eq","sqrt_eq; sqrt_eq(ADC); Entries",2000,0,500);
  histos->Add(sqrt_eq);


  //----------------------------------------------------------------------------

  // clean events
  TH1F *hclus_vs_event_clean =
      new TH1F("hclus_vs_event_clean", "hclus_vs_event;Event Number;Clusters", entries, 0, entries);
  histos->Add(hclus_vs_event_clean);
  TH1F *hclus_clean = new TH1F("hclus_clean", "hclus_clean;Clusters;Entries", 1000, 0, 1000);
  histos->Add(hclus_clean);
  TH1F *hclusSladd_clean =
      new TH1F("hclusSladd_clean", "hclusSladd_clean;Ladder;Clusters_{S}", 24, 0, 24); // total per ladder
  histos->Add(hclusSladd_clean);
  TH1F *hclusKladd_clean =
      new TH1F("hclusKladd_clean", "hclusKladd_clean;Ladder;Clusters_{K}", 24, 0, 24); // total per ladder
  histos->Add(hclusKladd_clean);
  TH2F *hclusSladd_vs_event_clean =
      new TH2F("hclusSladd_vs_event_clean", "hclusSladd_vs_event_clean;Event Number;Ladder;Clusters", entries, 0,
               entries, NJINF * NTDRS, 0, NJINF * NTDRS);
  histos->Add(hclusSladd_vs_event_clean);
  TH2F *hclusKladd_vs_event_clean =
      new TH2F("hclusKladd_vs_event_clean", "hclusKladd_vs_event_clean;Event Number;Ladder;Clusters", entries, 0,
               entries, NJINF * NTDRS, 0, NJINF * NTDRS);
  histos->Add(hclusKladd_vs_event_clean);

  //----------------------------------------------------------------------------

  // presel events
  TH1F *hclus_vs_event_presel =
      new TH1F("hclus_vs_event_presel", "hclus_vs_event;Event Number;Clusters", entries, 0, entries);
  histos->Add(hclus_vs_event_presel);
  TH1F *hclus_presel = new TH1F("hclus_presel", "hclus_presel;Clusters;Entries", 1000, 0, 1000);
  histos->Add(hclus_presel);
  TH1F *hclusSladd_presel =
      new TH1F("hclusSladd_presel", "hclusSladd_presel;Ladder;Clusters_{S}", 24, 0, 24); // total per ladder
  histos->Add(hclusSladd_presel);
  TH1F *hclusKladd_presel =
      new TH1F("hclusKladd_presel", "hclusKladd_presel;Ladder;Clusters_{K}", 24, 0, 24); // total per ladder
  histos->Add(hclusKladd_presel);
  TH2F *hclusSladd_vs_event_presel =
      new TH2F("hclusSladd_vs_event_presel", "hclusSladd_vs_event_presel;Event Number;Ladder;Clusters", entries, 0,
               entries, NJINF * NTDRS, 0, NJINF * NTDRS);
  histos->Add(hclusSladd_vs_event_presel);
  TH2F *hclusKladd_vs_event_presel =
      new TH2F("hclusKladd_vs_event_presel", "hclusKladd_vs_event_presel;Event Number;Ladder;Clusters", entries, 0,
               entries, NJINF * NTDRS, 0, NJINF * NTDRS);
  histos->Add(hclusKladd_vs_event_presel);

  //----------------------------------------------------------------------------

  // good tracks
  TH1F *chi_good = new TH1F("chi_good", "chi;log10(#chi^{2});Entries", 1000, -5, 10);
  histos->Add(chi_good);
  TH1F *theta_good = new TH1F("theta_good", "theta;#theta (rad);Entries", 10000, -1.0, 1.0);
  histos->Add(theta_good);
  TH1F *phi_good = new TH1F("phi_good", "phi;#phi (rad);Entries", 1000, -TMath::Pi(), TMath::Pi());
  histos->Add(phi_good);
  TH2F *thetaphi_good = new TH2F("thetaphi_good", "thetaphi;#theta (rad);#phi (rad);Entries", 10000, -1.0, 1.0, 1000,
                                 -TMath::Pi(), TMath::Pi());
  histos->Add(thetaphi_good);
  TH1F *X0_good = new TH1F("X0_good", "X0;X_{Z=0} (mm);Entries", 1000, -100, 100);
  histos->Add(X0_good);
  TH1F *Y0_good = new TH1F("Y0_good", "Y0;Y_{Z=0} (mm);Entries", 1000, -100, 100);
  histos->Add(Y0_good);
  TH2F *X0Y0_good = new TH2F("X0Y0_good", "X0Y0;X_{Z=0} (mm);Y_{Z=0} (mm);Entries", 1000, -100, 100, 1000, -100, 100);
  histos->Add(X0Y0_good);
  TH1F *X0TARGETDET_good = new TH1F("X0TARGETDET_good", "X0TARGETDET;X_{Z=TARGETDET} (mm);Entries", 1000, -100, 100);
  histos->Add(X0TARGETDET_good);
  TH1F *Y0TARGETDET_good = new TH1F("Y0TARGETDET_good", "Y0TARGETDET;Y_{Z=TARGETDET} (mm);Entries", 1000, -100, 100);
  histos->Add(Y0TARGETDET_good);
  TH2F *X0Y0TARGETDET_good =
      new TH2F("X0Y0TARGETDET_good", "X0Y0TARGETDET;X_{Z=TARGETDET} (mm);Y_{Z=TARGETDET} (mm);Entries", 1000, -100, 100,
               1000, -100, 100);
  histos->Add(X0Y0TARGETDET_good);

  TH1F *hclus_good = new TH1F("hclus_good", "hclus_good;Clusters;Entries", 1000, 0, 1000);
  histos->Add(hclus_good);

  // also for clusters not used in tracks
  TH1F *occupancy[NJINF * NTDRS];
  TH1F *occupancy_posX[NJINF * NTDRS];
  TH1F *occupancy_posY[NJINF * NTDRS];
  for (int tt = 0; tt < _maxtdr; tt++) {
    int jinfnum = 0;
    int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
    double pitchS = std::max(LadderConf::GetPitch(jinfnum, tdrnum, 0), 0.01);
    double pitchK = std::max(LadderConf::GetPitch(jinfnum, tdrnum, 1), 0.01);
    double pitchX = pitchS;
    double pitchY = pitchK;
    int nstripsX = NSTRIPSS;
    int nstripsY = NSTRIPSK;
    if (LadderConf::GetSideSwap(jinfnum, tdrnum)) {
      pitchX = pitchK;
      pitchY = pitchS;
      nstripsX = NSTRIPSK;
      nstripsY = NSTRIPSS;
    }
    //    printf("%d) %f %f -> %f %f\n", tdrnum, Cluster::GetPitch(jinfnum, tdrnum, 0), Cluster::GetPitch(jinfnum,
    //    tdrnum, 1), pitchS, pitchK); printf("%d %d\n", NSTRIPSS, NSTRIPSK);
    occupancy[tt] = new TH1F(Form("occupancy_0_%02d", tdrnum),
                             Form("occupancy_0_%02d;Channel number;Occupancy", tdrnum), 1024, 0, 1024);
    histos->Add(occupancy[tt]);
    if (nstripsX > 0) {
      occupancy_posX[tt] = new TH1F(Form("occupancy_posX_0_%02d", tdrnum),
                                    Form("occupancy_posX_0_%02d;Position_{X} (mm);Occupancy", tdrnum), 2 * nstripsX,
                                    -nstripsX * pitchX, nstripsX * pitchX);
      histos->Add(occupancy_posX[tt]);
    }
    if (nstripsY > 0) {
      occupancy_posY[tt] = new TH1F(Form("occupancy_posY_0_%02d", tdrnum),
                                    Form("occupancy_posY_0_%02d;Position_{Y} (mm);Occupancy", tdrnum), 2 * nstripsY,
                                    -nstripsY * pitchY, nstripsY * pitchY);
      histos->Add(occupancy_posY[tt]);
    }
  }
  TH1F *hclusSladd_good =
      new TH1F("hclusSladd_good", "hclusSladd_good;Ladder;Clusters_{S}", 24, 0, 24); // total per ladder
  histos->Add(hclusSladd_good);
  TH1F *hclusKladd_good =
      new TH1F("hclusKladd_good", "hclusKladd_good;Ladder;Clusters_{K}", 24, 0, 24); // total per ladder
  histos->Add(hclusKladd_good);
  TH1F *hclusSladd_good_ontrack =
      new TH1F("hclusSladd_good_ontrack", "hclusSladd_good_ontrack;Ladder;Clusters_{S,OnTrack}", 24, 0, 24);
  histos->Add(hclusSladd_good_ontrack);
  TH1F *hclusKladd_good_ontrack =
      new TH1F("hclusKladd_good_ontrack", "hclusKladd_good_ontrack;Ladder;Clusters_{K,OnTrack}", 24, 0, 24);
  histos->Add(hclusKladd_good_ontrack);

  // only for clusters used in tracks
  TH1F *occupancy_ontrack[NJINF * NTDRS];
  TH1F *occupancy_ontrack_posX[NJINF * NTDRS];
  TH1F *occupancy_ontrack_posY[NJINF * NTDRS];
  TH1F *chargeS[NJINF * NTDRS];
  TH1F *chargeK[NJINF * NTDRS];
  TH2F *charge2D[NJINF * NTDRS];
  TH1F *chargeS_ave;
  TH1F *chargeK_ave;
  TH2F *charge2D_ave;
  for (int tt = 0; tt < _maxtdr; tt++) {
    int jinfnum = 0;
    int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
    double pitchS = std::max(LadderConf::GetPitch(jinfnum, tdrnum, 0), 0.01);
    double pitchK = std::max(LadderConf::GetPitch(jinfnum, tdrnum, 1), 0.01);
    double pitchX = pitchS;
    double pitchY = pitchK;
    int nstripsX = NSTRIPSS;
    int nstripsY = NSTRIPSK;
    if (LadderConf::GetSideSwap(jinfnum, tdrnum)) {
      pitchX = pitchK;
      pitchY = pitchS;
      nstripsX = NSTRIPSK;
      nstripsY = NSTRIPSS;
    }
    occupancy_ontrack[tt] = new TH1F(Form("occupancy_ontrack_0_%02d", tdrnum),
                                     Form("occupancy_ontrack_0_%02d;Channel number;Occupancy", tdrnum), 1024, 0, 1024);
    histos->Add(occupancy_ontrack[tt]);
    if (nstripsX > 0) {
      occupancy_ontrack_posX[tt] = new TH1F(Form("occupancy_ontrack_posX_0_%02d", tdrnum),
                                            Form("occupancy_ontrack_posX_0_%02d;Position_{X} (mm);Occupancy", tdrnum),
                                            2 * nstripsX, -nstripsX * pitchX, nstripsX * pitchX);
      histos->Add(occupancy_ontrack_posX[tt]);
    }
    if (nstripsY > 0) {
      occupancy_ontrack_posY[tt] = new TH1F(Form("occupancy_ontrack_posY_0_%02d", tdrnum),
                                            Form("occupancy_ontrack_posY_0_%02d;Position_{Y} (mm);Occupancy", tdrnum),
                                            2 * nstripsY, -nstripsY * pitchY, nstripsY * pitchY);
      histos->Add(occupancy_ontrack_posY[tt]);
    }
    chargeS[tt] =
        new TH1F(Form("chargeS_0_%02d", tdrnum), Form("chargeS_0_%02d;Q_{S} (c.u.);Entries", tdrnum), 1000, 0, 100);
    histos->Add(chargeS[tt]);
    chargeK[tt] =
        new TH1F(Form("chargeK_0_%02d", tdrnum), Form("chargeK_0_%02d;Q_{K} (c.u.);Entries", tdrnum), 1000, 0, 100);
    histos->Add(chargeK[tt]);
    charge2D[tt] =
        new TH2F(Form("charge_0_%02d", tdrnum), Form("charge_0_%02d;Q_{S} (c.u.);Q_{K} (c.u.);Entries", tdrnum), 1000,
                 0, 100, 1000, 0, 100);
    histos->Add(charge2D[tt]);
  }
  chargeS_ave = new TH1F("chargeS", "chargeS;Q_{S} (c.u.);Entries", 1000, 0, 100);
  histos->Add(chargeS_ave);
  chargeK_ave = new TH1F("chargeK", "chargeK;Q_{K} (c.u.);Entries", 1000, 0, 100);
  histos->Add(chargeK_ave);
  charge2D_ave = new TH2F("charge", "charge;Q_{S} (c.u.);Q_{K} (c.u.);Entries", 1000, 0, 100, 1000, 0, 100);
  histos->Add(charge2D_ave);

  //----------------------------------------------------------------------------

  // gold tracks (S, K or S&K)
  TH1F *chi_gold = new TH1F("chi_gold", "chi;log10(#chi^{2});Entries", 1000, -5, 10);
  histos->Add(chi_gold);
  TH1F *theta_gold = new TH1F("theta_gold", "theta;#theta (rad);Entries", 10000, -1.0, 1.0);
  histos->Add(theta_gold);
  TH1F *phi_gold = new TH1F("phi_gold", "phi;#phi (rad);Entries", 1000, -TMath::Pi(), TMath::Pi());
  histos->Add(phi_gold);
  TH2F *thetaphi_gold = new TH2F("thetaphi_gold", "thetaphi;#theta (rad);#phi (rad);Entries", 10000, -1.0, 1.0, 1000,
                                 -TMath::Pi(), TMath::Pi());
  histos->Add(thetaphi_gold);
  TH1F *X0_gold = new TH1F("X0_gold", "X0;X_{Z=0} (mm);Entries", 1000, -100, 100);
  histos->Add(X0_gold);
  TH1F *Y0_gold = new TH1F("Y0_gold", "Y0;Y_{Z=0} (mm);Entries", 1000, -100, 100);
  histos->Add(Y0_gold);
  TH2F *X0Y0_gold = new TH2F("X0Y0_gold", "X0Y0;X_{Z=0} (mm);Y_{Z=0} (mm);Entries", 1000, -100, 100, 1000, -100, 100);
  histos->Add(X0Y0_gold);
  TH1F *X0TARGETDET_gold = new TH1F("X0TARGETDET_gold", "X0TARGETDET;X_{Z=TARGETDET} (mm);Entries", 1000, -100, 100);
  histos->Add(X0TARGETDET_gold);
  TH1F *Y0TARGETDET_gold = new TH1F("Y0TARGETDET_gold", "Y0TARGETDET;Y_{Z=TARGETDET} (mm);Entries", 1000, -100, 100);
  histos->Add(Y0TARGETDET_gold);
  TH2F *X0Y0TARGETDET_gold =
      new TH2F("X0Y0TARGETDET_gold", "X0Y0TARGETDET;X_{Z=TARGETDET} (mm);Y_{Z=TARGETDET} (mm);Entries", 1000, -100, 100,
               1000, -100, 100);
  histos->Add(X0Y0TARGETDET_gold);

  // gold tracks (S, K or S&K) and only for clusters used in tracks

  TH1F *hclusSladd_gold =
      new TH1F("hclusSladd_gold", "hclusSladd_gold;Ladder;Clusters_{S}", 24, 0, 24); // total per ladder
  histos->Add(hclusSladd_gold);
  TH1F *hclusKladd_gold =
      new TH1F("hclusKladd_gold", "hclusKladd_gold;Ladder;Clusters_{K}", 24, 0, 24); // total per ladder
  histos->Add(hclusKladd_gold);
  TH1F *hclusSladd_gold_ontrack =
      new TH1F("hclusSladd_gold_ontrack", "hclusSladd_gold_ontrack;Ladder;Clusters_{S,OnTrack}", 24, 0, 24);
  histos->Add(hclusSladd_gold_ontrack);
  TH1F *hclusKladd_gold_ontrack =
      new TH1F("hclusKladd_gold_ontrack", "hclusKladd_gold_ontrack;Ladder;Clusters_{K,OnTrack}", 24, 0, 24);
  histos->Add(hclusKladd_gold_ontrack);

  TH1F *residual_X[NJINF * NTDRS];
  TH1F *residual_Y[NJINF * NTDRS];
  for (int tt = 0; tt < _maxtdr; tt++) {
    int jinfnum = 0;
    int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
    double pitchS = std::max(LadderConf::GetPitch(jinfnum, tdrnum, 0), 0.01);
    double pitchK = std::max(LadderConf::GetPitch(jinfnum, tdrnum, 1), 0.01);
    double pitchX = pitchS;
    double pitchY = pitchK;
    int nstripsX = NSTRIPSS;
    int nstripsY = NSTRIPSK;
    if (LadderConf::GetSideSwap(jinfnum, tdrnum)) {
      pitchX = pitchK;
      pitchY = pitchS;
      nstripsX = NSTRIPSK;
      nstripsY = NSTRIPSS;
    }
    if (nstripsX > 0) {
      residual_X[tt] =
          new TH1F(Form("residual_X_0_%02d", tdrnum), Form("residual_X_0_%02d;Residual_{X} (mm);Entries", tdrnum),
                   2 * nstripsX, -float(nstripsX) / 100. * pitchX, float(nstripsX) / 100. * pitchX);
      histos->Add(residual_X[tt]);
    }
    if (nstripsY > 0) {
      residual_Y[tt] =
          new TH1F(Form("residual_Y_0_%02d", tdrnum), Form("residual_Y_0_%02d;Residual_{Y} (mm);Entries", tdrnum),
                   40 * nstripsY, -20 * float(nstripsY) / 100. * pitchY, 20 * float(nstripsY) / 100. * pitchY);
      histos->Add(residual_Y[tt]);
    }
  }

  //----------------------------------------------------------------------------

  return;
}

template <class Event, class RH> void FillAllHistos(TObjArray *histos, int NClusTot, Event *ev, int index_event) {



  if (ev->GetEventKind()==4){
  Cluster *cl;

  //new
  //TF1 *func = new TF1("func","3100/(-4381.65*x+4381.65*x*x+3100)",0,10000);
  TF1 *func_adc = new TF1("func_adc","449/(-495*x+498*x*x+449)",0,10000); //Z=2
  //TF1 *func_sqrt = new TF1("func_sqrt","21.19/(-12.63*x+12.7*x*x+21.19)",0,10000); //SQRT Z=2
  TF1 *func = new TF1("func","3100/(-4381.65*x+4381.65*x*x+3100)",0,10000);
  //TF1 *func2 = new TF1("func2","14.02-0.68*x+0.04*pow(x,2)-0.001*pow(x,3)+1.63e-05*pow(x,4)-1.22e-07*pow(x,5)+3.72e-10*pow(x,6)",0,10000);

  TH1 *hclus = (TH1 *)(histos->FindObject("hclus"));
  TH1 *hclus_vs_event = (TH1 *)(histos->FindObject("hclus_vs_event"));
  TH2 *hclusSladd_vs_event = (TH2 *)(histos->FindObject("hclusSladd_vs_event"));
  TH2 *hclusKladd_vs_event = (TH2 *)(histos->FindObject("hclusKladd_vs_event"));

  TH1 *hlength = (TH1 *)(histos->FindObject("hlength"));
  TH1 *hcog = (TH1 *)(histos->FindObject("hcog"));
  TH1 *hTotSigCorr = (TH1 *)(histos->FindObject("hTotSigCorr"));
  TH1 *hTotSigCorrEq = (TH1 *)(histos->FindObject("hTotSigCorrEq"));
  TH1 *hTotSig = (TH1 *)(histos->FindObject("hTotSig"));
  TH1 *hTotSigMaxCl =(TH1 *)(histos->FindObject("hTotSigMaxCl"));

  TH2 *htotQ_vs_eta_noCut = (TH2 *)(histos->FindObject("htotQ_vs_eta_noCut"));
  TH2 *hTotSignal_vs_cog = (TH2 *)(histos->FindObject("hTotSignal_vs_cog"));
  TH2 *hSeedSignal_vs_cog = (TH2 *)(histos->FindObject("hSeedSignal_vs_cog"));
  TH2 *hSecSignal_vs_cog = (TH2 *)(histos->FindObject("hSecSignal_vs_cog"));
  TH2 *hTotSignal_vs_eta = (TH2 *)(histos->FindObject("hTotSignal_vs_eta"));
  TH2 *hSeedSignal_vs_eta = (TH2 *)(histos->FindObject("hSeedSignal_vs_eta"));
  TH2 *hSecSignal_vs_eta = (TH2 *)(histos->FindObject("hSecSignal_vs_eta"));
  TH2 *hTotSignalCorr_vs_eta = (TH2 *)(histos->FindObject("hTotSignalCorr_vs_eta"));
  TH2 *htotQ_vs_cog = (TH2 *)(histos->FindObject("htotQ_vs_cog"));
  TH2 *htotQ_vs_eta = (TH2 *)(histos->FindObject("htotQ_vs_eta"));
  TH2 *htotQCorr_vs_eta = (TH2 *)(histos->FindObject("htotQCorr_vs_eta"));
  TH2 *sqrtTotSig_vs_eta = (TH2 *)(histos->FindObject("sqrtTotSig_vs_eta"));
  TH2 *hlength_vs_cog = (TH2 *)(histos->FindObject("hlength_vs_cog"));
  TH2 *hlength_vs_totSig = (TH2 *)(histos->FindObject("hlength_vs_totSig"));
  TH2 *hlength_vs_seedSig = (TH2 *)(histos->FindObject("hlength_vs_seedSig"));
  TH2 *hlength_vs_secSig = (TH2 *)(histos->FindObject("hlength_vs_secSig"));
  TH2 *hseedOsec_vs_eta = (TH2 *)(histos->FindObject("hseedOsec_vs_eta"));
  TH2 *hsecOthird_vs_eta = (TH2 *)(histos->FindObject("hsecOthird_vs_eta"));
  TH2 *hSigMinusSeed_vs_eta = (TH2 *)(histos->FindObject("hSigMinusSeed_vs_eta"));
  TH2 *hSigMinusSeed_Sec_vs_eta = (TH2 *)(histos->FindObject("hSigMinusSeed_Sec_vs_eta"));
  TH2 *hTotSignalCorr_vs_cog = (TH2 *)(histos->FindObject("hTotSignalCorr_vs_cog"));
  TH2 *hsqrtADC_vs_z = (TH2 *)(histos->FindObject("hsqrtADC_vs_z"));
  TH2 *hTotSigCorr_vs_cog_readout = (TH2 *)(histos->FindObject("hTotSigCorr_vs_cog_readout"));
  TH2 *sqrt_vs_cog_corrected = (TH2 *)(histos->FindObject("sqrt_vs_cog_corrected"));

  TH2 *sqrt_vs_eta_corrected = (TH2 *)(histos->FindObject("sqrt_vs_eta_corrected") );
  TH2 *sqrt_vs_cog_raw = (TH2 *)(histos->FindObject("sqrt_vs_cog_raw"));
  TH2 *sqrt_vs_cog_equalized = (TH2 *)(histos->FindObject("sqrt_vs_cog_equalized") );
  TH1 *sqrt_raw = (TH1F *)(histos->FindObject("sqrt_raw") );
  TH1 *sqrt_corr = (TH1F *)(histos->FindObject("sqrt_corr"));
  TH1 *sqrt_eq = (TH1F *)(histos->FindObject("sqrt_eq"));

  hclus->Fill(NClusTot);
  hclus_vs_event->Fill(index_event, NClusTot);
  //new, added by Alessio
  std::vector<float> totSignal;

  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {

    cl = ev->GetCluster(index_cluster);
    int ladder = cl->ladder;
    //      printf("%d --> %d\n", ladder, ut->GetRH(chain)->FindPos(ladder));
    int jinfnum = cl->GetJinf();
    int tdrnum = cl->GetTDR();
    int side = cl->side;
    //double charge = cl->GetCharge(); // unused for now
    //    printf("%d %d) %d %d\n", jinfnum, tdrnum, side, LadderConf::Instance()->GetSideSwap(jinfnum, tdrnum));

    /*if (side == 0 || (side == 1 && LadderConf::Instance()->GetSideSwap(jinfnum, tdrnum))) {
      hclusSladd_vs_event->Fill(index_event, ladder);
    } else {
      hclusKladd_vs_event->Fill(index_event, ladder);
    }*/

    int l = cl->GetLength();
    float totsig= cl->GetTotSig();

    hlength->Fill(l);
    //htotQ_vs_eta_noCut->Fill(cl->GetEta(),sqrt(cl->GetTotSig()));
    hlength_vs_cog->Fill(cl->GetCoG(),l);
    hlength_vs_totSig->Fill(totsig,l);
    hlength_vs_seedSig->Fill(cl->GetSeedVal(),l);
    hlength_vs_secSig->Fill(cl->GetSecVal(),l);
    hTotSigCorr->Fill(totsig*func_adc->Eval(cl->GetEta()));
    hTotSig->Fill(totsig);
    hlength->Fill(l);
    hcog->Fill(cl->GetCoG());
    //htotQ_vs_eta_noCut->Fill(cl->GetEta(),sqrt(cl->GetTotSig()));
    hlength_vs_cog->Fill(cl->GetCoG(),l);
    hlength_vs_totSig->Fill(cl->GetTotSig(),l);
    hlength_vs_seedSig->Fill(cl->GetSeedVal(),l);
    hlength_vs_secSig->Fill(cl->GetSecVal(),l);


    totSignal.push_back(cl->GetTotSig());
  }

  if (totSignal.size() > 0) {
    int max_index=0;
    float max = totSignal[0];
    for (int i=0; i<totSignal.size(); i++)
      if (totSignal[i]>max) {
        max = totSignal[i];
        max_index=i;
      }

    Cluster *clmax = ev->GetCluster(max_index);

    if (clmax->GetLength() >= 1) {
        std::vector<float> orderedSig = clmax->Sort();
        float eta = clmax->GetEta();
        float cog = clmax->GetCoG();
        float totSig = clmax->GetTotSig();
        float sqrt_adc = TMath::Sqrt(totSig);
        float seedSig = orderedSig[0];
        float secSig = orderedSig[1];
        float thirdSig = orderedSig[2];
        //double totCh = clmax->GetCharge();
        int len = clmax->GetLength();
        float corr_adc = func_adc->Eval(eta);
        float adcCorrected = totSig*corr_adc;
        int nVa = int(cog/64);
        float adcEq = adcCorrected;
        float sqrtCorrected = TMath::Sqrt(adcCorrected);

        if (nVa >=4 && nVa <= 12) {
          adcEq = adcCorrected*va[nVa-4]->Eval(adcCorrected);
          float sqrtEq = TMath::Sqrt(adcEq);
          sqrt_vs_cog_equalized->Fill(cog,sqrtEq);
        } else {
          sqrt_vs_cog_equalized->Fill(cog,sqrtCorrected);
        }

        hcog->Fill(clmax->GetCoG());

        if (eta <= 0.09 || eta >= 0.9) {
          hTotSigCorr_vs_cog_readout->Fill(cog, adcCorrected);
        }

        hTotSigMaxCl->Fill(totSig);
        hTotSignal_vs_cog->Fill(cog , totSig);
        hSeedSignal_vs_cog->Fill(cog , seedSig);
        hSecSignal_vs_cog->Fill(cog , secSig);
        hTotSignal_vs_eta->Fill(eta, totSig);
        //hSeedSignal_vs_eta->Fill(eta, seedSig);
        //hSecSignal_vs_eta->Fill(eta, secSig);
        hTotSignalCorr_vs_eta->Fill(eta, adcCorrected);

        //htotQ_vs_cog->Fill(cog, totCh);
        //htotQ_vs_eta->Fill(eta, totCh );
        //htotQCorr_vs_eta->Fill(eta, totCh*sqrt(func2->Eval(eta)) );

        sqrtTotSig_vs_eta->Fill(eta,sqrt(totSig));
        hTotSignalCorr_vs_cog->Fill(cog, adcCorrected );
        //hsqrtADC_vs_z->Fill(clmax->GetCharge(),sqrt(totSig));

        /*if (secSig != 0) {

          hseedOsec_vs_eta->Fill(eta,seedSig/secSig);

        }*/

        /*if (thirdSig != 0) {

          hsecOthird_vs_eta->Fill(eta,secSig/thirdSig);
        }
        float tot_seed = totSig-seedSig;
        float tot_seed_sec = totSig-seedSig - secSig;
        hSigMinusSeed_vs_eta->Fill(eta,tot_seed);*/
        //hSigMinusSeed_Sec_vs_eta->Fill(eta,tot_seed_sec);
        sqrt_vs_cog_corrected->Fill(cog,sqrtCorrected);
        sqrt_vs_eta_corrected->Fill(eta,sqrtCorrected);
        sqrt_vs_cog_raw->Fill(cog,sqrt_adc);
        sqrt_raw->Fill(sqrt_adc);
        sqrt_corr->Fill(sqrtCorrected);
        sqrt_eq->Fill(TMath::Sqrt(adcEq));

    }
  }
  delete func_adc;
  delete func;
  //delete func_sqrt;
  return;
}
}

template <class Event, class RH> void FillCleanHistos(TObjArray *histos, int NClusTot, Event *ev, int index_event, TFile *f) {

  Cluster *cl;

  /*TH1 *hclus_clean = (TH1 *)(histos->FindObject("hclus_clean"));
  TH1 *hclus_vs_event_clean = (TH1 *)(histos->FindObject("hclus_vs_event_clean"));
  TH1 *hclusSladd_clean = (TH1 *)(histos->FindObject("hclusSladd_clean"));
  TH1 *hclusKladd_clean = (TH1 *)(histos->FindObject("hclusKladd_clean"));
  TH2 *hclusSladd_vs_event_clean = (TH2 *)(histos->FindObject("hclusSladd_vs_event_clean"));
  TH2 *hclusKladd_vs_event_clean = (TH2 *)(histos->FindObject("hclusKladd_vs_event_clean"));*/

  TF1 *func_adc = new TF1("func_adc","6240.5/(-19950*x+19950*x*x+6240.5)",0,20000);

  //std::vector<TH3*> l1vs2vsEta_All;	
  	
  std::vector<TH2*> lTotSig_vs_cog;
  std::vector<TH2*> lTotSig_vs_eta_maxCl;
  std::vector<TH2*> lTotSig_vs_eta_AllCl;
  std::vector<TH2*> l1stStrip_vs_eta_maxCl;
  std::vector<TH2*> l2ndStrip_vs_eta_maxCl;
  std::vector<TH2*> l3rdStrip_vs_eta_maxCl;	
  std::vector<TH2*> l4thStrip_vs_eta_maxCl;
  std::vector<TH2*> lADC_vs_Strips_maxCl;
  //std::vector<TH2*> l1half_sig_vs_eta_maxCl;
  //std::vector<TH2*> l2half_sig_vs_eta_maxCl;
  std::vector<TH2*> l1strip1_vs_strip2_maxCl;
  std::vector<TH2*> l1strip1_vs_strip2_AllCl;
  //std::vector<TH2*> l1over2_All;
  std::vector<TH2*> l2vs3_All;
  std::vector<TH2*> l1vs3_All;
  std::vector<TH2*> l2vs4_All;
  //std::vector<TH2*> l1vs3Corr_All;
  std::vector<TH2*> l2vsEta_All;
  std::vector<TH2*> l1vsEta12_All;
  std::vector<TH2*> l2vsEta23_All;
  std::vector<TH2*> l3vsEta34_All;
  std::vector<TH2*> l4vsEta45_All;
  std::vector<TH2*> l2vsEta23Corr_All;
  std::vector<TH2*> l1And2vsEta_All;

  std::vector<TH2*> l2vsEta12_After6000;
  std::vector<TH2*> l2vsEta12_Before000;
  std::vector<TH2*> l2vsEta23_After6000;
  std::vector<TH2*> l2vsEta23_Before000;

  std::vector<TH2*> l1vsEta23;
  std::vector<TH2*> l12vsEta23;
  std::vector<TH2*> l123vsEta34;
  
  std::vector<TH1*> lcog_maxCl;		
  std::vector<TH1*> lstrip1_All;
  std::vector<TH1*> lstrip2_All;
  std::vector<TH2*> l1vsEta34;
  std::vector<TH2*> l1vsEta45;

  
  for (int ii=0; ii<nLadder; ii++) {
  	TH2 *h1 = (TH2 *)( histos->FindObject(name_TotSig_vs_cog[ii].Data()) );
  	TH2 *h2 = (TH2 *)( histos->FindObject(name_TotSig_vs_eta_maxCl[ii].Data()) );
  	TH2 *h3 = (TH2 *)( histos->FindObject(name_2ndStrip_vs_eta_maxCl[ii].Data()) );
  	TH2 *h4 = (TH2 *)( histos->FindObject(name_3rdStrip_vs_eta_maxCl[ii].Data()) );
  	TH2 *h5 = (TH2 *)( histos->FindObject(name_ADC_vs_Strips_maxCl[ii].Data()) );
  	TH2 *h6 = (TH2 *)( histos->FindObject(name_4thStrip_vs_eta_maxCl[ii].Data()) );
  	TH2 *h7 = (TH2 *)( histos->FindObject(name_1stStrip_vs_eta_maxCl[ii].Data()) );
  	TH1 *h8 = (TH1 *)( histos->FindObject(name_cog_maxCl[ii].Data()) );
  	//TH2 *h9 = (TH2 *)( histos->FindObject(name_1half_sig_vs_eta_maxCl[ii].Data()) );
  	//TH2 *h10 = (TH2 *)( histos->FindObject(name_2half_sig_vs_eta_maxCl[ii].Data()) );
  	TH2 *h11 = (TH2 *)( histos->FindObject(name_TotSig_vs_eta_AllCl[ii].Data()) );
	TH2 *h12 = (TH2 *)( histos->FindObject(name_strip1_vs_strip2_maxCl[ii].Data()) );
	TH2 *h13 = (TH2 *)( histos->FindObject(name_strip1_vs_strip2_All[ii].Data()) );
	TH1 *h14 = (TH1 *)( histos->FindObject(name_strip1_All[ii].Data()) );
	TH1 *h15 = (TH1 *)( histos->FindObject(name_strip2_All[ii].Data()) );
	//TH2 *h16 = (TH2 *)( histos->FindObject(name_st1over2m1_All[ii].Data()) );
	TH2 *h17 = (TH2 *)( histos->FindObject(name_2vs3_All[ii].Data()) );  
	TH2 *h18 = (TH2 *)( histos->FindObject(name_1vs3_All[ii].Data()) );  
	TH2 *h19 = (TH2 *)( histos->FindObject(name_2vs4_All[ii].Data()) );  
	//TH2 *h20 = (TH2 *)( histos->FindObject(name_1vs3Corr_All[ii].Data()) );  
	//TH3 *h21 = (TH3 *)( histos->FindObject(name_1vs2vsEta_All[ii].Data()) );
	TH2 *h22 = (TH2 *)( histos->FindObject(name_2vsEta_All[ii].Data()) );
	TH2 *h23 = (TH2 *)( histos->FindObject(name_2vsEta23_All[ii].Data()) );
	TH2 *h24 = (TH2 *)( histos->FindObject(name_2vsEta23Corr_All[ii].Data()) );
	TH2 *h25 = (TH2 *)( histos->FindObject(name_1And2vsEta_All[ii].Data()) );
	TH2 *h26 = (TH2 *)( histos->FindObject(name_1vsEta12_All[ii].Data()) );
	TH2 *h27 = (TH2 *)( histos->FindObject(name_3vsEta34_All[ii].Data()) );
	TH2 *h28 = (TH2 *)( histos->FindObject(name_4vsEta45_All[ii].Data()) );
	TH2 *h29 = (TH2 *)( histos->FindObject(name_2vsEta12_After6000[ii].Data()) );
	TH2 *h30 = (TH2 *)( histos->FindObject(name_2vsEta12_Before6000[ii].Data()) );
	TH2 *h31 = (TH2 *)( histos->FindObject(name_2vsEta23_After6000[ii].Data()) );
	TH2 *h32 = (TH2 *)( histos->FindObject(name_2vsEta23_Before6000[ii].Data()) );
	TH2 *h33 = (TH2 *)( histos->FindObject(name_1vsEta23[ii].Data())  );
	TH2 *h34 = (TH2 *)( histos->FindObject(name_12vsEta23[ii].Data())  );
	TH2 *h35 = (TH2 *)( histos->FindObject(name_123vsEta34[ii].Data())  );
	TH2 *h36 = (TH2 *)( histos->FindObject(name_1vsEta34[ii].Data())  );
	TH2 *h37 = (TH2 *)( histos->FindObject(name_1vsEta45[ii].Data())  );
	
  
  	lTotSig_vs_cog.push_back(h1);
  	lTotSig_vs_eta_maxCl.push_back(h2);
  	l2ndStrip_vs_eta_maxCl.push_back(h3);
  	l3rdStrip_vs_eta_maxCl.push_back(h4);
  	lADC_vs_Strips_maxCl.push_back(h5);
  	l4thStrip_vs_eta_maxCl.push_back(h6);
  	l1stStrip_vs_eta_maxCl.push_back(h7);
  	lcog_maxCl.push_back(h8);
  	//l1half_sig_vs_eta_maxCl.push_back(h9);
  	//l2half_sig_vs_eta_maxCl.push_back(h10);
  	lTotSig_vs_eta_AllCl.push_back(h11);
  	l1strip1_vs_strip2_maxCl.push_back(h12);
  	l1strip1_vs_strip2_AllCl.push_back(h13);
  	lstrip1_All.push_back(h14);
  	lstrip2_All.push_back(h15);
  	//l1over2_All.push_back(h16);
  	l2vs3_All.push_back(h17);
  	l1vs3_All.push_back(h18);
  	l2vs4_All.push_back(h19);
  	//l1vs3Corr_All.push_back(h20);
    //l1vs2vsEta_All.push_back(h21);
  	l2vsEta_All.push_back(h22);
  	l2vsEta23_All.push_back(h23);
  	l2vsEta23Corr_All.push_back(h24);
  	l1And2vsEta_All.push_back(h25);
  	l1vsEta12_All.push_back(h26);
  	l3vsEta34_All.push_back(h27);
  	l4vsEta45_All.push_back(h28);
  	l2vsEta12_After6000.push_back(h29);
  	l2vsEta12_Before000.push_back(h30);
  	l2vsEta23_After6000.push_back(h31);
  	l2vsEta23_Before000.push_back(h32);
  	l1vsEta23.push_back(h33);
  	l12vsEta23.push_back(h34);
  	l123vsEta34.push_back(h35);
  	l1vsEta34.push_back(h36);
  	l1vsEta45.push_back(h37);

  }

  std::vector<float> totSignal;
  
  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {

    cl = ev->GetCluster(index_cluster);
    std::vector<float> all_sorted_signals = cl->Sort();
    /*int ladder = cl->ladder;
    int side = cl->side;
    double charge = cl->GetCharge(); // unused for now */
    int Ladd = cl->laddNum();
  	if (Ladd >= 0 && Ladd <11) {	
  		float eta = cl->GetEta();
  		l2ndStrip_vs_eta_maxCl[Ladd]->Fill(eta, cl->GetSecVal() );
		lTotSig_vs_cog[Ladd]->Fill(cl->GetCoG(),cl->GetTotSig( ));
		lTotSig_vs_eta_AllCl[Ladd]->Fill(eta,cl->GetTotSig() );
		float strip1 = all_sorted_signals[0];
		float strip2 = all_sorted_signals[1];
		float strip3 = all_sorted_signals[2];
		float strip4 = all_sorted_signals[3];
		float strip5 = all_sorted_signals[4];
		float eta23 = cl->GetEtaBetween(strip2, strip3);	
		float eta34 = cl->GetEtaBetween(strip3, strip4);
		float eta45 = cl->GetEtaBetween(strip4, strip5);
		//float corr = func_adc->Eval(eta);
		//l1vs3Corr_All[Ladd]->Fill(strip3*corr, strip1*corr);
		//l1vs2vsEta_All[Ladd]->SetBinContent(strip1, strip2, eta, 1);
		l1vsEta34[Ladd]->Fill(eta34, strip1);
		l1vsEta45[Ladd]->Fill(eta45, strip1);
		l123vsEta34[Ladd]->Fill(eta34, strip1+strip2+strip3);
		l12vsEta23[Ladd]->Fill(eta23, strip1+strip2);
		l1vsEta23[Ladd]->Fill(eta23, strip1);
		if (strip1 >= 6000) {
			l2vsEta12_After6000[Ladd]->Fill(eta, strip2);	
			l2vsEta23_After6000[Ladd]->Fill(eta23, strip2);
		}
		if (strip1 < 6000) {
			l2vsEta12_Before000[Ladd]->Fill(eta, strip2);
			l2vsEta23_Before000[Ladd]->Fill(eta23, strip2);
		}
		l1vsEta12_All[Ladd]->Fill(eta, strip1);
		l3vsEta34_All[Ladd]->Fill(eta34, strip3);
		l4vsEta45_All[Ladd]->Fill(eta45, strip4);
		l2vsEta23Corr_All[Ladd]->Fill(eta23, strip2 * func_adc->Eval(eta23) );
		l2vsEta_All[Ladd]->Fill(eta,strip2);
		l2vsEta23_All[Ladd]->Fill(eta23,strip2);
		l1vs3_All[Ladd]->Fill(strip3, strip1);
		l2vs4_All[Ladd]->Fill(strip4, strip2);
		l2vs3_All[Ladd]->Fill(strip3, strip2);
		l1strip1_vs_strip2_AllCl[Ladd]->Fill(strip1, strip2);
		lstrip1_All[Ladd]->Fill(strip1);
		lstrip2_All[Ladd]->Fill(strip2);
   	}
   	
    totSignal.push_back(cl->GetTotSig());

    /*if (side == 0 || (side == 1 && LadderConf::Instance()->GetSideSwap(jinfnum, tdrnum))) {
      hclusSladd_clean->Fill(ladder);
      hclusSladd_vs_event_clean->Fill(index_event, ladder);
    } else {
      hclusKladd_clean->Fill(ladder);
      hclusKladd_vs_event_clean->Fill(index_event, ladder);
    }*/
    
 	}
	
  	if (totSignal.size() > 0) {
	    int max_index=0;
	    float max = totSignal[0];
	    for (int i=0; i<totSignal.size(); i++)
	      if (totSignal[i]>max) {
	        max = totSignal[i];
	        max_index=i;
	      }
	      
	  	Cluster *clmax = ev->GetCluster(max_index);
	  	std::vector<float> sorted_signals = clmax->Sort();
	  	if (clmax->GetLength() >= 1) {
	  	
	  		int Ladd = clmax->laddNum();
	  		  	if (Ladd >= 0 && Ladd <11) {
	  		  		float eta = clmax->GetEta();
	  		  		float tot = clmax->GetTotSig();
	  		  		float cog = clmax->GetCoG();
	  				lTotSig_vs_eta_maxCl[Ladd]->Fill(eta, tot );
	  				l1stStrip_vs_eta_maxCl[Ladd]->Fill(eta, sorted_signals[0] );
	  				l1strip1_vs_strip2_maxCl[Ladd]->Fill(sorted_signals[0], sorted_signals[1]);
	  				lcog_maxCl[Ladd]->Fill(clmax->GetCoG() );
	  				if (sorted_signals.size() > 4) {
	  					l4thStrip_vs_eta_maxCl[Ladd]->Fill(eta, sorted_signals[3] );
	  				}
	  				if (sorted_signals.size()>2 ) {
	  					l3rdStrip_vs_eta_maxCl[Ladd]->Fill(eta, sorted_signals[2] );
	  					for (int ii=0; ii<sorted_signals.size(); ii++) {
	  						lADC_vs_Strips_maxCl[Ladd]->Fill(ii, sorted_signals[ii]/tot );
	  					}
	  				}
	  		   	}
		}
	}
  

  return;
}

template <class Event, class RH> void FillPreselHistos(TObjArray *histos, int NClusTot, Event *ev, int index_event) {

  Cluster *cl;

  TH1 *hclus_presel = (TH1 *)(histos->FindObject("hclus_presel"));
  TH1 *hclus_vs_event_presel = (TH1 *)(histos->FindObject("hclus_vs_event_presel"));
  TH1 *hclusSladd_presel = (TH1 *)(histos->FindObject("hclusSladd_presel"));
  TH1 *hclusKladd_presel = (TH1 *)(histos->FindObject("hclusKladd_presel"));
  TH2 *hclusSladd_vs_event_presel = (TH2 *)(histos->FindObject("hclusSladd_vs_event_presel"));
  TH2 *hclusKladd_vs_event_presel = (TH2 *)(histos->FindObject("hclusKladd_vs_event_presel"));

  hclus_presel->Fill(NClusTot);
  hclus_vs_event_presel->Fill(index_event, NClusTot);

  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {

    cl = ev->GetCluster(index_cluster);
    int ladder = cl->ladder;
    //      printf("%d --> %d\n", ladder, ut->GetRH(chain)->FindPos(ladder));
    int jinfnum = cl->GetJinf();
    int tdrnum = cl->GetTDR();
    int side = cl->side;
    double charge = cl->GetCharge(); // unused for now

    if (side == 0 || (side == 1 && LadderConf::Instance()->GetSideSwap(jinfnum, tdrnum))) {
      hclusSladd_presel->Fill(ladder);
      hclusSladd_vs_event_presel->Fill(index_event, ladder);
    } else {
      hclusKladd_presel->Fill(ladder);
      hclusKladd_vs_event_presel->Fill(index_event, ladder);
    }
  }

  return;
}

template <class Event, class RH>
void FillGoodHistos(TObjArray *histos, int NClusTot, Event *ev, int _maxtdr, TChain *chain, bool strackok,
                    bool ktrackok) {

  Cluster *cl;

  using UT = Utilities<Event, RH>;
  UT *ut = new UT();

  static constexpr int NJINF = Event::GetNJINF();
  static constexpr int NTDRS = Event::GetNTDRS();
  static constexpr int NVAS = Event::GetNVAS();
  static constexpr int NCHAVA = Event::GetNCHAVA();
  static constexpr int NADCS = Event::GetNADCS();

  TH1 *chi_good = (TH1 *)(histos->FindObject("chi_good"));
  TH1 *theta_good = (TH1 *)(histos->FindObject("theta_good"));
  TH1 *phi_good = (TH1 *)(histos->FindObject("phi_good"));
  TH2 *thetaphi_good = (TH2 *)(histos->FindObject("thetaphi_good"));
  TH1 *X0_good = (TH1 *)(histos->FindObject("X0_good"));
  TH1 *Y0_good = (TH1 *)(histos->FindObject("Y0_good"));
  TH2 *X0Y0_good = (TH2 *)(histos->FindObject("X0Y0_good"));
  TH1 *X0TARGETDET_good = (TH1 *)(histos->FindObject("X0TARGETDET_good"));
  TH1 *Y0TARGETDET_good = (TH1 *)(histos->FindObject("Y0TARGETDET_good"));
  TH2 *X0Y0TARGETDET_good = (TH2 *)(histos->FindObject("X0Y0TARGETDET_good"));

  TH1 *hclus_good = (TH1 *)(histos->FindObject("hclus_good"));

  TH1 *occupancy[NJINF * NTDRS];
  TH1 *occupancy_posX[NJINF * NTDRS];
  TH1 *occupancy_posY[NJINF * NTDRS];
  for (int tt = 0; tt < _maxtdr; tt++) {
    int jinfnum = 0;
    int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
    occupancy[tt] = (TH1 *)(histos->FindObject(Form("occupancy_0_%02d", tdrnum)));
    occupancy_posX[tt] = (TH1 *)(histos->FindObject(Form("occupancy_posX_0_%02d", tdrnum)));
    occupancy_posY[tt] = (TH1 *)(histos->FindObject(Form("occupancy_posY_0_%02d", tdrnum)));
  }
  TH1 *hclusSladd_good = (TH1 *)(histos->FindObject("hclusSladd_good"));
  TH1 *hclusKladd_good = (TH1 *)(histos->FindObject("hclusKladd_good"));
  TH1 *hclusSladd_good_ontrack = (TH1 *)(histos->FindObject("hclusSladd_good_ontrack"));
  TH1 *hclusKladd_good_ontrack = (TH1 *)(histos->FindObject("hclusKladd_good_ontrack"));

  TH1 *occupancy_ontrack[NJINF * NTDRS];
  TH1 *occupancy_ontrack_posX[NJINF * NTDRS];
  TH1 *occupancy_ontrack_posY[NJINF * NTDRS];
  TH1 *chargeS[NJINF * NTDRS];
  TH1 *chargeK[NJINF * NTDRS];
  TH2 *charge2D[NJINF * NTDRS];
  TH1 *chargeS_ave;
  TH1 *chargeK_ave;
  TH2 *charge2D_ave;
  for (int tt = 0; tt < _maxtdr; tt++) {
    int jinfnum = 0;
    int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
    occupancy_ontrack[tt] = (TH1 *)(histos->FindObject(Form("occupancy_ontrack_0_%02d", tdrnum)));
    occupancy_ontrack_posX[tt] = (TH1 *)(histos->FindObject(Form("occupancy_ontrack_posX_0_%02d", tdrnum)));
    occupancy_ontrack_posY[tt] = (TH1 *)(histos->FindObject(Form("occupancy_ontrack_posY_0_%02d", tdrnum)));
    chargeS[tt] = (TH1 *)(histos->FindObject(Form("chargeS_0_%02d", tdrnum)));
    chargeK[tt] = (TH1 *)(histos->FindObject(Form("chargeK_0_%02d", tdrnum)));
    charge2D[tt] = (TH2 *)(histos->FindObject(Form("charge_0_%02d", tdrnum)));
  }
  chargeS_ave = (TH1 *)(histos->FindObject("chargeS"));
  chargeK_ave = (TH1 *)(histos->FindObject("chargeK"));
  charge2D_ave = (TH2 *)(histos->FindObject("charge"));

  TH1 *chi_gold = (TH1 *)(histos->FindObject("chi_gold"));
  TH1 *theta_gold = (TH1 *)(histos->FindObject("theta_gold"));
  TH1 *phi_gold = (TH1 *)(histos->FindObject("phi_gold"));
  TH2 *thetaphi_gold = (TH2 *)(histos->FindObject("thetaphi_gold"));
  TH1 *X0_gold = (TH1 *)(histos->FindObject("X0_gold"));
  TH1 *Y0_gold = (TH1 *)(histos->FindObject("Y0_gold"));
  TH2 *X0Y0_gold = (TH2 *)(histos->FindObject("X0Y0_gold"));
  TH1 *X0TARGETDET_gold = (TH1 *)(histos->FindObject("X0TARGETDET_gold"));
  TH1 *Y0TARGETDET_gold = (TH1 *)(histos->FindObject("Y0TARGETDET_gold"));
  TH2 *X0Y0TARGETDET_gold = (TH2 *)(histos->FindObject("X0Y0TARGETDET_gold"));

  TH1 *hclusSladd_gold = (TH1 *)(histos->FindObject("hclusSladd_gold"));
  TH1 *hclusKladd_gold = (TH1 *)(histos->FindObject("hclusKladd_gold"));
  TH1 *hclusSladd_gold_ontrack = (TH1 *)(histos->FindObject("hclusSladd_gold_ontrack"));
  TH1 *hclusKladd_gold_ontrack = (TH1 *)(histos->FindObject("hclusKladd_gold_ontrack"));

  TH1 *residual_X[NJINF * NTDRS];
  TH1 *residual_Y[NJINF * NTDRS];
  for (int tt = 0; tt < _maxtdr; tt++) {
    int jinfnum = 0;
    int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
    residual_X[tt] = (TH1 *)(histos->FindObject(Form("residual_X_0_%02d", tdrnum)));
    residual_Y[tt] = (TH1 *)(histos->FindObject(Form("residual_Y_0_%02d", tdrnum)));
  }

  //-----------------------------------------------------------------------------------------

  chi_good->Fill(log10(ev->GetChiTrack()));
  theta_good->Fill(ev->GetThetaTrack());
  phi_good->Fill(ev->GetPhiTrack());
  thetaphi_good->Fill(ev->GetThetaTrack(), ev->GetPhiTrack());
  X0_good->Fill(ev->GetS0Track());
  Y0_good->Fill(ev->GetK0Track());
  X0Y0_good->Fill(ev->GetS0Track(), ev->GetK0Track());
  X0TARGETDET_good->Fill(ev->ExtrapolateTrack(ZTARGETDET, 0));
  Y0TARGETDET_good->Fill(ev->ExtrapolateTrack(ZTARGETDET, 1));
  X0Y0TARGETDET_good->Fill(ev->ExtrapolateTrack(ZTARGETDET, 0), ev->ExtrapolateTrack(ZTARGETDET, 1));

  if (strackok || ktrackok) {
    chi_gold->Fill(log10(ev->GetChiTrack()));
    theta_gold->Fill(ev->GetThetaTrack());
    phi_gold->Fill(ev->GetPhiTrack());
    thetaphi_gold->Fill(ev->GetThetaTrack(), ev->GetPhiTrack());
    if (strackok)
      X0_gold->Fill(ev->GetS0Track());
    if (ktrackok)
      Y0_gold->Fill(ev->GetK0Track());
    X0Y0_gold->Fill(ev->GetS0Track(), ev->GetK0Track());
    if (strackok)
      X0TARGETDET_gold->Fill(ev->ExtrapolateTrack(ZTARGETDET, 0));
    if (ktrackok)
      Y0TARGETDET_gold->Fill(ev->ExtrapolateTrack(ZTARGETDET, 1));
    X0Y0TARGETDET_gold->Fill(ev->ExtrapolateTrack(ZTARGETDET, 0), ev->ExtrapolateTrack(ZTARGETDET, 1));
  }

  hclus_good->Fill(NClusTot);

  /*
  std::vector<double> v_cog_all_laddS[NJINF*NTDRS];//good tracks (not gold), all clusters
  std::vector<double> v_cog_all_laddK[NJINF*NTDRS];//good tracks (not gold), all clusters
  std::vector<double> v_cog_laddS[NJINF*NTDRS];//gold S tracks, clusters on track
  std::vector<double> v_cog_laddK[NJINF*NTDRS];//gold K tracks, clusters on track
  */

  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {

    cl = ev->GetCluster(index_cluster);
    int ladder = cl->ladder;
    //      printf("%d --> %d\n", ladder, ut->GetRH(chain)->FindPos(ladder));
    int jinfnum = cl->GetJinf();
    int tdrnum = cl->GetTDR();
    int side = cl->side;
    double charge = cl->GetCharge(); // unused for now

    occupancy[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(cl->GetCoG());
    //    printf("%d, %d) %f -> %f %f\n", ladder, side, cl->GetCoG(), cl->GetPosition(), cl->GetAlignedPosition());

    if (side == 0) {
      occupancy_posX[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(cl->GetAlignedPosition());
    } else {
      occupancy_posY[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(cl->GetAlignedPosition());
    }

    if (side == 0 || (side == 1 && LadderConf::Instance()->GetSideSwap(jinfnum, tdrnum))) {
      //      v_cog_all_laddS[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      hclusSladd_good->Fill(ladder);
      if (strackok) {
        hclusSladd_gold->Fill(ladder);
      }
    } else {
      //      v_cog_all_laddK[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      hclusKladd_good->Fill(ladder);
      if (ktrackok) {
        hclusKladd_gold->Fill(ladder);
      }
    }

    if (!ev->IsClusterUsedInTrack(index_cluster))
      continue;

    occupancy_ontrack[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(cl->GetCoG());

    if (side == 0) {
      occupancy_ontrack_posX[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(cl->GetAlignedPosition());
      if (strackok) {
        residual_X[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(cl->GetAlignedPosition() -
                                                            ev->ExtrapolateTrack(cl->GetZPosition(), 0));
      }
    } else {
      occupancy_ontrack_posY[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(cl->GetAlignedPosition());
      if (ktrackok) {
        residual_Y[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(cl->GetAlignedPosition() -
                                                            ev->ExtrapolateTrack(cl->GetZPosition(), 1));
      }
    }

    if (side == 0 || (side == 1 && LadderConf::Instance()->GetSideSwap(jinfnum, tdrnum))) {
      hclusSladd_good_ontrack->Fill(ladder);
      if (strackok) {
        //	v_cog_laddS[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
        hclusSladd_gold_ontrack->Fill(ladder);
      }
    } else {
      hclusKladd_good_ontrack->Fill(ladder);
      if (ktrackok) {
        //	v_cog_laddK[ut->GetRH(chain)->FindPos(ladder)].push_back(cl->GetAlignedPosition());
        hclusKladd_gold_ontrack->Fill(ladder);
      }
    }
  }

  std::vector<std::pair<int, std::pair<int, int>>> vec_charge = ev->GetHitVector(); // on track (good, not gold)
  for (unsigned int tt = 0; tt < vec_charge.size(); tt++) {
   	int jinfnum = cl->GetJinf();
    int tdrnum = cl->GetTDR();
    int ladder = vec_charge.at(tt).first;
    int index_cluster_S = vec_charge.at(tt).second.first;
    int index_cluster_K = vec_charge.at(tt).second.second;
    if (index_cluster_S >= 0) {
      chargeS[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(ev->GetCluster(index_cluster_S)->GetCharge());
    }
    if (index_cluster_K >= 0) {
      chargeK[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(ev->GetCluster(index_cluster_K)->GetCharge());
    }
    if (index_cluster_S >= 0 && index_cluster_K >= 0) {
      charge2D[ut->GetRH(chain)->FindPos(tdrnum,jinfnum)]->Fill(ev->GetCluster(index_cluster_S)->GetCharge(),
                                                        ev->GetCluster(index_cluster_K)->GetCharge());
    }
  }

  chargeS_ave->Fill(ev->GetChargeTrack(0));
  chargeK_ave->Fill(ev->GetChargeTrack(1));
  charge2D_ave->Fill(ev->GetChargeTrack(0), ev->GetChargeTrack(1));

  /*
  for (int ll=0; ll<NJINF*NTDRS; ll++) {
    hclusSladd_good->Fill(ut->GetRH(chain)->GetTdrNum(ll), v_cog_all_laddS[ll].size());
    hclusKladd_good->Fill(ut->GetRH(chain)->GetTdrNum(ll), v_cog_all_laddK[ll].size());
  }
  */

  return;
}

void NormalizePlots(TObjArray *histos, Long64_t cleanevs, Long64_t preselevs, Long64_t goodtracks, Long64_t goldStracks,
                    Long64_t goldKtracks) {

  TH1 *hclusSladd_clean = (TH1 *)(histos->FindObject("hclusSladd_clean"));
  TH1 *hclusKladd_clean = (TH1 *)(histos->FindObject("hclusKladd_clean"));

  TH1 *hclusSladd_presel = (TH1 *)(histos->FindObject("hclusSladd_presel"));
  TH1 *hclusKladd_presel = (TH1 *)(histos->FindObject("hclusKladd_presel"));

  TH1 *hclusSladd_good = (TH1 *)(histos->FindObject("hclusSladd_good"));
  TH1 *hclusKladd_good = (TH1 *)(histos->FindObject("hclusKladd_good"));
  TH1 *hclusSladd_good_ontrack = (TH1 *)(histos->FindObject("hclusSladd_good_ontrack"));
  TH1 *hclusKladd_good_ontrack = (TH1 *)(histos->FindObject("hclusKladd_good_ontrack"));

  TH1 *hclusSladd_gold = (TH1 *)(histos->FindObject("hclusSladd_gold"));
  TH1 *hclusKladd_gold = (TH1 *)(histos->FindObject("hclusKladd_gold"));
  TH1 *hclusSladd_gold_ontrack = (TH1 *)(histos->FindObject("hclusSladd_gold_ontrack"));
  TH1 *hclusKladd_gold_ontrack = (TH1 *)(histos->FindObject("hclusKladd_gold_ontrack"));

  //-----------------------------------------------

  hclusSladd_clean->Scale(1.0 / cleanevs);
  hclusKladd_clean->Scale(1.0 / cleanevs);

  hclusSladd_presel->Scale(1.0 / preselevs);
  hclusKladd_presel->Scale(1.0 / preselevs);

  hclusSladd_good->Scale(1.0 / goodtracks);
  hclusKladd_good->Scale(1.0 / goodtracks);
  hclusSladd_good_ontrack->Scale(1.0 / goodtracks);
  hclusKladd_good_ontrack->Scale(1.0 / goodtracks);

  hclusSladd_gold->Scale(1.0 / goldStracks);
  hclusKladd_gold->Scale(1.0 / goldKtracks);
  hclusSladd_gold_ontrack->Scale(1.0 / goldStracks);
  hclusKladd_gold_ontrack->Scale(1.0 / goldKtracks);

  return;
}

void CleanEmptyHistos(TObjArray *histos) {

  for (int ii = histos->GetEntries() - 1; ii >= 0; ii--) {
    TH1 *h = (TH1 *)(histos->At(ii));
    if (h->GetEntries() <= 0)
      h->Delete();
  }

  return;
}
