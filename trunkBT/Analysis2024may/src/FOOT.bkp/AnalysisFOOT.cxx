#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TLine.h"
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

int main(int argc, const char* argv[]) {
  
  if (argc<6) {
    printf("Usage:\n");
    printf("%s <output root-filename> <TDR number> <S/N cut on S> <S/N cut on K> <min cog> <max cog> <first input root-filename> [second input root-filename] ...\n", argv[0]);
    return 1;
  }
  

  // Initialize variables for S/N ang cog cuts
  int tdrnum = atoi(argv[2]);
  int SNcutS = atoi(argv[3]);
  int SNcutK = atoi(argv[4]);
  int mincog = atoi(argv[5]);
  int maxcog = atoi(argv[6]);

  if(SNcutS<0) SNcutS=0;
  if(SNcutK<0) SNcutK=0;
  if(mincog <0) mincog=0;
  if(maxcog >1024) maxcog=1024;
  //////////////////////////////////////////////


  TChain *chain = new TChain("t4");
     
  for (int ii=7; ii<argc; ii++) {
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
    _maxtdr = GetRH(chain)->GetNTdrs();
  }
  else {
    printf("Not able to find the RHClass header in the UserInfo...\n");
    return -9;
  }

  //  printf("%d\n", _maxladd);
  
  TFile* foutput = new TFile(output_filename.Data(), "RECREATE");
  foutput->cd();


  //////////////////Histos

  TH1D *hfluctS = new TH1D("hfluctS","hfluctS",1000,-20,20);
  hfluctS->GetXaxis()->SetTitle("ADC");
  TH1D *hfluctK = new TH1D("hfluctK","hfluctK",1000,-20,20);
  hfluctK->GetXaxis()->SetTitle("ADC");
  
  TH1D *hEnergyClusterS = new TH1D("hEnergyClusterS","hEnergyClusterS",1000,0,500);
  hEnergyClusterS->GetXaxis()->SetTitle("ADC");
  TH1D *hEnergyClusterK = new TH1D("hEnergyClusterK","hEnergyClusterK",1000,0,500);
  hEnergyClusterK->GetXaxis()->SetTitle("ADC");
  TH2D *hEnergyCluster2D = new TH2D("hEnergyCluster2D","hEnergyCluster2D",1000,0,500,1000,0,500);
  hEnergyCluster2D->GetXaxis()->SetTitle("ADC S");
  hEnergyCluster2D->GetYaxis()->SetTitle("ADC K");

  TH1D *hEnergyClusterSeedS = new TH1D("hEnergyClusterSeeedS","hEnergyClusterSeedS",1000,0,500);
  hEnergyClusterSeedS->GetXaxis()->SetTitle("ADC");
  TH1D *hEnergyClusterSeedK = new TH1D("hEnergyClusterSeeedK","hEnergyClusterSeedK",1000,0,500);
  hEnergyClusterSeedK->GetXaxis()->SetTitle("ADC");

  TH1D *hPercentageSeedS = new TH1D("hPercentageSeeedS","hPercentageSeedS",200,20,100);
  hPercentageSeedS->GetXaxis()->SetTitle("percentage");
  TH1D *hPercentageSeedK = new TH1D("hPercentageSeeedK","hPercentageSeedK",200,20,100);
  hPercentageSeedK->GetXaxis()->SetTitle("percentage");

  TH1D *hPercSeedSintegral = new TH1D("hPercSeeedSintegral","hPercSeedSintegral",200,20,100);
  hPercSeedSintegral->GetXaxis()->SetTitle("percentage");
  TH1D *hPercSeedKintegral = new TH1D("hPercSeeedKintegral","hPercSeedKintegral",200,20,100);
  hPercSeedKintegral->GetXaxis()->SetTitle("percentage");
  
  
  TH1D *hClusterChargeS = new TH1D("hClusterChargeS","hClusterChargeS",1000,0,5);
  hClusterChargeS->GetXaxis()->SetTitle("Charge");
  TH1D *hClusterChargeK = new TH1D("hClusterChargeK","hClusterChargeK",1000,0,5);
  hClusterChargeK->GetXaxis()->SetTitle("Charge");
  
  TH1D *hClusterSNs = new TH1D("hClusterSNs","hClusterSNs",200,0,100);
  hClusterSNs->GetXaxis()->SetTitle("S/N");
  TH1D *hClusterSNk = new TH1D("hClusterSNk","hClusterSNk",200,0,100);
  hClusterSNk->GetXaxis()->SetTitle("S/N");

  TH1D *hSeedSNs = new TH1D("hSeedSNs","hSeedSNs",200,0,100);
  hSeedSNs->GetXaxis()->SetTitle("S/N");
  TH1D *hSeedSNk = new TH1D("hSeedSNk","hSeedSNk",200,0,100);
  hSeedSNk->GetXaxis()->SetTitle("S/N");


  TH1D *hSeedSNs_1strip = new TH1D("hSeedSNs_1strip","hSeedSNs_1strip",200,0,100);
  hSeedSNs_1strip->GetXaxis()->SetTitle("S/N");
  TH1D *hSeedSNk_1strip = new TH1D("hSeedSNk_1strip","hSeedSNk_1strip",200,0,100);
  hSeedSNk_1strip->GetXaxis()->SetTitle("S/N");

  TH1D *hSeedSNs_2strip = new TH1D("hSeedSNs_2strip","hSeedSNs_2strip",200,0,100);
  hSeedSNs_2strip->GetXaxis()->SetTitle("S/N");
  TH1D *hSeedSNk_2strip = new TH1D("hSeedSNk_2strip","hSeedSNk_2strip",200,0,100);
  hSeedSNk_2strip->GetXaxis()->SetTitle("S/N");


  TH1D *hSeedChargeS = new TH1D("hSeedChargeS", "hSeedChargeS",1000,0,5);
  hSeedChargeS->GetXaxis()->SetTitle("Charge");
  TH1D *hSeedChargeK = new TH1D("hSeedChargeK", "hSeedChargeK",1000,0,5);
  hSeedChargeK->GetXaxis()->SetTitle("Charge");


  TH1D *hEnergyClusterSeedS_1strip = new TH1D("hEnergyClusterSeedS_1strip", "hEnergyClusterSeedS_1strip",1000,0,500);
  hEnergyClusterSeedS_1strip->GetXaxis()->SetTitle("ADC");
  TH1D *hEnergyClusterSeedK_1strip = new TH1D("hEnergyClusterSeedK_1strip", "hEnergyClusterSeedK_1strip",1000,0,500);
  hEnergyClusterSeedK_1strip->GetXaxis()->SetTitle("ADC");

  TH1D *hEnergyClusterSeedS_2strip = new TH1D("hEnergyClusterSeedS_2strip", "hEnergyClusterSeedS_2strip",1000,0,500);
  hEnergyClusterSeedS_2strip->GetXaxis()->SetTitle("ADC");
  TH1D *hEnergyClusterSeedK_2strip = new TH1D("hEnergyClusterSeedK_2strip", "hEnergyClusterSeedK_2strip",1000,0,500);
  hEnergyClusterSeedK_2strip->GetXaxis()->SetTitle("ADC");


  TH1D *hClusterCogS = new TH1D("hClusterCogS","hClusterCogS",250,0,1024);
  hClusterCogS->GetXaxis()->SetTitle("cog");
  TH1D *hClusterCogK = new TH1D("hClusterCogK","hClusterCogK",250,0,1024);
  hClusterCogK->GetXaxis()->SetTitle("cog");

  TH1D *hSeedPosS = new TH1D("hSeedPosS","hSeedPosS",250,0,1024);
  hSeedPosS->GetXaxis()->SetTitle("mm");
  TH1D *hSeedPosK = new TH1D("hSeedPosK","hSeedPosK",250,0,1024);
  hSeedPosK->GetXaxis()->SetTitle("mm");

  TH1F* hNclus = new TH1F("hclus", "hclus", 10, 0, 10);
  hNclus->GetXaxis()->SetTitle("n clusters");
  TH1F* hNclusS = new TH1F("hclusS", "hclusK", 10, 0, 10);
  hNclusS->GetXaxis()->SetTitle("n clusters");
  TH1F* hNclusK = new TH1F("hclusK", "hclusK", 10, 0, 10);
  hNclusK->GetXaxis()->SetTitle("n clusters");
  TH2F* hNclus2D = new TH2F("hclus2D", "hclus2D", 10, 0,10, 10, 0, 10);
  hNclus2D->GetXaxis()->SetTitle("n clusters S");
  hNclus2D->GetYaxis()->SetTitle("n clusters K");
  
  TH1F* hNstrip = new TH1F("hstrip", "hstrip", 10, -0.5, 9.5);
  hNstrip->GetXaxis()->SetTitle("n strips");
  TH1F* hNstripS = new TH1F("hstripS", "hstripK", 10, -0.5, 9.5);
  hNstripS->GetXaxis()->SetTitle("n strips");
  TH1F* hNstripK = new TH1F("hstripK", "hstripK", 10, -0.5, 9.5);
  hNstripK->GetXaxis()->SetTitle("n strips");

  TH1F* hEtaS = new TH1F("hEtaS", "hEtaS", 100, 0, 1);
  hEtaS->GetXaxis()->SetTitle("Eta");
  TH1F* hEtaK = new TH1F("hEtaK", "hEtaK", 100, 0, 1);
  hEtaK->GetXaxis()->SetTitle("Eta");



  TH2F* hADCvsWidth = new TH2F ("hADCvsWidth", "hADCvsWidth", 10, 0, 10, 1000, 0, 500);
  hADCvsWidth->GetXaxis()->SetTitle("# of strips");
  hADCvsWidth->GetYaxis()->SetTitle("ADC");

  TH2F* hADCvsPos = new TH2F ("hADCvsPos", "hADCvsPos", 1024, 0, 1024, 1000, 0, 500);
  hADCvsPos->GetXaxis()->SetTitle("cog");
  hADCvsPos->GetYaxis()->SetTitle("ADC");

  TH2F* hADCvsEtaS = new TH2F ("hADCvsEtaS", "hADCvsEtaS", 100, 0, 1, 1000, 0, 500);
  hADCvsEtaS->GetXaxis()->SetTitle("eta");
  hADCvsEtaS->GetYaxis()->SetTitle("ADC");
  
  TH2F* hADCvsEtaK = new TH2F ("hADCvsEtaK", "hADCvsEtaK", 100, 0, 1, 1000, 0, 500);
  hADCvsEtaK->GetXaxis()->SetTitle("eta");
  hADCvsEtaK->GetYaxis()->SetTitle("ADC");

  TH2F* hADCvsSNs = new TH2F ("hADCvsSNs", "hADCvsSNs", 500, 0, 50, 1000, 0, 500);
  hADCvsSNs->GetXaxis()->SetTitle("S/N");
  hADCvsSNs->GetYaxis()->SetTitle("ADC");

  TH2F* hADCvsSNk = new TH2F ("hADCvsSNk", "hADCvsSNk", 500, 0, 50, 1000, 0, 500);
  hADCvsSNk->GetXaxis()->SetTitle("S/N");
  hADCvsSNk->GetYaxis()->SetTitle("ADC");

  TH2F* hNStripvsSNs = new TH2F ("hNstripvsSNs", "hNstripvsSNs", 1000, 0, 50, 5, 0, 5);
  hNStripvsSNs->GetXaxis()->SetTitle("S/N");
  hNStripvsSNs->GetYaxis()->SetTitle("# of strips");
  TH2F* hNStripvsSNk = new TH2F ("hNstripvsSNk", "hNstripvsSNk", 1000, 0, 50, 5, 0, 5);
  hNStripvsSNk->GetXaxis()->SetTitle("S/N");
  hNStripvsSNk->GetYaxis()->SetTitle("# of strips");

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

    //Skip events with no clusters
    int NClusTot = ev->GetNClusTot();
    hNclus->Fill(NClusTot);
    if(NClusTot==0){

      for(int chan=0; chan<640; chan++){
	//cout << ev->GetCN_PosNum(0,(int)chan/64,0) << endl;
	double fluctS=ev->GetRawSignal_PosNum(0,chan,0)-ev->GetCalPed_PosNum(0,chan,0)-ev->GetCN_PosNum(0,(int)chan/64,0);
	hfluctS->Fill(fluctS);
      }

      for(int chan=640; chan<1024; chan++){
	
	//cout << ev->GetCN_PosNum(0,(int)chan/64,0) << endl;
	double fluctK=ev->GetRawSignal_PosNum(0,chan,0)-ev->GetCalPed_PosNum(0,chan,0)-ev->GetCN_PosNum(0,(int)chan/64,0);
	hfluctK->Fill(fluctK);
      }
      
      
	zeroclust++;}
      if(NClusTot==0) continue;
    
    
    int clusOnS = 0;
    int clusOnK = 0;
    

    //Calculate number of clusters on each side
    for(int i = 0; i<NClusTot; i++){
      cl = ev->GetCluster(i);                                                                                                     int ladder= cl->ladder;
      int side = cl->side;
      if(side==0){ 
	clusOnS++;}
      else{
	clusOnK++;}
    }
    hNclus2D->Fill(clusOnS,clusOnK);
   


    //Skip events with more than 1 cluster on either side
    //    if(clusOnS != 1 || clusOnK != 1) continue;
    //    if(clusOnS != clusOnK) continue;
   

    //Signal Correlation
    if(clusOnS == clusOnK){
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
      
   
    for(int i = 0; i<NClusTot; i++){
      cl = ev->GetCluster(i);
      int ladder = cl->ladder;
      if(ladder != tdrnum) continue;
      
      int side = cl->side;
      
      double clusADC = cl->GetTotSig();
      double clusSN = cl->GetTotSN();
      double cog = cl->GetCoG();
      double seedADC = cl->GetSeedVal();
      double seedSN = cl->GetSeedSN();
      double clusCharge = cl->GetCharge();
      double seedCharge = cl->GetSeedCharge();
      double seedPos = cl->GetSeedAdd();
      double eta = cl->GetEta();
      int nstrip = cl->GetLenght();
      bool onborder = cl->BorderRegion();

      //      if(onborder) continue;
      
    /*
    if(onborder){
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    cout << "Cluster COG " << cog << endl;
    cout << "Cluster on border " << onborder << endl;
    }
    */
    
    double SeedPercentage = 100 * seedADC/clusADC;
    if(side == 0){
      hClusterSNs->Fill(clusSN);
      hSeedSNs->Fill(seedSN);
      
      if(nstrip == 1){    
	hSeedSNs_1strip->Fill(seedSN);
      }else if(nstrip == 2){
      	hSeedSNs_2strip->Fill(seedSN);
      }      
    }else{
      hClusterSNk->Fill(clusSN);
      hSeedSNk->Fill(seedSN);
      
      if(nstrip == 1){  
        hSeedSNk_1strip->Fill(seedSN);
      }else if(nstrip == 2){
        hSeedSNk_2strip->Fill(seedSN);
      } 
      
    }

    //Cuts on S/N of cluster
    if(side == 0){
      if(clusSN < SNcutS) continue;
    }else{
      if(clusSN < SNcutK) continue;
    }


    //Cuts on cog of cluster
    //    if(!((cog > mincogS && cog < maxcogS) || (cog > mincogK && cog < maxcogK))) continue;

    if(cog < mincog || cog > maxcog) continue;

    hNstrip->Fill(nstrip);
    hADCvsWidth->Fill(nstrip,clusADC);
    hADCvsPos->Fill(cog,clusADC);

    if(side==0){
      hEnergyClusterS->Fill(clusADC); 
      hEnergyClusterSeedS->Fill(seedADC);
      if(nstrip == 1){
        hEnergyClusterSeedS_1strip->Fill(seedADC);
      }else if(nstrip == 2){
        hEnergyClusterSeedS_2strip->Fill(seedADC);
	hPercentageSeedS->Fill(SeedPercentage);
      }
      hClusterChargeS->Fill(clusCharge);
      hADCvsSNs->Fill(clusSN,clusADC);
      hNStripvsSNs->Fill(clusSN,nstrip);
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
      if(nstrip == 1){
        hEnergyClusterSeedK_1strip->Fill(seedADC);
      }else if(nstrip == 2){
        hEnergyClusterSeedK_2strip->Fill(seedADC);
	hPercentageSeedK->Fill(SeedPercentage);
      }
      hClusterChargeK->Fill(clusCharge);
      hADCvsSNk->Fill(clusSN,clusADC);
      
      hNStripvsSNk->Fill(clusSN,nstrip);      
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

  hfluctS->Write();
  hfluctK->Write();
  hNstrip->Write();
  hEnergyClusterS->Write();
  hEnergyClusterSeedS->Write();
  hEnergyClusterSeedS_1strip->Write();
  hEnergyClusterSeedS_2strip->Write();
  hClusterChargeS->Write();
  hSeedChargeS->Write();
  hClusterCogS->Write();
  hSeedPosS->Write();
  hEtaS->Write();
  hNstripS->Write();
  hEnergyClusterK->Write();
  hEnergyClusterSeedK->Write();
  hEnergyClusterSeedK_1strip->Write();
  hEnergyClusterSeedK_2strip->Write();
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

  hADCvsEtaS->Write();
  hADCvsEtaK->Write();

  hClusterSNs->Write();
  hClusterSNk->Write();

  hSeedSNs->Write();
  hSeedSNk->Write();
  hSeedSNs_1strip->Write();
  hSeedSNk_1strip->Write();
  hSeedSNs_2strip->Write();
  hSeedSNk_2strip->Write();

  hADCvsSNs->Write();
  hADCvsSNk->Write();
  
  hNStripvsSNs->Write();
  hNStripvsSNk->Write();
    
  hPercentageSeedS->Write();
  hPercentageSeedK->Write();

  Float_t sumS = 0;
  Float_t sumK = 0;
  for (Int_t i=1;i<=200;i++) {
    sumS += hPercentageSeedS->GetBinContent(i);
    sumK += hPercentageSeedK->GetBinContent(i);
    hPercSeedSintegral->SetBinContent(i,sumS);
    hPercSeedKintegral->SetBinContent(i,sumK);
  }
  

  
  hPercSeedSintegral->Write();
  hPercSeedKintegral->Write();

  //foutput->Write();
  foutput->Close();

  // cout << "Zero clust events " << zeroclust << endl;
  return 0;
}
