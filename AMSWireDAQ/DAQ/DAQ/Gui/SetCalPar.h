#ifndef SETCALPAR_H
#define SETCALPAR_H

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



class SetCalPar : public TGTransientFrame {

private:

  // Share memory
#define S_CONTROL
#undef  S_DATA
#undef  S_HISTO
#include "shm_define.code"


  const TGWindow *fMain;
  TGCompositeFrame *fSCP;

  TGGroupFrame *fSCPPass0;
  TGLabel *fSCP0f11; TGTextEntry *fSCP0f12; TGTextBuffer *fSCP0Tb12;

  TGGroupFrame *fSCPPass1;
  TGLabel *fSCP1f11; TGTextEntry *fSCP1f12; TGTextBuffer *fSCP1Tb12;

  TGGroupFrame *fSCPPass2;
  TGLabel *fSCP2f11; TGTextEntry *fSCP2f12; TGTextBuffer *fSCP2Tb12;
  TGLabel *fSCP2f21; TGTextEntry *fSCP2f22; TGTextBuffer *fSCP2Tb22;
  TGLabel *fSCP2f31; TGTextEntry *fSCP2f32; TGTextBuffer *fSCP2Tb32;
  TGLabel *fSCP2f41; TGTextEntry *fSCP2f42; TGTextBuffer *fSCP2Tb42;
  TGLabel *fSCP2f51; TGTextEntry *fSCP2f52; TGTextBuffer *fSCP2Tb52;

  TGGroupFrame *fSCPPass3;
  TGLabel *fSCP3f11; TGTextEntry *fSCP3f12; TGTextBuffer *fSCP3Tb12;
  TGLabel *fSCP3f21; TGTextEntry *fSCP3f22; TGTextBuffer *fSCP3Tb22;
  TGLabel *fSCP3f31; TGTextEntry *fSCP3f32; TGTextBuffer *fSCP3Tb32;

  TGGroupFrame *fSCPPass4;
  TGLabel *fSCP4f11; TGTextEntry *fSCP4f12; TGTextBuffer *fSCP4Tb12;
  TGLabel *fSCP4f21; TGTextEntry *fSCP4f22; TGTextBuffer *fSCP4Tb22;
  TGLabel *fSCP4f31; TGTextEntry *fSCP4f32; TGTextBuffer *fSCP4Tb32;


  TGTextButton *fSCPButtonOK; 
  TGTextButton *fSCPButtonSAVE; 
  TGTextButton *fSCPButtonCANCEL; 


public:
  SetCalPar(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
  ~SetCalPar();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void ProcessTextButton(Long_t parm1, Long_t parm2);
  void CloseWindow();
};


#endif  /*SETCALPAR_H*/
