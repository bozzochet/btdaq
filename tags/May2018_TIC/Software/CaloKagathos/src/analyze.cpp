/*
 * Noise.cpp
 *
 *  Created on: 14/gen/2013
 *      Author: mori
 */
#include <iostream>
#include <iomanip>
#include <new>

#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include <fstream>

#include "constants.h"
#include "functions.h"
#include "structures.h"
#include "IOfile.h"

using namespace std;

//#define CALIBRATION
//define PEDESTAL

// Open input file and perform some checks
IOfile *OpenIOfile(const char *fileName) {
	IOfile *iof = new IOfile;
	iof->OpenFile(fileName, "r");
	int n = iof->ReadRunHeader();
	if (n == -1) {
		cout << "[OpenIOfile] Wrong Run Header!!! Exit." << endl;
		//   exit(1);
		exit(EXIT_FAILURE);
	}
	return iof;
}

// Constant control parameters
const int nMinPulserEvents=50;
const double nMinPulserFraction=0.5;
const char *logfilename="./analysed_files/logfile.dat";

// Command line parameters
const char *calibFileName="";
bool calibRun= true;
const char *inputFileName, *outputFileName;
int nIters = 3; //number of itaration for perdestals, CN and sigma
bool verbose = false;

void HandleInputPar(int argc, char **argv) {

	for (int iPar = 1; iPar < argc; iPar++) {
		char *arg = argv[iPar];
		if (iPar == argc - 2)
			inputFileName = arg;
		if (iPar == argc - 1)
			outputFileName = arg;

		if (!strcmp(arg, "-c")) {
			calibRun = false;
			calibFileName = argv[++iPar];
			cout << calibFileName <<endl;
		}
		if (!strcmp(arg, "-n")) {
			if (calibRun) {
				nIters = atoi(argv[++iPar]);
				if (nIters == 0) {
					cout << "Warning: bad number of iterations. Exit.\n";
					exit(1);
				}
			}
		}
		if (!strcmp(arg, "-v"))
			verbose = true;
	}
}

/* Program to compute noise levels and common noise. */
/* Options
 */
int main(int argc, char **argv) {

	/* ********* Handle CLI arguments ********* */
	if (argc < 3) {
		cout << "Usage:\n"
				"   analyse <options> inputfile outputfile\n\n"
				"Options:\n"
				"     -c <file.root>    Read pedestals and sigma from file.root \n"
				"     -n <iters>        Set the number of iterations (default: <<"<<nIters<<"<<)\n"
				"     -v                Be verbose"<<endl;

		return 0;
	}
	HandleInputPar(argc, argv);

	const int str_len = strlen(inputFileName);
	bool isCalibration = (str_len > 4 && !strcmp(inputFileName + str_len - 4, ".ped"))?
			true : false;

	// Prepare data structure/
	//	double ped[NCASISCHIPS][NCASISCHANNELS]; // Pedestals
	//	double oldPed[NCASISCHIPS][NCASISCHANNELS]; // Pedestals
	//	float sigma[NCASISCHIPS][NCASISCHANNELS]; // Noise matrix
	short int gain[NCASISCHIPS][NCASISCHANNELS]; // Gain
	for (int iCasis = 0; iCasis < NCASISCHIPS; iCasis++) {
		for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS; iCasisChannel++) {
			//			oldPed[iCasis][iCasisChannel] = 0.;
			//			ped[iCasis][iCasisChannel] = 0.;
			//			sigma[iCasis][iCasisChannel] = 1e5;
			gain[iCasis][iCasisChannel] = -1;
		}
	}

	// ---------------------------
	// Prepare output file
	// ---------------------------

	CommonNoise* cNoise = new CommonNoise ;
	EventInfo* eventInfo = new EventInfo;
	//	Int_t EvNum;
#if defined(ADAMO) || defined(AMS)
	UInt_t seconds, subseconds;
#endif
	int casisTime;
	float raw[NCASISCHIPS][NCASISCHANNELS]; // raw signal matrix
	float signal[NCASISCHIPS][NCASISCHANNELS]; // Signal matrix in canali
	float signal_cn[NCASISCHIPS][NCASISCHANNELS]; // Signal matrix
	float signal_f[NCASISCHIPS][NCASISCHANNELS]; // Signal matrix (forced low gain)

	TFile *outFile = NULL;
	outFile = new TFile(outputFileName, "RECREATE");cout << "Creating  output file "<<outputFileName<<endl;

	TTree *outTree = new TTree("data", "Data from crystals");
	outTree->Branch("eventInfo", &(eventInfo->trigger), "trigger/I");
	outTree->Branch("eventFlag", &(eventInfo->good), "flag/I");
#if defined(ADAMO) || defined(AMS)
	outTree->Branch("seconds", &seconds, "seconds/i");
	outTree->Branch("subseconds", &subseconds, "subseconds/i");
#endif
	outTree->Branch("casisTime", &casisTime, "casisTime/I");
	outTree->Branch("gainBranch", &gain, Form("gain[%d][%d]/S", NCASISCHIPS, NCASISCHANNELS));
	outTree->Branch("rawBranch", &raw, Form("raw[%d][%d]/F", NCASISCHIPS, NCASISCHANNELS));
	outTree->Branch("signalBranch", &signal, Form("adc[%d][%d]/F", NCASISCHIPS, NCASISCHANNELS));
	outTree->Branch("signalfBranch", &signal_f, Form("adc_f[%d][%d]/F", NCASISCHIPS, NCASISCHANNELS));
	outTree->Branch("signalCNBranch", &signal_cn, Form("adc_cn[%d][%d]/F", NCASISCHIPS, NCASISCHANNELS));
	outTree->Branch("cnBranch", &(cNoise->commonNoise), Form("commonNoise[%d]/F:nChannels[%d]/I:disc[%d]/D:CN[%d]/D:commonNoiseLarge[%d]/D:nChannelsLarge[%d]/I:commonNoiseSmall[%d]/D:nChannelsSmall[%d]/I:CNLarge[%d]/D:CNSmall[%d]/D", NCASISCHIPS4CN, NCASISCHIPS4CN, NCASISCHIPS4CN, NCASISCHIPS4CN, NCASISCHIPS4CN, NCASISCHIPS4CN, NCASISCHIPS4CN, NCASISCHIPS4CN, NCASISCHIPS4CN, NCASISCHIPS4CN));

	Hits LADC;
	LADC.Reset();
	outTree->Branch("LADC.nHits",&LADC.nHits,"nHits/I");
	outTree->Branch("LADC.hitRow",LADC.hitRow,"hitRow[LADC.nHits]/I");
	outTree->Branch("LADC.hitCol",LADC.hitCol,"hitCol[LADC.nHits]/I");
	outTree->Branch("LADC.hitLayer",LADC.hitLayer,"hitLayer[LADC.nHits]/I");
	outTree->Branch("LADC.hitGain",LADC.hitGain,"hitGain[LADC.nHits]/I");
	outTree->Branch("LADC.hitSat",LADC.hitSat,"hitSat[LADC.nHits]/I");
	outTree->Branch("LADC.hitSignal",LADC.hitSignal,"hitSignal[LADC.nHits]/F");
	Hits SADC;
	SADC.Reset();
	outTree->Branch("SADC.nHits",&SADC.nHits,"nHits/I");
	outTree->Branch("SADC.hitRow",SADC.hitRow,"hitRow[SADC.nHits]/I");
	outTree->Branch("SADC.hitCol",SADC.hitCol,"hitCol[SADC.nHits]/I");
	outTree->Branch("SADC.hitLayer",SADC.hitLayer,"hitLayer[SADC.nHits]/I");
	outTree->Branch("SADC.hitGain",SADC.hitGain,"hitGain[SADC.nHits]/I");
	outTree->Branch("SADC.hitSat",SADC.hitSat,"hitSat[SADC.nHits]/I");
	outTree->Branch("SADC.hitSignal",SADC.hitSignal,"hitSignal[SADC.nHits]/F");

	float tCorr;
	outTree->Branch("tCorr", &tCorr, "tCorr/F");

	Hits LMIP;
	LMIP.Reset();
	Hits SMIP;
	SMIP.Reset();
