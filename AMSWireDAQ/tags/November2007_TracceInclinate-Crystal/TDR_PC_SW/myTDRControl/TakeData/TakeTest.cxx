#include "amswcom.h"
#include "amsw_lib.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <signal.h>      
#include <unistd.h>
#include <sys/time.h>
#include "TDatime.h"

#define LPRINTF(a...) { fprintf(logfile,"%s: ",TakeTime()); fprintf(logfile,a); fflush(logfile); }
#define PRINTF(a...) { LPRINTF(a); printf(a); fflush(stdout); }
#define TESTRXDONE(a) { ret=PrintRXDONE(a); if (ret) return ret; }

#define PREFPATH "/home/testsys/src/TDR"

#define kPEDS 1
#define kSIGL 2
#define kSIGH 4
#define kSIGR 8
#define kSTAT 16
#define kALL  0x1f

#define kRED 2
#define kRAW 1
#define kMIX 3

#define Ntdrs 24

#define    SUCCESS        0
#define    ERROR          1



typedef struct datades {
  int size[Ntdrs];
  int start[Ntdrs];
  int mode[Ntdrs]; // 1=raw, 2=red, 3=mix
  int error[Ntdrs];
} datades;

typedef struct header {
  int run;    // run number
  int date;        // date
  int time;   // time 
  double gonpar[3];    // goniometer parameters
  char trigger[20];    // trigger type
  char systemname[20]; // guess
} header;




enum sNODETYPE {
  kNODE_TDR=0,
  kNODE_JINF=1,
  kNODE_JINJ=2,
};



u_short dd[25000];
unsigned int Mask=0;
FILE *datafile, *logfile, *rawfile;
int mode[24];

CalibMemory Prog_3f83;

u_short JINFflash=0, JINJflash=0, TDRflash=0; 
int NodeType=0;
int EndLoop=0;
int ControlOn=0;
int ReqNum=0;
int FileFlush=0;
int DoScan=0;
int EventNo=0;
int StopWait=1;

int evtbad=0;
unsigned short maxsize=0;

double CN[Ntdrs][16];

char datafilnam[200];


char DATAPATH[200];// "/DATA2/tb2006jinf/datafiles"
char LOGPATH[200];//  "/DATA/tb2006/logfiles"

header Header;

//------------------------------------------
void InitCalParMem();
void CloseLogFile();
int GetConfig();
int OpenLogFile(int create=0);
int PrintRXDONE(AMSWcom *node);
void ShowHelp(char *cmd) ;
int StartUp(AMSWcom *node, int type);
char* TakeTime();
void PreExit();
void SetModeTdrs(AMSWcom *node, int type);
int ReadSlaveMask(AMSWcom *node, unsigned int addr);
int ShowConnect(SlaveMask mask) ;
//------------------------------------------


int main(int argv, char **argc) {
  int ret=0;

  if (argv<4) {
    ShowHelp(argc[0]);
    return 1;
  }


  int WRITE=1;
  if (strstr(argc[0],"NOWRITE")) WRITE=0;

  if (!strcmp(argc[2],"jinf")) NodeType=kNODE_JINF;
  else if (!strcmp(argc[2],"jinj")) NodeType=kNODE_JINJ;
  else { 
    printf("ERROR: Invalid node type\n");
    ShowHelp(argc[0]);
    return 2;
  }

  if (GetConfig()) return 4;
  if (OpenLogFile()) return 3;


  AMSWcom *Main=new AMSWcom();
  Main->SetHW((strstr(argc[0],"epp"))?kAMSW_EPP:kAMSW_PCI);
  if (Main->GetHW()==kAMSW_PCI) atexit(PreExit);
  Main->SetPort(atoi(argc[1]));

  if (Main->Init()==ERROR) {
    PRINTF("ERROR: amswire init failed\n");
    return 1;
  }

  Main->SetTimeOut(10000);

  memset(dd,0,sizeof(dd));


  InitCalParMem();

  sprintf(Header.systemname,"%s",(NodeType==kNODE_JINF)?"JINF/DOWNSTREAM":"JINJ/UPSTREAM");
  TDatime *StartProgTime=new TDatime();
  Header.date=StartProgTime->GetDate();
  Header.time=StartProgTime->GetTime();

  // actions to take
  
  if (!strcmp(argc[3],"INIT")) ret=StartUp(Main, NodeType );
  else {
    LPRINTF("Unknown command...\n");
    ret=1;
  }

  PRINTF("%s\n",(ret)?"ERROR":"READY");

  CloseLogFile();
  return 0;
}



