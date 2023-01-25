#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "anyoption.h"
#include <getopt.h>

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TPaveStats.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TTree.h"
#include <Compression.h>

#include "DecodeDataAMS.hh"
#include "DecodeDataAMSL0.hh"
#include "DecodeDataFOOT.hh"
#include "DecodeDataOCA.hh"

#include "GenericEvent.hpp"

using namespace std;

AnyOption *opt; // Handle the option input
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
  bool kFoot = false;
  bool kL0 = false;

  bool kOnlyProcessCal = false;

  bool kClusterize = false;
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

  opt = new AnyOption();

  opt->addUsage("Usage: ./Decode [options] [arguments]");
  opt->addUsage("");
  opt->addUsage("Options: ");
  opt->addUsage("  -h, --help  ................................. Print this help ");
  opt->addUsage("  --events  ................................... Number of events to decode (default is all events)");
  opt->addUsage(
      Form("  --rawdata <path/to/dir/with/raw> ............ Directory with raw data (%s is the default)", DirRaw));
  opt->addUsage(
      Form("  --caldata <path/to/dir/with/cal> ............ Directory with cal data (%s is the default)", DirCal));
  opt->addUsage(Form(
      "  --rootdata <path/to/dir/for/root> ........... Directory where to put ROOT file (%s is the default)", DirRoot));
  opt->addUsage("  --oca  ....................................... Read the OCA boards");
  opt->addUsage("  --foot ....................................... Read files from FOOT Bo TDAQ");
  opt->addUsage("  --l0 ....................................... Read files from AMSL0 DAQ");
  opt->addUsage(
      "  -c, --clusterize ............................ To perform an offline clusterization to the RAW event");
  opt->addUsage("                                                    (the bonding type is defined in ladderconf.dat");
  opt->addUsage("                                                     with the same codes as for --cworkaround)");
  opt->addUsage(" ");
  opt->addUsage("                                                To choose the clusterization thresholds: FOR ALL THE "
                "LADDERS, IGNORING THE ONES DEFINED IN ladderconf.dat:");
  opt->addUsage(
      "  --shighthreshold <X> ........................ S-side S/N high threshold. Used in the offline clusterization "
      "if option -c or to fill the plots for the ladders with raw events (3.5 is the default)");
  opt->addUsage(
      "  --slowthreshold  <X> ........................ S-side S/N low threshold. Used in the offline clusterization if "
      "option -c or to fill the plots for the ladders with raw events (1.0 is the default)");
  opt->addUsage(
      "  --khighthreshold <X> ........................ K-side S/N high threshold. Used in the offline clusterization "
      "if option -c or to fill the plots for the ladders with raw events (3.5 is the default)");
  opt->addUsage(
      "  --klowthreshold  <X> ........................ K-side S/N low threshold. Used in the offline clusterization if "
      "option -c or to fill the plots for the ladders with raw events (1.0 is the default)");
  opt->addUsage(" ");
  opt->addUsage("  --cworkaround <N> ........................... To choose the workaround clusterization:");
  opt->addUsage("                                                    FOR ALL THE LADDERS, IGNORING THE ONE DEFINED IN "
                "ladderconf.dat:");
  opt->addUsage("                                                    (option kept just for interface compatibility and "
                "to have the list of codes)");
  opt->addUsage("                                                    0 is the standard one (default)");
  opt->addUsage("                                                    1 for the Limadou monster. On S-side only even "
                "channels are bonded");
  opt->addUsage("                                                    2 for the DAMPE ladders read-out by AMS TDR. Only "
                "S-side, [0-191]+[320-511]");
  opt->addUsage("                                                    3 for the GSI ladders read-out by AMS TDR. K side "
                "has all 384 channels bonded to a single sensor");
  opt->addUsage(
      "  -m, --montecarlo ............................ To decode MonteCarlo simulation files (default is OFF)");
  opt->addUsage("  -l, --onlycal ............................... To only search and process the closer calibration run "
                "(default is OFF)");
  opt->addUsage("");
  opt->addUsage("  --calrunstart ....................................... Run/file number of calibration (first) "
                "(needed for AMSL0 or possible for (OCA, FOOT) to choose the calibration by hand)");
  opt->addUsage("  --calrunstop ....................................... Run/file number of calibration (last) (needed "
                "for AMSL0)");
  opt->addUsage("  --ancillary ....................................... Ancillary file number (only possible for AMS)");
  opt->addUsage("Arguments: ");
  opt->addUsage("  <runnum> [runstop (for AMSL0)]");

  //***********
  // set Flags
  //***********
  opt->setFlag("help", 'h');
  opt->setFlag("clusterize", 'c');
  opt->setFlag("montecarlo", 'm');
  opt->setFlag("onlycal", 'l');
  opt->setFlag("oca");
  opt->setFlag("foot");
  opt->setFlag("l0");

  //***********
  // set Options
  //***********
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

  //****************
  // Get Line Command
  //****************
  opt->processCommandArgs(argc, argv);

  //*************
  // Get Flags
  //*************
  if (opt->getFlag("help") || opt->getFlag('h')) {
    opt->printUsage();
    exit(2);
  }

  if (opt->getFlag("clusterize") || opt->getFlag('c')) {
    kClusterize = true;
  }

  if (opt->getFlag("montecarlo") || opt->getFlag('m')) {
    kMC = true;
  }

  if (opt->getFlag("onlycal") || opt->getFlag('l')) {
    kOnlyProcessCal = true;
  }

  if (opt->getFlag("oca")) {
    kOca = true;
  }

  if (opt->getFlag("foot")) {
    kFoot = true;
  }

  if (opt->getFlag("l0")) {
    kL0 = true;
  }

  //*********
  // Get Options
  //*********
  if (opt->getValue("rawdata")) {
    sprintf(DirRaw, "%s/", opt->getValue("rawdata"));
  }

  if (opt->getValue("caldata")) {
    sprintf(DirCal, "%s/", opt->getValue("caldata"));
  }

  if (opt->getValue("rootdata")) {
    sprintf(DirRoot, "%s/", opt->getValue("rootdata"));
  }

  if (opt->getValue("shighthreshold")) {
    shighthreshold = atof(opt->getValue("shighthreshold"));
  }

  if (opt->getValue("slowthreshold")) {
    slowthreshold = atof(opt->getValue("slowthreshold"));
  }

  if (opt->getValue("khighthreshold")) {
    khighthreshold = atof(opt->getValue("khighthreshold"));
  }

  if (opt->getValue("klowthreshold")) {
    klowthreshold = atof(opt->getValue("klowthreshold"));
  }

  if (opt->getValue("cworkaround")) {
    cworkaround = atoi(opt->getValue("cworkaround"));
  }

  if (opt->getValue("calrunstart")) {
    calrunstart = atoi(opt->getValue("calrunstart"));
  }

  if (opt->getValue("calrunstop")) {
    calrunstop = atoi(opt->getValue("calrunstop"));
  }

  if (opt->getValue("ancillary")) {
    ancillary = atoi(opt->getValue("ancillary"));
  }

  //  eventstoprocess
  if (opt->getValue("events")) {
    eventstoprocess = atoi(opt->getValue("events"));
  }

  //  printf("%d %f %f %f %f\n", kClusterize, shighthreshold, slowthreshold, khighthreshold, klowthreshold);

  //*************
  // Get Arguments
  //************

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

  if (ancillary < 0)
    sprintf(filename, "%s/run_%06d.root", DirRoot, run);
  else
    sprintf(filename, "%s/run_%06d_ANC_%d.root", DirRoot, run, ancillary);
  sprintf(pdf_filename, "%s.pdf", filename);

  //  int complevel=ROOT::CompressionSettings(ROOT::kLZMA, 2);
  int complevel = ROOT::CompressionSettings(ROOT::kZLIB, 2);
  printf("The choosen compression level is %d\n", complevel);
  TFile *foutput = new TFile(filename, "RECREATE", "File with the event tree", complevel);

  /// VV debug
  TTree *t4 = new TTree("t4", "My cluster tree");

  DecodeData *dd1 = nullptr;
  FlavorConfig fConf;
  if (kOca) {
    auto *dd = new DecodeDataOCA(DirRaw, DirCal, run, calrunstart);
    fConf = dd->FlavorConfig();
    t4->Branch("cluster_branch", dd->EventClassname(), &(dd->ev), 64000, 2);
    dd1 = static_cast<DecodeData *>(dd);
  } else if (kFoot) {
    auto *dd = new DecodeDataFOOT(DirRaw, DirCal, run, calrunstart);
    fConf = dd->FlavorConfig();
    t4->Branch("cluster_branch", dd->EventClassname(), &(dd->ev), 64000, 2);
    dd1 = static_cast<DecodeData *>(dd);
  } else if (kL0) {
    auto *dd = new DecodeDataAMSL0(DirRaw, DirCal, run, runstop, calrunstart, calrunstop);
    fConf = dd->FlavorConfig();
    t4->Branch("cluster_branch", dd->EventClassname(), &(dd->ev), 64000, 2);
    dd1 = static_cast<DecodeData *>(dd);
  } else {
    auto *dd = new DecodeDataAMS(DirRaw, DirCal, run, ancillary, kMC);
    fConf = dd->FlavorConfig();
    t4->Branch("cluster_branch", dd->EventClassname(), &(dd->ev), 64000, 2);
    dd1 = static_cast<DecodeData *>(dd);
  }

  if (!kOnlyProcessCal) {
    dd1->shighthreshold = shighthreshold;
    dd1->slowthreshold = slowthreshold;
    dd1->khighthreshold = khighthreshold;
    dd1->klowthreshold = klowthreshold;
    dd1->kClusterize = kClusterize;
    dd1->cworkaround = cworkaround;

    dd1->SetPrintOff();
    dd1->SetEvPrintOff();

    /// VV debug moved before dd1
    // TTree* t4= new TTree("t4","My cluster tree");
    /// VV debug given 64000
    // t4->Branch("cluster_branch","Event",&(dd1->ev),32000,2);
    double chaK[fConf.NTDRS];
    double chaS[fConf.NTDRS];
    double sigK[fConf.NTDRS];
    double sigS[fConf.NTDRS];
    double sonK[fConf.NTDRS];
    double sonS[fConf.NTDRS];

    int NTDR = dd1->GetNTdrRaw() + dd1->GetNTdrCmp();
    for (int ii = 0; ii < NTDR; ii++) {
      int IdTDR = dd1->GetTdrNum(ii);
      //      printf("%d\n", IdTDR);
      t4->Branch(Form("SignalS_Ladder%02d", IdTDR), &sigS[IdTDR], Form("SignalS_Ladder%02d/D", IdTDR));
      t4->Branch(Form("ChargeS_Ladder%02d", IdTDR), &chaS[IdTDR], Form("ChargeS_Ladder%02d/D", IdTDR));
      t4->Branch(Form("SoNS_Ladder%02d", IdTDR), &sonS[IdTDR], Form("SoNS_Ladder%02d/D", IdTDR));
      t4->Branch(Form("SignalK_Ladder%02d", IdTDR), &sigK[IdTDR], Form("SignalK_Ladder%02d/D", IdTDR));
      t4->Branch(Form("ChargeK_Ladder%02d", IdTDR), &chaK[IdTDR], Form("ChargeK_Ladder%02d/D", IdTDR));
      t4->Branch(Form("SoNK_Ladder%02d", IdTDR), &sonK[IdTDR], Form("SoNK_Ladder%02d/D", IdTDR));
    }
    sleep(3);

    if (!kFoot) { // not yet implemented
      auto *ddams = dynamic_cast<DecodeDataAMS *>(dd1);
      auto *ddoca = dynamic_cast<DecodeDataOCA *>(dd1);
      auto *ddamsl0 = dynamic_cast<DecodeDataAMSL0 *>(dd1);
      if (ddams) {
        t4->GetUserInfo()->Add(ddams->rh);
      } else if (ddoca) {
        t4->GetUserInfo()->Add(ddoca->rh);
      } else if (ddamsl0) {
        t4->GetUserInfo()->Add(ddamsl0->rh);
      } else {
        throw std::runtime_error("DecodeData object is not of type DecodeDataAMS nor DecodeDataOCA...");
      }
    }
    //    LadderConf::Instance()->Dump();
    t4->GetUserInfo()->Add(LadderConf::Instance()->GetLadderParamsMap());

    TObjArray *obj = t4->GetListOfBranches();
    for (int ii = 0; ii < obj->GetEntries(); ii++) {
      TBranch *branch = (TBranch *)(obj->At(ii));
      branch->SetCompressionLevel(6);
    }

    auto fillClusterArrays = [&chaK, &chaS, &sigK, &sigS, &sonK, &sonS](auto *dd) {
      auto fConf = dd->FlavorConfig();

      memset(chaK, 0, fConf.NTDRS * sizeof(chaK[0]));
      memset(chaS, 0, fConf.NTDRS * sizeof(chaS[0]));
      memset(sigK, 0, fConf.NTDRS * sizeof(sigK[0]));
      memset(sigS, 0, fConf.NTDRS * sizeof(sigS[0]));

      for (int cc = 0; cc < (dd->ev)->GetNClusTot(); cc++) {
        //      printf("This event has %d clusters\n", (dd->ev)->GetNClusTot());
        // printf("This event has CALPED %f\n", (dd->ev)->GetCalPed_PosNum(1,0,0));

        Cluster *cl = (dd->ev)->GetCluster(cc);
        int ladder = cl->ladder;
        //	printf("%d\n", ladder);
        double signal = cl->GetTotSig();
        // if (signal>4095) {
        //   printf("event %d, cluster %d, side %d), address %d, signal %f\n", (dd->ev)->GetEvtnum(), cc, cl->side,
        //   cl->address, signal); sleep(1);
        // }
        double charge = cl->GetCharge();
        double son = cl->GetTotSN();
        if (cl->side == 1) {
          if (charge > chaK[ladder]) { // filling only with the largest
            chaK[ladder] = charge;
            sigK[ladder] = signal;
            sonK[ladder] = son;
          }
        } else {
          if (charge > chaS[ladder]) { // filling only with the largest
            chaS[ladder] = charge;
            sigS[ladder] = signal;
            sonS[ladder] = son;
          }
        }
      }
    };

    auto fillRawArrays = [&chaK, &chaS, &sigK, &sigS, &sonK, &sonS](auto *dd) {
      auto fConf = dd->FlavorConfig();

      memset(chaK, 0, fConf.NTDRS * sizeof(chaK[0]));
      memset(chaS, 0, fConf.NTDRS * sizeof(chaS[0]));
      memset(sigK, 0, fConf.NTDRS * sizeof(sigK[0]));
      memset(sigS, 0, fConf.NTDRS * sizeof(sigS[0]));

      for (unsigned int iJinf = 0; iJinf < fConf.NJINF; ++iJinf) {
        for (unsigned int iTDR = 0; iTDR < fConf.NTDRS; ++iTDR) {
          for (unsigned int iCh = 0; iCh < (fConf.NVASS + fConf.NVASK) * fConf.NCHAVA; ++iCh) {
            // if ((dd->ev)->GetRawSignal_PosNum(iTDR, iCh, iJinf)>4095) {
            //   printf("event = %d, Jinf = %d, TDR = %d, Channel = %d) rawsignal = %f\n", (dd->ev)->GetEvtnum(), iJinf,
            //   iTDR, iCh, (dd->ev)->GetRawSignal_PosNum(iTDR, iCh, iJinf)); sleep(1);
            // }

            int ladder = iTDR + 100 * iJinf;
            double signal = (dd->ev)->GetRawSignal_PosNum(iTDR, iCh, iJinf);
            double son = (dd->ev)->GetRawSoN_PosNum(iTDR, iCh, iJinf);
            if (son != son) { // NaN, not a ladder really present
              son = 0;
            }
            //	    printf("%d %f %f\n", ladder, signal, son);

            int side = 0;
            if (iCh >= fConf.NCHAVA * fConf.NVASS) {
              side = 1;
            }

            LadderConf *ladderconf = LadderConf::Instance();
            if (ladderconf->GetSideSwap(iJinf, iTDR)) {
              if (side == 0) {
                side = 1;
              } else {
                side = 0;
              }
            }

            if (side == 1) {
              if (signal > sigK[ladder]) { // filling only with the largest
                sigK[ladder] = signal;
                sonK[ladder] = son;
              }
            } else {
              if (signal > sigS[ladder]) { // filling only with the largest
                sigS[ladder] = signal;
                sonS[ladder] = son;
              }
            }
          }
        }
      }
    };

    int ret1 = 0;
    while (true) {
      if (eventstoprocess != -1 && processed == eventstoprocess)
        break;

      ret1 = dd1->EndOfFile();
      if (ret1)
        break;

      ret1 = dd1->ReadOneEvent();

      // if(processed > 10){ break; }

      //    printf("%d\n", ret1);

      /// VV debug commented out
      // ret1=dd1->EndOfFile();
      // if (ret1) break;
      // if(processed==4 || processed==5){processed++;continue;}
      if (ret1 == 0) {
        processed++;

        if (kClusterize) {
          if (kOca) {
            fillClusterArrays(static_cast<DecodeDataOCA *>(dd1));
          } else if (kFoot) {
            fillClusterArrays(static_cast<DecodeDataFOOT *>(dd1));
          } else if (kL0) {
            fillClusterArrays(static_cast<DecodeDataAMSL0 *>(dd1));
          } else {
            fillClusterArrays(static_cast<DecodeDataAMS *>(dd1));
          }
        } else {
          if (kOca) {
            fillRawArrays(static_cast<DecodeDataOCA *>(dd1));
          } else if (kFoot) {
            fillRawArrays(static_cast<DecodeDataFOOT *>(dd1));
          } else if (kL0) {
            fillRawArrays(static_cast<DecodeDataAMSL0 *>(dd1));
          } else {
            fillRawArrays(static_cast<DecodeDataAMS *>(dd1));
          }
        }

        //      printf("%f %f %f %f %f %f\n", sigS[0], sigK[0], sigS[1], sigK[1], sigS[4], sigK[4]);

        t4->Fill();
        if (processed % 1000 == 0) {
          printf("\rProcessed %d events...", processed);
          fflush(stdout);
        }
      } else if (ret1 == -1) {
        printf("=======================> END of FILE\n");
        break;
      } else if (ret1 < -1) {
        printf("=======================> READ Error Event Skipped\n");
        readfailed++;
        break;
      } else {
        jinffailed++;
      }

      dd1->ClearEvent();
    }
    printf("\n");

    // CreatePdfWithPlots(dd1, pdf_filename);
    /// VV debug write to output file
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
    if (clone_chartA)
      delete clone_chartA;
    if (clone_chartB)
      delete clone_chartB;
  }
  delete canvas;
  if (fit_s)
    delete fit_s;
  if (fit_k)
    delete fit_k;

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
      sprintf(name, "ladder %ld %ld", jj, hh);
      sprintf(title, "ladder %ld %ld", jj, hh);
      PlotsWithFits(dd1->hocc[fConf.NTDRS * jj + hh], name, title, pdf_filename);
    }
  }

  for (size_t jj = 0; jj < fConf.NJINF; jj++) {
    for (size_t hh = 0; hh < fConf.NTDRS; hh++) {
      sprintf(name, "ladder %ld %ld", jj, hh);
      sprintf(title, "ladder %ld %ld", jj, hh);
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
          //	  printf("%f %f\n", mean, rms);
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
          //	  printf("%f %f\n", mean, rms);
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
