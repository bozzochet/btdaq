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
#include "TMath.h"
#include "TSpectrum.h"
#include "TStopwatch.h"
#include <fstream>
#include <vector>

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
    printf("%s <output root-filename> <first input root-filename> [second input root-filename] ...\n", argv[0]);
    return 1;
  }
  
  TChain *chain = new TChain("t4");
     
  for (int ii=2; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }

  TString output_filename = argv[1];
  
  Event *ev;
  Cluster *cl;
  
  Long64_t entries = chain->GetEntries();
  printf("This run has %lld entries\n", entries);
  
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
  //  printf("%d\n", _maxladd);
  
  TFile* foutput = new TFile(output_filename.Data(), "RECREATE");
  foutput->cd();



  //////////////////Histos                                                                                                                                    
  TH1D *hEnergyCluster = new TH1D("hEnergyCluster","hEnergyCluster",1000,0,5000);
  hEnergyCluster->GetXaxis()->SetTitle("ADC");
  TH1D *hEnergyClusterS = new TH1D("hEnergyClusterS","hEnergyClusterS",1000,0,5000);
  hEnergyCluster->GetXaxis()->SetTitle("ADC");
  TH1D *hEnergyClusterK = new TH1D("hEnergyClusterK","hEnergyClusterK",1000,0,5000);
  hEnergyCluster->GetXaxis()->SetTitle("ADC");
  TH2D *hEnergyCluster2D = new TH2D("hEnergyCluster2D","hEnergyCluster2D",1000,0,500,1000,0,500);
  hEnergyCluster2D->GetXaxis()->SetTitle("ADC S");
  hEnergyCluster2D->GetYaxis()->SetTitle("ADC K");


  TH1D *hEnergyClusterSeed = new TH1D("hEnergyClusterSeeed","hEnergyClusterSeed",1000,0,5000);
  hEnergyClusterSeed->GetXaxis()->SetTitle("ADC");
  TH1D *hEnergyClusterSeedS = new TH1D("hEnergyClusterSeeedS","hEnergyClusterSeedS",1000,0,5000);
  hEnergyClusterSeed->GetXaxis()->SetTitle("ADC");
  TH1D *hEnergyClusterSeedK = new TH1D("hEnergyClusterSeeedK","hEnergyClusterSeedK",1000,0,5000);
  hEnergyClusterSeed->GetXaxis()->SetTitle("ADC");

  TH1D *hClusterCharge = new TH1D("hClusterCharge","hClusterCharge",200,0,20);
  hClusterCharge->GetXaxis()->SetTitle("Charge");
  TH1D *hClusterChargeS = new TH1D("hClusterChargeS","hClusterChargeS",200,0,20);
  hClusterCharge->GetXaxis()->SetTitle("Charge");
  TH1D *hClusterChargeK = new TH1D("hClusterChargeK","hClusterChargeK",200,0,20);
  hClusterCharge->GetXaxis()->SetTitle("Charge");
  

  TH1D *hClusterSN = new TH1D("hClusterSN","hClusterSN",200,0,20);
  TH1D *hClusterSNs = new TH1D("hClusterSNs","hClusterSNs",200,0,20);
  TH1D *hClusterSNk = new TH1D("hClusterSNk","hClusterSNk",200,0,20);


  TH1D *hSeedCharge = new TH1D("hSeedCharge", "hSeedCharge",200,0,20);
  hSeedCharge->GetXaxis()->SetTitle("Charge");
  TH1D *hSeedChargeS = new TH1D("hSeedChargeS", "hSeedChargeS",200,0,20);
  hSeedCharge->GetXaxis()->SetTitle("Charge");
  TH1D *hSeedChargeK = new TH1D("hSeedChargeK", "hSeedChargeK",200,0,20);
  hSeedCharge->GetXaxis()->SetTitle("Charge");

  TH1D *hClusterCog = new TH1D("hClusterCog","hClusterCog",250,0,1024);
  hClusterCog->GetXaxis()->SetTitle("cog");
  TH1D *hClusterCogS = new TH1D("hClusterCogS","hClusterCogS",250,0,1024);
  hClusterCog->GetXaxis()->SetTitle("cog");
  TH1D *hClusterCogK = new TH1D("hClusterCogK","hClusterCogK",250,0,1024);
  hClusterCog->GetXaxis()->SetTitle("cog");


  TH1D *hSeedPos = new TH1D("hSeedPos","hSeedPos",250,0,1024);                                                                                     
  hSeedPos->GetXaxis()->SetTitle("mm");  
  TH1D *hSeedPosS = new TH1D("hSeedPosS","hSeedPosS",250,0,1024);
  hSeedPos->GetXaxis()->SetTitle("mm");
  TH1D *hSeedPosK = new TH1D("hSeedPosK","hSeedPosK",250,0,1024);
  hSeedPos->GetXaxis()->SetTitle("mm");

  TH1F* hNclus = new TH1F("hclus", "hclus", 10, 0, 10);
  hNclus->GetXaxis()->SetTitle("n clusters");
  TH1F* hNclusS = new TH1F("hclusS", "hclusK", 10, 0, 10);
  hNclus->GetXaxis()->SetTitle("n clusters");
  TH1F* hNclusK = new TH1F("hclusK", "hclusK", 10, 0, 10);
  hNclus->GetXaxis()->SetTitle("n clusters");
  TH2F* hNclus2D = new TH2F("hclus2D", "hclus2D", 10, 0,10, 10, 0, 10);
  hNclus2D->GetXaxis()->SetTitle("n clusters S");
  hNclus2D->GetYaxis()->SetTitle("n clusters K")
