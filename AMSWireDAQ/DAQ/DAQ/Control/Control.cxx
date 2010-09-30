#include "Queue.h"
#include "Share.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <sys/stat.h>

#include <time.h>
#include <TSystem.h>

#include "TrackerLadders.h"

#define Q_CTL
#define Q_LOG
#define Q_GUI
#define Q_RUN
#define Q_DSP
#include "msg_define.code"

#define S_CONTROL
#define S_DATA
#define S_HISTO
#define S_SHM_INIT
#include "shm_define.code"

long mtype;
int par1;
int par2;

int mS1,mS2,mK,nowS;
int eventSummary;

TrackerLadders InnerTracker;

void ProcessGUI(int par1, int par2);
void ProcessDSP(int par, int par2);
void ProcessRUN(int par1, int par2);

void LoadPreferences();
void SetPref(char *item, char *prefstring);
void SavePreferences();

int OpenDirectory(char *aDirectory);
void CreateDirectory(char *aDirectory);
int OpenFile(char *aFile);
int LastRunNumber(char *aFile);

int CheckLadder();

void StartRun(int newRun);
void StopRun(int nowhalt);

void RunStarted();
void RunStoped(int nowhalt);

void DefaultClusterConstants();

void SetupCalibration(int level);
void DefaultCalibrationConstants();
void NextCalStep();

void SetupGain();
void DefaultGainConstants();
void NextGainStep();

void FullCalib();

void DSPCalib(int step);

void AMSTest();
void SourceTest();

void Cosmic();

char datamode[25],source[25];
void UpdateRunInfo();
void UpdateRunSummary();

void UpdateStation(int aStation);
void WriteStation();
void ReadStation();

void UpdateLadOctNames();
void OctantCalib(int step);

void CreateNewTdr(int tdr);

int CheckProcess(int proc, int kMSG);
void catch_alarm(int sig_num);  

void catch_alarm(int sig_num){
  signal(SIGALRM,catch_alarm);
}
char CalMessage[5][100]; // Message to appear on log file for eack cal step

using namespace std;

int main(int argc, char **argv) 
{
  char file[200];
  int timeoutcnt=0;
  // Creating queue messages  
#include "msg_definition.code"

  // Creating share memory segments
#include "shm_definition.code"

  //cout << "histo = " << histo << endl;
  //cout << "control = " << control << endl;
  //cout << "data = " << data << endl;

  //printf("size of histo = %d\n",sizeof(*histo));
  //printf("size of control = %d\n",sizeof(*control));
  //printf("size of data = %d\n",sizeof(*data));

  // Set up important parameters
  control->RunStatus = kRUNSTATUS_STOP;
  control->ActiveS1 = 1;
  control->ActiveS2 = 1;
  control->ActiveK  = 1;

  control->ShowBadChannels=1;

  sprintf(control->Version,"6.0");
  
  //sprintf(control->TempId,"xxxxxx");
  
  control->On=1;
  control->CTL=1;
  control->LOG=0;
  control->RUN=0;
  control->DSP=0;
  control->GUI=0;

  control->OnlyHybrid = 0;

  // setting default values
  sprintf(control->fLadderNameAtExit,"testladder");
  control->Shaping=3.0; // in us
  control->TDRref=0;
  control->ChannelFrequency=5.0; // in MHz

  control->Hardware=kHARDWARE_JINF_PCI;
  control->EPPport=0;
  control->PCIcard=0;
  control->PCIport=0;

  control->TrackerXtant=0;

  control->ReadoutPWG=0;
  control->RecordGifMovie=0;
  control->DynPed=0;

  control->JINFaddr=0xffff;

  control->SaveHistoRawcal=1;

  LoadPreferences();

  DefaultCalibrationConstants();
  DefaultGainConstants();
  DefaultClusterConstants();

  InnerTracker.ReadFile("TrackerLadders/ladlist.txt");

  printf("reference tdr=%d\n", control->TDRref);
  UpdateLadOctNames();
  printf("reference tdr=%d\n", control->TDRref);

  control->DACValue=100;

  control->NEvents_max = 0;

  //  sprintf(control->fDirMain,"%s%s%s","/data/",getenv("USER"),"/");
  sprintf(control->fDirMain,"%s%s%s%s%s","/home/",getenv("USER"),"/data/",getenv("USER"),"/");
  printf("main dir is: %s\n",control->fDirMain);
  control->CTL_pid = getpid();

  // Waking up processes
  strcpy(file,getenv("PWD"));
  strcat(file,"/LOG");
  if (fork() == 0) {
    control->LOG_pid = getpid();
    execl(file,NULL); 
  }
  //CheckProcess(kLOG,0);
  for (int cnt=0; cnt<20; cnt++) {if (CheckProcess(kLOG,0)!=kTimeOut) break;sleep(1);}

  strcpy(file,getenv("PWD"));
  strcat(file,"/RUN");
  if (fork() == 0) {
    control->RUN_pid = getpid();
    execl(file,NULL);
  }
  //CheckProcess(kRUN,0);
  for (int cnt=0; cnt<20; cnt++) {if (CheckProcess(kRUN,0)!=kTimeOut) break;sleep(1);}


  strcpy(file,getenv("PWD"));
  strcat(file,"/DSP");
  if (fork() == 0){
    control->DSP_pid = getpid();
    execl(file,NULL);
  }
  //CheckProcess(kDSP,0);
  for (int cnt=0; cnt<20; cnt++) {if (CheckProcess(kDSP,0)!=kTimeOut) break;sleep(1);}

  control->LadderType=kLadderTest;
  //  strcpy(control->fLadderName,"testladder");
  sprintf(control->fLadderName,"TDR%03d",control->TDRref);
  printf("ladder name to be created: %s\n",control->fLadderName);
  toDSP->Send(kCTL,kCreateLadder,0);

  strcpy(file,getenv("PWD"));
  strcat(file,"/GUI");
  if (fork() == 0) {
    control->GUI_pid = getpid();
    execl(file,NULL);
  }
  for (int cnt=0; cnt<20; cnt++) {if (CheckProcess(kGUI,0)!=kTimeOut) break;sleep(1);}




  //  CheckProcess(kLOG,0);
  //  CheckProcess(kRUN,0);
  //  CheckProcess(kGUI,0);
  //  CheckProcess(kDSP,0);

  UpdateStation(0);  // Read .stationid file

  //sleep(1);

  //control->fVoltage='T';
  //strcpy(control->fSensors,"ES");
  //control->fAssembly='T';
  //control->fLayer='L';
  //strcpy(control->fSerial,"ADD");


  sleep(2); // want to be sure that ladder is fully created...

  // Main Loop
  while (control->On) {
    toCTL->Wait(0,&mtype,&par1,&par2);

    switch (mtype) { 
    case kGUI:
      ProcessGUI(par1,par2);
      break;
    case kDSP:
      ProcessDSP(par1,par2);
      break;
    case kRUN:
      ProcessRUN(par1,par2);
      break;
    }
  }

  printf("control: before savepreferences\n");
  SavePreferences();
  printf("control: after save\n");
  while (timeoutcnt<1) { timeoutcnt+=(CheckProcess(kDSP,0)==kTimeOut); }
  printf("DSP left\n");
  timeoutcnt=0;
  while (timeoutcnt<1) { printf("run timeoutcnt=%d\n", timeoutcnt);timeoutcnt+=(CheckProcess(kRUN,0)==kTimeOut); }
  printf("RUN left\n");
  timeoutcnt=0;
  while (timeoutcnt<1) { timeoutcnt+=(CheckProcess(kGUI,0)==kTimeOut); }
  printf("GUI left\n");
  timeoutcnt=0;
  while (timeoutcnt<1) { timeoutcnt+=(CheckProcess(kLOG,0)==kTimeOut); }
  printf("LOG left\n");
  printf("control: before deleting queue and shm\n");
  // Deleting everything and exiting
#include "msg_delete.code"
#include "shm_detach.code"
#include "shm_delete.code"

  printf("control says goodbye\n");
}

