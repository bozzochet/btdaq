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

#include "DataFileHandler.h"

using namespace std;

DataFileHandler &DataFileHandler::GetIstance() {
  static DataFileHandler instance;
  return instance;
}

DataFileHandler::DataFileHandler() {
	fTCC=NULL;
	fTAMS=NULL;
	fNev=-1;
	//fTDRStatus=NULL;
}

DataFileHandler::~DataFileHandler() {

}

bool DataFileHandler::ReadCCFile(string inputCCFileName, bool adc_mip) {

	if(access(inputCCFileName.c_str(), F_OK ) == -1)	{
		cerr << "No existing CC input file in DataFileHandler: Exit..." << endl;
		exit(EXIT_FAILURE);
	}

	TFile *inputFile = new TFile(inputCCFileName.c_str(), "READ");
	inputFile->GetObject("data",fTCC);
	if(!fTCC) {
		cout << "No existing CC tree: Exit..." << endl;
		exit(EXIT_FAILURE);
	}

	fTCC->SetBranchAddress("eventInfo", &(fEventInfo.trigger));
	fTCC->SetBranchAddress("eventFlag", &(fEventInfo.good));
	fTCC->SetBranchAddress("casisTime", &fCasisTime);
	fTCC->SetBranchAddress("tCorr", &fTCorr);

	if(adc_mip) {
		fTCC->SetBranchAddress("LADC.nHits",&fNHits_l);
		fTCC->SetBranchAddress("LADC.hitSignal",&fHitSignal_l);
		fTCC->SetBranchAddress("LADC.hitRow",&fHitRow_l);
		fTCC->SetBranchAddress("LADC.hitCol",&fHitCol_l);
		fTCC->SetBranchAddress("LADC.hitLayer",&fHitLayer_l);
	} else {
		fTCC->SetBranchAddress("LMIP.nHits",&fNHits_l);
		fTCC->SetBranchAddress("LMIP.hitSignal",&fHitSignal_l);
		fTCC->SetBranchAddress("LMIP.hitRow",&fHitRow_l);
		fTCC->SetBranchAddress("LMIP.hitCol",&fHitCol_l);
		fTCC->SetBranchAddress("LMIP.hitLayer",&fHitLayer_l);
	}

	if(adc_mip) {
		fTCC->SetBranchAddress("SADC.nHits",&fNHits_s);
		fTCC->SetBranchAddress("SADC.hitSignal",&fHitSignal_s);
		fTCC->SetBranchAddress("SADC.hitRow",&fHitRow_s);
		fTCC->SetBranchAddress("SADC.hitCol",&fHitCol_s);
		fTCC->SetBranchAddress("SADC.hitLayer",&fHitLayer_s);
	} else  {
		fTCC->SetBranchAddress("SMIP.nHits",&fNHits_s);
		fTCC->SetBranchAddress("SMIP.hitSignal",&fHitSignal_s);
		fTCC->SetBranchAddress("SMIP.hitRow",&fHitRow_s);
		fTCC->SetBranchAddress("SMIP.hitCol",&fHitCol_s);
		fTCC->SetBranchAddress("SMIP.hitLayer",&fHitLayer_s);
	}
  return true;
}

bool DataFileHandler::ReadAMSFile(string inputAMSFileName) {
	if (access(inputAMSFileName.c_str(), F_OK ) == -1) {
		cerr << "No existing AMS input file in DataFileHandler: Exit..." << endl;
		exit(EXIT_FAILURE);
	}

	//TFile *inputFile = new TFile(inputAMSFileName.c_str(), "READ");
	//inputFile->GetObject("t4",fTAMS);
	fTAMS = new TChain("t4");
	fTAMS->Add(inputAMSFileName.c_str());
	if(!fTAMS) {
		cout << "No existing AMS tree: Exit..." << endl;
		exit(EXIT_FAILURE);
	}

	// Read AMS variables
  printf("---------------------------------------------\n");
  TString align_filename = "alignment.dat";
  //TString gaincorrection_filename = "gaincorrection.dat";
  Event::ReadAlignment(align_filename.Data());
  //Event::ReadGainCorrection(gaincorrection_filename.Data());
  //  exit(1);
  printf("---------------------------------------------\n");

  fAmsev = new Event();
  fTAMS->SetBranchAddress("cluster_branch", &fAmsev);
  //fTAMS->SetBranchAddress("TDRStatus[24]",fTDRStatus,&b_cluster_branch_TDRStatus);
  fTAMS->GetEntry(0);
  return true;
}

