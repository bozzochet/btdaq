#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string_view>
#include <sys/stat.h>
#include <sys/time.h>

#include <string_view>
//----------------ROOT includes---------------------------
#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLatex.h"
#include "TLine.h"
#include "TNtuple.h"
#include "TPad.h"
#include "TROOT.h"
#include "TString.h"
#include "TStyle.h"
#include "TText.h"

#include "anyoption.h"

#define NTDR 24  // maximum number of TDR
#define NJINF 24 // maximum number of JINF

void LinesVas(int nva = 16, int nch_for_va = 64);
void InitStyle();

using namespace std;

AnyOption *opt; // Handle the option input

class ConfPars {

public:
  char CALPATH[255];
  unsigned int TDRrefmask;
  unsigned int JINFrefmask;
  int flavour; // 0: AMS, 1: FOOT; 2: OCA, 3: AMSL0, 4: GSI2
  double PedYLim;
  ConfPars();
};

ConfPars::ConfPars() {
  sprintf(CALPATH, "./");
  flavour = 0;
  TDRrefmask = 0;
  JINFrefmask = 0;
  PedYLim = 2000.0;
}

ConfPars *CPars;

bool mute = false;

void ShowHelp(char *cmd);
void RefMask(int run_number, int jinfnum, std::string_view nameprefixin);
Int_t Summary(char *dir, int run_number, int jinfnum, int tdr_number, const char *nameprefixin, const char *nameout,
              const char *outkind, bool donotupdate = false);
Int_t Summary(char *filename, const char *nameout, const char *outkind, bool donotupdate = false);
int SummaryComplete(char *dir, int run_number, int jinfnum, std::string_view outkind, std::string_view nameprefix);

//--------------------------------------------------------------------------------------------
//                              Here comes the main...
//-------------------------------------------------------------------------------------------
int main(int argc, char **argv) {

  int ret = 0;

  TString _name = argv[0];
  if (_name.Contains("Mute"))
    mute = true;

  TApplication *myapp = NULL;
  if (!mute)
    myapp = new TApplication("myapp", &argc, argv);

  CPars = new ConfPars();
  //-------------------------------------------------
  //              COMMANDS
  //-------------------------------------------------

  opt = new AnyOption();

  opt->addUsage(Form("Usage: %s [options] <run #>", argv[0]));
  opt->addUsage("");
  opt->addUsage("Options: ");
  opt->addUsage("  -h, --help  ................................. Print this help");
  opt->addUsage("  --l0       ................................. AMSL0");
  opt->addUsage("  --foot      ................................. FOOT");
  opt->addUsage("  --oca       ................................. OCA");
  opt->addUsage("  --gsi2       ................................. GSI2");
  opt->addUsage(
      "  --outtype   ................................. Type of file: everything for pdf, \"ps\" for PostScript)");
  opt->addUsage("  --prefix    ................................. Name prefix (\"\" is the default)");
  opt->addUsage("  --caldir,   ................................. Cal dir (\"./CalData\" is the default)");

  //***********
  // set Flags
  //***********
  opt->setFlag("help", 'h');
  opt->setFlag("l0");
  opt->setFlag("oca");
  opt->setFlag("gsi2");
  opt->setFlag("foot");
  // opt->setFlag("pippo", 'p');

  //***********
  // set Options
  //***********
  opt->setOption("prefix");
  opt->setOption("caldir");
  opt->setOption("outtype");

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

  int flavourset = false;

  if (!flavourset) {
    if (opt->getFlag("l0")) {
      flavourset = true;
      CPars->flavour = 3;
    }
    if (opt->getFlag("oca")) {
      flavourset = true;
      CPars->flavour = 2;
    }
    if (opt->getFlag("foot")) {
      flavourset = true;
      CPars->flavour = 1;
    }
    if (opt->getFlag("gsi2")) {
      flavourset = true;
      CPars->flavour = 4;
    }
  } else {
    printf("You set more than one kind of detector...\n");
    printf("Setting back to the default: AMS...\n");
  }

  //*********
  // Get Options
  //*********
  char type[255];
  sprintf(type, "pdf");
  if (opt->getValue("outtype")) {
    TString _ot = opt->getValue("outtype");
    if (_ot == "ps") {
      sprintf(type, "ps");
    }
  }

  char caldir[255] = "./CalData";
  if (opt->getValue("caldir")) {
    sprintf(caldir, "%s/", opt->getValue("caldir"));
  }
  sprintf(CPars->CALPATH, "%s", caldir);
  //  printf("-- %s --\n", CPars->CALPATH);

  char nameprefix[255] = "";
  if (opt->getValue("prefix")) {
    sprintf(nameprefix, "%s/", opt->getValue("prefix"));
  }

  switch (opt->getArgc()) {
  case 1:
    for (int ii = 0; ii < NJINF; ii++) {
      RefMask(atoi(opt->getArgv(0)), ii, nameprefix);
      ret = SummaryComplete(caldir, atoi(opt->getArgv(0)), ii, type, nameprefix);
    }
    ret = 0;
    break;

  default:
    opt->printUsage();
    exit(-1);
    break;
  }

  if (!mute)
    myapp->Run();

  delete CPars;

  return ret;
}

