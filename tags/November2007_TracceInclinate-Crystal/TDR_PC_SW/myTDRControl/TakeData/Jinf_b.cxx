#include <sys/time.h>

#include "Jinf.h"
#include "PUtil.h"



ConfPars::ConfPars() {
  //  logfile=log;
  Ntdrs=NTDRS;
  delay=0x96; // units of 20 ns --> 0x96= 3 mu sec
  JINFflash=0;
  TDRflash=0; 
  memset(mode,0,sizeof(mode));
  sprintf(DATAPATH,"");
  sprintf(CALPATH,"");
  refmask=0;

}

int ConfPars::ReadConfig(char * conf_file) {
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
  fscanf(file,"%s %hx  ",&dummy, &JINFflash);
  fscanf(file,"%s %hx  ",&dummy, &TDRflash);
  fscanf(file,"%s %hx  ",&dummy, &delay);
  fscanf(file,"%s %s %s",&dummy, &dummy, &dummy);
  
  int a, tmp;
  for (int tdr=0; tdr<NTDRS-1; tdr++) {
    fscanf(file, "%d %d %d",&a,&tmp,&mode[tdr]);
    if(tmp) refmask|= 1<<tdr;
    //  LPRINTF("TDR %2d  %d %d will be set to%s%s%s acquisition mode\n",tdr,tmp,refmask,(mode[tdr]==0)?" NO ":"",(mode[tdr]&1)?" RAW":"",(mode[tdr]&2)?" RED":""); 
    tmp=0;
 }
  
  LPRINTF("TDR program: 0x%04x\n", TDRflash);
  LPRINTF("JINF program: 0x%04x\n", JINFflash);

	
  fclose(file);
  return ret;
}
//--------------------------------------------------------------
Jinf::Jinf(char* name,char* conf_file,int address,AMSWcom* node_in){
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

  LPRINTF("booting JINF at address 0x%04X  \n",selfaddress);
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

  LPRINTF("Searching for Slaves ... \n");
  node->SlaveTest(selfaddress);
  sleep(1);

  ret=ReadSlaveMask();
  if (ret<0) return ret;

  LPRINTF("Main: setting mode 0...\n");
  SetModeJinf(0);
  TESTRXDONE(node);

  LPRINTF("Main: setting delay to %d ns ...\n",CPars->delay*20);
  node->WriteDelay(selfaddress,CPars->delay); 
  TESTRXDONE(node);

  LPRINTF("All TDRs: boot and load version...\n");
  // cycling trough all connected TDR, loading the right DAQ program
  for (int ii=0;ii<NTDRS;ii++){
    // LPRINTF("%d : %d\n", ii, CPars->mask.ID[0]&(1<<ii)); //Only for debug...
    if(mask.ID[0]&(1<<ii)) {
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

  TESTRXDONE(node);
  
  LPRINTF("TDRs: setting mode 0...\n");
  SetModeTdrs(0);
  TESTRXDONE(node);
  
  //reset Counters and flush the buffers
  EventReset();

  LPRINTF("The whole system is ready and clean, now we can turn on the DAQ mode...\n");
	
  ret=SetModeTdrs();
	
  ret=SetModeJinf(1);
  TESTRXDONE(node);
	
  return ret;
}


int Jinf::ReadSlaveMask() {
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
    LPRINTF("ERROR: not all or not only needed tdrs seem to be connected\n");
    return -1;
  }
  else if( mask.ID[0]==0){
    LPRINTF("ERROR: no slaves at all are connected to main node\n");
    return -1;
  }
  else 
    LPRINTF("OK: all and only the expectes TDRs  are connected to main node\n");
 
  return ret;
}



int Jinf::SetModeJinf(int modein) {
  int ret;
  LPRINTF("Setting JINF 0x%04X to mode %d...\n",selfaddress,modein); 
  node->SetMode(selfaddress,modein);
  TESTRXDONE(node);
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
  int sizeMain=0, sizeTDR=0;

  LPRINTF("Getting last event numbers...\n");
	
  jinfevents=node->GetLastEventN(selfaddress);
  TESTRXDONE(node);
	
  LPRINTF("Main       : %6d\n",sizeMain);
	
  for (int ii=0;ii<NTDRS;ii++){
    //    LPRINTF("%d : %d\n", ii, CPars->mask.ID[0]&(1<<ii)); //only for debug
    if(CPars->refmask&(1<<ii) && CPars->mode[ii]) {
      int addr= GenAddress(ii);
      tdrevents[ii]=node->GetLastEventN(addr);
      TESTRXDONE(node);
      LPRINTF("TDR %02d     : %6d\n", ii, sizeTDR);
    }
  }

  return ret;
}


int Jinf::EventReset() {
  int ret=0;

  LPRINTF("Resetting Jinf 0x%04X node counter...\n",selfaddress);
  node->EventReset(selfaddress);
  TESTRXDONE(node);

  LPRINTF("Resetting tdrs counters...\n");
  node->EventReset(broadcastadd);
  TESTRXDONE(node);

  return ret;
}


int Jinf::SaveCalibrations(){
  int ret=0;
  char calfileprefix[255];
  sprintf("%s/%s",CPars->CALPATH,myname);
  for (int ii=0;ii<NTDRS;ii++)
    if(CPars->refmask&(1<<ii) && CPars->mode[ii]) {
      ret=SaveOneCalibration(ii, calfileprefix);
      if (ret) return ret;
    }
  return 0;
}

int Jinf::SaveOneCalibration(unsigned int tdr_number, char *calfileprefix) {
  int ret=0;
	
  char calfilename[255];
	
  sprintf(calfilename,"%s_%02d.cal", calfileprefix,tdr_number);

  LPRINTF("Collecting and saving calibration data for TDR %d\n", tdr_number);

  int addr=GenAddress(tdr_number);;

  node->GetCalibration(addr); // GetCalibration from TDR 0xXX3f requested as TDR NN

  TESTRXDONE(node);

  FILE *calfil;
  calfil=fopen(calfilename,"wt");
  if (calfil==NULL) {
    PRINTF("Error: file %s could not be created, perhaps the calibrations dir %s doesn't exist ?\n",calfilename, CPars->CALPATH);
    return 1;
  }

  for(int i=0; i<16; i++) fprintf(calfil,"%02d,   %d,  %d\n",i,0,0); // to keep compatibilty with standard cal files

  for(int i=0; i<1024; i++) {
    fprintf(calfil,"%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\n",1+i, 1+i/64, 1+i%64, node->ped[i], node->sigr[i], node->sig[i],0.0, node->sta[i]);
    //    LPRINTF("%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\n",1+i, 1+i/64, 1+i%64, node->ped[i], node->sigr[i], node->sig[i],0.0, node->sta[i]);//only for debug
  }

  TESTRXDONE(node);

  fclose(calfil); 
  LPRINTF("Calibration file %s saved\n",calfilename);

  return ret;
}


int Jinf::SaveCalWhenDone(int run_number,char *addprefix){
  char calfileprefix[255];
  int ping_ok=1;
  int ret=0;

  if(addprefix==0)
    sprintf(calfileprefix,"%s/%06d_%s",CPars->CALPATH,run_number,myname);
  else
    sprintf(calfileprefix,"%s/%06d_%s_%s",CPars->CALPATH,run_number,myname,addprefix);
  for (int ii=0;ii<NTDRS;ii++)
    if(CPars->refmask&(1<<ii)) {
      while (ping_ok!=0){
	int addr=GenAddress(ii);
        node->Ping(addr); // ping command goes timeout if we are calibrating
        node->Ping(addr); // sometimes the first ping goes ok even if we are calibrating
        ping_ok=GETRXDONE(node);//pinging two times only one TDR is enough...
      }
      ret=SaveOneCalibration(ii, calfileprefix);
      ping_ok=1;
      if (ret) return ret;
    }
  return 0;
}


void Jinf::ExtractEvent(int offset, int datasize) {

  memset(&descr,0xff,sizeof(descr));

  int cnt=0;
  cnt+=offset;
  unsigned short size=0,start=0,id=0, mode=0, error=0;
  do {
    size=node->Event[cnt];
    start=cnt+1;
    id=node->Event[cnt+size] & 0x1f;//Status & 1f return TDR number
    mode=3 & (node->Event[cnt+size]>>6);
    error=node->Event[cnt+size]>>8;
    descr.start[id]=start;
    descr.size[id]=size;
    descr.mode[id]=mode;
    descr.error[id]=error;
    cnt+=size+1;
    //printf("tdr %d size %d mode %d error %d\n", id,size,mode,error);
  } while (cnt<datasize-1); 
}



void Jinf::CheckEvent(int evtnum) {
  char message[200];


  for (int tdr=0;tdr<NTDRS;tdr++)
    if(CPars->refmask&(1<<tdr)){

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
	if (strlen(message)) LPRINTF("TDR %2d in event %d has errors:\n%s",tdr, evtnum,message);
	
      } else LPRINTF("In event %d, TDR %d did not reply !\n",evtnum,tdr);
    }
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
