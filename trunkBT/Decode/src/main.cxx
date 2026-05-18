

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <vector>

#include "anyoption.h"
#include <getopt.h>

// ROOT
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TPaveStats.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TTree.h"
#include <Compression.h>

// Decoders
#include "DecodeDataAMS.hh"
#ifndef NOAMSL0
#include "DecodeDataAMSL0.hh"
#endif
#include "DecodeDataFOOT.hh"
#include "DecodeDataOCA.hh"
#include "DecodeDataHEF.hh"

#include "Event.hpp"

using namespace std;

char progname[300];

void CreatePdfWithPlots(DecodeData *dd1, char *pdf_filename);
void PlotsWithFits(TH1 *histo, char *name, char *title, char *pdf_filename);

int main(int argc, char **argv) {

  char filename[300], pdf_filename[1024];

  char DirRaw[255];
  char DirCal[255];
  char DirRoot[255];

  double shighthreshold = 3.5;
  double slowthreshold = 1.0;
  double khighthreshold = 3.5;
  double klowthreshold = 1.0;

  bool kMC = false;
  bool kOca = false;
  bool kHef = false;
  bool kFoot = false;
  bool kL0 = false;
  bool kL0old = false;
  bool kPri = false;
  bool kEvPri = false;

  bool kOnlyProcessCal = false;

  bool kClusterize = false;
  bool kExtCalfile = false;
  int cworkaround = 0;

  int run = 110;
  int runstop = -1;
  int calrunstart = 0;
  int calrunstop = 0;
  int ancillary = -1;
  int eventstoprocess = -1;
  int processed = 0;
  int jinffailed = 0;
  int readfailed = 0;

  sprintf(DirRoot, "./RootData/");
  sprintf(DirRaw, "./RawData/");
  sprintf(DirCal, "./CalData/");

  auto opt = std::make_unique<AnyOption>();

  opt->addUsage("Usage: ./Decode [options] [arguments]");
  opt->addUsage("");
  opt->addUsage("Options: ");
  opt->addUsage("  -h, --help  ................................. Print this help");
  opt->addUsage("  --pri  ...................................... Enable some general debug printouts");
  opt->addUsage("  --evpri  .................................... Enable event base debug printouts");
  opt->addUsage("  --events  ................................... Number of events to decode (default is all events)");
  opt->addUsage(
      Form("  --rawdata <path/to/dir/with/raw> ............ Directory with raw data (%s is the default)", DirRaw));
  opt->addUsage(
      Form("  --caldata <path/to/dir/with/cal> ............ Directory with cal data (%s is the default)", DirCal));
  opt->addUsage(Form(
      "  --rootdata <path/to/dir/for/root> ........... Directory where to put ROOT file (%s is the default)", DirRoot));
  opt->addUsage("  --oca  ...................................... Read the OCA boards");
  opt->addUsage("  --hef  ...................................... Read the HEF boards");
  opt->addUsage("  --foot ...................................... Read files from FOOT Bo TDAQ");
  opt->addUsage("  --l0 ........................................ Read files from AMSL0 DAQ (USB-LF and v>=5)");
  opt->addUsage("  --l0old ..................................... Read files from AMSL0 DAQ (USB-LEF)");
  opt->addUsage(
      "  -c, --clusterize ............................ To perform an offline clusterization to the RAW event");
  opt->addUsage("                                                (the bonding type is defined in ladderconf.dat");
  opt->addUsage("                                                with the same codes as for --cworkaround)");
  opt->addUsage("  --calrunstart ............................... Run/file number of calibration (first)\n"
                "                                                (needed for AMSL0 or possible for (OCA, FOOT, HEF) to\n"
                "                                                choose the calibration by hand)");
  opt->addUsage(
      "  --calrunstop ................................ Run/file number of calibration (last) (needed for AMSL0)");
  opt->addUsage("  --extcalfile ................................ Load external calibration files (only implemented for OCA and HEF)");
  opt->addUsage("  --ancillary ................................. Ancillary file number (only possible for AMS)");
  opt->addUsage(
      "  -m, --montecarlo ............................ To decode MonteCarlo simulation files (default is OFF)");
  opt->addUsage(
      "  -l, --onlycal ............................... To only search and process the (closer automatically\n"
      "                                                chosen for AMS, OCA, FOOT and HEF, but not AMSL0) calibration run.\n"
      "                                                Only 10k events used for calibration. When using this flag,\n"
      "                                                the run number provided is used for the output file root name,\n"
      "                                                that will contain \"ONLYCAL\". (default is OFF)");
  opt->addUsage(
      "*************************************************************************************************************\n"
      "To choose the clusterization thresholds: FOR ALL THE LADDERS, IGNORING the ones defined in ladderconf.dat:");
  opt->addUsage(
      "  --shighthreshold <X> ........................ S-side S/N high threshold. Used in the offline clusterization\n"
      "                                                if option -c or to fill the plots for the ladders with raw\n"
      "                                                events (3.5 is the default)");
  opt->addUsage(
      "  --slowthreshold  <X> ........................ S-side S/N low threshold. Used in the offline clusterization\n"
      "                                                if option -c or to fill the plots for the ladders with raw\n"
      "                                                events (1.0 is the default)");
  opt->addUsage(
      "  --khighthreshold <X> ........................ K-side S/N high threshold. Used in the offline clusterization\n"
      "                                                if option -c or to fill the plots for the ladders with raw\n"
      "                                                events (3.5 is the default)");
  opt->addUsage(
      "  --klowthreshold  <X> ........................ K-side S/N low threshold. Used in the offline clusterization\n"
      "                                                if option -c or to fill the plots for the ladders with raw\n"
      "                                                events (1.0 is the default)");
  opt->addUsage("  --cworkaround <N> ........................... To choose the workaround clusterization:");
  opt->addUsage("                                                    FOR ALL THE LADDERS, IGNORING the one defined in\n"
                "                                                    ladderconf.dat:");
  opt->addUsage(
      "                                                    (option kept just for interface compatibility and\n"
      "                                                    to have the list of codes)");
  opt->addUsage("                                                    0 is the standard one (default)");
  opt->addUsage("                                                    1 for the Limadou monster. On S-side only even\n"
                "                                                    channels are bonded");
  opt->addUsage(
      "                                                    2 for the DAMPE ladders read-out by AMS TDR. Only\n"
      "                                                    S-side, [0-191]+[320-511]");
  opt->addUsage(
      "                                                    3 for the GSI ladders read-out by AMS TDR. K side\n"
      "                                                    has all 384 channels bonded to a single sensor");
  opt->addUsage("******************************************************************************************************"
                "*******");
  opt->addUsage("Arguments: ");
  opt->addUsage("  <runnum> [runstop (for AMSL0)]");

  opt->setFlag("help", 'h');
  opt->setFlag("pri");
  opt->setFlag("evpri");
  opt->setFlag("clusterize", 'c');
  opt->setFlag("montecarlo", 'm');
  opt->setFlag("onlycal", 'l');
  opt->setFlag("extcalfile");
  opt->setFlag("oca");
  opt->setFlag("hef");
  opt->setFlag("foot");
  opt->setFlag("l0");
  opt->setFlag("l0old");

  opt->setOption("rawdata");
  opt->setOption("caldata");
  opt->setOption("calrunstart");
  opt->setOption("calrunstop");
  opt->setOption("ancillary");
  opt->setOption("rootdata");
  opt->setOption("shighthreshold");
  opt->setOption("slowthreshold");
  opt->setOption("khighthreshold");
  opt->setOption("klowthreshold");
  opt->setOption("cworkaround");
  opt->setOption("events");

  opt->processCommandArgs(argc, argv);

  if (opt->getFlag("help") || opt->getFlag('h')) {
    opt->printUsage();
    exit(2);
  }

  if (opt->getFlag("pri"))
    kPri = true;
  if (opt->getFlag("evpri"))
    kEvPri = true;
  if (opt->getFlag("clusterize") || opt->getFlag('c'))
    kClusterize = true;
  if (opt->getFlag("extcalfile"))
    kExtCalfile = true;
  if (opt->getFlag("montecarlo") || opt->getFlag('m'))
    kMC = true;
  if (opt->getFlag("onlycal") || opt->getFlag('l'))
    kOnlyProcessCal = true;
  if (opt->getFlag("oca"))
    kOca = true;
  if (opt->getFlag("hef"))
    kHef = true;
  if (opt->getFlag("foot"))
    kFoot = true;
  if (opt->getFlag("l0"))
    kL0 = true;
  if (opt->getFlag("l0old"))
    kL0old = true;

  if (opt->getValue("rawdata"))
    sprintf(DirRaw, "%s/", opt->getValue("rawdata"));
  if (opt->getValue("caldata"))
    sprintf(DirCal, "%s/", opt->getValue("caldata"));
  if (opt->getValue("rootdata"))
    sprintf(DirRoot, "%s/", opt->getValue("rootdata"));

  if (opt->getValue("shighthreshold"))
    shighthreshold = atof(opt->getValue("shighthreshold"));
  if (opt->getValue("slowthreshold"))
    slowthreshold = atof(opt->getValue("slowthreshold"));
  if (opt->getValue("khighthreshold"))
    khighthreshold = atof(opt->getValue("khighthreshold"));
  if (opt->getValue("klowthreshold"))
    klowthreshold = atof(opt->getValue("klowthreshold"));

  if (opt->getValue("cworkaround"))
    cworkaround = atoi(opt->getValue("cworkaround"));
  if (opt->getValue("calrunstart"))
    calrunstart = atoi(opt->getValue("calrunstart"));
  if (opt->getValue("calrunstop"))
    calrunstop = atoi(opt->getValue("calrunstop"));
  if (opt->getValue("ancillary"))
    ancillary = atoi(opt->getValue("ancillary"));
  if (opt->getValue("events"))
    eventstoprocess = atoi(opt->getValue("events"));

  switch (opt->getArgc()) {
  case 1:
    run = atoi(opt->getArgv(0));
    runstop = -1;
    break;
  case 2:
    run = atoi(opt->getArgv(0));
    runstop = atoi(opt->getArgv(1));
    break;
  default:
    opt->printUsage();
    exit(-1);
    break;
  }

  sprintf(progname, "%s", argv[0]);

  printf("Reading Raw Data from %s\n", DirRaw);
  printf("Reading Cal Data from %s\n", DirCal);
  printf("Writing output in %s\n", DirRoot);

  if (ancillary < 0) {
    sprintf(filename, "%s/run_%06d.root", DirRoot, run);
    if (kOnlyProcessCal) {
      sprintf(filename, "%s/run_%06d_ONLYCAL.root", DirRoot, run);
    }
  } else {
    sprintf(filename, "%s/run_%06d_ANC_%d.root", DirRoot, run, ancillary);
  }
  sprintf(pdf_filename, "%s.pdf", filename);

  int complevel = ROOT::CompressionSettings(ROOT::RCompressionSetting::EAlgorithm::kZLIB, 2);
  printf("The choosen compression level is %d\n", complevel);

  TFile *foutput = new TFile(filename, "RECREATE", "File with the event tree", complevel);

  TTree *t3 = NULL;
  TTree *t4 = NULL;
  if (!kOnlyProcessCal) {
    t4 = new TTree("t4", "My cluster tree");
  } else {
    t3 = new TTree("t3", "My calibration tree");
  }

  int bufsize = 32000;
  int splitlevel = 99;

  DecodeData *dd1 = nullptr;
  FlavorConfig fConf;

  if (kOca) {
    auto *dd = new DecodeDataOCA(DirRaw, DirCal, run, calrunstart, kOnlyProcessCal, kExtCalfile);
    fConf = dd->FlavorConfig();
    if (!kOnlyProcessCal)
      t4->Branch("cluster_branch", dd->EventClassname(), &(dd->ev), bufsize, splitlevel);
    auto calibs = dd->GetCalibrations();
    foutput->WriteTObject(&calibs, "cals");
    dd1 = static_cast<DecodeData *>(dd);
  } else if (kHef) {
    auto *dd = new DecodeDataHEF(DirRaw, DirCal, run, calrunstart, kOnlyProcessCal, kExtCalfile);
    fConf = dd->FlavorConfig();
    if (!kOnlyProcessCal)
      t4->Branch("cluster_branch", dd->EventClassname(), &(dd->ev), bufsize, splitlevel);
    auto calibs = dd->GetCalibrations();
    foutput->WriteTObject(&calibs, "cals");
    dd1 = static_cast<DecodeData *>(dd);
  } else if (kFoot) {
    auto *dd = new DecodeDataFOOT(DirRaw, DirCal, run, calrunstart);
    fConf = dd->FlavorConfig();
    if (!kOnlyProcessCal)
      t4->Branch("cluster_branch", dd->EventClassname(), &(dd->ev), bufsize, splitlevel);
    dd1 = static_cast<DecodeData *>(dd);
  }
#ifndef NOAMSL0
  else if (kL0 || kL0old) {
    kL0 = true;
    auto *dd =
        new DecodeDataAMSL0(DirRaw, DirCal, run, runstop, calrunstart, calrunstop, kL0old ? 0 : 1, kOnlyProcessCal);
    fConf = dd->FlavorConfig();
    if (!kOnlyProcessCal)
      t4->Branch("cluster_branch", dd->EventClassname(), &(dd->ev), bufsize, splitlevel);
    auto calibs = dd->GetCalibrations();
    foutput->WriteTObject(&calibs, "cals");
    dd1 = static_cast<DecodeData *>(dd);
  }
#endif
  else {
    auto *dd = new DecodeDataAMS(DirRaw, DirCal, run, ancillary, kMC);
    fConf = dd->FlavorConfig();
    if (!kOnlyProcessCal)
      t4->Branch("cluster_branch", dd->EventClassname(), &(dd->ev), bufsize, splitlevel);
    dd1 = static_cast<DecodeData *>(dd);
  }

  if (!kOnlyProcessCal) {
    TBranch *branch = t4->GetBranch("cluster_branch");
    if (branch)
      branch->SetCompressionLevel(6);
  }

  if (kPri)
    dd1->SetPrintOn();
  else
    dd1->SetPrintOff();

  if (kEvPri)
    dd1->SetEvPrintOn();
  else
    dd1->SetEvPrintOff();

  if (!kOnlyProcessCal) {

    dd1->shighthreshold = shighthreshold;
    dd1->slowthreshold = slowthreshold;
    dd1->khighthreshold = khighthreshold;
    dd1->klowthreshold = klowthreshold;
    dd1->kClusterize = kClusterize;
    dd1->cworkaround = cworkaround;
    

    dd1->SetPrintOff();
    dd1->SetEvPrintOff();

    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    /*

    double chaK[fConf.NJINF][fConf.NTDRS];
    double chaS[fConf.NJINF][fConf.NTDRS];
    double sigK[fConf.NJINF][fConf.NTDRS];
    double sigS[fConf.NJINF][fConf.NTDRS];
    double sonK[fConf.NJINF][fConf.NTDRS];
    double sonS[fConf.NJINF][fConf.NTDRS];
    */

    const int NJINF = (int)fConf.NJINF;
    const int NTDRS = (int)fConf.NTDRS;

    // (j,t) -> j*NTDRS + t
    auto idx2 = [NTDRS](int j, int t) -> int { return j * NTDRS + t; };

    std::vector<double> chaK(NJINF * NTDRS, 0.0);
    std::vector<double> chaS(NJINF * NTDRS, 0.0);
    std::vector<double> sigK(NJINF * NTDRS, 0.0);
    std::vector<double> sigS(NJINF * NTDRS, 0.0);
    std::vector<double> sonK(NJINF * NTDRS, 0.0);
    std::vector<double> sonS(NJINF * NTDRS, 0.0);

    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    int NTDR = dd1->GetNTdrRaw() + dd1->GetNTdrCmp();
    for (int ii = 0; ii < NTDR; ii++) {
      int Jinfnum = dd1->GetJinfNum_byglobindex(ii);
      int Tdrnum = dd1->GetTdrNum_byglobindex(ii);
      int IdTDR = dd1->ComputeTdrId(Tdrnum, Jinfnum);

      //--------------------------------------------------------------------------------------------

      //--------------------------------------------------------------------------------------------
      /*

      t4->Branch(Form("SignalS_Ladder%03d", IdTDR), &sigS[Jinfnum][Tdrnum], Form("SignalS_Ladder%03d/D", IdTDR));
      t4->Branch(Form("ChargeS_Ladder%03d", IdTDR), &chaS[Jinfnum][Tdrnum], Form("ChargeS_Ladder%03d/D", IdTDR));
      t4->Branch(Form("SoNS_Ladder%03d", IdTDR), &sonS[Jinfnum][Tdrnum], Form("SoNS_Ladder%03d/D", IdTDR));
      t4->Branch(Form("SignalK_Ladder%03d", IdTDR), &sigK[Jinfnum][Tdrnum], Form("SignalK_Ladder%03d/D", IdTDR));
      t4->Branch(Form("ChargeK_Ladder%03d", IdTDR), &chaK[Jinfnum][Tdrnum], Form("ChargeK_Ladder%03d/D", IdTDR));
      t4->Branch(Form("SoNK_Ladder%03d", IdTDR), &sonK[Jinfnum][Tdrnum], Form("SoNK_Ladder%03d/D", IdTDR));
      */

      t4->Branch(Form("SignalS_Ladder%03d", IdTDR), &sigS[idx2(Jinfnum, Tdrnum)], Form("SignalS_Ladder%03d/D", IdTDR));
      t4->Branch(Form("ChargeS_Ladder%03d", IdTDR), &chaS[idx2(Jinfnum, Tdrnum)], Form("ChargeS_Ladder%03d/D", IdTDR));
      t4->Branch(Form("SoNS_Ladder%03d", IdTDR), &sonS[idx2(Jinfnum, Tdrnum)], Form("SoNS_Ladder%03d/D", IdTDR));
      t4->Branch(Form("SignalK_Ladder%03d", IdTDR), &sigK[idx2(Jinfnum, Tdrnum)], Form("SignalK_Ladder%03d/D", IdTDR));
      t4->Branch(Form("ChargeK_Ladder%03d", IdTDR), &chaK[idx2(Jinfnum, Tdrnum)], Form("ChargeK_Ladder%03d/D", IdTDR));
      t4->Branch(Form("SoNK_Ladder%03d", IdTDR), &sonK[idx2(Jinfnum, Tdrnum)], Form("SoNK_Ladder%03d/D", IdTDR));
    }

    auto *ddams = dynamic_cast<DecodeDataAMS *>(dd1);
    auto *ddoca = dynamic_cast<DecodeDataOCA *>(dd1);
    auto *ddhef = dynamic_cast<DecodeDataHEF *>(dd1);
    auto *ddfoot = dynamic_cast<DecodeDataFOOT *>(dd1);
#ifndef NOAMSL0
    auto *ddamsl0 = dynamic_cast<DecodeDataAMSL0 *>(dd1);
#endif
    (void)ddams;
    (void)ddoca;
    (void)ddhef;
    (void)ddfoot;
#ifndef NOAMSL0
    (void)ddamsl0;
#endif

    t4->GetUserInfo()->Add(dd1->rh);
    t4->GetUserInfo()->Add(LadderConf::Instance()->GetLadderParamsMap());

    TObjArray *obj = t4->GetListOfBranches();
    for (int ii = 0; ii < obj->GetEntries(); ii++) {
      TBranch *branch = (TBranch *)(obj->At(ii));
      branch->SetCompressionLevel(6);
    }

    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /*

    auto fillClusterArrays = [dd1, &chaK, &chaS, &sigK, &sigS, &sonK, &sonS](auto *dd) {
      auto fConf = dd->FlavorConfig();
      for (int jj = 0; jj < fConf.NJINF; jj++) {
        memset(chaK[jj], 0, fConf.NTDRS * sizeof(chaK[jj][0]));
        memset(chaS[jj], 0, fConf.NTDRS * sizeof(chaS[jj][0]));
        memset(sigK[jj], 0, fConf.NTDRS * sizeof(sigK[jj][0]));
        memset(sigS[jj], 0, fConf.NTDRS * sizeof(sigS[jj][0]));
      }
      for (int cc = 0; cc < (dd->ev)->GetNClusTot(); cc++) {
        Cluster *cl = (dd->ev)->GetCluster(cc);
        int ladder = cl->ladder;
        int Jinfnum = dd1->GetJinfNum_byID(ladder);
        int Tdrnum = dd1->GetTdrNum_byID(ladder);
        double signal = cl->GetTotSig();
        double charge = cl->GetCharge();
        double son = cl->GetTotSN();
        if (cl->side == 1) {
          if (charge > chaK[Jinfnum][Tdrnum]) {
            chaK[Jinfnum][Tdrnum] = charge;
            sigK[Jinfnum][Tdrnum] = signal;
            sonK[Jinfnum][Tdrnum] = son;
          }
        } else {
          if (charge > chaS[Jinfnum][Tdrnum]) {
            chaS[Jinfnum][Tdrnum] = charge;
            sigS[Jinfnum][Tdrnum] = signal;
            sonS[Jinfnum][Tdrnum] = son;
          }
        }
      }
    };
    */

    auto fillClusterArrays = [dd1, &chaK, &chaS, &sigK, &sigS, &sonK, &sonS, idx2](auto *dd) {
      std::fill(chaK.begin(), chaK.end(), 0.0);
      std::fill(chaS.begin(), chaS.end(), 0.0);
      std::fill(sigK.begin(), sigK.end(), 0.0);
      std::fill(sigS.begin(), sigS.end(), 0.0);
      std::fill(sonK.begin(), sonK.end(), 0.0);
      std::fill(sonS.begin(), sonS.end(), 0.0);

      for (int cc = 0; cc < (dd->ev)->GetNClusTot(); cc++) {

        Cluster *cl = (dd->ev)->GetCluster(cc);
        int ladder = cl->ladder;

        int Jinfnum = dd1->GetJinfNum_byID(ladder);
        int Tdrnum = dd1->GetTdrNum_byID(ladder);
        const int k = idx2(Jinfnum, Tdrnum);

        double signal = cl->GetTotSig();
        double charge = cl->GetCharge();
        double son = cl->GetTotSN();

        if (cl->side == 1) {
          if (charge > chaK[k]) {
            chaK[k] = charge;
            sigK[k] = signal;
            sonK[k] = son;
          }
        } else {
          if (charge > chaS[k]) {
            chaS[k] = charge;
            sigS[k] = signal;
            sonS[k] = son;
          }
        }
      }
    };

    /*

    auto fillRawArrays = [dd1, &chaK, &chaS, &sigK, &sigS, &sonK, &sonS](auto *dd) {
      auto fConf = dd->FlavorConfig();
      for (int jj = 0; jj < fConf.NJINF; jj++) {
        memset(chaK[jj], 0, fConf.NTDRS * sizeof(chaK[jj][0]));
        memset(chaS[jj], 0, fConf.NTDRS * sizeof(chaS[jj][0]));
        memset(sigK[jj], 0, fConf.NTDRS * sizeof(sigK[jj][0]));
        memset(sigS[jj], 0, fConf.NTDRS * sizeof(sigS[jj][0]));
      }
      int NTDR = dd1->GetNTdrRaw() + dd1->GetNTdrCmp();
      {
        for (int ii = 0; ii < NTDR; ii++) {
          int Jinfnum = dd1->GetJinfNum_byglobindex(ii);
          int Tdrnum = dd1->GetTdrNum_byglobindex(ii);
          int IdTDR = dd1->ComputeTdrId(Tdrnum, Jinfnum);
          for (unsigned int iCh = 0; iCh < (fConf.NVASS + fConf.NVASK) * fConf.NCHAVA; ++iCh) {
            double signal = (dd->ev)->GetRawSignal_bynums(Tdrnum, iCh, Jinfnum);
            double son = (dd->ev)->GetRawSoN_bynums(Tdrnum, iCh, Jinfnum);
            if (son != son) { son = 0; }
            int side = 0;
            if (iCh >= fConf.NCHAVA * fConf.NVASS) { side = 1; }
            LadderConf *ladderconf = LadderConf::Instance();
            if (ladderconf->GetSideSwap(Jinfnum, Tdrnum)) { side = (side==0)?1:0; }
            if (side == 1) {
              if (signal > sigK[Jinfnum][Tdrnum]) { sigK[Jinfnum][Tdrnum] = signal; sonK[Jinfnum][Tdrnum] = son; }
            } else {
              if (signal > sigS[Jinfnum][Tdrnum]) { sigS[Jinfnum][Tdrnum] = signal; sonS[Jinfnum][Tdrnum] = son; }
            }
          }
        }
      }
    };
    */

    auto fillRawArrays = [dd1, &chaK, &chaS, &sigK, &sigS, &sonK, &sonS, idx2](auto *dd) {
      std::fill(chaK.begin(), chaK.end(), 0.0);
      std::fill(chaS.begin(), chaS.end(), 0.0);
      std::fill(sigK.begin(), sigK.end(), 0.0);
      std::fill(sigS.begin(), sigS.end(), 0.0);
      std::fill(sonK.begin(), sonK.end(), 0.0);
      std::fill(sonS.begin(), sonS.end(), 0.0);

      auto fConfLocal = dd->FlavorConfig();

      int NTDR_local = dd1->GetNTdrRaw() + dd1->GetNTdrCmp();
      for (int ii = 0; ii < NTDR_local; ii++) {
        int Jinfnum = dd1->GetJinfNum_byglobindex(ii);
        int Tdrnum = dd1->GetTdrNum_byglobindex(ii);
        const int k = idx2(Jinfnum, Tdrnum);

        for (unsigned int iCh = 0; iCh < (fConfLocal.NVASS + fConfLocal.NVASK) * fConfLocal.NCHAVA; ++iCh) {

          double signal = (dd->ev)->GetRawSignal_bynums(Tdrnum, iCh, Jinfnum);
          double son = (dd->ev)->GetRawSoN_bynums(Tdrnum, iCh, Jinfnum);

          if (son != son) {
            son = 0;
          }

          int side = 0;
          if (iCh >= fConfLocal.NCHAVA * fConfLocal.NVASS) {
            side = 1;
          }

          LadderConf *ladderconf = LadderConf::Instance();
          if (ladderconf->GetSideSwap(Jinfnum, Tdrnum)) {
            side = (side == 0) ? 1 : 0;
          }

          if (side == 1) {
            if (signal > sigK[k]) {
              sigK[k] = signal;
              sonK[k] = son;
            }
          } else {
            if (signal > sigS[k]) {
              sigS[k] = signal;
              sonS[k] = son;
            }
          }
        }
      }
    };
    //***************************************************************************************************

    auto start = std::chrono::system_clock::now();

    int ret1 = 0;
    while (true) {
      if (eventstoprocess != -1 && processed == eventstoprocess)
        break;

      ret1 = dd1->EndOfFile();
      if (ret1)
        break;

      ret1 = dd1->ReadOneEvent();

      if (ret1 == 0) {
        processed++;

        if (kClusterize) {
          if (kOca) {
            fillClusterArrays(static_cast<DecodeDataOCA *>(dd1));
          } else if (kHef) {
            fillClusterArrays(static_cast<DecodeDataHEF *>(dd1));
          } else if (kFoot) {
            fillClusterArrays(static_cast<DecodeDataFOOT *>(dd1));
          }
#ifndef NOAMSL0
          else if (kL0) {
            fillClusterArrays(static_cast<DecodeDataAMSL0 *>(dd1));
          }
#endif
          else {
            fillClusterArrays(static_cast<DecodeDataAMS *>(dd1));
          }
        } else {
          if (kOca) {
            fillRawArrays(static_cast<DecodeDataOCA *>(dd1));
          } else if (kHef) {
            fillRawArrays(static_cast<DecodeDataHEF *>(dd1));
          } else if (kFoot) {
            fillRawArrays(static_cast<DecodeDataFOOT *>(dd1));
          }
#ifndef NOAMSL0
          else if (kL0) {
            fillRawArrays(static_cast<DecodeDataAMSL0 *>(dd1));
          }
#endif
          else {
            fillRawArrays(static_cast<DecodeDataAMS *>(dd1));
          }
        }

        t4->Fill();
        std::cout << "\rProcessed " << processed << " events" << std::flush;

      } else if (ret1 == -1) {
        printf("=======================> END of FILE\n");
        break;
      } else if (ret1 < -1) {
        readfailed++;
      } else {
        jinffailed++;
      }

      dd1->ClearEvent();
    }
    printf("\n");

    auto stop = std::chrono::system_clock::now();
    std::cout << "Event processing took " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
              << "ms\n";

    // CreatePdfWithPlots(dd1, pdf_filename);

    foutput->cd();
    t4->Write("", TObject::kOverwrite);
    if (dd1->GetMCTruth()) {
      TTree *mcht = dd1->GetMCTruth()->CloneTree();
      mcht->Write("", TObject::kOverwrite);
    }

    printf("\nProcessed %5d  Events\n", processed + readfailed + jinffailed);
    printf("Accepted  %5d  Events\n", processed);
    printf("Rejected  %5d  Events --> Read Error\n", readfailed);
    printf("Rejected  %5d  Events --> Jinf/Jinj Error\n", jinffailed);

  } else {

    auto GCCNs = dd1->GetCalibrationCNs();

    float *tbsCNs = new float[GCCNs.size() * GCCNs[0].size() * GCCNs[0][0].size()];
    float ***CNs;
    CNs = new float **[GCCNs.size()];
    for (long int jj = 0; jj < (long int)GCCNs.size(); jj++) {
      CNs[jj] = new float *[GCCNs[0].size()];
      for (long int tt = 0; tt < (long int)GCCNs[0].size(); tt++) {
        long int index =
            (long int)GCCNs[0].size() * (long int)GCCNs[0][0].size() * jj + (long int)GCCNs[0][0].size() * tt;
        CNs[jj][tt] = &tbsCNs[index];
      }
    }
    {
      TBranch *branch =
          t3->Branch("CNs", tbsCNs, Form("CNs[%lu][%lu][%lu]/F", GCCNs.size(), GCCNs[0].size(), GCCNs[0][0].size()));
      if (branch)
        branch->SetCompressionLevel(6);
    }

    auto GCSignals = dd1->GetCalibrationSignals();
    float *tbsSignals = new float[GCSignals.size() * GCSignals[0].size() * GCSignals[0][0].size()];
    float ***Signals;
    Signals = new float **[GCSignals.size()];
    for (long int jj = 0; jj < (long int)GCSignals.size(); jj++) {
      Signals[jj] = new float *[GCSignals[0].size()];
      for (long int tt = 0; tt < (long int)GCSignals[0].size(); tt++) {
        long int index = (long int)GCSignals[0].size() * (long int)GCSignals[0][0].size() * jj +
                         (long int)GCSignals[0][0].size() * tt;
        Signals[jj][tt] = &tbsSignals[index];
      }
    }
    {
      TBranch *branch =
          t3->Branch("Signals", tbsSignals,
                     Form("Signals[%lu][%lu][%lu]/F", GCSignals.size(), GCSignals[0].size(), GCSignals[0][0].size()));
      if (branch)
        branch->SetCompressionLevel(6);
    }

    for (long int iEv = 0; iEv < (long int)GCCNs[0][0][0].size(); iEv++) {
      for (long int jj = 0; jj < (long int)GCCNs.size(); jj++) {
        for (long int tt = 0; tt < (long int)GCCNs[0].size(); tt++) {
          for (long int vv = 0; vv < (long int)GCCNs[0][0].size(); vv++) {
            if (iEv < (long int)GCCNs[jj][tt][vv].size()) {
              CNs[jj][tt][vv] = GCCNs.at(jj).at(tt).at(vv).at(iEv);
              if (fabs(CNs[jj][tt][vv]) > 100) {
                static bool alreadyprinted = false;
                if (!alreadyprinted) {
                  printf("printing CN events greater (abs) than 100...\n");
                  alreadyprinted = true;
                }
                printf("GCCNs[%lu][%lu][%lu][%lu] = %f\n", (unsigned long)jj, (unsigned long)tt, (unsigned long)vv,
                       (unsigned long)iEv, GCCNs[jj][tt][vv][iEv]);
              }
            } else {
              CNs[jj][tt][vv] = 0.0;
            }
          }

          for (long int cc = 0; cc < (long int)GCSignals[0][0].size(); cc++) {
            if (iEv < (long int)GCSignals[jj][tt][cc].size()) {
              Signals[jj][tt][cc] = GCSignals.at(jj).at(tt).at(cc).at(iEv);
            } else {
              Signals[jj][tt][cc] = 0.0;
            }
          }
        }
      }
      t3->Fill();
    }
    foutput->cd();
    // t3->Write("", TObject::kOverwrite);
  }

  delete dd1;

  foutput->Write("", TObject::kOverwrite);
  foutput->Close("R");

  return 0;
}

