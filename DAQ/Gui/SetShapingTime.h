#ifndef SETSHAPINGTIME_H
#define SETSHAPINGTIME_H

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



class SetShapingTime : public TGTransientFrame {

private:

  // Share memory
#define S_CONTROL
#undef  S_DATA
#undef  S_HISTO
#include "shm_define.code"


  const TGWindow *fMain;
  TGCompositeFrame *fSST;

  TGLabel *fSSTf11; 
  TGTextEntry *fSSTf12; TGTextBuffer *fSSTTb12;
  TGLabel *fSSTf13; 

  TGTextButton *fSSTButtonOK; 
  TGTextButton *fSSTButtonSAVE; 
  TGTextButton *fSSTButtonCANCEL; 


public:
  SetShapingTime(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
  ~SetShapingTime();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void ProcessTextButton(Long_t parm1, Long_t parm2);
  void CloseWindow();
};


#endif  /*SETSHAPINGTIME_H*/











