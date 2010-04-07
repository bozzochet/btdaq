//  $Id: AMSR_SiHit.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

#ifndef AMSR_SiHit_H
#define AMSR_SiHit_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Silicon Hit                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "AMSR_3DCluster.h"

class AMSR_SiHit : public AMSR_3DCluster {

private:
   Int_t          m_Status;         // Status word
   Int_t          m_Plane;          // Layer number (1...6 : up ... down)
   Int_t          m_X;              // X track cluster
   Int_t          m_Y;              // Y track cluster
// Float_t	  m_Position[3];    // Hit position in global coordinate system
   Float_t	  m_ErrPosition[3]; // Error of m_Position[]
   Float_t	  m_Amplitude;	    // Amplitude
   Float_t	  m_AsymAmpl;	    // Asymmetry of amplitudes in X vs Y side
   TObjArray *    m_Tracks;         // Tracks that pass this hit


  public:
                  AMSR_SiHit() {;}
                  AMSR_SiHit(Int_t status, Int_t plane, Int_t px, Int_t py,
				Float_t * hit, Float_t * errhit, 
				Float_t ampl, Float_t asym,
				TObjArray * tracks);
   virtual       ~AMSR_SiHit() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py) const;
   virtual void   Paint(Option_t *option="");
   virtual void   Clear(Option_t *option="");

//      Getters
   Int_t          GetStatus()    {return m_Status;}
   Int_t          GetPlane()     {return m_Plane;}
   Int_t          GetX()         {return m_X;}
   Int_t          GetY()         {return m_Y;}
   Float_t        GetAmplitude() {return m_Amplitude;}
   Float_t        GetAsymAmpl()  {return m_AsymAmpl;}
   TObjArray *    GetTracks()    {return m_Tracks;}

   virtual Bool_t Is3D() {return kTRUE;}
//   virtual void   Sizeof3D() const;

//      Setters
//   void           AddTrack(AMSR_Track * track);

//	Friends
//   friend AMSR_SiHitMaker;

   ClassDef(AMSR_SiHit, 1)   //AMS Time of Flight Cluster
};

#endif


