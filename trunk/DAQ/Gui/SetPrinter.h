#ifndef SETPRINTER_H
#define SETPRINTER_H

#include <TROOT.h>
#include <TApplication.h> 
#include <TGClient.h>
#include <TCanvas.h>
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
#include <TPostScript.h>

#include <iostream>
#include <iomanip>
#include <stdlib.h>
//#include <strstream.h>
#include <fstream>

#include "DefineFonts.h"
#include "Share.h"



class SetPrinter : public TGTransientFrame {

private:

  // Share memory
#define S_CONTROL
#undef  S_DATA
#undef  S_HISTO
#include "shm_define.code"


  const TGWindow *fMain;
  TGCompositeFrame *fSPr;

  TGCheckButton *fSPrf11; TGTextEntry *fSPrf12; TGTextBuffer *fSPrTb12;
  TGCheckButton *fSPrf21; TGTextEntry *fSPrf22; TGTextBuffer *fSPrTb22;
  TGLabel *fSPrf31; TGCheckButton *fSPrf32; TGCheckButton *fSPrf33;

  TGTextButton *fSPrButtonOK; 
  TGTextButton *fSPrButtonCANCEL; 

  TRootEmbeddedCanvas *myCanvas;

public:
  SetPrinter(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, 
	     TRootEmbeddedCanvas *fCanvas, const char *fName);
  ~SetPrinter();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void ProcessTextButton(Long_t parm1, Long_t parm2);
  void CloseWindow();
};


#endif  /*SETPRINTER_H*/