void ProcessGUI(int par1, int par2){
  int retval;
  char aMessage[200];
  int it=1;

  char filename[100],filename2[100];
  char command[250];


  switch (par1) {
  case kInterrupt:
    kill(control->GUI_pid,SIGUSR1);
    break;

  case kNewLadder:
    toDSP->Send(kCTL,kCheckLadder,0);
    break;

  case kSetOctant:
    UpdateLadOctNames();
    toGUI->Send(kCTL,kUpdateToolTipsTdrs,0);
    // and we continue with setting the ref. TDR..., so no "break" here
  case kSetReferenceTDR:
    printf("control sends to Run reference signal\n");
    toRUN->Send(kCTL,kSetReferenceTDR,0);

    // and we create a new "ladder"

    CreateNewTdr(control->TDRref);

    break;

   case kSetStation:
    UpdateStation(par2);  
    break;

  case kResetHW:
    toDSP->Send(kCTL,kResetHW,0);
    break;

  case kReadTemp:
    toDSP->Send(kCTL,kReadTemp,0);
    break;

  case kTDRExit:
    printf("Control received an exit signal from GUI\n");
    toDSP->Send(kCTL,kTDRExit,0);
    toRUN->Send(kCTL,kTDRExit,0);
    signal(SIGALRM,catch_alarm);  // Now sleep for 1 second
    alarm(1);pause();alarm(0);    // ... please
    control->On=0;
    printf("control->On is now 0\n");
    toDSP->Send(0,0,0);
    toRUN->Send(0,0,0);
    toLOG->Send(kCTL,kTDRExit,0);
    break;

  case kCheckProcess:
    switch (par2) {
    case kCTL:
      toGUI->Send(kCTL,kProcessOk,kCTL);
      toLOG->Send(kCTL,"Control is alive",1);
      break;
    default:
      CheckProcess(par2,1);
      break;
    }
    break;

  case kCreateLadder:
    sprintf(aMessage,"%s",control->fLadderName);
    toLOG->Send(kCTL,aMessage,1);
    toDSP->Send(kCTL,kCreateLadder,0);
    break;

  case kSaveLadder:
    toDSP->Send(kCTL,kSaveLadder,0);
    break;

  case kDeleteLadder:
    toDSP->Send(kCTL,kDeleteLadder,0);
    break;

  case kSetLogStatus:
    control->LogStatus=par2;
    break;
  case kSetDataMode:
    control->DataMode=par2;
    break;
  case kSetTrigger:
    control->Trigger=par2;
    break;
  case kSetSource:
    control->Source=par2;
    break;

  case kFullCalibration:
    control->FullCalib=1;
    control->CalibSequence=0;
    //FullCalib();
    break;  

  case kAMSTest:
    sprintf(control->fDirTest,"%s/Test",control->fDirLadder);
    if (OpenDirectory(control->fDirTest) == 0 ) 
      CreateDirectory(control->fDirTest);

    it = 1;
    sprintf(filename,"%s/Test/%s-%c-%s-%02d.des",control->fDirLadder,
	    control->fLadderName,control->AMSTest,control->StationCode,it);
    while (OpenFile(filename) != 0 ) {
      it++;
      sprintf(filename,"%s/Test/%s-%c-%s-%02d.des",control->fDirLadder,
	      control->fLadderName,control->AMSTest,control->StationCode,it);
    } 
    control->AMSTestNumber=it;

    sprintf(filename,"%s/%s.des",control->fDirLadder,control->fLadderName); 

    sprintf(filename2,"%s/%s-%c-%s-%02d.des",
	    control->fDirTest,control->fLadderName,control->AMSTest,
	    control->StationCode,control->AMSTestNumber);
    sprintf(command,"cp %s %s",filename,filename2);
    system(command);

    sprintf(aMessage,"Starting Test %c%02d",control->AMSTest,it);
    toLOG->Send(kCTL,aMessage,1);
    control->TestSequence=0;
    control->CalibSequence=0;
    control->FullCalib=1;
    control->LogStatus = kLOGSTATUS_TEST;

    //      toDSP->Send(kCTL,kReadTemp,0);
    //      toCTL->TimeOut(kDSP,&par1,&par2,2);
    //      switch (par1) {
    //      case kShowTemp:
    //        toGUI->Send(kCTL,kShowTemp,0);
    //        break;

    //      case kTemperature_Problem:
    //        toGUI->Send(kCTL,kTemperature_Problem,0);
    //        break;

    //      case kTemperature_Error:
    //        toGUI->Send(kCTL,kTemperature_Error,0);
    //        break;

    //      }

    AMSTest();
    FullCalib();
    break;


  case kSourceTest:
    sprintf(control->fDirTest,"%s/Test",control->fDirLadder);
    if (OpenDirectory(control->fDirTest) == 0 ) 
      CreateDirectory(control->fDirTest);

    it = 1;
    sprintf(filename,"%s/Test/%s-%c-%s-%02d.des",control->fDirLadder,
	    control->fLadderName,control->AMSTest,control->StationCode,it);
    while (OpenFile(filename) != 0 ) {
      it++;
      sprintf(filename,"%s/Test/%s-%c-%s-%02d.des",control->fDirLadder,
	      control->fLadderName,control->AMSTest,control->StationCode,it);
    } 
    control->AMSTestNumber=it;

    sprintf(filename,"%s/%s.des",control->fDirLadder,control->fLadderName); 

    sprintf(filename2,"%s/%s-%c-%s-%02d.des",
	    control->fDirTest,control->fLadderName,control->AMSTest,
	    control->StationCode,control->AMSTestNumber);
    sprintf(command,"cp %s %s",filename,filename2);
    system(command);

    control->CalibSequence=0;
    control->SourceSequence = 0;
    DefaultCalibrationConstants();
    SetupCalibration(cPASS_ALL);
    //control->Trigger=kTRIGGER_RANDOM; //
    SourceTest();
    break;
  case kSetupCalibration:
    SetupCalibration(par2);
    break;

  case kSetupGain:
    SetupGain();
    break;

  case kWriteCalSummary:
    toDSP->Send(kCTL,kWriteCalSummary,0);
    break;

  case kSaveCalibration:
    toDSP->Send(kCTL,kSaveCalibration,0);
    break;

  case kSaveGain:
    toDSP->Send(kCTL,kSaveGain,0);
    break;

  case kRefreshTime:
    toDSP->Send(kCTL,kRefreshTime,0);
    break;

  case kStartRun:
    retval=CheckLadder();
    if (retval == kLadderExist) {
      switch (control->DataMode){   
      case kDATAMODE_CALIBRATION:
	NextCalStep();      
	break;
      case kDATAMODE_GAIN:
	NextGainStep();
	break;
      case kFULLCALIBRATION:
	FullCalib(); 
	break;
      case kDSPCALIBRATION:
	control->DSPCalibStep=3;
	DSPCalib(control->DSPCalibStep);
	break;
      case kOCTANTCALIBRATION:
	OctantCalib(0);
	break;
      case kDATAMODE_COSMIC:
	//control->Trigger=kTRIGGER_RANDOM;
	control->CosmicSequence=0;
	control->CosmicId=-1;
	DefaultCalibrationConstants();
	SetupCalibration(cPASS_ALL);
	Cosmic();
	break;
      case kDATAMODE_SOURCE:
	//control->Trigger=kTRIGGER_RANDOM;
	control->SourceSequence=0;
	DefaultCalibrationConstants();
	SetupCalibration(cPASS_ALL);
	SourceTest();
	break;
      default:
	StartRun(1);
	break;
      }
    }
    break;      

  case kStopRun:
    StopRun(1);
    break;
  }
}

