//  $Id: TRadioButton.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef TRadioButton_H
#define TRadioButton_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Displayable Toggle                                                   //
//                                                                      //
// A TRadioButton can have two states:  Pressed or Unpressed            //
// When it is pressed, the externally supplied boolean variable is set  //
// to kTRUE and the method is called.  When it is unpressed, the        //
// boolean variable is set to kFALSE and offMethod is called if it is   //
// defined, otherwise method is called.  In this case the method should //
// be aware of the boolean variable.                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TPad
#include <TPad.h>
#endif
#ifndef ROOT_TButton
#include <TButton.h>
#endif
#ifndef ROOT_TText
#include <TText.h>
#endif

class TRadioButton : public TButton {

  protected:
   Bool_t        *fState;       // Point to externally supplied state word
   char          *fOffMethod;	// method to execute when toggled "off"

  public:
                  TRadioButton() {}
                  TRadioButton(Bool_t *state, char * onMethod, 
			  Coord_t x1, Coord_t y1, Coord_t x2, Coord_t y2,
			  char * offMethod = 0);
   virtual       ~TRadioButton() {} 
           void   ExecuteEvent(Int_t event, Int_t px, Int_t py);

   ClassDef(TRadioButton, 0)   // Displayable toggle class
};

#endif


