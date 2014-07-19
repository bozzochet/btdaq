//  $Id: AMSR_TrdClusterReader.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_TrdClusterReader_H
#define AMSR_TrdClusterReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS TRD Cluster Reader                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif

#include "AMSR_Maker.h"


class AMSR_TrdClusterReader : public AMSR_Maker {

private:

   Int_t          m_Nclusters;

//        histograms
//   TH1F          *m_Multiplicity;


  public:
                  AMSR_TrdClusterReader() {;}
                  AMSR_TrdClusterReader(const char *name, const char *title);
   virtual       ~AMSR_TrdClusterReader() {;}
   virtual void   Finish();

   virtual void   Make();
   virtual void   PrintInfo();
// virtual Bool_t Enabled(TObject * obj);

//
//      Getters
//

//
//      Setters
//
   void           AddCluster(Int_t status, 
                             Float_t * coo, Int_t layer,
                             Float_t * coodir, Int_t multip, Int_t hmultip,
                             Float_t edep, Int_t prawhit);
   void           RemoveCluster(const Int_t cluster);
   void           Clear(Option_t *option="");

   ClassDef(AMSR_TrdClusterReader, 1)   // AMS Time of Flight Cluster Maker
};

#endif



