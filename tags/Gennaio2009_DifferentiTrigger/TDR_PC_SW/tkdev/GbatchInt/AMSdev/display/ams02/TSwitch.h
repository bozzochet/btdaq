//  $Id: TSwitch.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef TSwitch_H
#define TSwitch_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Displayable Switches                                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TPad
#include <TPad.h>
#endif
#include "TRadioButton.h"
#ifndef ROOT_TText
#include <TText.h>
#endif

class TSwitch : public TPad {

  protected:
   TRadioButton  *fButton;	// the RadioButton displaying the logical state 
   TText         *fText;	// Text to be displayed

  public:
                  TSwitch() {}
                  TSwitch(char * title, Bool_t *state, char * method,
			     Coord_t x1, Coord_t y1, Coord_t x2, Coord_t y2,
				char * offMethod=0);
   virtual       ~TSwitch() { delete fButton; delete fText;}
           void   ExecuteEvent(Int_t event, Int_t px, Int_t py);
   TRadioButton  *GetButton() { return fButton; }

   ClassDef(TSwitch, 0)   // Displayable Switch class
};

#endif


