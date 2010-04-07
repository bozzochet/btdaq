//  $Id: TRadioButton.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Displayable Toggle class                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TCanvas.h>
#include <TInterpreter.h>

#include "TRadioButton.h"
#include "Debugger.h"

ClassImp(TRadioButton)


//_____________________________________________________________________________
TRadioButton::TRadioButton(Bool_t *state, char * onMethod, 
		 Coord_t x1, Coord_t y1, Coord_t x2, Coord_t y2,
		 char * offMethod)
  : TButton("", onMethod, x1, y1, x2, y2) 
{
   debugger.Print("initializing TRadioButton.\n");

   fState     = state;
   fOffMethod = offMethod;

   if (*fState) SetBorderMode(-1);
   else         SetBorderMode(1);

}


//______________________________________________________________________________
void  TRadioButton::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*
//*-*                  =========================================
//  This member function is called when a Button object is clicked.
//

   char command[128];
   if (fMother->GetCanvas()->IsEditable()) {
      TPad::ExecuteEvent(event,px,py);
      return;
   }

   switch (event) {
   case kButton1Down:
      if (*fState)  SetBorderMode(1);		// kTRUE  -> kFALSE
      else          SetBorderMode(-1);		// kFALSE -> kTRUE
      *fState = (! *fState);
//    fFocused=1;
      Modified();
      GetCanvas()->Modified();
      Update();
      break;

   case kMouseMotion:

      break;

   case kButton1Motion:
	/*
      if (px<XtoAbsPixel(1) && px>XtoAbsPixel(0) &&
          py<YtoAbsPixel(0) && py>YtoAbsPixel(1)) {
         if (!fFocused) {
            SetBorderMode(-1);
            fFocused=1;
            Modified();
            GetCanvas()->Modified();
            Update();
         }
      } else if (fFocused) {
         SetBorderMode(1);
         fFocused=0;
         Modified();
         GetCanvas()->Modified();
         Update();
      }
	*/
      break;

   case kButton1Up:
      // if (fFocused) gInterpreter->ProcessLine(GetMethod());
      if ( ! *fState && fOffMethod != 0) gInterpreter->ProcessLine(fOffMethod);
      else                               gInterpreter->ProcessLine(GetMethod());

      break;
   }
}
