#include "amswcomtest.h"
#include "Queue.h"
#include "Share.h"
#include "Mclib.h"
#include "Time.h"
#include "TDRbits.h"
#include "EventStruct.h"
#include "gse.h"     // driver header


#include <math.h> // for exp floor
#include <stdlib.h>  // for random RAND_MAX and rand
#include <time.h>

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>


#include <sys/time.h> 
#include <sys/types.h> 
#include <sys/ioctl.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <getopt.h> 
#include <sys/stat.h>
//#include <linux/delay.h>
#include <sys/poll.h>



#define cS1   0x20          // Activate S1
#define cS2   0x40          // Activate S2
#define cK    0x80          // Activate K

#define cRESET       0x10   // reset hardware
#define cWRITEFIFO   0x     
#define cREADHCC     0x
#define cREADCAL     0x
#define cREADTEMP    0x
#define cREADSTATUS  0x


#define Q_CTL
#define Q_LOG
#undef  Q_GUI
#define Q_RUN
#define Q_DSP
#include "msg_define.code"

#define S_CONTROL
#define S_DATA
#define S_HISTO
#include "shm_define.code"

#define Ntdrs 24

typedef struct datades {
  int size[Ntdrs];
  int start[Ntdrs];
  int mode[Ntdrs]; // 1=raw, 2=red, 3=mix
  int error[Ntdrs];
} datades;


Time *RunTime;

float fSimCN[max_number_va];
float fSimPedestal[max_number_va][channel_per_va];
float fSimSigma[max_number_va][channel_per_va];
float fSimGain[max_number_va][channel_per_va];
int fSimStatus[max_number_va][channel_per_va];

long mtype;
int par1;
int par2;

void ProcessCTL(int par1, int par2);
void ProcessDSP(int par1, int par2);

int TakeData();
int GetEvent();
static int wait_irq (int fd, struct timeval *tv);
int ReadEvent();
void UpdateRunInfo();

bool ResetHW(int iReport);
void StartRun();
void StopRun();

void TakeRawCrate(int nevents);
void TakeRawCratePwg(int nevents);
void PrepareSimulation();
int GenerateEvent();

FILE *ifp=NULL; 
void PrepareBeamTestX5();
int ReadBeamTestX5();
void PrepareBinary();
int ReadBinary();
void PrepareAscii();
int ReadAscii();
void PrepareTDRCDP();
int ReadTDRCDP();
int ReadJINF();
int ReadTDR();
void GetDSPCalib(unsigned short tdr);
void ReadDSPCalib(unsigned short tdr);

void ReadTemp();
unsigned short calc_CRC(unsigned short shift_reg, unsigned short data);

void ReadSlaveMask(AMSWcom *node, unsigned short addr);
void ChangeReferenceTDR(int tdr);
void ChangeReferenceTDRPWG(int tdr);

void ExtractEvent(unsigned short *data, int datasize, datades* descr);
int PrintRXDONE(AMSWcom *node);
int CheckEvent(int evtnum, int tdr, datades descr);
int RunLog();

int fd;                    // file descriptor
int AnalogChain;

int FirstTime=1;

AMSWcom *Jinf=NULL;

int NEventsOld=0;

void bit_print(int v) {
  int i, mask = 1 << 31;
  for (i=1;i<=32;++i) {
    putchar(((v&mask)==0) ? '0' :'1');
    v<<=1;
    if (i%8 == 0 && i != 32)
      putchar(' ');
  }
}


int main(int argc, char **argv) 
{

  char file[200];

  // Creating queue messages  
#include "msg_definition.code"

  // Creating share memory segments
#include "shm_definition.code"


  //nice(-10);
  RunTime = new Time();

  // Opening port
  //fd = open("/dev/gse",O_RDWR);
  //if (fd == -1) { 
  //  perror ("/dev/gse"); 
  //  return fd; 
  //}; 
  int rto;
  //ioctl(fd,GSE_READTIMEOUT,&rto);    // clearing timeouts

  //JINFepp->Command_Lv1();
  //JINFepp->Command_Summary();


 
      //EppLogger *log=new EppLogger();
      

      


  while (control->On) {
    printf("run is waiting for a new signal\n");
    toRUN->Wait(0,&mtype,&par1,&par2);
    printf("run received a signal\n");
    switch (mtype) {
    case kCTL:
      ProcessCTL(par1,par2);
      break;
    case kDSP:
      ProcessDSP(par1,par2);
      break;
    }
  }

  printf("run is about to detach shm\n");
#include "shm_detach.code"
  printf("run says goodbye\n");
}

void ProcessCTL(int par1, int par2){  
  char aMessage[200];
  printf("welcome to processctl...\n");
  switch (par1) {

  case kWakeUp:
    toCTL->Send(kRUN,kReady,0);
    break;


  case kTDRExit:
    printf("run received an exit signal from control\n");
    toLOG->Send(kRUN,"Finisihing RUN ... Goodbye",1);
    printf("run send exit confirmation to log\n");
    break;
  }
}

void ProcessDSP(int par1, int par2){
  switch (par1) {

  case kResetHW:
    ResetHW(1);
    break;

  case kStartRun:
    StartRun();
    break;
  case kStopRun:
    StopRun();
    break;

  case kRefreshTime:
    control->RunTime = RunTime->GetElapsed();
    break;

  case kReadTemp:
    ReadTemp();
    break;

  case kReadDSPCalib:
    ReadDSPCalib(control->TDRDSPselect);
    break;

  }
}

void UpdateRunInfo(){
  if (control->RunStatus == kRUNSTATUS_RUN)RunTime->UpdateLive();
  //  printf("diff time=%d\n",RunTime->GetElapsed()-control->RunTime);
  control->RunTime     = RunTime->GetElapsed();
  control->EventRate   = RunTime->GetEventInstantRate(control->NEvents-NEventsOld);
  //control->LiveTime    = RunTime->GetLiveTime();
  control->InsLiveTime = RunTime->GetInsLiveTime();
  //printf("diff events=%d  %f\n",control->NEvents-NEventsOld, control->EventRate);
  NEventsOld=control->NEvents;

  switch(control->Source){   
  case kSOURCE_BEAMTESTX5:
    //    fclose(ifp); // close file
    break;
  }
}

int TakeData(){  
  char aMessage[200];
  //printf("welcome to takedata\n");
  int fReadOK = 0;
  toDSP->Send(kRUN,kRunStarted,0);
  if (control->Source==kSOURCE_JINF) Jinf->WriteDelay(control->JINFaddr,(unsigned short)((control->Shaping-0.4)/0.02));

  while (control->RunStatus == kRUNSTATUS_RUN && control->On){
    toRUN->Receive(&mtype,&par1,&par2);
    switch (mtype) {
    case kDSP: 
      break;
    case kCTL:
      if (par1==kSetReferenceTDR) {
	printf("we are in case setreftdr\n");
	if (control->ReadoutPWG) ChangeReferenceTDRPWG(control->TDRref);
	else ChangeReferenceTDR(control->TDRref);
	sprintf(aMessage,"Reference TDR set to %d",control->TDRref);
	toLOG->Send(kRUN,aMessage,1);
      }
      break;
    default:       	
      fReadOK = GetEvent();
      break;
    }
  }
  return fReadOK;
}

