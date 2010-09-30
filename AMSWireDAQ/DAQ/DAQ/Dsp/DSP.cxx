#include "Queue.h"
#include "Share.h"
#include "Ladder.h"


#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>


#define Q_CTL
#define Q_LOG
#undef  Q_GUI
#define Q_RUN
#define Q_DSP
#include "msg_define.code"

#define S_CONTROL
#define S_DATA
#define  S_HISTO
#include "shm_define.code"

long mtype;
int par1;
int par2;
Ladder *myLadder;


void ProcessCTL(int par1, int par2);
void ProcessRUN(int par1, int par2);

void StartRun(int newRun); //Gain mode if newrun==0 then no reset!!!
void StopRun();

void FinishPassCal();
void FinishPassGain();

void ReadEvent();
void ReadDSPCalib();
void SaveDSPCalib();

int main(int argc, char **argv) 
{

  myLadder = NULL;
  
  char file[200];

  // Creating queue messages  
#include "msg_definition.code"


  // Creating share memory segments
#include "shm_definition.code"


  // Main Loop
  while (control->On) { 
    toDSP->Wait(0,&mtype,&par1,&par2);
    switch (mtype) {
    case kCTL:
      ProcessCTL(par1,par2);
      break;
    case kRUN:
      ProcessRUN(par1,par2);
      break;
    }
  }
  printf("DSP about to detach shm\n");

#include "shm_detach.code"

  printf("DSP says goodbye\n");

}

void ProcessCTL(int par1, int par2){
    char aMessage[200];
  switch (par1) {
  case kWakeUp:
    toCTL->Send(kDSP,kReady,0);
    // if (myLadder == NULL && par2==2) toCTL->Send(kDSP,kLadderNotExist,0);
    break;

  case kCheckLadder:
    if (myLadder == NULL) {
      toCTL->Send(kDSP,kLadderNotExist,0);
    } else {
      toCTL->Send(kDSP,kLadderExist,0);
    }
    break;

  case kCreateLadder:
    sprintf(aMessage,"Creating a ladder");
    toLOG->Send(kDSP,aMessage,1);
    // for security...
    if (myLadder) { 
      delete myLadder;
      myLadder=0;
    }
    myLadder = new Ladder(control->fLadderName);
    sprintf(aMessage,"%s",control->fLadderName);
    toLOG->Send(kDSP,aMessage,1);

    //myLadder = new Ladder(control->fVoltage ,control->fSensors,
    //			  control->fAssembly,control->fLayer,
    //                        control->fSerial);  
    //sprintf(control->fLadderName,myLadder->GetName()); 
    toCTL->Send(kDSP,kLadderCreated,0);
    break;

  case kSaveLadder:
    toCTL->Send(kDSP,kLadderSaved,0);
    break;

  case kResetHW:
    toRUN->Send(kDSP,kResetHW,0);
    break;

  case kDeleteLadder:
    delete myLadder;
    myLadder = NULL;
    toCTL->Send(kDSP,kLadderDeleted,0);
    break;

  case kStartRun:
    StartRun(par2);
    break;
  case kStopRun:
    StopRun();
    break;

  case kReadTemp:
    toRUN->Send(kDSP,kReadTemp,0);
    break;

  case kRefreshTime:
    toRUN->Send(kDSP,kRefreshTime,0);
    break;

  case kFinishPassCal:
    FinishPassCal();
    break;

  case kFinishPassGain:
    FinishPassGain();
    break;

  case kSaveCalibration:
    printf("before saving cal\n");
    myLadder->SaveCalibration(par2);
    printf("after saving cal\n");
    if (par2 != 0) toCTL->Send(kDSP,kEndAMSTest,0);
    break;

  case kComputeGain:
    myLadder->ComputeGain();
    break;

  case kSaveGain:
    myLadder->SaveGain();
    break;

  case kWriteCalSummary:
    printf("before write summary\n");
    myLadder->WriteSummary(par2);
    printf("after write summary\n");
    toCTL->Send(kDSP,kShowCalSummary,0);
    break;

  case kReadDefaultCalibration:
    // now we load the ref. calibration, if it exists
    if (!myLadder->ReadRefCalibration()) toLOG->Send(kDSP,"Reference calibration loaded",1); 
    if (!myLadder->ReadCalibration()) toLOG->Send(kDSP,"Last calibration loaded",1); 
    break;

  case kSaveDSPCalib:
    SaveDSPCalib();
    break;

  case kTakeDSPCalib:
    toRUN->Send(kDSP,kReadDSPCalib,0);
    break;

  case kTDRExit:
    printf("DSP received an exit signal from control\n");
    toLOG->Send(kDSP,"Finisihing DSP ... Goodbye",1);
    break;

    
  }
}


