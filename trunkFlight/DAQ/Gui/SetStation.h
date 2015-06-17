#ifndef SETSTATION_H
#define SETSTATION_H

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

#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <strstream.h>
#include <fstream.h>

#include "DefineFonts.h"
#include "Share.h"



class SetStation : public TGTransientFrame {

private:

  // Share memory
#define S_CONTROL
#undef  S_DATA
#undef  S_HISTO
#include "shm_define.code"

  const TGWindow *fMain;
  TGCompositeFrame *fSS;

  TGLabel *fSSf11; 
  TGTextEntry *fSSf12; TGTextBuffer *fSLTTb12;

  TGTextButton *fSSButtonOK; 
  TGTextButton *fSSButtonCANCEL; 

public:
  SetStation(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
  ~SetStation();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void ProcessTextButton(Long_t parm1, Long_t parm2);
  void CloseWindow();
};


#endif  /*SETSTATION_H*/