int StartUp(AMSWcom *node, int type) {
  int ret=0;

  LPRINTF("boot Main... ");
  node->Boot(0xffff, (type==kNODE_JINF)?JINFflash:JINJflash);  // 3381
  sleep(5);
  TESTRXDONE(node);

  ret=ReadSlaveMask(node,0xffff);
  if (ret) return ret;

  LPRINTF("Main: set mode 0...\n");
  node->SetMode(0xffff,0);
  TESTRXDONE(node);

  if (type==kNODE_JINF) {
    LPRINTF("Main: setting delay...\n");
    node->WriteDelay(0xffff,0x96); // delay in 20ns units
    TESTRXDONE(node);
  }

  LPRINTF("All TDRs: boot and load version...\n");
  node->Boot(0x4000, TDRflash);
  TESTRXDONE(node);
  sleep(1);

  SetModeTdrs(node,type);

  LPRINTF("Resetting main node counter...\n");
  node->EventReset(0xffff);
  TESTRXDONE(node);

  LPRINTF("Resetting tdrs counters...\n");
  node->EventReset(0x4000);
  TESTRXDONE(node);

  LPRINTF("Main, setting mode to acquisition...\n");
  node->SetMode(0xffff, 2);
  TESTRXDONE(node);
  sleep(1);

  return ret;
}


void ShowHelp(char *cmd) {
  printf("usage: %s port nodetype action [par0] [par1] [...]\n",cmd);
  printf("where nodetype is:\n");
  printf("jinf for the JINF\n");
  printf("jinj for the JINJ\n\n");
  printf("where action is:\n");
  printf("INIT      : to start initialization of the system\n");
  printf("GETEVTNUM : to get the last event number\n");
  printf("STARTRUN  : to start a run (needs the run number)\n");
  printf("CALIBRAW  : to trigger a raw data calibration\n");
  printf("NEWLOG    : to create a new log file\n");

  printf("CALIBDSP  : to trigger specifically a DSP calibration\n");
  printf("RESETCNT  : to reset buffers and counters\n");
  printf("CALIBGAIN : to start a gain calibration\n");
  printf("LOADCAL   : to load a calibration into the DSP memory\n");
}



int GetConfig() {
  memset(mode,0,sizeof(mode));

  char prefname[100];
  sprintf(prefname,"%s/.TakeData",PREFPATH);
  FILE *file=fopen(prefname,"rt");

  if (file==NULL) {
    PRINTF("ERROR: file .TakeData not found, I need this file\n"); 
    return 1;
  }


  fscanf(file,"%s",&DATAPATH);
  fscanf(file,"%s",&LOGPATH);

  for (int tdr=0; tdr<24; tdr++) {
    fscanf(file, "%d", &mode[tdr]);
    //LPRINTF("TDR %2d will be set to%s%s%s acquisition mode\n",tdr,(mode[tdr]==0)?" NO ":"",(mode[tdr]&1)?" RAW":"",(mode[tdr]&2)?" RED":"");
  }

  fscanf(file, "%hx %hx %hx", &TDRflash, &JINFflash, &JINJflash);

  //LPRINTF("TDR program: 0x%04x\n", TDRflash);
  //LPRINTF("JINF program: 0x%04x\n", JINFflash);
  //LPRINTF("JINJ program: 0x%04x\n", JINJflash);

  return 0;

}

int OpenLogFile(int create) {
  int ret=0;

  time_t tim;
  time(&tim);
  tm *ltim=localtime(&tim);
  char time[50], filename[200], cmd[200];

  logfile=NULL;
  
  if (create) {
    strftime(time,50,"%d%m%Y%H%M%S",ltim);
    sprintf(filename,"%s/log_%s.txt",LOGPATH,time);

    logfile=fopen(filename,"wt");
    if (logfile==NULL) {
      printf("ERROR: log file could not be created\n");
      ret=1;
    }
    
    sprintf(cmd, "rm -f %s/log.txt",LOGPATH);
    system(cmd);
    sprintf(cmd, "ln -s log_%s.txt %s/log.txt", time, LOGPATH);
    system(cmd);
  }
  else {
    sprintf(filename,"%s/log.txt",LOGPATH);
   
    logfile=fopen(filename,"at");
    if (logfile==NULL) {
      printf("ERROR: log file could not be opened\n");
      ret=1;
    }
 
  }
  return ret;
}


void CloseLogFile() {
  fclose(logfile);
}


