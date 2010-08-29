#include "Jinj.h"
#include "PUtil.h"
#include <cstring> // needed to add this for gcc 4.3
#include <cmath>

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
  char dummy1[255];
  char dummy2[255];
  char dummy3[255];
  char dummy4[255];
  int firsttime=0;
  FILE *file=fopen(conf_file,"r");

  if (file==NULL) {
    PRINTF("ERROR: The configuration file %s was not found, I need this file\n", conf_file);
    return 1;
  }

  fscanf(file,"%s  %s", &dummy1, &DATAPATH);
  fscanf(file,"%s  %s", &dummy1, &CALPATH);
  fscanf(file,"%s %hx  ", &dummy1, &JINJflash);
  fscanf(file,"%s %s %s %s", &dummy1, &dummy2, &dummy3, &dummy4);
  
  int a, tmp;
  for (int slave=0; slave<NSLAVE; slave++) {
    fscanf(file, "%d %d %hd %s", &a, &tmp, &(mode[slave]), SlaveConfFile[slave]);
    //    printf("%d: %d %d %d %s\n", slave, a, tmp, mode[slave], SlaveConfFile[slave]);//only for debug
    if(tmp) {
      refmask = refmask|((unsigned int)pow(2.,slave));
      //      printf("RefMask = %d\n", refmask);//only for debug
    }
  }

  //  for (int slave=0; slave<NSLAVE; slave++) printf("%d: %d %s\n", slave, mode[slave], SlaveConfFile[slave]);//only for debug

  LPRINTF("JINJ program: 0x%04x\n", JINJflash);

	
  fclose(file);
  return ret;
}
//--------------------------------------------------------------

Jinj::Jinj(const char* name, const char* conf_file, int address, AMSWcom* node_in, bool flagfake){ // gcc 4.3 : added 'const'
  char namejinf[50];

  FlagFake=flagfake;
  if(flagfake) PRINTF("Creating a fake Jinj (using the first Jinf found in Jinj configuration file)!\n");
  sprintf(myname,"%s",name);
  selfaddress=address;
  broadcastadd=0x40004000;//no hacking if fake jinj because broadcast seems never used at Jinj level but only at Jinf level...
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
  for (int ii=0;ii<NSLAVE;ii++) {
    //    printf("RefMask Read = %d\n", CPars->refmask);//only for debug
    //    printf("Comparison with 1<<%d (%d) = %d\n", ii , (1<<ii), CPars->refmask&(1<<ii));//only for debug
    //    printf("CPars->SlaveConfFile[%d] = %s\n", ii, CPars->SlaveConfFile[ii]);//only for debug
    if((CPars->refmask&(1<<ii)) && strstr(CPars->SlaveConfFile[ii],"JINF")) {
      sprintf(namejinf,"JINF_%02d",ii);
      printf("Created JINF_%02d\n",ii);//only for debug
      int addr=0;
      if (FlagFake) addr=0xffff;
      else addr=ii<<8|0x3f;
      if(!Slave[NSlave]) {
	Slave[NSlave]= new Jinf(namejinf,CPars->SlaveConfFile[ii],addr,node);
	SlaveAdd[NSlave]=ii;
	//	printf("SlaveAdd[%d] = %d\n",NSlave, SlaveAdd[NSlave]);//only for debug
	NSlave++;
	if(flagfake) break;
      }
    }
    else if((CPars->refmask&(1<<ii)) && strstr(CPars->SlaveConfFile[ii],"JLV1")) {
      if (!FlagFake){//if fake Jinj cannot be any JLV1...
	sprintf(namejinf,"JLV1_%02d",ii);
	printf("Created JLV1_%02d\n",ii);//only for debug
	int addr=ii<<8|0x3f;
	if(!Slave[NSlave]) {
	  Slave[NSlave]= new JLV1(namejinf,CPars->SlaveConfFile[ii],addr,node);
	  SlaveAdd[NSlave]=ii;
	  NSlave++;
	}
      }
    }
  }
}


Jinj::~Jinj(){
  if(CPars) delete CPars;
  for (int ii=0;ii<NSLAVE;ii++)
    if(Slave[ii]) delete Slave[ii];
}

