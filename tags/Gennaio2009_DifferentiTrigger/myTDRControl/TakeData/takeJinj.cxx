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
//----------------NI include-------------------------------
#include "TrigClass.h"
#include "PUtil.h"
#include "Jinj.h"
#include "JinjSlave.h"
#include "Jinf.h"
#include "JLV1.h"

int ControlOn=1;
int printevent=0;

using namespace std;
TrigClass* trig;

Jinj* JJ=0;

timeval unixtime;

int eventsinbuffer=16;

//typedef struct wholeheader { // gcc 4.3, considers 'typedef' useless
struct wholeheader {
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

int StartUp();
void ShowHelp(char *cmd);
void PreExit();
int StartRun(AMSWcom *node, int nevents=0, int fake=0);
void StopRun(int dum);
int Calibrate(AMSWcom* node, int secondstep=0);
int SaveCalibration(AMSWcom* node, int runnum);
void PrintAllEventNumber(int evtcnt);
int BeaMon(AMSWcom *node, int step);
int StatusJinj(AMSWcom *node, int start, int datasize, int evtnum);
int StatusJinf(AMSWcom *node, int start, int datasize, int evtnum);
int CheckStatus(const char* nodetype, unsigned short status, int evtnum, unsigned short Jinfnum); // gcc 4.3: added 'const'


//--------------------------------------------------------------------------------------------
//                              Here comes the main...
//-------------------------------------------------------------------------------------------

int main(int argc, char **argv) {
  int ret=0;
  int NodeType=0;
  char logfilename[255];
  char LOGPATH[255]="./";

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

  //create a new JINJ object
  JJ= new Jinj("JINJ","JINJ.conf",0xffff,Main);
	
	JLV1* Jlv1 = JJ->GetJLV1Pointer();
	if (!Jlv1) {
		PRINTF("There's no JLV1 card, I use fake trigger class...\n");
  }
	trig=new TrigClass(Jlv1);
  
  if (ret) {
    PRINTF("%s\n","ERROR");
    return 0;
  }

  //-------------------------------------------------
  //              COMMANDS
  //-------------------------------------------------
  if (!strcmp(argv[2],"INIT")) ret=StartUp();
  else if (!strcmp(argv[2],"NEWLOG")){
    fclose(logfile);
    unlink(logfilename);
    logfile=fopen(logfilename,"w");
    fprintf(logfile,"%s:-------------------------------NEW LOG FILE-----------------------------------\n",TakeTime());
    ret=0;
  }
  else if (!strcmp(argv[2],"CALIB")){
    if (argc==3) {
      ret=Calibrate(Main);
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
  else if (!strcmp(argv[2],"COMPLETERUN")) {
    if (argc==3) {
				ret=Calibrate(Main);
				ret+=StartRun(Main);
		}
    else if (argc==4) {
				ret=Calibrate(Main);
				ret+=StartRun(Main, atoi(argv[3]));
		}
    else if (argc==5) {
				ret=Calibrate(Main, atoi(argv[4]));
				ret+=StartRun(Main, atoi(argv[3]));
		}
    else if (argc>5) {
    				ret=Calibrate(Main, atoi(argv[4]));
				ret+=StartRun(Main, atoi(argv[3]), atoi(argv[5]));
		}
  }
	else if (!strcmp(argv[2],"BEAMONITOR")) {
    if (argc<4) {
      ret=1;
      ShowHelp(argv[0]);
    }
    else BeaMon(Main,atoi(argv[3]));//the parameter is the distance number between two trigger displayed 
  }
  else {
    ret=1;
    ShowHelp(argv[0]);
    //   TERMINATE
    fclose(logfile);
    if (Main) delete Main;
    if (trig) delete trig;
    return 0;
  }
  
  PRINTF("%s\n",(ret)?"ERROR":"READY");

  //   TERMINATE
  fclose(logfile);
  if (Main) delete Main;
  if (trig) delete trig;
  return 0;
}



void ShowHelp(char *cmd) {
  printf("Usage: %s port action <par0> <par1> [par2] [...]\n",cmd);
  printf("where action is:\n");
  printf("INIT                                           : to start initialization of the system\n");
  printf("NEWLOG                                         : to create a new log file\n");
  printf("GETEVTNUM                                      : to get the last event number\n");
  printf("RESETCNTS                                      : to reset buffers and counters\n");
  printf("CALIB [SECONDSTEP]                             : to launch specifically a calibration\n");
  printf("                                                 [SECONDSTEP=1 for second step]\n");
  printf("SAVECALIB  <RUN NUM>                           : to save the calibration loaded in the flash memory\n");
  printf("TAKEDATA  [MAX EV] [CALTRIG]                   : to start a data taking\n");
  printf("                                                 [CALTRIG=1 if we want data taking with calibration trigger]\n");
  printf("COMPLETERUN  [MAX EV] [SECONDSTEP] [CALTRIG]   : to start a complete run (calibration + data taking)\n");
  printf("                                                 [SECONDSTEP=1 for second step]\n");
  printf("                                                 [CALTRIG=1 if we want data taking with calibration trigger]\n");
  printf("BEAMONITOR <STEP>                              : to check if we have triggers\n");
  printf("                                                 <STEP is the event number distance between two event displayed>\n");
  //--------------not yet implemented---------------------------
  //  printf("LOADCAL   : to load a calibration into the DSP memory\n");
}

void PreExit() {
  amsw_close(-1);
}

int StartUp(){
  int ret=0;
  
  ret=JJ->SelfInit();
  if(ret) return ret;
  
  JLV1* Jlv1 = JJ->GetJLV1Pointer();
  
  if (Jlv1) Jlv1->Init();
    
  if(trig->TriggerOff()) return 1;   

  ret=JJ->InitJinfs();
  if(ret) return ret;
  
  ret=JJ->SetDelay();//Set Jinfs delays
  if(ret) return ret;
  
  ret=JJ->EventReset();
  if(ret) return ret;

  return ret;
}


int Calibrate(AMSWcom* node, int secondstep) {
	int ret=0;

//  trig->CalibTrig();
  trig->TriggerOff();
	trig->CalibTriggerOff();
	
  for (int ii=0;ii<NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
      int addr=ii<<8|0x4000003f;
      node->Calibrate(addr,1);//Stop the calibration for precaution
    }
  }
	  
  ret=JJ->SetModeJinj(0);
	if (ret) return ret;
  ret=JJ->SetModeSlaves(0,0);
	if (ret) return ret;
  ret=JJ->EventReset();
	if (ret) return ret;  

  for (int ii=0;ii<NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
      int addr=ii<<8|0x4000003f;
      Jinf* JINF = (Jinf*)(JJ->GetSlavePointer(ii));
      for (int jj=0;jj<NTDRS;jj++){
        if(JINF->CPars->refmask&(1<<jj)) {
          JINF->WriteCalibPar(node,jj);
        }
      }
      node->Calibrate(addr,0,0x8000);//esterno
    }
  }
  TESTRXDONE(node);

	gettimeofday(&unixtime, NULL);
	int runnum=unixtime.tv_sec;
	PRINTF("Starting Calibration %d\n", runnum);
	
	PRINTF(": calibrating all connected TDRs...\n");  
  	
  if(trig->CalibTriggerOn()) return 1;

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
							//	jj=100;
							//	ii=100;
								ret=0;
								control=1;
							}
							else {
								PRINTF("Calibration finished with errors...\n");
								PRINTF("We took %d events for calibration\n", (node->Event[usize-3]));
							//	jj=100;
							//	ii=100;
								ret=0;
								control=1;
							}
						}
						else sleep(1);
						if (cnt>60) {
							PRINTF("Timeout, calibration not finished!\n");
							PRINTF("We took %d events for calibration\n", (node->Event[usize-3]));
						//	jj=100;
						//	ii=100;
							ret=1;
							control=1;
						}
					}
				}
			}
		}
	}
	
	if(trig->CalibTriggerOff()) return 1;

	for (int ii=0;ii<NSLAVE;ii++) {
          if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
            int addr=ii<<8|0x4000003f;
            node->Calibrate(addr,1);//Stop the calibration for precaution
          }
        }
	
	TESTRXDONE(node);
	
	int rettemp=ret;
	
       if(secondstep){
	for (int ii=0;ii<NSLAVE;ii++) {
          if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
            int addr=ii<<8|0x3f;//cause GenAddress this is useless...
            Jinf* jinf=(Jinf*)(JJ->GetSlavePointer(ii));
              for (int jj=0;jj<NTDRS;jj++){
	        if(jinf->CPars->refmask&(1<<jj)) {
      		  int address=jinf->GenAddress(jj);
		    PRINTF("Sending second step calibration and asking for finish to address %08x \n",address);
		    node->Calibrate(address,0,0x8008);//the second step of calibration...
	            int control=0;
		    int cnt=0;
		      while(control==0){
			cnt++;
			usize=node->GetCalibration(address,0x1);//Changing parameter here has no issue...
			  PRINTF("We took %d events for second step calibration\n", (node->Event[usize-3]));
			    if((node->Event[usize-2]) & 0x1) {//the word before the last word is the calibrations status...
			      if (!((node->Event[usize-2]) & 0xfc)) {
			        PRINTF("Second step calibration finished!\n");
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
			     if (cnt>60) {
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
	
	  for (int ii=0;ii<NSLAVE;ii++) {
	    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JINF")) {
	      int addr=ii<<8|0x4000003f;
	      node->Calibrate(addr,1);//Stop the calibration for precaution
	    }
	  }
	
	TESTRXDONE(node);
	
      }
	
	ret+=rettemp;

	if(ret) {PRINTF("Calibration Failed ...\n I give Up\n"); return 1;}
	
	ret=SaveCalibration(node, runnum);
	
  if(ret) {PRINTF("Save Calibration Failed ...\n I give Up\n"); return 1;}

  JJ->EventReset();

  LPRINTF("Switching to the DAQ mode...\n");

  ret=JJ->SetModeSlaves();
	if (ret) return ret;
  ret=JJ->SetModeJinj(1);
	if (ret) return ret;

  return ret;
}

int SaveCalibration(AMSWcom* node, int runnum){
	int ret=0;

	unsigned short usize=0;//lo short sono 16 bit (2 Bytes). sizeof() restituisce il numero di Bytes dell'argomento
	int size_u_short=sizeof(u_short); //u_short is a typedef (I don't know in which library) to a generic unsigned short int (however sizeof(u_short) is 2!!)

	unsigned short DSPCRC16=0;//Not in struct because it must be wrote after data flow
	wholeheader Head;
	
	HeaderReset(&Head,0);
	
	Head.RUNNUMMSB=runnum>>16;
	Head.RUNNUMLSB=runnum&0xffff;
	Head.RUNTAGMSB=Head.RUNNUMMSB;
	Head.RUNTAGLSB=Head.RUNNUMLSB;

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
					usize=node->GetCalibration(address,0xcf);
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
	
  return;
}

int StartRun(AMSWcom *node, int nevents, int fake) {
//  if(!fake) if(trig->BeamTrig()) return 1;
//  if(trig->CalibTrig()) return 1;

//  if(trig->BeamTrig()) return 1;
 
  if(trig->TriggerOff()) return 1;
  int ret=0;

  if (!nevents) nevents=-1;

  ControlOn=1;//if 1 we take data (we're in loop), when becames != 1 (e.g. 0) the system exit from the loop
	
  signal(SIGTERM,StopRun);//killing the PID of the process we call the function StopRun that stop the taking of data in the right way (the param send to StopRun is SIGTERM itself and we need that StopRun accepts a param even if cannot use it)
  signal(SIGINT,StopRun);//'CTRL_Cing' the program it exits in the right way 
  signal(SIGUSR1,PrintNumbers);//sending KILL -USR1 we print the numbers of taken events


	unsigned short usize=0;//lo short sono 16 bit (2 Bytes). sizeof() restituisce il numero di Bytes dell'argomento
	int size_u_short=sizeof(u_short); //u_short is a typedef (I don't know in which library) to a generic unsigned short int (however sizeof(u_short) is 2!!)
			

	unsigned short DSPCRC16=0;//Not in struct because it must be wrote after data flow 
	wholeheader Head;
	
	HeaderReset(&Head,1);
	
	gettimeofday(&unixtime, NULL);
	int runnum=unixtime.tv_sec;
	Head.RUNNUMMSB=runnum>>16;
	Head.RUNNUMLSB=runnum&0xffff;
	PRINTF("Starting run %d\n",runnum);
	Head.RUNTAGMSB=Head.RUNNUMMSB;
  Head.RUNTAGLSB=Head.RUNNUMLSB;
	
  //----------------opening data file for writing-----------------
  char datafilename[255];
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
  datafile=fopen(datafilename,"wt");
  if (datafile==NULL) {
    PRINTF("Error: file %s could not be created, perhaps the data dir %s doesn't exist?\n",datafilename, JJ->CPars->DATAPATH);
    return 1;
  }
    
  int evtcnt=0;//counts the number of data collected

  ret=EventReset(node);

  int addr=-1;
  for (int ii=0;ii<NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JLV1")) {
      addr=ii<<8|0x3f;
      break;
    }
  }
  if(addr>0){
    PRINTF("Loading JLV1 configuration for address %x\n", addr);
//    node->WriteJLV1Conf(addr);
//   TESTRXDONE(node);
  }
  else{
    PRINTF("Cannot configure the trigger!! I give Up!\n");
    return 1;
  }
  if(trig->TriggerOn()) return 1;

  PRINTF("Entering into the Data Acquisition loop...\n");

//----------------- pezzo alternativo al fork: ------------------------------------------- 
  pid_t pid= getpid();
  FILE* pid_file=fopen("./TakeJPCI.pid","w+");
  fprintf(pid_file,"%d\n",pid);
  fclose(pid_file);

//----------------- con il fork: ---------------------------------------------------------
//   int pid= fork();
//   if(pid) {  //father
//     //PRINTF("READY\n");
//     FILE* pid_file=fopen("./TakeJPCI.pid","w+");
//     fprintf(pid_file,"%d\n",pid);
//     fclose(pid_file);
//     return 0;    
//   }
//   else {  //child

	eventsinbuffer=16;

      while(ControlOn) {
//				PrintAllEventNumber(evtcnt);//only for debug
				if((evtcnt-(evtcnt/1000)*1000)==0&&evtcnt>999) printevent=1;
				if (printevent==1) {
					PrintAllEventNumber(evtcnt);
					printevent=0;
				}

				usize=0;
				usize=node->GetEvent(0xffff);//The real data taking from main node!! This give back the size of the event (in unit of 16 bit words)
//				ret=PrintRXDONE(node);

				Head.DSPSIZE=usize*2+4;
				Head.SIZE=(ushort)(8+2+Head.JMDCSIZE+2+Head.DSPSIZE);

				if (usize>0) {
				
//---------------only for debug------------------------------
/*
					for (int ij=0; ij<usize; ij++) {
						if (ij%16==0) printf("%5d: ",ij);
						printf("%04x ",node->Event[ij]);
						if ((ij+1)%16==0) printf("\n");
					}
//					printf("CRC= 0x%04x\n",node->GetCRC());  
					printf("\n");
					printf("Event size = %d\n", node->GetEventSize());
*/
//-----------------------------------------------------------

//					printf("ControlOn: %d\n", ControlOn);//only for debug

					evtcnt++;//another event is took...
//					printf("Event took: %d (size: %d)\n", evtcnt, usize);//only for debug


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
					fwrite(node->Event,usize*size_u_short, 1, datafile);
					fwrite(&DSPCRC16, sizeof(DSPCRC16), 1, datafile);
					
					int evtnumoffset=0;/*useful if there's the event number at the beginning of the data flow (the code is made to work if:
															 event# + JINF1-Size ..... JINF1-Status + ... + JINFN-Size .... JINFN-Status)
															 and if usize is the size of data comprised the event# word at the beginning*/ 
					//ret=+StatusJinj(node, evtnumoffset, usize, evtcnt);
				}
				if (evtcnt==nevents || eventsinbuffer<16) StopRun(0);//if ControlOn becomes 0 we exit from the data taking loop, if nevents is 0, the condition is false even the first time
      }

      fclose(datafile);
      JJ->GetEventNumber();
      PrintAllEventNumber(evtcnt);//only for debug
      PRINTF("Exited from the acquisition, file %s saved.\n", datafilename);
      PRINTF("%d events acquired.\n", evtcnt);
			PRINTF("There were %d errors during run.\n",ret)
      unlink("./TakeJPCI.pid");

      return ret;
//    }//serve se uso il fork
}

void PrintAllEventNumber(int evtcnt) {
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

  oldcount=evtcnt;
  oldtimes=newtimes;
  oldtimeu=newtimeu;

  JJ->GetEventNumber();
//  int cc=trig->ReadCounter();
  int cc=evtcnt;
  PRINTF("Rate is %f Hz\n",rate);
  PRINTF("Trigger: %6d \n%s\n----------------------------------------------------------------------------------\n\n", cc, JJ->PrintAllEventNumber());
  FILE* nlog=fopen("nlog.txt","a");
  fprintf(nlog,"Trigger: %6d \n%s\n----------------------------------------------------------------------------------\n\n", cc, JJ->PrintAllEventNumber());
  fclose(nlog);
  return;
}

int BeaMon(AMSWcom *node, int step) {
  //  if(trig->BeamTrig()) return;
  int ret=0;
 
  if(trig->TriggerOff()) return 1;

  ControlOn=1;//if 1 we take data (we're in loop), when becames != 1 (e.g. 0) the system exit from the loop
	
  signal(SIGTERM,StopRun);//killing the PID of the process we call the function StopRun that stop the taking of data in the right way (the param send to StopRun is SIGTERM itself and we need that StopRun accepts a param even if cannot use it)
  signal(SIGINT,StopRun);//'CTRL_Cing' the program it exits in the right way 
  signal(SIGUSR1,PrintNumbers);//sending KILL -USR1 we print the numbers of taken events
   
  int evtcnt=0;//counts the number of data collected

  unsigned short usize=0;

  EventReset(node);
  
  int addr=-1;
  for (int ii=0;ii<NSLAVE;ii++) {
    if((JJ->CPars->refmask&(1<<ii)) && strstr(JJ->CPars->SlaveConfFile[ii],"JLV1")) {
      addr=ii<<8|0x3f;
      break;
    }
  }
  if(addr>0){
    PRINTF("Loading JLV1 configuration for address %x\n", addr);
//    node->WriteJLV1Conf(addr);
//   TESTRXDONE(node);
  }
  else{
    PRINTF("Cannot configure the trigger!! I give Up!\n");
    return 1;
  }
  if(trig->TriggerOn()) return 1;

  PRINTF("Entering into the Fake Data Acquisition loop...\n");

  // pezzo alternativo al fork 
  
  pid_t pid= getpid();
  FILE* pid_file=fopen("./TakeJPCI.pid","w+");
  fprintf(pid_file,"%d\n",pid);
  fclose(pid_file);



  //  int pid= fork();
  //   if(pid) {  //father
  //     //PRINTF("READY\n");
  //     FILE* pid_file=fopen("./TakeJPCI.pid","w+");
  //     fprintf(pid_file,"%d\n",pid);
  //     fclose(pid_file);
  //     return 0;
    
  //   }else    //child
  {      

    int evtstore=0;
    eventsinbuffer=16;


    while(ControlOn) {
      //  PrintAllEventNumber(evtcnt);//only for debug
      //      if((evtcnt%step)==0 ) printevent=1;
      //  if (printevent==1 && evtstore!=evtcnt) {
      if(evtcnt%step==0 ) {
	PrintAllEventNumber(evtcnt);
	//printevent=0;
      }
      //      evtstore=evtcnt;
      //				printf("Evtstore: %d | Evtcnt: %d\n", evtstore, evtcnt);
      usize=0;
      usize=node->GetEvent(0xffff);//The real data taking from main node!!
       if (usize>0) 
      evtcnt++;//another event is took...
      
      if (eventsinbuffer<16) StopRun(0);//if ControlOn becomes 0 we exit from the data taking loop, if nevents is 0, the condition is false even the first time
	      
    }

    PRINTF("Exited from the fake acquisition.\n");
    unlink("./TakeJPCI.pid");

    return ret;
  }
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
				if (replycode==0x1) PRINTF("First received packet have no BC 1 bit set\n");
				if (replycode==0x2) PRINTF("No room to append Bc=11 packet\n");
				if (replycode==0x3) PRINTF("No valid data\n");
				if (replycode==0x4) PRINTF("Reserved for EVBLD (event No mismatch)\n");
				if (replycode==0x5) PRINTF("BC=11 packet with CRC error\n");
				if (replycode==0x6) PRINTF("Timeout on the reply to NEXT command\n");
				if (replycode==0x7) PRINTF("0-lenght reply to NEXT command\n");
				if (replycode==0x8) PRINTF("Intermediate/last packet has wrong BC bit settings\n");
				if (replycode==0x9) PRINTF("No buffers available for multi-packet transaction\n");
				if (replycode==0xA) PRINTF("No room to append BC=00/10 packet\n");
				if (replycode==0xB) PRINTF("Reserved for EVBLD (event No mismatch, multi packet)\n");
				if (replycode==0xC) PRINTF("Multi-packet reply with CRC error\n");
			}
			else {
				if (replycode==0x1) PRINTF("0-length reply NEXT\n");
				if (replycode==0x2) PRINTF("0-length reply ABORT\n");
				if (replycode==0x3) PRINTF("0-length reply ERROR\n");
				if (replycode==0x4) PRINTF("0-length reply END\n");
				if (replycode==0x5) PRINTF("Timeout\n");
				if (replycode==0x6) PRINTF("AMSWire errors\n");
			}
		}
	} 
	return ret;
}