#ifndef CALIBRATION
	outTree->Branch("LMIP.nHits",&LMIP.nHits,"nHits/I");
	outTree->Branch("LMIP.hitRow",LMIP.hitRow,"hitRow[LMIP.nHits]/I");
	outTree->Branch("LMIP.hitCol",LMIP.hitCol,"hitCol[LMIP.nHits]/I");
	outTree->Branch("LMIP.hitLayer",LMIP.hitLayer,"hitLayer[LMIP.nHits]/I");
	outTree->Branch("LMIP.hitGain",LMIP.hitGain,"hitGain[LMIP.nHits]/I");
	outTree->Branch("LMIP.hitSat",LMIP.hitSat,"hitSat[LMIP.nHits]/I");
	outTree->Branch("LMIP.hitSignal",LMIP.hitSignal,"hitSignal[LMIP.nHits]/F");
	//
	outTree->Branch("SMIP.nHits",&SMIP.nHits,"nHits/I");
	outTree->Branch("SMIP.hitRow",SMIP.hitRow,"hitRow[SMIP.nHits]/I");
	outTree->Branch("SMIP.hitCol",SMIP.hitCol,"hitCol[SMIP.nHits]/I");
	outTree->Branch("SMIP.hitLayer",SMIP.hitLayer,"hitLayer[SMIP.nHits]/I");
	outTree->Branch("SMIP.hitGain",SMIP.hitGain,"hitGain[SMIP.nHits]/I");
	outTree->Branch("SMIP.hitSat",SMIP.hitSat,"hitSat[SMIP.nHits]/I");
	outTree->Branch("SMIP.hitSignal",SMIP.hitSignal,"hitSignal[SMIP.nHits]/F");
#endif

	Shower LShower;
	LShower.Reset();
	Shower SShower;
	SShower.Reset();
#ifndef CALIBRATION
	outTree->Branch("LShower.totSignal",  &LShower.totSignal, "totSignal/F");
	outTree->Branch("LShower.layerStart", &LShower.layerStart,"layerStart/I" );
	outTree->Branch("LShower.layerHitMax", &LShower.layerHitMax,"layerHitMax/I" );
	outTree->Branch("LShower.cubeHitMax", &LShower.cubeHitMax ,"cubeHitMax/I");
	outTree->Branch("LShower.signalHitMax",&LShower.signalHitMax,"signalHitMax/I" );
	outTree->Branch("LShower.nHitsLayer", LShower.nHitsLayer,Form("nHitsLayer[%d]/I",NLAYERS));
	outTree->Branch("LShower.signalLayer", LShower.signalLayer,Form("signalLayer[%d]/F",NLAYERS));
	outTree->Branch("LShower.signalHitM",LShower.signalHitM,Form("signalHitM[%d]/F",NLAYERS) );
	outTree->Branch("LShower.signalHitC",LShower.signalHitC,Form("signalHitC[%d]/F",NLAYERS) );
	//
	outTree->Branch("SShower.totSignal",  &SShower.totSignal, "totSignal/F");
	outTree->Branch("SShower.layerStart", &SShower.layerStart,"layerStart/I" );
	outTree->Branch("SShower.layerHitMax", &SShower.layerHitMax,"layerHitMax/I" );
	outTree->Branch("SShower.cubeHitMax", &SShower.cubeHitMax ,"cubeHitMax/I");
	outTree->Branch("SShower.signalHitMax",&SShower.signalHitMax,"signalHitMax/I" );
	outTree->Branch("SShower.nHitsLayer", SShower.nHitsLayer,Form("nHitsLayer[%d]/I",NLAYERS));
	outTree->Branch("SShower.signalLayer", SShower.signalLayer,Form("signalLayer[%d]/F",NLAYERS));
	outTree->Branch("SShower.signalHitM",SShower.signalHitM,Form("signalHitM[%d]/F",NLAYERS) );
	outTree->Branch("SShower.signalHitC",SShower.signalHitC,Form("signalHitC[%d]/F",NLAYERS) );
#endif

	/////////////////////////////////////////////////////////////////////
	///
	/// Read ADC-t-MIP conversion parameters
	///
	/////////////////////////////////////////////////////////////////////
#if defined(SPS2017Aug) || defined(SPS2017Nov) || defined(TIC)
	TString filePar = "./tables/ADC2MIP-Muons50-SPS2017.dat";
	TString filePar2 = "./tables/L2SHistos_eALL.root-parameters-final-2017";//large/small
#else
	TString filePar = "./tables/ADC2MIP-Muons150-SPS2016.dat";
	TString filePar2 = "./tables/L2SHistos_eALL.root-parameters-final-2016";//large/small
