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

bool adc_mip=true; // true -> use ADC, false -> use MIP (small diodes are not well calibrated so far)
//int COLOR[] = {kRed,kBlue,kGreen,kYellow};

void HandleInputPar(int argc, char **argv)
{
  stringstream usage;
  usage.clear();
  usage << endl;
  usage << "Usage: EventCounter [inputFileList]" << endl << endl;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv){

  HandleInputPar(argc, argv);
  
  int bho=1;
  TApplication theApp("App",&bho,argv);

  // Read input fil list
  string line;
  vector <string> fileNameVect;
  vector <string> fileNameLocalTVect;
  ifstream myfile (argv[1]);
  if (myfile.is_open()) {
    cout << "Reading file: " << argv[1] << endl; 
    while ( getline (myfile,line) ) {
      fileNameVect.push_back(line);
      cout << line << '\n';
    }
    myfile.close();
  } else {
    cerr << "Input file list not open, exit!" << endl;
    exit(1);
  }
  gROOT->Reset();

  // scan all file in list
  vector < int > NtotVect, NpedVect, NphysVect;
  vector < bool > errorVect;
  for (int iFile=0; iFile<fileNameVect.size(); iFile++) {
    // create name of file
    string inputCCFileName = "/home/calocube/TIC_TB2018/May2018_TIC/Software/CaloKagathos/analysed_files/CC";
    string inputAMSFileName = "/home/calocube/TIC_TB2018/May2018_TIC/Software/Perugia/Decode/RootData/run_";   
    time_t utime = atoi(fileNameVect[iFile].c_str());
    struct tm * tmstruct = localtime ( & utime);
    char loctime[256];
    strftime(loctime,256,"%Y%m%d_%H%M%S",tmstruct);
    fileNameLocalTVect.push_back(loctime);
    inputCCFileName = inputCCFileName + loctime + ".root";
    inputAMSFileName = inputAMSFileName + fileNameVect[iFile] + ".root";
    cout << "Open: " << inputCCFileName << " and " << inputAMSFileName << endl;
    // open files and trees
    DataFileHandler::GetIstance().ReadCCFile(inputCCFileName,adc_mip);
    DataFileHandler::GetIstance().ReadAMSFile(inputAMSFileName);  
    int NevTot =  DataFileHandler::GetIstance().GetEntries();
    int NevCC = DataFileHandler::GetIstance().GetCCEntries();
    int NevAMS = DataFileHandler::GetIstance().GetAMSEntries();
    if (NevCC != NevAMS) {
      cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ERROR: N. AMS != N. CALOCUBE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
      errorVect.push_back(true);
    } else {
      errorVect.push_back(false);
    }
    int Nped = 0;
    int Nphys = 0;
    cout << "counting ped event" << endl; 
    for(int iEv=0; iEv<NevTot; iEv++) {
      if (((int)(((double)iEv/(double)NevTot)*100))%10 == 0) cout << ((double)iEv/(double)NevTot)*100 << "% \xd"; 
      DataFileHandler::GetIstance().GetEntry(iEv);
      if(DataFileHandler::GetIstance().IsPed()) Nped++;
      else Nphys++;
    }
    cout << "file: " << fileNameLocalTVect[iFile] << " Ntot: " << NevTot << " Nphys " << Nphys << " Nped " << Nped << endl;
    NtotVect.push_back(NevTot);
    NpedVect.push_back(Nped);
    NphysVect.push_back(Nphys);
    DataFileHandler::GetIstance().Reset();
  }

  // print summary table
  cout << endl << endl << endl << "-------------- Print summary ----------------" << endl;
  int NRunTot=0;
  int NRunPed=0;
  int NRunPhys=0;  
  int NRunTotGOOD=0;
  int NRunPedGOOD=0;
  int NRunPhysGOOD=0;
  int NRunError=0;
  for (int iFile=0; iFile<fileNameVect.size(); iFile++) {
    cout << "File " << fileNameLocalTVect[iFile] << " N. tot " << NtotVect[iFile] << " N. phys " << NphysVect[iFile] << " N. ped " << NpedVect[iFile];
    if (errorVect[iFile]) cout << " BAD!! ";
    cout << endl;
    NRunTot+= NtotVect[iFile];
    NRunPed+= NpedVect[iFile];
    NRunPhys+= NphysVect[iFile];
    if (errorVect[iFile]) NRunError++;
    else {
      NRunTotGOOD+= NtotVect[iFile];
      NRunPedGOOD+= NpedVect[iFile];
      NRunPhysGOOD+= NphysVect[iFile];
    }
  }
  cout << endl << endl << endl << "-------------- Print total number ----------------" << endl;
  cout << "----Total---- N. tot " << NRunTot << " N. phys " << NRunPhys << " N. ped " << NRunPed << " N. error " << NRunError << endl;
  cout << "----Total GOOD---- N. tot " << NRunTotGOOD << " N. phys " << NRunPhysGOOD << " N. ped " << NRunPedGOOD << endl;
  theApp.Terminate();

}
