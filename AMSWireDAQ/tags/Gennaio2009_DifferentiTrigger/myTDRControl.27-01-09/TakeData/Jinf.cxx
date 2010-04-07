#include <sys/time.h>
#include <sys/stat.h>
#include "Jinf.h"
#include "PUtil.h"

//--------------------------------------------------------------
Jinf::Jinf(char* name,char* conf_file,int address,AMSWcom* node_in):JinjSlave( name, conf_file, address, node_in){
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
  jinfevents=0;
  for (int ii=0;ii<24;ii++)tdrevents[ii]=0;
}
//--------------------------------------------------------------
Jinf::~Jinf(){

  if(CPars) delete CPars;
}
//--------------------------------------------------------------

int Jinf::GenAddress(int tdrnum){
  int tdradd=(tdrnum<<8| 0x3f);
  if(selfaddress==0xffff) return tdradd; 
  else return (tdradd<<16|selfaddress);
}

//-------------------------------------------------
int Jinf::Init() {
  int ret=0;
  u_short JINFflashtemp = CPars->JINFflash & 4095; // JINFflash & 0x0FFF : Xabc -> 0abc;
  u_short TDRflashtemp = CPars->TDRflash & 4095; // TDRflash & 0x0FFF : Xabc -> 0abc

  PRINTF("booting JINF at address 0x%04X with program 0x%x\n",selfaddress,JINFflashtemp );
  // loading the right DAQ program
  node->Boot(selfaddress, JINFflashtemp | 8192); // JINFflash | 0x2000 : 0abc -> 2abc
  sleep(5);
  ret=PrintRXDONE(node);
  if (ret==0)   LPRINTF("Loaded program 0x%x on Jinf\n", JINFflashtemp | 8192);
  if (ret) {
    node->Boot(selfaddress, JINFflashtemp | 12288); // JINFflash | 0x3000 : 0abc -> 3abc
    sleep(5);
    ret=PrintRXDONE(node);
    if (ret==0)   LPRINTF("Loaded program 0x%x on Jinf\n", JINFflashtemp | 12288); 
  }
  if (ret) {
    node->Boot(selfaddress, JINFflashtemp | 4096); // JINFflash | 0x1000 : 0abc -> 1abc
    sleep(5);
    ret=PrintRXDONE(node);
    if (ret==0)   LPRINTF("Loaded program 0x%x on Jinf\n", JINFflashtemp | 4096);
  }
  if (ret) {
    node->Boot(selfaddress, JINFflashtemp); // JINFflash = 0abc
    sleep(5);
    ret=PrintRXDONE(node);
    if (ret==0)   LPRINTF("Loaded program 0x%x on Jinf\n", JINFflashtemp);
  }

  sleep(1);

  TESTRXDONE(node);

  LPRINTF("Main: setting mode 0...\n");
  SetMode(0);
  TESTRXDONE(node);

  PRINTF("Main: setting delay to %d ns ...\n",CPars->delay*20);
  node->WriteDelay(selfaddress,CPars->delay); 
  TESTRXDONE(node);
	
	PRINTF("Searching for Slaves ... \n");
  node->SlaveTest(selfaddress);
  sleep(1);

  ret=ReadSlaveMask();
  if (ret<0) return ret;
  TESTRXDONE(node);

  PRINTF("All TDRs: booting with program 0x%x\n",TDRflashtemp);
  // cycling trough all connected TDR, loading the right DAQ program
  for (int ii=0;ii<NTDRS;ii++){
    // LPRINTF("%d : %d\n", ii, CPars->mask.ID[0]&(1<<ii)); //Only for debug...
    if(CPars->refmask&(1<<ii)) {//forzare una maschera e POI riverificare se questa e' stata attesa!!!!
      int addr=GenAddress(ii);
      node->Boot(addr, TDRflashtemp | 0x2000); // TDRflash | 0x2000 : 0abc -> 2abc
      sleep(1);
      ret=PrintRXDONE(node);
      if (ret==0)   LPRINTF("Loaded program 0x%x on TDR %d\n", TDRflashtemp | 8192, ii);
      if (ret) {
	node->Boot(addr, TDRflashtemp | 0x3000); // TDRflash | 0x3000 : 0abc -> 3abc
	sleep(1);
	ret=PrintRXDONE(node);
	if (ret==0)   LPRINTF("Loaded program 0x%x on TDR %d\n", TDRflashtemp | 12288, ii);
      }
      if (ret) {
	node->Boot(addr, TDRflashtemp  | 0x1000); // TDRflash | 0x1000 : 0abc -> 1abc
	sleep(1);
	ret=PrintRXDONE(node);
	if (ret==0)   LPRINTF("Loaded program 0x%x on TDR %d\n", TDRflashtemp | 4096, ii);
      }
      if (ret) {
	node->Boot(addr, TDRflashtemp); // JINFflash = 0abc
	sleep(1);
	ret=PrintRXDONE(node);
	if (ret==0)   LPRINTF("Loaded program 0x%x on TDR %d\n", TDRflashtemp, ii);
      }
    }
  }

  sleep(1);
  
  LPRINTF("TDRs: setting mode 0...\n");
  SetModeTdrs(0);
  TESTRXDONE(node);
  
  //reset Counters and flush the buffers
  EventReset();

  PRINTF("JINF is ready and clean, now we can turn on the DAQ mode...\n");
	
  ret=SetModeTdrs();
	
  ret=SetMode(1);
  TESTRXDONE(node);
	
  return ret;
}

