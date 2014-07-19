#include <TROOT.h>
#include <TGFrame.h>
#include <TApplication.h> 
#include <TGClient.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGMenu.h>
#include <TGTab.h>
#include <TGComboBox.h>
#include <TRootEmbeddedCanvas.h>
#include <TGFrame.h>
#include <TGString.h>

#include <iostream.h>
#include <iomanip.h>
#include <strstream.h>
#include <fstream.h>

#include "Messages.h"

#include "DefineFonts.h"
#include "DefineLadder.h"
#include "../Control/Share.h"

#define S_CONTROL
#undef  S_DATA
#undef S_HISTO
#include "../Control/shm_define.code"

class NewLadder : public TGTransientFrame, public DefineFonts, 
                  public DefineLadder{
public:
  NewLadder(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
};
NewLadder::NewLadder(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h) : TGTransientFrame(p, main, w, h) 
{}
