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

/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

/* from the CommonTool dir */
#include "TrackSelection.hh"
#include "Utilities.hh"
/* end */

using namespace std;


class CaloAMS2DViewer{

public:
	CaloAMS2DViewer(bool adc_mip=false);
	void Inizialize2Dviewer(TCanvas *c2D);
	void Fill2DCCviewer(TCanvas *c2D,Int_t nHits_l, Float_t hitSignal_l[],Int_t hitRow_l[], Int_t hitCol_l[], Int_t hitLayer_l[]);
	void Fill2DAMSviewer(TCanvas *c2D, Event *ev);
	void DeleteImage();
	~CaloAMS2DViewer();
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
	static int fAMSTDRNUM[];
	const double fAMSPITCHX;
	const double fAMSPITCHY;
	const int fAMSNSTRIP;
	const int fAMSNLAY;
	const double fAMSSIZEZ;
	const int fAMSREBIN;

	// 3 dim vector [X][XoY]
	vector< vector< TBox* > > fCubeFrameX;
	vector< vector< TBox* > > fCubeFrameY;
	vector< TLine* > fAMSLineX;
	vector< TLine* > fAMSLineY;
	TH2D *h2DCalocubeX;
	TH2D *h2DCalocubeY;
	TH2D *h2DBaseX;
	TH2D *h2DBaseY;
	TH2D *h2DAMSX;
	TH2D *h2DAMSY;
	bool fAdc_mip;
};

#endif

