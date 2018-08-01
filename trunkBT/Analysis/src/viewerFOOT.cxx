#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TROOT.h"
#include "TF1.h"
#include "TGraph.h"
#include "TMath.h"
#include "TStopwatch.h"
#include "TLine.h"
#include <fstream>
#include <vector>
#include <string>

#include "Event.hh"
#include "Utilities.hh"

using namespace std;


int main(int argc, char* argv[]) {
  
  if (argc<3) {
    printf("Usage:\n");
    printf("%s <tdrposnum> <event(s) to display> <0: single event, 1:many events, 2: cumulate event> <first input root-filename> [second input root-filename] ...\n", argv[0]);
    return 1;
  }

  string tempname = argv[4];

  int pos1 = tempname.find("run_");
  int pos2 = tempname.find(".root");
  string firstfile = tempname.substr(pos1,pos2-pos1);
  
  TString output_filename = "Viewer/viewer_out_"+firstfile+".root";
  TFile* foutput = new TFile(output_filename.Data(), "UPDATE");
  foutput->cd();



  
  TChain *chain = new TChain("t4");     
  for (int ii=4; ii<argc; ii++) {
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
  gr_event->GetXaxis()->SetNdivisions(16,false);

  
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
  int maxadc=-999;
  int minadc=0;

  TCanvas *c2 = new TCanvas("c2", "c2", 1920, 1080);

  if(atoi(argv[3])==1){
    c2->Print("viewer_tmp.pdf(","pdf");
  }
  
  
  for(int evt=0; evt < atoi(argv[2]); evt++)
  {
    
    if(atoi(argv[3])==0){
      chain->GetEntry(atoi(argv[2]));
      evt=atoi(argv[2]);
    } else{
      chain->GetEntry(evt);
    }
    
    bool isevent= true;


    
    if(atoi(argv[3])==1){
      gr_event->Set(0);
    }
      

      for(int chan=0; chan< 1024; chan++){
      double testADC=ev->GetRawSignal_PosNum(atoi(argv[1]),chan,0);
      double calADC=ev->GetCalPed_PosNum(atoi(argv[1]),chan,0);
      double cnADC=ev->GetCN_PosNum(atoi(argv[1]),(int)chan/64,0);
      double test=testADC-calADC-cnADC;

      if(test > maxadc) maxadc=test;
      if(test < minadc) minadc=test;
      //if(test > 20){isevent=true;}else{isevent=false;}
      gr_event->SetPoint(gr_event->GetN(),chan, test);
    }

    if(isevent){
      
    TH1F *frame = gPad->DrawFrame(0, minadc-20,1024,maxadc+20);

    if(atoi(argv[3])==1){
      frame->SetTitle("Event number: "+TString::Format("%02d",(int)evt));
    }else if(atoi(argv[3])==0){
      frame->SetTitle("Event number: "+TString::Format("%02d",(int)atoi(argv[2])));
    }else{
      frame->SetTitle("Number of events: "+TString::Format("%02d",(int)atoi(argv[2])));
    }
    
    frame->GetXaxis()->SetNdivisions(-16);
    frame->GetXaxis()->SetTitle("Strip number");
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
    }
    if(atoi(argv[3])==1){
      c2->Print("viewer_tmp.pdf");
    }
    
  }
    c2->Draw();
    c2->Write();

    if(atoi(argv[3])==1){
      c2->Clear();
      c2->Print("viewer_tmp.pdf)","pdf");
    }else{
      c2->Print("viewer_tmp.pdf","pdf");
    }

    TString term_cmd = "mv viewer_tmp.pdf Viewer/"+firstfile+"_evt_"+argv[2]+"_"+argv[3]+".pdf";
    system(term_cmd.Data());

    cout << endl;
    cout << "Results in file " << firstfile+"_evt_"+argv[2]+"_"+argv[3]+".pdf" << endl;
    cout << endl;
    
  sw.Stop(); 
  sw.Print();  
  return 0;
}
