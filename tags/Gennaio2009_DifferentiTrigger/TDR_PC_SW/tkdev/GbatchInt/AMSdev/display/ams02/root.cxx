//  $Id: root.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

#include "EventRoot02.h"
#include "AMSEventHeaderRoot.h"
#include "TRDClusterRoot.h"
#include "MCEventGRoot02.h"
#include "TRDTrackRoot.h"
#include "TrTrackRoot02.h"

EventRoot02::EventRoot02() {
   fBeta = 0;
   fCharge = 0;
   fParticle = 0;
   fTOFcluster = 0;
   fECALshower = 0;
   fECALcluster = 0;
   fECAL2Dcluster = 0;
   fECALhit = 0;
   fTOFMCcluster = 0;
   fTrCluster = 0;
   fTrMCCluster = 0;
   fTRDMCCluster = 0;
   fTRDrawhit = 0;
   fTRDcluster = 0;
   fTRDsegment = 0;
   fTRDtrack = 0;
   fTRrechit = 0;
   fTRtrack = 0;
   fMCtrtrack = 0;
   fMCeventg = 0;
   fAntiCluster = 0;
   fAntiMCCluster = 0;
   fLVL3 = 0;
   fLVL1 = 0;
   fTrRawCluster = 0;
   fAntiRawCluster = 0;
   fTOFRawCluster = 0;
   fRICMC = 0;
   fRICEvent = 0;
   fRICRing = 0;
}

EventRoot02::~EventRoot02() {
   delete fBeta;   fBeta = 0;
   delete fCharge;   fCharge = 0;
   delete fParticle;   fParticle = 0;
   delete fTOFcluster;   fTOFcluster = 0;
   delete fECALshower;   fECALshower = 0;
   delete fECALcluster;   fECALcluster = 0;
   delete fECAL2Dcluster;   fECAL2Dcluster = 0;
   delete fECALhit;   fECALhit = 0;
   delete fTOFMCcluster;   fTOFMCcluster = 0;
   delete fTrCluster;   fTrCluster = 0;
   delete fTrMCCluster;   fTrMCCluster = 0;
   delete fTRDMCCluster;   fTRDMCCluster = 0;
   delete fTRDrawhit;   fTRDrawhit = 0;
   delete fTRDcluster;   fTRDcluster = 0;
   delete fTRDsegment;   fTRDsegment = 0;
   delete fTRDtrack;   fTRDtrack = 0;
   delete fTRrechit;   fTRrechit = 0;
   delete fTRtrack;   fTRtrack = 0;
   delete fMCtrtrack;   fMCtrtrack = 0;
   delete fMCeventg;   fMCeventg = 0;
   delete fAntiCluster;   fAntiCluster = 0;
   delete fAntiMCCluster;   fAntiMCCluster = 0;
   delete fLVL3;   fLVL3 = 0;
   delete fLVL1;   fLVL1 = 0;
   delete fTrRawCluster;   fTrRawCluster = 0;
   delete fAntiRawCluster;   fAntiRawCluster = 0;
   delete fTOFRawCluster;   fTOFRawCluster = 0;
   delete fRICMC;   fRICMC = 0;
   delete fRICEvent;   fRICEvent = 0;
   delete fRICRing;   fRICRing = 0;
}


AMSEventHeaderRoot::AMSEventHeaderRoot() { }

AMSEventHeaderRoot::~AMSEventHeaderRoot() { }

TRDClusterRoot::TRDClusterRoot() { } 
 
TRDClusterRoot::~TRDClusterRoot() { } 

MCEventGRoot02::MCEventGRoot02() { }

MCEventGRoot02::~MCEventGRoot02() { }

TRDTrackRoot::TRDTrackRoot() {
   fTRDSegment = 0;
}

TRDTrackRoot::~TRDTrackRoot() {
   delete fTRDSegment;   fTRDSegment = 0;
}

TrTrackRoot02::TrTrackRoot02() {
   fTrRecHit = 0;
}

TrTrackRoot02::~TrTrackRoot02() {
   delete fTrRecHit;   fTrRecHit = 0;
}


ClassImp(EventRoot02)
ClassImp(AMSEventHeaderRoot)
ClassImp(TRDClusterRoot)
ClassImp(MCEventGRoot02)
ClassImp(TRDTrackRoot)
ClassImp(TrTrackRoot02)


