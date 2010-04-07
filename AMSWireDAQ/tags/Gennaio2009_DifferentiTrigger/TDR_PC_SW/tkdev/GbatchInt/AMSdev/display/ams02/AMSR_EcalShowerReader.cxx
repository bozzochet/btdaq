//  $Id: AMSR_EcalShowerReader.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root TrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
//#include "ATLFMCMaker.h"
#include "AMSR_EcalShowerReader.h"
#include "AMSR_EcalShower.h"
#include "AMSR_Ntuple.h"

const Float_t kPI   = TMath::Pi();
const Float_t k2PI  = 2*kPI;
const Int_t kMAXTRA = 100;

const Int_t maxtr=20;

ClassImp(AMSR_EcalShowerReader)


//_____________________________________________________________________________
AMSR_EcalShowerReader::AMSR_EcalShowerReader()
{
   m_NTracks = 0;
}

//_____________________________________________________________________________
AMSR_EcalShowerReader::AMSR_EcalShowerReader(const char *name, const char *title)
                 :AMSR_Maker(name,title)
{
//    Default Setters for tracks

   m_Fruits     = new TClonesArray("AMSR_EcalShower",maxtr, kFALSE);
   m_BranchName = "EcalShowers";
   m_NTracks    = 0;
// Please, how to do this optionally ??!!!
   Save();
}

//_____________________________________________________________________________
AMSR_EcalShowerReader::~AMSR_EcalShowerReader()
{
   //dummy
}

//_____________________________________________________________________________
AMSR_EcalShower *AMSR_EcalShowerReader::AddTrack()
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
   return new(tracks[m_NTracks++]) AMSR_EcalShower();
   //   return new(tracks[m_NTracks++]) AMSR_EcalShower();
}

//_____________________________________________________________________________
void AMSR_EcalShowerReader::Clear(Option_t *option)
{
//    Reset Track Maker

   m_NTracks = 0;
   if (m_Fruits) m_Fruits->Clear();
}


//_____________________________________________________________________________
void AMSR_EcalShowerReader::Finish()
{
// Function called when all makers for one event have been called

}

//_____________________________________________________________________________
void AMSR_EcalShowerReader::Make()
{

//   cout <<" here in the make odnako"<<endl;

   Int_t k, i;
   ECALSHOWER_DEF *_ntuple = (gAMSR_Root->GetNtuple())->m_Blkecsh;
   
   m_NTracks = _ntuple->Necsh;
   if(m_NTracks >= maxtr)m_NTracks=maxtr;
   debugger.Print("AMSR_EcalShowerReader::Make(): making %d tracks.\n", m_NTracks);
   TClonesArray &tracks = *(TClonesArray*)m_Fruits;
   for (k=0; k<m_NTracks; k++) {

      new(tracks[k]) AMSR_EcalShower();
      AMSR_EcalShower * t = (AMSR_EcalShower *) tracks[k];
      t->m_ID              = k + 1;	    // in ntuple it starts at 1 not 0
      t->m_Status=_ntuple->Status[k];
      t->m_Energy=_ntuple->EnergyC[k];
     
      t->m_ErrEnergy=_ntuple->ErEnergyC[k];
      t->m_FChi2=_ntuple->Chi2Dir[k];
      t->m_FTheta=acos(_ntuple->EMDir[k][2]);
      t->m_FPhi=atan2(_ntuple->EMDir[k][1],_ntuple->EMDir[k][0]);
      for(int i=0;i<3;i++)t->m_FP0[i]=_ntuple->Entry[k][i];
      t->SetHelix();
//      cout <<t->m_Energy<<" "<<t->m_FTheta<<" "<<t->m_FPhi<<" "<<t->m_FChi2<<endl;
   }
   debugger.Print("AMSR_EcalShowerReader::Make(): %d tracks made.\n", m_NTracks);

   // m_Mult->Fill(m_NTracks+0.01);		// somehow this line is generating segmenation violations...
//   cout <<" out here in the make odnako"<<endl;
}

//_____________________________________________________________________________
void AMSR_EcalShowerReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSR_EcalShowerReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation among particles
}
*/