void ProcessDSP(int par1, int par2){
  char aMessage[200];

  switch (par1) {
  case kLadderExist:
    toGUI->Send(kCTL,kLadderExist,0);
    break;

  case kLadderNotExist:
    toGUI->Send(kCTL,kLadderNotExist,0);
    break;

  case kDefineLadderPlease:
    toGUI->Send(kCTL,kDefineLadderPlease,0);
    break;

  case kLadderCreated:
    sprintf(control->fDirLadder,"%s%s",control->fDirMain,control->fLadderName);
    sprintf(control->fRunFileName,"%s/Run.Summary",control->fDirLadder);
    if (OpenDirectory(control->fDirLadder) != 0){
      control->NRun=LastRunNumber(control->fRunFileName);
      sprintf(aMessage,
	      "Reading Run info from Ladder directory - %d runs found",
	      control->NRun);
      toLOG->Send(kCTL,aMessage,1);
      // Ladder exists ask for calibration files
      toDSP->Send(kCTL,kReadDefaultCalibration,0);
    } else {      
      CreateDirectory(control->fDirLadder);
      control->NRun=0;
      toLOG->Send(kCTL,"Ladder directory created",1);
      // Go directly to calibration	
    }

    sprintf(aMessage,"Ladder %s created",control->fLadderName);
    toLOG->Send(kCTL,aMessage,1);
    toGUI->Send(kCTL,kLadderCreated,0);
    if (control->DataMode==kDSPCALIBRATION) {
      control->NRun++;
      UpdateRunSummary();
      DSPCalib(control->DSPCalibStep);
    }
    break;
    
  case kLadderSaved:
    toGUI->Send(kCTL,kLadderSaved,0);
    break;

  case kDSPCalibDone:
    DSPCalib(control->DSPCalibStep);
    break;
  case kDSPCalibRead:
    toDSP->Send(kCTL,kSaveDSPCalib,0);
    break;
  case kDSPCalibSaved:
    DSPCalib(control->DSPCalibStep);
    break;    

  case kLadderDeleted: 
    sprintf(aMessage,"Ladder %s deleted",control->fLadderName);
    toLOG->Send(kCTL,aMessage,1);    
    strcpy(control->fLadderName," ");
    //control->fVoltage=' ';
    //strcpy(control->fSensors," ");
    //control->fAssembly=' ';
    //control->fLayer=' ';
    //strcpy(control->fSerial," ");
    
    toGUI->Send(kCTL,kLadderDeleted,0);
    break;

    //case kNextCalStep:
    //NextCalStep();
    //break;

    //case kNextGainStep:
    //NextGainStep();
    //break;

  case kShowCalSummary:
    toGUI->Send(kCTL,kShowCalSummary,0);
    break;

  case kStopRun:
    if (control->DataMode==kDSPCALIBRATION) DSPCalib(2);
    StopRun(0);
    break;

  case kRunStoped:
    RunStoped(0);
    break;

  case kHardware_Problem:
    toGUI->Send(kCTL,kHardware_Problem,0);
    break;

  case kShowTemp:
    toGUI->Send(kCTL,kShowTemp,0);
    break;

  case kTemperature_Problem:
    toGUI->Send(kCTL,kTemperature_Problem,0);
    break;

  case kTemperature_Error:
    toGUI->Send(kCTL,kTemperature_Error,0);
    break;

  case kEndAMSTest:  
    AMSTest();
    break;

  case kShowConnectedTdrs:
    printf("ctl sends to gui, showconnected tdrs\n");
    toGUI->Send(kCTL, kShowConnectedTdrs, 0);
    break;

  }
}


void ProcessRUN(int par1, int par2){
  //  switch (par1) {
  //  case kPauseRun:
  //    PauseRun();
  //    break;    
  //  }
}


int CheckProcess(int proc, int kMSG){
  switch (proc) {
  case kLOG:
    toLOG->Send(kCTL,"Are you alive?",-kWakeUp);
    break;
  case kRUN:
    toRUN->Send(kCTL,kWakeUp,0);
    break;
  case kDSP:
    toDSP->Send(kCTL,kWakeUp,0);
    break;
  case kGUI:
    toGUI->Send(kCTL,kWakeUp,0);
    break;
  }

  toCTL->TimeOut(proc,&par1,&par2,5); // 5 secs of time out

  switch (par1) {
  case kReady: 
    switch (proc) { 
    case kLOG:
      control->LOG = 1;
      if (kMSG != 0) toLOG->Send(kCTL,"LOG is alive",1);
      break;
    case kRUN:
      control->RUN = 1;
      if (kMSG != 0) toLOG->Send(kCTL,"RUN is alive",1);
      break;
    case kDSP:
      control->DSP = 1;
      if (kMSG != 0) toLOG->Send(kCTL,"DSP is alive",1);
      //toDSP->Send(kCTL,kCheckLadder,0);
      break;
    case kGUI:
      control->GUI = 1;
      if (kMSG != 0) toLOG->Send(kCTL,"GUI is alive",1);
      break;
    }
    if (kMSG) toGUI->Send(kCTL,kProcessOk,proc); 
    break;

  case kTimeOut:
    switch (proc) { 
    case kLOG:
      control->LOG = 0;
      toLOG->Send(kCTL,"LOG is dead",2);
      break;
    case kRUN:
      control->RUN = 0;
      toLOG->Send(kCTL,"RUN is dead",2);
      break;
    case kDSP:
      control->DSP = 0;
      toLOG->Send(kCTL,"DSP is dead",2);
      break;
    case kGUI:
      control->GUI = 0;
      toLOG->Send(kCTL,"GUI is dead",2);
      break;
    }
    if (kMSG != 0) toGUI->Send(kCTL,kProcessNotOk,proc); 
    break;
  }

  return par1;

}


