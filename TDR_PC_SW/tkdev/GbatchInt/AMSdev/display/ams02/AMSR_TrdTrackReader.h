#ifndef AMSR_TrdTrackReader_H
#define AMSR_TrdTrackReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root TrdTrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef AMSR_Maker_H
#include "AMSR_Maker.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif

class AMSR_TrdTrack;

class AMSR_TrdTrackReader : public AMSR_Maker {

protected:
   Int_t          m_NTrdTracks;           //Number of TrdTracks

//     Tracks histograms
   TH1F          *m_Mult;              //TrdTracks multiplicity

public:
                  AMSR_TrdTrackReader();
                  AMSR_TrdTrackReader(const char *name, const char *title);
   virtual       ~AMSR_TrdTrackReader();
   AMSR_TrdTrack      *AddTrdTrack(Int_t code, Int_t mcparticle);
   virtual void   Clear(Option_t *option="");
   virtual void   Finish();
//         void   HelixParameters(Float_t charge, Float_t *vert1, Float_t *pvert1, Float_t *b);
   virtual void   Make();
   virtual void   PrintInfo();
// virtual Bool_t Enabled(TObject * obj);

//    Getters

//     Getters TrdTracks histograms
   TH1F          *Mult() {return m_Mult;}


//    Setters for TrdTracks

   ClassDef(AMSR_TrdTrackReader, 1)   //AMSR_Root TrdTrackReader
};

#endif
