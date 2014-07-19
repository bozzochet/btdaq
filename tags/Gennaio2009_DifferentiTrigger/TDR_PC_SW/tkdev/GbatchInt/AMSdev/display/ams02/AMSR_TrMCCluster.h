//  $Id: AMSR_TrMCCluster.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

#ifndef AMSR_TrMCCluster_H
#define AMSR_TrMCCluster_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "AMSR_3DCluster.h"

class AMSR_TrMCCluster : public AMSR_3DCluster {

private:
   Int_t          m_Part;        // Geant Particle ID
   Float_t        m_Signal;        // Edep in MeV

   Int_t          m_NTracks;       // number of tracks in m_Tracks;
   TObjArray *    m_Tracks;        // Tracks that pass this cluster


  public:
                  AMSR_TrMCCluster() {;}
                  AMSR_TrMCCluster(Int_t part, 
				Float_t signal, 
				Float_t * coo, Float_t * ercoo,
				Int_t ntracks, TObjArray * tracks);
   virtual       ~AMSR_TrMCCluster() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py) const;
   virtual void   Paint(Option_t *option="");
   virtual void   Clear(Option_t *option="");

//      Getters
   Int_t          GetStatus()     {return m_Part;}
   Float_t        GetSignal()     {return m_Signal;}
   TObjArray *    GetTracks()     {return m_Tracks;}

   virtual Bool_t Is3D() {return kTRUE;}
//   virtual void   Sizeof3D() const;

//      Setters

//	Friends
//   friend AMSR_TrMCClusterReader;

   ClassDef(AMSR_TrMCCluster, 1)   //AMS Time of Flight Cluster
};


#endif


