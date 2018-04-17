#include <TROOT.h>
#include <TVirtualX.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TLegend.h>
#include <TPolyLine.h>
#include <TGaxis.h>
#include <TApplication.h>
#include <TMath.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphErrors.h>

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "constants.h"
#include "functions.h"
#include "structures.h"

#ifdef AMS
#include "Cluster.hh"
#include "Event.hh"
#include "TrackSelection.hh"
#include "Utilities.hh"
#endif

using namespace std;

//command line variables
const char *inputFileName = "";
bool save_histo = false;
bool save_global=false;
bool show_pedestal = false;
bool show_cn_pedestal = false;
bool view_pileup = false;
bool look_cn = false;
bool view_pedestal = false;
bool view_cn_pedestal = false;
bool view_data = false;
bool raw = false;
bool muon = false;
bool view_hidra = false;
bool view_nello = false;
bool view_cn_nello = false;
bool view_centroid = false;
bool small_large = false;
bool log_y = false;
int layerbegin = 0;
int layerend = NLAYERS;
int chipbegin = 0;
int chipend = NCASISCHIPS;
int nbindata=250;
double minrangedata=0.;
double maxrangedata=10000.;
int nbinpedestal=100;
double minrangepedestal=-300.;
double maxrangepedestal=300.;
bool show_tracker_pedestal=false;
bool show_tracker_hitmap=false;
int CC_flag=-1;
double CC_threshold_low=-1.;
double CC_threshold_high=-1.;
int CC_layer=-1;	
const char *dir="./";

//General variables
TFile *fCC;
TCanvas *cc;
TCanvas *cg;
#if defined (SPS2016) || defined (SPS2017Aug) || defined(SPS2017Nov)
const int ncols=NCOLS;
const int nrows=NCOLS;
#endif

//Tree variables
#if defined (SPS2016) || defined (SPS2017Aug) || defined(SPS2017Nov)
EventInfo eventInfo;
Int_t casisTime;
Float_t tCorr;
Short_t gain[NCASISCHIPS][NCASISCHANNELS]; // fain
Float_t Signal[NCASISCHIPS][NCASISCHANNELS]; // Signal matrix
Float_t Signal_cn[NCASISCHIPS][NCASISCHANNELS]; // Signal matrix
Hits LADC;
Hits SADC;
Hits LMIP;
Hits SMIP;
Shower LShower;
Shower SShower;
#else
EventInfo eventInfo;
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
Float_t Signal[NCASISCHIPS][NCASISCHANNELS];
Float_t Signal_cn[NCASISCHIPS][NCASISCHANNELS];
#endif

//Tree Construction
void ConstructTree(TTree *tree) {
#if defined (SPS2016) || defined (SPS2017Aug) || defined(SPS2017Nov)
	tree->SetBranchStatus("*",0); //disable all branches

	tree->SetBranchStatus("eventInfo", 1);
	tree->SetBranchStatus("eventFlag", 1);
	tree->SetBranchStatus("casisTime", 1);
	tree->SetBranchStatus("tCorr", 1);

	tree->SetBranchStatus("gainBranch", 1);
	tree->SetBranchStatus("signalBranch", 1);
	tree->SetBranchStatus("signalCNBranch", 1);

	tree->SetBranchStatus("LADC.nHits", 1);
	tree->SetBranchStatus("LADC.hitRow", 1);
	tree->SetBranchStatus("LADC.hitCol", 1);
	tree->SetBranchStatus("LADC.hitLayer", 1);
	tree->SetBranchStatus("LADC.hitGain", 1);
	tree->SetBranchStatus("LADC.hitSat", 1);
	tree->SetBranchStatus("LADC.hitSignal", 1);
	tree->SetBranchStatus("SADC.nHits", 1);

	tree->SetBranchStatus("SADC.hitRow", 1);
	tree->SetBranchStatus("SADC.hitCol", 1);
	tree->SetBranchStatus("SADC.hitLayer", 1);
	tree->SetBranchStatus("SADC.hitGain", 1);
	tree->SetBranchStatus("SADC.hitSat", 1);
	tree->SetBranchStatus("SADC.hitSignal", 1);

	tree->SetBranchStatus("LMIP.nHits", 1);
	tree->SetBranchStatus("LMIP.hitRow", 1);
	tree->SetBranchStatus("LMIP.hitCol", 1);
	tree->SetBranchStatus("LMIP.hitLayer", 1);
	tree->SetBranchStatus("LMIP.hitGain", 1);
	tree->SetBranchStatus("LMIP.hitSat", 1);
	tree->SetBranchStatus("LMIP.hitSignal", 1);

	tree->SetBranchStatus("SMIP.nHits", 1);
	tree->SetBranchStatus("SMIP.hitRow", 1);
	tree->SetBranchStatus("SMIP.hitCol", 1);
	tree->SetBranchStatus("SMIP.hitLayer", 1);
	tree->SetBranchStatus("SMIP.hitGain", 1);
	tree->SetBranchStatus("SMIP.hitSat", 1);
	tree->SetBranchStatus("SMIP.hitSignal", 1);

	tree->SetBranchAddress("eventInfo", &(eventInfo.trigger));
	tree->SetBranchAddress("eventFlag", &(eventInfo.good));
	tree->SetBranchAddress("casisTime", &casisTime);
	tree->SetBranchAddress("tCorr", &tCorr);

	tree->SetBranchAddress("gainBranch", &gain);
	tree->SetBranchAddress("signalBranch", &Signal);
	tree->SetBranchAddress("signalCNBranch", &Signal_cn);

	tree->SetBranchAddress("LADC.nHits",&LADC.nHits);
	tree->SetBranchAddress("LADC.hitRow",LADC.hitRow);
	tree->SetBranchAddress("LADC.hitCol",LADC.hitCol);
	tree->SetBranchAddress("LADC.hitLayer",LADC.hitLayer);
	tree->SetBranchAddress("LADC.hitGain",LADC.hitGain);
	tree->SetBranchAddress("LADC.hitSat",LADC.hitSat);
	tree->SetBranchAddress("LADC.hitSignal",LADC.hitSignal);
	tree->SetBranchAddress("SADC.nHits",&SADC.nHits);

	tree->SetBranchAddress("SADC.hitRow",SADC.hitRow);
	tree->SetBranchAddress("SADC.hitCol",SADC.hitCol);
	tree->SetBranchAddress("SADC.hitLayer",SADC.hitLayer);
	tree->SetBranchAddress("SADC.hitGain",SADC.hitGain);
	tree->SetBranchAddress("SADC.hitSat",SADC.hitSat);
	tree->SetBranchAddress("SADC.hitSignal",SADC.hitSignal);

	tree->SetBranchAddress("LMIP.nHits",&LMIP.nHits);
	tree->SetBranchAddress("LMIP.hitRow",LMIP.hitRow);
	tree->SetBranchAddress("LMIP.hitCol",LMIP.hitCol);
	tree->SetBranchAddress("LMIP.hitLayer",LMIP.hitLayer);
	tree->SetBranchAddress("LMIP.hitGain",LMIP.hitGain);
	tree->SetBranchAddress("LMIP.hitSat",LMIP.hitSat);
	tree->SetBranchAddress("LMIP.hitSignal",LMIP.hitSignal);

	tree->SetBranchAddress("SMIP.nHits",&SMIP.nHits);
	tree->SetBranchAddress("SMIP.hitRow",SMIP.hitRow);
	tree->SetBranchAddress("SMIP.hitCol",SMIP.hitCol);
	tree->SetBranchAddress("SMIP.hitLayer",SMIP.hitLayer);
	tree->SetBranchAddress("SMIP.hitGain",SMIP.hitGain);
	tree->SetBranchAddress("SMIP.hitSat",SMIP.hitSat);
	tree->SetBranchAddress("SMIP.hitSignal",SMIP.hitSignal);
#else
	tree->SetBranchStatus("*",0); //disable all branches

	tree->SetBranchStatus("nHits", 1);
	tree->SetBranchStatus("hitSignal", 1);
	tree->SetBranchStatus("hitSignal_MIPCR", 1);
	tree->SetBranchStatus("hitRow", 1);
	tree->SetBranchStatus("hitCol", 1);
	tree->SetBranchStatus("hitLayer", 1);
	tree->SetBranchStatus("xHit", 1);
	tree->SetBranchStatus("yHit", 1);
	tree->SetBranchStatus("zHit", 1);
	tree->SetBranchStatus("signalBranch", 1);
	tree->SetBranchStatus("signalCNBranch", 1);

	tree->SetBranchStatus("eventInfo", 1);
	tree->SetBranchStatus("eventInfoGood", 1);

	tree->SetBranchAddress("nHits",&nHits);
	tree->SetBranchAddress("hitSignal",&hitSignal);
	tree->SetBranchAddress("hitSignal_MIPCR",&hitSignal_MIPCR);
	tree->SetBranchAddress("hitRow",&hitRow);
	tree->SetBranchAddress("hitCol",&hitCol);
	tree->SetBranchAddress("hitLayer",&hitLayer);
	tree->SetBranchAddress("xHit",&xHit);
	tree->SetBranchAddress("yHit",&yHit);
	tree->SetBranchAddress("zHit",&zHit);
	tree->SetBranchAddress("signalBranch", &Signal);
	tree->SetBranchAddress("signalCNBranch", &Signal_cn);

	tree->SetBranchAddress("eventInfo", &(eventInfo.trigger));
	tree->SetBranchAddress("eventInfoGood", &(eventInfo.good));
#endif
}

