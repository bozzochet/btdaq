//  $Id: AMSR_MCParticle.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_MCParticle_H
#define AMSR_MCParticle_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root particle class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include <THelix.h>

class AMSR_MCParticleReader;


//class AMSR_MCParticle : public AMSR_Track {
class AMSR_MCParticle : public THelix {

private:

   Int_t	m_GeantID;		//Particle ID a la Geant
   Float_t	m_Mass;			//Mass
   Float_t	m_Momentum;		//Momentum
   Float_t	m_Charge;		//Charge
   Float_t	m_Position[3];		//Track Position (on entry)
   Float_t	m_Dir[3];		//Track Position (on entry)

public:
                  AMSR_MCParticle() {;}
   virtual       ~AMSR_MCParticle() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py) const;
           void   SetHelix();
   virtual void   Paint(Option_t *option="");

   //
   // Getters and Setters...
   //
   friend class AMSR_MCParticleReader;

   ClassDef(AMSR_MCParticle, 1)   //AMSR_Root track class
};

#endif
