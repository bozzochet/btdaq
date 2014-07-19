#include "DecodeData.hh"
#include "TTree.h"
#include "TFile.h"
#include "Event.hh"
#include <iostream>

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

  sprintf(dirPGOUT,"./");
  sprintf(dirPG0,"../");
  sprintf(dirCAL0,"./");
  
  sprintf(progname,"%s",argv[0]);
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
  
  for (int hh=0;hh<NTDRS;hh++){
    sprintf(name,"lad%d",hh);
    dd1->hmio[hh]= new TH1F(name,name,1024,0,1024);
  }
  
  TTree* t4= new TTree("t4","My cluster tree");
  t4->Branch("cluster_branch","Event",&(dd1->ev),32000,2);
  t4->GetUserInfo()->Add(dd1->rh);


  int ret1=0;
  for (int ii=0;ii<10;){
    ret1=dd1->EndOfFile();
    
    if (ret1) break;

    ret1=dd1->ReadOneEvent();
    
    if (ret1==0) {
      processed++;
      t4->Fill();
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

  return 0;
}


