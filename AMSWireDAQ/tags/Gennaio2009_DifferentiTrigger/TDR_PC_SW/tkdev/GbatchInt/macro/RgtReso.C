//  $Id: RgtReso.C,v 1.1 2008/01/21 21:58:35 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  RgtReso.C
///\brief GbatchTest module gbtrk.C
///
/// A module class to be defined by user.
/// The class name must be GbatchTest.
/// Its instance is created by gbtest.C macro and called.
/// Four user methods, PreRun, PostRun, PreEvent, PostEvent 
/// are called from GbatchTRK.  
///
///\date  2007/11/29 SH  First test version
///$Date: 2008/01/21 21:58:35 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchTRK.h"
#include "GbatchRec.h"

#include "TROOT.h"
#include "TRandom.h"
#include "TH1.h"
#include "TFile.h"

class GbatchTest : public GbatchTRK {

private:
  TH1D *hist1;
  TH1D *hist2;

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
  hist1 = new TH1D("hist1", "d(1/R)",   1000, -0.1, 0.1);
  hist2 = new TH1D("hist2", "d(1/R)*R", 1000,  -10,  10);
}

void GbatchTest::PostRun(void)
{
  // Write histograms
  TFile of(Form("root/gbtrk.root.%04.0f", _momentum), "recreate");
  gROOT->GetList()->Write();
}

void GbatchTest::PreEvent(void)
{
  // Set random position
  _vertex[0] = -10+20*gRandom->Rndm();  // in cm
  _vertex[1] = -10+20*gRandom->Rndm();  // in cm
}

void GbatchTest::PostEvent(void)
{
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
}
