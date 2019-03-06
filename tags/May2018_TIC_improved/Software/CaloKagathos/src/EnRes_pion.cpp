//Created by Lorenzo Pacini

#include "TROOT.h"
#include "TVirtualX.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TLegend.h"
#include "TPolyLine.h"
#include "TGaxis.h"
#include "TApplication.h"
#include "TMath.h"
#include "TFile.h"
#include "TString.h"
#include "TPaveText.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

const int NCRYSTALS = 9; 
//const int NLAYERS = 14; 
const int NLAYERS = 15; 

const int NCASISCHIPS = 9; // Number of CASIC chips, macro defined in IOfile.h
const int NCASISCHANNELS = 16; // Number of channels for each CASIS chip, macro defined in IOfile.h
const float SIZE = 3.9;

const int NCOL = 3;
const int NROW = 3;
const int NLAY = 14;
const int NINSTRUMCASISCHANNELS = 15; // Number of instrumented channels for each CASIS chip

const int SMALLPDNUM=3;

// small PD calibration
double smallPDcalib[SMALLPDNUM];

//-------------------------------------------------------------------------------------------------------
// **************      		     Data Format Changed		          ***********************
//-------------------------------------------------------------------------------------------------------

// Trigger types
const int RANDOMTRIGGER = 0x40;
const int PHYSICSTRIGGER = 0x20;

// CALOCUBES good event flag
const int GOODEVENTFLAG = 0x02;
const int BADEVENTFLAG = 0x04;

//-------------------------------------------------------------------------------------------------------
// **************      		     Cube and Casis Map Changed		          ***********************
//-------------------------------------------------------------------------------------------------------

bool InstrumentedChannel(int casis, int casisChannel) {
   // disconnected
  if (casis > 2) {
  	if (casisChannel == NINSTRUMCASISCHANNELS)
    	return true;
  }
  return true;
}

bool GoodChannelForCN(int casis, int casisChannel) {
  // disconnected
  if (casis > 2) {
  	if (casisChannel == NINSTRUMCASISCHANNELS)
    	return true;
  }
  return false;
}

// Map of channels. Returns false if the channel is bad
bool GoodChannel(int casis, int casisChannel) {
	if( GoodChannelForCN(casis,casisChannel) )return false;
	return true;
}

// Associate chip to cube
// Returns true if the channel is associated to a cube
// WARNING: the map is outdated, must be updated with the actual channel-cube map

bool Chip2Cube(int casis, int casisChannel, int &row, int &col, int &layer) {

	if ( !InstrumentedChannel(casis, casisChannel) )return false;
  	static const int layerMap[NCASISCHIPS][NCASISCHANNELS] = {
    		{ 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 14+3},  // CASIS 0
    		{ 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 14+2},  // CASIS 1
    		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 14+1},  // CASIS 2
    		{ 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9,-1},  // CASIS 3
    		{ 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8,-1},  // CASIS 4
    		{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6,-1},  // CASIS 5
    		{13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,-1},  // CASIS 6
    		{11,11,11,12,12,12,12,12,12,12,12,12,13,13,13,-1},  // CASIS 7
    		{10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,-1}   // CASIS 8
  	};

  	static const int cubeMap[NCASISCHIPS][NCASISCHANNELS] = {
    		{3,4,5,6,7,8,6,7,8,3,4,5,0,1,2,4},  // CASIS 0
    		{6,7,8,0,1,2,3,4,5,6,7,8,0,1,2,4},  // CASIS 1
    		{0,1,2,3,4,5,6,7,8,0,1,2,3,4,5,4},  // CASIS 2
    		{3,4,5,6,7,8,6,7,8,3,4,5,0,1,2,-1},  // CASIS 3
    		{6,7,8,0,1,2,3,4,5,6,7,8,0,1,2,-1},  // CASIS 4
    		{0,1,2,3,4,5,6,7,8,0,1,2,3,4,5,-1},  // CASIS 5
    		{3,4,5,6,7,8,6,7,8,3,4,5,0,1,2,-1},  // CASIS 6
    		{6,7,8,0,1,2,3,4,5,6,7,8,0,1,2,-1},  // CASIS 7
    		{0,1,2,3,4,5,6,7,8,0,1,2,3,4,5,-1}   // CASIS 8
  	};

	if(cubeMap[casis][casisChannel]<0||layerMap[casis][casisChannel]<0)return false;

	row = (int)(cubeMap[casis][casisChannel] / 3);
	col = (cubeMap[casis][casisChannel] % 3);
	layer = layerMap[casis][casisChannel];
	return true;

}

struct EventInfo {
	Int_t trigger;
	Int_t good;
	void Reset() {
		trigger = 0;
		good = 0;
	}
};

bool muon = true;
const char *inputFileName = "";
bool save_histo = false;
int layerbegin = 0;
int layerend = 14;
int InitLayerMax=5;
float MIPcut=0.7;
double minrangedata=0.;
double maxrangedata=10000.;
const char *dir="./";

