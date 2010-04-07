#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "Event.h"
//#include "TCanvas.h"
#include "TNtuple.h"
//#include "TProfile2D.h"
#include "TrDHF.h"

using namespace std;

int main(int argc,char** argv){          // Inizializzazione variabili
  char name[255];
  char filename[255];
  char dir1[255];
  int run;
  int maxev=1000000;
  char scanname[20];

 
  sprintf(dir1,"./" );  //scrive sul file

  if(argc==2){
    run=atoi(argv[1]);
    maxev= 99999999;
  }
  else if(argc==3){
    run=atoi(argv[1]);
    maxev= atoi(argv[2]);
  }else{
    printf(" Usage %s   run [maxev]\n",argv[0]);
    printf(" It seaches for files in the directory:\n");
    printf("    %s  \n",dir1);
    printf("Write in ouput Houtdst_<run>.root\n");
    exit(1);  
  }
  
  
  Event* ev=new Event(); //nuova variabile dst 
  //File di uscita
  sprintf(name,"Houtdst_%d.root",run);   
  TFile gg(name,"recreate");
  TrDHF* Histos= new TrDHF(gg,"HISTOS","HISTOS");

  Histos->BookHistos();
  
  sprintf(filename,"%s/data_%d.root",dir1,run);
  TFile* f= TFile::Open(filename);  // apri il file(i file stanno in Crystal/tb...)
    
    
  //lettura TTree4
  TTree *t4=(TTree*) f->Get("t4"); 
  TBranch *bra=t4->GetBranch("Tracker"); 
  bra->SetAddress(&dst);
  
    
  
    

    
    Int_t ntot=t4->GetEntries(); // total number of events 
    float pperc=0;
    float perc;
    int nentries= ((maxev<ntot)? maxev : ntot);
    cout<< " Number of entries : "<<ntot<< " maxev " <<maxev<<endl;
    
    for (int ii=0;ii<nentries;ii++){
      perc=ii/(nentries*1.);
      if (perc>=pperc){printf("Processed %5.0f%%\n",pperc*100);pperc+=0.1;}
      dst->Clear();  
      t4->GetEntry(ii);
      //End entries loop
      Histos->FillAll(dst);
    }
  
    
    f->Close();
    
  
    gg->Write();
    gg->Close();
    
    
  return 0;
  
}