int GetEvent(){
  //printf("welcome to getevent\n");
  int fReadOK=0;
  int dif = data->fIn - data->fOut; 
  //  printf("fIn=%d, fOut=%d\n", data->fIn, data->fOut);
  if ( dif >=0 && dif <=8) {   // Stack has any slot free
    RunTime->UpdateDead();
    switch (control->Source) {
    case kSOURCE_SIMULATION:
      fReadOK=GenerateEvent();
      break;
    case kSOURCE_REALDATA:
      for (int mycnt=0; mycnt<300; mycnt++) {
	fReadOK=ReadEvent();
	if (fReadOK) break;
	if ((mycnt+1)%20==0) toLOG->Send(kRUN,"Waiting for event, be patient",1);
	printf("Event: %d, loop %d\n",control->NEvents,mycnt);
      }
      break;
    case kSOURCE_JINF:
      //printf("case jinf\n");
      fReadOK=ReadJINF();
      break;
    case kSOURCE_TDR:
      fReadOK=ReadTDR();
      break;
    case kSOURCE_BINARYFILE:
      fReadOK=ReadBinary();
      break;
    case kSOURCE_ASCIIFILE:
      fReadOK=ReadAscii();
      break;
    case kSOURCE_BEAMTESTX5:
      fReadOK=ReadBeamTestX5();
      break;
    case kSOURCE_TDRCDP:
      fReadOK=ReadTDRCDP();
      break;
    }
    if (fReadOK){
      data->fIn++;
      toDSP->Send(kRUN,kReadEvent,0);
    } else {
      if ((control->Trigger == kTRIGGER_EXTERNAL) || (control->Source==kSOURCE_JINF) || (control->Source==kSOURCE_TDR)){
	if ((RunTime->GetElapsed()-control->RunTime) > 1000) UpdateRunInfo();
      } else { 	
        if (control->Source == kSOURCE_REALDATA)
	  toDSP->Send(kRUN,kHardware_Problem,0);
	FirstTime=1;
	StopRun();
	usleep(200); // wait a bit to avoid read another event
      }
    }
    if ( ((control->NEvents-NEventsOld)>500) || 
      (RunTime->GetElapsed()-control->RunTime > 1000)) UpdateRunInfo();
  } else {
    if (RunTime->GetElapsed()-control->RunTime > 1000) UpdateRunInfo();
    RunTime->UpdateLive();
  }
  return fReadOK;
}


static int wait_irq (int fd, struct timeval *tv) 
{ 
  //pollfd pfd;
  //pfd.fd = fd;
  //pfd.events = POLLIN|POLLPRI;
  //return poll (&pfd, 1, -1);

  fd_set rfds; 
  FD_ZERO (&rfds); 
  FD_SET (fd, &rfds); 
  return select (fd + 1, &rfds, NULL, NULL, tv); 
} 

bool ResetHW(int iReport){
  unsigned char Data;
  unsigned int Mask1=0,Mask2=0, Mask3=0, Mask4=0;
  CalibMemory Prog_3f13, Prog_3f83;

  Prog_3f13.Ped=0x1300;
  Prog_3f13.MemPed=kDSP_PM;

  Prog_3f13.SigH=0x1b04;
  Prog_3f13.MemSigH=kDSP_PM;
  
  Prog_3f13.SigL=0x1b04;
  Prog_3f13.MemSigL=kDSP_DM;

  Prog_3f13.Stat=0x1702;
  Prog_3f13.MemStat=kDSP_PM;

  Prog_3f13.Sigr=0x1702;
  Prog_3f13.MemSigr=kDSP_DM;

  Prog_3f13.SigHFactor=8*19.0; //3.5; 
  Prog_3f13.SigLFactor=8*9.0; //2.0;

  Prog_3f13.SigrFactor=8*10.0; // precision*real_factor

  Prog_3f13.PedFactor=8.0; // precision


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


  Jinf->SetCalParMem(&Prog_3f83);  
  Jinf->SetTimeOut(4000);

  switch (control->Hardware) {
  case kHARDWARE_JINF_EPP:
  case kHARDWARE_JINF_PCI:
    // it might be possible, in the near future, that we just decide to change the hw/port in the same session. So I let following lines here   
    if (control->Hardware==kHARDWARE_JINF_EPP) {
      Jinf->SetHW(kAMSW_EPP);
      Jinf->SetPort(control->EPPport);
    }
    if (control->Hardware==kHARDWARE_JINF_PCI) {
      Jinf->SetHW(kAMSW_PCI);
      Jinf->SetPort(control->PCIport);
    }

    printf("hw=%d  port=%d\n",Jinf->GetHW(),Jinf->GetPort());

    toLOG->Send(kRUN,"Please wait... JINF/TDRs initialization",1);
    if (Jinf->Init()==ERROR) {
      printf("init error\n");
      char errmsg[200];
      sprintf(errmsg,"Initialization for %s port %d: error, check your hardware\n",(control->Hardware==kHARDWARE_JINF_PCI)?"PCI":"EPP",Jinf->GetPort());
      toLOG->Send(kRUN,errmsg,1);
      return ERROR;
    }

    if (control->Source==kSOURCE_JINF) {
      if (control->JINFaddr!=0xffff) {
	toLOG->Send(kRUN,"Booting JINJ...",1);
	Jinf->Boot(0xffff,control->JINJprog);
	sleep(2);
	Jinf->SetMode(0xffff,0); // setting Jinj in no daq mode
      }
      toLOG->Send(kRUN,"Booting JINF...",1);
      Jinf->Boot(control->JINFaddr, control->JINFprog);
      sleep(4);
      Jinf->SetSSF(control->JINFaddr,0); // we switch off jinf brother
      Jinf->SetMode(control->JINFaddr,0); // no acquisition, but it seems triggers still disturb the system
      Jinf->SlaveTest(control->JINFaddr);
      sleep(3);
      toLOG->Send(kRUN,"Booting TDRs...",1);
      unsigned int groupaddr=(control->JINFaddr==0xffff)?0x4000:((0x4000<<16)+control->JINFaddr);
      Jinf->Boot(groupaddr, control->TDRprog);
      sleep(4);
      ReadSlaveMask(Jinf, control->JINFaddr);
      sleep(1);
       Jinf->SetMode(groupaddr,0);
      sleep(2);
      Jinf->SetMode(control->JINFaddr,1); // say to Jinf we want to take data
    } else if (control->Source==kSOURCE_TDR) {
      char mess[100];
      sprintf(mess,"Booting TDR (address 0x%04x) with program 0x%04x", control->JINFaddr, control->TDRprog);
      toLOG->Send(kRUN,mess,1);
      Jinf->Boot(control->JINFaddr, control->TDRprog);
      sleep(5);
      sprintf(mess,"rxdone=%04x",Jinf->GetRX_DONE());
      toLOG->Send(kRUN,mess,1);
      Jinf->SetMode(control->JINFaddr,1);
    }

    toLOG->Send(kRUN,"JINF/TDRs initialization done",1);
    
    //GetDSPCalib(0);
    break;

  case kHARDWARE_TESTBOX_EPP:
    Data=cRESET; 
    ioctl(fd,GSE_W_EPP,&Data);
    break;
  }
  if (iReport == 1) toLOG->Send(kRUN,"Hardware reset sent",0);
  return SUCCESS;
}


void ChangeReferenceTDR(int tdr) {
  //printf("we are in changereftdr\n");
  unsigned int Mask0=1<<tdr;
  unsigned int Mask0inv=~Mask0; // for busy, all are ignored but TDR4
  unsigned int groupaddr=0x2a00;
  unsigned int tdraddr=0x3f+(tdr<<8);
  if (control->JINFaddr!=0xffff) {
    groupaddr=(groupaddr<<16)+control->JINFaddr;
    tdraddr=(tdraddr<<16)+control->JINFaddr;
  }
  

  Jinf->SetMode(control->JINFaddr,0);  
  Jinf->SetMode(groupaddr,0); 
  Jinf->EventReset(control->JINFaddr);
  Jinf->EventReset(groupaddr);
  Jinf->WriteSlaveMask(control->JINFaddr,0,Mask0);
  Jinf->WriteBusyMask(control->JINFaddr, Mask0inv);
  Jinf->SetMode(control->JINFaddr,1);
  Jinf->SetMode(tdraddr,1); // mixed data

}


