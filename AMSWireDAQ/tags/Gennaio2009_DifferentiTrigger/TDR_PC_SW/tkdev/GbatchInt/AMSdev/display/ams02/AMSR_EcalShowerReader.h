//  $Id: AMSR_EcalShowerReader.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_EcalShowerReader_H
#define AMSR_EcalShowerReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root TrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef AMSR_Maker_H
#include "AMSR_Maker.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif

class AMSR_EcalShower;

class AMSR_EcalShowerReader : public AMSR_Maker {

protected:
   Int_t          m_NTracks;           //Number of tracks
/*
   Float_t        m_MinPT;             //Minimum PT for track
   Float_t        m_MaxEta;            //maximum eta for track
*/

//     Tracks histograms
   TH1F          *m_Mult;              //tracks multiplicity

public:
                  AMSR_EcalShowerReader();
                  AMSR_EcalShowerReader(const char *name, const char *title);
   virtual       ~AMSR_EcalShowerReader();
   AMSR_EcalShower      *AddTrack();
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

   ClassDef(AMSR_EcalShowerReader, 1)   //AMSR_Root TrackReader
};

#endif
