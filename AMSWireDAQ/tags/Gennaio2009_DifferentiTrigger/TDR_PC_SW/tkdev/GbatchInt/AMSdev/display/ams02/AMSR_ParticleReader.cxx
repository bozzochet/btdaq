//  $Id: AMSR_ParticleReader.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root TrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_ParticleReader.h"
#include "AMSR_Particle.h"
#include "AMSR_Ntuple.h"

const Float_t kPI   = TMath::Pi();
const Float_t k2PI  = 2*kPI;
const Int_t kMAXTRA = 100;

ClassImp(AMSR_ParticleReader)


//_____________________________________________________________________________
AMSR_ParticleReader::AMSR_ParticleReader()
{
   m_NParticles = 0;
}

//_____________________________________________________________________________
AMSR_ParticleReader::AMSR_ParticleReader(const char *name, const char *title)
                 :AMSR_Maker(name,title)
{
//    Default Setters for tracks

   m_Fruits     = new TClonesArray("AMSR_Particle", 3, kFALSE);
   m_BranchName = "Particles";
   m_NParticles    = 0;
// Please, how to do this optionally ??!!!
   Save();
}

//_____________________________________________________________________________
AMSR_ParticleReader::~AMSR_ParticleReader()
{
   //dummy
}

//_____________________________________________________________________________
AMSR_Particle *AMSR_ParticleReader::AddParticle(Int_t code, Int_t mcparticle)
{
//            Add a new particle to the list of particles

 //Note the use of the "new with placement" to create a new track object.
 //This complex "new" works in the following way:
 //   tracks[i] is the value of the pointer for track number i in the TClonesArray
 //   if it is zero, then a new track must be generated. This typically
 //   will happen only at the first events
 //   If it is not zero, then the already existing object is overwritten
 //   by the new track parameters.
 // This technique should save a huge amount of time otherwise spent
 // in the operators new and delete.

   TClonesArray &particles = *(TClonesArray*)m_Fruits;
   return new(particles[m_NParticles++]) AMSR_Particle();
}

//_____________________________________________________________________________
void AMSR_ParticleReader::Clear(Option_t *option)
{
//    Reset Track Maker

   m_NParticles = 0;
   if (m_Fruits) m_Fruits->Clear();
}


//_____________________________________________________________________________
void AMSR_ParticleReader::Finish()
{
// Function called when all makers for one event have been called

}

//_____________________________________________________________________________
void AMSR_ParticleReader::Make()
{

   Int_t k, i;
   PARTICLE_DEF *_ntuple  = (gAMSR_Root->GetNtuple())->m_BlkParticle;
   BETA_DEF *beta_ntp     = (gAMSR_Root->GetNtuple())->m_BlkBeta;
   TRTRACK_DEF *track_ntp = (gAMSR_Root->GetNtuple())->m_BlkTrtrack;

   m_NParticles = _ntuple->npart;

   //   m_NParticles=0;
   debugger.Print("AMSR_ParticleReader::Make(): making %d particles.\n", m_NParticles);
   TClonesArray &particles = *(TClonesArray*)m_Fruits;
   for (k=0; k<m_NParticles; k++) {
      debugger.Print("Making particle #%d:\n",k);

      new(particles[k]) AMSR_Particle();
      AMSR_Particle * t = (AMSR_Particle *) particles[k];
/* need to be updated according to the AMS ntuple structure, 15/12/98, YeSW
*/
      t->m_PBeta            = _ntuple->pbetap[k];
      t->m_PCharge          = _ntuple->pchargep[k];
      t->m_PTrack           = _ntuple->ptrackp[k];
      t->m_GeantID          = _ntuple->pid[k];
      t->m_Mass             = _ntuple->pmass[k];
      t->m_ErrMass          = _ntuple->perrmass[k];
      t->m_Momentum         = _ntuple->pmom[k];
      t->m_ErrMomentum      = _ntuple->perrmom[k];
      t->m_Charge           = _ntuple->pcharge[k];
      t->m_Theta            = _ntuple->ptheta[k];
      t->m_Phi              = _ntuple->pphi[k];
      t->m_beta             = _ntuple->pbeta[k];
      t->m_errbeta          = _ntuple->perrbeta[k];
     t->m_betapattern      = t->m_PBeta>0?beta_ntp->betapattern[t->m_PBeta-1]:-1;
      t->m_pattern          = t->m_PTrack>0?track_ntp->pattern[t->m_PTrack-1]:-1;
      t->m_trstatus         = t->m_PTrack>0?track_ntp->trstatus[t->m_PTrack-1]:-1;
      for (i=0; i<3; i++)
	{
          t->m_Position[i]  = _ntuple->pcoo[k][i];
	}
      debugger.Print("Theta, phi,pos %f %f %f %f %f:\n",
              t->m_Theta, t->m_Phi, t->m_Position[0],
              t->m_Position[1],t->m_Position[2]);

      t->SetHelix();
   }

//   m_Mult->Fill(m_NParticles+0.01);
}

//_____________________________________________________________________________
void AMSR_ParticleReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSR_ParticleReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation among particles
}
*/

