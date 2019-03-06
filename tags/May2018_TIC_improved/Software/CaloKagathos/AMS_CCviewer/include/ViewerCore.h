#ifndef VIEWERCORE_H_
#define VIEWERCORE_H_

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

#include "Calo3DViewer.h"
#include "CaloAMS2DViewer.h"
#include "DataFileHandler.h"

using namespace std;

class ViewerCore{

public:
	static ViewerCore &GetIstance();
	void SetOption(int VCNF,int eventStart,bool adc_mip, bool batch);
	// return 1, both files, 2 CC file, 3 AMS file, 0 no files.
	int OpenRootTrees(string inputCCFileName,string inputAMSFileName);
	void CreateCanvas();
	void SetCanvasEvent(TCanvas *c3D,TCanvas *c2D);
	void SetCanvasAMS(TCanvas *cAMS) {fCAMS=cAMS;};
	void SetCanvasDAMPE(TCanvas *cDAMPE) {fCDAMPE=cDAMPE;};
	void SetCanvasTracker(TCanvas *cTracker) {fCTracker=cTracker;};
	vector<TCanvas *> GetCanvasVector() {return fCVect;}
	void InizializeViewers();
	// 1 ok, 0 event>Entries, 2 not selected event.
	short DrawEvent(int iEv);
	short DrawNextEvent();
	bool DrawAMSOccupancy();
	bool DrawDAMPEOccupancy();
	void DeleteEvent();
	bool NeedToDelete() {return fNeedToDelete;};
	void PrintEvent();
	int GetCurrentEvent() {return fPrevEvent;};
	~ViewerCore();

private:

	ViewerCore();
	ViewerCore(const ViewerCore&);
	int fEventStart;
	int fAdc_mip;
	bool fBatch;
	int fVCONF;
	Calo3DViewer *fCalo3dviewer;
	CaloAMS2DViewer *fCaloams2dviewer;
	TCanvas *fC2D;
	TCanvas *fC3D;
	TCanvas *fCAMS;
	TCanvas *fCDAMPE;
	TCanvas *fCTracker;
	vector<TCanvas *> fCVect;
	int fPrevEvent;
	bool fNeedToDelete;
};

#endif