void ProcessRUN(int par1, int par2){
  switch (par1) {  
  case kRunStarted:
    toCTL->Send(kDSP,kRunStarted,0);
    break;
  case kRunNotStarted:
    toCTL->Send(kDSP,kRunNotStarted,0);
    break;
  case kRunStoped:
    toCTL->Send(kDSP,kRunStoped,0);
    break;
  case kHardware_Problem:
    toCTL->Send(kDSP,kHardware_Problem,0);
  case kReadEvent:
    ReadEvent();
    break;
  case kShowTemp:
    toCTL->Send(kDSP,kShowTemp,0);
    break;
  case kTemperature_Problem:
    toCTL->Send(kDSP,kTemperature_Problem,0);
    break;
  case kTemperature_Error:
    toCTL->Send(kDSP,kTemperature_Error,0);
    break;
  case kDSPCalibDone:
    toCTL->Send(kDSP,kDSPCalibDone,0);
    break;
  case kReadDSPCalib:
    ReadDSPCalib();
    break;	  
  case kShowConnectedTdrs:
    printf("dsp sends to ctl, showconnectedtdrs\n");
    toCTL->Send(kDSP, kShowConnectedTdrs, 0);
    break;
  }
}

void StartRun(int newRun){
  char aMessage[200];
  control->NEvents=0;
  control->NBadPwrS=0;
  control->NBadPwrK=0;

  switch (control->LogStatus){
    case kLOGSTATUS_BINARY:
    case kLOGSTATUS_ASCII:
      if ( control->DataMode == kDATAMODE_RAW ||
	  (control->DataMode == kDATAMODE_CALIBRATION && 
	   control->CurrentCalStep == 0) ||
	  (control->DataMode == kDATAMODE_COSMIC && 
	   control->CosmicSequence == 0 && control->CurrentCalStep == 0) ||
	  (control->DataMode == kDATAMODE_COSMIC && 
	   control->CosmicSequence == 1) ||
	  (control->DataMode == kDATAMODE_SOURCE && 
	   control->SourceSequence == 0 && control->CurrentCalStep == 0) ||
	  (control->DataMode == kDATAMODE_SOURCE && 
	   control->SourceSequence > 0) ||
	  (control->DataMode == kDATAMODE_REDUCTION) )  {
	myLadder->OpenOutputFile();
	sprintf(aMessage,"Opened output file: %s",control->fFileName);
	toLOG->Send(kDSP,aMessage,1); 
      }
      break;

    case kLOGSTATUS_TEST:	
      if (control->TestSequence == 1 && control->CurrentCalStep == 0){ 
        myLadder->OpenOutputFile();
        sprintf(aMessage,"Opened output file: %s",control->fFileName);
        toLOG->Send(kDSP,aMessage,1); 
      }
      break;
  }

  switch (control->DataMode) {
  case kDATAMODE_FFT:
    myLadder->Reset(kDATAMODE_FFT);
    memset(histo->fhSpectrumS1,0,sizeof(histo->fhSpectrumS1));
    memset(histo->fhSpectrumS2,0,sizeof(histo->fhSpectrumS2));
    memset(histo->fhSpectrumK,0,sizeof(histo->fhSpectrumK));
    break;
  case kDATAMODE_RAW:
    myLadder->Reset(kDATAMODE_RAW);
    memset(histo->fhSignalMean,0,sizeof(histo->fhSignalMean));
    memset(histo->fhSignalHits,0,sizeof(histo->fhSignalHits));
    break;
  case kDATAMODE_CALIBRATION:  
    myLadder->Reset(kDATAMODE_CALIBRATION);    
    break;
  case kDATAMODE_COSMIC:
    if (control->CosmicSequence==0) 
      myLadder->Reset(kDATAMODE_CALIBRATION);
  case kDATAMODE_REDUCTION:
    myLadder->Reset(kDATAMODE_REDUCTION);
    memset(histo->fhClusLength_S,  0,sizeof(histo->fhClusLength_S));
    memset(histo->fhClusLength_K,  0,sizeof(histo->fhClusLength_K));
    memset(histo->fhClusNumber_S,  0,sizeof(histo->fhClusNumber_S));
    memset(histo->fhClusNumber_K,  0,sizeof(histo->fhClusNumber_K));
    memset(histo->fhClusCog,       0,sizeof(histo->fhClusCog));
    memset(histo->fhClusMaxloc,    0,sizeof(histo->fhClusMaxloc));
    memset(histo->fhClusIntegral_S,0,sizeof(histo->fhClusIntegral_S));
    memset(histo->fhClusIntegral_K,0,sizeof(histo->fhClusIntegral_K));
    memset(histo->fhClusSovern_S,  0,sizeof(histo->fhClusSovern_S));
    memset(histo->fhClusSovern_K,  0,sizeof(histo->fhClusSovern_K));
    break;
  case kDATAMODE_GAIN:
    if (newRun > 0) {
      memset(histo->fhSignalMean,0,sizeof(histo->fhSignalMean));
      memset(histo->fhSignalHits,0,sizeof(histo->fhSignalHits));
      myLadder->Reset(kDATAMODE_GAIN);
    }
    break;
  case kDATAMODE_SCOPE:
    memset(histo->fhShaping,0,sizeof(histo->fhShaping));
    myLadder->Reset(kDATAMODE_SCOPE);
    break;
  case kDATAMODE_SOURCE:
    myLadder->Reset(kDATAMODE_CALIBRATION);    
    myLadder->Reset(kDATAMODE_REDUCTION);
    memset(histo->fhClusLength_S,  0,sizeof(histo->fhClusLength_S));
    memset(histo->fhClusLength_K,  0,sizeof(histo->fhClusLength_K));
    memset(histo->fhClusNumber_S,  0,sizeof(histo->fhClusNumber_S));
    memset(histo->fhClusNumber_K,  0,sizeof(histo->fhClusNumber_K));
    memset(histo->fhClusCog,       0,sizeof(histo->fhClusCog));
    memset(histo->fhClusMaxloc,    0,sizeof(histo->fhClusMaxloc));
    memset(histo->fhClusIntegral_S,0,sizeof(histo->fhClusIntegral_S));
    memset(histo->fhClusIntegral_K,0,sizeof(histo->fhClusIntegral_K));
    memset(histo->fhClusSovern_S,  0,sizeof(histo->fhClusSovern_S));
    memset(histo->fhClusSovern_K,  0,sizeof(histo->fhClusSovern_K));

    break;
  }

  toRUN->Send(kDSP,kStartRun,0);
}

