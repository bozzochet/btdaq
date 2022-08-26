#ifndef DecodeDataAMS_h
#define DecodeDataAMS_h

#include <cstdio>
#include <vector>

#include "TBranch.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

#include "DecodeData.hh"
#include "GenericEvent.hh"

class DecodeDataAMS : public DecodeData {
public:
  using EventAMS = GenericEvent<1, 24, 64, 3, 16, 10>;
  using RHClassAMS = RHClass<EventAMS::GetNJINF(), EventAMS::GetNTDRS()>;
  using calibAMS = calib<EventAMS::GetNCHAVA() * EventAMS::GetNVAS()>;

private:
  TFile *mcf = nullptr;  // added by Viviana
  TTree *mcrt = nullptr; // added by Viviana
  TTree *mcht = nullptr; // added by Viviana

  char rawname[300];
  char rawdir[300];
  char rawCaldir[300];
  // int tdroffset{0};
  calibAMS cals[EventAMS::GetNJINF() * EventAMS::GetNTDRS()]{};
  EventAMS::JArray<int> JinfMap{0};
  int ntdrMC{0}; // added by Viviana, controllare se matcha con le modifiche che avevo fatto alle Map
  bool out_flag{false};
  char type[10]{"Jinf"}; // JinF or JinJ
  void FindCalibs();
  int ReadCalib(FILE *fil, calibAMS *cal);
  int ReadCalib_mc(FILE *fil, calibAMS *cal);
  int FindPosMC(int hitnum); // added by Viviana, serve veramente?
  void mysort(laddernumtype *aa, int nel);

  bool kMC;

public:
  EventAMS *ev;
  RHClassAMS *rh;

  int nlayers; // added by Viviana
  double gene; // added by Viviana
  int gevt;    // added by Viviana
  int evenum{0};

  int nhits; // added by Viviana
  int ntothits;
  int pphit; // added by Viviana

  //// generalization for nhits!=nlayers
  int hvol[MAXNHITS];     // added by Viviana
  double hvolz[MAXNHITS]; // added by Viviana
  double xcoord[MAXNHITS];
  double ycoord[MAXNHITS];
  double zcoord[MAXNHITS];
  // int echx[MAXNHITS]; //added by Viviana
  // int echy[MAXNHITS];//added by Viviana
  // double edep[MAXNHITS];  //added by Viviana
  double eEne[MAXNHITS]; // added by Viviana
  double eDep[MAXNHITS]; // added by Viviana

  int pdg[MAXNHITS];
  int parid[MAXNHITS];
  int trid[MAXNHITS];

  // new GGS output for channels and deposits
  int chXY[MAXNHITS];
  int hitStrips[MAXNHITS];
  int simStrips[MAXNHITS];
  std::vector<int> *hitChan = 0;
  std::vector<double> *hitDep = 0;
  std::vector<int> *simChan = 0;
  std::vector<double> *simDep = 0;

  virtual void DumpRunHeader() override;

public:
  DecodeDataAMS() = default;
  DecodeDataAMS(char *ifname, char *caldir, int run, int ancillary, bool _kMC = false);
  virtual ~DecodeDataAMS();

  virtual ::FlavorConfig FlavorConfig() final {
    return {EventAMS::GetNJINF(), EventAMS::GetNTDRS(), EventAMS::GetNCHAVA(), EventAMS::GetNADCS(),
            EventAMS::GetNVAS()};
  };
  virtual TString EventClassname() final {return ev->ClassName(); };

  virtual int GetTdrNum(size_t pos) final;
  virtual int GetTdrType(size_t pos) final;

  virtual int SkipOneEvent(int evskip = 1) final;
  virtual int ReadOneEvent() final;
  virtual void ClearEvent() final { ev->Clear(); };

  virtual void OpenFile(const char *ifname, const char *caldir, int run, int ancillary) final;
  virtual void CloseFile() final;
  virtual int EndOfFile() final;

  // data
  void OpenFile_data(const char *ifname, const char *caldir, int run, int ancillary);
  int ReadOneEvent_data();
  int ReadOneTDR(int jinf = 0);
  int ReadOneJINF();
  int SkipOneEvent_data(int evskip = 1);
  int EndOfFile_data();

  // mc
  //  void OpenFile_mc(char* ifname, char* caldir, int run, int ancillary);
  void OpenFile_mc(const char *ifname);
  int ReadOneEvent_mc();
  int SkipOneEvent_mc(int evskip = 1);

  int EndOfFile_mc();

  virtual TTree *GetMCTruth() final { return mcht; }; // CB
};

#endif
