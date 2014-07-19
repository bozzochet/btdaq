#ifndef SELECTLADDER_H
#define SELECTLADDER_H

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

#include <iostream>
#include <iomanip>
#include <stdlib.h>
//#include <strstream.h>
#include <fstream>

#include "DefineFonts.h"
#include "Share.h"



class SelectLadder : public TGTransientFrame {

private:


#define S_CONTROL
#undef  S_DATA
#undef  S_HISTO
#include "shm_define.code"


  const TGWindow *fMain;
  TGCompositeFrame *fSL;

  TGTextButton *fSLButtonLast; 
  TGTextButton *fSLButtonTest; 
  TGTextButton *fSLButtonAMS; 
  TGTextButton *fSLButtonHybrid; 

public:
  SelectLadder(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
  ~SelectLadder();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void ProcessTextButton(Long_t parm1, Long_t parm2);
  void CloseWindow();
};


#endif /* SELECTLADDER_H */


