#ifndef AMSWIREWINDOW_H
#define AMSWIREWINDOW_H

#include <TROOT.h>
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
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGNumberEntry.h>

#include <iostream>
#include <iomanip>
#include <stdlib.h>
//#include <strstream.h>
#include <fstream>

#include "DefineFonts.h"
#include "Share.h"

class AMSWireWindow : public TGTransientFrame, public DefineFonts {
private:

  // Share memory
#define S_CONTROL
#undef  S_DATA
#undef  S_HISTO
#include "shm_define.code"

  const TGWindow *fMain;
  TGLabel *commlabel, *portlabel, *subaddlabel;
  TGLabel *sectlabel[9], *idlabel[9], *adhlabel[9], *erslabel[9], *stalabel[9];
  TGComboBox *HWCombo, *PortCombo;
  TGTextButton *CancelButton, *OkButton;
  TGNumberEntry *NodeNumEnt;

  int AmswHardware, AmswEppPort, AmswPciPort;

public:
  AMSWireWindow(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
  ~AMSWireWindow();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void ProcessTextButton(Long_t parm1, Long_t parm2);
  void ProcessComboBox(Long_t parm1, Long_t parm2);
  void ProcessTextEntry(Long_t parm1, Long_t parm2);
  void CloseWindow();
};


#endif  /*AMSWIREWINDOW_H*/
