//  $Id: AMSR_MCParticleReader.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_MCParticleReader_H
#define AMSR_MCParticleReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root Particle Reader class.                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef AMSR_Maker_H
#include "AMSR_Maker.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif

class AMSR_MCParticle;

class AMSR_MCParticleReader : public AMSR_Maker {

protected:
   Int_t          m_NParticles;           //Number of tracks


public:
                  AMSR_MCParticleReader();
                  AMSR_MCParticleReader(const char *name, const char *title);
   virtual       ~AMSR_MCParticleReader();
   AMSR_MCParticle   *AddParticle(Int_t code, Int_t mcparticle);
   virtual void   Clear(Option_t *option="");
   virtual void   Finish();
   virtual void   Make();
   virtual void   PrintInfo();

//    Getters



//    Setters for tracks

   ClassDef(AMSR_MCParticleReader, 1)   //AMSR_Root Particle Reader
};

#endif