void RefMask(int run_number, int jinfnum, std::string_view nameprefixin) {

  CPars->TDRrefmask = 0;
  CPars->JINFrefmask = 0;

  char calfileprefix[512];

  //  char prefix[256];
  //
  //  sprintf(prefix, "%s", nameprefixin);
  //  if (prefix[0] != 0)
  //    sprintf(prefix, "%s_", prefix);

  std::string prefix_s{nameprefixin};
  if (!prefix_s.empty()) {
    prefix_s += '_';
  }

  char *prefix = prefix_s.data();

  //  printf("**** flavour = %d\n", CPars->flavour);

  if (CPars->flavour == 3) { // AMSL0
    // First 4 digits: dir number
    unsigned int dirNum = run_number / 1000;
    // Last 4 digits: block number
    unsigned int blockNum = run_number % 1000;
    sprintf(calfileprefix, "%s/%s%04d/%03d", CPars->CALPATH, prefix, dirNum, blockNum);
    CPars->PedYLim = 10000.0;
  } else {
    //  sprintf(calfileprefix,"%s/%s%06d", CPars->CALPATH, prefix, run_number);
    sprintf(calfileprefix, "%s/%s%d", CPars->CALPATH, prefix, run_number);
  }

  //  printf("calfileprefix: %s\n", calfileprefix);

  char calfilename[530];

  for (int ii = 0; ii < NTDR; ii++) {
    sprintf(calfilename, "%s_%02d%02d.cal", calfileprefix, jinfnum, ii);
    struct stat buf;
    if (stat(calfilename, &buf) == 0) {
      CPars->TDRrefmask = CPars->TDRrefmask | 1 << ii;
      CPars->JINFrefmask = CPars->JINFrefmask | 1 << jinfnum;
      printf("I,ve found %s for TDR number %d\n", calfilename, ii);
    }
  }
  return;
}

