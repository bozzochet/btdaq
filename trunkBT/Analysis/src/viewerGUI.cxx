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

#include"viewerGUI.hh"

#include "Event.hh"
#include "Utilities.hh"
#include "DecodeData.hh"

MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h)
{
  // Create a main frame
  fMain = new TGMainFrame(p,w,h);

  // Create canvas widget
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,1024,500);
  fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10,10,10,1));


  fHor0 = new TGHorizontalFrame(fMain, 1024, 20);
  fHor0b = new TGHorizontalFrame(fMain, 1024, 20);

  TGVerticalFrame *fVer0 = new TGVerticalFrame(fHor0b, 10, 10);
  TGVerticalFrame *fVer1 = new TGVerticalFrame(fHor0b, 10, 10);

  //  fStatusBar = new TGStatusBar(fVer1,1024,100,kHorizontalFrame);
  //  fStatusBar = new TGLabel(fVer1,"Test",TGLabel::GetDefaultGC()(),TGLabel::GetDefaultFontStruct(),kSunkenFrame);
  fStatusBar = new TGTextView(fVer1,500,150);
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

  fOpen = new TGTextButton(fHor0, "&Open");
  fOpen->Connect("Clicked()","MyMainFrame",this,"DoOpen()");

  fDraw = new TGTextButton(fHor0, "&Draw");
  fDraw->Connect("Clicked()","MyMainFrame",this,"DoDraw()");

  fExit = new TGTextButton(fHor0, "&Exit", "gApplication->Terminate(0)");

  evtLabel = new TGLabel(fHor1, "Event Number:");
  fHor1->AddFrame(evtLabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 5, 2, 2, 2));
  fNumber = new TGNumberEntry(fHor1, 0, 9,999, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, 99999);
  fNumber->GetNumberEntry()->Connect("ReturnPressed()", "MyMainFrame", this,"DoDraw()");

  tdrLabel = new TGLabel(fHor2, "TDR PosNum:  ");
  fHor2->AddFrame(tdrLabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 5, 2, 2, 2));
  fNumber2 = new TGNumberEntry(fHor2, 0, 9,999, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, 24);
  fNumber2->GetNumberEntry()->Connect("ReturnPressed()", "MyMainFrame", this,"DoDraw()");

  fileLabel = new TGLabel(fHor3, "No rootfile opened");
  fHor3->AddFrame(fileLabel, new TGLayoutHints(kLHintsExpandX|kLHintsLeft|kLHintsCenterY,5, 2, 2, 2));

  fHor1->AddFrame(fNumber, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));
  fHor2->AddFrame(fNumber2, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));

  fHor0->AddFrame(fOpen, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  fHor0->AddFrame(fDraw, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  fHor0->AddFrame(fExit, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

  fHor0b-> AddFrame(fVer0,new TGLayoutHints(kLHintsCenterX|kLHintsExpandX|kLHintsExpandY, 2, 2, 5, 1));
  fHor0b-> AddFrame(fVer1,new TGLayoutHints(kLHintsCenterX|kLHintsExpandX|kLHintsExpandY, 2, 2, 5, 1));

  fMain->AddFrame(fHor0,new TGLayoutHints(kLHintsCenterX, 2, 2, 5, 1));
  fMain->AddFrame(fHor0b,new TGLayoutHints(kLHintsExpandX|kLHintsCenterX, 2, 2, 5, 1));

  fVer0->AddFrame(fHor1,new TGLayoutHints(kLHintsCenterX|kLHintsCenterY, 2, 2, 5, 1));
  fVer0->AddFrame(fHor2,new TGLayoutHints(kLHintsCenterX|kLHintsCenterY, 2, 2, 5, 1));
  fMain->AddFrame(fHor3,new TGLayoutHints(kLHintsExpandX|kLHintsCenterX, 2, 2, 5, 1));

  //  fStatusBar->Resize(0,200);
  fVer1->AddFrame(fStatusBar, new TGLayoutHints(kLHintsCenterX| kLHintsBottom |kLHintsLeft |kLHintsExpandY, 5, 5, 2, 2));

  fMain->SetCleanup(kDeepCleanup);
  fMain->SetWindowName("AMS Raw Event Viewer");
  fMain->MapSubwindows();
  fMain->Resize(fMain->GetDefaultSize());
  fMain->MapWindow();
}

const char *filetypes[] = {
  "ROOT files",    "*.root",
  "All files",     "*",
  0,               0
};

void viewer(int tdr, int evt, char filename[200]) {

  TChain *chain = new TChain("t4");
  chain->Add(filename);
  Long64_t entries = chain->GetEntries();

  RHClass* rh = (RHClass*)chain->GetTree()->GetUserInfo()->At(0);

  Event *ev;
  Cluster *cl;

  TGraph *gr_event=new TGraph();
  gr_event->SetMarkerStyle(23);
  gr_event->SetMarkerColor(kBlue);
  gr_event->SetLineColor(kBlue);
  gr_event->GetXaxis()->SetNdivisions(16,false);

  ev = new Event();

  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  int maxadc=-999;
  int minadc=0;

  chain->GetEntry(evt);

  gr_event->Set(0);
  maxadc=0;
  minadc=0;

  for(int chan=0; chan< 1024; chan++){
    double ADC=ev->GetRawSignal_PosNum(tdr,chan,0);
    double calADC=ev->GetCalPed_PosNum(tdr,chan,0);
    double cnADC=ev->GetCN_PosNum(tdr,(int)chan/64,0);
    double signal=ADC-calADC-cnADC;

    if(signal > maxadc) maxadc=signal;
    if(signal < minadc) minadc=signal;
    gr_event->SetPoint(gr_event->GetN(),chan, signal);
  }

  TH1F *frame = gPad->DrawFrame(0, minadc-20,1024,maxadc+20);

  frame->SetTitle("Event number "+TString::Format("%d",(int)evt)+" on TDR "+TString::Format("%02d",(int)rh->tdrRawMap[tdr]));
  frame->GetXaxis()->SetNdivisions(-16);
  frame->GetXaxis()->SetTitle("Strip number");
  frame->GetYaxis()->SetTitle("ADC");

  gr_event->SetMarkerSize(0.5);
  gr_event->Draw("*lSAME");

  TLine *line = new TLine(64,minadc-20,64,maxadc+20);
  line->SetLineColor(kRed);
  line->Draw();
  TLine *line1 = new TLine(128,minadc-20,128,maxadc+20);
  line1->SetLineColor(kRed);
  line1->Draw();
  TLine *line2 = new TLine(192,minadc-20,192,maxadc+20);
  line2->SetLineColor(kRed);
  line2->Draw();
  TLine *line3 = new TLine(256,minadc-20,256,maxadc+20);
  line3->SetLineColor(kRed);
  line3->Draw();
  TLine *line4 = new TLine(320,minadc-20,320,maxadc+20);
  line4->SetLineColor(kRed);
  line4->Draw();
  TLine *line5 = new TLine(384,minadc-20,384,maxadc+20);
  line5->SetLineColor(kRed);
  line5->Draw();
  TLine *line6 = new TLine(448,minadc-20,448,maxadc+20);
  line6->SetLineColor(kRed);
  line6->Draw();
  TLine *line7 = new TLine(512,minadc-20,512,maxadc+20);
  line7->SetLineColor(kRed);
  line7->Draw();
  TLine *line8 = new TLine(576,minadc-20,576,maxadc+20);
  line8->SetLineColor(kRed);
  line8->Draw();
  TLine *line9 = new TLine(640,minadc-20,640,maxadc+20);
  line9->SetLineColor(kRed);
  line9->Draw();
  TLine *line10 = new TLine(704,minadc-20,704,maxadc+20);
  line10->SetLineColor(kRed);
  line10->Draw();
  TLine *line11 = new TLine(768,minadc-20,768,maxadc+20);
  line11->SetLineColor(kRed);
  line11->Draw();
  TLine *line12 = new TLine(832,minadc-20,832,maxadc+20);
  line12->SetLineColor(kRed);
  line12->Draw();
  TLine *line13 = new TLine(896,minadc-20,896,maxadc+20);
  line13->SetLineColor(kRed);
  line13->Draw();
  TLine *line14 = new TLine(960,minadc-20,960,maxadc+20);
  line14->SetLineColor(kRed);
  line14->Draw();

  gr_event->Draw();
}

void MyMainFrame::DoDraw() {
  if(gROOT->GetListOfFiles()->FindObject((char*)(fileLabel->GetText())->GetString())){
  viewer(fNumber2->GetNumberEntry()->GetIntNumber(), fNumber->GetNumberEntry()->GetIntNumber(), (char*)(fileLabel->GetText())->GetString());
    TCanvas *fCanvas = fEcanvas->GetCanvas();
    fCanvas->cd();
    fCanvas->Update();
  }
}

void MyMainFrame::DoOpen() {
  static TString dir(".");
  TGFileInfo fi;
  fi.fFileTypes = filetypes;
  fi.fIniDir    = StrDup(dir);
  printf("fIniDir = %s\n", fi.fIniDir);
  new TGFileDialog(gClient->GetRoot(), fMain, kFDOpen, &fi);
  printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);

  if(fi.fFilename){
    TFile *f = TFile::Open(fi.fFilename);
    bool ISt4=f->GetListOfKeys()->Contains("t4");
    if(ISt4){
      TTree* t =(TTree*)f->Get("t4");
      int entries  = t->GetEntries();
      RHClass* rh = (RHClass*)t->GetUserInfo()->At(0);

      fStatusBar->Clear();
      fStatusBar->LoadBuffer("Run: "+TGString(rh->Run)+" - Date "+TString(rh->date));
      fStatusBar->AddLine("Number of events: "+TGString(entries));
      fStatusBar->AddLine("# Jinf = "+TGString(rh->nJinf));

      for (int ii=0;ii<rh->nJinf;ii++){
        fStatusBar->AddLine("Jinf Map pos: "+TGString(ii)+" Jinf num: "+TGString(rh->JinfMap[ii]));
      }

      fStatusBar->AddLine("# TDR RAW = "+TGString(rh->ntdrRaw));
      for (int ii=0;ii<rh->ntdrRaw;ii++){
        fStatusBar->AddLine("TDR RAW Map pos: "+TGString(ii)+" tdrnum: "+TGString(rh->tdrRawMap[ii]));
      }

      fStatusBar->AddLine("# TDR CMP = "+TGString(rh->ntdrCmp));
      for (int ii=0;ii<rh->ntdrCmp;ii++){
        fStatusBar->AddLine("TDR CMP Map pos: "+TGString(ii)+" tdrnum: "+TGString(rh->tdrCmpMap[ii]));
      }

      fileLabel->SetText(fi.fFilename);
      fNumber->SetText("0");
      DoDraw();
    }else{
      fStatusBar->Clear();
      fStatusBar->AddLine("ERROR: the file does not contain t4 TTree");
      return;
    }
  }
  dir = fi.fIniDir;
}

MyMainFrame::~MyMainFrame() {
  // Clean up used widgets: frames, buttons, layout hints
  fMain->Cleanup();
  delete fMain;
}

void viewerGUI() {
  // Popup the GUI...
  new MyMainFrame(gClient->GetRoot(),500,500);
}

int main(int argc, char **argv) {
   TApplication theApp("App",&argc,argv);
   viewerGUI();
   theApp.Run();
   return 0;
}
