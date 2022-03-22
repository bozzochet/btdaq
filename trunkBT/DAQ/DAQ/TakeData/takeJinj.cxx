#include "amswcom.h"
#include "amsw_lib.h"
#include <cstring> // needed to add this for gcc 4.3
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <signal.h>
#include <math.h>
#include <sys/time.h>
#include <TDatime.h>
//------------------------
#include "TrigClass.h"
#include "PUtil.h"
#include "Jinj.h"
#include "JinjSlave.h"
#include "Jinf.h"
#include "JLV1.h"

#define MAXEVENTSINBUFFER 99999
int eventsinbuffer=MAXEVENTSINBUFFER;

int WritingMode=1;//0 is the AMSBlock mode, 1 the "standard mode

int ControlOn=1;
int printevent=0;
int daq=1;
int step=5000;
int timestep=10;

using namespace std;
TrigClass* trig;

Jinj* JJ=0;

timeval unixtime;

#pragma pack(push,1)

//typedef struct header { // gcc 4.3, considers 'typedef' useless
struct header {//for file writing NOT in AMSBlock
	int run;// run number
	char date[50];// date
	double gonpar[4];// goniometer parameters
	unsigned int refmaskjj;// Jinj refmask
	unsigned int refmask[24];// JinjSlave refmask
};

#pragma pack(pop)

//typedef struct wholeheader { // gcc 4.3, considers 'typedef' useless
struct wholeheader {//for file writing in ASMBlock
	//---- Primary and secondary header ---------------------//
	unsigned short SIZE;
	unsigned short RRRWNODETYPE;
	unsigned short FBITAG;
	unsigned short TIMEMSB;
	unsigned short TIMELSB;
	//---- JMDC data block ----------------------------------//
	unsigned short JMDCSIZE;
	unsigned short JMDCRRRWNODETYPE;

	unsigned short RUNNUMMSB;
	unsigned short RUNNUMLSB;	
	unsigned short RUNTAGMSB;
	unsigned short RUNTAGLSB;
	unsigned short EVTNUMMSB;
	unsigned short EVTNUMLSB;
	unsigned short JMDCTIMEMSB;
	unsigned short JMDCTIMELSB;
	unsigned short JMDCTIMEFINEMSB;
	unsigned short JMDCTIMEFINELSB;
	unsigned short GReservedGroups;
	//---- DSP Slave Format ----------------------------------//
	unsigned short DSPSIZE;
	unsigned short DSPRRRWNODETYPE;

	//	unsigned short DSPCRC16;//Not in struct because it must be wrote after data flow 
};

int StartUp(AMSWcom *node);
void ShowHelp(char *cmd);
void PreExit();
int StartRun(AMSWcom *node, int nevents=0, int fake=0);
void StopRun(int dum);
int Calibrate(AMSWcom* node);
int SaveCalibration(AMSWcom* node, int runnum);
void PrintAllEventNumber(int evtcnt, int sumsize);
void PrintEventNumberTime(int evtcnt, int sumsize);
int StatusJinj(AMSWcom *node, int start, int datasize, int evtnum);
int StatusJinf(AMSWcom *node, int start, int datasize, int evtnum);
int CheckStatus(const char* nodetype, unsigned short status, int evtnum, unsigned short Jinfnum); // gcc 4.3: added 'const'
void HeaderReset(wholeheader* Head, int runtype);
void headerReset(header* Header);


//--------------------------------------------------------------------------------------------
//                              Here comes the main...
//-------------------------------------------------------------------------------------------