int DataFileHandler::GetEntries() {
	if (fNev>0) // fNev already computed
		return fNev;

	if (fTCC!=NULL) {
		cout << "CC N.Events "<< fTCC->GetEntries() <<endl;
		fNev = fTCC->GetEntries();
	} else {
		cout << "AMS N.Events "<<fTAMS->GetEntries()<<endl;
		fNev = fTAMS->GetEntries();
	}
	return fNev;
}

int DataFileHandler::GetAMSEntries() {
	int amsNev=0;
	if (fTCC!=NULL) {
		cout << "AMS N.Events "<<fTAMS->GetEntries()<<endl;
		amsNev = fTAMS->GetEntries();
	} else {
		cerr << "In ataFileHandler::GetAMSEntries() no AMS tree found" << endl;
		return 0;
	}
	return amsNev;
}

int DataFileHandler::GetCCEntries() {
	int amsNev=0;
	if (fTCC!=NULL) {
		cout << "CC N.Events "<<fTCC->GetEntries()<<endl;
		amsNev = fTCC->GetEntries();
	} else {
		cerr << "In ataFileHandler::GetCCEntries() no CC tree found" << endl;
		return 0;
	}
	return amsNev;
}

void DataFileHandler::GetEntry(int iEv) {
  if (fTCC!=NULL) fTCC->GetEntry(iEv);
  if (fTAMS!=NULL) fTAMS->GetEntry(iEv);
}

void DataFileHandler::GetAMSEntry(int iEv) {
	if (fTAMS!=NULL) fTAMS->GetEntry(iEv);
	else {
		cerr << "In ataFileHandler::GetAMSEntry() no AMS tree found" << endl;
		exit(EXIT_FAILURE);
	}
}

void DataFileHandler::GetCCEntry(int iEv) {
	if (fTCC!=NULL) fTCC->GetEntry(iEv);
	else {
		cerr << "In ataFileHandler::GetCCEntry() no CC tree found" << endl;
		exit(EXIT_FAILURE);
	}
}

bool DataFileHandler::Selection() {
	// CC selection (phys trigger, good event, nHit>15)
	if (fTCC!=NULL) {
		bool GOOD = true;
		if(fEventInfo.trigger!=PHYSICSTRIGGER) GOOD = false;
		if(fEventInfo.good!=GOODEVENTFLAG) GOOD = false;
		if(fNHits_l<5) GOOD = false;
		if(!GOOD)return false;
	}
	// AMS event selection -> ncluster > 4
	if (fTAMS!=NULL) {
		if (fAmsev->GetNClusTot()<2) {
			return false;
		}
	}
	return true;
}

bool DataFileHandler::IsPed() {
  // CC selection (phys trigger, good event, nHit>15)
  if (fTCC!=NULL) {
    if(fEventInfo.trigger!=PHYSICSTRIGGER) return true;
  }
  return false;
}

bool DataFileHandler::IsGood() {
  // CC selection (phys trigger, good event, nHit>15)
  if (fTCC!=NULL) {
    if(fEventInfo.good==GOODEVENTFLAG) return true;
  }
  return false;
}

bool DataFileHandler::IsCCPresent() {
	if (fTCC==NULL) return false;
	return true;
}

bool DataFileHandler::IsAMSPresent() {
	if (fTAMS==NULL) return false;
	return true;
}

