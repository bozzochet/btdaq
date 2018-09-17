#ifndef DATAFILEHANDLER_H_
#define DATAFILEHANDLER_H_

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sstream>

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

#include "constants.h"
#include "functions.h"
#include "structures.h"

/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

/* from the CommonTool dir */
#include "TrackSelection.hh"
#include "Utilities.hh"
/* end */

using namespace std;

class DataFileHandler{

public:
	static DataFileHandler &GetIstance();

	bool ReadCCFile(string inputCCFileName, bool adc_mip=false);
	bool ReadAMSFile(string inputAMSFileName);
	void GetEntry(int iEv);
	bool Selection();
	int GetEntries();
	int GetAMSEntries();
	void GetAMSEntry(int iEv);
	int GetCCEntries();
	void GetCCEntry(int iEv);
	bool IsCCPresent();
	bool IsAMSPresent();
	bool IsPed();
	bool IsGood();
	void Reset();

	// CC var.
	// Read calocube variables
	EventInfo fEventInfo;
	Int_t fCasisTime;
	Float_t fTCorr;
	// large diode
	Int_t fNHits_l;
	Float_t fHitSignal_l[NCASISCHIPS*NCASISCHANNELS];
	Int_t   fHitRow_l[NCASISCHIPS*NCASISCHANNELS];
	Int_t   fHitCol_l[NCASISCHIPS*NCASISCHANNELS];
	Int_t   fHitLayer_l[NCASISCHIPS*NCASISCHANNELS];
	// small diode
	Int_t   fNHits_s;
	Float_t fHitSignal_s[NCASISCHIPS*NCASISCHANNELS];
	Int_t   fHitRow_s[NCASISCHIPS*NCASISCHANNELS];
	Int_t   fHitCol_s[NCASISCHIPS*NCASISCHANNELS];
	Int_t   fHitLayer_s[NCASISCHIPS*NCASISCHANNELS];

	// AMS var
	Event *fAmsev;
	//int fTDRStatus[24];
	//TBranch        *b_cluster_branch_TDRStatus;

	~DataFileHandler();

private:

	DataFileHandler();
	DataFileHandler(const DataFileHandler&);
	TTree *fTCC;
	TFile *fCCFile;

public:
	TChain *fTAMS;

private:
	int fNev;
};

#endif
