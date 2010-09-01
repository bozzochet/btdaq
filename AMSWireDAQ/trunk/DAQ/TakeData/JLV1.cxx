#include <sys/time.h>
#include <sys/stat.h>
#include "JLV1.h"
#include "PUtil.h"


//--------------------------------------------------------------
JLV1::JLV1(char* name,char* conf_file,int address,AMSWcom* node_in):JinjSlave(name, conf_file, address, node_in){
  //  logfile=log;
  sprintf(myname,"%s",name);
  selfaddress=address;
  if(selfaddress==0xffff) broadcastadd=0x4000;
  else broadcastadd=(0x4000<<16)|selfaddress;
  node=node_in;
  CPars= new ConfPars();
  CPars->ReadConfig(conf_file);
  mask.Nmask=0;
  for (int ii=0;ii<16;ii++) mask.ID[ii]=0; 
  jlv1events=0;
}
//--------------------------------------------------------------
JLV1::~JLV1(){

  if(CPars) delete CPars;
}
//--------------------------------------------------------------
int JLV1::Init() {
  int ret=0;
  u_short JLV1flashtemp = CPars->JLV1flash & 4095; // JLV1flash & 0x0FFF : Xabc -> 0abc;

  PRINTF("booting JLV1 at address 0x%04X WITH PROGRAM 0x%x \n",selfaddress,JLV1flashtemp );
  node->Boot(selfaddress);
  sleep(2);
  ret=PrintRXDONE(node);
  // loading the right DAQ program
  node->FlashLoad(selfaddress, JLV1flashtemp | 8192); // JLV1flash | 0x2000 : 0abc -> 2abc
  ret=PrintRXDONE(node);
  if (ret==0)   LPRINTF("Loaded program 0x%x on JLV1\n", JLV1flashtemp | 8192);
  if (ret) {
    node->FlashLoad(selfaddress, JLV1flashtemp | 12288); // JLV1flash | 0x3000 : 0abc -> 3abc
    ret=PrintRXDONE(node);
    if (ret==0)   LPRINTF("Loaded program 0x%x on JLV1\n", JLV1flashtemp | 12288); 
  }
  if (ret) {
    node->FlashLoad(selfaddress, JLV1flashtemp | 4096); // JLV1flash | 0x1000 : 0abc -> 1abc
    ret=PrintRXDONE(node);
    if (ret==0)   LPRINTF("Loaded program 0x%x on JLV1\n", JLV1flashtemp | 4096);
  }
  if (ret) {
    node->FlashLoad(selfaddress, JLV1flashtemp); // JLV1flash = 0abc
    ret=PrintRXDONE(node);
    if (ret==0)   LPRINTF("Loaded program 0x%x on JLV1\n", JLV1flashtemp);
  }

  sleep(1);

  TESTRXDONE(node);
	
  LPRINTF("Main: setting mode 0...\n");
  SetMode(0);
  TESTRXDONE(node);

//  LPRINTF("Main: setting delay to %d ns ...\n",CPars->delay*20);
//  node->WriteDelay(selfaddress,CPars->delay); 
//  TESTRXDONE(node);

  sleep(1);
  
  //reset Counters and flush the buffers
  EventReset();

  PRINTF("JLV1 is ready and clean, now we can turn on the DAQ mode...\n");
	
  ret=SetMode(1);
  TESTRXDONE(node);
	
  return ret;
}

int JLV1::SetDelay(){
  int ret=0;
  //  LPRINTF("Main: setting delay to %d ns ...\n",CPars->delay*20);
  //  node->WriteDelay(selfaddress,CPars->delay); 
  //  TESTRXDONE(node);
  return ret;
}

int JLV1::SetMode(int modein) {
  int ret=0;
  PRINTF("Setting JLV1 0x%04X to mode %d...\n",selfaddress,modein); 
  node->SetMode(selfaddress,modein);
  TESTRXDONE(node);
	return ret;
}

int JLV1::GetEventNumber() {
  int ret=0;

  PRINTF("Getting last event numbers from %x\n", selfaddress);
	
  jlv1events=node->GetLastEventN(selfaddress);
  TESTRXDONE(node);

  //  PRINTF("JLV1       : %6d, %6d\n",jlv1events, node->Event[1]);//only for debug
	
  return ret;
}

