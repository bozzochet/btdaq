#include "DecodeData.hh"
#include "TTree.h"
#include "TFile.h"
#include "Event.hh"
#include <iostream>
#include "DHF.hh"
using namespace std;


char progname[50];

int main(int argc,char** argv){
  char filename[255];
  char name[255];
  char dirPG0[255];
  char dirCAL0[255];

  char dirPGOUT[255];
  int run=110;

  int processed=0;
  int jinffailed=0;
  int readfailed=0;

  int fast=0;
  
  sprintf(dirPGOUT,"/data/Crystal_nov07/root");
  sprintf(dirPG0,"/data/Crystal_nov07/Data");
  sprintf(dirCAL0,"/data/Crystal_nov07/Calibrations");
  
  sprintf(progname,"%s",argv[0]);
  if (strstr(argv[0],"FAST")) {
    fast=1;//calling the program XXXFASTXXX we produce a fast scan of first 50000 events (if there are)
  }
  if(argc==2){
      run= atoi(argv[1]);
    }else if(argc==3){
      run= atoi(argv[1]);
      sprintf(dirPGOUT,"%s/",argv[2]);
    }else if(argc==4){
      run= atoi(argv[1]);
      sprintf(dirPGOUT,"%s/",argv[2]);
      sprintf(dirPG0,"%s/",argv[3]);
    }else if(argc==5){
      run= atoi(argv[1]);
      sprintf(dirPGOUT,"%s/",argv[2]);
      sprintf(dirPG0,"%s/",argv[3]);
      sprintf(dirCAL0,"%s/",argv[4]);

    }else{
      printf(" Usage %s <runnum>  [dirPGOUT] [dirPG0] [dirCAL0]  \n",argv[0]);
      printf(" If you do not specify directories the default is:\n");
      printf("     dirPG0 =  %s  \n",dirPG0);
      printf("     dirCAL0 = %s \n",dirCAL0);
      printf("     dirPGOUT = %s \n",dirPGOUT);
      exit(1);  
    }


  printf("reading PG0 from %s\n",dirPG0);
  printf("reading CAL0 from %s\n",dirCAL0);
  printf("writing output in %s\n",dirPGOUT);
  
  DecodeData *dd1= new DecodeData(dirPG0,dirCAL0,run);
  
  //  dd1->rh->Print();
  dd1->SetPrintOff();
  dd1->SetEvPrintOff();
  
  sprintf(filename,"%s/run_%06d.root",dirPGOUT,run);
  TFile *g=new TFile(filename,"RECREATE");
  

  
  TTree* t4= new TTree("t4","My cluster tree");
  t4->Branch("cluster_branch","Event",&(dd1->ev),32000,2);
  t4->GetUserInfo()->Add(dd1->rh);

  
  char nameh[255];
  //File di uscita
//  sprintf(nameh,"%s/Hout_%06d.root",dirPGOUT,run); 
//  TFile* h=new TFile(nameh,"recreate");
  sprintf(nameh,"Histos_%06d",run);
  DHF* Histos= new DHF(g,nameh,nameh,dd1->rh->ntdrCmp,dd1->rh->tdrCmpMap);
  Histos->Init();


  int ret1=0;
  for (int ii=0;ii<10;){
    ret1=dd1->EndOfFile();
    
    if (ret1) break;

    ret1=dd1->ReadOneEvent();
    Histos->FillAll(dd1->ev);
//    if((processed-(processed/1000)*1000)==0)  
	printf("Processed: %6d events\n",processed);
    if(fast) {
      if (processed==50000) ret1=999;
    }  
    if (ret1==0) {
      processed++;
      t4->Fill();
    }
    else if (ret1==999) {
      printf("=======================> FIRST 50K events \n");
      break;
    }
    else if(ret1==-1){
      printf("=======================> END of FILE \n");
      break;
    }
    else if(ret1<-1){
      printf("=======================> READ Error Event Skipped \n");
      readfailed++;
      break;
    }
    else{
      printf("This event failed...\n");
      jinffailed++;
    }
    
    dd1->ev->Clear();
  }
  
  
  
  t4->Write("",TObject::kOverwrite);
  
  printf("\nProcessed %5d  Events\n",processed+readfailed+jinffailed);
  printf("Accepted  %5d  Events\n",processed);
  printf("Rejected  %5d  Events --> Read Error\n",readfailed);
  printf("Rejected  %5d  Events --> Jinf/Jinj Error\n",jinffailed);
  
  delete dd1;

  g->Write("",TObject::kOverwrite);
  g->Close("R");


//  h->Write("",TObject::kOverwrite);
//  h->Close("R");
  

  return 0;
}


