#include "amswcom.h"
#include "amsw_lib.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <signal.h>
#include <math.h>
#include <sys/time.h>
//----------------NI include-------------------------------
#include "PUtil.h"


int ControlOn=1;
int printevent=0;
char logpath[122]="./slowcontrol/";
ushort JinfAddress=0x3f;
ushort QlistFile=0x7101;
int StartRun(AMSWcom *node, int nevents, int period);

void StopRun(int dum) {
  ControlOn=0;
  printf("I'm stopping the data taking: %d\n",ControlOn);//only for debug
  return;
}
void ShowHelp(char *cmd) {
  printf("Usage: %s [port] [nevents] [period_msec]\n",cmd);
  printf("Starts the Slow control data acquisition stop with ctrl+C\n");
}

void PreExit() {
  amsw_close(-1);
}
using namespace std;

timeval unixtime;

typedef struct wholeheader {
//---- Primary and secondary header ---------------------//
	unsigned short SIZE;
	unsigned short RRRWNODETYPE;
	unsigned short TYPE_EXT;
	unsigned short FBITAG;
	int TIMEMSB;
	int TIMELSB;
};




//--------------------------------------------------------------------------------------------
//                              Here comes the main...
//-------------------------------------------------------------------------------------------

int main(int argc, char **argv) {
  int ret=0;
  int NodeType=0;
  char logfilename[255];
  char LOGPATH[255]="./";

  if (argc<4) {
    ShowHelp(argv[0]);
    return 1;
  }

  //openlogfile
  sprintf(logfilename,"%s/log_slow.txt",LOGPATH);
  logfile=fopen(logfilename,"a"); //append mode
  if (logfile==NULL) {
    printf("ERROR: file %s could not be created, perhaps the log dir doesn't exist ?\n",logfilename);
    ret=1;
    printf("%s\n",(ret)?"ERROR":"READY"); 
    return 0;
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
    if (Main) delete Main;
    return 0;
  }
  //set the time out
  Main->SetTimeOut(10000);

  //-------------------------------------------------
  //              COMMANDS
  //-------------------------------------------------
  

  if (argc==4) {
    int nevents=atoi(argv[2]);    
    int period_msec=atoi(argv[3]);
    ret=StartRun(Main,nevents,period_msec);
  }
  else {
    ret=1;
    ShowHelp(argv[0]);
    //   TERMINATE
    fclose(logfile);
    if (Main) delete Main;
      return 0;
  }

  PRINTF("%s\n",(ret)?"ERROR":"READY");

  //   TERMINATE
  fclose(logfile);
  if (Main) delete Main;
  return 0;
}







int StartRun(AMSWcom *node, int nevents, int period) {
  int ret=0;
  ControlOn=1;//if 1 we take data (we're in loop), when becames != 1 (e.g. 0) the system exit from the loop
	
  signal(SIGTERM,StopRun);//killing the PID of the process we call the function StopRun that stop the taking of data in the right way (the param send to StopRun is SIGTERM itself and we need that StopRun accepts a param even if cannot use it)
  signal(SIGINT,StopRun);//'CTRL_Cing' the program it exits in the right way 


  unsigned short usize=0;//lo short sono 16 bit (2 Bytes). sizeof() restituisce il numero di Bytes dell'argomento
  int size_u_short=sizeof(u_short); //u_short is a typedef (I don't know in which library) to a generic unsigned short int (however sizeof(u_short) is 2!!)
			

  unsigned short DSPCRC16=0;//Not in struct because it must be wrote after data flow 
  wholeheader Head;
  
  Head.SIZE=0;
  Head.RRRWNODETYPE=0x905F;
  Head.TYPE_EXT=0x381;
  Head.FBITAG=0;
  Head.TIMEMSB=0;
  Head.TIMELSB=0;
  
 
  
  gettimeofday(&unixtime, NULL);
  int RUNNUM=unixtime.tv_sec;
  LPRINTF("Starting SlowControl run %d\n",RUNNUM);

  
  //----------------opening data file for writing-----------------
  char datafilename[255];
  sprintf(datafilename,"%s/SlowData_%d.dat",logpath , RUNNUM);

  struct stat buf;
  //check if the file already exists
  if (stat(datafilename,&buf)==0) {
    ret=0;
    char reply[256];
    PRINTF("I've found yet a file %s for Run number %d\n", datafilename, RUNNUM);
    PRINTF("You want to ovewrite it ([Y] [N])?\n");
    scanf("%s",reply);
    PRINTF("You reply was %s so now I ",reply);
    if (!strcmp(reply,"Y")) {
      PRINTF("overwriting this file  %s\n",datafilename);
    }
    else {
      PRINTF("exit\n");
      return 1;
    }
  }
  
  FILE *datafile;
  datafile=fopen(datafilename,"wt");
  if (datafile==NULL) {
    PRINTF("Error: file %s could not be created, perhaps the data dir %s doesn't exist?\n",datafilename, logpath);
    return 1;
  }
    
  int evtcnt=0;//counts the number of data collected
  //erase QList
  node->Command(JinfAddress,cAMSW_QLISTER,1,0xffff);
  // Load the Qlist file
  node->FlashLoad(JinfAddress,QlistFile);
  LPRINTF("Entering into the SlowControl  Acquisition loop...\n");

//----------------- pezzo alternativo al fork: ------------------------------------------- 
  pid_t pid= getpid();
  FILE* pid_file=fopen("/home/testsys/TakeSlow.pid","w+");
  fprintf(pid_file,"%d\n",pid);
  fclose(pid_file);

//----------------- con il fork: ---------------------------------------------------------
//   int pid= fork();
//   if(pid) {  //father
//     //PRINTF("READY\n");
//     FILE* pid_file=fopen("/home/testsys/TakeJPCI.pid","w+");
//     fprintf(pid_file,"%d\n",pid);
//     fclose(pid_file);
//     return 0;    
//   }
//   else {  //child

  while(ControlOn) {
    Head.SIZE=12;
    usize=0;
    node->Command(JinfAddress,cAMSW_QLISTRD,0);
    node->PrintRXDONE();
    usize=node->GetEventSize();
    
    if (usize>0) {
      evtcnt++;//another event is took...
      
      Head.SIZE+=usize*2;

      gettimeofday(&unixtime, NULL);
      Head.TIMEMSB=unixtime.tv_sec;
      Head.TIMELSB=unixtime.tv_usec;
      
      fwrite(&Head, sizeof(Head), 1, datafile);
      fwrite(node->Event,usize*size_u_short, 1, datafile);				
      //      fwrite(&DSPCRC16, sizeof(DSPCRC16), 1, datafile);
      usleep(period*1000);
    }
    if (evtcnt==nevents) StopRun(0);//if ControlOn becomes 0 we exit from the data taking loop, if nevents is 0, the condition is false even the first time
  }
  
  fclose(datafile);

  PRINTF("Exited from the SlowControl acquisition, file %s saved.\n", datafilename);
  PRINTF("%d events acquired.\n", evtcnt);
  PRINTF("There were &d errors during run.\n",ret);
  unlink("/home/testsys/TakeSlow.pid");
  
  return ret;
  //    }//serve se uso il fork
}

