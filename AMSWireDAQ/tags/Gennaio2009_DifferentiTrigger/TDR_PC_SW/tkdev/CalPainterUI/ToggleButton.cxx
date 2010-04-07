
#include <TGToolTip.h>
#include "ToggleButton.h"

ClassImp(ToggleButton);

ToggleButton::ToggleButton(TGWindow *p,char *t):TGTextButton(p,t){
	fStayDown = kTRUE;
}

void ToggleButton::SetState(EButtonState state, Bool_t emit)
{
   if (state != fState) {
      if(state == kButtonUp){
      	fOptions &= ~kSunkenFrame;
      	fOptions |= kRaisedFrame;
      }
      else if(state == kButtonDown){
      	fOptions &= ~kRaisedFrame;
      	fOptions |= kSunkenFrame;
      }
      fPrevState = fState = state;

      if (emit) {
         // button signals
         EmitSignals();
      }
      DoRedraw();
   }
}

void ToggleButton::EmitSignals(Bool_t wasUp){
   if (fState == kButtonUp)   Released();            // emit Released
   if (fState == kButtonDown) Pressed();             // emit Pressed
   Clicked();                                        // emit Clicked
   Toggled(fState == kButtonDown);                   // emit Toggled
}

Bool_t ToggleButton::HandleButton(Event_t *event)
{
   if (fTip) fTip->Hide();

   if (fState == kButtonDisabled) return kTRUE;

   Bool_t in = (event->fX >= 0) && (event->fY >= 0) &&
               (event->fX <= (Int_t)fWidth) && (event->fY <= (Int_t)fHeight);

   // We don't need to check the button number as GrabButton will
   // only allow button1 events
   if (event->fType == kButtonPress) {
      fgReleaseBtn = 0;
      if (in) {
	 fOptions &= ~kRaisedFrame;
         fOptions |= kSunkenFrame;
         Pressed();
      }
   } else { // ButtonRelease
//      if (in) {
	 fState = kButtonDown;
         Released();
  	SendMessage(fMsgWindow, MK_MSG(kC_COMMAND, kCM_RADIOBUTTON),
                     fWidgetId, (Long_t) fUserData);
         fClient->ProcessLine(fCommand, MK_MSG(kC_COMMAND, kCM_RADIOBUTTON),
                              fWidgetId, (Long_t) fUserData);
         if (fState != fPrevState) {
            Clicked();
            Toggled(fState == kButtonDown);
            fPrevState = fState;
  //       }
      }
      fgReleaseBtn = fId;
   }

   DoRedraw();
   return kTRUE;
}