void HandleInputPar(int argc, char **argv)
{
	stringstream usage;
	usage.clear();
	usage << endl;
	usage << "Usage:" << endl << endl;
	usage << "CALOCUBE OPTIONS" << endl;
	usage << "-i\t\t\tname of the input file" << endl;
	usage << "-p\t\t\tshow pedestal mean and RMS for all channels" << endl;
	usage << "-q\t\t\tshow pedestal mean and RMS with common noise subtraction for all channels" << endl;
	usage << "-c\t\t\tshow common noise on pedestal events on all chips" << endl;
	usage << "-u\t\t\tshow pileup distribution around zero" << endl;
	usage << "-r\t\t\tshow pedestal on layers between layer begin and layer end" << endl;
	usage << "-l\t\t\tshow pedestal with common noise subtraction on layers between layer begin and layer end" << endl;
	usage << "-d\t\t\tshow data on layers between layer begin and layer end" << endl;
	usage << "-R\t\t\tshow data using raw distribution with common noise subtraction" << endl;
	usage << "-M\t\t\tshow data using Muon MIP scale" << endl;
	usage << "-H\t\t\tshow pedestal(-r), pedestal with cn(-l) or data(-d) using chip map" << endl;
	usage << "-I\t\t\tshow data on Nello's crystals w/o common noise subtraction" << endl;
	usage << "-J\t\t\tshow data on Nello's crystals w/ common noise subtraction" << endl;
	usage << "-v\t\t\tshow centroid based on shower deposit" << endl;
	usage << "-D\t\t\tshow centroid using small sensor" << endl;
	usage << "-b\t\t\tset layer begin : default 0" << endl;
	usage << "-e\t\t\tset layer end : default NLAYERS" << endl;
	usage << "-B\t\t\tset chip begin : default 0" << endl;
	usage << "-E\t\t\tset chip end : default NCASISCHIPS" << endl;
	usage << "-s\t\t\tsave histogram while they are shown" << endl;
	usage << "-g\t\t\tsave all shown histogram in a pdf file without showing them" << endl;
	usage << "-t\t\t\tdirectory where histogram are saved : default ./" << endl;
	usage << "-y\t\t\tset log y" << endl;
	usage << "-n\t\t\tset nbin range for data" << endl;
	usage << "-a\t\t\tset minrange range for data" << endl;
	usage << "-z\t\t\tset maxrange range for data" << endl;
	usage << "-N\t\t\tset nbin range for pedestal" << endl;
	usage << "-A\t\t\tset minrange range for pedestal" << endl;
	usage << "-Z\t\t\tset maxrange range for pedestal" << endl << endl;
#if defined(AMS) || defined(ADAMO)
	usage << "Tracker OPTIONS" << endl;
	usage << "-P\t\t\tshow pedestal mean and RMS for all channels" << endl;
	usage << "-V\t\t\tshow hitmap (in order to set a flag use \"-C\", \"-S\" and \"-T\" option)" << endl;
	usage << "-C\t\t\tflag to select events for \"-H\" option: default -1" << endl;
	usage << "\t\t\t\t-1 : no cut (default)" << endl;
	usage << "\t\t\t\t[0, NCUBES-1] : events with energy release on at least one of selected ID cubes" << endl;
	usage << "\t\t\t\tNCUBES : events with energy release on at least one of all cubes" << endl;
	usage << "[-S, -T]\t\t\tenergy range to select events for \"-H\" option" << endl;
	usage << "\t\t\t\tif C=-1 : no effect (default)" << endl;
	usage << "\t\t\t\tif C=[0, NCUBES-1]: energy threshold on energy of a single cube" << endl;
	usage << "\t\t\t\tif C=NCUBES : energy threshold on total energy" << endl;
	usage << "-L\t\t\tlayer to select events for \"-H\" option" << endl;
	usage << "\t\t\t\tif C=-1 : no effect (default)" << endl;
	usage << "\t\t\t\tif C=[0, NCUBES-1] : 0-14 events with energy release on selected layers" << endl;
	usage << "\t\t\t\tif C=NCUBES : nothing" << endl;
#endif
	usage << "-h\t\t\thelp" << endl << endl;

	int c;

	while (
#if defined(AMS) || defined(ADAMO)
			(c = getopt(argc, argv, "i:pqcurldRMHIJvDb:e:B:E:sgt:yn:a:z:N:A:Z:PVC:S:T:L:h")) != -1
#else
			(c = getopt(argc, argv, "i:pqcurldRMHIJvDb:e:B:E:sgt:yn:a:z:N:A:Z:h")) != -1
#endif
	){
		switch (c) {
		case 'i':
			inputFileName = optarg;
			break;
		case 'p':
			show_pedestal = true;
			break;
		case 'q':
			show_cn_pedestal = true;
			break;
		case 'c':
			look_cn = true;
			break;
		case 'u':
			view_pileup = true;
			break;
		case 'r':
			view_pedestal = true;
			break;
		case 'l':
			view_cn_pedestal = true;
			break;
		case 'd':
			view_data = true;
			break;
		case 'R':
			raw = true;
			break;
		case 'M':
			muon = true;
			break;
		case 'H':
			view_hidra = true;
			break;
		case 'I':
			view_nello = true;
			break;
		case 'J':
			view_cn_nello = true;
			break;
		case 'v':
			view_centroid = true;
			break;
		case 'D':
			small_large = true;
			break;
		case 'b':
			layerbegin = atoi(optarg);
			if (layerbegin < 0 || layerbegin > NLAYERS-1) {
				cout << "Warning: Please enter a valid layer begin number. Exit.\n";
				exit(1);
			}
			break;
		case 'e':
			layerend = atoi(optarg);
			if (layerend < 0 || layerend > NLAYERS-1) {
				cout << "Warning: Please enter a valid layer end number. Exit.\n";
				exit(1);
			}
			break;
		case 'B':
			chipbegin = atoi(optarg);
			if (chipbegin < 0 || chipbegin > NCASISCHIPS-1) {
				cout << "Warning: Please enter a valid chip begin number. Exit.\n";
				exit(1);
			}
			break;
		case 'E':
			chipend = atoi(optarg);
			if (chipend < 0 || chipend > NCASISCHIPS-1) {
				cout << "Warning: Please enter a valid chip end number. Exit.\n";
				exit(1);
			}
			break;
		case 's':
			save_histo = true;
			break;
		case 't':
			dir = optarg;
			break;
		case 'g':
			save_global = true;
			break;
		case 'y':
			log_y = true;
			break;
		case 'n':
			nbindata=atoi(optarg);
			break;
		case 'a':
			minrangedata=atof(optarg);
			break;
		case 'z':
			maxrangedata=atof(optarg);
			break;
		case 'N':
			nbinpedestal=atoi(optarg);
			break;
		case 'A':
			minrangepedestal=atof(optarg);
			break;
		case 'Z':
			maxrangepedestal=atof(optarg);
			break;
#if defined(AMS) || defined(ADAMO)
		case 'P':
			show_tracker_pedestal = true;
			break;
		case 'V':
			show_tracker_hitmap = true;
			break;
		case 'C':
			CC_flag = atoi(optarg);
			if (CC_flag < -1 || CC_flag > NCUBES) {
				cout << "Warning: Please enter a valid flag. Exit.\n";
				exit(1);
			}
			break;
		case 'S':
			CC_threshold_low = atof(optarg);
			if (CC_threshold_low < 0.) {
				cout << "Warning: Please enter a valid low threshold. Exit.\n";
				exit(1);
			}
			break;
		case 'T':
			CC_threshold_high = atof(optarg);
			if (CC_threshold_high < 0.) {
				cout << "Warning: Please enter a valid high threshold. Exit.\n";
				exit(1);
			}
			break;
		case 'L':
			CC_layer = atoi(optarg);
			if (CC_layer < 0 || CC_layer > NLAYERS-1) {
				cout << "Warning: Please enter a valid layer. Exit.\n";
				exit(1);
			}
			break;
#endif
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

void exit_safe(int ret_val) {

	cc->Close();

	if(fCC->IsOpen())
		fCC->Close();

	exit(ret_val);
}

void sighandler(int code){
	cout << endl;
	cout << "Catched Signal " << code << endl;
	exit_safe(EXIT_SUCCESS);
}

int main(int argc, char** argv){

	//Handle input

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

	//Check how many events are in the file

	fCC = new TFile(inputFileName);

	TTree *dummytree = (TTree *)fCC->Get("data");

	if(!dummytree)
	{
		cout << "No TTree \"data\" TTree found: Exit... " << endl;
		exit_safe(EXIT_FAILURE);
	}

	EventInfo dummyeventInfo;

	dummytree->SetBranchAddress("eventInfo", &(dummyeventInfo.trigger));
	dummytree->SetBranchAddress("eventFlag", &(dummyeventInfo.good));

	int nEvents = dummytree->GetEntries();
	int pedEvents = 0;
	int dataEvents = 0;
	int pedGood = 0;
	int dataGood = 0;

	for(int iev=0; iev<nEvents; iev++){

		dummytree->GetEntry(iev);

		if(dummyeventInfo.trigger==PHYSICSTRIGGER)
		{
			dataEvents++;

			if(dummyeventInfo.good==GOODEVENTFLAG)
				dataGood++;
		}
		if(dummyeventInfo.trigger==RANDOMTRIGGER)
		{
			pedEvents++;

			if(dummyeventInfo.good==GOODEVENTFLAG)
				pedGood++;
		}
	}

	cout << "Calocube Total Events = " << nEvents << endl;
	cout << "Good pedestal event = " << pedGood << " of " << pedEvents << endl;
	cout << "Good data event = " << dataGood << " of " << dataEvents << endl;
	cout << endl;

	delete dummytree;

#ifdef AMS
	dummytree = (TTree *)fCC->Get("t4");

	if(!dummytree)
	{
		cout << "No TTree \"t4\" TTree found: Exit... " << endl;
		exit_safe(EXIT_FAILURE);
	}

	nEvents = dummytree->GetEntries();

	cout << "AMS Total Events = " << nEvents << endl;
	cout << endl;

	delete dummytree;
#elif ADAMO
	dummytree = (TTree *)fCC->Get("h99");

	if(!dummytree)
	{
		cout << "No TTree \"h99\" TTree found: Exit... " << endl;
		exit_safe(EXIT_FAILURE);
	}

	nEvents = dummytree->GetEntries();

	cout << "ADAMO Total Events = " << nEvents << endl;
	cout << endl;

	delete dummytree;
#endif

	//Configure graphics

	gROOT->Reset();

	int tmp=1;
	TApplication theApp("App",&tmp,argv); //TApplication

	gStyle->SetOptStat("neMuRo");
	gStyle->SetStatFormat("6.3f");

	Int_t  x, y;
	UInt_t w, h;
	gVirtualX->GetWindowSize(gVirtualX->GetDefaultRootWindow(),x,y,w,h);

	if(save_global) {
		gROOT->SetBatch();
		cg = new TCanvas("global_plot","global_plot", x,y,w,h);
		cg->Print(Form("%s/%s.pdf[", dir, inputFileName));
		cg->cd();
	}

	//========================================================
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// --------------------- GENERIC -------------------------
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//========================================================

	if(show_pedestal)
	{
		cc = new TCanvas("pedestal_raw","pedestal_raw", x,y,w,h);
		cc->Divide(1,1);

		TH1F *pedMean = (TH1F *)fCC->Get("pedHisto1DRaw");
		TH1F *pedRMS = (TH1F *)fCC->Get("sigmaHisto1DRaw");

		if(!pedMean)
		{
			cout << "No pedHisto1D found: Exit... " << endl;
			exit_safe(EXIT_FAILURE);
		}

		if(!pedRMS)
		{
			cout << "No sigmaHisto1DRaw found: Exit... " << endl;
			exit_safe(EXIT_FAILURE);
		}

		gPad->SetGridx();
		pedMean->GetXaxis()->SetRangeUser(0., (NCASISCHIPS-9.)*NCASISCHANNELS);
		pedMean->GetXaxis()->SetNdivisions((NCASISCHIPS-9)/3, 3, 0, kFALSE);
		pedMean->GetXaxis()->SetLabelSize(0.025);
		pedMean->GetYaxis()->SetLabelSize(0.025);
		pedMean->Draw("HIST");
		gPad->SetLogy(log_y);

		cc->Update();
		if(save_histo)
			cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_mean.eps");
		if(save_global)
			cg->Print(Form("%s/%s.pdf", dir, inputFileName));

		cc->WaitPrimitive();
		cc->Update();

		gPad->SetGridx();
		pedRMS->GetXaxis()->SetRangeUser(0., (NCASISCHIPS-9.)*NCASISCHANNELS);
		pedRMS->GetXaxis()->SetNdivisions((NCASISCHIPS-9)/3, 3, 0, kFALSE);
		pedRMS->GetXaxis()->SetLabelSize(0.025);
		pedRMS->GetYaxis()->SetLabelSize(0.025);
		pedRMS->Draw("HIST");
		gPad->SetLogy(log_y);

		cc->Update();
		if(save_histo)
			cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_rms.eps");
		if(save_global)
			cg->Print(Form("%s/%s.pdf", dir, inputFileName));

		cc->WaitPrimitive();
		cc->Update();

		cc->Close();
	}

	if(show_cn_pedestal)
	{
		cc = new TCanvas("pedestal_cn","pedestal_cn", x,y,w,h);
		cc->Divide(2,1);

		TH1F *pedMean = (TH1F *)fCC->Get("pedHisto1D");
		TH1F *pedRMS = (TH1F *)fCC->Get("sigmaHisto1D");

		//TH1F *pedMeanTrue = (TH1F *)fCC->Get("pedHisto1DTrue");
		//TH1F *pedRMSTrue = (TH1F *)fCC->Get("sigmaHisto1DTrue");

		if(!pedMean)
		{
			cout << "No pedHisto1D found: Exit... " << endl;
			exit_safe(EXIT_FAILURE);
		}

		if(!pedRMS)
		{
			cout << "No sigmaHisto1D found: Exit... " << endl;
			exit_safe(EXIT_FAILURE);
		}

		gPad->SetGridx();
		pedMean->GetXaxis()->SetRangeUser(0., (NCASISCHIPS-9.)*NCASISCHANNELS);
		pedMean->GetXaxis()->SetNdivisions((NCASISCHIPS-9)/3, 3, 0, kFALSE);
		pedMean->GetXaxis()->SetLabelSize(0.025);
		pedMean->GetYaxis()->SetLabelSize(0.025);
		pedMean->Draw("HIST");
		gPad->SetLogy(log_y);

		//pedMeanTrue->SetLineColor(kRed);
		//pedMeanTrue->Draw("HIST SAME");

		cc->Update();
		if(save_histo)
			cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_mean.eps");
		if(save_global)
			cg->Print(Form("%s/%s.pdf", dir, inputFileName));

		cc->WaitPrimitive();
		cc->Update();

		gPad->SetGridx();
		pedRMS->GetXaxis()->SetRangeUser(0., (NCASISCHIPS-9.)*NCASISCHANNELS);
		pedRMS->GetXaxis()->SetNdivisions((NCASISCHIPS-9)/3, 3, 0, kFALSE);
		pedRMS->GetXaxis()->SetLabelSize(0.025);
		pedRMS->GetYaxis()->SetLabelSize(0.025);
		pedRMS->Draw("HIST");
		gPad->SetLogy(log_y);

		//pedRMSTrue->SetLineColor(kRed);
		//pedRMSTrue->Draw("HIST SAME");

		cc->Update();
		if(save_histo)
			cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_rms.eps");
		if(save_global)
			cg->Print(Form("%s/%s.pdf", dir, inputFileName));

		cc->WaitPrimitive();
		cc->Update();

		cc->Close();
	}

	if(look_cn)
	{
#if defined (SPS2016) || defined (SPS2017Aug) || defined(SPS2017Nov)
		cc = new TCanvas("commmonNoise","commonNoise", x,y,w,h);
		cc->Divide(3,4);

		TGraphErrors *gAlphaLarge[NCASISCHIPS4CN];
		TGraphErrors *gAlphaSmall[NCASISCHIPS4CN];
		for (int iBoard = 0; iBoard<NCASISCHIPS/NCASIS_CHIPS_X_BOARD; iBoard++)
		{
			for (int iChip = 0; iChip<NCASIS_CHIPS_X_BOARD; iChip++)
			{
				for (int iSide = 0; iSide<NSIDE; iSide++)
				{
					int id = iBoard * NCASIS_CHIPS_X_BOARD * NSIDE + iChip * NSIDE + iSide;

					for (int iSensor = 0; iSensor<NSENSORS; iSensor++)
					{
						if(iBoard>=15) continue;

						cc->cd(iSensor * NCASIS_CHIPS_X_BOARD * NSIDE + iChip * NSIDE + iSide + 1);

						if(iSensor==0) {
							gAlphaLarge[id] = (TGraphErrors *)fCC->Get(Form("gAlphaLarge_%d", id));

							if(!gAlphaLarge[id])
							{
								cout << "No gAlphaLarge found: Exit... " << endl;
								exit_safe(EXIT_FAILURE);
							}

							gPad->Clear();
							gAlphaLarge[id]->Draw();
							gAlphaLarge[id]->SetTitle(Form("Scolopendra %d Large", id));
						} else if(iSensor==1) {
							gAlphaSmall[id] = (TGraphErrors *)fCC->Get(Form("gAlphaSmall_%d", id));

							if(!gAlphaSmall[id])
							{
								cout << "No gAlphaSmall found: Exit... " << endl;
								exit_safe(EXIT_FAILURE);
							}

							gPad->Clear();
							gAlphaSmall[id]->Draw();
							gAlphaLarge[id]->SetTitle(Form("Scolopendra %d Small", id));
						}
					}
				}
			}

			cc->Update();
			if(save_histo)
				cc->Print(Form("%s/%s_board_%d.eps", dir, cc->GetName(), iBoard));
			if(save_global)
				cg->Print(Form("%s/%s.pdf", dir, inputFileName));

			cc->WaitPrimitive();
			cc->Update();
		}
		cc->Close();
#else
		cc = new TCanvas("commmonNoise","commonNoise", x,y,w,h);
		cc->Divide(3,3);

		TH1F *cn[NCASISCHIPS];
		for (int iChip = 0; iChip<NCASISCHIPS; iChip++)
		{
			cc->cd(iChip + 1);

			cn[iChip] = (TH1F *)fCC->Get(Form("commonNoiseHisto_%d", iChip));
			if(!cn[iChip])
			{
				cout << "No commonNoiseHisto found: Exit... " << endl;
				exit_safe(EXIT_FAILURE);
			}

			cn[iChip]->Draw();
		}

		cc->Update();
		if(save_histo)
			cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
		if(save_global)
			cg->Print(Form("%s/%s.pdf", dir, inputFileName));

		cc->WaitPrimitive();
		cc->Update();

		cc->Close();
#endif
	}

	//========================================================
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ------------------- HIDRA DEBUG -----------------------
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//========================================================

	if(view_hidra) {
		if(chipbegin>chipend) {
			cout << "chip begin > chip end: Exit...";
			exit_safe(EXIT_FAILURE);
		}

		//========================================================
		// create histos
		//========================================================
		TH1F *hChannel[NCASISCHIPS][NCASISCHANNELS];
		for(int k=0; k<NCASISCHIPS; k++) {
			for(int j=0; j<NCASISCHANNELS; j++) {
				hChannel[k][j]=new TH1F("", "", nbinpedestal, minrangepedestal, maxrangepedestal);
				if (view_pedestal) hChannel[k][j]->SetName(Form("pedestalChip_%d_%d", k, j));
				else if (view_cn_pedestal) hChannel[k][j]->SetName(Form("pedestalCNChip_%d_%d", k, j));
				else hChannel[k][j]->SetName(Form("DataChip_%d_%d", k, j));
			}
		}

		TTree *tree = (TTree *)fCC->Get("data");

		//tree->SetMakeClass(1);

		if(!tree)
		{
			cout << "No TTree found: Exit... " << endl;
			exit_safe(EXIT_FAILURE);
		}

		ConstructTree(tree);

		// start loop over pedestal events
		int nEvent = tree->GetEntries();
		bool goodEvent = false;
		for(int iev=0; iev<nEvent; iev++){

			tree->GetEntry(iev);

			if (view_pedestal || view_cn_pedestal) {
				if(eventInfo.trigger!=RANDOMTRIGGER) continue;
			} else {
				if(eventInfo.trigger!=PHYSICSTRIGGER)continue;
			}
			if(eventInfo.good!=GOODEVENTFLAG)continue;
			goodEvent = true;
			for (int iChip = 0; iChip < NCASISCHIPS;iChip++) {
				for (int iChannel = 0;iChannel < NCASISCHANNELS;iChannel++) {
					if (view_cn_pedestal) hChannel[iChip][iChannel]->Fill(Signal[iChip][iChannel]);
					else if (view_pedestal) hChannel[iChip][iChannel]->Fill(Signal_cn[iChip][iChannel]);
					else hChannel[iChip][iChannel]->Fill(Signal[iChip][iChannel]);
				}
			}
		}

		if(goodEvent) {
			if(cc) cc->Close();
			cc = new TCanvas("canvas","canvas", x,y,w,h);
			cc->Divide(6,5);

			for(int k=chipbegin; k<=chipend; k++) {
				if (view_pedestal) {
					cc->SetTitle(Form("PedestalOnChip_%d", k));
					cc->SetName(Form("PedestalOnChip_%d", k));
				} else if (view_cn_pedestal) {
					cc->SetTitle(Form("PedestalCNOnChip_%d", k));
					cc->SetName(Form("PedestalCNOnChip_%d", k));
				} else {
					cc->SetTitle(Form("DataOnChip_%d", k));
					cc->SetName(Form("DataOnChip_%d", k));
				}

				cout << "CHIP " << k << endl;
				cout << "-------------------------------------------------------------------" << endl;
				for(int j=0; j<NCASISCHANNELS; j++){
					cc->cd(j+1);

					int row = -1;
					int col = -1;
					int layer = -1;
					int sensor = -1;

					if(Chip2Cube(k, j, row, col, layer, sensor)) {
						if(sensor==0) hChannel[k][j]->SetLineColor(kBlue);
						else hChannel[k][j]->SetLineColor(kRed);
					} else {
						hChannel[k][j]->SetLineColor(kBlack);
						hChannel[k][j]->SetLineStyle(2);
					}

					hChannel[k][j]->Draw();
					gPad->SetLogy(log_y);
					cout << "CHANNEL " << j <<
							" Mean " <<  hChannel[k][j]->GetMean() <<
							" RMS " <<  hChannel[k][j]->GetRMS() << endl;
				}
				cout << endl;

				cc->Update();
				if(save_histo) {
					if(view_pedestal)
						cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_pedestal.eps");
					else if (view_cn_pedestal)
						cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_pedestal_cn.eps");
					else
						cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_data.eps");
				}
				if(save_global)
					cg->Print(Form("%s/%s.pdf", dir, inputFileName));

				cc->WaitPrimitive();
				cc->Update();
			}
			cc->Close();
		}
		else
			cout << "No good event found" << endl;
		delete tree;
	}

	if(!view_hidra)
		if(view_pileup || view_pedestal || view_cn_pedestal || view_data)
		{
			cc = new TCanvas("dummy","dummy", x,y,w,h);

			if(layerbegin>layerend)
			{
				cout << "Layer begin > layer end: Exit...";
				exit_safe(EXIT_FAILURE);
			}

			TTree *tree = (TTree *)fCC->Get("data");

			//tree->SetMakeClass(1);

			if(!tree)
			{
				cout << "No TTree found: Exit... " << endl;
				exit_safe(EXIT_FAILURE);
			}

			ConstructTree(tree);

			if(view_pileup) {

				//========================================================
				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				// --------------------- PILE UP -------------------------
				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				//========================================================

#if defined (SPS2016) || defined (SPS2017Aug) || defined(SPS2017Nov)
				//========================================================
				// create histos
				//========================================================

				TH1F *pileUp[NSENSORS];
				pileUp[0] = new TH1F("pileup_Large", "", 100, -200000., +200000.);
				pileUp[1] = new TH1F("pileup_Small", "", 100, -2000., +2000.);

				////////////////////////////////////////////////////////
				// start loop over events
				////////////////////////////////////////////////////////

				int nEvent = tree->GetEntries();
				for(int iev=0; iev<nEvent; iev++){
					tree->GetEntry(iev);

					//==================================
					// selection
					//==================================
					if(eventInfo.trigger!=PHYSICSTRIGGER)continue;
					if(eventInfo.good!=GOODEVENTFLAG)continue;

					double sumdE[NSENSORS]={0.};
					for (int iCasis = 0; iCasis < NCASISCHIPS; iCasis++) {
						for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS; iCasisChannel++) {
							int row = -1;
							int col = -1;
							int layer = -1;
							int sensor = -1;

							if(Chip2Cube(iCasis, iCasisChannel, row, col, layer, sensor)) {
								if(sensor!=0 && sensor!=1)
									continue;

								sumdE[sensor]+=Signal[iCasis][iCasisChannel];
							}
						}
					}
					for(int isensor=0; isensor<NSENSORS; isensor++)
						pileUp[isensor]->Fill(sumdE[isensor]);
				}

				if(cc) cc->Close();
				cc = new TCanvas("pileup","pileup", x,y,w,h);
				cc->Divide(2);

				for(int isensor=0; isensor<NSENSORS; isensor++) {
					cc->cd(isensor+1);

					pileUp[isensor]->Draw();
					pileUp[isensor]->SetTitle(isensor==0? "Large PD" : "Small PD");
					pileUp[isensor]->GetXaxis()->SetTitle("sumdE [ADC]");
					gPad->SetLogy(log_y);
				}

				if(save_histo)
					cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_pile_up.eps");
				if(save_global)
					cg->Print(Form("%s/%s.pdf", dir, inputFileName));

				cc->WaitPrimitive();
				cc->Update();

				cc->Close();
#else
				cout << "Pile up option not implemented before 2016" << endl;
#endif
			}

			//========================================================
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// -------------------- PEDESTAL -------------------------
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			//========================================================

			if(view_pedestal || view_cn_pedestal)
			{
#if defined (SPS2016) || defined (SPS2017Aug) || defined(SPS2017Nov)
				//========================================================
				// create histos
				//========================================================

				TH1F *cubePed[NLAYERS][NCUBES][NSENSORS];
				for(int k=0; k<NLAYERS; k++)
					for(int j=0; j<NCUBES; j++)
						for(int i=0; i<NSENSORS; i++)
						{
							cubePed[k][j][i]=new TH1F("", "", nbinpedestal, minrangepedestal, maxrangepedestal);
							if(i==0)
								cubePed[k][j][i]->SetName(Form("pedestalCube_Large_%d_%d", k, j));
							else
								cubePed[k][j][i]->SetName(Form("pedestalCube_Small_%d_%d", k, j));
						}

				////////////////////////////////////////////////////////
				// start loop over events
				////////////////////////////////////////////////////////

				int nEvent = tree->GetEntries();
				int cube = -1;
				bool goodEvent = false;
				for(int iev=0; iev<nEvent; iev++){
					tree->GetEntry(iev);

					//==================================
					// selection
					//==================================
					if(eventInfo.trigger!=RANDOMTRIGGER)continue;
					if(eventInfo.good!=GOODEVENTFLAG)continue;

					goodEvent = true;

					for (int iCasis = 0; iCasis < NCASISCHIPS; iCasis++) {
						for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS; iCasisChannel++) {
							int row = -1;
							int col = -1;
							int layer = -1;
							int sensor = -1;

							if(Chip2Cube(iCasis, iCasisChannel, row, col, layer, sensor)) {
								if(sensor!=0 && sensor!=1)
									continue;

								cube = row*NCOLS + col;
								if(view_cn_pedestal)
									cubePed[layer][cube][sensor]->Fill(Signal[iCasis][iCasisChannel]);
								else if(view_pedestal)
									cubePed[layer][cube][sensor]->Fill(Signal_cn[iCasis][iCasisChannel]);
							}
						}
					}
				}

				if(goodEvent)
				{
					//==================================
					// draw event
					//==================================
					if(cc) cc->Close();
					cc = new TCanvas("canvas","canvas", x,y,w,h);
					cc->Divide(5,5);

					for(int i=0; i<NSENSORS; i++) {
						for(int k=layerbegin; k<layerend; k++)
						{
							if (i==0) {
								cc->SetTitle(Form("PedestalOnLayer_Large_%d", k));
								cc->SetName(Form("PedestalOnLayer_Large_%d", k));
							} else {
								cc->SetTitle(Form("PedOnLayer_Small_%d", k));
								cc->SetName(Form("PedOnLayer_Small_%d", k));
							}

							for(int j=0; j<NCUBES; j++)
							{
								cc->cd(j+1);
								cubePed[k][j][i]->Draw();
								gPad->SetLogy(log_y);
							}
							cc->Update();
							if(save_histo) {
								if (view_pedestal)
									cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_pedestal.eps");
								else if (view_cn_pedestal)
									cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_pedestal_cn.eps");
							}
							if(save_global)
								cg->Print(Form("%s/%s.pdf", dir, inputFileName));

							cc->WaitPrimitive();
							cc->Update();
						}
					}

					cc->Close();
				}
				else
					cout << "No good pedestal event found" << endl;
#else
				//========================================================
				// create histos
				//========================================================

				TH1F *cubePed[NLAYERS][NCUBES];
				for(int k=0; k<NLAYERS; k++)
				{
					for(int j=0; j<NCUBES; j++)
					{
						cubePed[k][j]=new TH1F("", "", nbinpedestal, minrangepedestal, maxrangepedestal);
						cubePed[k][j]->SetName(Form("pedestalCube_%d_%d", k, j));
					}
				}
				TH1F *cubePedAux[NLAYERS][NCUBES];
				for(int k=0; k<NLAYERS; k++)
				{
					for(int j=0; j<NCUBES; j++)
					{
						cubePedAux[k][j]=new TH1F("", "", nbinpedestal, minrangepedestal, maxrangepedestal);
						cubePedAux[k][j]->SetName(Form("pedestalCube2_%d_%d", k, j));
					}
				}


				////////////////////////////////////////////////////////
				// start loop over events
				////////////////////////////////////////////////////////

				int nEvent = tree->GetEntries();
				int hitCube = -1;
				bool goodEvent = false;
				for(int iev=0; iev<nEvent; iev++){

					tree->GetEntry(iev);

					//==================================
					// selection
					//==================================
					if(eventInfo.trigger!=RANDOMTRIGGER)continue;
					if(eventInfo.good!=GOODEVENTFLAG)continue;

					goodEvent = true;

					for (int iCasis = 0; iCasis < NCASISCHIPS; iCasis++) {
						for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS; iCasisChannel++) {

							int row = -1;
							int col = -1;
							int layer = -1;

							if(Chip2Cube(iCasis, iCasisChannel, row, col, layer))
							{
								hitCube = row*NCOLS + col;
								if(layer<NLAYERS)
									cubePed[layer][hitCube]->Fill(Signal[iCasis][iCasisChannel]);
								else
								{
									layer -= NLAYERS;
									cubePedAux[layer][hitCube]->Fill(Signal[iCasis][iCasisChannel]);
								}
							}
						}
					}
				}

				if(goodEvent)
				{
					//==================================
					// draw event
					//==================================
					if(cc) cc->Close();
					cc = new TCanvas("canvas","canvas", x,y,w,h);
					cc->Divide(3,3);

					for(int k=layerbegin; k<layerend + 1; k++)
					{
						cc->SetTitle(Form("PedestalOnLayer_%d", k));
						cc->SetName(Form("PedestalOnLayer_%d", k));

						for(int j=0; j<NCUBES; j++)
						{
							cc->cd(j+1);
							cubePed[k][j]->Draw();
							gPad->SetLogy(log_y);
						}
						cc->Update();
						if(save_histo)
							cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
						if(save_global)
							cg->Print(Form("%s/%s.pdf", dir, inputFileName));

						cc->WaitPrimitive();
						cc->Update();
					}

					cc->Close();
					cc = new TCanvas("canvas","canvas", x,y,w,h);
					cc->Divide(3,1);

					cc->SetTitle(Form("Pedestal_small"));
					cc->SetName(Form("Pedestal_small"));

					for(int k=NLAYERS; k<NLAYERS + 3; k++)
					{
						cc->cd(k+1-NLAYERS);
						cubePedAux[k-NLAYERS][4]->Draw();
						gPad->SetLogy(log_y);
					}
					cc->Update();
					if(save_histo)
						cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
					if(save_global)
						cg->Print(Form("%s/%s.pdf", dir, inputFileName));

					cc->WaitPrimitive();
					cc->Update();

					cc->Close();
				}
				else
					cout << "No good pedestal event found" << endl;
#endif
			}

			//========================================================
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// ---------------------- DATA ---------------------------
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			//========================================================

			if(view_data)
			{
#if defined(SPS2016) || defined(SPS2017Aug) || defined(SPS2017Nov)
				//========================================================
				// create histos
				//========================================================

				TH1F *cubeData[NLAYERS][NCUBES][NSENSORS];

				for(int k=0; k<NLAYERS; k++)
					for(int j=0; j<NCUBES; j++)
						for(int i=0; i<NSENSORS; i++)
						{
							cubeData[k][j][i]=new TH1F("", "", nbindata, minrangedata, maxrangedata);
							if(i==0)
								cubeData[k][j][i]->SetName(Form("signalCube_Large_%d_%d", k, j));
							else
								cubeData[k][j][i]->SetName(Form("signalCube_Small_%d_%d", k, j));
						}

				TH1F *cubeTotLarge = new TH1F("total_signal_large", "Large PD", nbindata, 5.*minrangedata, 5.*maxrangedata);
				TH1F *cubeTotSmall = new TH1F("total_signal_small", "Small PD", nbindata, 5.*minrangedata/100., 5.*maxrangedata/100.);

				////////////////////////////////////////////////////////
				// start loop over events
				////////////////////////////////////////////////////////

				int nEvent = tree->GetEntries();
				int cube = -1;
				bool goodEvent = false;
				for(int iev=0; iev<nEvent; iev++){
					tree->GetEntry(iev);

					//==================================
					// selection
					//==================================
					if(eventInfo.trigger!=PHYSICSTRIGGER)continue;
					if(eventInfo.good!=GOODEVENTFLAG)continue;

					goodEvent = true;

					Double_t totalLarge = 0;
					Double_t totalSmall = 0;

					if(raw) {
						for (int iCasis = 0; iCasis < NCASISCHIPS; iCasis++) {
							for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS; iCasisChannel++) {
								int row = -1;
								int col = -1;
								int layer = -1;
								int sensor = -1;

								if(Chip2Cube(iCasis, iCasisChannel, row, col, layer, sensor)) {
									if(sensor!=0 && sensor!=1)
										continue;

									cube = row*NCOLS + col;
									cubeData[layer][cube][sensor]->Fill(Signal[iCasis][iCasisChannel]);
									if(sensor==0)
										totalLarge += Signal[iCasis][iCasisChannel];
									else
										totalSmall += Signal[iCasis][iCasisChannel];
								}
							}
						}

						cubeTotLarge->Fill(totalLarge);
						cubeTotSmall->Fill(totalSmall);
					} else {
						Hits hits_large;
						Hits hits_small;

						hits_large = muon? LMIP : LADC;
						hits_small = muon? SMIP : SADC;

						for(int ihits=0; ihits<hits_large.nHits; ihits++){
							const int hitLayer = hits_large.hitLayer[ihits];
							const int hitCube = hits_large.hitRow[ihits]*NCOLS + hits_large.hitCol[ihits];
							const double hitSig = hits_large.hitSignal[ihits];

							cubeData[hitLayer][hitCube][0]->Fill(hitSig);
							totalLarge += hitSig;
						}
						for(int ihits=0; ihits<hits_small.nHits; ihits++){
							const int hitLayer = hits_small.hitLayer[ihits];
							const int hitCube = hits_small.hitRow[ihits]*NCOLS + hits_small.hitCol[ihits];
							const double hitSig = hits_small.hitSignal[ihits];

							cubeData[hitLayer][hitCube][1]->Fill(hitSig);
							totalSmall += hitSig;
						}

						cubeTotLarge->Fill(totalLarge);
						cubeTotSmall->Fill(totalSmall);
					}
				}

				if(goodEvent)
				{
					//==================================
					// draw event
					//==================================
					if(cc) cc->Close();
					cc = new TCanvas("canvas","canvas", x,y,w,h);
					cc->Divide(5,5);

					for(int i=0; i<NSENSORS; i++) {
						for(int k=layerbegin; k<layerend; k++)
						{
							if (i==0) {
								cc->SetTitle(Form("energyOnLayer_Large_%d", k));
								cc->SetName(Form("energyOnLayer_Large_%d", k));
							} else {
								cc->SetTitle(Form("energyOnLayer_Small_%d", k));
								cc->SetName(Form("energyOnLayer_Small_%d", k));
							}

							for(int j=0; j<NCUBES; j++)
							{
								cc->cd(j+1);
								cubeData[k][j][i]->Draw();
								gPad->SetLogy(log_y);
							}
							cc->Update();
							if(save_histo)
								cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
							if(save_global)
								cg->Print(Form("%s/%s.pdf", dir, inputFileName));

							cc->WaitPrimitive();
							cc->Update();
						}
					}

					cc->Close();

					cc = new TCanvas("total_signal_large","total_signal_large", x,y,w,h);

					cubeTotLarge->Draw();
					gPad->SetLogy(log_y);
					cc->Update();
					if(save_histo)
						cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
					if(save_global)
						cg->Print(Form("%s/%s.pdf", dir, inputFileName));

					cc->WaitPrimitive();
					cc->Update();
					cc->Close();

					cc = new TCanvas("total_signal_small","total_signal_small", x,y,w,h);

					cubeTotSmall->Draw();
					gPad->SetLogy(log_y);
					cc->Update();
					if(save_histo)
						cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
					if(save_global)
						cg->Print(Form("%s/%s.pdf", dir, inputFileName));

					cc->WaitPrimitive();
					cc->Update();
					cc->Close();
				}
				else
					cout << "No good physics event found" << endl;
#else
				//========================================================
				// create histos
				//========================================================

				TH1F *cubeData[NLAYERS][NCUBES];
				for(int k=0; k<NLAYERS; k++)
				{
					for(int j=0; j<NCUBES; j++)
					{
						cubeData[k][j]=new TH1F("", "", nbindata, minrangedata, maxrangedata);
						cubeData[k][j]->SetName(Form("signalCube_%d_%d", k, j));
					}
				}
				TH1F *cubeData2[NLAYERS][NCUBES];
				for(int k=0; k<NLAYERS; k++)
				{
					for(int j=0; j<NCUBES; j++)
					{
						cubeData2[k][j]=new TH1F("", "", nbinpedestal, minrangedata, maxrangedata/60.);
						cubeData2[k][j]->SetName(Form("signalCube2_%d_%d", k, j));
					}
				}

				TH1F *cubeTot = new TH1F("total Signal", "total Signal", nbindata, 5.*minrangedata, 5.*maxrangedata);

				////////////////////////////////////////////////////////
				// start loop over events
				////////////////////////////////////////////////////////

				int nEvent = tree->GetEntries();
				int hitCube = 0;
				bool goodEvent = false;
				for(int iev=0; iev<nEvent; iev++){

					tree->GetEntry(iev);
					//==================================
					// selection
					//==================================
					if(eventInfo.trigger!=PHYSICSTRIGGER)continue;
					if(eventInfo.good!=GOODEVENTFLAG)continue;
					if(nHits==0)continue;

					goodEvent = true;

					Double_t total = 0;

					for(int ihits=0; ihits<nHits; ihits++){
						hitCube = (3*hitRow[ihits]) + hitCol[ihits];
						if(hitLayer[ihits]<NLAYERS)
						{
							if(muon) {
								cubeData[(hitLayer[ihits])][hitCube]->Fill(hitSignal_MIPCR[ihits]);
								total += hitSignal_MIPCR[ihits];
							} else {
								cubeData[(hitLayer[ihits])][hitCube]->Fill(hitSignal[ihits]);
								total += hitSignal[ihits];
							}
						}
						else
						{
							hitLayer[ihits]-=NLAYERS;
							if(muon) {
								cubeData2[(hitLayer[ihits])][hitCube]->Fill(hitSignal_MIPCR[ihits]);
								total += hitSignal_MIPCR[ihits];
							} else {
								cubeData2[(hitLayer[ihits])][hitCube]->Fill(hitSignal[ihits]);
								total += hitSignal[ihits];
							}
						}
					}

					cubeTot->Fill(total);
				}

				if(goodEvent)
				{
					//==================================
					// draw event
					//==================================
					if(cc) cc->Close();
					cc = new TCanvas("canvas","canvas", x,y,w,h);
					cc->Divide(3,3);

					for(int k=layerbegin; k<layerend + 1; k++)
					{
						if(muon) {
							cc->SetTitle(Form("energyCROnLayer_%d", k));
							cc->SetName(Form("energyCROnLayer_%d", k));
						} else {
							cc->SetTitle(Form("energyOnLayer_%d", k));
							cc->SetName(Form("energyOnLayer_%d", k));
						}

						for(int j=0; j<NCUBES; j++)
						{
							cc->cd(j+1);
							cubeData[k][j]->Draw();
							gPad->SetLogy(log_y);
						}
						cc->Update();
						if(save_histo)
							cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
						if(save_global)
							cg->Print(Form("%s/%s.pdf", dir, inputFileName));

						cc->WaitPrimitive();
						cc->Update();
					}
					cc->Close();

					cc = new TCanvas("canvas","canvas", x,y,w,h);
					if(muon) {
						cc->SetTitle(Form("energyCR_small"));
						cc->SetName(Form("energyCR_small"));
					} else {
						cc->SetTitle(Form("energy_small"));
						cc->SetName(Form("energy_small"));
					}
					cc->Divide(3, 1);
					for(int k=NLAYERS; k<NLAYERS + 3; k++)
					{
						cc->cd(k+1-NLAYERS);
						cubeData2[k-NLAYERS][4]->Draw();
						gPad->SetLogy(log_y);
					}
					cc->Update();
					if(save_histo)
						cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
					if(save_global)
						cg->Print(Form("%s/%s.pdf", dir, inputFileName));

					cc->WaitPrimitive();
					cc->Update();

					cc->Close();

					cc = new TCanvas("canvas","canvas", x,y,w,h);
					if(muon) {
						cc = new TCanvas("totalsignalCR","totalsignalCR", x,y,w,h);
					} else {
						cc = new TCanvas("totalsignal","totalsignal", x,y,w,h);
					}
					cc->Divide(1, 1);

					cubeTot->Draw();
					gPad->SetLogy(log_y);
					cc->Update();
					if(save_histo)
						cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
					if(save_global)
						cg->Print(Form("%s/%s.pdf", dir, inputFileName));

					cc->WaitPrimitive();
					cc->Update();
					cc->Close();
				}
				else
					cout << "No good physics event found" << endl;
#endif
			}

			delete tree;
		}

	//========================================================
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ---------------------- NELLO --------------------------
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//========================================================

	if(view_nello || view_cn_nello)
	{
#if defined(SPS2017Aug) || defined(SPS2017Nov)
		//========================================================
		// create histos
		//========================================================

		const int NGROUP=3;
		const int NCRYS_PER_GROUP=8;

		TH1F *cubeNello[NGROUP][NCRYS_PER_GROUP][NSENSORS];

		for(int k=0; k<NGROUP; k++)
			for(int j=0; j<NCRYS_PER_GROUP; j++)
				for(int i=0; i<NSENSORS; i++)
				{
					cubeNello[k][j][i]=new TH1F("", "", nbindata, minrangedata, maxrangedata);
					if(i==0)
						cubeNello[k][j][i]->SetName(Form("nelloCube_Large_%d_%d", k, j));
					else
						cubeNello[k][j][i]->SetName(Form("nelloCube_Small_%d_%d", k, j));
				}

		TTree *tree = (TTree *)fCC->Get("data");

		//tree->SetMakeClass(1);

		if(!tree)
		{
			cout << "No TTree found: Exit... " << endl;
			exit_safe(EXIT_FAILURE);
		}

		ConstructTree(tree);

		////////////////////////////////////////////////////////
		// start loop over events
		////////////////////////////////////////////////////////

		int nEvent = tree->GetEntries();
		int cube = -1;
		bool goodEvent = false;
		for(int iev=0; iev<nEvent; iev++){
			tree->GetEntry(iev);

			//==================================
			// selection
			//==================================
			if(eventInfo.trigger!=PHYSICSTRIGGER)continue;
			if(eventInfo.good!=GOODEVENTFLAG)continue;

			goodEvent = true;

			for (int iCasis = 0; iCasis < NCASISCHIPS; iCasis++) {
				for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS; iCasisChannel++) {
					int row = -1;
					int col = -1;
					int layer = -1;
					int sensor = -1;

					if(Chip2Cube(iCasis, iCasisChannel, row, col, layer, sensor)) {
						cube = row*NCOLS + col;

						int group=-1;

						if(cube==27) group=0;
						if(cube==28) group=1;
						if(cube==25) group=2;

						if(group==0) layer -= 10;
						if(group==1) layer -= 12;
						if(group==2) layer -= 12;

						if(group==0) sensor -= 1000000;
						if(group==1) sensor -= 1000000;
						if(group==2) sensor -= 1000;

						if(group<0) continue;

						if(view_nello)
							cubeNello[group][layer][sensor]->Fill(Signal_cn[iCasis][iCasisChannel]);
						else if(view_cn_nello)
							cubeNello[group][layer][sensor]->Fill(Signal[iCasis][iCasisChannel]);
					}
				}
			}
		}

		if(goodEvent)
		{
			//==================================
			// draw event
			//==================================
			if(cc) cc->Close();
			cc = new TCanvas("canvas","canvas", x,y,w,h);
			cc->Divide(4, 4);

			for(int k=0; k<NGROUP; k++)
			{
				cc->SetTitle(Form("crystal_group_%d", k));
				cc->SetName(Form("crystal_group_%d", k));

				for(int j=0; j<NCRYS_PER_GROUP; j++)
					for(int i=0; i<NSENSORS; i++) {

						cc->cd(j+i*NCRYS_PER_GROUP+1);

						if (k==0 && j>=8) {
							gPad->Clear();
							continue;
						}
						if (k==1 && j>=5) {
							gPad->Clear();
							continue;
						}
						if (k==2 && j>=6) {
							gPad->Clear();
							continue;
						}

						cubeNello[k][j][i]->SetLineColor(i==0? kBlue : kRed);
						cubeNello[k][j][i]->Draw();
						gPad->SetLogy(log_y);
					}
				cc->Update();
				if(save_histo) {
					if (view_nello) 
						cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
					else if (view_cn_nello) 
						cc->Print(TString(dir) + "/" + TString(cc->GetName()) + "_cn.eps");
				}	
				if(save_global)
					cg->Print(Form("%s/%s.pdf", dir, inputFileName));
				cc->WaitPrimitive();
				cc->Update();
			}

			cc->Close();
		}
		else
			cout << "No good physics event found" << endl;

		delete tree;