void HandleInputPar(int argc, char **argv)
{
	stringstream usage;
	usage.clear();
	usage << endl;
	usage << "Usage:" << endl << endl;
	usage << "CALOCUBE OPTIONS" << endl;
	usage << "-i\t\t\tname of the input file" << endl;
	usage << "-a\t\t\tset minrangedata range for data" << endl;
	usage << "-z\t\t\tset maxrangedata range for data" << endl;
	usage << "-L\t\t\t Max layer forhadronic shower develop (default 5)" << endl;
	usage << "-C\t\t\t Threshold in MIP for single cube signal, used for hShower identification (default 0.7MIP)" << endl;
	usage << "-h\t\t\thelp" << endl << endl;

	int c;

	while ((c = getopt(argc, argv, "i:a:z:L:C:h")) != -1){
		switch (c) {
		case 'i':
			inputFileName = optarg;
			break;
		case 'a':
			minrangedata=atof(optarg);
			break;
		case 'z':
			maxrangedata=atof(optarg);
			break;
		case 'L':
			InitLayerMax=atoi(optarg);
			if (InitLayerMax >= 0 && InitLayerMax < NLAYERS) {}
			else {
				cout << "Init layer for hshower is <0 or >14, exit!!";
				exit(0);
			}
			break;
		case 'C':
			MIPcut=atof(optarg);
			if (MIPcut>=0) {}
			else {
				cout << "MIPcut <0, exit!!" << endl;
				exit(0);
			}
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

TFile *fCC;
TCanvas *cc;

void exit_safe(int ret_val) {

	cc->Close();

	if(fCC->IsOpen())
		fCC->Close();

	exit(ret_val);
}

void sighandler(int code){
	cout << endl;
	cout << "Catched signal " << code << endl;
	exit_safe(EXIT_SUCCESS);
}

Double_t AlessiaFunc (Double_t *px, Double_t *par) {
	Double_t y;
	Double_t x= px[0];
	Double_t s = (2./2.35)*asinh(par[3]*2.35/2);
	if (x<par[1]+(par[2]/par[3]))
		y = par[0]*(par[3]/( sqrt(2*TMath::Pi())*par[2]*s ) ) * exp(- (pow( log( 1 - (par[3]*(x-par[1])/par[2]) ) ,2) )/(2*s*s) - s*s/2 );
	else
		y=0;
	return y;
}

int hShowreInit (Int_t hitLayer[], Int_t nHits, Float_t hitSignal_MIP2H[]) {

	Int_t NhitLayer[NLAYERS];
	for (int iLayer=0; iLayer < NLAYERS; iLayer++) NhitLayer[iLayer]=0;
	if (MIPcut==0) return 0;
	for(int ihits=0; ihits<nHits; ihits++){
		if( (hitLayer[ihits]>=0) && (hitLayer[ihits]<NLAYERS) && (hitSignal_MIP2H[ihits]>MIPcut)) {
			//cout << "MIP " << hitSignal_MIP2H[ihits] << " layer " << hitLayer[ihits] << endl;
			NhitLayer[hitLayer[ihits]]++;
		}
	}
	for (int iLayer=0; iLayer < NLAYERS; iLayer++) {
		if (NhitLayer[iLayer] == 9) return iLayer;
	}
	return 999;
}


int main(int argc, char** argv){

	signal(SIGINT, sighandler);
	signal(SIGQUIT, sighandler);
	signal(SIGTERM, sighandler);
	signal(SIGUSR1, sighandler);
	signal(SIGUSR2, sighandler);

	HandleInputPar(argc, argv);

	if(access(inputFileName, F_OK ) == -1)
	{
		cout << "No existing input file: Exit..." << endl;
		exit(EXIT_FAILURE);
	}

	int tmp=1;
	TApplication theApp("App",&tmp,argv); //TApplication

	fCC = new TFile(inputFileName);

	Int_t  x, y;
	UInt_t w, h;
	gVirtualX->GetWindowSize(gVirtualX->GetDefaultRootWindow(),x,y,w,h);

	gStyle->SetOptStat("neMuRo");
	gStyle->SetStatFormat("6.3f");

	//cc = new TCanvas("dummy","dummy", x,y,w,h);

	if(layerbegin>layerend){
		cout << "Layer begin > layer end: Exit...";
		exit_safe(EXIT_FAILURE);
	}
	TTree *tree = (TTree *)fCC->Get("data");

	if(!tree) {
		cout << "No TTree found: Exit... " << endl;
		exit_safe(EXIT_FAILURE);
	}

	Int_t nHits;
	Float_t hitSignal[NCASISCHIPS*NCASISCHANNELS];
	Float_t hitSignal_MIP2H[NCASISCHIPS*NCASISCHANNELS];
	Float_t hitSignal_MIP4He[NCASISCHIPS*NCASISCHANNELS];
	Float_t hitSignal_MIPCR[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitRow[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitCol[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitLayer[NCASISCHIPS*NCASISCHANNELS];
	Float_t   xHit[NCASISCHIPS*NCASISCHANNELS];
	Float_t   yHit[NCASISCHIPS*NCASISCHANNELS];
	Float_t   zHit[NCASISCHIPS*NCASISCHANNELS];
	Float_t signal[NCASISCHIPS][NCASISCHANNELS];
	Float_t signal_cn[NCASISCHIPS][NCASISCHANNELS];
	Float_t signal_ped[NCASISCHIPS][NCASISCHANNELS];
	tree->SetBranchAddress("nHits",&nHits);
	tree->SetBranchAddress("hitSignal",&hitSignal);
	tree->SetBranchAddress("hitSignal_MIP2H",&hitSignal_MIP2H);
	tree->SetBranchAddress("hitSignal_MIP4He",&hitSignal_MIP4He);
	tree->SetBranchAddress("hitSignal_MIPCR",&hitSignal_MIPCR);
	tree->SetBranchAddress("hitRow",&hitRow);
	tree->SetBranchAddress("hitCol",&hitCol);
	tree->SetBranchAddress("hitLayer",&hitLayer);
	tree->SetBranchAddress("xHit",&xHit);
	tree->SetBranchAddress("yHit",&yHit);
	tree->SetBranchAddress("zHit",&zHit);
	tree->SetBranchAddress("signalBranch", &signal);
	tree->SetBranchAddress("signalCNBranch", &signal_cn);
	tree->SetBranchAddress("signalNoPedSubBranch", &signal_ped);
	EventInfo eventInfo;
	tree->SetBranchAddress("eventInfo", &(eventInfo.trigger));
	tree->SetBranchAddress("eventInfoGood", &(eventInfo.good));

	//========================================================
	// create total histos
	//========================================================

	TH1F *cubeTot = new TH1F("total_signal", "total signal  (MIP=20MeV); MIP; Events", 100, minrangedata, maxrangedata);
	cubeTot->GetYaxis()->SetTitleOffset(1.4);
	TH1F *hLongShower = new TH1F("Longitudinal_profile", "Longitudinal hadronic shower profile  (MIP=20MeV, 1Layers=0.082Lambda); Layers; MIPs", 17, -1.5,15.5);
	hLongShower->GetYaxis()->SetTitleOffset(1.4);
	////////////////////////////////////////////////////////

	// start loop over events
	////////////////////////////////////////////////////////
	int nEvent = tree->GetEntries();
	int nEventSel = 0;

	for(int iev=0; iev<nEvent; iev++){

		tree->GetEntry(iev);
		//==================================
		// selection
		//==================================
		if(eventInfo.trigger!=PHYSICSTRIGGER)continue;
		if(eventInfo.good!=GOODEVENTFLAG)continue;

		int InitLayer = 0;
		InitLayer = hShowreInit(hitLayer, nHits, hitSignal_MIP2H);
		if (InitLayer>InitLayerMax) {
			continue;
		}
		//else cout << "h Shower Init layer " << InitLayer << endl;
		Double_t total = 0;
		++nEventSel;

		for(int ihits=0; ihits<nHits; ihits++){
			//hitCube = (3*hitRow[ihits]) + hitCol[ihits];
			if ((hitLayer[ihits]>=0) && (hitLayer[ihits]<NLAYERS)) {
				if(muon) {
					total += hitSignal_MIP2H[ihits];
					hLongShower->Fill(hitLayer[ihits],hitSignal_MIP2H[ihits]);
				} else {
					total += hitSignal[ihits];
					hLongShower->Fill(hitLayer[ihits],hitSignal[ihits]);
				}
			}
		}
		cubeTot->Fill(total);
	}

	hLongShower->Scale(1./nEventSel);
	 

	if(muon) {
		cc = new TCanvas("totalsignalMIP","totalsignalMIP", x,y,w,h);
	} else {
		cc = new TCanvas("totalsignal","totalsignal", x,y,w,h);
	}
	cc->Divide(3);
	cc->cd(1);
	gPad->SetPad(0., 0., 0.49, 1);
	cubeTot->Draw();
	cc->Update();
	cc->cd(2);
	gPad->SetPad(0.5, 0.1, 1, 1 );
	hLongShower->Draw();
	cc->cd(3);
	gPad->SetPad(0.6, 0, 0.9, 0.1 );
	cc->Update();
	TPaveText *pt = new TPaveText(.05,.1,.95,.95);
	pt->AddText("Click x to close!");
	pt->Draw();
	cc->Update();
	/*
	cc->cd(1);
	TF1 *Func = new TF1("AlessiaFunc",AlessiaFunc, 2000000, 4000000, 4);
	Func->SetParameter(0,110000000);
	Func->SetParameter(1,3100000);
	Func->SetParameter(2,180000);
	Func->SetParameter(3,0.3);
	cubeTot->Fit(Func,"R");

	cc->cd(2);
	TF1 *FuncSmall = new TF1("AlessiaFuncSmall",AlessiaFunc, 2000000, 4000000, 4);
	FuncSmall->SetParameter(0,110000000);
	FuncSmall->SetParameter(1,3100000);
	FuncSmall->SetParameter(2,180000);
	FuncSmall->SetParameter(3,0.4);
	cubeTotSmall->Fit(FuncSmall,"R");
*/
	gPad->WaitPrimitive();
	cc->Close();

	fCC->Close();
	return 0;
}