int main(int argc, char **argv) {
  int ret=0;
  int NodeType=0;
  char logfilename[512];
  char shortlogfilename[512];
  char LOGPATH[255]="./log/";
  bool FakeFlag=0;//0 means there's a Jinj, if 1 a fake Jinj class will be used (it depends on the program name used [J->Jinj]) 
  
  if (argc<3) {
    ShowHelp(argv[0]);
    return 1;
  }
  
  //openlogfile
  sprintf(logfilename,"%s/log.txt",LOGPATH);
  logfile=fopen(logfilename,"a"); //append mode
  if (logfile==NULL) {
    printf("ERROR: file %s could not be created, perhaps the log dir doesn't exist ?\n",logfilename);
    ret=1;
    printf("%s\n",(ret)?"ERROR":"READY"); 
    return 0;
  }
  sprintf(shortlogfilename,"%s/shortlog.txt", LOGPATH);
  shortlogfile=fopen(shortlogfilename,"a"); //append mode
  if (shortlogfile==NULL) {
    printf("ERROR: file %s could not be created, perhaps the log dir doesn't exist ?\n",shortlogfilename);
    ret=1;
    printf("%s\n",(ret)?"ERROR":"READY"); 
    return 0;
  }
  
  if (strstr(argv[0],"J")) {
    PRINTF("Program %s were called: assuming the presence of a Jinj!\n", argv[0]);
    FakeFlag=0;
  }
  else {
    PRINTF("Program %s were called: using a fake Jinj class!\n", argv[0]);
    FakeFlag=1;
  }
  
  //create the AMSwire comunication class
  AMSWcom *Main=new AMSWcom();
  
  //Set the PC inteface to AMSwire PCI or EPP
  Main->SetHW((strstr(argv[0],"EPP"))?kAMSW_EPP:kAMSW_PCI);//if the program's name contain "EPP" the communication is over EPP, otherwise PCI
  if (Main->GetHW()==kAMSW_PCI){
    atexit(PreExit);
  }
  
  if (strstr(argv[0],"EPP")) {
    PRINTF("Using EPP connection\n");
  }
  else PRINTF("Using PCI connection\n");
  
  //set the port to talk to
  Main->SetPort(atoi(argv[1]));
  // set the class to non interactive behaviour
  Main->SetBatch();
  // Init the AMSwire class
  if (Main->Init()==ERROR) {
    PRINTF("ERROR: amswire init failed\n");
    ret=1;
    PRINTF("%s\n",(ret)?"ERROR":"READY"); 
    //   TERMINATE
    fclose(logfile);
    fclose(shortlogfile);
    if (Main) delete Main;
    return 0;
  }
  //set the time out
  Main->SetTimeOut(10000);
  
  //create a new JINJ object
  JJ= new Jinj("JINJ", "JINJ.conf", 0xffff, Main, FakeFlag);
  
  //create Trigger Object
  JLV1* Jlv1 = JJ->GetJLV1Pointer();
  if (Jlv1) {
    trig=new TrigClass(Jlv1);//JLV1
    PRINTF("There's a JLV1 card! It will be the trigger of the DAQ...\n");
  }
  else {
    trig= new TrigClass("localhost",1700);//NI-USB
    trig->PrintOff();
    int ni=trig->Init();
    if (ni) {
      if (trig) delete trig;
      PRINTF("There's no JLV1 card and no NI-USB! A manual trigger will be used...\n");
      trig=new TrigClass(NULL);//Fake
    }
    else{
      PRINTF("There's no JLV1 card! The NI-USB will be used as trigger...\n");
    }
    trig->TriggerOff();
  }
  
  TDatime *StartProgTime=new TDatime();//there's was in an old version of this DAQ software (maybe is needed only by the "standard" file writing and not when writing in AMSBlock)
  
  ret=JJ->SetDelay();
  printf("SetDelay results in %d\n", ret);
  if (ret) {
    PRINTF("%s\n","ERROR");
    return 0;
  }
  //-------------------------------------------------
  //              COMMANDS
  //-------------------------------------------------
  if (!strcmp(argv[2],"INIT")) {
    ret=StartUp(Main);
  }
  else if (!strcmp(argv[2],"NEWLOG")){
    fclose(logfile);
    unlink(logfilename);
    logfile=fopen(logfilename,"w");
    fprintf(logfile,"%s:-------------------------------NEW LOG FILE-----------------------------------\n",TakeTime());
    ret=0;
  }
  else if (!strcmp(argv[2],"CALIB")){
    printf("running calibration (%d arguments)\n", argc);
    if (argc>2) {
      ret=Calibrate(Main);
      printf("Calibrate(Main) = %d\n", ret);
    }
  }
  else if (!strcmp(argv[2],"SAVECALIB")){
    if (argc<4) {
      ret=1;
      ShowHelp(argv[0]);
    }
    else ret=SaveCalibration(Main, atoi(argv[3])); 
  }
  else if (!strcmp(argv[2],"GETEVTNUM")) {
    int tcount=trig->ReadCounter();
    ret=JJ->GetEventNumber();
    char* ss=JJ->PrintAllEventNumber(tcount);
    if(ss) PRINTF("%d ||%s\n",tcount,ss);
  }
  else if (!strcmp(argv[2],"RESETCNTS")){
    ret=JJ->EventReset();
    trig->ResetCounter();
  }
  else if (!strcmp(argv[2],"TAKEDATA")) {
    if (argc==3) ret=StartRun(Main);
    else if (argc==4) ret=StartRun(Main, atoi(argv[3]));
    else if (argc>4) ret=StartRun(Main, atoi(argv[3]), atoi(argv[4]));  
  }
  else if (!strcmp(argv[2],"COMPLETERUN")) {
    if (argc==3) {
      ret=Calibrate(Main);
      ret+=StartRun(Main);
    }
    else if (argc>3) {
      ret=Calibrate(Main);
      ret+=StartRun(Main, atoi(argv[3]));
    }
  }
  else if (!strcmp(argv[2],"BEAMONITOR")) {
    if (argc<4) {
      ret=1;
      ShowHelp(argv[0]);
    }
    else {
      daq=0;
      step= atoi(argv[3]);//this is the distance number between two trigger displayed 
      StartRun(Main);
    }
  }
  else {
    ret=1;
    ShowHelp(argv[0]);
    //   TERMINATE
    fclose(logfile);
    fclose(shortlogfile);
    if (Main) delete Main;
    if (trig) delete trig;
    if (StartProgTime) delete StartProgTime;
    return 0;
  }

  JJ->SetModeSlaves(0, 0);
  
  PRINTF("%s\n",(ret)?"ERROR":"READY");
  
  //   TERMINATE
  fclose(logfile);
  fclose(shortlogfile);
  if (JJ) delete JJ;
  if (Main) delete Main;
  if (trig) delete trig;
  if (StartProgTime) delete StartProgTime;
  return 0;
}



void ShowHelp(char *cmd) {
	printf("Usage: %s port action <par0> <par1> [par2] [...]\n",cmd);
	printf("where action is:\n");
	printf("INIT                                           : to start initialization of the system\n");
	printf("NEWLOG                                         : to create a new log file\n");
	printf("GETEVTNUM                                      : to get the last event number\n");
	printf("RESETCNTS                                      : to reset buffers and counters\n");
	printf("CALIB                                          : to launch specifically a calibration\n");
	printf("SAVECALIB  <RUN NUM>                           : to save the calibration loaded in the flash memory\n");
	printf("TAKEDATA  [MAX EV]                             : to start a data taking\n");
	printf("COMPLETERUN  [MAX EV]                          : to start a complete run (calibration + data taking)\n");
	printf("BEAMONITOR <STEP>                              : to check if we have triggers\n");
	printf("                                                 <STEP is the event number distance between two event displayed>\n");
}