int SummaryComplete(char *dir, int run_number, int jinfnum, std::string_view outkind, std::string_view nameprefix) {
  int ret = 0;

  if (CPars->JINFrefmask & (1 << jinfnum)) {
    int lasttdr = 0;
    int tdrcount = 0;

    char calfileprefix[255];
    char nameouttemp[384];
    char nameout[264];
    std::string nameprefixtemp{"summary-"};

    if (nameprefix.empty()) {
      nameprefixtemp += '_';
    } else {
      nameprefixtemp += nameprefix;
    }

    if (CPars->flavour == 3) { // AMSL0
      // First 4 digits: dir number
      unsigned int dirNum = run_number / 1000;
      // Last 4 digits: block number
      unsigned int blockNum = run_number % 1000;
      snprintf(calfileprefix, 255, "%s/%04d/%s%02d_%03d", dir, dirNum, nameprefixtemp.c_str(), jinfnum, blockNum);
    } else {
      //  sprintf(calfileprefix,"%s/%s%02d_%06d", dir, nameprefixtemp, jinfnum, run_number);
      snprintf(calfileprefix, 255, "%s/%s%02d_%d", dir, nameprefixtemp.c_str(), jinfnum, run_number);
    }
    //    printf("calfileprefix: %s\n", calfileprefix);
    snprintf(nameout, 264, "%s.cal.%s", calfileprefix, outkind.data());
    //    printf("nameout: %s\n", nameout);

    //  sprintf(systemcommand,"evince %s &",nameout);

    for (int ii = 0; ii < NTDR; ii++) {
      //    LPRINTF("%d : %d\n", ii, CPars->TDRrefmask&(1<<ii)); //only for debug
      if (CPars->TDRrefmask & (1 << ii)) {
        tdrcount++;
      }
    }
    if (tdrcount != 0)
      printf("We have %d file of calibrations for run #%d, Jinf #%d, in the directory \"%s\" with prefix name \"%s\"\n",
             tdrcount, run_number, jinfnum, dir, nameprefix.data());

    for (int ii = 0; ii < NTDR; ii++) {
      //    LPRINTF("%d : %d\n", ii, CPars->TDRrefmask&(1<<ii)); //only for debug
      if (CPars->TDRrefmask & (1 << ii)) {
        lasttdr++;
        printf("Summarizing %dth calibration file...\n", lasttdr);
        if (lasttdr == tdrcount) {
          printf("last TDR (# %d)..\n", ii);
          snprintf(nameouttemp, 384, "%s)", nameout);
        } else if (lasttdr == 1) {
          snprintf(nameouttemp, 384, "%s(", nameout);
          printf("first TDR (# %d)...\n", ii);
        } else {
          snprintf(nameouttemp, 384, "%s", nameout);
          printf("n-th TDR (# %d)...\n", ii);
        }
        bool donotupdate = false;
        if (CPars->flavour == 4) { // GSI2
          if (lasttdr % 2 == 0) {
            donotupdate = true;
          }
          //          printf("%d -> %d\n", lasttdr, donotupdate);
        }
        ret = Summary(dir, run_number, jinfnum, ii, nameprefix.data(), nameouttemp, outkind.data(), donotupdate);
      }
    }
    //  struct stat buff;
    // if (!mute) {
    //   if (stat(nameout,&buff)==0) system(systemcommand);
    // }
  }

  return ret;
}

Int_t Summary(char *dir, int run_number, int jinfnum, int tdr_number, const char *nameprefixin, const char *nameout,
              const char *outkind, bool donotupdate) {
  int ret = 0;

  char calfileprefix[512];
  Char_t filename[530];
  char nameprefixintemp[255];

  if (nameprefixin[0] != 0)
    sprintf(nameprefixintemp, "%s_", nameprefixin);
  else
    sprintf(nameprefixintemp, "%s", nameprefixin);

  if (CPars->flavour == 3) { // AMSL0
    // First 4 digits: dir number
    unsigned int dirNum = run_number / 1000;
    // Last 4 digits: block number
    unsigned int blockNum = run_number % 1000;
    sprintf(calfileprefix, "%s/%s%04d/%03d", CPars->CALPATH, nameprefixin, dirNum, blockNum);
  } else {
    //  sprintf(calfileprefix,"%s/%s%06d", dir, nameprefixintemp, run_number);
    sprintf(calfileprefix, "%s/%s%d", dir, nameprefixintemp, run_number);
  }

  sprintf(filename, "%s_%02d%02d.cal", calfileprefix, jinfnum, tdr_number);
  //  printf("filename: %s\n", filename);

  ret = Summary(filename, nameout, outkind, donotupdate);

  return ret;
}

