#include "Jinj.h"
#include "PUtil.h"
#include <cstring> // needed to add this for gcc 4.3

ConfParsJ::ConfParsJ() {
  //  logfile=log;
  Nslave=NSLAVE;
  JINJflash=0;
  memset(mode,0,sizeof(mode));
  sprintf(DATAPATH,"");
  sprintf(CALPATH,"");
  refmask=0;
}

int ConfParsJ::ReadConfig(const char* conf_file) { // gcc 4.3: added 'const'
  int ret=0;
  char dummy[100];
  int firsttime=0;
  FILE *file=fopen(conf_file,"r");

  if (file==NULL) {
    PRINTF("ERROR: The configuration file %s was not found, I need this file\n", conf_file);
    return 1;
  }


  fscanf(file,"%s  %s",&dummy,&DATAPATH);
  fscanf(file,"%s  %s",&dummy,&CALPATH);
  fscanf(file,"%s %hx  ",&dummy, &JINJflash);
  fscanf(file,"%s %s %s  %s",&dummy, &dummy, &dummy, &dummy);
  
  int a, tmp;
  for (int slave=0; slave<NSLAVE-1; slave++) {

    fscanf(file, "%d %d %d %s",&a,&tmp,&mode[slave],SlaveConfFile[slave]);
    if(tmp) refmask+= 1<<slave;
    //LPRINTF("TDR %2d will be set to%s%s%s acquisition mode\n",tdr,(mode[tdr]==0)?" NO ":"",(mode[tdr]&1)?" RAW":"",(mode[tdr]&2)?" RED":"");
  }
  
  LPRINTF("JINJ program: 0x%04x\n", JINJflash);

	
  fclose(file);
  return ret;
}
//--------------------------------------------------------------

Jinj::Jinj(const char* name, const char* conf_file, int address, AMSWcom* node_in){ // gcc 4.3 : added 'const'
  char namejinf[50];

  sprintf(myname,"%s",name);
  selfaddress=address;
  broadcastadd=0x40004000;
  node=node_in;
  CPars= new ConfParsJ();
  CPars->ReadConfig(conf_file);
  mask.Nmask=0;
  for (int ii=0;ii<16;ii++) mask.ID[ii]=0; 
  
  jinjevents=0;
  for (int ii=0;ii<NSLAVE;ii++){
    jinfevents[ii]=0;
    Slave[ii]=NULL;
    SlaveAdd[ii]=-1;
  }
  
  NSlave=0;
  for (int ii=0;ii<NSLAVE;ii++)
    if((CPars->refmask&(1<<ii)) && strstr(CPars->SlaveConfFile[ii],"JINF")) {
      sprintf(namejinf,"JINF_%02d",ii);
//			printf("Created JINF_%02d\n",ii);//only for debug
      int addr=ii<<8|0x3f;
      if(!Slave[NSlave]) {
				Slave[NSlave]= new Jinf(namejinf,CPars->SlaveConfFile[ii],addr,node);
				SlaveAdd[NSlave]=ii;
				NSlave++;
      }
    }
		else if((CPars->refmask&(1<<ii)) && strstr(CPars->SlaveConfFile[ii],"JLV1")) {
      sprintf(namejinf,"JLV1_%02d",ii);
//			printf("Created JLV1_%02d\n",ii);//only for debug
      int addr=ii<<8|0x3f;
      if(!Slave[NSlave]) {
				Slave[NSlave]= new JLV1(namejinf,CPars->SlaveConfFile[ii],addr,node);
				SlaveAdd[NSlave]=ii;
				NSlave++;
      }
    }
}


Jinj::~Jinj(){
  if(CPars) delete CPars;
  for (int ii=0;ii<24;ii++)
    if(Slave[ii]) delete Slave[ii];
}

int Jinj::SelfInit() {

  int ret=0;
  u_short JINJflashtemp = CPars->JINJflash & 4095; // JINJflash & 0x0FFF : Xabc -> 0abc;

  PRINTF("booting JINJ at address 0x%04X with program 0x%x \n",selfaddress,JINJflashtemp);
  // loading the right DAQ program
  node->Boot(selfaddress, JINJflashtemp | 8192); // JINJflash | 0x2000 : 0abc -> 2abc
  sleep(5);
  ret=PrintRXDONE(node);
  if (ret==0)   LPRINTF("Loaded program 0x%x on Jinj\n", JINJflashtemp | 8192);
  if (ret) {
    node->Boot(selfaddress, JINJflashtemp | 12288); // JINJflash | 0x3000 : 0abc -> 3abc
    sleep(5);
    ret=PrintRXDONE(node);
    if (ret==0)   LPRINTF("Loaded program 0x%x on Jinj\n", JINJflashtemp | 12288); 
  }
  if (ret) {
    node->Boot(selfaddress, JINJflashtemp | 4096); // JINJflash | 0x1000 : 0abc -> 1abc
    sleep(5);
    ret=PrintRXDONE(node);
    if (ret==0)   LPRINTF("Loaded program 0x%x on Jinj\n", JINJflashtemp | 4096);
  }
  if (ret) {
    node->Boot(selfaddress, JINJflashtemp); // JINJflash = 0abc
    sleep(5);
    ret=PrintRXDONE(node);
    if (ret==0)   LPRINTF("Loaded program 0x%x on Jinj\n", JINJflashtemp);
  }

  sleep(1);

  TESTRXDONE(node);

  // FIXME Hard coded to switch off the secon JLV1
  SwitchOffNode(17);
	
	PRINTF("Searching for Slaves ... \n");
  node->SlaveTest(selfaddress);
  sleep(1);
	TESTRXDONE(node);
  
  ret=ReadSlaveMask();
  if (ret<0) return ret;
	else ret=0;
	
	return ret;
}

