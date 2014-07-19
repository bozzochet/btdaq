//  $Id: AMSR_ParticleReader.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_ParticleReader_H
#define AMSR_ParticleReader_H

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

class AMSR_Particle;

class AMSR_ParticleReader : public AMSR_Maker {

protected:
   Int_t          m_NParticles;           //Number of tracks

//     Particles histograms
   TH1F          *m_Mult;              //tracks multiplicity

public:
                  AMSR_ParticleReader();
                  AMSR_ParticleReader(const char *name, const char *title);
   virtual       ~AMSR_ParticleReader();
   AMSR_Particle   *AddParticle(Int_t code, Int_t mcparticle);
   virtual void   Clear(Option_t *option="");
   virtual void   Finish();
//         void   HelixParameters(Float_t charge, Float_t *vert1, Float_t *pvert1, Float_t *b);
   virtual void   Make();
   virtual void   PrintInfo();
// virtual Bool_t Enabled(TObject * obj);

//    Getters

//     Getters Tracks histograms
   TH1F          *Mult() {return m_Mult;}


//    Setters for tracks

   ClassDef(AMSR_ParticleReader, 1)   //AMSR_Root Particle Reader
};

#endif