void StopRun(){
  char aMessage[200];
  switch (control->LogStatus){
    case kLOGSTATUS_BINARY:
    case kLOGSTATUS_ASCII:	
      if ( control->DataMode == kDATAMODE_RAW ||
	  (control->DataMode == kDATAMODE_CALIBRATION && 
	   control->CurrentCalStep == control->MaxCalStep-1) ||
	  (control->DataMode == kDATAMODE_COSMIC && 
	   control->CosmicSequence == 0 && 
	   control->CurrentCalStep == control->MaxCalStep-1) ||
	  (control->DataMode == kDATAMODE_COSMIC && 
	   control->CosmicSequence == 1) || 
	  (control->DataMode == kDATAMODE_SOURCE && 
	   control->SourceSequence == 0 && 
	   control->CurrentCalStep == control->MaxCalStep-1) ||
	   (control->DataMode == kDATAMODE_SOURCE && 
	   control->SourceSequence > 0) ||
	  (control->DataMode == kDATAMODE_REDUCTION) )  {
        sprintf(aMessage,"Closed output file: %s",control->fFileName);
	toLOG->Send(kDSP,aMessage,1); 
	myLadder->CloseOutputFile();
      }
      break;

    case kLOGSTATUS_TEST:	
      if (control->TestSequence == 2 && 
	  control->CurrentGainStep == control->MaxGainStep-1 &&
	  control->CloseTestFile == 1){ 
        sprintf(aMessage,"Closed output file: %s",control->fFileName);
        toLOG->Send(kDSP,aMessage,1); 
        myLadder->CloseOutputFile();
      }
      break;
  }
  toRUN->Send(kDSP,kStopRun,0);
}