void PreExit() {
	amsw_close(-1);
}

int StartUp(AMSWcom* node){
  int ret=0;
  
  SLPRINTF("INIT\n");

  ret=JJ->SelfInit();
  if(ret) return ret;
  
  JLV1* Jlv1 = JJ->GetJLV1Pointer();
  
  if (Jlv1) Jlv1->Init();
  
  ////if(trig->TriggerOff()) return 1;
  TESTRXDONE(node);
  
  ret=JJ->InitJinfs();
  if(ret) return ret;

  ret=JJ->SetDelay();//Set Jinfs delays
  if(ret) return ret;
  
  ret=JJ->EventReset();
  if(ret) return ret;
  
  return ret;
}


int Calibrate(AMSWcom* node) {
  int ret = 0, runnum = time(NULL);
  trig->TriggerOff();
  printf("node is %p\n", node);
  TESTRXDONE(node);
  sleep(1);
  for (int ii = 0; ii < JJ->NSlave; ii++) {
    JinjSlave *slave = JJ->GetSlavePointer(ii);
    slave->timestamp = runnum;
    snprintf(slave->myaction, 20, "calib");
  }
  //Stop the calibration for precaution
  for (int ii=0;ii< NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
      Jinf* JINF = (Jinf*)(JJ->GetSlavePointer(ii));
      JINF->Calibrate(node,1);
    }
    else if ((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii], "DAMPE")) {
      c_dampe *dampe = (c_dampe *)(JJ->GetSlavePointer(ii));
      dampe->SetMode(0);
    }
  }
  //----------------------------------

  ret=JJ->SetModeJinj(0);
  printf("SetModeJinj(0)\n");
  if (ret) return ret;
  ret=JJ->SetModeSlaves(0,0);
  printf("SetModeSlaves(0,0)\n");
  if (ret) return ret;
  ret=JJ->EventReset();
  printf("EventReset()\n");
  if (ret) return ret;  

  for (int ii=0;ii<NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
      Jinf* JINF = (Jinf*)(JJ->GetSlavePointer(ii));
      JINF->WriteCalibPar(node);
      JINF->Calibrate(node,0,0x8000);//esterno
    }
    else if ((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"DAMPE")) {
      c_dampe *dampe = (c_dampe *)(JJ->GetSlavePointer(ii));
      dampe->SetMode(1);
    }
  }
  PRINTF("Starting Calibration %d\n", runnum);
  PRINTF(": calibrating all connected TDRs...\n");

  SLPRINTF("CALIBRATION %d\n", runnum);

  if(trig->CalibTriggerOn()) return 1;
  TESTRXDONE(node);

  unsigned short usize=0;

  for (int ii=0;ii<NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
      int addr=ii<<8|0x3f;//cause GenAddress this is useless...
      Jinf* jinf=(Jinf*)(JJ->GetSlavePointer(ii));
      for (int jj=0;jj<NTDRS;jj++){
	if(jinf->CPars->refmask&(1<<jj)) {
	  int address=jinf->GenAddress(jj);
	  PRINTF("Asking for calibration to address %08x \n",address);//only for debug
	  int control=0;
	  int cnt=0;
	  while(control==0){
	    cnt++;
	    usize=node->GetCalibration(address,0x1);//Changing parameter here has no issue...
	    PRINTF("We took %d events for calibration\n", (node->Event[usize-3]));
	    if((node->Event[usize-2]) & 0x1) {//the word before the last word is the calibrations status...
	      if (!((node->Event[usize-2]) & 0xfc)) {
		PRINTF("Calibration finished!\n");
		PRINTF("We took %d events for calibration\n", (node->Event[usize-3]));
		ret=0;
		control=1;
	      }
	      else {
		PRINTF("Calibration finished with errors...\n");
		PRINTF("We took %d events for calibration\n", (node->Event[usize-3]));
		ret=0;
		control=1;
	      }
	    }
	    else sleep(1);
	    if (cnt>6000) {
	      PRINTF("Timeout, calibration not finished!\n");
	      PRINTF("We took %d events for calibration\n", (node->Event[usize-3]));
	      ret=1;
	      control=1;
	    }
	  }
	}
      }
    }
  }

  if(trig->CalibTriggerOff()) return 1;
  TESTRXDONE(node);

  //Stopping calibration
  for (int ii=0;ii<NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
      Jinf* JINF = (Jinf*)(JJ->GetSlavePointer(ii));
      JINF->Calibrate(node,1);
    }
    else if ((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"DAMPE")) {
      c_dampe *dampe = (c_dampe *)(JJ->GetSlavePointer(ii));
      dampe->SetMode(0);
    }
  }
  //----------------------

  TESTRXDONE(node);

  if(ret) {
    PRINTF("Calibration Failed ...\n I give Up\n");
    return 1;
  }

  ret=SaveCalibration(node, runnum);

  if(ret) {
    PRINTF("Save Calibration Failed ...\n I give Up\n");
    return 1;
  }

  JJ->EventReset();

  LPRINTF("Switching to the DAQ mode...\n");

  // ret=JJ->SetModeSlaves();
  // if (ret) return ret;
  ret=JJ->SetModeJinj(1);
  if (ret) return ret;

  return ret;
}

