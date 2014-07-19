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


int WritingMode=0;//0 is the AMSBlock mode, 1 the "standard mode

int ControlOn=1;
int printevent=0;
int daq=1;
int step=5000;
int port;

using namespace std;
TrigClass* trig;

Jinj* JJ=0;

timeval unixtime;

int eventsinbuffer=16;

//typedef struct header { // gcc 4.3, considers 'typedef' useless
struct header {//for file writing NOT in AMSBlock
  int run;// run number
  char date[50];// date
  double gonpar[4];// goniometer parameters
  unsigned int refmaskjj;// Jinj refmask
  unsigned int refmask[24];// JinjSlave refmask
};

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
int LaserRun(AMSWcom *node, int nevents=0, int fake=0);
void StopRun(int dum);
int Calibrate(AMSWcom* node, int secondstep);
int SaveCalibration(AMSWcom* node, int runnum);
void PrintAllEventNumber(int evtcnt, int sumsize);
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
  char logfilename[255];
  char shortlogfilename[255];
  char LOGPATH[255]="./";
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
  sprintf(shortlogfilename,"%s/shortlog.txt",LOGPATH);
  shortlogfile=fopen(shortlogfilename,"a"); //append mode
  if (shortlogfile==NULL) {
    printf("ERROR: file %s could not be created, perhaps the log dir doesn't exist ?\n",shortlogfilename);
    ret=1;
    printf("%s\n",(ret)?"ERROR":"READY"); 
    return 0;
  }

  if (strstr(argv[0],"Jinj")) {
    PRINTF("Program %s were called: assuming the presence of a Jinj!\n", argv[0]);
    FakeFlag=0;
  }
  else {
    PRINTF("Program %s were called: using a fake Jinj class!\n", argv[0]);
    FakeFlag=1;
  }

  //create the AMSwire comunication class
  AMSWcom *Main=new AMSWcom();

  //Set the PC inteface to AMSwire PCI or EPP or JMDC, depending on program's name
  int mode=0;
  if (strstr(argv[0],"EPP")) {
    mode = kAMSW_EPP;
    PRINTF("Using EPP connection\n");
  }
  else if (strstr(argv[0],"JMDC")) {
    mode = kAMSW_JMDC;
    PRINTF("Using JMDC connection\n");
  }
  else {
    mode = kAMSW_PCI;
    PRINTF("Using PCI connection\n");
  }
  Main->SetHW(mode);
  
  if (Main->GetHW()==kAMSW_PCI){
    atexit(PreExit);
  }



  //set the port to talk to
  port=atoi(argv[1]);
  Main->SetPort(port);
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
    trig=new TrigClass("localhost",1700);//NI-USB
    trig->PrintOff();
    int ni=trig->Init();
    if (ni) {
      trig=new TrigClass(0);//Fake
      PRINTF("There's no JLV1 card and no NI-USB! A manual trigger will be used...\n");
    }
    else{
      PRINTF("There's no JLV1 card! The NI-USB will be used as trigger...\n");
    }
    trig->TriggerOff();
  }

  TDatime *StartProgTime=new TDatime();//there's was in an old version of this DAQ software (maybe is needed only by the "standard" file writing and not when writing in AMSBlock

  if (ret) {
    PRINTF("%s\n","ERROR");
    return 0;
  }
  
  //-------------------------------------------------
  //              COMMANDS
  //-------------------------------------------------
  if (!strcmp(argv[2],"INIT")) ret=StartUp(Main);
  else if (!strcmp(argv[2],"NEWLOG")){
    fclose(logfile);
    unlink(logfilename);
    logfile=fopen(logfilename,"w");
    fprintf(logfile,"%s:-------------------------------NEW LOG FILE-----------------------------------\n",TakeTime());
    ret=0;
  }
  else if (!strcmp(argv[2],"CALIB")){
    if (argc==3) {
      ret=Calibrate(Main,0);
    }
    else if (argc==4) {
      ret=Calibrate(Main,atoi(argv[3]));
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
  else if (!strcmp(argv[2],"TAKELASER")) {
    if (argc==3) ret=LaserRun(Main);
    else if (argc==4) ret=LaserRun(Main, atoi(argv[3]));
    else if (argc>4) ret=LaserRun(Main, atoi(argv[3]), atoi(argv[4]));  
  }
  else if (!strcmp(argv[2],"COMPLETERUN")) {
    if (argc==3) {
      ret=Calibrate(Main, 0);
      ret+=StartRun(Main);
    }
    else if (argc>3) {
      ret=Calibrate(Main,0);
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
  
  PRINTF("%s\n",(ret)?"ERROR":"READY");

  //   TERMINATE
  fclose(logfile);
  fclose(shortlogfile);
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
  printf("CALIB  [SECONDPASS]                            : to launch specifically a calibration\n");
  printf("                                                 <if SECONDPASS=1, 2nd pass calibration is made>\n");
  printf("SAVECALIB  <RUN NUM>                           : to save the calibration loaded in the flash memory\n");
  printf("TAKEDATA  [MAX EV]                             : to start a data taking\n");
  printf("TAKELASER  [MAX EV]                            : to start a laser data taking\n");
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
  
  if (Jlv1) {
    ret=Jlv1->Init();
    if(ret) return ret;
  }

  unsigned long long int busy=0; //long long int to have 64 bits. We need 40. 1 means that busy from this slave is NOT considered, 0 that is considered.
  ushort busy0x12; //BUSY/TRST Mask 0x12
//     00       Mask of TRST-0
//     01       Mask of TRST-1
//     04-02    Reserved
//     5        Mask of Busy from Sequencer
//     6-7      Reserved
//     15-8     Mask of FE-Busy (bit7-bit0)
  ushort busy0x13; //BUSY Mask 0x13
//     15-0     Mask of FE-Busy (bit23-bit8)
  ushort busy0x14; //BUSY Mask 0x14
//     15-0     Mask of FE-Busy (bit39-bit24)

  //   Flight Crate Link vs FE-Busy bit ---------------------------//
  unsigned long long int CrateBit[24];
  CrateBit[0]=0x3;           //T2
  CrateBit[1]=0xC;           //T3
  CrateBit[2]=0x30;          //U1
  CrateBit[3]=0xC0;          //TO
  CrateBit[4]=0x400;         //S1A
  CrateBit[5]=0x800;         //S1B
  CrateBit[6]=0x4000;        //S0A
  CrateBit[7]=0x8000;        //S0B
  CrateBit[8]=0x30000;       //U0
  CrateBit[9]=0x300;         //T1
  CrateBit[10]=0x300000;     //R0
  CrateBit[11]=0xC00000;     //R1
  CrateBit[12]=0x3000000;    //E0
  CrateBit[13]=0xC000000;    //E1
  CrateBit[14]=0;            //J1A not in busy mask of itself!!!
  CrateBit[15]=0;            //J1B not in busy mask of itself!!!
  CrateBit[16]=0xC0000000;   //T4
  CrateBit[17]=0x30000000;   //T5
  CrateBit[18]=0x4000000;    //S2A
  CrateBit[19]=0x8000000;    //S2B
  CrateBit[20]=0x4000000;    //S3A
  CrateBit[20]=CrateBit[20]<<8; // too large if declared directly
  CrateBit[21]=0x8000000;    //S3B
  CrateBit[21]=CrateBit[21]<<8; // too large if declared directly
  CrateBit[22]=0x3000000;     //T6
  CrateBit[22]=CrateBit[22]<<8; // too large if declared directly
  CrateBit[23]=0x30000000;    //T7
  CrateBit[23]=CrateBit[23]<<8; // too large if declared directly
  //     -----------------------------------------------------------//

  for (int ii=0;ii<NSLAVE;ii++) {
    if(!(JJ->CPars->refmask&(1<<ii))) { // if NOT in refmask (so NOT in conf file) I have to ignore the busy
      busy|=CrateBit[ii];
    }
  }

  busy=busy<<8; //bit 0-7 (of reg0x12) not used in subdetector busy mask

  unsigned long long int temp=0;

  temp=(0xff00);
  busy0x12=(busy&temp)|0x00; // 0x00 is the default value for bit 0-7 (of reg0x12, not used in subdetector busy mask). Is 0 but having specified here maybe useful to mantain (or set) his value for different setups 

  temp=(0xffff)<<8; //a shift of 16 is too large if made in a single pass
  temp=temp<<8; //a shift of 16 is too large if made in a single pass
  //temp=0xffff0000
  busy0x13=(busy&temp)>>16;

  temp=(0xffff)<<8; //a shift of 32 is too large if made in a single pass
  temp=temp<<8; //a shift of 32 is too large if made in a single pass
  temp=temp<<8; //a shift of 32 is too large if made in a single pass
  temp=temp<<8; //a shift of 32 is too large if made in a single pass
  //temp=0xffff00000000
  busy0x14=(busy&temp)>>32;

  // Da prove sull HW riusulta
  busy0x12=0x3000;
  busy0x13=0xfffc;
  busy0x14=0x303f;


  if (Jlv1) {
    ret=Jlv1->WriteBusyMask(busy0x12, busy0x13, busy0x14);
    printf("Writing this Busy Mask %llx (40 bits) on the JLV1...\n", busy>>8); //busy re-shifted back to NOT visualize bit 0-7 (of reg 0x12, not used in subdetector busy mask)
    if(ret) return ret;
  }

  ret=JJ->InitJinfs();
  if(ret) return ret;

  ret=JJ->WriteMask(JJ->CPars->refmask);
  if(ret) return ret;
  
  //  ret=JJ->SetDelay();//Set Jinfs delays
  //  if(ret) return ret;
  
  ret=JJ->EventReset();
  if(ret) return ret;

  return ret;
}


int Calibrate(AMSWcom* node, int secondstep) {
  int ret=0;
  
  trig->TriggerOff();
  TESTRXDONE(node);
  sleep(1);
  
  //Stop the calibration for precaution
  for (int ii=0;ii<NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
      Jinf* JINF = (Jinf*)(JJ->GetSlavePointer(ii));
      JINF->Calibrate(node,1);
    }
  }
  //----------------------------------

  ret=JJ->SetDelay();
  if(ret) return ret;
  
  int steps=1;
  //  if (secondstep) steps=2;

  for (int ss=0; ss<steps; ss++){
    int dtime=0;
    if (secondstep){
      if (ss==0) dtime=secondstep;//maybe it can be set trough a configuration file
      if (ss==1) dtime=170;//maybe it can be set trough a configuration file
    }
    //    printf("Seconstep %d, steps%d, dtime %d\n", secondstep, steps, dtime);//only for debug
    
    ret=JJ->SetModeJinj(0);
    if (ret) return ret;
    ret=JJ->SetModeSlaves(0,0);
    if (ret) return ret;
    ret=JJ->EventReset();
    if (ret) return ret;  
    
    for (int ii=0;ii<NSLAVE;ii++) {
      if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
	Jinf* JINF = (Jinf*)(JJ->GetSlavePointer(ii));
	if (!secondstep){
	  JINF->Calibrate(node,0,0x8000);//external
	  PRINTF("Calibrate %hx 0x8000!!!!!!!!!!!!!!\n", node);
	}
	else {
	  JINF->Calibrate(node,0,0x8008);//external second pass
	  PRINTF("Calibrate %hx 0x8008!!!!!!!!!!!!!!\n", node);
	}
      }
    }
    
    if (secondstep) sleep(2);//to grant the change of run number
    gettimeofday(&unixtime, NULL);
    int runnum=unixtime.tv_sec;
    if (!secondstep){
      PRINTF("Starting Calibration %d\n", runnum);
    }
    else {
      PRINTF("Starting Second Pass Calibration (%d musec) %d\n", dtime, runnum);
    }
    PRINTF(": calibrating all connected TDRs...\n");
    
    if (!secondstep){
      SLPRINTF("CALIBRATION %d\n", runnum);
    }
    else {
      SLPRINTF("CALIBRATION - 2nd PASS (%d musec) %d\n", dtime, runnum);
    }   
    
    if (secondstep){
      if(trig->Calib2PassTriggerOn(dtime)) return 1;
    }
    else {
      if(trig->CalibTriggerOn()) return 1;
    }
    TESTRXDONE(node);
    
    unsigned short usize=0;
    
    //    if (secondstep){
    //      PRINTF("For now waiting 30 seconds and then assuming that calibration is fineshed...\n");
    //      sleep(30);
    //    }
    //    else{
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
		usize=node->GetCalibration(address,0x0);//With the par 0 we're asking for "Current Calibration Status Command" 
		PRINTF("We took %d events for calibration\n", (node->Event[4]));
		if ( (node->Event[4])>=((!secondstep)?5120:2048) ) {
		  PRINTF("Calibration finished!\n");
		  PRINTF("We took %d events for calibration\n", (node->Event[4]));
		  ret=0;
		  control=1;
		}
		else
		  sleep(1);
		if (cnt>60) {
		  PRINTF("Timeout, calibration not finished!\n");
		  PRINTF("We took %d events for calibration\n", (node->Event[4]));
		  ret=1;
		  control=1;
		}
	      }
	    }
	  }
	}
      }
      //    }
    
    
    if(trig->CalibTriggerOff()) return 1;
    TESTRXDONE(node);
    
    //Stopping calibration
    for (int ii=0;ii<NSLAVE;ii++) {
      if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
	Jinf* JINF = (Jinf*)(JJ->GetSlavePointer(ii));
	JINF->Calibrate(node,1);
      }
    }
    //----------------------
    
    TESTRXDONE(node);
    
    if(ret) {
      PRINTF("Calibration Failed ...\n I give Up\n");
      return 1;
    }
    
    ret=SaveCalibration(node, runnum);
    //PRINTF("FOR NOW NOT SAVING CALIBRATIONS!!!!!!!!!!!!!!!!!!\n");
    
    if(ret) {
      PRINTF("Save Calibration Failed ...\n I give Up\n");
      return 1;
    }
    
    JJ->EventReset();
  }
  
  //  ret=JJ->SetModeSlaves(1,2);
  //  PRINTF("FOR NOW, AT THE END OF CALIBRATION SET to 2\n");
  //  if (ret) return ret;
  //  ret=JJ->SetModeJinj(1);
  // if (ret) return ret;
  
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
    char datafilename[255];
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
	    usize=node->GetCalibration(address,0x1);//With the par 1 we're asking for "Calibration"
	    TESTRXDONE(node);
	    Head.DSPSIZE=usize*2+4;
	    Head.SIZE=(ushort)(8+2+Head.JMDCSIZE+2+Head.DSPSIZE);
	    
	    offset=0;
	    if(ii==0) offset=330; //T2
	    if(ii==1) offset=354; //T3
	    if(ii==3) offset=282; //T0
	    if(ii==9) offset=306; //T1
	    if(ii==16) offset=378; //T4
	    if(ii==17) offset=402; //T5
	    if(ii==22) offset=426; //T6
	    if(ii==23) offset=450; //T7
	    PRINTF("Offset = %d \n",offset);//only for debug
	    
	    Head.EVTNUMMSB=tdrnum>>16;//	EVTNUM is the sequence number of TDR'request
	    Head.EVTNUMMSB=tdrnum&0xffff;//	EVTNUM is the sequence number of TDR'request
	    
	    PRINTF("TDRNUM = %d \n",tdrnum);//only for debug					
	    tdrnum++;
	    Head.DSPRRRWNODETYPE=DSPNT+((offset+jj)<<5); //come node deve avere il numero del tdr + un offset (cioè tipo da 450 a 473...)

	    /*
	    for (int ii=0; ii<usize; ii++){
	      if (ii%8 == 0) {
		printf("\n");
	      }
	      printf("%hx ", node->Event[ii]);
	    }
	    printf("\n");
	    */

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
	jinf->SaveCalibrations(runnum, ii);
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
  JJ->EventReset();
  TESTRXDONE(node);

  sleep(1);
  
  trig->ResetCounter();
    
  ret=JJ->SetModeSlaves(1,0);
  if (ret) return ret;
  ret=JJ->SetModeJinj(1);
  if (ret) return ret;

  JJ->EventReset();
  
  sleep(1);
  
  return ret;
}

