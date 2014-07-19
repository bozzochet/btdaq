#include "GUITimer.h"

GUITimer::GUITimer(MainFrame *main, Long_t ms = 1000, Bool_t mode = kTRUE)
  : TTimer(ms,mode){
  fMain=main;
}

GUITimer::~GUITimer(){ }

Bool_t GUITimer::Notify(){
  fMain->ProcessGUI();
  Reset();
  return kFALSE;
}

