#include "TDRSignalHandler.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>

Bool_t TDRSignalHandler::Notify()
{
  Int_t aMessage;

  switch (fSignal){
  case kSigBus:
    break;
  case kSigSegmentationViolation:
    break;
  case kSigSystem:
    break;
  case kSigPipe:
    break;
  case kSigIllegalInstruction:
    break;
  case kSigQuit:
    break;
  case kSigInterrupt:
    break;
  case kSigWindowChanged:
    break;
  case kSigAlarm:
    break;
  case kSigChild:
    break;
  case kSigUrgent:
    break;
  case kSigFloatingException:
    break;
  case kSigTermination:
    break;
  case kSigUser1:
    aMessage=MK_MSG((EWidgetMessageTypes)kC_TDRSIGNAL,
		    (EWidgetMessageTypes)0);
    SendMessage(fMain,aMessage,0,0);
    break;
  case kSigUser2:
    break;
  }
  return kTRUE;
}

 void TDRSignalHandler::SendMessage(const TGWindow *w, Long_t msg, 
				    Long_t parm1, Long_t parm2)
{
   // Send message (i.e. event) to window w. Message is encoded in one long
   // as message type and up to two long parameters.

   Event_t event;

   if (w) {
      event.fType   = kClientMessage;
      event.fFormat = 32;
      event.fHandle = gROOT_MESSAGE;

      event.fWindow  = w->GetId();
      event.fUser[0] = msg;
      event.fUser[1] = parm1;
      event.fUser[2] = parm2;
      event.fUser[3] = 0;
      event.fUser[4] = 0;

      gVirtualX->SendEvent(w->GetId(), &event);
   }
}
