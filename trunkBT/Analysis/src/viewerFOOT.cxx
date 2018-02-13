#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TROOT.h"
#include "TF1.h"
#include "TGraph.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TStopwatch.h"
#include "TLine.h"
#include <fstream>
#include <vector>
#include <string>

/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

/* from the CommonTool dir */
#include "TrackSelection.hh"
#include "Utilities.hh"
/* end */

using namespace std;


int main(int argc, char* argv[]) {
  
  if (argc<3) {
    printf("Usage:\n");
    printf("%s <tdrposnum> <events to display> <first input root-filename> [second input root-filename] ...\n", argv[0]);
    return 1;
  }

  string tempname = argv[3];
  int pos1 = tempname.find("run_");
  int pos2 = tempname.find(".root");
  string firstfile = tempname.substr(pos1,pos2-pos1);

  
  TChain *chain = new TChain("t4");
     
  for (int ii=3; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }
  
  Event *ev;
  Cluster *cl;
  
  Long64_t entries = chain->GetEntries();
  printf("This run has %lld entries\n", entries);



  TGraph *gr_event=new TGraph();
  gr_event->SetMarkerStyle(23);
  gr_event->SetMarkerColor(kBlue);
  gr_event->SetLineColor(kBlue);


  
  ev = new Event();
  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  int _maxtdr = NJINF*NTDRS;
  
  if (GetRH(chain)) {
    GetRH(chain)->Print();
    _maxtdr = GetRH(chain)->ntdrCmp;
  }
  else {
    printf("Not able to find the RHClass header in the UserInfo...\n");
    return -9;
  }
  
  TStopwatch sw;
  sw.Start();

  double perc=0;


  for(int eventnum=0; eventnum< atoi(argv[2]); eventnum++){

    gr_event->Set(0);

    chain->GetEntry(eventnum);
    
    for(int chan=0; chan< 1024; chan++){
      double testADC=ev->GetRawSignal_PosNum(atoi(argv[1]),chan,0);
      double calADC=ev->GetCalPed_PosNum(atoi(argv[1]),chan,0);
      double test=testADC-calADC;
    
      gr_event->SetPoint(gr_event->GetN(),chan, test);
    }
    
    TCanvas *c2 = new TCanvas("c2", "c2", 1920, 1080);
    TH1F *frame = gPad->DrawFrame(0, 0,1024,150);

    frame->SetTitle("Event "+TString::Format("%02d",(int)eventnum));
    frame->GetXaxis()->SetNdivisions(-16);
    frame->GetXaxis()->SetTitle("Strip number");
    gr_event->SetMarkerSize(0.5);
    gr_event->Draw("*l");
   
    TLine *line = new TLine(64,0,64,150);
    line->SetLineColor(kRed);
    line->Draw();
    TLine *line1 = new TLine(128,0,128,150);
    line1->SetLineColor(kRed);
    line1->Draw();
    TLine *line2 = new TLine(192,0,192,150);
    line2->SetLineColor(kRed);
    line2->Draw();
    TLine *line3 = new TLine(256,0,256,150);
    line3->SetLineColor(kRed);
    line3->Draw();
    TLine *line4 = new TLine(320,0,320,150);
    line4->SetLineColor(kRed);
    line4->Draw();
    TLine *line5 = new TLine(384,0,384,150);
    line5->SetLineColor(kRed);
    line5->Draw();
    TLine *line6 = new TLine(448,0,448,150);
    line6->SetLineColor(kRed);
    line6->Draw();
    TLine *line7 = new TLine(512,0,512,150);
    line7->SetLineColor(kRed);
    line7->Draw();
    TLine *line8 = new TLine(576,0,576,150);
    line8->SetLineColor(kRed);
    line8->Draw();
    TLine *line9 = new TLine(640,0,640,150);
    line9->SetLineColor(kRed);
    line9->Draw();
    TLine *line10 = new TLine(704,0,704,150);
    line10->SetLineColor(kRed);
    line10->Draw();
    TLine *line11 = new TLine(768,0,768,150);
    line11->SetLineColor(kRed);
    line11->Draw();
    TLine *line12 = new TLine(832,0,832,150);
    line12->SetLineColor(kRed);
    line12->Draw();
    TLine *line13 = new TLine(896,0,896,150);
    line13->SetLineColor(kRed);
    line13->Draw();
    TLine *line14 = new TLine(960,0,960,150);
    line14->SetLineColor(kRed);
    line14->Draw();

    c2->SaveAs("ViewerFrames/"+firstfile+"_frame_"+TString::Format("%09d",(int)eventnum)+".png");
   
    
}
      
  sw.Stop(); 
  sw.Print();  
  return 0;
}
