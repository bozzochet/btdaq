#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <TApplication.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TLegend.h>
#include <TPolyLine.h>
#include <TGaxis.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>

#include "DataFileHandler.h"

using namespace std;

//command line variables
string inputCCFileName = "";
string inputAMSFileName = "";
bool adc_mip=true; // true -> use ADC, false -> use MIP (small diodes are not well calibrated so far)
int bunch_length = 10000;
int DAMPETDRNUM[] = {15,14,11,10,7,6,3,2};
int DAMPENTDR = 8;
//int COLOR[] = {kRed,kBlue,kGreen,kYellow};

void HandleInputPar(int argc, char **argv)
{
  stringstream usage;
  usage.clear();
  usage << endl;
  usage << "Usage:" << endl << endl;
  usage << "CALOCUBE OPTIONS" << endl;
  usage << "-C\t\t\tname of the input CC file" << endl;
  usage << "-A\t\t\tname of the input AMS file" << endl;
  usage << "-h\t\t\thelp" << endl << endl;

  int c;
  
  while ((c = getopt(argc, argv, "A:V:C:r:abh")) != -1){
    switch (c) {
    case 'C':
      inputCCFileName = optarg;
      break;
    case 'A':
      inputAMSFileName = optarg;
      break;
    case 'h':
      cout << usage.str().c_str() << endl;
      exit(0);
      break;
    default:
      cout << usage.str().c_str() << endl;
      exit(0);
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv){

  HandleInputPar(argc, argv);
  
  int tmp=1;
  TApplication theApp("App",&tmp,argv);
  
  gROOT->Reset();
  if (inputCCFileName=="" && inputAMSFileName=="") {
    cerr << "No imput files found, exit" << endl;
    exit(EXIT_FAILURE);
  }

  // open files and trees
  if (inputCCFileName!="") {
    DataFileHandler::GetIstance().ReadCCFile(inputCCFileName,adc_mip);
  } else {
    cerr << "No CC file is present, exit." << endl;
    exit(EXIT_FAILURE);
  }
  if (inputAMSFileName!="") {
    DataFileHandler::GetIstance().ReadAMSFile(inputAMSFileName);
  } else {
    cout << "No AMS file is present, exit."<< endl;
    exit(EXIT_FAILURE);
  }

  RHClass* rh=GetRH(DataFileHandler::GetIstance().fTAMS);
  if (rh) {
    rh->Print();
  }
  else {
    printf("Not able to find the RHClass header in the UserInfo...\n");
    exit(1);
  }

  int Nev =  DataFileHandler::GetIstance().GetEntries();
  int Nped = 0;
  // count pedestal event
  cout << "counting ped event" << endl; 
  for(int iEv=0; iEv<Nev; iEv++) {
    if (((int)(((double)iEv/(double)Nev)*100))%10 == 0) cout << ((double)iEv/(double)Nev)*100 << "% \xd"; 
    DataFileHandler::GetIstance().GetEntry(iEv);
    if(DataFileHandler::GetIstance().IsPed()) Nped++;
  }
  int nbunch = Nped/bunch_length;
  if (Nped%bunch_length!=0) nbunch++;
  cout << "N bunch "<< nbunch << endl;

  // Create canvas and histos
  vector < vector < TH1D *> > HistoVect; // [iLadder][iBunch]
  vector < TCanvas *> CanvVect; // [iLadder] (divide ibunch)
  for (int iLadder=0; iLadder<DAMPENTDR; iLadder++) {
    stringstream cname, ctitle;
    cname << "cTDR" << DAMPETDRNUM[iLadder];
    ctitle << "Ped TDR " << DAMPETDRNUM[iLadder];
    CanvVect.push_back(new TCanvas(cname.str().c_str(),ctitle.str().c_str(),800,800 ));
    //CanvVect.back()->Divide(0,nbunch);
    vector < TH1D *> tmpvect;
    for (int iBunch=0; iBunch<nbunch; iBunch++) {
      stringstream hname, htitle;
      hname << "hTDR_" << DAMPETDRNUM[iLadder] << "_bunch_" << iBunch;
      htitle << "Ped TDR " << DAMPETDRNUM[iLadder];
      cout << "Creating "<< htitle.str() << endl;
      tmpvect.push_back(new TH1D(hname.str().c_str(),htitle.str().c_str(),1024,0,1024));
    }
    HistoVect.push_back(tmpvect);
  }

  // fill pedestal event
  Nped = 0;
  cout << endl << "filling histos" << endl;
  for(int iEv=0; iEv<Nev; iEv++) {
    if (((int)(((double)iEv/(double)Nev)*100))%10 == 0) cout << ((double)iEv/(double)Nev)*100 << "% \xd"; 
    int iBunch = Nped/bunch_length;
    DataFileHandler::GetIstance().GetEntry(iEv);
    if(!DataFileHandler::GetIstance().IsPed()) continue;
    // fill raw dampe ladder
    for (int iLadder=0; iLadder<DAMPENTDR; iLadder++) {
      //for (int iTDR=0; iTDR<24; iTDR++) {
      //cout << iTDR << " " << (DataFileHandler::GetIstance().fTDRStatus[iTDR] & 0x1f) << " " << iLadder << endl;
      //if ( (DataFileHandler::GetIstance().fTDRStatus[iTDR] & 0x1f) == iLadder) {
      //  tdrposnum = iTDR;
      //}
      //}
      //cout << "Event " << iEv <<  " Ladder " << iLadder  << " Bunch " << iBunch << endl;
      for (int iStrip=0; iStrip<1024; iStrip++) {
	//cout << "Ladder " << iLadder << " Strip: " << iStrip << " Signal: " << DataFileHandler::GetIstance().fAmsev-> GetRawSignal(rh,iLadder,iStrip) << endl;
	HistoVect[iLadder][iBunch]->Fill(iStrip,DataFileHandler::GetIstance().fAmsev->GetRawSignal(rh,DAMPETDRNUM[iLadder],iStrip));
      }
    }
    Nped++;
  } 

  for (int iLadder=0; iLadder<DAMPENTDR; iLadder++) {
    CanvVect[iLadder]->cd();
    TLegend *leg = new TLegend(0.7,0.1,0.9,0.7);
    for (int iBunch=0; iBunch<(nbunch-1); iBunch++) {
      HistoVect[iLadder][iBunch]->SetLineColor(iBunch+1);
      stringstream legname;
      legname << "Time bunch " << iBunch;
      leg->AddEntry(HistoVect[iLadder][iBunch],legname.str().c_str(),"l");
      if (iBunch==(nbunch-1)) HistoVect[iLadder][iBunch]->Scale((double)1/((double)(Nped-(bunch_length*iBunch))));
      else HistoVect[iLadder][iBunch]->Scale((double)1/((double)bunch_length));
      if (iBunch==0) HistoVect[iLadder][iBunch]->Draw();
      else HistoVect[iLadder][iBunch]->Draw("same");
      gPad->Update();
    }
    leg->Draw("same");
  }
  gPad->WaitPrimitive();

}