int Jinj::InitJinfs() {
	int ret=0;

  for (int ii=0;ii<NSlave;ii++){
		if (Slave[ii]->CPars->type == 0){//it's a Jinf
			ret=Slave[ii]->Init();
			if (ret) return ret;
		}
  }
	
  ret=SetModeSlaves(0,0);
  if (ret) return ret;
  
  ret=EventReset();
  if (ret) return ret;

  LPRINTF("Main: setting mode 0...\n");
  SetModeJinj(0);
  TESTRXDONE(node);
  
  ret=SetModeSlaves(1,-1);
  if (ret) return ret;
  
  ret=SetModeJinj(1);
  return ret;
}


int Jinj::SetDelay(){
  int ret=0;
  for (int ii=0;ii<NSlave;ii++){
    ret=Slave[ii]->SetDelay();
  }
  return ret;
}


int Jinj::SetModeSlaves(int jinjslavemode,int tdrmode){
  int ret=0;
  for (int ii=0;ii<NSlave;ii++){
		ret+=Slave[ii]->SetMode(jinjslavemode);
		if (Slave[ii]->CPars->type == 0){//it's a Jinf...
			ret+=((Jinf*)Slave[ii])->SetModeTdrs(tdrmode);
		}
  }
  return ret;
}

int Jinj::SetModeJinj(int modein) {
  int ret=0;
  LPRINTF("Setting JINJ 0x%04X to mode %d...\n",selfaddress,modein); 
  node->SetMode(selfaddress,modein);
  TESTRXDONE(node);
  return ret;
}

int Jinj::ReadSlaveMask() {
  // returns -1 on error
  // else returns the number of connected TDRs

  int ret=0;
  int expected=0;

  LPRINTF("Reading slave mask for address 0x%04x\n",selfaddress);

  mask=node->ReadSlaveMask(selfaddress);
  sleep(1);
  TESTRXDONE(node);

  ret=ShowConnect(mask);

  if(mask.ID[0]!=CPars->refmask){
    PRINTF("ERROR: not all or not only needed Slaves seem to be connected\n");
    //return -1;//the correct exit
		return 0;//for linking a Jinf useless
  }
  else if( mask.ID[0]==0){
    PRINTF("ERROR: no Slaves at all are connected to main node\n");
    return -1;
  }
  else 
    PRINTF("OK: all and only the expectes Slaves are connected to main node\n");
 
  return ret;
}


int Jinj::GetEventNumber() {
  int ret=0;

  PRINTF("Getting last event numbers from %x\n", selfaddress);
	
  jinjevents=node->GetLastEventN(selfaddress);
  TESTRXDONE(node);
	
  // PRINTF("JINJ      : %6d, %6d\n",jinjevents, node->Event[1]);
	
  for (int ii=0;ii<NSlave;ii++){
    Slave[ii]->GetEventNumber();
  }

  return ret;
}

char* Jinj::PrintAllEventNumber(int log) {
  int ret=0;
  static char numbers[1023];
  char savenumbers[1023];
  char * pp[24];
  sprintf(numbers,"Jinj: %6d ",jinjevents);

  LPRINTF("Printing all last event numbers...\n");
  
  for (int ii=0;ii<NSlave;ii++){
    sprintf(numbers,"%s\n%s\n",numbers,Slave[ii]->PrintAllEventNumber(0,SlaveAdd[ii])); 
  }
  // 	//only for now!!!!!!!!!!!!!!!!!!!!--------------------
  // 	if (NSlave==1) sprintf(numbers,"%s 99F 999999  99 999999  99 999999  99 999999  99 999999",numbers);
  // 	//----------------------------------------------------
	
	
  if (log) PRINTF("%s\n", numbers);

  return numbers;
}

int Jinj::ShowConnect(SlaveMask mask) {
  int ntdrs=0;
  char line[100];
  const int config[NSLAVE]={0,4,8,0xc,0x10,0x14,0x16,0x12,0xe,0xa,6,2,1,5,9,0xd,0x11,0x15,0x17,0x13,0xf,0xb,7,3};  
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


int Jinj::EventReset() {
  int ret=0;

 for (int ii=0;ii<NSlave;ii++) {
   Slave[ii]->EventReset();
 }
 
 PRINTF("Resetting Jinj 0x%04X node counter...\n",selfaddress);
 node->EventReset(selfaddress);
 TESTRXDONE(node);

 

  return ret;
}

JinjSlave* Jinj::GetSlavePointer(int id){
  
  for (int ii=0;ii<NSLAVE;ii++) {
    if(SlaveAdd[ii]==id) return Slave[ii];
  }
	
  PRINTF(" Error Slave %02d should not be connected \n",id);
  return NULL;
}

JLV1* Jinj::GetJLV1Pointer(){
  JLV1* pointerout=0;
	for (int ii=0;ii<NSlave;ii++){
		if (Slave[ii]->CPars->type == 1){//it's the JLV1...
			pointerout = (JLV1*)Slave[ii];
		}
  }
	return pointerout;
}

int Jinj::SwitchOffNode(int jinjport){
  int ret=0;
  short int address=0x3f;
  address|=(jinjport<<8);
  ushort parms[10];
  
  node->Command2(address,0x4e,0,parms);
  TESTRXDONE(node);  
  printf(" Switching off JINJ Slave %d at address %x ret is %d \n",jinjport,address,ret);
  return ret;

}
