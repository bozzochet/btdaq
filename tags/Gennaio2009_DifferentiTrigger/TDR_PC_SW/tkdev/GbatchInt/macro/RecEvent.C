
#include "Event.h"
#include "TrReconstruction.h"
#include "TrTrack.h"
#include "TkDBc.h"
#include "TrCalDB.h"

#include "TFile.h"
#include "TTree.h"
#include "TStopwatch.h"

void RecEvent(const char *inpname, const char *calname, 
	      const char *outname, const char *dbcname = 0,
	      Int_t mag_stat = 1, Int_t nevt = 0)
{
  // Open input root file
  TFile f(inpname);
  if (!f.IsOpen()) return;

  // Get tree
  TTree *t4 = (TTree *)f.Get("t4");
  if (!t4) return;

  // Create and initialize TkDBc
  TkDBc::CreateTkDBc();
  if (dbcname) TkDBc::Head->init((char*)dbcname);
  else TkDBc::Head->init();

  // Read calibration data
  TrCalDB::Load((char*)calname);
  if (!TrCalDB::Head) return;

  // Create an Event object
  Event *event = new Event;

  // Simulation data
  Double_t simrgt, simvtx[3], simdir[3];

  // Set Branch address
  t4->SetBranchAddress("simrgt", &simrgt);
  t4->SetBranchAddress("simvtx",  simvtx);
  t4->SetBranchAddress("simdir",  simdir);
  t4->SetBranchAddress("Tracker", &event);

  // Create a TrReconstruction object
  TrReconstruction* trec = new TrReconstruction;

  // Set Event pointer
  trec->SetEvent(event);
  TrTrack::_event = event;

  // Open output root file
  TFile of(outname, "new");
  if (!of.IsOpen()) return;

  // Create a new tree
  TTree *t4new = new TTree("t4", "AMSEvents");
  t4new->Branch("simrgt", &simrgt, "simrgt/D");
  t4new->Branch("simvtx",  simvtx, "simvtx[3]/D");
  t4new->Branch("simdir",  simdir, "simdir[3]/D");
  t4new->Branch("Tracker", "Event", &(event));

  // Some settings in TrCluster
  trec->SetThrSeed(0, 3.5); trec->SetThrNeig(0, 0.1);
  trec->SetThrSeed(1, 3.5); trec->SetThrNeig(1, 0.1);
  trec->SetThrProb(0);

  // Magnet field status
  TrField::_magstat = mag_stat;

  // Some settings in TrTrack for no B run
  if (!mag_stat) {
    TrTrack::MaxChisq[0] = 100;
    TrTrack::MaxChisq[1] = 10000;
    TrTrack::SearchRegStrLine = 100;
    TrTrack::SearchRegCircle  = 100;
    TrTrack::MaxNtrack = 1;
    TrTrack::ErrX = 300e-4;
    TrTrack::ErrY = 300e-4;
  }

  // Read magnetic field data for B run
  if (mag_stat) {
    TrField *fld = TrField::GetPtr();
    fld->Read("dat/MagneticFieldMap.0.dat");
  }

  if (nevt <= 0) nevt = t4->GetEntries();

  Int_t NrawMax = 200;
  Int_t NclsMax = 200;
  Int_t NhitMax =  50;

  Int_t nfill = 0; 

  TStopwatch timer;
  timer.Start();

  cout << nevt << " events to be processed" << endl;

  // Loop for each event
  for (Int_t i = 0; i < nevt; i++) {
    t4->GetEntry(i);

    if (i%1000 == 0) {
      Double_t tm = timer.RealTime();
      timer.Continue();
      cout << Form("Event: %7d  Filled: %7d  Time: %4.0f sec  Rate: %5.1f Hz", 
		   i, nfill, tm, i/tm) << endl;
    }

    if (event->NTrRawClusters < NrawMax) {
      trec->BuildTrClusters();

      if (event->NTrClusters < NclsMax) {
	trec->BuildTrRecHits();

	if (event->NTrRecHits < NhitMax) {
	  if (TrTrack::build()) {
	    t4new->Fill();
	    nfill++;
	  }
	}
      }
    }
  }

  t4new->Write();

  cout << Form("Event: %7d  Filled: %7d", nevt, nfill) << endl;
}
