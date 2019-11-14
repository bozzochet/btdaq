#include "TFile.h"
#include "TChain.h"
#include "TGraph.h"
#include "TLine.h"
#include "TROOT.h"
#include <TApplication.h>
#include <TCanvas.h>
#include <TString.h>
#include <TGCanvas.h>
#include <string.h>
#include <TGWindow.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <TGTextView.h>
#include <TGLayout.h>
#include <TGFrame.h>
#include <TGFileDialog.h>
#include <TGClient.h>

#include "viewerGUI.hh"

#include <iostream>
#include <fstream>

#include "Event.hh"
#include "Utilities.hh"
#include "DecodeData.hh"

using namespace std;

MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h)
{
  // Create a main frame
  fMain = new TGMainFrame(p, w, h);

  // Create canvas widget
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas", fMain, 1024, 500);
  fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 1));

  fHor0 = new TGHorizontalFrame(fMain, 1024, 20);
  fHor0b = new TGHorizontalFrame(fMain, 1024, 20);

  TGVerticalFrame *fVer0 = new TGVerticalFrame(fHor0b, 10, 10);
  TGVerticalFrame *fVer1 = new TGVerticalFrame(fHor0b, 10, 10);

  //  fStatusBar = new TGStatusBar(fVer1,1024,100,kHorizontalFrame);
  //  fStatusBar = new TGLabel(fVer1,"Test",TGLabel::GetDefaultGC()(),TGLabel::GetDefaultFontStruct(),kSunkenFrame);
  fStatusBar = new TGTextView(fVer1, 500, 150);
  fStatusBar->LoadBuffer("Event viewer for AMS raw data .root files.");
  fStatusBar->AddLine("");
  fStatusBar->AddLine("Files must have been acquired in raw (non compressed) mode.");
  fStatusBar->AddLine("");
  fStatusBar->AddLine("0-639 is S-side, 640-1024 is K-side.");
  fStatusBar->AddLine("");
  fStatusBar->AddLine("");
  fStatusBar->AddLine("");
  fStatusBar->AddLine("File header and error messages will appear here.");

  fHor1 = new TGHorizontalFrame(fVer0, 1024, 20);
  fHor2 = new TGHorizontalFrame(fVer0, 1024, 20);
  fHor3 = new TGHorizontalFrame(fMain, 1024, 20);
  fHor4 = new TGHorizontalFrame(fVer0, 1024, 20);

  fOpen = new TGTextButton(fHor0, "&Open");
  fOpen->Connect("Clicked()", "MyMainFrame", this, "DoOpen()");

  fDraw = new TGTextButton(fHor0, "&Draw");
  fDraw->Connect("Clicked()", "MyMainFrame", this, "DoDraw()");

  fExit = new TGTextButton(fHor0, "&Exit", "gApplication->Terminate(0)");

  fSave = new TGTextButton(fHor0, "&Save");
  fSave->Connect("Clicked()", "MyMainFrame", this, "DoSave()");

  evtLabel = new TGLabel(fHor1, "Event Number:");
  fHor1->AddFrame(evtLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 2, 2, 2));
  fNumber = new TGNumberEntry(fHor1, 0, 9, 999, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, 99999);
  fNumber->GetNumberEntry()->Connect("ReturnPressed()", "MyMainFrame", this, "DoDraw()");

  tdrLabel = new TGLabel(fHor2, "TDR PosNum:  ");
  fHor2->AddFrame(tdrLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 2, 2, 2));
  fNumber2 = new TGNumberEntry(fHor2, 0, 9, 999, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, 24);
  fNumber2->GetNumberEntry()->Connect("ReturnPressed()", "MyMainFrame", this, "DoDraw()");

  fPed = new TGCheckButton(fHor4, "Pedestal subtraction");
  fPed->SetOn();
  fHor4->AddFrame(fPed, new TGLayoutHints(kLHintsExpandX | kLHintsLeft | kLHintsCenterY, 5, 2, 2, 2));

  fileLabel = new TGLabel(fHor3, "No rootfile opened");
  fHor3->AddFrame(fileLabel, new TGLayoutHints(kLHintsExpandX | kLHintsLeft | kLHintsCenterY, 5, 2, 2, 2));

  fHor1->AddFrame(fNumber, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));
  fHor2->AddFrame(fNumber2, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));
  fHor4->AddFrame(fPed, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));

  fHor0->AddFrame(fOpen, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  fHor0->AddFrame(fDraw, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  fHor0->AddFrame(fSave, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  fHor0->AddFrame(fExit, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

  fHor0b->AddFrame(fVer0, new TGLayoutHints(kLHintsCenterX | kLHintsExpandX | kLHintsExpandY, 2, 2, 5, 1));
  fHor0b->AddFrame(fVer1, new TGLayoutHints(kLHintsCenterX | kLHintsExpandX | kLHintsExpandY, 2, 2, 5, 1));

  fMain->AddFrame(fHor0, new TGLayoutHints(kLHintsCenterX, 2, 2, 5, 1));
  fMain->AddFrame(fHor0b, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX, 2, 2, 5, 1));

  fVer0->AddFrame(fHor1, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 2, 2, 5, 1));
  fVer0->AddFrame(fHor2, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 2, 2, 5, 1));
  fVer0->AddFrame(fHor4, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 2, 2, 5, 1));
  fMain->AddFrame(fHor3, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX, 2, 2, 5, 1));

  //  fStatusBar->Resize(0,200);
  fVer1->AddFrame(fStatusBar, new TGLayoutHints(kLHintsCenterX | kLHintsBottom | kLHintsLeft | kLHintsExpandY, 5, 5, 2, 2));

  fMain->SetCleanup(kDeepCleanup);
  fMain->SetWindowName("AMS Raw Event Viewer");
  fMain->MapSubwindows();
  fMain->Resize(fMain->GetDefaultSize());
  fMain->MapWindow();
}