#endif
	double MIP2ADCNorm[NROWS][NCOLS][NLAYERS]; //NB: Eugenio changed ADC2MIP into MIP2ADC in order to mask dead channels
	double S2LNorm[NROWS][NCOLS][NLAYERS]; //NB: As descripted before a "0" in the table means dead channel
	int CalFlag[NROWS][NCOLS][NLAYERS];
	for(int ir=0; ir<NROWS; ir++){
		for(int ic=0; ic<NCOLS; ic++){
			for(int il=0; il<NLAYERS; il++){
#if defined(SPS2017Aug) || defined(SPS2017Nov) || defined(TIC)
				MIP2ADCNorm[ir][ic][il]= 1./551.6;
				S2LNorm[ir][ic][il]=97.7;
				CalFlag[ir][ic][il]=0;
#else
				MIP2ADCNorm[ir][ic][il]= 1./604.6;
				S2LNorm[ir][ic][il]=109.6;
				CalFlag[ir][ic][il]=0;
#endif
			};
		};
	};
	TF1* fCorr = NULL;

	if(!filePar.IsNull()){
		fstream filestr (filePar.Data(), fstream::in );
		if(!filestr.is_open()) {
			cout << " Calibration file "<<filePar<<" not found"<<endl;
			return 0;
		}else{
			cout << endl;
			cout << " (((( Reading ADC2MIP calibration parameters ))))"<<endl;
			cout << filePar <<endl;
			cout << endl;
		}
		while(filestr.is_open()){
			char line[200];
			for(int i=0 ; i<10; i++)filestr.getline(line,200); //skip first 10 lines
			for(;;){
				int layer,cube;
				float peak,err;
				filestr >> layer >> cube >> peak >>err;
				cout << layer << " "<<cube<<" "<<peak<<endl;
				int row = (int)(cube/NCOLS);
				int col = cube%NCOLS;
				if(peak>0) {
					MIP2ADCNorm[row][col][layer]=1./peak;
					CalFlag[row][col][layer]+=1;
				} else if(peak==0) {
					MIP2ADCNorm[row][col][layer]=0.;
					CalFlag[row][col][layer]+=1;
				}

				if(!filestr.good())break;
			}
			filestr.close();
		}
		// timing
		double p[6];
		int np=-1;
		fstream filestr_t( (filePar+"-timing").Data(), fstream::in );
		if(!filestr_t.is_open()) {
			cout << " Timing info for file "<<filePar<<" not found"<<endl;
		}else{
			cout << endl;
			cout << " (((( Reading timing-correction  parameters ))))"<<endl;
			cout <<  (filePar+"-timing").Data()<<endl;
			cout << endl;
		}
		while(filestr_t.is_open() && !filestr_t.eof()){
			np++;
			filestr_t >> *(p+np) ;
			cout <<np<<" -- "<<*(p+np)<<endl;
		}
		filestr_t.close();
		TF1* ff = NULL;
		cout << "N.parameters "<<np<<endl;
		if(np==3){
			ff = new TF1("f","[0]*(1+[1]*TMath::Exp(TMath::Min(x,2000.)/[2]))",0.,2200.);
			cout << ff->GetExpFormula()<<endl;
			for(int ipa =0; ipa<np; ipa++)ff->SetParameter(ipa,p[ipa]);
			fCorr = ff;
		}else if(np==6){
			ff = new TF1("f","[0]*(1+[1]*TMath::Exp(TMath::Min(x,2000.)/[2]))+[3]*(1+[4]*TMath::Exp(TMath::Min(x,2000.)/[5]))",0.,2200.);
			cout << ff->GetExpFormula()<<endl;
			for(int ipa =0; ipa<np; ipa++)ff->SetParameter(ipa,p[ipa]);
			fCorr = ff;
		}else{
			cout << " ?????? "<<endl;
		}
	}
	if(!filePar2.IsNull()){
		fstream filestr2(filePar2.Data(), fstream::in );
		if(!filestr2.is_open()) {
			cout << " Calibration file "<<filePar2<<" not found"<<endl;
			return 0;
		}else{
			cout << endl;
			cout << " (((( Reading S2L calibration parameters ))))"<<endl;
			cout << filePar2 <<endl;
			cout << endl;
		}
		while(filestr2.is_open()){
			char line[200];
			for(int i=0 ; i<10; i++)filestr2.getline(line,200); //skip first 10 lines
			for(;;){
				int layer,cube;
				float val,err;
				filestr2 >> layer >> cube >> val >>err;
				cout << layer << " "<<cube<<" "<<val<<endl;
				int row = (int)(cube/NCOLS);
				int col = cube%NCOLS;
				if(val>0) {
					S2LNorm[row][col][layer]=val;
					CalFlag[row][col][layer]+=2;
				} else if(val==0) {
					S2LNorm[row][col][layer]=0.;
					CalFlag[row][col][layer]+=2;
				}

				if(!filestr2.good())break;
			}
			filestr2.close();
		}
	}

	//+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
	//  PED-SIG-ALPHA calibration
	//+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
	calibParam* param = new calibParam;

	// ----------------------------------------------------------------------------------------------------------------
	// Prepare PED-SIG-ALPHA calibration output
	// ----------------------calibparam------------------------------------------------------------------------------------------
	TH1F* sigmaHisto1D_cube = new TH1F("sigmaHisto1DCube", "Sigma (1D histogram) instrumented", NTOTCASISCHANNELS, 0, NTOTCASISCHANNELS);
	TH1F* pedHisto1D_raw = new TH1F("pedHisto1DRaw", "Pedestals (1D histogram) raw", NTOTCASISCHANNELS, 0, NTOTCASISCHANNELS);
	TH1F* sigmaHisto1D_raw = new TH1F("sigmaHisto1DRaw", "Sigma (1D histogram) raw", NTOTCASISCHANNELS, 0, NTOTCASISCHANNELS);
	TH1F* pedHisto1D = new TH1F("pedHisto1D", "Pedestals (1D histogram)", NTOTCASISCHANNELS, 0, NTOTCASISCHANNELS);
	TH1F* sigmaHisto1D = new TH1F("sigmaHisto1D", "Sigma (1D histogram)", NTOTCASISCHANNELS, 0, NTOTCASISCHANNELS);
	TH1F* pedHisto1D_true = new TH1F("pedHisto1DTrue", "Pedestals (1D histogram) true", NTOTCASISCHANNELS, 0, NTOTCASISCHANNELS);
	TH1F* sigmaHisto1D_true = new TH1F("sigmaHisto1DTrue", "Sigma (1D histogram) true", NTOTCASISCHANNELS, 0, NTOTCASISCHANNELS);

	TH1F* alphaLargeHisto1D = new TH1F("alphaLargeHisto1D", "Alpha large (1D histogram)", NCASISCHIPS4CN, 0, NCASISCHIPS4CN);
	TH1F* alphaSmallHisto1D = new TH1F("alphaSmallHisto1D", "Alpha small (1D histogram)", NCASISCHIPS4CN, 0, NCASISCHIPS4CN);

	// histo for CN estimation
	//	TH1F *commonNoiseHisto[NCASISCHIPS4CN];
	TH2F *hrho_0[NCASISCHIPS4CN];
	TH2F *hrho[NCASISCHIPS4CN];
	TGraphErrors *gAlphaLarge[NCASISCHIPS4CN];
	TGraphErrors *gAlphaSmall[NCASISCHIPS4CN];
	TF1 *fAlphaLarge[NCASISCHIPS4CN];
	TF1 *fAlphaSmall[NCASISCHIPS4CN];
	for (int iHisto = 0; iHisto < NCASISCHIPS4CN; iHisto++) {
		//commonNoiseHisto[iHisto] = new TH1F(Form("commonNoiseHisto_%d", iHisto), "Common noise (instrumented channels) histogram", 500, -1000, 1000);
		hrho_0[iHisto] = new TH2F(Form("rho_0_%d", iHisto), "Correlation coeff.           ",NCASISCHANNELS4CN,-0.5,NCASISCHANNELS4CN-0.5,NCASISCHANNELS4CN,-0.5,NCASISCHANNELS4CN-0.5);
		hrho[iHisto]   = new TH2F(Form("rho_%d",   iHisto), "Correlation coeff. (residual)",NCASISCHANNELS4CN,-0.5,NCASISCHANNELS4CN-0.5,NCASISCHANNELS4CN,-0.5,NCASISCHANNELS4CN-0.5);
		gAlphaLarge[iHisto] = NULL; //new TGraph(); gAlphaLarge[iHisto]->SetName(Form("gAlphaLarge_%d", iHisto));
		gAlphaSmall[iHisto] = NULL; //new TGraph(); gAlphaSmall[iHisto]->SetName(Form("gAlphaSmall_%d", iHisto));
		fAlphaLarge[iHisto] = new TF1(Form("fAlphaLarge_%d", iHisto),"x*[0]",-1000,1000);
		fAlphaSmall[iHisto] = new TF1(Form("fAlphaSmall_%d", iHisto),"x*[0]",-1000,1000);
		fAlphaLarge[iHisto]->SetParameter(0,2.);
		fAlphaSmall[iHisto]->SetParameter(0,1.);
#if defined(SPS2017Aug) || defined(SPS2017Nov) || defined(TIC)
		if(iHisto==49) { //Chip 24 CNb broken
			fAlphaLarge[iHisto]->SetParameter(0,-2.);
			fAlphaSmall[iHisto]->SetParameter(0,-1.);
		}
#endif
	}

	// Begin iterative pedestals, common noise and sigma estimation
	// Algorithm:

	calibParam* paramTemp = new calibParam; //temporary
	param->Reset();
	paramTemp->Reset();
	//	double cnRMS[NCASISCHIPS4CN];
	//	double cnAV[NCASISCHIPS4CN];
	//	int    cnEvents[NCASISCHIPS4CN];
	//	double cnRMSprev[NCASISCHIPS4CN];
	//	for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++) {
	//		cnRMS[iCasis] = 0.;
	//		cnAV[iCasis]  = 0.;
	//		cnEvents[iCasis]  = 0;
	//		cnRMSprev[iCasis] = 0.;
	//	}

	double cov[NCASISCHIPS4CN][NCASISCHANNELS4CN][NCASISCHANNELS4CN];//covariance matrix
	double rho[NCASISCHIPS4CN][NCASISCHANNELS4CN][NCASISCHANNELS4CN];//correlation matrix
	double ave[NCASISCHIPS4CN][NCASISCHANNELS4CN];//average (pedestal)

	if (calibRun) {
		for (int iIter = 0; iIter < nIters; iIter++) {

			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			// loop over pulser events for pedestal estimation
			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

			//Si una il loop sui file di impulsatore per vedere se e come sono
			//correlati i canali dell'elettronica (fare la calibrazione)
			//non si puo' usare gli eventi di impulsatore per trovare il
			//common noise (da sottrarre poi al segnale) perche' esso cambia
			//evento per evento!!!
			//Per la stima del common noise si deve trovare il giusto algoritmo
			//e va calcolato e sottratto evento per evento! --> vedi loop sugli eventi (dopo)
			//Qui viene utilizzato sempre lo stesso meccanismo perche' e' un retaggio di pamela:
			//nel tracciatore si trova bene il CN evento per evento dalle strisce che
			//non sono state colpite, ma questo non funziona col calorimetro visto che
			//in generale sara' completamente illuminato in caso di segnale.


			//Al TB SPS2016Sep ci sono stati dei problemi con il rumore e in lab
			//s'e' visto che i due lati della scolopendra sono anticorrelati per
			//il rumore quindi in questo caso bisogna fare il calcolo del CN
			//separatamente per lato A e lato B.
			//Per fare questo si considera un numero doppio di chip (da 54 diventano 108)
			//e per ogni chip si considerano la meta' dei canali (da 28 diventano 14).


			cout << " **** Begin iteration " << iIter << endl;

			cout << "Open input file... "<< inputFileName <<endl;
			IOfile *inputFile = OpenIOfile(inputFileName);
			cout << "...reset variables..."<<endl;

			// --------------------------------
			// Reset buffers for this iteration
			// --------------------------------
			//unsigned int nPedEvents[NCASISCHIPS][NCASISCHANNELS];
			for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++){

				//int iHydra = iCasis/2;

				//				cnRMS[iCasis] = 0.;
				//				cnAV[iCasis]  = 0.;
				//				cnEvents[iCasis]  = 0;
				for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS4CN; iCasisChannel++){

					//int iHydraChannel = iCasisChannel+(14*(iCasis%2));

					ave[iCasis][iCasisChannel] = 0.;
					for (int jCasisChannel = 0; jCasisChannel <= iCasisChannel; jCasisChannel++){
						cov[iCasis][iCasisChannel][jCasisChannel] = 0.;
						rho[iCasis][iCasisChannel][jCasisChannel] = 0.;
					}
				}

				if(gAlphaLarge[iCasis])gAlphaLarge[iCasis]->Delete();
				if(gAlphaSmall[iCasis])gAlphaSmall[iCasis]->Delete();
				gAlphaLarge[iCasis] = new TGraphErrors(0); gAlphaLarge[iCasis]->SetName(Form("gAlphaLarge_%d", iCasis));
				gAlphaSmall[iCasis] = new TGraphErrors(0); gAlphaSmall[iCasis]->SetName(Form("gAlphaSmall_%d", iCasis));
			}
			param->Reset();
			cNoise->Reset();

			// ----------------------------------------
			// Start event loop for pedestal estimation
			// ----------------------------------------
			unsigned int i = 0;//check counter
			int pulserEvents = 0;//pulser counter
			int pulserLowGainEvents = 0;//pulser low gain counter
			int pulserNullSigEvents = 0;//pulser null signal counter
			cout << "...loop over events (PULSER)..."<<endl;
			while (true) {
				/////////////////////////////////////////////// Read buffer
#if defined(ADAMO) || defined(AMS)
				int m = inputFile->ReadEventHeader();
				if (m == -1)
					break;

				//		        struct VME_Header header = inputFile->GetEventHeader();
				//		        cout << header.Flag << endl;
				//		        cout << header.EvNum << endl;
				//		        cout << header.Sec << endl;
				//		        cout << header.nSec << endl;
#endif
				int n = inputFile->ReadEvent();
				if (n == -2) {
					cout << "Event corrupted. Skipping" << endl;
					continue;
				}
				else if (n == -1)
					break;
				++i; //increment check counter
				///////////////////////////////////////////////

#if defined(SPS2017Aug) || defined(SPS2017Nov) || defined(TIC)
				if (inputFile->GetLACTrigger() != RANDOMTRIGGER || isCalibration) //32-64
					continue;
				//				if (inputFile->GetCCTrigger() != GOODEVENTFLAG) //2-4
				//					continue;
#else
				if ((inputFile->GetCCTrigger()&0xf0) != RANDOMTRIGGER || isCalibration) //32-64
					continue;
				//				if ((inputFile->GetCCTrigger()&0x0f) != GOODEVENTFLAG) //2-4
				//					continue;
#endif

				pulserEvents++; //pulser counter

				// -------------------------------------------------
				// common noise evaluation
				// -------------------------------------------------
				if(iIter>0) {

					cNoise->Compute(inputFile,paramTemp,250.);

					for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++) {
						int nInstLarge = 0;
						int nInstSmall = 0;
						for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS4CN; iCasisChannel++) {
							int iHydra = iCasis/2;
							int iHydraChannel = iCasisChannel+(14*(iCasis%2));
							int sensor = GetSensor(iHydra,iHydraChannel);

							if(!GoodChannel(iHydra, iHydraChannel)) continue;
							if(sensor==0)nInstLarge++;
							if(sensor==1)nInstSmall++;

						}
						int nGL = gAlphaLarge[iCasis]->GetN();
						int nGS = gAlphaSmall[iCasis]->GetN();
						if(cNoise->nChannelsLarge[iCasis]==nInstLarge)gAlphaLarge[iCasis]->SetPoint( nGL, cNoise->CN[iCasis], cNoise->commonNoiseLarge[iCasis]);
						if(cNoise->nChannelsLarge[iCasis]==nInstLarge)gAlphaLarge[iCasis]->SetPointError( nGL, 0., 0.);
						if(cNoise->nChannelsSmall[iCasis]==nInstSmall)gAlphaSmall[iCasis]->SetPoint( nGS, cNoise->CN[iCasis], cNoise->commonNoiseSmall[iCasis]);
						if(cNoise->nChannelsSmall[iCasis]==nInstSmall)gAlphaSmall[iCasis]->SetPointError( nGS, 0., 0.);
					}

				}

				// -------------------------------------------------
				// increment PED-SIG and COV counters
				// -------------------------------------------------
				bool low_gain=false;
				bool null_sig=false;
				for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++) {

					for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS4CN; iCasisChannel++) {

						int iHydra = iCasis/2;
						int iHydraChannel = iCasisChannel+(14*(iCasis%2));

						int adc = inputFile->GetCCUBEdata(iHydra, iHydraChannel);
#ifdef PEDESTAL
						int gain = 0;
#else
						int gain = inputFile->GetCCGain(iHydra, iHydraChannel);
#endif

						if(gain != 0 && iHydra < 45)
							if(!(iHydra == 24 && iHydraChannel == 20)) //CNb broken on this chip
							{
								cout << " Detected low gain in pedestal computation on chip " << iHydra << " channel " << iHydraChannel << endl;
								low_gain=true;
							}

						if(adc == 0 && iHydra < 45)
							if(!(iHydra == 24 && iHydraChannel == 20)) //CNb broken on this chip
							{
								cout << " Detected null signal in pedestal computation on chip " << iHydra << " channel " << iHydraChannel << endl;
								null_sig=true;
							}

						int model = 1;
						if(iIter>1)model = 2;

						double cn = cNoise->GetCN(iCasis,iCasisChannel,model);

						if(iIter ==0 && cn != 0.) {
							ofstream logfile(logfilename, ofstream::app);
							logfile << inputFileName << "\tNonZeroCN@Iter0" << endl;
							logfile.close();
							cout << " Detected non-zero common noise on iteration 0 : Exit..."<<endl;
							outFile->Close();
							exit(EXIT_FAILURE);
						}

						//**** covariance matrix ****
						if(iIter == nIters-1 || iIter == 0){// @ first- last iteration
							ave[iCasis][iCasisChannel] += (adc - paramTemp->ped[iHydra][iHydraChannel] - cn); //+= (adc);
							for (int jCasisChannel = 0; jCasisChannel <= iCasisChannel; jCasisChannel++){
								int jHydraChannel = jCasisChannel+(14*(iCasis%2));

								double cn_j = cNoise->GetCN(iCasis,jCasisChannel,model);
								int adc_j = inputFile->GetCCUBEdata(iHydra, jHydraChannel);

								cov[iCasis][iCasisChannel][jCasisChannel] += (adc - paramTemp->ped[iHydra][iHydraChannel]- cn)*(adc_j- paramTemp->ped[iHydra][jHydraChannel] - cn_j) ; //+= (adc)*(adc_j) ;

							};
						};
						//***************************

						double val = (double)adc - cn;

						double cut = tCut * paramTemp->sig[iHydra][iHydraChannel];
						if(iIter==0)cut = 1e9;

						double ped = paramTemp->ped[iHydra][iHydraChannel];
						if( fabs(val - ped) >  cut )continue;

						param->ped[iHydra][iHydraChannel] += val;
						param->sig[iHydra][iHydraChannel] += val*val;
						param->nEvents[iHydra][iHydraChannel]++;

					}//end loop over chennels
				}// end loop over chips

				if(low_gain)
					pulserLowGainEvents++;
				if(null_sig)
					pulserNullSigEvents++;
			}// end loop over events

			if(pulserEvents<nMinPulserEvents) {
				ofstream logfile(logfilename, ofstream::app);
				logfile << inputFileName << "\tPulserEvents(" <<
						pulserEvents << ")<" << nMinPulserEvents << endl;
				logfile.close();
				cout << " Only " << pulserEvents << " pulser events (minimum " <<
						nMinPulserEvents << ") in the given file : Exit..."<<endl;
				outFile->Close();
				exit(EXIT_FAILURE);
			}
			if((double(pulserLowGainEvents)/(double)pulserEvents)>nMinPulserFraction) {
				ofstream logfile(logfilename, ofstream::app);
				logfile << inputFileName << "\tPulserWithLowGain(" <<
						(double(pulserLowGainEvents)/(double)pulserEvents) << ")>" << nMinPulserFraction << endl;
				logfile.close();
				cout << " Fraction of pedestal events with at least one channel in low gain is " <<
						(double(pulserLowGainEvents)/(double)pulserEvents) << " (maximum " << nMinPulserFraction <<") : Exit..."<<endl;
				outFile->Close();
				exit(EXIT_FAILURE);
			}
			if((double(pulserNullSigEvents)/(double)pulserEvents)>nMinPulserFraction) {
				ofstream logfile(logfilename, ofstream::app);
				logfile << inputFileName << "\tPulserWithNullSignal(" <<
						(double(pulserNullSigEvents)/(double)pulserEvents) << ")>" << nMinPulserFraction << endl;
				logfile.close();
				cout << " Fraction of pedestal events with at least one channel with null signal is " <<
						(double(pulserNullSigEvents)/(double)pulserEvents) << " (maximum " << nMinPulserFraction <<") : Exit..."<<endl;
				outFile->Close();
				exit(EXIT_FAILURE);
			}

			// ----------------------------------
			// Compute sigmas & pedestals
			// ----------------------------------
			cout << " N.events (PULSER) "<<pulserEvents<<endl;
			if(pulserEvents==0)continue;

			cout << " Calculate calibration parameters  "<<endl;
			for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++) {

				int iHydra = iCasis/2;

				for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS4CN; iCasisChannel++) {

					int iHydraChannel = iCasisChannel+(14*(iCasis%2));
					int sensor = GetSensor(iHydra,iHydraChannel);

					if (param->nEvents[iHydra][iHydraChannel] > 0) {
						/////////
						// PED //
						/////////
						param->ped[iHydra][iHydraChannel] /= param->nEvents[iHydra][iHydraChannel];
						paramTemp->ped[iHydra][iHydraChannel] = param->ped[iHydra][iHydraChannel];
						/////////
						// SIG //
						/////////
						param->sig[iHydra][iHydraChannel] /= param->nEvents[iHydra][iHydraChannel];
						param->sig[iHydra][iHydraChannel] -= pow(param->ped[iHydra][iHydraChannel],2);
						if(param->sig[iHydra][iHydraChannel]>=0)
							param->sig[iHydra][iHydraChannel] = sqrt( param->sig[iHydra][iHydraChannel] );
						else param->sig[iHydra][iHydraChannel] = 0.;
						paramTemp->sig[iHydra][iHydraChannel] = param->sig[iHydra][iHydraChannel];
						// -----------------
						// @ first iteration
						// -----------------
						if(iIter==0){
							pedHisto1D_raw->Fill(iHydraChannel + iHydra * NCASISCHANNELS,param->ped[iHydra][iHydraChannel] );
							sigmaHisto1D_raw->Fill(iHydraChannel + iHydra * NCASISCHANNELS,param->sig[iHydra][iHydraChannel] );
						}
						// ----------------
						// @ last iteration
						// ----------------
						if(iIter==nIters-1){
							pedHisto1D->Fill(iHydraChannel + iHydra * NCASISCHANNELS, param->ped[iHydra][iHydraChannel]);
							sigmaHisto1D->Fill(iHydraChannel + iHydra * NCASISCHANNELS, param->sig[iHydra][iHydraChannel]);

							if(sensor>=0)sigmaHisto1D_cube->Fill(iHydraChannel + iHydra * NCASISCHANNELS, param->sig[iHydra][iHydraChannel]);
						}
						if(iHydra == 2 || verbose){
							cout << " CHIP "<<iHydra;
							cout << " Channel " << setw(4)<<iHydraChannel;
							cout << " PED = <ADC-CN> "<<setw(9)<< param->ped[iHydra][iHydraChannel];
							cout << " SIG = RMS(ADC-CN) " <<setw(5)<< param->sig[iHydra][iHydraChannel];
							cout << " N.ev " <<setw(5)<< param->nEvents[iHydra][iHydraChannel] << endl;
						}
						if(param->nEvents[iHydra][iHydraChannel]<(unsigned int)pulserEvents){
							cout << " CHIP "<<iHydra;
							cout << " Channel " << setw(4)<<iHydraChannel;
							cout << " PED = <ADC-CN> "<<setw(9)<< param->ped[iHydra][iHydraChannel];
							cout << " SIG = RMS(ADC-CN) " <<setw(5)<< param->sig[iHydra][iHydraChannel];
							cout << " N.ev " <<setw(5)<< param->nEvents[iHydra][iHydraChannel];
							cout << " **warning**";
							cout << endl;
						}
					}
					else
						if(verbose)cout << "Warning: can't compute pedestal for Casis " << iHydra << " channel " << iHydraChannel
						<< ": no events." << endl;

					//**** ***************** ****
					//**** covariance matrix ****
					//**** ***************** ****
					if(iIter == nIters-1 || iIter == 0){// @ first-last iteration
						ave[iCasis][iCasisChannel] /= (double)param->nEvents[iHydra][iHydraChannel] ;
						for (int jCasisChannel = iCasisChannel; jCasisChannel >= 0; jCasisChannel--){
							cov[iCasis][iCasisChannel][jCasisChannel] /= (double)param->nEvents[iHydra][iHydraChannel] ;
							cov[iCasis][iCasisChannel][jCasisChannel] -= ave[iCasis][iCasisChannel]*ave[iCasis][jCasisChannel];
							if( sqrt(cov[iCasis][iCasisChannel][iCasisChannel]*cov[iCasis][jCasisChannel][jCasisChannel]) > 0.)
								rho[iCasis][iCasisChannel][jCasisChannel] = cov[iCasis][iCasisChannel][jCasisChannel] / sqrt(cov[iCasis][iCasisChannel][iCasisChannel]*cov[iCasis][jCasisChannel][jCasisChannel]);

							if(iIter==0)       {
								hrho_0[iCasis]->Fill((float)iCasisChannel,(float)jCasisChannel,rho[iCasis][iCasisChannel][jCasisChannel]);
								hrho_0[iCasis]->Fill((float)jCasisChannel,(float)iCasisChannel,rho[iCasis][iCasisChannel][jCasisChannel]*(iCasisChannel!=jCasisChannel));
							};

							if(iIter==nIters-1) {
								int iHydra = iCasis/2;
								int iHydraChannel = iCasisChannel+(14*(iCasis%2));
								int jHydraChannel = jCasisChannel+(14*(iCasis%2));

								if(InstrumentedChannel(iHydra,iHydraChannel)&&InstrumentedChannel(iHydra,jHydraChannel)){
									hrho[iCasis]->Fill((float)iCasisChannel,(float)jCasisChannel,rho[iCasis][iCasisChannel][jCasisChannel]);
									hrho[iCasis]->Fill((float)jCasisChannel,(float)iCasisChannel,rho[iCasis][iCasisChannel][jCasisChannel]*(iCasisChannel!=jCasisChannel));
								};
							};

						}
					};
				}//end loop over channels
			}//end loop over chips


			// --------------------------------------------------------
			// Fit correlation histograms and estimate ALPHA parameters
			// --------------------------------------------------------

			double alphaLargeErr[NCASISCHIPS4CN];
			double alphaSmallErr[NCASISCHIPS4CN];
			for (int iHisto = 0; iHisto < NCASISCHIPS4CN; iHisto++) {
				if(fAlphaLarge[iHisto]&&gAlphaLarge[iHisto]->GetN()>10)gAlphaLarge[iHisto]->Fit(fAlphaLarge[iHisto],"Q","0");
				if(fAlphaSmall[iHisto]&&gAlphaSmall[iHisto]->GetN()>10)gAlphaSmall[iHisto]->Fit(fAlphaSmall[iHisto],"Q","0");

				param->alphaLarge[iHisto] = fAlphaLarge[iHisto]->GetParameter(0);
				param->alphaSmall[iHisto] = fAlphaSmall[iHisto]->GetParameter(0);
				paramTemp->alphaLarge[iHisto] = fAlphaLarge[iHisto]->GetParameter(0);
				paramTemp->alphaSmall[iHisto] = fAlphaSmall[iHisto]->GetParameter(0);

				alphaLargeErr[iHisto] = fAlphaLarge[iHisto]->GetParError(0);
				alphaSmallErr[iHisto] = fAlphaSmall[iHisto]->GetParError(0);
			}

			// -----------------
			// @ first iteration
			// -----------------
			if(iIter==0){
			}

			// ----------------
			// @ last iteration
			// ----------------
			if(iIter==nIters-1){
				const int nbin=alphaLargeHisto1D->GetXaxis()->GetNbins();
				assert(nbin==NCASISCHIPS4CN);
				for(int ibin=0; ibin<nbin; ++ibin) {
					alphaLargeHisto1D->SetBinContent(ibin+1, param->alphaLarge[ibin]);
					alphaLargeHisto1D->SetBinError(ibin+1, alphaLargeErr[ibin]);
					alphaSmallHisto1D->SetBinContent(ibin+1, param->alphaSmall[ibin]);
					alphaSmallHisto1D->SetBinError(ibin+1, alphaSmallErr[ibin]);
				}
				//				alphaLargeHisto1D->SetContent(param->alphaLarge);
				//				alphaLargeHisto1D->SetError(alphaLargeErr);
				//				alphaSmallHisto1D->SetContent(param->alphaSmall);
				//				alphaSmallHisto1D->SetError(alphaSmallErr);
			}

			cout << "...close input file."<<endl;
			inputFile->CloseFile();
			delete inputFile;
		}//end iterations loop

		cout << "End iterations "<<endl;
	} else {
		// ---------------------------------------
		// Copy sigma histos from calibration file
		// ---------------------------------------
		cout << endl;
		cout << " -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ "<<endl;
		cout << " PED-SIG-ALPHA from input file "<<calibFileName<<endl;
		cout << " -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ "<<endl;
		cout << endl;
		TFile *calibFile = new TFile(calibFileName);
		if(  calibFile->IsZombie() )return 0;
		if( !calibFile->IsOpen() )return 0;

		if(!calibFile->Get("sigmaHisto1D")  )return 0;
		if(!calibFile->Get("pedHisto1D")  )return 0;
		if(!calibFile->Get("alphaLargeHisto1D")  )return 0;
		if(!calibFile->Get("alphaSmallHisto1D")  )return 0;

		if(sigmaHisto1D) sigmaHisto1D->Delete();
		if(pedHisto1D) pedHisto1D->Delete();
		if(alphaLargeHisto1D) alphaLargeHisto1D->Delete();
		if(alphaSmallHisto1D) alphaSmallHisto1D->Delete();

		sigmaHisto1D = (TH1F*) calibFile->Get("sigmaHisto1D")->Clone();
		pedHisto1D = (TH1F*) calibFile->Get("pedHisto1D")->Clone();
		alphaLargeHisto1D = (TH1F*) calibFile->Get("alphaLargeHisto1D")->Clone();
		alphaSmallHisto1D = (TH1F*) calibFile->Get("alphaSmallHisto1D")->Clone();

		if(!sigmaHisto1D)return 0;
		if(!pedHisto1D)return 0;
		if(!alphaLargeHisto1D)return 0;
		if(!alphaSmallHisto1D)return 0;

		cout << "...set parameters from histograms..."<<endl;
		if(sigmaHisto1D)param->SetSigma(sigmaHisto1D);
		if(pedHisto1D)param->SetPedestal(pedHisto1D);
		if(alphaLargeHisto1D&&alphaSmallHisto1D)param->SetAlpha(alphaLargeHisto1D,alphaSmallHisto1D);

		cout << "Check parameters "<<endl;
		cout << "ped[6][4] "<<param->ped[6][4]<<endl;
		cout << "sig[6][4] "<<param->sig[6][4]<<endl;
		cout << "alphaLarge[6] "<<param->alphaLarge[6]<<endl;
		cout << "alphaSmall[6] "<<param->alphaSmall[6]<<endl;
	}

	//===========================================================================================
	//    PEDESTAL CHECK
	//===========================================================================================

	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// loop over events for pedestal subtraction
	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// Event loop for common noise and pedestal subtraction and sigma estimation

	cout << "Open input file... "<<inputFileName<<endl;
	IOfile *inputFileCheck = OpenIOfile(inputFileName);

	// ----------------------------------
	// start loop over pulser events
	// ----------------------------------
	cout << "Pedestal Check (PULSER)"<<endl;

	calibParam* paramTrue = new calibParam;
	paramTrue->Reset();

	while (1) {

		/////////////////////////////////////////////// Read buffer
#if defined(ADAMO) || defined(AMS)
		int m = inputFileCheck->ReadEventHeader();
		if (m == -1)
			break;

		//		        struct VME_Header header = inputFileCheck->GetEventHeader();
		//		        cout << header.Flag << endl;
		//		        cout << header.EvNum << endl;
		//		        cout << header.Sec << endl;
		//		        cout << header.nSec << endl;
#endif
		int n = inputFileCheck->ReadEvent();
		if (n == -2) {
			cout << "Event corrupted. Skipping" << endl;
			continue;
		}
		else if (n == -1)
			break;

		///////////////////////////////////////////////

		// ----------------------------------
		// save event info
		// ----------------------------------
		eventInfo->Reset();
#if defined(SPS2017Aug) || defined(SPS2017Nov) || defined(TIC)
		if (inputFileCheck->GetLACTrigger() != RANDOMTRIGGER || isCalibration) //32-64
			continue;
		//		if (inputFileCheck->GetCCTrigger() != GOODEVENTFLAG) //2-4
		//			continue;
#else
		if ((inputFileCheck->GetCCTrigger()&0xf0) != RANDOMTRIGGER || isCalibration) //32-64
			continue;
		//		if ((inputFileCheck->GetCCTrigger()&0x0f) != GOODEVENTFLAG) //2-4
		//			continue;
#endif

		// ------------------------------------------------------
		// common-noise computation
		// ------------------------------------------------------
		cNoise->Reset();
		cNoise->Compute(inputFileCheck,param,250.);

		// ------------------------------------------------------
		// common-noise and pedestal subtraction
		// ------------------------------------------------------
		for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++) {

			int iHydra = iCasis/2;

			// Compute signal and fill sigma histogram for channels of this chip
			for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS4CN; iCasisChannel++){

				int iHydraChannel = iCasisChannel+(14*(iCasis%2));

				float buf = inputFileCheck->GetCCUBEdata(iHydra, iHydraChannel);

				double cn = cNoise->GetCN(iCasis,iCasisChannel,2);

				double val = buf - cn;

				paramTrue->ped[iHydra][iHydraChannel] += val;
				paramTrue->sig[iHydra][iHydraChannel] += val*val;
				paramTrue->nEvents[iHydra][iHydraChannel]++;
			}
		}
	}

	cout << "...close input file."<<endl;
	inputFileCheck->CloseFile();
	delete inputFileCheck;

	for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++) {

		int iHydra = iCasis/2;

		for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS4CN; iCasisChannel++) {

			int iHydraChannel = iCasisChannel+(14*(iCasis%2));

			if (paramTrue->nEvents[iHydra][iHydraChannel] > 0) {
				/////////
				// PED //
				/////////
				paramTrue->ped[iHydra][iHydraChannel] /= paramTrue->nEvents[iHydra][iHydraChannel];
				/////////
				// SIG //
				/////////

				paramTrue->sig[iHydra][iHydraChannel] /= paramTrue->nEvents[iHydra][iHydraChannel];

				paramTrue->sig[iHydra][iHydraChannel] -= pow(paramTrue->ped[iHydra][iHydraChannel],2);

				if(paramTrue->sig[iHydra][iHydraChannel]>=0)
					paramTrue->sig[iHydra][iHydraChannel] = sqrt( paramTrue->sig[iHydra][iHydraChannel] );
				else
					paramTrue->sig[iHydra][iHydraChannel] = 0.;

				pedHisto1D_true->Fill(iHydraChannel + iHydra * NCASISCHANNELS, paramTrue->ped[iHydra][iHydraChannel]);
				sigmaHisto1D_true->Fill(iHydraChannel + iHydra * NCASISCHANNELS, paramTrue->sig[iHydra][iHydraChannel]);
			}
		}
	}

	//===========================================================================================
	//    DATA REDUCTION
	//===========================================================================================

	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// loop over events for pedestal subtraction
	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// Event loop for common noise and pedestal subtraction and sigma estimation

	unsigned int nEvents = 0;
	cout << "Open input file... "<<inputFileName<<endl;
	IOfile *inputFile = OpenIOfile(inputFileName);

	// ----------------------------------
	// start loop over events
	// ----------------------------------
	cout << "Loop over events (TRIGGER&PULSER)"<<endl;
	unsigned int i=0;//chrck counter

	while (1) {

		/////////////////////////////////////////////// Read buffer
#if defined(ADAMO) || defined(AMS)
		int m = inputFile->ReadEventHeader();
		if (m == -1)
			break;
#endif

#ifdef ADAMO
		seconds = (UInt_t) inputFile->GetEventHeader().Sec;
		subseconds = (UInt_t) inputFile->GetEventHeader().nSec;
#elif AMS
		seconds = (UInt_t) (inputFile->GetEventHeader().ams_header.JMDCTIMEMSB*pow(2., 16.)+inputFile->GetEventHeader().ams_header.JMDCTIMELSB);
		subseconds = (UInt_t) (inputFile->GetEventHeader().ams_header.JMDCTIMEFINEMSB*pow(2., 16.)+inputFile->GetEventHeader().ams_header.JMDCTIMEFINELSB);
#endif

		int n = inputFile->ReadEvent();
		if (n == -2) {
			cout << "Event corrupted. Skipping" << endl;
			continue;
		}
		else if (n == -1)
			break;

		++i;//check counter
		///////////////////////////////////////////////

		// ----------------------------------
		// save event info
		// ----------------------------------
		eventInfo->Reset();
#if defined(SPS2017Aug) || defined(SPS2017Nov) || defined(TIC)
		eventInfo->trigger = inputFile->GetLACTrigger();
		eventInfo->good    = inputFile->GetCCTrigger();
#else
		eventInfo->trigger = (inputFile->GetCCTrigger()&0xf0);
		eventInfo->good    = (inputFile->GetCCTrigger()&0x0f);
#endif
		casisTime = inputFile->GetCCTime();

		nEvents++;

		if(!(nEvents%1000))cout << (inputFile->GetCCTrigger()) <<" " ;

		// Reset signal buffer
		for (int iCasis = 0; iCasis < NCASISCHIPS; iCasis++) {
			for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS; iCasisChannel++) {
				signal[iCasis][iCasisChannel] = 0.;
				signal_cn[iCasis][iCasisChannel] = 0.;
				signal_f[iCasis][iCasisChannel] = 0.;
			}
		}

		// ------------------------------------------------------
		// common-noise computation
		// ------------------------------------------------------
		cNoise->Reset();
		cNoise->Compute(inputFile,param,250.);

		// ------------------------------------------------------
		// common-noise and pedestal subtraction
		// ------------------------------------------------------
		for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++) {

			int iHydra = iCasis/2;

			// Compute signal and fill sigma histogram for channels of this chip
			for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS4CN; iCasisChannel++){

				int iHydraChannel = iCasisChannel+(14*(iCasis%2));

				float buf = inputFile->GetCCUBEdata(iHydra, iHydraChannel);
#ifdef PEDESTAL
				gain[iHydra][iHydraChannel] = 0;
#else
				gain[iHydra][iHydraChannel] = inputFile->GetCCGain(iHydra, iHydraChannel);
#endif

				double cn = cNoise->GetCN(iCasis,iCasisChannel,2);

				// ---------------------
				// raw
				// ---------------------
				raw[iHydra][iHydraChannel] = buf;
				if(gain[iHydra][iHydraChannel]==1) raw[iHydra][iHydraChannel] *= -1.;

				// ---------------------
				// guadagno forzato basso
				// ---------------------
				/// nel caso di guadagno forzato basso non va sottratto 2000
				signal_f[iHydra][iHydraChannel]   = buf - (float)param->ped[iHydra][iHydraChannel];
				if(gain[iHydra][iHydraChannel]==1)   signal_f[iHydra][iHydraChannel] *= 20.;

				// ---------------------
				// guadagno dinamico
				// ---------------------
				signal[iHydra][iHydraChannel]   = buf - (float)param->ped[iHydra][iHydraChannel] - (float)cn;
				if(gain[iHydra][iHydraChannel]==1)   signal[iHydra][iHydraChannel] *= 20.;
				if(gain[iHydra][iHydraChannel]==1)   signal[iHydra][iHydraChannel] -= 2000.;

				// ---------------------
				// guadagno dinamico
				// ---------------------
				signal_cn[iHydra][iHydraChannel]   = buf - (float)param->ped[iHydra][iHydraChannel];
				if(gain[iHydra][iHydraChannel]==1)   signal_cn[iHydra][iHydraChannel] *= 20.;
				if(gain[iHydra][iHydraChannel]==1)   signal_cn[iHydra][iHydraChannel] -= 2000.;
			}
		}

		// ------------------------------------------------------
		// end common-noise and pedestal subtraction
		// ------------------------------------------------------

		// ------------------------------------------------------
		// look for hifts
		// ------------------------------------------------------


		LADC.Reset();
		SADC.Reset();
		LMIP.Reset();
		SMIP.Reset();

		tCorr = 1;
		if(fCorr)tCorr = fCorr->Eval(0)/fCorr->Eval(casisTime/10.); //ns

		for (int iCasis = 0; iCasis < NCASISCHIPS; iCasis++) {
			for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS; iCasisChannel++) {
				float noise = (float)param->sig[iCasis][iCasisChannel];//random noise (CN subtracted)
				int irow,icol,ilay,isen;
				if(!Chip2Cube(iCasis, iCasisChannel,irow,icol,ilay,isen))continue;
				if(!GoodChannel(iCasis, iCasisChannel))continue;
				//
				// ADC chennels
				//
				if(

						signal[iCasis][iCasisChannel] > tCutHit * noise &&
						true){
					if(isen==0){ //Large
						LADC.hitRow[LADC.nHits] = irow;
						LADC.hitCol[LADC.nHits] = icol;
						LADC.hitLayer[LADC.nHits] = ilay;
						LADC.hitGain[LADC.nHits] = gain[iCasis][iCasisChannel];
						LADC.hitSat[LADC.nHits] = 0;
						LADC.hitSignal[LADC.nHits] = signal[iCasis][iCasisChannel];
						LADC.nHits++;

					}else if (isen == 1){ // Small
						SADC.hitRow[SADC.nHits] = irow;
						SADC.hitCol[SADC.nHits] = icol;
						SADC.hitLayer[SADC.nHits] = ilay;
						SADC.hitGain[SADC.nHits] = gain[iCasis][iCasisChannel];
						SADC.hitSat[SADC.nHits] = 0;
						SADC.hitSignal[SADC.nHits] = signal[iCasis][iCasisChannel];
						SADC.nHits++;
					}
					//
					// equalizeds  signals
					//
					float mip = tCorr * signal[iCasis][iCasisChannel] * MIP2ADCNorm[irow][icol][ilay] ;

					if(
							isen==0 &&
							mip > mipCutHit &&
							true){
						LMIP.hitRow[LMIP.nHits] = irow;
						LMIP.hitCol[LMIP.nHits] = icol;
						LMIP.hitLayer[LMIP.nHits] = ilay;
						LMIP.hitGain[LMIP.nHits] = gain[iCasis][iCasisChannel];
						LMIP.hitSat[LMIP.nHits] = 0;
						LMIP.hitSignal[LMIP.nHits] = mip;
						LMIP.nHits++;
						//
						int lCasis,lCasisChannel;
						if(Cube2Chip(irow,icol,ilay,1,lCasis,lCasisChannel)){
							SMIP.hitRow[SMIP.nHits] = irow;
							SMIP.hitCol[SMIP.nHits] = icol;
							SMIP.hitLayer[SMIP.nHits] = ilay;
							SMIP.hitSignal[SMIP.nHits] = tCorr * S2LNorm[irow][icol][ilay] * MIP2ADCNorm[irow][icol][ilay] * signal[lCasis][lCasisChannel];
							SMIP.hitGain[SMIP.nHits]   = gain[lCasis][lCasisChannel];
							SMIP.hitSat[SMIP.nHits]    = 0;
							SMIP.nHits++;

						}
					}
				}
			}
		}

		/////////////////////
		LShower.Reset();
		for(int ih=0; ih<LMIP.nHits; ih++){
			int iCube = LMIP.hitCol[ih] + LMIP.hitRow[ih]*NCOLS;
			int iLayer = LMIP.hitLayer[ih];

			float hitSignal = LMIP.hitSignal[ih];

			LShower.nHitsLayer[iLayer]++;              //n.hits per layer
			LShower.signalLayer[iLayer] += hitSignal;  //total signal per layer
			LShower.totSignal += hitSignal;            //total signal

			if(iCube==12)LShower.signalHitC[iLayer]=hitSignal;
			if(hitSignal > LShower.signalHitM[iLayer])LShower.signalHitM[iLayer]=hitSignal;

			// shower maximum
			if(hitSignal>LShower.signalHitMax){
				LShower.cubeHitMax = iCube;
				LShower.layerHitMax = iLayer;
				LShower.signalHitMax = hitSignal;
			}
		}
		for(int il=0; il<=NLAYERS; il++){
			if(LShower.signalLayer[il]>15){
				LShower.layerStart = il;
				break;
			}
		}
		/////////////////////
		SShower.Reset();
		for(int ih=0; ih<SMIP.nHits; ih++){
			int iCube = SMIP.hitCol[ih] + SMIP.hitRow[ih]*NCOLS;
			int iLayer = SMIP.hitLayer[ih];

			float hitSignal = SMIP.hitSignal[ih];

			SShower.nHitsLayer[iLayer]++;              //n.hits per layer
			SShower.signalLayer[iLayer] += hitSignal;  //total signal per layer
			SShower.totSignal += hitSignal;            //total signal

			if(iCube==12)SShower.signalHitC[iLayer]=hitSignal;
			if(hitSignal > SShower.signalHitM[iLayer])SShower.signalHitM[iLayer]=hitSignal;

			// shower maximum
			if(hitSignal>SShower.signalHitMax){
				SShower.cubeHitMax = iCube;
				SShower.layerHitMax = iLayer;
				SShower.signalHitMax = hitSignal;
			}
		}
		for(int il=0; il<=NLAYERS; il++){
			if(SShower.signalLayer[il]>15){
				SShower.layerStart = il;
				break;
			}
		}

		outTree->Fill();

		if(nEvents%5000 == 0)cout<<"data analysed: "<< nEvents<<endl;;
		if(nEvents%5000 == 0 && outFile)cout<<"  ("<< (int)(outFile->GetBytesWritten()/1000000)<<" MB) "<<endl;

	}// End of event loop with while(1)

	cout <<"total data analysed: "<< nEvents << endl;
	inputFile->CloseFile();
	delete inputFile;

	//===========================================================================================
	//  END DATA REDUCTION
	//===========================================================================================

	try
	{
		if(!outFile) {
			outFile = new TFile(outputFileName, "RECREATE");
			cout << "Creating  output file "<<outputFileName<<endl;
		}
		if(outFile->IsZombie())return 0;
		outFile->cd();

		cout << " tree: GetCacheSize() "<< outTree->GetCacheSize()<<endl;;
		cout << " file: GetBestBuffer() "<< (int)(outFile->GetBestBuffer()/1000)<<" kB"<<endl;
		cout << " file: GetBytesWritten() "<< (int)(outFile->GetBytesWritten()/1000000)<<" MB"<<endl;
		cout << "...and save histos and trees. "<< outFile->GetName()<<endl;

		if(calibRun){

			// ---------------------------------------------------------------
			// Save calibration data PED-SIG-ALPHA
			// ---------------------------------------------------------------

			for (int iHisto = 0; iHisto < NCASISCHIPS4CN; iHisto++) {
				//----------------------
				//correlation histograms
				//----------------------
				if(hrho_0[iHisto])hrho_0[iHisto]->Write();
				if(hrho[iHisto])hrho[iHisto]->Write();

				//----------------------
				//ALPHA parameters
				//----------------------
				if(gAlphaLarge[iHisto])gAlphaLarge[iHisto]->Write();
				if(gAlphaSmall[iHisto])gAlphaSmall[iHisto]->Write();
			}
		}


		//----------------------
		// PED & SIG
		//----------------------

		if(pedHisto1D_raw)pedHisto1D_raw->Write();
		if(sigmaHisto1D_raw)sigmaHisto1D_raw->Write();

		if(pedHisto1D)pedHisto1D->Write();
		if(sigmaHisto1D)sigmaHisto1D->Write();

		if(pedHisto1D_true)pedHisto1D_true->Write();
		if(sigmaHisto1D_true)sigmaHisto1D_true->Write();

		if(sigmaHisto1D_cube)sigmaHisto1D_cube->Write();

		if(alphaLargeHisto1D)alphaLargeHisto1D->Write();
		if(alphaSmallHisto1D)alphaSmallHisto1D->Write();

		if(outTree)outTree->Write();

		cout << "... passo e chiudo."<<endl;

		outFile->Close();
		delete outFile;

		cout << "Fatto."<<endl;
	}
	catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << '\n';
	}

	return 0;
}


