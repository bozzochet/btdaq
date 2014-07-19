//  $Id: AMSR_Axis.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_Axis_H
#define AMSR_Axis_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Axis class                                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TPolyLine3D
#include <TPolyLine3D.h>
#endif



class AMSR_Axis : public TPolyLine3D {

private:

   char          *m_Title;	//title of the axis (usually "x", "y", "z")

public:
                  AMSR_Axis() {;}
                  AMSR_Axis(Float_t end[3], char * title);
                  AMSR_Axis(Float_t x, Float_t y, Float_t z, char * title);
   virtual       ~AMSR_Axis() { delete m_Title;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py) const;
   virtual void   Paint(Option_t *option="");

   ClassDef(AMSR_Axis, 0)   //AMS axis class
};

#endif