void DefaultClusterConstants(){
  control->fCut1=3.5;
  control->fCut2=1.0;
  control->fMinInt=0.0;
  control->fMaskClu=252; //  252=011111100
}

void DefaultCalibrationConstants(){
  control->EventsStep[0] = 1024;//128;
  control->EventsStep[1] = 1024;//128;
  control->EventsStep[2] = 1024;//128;
  control->EventsStep[3] = 2048;//128;
  control->EventsStep[4] = 1024;//128;
  control->SRSigmaCut =  1.75; // as far as I can see, this is not used
  control->SRBin      =  128;  // as far as I can see, this is not used
  control->SRLow      =  1.5; //1.0
  control->SRHigh     = 25.0;
  control->SigmaLow   = 1.5; //1.2
  control->SigmaHigh  = 4.5; //5.0
  control->GASigmaCut =  3.0; //3
  control->GAOut      =  0.05;
}

void DefaultGainConstants(){
  control->DACmeasure[0] =0; control->DAC[0] =10;  control->DACEvent[0] =10;
  control->DACmeasure[1] =0; control->DAC[1] =20;  control->DACEvent[1] =10;
  control->DACmeasure[2] =0; control->DAC[2] =40;  control->DACEvent[2] =10;
  control->DACmeasure[3] =1; control->DAC[3] =60;  control->DACEvent[3] =40;
  control->DACmeasure[4] =0; control->DAC[4] =80;  control->DACEvent[4] =10;
  control->DACmeasure[5] =1; control->DAC[5] =90;  control->DACEvent[5] =40;
  control->DACmeasure[6] =0; control->DAC[6] =100; control->DACEvent[6] =10;
  control->DACmeasure[7] =1; control->DAC[7] =120; control->DACEvent[7] =40;
  control->DACmeasure[8] =0; control->DAC[8] =140; control->DACEvent[8] =10;
  control->DACmeasure[9] =0; control->DAC[9] =160; control->DACEvent[9] =10;
  control->DACmeasure[10]=0; control->DAC[10]=180; control->DACEvent[10]=10;
  control->DACmeasure[11]=0; control->DAC[11]=200; control->DACEvent[11]=10;
  control->DACmeasure[12]=0; control->DAC[12]=220; control->DACEvent[12]=10;
  control->DACmeasure[13]=0; control->DAC[13]=240; control->DACEvent[13]=10;
  control->DACmeasure[14]=0; control->DAC[14]=250; control->DACEvent[14]=10;
  control->DACmeasure[15]=0; control->DAC[15]=0;   control->DACEvent[15]=10;

  
  control->GainLow = 1.0;
}


int OpenDirectory(char *aDirectory){
  struct stat finfo;
  if (stat(aDirectory, &finfo) < 0)
    return 0;
  if (!S_ISDIR(finfo.st_mode))
    return 0;
  return 1;
}
int OpenFile(char *aFile){
  struct stat finfo;
  if (stat(aFile, &finfo) < 0) 
    return 0;
  printf("bytes = %d\n",finfo.st_size);
  if (!S_ISREG(finfo.st_mode)!=0)
    return 0;
  return 1;
}

void CreateDirectory(char *aDirectory){ 
  char command[200];
  sprintf(command,"mkdir %s",aDirectory);
  system(command);

  FILE *ifp;
  sprintf(command,"%s/Run.Summary",aDirectory);
  ifp=fopen(command,"w");
  fclose(ifp);
}

int LastRunNumber(char *aFile){
  FILE *ifp;
  char str[200];
  ifp=fopen(aFile,"r");
  if (ifp==0) printf("%s: file problem...\n",aFile);
  int nRun=0;
  while (fscanf(ifp,"%[^\n]\n",str)!=EOF) nRun++;
  fclose(ifp);		
  return nRun;
}

int CheckLadder(){
  int retval=0;
  toDSP->Send(kCTL,kCheckLadder,0);
  toCTL->TimeOut(kDSP,&par1,&par2,3);
  switch (par1) {
  case kTimeOut:
    retval=-1;
    break;
  case kLadderExist:
    retval=kLadderExist;
    break;
  case kLadderNotExist:
    retval=kLadderNotExist;
    toGUI->Send(kCTL,kDefineLadderPlease,0);
    break;
  }
  return retval;
}

void StartRun(int newRun){
  time_t now;
  char aMessage[200];
  if (newRun != 0) {
    UpdateRunInfo();
    control->NRun++;
    if (control->DataMode == kDATAMODE_COSMIC &&
	control->CosmicId == -1) control->CosmicId = control->NRun;
    now = time(NULL);
    strftime(control->TimeStartRun,26,"%d/%m/%y %H:%M:%S",localtime(&now));
    sprintf(aMessage,"Start Run %05d   Source: %s   DataMode: %s",
	    control->NRun,source,datamode);
    toLOG->Send(kCTL,aMessage,1);
  }
  control->RunStatus=kRUNSTATUS_RUN;
  toDSP->Send(kCTL,kStartRun,newRun);
  toCTL->TimeOut(kDSP,&par1,&par2,20); // seconds of timeout, due to jinf init
  switch (par1) {
  case kTimeOut:
    break;
  case kRunStarted:
    RunStarted();
    break;
  case kRunNotStarted:
    printf("kRunNotStarted\n");
    StopRun(1);
    break;
  case kDefineLadderPlease:
    break;
  }
}

void StopRun(int halt){
  //printf("welcome to stop run\n");
  control->RunStatus=kRUNSTATUS_STOP;
  toDSP->Send(kCTL,kStopRun,0);
  toCTL->TimeOut((int)kDSP,&par1,&par2,3);
  switch (par1) {
  case kTimeOut:
    break;
  case kRunStoped:
    RunStoped(halt);
    break;
  case kRunNotStoped:
    break;
  }
  //printf("leaving stop run\n");
}

void RunStarted(){
  control->RunStatus=kRUNSTATUS_RUN;
  control->Enable_START_RUN=0; 
  control->Enable_STOP_RUN=1;
  toGUI->Send(kCTL,kShowConnectedTdrs,0);
  toGUI->Send(kCTL,kRunStarted,0);
}

void RunStoped(int nowhalt){
  control->RunStatus=kRUNSTATUS_STOP;
  control->Enable_START_RUN=1; 
  control->Enable_STOP_RUN=0;

  toGUI->Send(kCTL,kRunStoped,0);
  if (nowhalt==0) {
    switch (control->DataMode){
    case kDATAMODE_CALIBRATION:
      NextCalStep();
      break;
    case kOCTANTCALIBRATION:
      OctantCalib(1);
      break;
    case kDATAMODE_GAIN:
      NextGainStep();
      break;
    case kDATAMODE_COSMIC:
      Cosmic();
      break;
    case kDATAMODE_SOURCE:
      SourceTest();
      break;
    default:
      UpdateRunSummary();
    }
  } else {
    control->LogStatus = kLOGSTATUS_NOTLOG;
    UpdateRunSummary();
  }
  // 
  if (control->AMSTest   >= 'A' && control->TestOn == 0 && 
      control->AMSTest != 'v' && control->AMSTest != 'w') AMSTest();
  if (control->FullCalib == 1   && control->TestOn == 0) FullCalib();
}


