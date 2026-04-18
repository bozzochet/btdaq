#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStopwatch.h"
#include "TTree.h"
#include <vector>

#include "Event.hpp"
//#include "GenericEvent.hpp"
#include "LadderConf.hh"



#include "Utilities.hpp"


using namespace std;

using EventOCA = Event<1, 24, 64, 5, 10, 0>;
using calibOCA = calib<EventOCA::GetNCHAVA() * EventOCA::GetNVAS()>;


using EventFOOT = Event<1, 24, 64, 5, 10, 0>;
using calibFOOT = calib<EventFOOT::GetNCHAVA() * EventFOOT::GetNVAS()>;
using RHClassFOOT = RHClass<EventFOOT::GetNJINF(), EventFOOT::GetNTDRS()>;

using EventAMSL0 = Event<8, 9, 64, 8, 16, 0>;
using calibAMSL0 = calib<EventAMSL0::GetNCHAVA() * EventAMSL0::GetNVAS()>;
using RHClassAMSL0 = RHClass<EventAMSL0::GetNJINF(), EventAMSL0::GetNTDRS()>;

using EventAMS = Event<1, 24, 64, 3, 10, 6>;
using calibAMS = calib<EventAMS::GetNCHAVA() * EventAMS::GetNVAS()>;
using RHClassAMS = RHClass<EventAMS::GetNJINF(), EventAMS::GetNTDRS()>;

template <class Event, class RH> int ProcessChain(TChain *ch, TString output_filename);

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
  
    return ProcessChain<EventOCA, RHClassOCA>(chain, output_filename);
  } else if (kFoot) {

    return ProcessChain<EventFOOT, RHClassFOOT>(chain, output_filename);
  } else if (kAMSL0) {
;
    return ProcessChain<EventAMSL0, RHClassFOOT>(chain, output_filename);
  } else {

    return ProcessChain<EventAMS, RHClassAMS>(chain, output_filename);
  }
  printf("---------------------------------------------\n");

  return 0;
}

template <class Event, class RH> int ProcessChain(TChain *chain, TString output_filename) {

  using UT = Utilities<Event, RH>;
  UT *ut = new UT();

  static constexpr int NJINF = Event::GetNJINF();
  static constexpr int NTDRS = Event::GetNTDRS();

  Event *ev = NULL;

  Long64_t entries = chain->GetEntries();
  printf("This run has %lld entries\n", entries);

  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  ev->ReadGainCorrection("gaincorrection.dat");

  int _maxtdr = NJINF * NTDRS;

  if (ut->GetRH(chain)) {
    ut->GetRH(chain)->Print();
    _maxtdr = ut->GetRH(chain)->GetNTdrs();
  } else {
    printf("Not able to find the RHClass header in the UserInfo...\n");
    return -9;
  }

  TFile *foutput = new TFile(output_filename.Data(), "RECREATE");

  static constexpr int NVAS   = Event::GetNVAS();
  static constexpr int NCHAVA = Event::GetNCHAVA();


  std::vector<TH1F *> hcog(NTDRS, nullptr);
  for (int itdr = 0; itdr < NTDRS; itdr++) {
    hcog[itdr] = new TH1F(Form("hcog_sensor%02d", itdr),
                          Form("Occupancy sensor%02d; CoG; Entries", itdr),
                          NVAS * NCHAVA, 0, NVAS * NCHAVA);
  }



  double perc = 0;

  for (int index_event = 0; index_event < entries; index_event++) {
    Double_t pperc = 100.0 * ((index_event + 1.0) / entries);
    if (pperc >= perc) {
      printf("\rProcessed %d out of %lld: %d%%", (index_event + 1), entries,
             (int)(100.0 * (index_event + 1.0) / entries));
      std::cout << std::flush;
      perc++;
    }
    chain->GetEntry(index_event);

 
      for (int index_cluster = 0; index_cluster < ev->GetNClusTot(); index_cluster++) {
        Cluster *cl = ev->GetCluster(index_cluster);
        int tdr = cl->GetTDR();
          hcog[tdr]->Fill(cl->GetCoG());
      }

  }
  std::cout << '\n';








  for (int itdr = 0; itdr < NTDRS; itdr++) {
    if (hcog[itdr]->GetEntries() == 0) continue;

    hcog[itdr]->Write();


  }




  foutput->Close();

  return 0;
}