void InitCalParMem() {
  //   //---------------- Program 3123 ---------------------
  //   Prog_3123.Ped=0x1300;
  //   Prog_3123.MemPed=kDSP_PM;

  //   Prog_3123.SigH=0x1b04;
  //   Prog_3123.MemSigH=kDSP_PM;

  //   Prog_3123.SigL=0x1b04;
  //   Prog_3123.MemSigL=kDSP_PM;

  //   Prog_3123.Stat=0x1702;
  //   Prog_3123.MemStat=kDSP_PM;
  //   Prog_3123.Sigr=0x1701;
  //   Prog_3123.MemSigr=kDSP_DM;

  //   Prog_3123.SigHFactor=1.0;
  //   Prog_3123.SigLFactor=1.0;

  //   //-------------------- Program 3ed3 ---------------------
  //   Prog_3ed3.Ped=0x1300;
  //   Prog_3ed3.MemPed=kDSP_PM;
  
  //   Prog_3ed3.SigH=0x1b04;
  //   Prog_3ed3.MemSigH=kDSP_PM;
  
  //   Prog_3ed3.SigL=0x1b04;
  //   Prog_3ed3.MemSigL=kDSP_DM;

  //   Prog_3ed3.Stat=0x1702;
  //   Prog_3ed3.MemStat=kDSP_PM;

  //   Prog_3ed3.Sigr=0x1700;
  //   Prog_3ed3.MemSigr=kDSP_DM;

  //   Prog_3ed3.SigHFactor=3.5;
  //   Prog_3ed3.SigLFactor=2.0;



  //-------------------- Program 3f83 ---------------------
  Prog_3f83.Ped=0x1300;
  Prog_3f83.MemPed=kDSP_PM;

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


};


int PrintRXDONE(AMSWcom *node) {
  int error=0;
  if (node->GetHW()==kAMSW_EPP) { 
    LPRINTF("rxdone = %04x\n",node->GetRX_DONE());
    if (node->GetRX_DONE()!=0x4180) error=1; 
  }
  if (node->GetHW()==kAMSW_PCI) {
    LPRINTF("rxdone = %d\n",(short) node->GetRX_DONE());
    if (node->GetRX_DONE()!=0) error=1; 
  }


  return error;

}


void PreExit() {
  amsw_close(-1);
}




char* TakeTime() {

  static char ret[100];

  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  sprintf(ret,"%s",asctime (timeinfo));
  ret[strlen(ret)-1]='\0';

  return ret;
}


void SetModeTdrs(AMSWcom *node, int type) {
  struct timeval start,stop;

  gettimeofday(&start,0);

  LPRINTF("Setting mode %d to TDR 0...\n", mode[0]); node->SetMode(0x003f,mode[0]); //TESTRXDONE(node);
  LPRINTF("Setting mode %d to TDR 1...\n", mode[1]); node->SetMode(0x013f,mode[1]); //TESTRXDONE(node);

  if (type==kNODE_JINF) {
    if (mode[4])  {
      LPRINTF("Setting mode %d to TDR 4...\n", mode[4]); 
      node->SetMode(0x043f,mode[4]); //TESTRXDONE(node);
    }
    if (mode[5]) {
      LPRINTF("Setting mode %d to TDR 5...\n", mode[5]); 
      node->SetMode(0x053f,mode[5]); //TESTRXDONE(node);
    }
  }

  gettimeofday(&stop,0);
  LPRINTF("Done... time needed: %.1f ms\n",1000*(stop.tv_sec-start.tv_sec)+1e-3*(stop.tv_usec-start.tv_usec));

}


int ReadSlaveMask(AMSWcom *node, unsigned int addr) {
  int ret=0;
  int expected=0;

  for (int i=0; i<Ntdrs; i++) expected+=(mode[i]>0);

  LPRINTF("Reading slave mask for address 0x%04x\n",addr);

  SlaveMask mask=node->ReadSlaveMask(addr);
  sleep(1);
  TESTRXDONE(node);

  if (node->GetEventSize()) {
    ret=(ShowConnect(mask)!=expected);
    if (ret) LPRINTF("ERROR: not all needed tdrs seem to be connected\n");
  } else LPRINTF("ERROR: no slaves at all are connected to main node\n");
  return ret;
}


int ShowConnect(SlaveMask mask) {
  int ntdrs=0;
  char line[100];
  const int config[24]={0,4,8,0xc,0x10,0x14,0x16,0x12,0xe,0xa,6,2,  1,5,9,0xd,0x11,0x15,0x17,0x13,0xf,0xb,7,3};  
  LPRINTF("Summary:\n");
  const char on[2][5]={"[ ]\0","[X]\0"};
  
  for (int id=0; id<mask.Nmask; id++) {
    unsigned int connect=mask.ID[id];
    if (id==0) Mask=connect;
    LPRINTF("Mask ID=%02x : 0x%06x\n",id,connect);
    
    for (int j=0; j<2; j++) {
      sprintf(line,"");
      for (int i=0; i<12; i++) {
	int val=(connect&(1<<config[j*12+i]))?1:0;
	sprintf(line,"%s %3d:%s", line,config[j*12+i],on[val]);
	ntdrs+=val;
      }
      LPRINTF("%s\n", line);
    }
  }

  return ntdrs;
  
}