#else
		cout << "No Nello crystals before SPS2017!" << endl;
#endif
	}

	//========================================================
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// --------------------- CENTROID ------------------------
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//========================================================

	if(view_centroid)
	{
		TH2F *hBarycenter = new TH2F("Barycenter", "Barycenter",
				(int)((CsI_SIZE+CsI_GAP)*5./0.1), -2.5*(CsI_SIZE+CsI_GAP), +2.5*(CsI_SIZE+CsI_GAP),
				(int)((CsI_SIZE+CsI_GAP)*5./0.1), -2.5*(CsI_SIZE+CsI_GAP), +2.5*(CsI_SIZE+CsI_GAP));

		TTree *tree = (TTree *)fCC->Get("data");

		//tree->SetMakeClass(1);

		if(!tree)
		{
			cout << "No TTree found: Exit... " << endl;
			exit_safe(EXIT_FAILURE);
		}

		ConstructTree(tree);

		int nEvent = tree->GetEntries();
		for(int iev=0; iev<nEvent; iev++){
			tree->GetEntry(iev);

			//==================================
			// selection
			//==================================
			if(eventInfo.trigger!=PHYSICSTRIGGER)continue;
			if(eventInfo.good!=GOODEVENTFLAG)continue;

			double dE_x[ncols] = {0.};
			double dE_y[nrows] = {0.};

			Hits hits;
			if(muon)
				hits = small_large? SMIP : LMIP;
			else
				hits = small_large? SADC : LADC;

			double threshold;
			if(muon)
				threshold = 1.;
			else
				threshold = small_large? 1000. : 250.;

			for(int ih=0; ih<hits.nHits; ih++) {
				float hitSignal = hits.hitSignal[ih];
				if(hitSignal>threshold) {
					dE_x[hits.hitCol[ih]] += hitSignal;
					dE_y[hits.hitRow[ih]] += hitSignal;
				}
			}

			double w;

			double x=0.;
			w=0.;
			for(int icol=0; icol<ncols; ++icol) {
				double pos = (CsI_SIZE * (((double)icol) + 0.5)) + (CsI_GAP * ((double)icol)); //Origin in bottom left corner
				pos -= (2.5*CsI_SIZE) + (2.*CsI_GAP); //Origin in central cube
				x += dE_x[icol] * pos;
				w += dE_x[icol];
			}
			x /= w;

			if(w==0. || x==0.) continue;

			double  y=0.;
			w=0.;
			for(int irow=0; irow<nrows; ++irow) {
				double pos = (CsI_SIZE * (nrows - (((double)irow) + 0.5))) + (CsI_GAP * (nrows-1. - (double)irow)); //Origin in bottom left corner
				pos -= (2.5*CsI_SIZE) + (2.*CsI_GAP); //Origin in central cube
				y += dE_y[irow] * pos;
				w += dE_y[irow];
			}
			y /= w;

			if(w==0. || y==0.) continue;

			hBarycenter->Fill(x, y);
		}

		cc = new TCanvas("Barycenter", "Barycenter", (int)((x+w-h)/2.), y , (int)((x+w+h)/2.) ,h);
		cc->cd();

		TPave *pave[nrows][ncols];
		for(int irow=0; irow<nrows; ++irow)
			for(int icol=0; icol<ncols; ++icol) {
				const double center = (CsI_SIZE*2.5) + (CsI_GAP*2.);
				const double x1 = (CsI_SIZE * ((double)icol)) + (CsI_GAP * ((double)icol)) - center;
				const double y1 = (CsI_SIZE * ((double)irow)) + (CsI_GAP * ((double)irow)) - center;
				const double x2 = x1 + CsI_SIZE;
				const double y2 = y1 + CsI_SIZE;
				pave[irow][icol] = new TPave(x1, y1, x2, y2);
				pave[irow][icol]->SetFillStyle(0);
				pave[irow][icol]->SetLineWidth(2);
			}

		//hBarycenter->Rebin2D(8, 8);
		hBarycenter->Draw("COLZ");

		for(int irow=0; irow<nrows; ++irow)
			for(int icol=0; icol<ncols; ++icol)
				pave[irow][icol]->Draw("same");
		gPad->Modified();
		gPad->Update();

		cc->Update();
		if(save_histo)
			cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
		if(save_global)
			cg->Print(Form("%s/%s.pdf", dir, inputFileName));

		cc->WaitPrimitive();
		cc->Update();
	}


	//========================================================
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// -------------------- TRACKER --------------------------
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//========================================================
#if defined(AMS) || defined(ADAMO)

	if(show_tracker_pedestal)
	{
#ifdef AMS
		cout << "This function is not implemented for AMS" << endl;
#elif ADAMO
		gStyle->SetOptStat(0);
		gStyle->SetTitleFontSize(0.08);

		TH1F *pedMeanADAMO[NLAYER_ADAMO][NXY_ADAMO];
		TH1F *pedRMSADAMO[NLAYER_ADAMO][NXY_ADAMO];

		for(int ilayer=0; ilayer < NLAYER_ADAMO; ++ilayer)
			for(int ixy=0; ixy < NXY_ADAMO; ++ixy)
			{
				const int imean=100+ixy+(1+ilayer)*NXY_ADAMO-1;
				const int irms=200+ixy+(1+ilayer)*NXY_ADAMO-1;
				pedMeanADAMO[ilayer][ixy] = (TH1F *)fCC->Get(Form("h%d", imean));
				pedMeanADAMO[ilayer][ixy]->SetTitle(Form("Pedestal Mean %d%s", 1+ilayer, ixy==0? "X" : "Y"));
				pedRMSADAMO[ilayer][ixy] = (TH1F *)fCC->Get(Form("h%d", irms));
				pedRMSADAMO[ilayer][ixy]->SetTitle(Form("Pedestal RMS %d%s", 1+ilayer, ixy==0? "X" : "Y"));

				if(!pedMeanADAMO[ilayer][ixy])
				{
					cout << "No " << Form("h%d", imean) << " found: Exit... " << endl;
					exit_safe(EXIT_FAILURE);
				}

				if(!pedRMSADAMO[ilayer][ixy])
				{
					cout << "No " << Form("h%d", irms) << " found: Exit... " << endl;
					exit_safe(EXIT_FAILURE);
				}
			}

		for(int ixy=0; ixy < NXY_ADAMO; ++ixy)
		{
			cc = new TCanvas(ixy==0? "pedestal_ADAMO_x" : "pedestal_ADAMO_y",ixy==0? "pedestal_ADAMO_x" : "pedestal_ADAMO_y", x,y,w,h);
			cc->Divide(2,5);

			for(int ilayer=0; ilayer < NLAYER_ADAMO; ++ilayer)
			{
				cc->cd(1+(2*ilayer));
				gPad->SetGridx();
				pedMeanADAMO[ilayer][ixy]->GetXaxis()->SetNdivisions(NCHIP_ADAMO, 0, 0, kFALSE);
				pedMeanADAMO[ilayer][ixy]->Draw();
				pedMeanADAMO[ilayer][ixy]->GetXaxis()->SetLabelSize(0.08);
				gPad->Update();
				pedMeanADAMO[ilayer][ixy]->GetYaxis()->SetLabelSize(0.08);
				gPad->Update();

				cc->cd(2+(2*ilayer));
				gPad->SetGridx();
				pedRMSADAMO[ilayer][ixy]->GetXaxis()->SetNdivisions(NCHIP_ADAMO, 0, 0, kFALSE);
				pedRMSADAMO[ilayer][ixy]->Draw();
				pedRMSADAMO[ilayer][ixy]->GetXaxis()->SetLabelSize(0.08);
				gPad->Update();
				pedRMSADAMO[ilayer][ixy]->GetYaxis()->SetLabelSize(0.08);
				gPad->Update();
			}
			cc->Update();
			if(save_histo)
				cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
			if(save_global)
				cg->Print(Form("%s/%s.pdf", dir, inputFileName));

			cc->WaitPrimitive();
			cc->Update();

			cc->Close();
		}

		gStyle->SetOptStat("neMuRo");
		gStyle->SetTitleFontSize();
#endif
	}

	if(show_tracker_hitmap)
	{
		gStyle->SetOptStat(0);

#ifdef AMS
		//Initialize AMS Tree
		TTree *ADtree = (TTree *)fCC->Get("t4");
		if(!ADtree)
		{
			cout << "No TTree \"t4\" found: Exit... " << endl;
			exit_safe(EXIT_FAILURE);
		}
		Event *ev;
		Cluster *cl;
		ev = new Event();
		ADtree->SetBranchAddress("cluster_branch", &ev);
#elif ADAMO
		//Initialize ADAMO Tree
		TTree *ADtree = (TTree *)fCC->Get("h99");
		if(!ADtree)
		{
			cout << "No TTree \"h99\" found: Exit... " << endl;
			exit_safe(EXIT_FAILURE);
		}
		Float_t ADAMO_Position[NLAYER_ADAMO][NXY_ADAMO];
		Float_t ADAMO_Signal[NLAYER_ADAMO][NXY_ADAMO];
		Float_t ADAMO_Cluster[NLAYER_ADAMO][NXY_ADAMO];
		for(int ilayer=0; ilayer < NLAYER_ADAMO; ++ilayer)
			for(int ixy=0; ixy < NXY_ADAMO; ++ixy)
			{
				ADtree->SetBranchAddress(Form("%s%d", ixy==0? "X":"Y", 1+ilayer) ,&ADAMO_Position[ilayer][ixy]);
				ADtree->SetBranchAddress(Form("S%s%d", ixy==0? "x":"y", 1+ilayer) ,&ADAMO_Signal[ilayer][ixy]);
				ADtree->SetBranchAddress(Form("Ncl%s%d", ixy==0? "x":"y", 1+ilayer) ,&ADAMO_Cluster[ilayer][ixy]);
			}
#endif

		int nEventADAMO = ADtree->GetEntries();

		//Initialize Calocube Tree
		TTree *CCtree;

		int nEventCalocube=(int)TMath::Power(10, 6);

		CCtree = (TTree *)fCC->Get("data");
		if(!CCtree)
		{
			cout << "No TTree \"data\" found: Exit... " << endl;
			exit_safe(EXIT_FAILURE);
		}

		ConstructTree(CCtree);

		nEventCalocube = CCtree->GetEntries();

		int nEvent = TMath::Min(nEventADAMO, nEventCalocube);

#ifdef AMS
		TH1F* occupancy_posS[NLAYER_AMS];
		TH1F* occupancy_posK[NLAYER_AMS];
		for (int il=0; il<NLAYER_AMS; il++) {
			occupancy_posS[il] = new TH1F(Form("occupancy_posS_%02d", il), Form("occupancy_posS_%02d", il), 2*NSTRIPSS, -NSTRIPSS*SPITCH, NSTRIPSS*SPITCH);
			occupancy_posK[il] = new TH1F(Form("occupancy_posK_%02d", il), Form("occupancy_posK_%02d", il), 2*NSTRIPSK, -NSTRIPSK*KPITCH, NSTRIPSK*KPITCH);
		}
		TH2F *hVertex = new TH2F("Vertex", "Vertex", 1000, -100, 100, 1000, -100, 100);
		TH2F *hPosCor[NXY];
		hPosCor[0] = new TH2F("x correlation", "x correlation",
				(int)((CsI_SIZE+CsI_GAP)*5./0.1), -2.5*(CsI_SIZE+CsI_GAP), +2.5*(CsI_SIZE+CsI_GAP), 1000, -100, 100);
		hPosCor[1] = new TH2F("y correlation", "y correlation",
				(int)((CsI_SIZE+CsI_GAP)*5./0.1), -2.5*(CsI_SIZE+CsI_GAP), +2.5*(CsI_SIZE+CsI_GAP), 1000, -100, 100);
#elif ADAMO
		TH2F *hVertex = new TH2F("Vertex", "Vertex", NSTRIP_ADAMO, 0., X_ADAMO, NSTRIP_ADAMO, 0., Y_ADAMO);
		TH2F *hPosCor[NXY];
		hPosCor[0] = new TH2F("x correlation", "x correlation",
				(int)((CsI_SIZE+CsI_GAP)*5./0.1), -2.5*(CsI_SIZE+CsI_GAP), +2.5*(CsI_SIZE+CsI_GAP), NSTRIP_ADAMO, 0., X_ADAMO);
		hPosCor[1] = new TH2F("y correlation", "y correlation",
				(int)((CsI_SIZE+CsI_GAP)*5./0.1), -2.5*(CsI_SIZE+CsI_GAP), +2.5*(CsI_SIZE+CsI_GAP), NSTRIP_ADAMO, 0., Y_ADAMO);
#endif

		for(int iev=0; iev<nEvent; iev++){
			double bar[NXY]={0.};

			if(CC_flag > -1)
			{
				bool goodEvent = false;

				CCtree->GetEntry(iev);
				//==================================
				// selection
				//==================================
				if(eventInfo.trigger!=PHYSICSTRIGGER)continue;
				if(eventInfo.good!=GOODEVENTFLAG)continue;

				if(CC_flag==NCUBES)
				{
#if defined(SPS2016) || defined(SPS2017Aug) || defined(SPS2017Nov)
					Hits hits;
					if(muon)
						hits = LMIP;
					else
						hits = LADC;

					if(hits.nHits>=0) goodEvent = true;

					if(CC_threshold_low >= 0. && CC_threshold_high >= 0. )
					{
						if ( CC_threshold_high <= CC_threshold_low )
						{
							cout << "Energy range not properly selected: Exit... " << endl;
							exit(1);
						}

						Double_t total = 0;

						for(int ihits=0; ihits<hits.nHits; ihits++)
							total += hits.hitSignal[ihits];

						if(total >= CC_threshold_low && total <= CC_threshold_high)
							goodEvent = true;
					}
#else
					if(nHits>=0) goodEvent = true;

					if(CC_threshold_low >= 0. && CC_threshold_high >= 0. )
					{
						if ( CC_threshold_high <= CC_threshold_low )
						{
							cout << "Energy range not properly selected: Exit... " << endl;
							exit(1);
						}

						Double_t total = 0;

						for(int ihits=0; ihits<nHits; ihits++)
							total += hitSignal[ihits];

						if(total >= CC_threshold_low && total <= CC_threshold_high)
							goodEvent = true;
					}
#endif
				}
				else if(CC_flag>-1)
				{
					if((CC_threshold_low < 0. && CC_threshold_high < 0. )
							|| ( CC_threshold_high <= CC_threshold_low ))
					{
						cout << "Energy range not properly selected: Exit... " << endl;
						exit(1);
					}

#if defined(SPS2016) || defined(SPS2017Aug) || defined(SPS2017Nov)
					Hits hits;
					if(muon)
						hits = LMIP;
					else
						hits = LADC;

					for(int ihits=0; ihits<hits.nHits; ihits++){
						const int hitLayer = hits.hitLayer[ihits];
						const int hitCube = hits.hitRow[ihits]*NCOLS + hits.hitCol[ihits];
						const double hitSig = hits.hitSignal[ihits];

						if(CC_layer > -1.)
						{
							if(hitCube == CC_flag && hitSig >= CC_threshold_low && hitSig < CC_threshold_high && hitLayer == CC_layer)
								goodEvent = true;
						}
						else
						{
							if(hitCube == CC_flag && hitSig >= CC_threshold_low && hitSig < CC_threshold_high)
								goodEvent = true;
						}
					}
#else
					for(int ihits=0; ihits<nHits; ihits++){
						const int hitLayer = hitLayer[ihits];
						const int hitCube = hitRow[ihits]*NCOLS + hitCol[ihits];
						const double hitSig = hitSignal[ihits];

						if(CC_layer > -1.)
						{
							if(hitCube == CC_flag && hitSig >= CC_threshold_low && hitSig < CC_threshold_high && hitLayer == CC_layer)
								goodEvent = true;
						}
						else
						{
							if(hitCube == CC_flag && hitSig >= CC_threshold_low && hitSig < CC_threshold_high)
								goodEvent = true;
						}
					}
#endif
				}
				else //if no selection is applied compare centroids
				{
					double dE_x[ncols] = {0.};
					double dE_y[nrows] = {0.};

					Hits hits;
					if(muon)
						hits = small_large? SMIP : LMIP;
					else
						hits = small_large? SADC : LADC;

					double threshold;
					if(muon)
						threshold = 1.;
					else
						threshold = small_large? 1000. : 250.;

					for(int ih=0; ih<hits.nHits; ih++) {
						float hitSignal = hits.hitSignal[ih];
						if(hitSignal>threshold) {
							dE_x[hits.hitCol[ih]] += hitSignal;
							dE_y[hits.hitRow[ih]] += hitSignal;
						}
					}

					goodEvent = true;

					double w;

					double x=0.;
					w=0.;
					for(int icol=0; icol<ncols; ++icol) {
						double pos = (CsI_SIZE * (((double)icol) + 0.5)) + (CsI_GAP * ((double)icol)); //Origin in bottom left corner
						pos -= (2.5*CsI_SIZE) + (2.*CsI_GAP); //Origin in central cube
						x += dE_x[icol] * pos;
						w += dE_x[icol];
					}
					x /= w;
					bar[0] = x;

					if(w==0. || x==0.) goodEvent = false;

					double  y=0.;
					w=0.;
					for(int irow=0; irow<nrows; ++irow) {
						double pos = (CsI_SIZE * (nrows - (((double)irow) + 0.5))) + (CsI_GAP * (nrows-1. - (double)irow)); //Origin in bottom left corner
						pos -= (2.5*CsI_SIZE) + (2.*CsI_GAP); //Origin in central cube
						y += dE_y[irow] * pos;
						w += dE_y[irow];
					}
					y /= w;
					bar[1] = y;

					if(w==0. || y==0.) goodEvent = false;
				}

				if(!goodEvent) continue;
			}

			ADtree->GetEntry(iev);
#ifdef AMS
			int NClusTot = ev->GetNClusTot();

			double pos[NXY_AMS][NLAYER_AMS]={{0.}};
			for (int ic=0; ic<NClusTot; ic++) {
				cl = ev->GetCluster(ic);

				int ladder = cl->ladder;
				int side = cl->side;

				if (side==0) {
					occupancy_posS[ladder]->Fill(cl->GetCoG());
					pos[0][ladder] = cl->GetAlignedPosition();
				} else {
					occupancy_posK[ladder]->Fill(cl->GetCoG());
					pos[1][ladder] = cl->GetAlignedPosition();
				}
			}

			for (int il=0; il<NLAYER_AMS; il++)
				if(pos[0][il]!=0. && pos[1][il]!=0.) {
					hVertex->Fill(pos[0][il], pos[1][il]);
					for(int ixy=0; ixy < NXY; ++ixy)
						hPosCor[ixy]->Fill(bar[ixy], pos[ixy][il]);
				}
#elif ADAMO
			int maxdep = -TMath::Power(10, 6);
			double pos[NXY_ADAMO] = {0.};
			for(int ilayer=0; ilayer < NLAYER_ADAMO; ++ilayer)
				if((ADAMO_Cluster[ilayer][0]==1) && (ADAMO_Cluster[ilayer][1]==1))
				{
					const double dep = ADAMO_Signal[ilayer][0]+ADAMO_Signal[ilayer][1];
					if(dep > maxdep)
					{
						maxdep = dep;
						pos[0] = ADAMO_Position[ilayer][0]*XSTRIP_ADAMO;
						pos[1] = Y_ADAMO-(ADAMO_Position[ilayer][1]*YSTRIP_ADAMO);
					}
				}

			if (maxdep < 0) continue;
			hVertex->Fill(pos[0], pos[1]);
			for(int ixy=0; ixy < NXY; ++ixy)
				hPosCor[ixy]->Fill(bar[ixy], pos[ixy]);
#endif
		}

		cc = new TCanvas("Hitmap", "Hitmap", (int)(x+w/3.),y,(int)(w/3.),h);
		cc->cd();

#ifdef AMS
		hVertex->Rebin2D(1, 1);
#elif ADAMO
		hVertex->Rebin2D(8, 8);
#endif
		hVertex->Draw("COLZ");
		hVertex->GetXaxis()->SetTitle("x [mm]");
		hVertex->GetYaxis()->SetTitle("y [mm]");
		hVertex->Draw("COLZ");

		cc->Update();
		if(save_histo)
			cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
		if(save_global)
			cg->Print(Form("%s/%s.pdf", dir, inputFileName));

		cc->WaitPrimitive();
		cc->Update();

		cc->Close();

		cc = new TCanvas("Correlation", "Correlation", x,y,w,h);
		cc->Divide(2);
		for(int ixy=0; ixy < NXY; ++ixy) {
			cc->cd(ixy);
			hPosCor[ixy]->Draw("COLZ");
			hPosCor[ixy]->GetXaxis()->SetTitle(Form("%s_{Calocube} [cm]", ixy==0? "x" : "y"));
			hPosCor[ixy]->GetYaxis()->SetTitle(Form("%s_{Tracker} [cm]", ixy==0? "x" : "y"));
			hPosCor[ixy]->Draw("COLZ");
		}

		cc->Update();
		if(save_histo)
			cc->Print(TString(dir) + "/" + TString(cc->GetName()) + ".eps");
		if(save_global)
			cg->Print(Form("%s/%s.pdf", dir, inputFileName));

		cc->WaitPrimitive();
		cc->Update();

		cc->Close();

		gStyle->SetOptStat("neMuRo");

		if(CCtree) delete CCtree;
	}
#endif

	if(save_global)
		cg->Print(Form("%s/%s.pdf]", dir, inputFileName));
	if(cc) cc->Close();
	if(cg) cg->Close();

	fCC->Close();

	return 0;
}