void StopRun(int dum) {
  
  // 	printf("StopRun(%d): %d \n", dum, eventsinbuffer);/only for debug
  
  if (eventsinbuffer==16){
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
  int ret=0;
  
  if(trig->TriggerOff()) return 1;
  TESTRXDONE(node);

  
  //  if(trig->PulseTrigger()) return 1;
  //  PRINTF("NOW YOU ARE SETTING THE PULSE MODE, IS A TEMPORARY MODE SO USE IT WITH CARE AND AT YOUR OWN RISK!!!!!!!!\n");
  //  TESTRXDONE(node);
  

  if (!nevents) nevents=-1;
  
  ControlOn=1;//if 1 we take data (we're in loop), when becames != 1 (e.g. 0) the system exit from the loop
  
  signal(SIGTERM,StopRun);//killing the PID of the process we call the function StopRun that stop the taking of data in the right way (the param send to StopRun is SIGTERM itself and we need that StopRun accepts a param even if cannot use it)
  signal(SIGINT,StopRun);// sending 'CTRL_C' the program exits in the right way 
  signal(SIGQUIT,PrintNumbers);//sending 'CTRL \' we print the numbers of taken events  
  
  unsigned short usize=0;//a "short" are 16 bit (2 Bytes). sizeof() returns argument's number of Bytes
  int size_u_short=sizeof(u_short); //u_short is a typedef (I don't know in which library) to a generic unsigned short int (however sizeof(u_short) is 2!!)

  //----------------opening data file for writing-----------------
  char datafilename[255];

  gettimeofday(&unixtime, NULL);
  int runnum=unixtime.tv_sec;//the run number is the unix time of the data taking start

  sprintf(datafilename,"%s/%d.dat", JJ->CPars->DATAPATH, runnum);
  
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
  if (ret) return ret;  
  ret=JJ->SetModeJinj(1);
  if (ret) return ret;
  ret=JJ->SetModeSlaves();
  if (ret) return ret;

  int errcnt=0;
  
  sleep(1);
  
  if(trig->TriggerOn()) return 1;
  TESTRXDONE(node);
  
  pid_t pid= getpid();
  FILE* pid_file=fopen("./TakeJPCI.pid","w+");
  fprintf(pid_file,"%d\n",pid);
  fclose(pid_file);

  //---------------------Entering into the Data Acquisition loop...---------------
  if(daq) {
    PRINTF("Entering into the Data Acquisition loop...\n");
  }
  else {
    PRINTF("Entering into the (Fake) Data Acquisition loop...\n");
  }

  eventsinbuffer=16;
  printevent=0;
  while(ControlOn) {
    //    PrintAllEventNumber(evtcnt, sumsize);//only for debug
    if((evtcnt%step==0&&evtcnt!=0)||printevent)  {
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
      
      if (JStatus!=0x40){
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
	  if (writtensize!=usize) PRINTF("There's a big problem %d %d\n!", writtensize, usize);
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
    if (evtcnt==nevents || eventsinbuffer<16) StopRun(0);//if ControlOn becomes 0 we exit from the data taking loop, if nevents is 0, the condition is false even the first time
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
      
  return ret;
}

int LaserRun(AMSWcom *node, int nevents, int fake) {
  int ret=0;

  SLPRINTF("LASER RUN:\n");
 
  ret=EventReset(node);
  if (ret) return ret;  
  ret=JJ->SetModeJinj(0);
  if (ret) return ret;
  ret=JJ->SetModeSlaves(0,0);
  if (ret) return ret;

  sleep(1);

  for (int ii=0;ii<NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
      Jinf* JINF = (Jinf*)(JJ->GetSlavePointer(ii));
      ret+=JINF->WriteLaserPar();
    }
  }

  sleep(1);

  ret=EventReset(node);
  if (ret) return ret;  
  ret=JJ->SetModeJinj(1);
  if (ret) return ret;
  ret=JJ->SetModeSlaves(1,2);
  if (ret) return ret;

  sleep(1);

  ret+=StartRun(node, nevents, fake);

  sleep(1);

  for (int ii=0;ii<NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
      Jinf* JINF = (Jinf*)(JJ->GetSlavePointer(ii));
      ret+=JINF->WriteNoLaserPar();
    }
  }

  sleep(1);

  ret=EventReset(node);
  if (ret) return ret;  
  ret=JJ->SetModeJinj(0);
  if (ret) return ret;
  ret=JJ->SetModeSlaves(0,0);
  if (ret) return ret;

  return ret;
}
//===============================================================================================================


void PrintAllEventNumber(int evtcnt, int sumsize) {
  char numbers[1023];
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

  int cc=0;
  if(!(JJ->GetJLV1Pointer()))  cc=trig->ReadCounter();//ReadCounter works only with NI-USB (so not with JLV1)
  else cc=evtcnt;
  
  PRINTF("Rate is %f Hz, Mean Event Size is %f words\n", rate, meansize);
  PRINTF("Trigger: %6d \n%s\n----------------------------------------------------------------------------------\n\n", cc, JJ->PrintAllEventNumber());
  FILE* nlog=fopen("nlog.txt","a");
  fprintf(nlog,"Rate is %f Hz, Mean Event Size is %f words\n", rate, meansize);
  fprintf(nlog,"Trigger: %6d \n%s\n----------------------------------------------------------------------------------\n\n", cc, JJ->PrintAllEventNumber());
  fclose(nlog);
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
  else {
    if (port==0) Head->DSPRRRWNODETYPE=0x9001;// 1 0 + 128 + TYPE=1
    if (port==1) Head->DSPRRRWNODETYPE=0x9021;// 1 0 + 129 + TYPE=1
    if (port==2) Head->DSPRRRWNODETYPE=0x9041;// 1 0 + 130 + TYPE=1
    if (port==3) Head->DSPRRRWNODETYPE=0x9061;// 1 0 + 131 + TYPE=1
  }
  Head->SIZE=8+2+Head->JMDCSIZE+2+Head->DSPSIZE;
}

void headerReset(header* Header){
  Header->run=0;// run number
  sprintf(Header->date,"");// date
  for (int ii=0; ii<4; ii++){
    Header->gonpar[ii]=0;// goniometer parameters
  }
  Header->refmaskjj=0;// Jinj refmask
  for (int ii=0; ii<NSLAVE; ii++){
    Header->refmask[ii]=0;// JinjSlave refmask
  }
}
