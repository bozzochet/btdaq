//  $Id: AMSR_TrackReader.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root TrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_Types.h"
//#include "ATLFMCMaker.h"
#include "AMSR_TrackReader.h"
#include "AMSR_Track.h"
#include "AMSR_Ntuple.h"
#include "TrTrackRoot02.h"
const Float_t kPI   = TMath::Pi();
const Float_t k2PI  = 2*kPI;
const Int_t kMAXTRA = 100;

const Int_t maxtr=20;

ClassImp(AMSR_TrackReader)


//_____________________________________________________________________________
AMSR_TrackReader::AMSR_TrackReader()
{
   m_NTracks = 0;
}

//_____________________________________________________________________________
AMSR_TrackReader::AMSR_TrackReader(const char *name, const char *title)
                 :AMSR_Maker(name,title)
{
//    Default Setters for tracks

   m_Fruits     = new TClonesArray("AMSR_Track",maxtr, kFALSE);
   m_BranchName = "Tracks";
   m_NTracks    = 0;
// Please, how to do this optionally ??!!!
   Save();
}

//_____________________________________________________________________________
AMSR_TrackReader::~AMSR_TrackReader()
{
   //dummy
}

//_____________________________________________________________________________
AMSR_Track *AMSR_TrackReader::AddTrack(Int_t code, Int_t mcparticle)
{
//            Add a new track to the list of tracks

 //Note the use of the "new with placement" to create a new track object.
 //This complex "new" works in the following way:
 //   tracks[i] is the value of the pointer for track number i in the TClonesArray
 //   if it is zero, then a new track must be generated. This typically
 //   will happen only at the first events
 //   If it is not zero, then the already existing object is overwritten
 //   by the new track parameters.
 // This technique should save a huge amount of time otherwise spent
 // in the operators new and delete.

   TClonesArray &tracks = *(TClonesArray*)m_Fruits;
   return new(tracks[m_NTracks++]) AMSR_Track(code,mcparticle);
   //   return new(tracks[m_NTracks++]) AMSR_Track(code,mcparticle);
}

//_____________________________________________________________________________
void AMSR_TrackReader::Clear(Option_t *option)
{
//    Reset Track Maker

   m_NTracks = 0;
   if (m_Fruits) m_Fruits->Clear();
}


//_____________________________________________________________________________
void AMSR_TrackReader::Finish()
{
// Function called when all makers for one event have been called

}

//_____________________________________________________________________________
void AMSR_TrackReader::Make()
{

   EDataFileType m_DataFileType=gAMSR_Root->GetDataFileType();    
   Int_t k, i;
   if (m_DataFileType == kNtupleFile) {
     TRTRACK_DEF *_ntuple = (gAMSR_Root->GetNtuple())->m_BlkTrtrack;
   
     m_NTracks = _ntuple->ntrtr;
     if(m_NTracks >= maxtr)m_NTracks=maxtr;
     debugger.Print("AMSR_TrackReader::Make(): making %d tracks.\n", m_NTracks);
     TClonesArray &tracks = *(TClonesArray*)m_Fruits;
     for (k=0; k<m_NTracks; k++) {
        debugger.Print("Making track #%d: status %d, pattern %d, %d hits\n",
	 	k, _ntuple->trstatus[k], _ntuple->pattern[k], _ntuple->nhits[k]);

       new(tracks[k]) AMSR_Track(_ntuple->trstatus[k], _ntuple->pattern[k]);
       AMSR_Track * t = (AMSR_Track *) tracks[k];
       t->m_ID              = k + 1;	    // in ntuple it starts at 1 not 0
       t->m_NHits           = _ntuple->nhits[k];
       for (i=0; i<6; i++)
	 t->m_PHits[i]      = _ntuple->phits[k][i];
      //      t->m_FastFitDone     = _ntuple->Fastfitdone[k];
         t->m_GeaneFitDone    = _ntuple->Geanefitdone[k];
         t->m_AdvancedFitDone = _ntuple->Advancedfitdone[k];
         t->m_Chi2StrLine     = _ntuple->Chi2strline[k];
         t->m_Chi2Circle      = _ntuple->Chi2circle[k];
         t->m_CircleRigidity  = _ntuple->Circleridgidity[k];

         t->m_FChi2           = _ntuple->Chi2fastfit[k];
         t->m_FRigidity       = _ntuple->Ridgidity[k];
         t->m_FErrRigidity    = _ntuple->Errridgidity[k];
         t->m_FTheta          = _ntuple->Theta[k];
         t->m_FPhi            = _ntuple->phi[k];
         for(i=0; i<3; i++)
           t->m_FP0[i]        = _ntuple->p0[k][i];

           t->m_GChi2           = _ntuple->gchi2[k];
           t->m_GRigidity       = _ntuple->gridgidity[k];
           t->m_GErrRigidity    = _ntuple->gerrridgidity[k];
//      t->m_GTheta          = _ntuple->gtheta[k];
//      t->m_GPhi            = _ntuple->gphi[k];
//      for(i=0; i<3; i++)
//        t->m_GP0[i]        = _ntuple->gp0[k][i];

        for (i=0; i<2; i++) {
           t->m_HChi2[i]        = _ntuple->hchi2[k][i];
           t->m_HRigidity [i]   = _ntuple->Hridgidity[k][i];
           t->m_HErrRigidity[i] = _ntuple->Herrridgidity[k][i];
           t->m_HTheta[i]       = _ntuple->htheta[k][i];
           t->m_HPhi[i]         = _ntuple->hphi[k][i];
//	problem with 3d arrays
//      for(j=0; j<3; j++)
//           t->m_GP0[i][j]     = _ntuple->hp0[k][i][j];
         }

      //      t->m_FChi2MS         = _ntuple->fchi2ms[k];
      // t->m_GChi2MS         = _ntuple->gchi2ms[k];
         t->m_RigidityMS      = _ntuple->ridgidityms[k];
      //      t->m_GRigidityMS     = _ntuple->gridgidityms[k];

         debugger.Print("Theta, phi,pos %f %f %f %f %f:\n",
              t->m_FTheta, t->m_FPhi, t->m_FP0[0],
              t->m_FP0[1],t->m_FP0[2]);


         t->SetHelix();
      }
   }
   else if (m_DataFileType == kRootFile){

     TClonesArray *_roottree = (gAMSR_Root->GetNtuple())->m_trdtrk;

     m_NTracks = _roottree->GetEntries();
     if(m_NTracks >= maxtr)m_NTracks=maxtr;

     debugger.Print("AMSR_TrackReader::Make(): making %d tracks.\n", m_NTracks);
      TClonesArray &tracks = *(TClonesArray*)m_Fruits;

      TObject *element=_roottree->At(0); 
      TrTrackRoot02* p_ok=dynamic_cast<TrTrackRoot02*>(element); 

      if (p_ok) {
       for (k=0; k<m_NTracks;k++) {
         debugger.Print("Making particle #%d:\n",k);
 
         new(tracks[k]) AMSR_Track(p_ok->Status, p_ok->Pattern);
         AMSR_Track * t = (AMSR_Track *) tracks[k];

	 element=_roottree->At(k);             
         TrTrackRoot02* p_obj=dynamic_cast<TrTrackRoot02*>(element);

 // Need to fill in here...

       } 
      }



   }
   debugger.Print("AMSR_TrackReader::Make(): %d tracks made.\n", m_NTracks);

   // m_Mult->Fill(m_NTracks+0.01);		// somehow this line is generating segmenation violations...
}

//_____________________________________________________________________________
void AMSR_TrackReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSR_TrackReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation among particles
}
*/