void PlotsWithFits(TH1 *histo, char *name, char *title, char *pdf_filename) {

  static bool first = true; 
  char local_pdf_filename[255];

  TCanvas *canvas = new TCanvas(name, name, 1024, 1024);


  TF1 *fit_s = new TF1("fit_s", "gaus", 0, 639);
  TF1 *fit_k = new TF1("fit_k", "gaus", 640, 1023);
  fit_s->SetLineColor(kBlue);
  fit_k->SetLineColor(kRed);

  int entries = (int)(histo->GetEntries());
  if (entries >= 1) {
    TH1F *clone_chartA = (TH1F *)histo->Clone("cloneA");
    TH1F *clone_chartB = (TH1F *)histo->Clone("cloneB");

    clone_chartA->Fit(fit_s, "R");
    clone_chartB->Fit(fit_k, "R");

    clone_chartA->Draw();
    gPad->Modified();
    gPad->Update();
    TPaveStats *statA = (TPaveStats *)(clone_chartA->GetListOfFunctions()->FindObject("stats"));

    clone_chartB->Draw("SAMES");
    gPad->Modified();
    gPad->Update();
    TPaveStats *statB = (TPaveStats *)(clone_chartB->GetListOfFunctions()->FindObject("stats"));

    if (statA && statB) {
      statA->SetTextColor(kBlue);
      statB->SetTextColor(kRed);

      statA->SetX1NDC(0.12);
      statA->SetX2NDC(0.32);
      statA->SetY1NDC(0.75);

      statB->SetX1NDC(0.72);
      statB->SetX2NDC(0.92);
      statB->SetY1NDC(0.78);

      statA->Draw();
      canvas->Update();
    }

    canvas->Update();
    canvas->Modified();
    canvas->Update();
    canvas->SetTitle(title);

  
    if (!first)
      strcpy(local_pdf_filename, pdf_filename);
    else {
      sprintf(local_pdf_filename, "%s(", pdf_filename);
      first = false;
    }

    canvas->Print(local_pdf_filename, "pdf");

    if (clone_chartA) delete clone_chartA;
    if (clone_chartB) delete clone_chartB;
  }

  delete canvas;
  if (fit_s) delete fit_s;
  if (fit_k) delete fit_k;

  return;
}


