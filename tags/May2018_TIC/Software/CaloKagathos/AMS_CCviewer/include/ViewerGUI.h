#ifndef VIEWERGUI_H_
#define VIEWERGUI_H_

#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TGFrame.h>
#include <TGWindow.h>
#include <TRootEmbeddedCanvas.h>
#include <TGNumberEntry.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

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

class TGWindow;
class TGMainFrame;
class TRootEmbeddedCanvas;

class ViewerGUI {
   RQ_OBJECT("ViewerGUI")
private:
   TGMainFrame *fMain;
   TRootEmbeddedCanvas *fEc2D;
   TRootEmbeddedCanvas *fEc3D;
   TRootEmbeddedCanvas *fEcAMS;
   TRootEmbeddedCanvas *fEcDAMPE;
   TRootEmbeddedCanvas *fEcTracker;
   TGNumberEntry *fEventNumbEntry;
public:
   ViewerGUI(const TGWindow *p,UInt_t w,UInt_t h, int VCONF);
   virtual ~ViewerGUI();
   void DoNextEvent();
   void DoEvent();
   void DoPrintEvent();
   void DoDrawAMSOccupancy();
   void DoDrawDAMPEOccupancy();
   void DoPrintAMSOccupancy();
   void DoPrintDAMPEOccupancy();
   TCanvas *GetC2D() {return fEc2D->GetCanvas();};
   TCanvas *GetC3D() {return fEc3D->GetCanvas();};
   TCanvas *GetCAMS() {return fEcAMS->GetCanvas();};
   TCanvas *GetCDAMPE() {return fEcDAMPE->GetCanvas();};
   TCanvas *GetCTracker() {return fEcTracker->GetCanvas();};
};

#endif