int Jinj::SelfInit() {

  int ret=0;
  if (FlagFake){
    // no action needed, assuming as true the CPars->refmask without probing with ReadSlaveMask()
  }
  else {
    u_short JINJflashtemp = CPars->JINJflash & 0xfff; // JINJflash & 0x0FFF : Xabc -> 0abc;
    
    PRINTF("booting JINJ at address 0x%04X with program 0x%x \n",selfaddress,JINJflashtemp);
    node->Boot(selfaddress); 
    sleep(2);
    ret=PrintRXDONE(node);
    // loading the right DAQ program
    node->FlashLoad(selfaddress, JINJflashtemp | 8192); // JINJflash | 0x2000 : 0abc -> 2abc
    ret=PrintRXDONE(node);
    if (ret==0)   LPRINTF("Loaded program 0x%x on Jinj\n", JINJflashtemp | 8192);
    if (ret) {
      node->FlashLoad(selfaddress, JINJflashtemp | 12288); // JINJflash | 0x3000 : 0abc -> 3abc
      ret=PrintRXDONE(node);
      if (ret==0)   LPRINTF("Loaded program 0x%x on Jinj\n", JINJflashtemp | 12288); 
    }
    if (ret) {
      node->FlashLoad(selfaddress, JINJflashtemp | 4096); // JINJflash | 0x1000 : 0abc -> 1abc
      ret=PrintRXDONE(node);
      if (ret==0)   LPRINTF("Loaded program 0x%x on Jinj\n", JINJflashtemp | 4096);
    }
    if (ret) {
      node->FlashLoad(selfaddress, JINJflashtemp); // JINJflash = 0abc
      ret=PrintRXDONE(node);
      if (ret==0)   LPRINTF("Loaded program 0x%x on Jinj\n", JINJflashtemp);
    }
    
    sleep(1);
    
    TESTRXDONE(node);
    
    // FIXME Hard coded to switch off the second JLV1
    //    SwitchOffNode(16);
    
    PRINTF("Searching for Slaves ... \n");
    node->SlaveTest(selfaddress);
    sleep(1);
    TESTRXDONE(node);
  
    ret=ReadSlaveMask();
    if (ret<0) return ret;
    else ret=0;
  }
  
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
  if (!FlagFake){
    SetModeJinj(0);
    TESTRXDONE(node);
  }
  
  ret=SetModeSlaves(1,0);
  if (ret) return ret;
  
  if (!FlagFake){
    ret=SetModeJinj(1);
  }

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
      if (tdrmode==99){
	ret+=((Jinf*)Slave[ii])->SetModeTdrs();
      }
      else {
	ret+=((Jinf*)Slave[ii])->SetModeTdrs(tdrmode);
      }
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

  if(mask.ID[0]==0){
    PRINTF("ERROR: no Slaves at all are connected to this node (%x)\n", selfaddress);
    return -1;
  }
  else if(mask.ID[0]!=CPars->refmask){
    if(mask.ID[0]&(CPars->refmask)){ //the AND gives not a 0, so there're more slaves read than expected
      PRINTF("WARNING: not only needed Slaves seem to be connected to this node (%x)\n", selfaddress);
      return 0;//to make possibe linking a JinjSlave not used
    }
    else {
      PRINTF("ERROR: not all needed Slaves seem to be connected to this node (%x)\n", selfaddress);
      return -1;//exit with error
    }
  }
  else 
    PRINTF("OK: all and only the expectes Slaves are connected to this node (%x)\n", selfaddress);
  
  return ret;
}


int Jinj::GetEventNumber() {
  int ret=0;
  
  LPRINTF("Getting last event numbers from %04hx\n", selfaddress);
  
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

  if (log) PRINTF("%s\n", numbers);

  return numbers;
}

int Jinj::ShowConnect(SlaveMask mask) {
  int ntdrs=0;
  char line[100];
  const int config[NSLAVE]={0,4,8,0xc,0x10,0x14,0x16,0x12,0xe,0xa,6,2,1,5,9,0xd,0x11,0x15,0x17,0x13,0xf,0xb,7,3};  
  LPRINTF("Summary:\n");
  const char on[2][5]={"[ ]\0","[X]\0"};
  
  for (int id=0; id<mask.Nmask; id++) {//loop on the various masks
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
  
  printf("Switching off JINJ Slave %d at address %x...\n", jinjport, address);
  node->Command2(address,0x4e,0,parms);
  TESTRXDONE(node);

  
  return ret;  
}

int Jinj::WriteMask(unsigned int mask){
  int ret=0;
  ushort maskid=0x4000; //is sufficient to give the less significant ciphers (40XX->XX) but is correct also in this way

  printf("Writing mask (%d) %x for Jinj at address %x...\n", 0xff&maskid, mask, selfaddress);
  node->WriteSlaveMask(selfaddress, maskid, mask);
  TESTRXDONE(node);

  return ret;
}
