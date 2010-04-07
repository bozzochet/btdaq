//  $Id: DBcheck.C,v 1.4 2008/03/17 22:03:34 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  DBcheck.C
///\brief GbatchTest module gbtrk.C
///
/// A module class to be defined by user.
/// The class name must be GbatchTest.
/// Its instance is created by gbtest.C macro and called.
/// Four user methods, PreRun, PostRun, PreEvent, PostEvent 
/// are called from GbatchTRK.  
///
///\date  2007/11/29 SH  First test version
///$Date: 2008/03/17 22:03:34 $
///
///$Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchTRK.h"
#include "GbatchRec.h"

#include "TROOT.h"
#include "TRandom.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

class GbatchTest : public GbatchTRK {

private:
  TH1D *histn;
  TH2D *hist0;
  TH1D *hist1;
  TH1D *hist2;
  TH1D *hist3;
  TH2D *hist4;
  TH2D *hist5;

public:
  GbatchTest(void) {}
 ~GbatchTest() {}

protected:
  void PreRun   (void);
  void PostRun  (void);
  void PreEvent (void);
  void PostEvent(void);

  ClassDef(GbatchTest, 1)
};

ClassImp(GbatchTest)

void GbatchTest::PreRun(void)
{
  // Create histograms
  gROOT->cd();
  histn = new TH1D("histn", "Ntrack", 10, 0, 10);
  hist0 = new TH2D("hist0", "VTX", 140, -70, 70, 100, -50, 50);
  hist1 = new TH1D("hist1", "d(1/R)", 1000, -0.1, 0.1);
  hist2 = new TH1D("hist2", "ChisqX", 1000, 0, 100);
  hist3 = new TH1D("hist3", "ChisqY", 1000, 0, 100);
  hist4 = new TH2D("hist4", "ResX VS Layer", 8, 0.5, 8.5, 250, -0.5, 0.5);
  hist5 = new TH2D("hist5", "ResY VS Layer", 8, 0.5, 8.5, 250, -0.5, 0.5);
}

void GbatchTest::PostRun(void)
{
  // Write histograms
  TFile of(Form("root/dbtest.root.%04.0f", _momentum), "recreate");
  gROOT->GetList()->Write();
}

void GbatchTest::PreEvent(void)
{
  // Set random position
  _vertex[0] = -50+100*gRandom->Rndm();  // in cm
  _vertex[1] = -50+100*gRandom->Rndm();  // in cm
}

#include "Event.h"
#include "TrFit.h"
#include "TrTrack.h"
#include "TrRecHit.h"
#include "point.h"

void GbatchTest::PostEvent(void)
{
  Event *event = GetEvent();

  histn->Fill(event->NTrTracks);

  if (event->NTrTracks > 0) {
    hist0->Fill(_vertex[0], _vertex[1]);

    TrTrack *track = event->GetTrTrack(0);

    Double_t rgt = track->GetRigidity();
    if (rgt != 0) hist1->Fill (1/rgt-1/_momentum);

    hist2->Fill(track->GetChisqX());
    hist3->Fill(track->GetChisqY());

    Double_t ehit[3] = { 10e-4, 30e-4, 100e-4 };

    TrFit fit;
    for (Int_t i = 0; i < track->GetNhits(); i++) {
      TrRecHit *hit = track->GetHit(i);
      if (!hit) continue;

      AMSPoint hits = hit->GetCoord();
      fit.Add(hits[0], hits[1], hits[2], ehit[0], ehit[1], ehit[2]);
    }

    if (fit.Fit(TrFit::SIMPLE) > 0) {
      for (Int_t i = 0; i < track->GetNhits(); i++) {
	TrRecHit *hit = track->GetHit(i);
	if (!hit) continue;

	Int_t layer = hit->GetLayer();
	hist4->Fill(layer, fit.GetXr(i)*1e-3);
	hist5->Fill(layer, fit.GetYr(i)*1e-3);
      }
    }
  }

/*
  // Create GbatchRec object to access reconstructed tracks
  GbatchRec *rec = GbatchRec::GetPtr();

  rec->SetTracks();

  // Select the first track which has more than 7 hits
  if (rec->GetNtrack() > 0 && rec->GetNhits(0) >= 7) {

    // Fill difference of reconstructed 1/R from the initial value
    Double_t rgt = rec->GetRigidity(0);
    if (rgt != 0) {
      hist1->Fill (1/rgt-1/_momentum);
      hist2->Fill((1/rgt-1/_momentum)*_momentum);
    }
  }
*/
}
