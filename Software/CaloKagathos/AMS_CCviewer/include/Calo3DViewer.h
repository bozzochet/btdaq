#ifndef CALO3DVIEWER_H_
#define CALO3DVIEWER_H_

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

using namespace std;


class Calo3DViewer{

public:
	Calo3DViewer(bool adc_mip=false);
	void Inizialize3Dviewer(TCanvas *c3D);
	void Fill3Dviewer(TCanvas *c3D,Int_t nHits_l,Int_t nHits_s, Float_t hitSignal_l[], Float_t hitSignal_s[],
			Int_t hitRow_l[], Int_t hitRow_s[], Int_t hitCol_l[], Int_t hitCol_s[], Int_t hitLayer_l[], Int_t hitLayer_s[]);
	void DeleteImage(Int_t nHits_l,Int_t nHits_s, Float_t hitSignal_l[], Float_t hitSignal_s[],
			Int_t hitRow_l[], Int_t hitRow_s[], Int_t hitCol_l[], Int_t hitCol_s[], Int_t hitLayer_l[], Int_t hitLayer_s[]);
	~Calo3DViewer();
private:

	const int fNROW;
	const int fNCOL;
	const int fNLAY;
	const int fNCOLOR;
	const double fMIP2MeV; //30.995 MeV energy release by 50 GeV muons
  //ignoring losses different from ionization (bremmstrahlung, pair production and photonuclear)
  //and delta-ray production
	const float fSIZE;

	// 3 dim vector [fNCOL][fNROW][fNLAY];
	vector< vector< vector< TPolyLine* > > > fCubeSignal_l;
	vector< vector< vector< TPolyLine* > > > fCubeSignal_s;
	vector< vector< vector< TPolyLine* > > > fCubeFrame;
	bool fAdc_mip;
	float fLog10_paletteMin;
	float fLog10_paletteMax;
	TBox* fFront;
	static Color_t fColor[];

};

#endif
