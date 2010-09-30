#include <TROOT.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TSysEvtHandler.h>
#include <iostream.h>

class ListenGUI {
public:
   ListenGUI(); 
   ~ListenGUI();
   void SendMessage(const TGWindow *w, Long_t msg, Long_t parm1, Long_t parm2);
 private:
  const TGWindow *fMain;
};

