//  $Id: AMSR_TrdCluster.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

#ifndef AMSR_TrdCluster_H
#define AMSR_TrdCluster_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS TRD cluster                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "AMSR_3DCluster.h"

class AMSR_TrdCluster : public AMSR_3DCluster {

private:

  public:
                  AMSR_TrdCluster() {;}
   AMSR_TrdCluster(Float_t* coo, Float_t* ercoo);

   virtual       ~AMSR_TrdCluster() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py) const;
   virtual void   Paint(Option_t *option="");
   virtual void   Clear(Option_t *option="");

//      Getters

   virtual Bool_t Is3D() {return kTRUE;}
//   virtual void   Sizeof3D() const;

//      Setters
//	Friends
//   friend AMSR_TrdClusterMaker;

   ClassDef(AMSR_TrdCluster, 1)   //AMS TRD Cluster
};

#endif


