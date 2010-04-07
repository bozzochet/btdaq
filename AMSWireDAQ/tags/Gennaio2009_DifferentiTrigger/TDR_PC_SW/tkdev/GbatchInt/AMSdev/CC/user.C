//  $Id: user.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "typedefs.h"
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include "cern.h"
#include <stdio.h>
#include "user.h"
#include "trrec.h"
#include "tofrec02.h"
#include "ecalrec.h"
#include "event.h"
#include "trigger102.h"
#include "daqevt.h"
void AMSUser::InitJob(){
  if(!AMSJob::gethead()->isCalibration()){
    TOF2User::InitJob();
  }
}

void AMSUser::InitRun(){
}

void AMSUser::InitEvent(){
}

void AMSUser::Event(){
  bool glft(0);
//
  if(!AMSJob::gethead()->isCalibration()){
      Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
      if(ptr)glft=ptr->GlobFasTrigOK();
      if(!glft)return;// "no globFT in LVL1-trigger"
      TOF2User::Event();
  }
}


void AMSUser::EndJob(){
  if(!AMSJob::gethead()->isCalibration()){
    TOF2User::EndJob();
  }
}