const char *filetypes[] = {
    "ROOT files", "*.root",
    "All files", "*",
    0, 0};

void MyMainFrame::viewer(int tdr, int evt, char filename[200])
{

  TChain *chain = new TChain("t4");
  chain->Add(filename);
  Long64_t entries = chain->GetEntries();

  RHClass *rh = (RHClass *)chain->GetTree()->GetUserInfo()->At(0);

  Event *ev;
  Cluster *cl;

  gr_eventS->SetMarkerColor(kRed + 1);
  gr_eventS->SetLineColor(kRed + 1);
  gr_eventK->SetMarkerColor(kBlue + 2);
  gr_eventK->SetLineColor(kBlue + 2);

  gr_eventS->SetMarkerStyle(23);
  gr_eventK->SetMarkerStyle(23);
  gr_eventS->GetXaxis()->SetNdivisions(16, false);
  gr_eventK->GetXaxis()->SetNdivisions(16, false);

  ev = new Event();

  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  int maxadc = -999;
  int minadc = 0;

  chain->GetEntry(evt);

  gr_eventS->Set(0);
  gr_eventK->Set(0);
  
  maxadc = 0;
  minadc = 0;

  for (int chan = 0; chan < 1024; chan++)
  {
    double ADC = ev->GetRawSignal_PosNum(tdr, chan, 0);
    double calADC = ev->GetCalPed_PosNum(tdr, chan, 0);
    double cnADC = ev->GetCN_PosNum(tdr, (int)chan / 64, 0);
    int status = ev->GetCalStatus_PosNum(tdr, chan, 0);
    double signal;

    if (status!=0) continue;

    if( fPed->IsOn() ){
      signal = ADC - calADC - cnADC;
    } else {
      signal = ADC;
    }
  
    if (signal > maxadc)
      maxadc = signal;
    if (signal < minadc)
      minadc = signal;
    if (chan < 640)
    {
      gr_eventS->SetPoint(gr_eventS->GetN(), chan, signal);
    }
    else
    {
      gr_eventK->SetPoint(gr_eventK->GetN(), chan, signal);
    }
  }

  TH1F *frame = gPad->DrawFrame(0, minadc - 20, 1024, maxadc + 20);

  frame->SetTitle("Event number " + TString::Format("%0d", (int)evt) + " on TDR " + TString::Format("%02d", (int)rh->GetTdrNum(tdr)));
  frame->GetXaxis()->SetNdivisions(-16);
  frame->GetXaxis()->SetTitle("Strip number");
  frame->GetYaxis()->SetTitle("ADC");

  gr_eventS->SetMarkerSize(0.5);
  gr_eventS->Draw("*lSAME");
  gr_eventK->SetMarkerSize(0.5);
  gr_eventK->Draw("*lSAME");

  // TLine *line[14];
  for (int iline = 0; iline < 15; iline++)
  {
    line[iline] = new TGraph(2);
    //line[iline] = new TLine((iline + 1) * 64, minadc - 20, (iline + 1) * 64, maxadc + 20);
    line[iline]->SetPoint(0,(iline + 1) * 64,minadc-20);
    line[iline]->SetPoint(1,(iline + 1) * 64,maxadc+20);
    line[iline]->SetLineColor(kGray + 2);
    line[iline]->Draw();
  }

  gr_eventS->Draw();
  gr_eventK->Draw();

  TCanvas *fCanvas = fEcanvas->GetCanvas();
  fCanvas->cd();
  fCanvas->Update();

  delete chain;
  delete ev;
}

