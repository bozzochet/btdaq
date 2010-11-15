#include "Queue.h"
#include "Share.h"


#include <math.h> // for exp floor
#include <stdlib.h>  // for random RAND_MAX and rand
#include <time.h>

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <time.h>

#define Q_CTL
#define Q_LOG
#define  Q_GUI
#undef  Q_RUN
#undef  Q_DSP
#include "msg_define.code"

#define S_CONTROL
#undef  S_DATA
#undef  S_HISTO
#include "shm_define.code"


long mtype;
int par1;
int par2;
char msg[200];
int priority;

char timestamp[30];


void GetTimeStamp();

using namespace std;

int main(int argc, char **argv) 
{  
  char file[200];
  // Creating queue messages  
#include "msg_definition.code"

  // Creating share memory segments
#include "shm_definition.code"



  ofstream LogFile;

  strcpy(file,getenv("PWD"));
  strcat(file,"/Log/TDR.log");
  
  LogFile.open(file,ios::out);

  GetTimeStamp();
  LogFile << timestamp << " >>LOG: Initalizing processes...." << endl; 

  toLOG->Wait(kCTL,&mtype,&par1,&par2);
  if (par1 == kWakeUp) toCTL->Send(kLOG,kReady,0);

  while (control->On) {
    toLOG->Wait(0,&mtype,msg,&priority);
    GetTimeStamp();
    if (priority == -kWakeUp) toCTL->Send(kLOG,kReady,0);
    if (priority >= 0){
      switch (mtype) {
      case kCTL:
	LogFile << timestamp << " >>CTL: " << msg << endl; 
	if (strlen(control->LogMessage))
	  sprintf(control->LogMessage,"%s%s >>CTL: %s\n",control->LogMessage,timestamp,msg);
	else
	  sprintf(control->LogMessage,"%s >>CTL: %s\n",timestamp,msg);
	toGUI->Send(kCTL,kRefreshLog,0); // kCTl is ok...
	break;
      case kRUN:
	LogFile << timestamp << " >>RUN: " << msg << endl; 
	if (strlen(control->LogMessage))
	  sprintf(control->LogMessage,"%s%s >>RUN: %s\n",control->LogMessage,timestamp,msg);
	else
	sprintf(control->LogMessage,"%s >>RUN: %s\n",timestamp,msg);
	toGUI->Send(kCTL,kRefreshLog,0); // kCTl is ok...
	break;
      case kDSP:
	LogFile << timestamp << " >>DSP: " << msg << endl;
	if (strlen(control->LogMessage))
	  sprintf(control->LogMessage,"%s%s >>DSP: %s\n",control->LogMessage,timestamp,msg);
	else
	sprintf(control->LogMessage,"%s >>DSP: %s\n",timestamp,msg); 
	toGUI->Send(kCTL,kRefreshLog,0); // kCTl is ok...
	break;
      case kGUI:
	LogFile << timestamp << " >>GUI: " << msg << endl;
	if (strlen(control->LogMessage))
	  sprintf(control->LogMessage,"%s%s >>GUI: %s\n",control->LogMessage,timestamp,msg);
	else
	sprintf(control->LogMessage,"%s >>GUI: %s\n",timestamp,msg); 
	toGUI->Send(kCTL,kRefreshLog,0); // kCTl is ok...
	break;
      }
      

    }
  }

  LogFile.close();

#include "shm_detach.code"


}



void GetTimeStamp(){
time_t now;

now = time(NULL);
strftime(timestamp,26,"%a %d-%b-%Y %H:%M:%S:",localtime(&now));

}