void SetupCalibration(int level){
  control->TestOn = 1;
  control->CurrentCalStep = -1;
  switch (level) {
  case cPASS_ALL:
    control->MaxCalStep = 5;
    strcpy(CalMessage[0]," - Pedestal and Sigma Estimation:");
    strcpy(CalMessage[1]," - Pedestal computation:");
    strcpy(CalMessage[2]," - Sigma Raw computation:");
    strcpy(CalMessage[3]," - Sigma computation:");
    strcpy(CalMessage[4]," - Common Noise Width computation:");
    control->CalSequence[0] = 0;
    control->CalSequence[1] = 1;
    control->CalSequence[2] = 2;
    control->CalSequence[3] = 3;
    control->CalSequence[4] = 4;
    break;

  case cPASS_0:
    control->MaxCalStep = 1;
    control->CalSequence[0] = 0;
    strcpy(CalMessage[0]," - Pedestal and Sigma Estimation:");
    break;

  case cPASS_1:
    control->MaxCalStep = 1;
    control->CalSequence[0] = 1;
    strcpy(CalMessage[1]," - Pedestal computation:");
    break;

  case cPASS_2:
    control->MaxCalStep = 1;
    control->CalSequence[0] = 2;
    strcpy(CalMessage[2]," - Sigma Raw computation:");
    break;

  case cPASS_3:
    control->MaxCalStep = 1;
    control->CalSequence[0] = 3;
    strcpy(CalMessage[3]," - Sigma computation:");
    break;

  case cPASS_4:
    control->MaxCalStep = 1;
    control->CalSequence[0] = 4;
    strcpy(CalMessage[4]," - Common Noise Width computation:");
    break;
  }
  toLOG->Send(kCTL,"Starting Pedestal Calibration",1);
}


void NextCalStep(){
  char aMessage[200];
  int newRun=0;
  if (control->CurrentCalStep >= 0) {
    toDSP->Send(kCTL,kFinishPassCal,0);
    toCTL->TimeOut(kDSP,&par1,&par2,4);
    switch (par1) {
    case kTimeOut:
      toLOG->Send(kCTL,"       TimeOut in NextCalStep",1);
      break;
    case kNextCalStep:
      //toLOG->Send(kCTL,"       Finished",1);
      break;
    }
  } else {newRun=1;}

  control->CurrentCalStep++;  
  if (control->CurrentCalStep < control->MaxCalStep) { 
    sprintf(aMessage,"%s%d%s%6d%s","   >>> Pass: ", control->CurrentCalStep,
	    CalMessage[control->CurrentCalStep],
	    control->EventsStep[control->CurrentCalStep]," events"); 
    toLOG->Send(kCTL,aMessage,1);
    control->CalStep = control->CalSequence[control->CurrentCalStep];   
    control->NEvents_max=control->EventsStep[control->CurrentCalStep];
    StartRun(newRun);
  } else {
    toLOG->Send(kCTL,"Pedestal Calibration Finished",1);
    UpdateRunSummary();
    control->NEvents_max = 0;
    if (control->DataMode != kDATAMODE_COSMIC && 
	control->DataMode != kDATAMODE_SOURCE)
      control->DataMode = kDATAMODE_RAW;
    toGUI->Send(kCTL,kRefresh,0);
    //toDSP->Send(kCTL,kWriteCalSummary,0);  // Write Summary File
    toDSP->Send(kCTL,kSaveCalibration,0);  // Save Calibration file
    control->TestOn = 0; 
    //toGUI->Send(kCTL,kCalibrationFinished,0);
  }
}


void SetupGain(){
  control->TestOn = 1;  
  control->CurrentGainStep = -1; 
  control->MaxGainStep=0;
  memset(control->GainSequence,0,sizeof(control->GainSequence));
  int j=0;
  for (int i=0;i<GAIN_MAX;i++)
    if (control->DACmeasure[i]) {
      control->MaxGainStep++;
      control->GainSequence[j]=i;
      histo->fhDACGain[j]=control->DAC[i];
      histo->fhDACGainError[j]=4.;
      j++;
    }
  mS1 = control->ActiveS1;
  mS2 = control->ActiveS2;
  mK  = control->ActiveK;
  nowS = 1;
  toLOG->Send(kCTL,"Starting Gain Calibration",1);
}

void NextGainStep(){
  int newRun=0;
  char aMessage[200];
  if (control->CurrentGainStep >= 0) {
    toDSP->Send(kCTL,kFinishPassGain,0);
    toCTL->TimeOut(kDSP,&par1,&par2,4);
    switch (par1) {
    case kTimeOut:
      toLOG->Send(kCTL,"       TimeOut in NextGainStep",1);
      break;
    case kNextGainStep:
      //toLOG->Send(kCTL,"       Finished",1);
      break;
    }
  }
  
  control->CloseTestFile = 0;

  if (mS1 == 0 && mS2 == 0) nowS = 0;
  if (mK == 0) nowS = 1;
 
  if ((nowS == 1 && (mS1 == 1 || mS2 == 1)) ||
      (nowS == 0 && (mS1 == 0 || mS2 == 0))) control->CurrentGainStep++;  

  control->GainIndex=control->GainSequence[control->CurrentGainStep];

  if (nowS == 1) {
    control->ActiveS1 = 1 & mS1;
    control->ActiveS2 = 1 & mS2;
    control->ActiveK  = 0;
    if (control->CurrentGainStep==0 && control->FullCalib==0) newRun=1;
    if (control->CurrentGainStep == control->MaxGainStep-1 && mK == 0)
      control->CloseTestFile = 1;   
  } else {
    control->ActiveS1 = 0;
    control->ActiveS2 = 0;
    control->ActiveK  = 1 & mK;
    if (control->CurrentGainStep == 0 && mS1 == 0 && mS2 == 0) newRun=1;    
    if (control->CurrentGainStep == control->MaxGainStep-1)
      control->CloseTestFile = 1;
  }

  if (control->CurrentGainStep < control->MaxGainStep) {
    if ((nowS == 1 && (control->ActiveS1 == 1 || control->ActiveS2 == 1)) ||
	(nowS == 0 &&  control->ActiveK  == 1 )){
      control->NEvents_max=control->DACEvent[control->GainIndex];
      sprintf(aMessage,"   >>> Gain meas. %2d: DAC value:%03d  %6d Events",
	      control->CurrentGainStep,
	      control->DAC[control->GainIndex],
	      control->DACEvent[control->GainIndex]);
      if (nowS == 1) strcat(aMessage,"   S side");
      else           strcat(aMessage,"   K side");
      toLOG->Send(kCTL,aMessage,1);
      StartRun(newRun);
    }
  } else {
    //if ((nowS==1 && (mS1==1 || mS2==1) && mK == 0) || (nowS==0 && mK==1)){
    toLOG->Send(kCTL,"Gain Calibration Finished",1);
    UpdateRunSummary();
    control->NEvents_max = 0;
    control->DataMode = kDATAMODE_RAW;
    toGUI->Send(kCTL,kRefresh,0);
    toDSP->Send(kCTL,kComputeGain,0);
    control->ActiveS1 = mS1;
    control->ActiveS2 = mS2;
    control->ActiveK  = mK;
    control->TestOn = 0;
    //} else {
    //StartRun(newRun);
    //}


    //toGUI->Send(kCTL,kGainFinished,0);
  }

  if (nowS == 0) nowS=1; 
  else           nowS=0;

}

