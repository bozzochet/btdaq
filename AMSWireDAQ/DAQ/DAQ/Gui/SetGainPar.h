#ifndef SETGAINPAR_H
#define SETGAINPAR_H

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



class SetGainPar : public TGTransientFrame {

private:

  // Share memory
#define S_CONTROL
#undef  S_DATA
#undef  S_HISTO
#include "shm_define.code"


  const TGWindow *fMain;

  TGLabel       *fSGPLabel[16];
  TGCheckButton *fSGPCB[16];
  TGTextEntry   *fSGPDAC[16];   TGTextBuffer *fSGPDACTB[16];
  TGTextEntry   *fSGPEvent[16]; TGTextBuffer *fSGPEventTB[16];

  TGLabel *fSGPf11; TGTextEntry *fSGPf12; TGTextBuffer *fSGPTb12;  

  TGTextButton *fSGPButtonOK; 
  TGTextButton *fSGPButtonSAVE; 
  TGTextButton *fSGPButtonCANCEL; 

public:
  SetGainPar(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
  ~SetGainPar();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void ProcessTextButton(Long_t parm1, Long_t parm2);
  void CloseWindow();
};


#endif  /*SETGAINPAR_H*/
