#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"

/* from the Matteo 'Bozzochet' Duranti's API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

#include <vector>

#define MIPCHARGE 30

using namespace std;

int macro(const char *file, int ladder) {

  //  gSystem->Load("./libEvent.so");

  TTree *branch;
  TFile *stream = new TFile(file);
  Event *current_event;
  Cluster *current_cluster;
  RHClass *current_infos;

  Long64_t entries;

  TH1F* occupancy = new TH1F("occupancy", "occupancy", 1024, 0, 1024);
  TH1F* occupancy_add = new TH1F("occupancy_add", "occupancy_add", 1024, 0, 1024);

  TH2F* autocorr = new TH2F("autocorr", "autocorr", 1024, 0, 1024, 1024, 0, 1024);
  TH2F* autocorrBZ = new TH2F("autocorrBZ", "autocorrBZ", 1024, 0, 1024, 1024, 0, 1024);

  int nbins_signal=200;
  double bin_signal[nbins_signal+1];
  bin_signal[0]=1.0;
  //  printf("%d) %f\n", 0, bin_signal[0]);
  double deltalog = (log10(10000.0)-log10(1.0))/nbins_signal;
  for (int ii=1; ii<=200; ii++) {
    bin_signal[ii] = pow(10.0, log10(bin_signal[ii-1])+deltalog);
    //    printf("%d) %f\n", ii, bin_signal[ii]);
  }
  TH1F* signalS = new TH1F("signalS", "signalS", nbins_signal, bin_signal);
  TH1F* signalK = new TH1F("signalK", "signalK", nbins_signal, bin_signal);

  TH1F* chargeS = new TH1F("chargeS", "chargeS", 200, 0, 25);
  TH1F* chargeK = new TH1F("chargeK", "chargeK", 200, 0, 25);
  TH2F* charge = new TH2F("charge", "charge", 200, 0, 25, 200, 0, 25);
  TH2F* chargeBZ = new TH2F("chargeBZ", "chargeBZ", 200, 0, 25, 200, 0, 25);
  TH2F* chargeMATCH = new TH2F("chargeMATCH", "chargeMATCH", 200, 0, 25, 200, 0, 25);

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
	    vector<double> v_chargeBZ[2];
	    vector<double> v_cog[2];
	    vector<double> v_cogBZ[2];
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
		  chargeS->Fill(sqrt(current_cluster->GetTotSig())/sqrt(MIPCHARGE));
		}
		else {
		  signalK->Fill(current_cluster->GetTotSig());
		  chargeK->Fill(sqrt(current_cluster->GetTotSig())/sqrt(MIPCHARGE));
		}
		charge_vs_occupancy->Fill(current_cluster->GetCoG(), sqrt(current_cluster->GetTotSig())/sqrt(MIPCHARGE));
		v_charge[side].push_back(sqrt(current_cluster->GetTotSig())/sqrt(MIPCHARGE));
		v_cog[side].push_back(current_cluster->GetCoG());
		if (sqrt(current_cluster->GetTotSig())/sqrt(MIPCHARGE)>3.0) {
		  v_chargeBZ[side].push_back(sqrt(current_cluster->GetTotSig())/sqrt(MIPCHARGE));
		  v_cogBZ[side].push_back(current_cluster->GetCoG());
		}
	      }
	    }
	    for (int ii=0; ii<(int)(v_charge[0].size()); ii++) {
	      for (int jj=0; jj<(int)(v_charge[1].size()); jj++) {
		charge->Fill(v_charge[0][ii], v_charge[1][jj]);
		if (fabs(v_charge[0][ii]-v_charge[1][jj])/(0.5*(v_charge[0][ii]+v_charge[1][jj]))<1.0) chargeMATCH->Fill(v_charge[0][ii], v_charge[1][jj]);
	      }
	    }
	    for (int ii=0; ii<(int)(v_chargeBZ[0].size()); ii++) {
	      for (int jj=0; jj<(int)(v_chargeBZ[1].size()); jj++) {
		chargeBZ->Fill(v_chargeBZ[0][ii], v_chargeBZ[1][jj]);
	      }
	    }
	    nclusters->Fill(ncluslad);
	    for (int ss=0; ss<2; ss++) {
	      for (int ii=0; ii<(int)(v_cog[ss].size()); ii++) {
		for (int jj=0; jj<(int)(v_cog[ss].size()); jj++) {
		  if (ii!=jj) {
		    autocorr->Fill(v_cog[ss][ii], v_cog[ss][jj]);
		  }
		}
	      }
	      for (int ii=0; ii<(int)(v_cogBZ[ss].size()); ii++) {
		for (int jj=0; jj<(int)(v_cogBZ[ss].size()); jj++) {
		  if (ii!=jj) {
		    autocorrBZ->Fill(v_cogBZ[ss][ii], v_cogBZ[ss][jj]);
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }

  static TCanvas* cnclu = NULL;
  if (!cnclu) cnclu = new TCanvas("C_NClusters", "NClusters");
  cnclu->cd();
  nclusters->DrawCopy();

  static TCanvas* cwidth = NULL;
  if (!cwidth) cwidth = new TCanvas("C_Width", "Width");
  cwidth->cd();
  width->DrawCopy();

  static TCanvas* cocc = NULL;
  if (!cocc) cocc = new TCanvas("C_Occupancy", "Occupancy");
  cocc->cd();
  occupancy->DrawCopy();

  static TCanvas* cocc_add = NULL;
  if (!cocc_add) cocc_add = new TCanvas("C_Occupancy_Address", "Occupancy_Address");
  cocc_add->cd();
  occupancy_add->DrawCopy();

  static TCanvas* csigS = NULL;
  if (!csigS) csigS = new TCanvas("C_SignalS", "SignalS");
  csigS->SetLogx();
  csigS->cd();
  signalS->DrawCopy();

  static TCanvas* csigK = NULL;
  if (!csigK) csigK = new TCanvas("C_SignalK", "SignalK");
  csigK->SetLogx();
  csigK->cd();
  signalK->DrawCopy();

  static TCanvas* ccharS = NULL;
  if (!ccharS) ccharS = new TCanvas("C_ChargeS", "ChargeS");
  ccharS->SetLogy();
  ccharS->cd();
  chargeS->DrawCopy();

  static TCanvas* ccharK = NULL;
  if (!ccharK) ccharK = new TCanvas("C_ChargeK", "ChargeK");
  ccharK->SetLogy();
  ccharK->cd();
  chargeK->DrawCopy();

  static TCanvas* cchar = NULL;
  if (!cchar) cchar = new TCanvas("C_Charge", "Charge");
  cchar->SetLogz();
  cchar->cd();
  charge->DrawCopy("colz");

  static TCanvas* ccharBZ = NULL;
  if (!ccharBZ) ccharBZ = new TCanvas("C_ChargeBZ", "ChargeBZ");
  ccharBZ->SetLogz();
  ccharBZ->cd();
  chargeBZ->DrawCopy("colz");

  static TCanvas* ccharMATCH = NULL;
  if (!ccharMATCH) ccharMATCH = new TCanvas("C_ChargeMATCH", "ChargeMATCH");
  ccharMATCH->SetLogz();
  ccharMATCH->cd();
  chargeMATCH->DrawCopy("colz");

  static TCanvas* cchargvsocc = NULL;
  if (!cchargvsocc) cchargvsocc = new TCanvas("C_Charge_vs_Occupancy", "Charge_vs_Occupancy");
  cchargvsocc->SetLogz();
  cchargvsocc->cd();
  charge_vs_occupancy->DrawCopy("colz");

  static TCanvas* cautocorr = NULL;
  if (!cautocorr) cautocorr = new TCanvas("C_AutoCorrelation", "AutoCorrelation");
  cautocorr->SetLogz();
  cautocorr->cd();
  autocorr->DrawCopy("colz");

  static TCanvas* cautocorrBZ = NULL;
  if (!cautocorrBZ) cautocorrBZ = new TCanvas("C_AutoCorrelationBZ", "AutoCorrelationBZ");
  cautocorrBZ->SetLogz();
  cautocorrBZ->cd();
  autocorrBZ->DrawCopy("colz");
  
  delete stream;
  return 0;
}