;
  TH1F* hNstrip = new TH1F("hstrip", "hstrip", 10, 0, 10);
  hNstrip->GetXaxis()->SetTitle("n strips");
  TH1F* hNstripS = new TH1F("hstripS", "hstripK", 10, 0, 10);
  hNstrip->GetXaxis()->SetTitle("n strips");
  TH1F* hNstripK = new TH1F("hstripK", "hstripK", 10, 0, 10);
  hNstrip->GetXaxis()->SetTitle("n strips");


  TH1F* hEta = new TH1F("hEta", "hEta", 100, 0, 1);
  hEta->GetXaxis()->SetTitle("Eta"); 
  TH1F* hEtaS = new TH1F("hEtaS", "hEtaS", 100, 0, 1);
  hEta->GetXaxis()->SetTitle("Eta");
  TH1F* hEtaK = new TH1F("hEtaK", "hEtaK", 100, 0, 1);
  hEta->GetXaxis()->SetTitle("Eta");



  TH2F* hADCvsWidth = new TH2F ("hADCvsWidth", "hADCvsWidth", 10, 0, 10, 5000, 0, 1000);
  TH2F* hADCvsPos = new TH2F ("hADCvsPos", "hADCvsPos", 1024, 0, 1024, 1000, 0, 500);
  TH2F* hADCvsEta = new TH2F ("hADCvsEta", "hADCvsEta", 100, -1, 1, 5000, 0, 1000);
  TH2F* hADCvsEtaS = new TH2F ("hADCvsEtaS", "hADCvsEtaS", 100, 0, 1, 5000, 0, 1000);
  TH2F* hADCvsEtaK = new TH2F ("hADCvsEtaK", "hADCvsEtaK", 100, 0, 1, 5000, 0, 1000);
  
  TStopwatch sw;
  sw.Start();

  double perc=0;
  int zeroclust=0;  

  for (int index_event=0; index_event<entries; index_event++) {
    Double_t pperc=1000.0*((index_event+1.0)/entries);
    if (pperc>=perc) {
      printf("Processed %d out of %lld: %d%%\n", (index_event+1), entries, (int)(100.0*(index_event+1.0)/entries));
      perc++;
    }
    chain->GetEntry(index_event);

    //
    //for(int i=0; i< 1024; i++){
    //	double testADC=ev->GetRawSignal_PosNum(0,i,0);
    //	double calADC=ev->GetCalPed_PosNum(0,i,0);
    //}

    int NClusTot = ev->GetNClusTot();
    hNclus->Fill(NClusTot);
    if(NClusTot==0){zeroclust++;}
    if(NClusTot==0) continue;
    
    int clusOnS = 0;
    int clusOnK = 0;
    
    for(int i = 0; i<NClusTot; i++){
      cl = ev->GetCluster(i);                                                                                                     int ladder = cl->ladder;
      //  if(ladder!=12) continue;
      int side = cl->side;
      if(side==0){ 
	clusOnS++;}
      else{
	clusOnK++;}
    }
    
    hNclus2D->Fill(clusOnS,clusOnK);
    
    
    if(clusOnS == 1 &&  clusOnK==1){
      double clusS=-999;
      double clusK=-999;
      
      for(int i = 0; i<NClusTot;i++){
	cl = ev->GetCluster(i);
	int side = cl->side;
		
	if(side==0){
	  clusS=cl->GetTotSig();	
	}	  
	else{
	  clusK=cl->GetTotSig();
	}
      }

      hEnergyCluster2D->Fill(clusS,clusK);
    }
    
    if(clusOnS != clusOnK) continue;
    
      
    for(int i = 0; i<NClusTot; i++){
      cl = ev->GetCluster(i);
      int ladder = cl->ladder;
      int side = cl->side;
      //if(ladder!=12) continue;
      
    double clusADC = cl->GetTotSig();
    double clusSN = cl->GetTotSN();
    double cog = cl->GetCoG();
    double seedADC = cl->GetSeedVal();
    double clusCharge = cl->GetCharge();
    double seedCharge = cl->GetSeedCharge();
    double seedPos = cl->GetSeedAdd();
    double eta = cl->GetEta();
    int nstrip = cl->GetLenght();

    //if(clusADC < 250) continue;

    hEnergyCluster->Fill(clusADC);
    hEnergyClusterSeed->Fill(seedADC);
    hClusterCharge->Fill(clusCharge);
    hSeedCharge->Fill(seedCharge);
    hClusterCog->Fill(cog);
    hClusterSN->Fill(clusSN);
    hSeedPos->Fill(seedPos);
    if(nstrip==2){
    hEta->Fill(eta);
    hADCvsEta->Fill(eta,clusADC);
    }
    hNstrip->Fill(nstrip);

    hADCvsWidth->Fill(nstrip,clusADC);
    hADCvsPos->Fill(cog,clusADC);

    if(side==0){
      hEnergyClusterS->Fill(clusADC); 
      hEnergyClusterSeedS->Fill(seedADC);
      hClusterChargeS->Fill(clusCharge);
      hClusterSNs->Fill(clusSN);
      hSeedChargeS->Fill(seedCharge); 
      hClusterCogS->Fill(cog);
      hSeedPosS->Fill(seedPos);
      hNstripS->Fill(nstrip);
      if(nstrip==2){
	hEtaS->Fill(eta);
	hADCvsEtaS->Fill(eta,clusADC);
      }
    }else{
      hEnergyClusterK->Fill(clusADC); 
      hEnergyClusterSeedK->Fill(seedADC);
      hClusterChargeK->Fill(clusCharge);
      hClusterSNk->Fill(clusSN);
      hSeedChargeK->Fill(seedCharge); 
      hClusterCogK->Fill(cog);
      hSeedPosK->Fill(seedPos);
      hNstripK->Fill(nstrip);
      if(nstrip==2){
	hEtaK->Fill(eta);
	hADCvsEtaK->Fill(eta,clusADC);
      }
    }
    
    
    }    
    
  }
  
  sw.Stop();
  sw.Print();
 
  hEnergyCluster->Write();
  hEnergyClusterSeed->Write();
  hClusterCharge->Write();
  hSeedCharge->Write();
  hClusterCog->Write();
  hSeedPos->Write();
  hEta->Write();
  hNstrip->Write();
  hEnergyClusterS->Write();
  hEnergyClusterSeedS->Write();
  hClusterChargeS->Write();
  hSeedChargeS->Write();
  hClusterCogS->Write();
  hSeedPosS->Write();
  hEtaS->Write();
  hNstripS->Write();
  hEnergyClusterK->Write();
  hEnergyClusterSeedK->Write();
  hClusterChargeK->Write();
  hSeedChargeK->Write();
  hClusterCogK->Write();
  hSeedPosK->Write();
  hEtaK->Write();
  hNstripK->Write();
  hNclus2D->Write();
  hEnergyCluster2D->Write();
  hADCvsWidth->Write();
  hADCvsPos->Write();
  hADCvsEta->Write();
  hADCvsEtaS->Write();
  hADCvsEtaK->Write();
  hClusterSN->Write();
  hClusterSNs->Write();
  hClusterSNk->Write();
  //foutput->Write();
  foutput->Close();

  // cout << "Zero clust events " << zeroclust << endl;
  return 0;
}