int SaveCalibration(AMSWcom* node, int runnum){
  int ret=0;

  unsigned short usize=0;//a "short" are 16 bit (2 Bytes). sizeof() return the argument's number of Bytes
  int size_u_short=sizeof(u_short); //u_short is a typedef (I don't know in which library) to a generic unsigned short int (however sizeof(u_short) is 2!!)

  unsigned short DSPCRC16=0;//Not in struct because it must be wrote after data flow
  wholeheader Head;

  HeaderReset(&Head,0);

  Head.RUNNUMMSB=runnum>>16;
  Head.RUNNUMLSB=runnum&0xffff;
  Head.RUNTAGMSB=Head.RUNNUMMSB;
  Head.RUNTAGLSB=Head.RUNNUMLSB;

  if (WritingMode==0){
    //AMSBlock writing file mode  
    //----------------opening data file for writing-----------------
    char datafilename[512];
    sprintf(datafilename,"%s/%d.dat", JJ->CPars->CALPATH, runnum);

    struct stat buf;

    if (stat(datafilename,&buf)==0) {
      ret=0;
      char reply[256];
      PRINTF("I've found yet a file %s for Calibration number %d\n", datafilename, runnum);
      PRINTF("You want to ovewrite it ([Y] [N])?\n");
      scanf("%s",reply);
      PRINTF("You reply was %s so now I ",reply);
      if (!strcmp(reply,"Y")) {
	PRINTF("overwrite this file\n");
      }
      else {
	PRINTF("exit\n");
	return 1;
      }
    }

    FILE *datafile;
    datafile=fopen(datafilename,"w");
    if (datafile==NULL) {
      PRINTF("Error: file %s could not be created, perhaps the data dir %s doesn't exist?\n",datafilename, JJ->CPars->CALPATH);
      return 1;
    }

    int offset=0;
    int tdrnum=0;
    gettimeofday(&unixtime, NULL);
    Head.TIMEMSB=(ushort)(unixtime.tv_sec>>16&0xffff);
    Head.TIMELSB=(ushort) (unixtime.tv_sec&0xffff);
    Head.JMDCTIMEMSB=(ushort)(unixtime.tv_sec>>16&0xffff);
    Head.JMDCTIMELSB=(ushort)(unixtime.tv_sec&0xffff);
    Head.JMDCTIMEFINEMSB=(ushort)(unixtime.tv_usec>>16&0xffff);
    Head.JMDCTIMEFINELSB=(ushort)(unixtime.tv_usec&0xffff);

    int DSPNT=Head.DSPRRRWNODETYPE;

    for (int ii=0;ii<NSLAVE;ii++) {
      if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
	int addr=ii<<8|0x3f;//cause GenAddress this is useless...
	Jinf* firstjinf=(Jinf*)(JJ->GetSlavePointer(ii));
	for (int jj=0;jj<NTDRS;jj++){
	  if(firstjinf->CPars->refmask&(1<<jj)) {
	    int address=firstjinf->GenAddress(jj);
	    PRINTF("Getting calibration from address %08x \n",address);//only for debug
	    usize=node->GetCalibration(address,0xef);
	    Head.DSPSIZE=usize*2+4;
	    Head.SIZE=(ushort)(8+2+Head.JMDCSIZE+2+Head.DSPSIZE);

	    offset=0;
	    if(ii==1) offset=450;
	    PRINTF("Offset = %d \n",offset);//only for debug

	    Head.EVTNUMMSB=tdrnum>>16;//	EVTNUM is the sequence number of TDR'request
	    Head.EVTNUMMSB=tdrnum&0xffff;//	EVTNUM is the sequence number of TDR'request

	    PRINTF("EVTNUM = %d \n",tdrnum);//only for debug					
	    tdrnum++;
	    Head.DSPRRRWNODETYPE=DSPNT+((offset+jj)<<5); //come node deve avere il numero del tdr + un offset (cioè tipo da 450 a 473...)

	    fwrite(&(Head.SIZE), sizeof(Head), 1, datafile);
	    fwrite(node->Event,usize*size_u_short, 1, datafile);
	    fwrite(&DSPCRC16, sizeof(DSPCRC16), 1, datafile);
	  }
	}
      }
    }
    
    fclose(datafile);
    
    PRINTF("Calibration %d saved in %s\n", runnum, datafilename);
  }
  
  else if(WritingMode==1){
    for (int ii=0;ii<NSLAVE;ii++) {
      if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
	int addr=ii<<8|0x3f;//cause GenAddress this is useless...
	Jinf* jinf=(Jinf*)(JJ->GetSlavePointer(ii));
	jinf->SaveCalibrations(runnum, JJ->SlaveAdd[ii]);
      }
    }
  }
  
  return ret;
}

int EventReset(AMSWcom *node) {
	int ret=0;

	ret=JJ->SetModeJinj(0);
	if (ret) return ret;
	ret=JJ->SetModeSlaves(0,0);
	if (ret) return ret;

	LPRINTF("Resetting main node counter...\n");
	node->EventReset(0xffff);
	TESTRXDONE(node);

	JJ->EventReset();
	TESTRXDONE(node);

	sleep(1);

	trig->ResetCounter();

	LPRINTF("Turning on the DAQ mode...\n");

	ret=JJ->SetModeSlaves();
	if (ret) return ret;
	ret=JJ->SetModeJinj(1);
	if (ret) return ret;

	node->EventReset(0x4000013f);
	sleep(1);
	node->EventReset(0x4000);
	sleep(1);
	node->EventReset(0xffff);

	sleep(1);

	return ret;
}

void StopRun(int dum) {
  
  // 	printf("StopRun(%d): %d \n", dum, eventsinbuffer);/only for debug

  if (eventsinbuffer==MAXEVENTSINBUFFER){
    trig->TriggerOff();
  }
  else if (eventsinbuffer==0){
    ControlOn=0;
    //  	printf("I'm stopping the data taking: %d\n",ControlOn);//only for debug
  }
  
  eventsinbuffer=eventsinbuffer-1;

  
  //	printf("StopRun(%d): %d \n", dum, eventsinbuffer);//only for debug
  
  return;
}