void ChangeReferenceTDRPWG(int tdr) {
  //printf("we are in changereftdrpwg\n");

  char msg[100];
  sprintf(msg,"Changing reference tdr to %d",tdr);
  toLOG->Send(kRUN,msg,1);

  // here the mask includes the tdr and its powergroup.
  const int pwglist[24]={0,0,3,3,0,0,3,3,0,0,3,3,1,1,2,2,1,1,2,2,1,1,2,2};
  unsigned int Mask[4]={ 0x000333, 0x333000, 0xccc000, 0x000ccc };

  // now we have to adapt wrt to real connections on CDDC
  for (int i=0; i<4; i++) Mask[i]&=(control->TDRmask);

  unsigned int Mask0=Mask[pwglist[tdr]];
  unsigned int Mask0inv=~Mask0; // for busy, all are ignored but TDR4

  unsigned int groupaddr=0x4000;
  unsigned int tdraddr=0x3f+(tdr<<8);
  if (control->JINFaddr!=0xffff) {
    groupaddr=(groupaddr<<16)+control->JINFaddr;
    tdraddr=(tdraddr<<16)+control->JINFaddr;
  }

  Jinf->WriteSlaveMask(control->JINFaddr,0,control->TDRmask);
  Jinf->WriteBusyMask(control->JINFaddr,0,control->TDRmask);

  Jinf->SetMode(groupaddr,0); // all connected slaves
  Jinf->EventReset(groupaddr); // all connected slaves
  Jinf->SetMode(control->JINFaddr,0);  
  Jinf->EventReset(control->JINFaddr);
  Jinf->SetMode(groupaddr,2); 
  Jinf->SetMode(tdraddr,1); // the examined TDR is set to mixed mode
  Jinf->WriteSlaveMask(control->JINFaddr,0,Mask0);
  Jinf->WriteBusyMask(control->JINFaddr, Mask0inv);
  Jinf->SetMode(control->JINFaddr,1);
  //sleep(1);
}


void GetDSPCalib(unsigned short tdr) {

  char msg[100];

  unsigned int groupaddr=0x4000;
  if (control->JINFaddr!=0xffff) groupaddr=(groupaddr<<16)+control->JINFaddr;

  unsigned int mask=control->TDRmask;
  Jinf->WriteSlaveMask(control->JINFaddr,0,mask);
  Jinf->WriteBusyMask(control->JINFaddr,0);
  Jinf->Calibrate(groupaddr,0);
  sleep(20);

  //  ReadDSPCalib(tdr);
  toDSP->Send(kRUN,kDSPCalibDone,0);
}


void ReadDSPCalib(unsigned short tdr) {

  char msg[100];


  if ((control->TDRmask & (1<<tdr)) == (1<<tdr)) {

    unsigned int addr=(tdr<<8)+0x3f;
    if (control->JINFaddr!=0xffff) addr=(addr<<16)+control->JINFaddr;
    sprintf(msg,"Getting DSP calibration data for TDR %d",tdr);
    toLOG->Send(kRUN,msg,1);
    Jinf->GetCalibration(addr);
    
    for (int i=0; i<1024; i++) {
      //printf("%04d  %4.2lf  %4.2lf  %4.2lf  %10d\n",i,Jinf->ped[i],Jinf->sigr[i],Jinf->sig[i],Jinf->sta[i]);
      histo->fhDSPPedestal[i+1]=Jinf->ped[i];
      histo->fhDSPSigmaRaw[i+1]=Jinf->sigr[i];
      histo->fhDSPSigma[i+1]=Jinf->sig[i];
      histo->fhDSPStatus[i+1]=Jinf->sta[i];
    }
  } else {
    sprintf(msg,"TDR %d is not connected",tdr);
    toLOG->Send(kRUN,msg,1);
  }
  
  toDSP->Send(kRUN,kReadDSPCalib,0);
}


int ReadJINF() {

  //printf("welcome to readjinf\n");
  int ok=0;
  int h=0, va=0, vach=0,size=0;
  int islot = data->fIn%max_buf;
  unsigned short tdrevtlength=0,tdrnum=0, jinfevtnum=0;
  memset(data->Data[islot],0,sizeof(data->Data[islot]));

  switch (control->DataMode){
  case kDATAMODE_RAW:
  case kDATAMODE_FFT:
  case kDATAMODE_CALIBRATION:
  case kDATAMODE_REDUCTION:
  case kDATAMODE_COSMIC:
  case kDATAMODE_SOURCE:
    // here we could set the shaping time (but I think it should be set somewhere else)
    // Jinf->Command_SetShapingTime()
    if (control->Trigger == kTRIGGER_RANDOM) {
      //printf("lv1 will be sent...\n");
      Jinf->Command_Lv1();
    }
    
  case kDATAMODE_GAIN:
    // as of now, no gain calibration available
    break;

  case kDATAMODE_SCOPE:
    // I suspect it is the readchannel mode, not used with JINF
    break;
  }

  //printf("before jinf get event\n");
  Jinf->GetEvent(control->JINFaddr);
  if (control->Hardware==kHARDWARE_JINF_PCI) {
    if (Jinf->GetRX_DONE()!=0) { 
      char mess[100];
      sprintf(mess,"pci rxdone=%d",(short)Jinf->GetRX_DONE());
      toLOG->Send(kRUN,mess,1);
      StopRun();
      return 0;
    }
  }
  else 
    if ((Jinf->GetRX_DONE()!=0x4180) && (Jinf->GetRX_DONE()!=0x4100)) {
      char mess[100];
      sprintf(mess,"epp rxdone=%04x",Jinf->GetRX_DONE());
      toLOG->Send(kRUN,mess,1);
      StopRun();
      return 0;
    }
  //printf("after jinf get event, size=%d\n",size);
  // now we need to decode this event
  // for now, we suppose only one is raw, or at least, we look at only one raw.

  size=Jinf->GetEventSize();
  //printf("event size=%d\n",size);

  jinfevtnum=Jinf->Event[h++];
  while (h<size-2) {
    tdrevtlength=Jinf->Event[h++];
    tdrnum=0x1f & Jinf->Event[h+tdrevtlength-1];
    //printf("tdrnum=%2d tdrevtlength = 0x%04x\n",tdrnum,tdrevtlength);
    //printf("tdrnum=%d\n",tdrnum);
    if (tdrnum==control->TDRref && tdrevtlength>=0x0401) { // with a length of 0x401, it is most probably a raw event... if there is more, we have red data, which we do not care about now
      //printf("dump:\n");
      //for (int i=0;i<1024;i++) {
      //printf("%04x ",Jinf->event[h+i]);
      //if (i%10==0) printf("\n");
      //}
      //printf("end of dump\n");
      for(int ch=0,cha=0; ch<320;ch++) {
	//printf("ch=%d\n",ch);
	cha=h+3*ch;
	va=ch/channel_per_va;
	vach=ch%channel_per_va;
	data->Data[islot][va][vach]=Jinf->Event[cha]>>3;
	data->Data[islot][va+5][vach]=Jinf->Event[cha+1]>>3;
	data->Data[islot][va+10][vach]=Jinf->Event[cha+2]>>3;

      }
      for(int ch=0; ch<channel_per_va;ch++) {
	data->Data[islot][15][ch]=Jinf->Event[h+ch+960]>>3;
      }
      if (!(Jinf->Event[h+63+960] & 1)) control->NBadPwrS++;
      if (!(Jinf->Event[h+63+960] & 2)) control->NBadPwrK++;

      ok=1;
    }
    h+=tdrevtlength;
  }

  

  return ok;
}



