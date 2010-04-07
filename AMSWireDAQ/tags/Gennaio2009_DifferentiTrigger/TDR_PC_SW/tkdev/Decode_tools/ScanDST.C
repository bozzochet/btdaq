#include "Event.h"
#include "TrDHF.h"
#include "TrRecon.h"
#include "VCon_root.h"

#include "TBranch.h"
#include "TTree.h"
#include "TFile.h"

#include <iostream>

using namespace std;

int main(int argc,char** argv){ 

  // Parsing options   
  if (argc<2) {
    printf("Usage: %s <RunNumber> [MaxEntries] [RunFileDirectory]\n",argv[0]);
    printf("       writes ouput in ScanDST_<RunNumber>.root\n");
    exit(1);  
  }
  int  run   = atoi(argv[1]);
  int  maxev = (argc>1) ? atoi(argv[2]) : 99999999;
  char dir[255]; 
  (argc>2) ? sprintf(dir,"%s",argv[3]) : sprintf(dir,"./");

  // Options

 
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();




  // For Vitaly's Container structure
  //myholder *hold=new myholder();

 
  // Create the reconstruction object
  TrRecon::Create();
  TrRecon::UsingTrCalDB(TrCalDB::Head);
  
  TrRecon::gethead()->SetParFromDataCards();
  TrRecon* trec=TrRecon::gethead();
  trec->SetThrSeed(0,3.5); trec->SetThrNeig(0,1.);
  trec->SetThrSeed(1,3.5); trec->SetThrNeig(1,1.);
  trec->SetThrProb(0);

  TrRecon::TRCon= new VCon_root();




 
  // Input file
  char filename[255];
  Event* dst = new Event(); 
  sprintf(filename,"%s/data_%d.root",dir,run);
  TFile*   file = TFile::Open(filename,"r");
  TTree*   t4   = (TTree*) file->Get("t4"); 
  TBranch* bra  = t4->GetBranch("Tracker"); 
  bra->SetAddress(&dst);
  

  TrCalDB::Head= (TrCalDB*)file->Get("TrCalDB");
  TrClusterR::UsingTrCalDB(TrCalDB::Head);
  TrRawClusterR::UsingTrCalDB(TrCalDB::Head);

  // Output file
  sprintf(filename,"ScanDST_%d.root",run);   
  TFile* outfile = TFile::Open(filename,"recreate");
  outfile->cd();
  TrDHF* Histos = new TrDHF(outfile,"histos","histos");

  // Generic informations
  Histos->DefineTracker("nActiveLaddersA","; n. active ladders per event",100,0.,100.,0);
  Histos->DefineTracker("nActiveLaddersB","; n. active ladders per event",100,0.,100.,0);
  Histos->DefineTracker("nTrRawClustersA","; n. raw cluster per event",100,0.,100.,1);
  Histos->DefineTracker("nTrRawClustersB","; n. raw cluster per event",100,0.,100.,1);
  // Ladders information
  Histos->Define("SignalA","; cluster amplitude (ADC)",600,0.,300.,1);
  Histos->Define("SignalB","; cluster amplitude (ADC)",600,0.,300.,1);
  Histos->Define("Signal_SeedAddA","; seed address; cluster amplitude (ADC)",1024,0.,1024.,300,0.,600.);
  Histos->Define("Signal_SeedAddB","; seed address; cluster amplitude (ADC)",1024,0.,1024.,300,0.,600.);
  Histos->Define("OccupancyA","; seed address",1024,0.,1024.,0);
  Histos->Define("OccupancyB","; seed address",1024,0.,1024.,0);
  Histos->Define("MultiplicityA","; n. strips",100,0.,100.,1);
  Histos->Define("MultiplicityB","; n. strips",100,0.,100.,1);
  float SNA = 3.50;
  float SNB = 5.00;
  int   nfew = 0;
  int   ntoo = 0; 

  // Loop
  Int_t nentries = t4->GetEntries();
  printf("Number of entries: %d\n",nentries);
  nentries = (nentries<maxev) ? nentries : maxev;
  float pperc = 0.;
  float perc  = 0.; 
  for (int ii=0; ii<nentries; ii++) {
    perc = ii/(nentries*1.);
    if (perc>=pperc) { printf("Processed %3.0f%%\n",pperc*100); pperc+=0.1; }
    dst->Clear();  
    t4->GetEntry(ii);
    
    // number of raw clusters
    int nraw = dst->GetnTrRawClusters();

    // counting
    int nrawPA = 0; 
    int nrawNA = 0;
    int nrawPB = 0; 
    int nrawNB = 0;
    int nactiveA = 0;
    int nactiveB = 0;
    int usedA[192]; 
    int usedB[192]; 
    for (int gg=0; gg<192; gg++) { 
      usedA[gg] = 0; 
      usedB[gg] = 0; 
    }
    for (int rr=0; rr<nraw; rr++) {
      TrRawClusterR* raw = dst->GetTrRawCluster(rr);
      if (raw==0) continue; // no cluster	
      float seedSN = raw->GetSeedSN(3.50);
      if (seedSN<0) continue; // no seed	
      // number of raw clusters
      ( (raw->GetSide()==0)&&(seedSN>SNA) ) ? nrawPA++ : nrawNA++;
      ( (raw->GetSide()==0)&&(seedSN>SNB) ) ? nrawPB++ : nrawNB++;
      // number of active ladders
      int tkid  = raw->GetTkId();
      int entry = TkDBc::Head->TkId2Entry(tkid);
      if ( (usedA[entry]==0)&&(seedSN>SNA) ) { nactiveA++; usedA[entry] = 1; }
      if ( (usedB[entry]==0)&&(seedSN>SNB) ) { nactiveB++; usedB[entry] = 1; }
      // raw clusters signal 
      float signalA = raw->GetTotSignal(SNA,1.);
      float signalB = raw->GetTotSignal(SNB,1.);
      Histos->Fill(1,raw,"SignalA",signalA);
      Histos->Fill(1,raw,"SignalB",signalB);
      int nstripA  = raw->GetLength(SNA,1.);
      int nstripB  = raw->GetLength(SNB,1.);
      int seedaddA = raw->GetSeedAddress(SNA);
      int seedaddB = raw->GetSeedAddress(SNB);
      Histos->Fill(0,raw,"Signal_SeedAddA",(float)seedaddA,signalA);
      Histos->Fill(0,raw,"Signal_SeedAddB",(float)seedaddB,signalB);
      Histos->Fill(0,raw,"OccupancyA",(float)seedaddA);
      Histos->Fill(0,raw,"OccupancyB",(float)seedaddB);
      Histos->Fill(1,raw,"MultiplicityA",(float)nstripA);
      Histos->Fill(1,raw,"MultiplicityB",(float)nstripB);
    }
    Histos->FillTracker("nActiveLaddersA",nactiveA);
    Histos->FillTracker("nActiveLaddersB",nactiveB);
    Histos->FillTracker(0,"nTrRawClustersA",nrawPA);
    Histos->FillTracker(0,"nTrRawClustersB",nrawPB);
    Histos->FillTracker(1,"nTrRawClustersA",nrawNA);
    Histos->FillTracker(1,"nTrRawClustersB",nrawNB);
   
    // TrClusters
    // trec->BuildTrClusters();
    
    // trec->PrintEvent();

    // strange events (by number of raw clusters)
    if (nraw<1)   { 
      nfew++; 
      // printf("Event %7d without TrRawClusters\n",ii);       
      continue; 
    }
    if (nraw>500) { 
      ntoo++; 
      // printf("Event %7d with nTrRawClusters = %4d\n",ii,nraw); 
      continue; 
    }
  }
 
  printf("NEvents %7d without TrRawClusters\n",nfew);       
  printf("NEvents %7d with nTrRawClusters>500 \n",ntoo); 
	
  file->Close();
  outfile->Write();
  outfile->Close();
  
  return 0;
}