void PrintNumbers(int dum) {

	printevent=1;
	//  PRINTF("PrintEvent: %d\n", printevent);//only for debug

	return;
}

//=========================================================================================================
int StartRun(AMSWcom *node, int nevents, int fake) {
  
  int ret = 0, runnum = time(NULL);
  
  for (int ii=0; ii < JJ->NSlave; ii++) {
    JinjSlave *slave = JJ->GetSlavePointer(ii);
    slave->timestamp = runnum;
    snprintf(slave->myaction, 20, "data");
  }
  
  int ancillary_code;
  FILE *stream;
  ancillary_code = 0;
  char buffer[512], *pointer;
  /* reading ancillary codes */
  printf("readed: %s\n", JJ->CPars->CONFPATH);
  if ((stream = fopen(JJ->CPars->CONFPATH, "r"))) {
    while ((fgets(buffer, 512, stream))) {
      if ((pointer = strchr(buffer, '='))) {
	*pointer = '\0';
	pointer++;
	if (strstr(buffer, "data"))
	  ancillary_code = atoi(pointer);
      }
    }
    fclose(stream);
    printf("[>>> Ancillary] DATA code %d\n", ancillary_code);
  }
  else
    printf("404 - Ancillary's configuration file missing\n");
  
  if(trig->TriggerOff()) return 1;
  TESTRXDONE(node);
  
  if (!nevents) nevents=-1;
  
  ControlOn=1;//if 1 we take data (we're in loop), when becames != 1 (e.g. 0) the system exit from the loop
  
  signal(SIGTERM,StopRun);//killing the PID of the process we call the function StopRun that stop the taking of data in the right way (the param send to StopRun is SIGTERM itself and we need that StopRun accepts a param even if cannot use it)
  signal(SIGINT,StopRun);// sending 'CTRL_C' the program exits in the right way 
  signal(SIGQUIT,PrintNumbers);//sending 'CTRL \' we print the numbers of taken events
  
  
  unsigned short usize=0;//a "short" are 16 bit (2 Bytes). sizeof() returns argument's number of Bytes
  int size_u_short=sizeof(u_short); //u_short is a typedef (I don't know in which library) to a generic unsigned short int (however sizeof(u_short) is 2!!)
  
  //----------------opening data file for writing-----------------
  char datafilename[512];

  if (ancillary_code<0) {
    sprintf(datafilename,"%s/%d.dat", JJ->CPars->DATAPATH, runnum);
  }
  else {
    sprintf(datafilename,"%s/%d_ANC_%d.dat", JJ->CPars->DATAPATH, runnum, ancillary_code);
  }
  
  struct stat buf;
  
  if (stat(datafilename,&buf)==0) {
    ret=0;
    char reply[256];
    PRINTF("I've found yet a file %s for Run number %d\n", datafilename, runnum);
    PRINTF("You want to ovewrite it ([Y] [N])?\n");
    scanf("%s",reply);
    PRINTF("You reply was %s so now I ",reply);
    if (!strcmp(reply,"Y")) {
      PRINTF("overwrite this file\n");
    }
    else {
      PRINTF("exit\n");
      return 1;
    }
  }
  
  FILE *datafile;
  if (daq) {
    datafile=fopen(datafilename,"wt");
    if (datafile==NULL) {
      PRINTF("Error: file %s could not be created, perhaps the data dir %s doesn't exist?\n",datafilename, JJ->CPars->DATAPATH);
      return 1;
    }
  }
  
  //----------------------preparing headers--------------------------------
  wholeheader Head;//AMSBlock writing used
  unsigned short DSPCRC16=0;//Not in wholeheader struct because it must be wrote after data flow
  
  header Header;//standard way writing used
  
  if (daq){
    if(WritingMode==0){
      //needed by file writing in AMSBlock way
      HeaderReset(&Head,1);
      
      Head.RUNNUMMSB=runnum>>16;
      Head.RUNNUMLSB=runnum&0xffff;
      PRINTF("Starting run %d\n",runnum);
      Head.RUNTAGMSB=Head.RUNNUMMSB;
      Head.RUNTAGLSB=Head.RUNNUMLSB;
    }
    else if(WritingMode==1){
      //needed by file writing in "standard" way (not AMSBlock)
      headerReset(&Header);
      
      sprintf(Header.date,"%s",TakeTime());
      Header.run=runnum;
      Header.refmaskjj=JJ->CPars->refmask;
      for (int i=0;i<JJ->NSlave;i++) {
	Header.refmask[JJ->SlaveAdd[i]]=JJ->Slave[i]->CPars->refmask;
      }
      
      //     PRINTF("Run: %d   Date: %s\n",Header.run,Header.date);//only for debug
      //     for (int ii=0;ii<4;ii++) {
      //       PRINTF("Angle (%d) = %f\n", ii, Header.gonpar[ii]);//only for debug
      //     }
      //     PRINTF("RefmaskJJ: 0x%x\n", Header.refmaskjj);//only for debug
      //     for (int ii=0;ii<24;ii++) {
      //       PRINTF("Refmask (%d) = 0x%x\n", ii, Header.refmask[ii]);//only for debug
      //     }
      
      unsigned short headsiz=sizeof(Header)/2;// translated into size of 16 bits words
      fwrite(&headsiz, sizeof(headsiz), 1, datafile);
      fwrite(&Header,sizeof(Header),1,datafile);
    }
  }
  
  //--------------------preparing the last stuff...-------------------------
  
  int evtcnt=0;//counts the number of data collected
  int sumsize=0;//counts the sum of the sizes of data collected
  
  sleep(1);
  
  ret=EventReset(node);
  
  int errcnt=0;
  
  sleep(3);
  
  if(trig->TriggerOn()) return 1;
  TESTRXDONE(node);
  
  pid_t pid= getpid();
  FILE* pid_file=fopen("./TakeJPCI.pid","w+");
  fprintf(pid_file,"%d\n",pid);
  fclose(pid_file);
  int starttime;
  //---------------------Entering into the Data Acquisition loop...---------------
  if(daq) {
    PRINTF("Entering into the Data Acquisition loop...\n");
  }
  else {
    PRINTF("Entering into the (Fake) Data Acquisition loop...\n");
  }
  
  FILE* nlog=fopen("./log/nlog.txt","w");
  FILE* tlog=fopen("./log/tlog.txt","w");

  int timecounter = 1;
  eventsinbuffer=MAXEVENTSINBUFFER;
  printevent=0;
  
  int logtime;
  starttime = time(NULL);
  printf("DAQ start time %i\n",starttime);
  printf("\n");

 fprintf(tlog,"DAQ start time: %i",starttime);
 fprintf(nlog,"DAQ start time: %i",starttime);

 fclose(nlog);
 fclose(tlog);

  while(ControlOn) {
    //    PrintAllEventNumber(evtcnt, sumsize);//only for debug
    
    logtime = time(NULL);

    if((logtime-starttime)==timecounter*timestep)  {
      //      PRINTF("PrintEvent: %d\n", printevent);//only for debug
      printevent=0;
      // usleep(50000);
      //printf("Unix Time %i\n",logtime);
      PrintEventNumberTime(evtcnt, sumsize);
      sumsize=0;
      ++timecounter;
    }else if((evtcnt%step==0&&evtcnt!=0)||printevent)  {
      //      PRINTF("PrintEvent: %d\n", printevent);//only for debug                                                                          
      printevent=0;
      usleep(50000);
      PrintAllEventNumber(evtcnt, sumsize);
      sumsize=0;
    }


    usize=0;
    
    usize=node->GetEvent(0xffff);//The real data taking from main node!! This give back the size of the event (in unit of 16 bit words)
    
    Head.DSPSIZE=usize*2+4;
    Head.SIZE=(ushort)(8+2+Head.JMDCSIZE+2+Head.DSPSIZE);
    
    if (usize>0) {
      sumsize+=usize;
      //  PRINTF("Counted %d events\n",evtcnt);//only for debug
      int JStatus=node->Event[usize-1];
      if (!(JStatus==0x140 || JStatus==0x340)) {//Matteo duranti 20May2015 to hide the error coming from one TDR
	//      if (JStatus!=0x140){
	PRINTF(" JINJ Error %04hx  Event %d\n", JStatus, evtcnt);
	PrintAllEventNumber(evtcnt, sumsize);
	sumsize=0;
	errcnt++;
      }
      
      evtcnt++;//another event is took...
      //      PRINTF("Event took: %d (size: %d, status: %x)\n", evtcnt, usize, JStatus);//only for debug
      //      for (int ii=0; ii<usize; ii++){
      //	PRINTF("%x\n", node->Event[ii]);//only for debug
      //      }
      
      if(daq){
	if (WritingMode==0){
	  gettimeofday(&unixtime, NULL);
	  Head.TIMEMSB=(ushort)(unixtime.tv_sec>>16&0xffff);
	  Head.TIMELSB=(ushort) (unixtime.tv_sec&0xffff);
	  Head.JMDCTIMEMSB=(ushort)(unixtime.tv_sec>>16&0xffff);
	  Head.JMDCTIMELSB=(ushort)(unixtime.tv_sec&0xffff);
	  Head.JMDCTIMEFINEMSB=(ushort)(unixtime.tv_usec>>16&0xffff);
	  Head.JMDCTIMEFINELSB=(ushort)(unixtime.tv_usec&0xffff);
	  
	  Head.EVTNUMMSB=evtcnt>>16;
	  Head.EVTNUMLSB=evtcnt&0xffff;
	  fwrite(&Head, sizeof(Head), 1, datafile);
	  size_t writtensize=0;
	  writtensize=fwrite(node->Event,size_u_short, usize, datafile);
	  if (writtensize!=usize) {
	    PRINTF("There's a big problem %d %d\n!", (int)writtensize, usize);
	  }
	  fwrite(&DSPCRC16, sizeof(DSPCRC16), 1, datafile);
	}
	else if (WritingMode==1){
	  fwrite(&usize, sizeof(usize), 1, datafile);
	  fwrite(node->Event,usize*size_u_short,1,datafile);
	}
      }
      
      int evtnumoffset=0;/*useful if there's the event number at the beginning of the data flow (the code is made to work if:
			   event# + JINF1-Size ..... JINF1-Status + ... + JINFN-Size .... JINFN-Status)
			   and if usize is the size of data comprised the event# word at the beginning*/
      
      //ret=+StatusJinj(node, evtnumoffset, usize, evtcnt);//The control is yet no working properly...
      
    }
    if (evtcnt==nevents || eventsinbuffer<MAXEVENTSINBUFFER) StopRun(0);//if ControlOn becomes 0 we exit from the data taking loop, if nevents is 0, the condition is false even the first time
  }
  
  if(daq) fclose(datafile);
  
  JJ->GetEventNumber();
  PrintAllEventNumber(evtcnt, sumsize);//only for debug
  if(daq){ 
    PRINTF("Exited from the acquisition, file %s saved.\n", datafilename);
  }
  else {
    PRINTF("Exited from the fake acquisition, run number %d.\n", runnum);
  }
  PRINTF("%d events acquired.\n", evtcnt);
  SLPRINTF("RUN %d [%d events]\n", runnum, evtcnt);
  PRINTF("There were %d errors during run.\n",errcnt);
  unlink("./TakeJPCI.pid");
  
  ancillary_code++;
  //if ((stream = fopen(CPars->CONFPATH, "w"))) {
  //	fprintf(stream, "[INDEXES]\ndata=%d\n", ancillary_code);
  //	fclose(stream);
  //}

  //10 November 2015 - trick to add the stoptime to the file name...
  char newfilename[300];
  int stoptime = time(NULL);
  sprintf(newfilename,"%s/%d_ANC_%d.dat", JJ->CPars->DATAPATH, runnum, stoptime);
  char systemcommand[1024];
  sprintf(systemcommand, "cp -v %s %s", datafilename, newfilename);
  system(systemcommand);
  
  /* lets syncrhonize everything */
  //	system("./synchronize.sh");
  
  return ret;
}