int ReadTDR() {

  //printf("welcome to readjinf\n");
  int ok=0;
  int h=0, va=0, vach=0,size=0;
  int islot = data->fIn%max_buf;
  unsigned short tdrevtlength=0,tdrnum=0, jinfevtnum=0;
  memset(data->Data[islot],0,sizeof(data->Data[islot]));

  switch (control->DataMode){
  case kDATAMODE_RAW:
  case kDATAMODE_FFT:
  case kDATAMODE_CALIBRATION:
  case kDATAMODE_REDUCTION:
  case kDATAMODE_COSMIC:
  case kDATAMODE_SOURCE:
    // here we could set the shaping time (but I think it should be set somewhere else)
    // Jinf->Command_SetShapingTime()
    if (control->Trigger == kTRIGGER_RANDOM) {
      //printf("lv1 will be sent...\n");
      Jinf->Command_Lv1();
    }
    
  case kDATAMODE_GAIN:
    // as of now, no gain calibration available
    break;

  case kDATAMODE_SCOPE:
    // I suspect it is the readchannel mode, not used with JINF
    break;
  }

  //printf("before jinf get event\n");
  Jinf->GetEvent(control->JINFaddr);
  if (control->Hardware==kHARDWARE_JINF_PCI) {
    if (Jinf->GetRX_DONE()!=0) { 
      char mess[100];
      sprintf(mess,"pci rxdone=%d",(short)Jinf->GetRX_DONE());
      toLOG->Send(kRUN,mess,1);
      StopRun();
      return 0;
    }
  }
  else 
    if ((Jinf->GetRX_DONE()!=0x4180) && (Jinf->GetRX_DONE()!=0x4100)) {
      char mess[100];
      sprintf(mess,"epp rxdone=%04x",Jinf->GetRX_DONE());
      toLOG->Send(kRUN,mess,1);
      StopRun();
      return 0;
    }
  //printf("after jinf get event, size=%d\n",size);
  // now we need to decode this event
  // for now, we suppose only one is raw, or at least, we look at only one raw.

  size=Jinf->GetEventSize();
  //printf("event size=%d\n",size);

  jinfevtnum=Jinf->Event[h++];
  tdrevtlength=size;

  //  printf("%d %d\n",jinfevtnum, tdrevtlength);

  if (tdrevtlength==0x0402) { // with a length of 0x401, it is most probably a raw event...
    //printf("dump:\n");
    //for (int i=0;i<1024;i++) {
    //printf("%04x ",Jinf->event[h+i]);
    //if (i%10==0) printf("\n");
    //}
    //printf("end of dump\n");
    for(int ch=0,cha=0; ch<320;ch++) {
      //printf("ch=%d\n",ch);
      cha=h+3*ch;
      va=ch/channel_per_va;
      vach=ch%channel_per_va;
      data->Data[islot][va][vach]=Jinf->Event[cha]>>3;
      data->Data[islot][va+5][vach]=Jinf->Event[cha+1]>>3;
      data->Data[islot][va+10][vach]=Jinf->Event[cha+2]>>3;
      
    }
    for(int ch=0; ch<channel_per_va;ch++) {
      data->Data[islot][15][ch]=Jinf->Event[h+ch+960]>>3;

    }
    if (!(Jinf->Event[h+63+960] & 1)) control->NBadPwrS++;
    if (!(Jinf->Event[h+63+960] & 2)) control->NBadPwrK++;
    
    ok=1;
  }
  


  

  return ok;
}



int ReadEvent(){
  int idecal = 0; // =6 before changes
  int fReadOK_S1 = 2;
  int fReadOK_S2 = 2;
  int fReadOK_K  = 2;
  int retval = 0;
  struct timeval tv,tvext;
  unsigned int adc[EVENTMAX];
  unsigned char Data;
  int islot = data->fIn%max_buf;

  memset(data->Data[islot],0,sizeof(data->Data[islot]));

  tv.tv_sec=0;  // was 1 
  tv.tv_usec=50000;  // was 0

  tvext.tv_sec=1; 
  tvext.tv_usec=0;

  // ResetHW(0); // Reset hardware

  ioctl(fd,GSE_CLRIRQ,&retval);               // Clear IRQ line
  
  switch (control->DataMode){
  case kDATAMODE_RAW:
  case kDATAMODE_FFT:
  case kDATAMODE_CALIBRATION:
  case kDATAMODE_REDUCTION:
  case kDATAMODE_COSMIC:
  case kDATAMODE_SOURCE:
      Data = AnalogChain | 0x06;             // read_hcc int  
      if (control->Trigger == kTRIGGER_EXTERNAL) Data |= 0x01; 
      ioctl(fd,GSE_W_EPP,&Data);
      if (control->Trigger == kTRIGGER_EXTERNAL) {
      	usleep(1);
      	Data=(int) control->Shaping*10; // Data = 0x32; //Delay 5us .....
      	ioctl(fd,GSE_W_EPP,&Data);
      }
    break;
    
  case kDATAMODE_GAIN:
    idecal=0;
    Data= AnalogChain | 0x0a;                 // read_dac 
    ioctl(fd,GSE_W_EPP,&Data);
    //usleep(10000);
    //usleep(1);
    Data=~control->DAC[control->GainIndex];    // dac_value
    ioctl(fd,GSE_W_EPP,&Data);
    //usleep(100000);
    //usleep(1);
    break;

  case kDATAMODE_SCOPE:
    Data=AnalogChain | 0x03 ;              // read ChannelX
    ioctl(fd,GSE_W_EPP,&Data); 
    usleep(1);

    Data=~control->DACValue;               // DACvalue
    ioctl(fd,GSE_W_EPP,&Data);
    usleep(1); 

    Data=control->DACpos/255 & 31;    // high bit 
    ioctl(fd,GSE_W_EPP,&Data);
    usleep(1);

    Data=control->DACpos & 255;         // low bit 
    ioctl(fd,GSE_W_EPP,&Data);
    usleep(1);
    break;

  }

//    if (control->DataMode == kDATAMODE_GAIN) {
//      Data= AnalogChain | 0x0a;                 // read_dac 
//      ioctl(fd,GSE_W_EPP,&Data);
//      usleep(1);

//      Data=control->DAC[control->GainIndex];    // dac_value
//      ioctl(fd,GSE_W_EPP,&Data);
//      //sleep(1);
//    } else {
//      Data = AnalogChain | 0x06;                 // read_hcc int  
//      if (control->Trigger == kTRIGGER_EXTERNAL) Data |= 0x01;
//      ioctl(fd,GSE_W_EPP,&Data);
//      //usleep(1);
//    }


  int imax;
  if (control->ActiveS1==1) {
    //Data = cS1 | 0x02;    // read_fifo      
    //ioctl(fd,GSE_W_EPP,&Data);
    if (control->Trigger == kTRIGGER_EXTERNAL) {
      fReadOK_S1=wait_irq(fd,&tvext);
    } else {
      fReadOK_S1=wait_irq(fd,&tv); 
    }  

    if (fReadOK_S1) {
      switch (control->DataMode) {
	case kDATAMODE_SCOPE:
	  imax=90;
	  ioctl(fd,GSE_READCHANNEL,adc);
	  break;
	case kDATAMODE_GAIN:
	  imax=320;  
	  ioctl(fd,GSE_READGAIN,adc);
	  break;
	default:
	  imax=320;  
	  ioctl(fd,GSE_READEVENT,adc);
	}

      for (int i=0;i<imax;i++){
	int va=i/channel_per_va;
	int ch=i%channel_per_va;      
	data->Data[islot][va][ch]=(unsigned short) adc[i+idecal];
      }
    }
  }
  //sleep(1);

  if (control->ActiveS2 == 1) {
    //Data = cS2 | 0x02;    // read_fifo      0x8-:k 0x4-:s2 0x-2:s1  0xe-:all
    //ioctl(fd,GSE_W_EPP,&Data);
    if (control->Trigger == kTRIGGER_EXTERNAL) {
      if (control->ActiveS1 == 0 || fReadOK_S1 == 1) {
	fReadOK_S2=wait_irq(fd,&tvext);}
      else {fReadOK_S2=0;}   
    } else {
      fReadOK_S2=wait_irq(fd,&tv); 
    }  
    if (fReadOK_S2) { 
      switch (control->DataMode) {
	case kDATAMODE_SCOPE:
	  imax=90;
	  ioctl(fd,GSE_READCHANNEL,adc);
	  break;
	case kDATAMODE_GAIN:
	  imax=320;  
	  ioctl(fd,GSE_READGAIN,adc);
	  break;
	default:
	  imax=320;  
	  ioctl(fd,GSE_READEVENT,adc);
	}

      for (int i=0;i<imax;i++){
	int va=5+i/channel_per_va;
	int ch=i%channel_per_va;      
	data->Data[islot][va][ch]=(unsigned short) adc[i+idecal];
      }
    }
  }

  if (control->ActiveK == 1) {
    //Data = cK | 0x02;    // read_fifo      0x8-:k 0x4-:s2 0x-2:s1  0xe-:all
    //ioctl(fd,GSE_W_EPP,&Data);
    if (control->Trigger == kTRIGGER_EXTERNAL) {
      if ((control->ActiveS1==0 && control->ActiveS2==0) ||
	  fReadOK_S1 == 1 || fReadOK_S2 == 1) {
	fReadOK_K=wait_irq(fd,&tvext);}
      else {fReadOK_K=0;}
    } else {
      fReadOK_K=wait_irq(fd,&tv); 
    }  
    if (fReadOK_K) { 
      switch (control->DataMode) {
	case kDATAMODE_SCOPE:
	  imax=90;
	  ioctl(fd,GSE_READCHANNEL,adc);
	  break;
	case kDATAMODE_GAIN:
	  imax=384;  
	  ioctl(fd,GSE_READGAIN,adc);
	  break;
	default:
	  imax=384;  
	  ioctl(fd,GSE_READEVENT,adc);
	}

      for (int i=0;i<imax;i++){
	int va=10+i/channel_per_va;
	int ch=i%channel_per_va;      
	data->Data[islot][va][ch]=(unsigned short) adc[i+idecal];
      }
      //      if (control->DataMode == kDATAMODE_GAIN) 
      //	printf("DAC=%03d  data=%05d\n",
      //	       control->DAC[control->GainIndex], data->Data[islot][11][20]);
      
    }
  }
  
//  for (int i=0; i<384; i++){
//      int va=i/channel_per_va;
//      int ch=i%channel_per_va;      
//     printf("%8d  S1: %5d   S2: %5d    K: %5d\n",i,
//	     data->Data[islot][va][ch],
//	     data->Data[islot][va+5][ch],
//	     data->Data[islot][va+10][ch]);
//  }

  return fReadOK_S1*fReadOK_S2*fReadOK_K;
}