void UpdateRunInfo(){
  switch (control->Source){
  case kSOURCE_SIMULATION:
    sprintf(source,"Simulation ");
    break;
  case kSOURCE_REALDATA:
    sprintf(source,"Real Data  ");
    break;
  case kSOURCE_BINARYFILE:
    sprintf(source,"Binary File");
    break;
  case kSOURCE_ASCIIFILE:
    sprintf(source,"ASCII File ");
    break; 
  case kSOURCE_BEAMTESTX5:
    sprintf(source,"BeamTestX5 ");
    break;
  case kSOURCE_TDRCDP:
    sprintf(source,"TDR CDP ");
    break;
  case kSOURCE_JINF:
    sprintf(source,"JINF ");
    break;
  }

  switch (control->DataMode) {
  case kDATAMODE_RAW:
    sprintf(datamode,"Raw Data            ");
    break;
  case kDATAMODE_CALIBRATION:
    sprintf(datamode,"Pedestal Calibration");
    break;
  case kDATAMODE_REDUCTION:
    sprintf(datamode,"Data Reduction      ");
    break;
  case kDATAMODE_GAIN:
    sprintf(datamode,"Gain Calibration    ");
    break;
  case kDATAMODE_COSMIC:
    sprintf(datamode,"Cosmic Run          ");
    break;
  case kDATAMODE_SOURCE:
    sprintf(datamode,"Source Test         ");
    break;
  case kDSPCALIBRATION:
    sprintf(datamode,"DSP Calibration     ");
    break;
  }
}


void UpdateRunSummary(){
  FILE *ifp;
  int index;
  char comment[200],com[100];
  char aMessage[200];

  strcpy(comment,"-");
  switch (control->DataMode) {
  case kDATAMODE_RAW:
    eventSummary=control->NEvents;
    break;
  case kDATAMODE_CALIBRATION:
    eventSummary=0;
    for (int i=0;i<control->MaxCalStep;i++){
      eventSummary += control->EventsStep[i];
      sprintf(com,"%d:%d-",control->CalSequence[i],control->EventsStep[i]);
      strcat(comment,com);
    }
    if (control->FullCalib == 0){ 
      sprintf(aMessage,"Run %d finished after %d events",
	      control->NRun,eventSummary);
      toLOG->Send(kCTL,aMessage,1);
    }
    break;

  case kDATAMODE_GAIN:
    if (control->FullCalib == 0) eventSummary=0;
    for (int i=0;i<control->MaxGainStep;i++){
      index=control->GainSequence[i];
      eventSummary += control->DACEvent[index];
      if (control->FullCalib == 0) {
	sprintf(com,"%d:%d-",control->DAC[index],control->DACEvent[index]);
	strcat(comment,com);
      } 
    }
    if (control->FullCalib == 1)
      sprintf(comment,"Test %c-%02d",control->AMSTest,
	      control->AMSTestNumber);
    sprintf(aMessage,"Run %d finished after %d events",
	    control->NRun,eventSummary);
    toLOG->Send(kCTL,aMessage,1);
    break;

  case kDATAMODE_REDUCTION:
    break;
  case kDATAMODE_COSMIC:
    sprintf(comment,"Cosmic Id: %05d - ",control->CosmicId);
    if (control->CosmicSequence==0) {
      eventSummary=0;
      for (int i=0;i<control->MaxCalStep;i++){
	eventSummary += control->EventsStep[i];
	sprintf(com,"%d:%d-",control->CalSequence[i],control->EventsStep[i]);
	strcat(comment,com);
      }
    } else {
      eventSummary=control->NEvents;
    }
    break;
  case kDATAMODE_SOURCE:
    switch (control->SourceSequence) {
    case 0:
      sprintf(comment,"Calibration - ");
      eventSummary=0;
      for (int i=0;i<control->MaxCalStep;i++){
	eventSummary += control->EventsStep[i];
	sprintf(com,"%d:%d-",control->CalSequence[i],control->EventsStep[i]);
	strcat(comment,com);
      }
      break;
    case 1:
      sprintf(comment,"Position 2B - ");
      eventSummary=control->NEvents;      
      break;

    case 2:
      sprintf(comment,"Position 2C - ");
      eventSummary=control->NEvents;
      break;

    case 3:
      sprintf(comment,"Position 2D - ");
      eventSummary=control->NEvents;
      break;

    } 
      
    break;
  }

  if (control->OnlyHybrid == 1) {
    sprintf(com,"%s","Only Hybrid");
    strcat(comment,com);
  }
  
  if (control->Trigger == kTRIGGER_EXTERNAL) {
    sprintf(com,"Shaping=%3.1fus",control->Shaping);
    strcat(comment,com);
  }

  ifp=fopen(control->fRunFileName,"a+");
  fprintf(ifp,"%s Run:%05d %5d evts %s %s %s\n",
	  control->TimeStartRun,control->NRun,eventSummary,
	  source,datamode,comment);  
  fclose(ifp);
}


void Cosmic(){
  switch (control->CosmicSequence){
  case 0: // Calibration Sequence
    if (control->CurrentCalStep <  control->MaxCalStep) NextCalStep();
    if (control->CurrentCalStep == control->MaxCalStep){
      control->NEvents_max=6000;
      //control->Trigger=kTRIGGER_EXTERNAL;
      control->CosmicSequence=1;
      control->LogStatus=kLOGSTATUS_BINARY;
      StartRun(1);
    }
    break;
  case 1: // Data taking
    UpdateRunSummary();
    control->CosmicSequence=0;
    DefaultCalibrationConstants();
    SetupCalibration(cPASS_ALL);
    //control->Trigger=kTRIGGER_RANDOM;
    control->LogStatus=kLOGSTATUS_BINARY;
    NextCalStep();
    break;
  }
}


void DSPCalib(int step) {
  switch (step) {
  case 0:
    control->DSPCalibStep=4;
    StartRun(1);
    break;
  case 1:
    toGUI->Send(kCTL,kRefresh,0);
    sleep(1); // wait so that GUI has the time to refresh, before the Stop signal arrives
    //    toDSP->Send(kCTL,kSaveDSPCalib,0);
    break;
  case 2:
    control->DataMode = kDATAMODE_RAW;
    toGUI->Send(kCTL,kRefresh,0);
    break;
  case 3:
    //    control->DataMode=kDSPCALIBRATION;
    control->TDRDSPselect=0;
    CreateNewTdr(control->TDRDSPselect);
    control->DSPCalibStep=0;
    break;
  case 4:
    toDSP->Send(kCTL,kSaveDSPCalib,0);
    control->DSPCalibStep=5;
    break;
  case 5:
    control->TDRDSPselect++;
    if (control->TDRDSPselect<24) CreateNewTdr(control->TDRDSPselect);
    control->DSPCalibStep=6;
    break;
  case 6:
    control->DSPCalibStep=4;
    toDSP->Send(kCTL,kTakeDSPCalib,0);
    break;
  }
}