Int_t Summary(char *filename, const char *nameout, const char *outkind, bool donotupdate) {

  static int count = 0;

  InitStyle();

  gROOT->SetStyle("StyleWhite");
  //  gStyle->SetPaperSize(27,20);

  // MD: make all of them as vectors
  Int_t dumi;
  Float_t dumf;
  Double_t dumd;
  char dums[256];
  Double_t CN[160];
  Int_t va[10240];     // red but not used
  Int_t chofva[10240]; // red but not used
  Int_t ch[10240];     // red but not used
  Double_t ped[10240];
  Float_t rsig[10240];
  Float_t sig[10240];
  Int_t status[10240];
  TCanvas *c = NULL;

  for (Int_t ii = 0; ii < 10240; ii++) {
    ped[ii] = 0;
    rsig[ii] = 0;
    sig[ii] = 0;
    status[ii] = 0;
  }

  std::string s_filename = filename;
  std::string tdrname_ext = s_filename.substr(s_filename.find_last_of("_\\") + 1);
  typename std::string::size_type const p(tdrname_ext.find_last_of('.'));
  std::string tdrname = (p > 0 && p != std::string::npos) ? tdrname_ext.substr(0, p) : tdrname_ext;

  TLatex *comment = new TLatex(1 - 0.01, 0.01, Form("%s", filename));
  comment->SetNDC();
  comment->SetTextAngle(90);
  comment->SetTextAlign(11);
  comment->SetTextSize(0.015);

  FILE *calfil = NULL;
  calfil = fopen(filename, "r");
  if (!calfil) {
    printf("file %s does not exist\n", filename);
    return 1;
  }

  int const dimline = 512;
  char line[dimline];

  int nva = 0;
  int nch = 0;

  while (1) {
    if (fgets(line, dimline, calfil) != NULL) {
      if (*line == '#') { /* ignore comment line, but there shouln't be */
        continue;
      } else {
        int firstn = sscanf(line, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s", dums, dums, dums, dums, dums, dums, dums, dums);
        //	printf("%d)\n", firstn);
        if (firstn == 3 || firstn == 8) {
          if (firstn == 3) {
            sscanf(line, "%d,\t%f,\t%lf", &dumi, &dumf, &dumd);
            //	    printf("I did read: %d) %f %lf\n", dumi, dumf, dumd);
            CN[dumi] = dumd;
            nva++;
          } else {
            sscanf(line, "%d %d %d %lf %f %f %f %d", &ch[nch], &va[nch], &chofva[nch], &ped[nch], &rsig[nch], &sig[nch],
                   &dumf, &status[nch]);
            // printf("I did read: %d) VA=%d, ch_VA=%d ped=%lf rawsig=%f sig=%f ?=%f status=%d\n", ch[nch], va[nch],
            // chofva[nch],
            //  	   ped[nch], rsig[nch], sig[nch],
            //  	   dumf, status[nch]);
            nch++;
          }
        } else {
          printf("The number of elements must be 3 or 8. On this line we had %d elements. Skipping it...\n", firstn);
        }
      }
    } else {
      //      printf("... closing cal file\n");
      fclose(calfil);
      break;
    }
  }

  printf("I did read: %d VAs and %d channels\n", nva, nch);

  //-------------------------------------------------------------------------------------------------------

  static int _nch = nch;
  static int _nva = nva;
  if (nva != _nva || nch != _nch) {
    printf("Strange: {%d, %d} vs {%d, %d}...\n", nva, nch, _nva, _nch);
  }
  static TH1F *isto = new TH1F(Form("isto_%d", count), Form("%s: pedestals", tdrname.c_str()), nch, 1, nch);
  static TH1F *isto2 = new TH1F(Form("isto2_%d", count), Form("%s: sigma", tdrname.c_str()), nch, 1, nch);
  static TH1F *isto3 = new TH1F(Form("isto3_%d", count), Form("%s: sigma raw", tdrname.c_str()), nch, 1, nch);
  static TH1F *isto4 = new TH1F(Form("isto4_%d", count), Form("%s: strip status", tdrname.c_str()), nch, 1, nch);

  int skip = 128;
  int shift = 0;
  if (CPars->flavour == 4) { // GSI2
    if (donotupdate) {
      skip = 64;
      shift = 64;
    } else {
      skip = 64;
      shift = 0;
      isto->Reset();
      isto2->Reset();
      isto3->Reset();
      isto4->Reset();
    }
  } else {
    isto->Reset();
    isto2->Reset();
    isto3->Reset();
    isto4->Reset();
  }

  c = new TCanvas(Form("c_%d", count), Form("%s_%d", tdrname.c_str(), 0), 0 * 100, 0 * 10, 600, 400);
  comment->Draw();
  c->Update();
  c->Divide(2, 2);
  c->SetFillStyle(0);
  c->cd(1);
  gPad->SetFillStyle(0);

  c->cd(1);
  TH2F *fram = new TH2F(Form("fram_%d", count), Form("%s: pedestals", tdrname.c_str()), nch, 1, nch, CPars->PedYLim, 0,
                        CPars->PedYLim);
  fram->SetStats(0);
  fram->Draw();
  for (Int_t ii = 0; ii < nch; ii++)
    if (ii % 128 < skip)
      isto->SetBinContent(ii + shift, ped[ii]);
  isto->DrawCopy("same");
  gPad->Update();
  LinesVas(nva, ((int)(nch / nva)));
  gPad->Update();

  c->cd(2);
  gPad->SetFillStyle(0);
  TH2F *fram2 = new TH2F(Form("fram2_%d", count), Form("%s: sigma", tdrname.c_str()), nch, 1, nch, 11, 0, 10);
  fram2->SetStats(0);
  fram2->Draw();
  for (int ii = 0; ii < nch; ii++)
    if (ii % 128 < skip)
      isto2->SetBinContent(ii + shift, sig[ii]);
  isto2->DrawCopy("same");
  gPad->Update();
  LinesVas(nva, ((int)(nch / nva)));
  gPad->Update();

  c->cd(3);
  gPad->SetFillStyle(0);
  TH2F *fram3 = new TH2F(Form("fram3_%d", count), Form("%s: sigma raw", tdrname.c_str()), nch, 1, nch, 21, 0, 20);
  fram3->SetStats(0);
  fram3->Draw();
  for (int ii = 0; ii < nch; ii++)
    if (ii % 128 < skip)
      isto3->SetBinContent(ii + shift, rsig[ii]);
  isto3->DrawCopy("same");
  gPad->Update();
  LinesVas(nva, ((int)(nch / nva)));
  gPad->Update();

  c->cd(4);
  gPad->SetFillStyle(0);
  TH2F *fram4 = new TH2F(Form("fram4_%d", count), Form("%s: strip status", tdrname.c_str()), nch, 1, nch, 11, 0, 10);
  fram4->SetStats(0);
  fram4->DrawCopy();
  // int chip=0;
  // int channelofchip=0;
  for (int ii = 0; ii < nch; ii++) {
    if (ii % 128 < skip) {
      isto4->SetBinContent(ii + shift, status[ii]);
      // printf("canale: %d, chip: %d, CN: %lf\n", ii, chip, CN[chip]);
      //  channelofchip++;
      //  if (channelofchip==64) {
      //  	chip++;
      //  	channelofchip=0;//change the CN value every 16 bins (channels)
      //  }
    }
  }
  isto4->DrawCopy("same");
  gPad->Update();
  LinesVas(nva, ((int)(nch / nva)));
  gPad->Update();

  /*
  Char_t gen[200];
  sprintf(gen,"%s",filename);
  TLatex *ladnam=new TLatex(0.2,0.4,gen);
  ladnam->SetNDC();
  ladnam->SetTextSize(0.2);
  ladnam->Draw();
  */

  c->Update();
  if (CPars->flavour == 4) { // GSI2
    if (!donotupdate) {
      if (c) {
        delete c;
        c = 0;
      }
    }
  }

  if (c)
    c->Print(nameout, outkind);

  if (mute) {
    if (comment)
      delete comment;
    if (c)
      delete c;
    if (fram)
      delete fram;
    if (fram2)
      delete fram2;
    if (fram3)
      delete fram3;
    if (fram4)
      delete fram4;
    if (isto)
      delete isto;
    if (isto2)
      delete isto2;
    if (isto3)
      delete isto3;
    if (isto4)
      delete isto4;
  }

  count++;
  return 0;
}

void LinesVas(int nva, int nch_for_va) {
  Double_t xmin, ymin, xmax, ymax;
  gPad->GetRangeAxis(xmin, ymin, xmax, ymax);
  TLine *line = new TLine();
  line->SetLineColor(kBlue);
  TText *text = new TText();
  text->SetTextAlign(21); // vert and hor. alignment: horiz centered, top
  text->SetTextColor(kBlue);
  line->SetLineStyle(3);
  for (Int_t va = 1; va <= nva; va++) { // first and last separation lines are not drawn
    // line->SetLineStyle((va==10)?1:3);
    // if (va==11) {//this change the color from now on, since the `line` is always the same
    //   line->SetLineColor(kRed); text->SetTextColor(kRed);
    // }
    if (va * nch_for_va > xmin && va * nch_for_va < xmax)
      line->DrawLine(1 + va * nch_for_va, ymin, 1 + va * nch_for_va, ymax);
    if (va * nch_for_va > xmin && va * nch_for_va <= xmax)
      text->DrawText((va - 0.5) * nch_for_va, ymax + (ymax - ymin) * 0.01, Form("%d", va));
  }

  if (line)
    delete line;
  if (text)
    delete text;
}

void InitStyle() {
  gROOT->SetStyle("Plain");

  TStyle *myStyle[2], *tempo;
  myStyle[0] = new TStyle("StyleWhite", "white");
  myStyle[1] = new TStyle("StyleBlack", "black");

  tempo = gStyle;
  Int_t linecol, bkgndcol, histcol;

  for (Int_t style = 0; style < 2; style++) {

    linecol = kWhite * style + kBlack * (1 - style);
    bkgndcol = kBlack * style + kWhite * (1 - style);
    histcol = kYellow * style + kBlack * (1 - style); // was 95

    myStyle[style]->Copy(*tempo);

    myStyle[style]->SetCanvasBorderMode(0);
    myStyle[style]->SetCanvasBorderSize(1);
    myStyle[style]->SetFrameBorderSize(1);
    myStyle[style]->SetFrameBorderMode(0);
    myStyle[style]->SetPadBorderSize(1);
    myStyle[style]->SetStatBorderSize(1);
    myStyle[style]->SetTitleBorderSize(1);
    myStyle[style]->SetPadBorderMode(0);
    myStyle[style]->SetPalette(1, 0);
    myStyle[style]->SetPaperSize(20, 27);
    myStyle[style]->SetFuncColor(kRed);
    myStyle[style]->SetFuncWidth(1);
    myStyle[style]->SetLineScalePS(1);
    myStyle[style]->SetCanvasColor(bkgndcol);
    myStyle[style]->SetAxisColor(linecol, "XYZ");
    myStyle[style]->SetFrameFillColor(bkgndcol);
    myStyle[style]->SetFrameLineColor(linecol);
    myStyle[style]->SetLabelColor(linecol, "XYZ");
    myStyle[style]->SetPadColor(bkgndcol);
    myStyle[style]->SetStatColor(bkgndcol);
    myStyle[style]->SetStatTextColor(linecol);
    myStyle[style]->SetTitleColor(linecol, "XYZ");
    myStyle[style]->SetTitleFillColor(bkgndcol);
    myStyle[style]->SetTitleTextColor(linecol);
    myStyle[style]->SetLineColor(linecol);
    myStyle[style]->SetMarkerColor(histcol);
    myStyle[style]->SetTextColor(linecol);

    myStyle[style]->SetGridColor((style) ? 13 : kBlack);
    myStyle[style]->SetHistFillStyle(1001 * (1 - style));
    myStyle[style]->SetHistLineColor(histcol);
    myStyle[style]->SetHistFillColor((style) ? bkgndcol : kYellow);
    myStyle[style]->SetOptDate(22);
    myStyle[style]->GetAttDate()->SetTextColor(linecol);
    myStyle[style]->GetAttDate()->SetTextAngle(90);
    myStyle[style]->GetAttDate()->SetTextSize(0.01);
    myStyle[style]->SetDateY(1);
    myStyle[style]->SetDateX(1);
  }

  myStyle[1]->cd();

  gROOT->ForceStyle();

  // if (myStyle[0]) delete myStyle[0];
  // if (myStyle[1]) delete myStyle[1];
}
