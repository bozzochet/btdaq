#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include "TObjArray.h"
#include "TChain.h"
#include "TF1.h"

/* from the Matteo 'Bozzochet' Duranti's API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

#include <vector>

using namespace std;

double Z[6] = {
  0-3000,
  305-3000,
  362+305-3000,
  4095+362+305-3000,
  4095+362+305+379-3000,
  4095+362+305+379+990-3000,
};

double align_S[6] = {
  0.001*39975.943028,
  0.001*41255.147420,
  0.001*17173.970321,
  0.001*41755.635591,
  0.001*40921.845304,
  0.001*33841.101306
};

double align_K[6] = {
   0.001*92113.607793,
  0.001*108630.753474,
  0.001*116327.464394,
  0.001*115121.431475,
   0.001*67920.425844,
   0.001*75508.210764
};

int FindArrayNumber(int ladder);
void SavePDF( TObjArray *arrCanvas, const char *outfilename);

int macro(unsigned int runfirstnumbers) {

  //  gSystem->Load("./libEvent.so");

  TChain* ch_up = new TChain("t4");
  TChain* ch_dw = new TChain("t4");
  
  int n_up = ch_up->Add(Form("./RootData_upstream/run_%d*.root", runfirstnumbers));
  printf("Added %d files for the upstream chain\n", n_up);

  int n_dw = ch_dw->Add(Form("./RootData_downstream/run_%d*.root", runfirstnumbers));
  printf("Added %d files for the upstream chain\n", n_dw);
  
  
  Event* event_up=0;
  Event* event_dw=0;
  
  TH1F* occupancy_S[6];
  TH1F* occupancy_K[6];
  TH1F* occupancy_S_ru[6];
  TH1F* occupancy_K_ru[6];
  for (int ii=0; ii<6; ii++) {
    occupancy_S[ii] = new TH1F(Form("occupancy_S_%d", ii) , Form("occupancy_S_%d;Channel;Entries", ii), 650, 0, 650);
    occupancy_K[ii] = new TH1F(Form("occupancy_K_%d", ii) , Form("occupancy_K_%d;channel;Entries", ii), 5*384, 0, 5*384);
    occupancy_S_ru[ii] = new TH1F(Form("occupancy_S_ru_%d", ii) , Form("occupancy_S_ru_%d;Position (mm);Entries", ii), 2*640, -0.001*110*640, 0.001*110*640);
    occupancy_K_ru[ii] = new TH1F(Form("occupancy_K_ru_%d", ii) , Form("occupancy_K_ru_%d;Position (mm);Entries", ii), 5*384, -0.001*220*384, 0.001*5*220*384);
  }

  TH2F* downstream_K = new TH2F("downstream_K", "downstream_K;Z (mm);Position (mm);Entries", 20, -3100, 3600, 5*384, -0.001*220*384, 0.001*5*220*384);
  
  Long64_t entries_up = ch_up->GetEntries();
  Long64_t entries_dw = ch_dw->GetEntries();
  printf("This run has %lld (%lld) entries\n", entries_up, entries_dw);
  
  if (entries_up > 0) {
    event_up = new Event();
    event_dw = new Event();
    ch_up->SetBranchAddress("cluster_branch", &event_up);
    ch_up->GetEntry(0);
    ch_dw->SetBranchAddress("cluster_branch", &event_dw);
    ch_dw->GetEntry(0);
  }
  
  for (int index_event = 0; index_event < entries_up; index_event++) {
    ch_up->GetEntry(index_event);
    ch_dw->GetEntry(index_event);
    //    printf("%p %p\n", event_up, event_dw);
    int NClusTot_up = event_up->NClusTot;
    int NClusTot_dw = event_dw->NClusTot;
    int NClusTot = 0;
    Cluster* cluster;
    Event* event;
    for (int ii=0; ii<2; ii++) {
      if (ii==0) {
	NClusTot = NClusTot_up;
	event = event_up;
      }
      else if (ii==1) {
	NClusTot = NClusTot_dw;
	event = event_dw;
      }
      for (int index_cluster = 0; index_cluster < NClusTot; ++index_cluster) {
	cluster = event->GetCluster(index_cluster);
	int side=cluster->side;
	int index = FindArrayNumber(cluster->ladder);
	//	printf("%d\n", index);
	if (cluster->ladder==4 && (cluster->address==895 || cluster->address==896)) continue;
	//	if (cluster->ladder==4) printf("%d\n", cluster->address);
	if (side==0) {
	  occupancy_S[index]->Fill(cluster->GetCoG());
	  occupancy_S_ru[index]->Fill(0.001*110*cluster->GetCoG()-align_S[index]);
	}
	else {
	  for (int mm=0; mm<5; mm++) {
	    occupancy_K[index]->Fill(cluster->GetCoG()-640+mm*384);
	    occupancy_K_ru[index]->Fill(0.001*220*(cluster->GetCoG()-640+mm*384)-align_K[index]);
	    downstream_K->Fill(Z[index], 0.001*220*(cluster->GetCoG()-640+mm*384)-align_K[index]);
	  }
	}
      }
    }
  }
  
  string outfilename = Form("%s/output_%d.pdf","output", runfirstnumbers);
  TObjArray *arrCanvas = new TObjArray();

  TF1* fitfunc = new TF1("fitfunc", "gaus", -9999999, 99999999);

  printf("**********************S********************\n");
  static TCanvas* cocc_S = NULL;
  if (!cocc_S) cocc_S = new TCanvas("C_Occupancy_S", "Occupancy_S");
  cocc_S->Divide(3,2);
  cocc_S->cd();
  for (int ii=0; ii<6; ii++) {
    cocc_S->cd(ii+1);
    occupancy_S[ii]->DrawCopy();
  }
  arrCanvas->Add(cocc_S);

  static TCanvas* cocc_S_ru = NULL;
  if (!cocc_S_ru) cocc_S_ru = new TCanvas("C_Occupancy_S_ru", "Occupancy_S_ru");
  cocc_S_ru->Divide(3,2);
  cocc_S_ru->cd();
  for (int ii=0; ii<6; ii++) {
    cocc_S_ru->cd(ii+1);
    occupancy_S_ru[ii]->DrawCopy();
    occupancy_S_ru[ii]->Fit(fitfunc, "Q");
    occupancy_S_ru[ii]->Fit(fitfunc, "Q");
    printf("%f\n", fitfunc->GetParameter(1));
  }
  arrCanvas->Add(cocc_S_ru);

  printf("**********************K********************\n");
  static TCanvas* cocc_K = NULL;
  if (!cocc_K) cocc_K = new TCanvas("C_Occupancy_K", "Occupancy_K");
  cocc_K->Divide(3,2);
  cocc_K->cd();
  for (int ii=0; ii<6; ii++) {
    cocc_K->cd(ii+1);
    occupancy_K[ii]->DrawCopy();
  }
  arrCanvas->Add(cocc_K);

  static TCanvas* cocc_K_ru = NULL;
  if (!cocc_K_ru) cocc_K_ru = new TCanvas("C_Occupancy_K_ru", "Occupancy_K_ru");
  cocc_K_ru->Divide(3,2);
  cocc_K_ru->cd();
  for (int ii=0; ii<6; ii++) {
    cocc_K_ru->cd(ii+1);
    occupancy_K_ru[ii]->DrawCopy();
    float lowlimit, highlimit;
    //before alignment && no deflection
    // if (ii==0 || ii==1) {
    //   lowlimit=50;
    //   highlimit=150;
    // }
    // else if (ii==2 || ii==3) {
    //   lowlimit=70;
    //   highlimit=170;
    // }
    // else if (ii==4 || ii==5) {
    //   lowlimit=20;
    //   highlimit=120;
    // }
    //after alignment && no deflection
    // lowlimit = -50;
    // highlimit = 50;
    if (ii==0 || ii==1 || ii==2) {
      lowlimit = -50;
      highlimit = 50;
    }
    else if (ii==3 || ii==4) {
      lowlimit=0;
      highlimit=100;
    }
    else if (ii==5) {
      lowlimit=-30;
      highlimit=80;
    }    
    occupancy_K_ru[ii]->Fit(fitfunc, "Q", "", lowlimit, highlimit);
    occupancy_K_ru[ii]->Fit(fitfunc, "Q", "", lowlimit, highlimit);
    printf("%f\n", fitfunc->GetParameter(1));
  }
  arrCanvas->Add(cocc_K_ru);

  static TCanvas* cdown_K = NULL;
  if (!cdown_K) cdown_K = new TCanvas("C_Downstream_K", "Downstream_K");
  cdown_K->cd();
  downstream_K->DrawCopy("colz");
  arrCanvas->Add(cdown_K);
  
  SavePDF(arrCanvas, outfilename.c_str());

  return 0;
}

int FindArrayNumber(int ladder){

  if (ladder==0) return 0;
  else if (ladder==1) return 1;
  else if (ladder==4) return 2;
  else if (ladder==12) return 3;
  else if (ladder==16) return 4;
  else if (ladder==17) return 5;

  return -9;
}

void SavePDF( TObjArray *arrCanvas, const char *outfilename){

  if(!arrCanvas) return;
  TCanvas *c = NULL;
  c = new TCanvas();
  c->SaveAs(Form("%s(", outfilename));
  if (c) delete c;
  
  for(int ic=0; ic<(int)arrCanvas->GetEntries(); ic++)
    {
      c = (TCanvas*)(arrCanvas->At(ic));
      c->SaveAs(Form("%s", outfilename));
    }
  c = new TCanvas();
  c->SaveAs(Form("%s)", outfilename));
  if (c) delete c;
  
  arrCanvas->Clear();
  return;

}
