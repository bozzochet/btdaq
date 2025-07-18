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
  bool kL0old = false;
  bool kPri = false;
  bool kEvPri = false;

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

  auto opt = std::make_unique<AnyOption>(); // Handle the option input

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
  opt->addUsage("  --foot ...................................... Read files from FOOT Bo TDAQ");
  opt->addUsage("  --l0 ........................................ Read files from AMSL0 DAQ (USB-LF and v>=5)");
  opt->addUsage("  --l0old ..................................... Read files from AMSL0 DAQ (USB-LEF)");
  opt->addUsage(
      "  -c, --clusterize ............................ To perform an offline clusterization to the RAW event");
  opt->addUsage("                                                (the bonding type is defined in ladderconf.dat");
  opt->addUsage("                                                with the same codes as for --cworkaround)");
  opt->addUsage("  --calrunstart ............................... Run/file number of calibration (first)\n"
                "                                                (needed for AMSL0 or possible for (OCA, FOOT) to\n"
                "                                                choose the calibration by hand)");
  opt->addUsage(
      "  --calrunstop ................................ Run/file number of calibration (last) (needed for AMSL0)");
  opt->addUsage("  --ancillary ................................. Ancillary file number (only possible for AMS)");
  opt->addUsage(
      "  -m, --montecarlo ............................ To decode MonteCarlo simulation files (default is OFF)");
  opt->addUsage(
      "  -l, --onlycal ............................... To only search and process the (closer automatically\n"
      "                                                chosen for AMS, OCA and FOOT, but not AMSL0) calibration run.\n"
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

  //***********
  // set Flags
  //***********
  opt->setFlag("help", 'h');
  opt->setFlag("pri");
  opt->setFlag("evpri");
  opt->setFlag("clusterize", 'c');
  opt->setFlag("montecarlo", 'm');
  opt->setFlag("onlycal", 'l');
  opt->setFlag("oca");
  opt->setFlag("foot");
  opt->setFlag("l0");
  opt->setFlag("l0old");

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

  if (opt->getFlag("pri")) {
    kPri = true;
  }

  if (opt->getFlag("evpri")) {
    kEvPri = true;
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

  if (opt->getFlag("l0old")) {
    kL0old = true;
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

  if (ancillary < 0) {
    sprintf(filename, "%s/run_%06d.root", DirRoot, run);
    if (kOnlyProcessCal) {
      sprintf(filename, "%s/run_%06d_ONLYCAL.root", DirRoot, run);
    }
  } else
    sprintf(filename, "%s/run_%06d_ANC_%d.root", DirRoot, run, ancillary);
  sprintf(pdf_filename, "%s.pdf", filename);

  //  int complevel = ROOT::CompressionSettings(ROOT::kLZMA, 8); // MD: gain factor 2 in size, but lose factor 4 in
  //  speed int complevel = ROOT::CompressionSettings(ROOT::kLZMA, 2); // MD: the complevel seems that doesn't really
  //  matter...
  //  int complevel = ROOT::CompressionSettings(ROOT::kZLIB, 2);
  int complevel = ROOT::CompressionSettings(ROOT::RCompressionSetting::EAlgorithm::kZLIB, 2);
  printf("The choosen compression level is %d\n", complevel);
  TFile *foutput = new TFile(filename, "RECREATE", "File with the event tree", complevel);

  TTree *t3 = new TTree("t3", "My calibration tree");
  TTree *t4 = NULL;
  if (!kOnlyProcessCal) {
    t4 = new TTree("t4", "My cluster tree");
  }

  // int bufsize = 64000;
  // int splitlevel = 2;
  int bufsize = 32000;
  int splitlevel = 99;

  DecodeData *dd1 = nullptr;
  FlavorConfig fConf;
  if (kOca) {
    auto *dd = new DecodeDataOCA(DirRaw, DirCal, run, calrunstart, kOnlyProcessCal);
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
  } else if (kL0 || kL0old) {
    kL0 = true; // in the kL0old case we set also kL0 since now the right DecodeStyle is set already
    auto *dd =
        new DecodeDataAMSL0(DirRaw, DirCal, run, runstop, calrunstart, calrunstop, kL0old ? 0 : 1, kOnlyProcessCal);
    fConf = dd->FlavorConfig();
    if (!kOnlyProcessCal)
      t4->Branch("cluster_branch", dd->EventClassname(), &(dd->ev), bufsize, splitlevel);
    auto calibs = dd->GetCalibrations();
    foutput->WriteTObject(&calibs, "cals");
    // printf("******* %f\n", calibs.at(0).at(0).ped[0]);
    // printf("******* %f\n", calibs.at(1).at(0).ped[0]);
    // printf("calibs.at(0).at(0).ped[0]): %p\n", &(calibs.at(0).at(0).ped[0]));
    // printf("calibs.at(1).at(0).ped[0]): %p\n", &(calibs.at(1).at(0).ped[0]));
    dd1 = static_cast<DecodeData *>(dd);
  } else {
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
    //    sleep(3);

    auto *ddams = dynamic_cast<DecodeDataAMS *>(dd1);
    auto *ddoca = dynamic_cast<DecodeDataOCA *>(dd1);
    auto *ddfoot = dynamic_cast<DecodeDataFOOT *>(dd1);
    auto *ddamsl0 = dynamic_cast<DecodeDataAMSL0 *>(dd1);
    if (ddams) {
      t4->GetUserInfo()->Add(ddams->rh);
    } else if (ddoca) {
      t4->GetUserInfo()->Add(ddoca->rh);
    } else if (ddfoot) {
      t4->GetUserInfo()->Add(ddfoot->rh);
    } else if (ddamsl0) {
      t4->GetUserInfo()->Add(ddamsl0->rh);
    } else {
      throw std::runtime_error("DecodeData object is not of type DecodeDataAMS nor DecodeDataOCA...");
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

    auto fillRawArrays = [dd1, &chaK, &chaS, &sigK, &sigS, &sonK, &sonS](auto *dd) {
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

            int ladder = dd1->ComputeTdrNum(iTDR, iJinf);
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

    auto start = std::chrono::system_clock::now();

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
    //    printf("GCCNs = %p\n", &GCCNs);
    // tbsCNS allocated as 'flat' array so that the memory allocation is contiguous in memory, as required by
    // TTree::Branch() another solution could be to allocate all the "pointer" components of CNs and only after (i.e.
    // restarting the nested loops) allocate the CNs[jj][tt] one after the other to have all of them contiguous
    // in the used solution is important to pass tbsCNs to Branch, not CNs
    // in the alternative solution, however, we should use &CNs[0][0][0] and not CNs (the first block in memory are just
    // pointers) that are clearly not equivalent
    // (https://root-forum.cern.ch/t/adding-a-branch-of-multidimensional-array-to-ttree/29083 only applies for an array
    // in the stack, not in the heap
    float *tbsCNs = new float[GCCNs.size() * GCCNs[0].size() * GCCNs[0][0].size()];
    float ***CNs;
    CNs = new float **[GCCNs.size()];
    for (long int jj = 0; jj < GCCNs.size(); jj++) {
      //      printf("GCCNs[%lu] = %p\n", jj, &GCCNs[jj]);
      CNs[jj] = new float *[GCCNs[0].size()];
      for (long int tt = 0; tt < GCCNs[0].size(); tt++) {
        //        printf("GCCNs[%lu][%lu] = %p\n", jj, tt, &GCCNs[jj][tt]);
        //        CNs[jj][tt] = new float[GCCNs[0][0].size()];
        //        printf("Allocating %lu floats for CNs[%lu][%lu]...\n", GCCNs[0][0].size(), jj, tt);
        long int index = GCCNs[0].size() * GCCNs[0][0].size() * jj + GCCNs[0][0].size() * tt;
        //        printf("[%lu][%lu] = %lu*%lu*%lu + %lu*%lu = [%lu]\n", jj, tt, GCCNs[0].size(), GCCNs[0][0].size(),
        //        jj, GCCNs[0][0].size(), tt, index);
        CNs[jj][tt] = &tbsCNs[index];
      }
    }
    {
      TBranch *branch =
          //	t3->Branch("CNs", CNs, Form("CNs[%lu][%lu][%lu]/F", GCCNs.size(), GCCNs[0].size(), GCCNs[0][0].size()));
          t3->Branch("CNs", tbsCNs, Form("CNs[%lu][%lu][%lu]/F", GCCNs.size(), GCCNs[0].size(), GCCNs[0][0].size()));
      if (branch)
        branch->SetCompressionLevel(6);
    }

    auto GCSignals = dd1->GetCalibrationSignals();
    float *tbsSignals = new float[GCSignals.size() * GCSignals[0].size() * GCSignals[0][0].size()];
    float ***Signals;
    Signals = new float **[GCSignals.size()];
    for (long int jj = 0; jj < GCSignals.size(); jj++) {
      Signals[jj] = new float *[GCSignals[0].size()];
      for (long int tt = 0; tt < GCSignals[0].size(); tt++) {
        //        Signals[jj][tt] = new float[GCSignals[0][0].size()];
        long int index = GCSignals[0].size() * GCSignals[0][0].size() * jj + GCSignals[0][0].size() * tt;
        Signals[jj][tt] = &tbsSignals[index];
      }
    }
    {
      TBranch *branch =
          //	t3->Branch("Signals", Signals,
          t3->Branch("Signals", tbsSignals,
                     Form("Signals[%lu][%lu][%lu]/F", GCSignals.size(), GCSignals[0].size(), GCSignals[0][0].size()));
      if (branch)
        branch->SetCompressionLevel(6);
    }

    /*
    printf("%lu\n", GCCNs[0][0][0].size());
    printf("%lu\n", GCSignals[0][0][0].size());
    printf("%lu\n", GCCNs[0][0].size());
    printf("%lu\n", GCSignals[0][0].size());
    printf("%lu\n", GCCNs[0].size());
    printf("%lu\n", GCSignals[0].size());
    printf("%lu\n", GCCNs.size());
    printf("%lu\n", GCSignals.size());
    */

    for (long int iEv = 0; iEv < GCCNs[0][0][0].size(); iEv++) {
      for (long int jj = 0; jj < GCCNs.size(); jj++) {
        for (long int tt = 0; tt < GCCNs[0].size(); tt++) {
          for (long int vv = 0; vv < GCCNs[0][0].size(); vv++) {
            if (iEv < GCCNs[jj][tt][vv].size()) {
              CNs[jj][tt][vv] = GCCNs.at(jj).at(tt).at(vv).at(iEv);
              if (fabs(CNs[jj][tt][vv]) > 100) {
                printf("GCCNs[%lu][%lu][%lu][%lu] = %f\n", jj, tt, vv, iEv, GCCNs[jj][tt][vv][iEv]);
                printf("CNs[%lu][%lu][%lu] = %f\n", jj, tt, vv, CNs[jj][tt][vv]);
              }
            } else {
              CNs[jj][tt][vv] = 0.0;
              // if (iEv == 0)
              //   printf("%lu %lu %lu\n", jj, tt, vv);
            }
          }
          for (long int cc = 0; cc < GCSignals[0][0].size(); cc++) {
            if (iEv < GCSignals[jj][tt][cc].size()) {
              Signals[jj][tt][cc] = GCSignals.at(jj).at(tt).at(cc).at(iEv);
              // if (cc == 0) {
              //   printf("Signals[%lu][%lu][%lu] = %f\n", jj, tt, cc, Signals[jj][tt][cc]);
              //   printf("GCSignals[%lu][%lu][%lu][%lu] = %f\n", jj, tt, cc, iEv,
              //   GCSignals.at(jj).at(tt).at(cc).at(iEv));
              // }
            } else {
              Signals[jj][tt][cc] = 0.0;
              // if (iEv == 0)
              //   printf("%lu %lu %lu\n", jj, tt, cc);
            }
          }
        }
      }

      /*
      if (iEv == 0) {
        for (long int jj = 0; jj < GCCNs.size(); jj++) {
          for (long int tt = 0; tt < GCCNs[0].size(); tt++) {
            for (long int vv = 0; vv < GCCNs[0][0].size(); vv++) {
              long int index = GCCNs[0].size() * GCCNs[0][0].size() * jj + GCCNs[0][0].size() * tt + vv;
              float pointer = CNs[jj][tt][vv];
              float direct = tbsCNs[index];
              double diff = pointer - direct;
              if (fabs(diff) > 1.0E-10) {
                printf("CNs[%lu][%lu][%lu] = %f\n", jj, tt, vv, pointer);
                printf("tbsCNs[%lu] = %f\n", index, direct);
              }
            }
          }
        }
      }
      */

      t3->Fill();
    }
    foutput->cd();
    //    t3->Write("", TObject::kOverwrite);
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
