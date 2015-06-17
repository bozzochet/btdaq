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

/* from the Matteo 'Bozzochet' Duranti's API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

#include <vector>

using namespace std;

void SavePDF( TObjArray *arrCanvas, const char *outfilename);

int macro(const long run, int anc, int ladder) {

  //  gSystem->Load("./libEvent.so");

  TTree *branch;
  string file = Form("RootData/run_%ld_ANC_%d.root", run, anc);
  TFile *stream = new TFile(file.c_str());

  Event *current_event;
  Cluster *current_cluster;
  RHClass *current_infos;

  Long64_t entries;

  TH1F* occupancy = new TH1F("occupancy", "occupancy", 1024, 0, 1024);
  TH1F* occupancy_add = new TH1F("occupancy_add", "occupancy_add", 1024, 0, 1024);

  bool signal_log=false;

  int nbins_signal=200;
  double bin_signal[nbins_signal+1];
  if(signal_log)
    {
      bin_signal[0]=1.0;
      double deltalog = (log10(10000.0)-log10(1.0))/nbins_signal;
      for (int ii=1; ii<=nbins_signal; ii++) { bin_signal[ii] = pow(10.0, log10(bin_signal[ii-1])+deltalog); }
    }
  else
    {
      double sigmin=0, sigmax=200;
      double delta = (sigmax-sigmin)/nbins_signal;
      for (int ii=0; ii<=nbins_signal; ii++) { bin_signal[ii] = sigmin + ii*delta; }
    }

  TH1F* signalS = new TH1F("signalS", "signalS", nbins_signal, bin_signal);
  TH1F* signalK = new TH1F("signalK", "signalK", nbins_signal, bin_signal);

  TH1F* chargeS = new TH1F("chargeS", "chargeS", 200, 0, 25);
  TH1F* chargeK = new TH1F("chargeK", "chargeK", 200, 0, 25);
  TH2F* charge = new TH2F("charge", "charge", 200, 0, 25, 200, 0, 25);

  TH2F* charge_vs_occupancy = new TH2F("charge_vs_occupancy", "charge_vs_occupancy", 1024, 0, 1024, 200, 0, 25);

  TH1F* nclusters = new TH1F("nclusters", "nclusters", 40, 0, 40);

  TH1F* width = new TH1F("width", "width", 128, 0, 128);

  if (stream->IsOpen()) {
    if ((branch = (TTree *)(stream->Get("t4")))) {
      entries = branch->GetEntries();
      printf("This run has %lld entries\n", entries);
      if (entries > 0) {
	current_event = new Event();
	branch->SetBranchAddress("cluster_branch", &current_event);
	branch->GetEntry(0);
	if ((current_infos = (RHClass *)branch->GetUserInfo()->At(0))) {
	  for (int index_event = 0; index_event < entries; ++index_event) {
	    branch->GetEntry(index_event);
	    int NClusTot = current_event->NClusTot;
	    int ncluslad = 0;
	    vector<double> v_charge[2];
	    for (int index_cluster = 0; index_cluster < NClusTot; ++index_cluster) {
	      current_cluster = current_event->GetCluster(index_cluster);
	      int this_lad = current_cluster->ladder;
	      if (this_lad == ladder) {
		ncluslad++;
		occupancy->Fill(current_cluster->GetCoG());
		occupancy_add->Fill(current_cluster->address);
		width->Fill(current_cluster->length);
		int side=current_cluster->side;
		if (side==0) {
		  signalS->Fill(current_cluster->GetTotSig());
		  chargeS->Fill(sqrt(current_cluster->GetTotSig())/sqrt(35.0));
		}
		else {
		  signalK->Fill(current_cluster->GetTotSig());
		  chargeK->Fill(sqrt(current_cluster->GetTotSig())/sqrt(35.0));
		}
		charge_vs_occupancy->Fill(current_cluster->GetCoG(), sqrt(current_cluster->GetTotSig())/sqrt(35.0));
		v_charge[side].push_back(sqrt(current_cluster->GetTotSig())/sqrt(35.0));
	      }
	    }
	    for (int ii=0; ii<(int)(v_charge[0].size()); ii++) {
	      for (int jj=0; jj<(int)(v_charge[1].size()); jj++) {
		charge->Fill(v_charge[0][ii], v_charge[1][jj]);
	      }
	    }
	    nclusters->Fill(ncluslad);
	  }
	}
      }
    }
  
    string outfilename = Form("%s/output_%ld_%d_lad_%d.pdf","output",run,anc,ladder);
    TObjArray *arrCanvas = new TObjArray();
    
    static TCanvas* cnclu = NULL;
    if (!cnclu) cnclu = new TCanvas("C_NClusters", "NClusters"); arrCanvas->Add(cnclu);
    cnclu->cd();
    nclusters->DrawCopy();
    arrCanvas->Add(cnclu);
    
    static TCanvas* cwidth = NULL;
    if (!cwidth) cwidth = new TCanvas("C_Width", "Width");
    cwidth->cd();
    width->DrawCopy();
    arrCanvas->Add(cwidth);
    
    static TCanvas* cocc = NULL;
    if (!cocc) cocc = new TCanvas("C_Occupancy", "Occupancy");
    cocc->cd();
    occupancy->DrawCopy();
    arrCanvas->Add(cocc);
    
    static TCanvas* cocc_add = NULL;
    if (!cocc_add) cocc_add = new TCanvas("C_Occupancy_Address", "Occupancy_Address");
    cocc_add->cd();
    occupancy_add->DrawCopy();
    arrCanvas->Add(cocc_add);
    
    static TCanvas* csigS = NULL;
    if (!csigS) csigS = new TCanvas("C_SignalS", "SignalS");
    if(signal_log) csigS->SetLogx();
    csigS->cd();
    signalS->DrawCopy();
    arrCanvas->Add(csigS);

    static TCanvas* csigK = NULL;
    if (!csigK) csigK = new TCanvas("C_SignalK", "SignalK");
    if(signal_log) csigK->SetLogx();
    csigK->cd();
    signalK->DrawCopy();
    arrCanvas->Add(csigK);
    
    static TCanvas* ccharS = NULL;
    if (!ccharS) ccharS = new TCanvas("C_ChargeS", "ChargeS");
    ccharS->SetLogy();
    ccharS->cd();
    chargeS->DrawCopy();
    arrCanvas->Add(ccharS);
    
    static TCanvas* ccharK = NULL;
    if (!ccharK) ccharK = new TCanvas("C_ChargeK", "ChargeK");
    ccharK->SetLogy();
    ccharK->cd();
    chargeK->DrawCopy();
    arrCanvas->Add(ccharK);

    static TCanvas* cchar = NULL;
    if (!cchar) cchar = new TCanvas("C_Charge", "Charge");
    cchar->SetLogz();
    cchar->cd();
    charge->DrawCopy("colz");
    arrCanvas->Add(cchar);
    
    static TCanvas* cchargvsocc = NULL;
    if (!cchargvsocc) cchargvsocc = new TCanvas("C_Charge_vs_Occupancy", "Charge_vs_Occupancy");
    cchargvsocc->SetLogz();
    cchargvsocc->cd();
    charge_vs_occupancy->DrawCopy("colz");
    arrCanvas->Add(cchargvsocc);
    
    SavePDF(arrCanvas, outfilename.c_str());
  }

  delete stream;
  return 0;
}

void SavePDF( TObjArray *arrCanvas, const char *outfilename){

  if(!arrCanvas) return;
  TCanvas *c = NULL;
  c = new TCanvas();
  c->SaveAs(Form("%s(", outfilename));
  for(int ic=0; ic<(int)arrCanvas->GetEntries(); ic++)
    {
      c = (TCanvas*)(arrCanvas->At(ic));
      c->SaveAs(Form("%s", outfilename));
    }
  c = new TCanvas();
  c->SaveAs(Form("%s)", outfilename));
  
  arrCanvas->Clear();
  return;

}
