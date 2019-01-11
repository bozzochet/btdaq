#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TLine.h"
#include "TMath.h"
#include "TROOT.h"
#include <RQ_OBJECT.h>
#include <string>
#include <TApplication.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGLayout.h>
#include <TGNumberEntry.h>
#include <TGTextEntry.h>
#include <TGWindow.h>
#include <TRootEmbeddedCanvas.h>
#include <TString.h>
#include <vector>
#include <TGFileDialog.h>

#include "../Decode/Event.hh"
#include "../CommonTool/Utilities.hh"
#include "../Decode/DecodeData.hh"

using namespace std;

const char *filetypes[] = { "All files",     "*",
                            "ROOT files",    "*.root",
			    0,               0 };


class MyMainFrame : public TGMainFrame { 
  RQ_OBJECT("MyMainFrame")
 private:
  TGMainFrame         *fMain;
  TGDockableFrame     *fMenuDock;
  TRootEmbeddedCanvas *fEcanvas;
  TGNumberEntry       *fNumber;
  TGNumberEntry       *fNumber2;
  TGHorizontalFrame   *fHor0;
  TGHorizontalFrame   *fHor1;
  TGHorizontalFrame   *fHor2;
  TGHorizontalFrame   *fHor3;
  TGTextButton        *fExit;
  TGTextButton        *fDraw;
  TGTextButton        *fOpen;
  TGTextEntry         *fEntry;
  TGLabel             *evtLabel;
  TGLabel             *tdrLabel;
  TGLabel             *fileLabel;

 public:
  MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
  virtual ~MyMainFrame();
  void DoDraw();
  void DoOpen();
  ClassDef(MyMainFrame, 0)
    };

MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h)
: TGMainFrame(p,w,h)
{
  // Create a main frame
  fMain = new TGMainFrame(p,w,h);

  // Create canvas widget
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas",this,1024,500);
  AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX |
				       kLHintsExpandY, 10,10,10,1));

  fHor0 = new TGHorizontalFrame(this, 1024, 20);
  fHor1 = new TGHorizontalFrame(this, 1024, 20);
  fHor2 = new TGHorizontalFrame(this, 1024, 20);
  fHor3 = new TGHorizontalFrame(this, 1024, 20);


  fOpen = new TGTextButton(fHor0, "&Open");
  fOpen->Connect("Clicked()","MyMainFrame",this,"DoOpen()");
  
  fDraw = new TGTextButton(fHor0, "&Draw");
  fDraw->Connect("Clicked()","MyMainFrame",this,"DoDraw()");

  fExit = new TGTextButton(fHor0, "&Exit", "gApplication->Terminate(0)");
  
  evtLabel = new TGLabel(fHor1, "Event Number:");
  fHor1->AddFrame(evtLabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,
					      5, 2, 2, 2));
  fNumber = new TGNumberEntry(fHor1, 0, 9,999, TGNumberFormat::kNESInteger,
			      TGNumberFormat::kNEANonNegative,
			      TGNumberFormat::kNELLimitMinMax,
                                               0, 99999);

  tdrLabel = new TGLabel(fHor2, "TDR PosNum:  ");
  fHor2->AddFrame(tdrLabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,
					      5, 2, 2, 2));
  fNumber2 = new TGNumberEntry(fHor2, 0, 9,999, TGNumberFormat::kNESInteger,
                              TGNumberFormat::kNEANonNegative,
                              TGNumberFormat::kNELLimitMinMax,
                                               0, 24);
  
  fileLabel = new TGLabel(fHor3, "No rootfile opened");
  fHor3->AddFrame(fileLabel, new TGLayoutHints(kLHintsExpandX|kLHintsLeft|kLHintsCenterY,
					     5, 2, 2, 2));
  
  fHor1->AddFrame(fNumber, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));
  fHor2->AddFrame(fNumber2, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));
  
  fHor0->AddFrame(fOpen, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  fHor0->AddFrame(fDraw, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  fHor0->AddFrame(fExit, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

  AddFrame(fHor0,new TGLayoutHints(kLHintsCenterX, 2, 2, 5, 1));
  AddFrame(fHor1,new TGLayoutHints(kLHintsCenterX, 2, 2, 5, 1));
  AddFrame(fHor2,new TGLayoutHints(kLHintsCenterX, 2, 2, 5, 1));
  AddFrame(fHor3,new TGLayoutHints(kLHintsCenterX, 2, 2, 5, 1));
  
  
  SetCleanup(kDeepCleanup);
  SetWindowName("AMS Raw Event Viewer");
  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
  
}

void viewer(int tdr, int evt, char filename[200]) {


  TChain *chain = new TChain("t4");
  chain->Add(filename);
  
  //TFile* f = new TFile(filename,"READ");
  Long64_t entries = chain->GetEntries();
  printf("This run has %lld entries\n", entries);

  // TTree* t =(TTree*)f->Get("t4");

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
    
    frame->SetTitle("Event number: "+TString::Format("%02d",(int)evt)+" on TDR: "+TString::Format("%02d",(int)tdr));
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
    viewer(fNumber2->GetNumberEntry()->GetIntNumber(), fNumber->GetNumberEntry()->GetIntNumber(), (char*)(fileLabel->GetText())->GetString()); 
      TCanvas *fCanvas = fEcanvas->GetCanvas();
      fCanvas->cd();
      fCanvas->Update();
}


void MyMainFrame::DoOpen() {
            static TString dir(".");
            TGFileInfo fi;
            fi.fFileTypes = filetypes;
	    //            fi.fIniDir    = StrDup(dir);
            printf("fIniDir = %s\n", fi.fIniDir);
            new TGFileDialog(gClient->GetRoot(), fMain, kFDOpen, &fi);
            printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
	    fileLabel->SetText(fi.fFilename);
	    // dir = fi.fIniDir;
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
