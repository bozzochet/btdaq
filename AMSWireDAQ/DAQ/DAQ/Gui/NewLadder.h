#ifndef NEWLADDER_H
#define NEWLADDER_H  

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
#include <TGTextBuffer.h>
#include <TGTextEntry.h>
#include <TSystem.h>

#include <iostream>
#include <iomanip>
#include <stdlib.h>
//#include <strstream.h>
#include <fstream>

#include "DefineFonts.h"
#include "Share.h"


class NewLadder : public TGTransientFrame {

private:

  // Share memory
#define S_CONTROL
#undef  S_DATA
#undef  S_HISTO
#include "shm_define.code"

  const TGWindow *fMain;
  TGLabel *fShowName;
  TGTextEntry *fVal4,*fVal5,*fVal6;

  char fLadderName[15];  // Name of the ladder fName=
  char fVoltage;        // (L)ow or (H)igh voltage
  char fSensors[3];     // # of Silicon sensors (01,02,....10,11...)
  char fAssembly[3];       // Assembly Line (G)eneve, (P)erugia, (Z)urich
  char fLayer;          // (I)nner, (O)uter, (T)est
  char fSerial[4];      // serial number per ass. line (001,.,090,.,230,.) 

  char fHybridName[15];
  char fHybridType[5];     // Hybrid type 2024=S 2025=K 
  char fHybridBatch[4];
  char fHybridSerial[4];

public:
  NewLadder(int LoH,const TGWindow *p,const TGWindow *main,UInt_t w,UInt_t h);
  ~NewLadder();
  void ANewLadder();
  void ANewHybrid();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void ProcessComboBox(Long_t parm1, Long_t parm2);
  void ProcessTextEntry(Long_t parm1, Long_t parm2);
  void ProcessTextButton(Long_t parm1, Long_t parm2);
  void CloseWindow();
};

#endif /* NEWLADDER_H */  