void MyMainFrame::savetofile(int tdr, int evt, char filename[200])
{
  ofstream myfile;
  TString outfile = filename;
  myfile.open (outfile+"_TDR_" + TString::Format("%02d", (int)tdr)+".txt",std::ios_base::app);

  TChain *chain = new TChain("t4");
  chain->Add(filename);
  Long64_t entries = chain->GetEntries();

  RHClass *rh = (RHClass *)chain->GetTree()->GetUserInfo()->At(0);

  Event *ev;
  Cluster *cl;

  ev = new Event();

  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  chain->GetEntry(evt);

  for (int chan = 0; chan < 1024; chan++)
  {
    double ADC = ev->GetRawSignal_PosNum(tdr, chan, 0);
    double calADC = ev->GetCalPed_PosNum(tdr, chan, 0);
    double cnADC = ev->GetCN_PosNum(tdr, (int)chan / 64, 0);
    int status = ev->GetCalStatus_PosNum(tdr, chan, 0);
    double signal;

    if (status!=0) continue;

    if( fPed->IsOn() ){
      signal = ADC - calADC - cnADC;
    } else {
      signal = ADC;
    }

  const size_t bufsize = 256*1024;
  char buf[bufsize];
  myfile.rdbuf()->pubsetbuf(buf, bufsize);

  myfile << evt << " " << chan << " " << signal << endl;

  }
  
  TCanvas *fCanvas = fEcanvas->GetCanvas();
  fCanvas->cd();
  fCanvas->SaveAs(outfile+"_TDR_" + TString::Format("%02d", (int)tdr)+".png");

  delete chain;
  delete ev;
  myfile.close();
}


void MyMainFrame::DoDraw()
{
  if (gROOT->GetListOfFiles()->FindObject((char *)(fileLabel->GetText())->GetString()))
  {
    viewer(fNumber2->GetNumberEntry()->GetIntNumber(), fNumber->GetNumberEntry()->GetIntNumber(), (char *)(fileLabel->GetText())->GetString());  
  }
}

void MyMainFrame::DoSave()
{
  if (gROOT->GetListOfFiles()->FindObject((char *)(fileLabel->GetText())->GetString()))
  {
      savetofile(fNumber2->GetNumberEntry()->GetIntNumber(), fNumber->GetNumberEntry()->GetIntNumber(), (char *)(fileLabel->GetText())->GetString());  
  }
}

void MyMainFrame::DoOpen()
{
  static TString dir(".");
  TGFileInfo fi;
  fi.fFileTypes = filetypes;
  fi.fIniDir = StrDup(dir);
  printf("fIniDir = %s\n", fi.fIniDir);
  new TGFileDialog(gClient->GetRoot(), fMain, kFDOpen, &fi);
  printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);

  if (fi.fFilename)
  {
    TFile *f = TFile::Open(fi.fFilename);
    bool ISt4 = f->GetListOfKeys()->Contains("t4");
    if (ISt4)
    {
      TTree *t = (TTree *)f->Get("t4");
      int entries = t->GetEntries();
      RHClass *rh = (RHClass *)t->GetUserInfo()->At(0);

      fStatusBar->Clear();
      fStatusBar->LoadBuffer("Run: " + TGString(rh->GetRun()) + " - Date " + TString(rh->GetDate()));
      fStatusBar->AddLine("Number of events: " + TGString(entries));
      fStatusBar->AddLine("# Jinf = " + TGString(rh->GetNJinfs()));

      for (int ii = 0; ii < rh->GetNJinfs(); ii++)
      {
        fStatusBar->AddLine("Jinf Map pos: " + TGString(ii) + " Jinf num: " + ii);
      }

      fStatusBar->AddLine("# TDR RAW = " + TGString(rh->GetNTdrsRaw()));
      for (int ii = 0; ii < rh->GetNTdrsRaw(); ii++)
      {
        fStatusBar->AddLine("TDR RAW Map pos: " + TGString(ii) + " tdrnum: " + TGString(rh->GetTdrNum(ii)));
      }

      fStatusBar->AddLine("# TDR CMP = " + TGString(rh->GetNTdrsCmp()));
      for (int ii = 0; ii < rh->GetNTdrsCmp(); ii++)
      {
        fStatusBar->AddLine("TDR CMP Map pos: " + TGString(ii) + " tdrnum: " + TGString(rh->GetTdrNum(ii)));
      }

      fileLabel->SetText(fi.fFilename);
      fNumber->SetText("0");
      DoDraw();
    }
    else
    {
      fStatusBar->Clear();
      fStatusBar->AddLine("ERROR: the file does not contain t4 TTree");
      return;
    }
  }
  dir = fi.fIniDir;
}

MyMainFrame::~MyMainFrame()
{
  // Clean up used widgets: frames, buttons, layout hints
  fMain->Cleanup();
  delete fMain;
}

void viewerGUI()
{
  // Popup the GUI...
  new MyMainFrame(gClient->GetRoot(), 500, 500);
}

int main(int argc, char **argv)
{
  TApplication theApp("App", &argc, argv);
  viewerGUI();
  theApp.Run();
  return 0;
}