void CreatePdfWithPlots(DecodeData *dd1, char *pdf_filename) {
  auto fConf = dd1->FlavorConfig();

  char local_pdf_filename[255];
  char title[255];
  char name[255];

  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1);

  for (size_t jj = 0; jj < fConf.NJINF; jj++) {
    for (size_t hh = 0; hh < fConf.NTDRS; hh++) {
      sprintf(name, "ladder %ld %ld", (long)jj, (long)hh);
      sprintf(title, "ladder %ld %ld", (long)jj, (long)hh);
      PlotsWithFits(dd1->hocc[fConf.NTDRS * jj + hh], name, title, pdf_filename);
    }
  }

  for (size_t jj = 0; jj < fConf.NJINF; jj++) {
    for (size_t hh = 0; hh < fConf.NTDRS; hh++) {
      sprintf(name, "ladder %ld %ld", (long)jj, (long)hh);
      sprintf(title, "ladder %ld %ld", (long)jj, (long)hh);
      PlotsWithFits(dd1->hoccseed[fConf.NTDRS * jj + hh], name, title, pdf_filename);
    }
  }

  for (size_t jj = 0; jj < fConf.NJINF; jj++) {
    for (size_t hh = 0; hh < fConf.NTDRS; hh++) {
      for (int ss = 0; ss < 2; ss++) {
        TCanvas *canvas = new TCanvas("dummy", "dummy", 1024, 1024);
        canvas->SetLogy(true);
        dd1->hsignal[jj * fConf.NTDRS + hh][ss]->Draw();
        int entries = (int)(dd1->hsignal[fConf.NTDRS * jj + hh][ss]->GetEntries());
        if (entries >= 1) {
          double mean = (dd1->hsignal[fConf.NTDRS * jj + hh][ss]->GetMean());
          double rms = (dd1->hsignal[fConf.NTDRS * jj + hh][ss]->GetRMS());
          (dd1->hsignal[fConf.NTDRS * jj + hh][ss])->GetXaxis()->SetRangeUser(mean - 5.0 * rms, mean + 9.0 * rms);
          canvas->Update();
          canvas->Modified();
          canvas->Update();
          canvas->Print(pdf_filename, "pdf");
        }
        delete canvas;
      }
    }
  }

  for (size_t jj = 0; jj < fConf.NJINF; jj++) {
    for (size_t hh = 0; hh < fConf.NTDRS; hh++) {
      for (int ss = 0; ss < 2; ss++) {
        TCanvas *canvas = new TCanvas("dummy", "dummy", 1024, 1024);
        canvas->SetLogy(true);
        dd1->hson[jj * fConf.NTDRS + hh][ss]->Draw();
        int entries = (int)(dd1->hson[fConf.NTDRS * jj + hh][ss]->GetEntries());
        if (entries >= 1) {
          double mean = (dd1->hson[fConf.NTDRS * jj + hh][ss]->GetMean());
          double rms = (dd1->hson[fConf.NTDRS * jj + hh][ss]->GetRMS());
          (dd1->hson[fConf.NTDRS * jj + hh][ss])->GetXaxis()->SetRangeUser(0.0, mean + 7.0 * rms);
          canvas->Update();
          canvas->Modified();
          canvas->Update();
          canvas->Print(pdf_filename, "pdf");
        }
        delete canvas;
      }
    }
  }


  TCanvas *c_exit = new TCanvas("dummy", "dummy", 1024, 1024);
  snprintf(local_pdf_filename, 255, "%s]", pdf_filename);
  c_exit->Print(local_pdf_filename, "pdf");
  delete c_exit;

  return;
}