int Jinf::SetDelay(){
	int ret=0;
  PRINTF("Main: setting delay to %d ns ...\n",CPars->delay*20);
  node->WriteDelay(selfaddress,CPars->delay); 
  TESTRXDONE(node);
	return ret;
}
	
int Jinf::ReadSlaveMask() {
  // returns -1 on error
  // else returns the number of connected TDRs

  int ret=0;
  int expected=0;

  PRINTF("Reading slave mask for address 0x%04x\n",selfaddress);

  mask=node->ReadSlaveMask(selfaddress);
  sleep(1);
  TESTRXDONE(node);

  ret=ShowConnect(mask);

  if(mask.ID[0]!=CPars->refmask){
    PRINTF("ERROR: not all or not only needed tdrs seem to be connected\n");
    return -1;
  }
  else if( mask.ID[0]==0){
    PRINTF("ERROR: no slaves at all are connected to main node\n");
    return -1;
  }
  else 
    PRINTF("OK: all and only the expectes TDRs  are connected to main node\n");
 
  return ret;
}



int Jinf::SetMode(int modein) {
  int ret=0;
  PRINTF("Setting JINF 0x%04X to mode %d...\n",selfaddress,modein); 
  node->SetMode(selfaddress,modein);
  TESTRXDONE(node);
	return ret;
}

int Jinf::SetModeTdrs(int modein) {
  int ret=0;
  struct timeval start,stop;

  gettimeofday(&start,0);

  for (int ii=0;ii<NTDRS;ii++)
    if (CPars->refmask&(1<<ii)) {
      int address=GenAddress(ii);
      int mode=modein;
      if(modein==-1) mode=CPars->mode[ii];
      LPRINTF("Setting mode %d to TDR %d...\n", mode,ii); 
      node->SetMode(address,mode); 
      TESTRXDONE(node);
    }

  gettimeofday(&stop,0);
  LPRINTF("Done... time needed: %.1f ms\n",1000*(stop.tv_sec-start.tv_sec)+1e-3*(stop.tv_usec-start.tv_usec));
  return ret;
}



int Jinf::GetEventNumber() {
  int ret=0;

  LPRINTF("Getting last event numbers...\n");
	
  jinfevents=node->GetLastEventN(selfaddress);
  TESTRXDONE(node);
	
  LPRINTF("Main       : %6d\n",jinfevents);
	
  for (int ii=0;ii<NTDRS;ii++){
    //    LPRINTF("%d : %d\n", ii, CPars->mask.ID[0]&(1<<ii)); //only for debug
    if(CPars->refmask&(1<<ii) && CPars->mode[ii]) {
      int addr= GenAddress(ii);
      tdrevents[ii]=node->GetLastEventN(addr);
      TESTRXDONE(node);
      LPRINTF("TDR %02d     : %6d\n", ii, tdrevents[ii]);
    }
  }

  return ret;
}

char* Jinf::PrintAllEventNumber(int log,int Jinfnum) {
  int ret=0;
  static char numbers[1023];
  sprintf(numbers,"");

  if (log) LPRINTF("Printing all last event numbers...\n");
	
  

  sprintf(numbers,"%02d %6d",Jinfnum,jinfevents);
 

  for (int ii=0;ii<NTDRS;ii++)
    if(CPars->refmask&(1<<ii) && CPars->mode[ii]) 
      sprintf(numbers,"%s |%04d %6d",numbers,ii+Jinfnum*100,tdrevents[ii]);
    
  if (log) PRINTF("%s\n", numbers);

  return numbers;
}

int Jinf::EventReset() {
  int ret=0;

 

  PRINTF("Resetting tdrs counters...\n");
  node->EventReset(broadcastadd);
  TESTRXDONE(node);

  PRINTF("Resetting Jinf 0x%04X node counter...\n",selfaddress);
  node->EventReset(selfaddress);
  TESTRXDONE(node);
  return ret;
}


int Jinf::ShowConnect(SlaveMask mask) {
  int ntdrs=0;
  char line[100];
  const int config[NTDRS]={0,4,8,0xc,0x10,0x14,0x16,0x12,0xe,0xa,6,2,1,5,9,0xd,0x11,0x15,0x17,0x13,0xf,0xb,7,3};  
  LPRINTF("Summary:\n");
  const char on[2][5]={"[ ]\0","[X]\0"};
  
  for (int id=0; id<mask.Nmask; id++) {
    unsigned int connect=mask.ID[id];
    //    if (id==0) Mask=connect;
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

int Jinf::OverWriteDelay(u_short del){
  int ret=0;
  u_short d2=del/20;
  PRINTF("%s: setting delay to %d ns ...\n",myname,d2*20);
  node->WriteDelay(selfaddress,d2); 
  TESTRXDONE(node);
  return 0;
}
