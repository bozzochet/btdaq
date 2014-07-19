//  $Id: CRtest.C,v 1.1 2008/05/09 10:29:11 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  CRtest.C
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
#include "TH2.h"
#include "TMath.h"

#include <ctime>
#include <cstdio>

class GbatchTest : public GbatchTRK {

public:
  static TString flux_fname;
  static TString output_dir;
  static int rgt_mode;
  static int mag_stat;
  static int disa_mode;
  static int kill_ladder;
  static int rand_seed;

private:
  TFile *outfile;
  TTree *evttree;

  Event *event;

  TrFit trfit;

  TH1F *hflux;

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

TString GbatchTest::flux_fname  = "dat/pflux_cutave.root";
TString GbatchTest::output_dir  = "wdir";
int     GbatchTest::rgt_mode    = 0;
int     GbatchTest::mag_stat    = 0;
int     GbatchTest::disa_mode   = 0;
int     GbatchTest::kill_ladder = 0;
int     GbatchTest::rand_seed   = -1;

void GbatchTest::PreInit(void)
{
  // Read TkDBc
  if (disa_mode > 0) {
    char tfile[256];
    sprintf(tfile, "gen/tkdbc.dat.da%03d", disa_mode);
    TkDBc::Head->read(tfile); 
    cout << "### CRtest::Info ### TkDBc: " << tfile << endl;
  }
  else
    cout << "### CRtest::Info ### TkDBc: default" << endl;
}

void GbatchTest::PreRun(void)
{
  // Get proton flux
  hflux = 0;

  if (flux_fname != "") {
    TFile f(flux_fname);
    if (f.IsOpen()) {
      gROOT->cd();
      hflux = (TH1F *)f.Get("hist")->Clone();
      cout << "### CRtest::Info ### Input flux: " 
	   << f.GetName() << endl;
    }
  }

  // Create histograms
  gROOT->cd();

  time_t tm;
  if (rand_seed < 0) time(&tm);
  else tm = rand_seed;

  UInt_t seed = tm-1200000000;
  gRandom->SetSeed(seed);
  cout << "### CRtest::Info ### Random seed is " << seed << endl;

  event = new Event;
  SetEvent(event);

  if (disa_mode > 0)
    outfile = new TFile(Form("%s/da%03d/sim_%10d.root", 
			   output_dir.Data(), disa_mode, (int)tm), "recreate");
  else
    outfile = new TFile(Form("%s/sim_%10d.root", 
			     output_dir.Data(), (int)tm), "recreate");
  evttree = new TTree("t4","AMSEvents");
  evttree->Branch("simrgt", &simrgt, "simrgt/D");
  evttree->Branch("simvtx",  simvtx, "simvtx[3]/D");
  evttree->Branch("simdir",  simdir, "simdir[3]/D");
  evttree->Branch("Tracker", "Event", &event);

  ntot = nsel = 0;

  cout << "### CRtest::Info ### Output file is " 
       << outfile->GetName() << endl;

  // Magnetic field off
  TrField::_magstat = mag_stat;
  MAGSFFKEY.magstat = mag_stat;

  // Disable ladders
  if (kill_ladder)
    DisableLadders("dat/kill_tkid.dat");
}

void GbatchTest::PostRun(void)
{
  // Write TrCalDB
  TrCalDB::Head->Write();

  // Write tree
  evttree->Write();

  // Write histograms
  outfile->Write();

  // Write flux histogram
  if (hflux) hflux->Write();

  // Close file
  delete outfile;

//  double sw0 = (1.2*1.2-0.24*0.24*4)*TMath::TwoPi();
  double sw0 = (1.3*1.3)*TMath::TwoPi();

  cout << "### CRtest::Info ### Total generaged " << ntot << endl;
  cout << "### CRtest::Info ### Generation acceptance is " << sw0 << endl;
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
  double dx, dy, dz;
  double tx[2], ty[2], tz[2] = { 0, -65 };
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

    tx[0] = vx+dx*(tz[0]-vz)/dz;
    ty[0] = vy+dy*(tz[0]-vz)/dz;
    tx[1] = vx+dx*(tz[1]-vz)/dz;
    ty[1] = vy+dy*(tz[1]-vz)/dz;

    ntot++;
  } while (TMath::Abs(tx[0]) > 65 || TMath::Abs(ty[0]) > 65 ||
	   TMath::Abs(tx[1]) > 65 || TMath::Abs(ty[1]) > 65);
/*
  } while (TMath::Abs(tx[0]) > 60 || TMath::Abs(ty[0]) > 60 ||
	   TMath::Abs(tx[1]) > 60 || TMath::Abs(ty[1]) > 60 ||
	  (TMath::Abs(tx[1]) > 36 && TMath::Abs(ty[1]) > 36));
*/
  simdir[0] = _direction[0] = dx;
  simdir[1] = _direction[1] = dy;
  simdir[2] = _direction[2] = dz;

  // Set random momentum (Flat spectrum between 0.1-100 GeV/c)
  if (rgt_mode == 0) {
    if (hflux)
      simrgt = _momentum = hflux->GetRandom();
    else
//    simrgt = _momentum = TMath::Power(10, 3*gRandom->Rndm()); // 1GV-1TV
      simrgt = _momentum = TMath::Power(10, -1+3*gRandom->Rndm()); // 1-100GV
  }
  else simrgt = _momentum;

  // Set particle as mu+ (mu- is 6)
  //_particle = 5;
  // Set particle as p
  _particle = 14;
}

void GbatchTest::PostEvent(void)
{
  evttree->Fill();
}

