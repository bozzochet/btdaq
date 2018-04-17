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
double smallPD2MIP[SMALLPDNUM];

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
double SmallBigRatio[SMALLPDNUM]={128.4,130.8,122.0};
bool RatioCalc = false;
const char *inputFileName = "";
bool save_histo = false;
int layerbegin = 0;
int layerend = 14;
int InitLayerMax=14;
float MIPcut=0;
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
	usage << "-R\t\t\t Ratio from small and big PD is calculated by the software, default use hard-coded numbers" << endl;
	usage << "-h\t\t\thelp" << endl << endl;

	int c;

	while ((c = getopt(argc, argv, "i:a:z:L:C:Rh")) != -1){
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
		case 'R':
			RatioCalc = true;
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

/*
par[0] -> Normalization factor
par[1] -> Mean
par[2] -> Sigma
par[3] -> Asymmetry
*/
Double_t NorGausFunc (Double_t *px, Double_t *par) {
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
	if (MIPcut==0 || muon==0) return 0;
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
	gStyle->SetOptFit();

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

	gStyle->SetStatFormat("6.3f");

	// hard-coded ratio for small PD
	SmallBigRatio[0]=120;
	SmallBigRatio[0]=120;
	SmallBigRatio[0]=120;

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

	int nEvent = tree->GetEntries();

	if (RatioCalc) {
		//-------------------------------------------------------
		// Small PD calibration
		//-------------------------------------------------------

		for(int Ismall=0; Ismall<SMALLPDNUM; Ismall++) smallPD2MIP[Ismall] = 0;

		TH1D *hSBratio[3];
		hSBratio[0] = new TH1D("Small, big ratio, layers 2", "Small, big ratio, layers 2", 100, 90, 170);
		hSBratio[1] = new TH1D("Small, big ratio, layers 3", "Small, big ratio, layers 3", 100, 90, 170);
		hSBratio[2] = new TH1D("Small, big ratio, layers 4", "Small, big ratio, layers 4", 100, 90, 170);

		for(int iev=0; iev<nEvent; iev++){

			tree->GetEntry(iev);
			//==================================
			// selection
			//==================================

			if(eventInfo.trigger!=PHYSICSTRIGGER)continue;
			if(eventInfo.good!=GOODEVENTFLAG)continue;
			if(nHits==0)continue;

			std::vector <double> smallPD(SMALLPDNUM,0);
			std::vector <double> bigPD(SMALLPDNUM,0);
			std::vector <double> bigPDMIP(SMALLPDNUM,0);

			for(int ihits=0; ihits<nHits; ihits++){
				bool big_flag = hitRow[ihits] ==1 && hitCol[ihits]==1 && (hitLayer[ihits]==2 || hitLayer[ihits]==3 || hitLayer[ihits]==4);
				if (big_flag) {
					bigPD[hitLayer[ihits]-2]=hitSignal[ihits];
					bigPDMIP[hitLayer[ihits]-2]=hitSignal_MIP2H[ihits];
					//cout << "big " << bigPD[hitLayer[ihits]-2] << endl;
				}
				bool small_flag = hitRow[ihits] ==1 && hitCol[ihits]==1 && (hitLayer[ihits]==15 || hitLayer[ihits]==16 || hitLayer[ihits]==17);
				if (small_flag) {
					smallPD[hitLayer[ihits]-15]=hitSignal[ihits];
					//cout << "samll " << smallPD[hitLayer[ihits]-15] << endl;
				}
			}

			for (unsigned int Ismall=0; Ismall<SMALLPDNUM; Ismall++){
				if (smallPD[Ismall] && bigPD[Ismall]) {
					//cout << bigPD[Ismall]/smallPD[Ismall] << endl;
					hSBratio[Ismall]->Fill(bigPD[Ismall]/smallPD[Ismall]);
					if (smallPD2MIP[Ismall]==0) {
						smallPD2MIP[Ismall] = bigPDMIP[Ismall]/bigPD[Ismall];
						cout << "small PD to MIP " << smallPD2MIP[Ismall] << endl;
					}
				}
			}
		}

		cc = new TCanvas("Small, big ratio","Small, big ratio", x,y,w,h);
		cc->Divide(SMALLPDNUM);
		for (unsigned int Ismall=0; Ismall<SMALLPDNUM; Ismall++){
			cc->cd(Ismall+1);
			hSBratio[Ismall]->Draw();
			cout << Ismall << " small/big signals mean " << hSBratio[Ismall]->GetMean() << " and RMS " << hSBratio[Ismall]->GetRMS() << endl;
			smallPDcalib[Ismall] = hSBratio[Ismall]->GetMean();
		}
		cc->Update();
		cc->WaitPrimitive();
		cc->Close();

	}
	else {
		for(int Ismall=0; Ismall<SMALLPDNUM; Ismall++) smallPDcalib[Ismall] = SmallBigRatio[Ismall];

		for(int iev=0; iev<nEvent; iev++){

			tree->GetEntry(iev);
			//==================================
			// selection
			//==================================

			if(eventInfo.trigger!=PHYSICSTRIGGER)continue;
			if(eventInfo.good!=GOODEVENTFLAG)continue;
			if(nHits==0)continue;

			std::vector <double> bigPD(SMALLPDNUM,0);
			std::vector <double> bigPDMIP(SMALLPDNUM,0);

			for(int ihits=0; ihits<nHits; ihits++){
				bool big_flag = hitRow[ihits] ==1 && hitCol[ihits]==1 && (hitLayer[ihits]==2 || hitLayer[ihits]==3 || hitLayer[ihits]==4);
				if (big_flag) {
					bigPD[hitLayer[ihits]-2]=hitSignal[ihits];
					bigPDMIP[hitLayer[ihits]-2]=hitSignal_MIP2H[ihits];
				}
			}

			for (unsigned int Ismall=0; Ismall<SMALLPDNUM; Ismall++){
				if (bigPD[Ismall]>1000.) {
					if (smallPD2MIP[Ismall]==0) {
						smallPD2MIP[Ismall] = bigPDMIP[Ismall]/bigPD[Ismall];
						cout << "small PD to MIP " << smallPD2MIP[Ismall] << endl;
					}
				}
			}
		}
	}
	//========================================================
	// create total histos
	//========================================================

	TH1F *cubeTot = new TH1F("total_signal", "total signal  (MIP=20MeV); MIP; Events", 100, minrangedata, maxrangedata);
		cubeTot->GetYaxis()->SetTitleOffset(1.4);
	TH1F *cubeTotSmall = new TH1F("total_signal_small", "total signal with 3 small PD  (MIP=20MeV); MIP; Events", 100, minrangedata, maxrangedata);
		cubeTotSmall->GetYaxis()->SetTitleOffset(1.4);
	TH1F *hLongShower = new TH1F("Longitudinal_profile", "Longitudinal hadronic shower profile  (MIP=20MeV, 1Layers=2X0); Layers; MIPs", 17, -1.5,15.5);
		hLongShower->GetYaxis()->SetTitleOffset(1.4);
	TH1F *hLongShowerSmall = new TH1F("Longitudinal_profile_small", "Longitudinal hadronic shower profile with small PD (MIP=20MeV, 1Layers=2X0); Layers; MIPs", 17, -1.5,15.5);
		hLongShowerSmall->GetYaxis()->SetTitleOffset(1.4);

	////////////////////////////////////////////////////////
	// start loop over events
	////////////////////////////////////////////////////////

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
		Double_t totalSmall=0;
		++nEventSel;

		for(int ihits=0; ihits<nHits; ihits++){
			//hitCube = (3*hitRow[ihits]) + hitCol[ihits];
			if(muon) {
				bool big_flag = hitRow[ihits] ==1 && hitCol[ihits]==1 && (hitLayer[ihits]==2 || hitLayer[ihits]==3 || hitLayer[ihits]==4);
				bool small_flag = hitRow[ihits] ==1 && hitCol[ihits]==1 && (hitLayer[ihits]==15 || hitLayer[ihits]==16 || hitLayer[ihits]==17);
				if (!big_flag && !small_flag){
					total += hitSignal_MIP2H[ihits];
					hLongShower->Fill(hitLayer[ihits],hitSignal_MIP2H[ihits]);
					totalSmall += hitSignal_MIP2H[ihits];
					hLongShowerSmall->Fill(hitLayer[ihits],hitSignal_MIP2H[ihits]);
				}
				if (big_flag){
					total += hitSignal_MIP2H[ihits];
					hLongShower->Fill(hitLayer[ihits],hitSignal_MIP2H[ihits]);
				}
				if (small_flag){
					double smallSignal = hitSignal[ihits]*smallPDcalib[hitLayer[ihits]-15]*smallPD2MIP[hitLayer[ihits]-15];
					//cout << "small signal " << smallSignal << endl;
					totalSmall += smallSignal;
					hLongShowerSmall->Fill(hitLayer[ihits]-13,smallSignal);
				}
			} else {
				bool big_flag = hitRow[ihits] ==1 && hitCol[ihits]==1 && (hitLayer[ihits]==2 || hitLayer[ihits]==3 || hitLayer[ihits]==4);
				bool small_flag = hitRow[ihits] ==1 && hitCol[ihits]==1 && (hitLayer[ihits]==15 || hitLayer[ihits]==16 || hitLayer[ihits]==17);
				if (!big_flag && !small_flag){
					total += hitSignal[ihits];
					hLongShower->Fill(hitLayer[ihits],hitSignal[ihits]);
					totalSmall += hitSignal[ihits];
					hLongShowerSmall->Fill(hitLayer[ihits],hitSignal[ihits]);
				}
				if (big_flag){
					total += hitSignal[ihits];
					hLongShower->Fill(hitLayer[ihits],hitSignal[ihits]);
				}
				if (small_flag){
					double smallSignal = hitSignal[ihits]*smallPDcalib[hitLayer[ihits]-15];
					//cout << "small signal " << smallSignal << endl;
					totalSmall += smallSignal;
					hLongShowerSmall->Fill(hitLayer[ihits]-13,smallSignal);
				}
			}
		}
		cubeTot->Fill(total);
		cubeTotSmall->Fill(totalSmall);
	}
	hLongShower->Scale(1./nEventSel);
	hLongShowerSmall->Scale(1./nEventSel);

	if(muon) {
		cc = new TCanvas("totalsignalMIP","totalsignalMIP", x,y,w,h);
	} else {
		cc = new TCanvas("totalsignal","totalsignal", x,y,w,h);
	}
	cc->Divide(5);
	cc->cd(1);
	gPad->SetPad(0., 0., 0.49, 0.49);
	cc->Update();
	cubeTot->Draw();
	TF1 *Func = new TF1("NorGausFunc",NorGausFunc, minrangedata, maxrangedata, 4);
	Func->SetParameter(0,cubeTot->Integral());
	Func->SetParameter(1,cubeTot->GetMean());
	Func->SetParameter(2,cubeTot->GetRMS());
	Func->SetParameter(3,0.3);
	cubeTot->Fit(Func,"R");
	double Mean  = Func->GetParameter(1);
	double Sigma = Func->GetParameter(2);
	TPaveText *ptCubeTot = new TPaveText(.7,.2,.95,.4,"NDC");
	string cubeTotText = Form("Resolution = %6.2f", (Sigma/Mean)*100);
	ptCubeTot->AddText(cubeTotText.data());
	ptCubeTot->Draw();
	cc->Update();
	cc->cd(2);
	gPad->SetPad(0., 0.50, 0.49, 1.);
	cc->Update();
	cubeTotSmall->Draw();
	TF1 *FuncSmall = new TF1("NorGausFuncSmall",NorGausFunc, minrangedata, maxrangedata, 4);
	FuncSmall->SetParameter(0,cubeTotSmall->Integral());
	FuncSmall->SetParameter(1,cubeTotSmall->GetMean());
	FuncSmall->SetParameter(2,cubeTotSmall->GetRMS());
	FuncSmall->SetParameter(3,0.3);
	cubeTotSmall->Fit(FuncSmall,"R");
	Mean  = FuncSmall->GetParameter(1);
	Sigma = FuncSmall->GetParameter(2);
	TPaveText *ptCubeTotSmall = new TPaveText(.7,.2,.95,.4,"NDC");
	string cubeTotSmallText = Form("Resolution = %6.2f", (Sigma/Mean)*100);
	ptCubeTotSmall->AddText(cubeTotSmallText.data());
	ptCubeTotSmall->Draw();
	cc->Update();
	cc->cd(3);
	gPad->SetPad(0.5, 0.1, 1, 0.55);
	hLongShower->Draw();
	cc->cd(4);
	gPad->SetPad(0.5, 0.56, 1, 1);
	hLongShowerSmall->Draw();
	cc->cd(5);
	gPad->SetPad(0.6, 0, 0.9, 0.1 );
	cc->Update();
	TPaveText *pt = new TPaveText(.05,.1,.95,.95);
	pt->AddText("Double click to close!");
	pt->Draw();
	cc->Update();

	gPad->WaitPrimitive();
	cc->Close();

	fCC->Close();
	return 0;
}



