//  $Id: AMSR_3DCluster.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_3DCluster_H
#define AMSR_3DCluster_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_3DCluster                                                         //
//                                                                      //
// Class for clusters.  All clusters are assumed to be in brick shapes. //
// As of 1-Dec-1997 this class is just a TMarker3DBox with my own       //
// implementation of SetPoints() and DistancetoPrimitive().             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TMarker3DBox
#include <TMarker3DBox.h>
#endif

class AMSR_Display;

class AMSR_3DCluster : public TMarker3DBox { 

protected:
   Int_t             fRange;	//Range that considered close on view pad

public:
                     AMSR_3DCluster();
                     AMSR_3DCluster(Float_t x, Float_t y, Float_t z,
                              Float_t dx, Float_t dy, Float_t dz,
                              Float_t theta, Float_t phi, Int_t range=1);
                     AMSR_3DCluster(Float_t * xyz, Float_t * dxyz, 
			      Float_t *cos, Int_t range=5);
   virtual          ~AMSR_3DCluster() {;}
   virtual void      Delete(Option_t *option="") {;}
   virtual char     *GetObjectInfo(Int_t px, Int_t py) const;
     Float_t const * GetPosition();
     Float_t const * GetSize();
//   virtual void      Paint(Option_t* option);
//   virtual void      PaintShape(X3DBuffer* buff);
   Int_t             DistancetoPrimitive(Int_t px, Int_t py);
   void              SetPoints(Float_t * buff);
   void		     SetPosition(Float_t x, Float_t y, Float_t z);
//   void		     SetSize(Float_t dx, Float_t dy, Float_t dz);
   void		     SetDirection(Float_t theta, Float_t phi);
   void		     Sizeof3D() const;

   ClassDef(AMSR_3DCluster, 0)   //Cluster class
};

#endif
