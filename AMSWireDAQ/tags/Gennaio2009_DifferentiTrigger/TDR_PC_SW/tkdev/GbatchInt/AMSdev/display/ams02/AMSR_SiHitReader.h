//  $Id: AMSR_SiHitReader.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_SiHitReader_H
#define AMSR_SiHitReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Silicion Tracker Hit Reader                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif

#include "AMSR_Maker.h"


class AMSR_SiHitReader : public AMSR_Maker {

 protected:

   Int_t          m_Nclusters;

//        histograms
//   TH1F          *m_Multiplicity;


 public:
   Bool_t         DrawUsedHitsOnly;

                  AMSR_SiHitReader() {;}
                  AMSR_SiHitReader(const char *name, const char *title);
   virtual       ~AMSR_SiHitReader() {;}
   virtual void   Finish();

   virtual void   Make();
   virtual void   PrintInfo();
   virtual Bool_t Enabled(TObject * obj);

//
//      Getters
//

//
//      Setters
//
   void           AddCluster(Int_t status, Int_t plane, Int_t px, Int_t py,
                             Float_t * hit, Float_t * errhit,
                             Float_t ampl, Float_t asym,
                             TObjArray * tracks=0);
   void           RemoveCluster(const Int_t cluster);
   void           Clear(Option_t *option="");

   ClassDef(AMSR_SiHitReader, 1)   // AMS Time of Flight Cluster Maker
};

#endif



