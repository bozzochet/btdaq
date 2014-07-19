#include "Jinj.h"
#include "PUtil.h"


ConfParsJ::ConfParsJ() {
  //  logfile=log;
  Nslave=NSLAVE;
  JINJflash=0;
  memset(mode,0,sizeof(mode));
  sprintf(DATAPATH,"");
  sprintf(CALPATH,"");
  refmask=0;
}

int ConfParsJ::ReadConfig(char * conf_file) {
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
  for (int tdr=0; tdr<NSLAVE-1; tdr++) {

    fscanf(file, "%d %d %d %s",&a,&tmp,&mode[tdr],SlaveConfFile[tdr]);
    if(tmp) refmask+= 1<<tdr;
    //LPRINTF("TDR %2d will be set to%s%s%s acquisition mode\n",tdr,(mode[tdr]==0)?" NO ":"",(mode[tdr]&1)?" RAW":"",(mode[tdr]&2)?" RED":"");
  }
  
  LPRINTF("JINJ program: 0x%04x\n", JINJflash);

	
  fclose(file);
  return ret;
}
//--------------------------------------------------------------

Jinj::Jinj(char* name,char* conf_file,int address, AMSWcom* node_in){
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
    if(CPars->refmask&(1<<ii)) {
      sprintf(namejinf,"JINF_%02d",ii);
      int addr=ii<<8|0x3f;
      if(!Slave[NSlave]) {
	Slave[NSlave]= new Jinf(namejinf,CPars->SlaveConfFile[ii],addr,node);
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

int Jinj::Init() {

  int ret=0;
  u_short JINJflashtemp = CPars->JINJflash & 4095; // JINFflash & 0x0FFF : Xabc -> 0abc;

  LPRINTF("booting JINJ at address 0x%04X with program 0x%x \n",selfaddress,JINJflashtemp);
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

  for (int ii=0;ii<NSlave;ii++){
    ret=Slave[ii]->Init();
    if (ret) return ret;
  }
  ret=SetModeSlaves(0,0);
  if (ret) return ret;
  
  ret=EventReset();
  if (ret) return ret;

  LPRINTF("Searching for Slaves ... \n");
  node->SlaveTest(selfaddress);
  sleep(1);
  
  ret=ReadSlaveMask();
  if (ret<0) return ret;

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

int Jinj::SetModeSlaves(int jinfmode,int tdrmode){
  int ret=0;
  for (int ii=0;ii<NSlave;ii++){
    ret+=Slave[ii]->SetModeJinf(jinfmode);
    ret+=Slave[ii]->SetModeTdrs(tdrmode);
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
    LPRINTF("ERROR: not all or not only needed JINFs seem to be connected\n");
    return -1;
  }
  else if( mask.ID[0]==0){
    LPRINTF("ERROR: no slaves at all are connected to main node\n");
    return -1;
  }
  else 
    LPRINTF("OK: all and only the expectes JINFs  are connected to main node\n");
 
  return ret;
}


int Jinj::GetEventNumber() {
  int ret=0;

  LPRINTF("Getting last event numbers...\n");
	
  jinjevents=node->GetLastEventN(selfaddress)-1;
  TESTRXDONE(node);
	
  LPRINTF("JINJ      : %6d\n",jinjevents);
	
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
  sprintf(numbers,"%6d ",jinjevents);

  LPRINTF("Printing all last event numbers...\n");
  
  for (int ii=0;ii<NSlave;ii++)
    sprintf(numbers,"%s |%s",numbers,Slave[ii]->PrintAllEventNumber(0,SlaveAdd[ii])); 
  
  
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

int   Jinj::SaveCalWhenDone(int run_number,char *addprefix){
  int ret=0;
  if(Slave[0]->CalIsDone()==0)
    for (int ii=0;ii<NSlave;ii++){
      ret=Slave[ii]->SaveCalibrations(run_number,SlaveAdd[ii],addprefix);//Slaveadd[ii]
      if(ret) return ret;
    }
  return ret;
}


int Jinj::EventReset() {
  int ret=0;

 for (int ii=0;ii<NSlave;ii++)
   Slave[ii]->EventReset();
 
 LPRINTF("Resetting Jinj 0x%04X node counter...\n",selfaddress);
 node->EventReset(selfaddress);
 TESTRXDONE(node);

 

  return ret;
}


void Jinj::ExtractEvent(int offset, int datasize) {

  int cnt=0;
  cnt+=offset; // skip the event number
  unsigned short size=0,start=0,id=0, mode=0, error=0;
  Jinf* aa;
  do {
    size=node->Event[cnt];
    start=cnt+1;
    id=node->Event[cnt+size] & 0x1f;//Status & 1f return JINF number
    if(aa=GetSlavePointer(id))
      aa->ExtractEvent(start,size);
    cnt+=size+1;
  } while (cnt<datasize-1); 
  return;
}

void Jinj::CheckEvent(int evtnum){
  for (int ii=0;ii<NSlave;ii++)
    Slave[ii]->CheckEvent(evtnum);
 return; 
}

Jinf* Jinj::GetSlavePointer(int id){
  
  for (int ii=0;ii<NSLAVE;ii++)
    if(SlaveAdd[ii]==id) return Slave[ii];
  
  LPRINTF(" Error Slave %02d should not be connected \n",id);
  return NULL;
}