//void ReadEvent(){
//  GenerateEvent();
//  fReadOK=1;
//}

int GenerateEvent(){  
  float cn[max_number_va];
  int islot = data->fIn%max_buf;

  int fReadOK;
  fReadOK=1;
  // Generate Common Noise + Pedestal

  memset(data->Data[islot],0,sizeof(data->Data[islot]));
  for (int va=0; va<max_number_va; va++){
    if ( (control->ActiveS1 && va<5) ||
	 (control->ActiveS2 && va>=5 && va<10) ||
	 (control->ActiveK  && va>=10)){
      cn[va]=Gaussian(0.0,fSimCN[va]);    // Common Noise
      for (int ch=0; ch<channel_per_va; ch++){
	data->Data[islot][va][ch]= (unsigned short) 
	  (fSimPedestal[va][ch] + Gaussian(0.0,fSimSigma[va][ch])+cn[va]);
      }
    }
  }

  if (control->DataMode == kDATAMODE_GAIN) {
    for (int va=0; va<max_number_va; va++)
      if ( (control->ActiveS1 && va<5) ||
	   (control->ActiveS2 && va>=5 && va<10) ||
	   (control->ActiveK  && va>=10)){
	for (int ch=0; ch<channel_per_va; ch++){
	  data->Data[islot][va][ch]+= 
	      (unsigned short) control->DAC[control->GainIndex]*10;
	}    
      }
  }

  if (control->DataMode == kDATAMODE_RAW ||
      control->DataMode == kDATAMODE_FFT || 
      control->DataMode == kDATAMODE_REDUCTION) {
  // Generate Signal

    int maxch,len,lmin,lmax,ldif;
    float hit,tot,sum[100],signal;    
    maxch = max_number_va*channel_per_va;
    // hit=(float)rand()/RAND_MAX*maxch; 
    hit = 500.0;
    len=Poisson(0.8)+1;
    
    lmin = (int)(hit-(float)len/2.0+0.5);
    lmax = (int)(hit+(float)len/2.0+0.5);
    ldif=lmax-lmin+1;
    tot=0;
    for (int i=0;i<ldif+1;i++){
      sum[i]=exp(-pow((float)(lmin+i-hit)/len/3.0,2.0));
      tot+=sum[i];  
    }
    for (int i=0;i<ldif+1;i++) sum[i]/=tot;
    
    signal=Landau(50.0,5.0);
    
    for (int i=lmin;i<lmax+1;i++){
      int va=i/channel_per_va;
      int ch=i%channel_per_va;
      if ( (control->ActiveS1 && va<5) ||
	   (control->ActiveS2 && va>=5 && va<10) ||
	   (control->ActiveK  && va>=10)){
	data->Data[islot][va][ch]+=(unsigned short)(sum[i-lmin]*signal);
      }
    }



    hit = 850.0;
    len=Poisson(0.8)+1;
    
    lmin = (int)(hit-(float)len/2.0+0.5);
    lmax = (int)(hit+(float)len/2.0+0.5);
    ldif=lmax-lmin+1;
    tot=0;
    for (int i=0;i<ldif+1;i++){
      sum[i]=exp(-pow((float)(lmin+i-hit)/len/3.0,2.0));
      tot+=sum[i];  
    }
    for (int i=0;i<ldif+1;i++) sum[i]/=tot;
    
    signal=Landau(50.0,5.0);
    
    for (int i=lmin;i<lmax+1;i++){
      int va=i/channel_per_va;
      int ch=i%channel_per_va;
      if ( (control->ActiveS1 && va<5) ||
	   (control->ActiveS2 && va>=5 && va<10) ||
	   (control->ActiveK  && va>=10)){
	data->Data[islot][va][ch]+=(unsigned short)(sum[i-lmin]*signal);
      }
    }

  }
  return fReadOK;
}


