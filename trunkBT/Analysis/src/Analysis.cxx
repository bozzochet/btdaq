#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
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
using EventAMSL0 = GenericEvent<1, 2, 64, 8, 16, 0>;
using calibAMSL0 = calib<EventAMSL0::GetNCHAVA() * EventAMSL0::GetNVAS()>;
using RHClassAMSL0 = RHClass<EventAMSL0::GetNJINF(), EventAMSL0::GetNTDRS()>;
//---
using EventAMS = GenericEvent<1, 24, 64, 3, 16, 10>;
using calibAMS = calib<EventAMS::GetNCHAVA() * EventAMS::GetNVAS()>;
using RHClassAMS = RHClass<EventAMS::GetNJINF(), EventAMS::GetNTDRS()>;

template <class Event, class RH> int ProcessChain(TChain *ch, TString output_filename);
template <class Event, class RH> void BookHistos(TObjArray *histos, Long64_t entries, int _maxtdr, TChain *chain);
template <class Event, class RH> void FillAllHistos(TObjArray *histos, int NClusTot, Event *ev, int index_event);
template <class Event, class RH> void FillCleanHistos(TObjArray *histos, int NClusTot, Event *ev, int index_event);
template <class Event, class RH> void FillPreselHistos(TObjArray *histos, int NClusTot, Event *ev, int index_event);
template <class Event, class RH>
void FillGoodHistos(TObjArray *histos, int NClusTot, Event *ev, int _maxtdr, TChain *chain, bool strackok,
                    bool ktrackok);
void NormalizePlots(TObjArray *histos, Long64_t cleanevs, Long64_t preselevs, Long64_t goodtracks, Long64_t goldStracks,
                    Long64_t goldKtracks);
void CleanEmptyHistos(TObjArray *histos);

//----------------------------------------

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
  foutput->cd();

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

    FillAllHistos<Event, RH>(histos, NClusTot, ev, index_event);

    // at least 4 clusters (if we want 2 on S and 2 on K this is really the sindacal minimum...)
    // and at most 100 (to avoid too much noise around and too much combinatorial)
    // at most 6 clusters per ladder (per side) + 0 additional clusters in total (per side)
    //  ts->CleanEvent(ev, ut->GetRH(chain), 4, 100, 6, 6, 0, 0);

    bool cleanevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 9999, 9999, 9999, 9999, 9999);
    if (!cleanevent)
      continue;
    cleanevs++; // in this way this number is giving the "complete reasonable sample"

    FillCleanHistos<Event, RH>(histos, NClusTot, ev, index_event);

    //    bool preselevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 100, 3, 3, 0, 0);//valid for TIC TB?
    bool preselevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 100, 10, 10, 0, 0);
    if (!preselevent)
      continue;
    preselevs++;

    FillPreselHistos<Event, RH>(histos, NClusTot, ev, index_event);

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

    FillGoodHistos<Event, RH>(histos, NClusTot, ev, _maxtdr, chain, strackok, ktrackok);

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

  Cluster *cl;

  TH1 *hclus = (TH1 *)(histos->FindObject("hclus"));
  TH1 *hclus_vs_event = (TH1 *)(histos->FindObject("hclus_vs_event"));
  TH2 *hclusSladd_vs_event = (TH2 *)(histos->FindObject("hclusSladd_vs_event"));
  TH2 *hclusKladd_vs_event = (TH2 *)(histos->FindObject("hclusKladd_vs_event"));

  hclus->Fill(NClusTot);
  hclus_vs_event->Fill(index_event, NClusTot);

  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {

    cl = ev->GetCluster(index_cluster);
    int ladder = cl->ladder;
    //      printf("%d --> %d\n", ladder, ut->GetRH(chain)->FindPos(ladder));
    int jinfnum = cl->GetJinf();
    int tdrnum = cl->GetTDR();
    int side = cl->side;
    double charge = cl->GetCharge(); // unused for now
    //    printf("%d %d) %d %d\n", jinfnum, tdrnum, side, LadderConf::Instance()->GetSideSwap(jinfnum, tdrnum));

    if (side == 0 || (side == 1 && LadderConf::Instance()->GetSideSwap(jinfnum, tdrnum))) {
      hclusSladd_vs_event->Fill(index_event, ladder);
    } else {
      hclusKladd_vs_event->Fill(index_event, ladder);
    }
  }

  return;
}

template <class Event, class RH> void FillCleanHistos(TObjArray *histos, int NClusTot, Event *ev, int index_event) {

  Cluster *cl;

  TH1 *hclus_clean = (TH1 *)(histos->FindObject("hclus_clean"));
  TH1 *hclus_vs_event_clean = (TH1 *)(histos->FindObject("hclus_vs_event_clean"));
  TH1 *hclusSladd_clean = (TH1 *)(histos->FindObject("hclusSladd_clean"));
  TH1 *hclusKladd_clean = (TH1 *)(histos->FindObject("hclusKladd_clean"));
  TH2 *hclusSladd_vs_event_clean = (TH2 *)(histos->FindObject("hclusSladd_vs_event_clean"));
  TH2 *hclusKladd_vs_event_clean = (TH2 *)(histos->FindObject("hclusKladd_vs_event_clean"));

  hclus_clean->Fill(NClusTot);
  hclus_vs_event_clean->Fill(index_event, NClusTot);

  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {

    cl = ev->GetCluster(index_cluster);
    int ladder = cl->ladder;
    //      printf("%d --> %d\n", ladder, ut->GetRH(chain)->FindPos(ladder));
    int jinfnum = cl->GetJinf();
    int tdrnum = cl->GetTDR();
    int side = cl->side;
    double charge = cl->GetCharge(); // unused for now

    if (side == 0 || (side == 1 && LadderConf::Instance()->GetSideSwap(jinfnum, tdrnum))) {
      hclusSladd_clean->Fill(ladder);
      hclusSladd_vs_event_clean->Fill(index_event, ladder);
    } else {
      hclusKladd_clean->Fill(ladder);
      hclusKladd_vs_event_clean->Fill(index_event, ladder);
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

    occupancy[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetCoG());
    //    printf("%d, %d) %f -> %f %f\n", ladder, side, cl->GetCoG(), cl->GetPosition(), cl->GetAlignedPosition());

    if (side == 0) {
      occupancy_posX[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
    } else {
      occupancy_posY[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
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

    occupancy_ontrack[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetCoG());

    if (side == 0) {
      occupancy_ontrack_posX[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
      if (strackok) {
        residual_X[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition() -
                                                            ev->ExtrapolateTrack(cl->GetZPosition(), 0));
      }
    } else {
      occupancy_ontrack_posY[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
      if (ktrackok) {
        residual_Y[ut->GetRH(chain)->FindPos(ladder)]->Fill(cl->GetAlignedPosition() -
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
    int ladder = vec_charge.at(tt).first;
    int index_cluster_S = vec_charge.at(tt).second.first;
    int index_cluster_K = vec_charge.at(tt).second.second;
    if (index_cluster_S >= 0) {
      chargeS[ut->GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_S)->GetCharge());
    }
    if (index_cluster_K >= 0) {
      chargeK[ut->GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_K)->GetCharge());
    }
    if (index_cluster_S >= 0 && index_cluster_K >= 0) {
      charge2D[ut->GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_S)->GetCharge(),
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
