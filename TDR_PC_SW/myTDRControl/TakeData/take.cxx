#include "amswcom.h"
#include "amsw_lib.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <math.h>
#include <TDatime.h>
//----------------NI include-------------------------------
#include "TrigClass.hh"
#include "PUtil.h"
#include "Jinf.h"


int ControlOn=1;
int printevent=0;

CalibMemory Prog_3f83; //Typedef from amswcom.h

using namespace std;
TrigClass* trig;

Jinf* J1=0;

typedef struct header {
  int run;    // run number
  int date;        // date
  int time;   // time 
  double gonpar[4];// goniometer parameters
	unsigned int refmaskjj;
	unsigned int refmask[24];
} header;

header Header;

void InitCalParMem(); //For setting values of all of CalibMemory variable
int StartUp();
void ShowHelp(char *cmd);
void PreExit();//When we are using it?
int StartRun(AMSWcom *node, int run_number, int nevents, int scanpos, char *dir);
void StopRun(int dum);
int Calibrate(AMSWcom* node,int run_number,char * addprefix);
void PrintAllEventNumber();

//--------------------------------------------------------------------------------------------
//                              Here comes the main...
//-------------------------------------------------------------------------------------------

int main(int argv, char **argc) {
	int ret=0;
  int NodeType=0;
  char logfilename[255];
  char LOGPATH[255]="./";

  if (argv<3) {
    ShowHelp(argc[0]);
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

  //setting values of Calibmemory variables, usefull for reading calibration memory
  InitCalParMem();

  //create the AMSwire comunication class
  AMSWcom *Main=new AMSWcom();
  //Set the PC inteface to AMSwire PCI or EPP
  Main->SetHW((strstr(argc[0],"epp"))?kAMSW_EPP:kAMSW_PCI);
  if (Main->GetHW()==kAMSW_PCI){
    atexit(PreExit);
  }
  //set the port to talk to
  Main->SetPort(atoi(argc[1]));
  // set the class to non interactive behaviour
	Main->SetBatch();
  // Init the AMSwire class
  if (Main->Init()==ERROR) {
    PRINTF("ERROR: amswire init failed\n");
    ret=1;
    PRINTF("%s\n",(ret)?"ERROR":"READY"); 
    //   TERMINATE
    fclose(logfile);
    if (Main) delete Main;
    return 0;
  }
  //set the time out
  Main->SetTimeOut(10000);
  Main->SetCalParMem(&Prog_3f83);

  //create a new JINF object
  J1= new Jinf("jinf1","JINF_00.conf",0xffff,Main);

	trig=new TrigClass("localhost",1700);
	trig->PrintOff();
	trig->TriggerOff();
	trig->CalibTrig();//to avoid, in case of problem, situations like calibrations with beam; is better to have data taking with no trigger beam and then remove this events...

	TDatime *StartProgTime=new TDatime();

	header Header;

  //-------------------------------------------------
  //              COMMANDS
  //-------------------------------------------------
  if (!strcmp(argc[2],"INIT")) ret=StartUp();
  else if (!strcmp(argc[2],"NEWLOG")){
    fclose(logfile);
    unlink(logfilename);
    logfile=fopen(logfilename,"w");
    fprintf(logfile,"%s:-------------------------------NEW LOG FILE-----------------------------------\n",TakeTime());
    ret=0;
  }
  else if (!strcmp(argc[2],"CALIBDSP")){
    if (argv==3) ret=Calibrate(Main,0,NULL); // without any run parameter sets the run number to 0
    else if (argv==4) ret=Calibrate(Main,atoi(argc[3]),NULL);
    else if (argv>4) ret=Calibrate(Main,atoi(argc[3]),argc[4]);
  }
  else if (!strcmp(argc[2],"GETEVTNUM")) {
    ret=J1->GetEventNumber();
		PrintAllEventNumber();
	}
  else if (!strcmp(argc[2],"RESETCNTS"))
    ret=J1->EventReset();
  else if (!strcmp(argc[2],"STARTRUN")) {
    if (argv<4) {
      ret=1;
			ShowHelp(argc[0]);
    }
		else {
  		Header.date=StartProgTime->GetDate();
  		Header.time=StartProgTime->GetTime();
		  Header.run=atoi(argc[3]);
			Header.refmaskjj=0;
			for (int i=0;i<NTDRS;i++) Header.refmask[i]=0;
			Header.refmask[00]=J1->CPars->refmask;



			if (argv==4) ret=StartRun(Main, atoi(argc[3]), 0, 0, J1->CPars->DATAPATH);
			else if (argv==5) ret=StartRun(Main, atoi(argc[3]), atoi(argc[4]), 0, J1->CPars->DATAPATH);
			else if (argv==6) ret=StartRun(Main, atoi(argc[3]), atoi(argc[4]), atoi(argc[5]), J1->CPars->DATAPATH);
				else if (argv>6) ret=StartRun(Main, atoi(argc[3]), atoi(argc[4]), atoi(argc[5]), argc[6]);
		}
  }
  else {
    ret=1;
    ShowHelp(argc[0]);
    //   TERMINATE
    fclose(logfile);
    if (Main) delete Main;
    if (trig) delete trig;
		if (StartProgTime) delete StartProgTime;
    return 0;
  }

  PRINTF("%s\n",(ret)?"ERROR":"READY");

  //   TERMINATE
  fclose(logfile);
  if (Main) delete Main;
  if (trig) delete trig;
	if (StartProgTime) delete StartProgTime;
  return 0;
}


void ShowHelp(char *cmd) {
  printf("Usage: %s port action [par0] [par1] [...]\n",cmd);
  printf("where action is:\n");
  printf("INIT      : to start initialization of the system\n");
  printf("NEWLOG    : to create a new log file\n");
  printf("CALIBDSP  [RUNNUM] [file_prefix]: to trigger specifically a DSP calibration\n");
  printf("GETEVTNUM : to get the last event number\n");
  printf("RESETCNTS : to reset buffers and counters\n");
  printf("STARTRUN  <RUNNUM> [maxev] [scanpos]: to start a run (needs the run number)\n");
  //--------------not yet implemented---------------------------
  //  printf("LOADCAL   : to load a calibration into the DSP memory\n");
}

void PreExit() {
  amsw_close(-1);
}

int StartUp(){
	int ret=0;
  if(trig->TriggerOff()) return 1;
  ret=J1->Init();
	if(ret) return 1;
  return ret;
}

int Calibrate(AMSWcom* node,int run_number,char * addprefix) {
	trig->CalibTrig();
  trig->TriggerOff();

  int ret=0;

  ret=J1->SetModeJinf(0);
	if (ret) return ret;
  ret=J1->SetModeTdrs(0);
	if (ret) return ret;
  ret=J1->EventReset();
	if (ret) return ret;


  LPRINTF("Calibrating all connected TDRs...\n");
  node->Calibrate(J1->broadcastadd,0);
  TESTRXDONE(node);

  if(trig->TriggerOn()) return 1;

  if(J1->CalIsDone()==0){
    ret=J1->SaveCalibrations(run_number,0,addprefix);
    if(ret) {LPRINTF("Save Calibration Failed ...\n I give Up\n"); exit(2);}
  }

  if(trig->TriggerOff()) return 1;

  J1->EventReset();

  LPRINTF("Turning on the DAQ mode...\n");

  ret=J1->SetModeTdrs();
	if (ret) return ret;
  ret=J1->SetModeJinf(1);
	if (ret) return ret;

  return ret;
}

void InitCalParMem() {
  //-------------------- Program 3f83 ---------------------
  Prog_3f83.Ped=0x1300;
  Prog_3f83.MemPed=kDSP_PM; //kDSP_PM & KDSP_DM taken fron amswcom.h

  Prog_3f83.SigH=0x1b04;
  Prog_3f83.MemSigH=kDSP_PM;
  
  Prog_3f83.SigL=0x1b04;
  Prog_3f83.MemSigL=kDSP_DM;

  Prog_3f83.Stat=0x1702;
  Prog_3f83.MemStat=kDSP_PM;

  Prog_3f83.Sigr=0x1702;
  Prog_3f83.MemSigr=kDSP_DM;

  Prog_3f83.SigHFactor=8*3.5; //3.5; 
  Prog_3f83.SigLFactor=8*2.0; //2.0;

  Prog_3f83.SigrFactor=8*10.0; // precision*real_factor

  Prog_3f83.PedFactor=8.0; // precision
	
	return;
}




int EventReset(AMSWcom *node) {
  int ret=0;

  LPRINTF("Resetting main node counter...\n");
  node->EventReset(0xffff);
  TESTRXDONE(node);

  J1->EventReset();
  TESTRXDONE(node);

	trig->ResetCounter();
	
  return ret;
}

void StopRun(int dum) {
 	trig->TriggerOff();

  ControlOn=0;
  //	printf("I'm stopping the data taking: %d\n",ControlOn);//only for debug

	return;
}

void PrintNumbers(int dum) {

  printevent=1;

	return;
}

int StartRun(AMSWcom *node, int run_number, int nevents, int scanpos, char *dir) {
  if(trig->BeamTrig()) return 1;
  if(trig->TriggerOff()) return 1;
  int ret=0;

	if (!nevents) nevents=-1;	

  ControlOn=1;//if 1 we take data (we're in loop), when becames != 1 (e.g. 0) the system exit from the loop
	
  signal(SIGTERM,StopRun);//killing the PID of the process we call the function StopRun that stop the taking of data in the right way (the param send to StopRun is SIGTERM itself and we need that StopRun accepts a param even if cannot use it)
  signal(SIGINT,StopRun);
	signal(SIGUSR1,PrintNumbers);

  LPRINTF("Starting run %d\n",run_number);

  //----------------data acquisition starts here------------------

  //----------------opening data file for writing-----------------
  char datafilename[255];
  sprintf(datafilename,"%s/%06d_%04d.dat", J1->CPars->DATAPATH, run_number, scanpos);

  FILE *datafile;
  datafile=fopen(datafilename,"wt");//la t a che serve?!
  if (datafile==NULL) {
    PRINTF("Error: file %s could not be created, perhaps the data dir %s doesn't exist ?\n",datafilename, J1->CPars->DATAPATH);
    return 1;
  }
	else {
    unsigned short headsiz=sizeof(Header)/2; // translated into size of 16 bits words
    fwrite(&headsiz, sizeof(headsiz), 1, datafile);
    fwrite(&Header,sizeof(Header),1,datafile);
  }

  int evtcnt=0;//counts the number of data collected

  unsigned short usize=0;
  int size_u_short=sizeof(u_short);

  ret=EventReset(node);

  if(trig->TriggerOn()) return 1;

  LPRINTF("Entering into the Data Acquisition loop...\n");

  while(ControlOn) {
			if (printevent==1) {
			PrintAllEventNumber();
			printevent=0;
		}
    usize=0;
    usize=node->GetEvent(0xffff);//The real data taking from main node!!
    //ret=PrintRXDONE(node);

    if (usize>0) {
      //		  printf("ControlOn: %d\n", ControlOn);//only for debug
      evtcnt++;//another event is took...
      //			printf("Event took: %d\n", evtcnt);//only for debug
      fwrite(&usize, sizeof(usize), 1, datafile);
      fwrite(node->Event,usize*size_u_short,1,datafile);
      int offset=1;   // the off set must be 1 to skip the event number which is the first word of the event
      J1->ExtractEvent(offset,usize); 
      J1->CheckEvent(evtcnt);
    }
    if (evtcnt==nevents) StopRun(0);//if ControlOn becomes 0 we exit from the data taking  loop, if nevents is 0, the condition is false even the first time
  }

  fclose(datafile);
  J1->GetEventNumber();
  PRINTF("Exited from the acquisition, file %s saved.\n", datafilename);
  PRINTF("%d events acquired\n", evtcnt);

  return ret;
}

void PrintAllEventNumber() {
	char numbers[1023];
	
	PRINTF("%6d %s\n", trig->ReadCounter(), J1->PrintAllEventNumber());
	
	return;
	}