//===============================================================================================================


void PrintAllEventNumber(int evtcnt, int sumsize) {
	char reply[2048];
	static int oldtimes=0;
	static int oldtimeu=0;
	static int oldcount=0;
	timeval unixtime;
	gettimeofday(&unixtime, NULL);

	int newtimes=unixtime.tv_sec;
	int newtimeu=unixtime.tv_usec;

	double deltat=(newtimes-oldtimes)+(newtimeu-oldtimeu)*1.e-6;
	double rate=(evtcnt-oldcount)/deltat;

	//  PRINTF("Sumsize = %d\n");//only for debug
	double meansize= sumsize*1./((evtcnt-oldcount)*1.);

	oldcount=evtcnt;
	oldtimes=newtimes;
	oldtimeu=newtimeu;

	JJ->GetEventNumber();
	sprintf(reply,"%s", JJ->PrintAllEventNumber());

	int cc=0;
	if(!(JJ->GetJLV1Pointer()))  cc=trig->ReadCounter();//ReadCounter works only with NI-USB (so not with JLV1)
	else cc=evtcnt;

	PRINTF("Rate is %f Hz, Mean Event Size is %f words\n", rate, meansize);
	PRINTF("Trigger: %6d \n%s\n----------------------------------------------------------------------------------\n\n", cc, reply);
	FILE* nlog=fopen("./log/nlog.txt","a");
	fprintf(nlog,"\nUnixTime: %i\n",newtimes);
	fprintf(nlog,"Rate is %f Hz, Mean Event Size is %f words\n", rate, meansize);
	fprintf(nlog,"Trigger: %6d \n%s\n----------------------------------------------------------------------------------\n\n", cc, reply);
	fclose(nlog);
	return;
}

