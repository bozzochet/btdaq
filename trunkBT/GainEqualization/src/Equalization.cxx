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
#include "TProfile.h"
#include <fstream>
#include <vector>
#include <algorithm>

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
  
  if (argc<6) {
    printf("Usage:\n");
    printf("%s <output root-filename> <target mean ADC on S> <target width on S> <target mean ADC on K> <target width on K> <first input root-filename> [second input root-filename] ...\n", argv[0]);
    return 1;
  }
  

  // Initialize variables
  int targetMeanS = atoi(argv[2]);
  int targetSigmaS = atoi(argv[3]);
  int targetMeanK = atoi(argv[4]);
  int targetSigmaK = atoi(argv[5]);
  //////////////////////////////////////////////


  TChain *chain = new TChain("t4");
     
  for (int ii=6; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }


  TString gaincorrection_filename = "gaincorrection.dat";
  TString output_filename = argv[1];

  printf("---------------------------------------------\n");
  Event::ReadGainCorrection(gaincorrection_filename.Data());
  printf("---------------------------------------------\n");

  ofstream myfile;
  myfile.open ("temp.txt");

  myfile << "#JINF" << "\t"<< "TDR" << "\t"<<   "VA" << "\t"<< "par1" << "\t"<< "par2" << "\t" << "par3" << "\n";
  
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



  //Event::GetGainCorrectionPar(int jinfnum, int tdrnum, int vanum, int component);

  int ntdrRaw = GetRH(chain)->ntdrRaw;
  int ntdrCmp = GetRH(chain)->ntdrCmp;

  struct info{
    int JINF;
    int TDR;
    TH2F *histo;

    bool operator()(const info& lhs, const info& rhs) const { lhs.TDR < rhs.TDR; }
  };
  
  info ladderinfo[ntdrRaw+ntdrCmp];

  for(int i=0; i<ntdrRaw+ntdrCmp; i++){
    ladderinfo[i].histo=new TH2F ("hADCvsPos_"+TString::Format("%d",i), "hADCvsPos_"+TString::Format("%d",i), 1024, 0, 1024, 1000, 0, 500);
    ladderinfo[i].histo->GetXaxis()->SetTitle("cog (mm)");
    ladderinfo[i].histo->GetYaxis()->SetTitle("ADC");
  }

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

    int NClusTot = ev->GetNClusTot();
    if(NClusTot==0){zeroclust++;}
    if(NClusTot==0) continue;
   
    for(int i = 0; i<NClusTot; i++){
      cl = ev->GetCluster(i);

      int clusJinf= cl->GetJinf();
      int ladder = cl->ladder;
      int side = cl->side;
      int tdrrawpos= GetRH(chain)->FindPosRaw(ladder);      
      int tdrcmppos= GetRH(chain)->FindPos(ladder);
      double clusADC = cl->GetTotSig();
      double cog = cl->GetCoG();

      //      cout << " ladder " << ladder << " tdrpos " << tdrrawpos << endl;
      
      if(tdrcmppos == -1){
	ladderinfo[tdrrawpos].histo->Fill(cog,clusADC);
	ladderinfo[tdrrawpos].JINF = clusJinf;
	ladderinfo[tdrrawpos].TDR = ladder;
      }else{
	ladderinfo[tdrrawpos].histo->Fill(cog,clusADC);
	ladderinfo[tdrcmppos].JINF = clusJinf;
        ladderinfo[tdrcmppos].TDR = ladder;	
      }
    
    }
  }

  std::sort(ladderinfo,ladderinfo+(ntdrRaw+ntdrCmp), ladderinfo[0]);
  
  for(int i=0; i<ntdrRaw+ntdrCmp; i++){
    for(int j=0; j < 16; j++){

      ladderinfo[i].histo->GetXaxis()->SetRangeUser(j*64,(j+1)*64);
      TH1D *projection = ladderinfo[i].histo->ProjectionY();
      int mean = projection->GetMean();
      int rms = projection->GetRMS();
      
      cout << "mean " << mean << " rms " << rms << endl;
      
      TF1 *f1 = new TF1("f1", "gaus", mean-10, mean+10);
      projection->Fit("f1","QR");
      
      double fitmean = f1->GetParameter(1);
      double fitrms  = f1->GetParameter(2);

      cout << "fit mean " << fitmean << " fit rms " << fitrms << endl;
      
      if(TMath::Abs(fitmean-mean) > 2*rms){
	fitmean=mean;
	fitrms=rms;
      }
      
      double par1 = targetMeanS-fitmean;
      double par2 = targetSigmaS/fitrms;
      
      myfile << ladderinfo[i].JINF << "\t" << ladderinfo[i].TDR << "\t" << j << "\t" << Form("%.2f",par1) << "\t" << Form("%.2f",par2) << "\t" << targetMeanS << "\n";
    }
  }

  sw.Stop();
  sw.Print();
 
  for(int i=0; i<ntdrRaw+ntdrCmp; i++){
    ladderinfo[i].histo->Write();
  }

  foutput->Close();
  myfile.close();

  gSystem->Exec("mv temp.txt gaincorrection.dat");

  return 0;
}