void PrepareSimulation(){
  ifstream inFile;
  inFile.open("dummy.cal",ios::in);
  int kk1,kk2,kk3;
  inFile >> kk1 >> kk2 >> kk3;
  for (int va=0;va<max_number_va;va++)
    inFile >> fSimCN[va];

  for (int va=0;va<max_number_va;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      inFile >> kk1 >> fSimPedestal[va][ch] >> fSimSigma[va][ch] 
	            >> fSimGain[va][ch]     >> fSimStatus[va][ch];
  inFile.close();

  srand(time(NULL));
}


void PrepareBeamTestX5(){
  struct EventX5Header *actual;
  actual = new EventX5Header;

  if (ifp != NULL) fclose(ifp);
  ifp = fopen(control->fFileName,"rb");
  fread(actual,sizeof(EventX5Header),1,ifp);  
}

int ReadBeamTestX5(){
  int fReadOK = 0;
  int islot = data->fIn%max_buf;
  int ich;
  struct EventX5 *actual;
  actual = new EventX5;


  if (fread(actual,sizeof(EventX5),1,ifp) !=0){ 
    fReadOK=0;
    switch (control->DataMode) {
    case kDATAMODE_RAW:
    case kDATAMODE_FFT:
      fReadOK=1;
      break;
    case kDATAMODE_CALIBRATION:
      if (READBIT(actual->EvtNumber,31) == 0) fReadOK=1;
      break;
    case kDATAMODE_REDUCTION:
      if (READBIT(actual->EvtNumber,31) != 0) fReadOK=1;
      break;
    }
      
    if (fReadOK) {
    ich=0; 
      for (int va=0; va<5; va++)
	for (int ch=0; ch<channel_per_va; ch++){
	  data->Data[islot][va][ch]=(unsigned short) actual->S_11[ich];
	  ich++;
	}  
      ich=0; 
      for (int va=5; va<10; va++)
	for (int ch=0; ch<channel_per_va; ch++){
	  data->Data[islot][va][ch]=(unsigned short) actual->S_22[ich];
	  ich++;
	}  
 
      ich=0;
      for (int va=10; va<16; va++)
	for (int ch=0; ch<channel_per_va; ch++){
	  data->Data[islot][va][ch]=(unsigned short) actual->K_2[ich];
	  ich++;	
	}  
    }
  } else {
    fReadOK=0;  // Stop Adquisition      
    StopRun(); 
  }
  delete actual;
  return fReadOK;
}

void PrepareTDRCDP(){
  char aMessage[200];
  if (ifp != NULL) fclose(ifp);
  ifp = fopen(control->fFileName,"rb");
  sprintf(aMessage,"Opening TDR CDP file: %s",control->fFileName);
  toLOG->Send(kRUN,aMessage,1);
}


int ReadTDRCDP(){
  int fReadOK=0;
  int islot = data->fIn%max_buf;
  char header0[200],header1[200];
  int ncols;

  //ncols=fscanf(ifp,"%s %s",header0,header1);
  //if (ncols < 0) return fReadOK;


  if (fread(data->Data[islot],sizeof(data->Data[islot]),1,ifp) != 0){
    fReadOK=1;
  } else {
    fReadOK=0;
    //StopRun();
  }


  //for (int va=0; va<max_number_va; va++)
  //  for (int ch=0; ch<channel_per_va; ch++)
  //    if (fscanf(ifp,"%d",&data->Data[islot][va][ch]) != EOF) {
  //  	fReadOK=1;
  //    } else {
  //  	fReadOK=0;
  //  	//StopRun();
  //    }
  return fReadOK;
}

void PrepareBinary(){
  char aMessage[200];
  if (ifp != NULL) fclose(ifp);
  ifp = fopen(control->fFileName,"rb");
  sprintf(aMessage,"Opening binary file: %s",control->fFileName);
  toLOG->Send(kRUN,aMessage,1);
}
int ReadBinary(){
  int fReadOK=0;
  int islot = data->fIn%max_buf;
  if (fread(data->Data[islot],sizeof(data->Data[islot]),1,ifp) != 0){
    fReadOK=1;
  } else {
    fReadOK=0;
    //StopRun();
  }
  return fReadOK;
}

void PrepareAscii(){
  char aMessage[200];
  if (ifp != NULL) fclose(ifp);
  ifp = fopen(control->fFileName,"r");
  sprintf(aMessage,"Opening ascii file: %s",control->fFileName);
  toLOG->Send(kRUN,aMessage,1);
}
int ReadAscii(){
  int fReadOK=0;
  int islot = data->fIn%max_buf;
  for (int va=0; va<max_number_va; va++)
    for (int ch=0; ch<channel_per_va; ch++)
      if (fscanf(ifp,"%d",&data->Data[islot][va][ch]) != EOF) {
	fReadOK=1;
      } else {
	fReadOK=0;
	//StopRun();
      }
  return fReadOK;
}


void StartRun(){
  char aMessage[200];

  data->fIn=0;
  data->fOut=0;

  AnalogChain = 0x00;
  if (control->ActiveS1) AnalogChain |= cS1; 
  if (control->ActiveS2) AnalogChain |= cS2; 
  if (control->ActiveK)  AnalogChain |= cK;

  NEventsOld=0;

  switch (control->Source) {
  case kSOURCE_REALDATA:
    //if (FirstTime || (control->DataMode==kDATAMODE_GAIN)) {
    if (FirstTime) {
      toLOG->Send(kRUN,"Please wait... initialization",1);
      sleep(2);
      ResetHW(0);
      FirstTime=0;
    } else usleep(100000);
    break;    
  case kSOURCE_SIMULATION:
    PrepareSimulation();
    break;
  case kSOURCE_BINARYFILE:
    PrepareBinary();
    break;
  case kSOURCE_ASCIIFILE:
    PrepareAscii();
    break;
  case kSOURCE_BEAMTESTX5:
    PrepareBeamTestX5();
    break;
  case kSOURCE_TDRCDP:
    PrepareTDRCDP();
    break;
  case kSOURCE_JINF:
  case kSOURCE_TDR:
    if (FirstTime) {
      sprintf(aMessage,"creating the AMSW object...");
      toLOG->Send(kRUN,aMessage,1);
      Jinf=new AMSWcom(); // default, port=0, hw=EPP
      //printf("object created\n");
      toLOG->Send(kRUN,"Jinf object created",1);
      amsw_close(-1); // just a test
      if (ResetHW(1)==ERROR) {
	toDSP->Send(kRUN,kRunNotStarted,0);
	return;
      }
      FirstTime=0;
    }
    //    Jinf->EventReset(0x4000);
    //    Jinf->EventReset(0xffff);
    break;
  }
  RunTime->Start();
  RunTime->RefLive();
  toDSP->Send(kRUN,kRunStarted,0);
  UpdateRunInfo();
  if (control->Source==kSOURCE_JINF && control->DataMode==kDSPCALIBRATION) { 
    GetDSPCalib(control->TDRref);
    
    //StopRun();
  } else if (control->Source==kSOURCE_JINF && control->DataMode==kOCTANTCALIBRATION) TakeRawCratePwg(1100); 
  else TakeData();
}

void StopRun(){
  char aMessage[200];
  RunTime->UpdateLive();
  UpdateRunInfo();
  switch (control->Source) {
  case kSOURCE_BINARYFILE:
    fclose(ifp); ifp = NULL;
    sprintf(aMessage,"Closing binary file: %s",control->fFileName);
    toLOG->Send(kRUN,aMessage,1);
    break;
  case kSOURCE_ASCIIFILE:
    fclose(ifp); ifp = NULL;
    sprintf(aMessage,"Closing ascii file: %s",control->fFileName);
    toLOG->Send(kRUN,aMessage,1);
    break;
  case kSOURCE_BEAMTESTX5:
    fclose(ifp); ifp = NULL;
    break;
  case kSOURCE_TDRCDP:
    fclose(ifp); ifp = NULL;
    sprintf(aMessage,"Closing TDR CDP file: %s",control->fFileName);
    toLOG->Send(kRUN,aMessage,1);
    break;
  }
  toDSP->Send(kRUN,kRunStoped,0);
}

void ReadTemp(){
  unsigned short temp[TEMPDEPTH];
  unsigned int Data;
  unsigned short CRCrom,CRCtemp;
  int retval = 0;
  int fReadOK=0;
  struct timeval tv;

  char ROM[100];  

  toLOG->Send(kRUN,"Reading Temperature",1);

  //memcpy(temp,0,sizeof(temp));
  
  switch (control->Source) {
    case 1000000: // such a value is normally not expected, thus following lines are deactivated...
    //case kSOURCE_REALDATA:
      printf("uh uh... this is not expected... do something !\n");
      tv.tv_sec = 3;
      tv.tv_usec = 0;

      ResetHW(0); // Reset hardware

      ioctl(fd,GSE_CLRIRQ,&retval);               // Clear IRQ line
 
      Data = 0x11;
      ioctl(fd,GSE_W_EPP,&Data);

//      fReadOK = wait_irq(fd,NULL);

 
      usleep(2000000);
      fReadOK=1;
      if (fReadOK) {
	ioctl(fd,GSE_READTEMP,temp);
	sprintf(control->TempId,"%02x%02x%02x%02x%02x%02x%02x",
		temp[6],temp[5],temp[4],temp[3],temp[2],temp[1],temp[0]);
	CRCrom=0;
	for (int i=0;i<7;i++) CRCrom=calc_CRC(CRCrom,temp[i]);

	CRCtemp=0;
	for (int i=9;i<17;i++) CRCtemp=calc_CRC(CRCtemp,temp[i]);
	control->HybridTemp = (float) temp[9]/2.0;

      	//sprintf(ROM,"ROM: %02x %02x %02x %02x %02x %02x %02x %02x",
	//	temp[7],temp[6],temp[5],temp[4],
	//	temp[3],temp[2],temp[1],temp[0]);
	//toLOG->Send(kRUN,ROM,1);
	//sprintf(ROM,"CRCrom: %02x",CRCrom);
	//toLOG->Send(kRUN,ROM,1);
	//sprintf(ROM,"MEM: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
	//   	temp[18],temp[17],temp[16],temp[15],temp[14],temp[13],temp[12],
	//    	temp[11],temp[10],temp[9]);
	//toLOG->Send(kRUN,ROM,1);
	//sprintf(ROM,"CRCtemp: %02x",CRCtemp);
	//toLOG->Send(kRUN,ROM,1);
	//sprintf(ROM,"Temp: %4.1f",control->HybridTemp);
	//toLOG->Send(kRUN,ROM,1);

	sprintf(ROM,"Finished Temp = %4.1f  ROM: %s",
		control->HybridTemp,control->TempId);
	toLOG->Send(kRUN,ROM,1);
	toDSP->Send(kRUN,kShowTemp,0);
      }
      break;

    default:
      control->HybridTemp=-3.0;
      sprintf(control->TempId,"aaaaaaaaaaaaaaaa");
      sprintf(ROM,"Temperature sensor can be readed only in Real Data mode");
      toLOG->Send(kRUN,ROM,1);
      toDSP->Send(kRUN,kShowTemp,0);
      fReadOK=1;
      break;
  }

  
  if (fReadOK == 0) toDSP->Send(kRUN,kTemperature_Problem,0);  
  //if (fReadOK == 0) toDSP->Send(kRUN,kTemperature_Error,0);  
}

unsigned short calc_CRC(unsigned short shift_reg, unsigned short data){
  unsigned short fb;
  for (int i=0;i<8;i++){  
    fb = (shift_reg & 0x01) ^ ((data & (0x01 << i)) >> i);
    shift_reg = (shift_reg >> 1);
    if (fb==1) shift_reg = shift_reg^0x8c;
  }
  return(shift_reg);
}



void ReadSlaveMask(AMSWcom *node, unsigned short addr) {
  SlaveMask mask=node->ReadSlaveMask(addr);
  
  if (node->GetEventSize()) control->TDRmask=mask.ID[0];  
}



//void ShowConnect(SlaveMask mask) {
  //  const int config[24]={0,4,8,0xc,0x10,0x14,0x16,0x12,0xe,0xa,6,2,  1,5,9,0xd,0x11,0x15,0x17,0x13,0xf,0xb,7,3};  

  //const char on[2][5]={"[ ]\0","[X]\0"};

  // here we are only interested in mask 0
  
  //int id=0;
  //unsigned int connect=mask.ID[id];
//control->TDRMask=connect;
//}



void TakeRawCrate(int nevents) {
  char message[100];
  u_short dd[25000];
  const unsigned int Mask[3]={ 0x0000ff, 0x00ff00, 0xff0000 };

  unsigned int groupaddr=0x4000;
  if (control->JINFaddr!=0xffff) groupaddr=(groupaddr<<16)+control->JINFaddr;
  

  Jinf->WriteBusyMask(control->JINFaddr, 0);
  Jinf->SlaveTest(control->JINFaddr);

  Jinf->SetMode(control->JINFaddr,0);
  Jinf->SetMode(groupaddr,0);
  Jinf->EventReset(groupaddr);
  Jinf->EventReset(control->JINFaddr);



  //  for (int i=0; i<3; i++) {
  //    //LPRINTF("Writing slave mask %d\n",i);
  //    Jinf->WriteSlaveMask(0xffff,0,Mask[i]);
  //    usleep(1000);
  //    Jinf->SetMode(0x4001+i,1);
  //    //TESTRXDONE(node);
  //  }



  int runno=RunLog();
  sprintf(message,"About to start raw data full crate calibration run %d...",runno);
  toLOG->Send(kRUN,message,1);

  //  OpenOctantFile();
  FILE *datafile;
  char filename[200];


  int evtcnt;

  unsigned short usize=0, jinfevtnum=0;

  datades DataDes;


  for (int group=0; group<3; group++) {
    datafile=0;
    sprintf(filename,"/data/trackertest/calibrations/caldat_%d_%05d.dat",group,runno);
    datafile=fopen(filename,"wb");
    if (datafile==0) {
      printf("file error...\n");
      continue;
    }
    
    evtcnt=0;
    Jinf->WriteSlaveMask(control->JINFaddr,0,Mask[group]);
    Jinf->WriteBusyMask(control->JINFaddr,0xffffff & (~Mask[group]));

    Jinf->EventReset(groupaddr);
    Jinf->EventReset(control->JINFaddr);

    Jinf->SetMode(groupaddr,1);

    Jinf->SetMode(control->JINFaddr,1);

    int ControlOn=1;
    while(ControlOn) {
      //Jinf->Command_Lv1();
      //usleep(10000); // leave the time to the TDRs to take data
      
      memset(dd,0,sizeof(dd));
      jinfevtnum=0;
      usize=0;
      usize=Jinf->GetEvent(control->JINFaddr);
      
      if (usize>0) {
	evtcnt++;
	fwrite(&usize, sizeof(usize), 1, datafile);
	fwrite(Jinf->Event,usize*sizeof(unsigned short),1,datafile);
	
	ExtractEvent(Jinf->Event,usize,&DataDes);
	for (int tdr=group*8; tdr<(group+1)*8; tdr++) CheckEvent(evtcnt,tdr,DataDes);
      }
      if (evtcnt==nevents) ControlOn=0;
    
      control->NEvents=evtcnt;
      if (control->NEvents%10==0) {
	RunTime->UpdateLive();
	UpdateRunInfo();
      }
    }

    fclose(datafile);  

    Jinf->SetMode(groupaddr,0);
    Jinf->SetMode(control->JINFaddr,0);

  }
  

  //  if (WRITE) CloseFile();

  sprintf(message,"Events for raw calibration acquired");
  toLOG->Send(kRUN,message,1);
  unsigned int oldmask=1<<control->TDRref;
  Jinf->WriteSlaveMask(control->JINFaddr,0,oldmask);
  Jinf->WriteBusyMask(control->JINFaddr,~oldmask);

  for (int group=0; group<3; group++) {
    sprintf(message,"bin/CALIBRAWTK %d %d %d", runno, group, control->TrackerXtant);
    toLOG->Send(kRUN,message,1);
    system(message);
  }

  StopRun();  
}



void TakeRawCratePwg(int nevents) {
  char message[100];
  u_short dd[25000];
  const unsigned int Mask[4]={ 0x000333, 0x333000, 0xccc000, 0x000ccc };
 
  unsigned int groupaddr=0x4000;
  if (control->JINFaddr!=0xffff) groupaddr=(groupaddr<<16)+control->JINFaddr;
 
  Jinf->WriteBusyMask(control->JINFaddr, 0);
  Jinf->SlaveTest(control->JINFaddr);

  Jinf->SetMode(control->JINFaddr,0);
  Jinf->SetMode(groupaddr,0);
  Jinf->EventReset(groupaddr);
  Jinf->EventReset(control->JINFaddr);



  //  for (int i=0; i<3; i++) {
  //    //LPRINTF("Writing slave mask %d\n",i);
  //    Jinf->WriteSlaveMask(0xffff,0,Mask[i]);
  //    usleep(1000);
  //    Jinf->SetMode(0x4001+i,1);
  //    //TESTRXDONE(node);
  //  }



  int runno=RunLog();
  sprintf(message,"About to start raw data full crate calibration run %d...",runno);
  toLOG->Send(kRUN,message,1);

  //  OpenOctantFile();
  FILE *datafile;
  char filename[200];


  int evtcnt;

  unsigned short usize=0, jinfevtnum=0;

  datades DataDes;

  int errorcnt=0;

  for (int group=0; group<4; group++) {
    datafile=0;
    sprintf(filename,"/data/trackertest/calibrations/caldat_%d_%05d.dat",group,runno);
    datafile=fopen(filename,"wb");
    if (datafile==0) {
      printf("file error...\n");
      continue;
    }
    
    evtcnt=0;
    Jinf->WriteSlaveMask(control->JINFaddr,0,Mask[group]);
    Jinf->WriteBusyMask(control->JINFaddr,0xffffff & (~Mask[group]));

    Jinf->EventReset(groupaddr);
    Jinf->EventReset(control->JINFaddr);
    sleep(1);
    Jinf->SetMode(groupaddr,1); // mixed mode
    Jinf->SetMode(control->JINFaddr,1);

    sleep(1);


    int ControlOn=1;
    int first10=0;

    while(ControlOn) {
      //Jinf->Command_Lv1();
      //usleep(10000); // leave the time to the TDRs to take data
      
      memset(dd,0,sizeof(dd));
      jinfevtnum=0;
      usize=0;
      usize=Jinf->GetEvent(control->JINFaddr);


      if (usize>0) {
	first10++;
	if (first10<10) continue; // we skip the first 10 events
	evtcnt++;
	fwrite(&usize, sizeof(usize), 1, datafile);
	fwrite(Jinf->Event,usize*sizeof(unsigned short),1,datafile);
	
	ExtractEvent(Jinf->Event,usize,&DataDes);
	errorcnt=0;
	for (int tdr=0; tdr<24; tdr++) if (Mask[group] & (1<<tdr)) errorcnt+=CheckEvent(evtcnt,tdr,DataDes);
      }
      if ((evtcnt==nevents) || errorcnt) ControlOn=0;
    
      control->NEvents=evtcnt;
      if (control->NEvents%10==0) {
	RunTime->UpdateLive();
	UpdateRunInfo();
      }
    }

    fclose(datafile);  

    Jinf->SetMode(groupaddr,0);
    Jinf->SetMode(control->JINFaddr,0);
    if (errorcnt) break;
  }
  

  //  if (WRITE) CloseFile();

  sprintf(message,"Events for raw calibration acquired");
  toLOG->Send(kRUN,message,1);
  unsigned int oldmask=1<<control->TDRref;
  Jinf->WriteSlaveMask(control->JINFaddr,0,oldmask);
  Jinf->WriteBusyMask(control->JINFaddr,~oldmask);

  if (errorcnt==0) 
    for (int group=0; group<4; group++) {
      sprintf(message,"bin/CALIBRAWTK %d %d %d", runno, group, control->TrackerXtant);
      toLOG->Send(kRUN,message,1);
      system(message);
    }
  
  StopRun();  
}




void ExtractEvent(unsigned short *data, int datasize, datades* descr) {

  memset(descr,0xff,sizeof(*descr));

  int cnt=0;
  unsigned short jinfevtnum=0,size=0,start=0,id=0, mode=0, error=0;
  jinfevtnum=data[cnt];
  cnt++;  
  do {
    size=data[cnt];
    start=cnt+1;
    id=data[cnt+size] & 0x1f;
    mode=3 & (data[cnt+size]>>6);
    error=data[cnt+size]>>8;
    cnt+=size+1;
    descr->start[id]=start;
    descr->size[id]=size;
    descr->mode[id]=mode;
    descr->error[id]=error;
    //printf("tdr %d size %d mode %d error %d\n", id,size,mode,error);
  } while (cnt<datasize-1); 
}



int PrintRXDONE(AMSWcom *node) {
  char message[200];
  int error=0;
  if (node->GetHW()==kAMSW_EPP) { 
    sprintf(message,"rxdone = %04x\n",node->GetRX_DONE());
    toLOG->Send(kRUN,message,1);
    if (node->GetRX_DONE()!=0x4180) error=1; 
  }
  if (node->GetHW()==kAMSW_PCI) {
    sprintf(message,"rxdone = %d\n",(short) node->GetRX_DONE());
    toLOG->Send(kRUN,message,1);
    if (node->GetRX_DONE()!=0) error=1; 
  }
  

  return error;

}


int CheckEvent(int evtnum, int tdr, datades descr) {
  char message[200], message2[200];
  int res=0;
  //  for (int id=0; id<Ntdrs; id++) {
  if (descr.size[tdr]) {
    memset(message,0,sizeof(message));
    if (descr.error[tdr] & 0x2) sprintf(message,"BUILD errors\n");
    if (descr.error[tdr] & 0x4) sprintf(message,"%sFE power fault\n",message);
    if (descr.error[tdr] & 0x80) {
      if (descr.error[tdr] & 0x8) sprintf(message,"%sTIMEOUT\n",message);
      if (descr.error[tdr] & 0x10) sprintf(message,"%sAMSW error\n",message);
      if (descr.error[tdr] & 0x20) sprintf(message,"%sAssembly error\n",message);
      if (descr.error[tdr] & 0x40) sprintf(message,"%sCRC error\n",message);
    }
    if (strlen(message)) {
      sprintf(message2,"TDR %2d in event %d has errors:\n%s",tdr, evtnum,message);
      res=1;
      toLOG->Send(kRUN,message2,1);
    }
  } else {
    sprintf(message2,"In event %d, TDR %d did not reply !\n",evtnum,tdr);
    res=1;
    toLOG->Send(kRUN,message2,1);
  }

  return res;
}



int RunLog() {
  int runno=-1;
  FILE *log;
  log=fopen("runlog.log","r+");
  if (fopen==NULL) { printf("runlog.log not found !\n"); return -1; }
  do {
    fscanf(log,"%d",&runno);
  } while (!feof(log));
  fprintf(log,"%d\n",++runno);
  fclose(log);
  return runno;
}