void ReadEvent(){
  static int icl;

  if (control->NEvents_max == 0 ||
      control->NEvents < control->NEvents_max) {
    icl=0;
    int islot = data->fOut%max_buf;
    myLadder->ReadEvent(islot);
    control->NEvents++;  // Increment NEvents
    
    //if (control->DataMode != kDATAMODE_SCOPE) myLadder->UpdateScope();
    myLadder->UpdateScope();
   
    switch (control->DataMode) {
    case kDATAMODE_FFT:
      myLadder->StartFFT();
      break;
    case kDATAMODE_RAW:
      myLadder->UpdateSignal(control->DynPed);
      if (control->LogStatus != kLOGSTATUS_NOTLOG) myLadder->SaveEvent();
      break;
    case kDATAMODE_CALIBRATION:
    case kDATAMODE_COSMIC:
      if (control->LogStatus != kLOGSTATUS_NOTLOG ||
	  control->AMSTest >= 'A') myLadder->SaveEvent(); 
      switch (control->CalStep) {
      case 0: myLadder->PassCal0(); break;
      case 1: myLadder->PassCal1(); break;
      case 2: myLadder->PassCal2(); break;
      case 3: myLadder->PassCal3(); break;
      case 4: myLadder->PassCal4(); break;
      }
      break;
    case kDATAMODE_REDUCTION:	
      myLadder->Reduction(control->DynPed);
      break;

    case kDATAMODE_GAIN:
      if (control->AMSTest >= 'A') myLadder->SaveEvent();
      myLadder->Gain();
      break;

    case kDATAMODE_SCOPE:
      myLadder->Channel();
      break;

    case kDATAMODE_SOURCE:
      myLadder->SaveEvent(); 
      if (control->SourceSequence == 0){
	switch (control->CalStep) {
	case 0: myLadder->PassCal0(); break;
	case 1: myLadder->PassCal1(); break;
	case 2: myLadder->PassCal2(); break;
	case 3: myLadder->PassCal3(); break;
	case 4: myLadder->PassCal4(); break;
	}
      } else {
	myLadder->Reduction(control->DynPed);
      }
      break;
    }
    data->fOut++;        // Increment Events readed buffer

  } else {
    icl++;
  }

  if (icl == 1) {
    switch (control->DataMode) {
    case kDATAMODE_RAW:
    case kDATAMODE_FFT:
      toCTL->Send(kDSP,kStopRun,0);
      break;

    case kDATAMODE_CALIBRATION:
    case kDATAMODE_COSMIC:
      toCTL->Send(kDSP,kStopRun,0);
      break;

    case kDATAMODE_REDUCTION:
      toCTL->Send(kDSP,kStopRun,0);
      break;

    case kDATAMODE_GAIN:
      toCTL->Send(kDSP,kStopRun,0);
      break;

    case kDATAMODE_SCOPE:
      toCTL->Send(kDSP,kStopRun,0);
      break;
    case kDATAMODE_SOURCE:
      toCTL->Send(kDSP,kStopRun,0);
      break;
    }
  }
}


void FinishPassCal(){
  switch (control->CalStep) {
  case 0:      
    myLadder->FinishPassCal0();
    break;
  case 1:
    myLadder->FinishPassCal1();
    break;
  case 2:
    myLadder->FinishPassCal2();
    break;
  case 3:
    myLadder->FinishPassCal3();
    break;
  case 4:
    myLadder->FinishPassCal4();
    break;
  }
  toCTL->Send(kDSP,kNextCalStep,0);
}


void FinishPassGain(){
  myLadder->FinishGain();
  toCTL->Send(kDSP,kNextGainStep,0);
}



void ReadDSPCalib() {
  myLadder->ReadDSPCalib();
  toCTL->Send(kDSP,kDSPCalibRead,0);
}


void SaveDSPCalib() {
  myLadder->SaveCalibration(0);
  if (control->TDRDSPselect==23) toCTL->Send(kDSP,kStopRun,0);
  else toCTL->Send(kDSP,kDSPCalibSaved,0);
  
}











