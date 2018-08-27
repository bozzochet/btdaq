#ifndef CALOAMS2DVIEWER_H_
#define CALOAMS2DVIEWER_H_

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
#include <TBox.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TLegend.h>
#include <TPolyLine.h>
#include <TGaxis.h>

#include <DataFileHandler.h>

using namespace std;

class CaloAMS2DViewer{

public:

	// event viewer
	CaloAMS2DViewer(bool adc_mip=false);
	void Inizialize2Dviewer(TCanvas *c2D,TCanvas *cTracker);
	void Fill2DCCviewer(TCanvas *c2D,Int_t nHits_l, Float_t hitSignal_l[],Int_t hitRow_l[], Int_t hitCol_l[], Int_t hitLayer_l[]);
	void Fill2DAMSviewer(TCanvas *c2D, Event *ev);
	void Fill2DDAMPEviewer(TCanvas *c2D,TCanvas *cTracker, Event *ev);
	void DeleteImage();
	~CaloAMS2DViewer();

	// cumulative plots
	// AMS occupancy
	void CreateAMSOccupancy(TCanvas *cAMS);
	void FillAMSOccupancy(Event *ev);
	void DrawAMSOccupancy(TCanvas *cAMS);
	// DAMPE occupancy
	void CreateDAMPEOccupancy(TCanvas *cDAMPE);
	void FillDAMPEOccupancy(Event *ev);
	void DrawDAMPEOccupancy(TCanvas *cDAMPE);

private:

	// Calocube geometry
	static double fCCLAYERZ[];
	static double fCCCUBEX[];
	static double fCCCUBEY[];
  const int fCCNROW;
	const int fCCNCOL;
	const int fCCNLAY;
	const double fCCMIP2MeV; //30.995 MeV energy release by 50 GeV muons
  //ignoring losses different from ionization (bremmstrahlung, pair production and photonuclear)
  //and delta-ray production
	const float fCCSIZEXY;
	const float fCCSIZEZ;

	// AMS geometry
	static double fAMSLAYERZ[];
	static double fAMSALIGNX[];
	static double fAMSALIGNY[];
	static bool fAMSDIRECTIONX[];
	static bool fAMSDIRECTIONY[];
	static int fAMSTDRNUM[];
	const double fAMSPITCHX;
	const double fAMSPITCHY;
	const int fAMSNSTRIPX;
	const int fAMSNSTRIPY;
	const int fAMSNLAY;
	const double fAMSSIZEZ;
	const int fAMSREBIN;
	const bool fAMSXSIDE;

	// DAMPE geometry
	static double fDAMPELAYERZ[];
	static double fDAMPEALIGNX[];
	static bool fDAMPEDIRECTIONX[];
	static int fDAMPETDRNUM[];
	const double fDAMPEPITCHX;
	const int fDAMPENSTRIP;
	const int fDAMPENLAY;
	const double fDAMPESIZEZ;
	const int fDAMPEREBIN;
	const bool fDAMPEXSIDE;

	// 3 dim vector [X][XoY]
	vector< vector< TBox* > > fCubeFrameX;
	vector< vector< TBox* > > fCubeFrameY;
	vector< TLine* > fAMSLineX;
	vector< TLine* > fAMSLineY;
	vector< TLine* > fDAMPELineX;
	TH2D *h2DCalocubeX;
	TH2D *h2DCalocubeY;
	TH2D *h2DBaseX;
	TH2D *h2DBaseY;
	TH2D *h2DAMSX;
	TH2D *h2DAMSY;
	TH2D *h2DDAMPEX;
	bool fAdc_mip;

	// Histo for cumulative plots
	vector<TH2F *> fHAMSOccupancyVect;
	vector<TH1F *> fHDAMPEOccupancyVect;
	vector<TH1F *> fHDAMPEHitVect;

	bool ReturnAMSHit(int iCluster, Event *ev, int &tdr, int &side, int &Layer, vector<int> &strip,vector<float> &position, vector<float> &signal);
	bool ReturnDAMPEHit(int iCluster, Event *ev, int &tdr, int &Ladder, vector<int> &strip,vector<float> &position, vector<float> &signal);

};

#endif

