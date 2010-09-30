#include "ListenGUI.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>


ListenGUI::ListenGUI(){

  while (1){
    cerr << "Hola EDU" << endl;
  }

}

void ListenGUI::SendMessage(const TGWindow *w, Long_t msg, 
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
