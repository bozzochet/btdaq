//  $Id: AMSR_Axis.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root Particle class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "AMSR_Axis.h"

#ifndef ROOT_TText
#include <TText.h>
#endif
#ifndef ROOT_TView
#include <TView.h>
#endif
#ifndef ROOT_TPad
#include <TPad.h>
#endif

#include "Debugger.h"

ClassImp(AMSR_Axis)

//_____________________________________________________________________________
AMSR_Axis::AMSR_Axis(Float_t x, Float_t y, Float_t z, char * title) 
     : TPolyLine3D(2)
{
   //
   // constructor for AMSR_Axis class
   //
   Float_t p[6];

   p[0] = p[1] = p[2] = 0.0;
   p[3] = x;
   p[4] = y;
   p[5] = z;

   SetPolyLine(2,p);

   m_Title = new char [strlen(title)+1];
   strcpy(m_Title,title);

   //m_View = view;
}


//_____________________________________________________________________________
AMSR_Axis::AMSR_Axis(Float_t end[3], char * title) 
     : TPolyLine3D(2)
{
   Float_t p[6];

   p[0] = p[1] = p[2] = 0.0;
   p[3] = end[0]; 
   p[4] = end[1];
   p[5] = end[2];

   SetPolyLine(2,p);

   m_Title = new char [strlen(title)+1];
   strcpy(m_Title,title);

   //m_View = view;
}


//_____________________________________________________________________________
void AMSR_Axis::Paint(Option_t *option)
{
   // Paint an axis...
   //

   TPolyLine3D::Paint(option);

   //
   // now draw the axis title
   //
   Float_t p[3], xndc[3];
   Float_t mag=1.5;
   for (Int_t i=0; i<3; i++) p[i] = mag * fP[i+3];
   debugger.Print("AMSR_Axis::Paint() with gPad = %s\n", gPad->GetName());
   TView * view = gPad->GetView();
   if (view) view->WCtoNDC(p,xndc);
   else      debugger.Print("no view associated with gPad in AMSR_Axis::Paint()\n");

   TText * text = new TText(xndc[0], xndc[1], m_Title);
   text->SetTextAlign(22);
   text->SetTextSize(0.3);
   text->Paint(option);
//   delete text;
}

//_____________________________________________________________________________
char * AMSR_Axis::GetObjectInfo(Int_t px, Int_t py) const
{
   static char info[30];
   sprintf(info,"%s axis", m_Title);

   return info;

}

