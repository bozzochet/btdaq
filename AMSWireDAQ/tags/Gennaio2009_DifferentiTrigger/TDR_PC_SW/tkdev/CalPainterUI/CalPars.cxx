#include "CalPars.h"

#include <iostream>


using namespace std;


  
int   CalPars::NumPed  =  500;
int   CalPars::NumRsig =  500;
int   CalPars::NumSig  = 1000;
int   CalPars::NumNG   = 1000;
int   CalPars::low_raw_noise  = 5 ;
int   CalPars::high_raw_noise = 5 ;
int   CalPars::low_noise      = 4 ;
int   CalPars::high_noise     = 4 ;
int   CalPars::NSigNG         = 2;
//Value in PerMille
int   CalPars::PercNG         = 100;


void CalPars::SetNumEv(int ped,int rsig,int sig,int ng){
  NumPed  =  ped;
  NumRsig =  rsig;
  NumSig  =  sig;
  NumNG   =  ng;
  return;
}

void CalPars::SetFlagPar(int lr,int hr,int ll,int hh){
  low_raw_noise  = lr ;
  high_raw_noise = hr ;
  low_noise      = ll ;
  high_noise     = hh ;
  return;
}


void CalPars::PrintPars(){
  cout <<"==============================================="<<endl;
  cout <<"Calibration Paramenters" <<endl;
  cout <<endl;
  cout <<"Event Ped   =  "<<CalPars::NumPed<<endl;
  cout <<"Event RSig  =  "<<CalPars::NumRsig<<endl;
  cout <<"Event Sig   =  "<<CalPars::NumSig<<endl;
  cout <<"Event NG    =  "<<CalPars::NumNG<<endl;
  cout <<endl;
  cout <<"Sigma raw #RMS LOW  = "<<CalPars::low_raw_noise<<endl;
  cout <<"Sigma raw #RMS HIGH = "<<CalPars::high_raw_noise<<endl;
  cout <<"Sigma     #RMS LOW  = "<<CalPars::low_noise<<endl;
  cout <<"Sigma     #RMS HIGH = "<<CalPars::high_noise<<endl;
  cout <<endl;
  cout <<"Not Gaussian: requires "<<PercNG/10.<<"% of events out of +- "<<NSigNG<<" sigma"<< endl;

  cout <<"==============================================="<<endl;
}

char* CalPars::FillParString(char* pstring){ 
  sprintf(pstring,"Events: Ped=%d RSig=%d Sig=%d NG=%d | Sigma Raw  LOW=%d HIGH=%d | Sigma  LOW=%d HIGH=%d |NG Sig=%d Perc=%4.2f%%",NumPed,NumRsig,NumRsig,NumNG,low_raw_noise,high_raw_noise,low_noise,high_noise,NSigNG,PercNG/10.);

  return pstring;
}
