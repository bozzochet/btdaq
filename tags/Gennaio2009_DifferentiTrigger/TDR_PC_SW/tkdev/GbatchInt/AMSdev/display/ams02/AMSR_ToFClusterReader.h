//  $Id: AMSR_ToFClusterReader.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_ToFClusterReader_H
#define AMSR_ToFClusterReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Time of Flight Cluster Reader                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif

#include "AMSR_Maker.h"


class AMSR_ToFClusterReader : public AMSR_Maker {

private:

   Int_t          m_Nclusters;

//        histograms
//   TH1F          *m_Multiplicity;


  public:
                  AMSR_ToFClusterReader() {;}
                  AMSR_ToFClusterReader(const char *name, const char *title);
   virtual       ~AMSR_ToFClusterReader() {;}
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
   void           AddCluster(Int_t status, Int_t plane, Int_t bar,
                             Float_t energy, Float_t time, Float_t ertime,
                             Float_t * coo, Float_t * ercoo,
                             Int_t ncells=0, Int_t npart=0,
                             Int_t ntracks=0, TObjArray * tracks=0);
   void           RemoveCluster(const Int_t cluster);
   void           Clear(Option_t *option="");

   ClassDef(AMSR_ToFClusterReader, 1)   // AMS Time of Flight Cluster Maker
};

#endif



