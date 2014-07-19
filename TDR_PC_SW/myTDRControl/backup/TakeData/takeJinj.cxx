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
#include "Jinj.h"

int ControlOn=1;
int printevent=0;

CalibMemory Prog_3f83; //Typedef from amswcom.h

using namespace std;
TrigClass* trig;

Jinj* JJ=0;

typedef struct header {
  int run;    // run number
  char date[50];        // date
  double gonpar[4];// goniometer parameters
  unsigned int refmaskjj;
  unsigned int refmask[24];
} header;

header Header;

void InitCalParMem(); //For setting values of all of CalibMemory variable
int StartUp();
void ShowHelp(char *cmd);
void PreExit();//When we are using it?
int StartRun(AMSWcom *node, int run_number, int nevents=0, int scanpos=0, int fake=0);
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
  JJ= new Jinj("JINJ","JinJ.conf",0xffff,Main);

  trig=new TrigClass("localhost",1700);
  trig->PrintOff();
  trig->TriggerOff();
  trig->CalibTrig();//to avoid, in case of problem, situations like calibrations with beam; is better to have data taking with no trigger beam and then remove this events...

  TDatime *StartProgTime=new TDatime();

  ret=JJ->SetDelay();
  if (ret) {
    PRINTF("%s\n","ERROR");
    return 0;
  }

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
    int tcount=trig->ReadCounter();
    ret=JJ->GetEventNumber();
    char* ss=JJ->PrintAllEventNumber();
    if(ss) PRINTF("%d ||%s\n",tcount,ss);
  }
  else if (!strcmp(argc[2],"RESETCNTS")){
    ret=JJ->EventReset();
    trig->ResetCounter();
  }
  else if (!strcmp(argc[2],"STARTRUN")) {
    if (argv<4) {
      ret=1;
      ShowHelp(argc[0]);
    }
    else {
      sprintf(Header.date,"%s",TakeTime());
      Header.run=atoi(argc[3]);
      Header.refmaskjj=JJ->CPars->refmask;
      for (int i=0;i<NTDRS;i++) Header.refmask[i]=0;
      for (int i=0;i<JJ->NSlave;i++) {
	    Header.refmask[JJ->SlaveAdd[i]]=JJ->Slave[i]->CPars->refmask;
      }

      if (argv==4) ret=StartRun(Main, atoi(argc[3]));
      else if (argv==5) ret=StartRun(Main, atoi(argc[3]), atoi(argc[4]));
      else if (argv>6){
	JJ->Slave[0]->OverWriteDelay(atoi(argc[5]));
	JJ->Slave[1]->OverWriteDelay(atoi(argc[6]));
	ret=StartRun(Main, atoi(argc[3]), atoi(argc[4]));
      }
//  else if (argv==6) ret=StartRun(Main, atoi(argc[3]), atoi(argc[4]), atoi(argc[5]));
//       else if (argv>6) ret=StartRun(Main, atoi(argc[3]), atoi(argc[4]), atoi(argc[5]), atoi(argc[6]));
      
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
  printf("STARTRUN  <RUNNUM> [maxev] [scanpos] [CALTRIG]: to start a run (needs the run number)\n");
  //--------------not yet implemented---------------------------
  //  printf("LOADCAL   : to load a calibration into the DSP memory\n");
}

void PreExit() {
  amsw_close(-1);
}







int StartUp(){
  int ret=0;
  if(trig->TriggerOff()) return 1; 
  ret=JJ->Init();
  if(ret) return ret;

  return ret;
}


int Calibrate(AMSWcom* node,int run_number,char * addprefix) {
  trig->CalibTrig();
  trig->TriggerOff();
	
  int ret=0;
  
  ret=JJ->SetModeJinj(0);
	if (ret) return ret;
  ret=JJ->SetModeSlaves(0,0);
	if (ret) return ret;
  ret=JJ->EventReset();
	if (ret) return ret;


  LPRINTF("Calibrating all connected TDRs...\n");
  node->Calibrate(JJ->broadcastadd,0);
  TESTRXDONE(node);
	
  if(trig->TriggerOn()) return 1;
  
  ret=JJ->SaveCalWhenDone(run_number,addprefix);
  if(ret) {LPRINTF("Save Calibration Failed ...\n I give Up\n"); exit(2);}

  if(trig->TriggerOff()) return 1;

  JJ->EventReset();

  LPRINTF("Turning on the DAQ mode...\n");

  ret=JJ->SetModeSlaves();
	if (ret) return ret;
  ret=JJ->SetModeJinj(1);
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

	sleep(1);

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

int StartRun(AMSWcom *node, int run_number, int nevents, int scanpos,int fake) {
  // if(!fake) if(trig->BeamTrig()) return 1;
//  if(trig->CalibTrig()) return 1;

  if(trig->BeamTrig()) return 1;
 
  if(trig->TriggerOff()) return 1;
  int ret=0;

  if (!nevents) nevents=-1;

  ControlOn=1;//if 1 we take data (we're in loop), when becames != 1 (e.g. 0) the system exit from the loop
	
  signal(SIGTERM,StopRun);//killing the PID of the process we call the function StopRun that stop the taking of data in the right way (the param send to StopRun is SIGTERM itself and we need that StopRun accepts a param even if cannot use it)
  signal(SIGINT,StopRun);//'CTRL_Cing' the program it exits in the right way 
  signal(SIGUSR1,PrintNumbers);//sending KILL -USR1 we print the numbers of taken events

  LPRINTF("Starting run %d\n",run_number);

  //----------------data acquisition starts here------------------

  //----------------opening data file for writing-----------------
  char datafilename[255];
  sprintf(datafilename,"%s/%06d_%04d.dat", JJ->CPars->DATAPATH, run_number, scanpos);

  FILE *datafile;
  datafile=fopen(datafilename,"wt");
  if (datafile==NULL) {
    PRINTF("Error: file %s could not be created, perhaps the data dir %s doesn't exist ?\n",datafilename, JJ->CPars->DATAPATH);
    return 1;
  }
  
    
  int evtcnt=0;//counts the number of data collected

  unsigned short usize=0;
  int size_u_short=sizeof(u_short);

  ret=EventReset(node);

  if(trig->TriggerOn()) return 1;

  LPRINTF("Entering into the Data Acquisition loop...\n");

  // pezzo alternativo al fork 
  
  pid_t pid= getpid();
  FILE* pid_file=fopen("/home/testsys/TakeJPCI.pid","w+");
  fprintf(pid_file,"%d\n",pid);
  fclose(pid_file);



 //  int pid= fork();
//   if(pid) {  //father
//     //PRINTF("READY\n");
//     FILE* pid_file=fopen("/home/testsys/TakeJPCI.pid","w+");
//     fprintf(pid_file,"%d\n",pid);
//     fclose(pid_file);
//     return 0;
    
//   }else    //child
    {
      //DEBUG  PRINTF(" %d  %s %X  \n",Header.run,Header.date,Header.refmaskjj);
      //DEBUG  for(int ii=0;ii<24;ii++)  PRINTF(" %X \n",Header.refmask[ii]);
      unsigned short headsiz=sizeof(Header)/2; // translated into size of 16 bits words
      fwrite(&headsiz, sizeof(headsiz), 1, datafile);
      fwrite(&Header,sizeof(Header),1,datafile);
      

      while(ControlOn) {
	//  PrintAllEventNumber();//only for debug
	if((evtcnt-(evtcnt/10000)*10000)==0&&evtcnt>9999)printevent=1;
	if (printevent==1) {
	  PrintAllEventNumber();
	  printevent=0;
	}
	usize=0;
	usize=node->GetEvent(0xffff);//The real data taking from main node!!
	//ret=PrintRXDONE(node);

	if (usize>0) {
	  //---------only for debug------------------------------
	  /*
	    for (int ij=0; ij<usize; ij++) {
	    if (ij%16==0) printf("%5d: ",ij);
	    printf("%04x ",node->Event[ij]);
	    if ((ij+1)%16==0) printf("\n");
	    }
	    //  printf("CRC= 0x%04x\n",node->GetCRC());  
	    printf("\n");
	    printf("Event size = %d\n", node->GetEventSize());
	  */
	  //-----------------------------------------------------
	  //printf("ControlOn: %d\n", ControlOn);//only for debug
	  evtcnt++;//another event is took...
	  //printf("Event took: %d\n", evtcnt);//only for debug
	  fwrite(&usize, sizeof(usize), 1, datafile);
	  fwrite(node->Event,usize*size_u_short,1,datafile);
	  int offset=1;   // the off set must be 1 to skip the event number which is the first word of the event
	  JJ->ExtractEvent(offset,usize);
	  JJ->CheckEvent(evtcnt);
	}
	if (evtcnt==nevents) StopRun(0);//if ControlOn becomes 0 we exit from the data taking  loop, if nevents is 0, the condition is false even the first time
      }

      fclose(datafile);
      JJ->GetEventNumber();
      PrintAllEventNumber();//only for debug
      PRINTF("Exited from the acquisition, file %s saved.\n", datafilename);
      PRINTF("%d events acquired\n", evtcnt);
      unlink("/home/testsys/TakeJPCI.pid");

      return ret;
    }
}

void PrintAllEventNumber() {
  char numbers[1023];
  JJ->GetEventNumber();
  int cc=trig->ReadCounter();
  PRINTF("%6d ||%s\n", cc, JJ->PrintAllEventNumber());
  FILE* nlog=fopen("nlog.txt","a");
  fprintf(nlog,"%6d ||%s\n", cc, JJ->PrintAllEventNumber());
  fclose(nlog);
  return;
}
