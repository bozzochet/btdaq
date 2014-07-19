
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root TrdTrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
//#include "ATLFMCMaker.h"
#include "AMSR_TrdTrackReader.h"
#include "AMSR_TrdTrack.h"
#include "AMSR_Ntuple.h"

const Float_t kPI   = TMath::Pi();
const Float_t k2PI  = 2*kPI;
const Int_t kMAXTRA = 100;

const Int_t maxtrdtr=40;

ClassImp(AMSR_TrdTrackReader)


//_____________________________________________________________________________
AMSR_TrdTrackReader::AMSR_TrdTrackReader()
{
   m_NTrdTracks = 0;
}

//_____________________________________________________________________________
AMSR_TrdTrackReader::AMSR_TrdTrackReader(const char *name, const char *title)
                 :AMSR_Maker(name,title)
{
//    Default Setters for TrdTracks

   m_Fruits     = new TClonesArray("AMSR_TrdTrack",maxtrdtr, kFALSE);
   m_BranchName = "TrdTracks";
   m_NTrdTracks    = 0;
// Please, how to do this optionally ??!!!
   Save();
}

//_____________________________________________________________________________
AMSR_TrdTrackReader::~AMSR_TrdTrackReader()
{
   //dummy
}

//_____________________________________________________________________________
AMSR_TrdTrack *AMSR_TrdTrackReader::AddTrdTrack(Int_t code, Int_t mcparticle)
{
//            Add a new TrdTrack to the list of TrdTracks

 //Note the use of the "new with placement" to create a new TrdTrack object.
 //This complex "new" works in the following way:
 //   tracks[i] is the value of the pointer for track number i in the TClonesArray
 //   if it is zero, then a new track must be generated. This typically
 //   will happen only at the first events
 //   If it is not zero, then the already existing object is overwritten
 //   by the new track parameters.
 // This technique should save a huge amount of time otherwise spent
 // in the operators new and delete.

   TClonesArray &TrdTracks = *(TClonesArray*)m_Fruits;
   return new(TrdTracks[m_NTrdTracks++]) AMSR_TrdTrack(code,mcparticle);
   //   return new(TrdTracks[m_NTrdTracks++]) AMSR_TrdTrack(code,mcparticle);
}

//_____________________________________________________________________________
void AMSR_TrdTrackReader::Clear(Option_t *option)
{
//    Reset TrdTrack Maker

   m_NTrdTracks = 0;
   if (m_Fruits) m_Fruits->Clear();
}


//_____________________________________________________________________________
void AMSR_TrdTrackReader::Finish()
{
// Function called when all makers for one event have been called

}

//_____________________________________________________________________________
void AMSR_TrdTrackReader::Make()
{

   Int_t k, i;
   TRDTRK_DEF *_ntuple = (gAMSR_Root->GetNtuple())->m_BlkTrdtrk;
   
   m_NTrdTracks = _ntuple->ntrdtrk;
   if(m_NTrdTracks >= maxtrdtr)m_NTrdTracks=maxtrdtr;
   debugger.Print("AMSR_TrdTrackReader::Make(): making %d TrdTracks.\n", m_NTrdTracks);
   TClonesArray &TrdTracks = *(TClonesArray*)m_Fruits;
   for (k=0; k<m_NTrdTracks; k++) {
      debugger.Print("Making TrdTrack #%d: status %d, pattern %d\n",
		k, _ntuple->status[k], _ntuple->pat[k]);

      new(TrdTracks[k]) AMSR_TrdTrack(_ntuple->status[k], _ntuple->pat[k]);
      AMSR_TrdTrack * t = (AMSR_TrdTrack *) TrdTracks[k];
      t->m_ID              = k + 1;	    // in ntuple it starts at 1 not 0

      for(int i=0;i<3;i++) t->m_Coo[i] = _ntuple->coo[k][i];
      for(int i=0;i<3;i++) t->m_ErCoo[i] = _ntuple->ercoo[k][i];

      t->m_Theta = _ntuple->theta[k];
      t->m_Phi = _ntuple->phi[k];

      t->SetHelix();
   }
   debugger.Print("AMSR_TrdTrackReader::Make(): %d TrdTracks made.\n", m_NTrdTracks);

}

//_____________________________________________________________________________
void AMSR_TrdTrackReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