void PrintEventNumberTime(int evtcnt, int sumsize) {
  char reply[2048];
  static int oldtimes=0;
  static int oldtimeu=0;
  static int oldcount=0;
  timeval unixtime;
  gettimeofday(&unixtime, NULL);

  int newtimes=unixtime.tv_sec;
  int newtimeu=unixtime.tv_usec;

  oldcount=evtcnt;
  oldtimes=newtimes;
  oldtimeu=newtimeu;

  JJ->GetEventNumber();
  sprintf(reply,"%s", JJ->PrintAllEventNumber());

  int cc=0;
  if(!(JJ->GetJLV1Pointer()))  cc=trig->ReadCounter();//ReadCounter works only with NI-USB (so not with JLV1)                                                    
  else cc=evtcnt;

  FILE* tlog=fopen("./log/tlog.txt","a");
  fprintf(tlog,"\nUnixTime: %i\n",newtimes);
  fprintf(tlog,"Trigger: %6d \n%s\n----------------------------------------------------------------------------------\n\n", cc, reply);
  fclose(tlog);
  return;
}



int StatusJinj(AMSWcom *node, int start, int datasize, int evtnum) {
	int ret=0;
	JinjSlave* aa;
	unsigned short status=0, size=0, slaveID=0;
	int substart=1;//beginning from 1 because 0 refer to the size of the subnode and 1 to the really beginning of subnode dataflow...
	substart+=start;//for skipping the event number
	status=node->Event[substart-1/*we go to the first word of real data flow (in all cases)*/
		+datasize-start/*we go to the first word after the end of the flow*/
		-1/*we finally arrive at status*/];
	ret+=CheckStatus("JINJ", status, evtnum, 255);
	while (substart<datasize-2) {/*substart is the address (of the array, from 0 to ...) of the first word of the data flow (the first after the size word).
				       The last GOOD substart value points before the last Jinf/Jlv1 status (Event[datasize-2]), if the last Jinf/Jlv1 block is empty (size=0 + status)
				       that is Event[datasize-3].*/
		size=node->Event[substart-1];//this give back the size of the event (the first word of the block...)
		status=node->Event[substart+size-1];//this give back the status of the subnode
		slaveID=status & 0x1F;//the 5 LSB of status give the slave ID
		if (aa=JJ->GetSlavePointer(slaveID)) {
			if (aa->CPars->type==0){//it's a Jinf...
				ret+=StatusJinf(node, substart, size, evtnum);
			}
			if (aa->CPars->type==1){//it's a Jlv1...
				ret+=CheckStatus("JLV1", status, evtnum, 255); 
			}
		}
		substart+=size+1;
	}
	return ret;
}