char* JLV1::PrintAllEventNumber(int log,int JLV1num) {
  int ret=0;
  static char numbers[1023];
  sprintf(numbers,"");

  if (log) LPRINTF("Printing all last event numbers...\n");
	
  sprintf(numbers,"Jlv1 %02d: %6d",JLV1num,jlv1events);
    
  if (log) PRINTF("%s\n", numbers);

  return numbers;
}

int JLV1::EventReset() {
  int ret=0;

  PRINTF("Resetting JLV1 0x%04X node counter...\n",selfaddress);
  node->EventReset(selfaddress);
  TESTRXDONE(node);
  return ret;
}

int JLV1::OverWriteDelay(u_short del){//come scritto in precedenza questa se ci sarà, sarà per qualche parametro tipo la frequenza del generatore, non certo il delay (TTH)
  int ret=0;
  u_short d2=del/20;
  //  LPRINTF("%s: setting delay to %d ns ...\n",myname,d2*20);
  //  node->WriteDelay(selfaddress,d2); 
  //  TESTRXDONE(node);
  return 0;
}

int JLV1::PulseTrigger(){
  int ret=0;

  //  node->Command(selfaddress, 0x49, 3, 0x6001, 0x0108, 0x4E1F);//(0x4E1F+1)*50usec; internal pulse - 1 hz
  node->Command(selfaddress, 0x49, 3, 0x6001, 0x0108, 0x7CF);//(0x7CF+1)*50usec; internal pulse - 10 hz
  //  node->Command(selfaddress, 0x49, 3, 0x6001, 0x0108, 0x00C7);//(0xC7+1)*50usec; internal pulse - 100 hz
  //  node->Command(selfaddress, 0x49, 3, 0x6001, 0x0108, 0x0013);//(0x13+1)*50usec; internal pulse - 1000hz
  TESTRXDONE(node);
  if (!ret) PRINTF("The trigger is set to pulser mode...\n");
  
  return ret;
}

int JLV1::TriggerOn(){
  int ret=0;
  
  node->Command(selfaddress, 0x49, 3, 0x6001, 0x0300, 1);
  //node->Command(selfaddress, 0xf, 7,0x8005,0,0x60c,0x30,0x60d,0,0x8ab8);
  TESTRXDONE(node);
  if (!ret) PRINTF("The trigger is on...\n");
  
  return ret;
}

int JLV1::TriggerOff(){
  int ret=0;
  
  node->Command(selfaddress, 0x49, 3, 0x6001, 0x0300, 0);
  //node->Command(selfaddress, 0xf, 7,0x8005,0,0x60c,0x30,0x60d,0x40,0xc2c7);
  TESTRXDONE(node);
  if (!ret) PRINTF("The trigger is off...\n");
  
  return ret;
}

int JLV1::CalibTriggerOn(){
  int ret=0;
  
  //  node->Command(selfaddress, 0x53, 2, 0, 0x80C7);//(0xC7+1)*50usec; internal pulse - 100hz
  node->Command(selfaddress, 0x53, 2, 0, 0x8013);//(0x13+1)*50usec; internal pulse - 1000hz
  TESTRXDONE(node);
  if (!ret) PRINTF("The calibration trigger is on...\n");
  
  return ret;
}

int JLV1::Calib2PassTriggerOn(int dtime){
 int ret=0;
 
 int dtime2=(dtime*100)/4;
unsigned short int par[3];
 par[0]=0;
par[1]=0x32;
par[2]=(dtime*100)/4; 
 node->Command2(selfaddress, 0x53, 3, par);
 TESTRXDONE(node);
 PRINTF("CALIB SECOND PASS (ret = %d)!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", ret);
 if (!ret) PRINTF("The calibration trigger is on...\n");
 
 return ret;
}

int JLV1::CalibTriggerOff(){
 int ret=0;
 
 node->Command(selfaddress, 0x53,1, 1);
 TESTRXDONE(node);
 if (!ret) PRINTF("The calibration trigger is off...\n");
 
 return ret;
}

int JLV1::WriteBusyMask(ushort busy0x12, ushort busy0x13, ushort busy0x14) {
  int ret=0;
  PRINTF("Writing BusyMask on JLV1 0x%04X...\n",selfaddress); 
  ret=node->WriteJLV1BusyMask(selfaddress, busy0x12, busy0x13, busy0x14);
  return ret;
}
