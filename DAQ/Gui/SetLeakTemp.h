#ifndef SETLEAKTEMP_H
#define SETLEAKTEMP_H

#include <TROOT.h>
#include <TApplication.h> 
#include <TGClient.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGMenu.h>
#include <TGTab.h>
#include <TGTextEdit.h>
#include <TGComboBox.h>
#include <TRootEmbeddedCanvas.h>
#include <TGFrame.h>
#include <TGString.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TSystem.h>

#include <iostream>
#include <iomanip>
#include <stdlib.h>
//#include <strstream.h>
#include <fstream>

#include "DefineFonts.h"
#include "Share.h"



class SetLeakTemp : public TGTransientFrame {

private:

  // Share memory
#define S_CONTROL
#undef  S_DATA
#undef  S_HISTO
#include "shm_define.code"


  const TGWindow *fMain;
  TGCompositeFrame *fSLT;

  TGLabel *fSLTf11; 
  TGTextEntry *fSLTf12; TGTextBuffer *fSLTTb12;
  TGLabel *fSLTf13; 

  TGLabel *fSLTf21; 
  TGTextEntry *fSLTf22; TGTextBuffer *fSLTTb22; 
  TGLabel *fSLTf23; 

  TGLabel *fSLTf31; 
  TGTextEntry *fSLTf32; TGTextBuffer *fSLTTb32; 
  TGLabel *fSLTf33; 


  TGLabel *fSLTf41; 
  TGTextEdit *fSLTf42; 

  TGTextButton *fSLTButtonOK; 
  TGTextButton *fSLTButtonSAVE; 
  TGTextButton *fSLTButtonCANCEL; 


public:
  SetLeakTemp(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
  ~SetLeakTemp();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void ProcessTextButton(Long_t parm1, Long_t parm2);
  void CloseWindow();
};


#endif  /*SETLEAKTEMP_H*/











