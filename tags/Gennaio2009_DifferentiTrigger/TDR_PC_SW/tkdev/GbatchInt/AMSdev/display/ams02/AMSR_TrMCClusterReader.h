//  $Id: AMSR_TrMCClusterReader.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_TRMCClusterReader_H
#define AMSR_TRMCClusterReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster Reader                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif

#include "AMSR_Maker.h"


class AMSR_TrMCClusterReader : public AMSR_Maker {

private:

   Int_t          m_Nclusters;

//        histograms
//   TH1F          *m_Multiplicity;


  public:
                  AMSR_TrMCClusterReader() {;}
                  AMSR_TrMCClusterReader(const char *name, const char *title);
   virtual       ~AMSR_TrMCClusterReader() {;}
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
   void           AddCluster(Int_t part, 
                             Float_t signal, 
                             Float_t * coo, Float_t * ercoo,
                             Int_t ntracks=0, TObjArray * tracks=0);
   void           RemoveCluster(const Int_t cluster);
   void           Clear(Option_t *option="");

   ClassDef(AMSR_TrMCClusterReader, 1)   // AMS Time of Flight Cluster Maker
};

#endif