void OctantCalib(int step) {
  switch (step) {
  case 0:
    control->DataMode=kOCTANTCALIBRATION;
    StartRun(0); // no run number update
    break;
  case 1:
    control->DataMode = kDATAMODE_RAW;
    toGUI->Send(kCTL,kRefresh,0);
    break;
  }
}

void FullCalib(){
  int aTest = 0;
  control->CalibSequence++;
  if (control->Source==kSOURCE_JINF && control->CalibSequence==2) control->CalibSequence++; // we do not care about gain yet...

  printf("welcome to FullCalib, sequ=%d\n", control->CalibSequence);

  switch (control->CalibSequence){
  case 1:
    control->DataMode=kDATAMODE_CALIBRATION;
    SetupCalibration(cPASS_ALL);
    NextCalStep();
    break;

  case 2:
    control->DataMode=kDATAMODE_GAIN;
    SetupGain();
    NextGainStep();
    break;

  case 3:
    control->FullCalib=0; 
    if (control->AMSTest >='A') aTest = 1; 
    printf("Full Calibration finished\n");
    toDSP->Send(kCTL,kWriteCalSummary,aTest);// Write Summary File
    toDSP->Send(kCTL,kSaveCalibration,aTest);// Save Calibration file
    toDSP->Send(kCTL,kSaveGain,aTest); // Save Gain values in a file
    toLOG->Send(kCTL,"Full Calibration finished",1);      
    toGUI->Send(kCTL,kRefresh,0);
    break;
  }
}


void AMSTest(){
  time_t now;
  char mydate[100]; 
  char filename[100],filecom[100];
  char command[200];

  control->TestSequence++;
  switch (control->TestSequence){
  case 1:
    DefaultCalibrationConstants();
    break;

  case 2:
    DefaultGainConstants();
    break;

  case 3:
    break;

  case 4:
    now = time(NULL);
    strftime(mydate,26,"%d/%m/%y %H:%M:%S",localtime(&now));
    //sprintf(filename,"%s/%s-%c-%s-%02d.des",
    //      control->fDirTest,control->fLadderName,control->AMSTest,
    //      control->StationCode,control->AMSTestNumber);
    //sprintf(filecom,"%s/%s.com",control->fDirLadder,control->fLadderName);
    //sprintf(command,"echo %s > temp",mydate);   system(command);
    //sprintf(command,"cat %s >> temp",filename); system(command);
    ////sprintf(command,"cat %s >> temp",filecom);  system(command);
    //usleep(100000);
    //sprintf(command,"rm -f %s",filename);       system(command); 
    //sprintf(command,"mv temp %s",filename);     system(command);

    sprintf(filename,"%s/%s-%c-%s-%02d.sum",
	    control->fDirTest,control->fLadderName,control->AMSTest,
	    control->StationCode,control->AMSTestNumber);
    sprintf(command,"echo %s > temp",mydate);   system(command);
    sprintf(command,"cat %s >> temp",filename); system(command);
    sprintf(command,"rm -f %s",filename);       system(command); 
    sprintf(command,"mv temp %s",filename);     system(command);


    toLOG->Send(kCTL,"Compressing data files...It takes some seconds",1);
    sprintf(filename,"%s/%s-%c-%s-%02d.raw",
	    control->fDirTest,control->fLadderName,control->AMSTest,
	    control->StationCode,control->AMSTestNumber);
    sprintf(command,"bzip2 %s",filename);  system(command);

    control->AMSTest='2';
    control->LogStatus = kLOGSTATUS_NOTLOG;

    toGUI->Send(kCTL,kRefresh,0);
    toLOG->Send(kCTL,"Test finished",1);
    break;	
  }  
}


void SourceTest(){
  char filename[100], command[200];
  int NumberOfEvents = 3000;
  
  switch (control->SourceSequence){
  case 0: // Calibration Sequence
    control->LogStatus = kLOGSTATUS_BINARY;
    if (control->CurrentCalStep <  control->MaxCalStep) NextCalStep();
    if (control->CurrentCalStep == control->MaxCalStep){
      toGUI->Send(kCTL,kSourceAck2b,0);
      par1 = 0;
      toCTL->Wait(kGUI,&mtype,&par1,&par2);
      if (par1 == kSource2bReady) {
	control->NEvents_max=NumberOfEvents;
	//control->Trigger=kTRIGGER_RANDOM;
	control->SourceSequence=1;
	StartRun(1);
      }
    }
    break;
    

  case 1:
    control->LogStatus = kLOGSTATUS_BINARY;
    UpdateRunSummary();
    toGUI->Send(kCTL,kSourceAck2c,0);
    toCTL->Wait(kGUI,&mtype,&par1,&par2);
    if (par1 == kSource2cReady){ 
      control->NEvents_max=NumberOfEvents;
      //control->Trigger=kTRIGGER_RANDOM;
      control->SourceSequence=2;
      StartRun(1);
    }
    break;

  case 2:
    control->LogStatus = kLOGSTATUS_BINARY;
    UpdateRunSummary();
    toGUI->Send(kCTL,kSourceAck2d,0);
    toCTL->Wait(0,&mtype,&par1,&par2);
    if (par1 == kSource2dReady){ 
      control->NEvents_max=NumberOfEvents;
      //control->Trigger=kTRIGGER_RANDOM;
      control->SourceSequence=3;
      StartRun(1);
    }
    break;

  case 3:
    UpdateRunSummary();
    control->DataMode=kDATAMODE_RAW;
    control->NEvents_max=0;

    toDSP->Send(kCTL,kSaveCalibration,1);
    toLOG->Send(kCTL,"Saving calibration file",1);

    toLOG->Send(kCTL,"Compressing data files...It takes some seconds",1);
    sprintf(filename,"%s/%s-%c-%s-%02d-cal.raw",
    	    control->fDirTest,control->fLadderName,control->AMSTest,
    	    control->StationCode,control->AMSTestNumber);
    sprintf(command,"bzip2 %s",filename);  system(command);

    sprintf(filename,"%s/%s-%c-%s-%02d-*.tdr",
    	    control->fDirTest,control->fLadderName,control->AMSTest,
    	    control->StationCode,control->AMSTestNumber);
    sprintf(command,"bzip2 %s",filename);  system(command);

    sprintf(filename,"%s/%s-%c-%s-%02d-*.raw",
    	    control->fDirTest,control->fLadderName,control->AMSTest,
    	    control->StationCode,control->AMSTestNumber);
    sprintf(command,"bzip2 %s",filename);  system(command);


    control->AMSTest='1';
    control->LogStatus = kLOGSTATUS_NOTLOG;

    toGUI->Send(kCTL,kRefresh,0);
    toLOG->Send(kCTL,"Test finished",1);    
    toGUI->Send(kCTL,kEndSourceTest,0);
    break;


  }

  //StartRun(1);
}


