//  $Id: Cosmic.C,v 1.1 2008/05/09 10:29:11 haino Exp $

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
///$Date: 2008/05/09 10:29:11 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchTRK.h"
#include "GbatchRec.h"

#include "Event.h"
#include "TrFit.h"
#include "TrField.h"
#include "TrTrack.h"
#include "TrRecHit.h"

#include "TkDBc.h"
#include "TkPlane.h"
#include "TkLadder.h"
#include "TrCalDB.h"

#include "dcards.h"

#include "TROOT.h"
#include "TRandom.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TMath.h"

#include <ctime>
#include <cstdio>

class GbatchTest : public GbatchTRK {

private:
  TFile *outfile;
  TTree *evttree;

  Event *event;

  TrFit trfit;

  TH1F *hflux;

  TH1F *hist0;

  int ntot;
  int nsel;

  Double_t simrgt;
  Double_t simvtx[3];
  Double_t simdir[3];

public:
  GbatchTest(void) {}
 ~GbatchTest() {}

protected:
  void PreInit  (void);
  void PreRun   (void);
  void PostRun  (void);
  void PreEvent (void);
  void PostEvent(void);

  ClassDef(GbatchTest, 1)
};

ClassImp(GbatchTest)

void GbatchTest::PreInit(void)
{
/*
  Int_t da = (Int_t)(_momentum/10);

  // Read TkDBc
  char tfile[256];
  sprintf(tfile, "gen/tkdbc.dat.da%02d", da);
  TkDBc::Head->read(tfile); 

  cout << "### Cosmic::Info ### TkDBc: " << tfile << endl;
*/
}

void GbatchTest::PreRun(void)
{
  // Get muon flux
  hflux = 0;
  TFile f("dat/muflux.root");
  if (f.IsOpen()) {
    gROOT->cd();
    hflux = (TH1F *)f.Get("hist")->Clone();
  }

  // Create histograms
  gROOT->cd();

  hist0 = new TH1F("hist0", "TkId", 256, 0.5, 8.5);

  time_t tm;
  time(&tm);

  UInt_t seed = tm-1200000000;
  gRandom->SetSeed(seed);
  cout << "### Cosmic::Info ### Random seed is " << seed << endl;

  event = new Event;
  SetEvent(event);

  outfile = new TFile(Form("wdir/sim_%10d.root", (int)tm), "recreate");
  evttree = new TTree("t4","AMSEvents");
  evttree->Branch("simrgt", &simrgt, "simrgt/D");
  evttree->Branch("simvtx",  simvtx, "simvtx[3]/D");
  evttree->Branch("simdir",  simdir, "simdir[3]/D");
  evttree->Branch("Tracker", "Event", &event);

  ntot = nsel = 0;

  cout << "### Cosmic::Info ### Output file is " 
       << outfile->GetName() << endl;

  // Magnetic field off
  TrField::_magstat = 0;
  MAGSFFKEY.magstat = 0;

  // Disable ladders
//  DisableLadders("dat/kill_tkid.dat");
}

void GbatchTest::PostRun(void)
{
  // Write TrCalDB
  TrCalDB::Head->Write();

  // Write tree
  evttree->Write();

  // Write histograms
  outfile->Write();

  // Write muon flux
  if (hflux) hflux->Write();

  hist0->Write();

  // Close file
  delete outfile;

  double sw0 = (1.2*1.2-0.24*0.24*4)*TMath::TwoPi();

  cout << "### Cosmic::Info ### Total generaged " << ntot << endl;
  cout << "### Cosmic::Info ### Generation acceptance is " << sw0 << endl;
}

void GbatchTest::PreEvent(void)
{
  // Set random position 1.2*1.2-0.24*0.24*4 = 1.2096 m2
  // (roughly correspondig to upper TOF geometry)
  double vx, vy, vz = 65;
  do {
    vx = -60+120*gRandom->Rndm();  // in cm
    vy = -60+120*gRandom->Rndm();  // in cm
  } while (TMath::Abs(vx) > 36 && TMath::Abs(vy) > 36);

  simvtx[0] = _vertex[0] = vx;  // in cm
  simvtx[1] = _vertex[1] = vy;  // in cm
  simvtx[2] = _vertex[2] = vz;  // in cm

  // Set random direction (2pi sr)
  double dx, dy, dz, tx, ty, tz = 0;
  do {
    // Zenith angle
    double cost = gRandom->Rndm();
    double sint = TMath::Sqrt(1-cost*cost);

    // Azimuth angle
    double phi  = TMath::TwoPi()*gRandom->Rndm()-TMath::Pi();
    double sinp = TMath::Sin(phi);
    double cosp = TMath::Cos(phi);

    dx = sint*cosp;
    dy = sint*sinp;
    dz = -cost;

    tx = vx+dx*(tz-vz)/dz;
    ty = vy+dy*(tz-vz)/dz;

    ntot++;
    // For the general run
//  } while (TMath::Abs(tx) > 65 || TMath::Abs(ty) > 65);
  } while (TMath::Abs(tx) > 70 || TMath::Abs(ty) > 70);

    // For Jan/Feb 2008 cosmic-ray run (half crate configuration)
//  } while (TMath::Abs(tx) > 50 || ty < -45 || 5 < ty);

  simdir[0] = _direction[0] = dx;
  simdir[1] = _direction[1] = dy;
  simdir[2] = _direction[2] = dz;

  // Set random momentum (Flat spectrum between 0.1-100 GeV/c)
  if (hflux) {
    Double_t mom;
    do {
      mom = hflux->GetRandom();
    } while (mom < 0.1 || 100 < mom);
    simrgt = _momentum = mom;
  }
  else
    simrgt = _momentum = TMath::Power(10, -1+3*gRandom->Rndm());

  // Set particle as mu+ (mu- is 6)
  _particle = 5;
}

void GbatchTest::PostEvent(void)
{
//  evttree->Fill();
  Event *event = GetEvent();

  if (event->NTrTracks != 1) return;

  TrTrack *track = event->GetTrTrack(0);

  for (Int_t i = 0; i < track->GetNhits(); i++) {
    TrRecHit *hit = track->GetHit(i);
    if (!hit) continue;

    Int_t tkid = hit->GetTkId();
    Int_t layr = TMath::Abs(tkid)/100;
    Int_t slot = tkid%100;

    Double_t xid = layr+slot/32.;
    hist0->Fill(xid);
  }
}