int StatusJinf(AMSWcom *node, int start, int datasize, int evtnum){
	int ret=0;
	Jinf* aa;
	unsigned short status=0, size=0, ID;
	int substart=1;//beginning from 1 because 0 refer to the size of the subnode and 1 to the really beginning of subnode dataflow...
	substart+=start;
	status=node->Event[substart-1/*we go to the first word of real data flow (in all cases)*/
		+datasize-start/*we go to the first word after the end of the flow*/
		-1/*we finally arrive at status*/];
	ret+=CheckStatus("JINF", status, evtnum, 255);
	ID=status & 0x1F;//the 5 LSB of status give the slave ID (the slave of the master that)
	while (substart<datasize-2) {
		size=node->Event[substart-1];//this give back the size of the event (the first word of the block...)
		status=node->Event[substart+size-1];//this give back the status of the subnode
		ret+=CheckStatus("TDR", status, evtnum, ID);
		substart+=size+1;
	}
	return ret;
}

int CheckStatus(const char* nodetype, unsigned short status, int evtnum, unsigned short Jinfnum){ // gcc 4.3: added 'const'
	int ret=0;
	unsigned short ID, CDPnode, mode, cumulerr, builderr, buildconderr, replycode, DATAbit, errors;
	ID=status & 0x1F;
	CDPnode=(status & 0x20)>>5;
	mode=(status & 0xC0)>>6;//0=incative, 1=raw, 2=compressed, 3=mixed
	cumulerr=(status & 0x100)>>8;
	builderr=(status & 0x200)>>9;
	buildconderr=(status & 0x400)>>10;
	replycode=(status & 0x7800)>>11;
	DATAbit=(status & 0x8000)>>15;
	errors=cumulerr+builderr+buildconderr+replycode;
	if (errors){
		PRINTF("There are some errors in event %d, in %s %d", evtnum, nodetype, ID);
		if (Jinfnum>>24) {
			PRINTF(" (JINF %d):\n", Jinfnum);
		}
		else {
			PRINTF(":\n");
		}
		if (cumulerr) PRINTF("Cumulative node status (OR of bits 11-8 in the Node Status Word)\n");
		if (builderr) PRINTF("Build errors (sequence errors (CDP) or event assembly (CDDC))\n");
		if (buildconderr) PRINTF("Build conditions errors (Sub-Detector specific)\n");
		if (replycode) {
			if (DATAbit) {
				if (replycode==0x1) {
					PRINTF("First received packet have no BC 1 bit set\n");
				}
				else if (replycode==0x2) {
					PRINTF("No room to append Bc=11 packet\n");
				}
				else if (replycode==0x3) {
					PRINTF("No valid data\n");
				}
				else if (replycode==0x4) {
					PRINTF("Reserved for EVBLD (event No mismatch)\n");
				}
				else if (replycode==0x5) {
					PRINTF("BC=11 packet with CRC error\n");
				}
				else if (replycode==0x6) {
					PRINTF("Timeout on the reply to NEXT command\n");
				}
				else if (replycode==0x7) {
					PRINTF("0-lenght reply to NEXT command\n");
				}
				else if (replycode==0x8) {
					PRINTF("Intermediate/last packet has wrong BC bit settings\n");
				}
				else if (replycode==0x9) {
					PRINTF("No buffers available for multi-packet transaction\n");
				}
				else if (replycode==0xA) {
					PRINTF("No room to append BC=00/10 packet\n");
				}
				else if (replycode==0xB) {
					PRINTF("Reserved for EVBLD (event No mismatch, multi packet)\n");
				}
				else if (replycode==0xC) {
					PRINTF("Multi-packet reply with CRC error\n");
				}
			}
			else {
				if (replycode==0x1) {
					PRINTF("0-length reply NEXT\n");
				}
				else if (replycode==0x2) {
					PRINTF("0-length reply ABORT\n");
				}
				else if (replycode==0x3) {
					PRINTF("0-length reply ERROR\n");
				}
				else if (replycode==0x4) {
					PRINTF("0-length reply END\n");
				}
				else if (replycode==0x5) {
					PRINTF("Timeout\n");
				}
				else if (replycode==0x6) {
					PRINTF("AMSWire errors\n");
				}
			}
		}
	} 
	return ret;
}

void HeaderReset(wholeheader* Head, int runtype){//0 = calibration, 1 = run

	Head->SIZE=(ushort)0;
	if (runtype) Head->RRRWNODETYPE=(ushort)0x8005;
	else Head->RRRWNODETYPE=(ushort)0x8006;
	Head->FBITAG=(ushort)0;
	Head->TIMEMSB=0;
	Head->TIMELSB=0;
	Head->JMDCSIZE=(ushort)24;
	Head->JMDCRRRWNODETYPE=Head->RRRWNODETYPE;

	Head->RUNNUMMSB=0;
	Head->RUNNUMLSB=0;

	Head->RUNTAGMSB=0;
	Head->RUNTAGLSB=0;

	Head->EVTNUMMSB=0;
	Head->EVTNUMLSB=0;

	Head->JMDCTIMEMSB=0;
	Head->JMDCTIMELSB=0;

	Head->JMDCTIMEFINEMSB=0;
	Head->JMDCTIMEFINELSB=0;
	Head->GReservedGroups=0;
	Head->DSPSIZE=4;
	if (!runtype) Head->DSPRRRWNODETYPE=0x800D;// 1 0 + NODE=(vedi dopo) + TYPE=13
	else Head->DSPRRRWNODETYPE=0x9041;// 1 0 + 130 + TYPE=1
	Head->SIZE=8+2+Head->JMDCSIZE+2+Head->DSPSIZE;
}

void headerReset(header* Header){
	Header->run=0;// run number
	char nulls[10] = "";
	sprintf(Header->date,"%s", nulls);// date
	for (int ii=0; ii<4; ii++){
		Header->gonpar[ii]=0;// goniometer parameters
	}
	Header->refmaskjj=0;// Jinj refmask
	for (int ii=0; ii<NSLAVE; ii++){
		Header->refmask[ii]=0;// JinjSlave refmask
	}
}