void UpdateStation(int aStation){
  switch (aStation){
  case kSTATION_GENEVA:
    sprintf(control->StationCode,"g"); 
    sprintf(control->StationLine,"Geneva");
    control->Station=aStation;
    WriteStation();
    break;       
  case kSTATION_PERUGIA:
    sprintf(control->StationCode,"p"); 
    sprintf(control->StationLine,"Perugia");
    control->Station=aStation;
    WriteStation();
    break;       
  case kSTATION_GYA:
    sprintf(control->StationCode,"a"); 
    sprintf(control->StationLine,"G&A");
    control->Station=aStation;
    WriteStation();
    break;       
  case kSTATION_PREVESSIN:
    sprintf(control->StationCode,"z"); 
    sprintf(control->StationLine,"ETH/Prevessin");
    control->Station=aStation;
    WriteStation();
    break;       
  case kSTATION_EDUARDO:
    sprintf(control->StationCode,"ge"); 
    sprintf(control->StationLine,"Geneva-Eduardo");
    control->Station=aStation;
    WriteStation();
    break;       
  case kSTATION_GIOVANNI:
    sprintf(control->StationCode,"pg"); 
    sprintf(control->StationLine,"Perugia-Giovanni");
    control->Station=aStation;
    WriteStation();
    break;       

  default:
    ReadStation();	
    break;	
  } 

}

void WriteStation(){
  FILE *ifp;
  char aFile[200];
  sprintf(aFile,".stationid");
  ifp=fopen(aFile,"w");
  fprintf(ifp,"%d\n",control->Station);
  fprintf(ifp,"%s\n",control->StationCode);
  fprintf(ifp,"%s\n",control->StationLine);
  fclose(ifp);      
}

void ReadStation(){
  FILE *ifp;
  char aFile[200];
  sprintf(aFile,".stationid");
  ifp=fopen(aFile,"r");
  if (ifp) {
    fscanf(ifp,"%d",&control->Station);
    fscanf(ifp,"%s",control->StationCode);
    fscanf(ifp,"%s",control->StationLine);
    fclose(ifp);
  }
  else printf("no .stationid file, please create one\n");
}


void UpdateLadOctNames() {
  char lowercase[20];
  for (int i=0; i<24; i++) {
    TkLadder *ladder=InnerTracker.FindLadder(control->TrackerXtant,i);
    sprintf(control->LadderNameOctant[i],"%s",ladder->GetLowercaseName());
    printf("%2d: %s\n",i, control->LadderNameOctant[i]);
  }
}

void LoadPreferences() {

  char item[200],prefstring[200];

  FILE *pref=NULL;
  pref=fopen(".prefs","rt");
  if (pref==NULL) return;

  while (!feof(pref)) {
    if (fscanf(pref,"%s %s",&item,&prefstring)>0) { 
      printf("load : %s %s\n", item, prefstring);
      SetPref(item,prefstring);
    }
  }
  
  fclose(pref);

}


void SetPref(char *item, char *prefstring) {

  if (!strcmp(item,"last_ladder=")) sprintf(control->fLadderNameAtExit,"%s",prefstring);
  if (!strcmp(item,"last_tdr=")) control->TDRref=atoi(prefstring);
  if (!strcmp(item,"trigger_to_hold(us)=")) control->Shaping=atof(prefstring);
  if (!strcmp(item,"channel_readout_frequency(MHz)=")) control->ChannelFrequency=atof(prefstring);
  if (!strcmp(item,"EPPport=")) control->EPPport=atoi(prefstring);
  if (!strcmp(item,"PCIport=")) control->PCIport=atoi(prefstring);
  if (!strcmp(item,"PCIcard=")) control->PCIcard=atoi(prefstring);

  if (!strcmp(item,"hardware(pci_epp)=")) {
    if (strcmp(prefstring,"epp")==0) control->Hardware=kHARDWARE_JINF_EPP;
    else control->Hardware=kHARDWARE_JINF_PCI;
  } 

  if (!strcmp(item,"JINJprog=")) sscanf(prefstring,"%x",&control->JINJprog);
  if (!strcmp(item,"JINFprog=")) sscanf(prefstring,"%x",&control->JINFprog);
  if (!strcmp(item,"TDRprog=")) sscanf(prefstring,"%x",&control->TDRprog);
  if (!strcmp(item,"JINFaddr=")) sscanf(prefstring,"%x",&control->JINFaddr);

  if (!strcmp(item,"TKxtant=")) sscanf(prefstring,"%d",&control->TrackerXtant);

  if (!strcmp(item,"PRINTER=")) sprintf(control->Printer,"%s",prefstring);
  if (!strcmp(item,"PRINT_CMD=")) sprintf(control->PrintCmd,"%s",prefstring);
  if (!strcmp(item,"PRINT_OPT=")) sprintf(control->PrintOpt,"%s",prefstring);

}


void SavePreferences() {

  FILE *pref=NULL;
  pref=fopen(".prefs","wt");
  if (pref==NULL) return;
  
  fprintf(pref,"last_ladder= %s\n", control->fLadderName);
  fprintf(pref,"last_tdr= %d\n", control->TDRref);
  fprintf(pref,"trigger_to_hold(us)= %2.1f\n", control->Shaping);
  fprintf(pref,"channel_readout_frequency(MHz)= %f\n", control->ChannelFrequency);
  fprintf(pref,"EPPport= %d\n", control->EPPport);
  fprintf(pref,"PCIport= %d\n", control->PCIport);
  fprintf(pref,"PCIcard= %d\n", control->PCIcard);

  fprintf(pref,"hardware(pci_epp)= %s\n",(control->Hardware==kHARDWARE_JINF_EPP)?"epp":"pci");
  fprintf(pref,"JINJprog= 0x%04x\n", control->JINJprog);
  fprintf(pref,"JINFprog= 0x%04x\n", control->JINFprog);
  fprintf(pref,"TDRprog= 0x%04x\n", control->TDRprog);
  fprintf(pref,"JINFaddr= 0x%04x\n", control->JINFaddr);

  fprintf(pref,"TKxtant= %d\n", control->TrackerXtant);

  fprintf(pref,"PRINTER= %s\n", control->Printer);
  fprintf(pref,"PRINT_CMD= %s\n", control->PrintCmd);
  fprintf(pref,"PRINT_OPT= %s\n", control->PrintOpt);


  fclose(pref);
}


void CreateNewTdr(int tdr) {
  char aMessage[200];

  if (!strcmp(control->LadderNameOctant[tdr],"empty")) 
    sprintf(control->fLadderName,"TDR%03d",tdr); 
  else sprintf(control->fLadderName,"%s",control->LadderNameOctant[tdr]); 
  sprintf(aMessage,"%s",control->fLadderName);
  toLOG->Send(kCTL,aMessage,1);
  toDSP->Send(kCTL,kCreateLadder,0);
}

