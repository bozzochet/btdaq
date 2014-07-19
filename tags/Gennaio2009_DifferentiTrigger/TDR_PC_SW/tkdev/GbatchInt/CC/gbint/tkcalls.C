// $Id: tkcalls.C,v 1.6 2008/04/02 11:35:12 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  tkcalls.cc
///\brief Definition of Geant3 user calles for only tracker setup
///
/// tkkine : called at every event from GUKINE
///
/// tkstep : called at every step  from GUSTEP
///
/// tkout  : called at every event from GUOUT
///          includes digitization and reconstruction
///
/// tkclr  : called at every event from GUOUT
///          clears the current event object
///
///\date  2007/11/27 SH  First test version
///\date  2007/11/29 SH  First stable version
///$Date: 2008/04/02 11:35:12 $
///
///$Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////////

#include "tkcalls.h"
#include "Event.h"
#include "TrMCCluster.h"
#include "TrDigi.h"
#include "TrReconstruction.h"
#include "TrRawCluster.h"
#include "TrCluster.h"
#include "TrRecHit.h"
#include "TrTrack.h"
#include "TrCalDB.h"

#include "cern.h"         // for GCVOLU
#include "event.h"        // for AMSEvent
#include "dcards.h"       // for FFKEY

namespace gbtrk {

void tkkine(void) 
{
//-------------------------------------
// called at the begeinning of 
// every event from GUKINE
//-------------------------------------

  // Make a new event object
  AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
  new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0,0,0)));
}

void tkstep(void)
{
//-------------------------------------
// called at every step from GUSTEP
//-------------------------------------

  // Check if the step in the STK volume
  if (GCVOLU.nlevel > 2 && GCTRAK.destep != 0 && GCTMED.isvol != 0 && 
      GCVOLU.names[2][0] == 'S' && 
      GCVOLU.names[2][1] == 'T' && 
      GCVOLU.names[2][2] == 'K') {

    // Call a cluster generation routine
    int lvl = GCVOLU.nlevel-1;  
    TrMCCluster::sitkhits(GCVOLU.number[lvl], GCTRAK.vect,
			  GCTRAK.destep, GCTRAK.step, GCKINE.ipart);   
  }
}

/// Switch to select reconstruction codes
ERecCodes reccode = GBATCH;

/// Debug switch
int debug = 0;

void TrRawClusterBuild(void);

TrReconstruction *trec = 0;
Event *event = 0;

void tkout(void)
{
//-------------------------------------
// called at the end of 
// every event from GUOUT
//-------------------------------------

  // Digitization
//TrMCCluster ::sitknoise();

  TrDigi::clear();
  TrDigi::sitkdigi();

  TrMCCluster::clear();

  int refit = 0;

  if (!trec) {
    trec = new TrReconstruction();
    trec->SetThrSeed(0, 3.5); trec->SetThrNeig(0, 0.1);
    trec->SetThrSeed(1, 3.5); trec->SetThrNeig(1, 0.1);
    trec->SetThrProb(0);
  }

  if (!event) event = new Event;
  trec->SetEvent(event);
  TrTrack::_event = event;

  event->Clear();

  if (reccode == TKREC || reccode == TKRAW) {
    // Raw cluster conversion
    TrRawClusterBuild();

    if (reccode == TKREC) {
      // Cluster/hit reconstruction
      trec->BuildTrClusters();
      trec->BuildTrRecHits();
/*
    if (debug >= 2) {
      TrCluster::print();
      TrRecHit ::print();
    }
*/
      // Track reconstruction
      TrTrack::build(refit);

      if (debug >= 1) TrTrack::print();
    }
  }
/*
  else {
    // Cluster/hit reconstruction
    AMSTrCluster::build(refit);
    AMSTrRecHit ::build(refit);

    if (debug >= 2) {
      AMSTrCluster::print();
      AMSTrRecHit ::print();
    }

    // Track reconstruction
    AMSTrTrack::setMargin(0);
    AMSTrTrack::buildPathIntegral(refit);

    if (debug >= 1) AMSTrTrack::print();
  }
*/
}

void tkclr(void)
{
//-------------------------------------
// called at the end of 
// every event from GUOUT
//-------------------------------------

  // Clear event object
  UPool.Release(0);
  AMSEvent::gethead()->remove();
  UPool.Release(1);
  AMSEvent::sethead(0);
  UPool.erase(2000000);
}

void TrRawClusterBuild(void)
{
  // Clear event

  for (int i = 0; i < TrDigi::GetN(); i++) {
    TrDigi *p = TrDigi::At(i);

    int adrs = p->getid();
    int tkid = adrs%2000-1000;
    int side = adrs/2000;
    int strp = p->getstrip();
    int offs = (side == 0) ? 640 : 0;

    TrLadCal *tcal = TrCalDB::Head->FindCal_TkId(tkid);
    if (!tcal) continue;

    short *adc = new short[p->getnelem()];
    float *sig = new float[p->getnelem()];
    short *sta = new short[p->getnelem()];

    for (int i = 0; i < p->getnelem(); i++) {
      adc[i] = p->getarray()[i];
      sig[i] = tcal->Sigma (strp+i+offs);
      sta[i] = tcal->Status(strp+i+offs);
    }

    // Convert AMSTrRawCluster into TrRawCluster
    TrRawCluster *cls 
      = new TrRawCluster(tkid, strp+offs, p->getnelem(), adc, sig, sta);
    event->AddTrRawCluster(cls);

    delete [] adc;
    delete [] sig;
    delete [] sta;
  }
}

}  // End of namespace gbtrk
