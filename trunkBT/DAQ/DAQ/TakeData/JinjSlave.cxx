#include <sys/time.h>
#include <sys/stat.h> 
#include <cstring> // needed to add for gcc 4.3
#include "JinjSlave.h"
#include "PUtil.h"


ConfPars::ConfPars() {
	//  logfile=log;
	type=0; //0 = Jinf, 1= JLV1, 2=Dampe
	Ntdrs=NTDRS;
	delay=0x96; // units of 20 ns --> 0x96= 3 mu sec //Non sò se ci sarà un qualche parametro di questo tipo sul JLV1, magari la frequenza del generatore...
	JINFflash=0;
	TDRflash=0; 
	JLV1flash=0;
	memset(mode,0,sizeof(mode));
	sprintf(DATAPATH,"%s", "");
	sprintf(CALPATH,"%s", "");
	sprintf(REMOTEPATH, "%s", "");
	refmask=0;
	memset(SLowTrash,0,sizeof(SLowTrash));
	memset(SHighTrash,0,sizeof(SHighTrash));
	memset(KLowTrash,0,sizeof(KLowTrash));
	memset(KHighTrash,0,sizeof(KHighTrash));
	memset(SigmaRowTrash,0,sizeof(SigmaRowTrash));
	memset(OccupancyThresh,0,sizeof(OccupancyThresh)); //da rivedere il nome della variabile!!!
}

//----------------------------------------------------------
int ConfPars::ReadConfig(char * conf_file) {
  int ret=0;
  char dummy[100];
  FILE *file=fopen(conf_file,"r");
  
  if (file==NULL) {
    PRINTF("ERROR: The configuration file %s was not found, I need this file\n", conf_file);
    return 1;
  }
  if (strstr(conf_file, "DAMPE")) {
    type = 2;
    fscanf(file, "%s %s   ", &dummy[0], &dampeIP[0]);
    fscanf(file, "%s %s   ", &dummy[0], &dampePORT[0]);
    fscanf(file, "%s %s   ", &dummy[0], &REMOTEPATH[0]);
    LPRINTF("DAMPE destination: %s:%s\n", dampeIP, dampePORT);
    LPRINTF("DAMPE remote directory: %s\n", REMOTEPATH);
  } else {
    fscanf(file,"%s  %s", &dummy[0], &DATAPATH[0]);
    fscanf(file,"%s  %s", &dummy[0], &CALPATH[0]);
    if (strstr(conf_file,"JLV1")){
      type=1;
      fscanf(file,"%s %hx  ", &dummy[0], &JLV1flash);
      fscanf(file,"%s %hd  ", &dummy[0], &delay);
      delay/=20;
      
      LPRINTF("JLV1 program: 0x%04x\n", JLV1flash);
    } else if (strstr(conf_file,"JINF")){
      type=0;
      fscanf(file,"%s %hx  ",&dummy[0], &JINFflash);
      fscanf(file,"%s %hx  ",&dummy[0], &TDRflash);
      fscanf(file,"%s %hd  ",&dummy[0], &delay);
      delay/=20;
      fscanf(file,"%s %s %s %s %s %s %s %s %s",&dummy[0], &dummy[0], &dummy[0], &dummy[0], &dummy[0], &dummy[0], &dummy[0], &dummy[0], &dummy[0]);
      
      int a, tmp, inutile;
      float b,c,d,e,f,g;
      for (int tdr=0; tdr<NTDRS; tdr++) {
	inutile = refmask;
	fscanf(file, "%d %d %hd %f %f %f %f %f %f", &a, &tmp, &mode[tdr], &b, &c, &d, &e, &f, &g);
	SLowTrash[tdr]=(short int)b*8;
	SHighTrash[tdr]=(short int)c*8;
	KLowTrash[tdr]=(short int)d*8;
	KHighTrash[tdr]=(short int)e*8;
	SigmaRowTrash[tdr]=(short int)f;
	OccupancyThresh[tdr]=(short int)g;
	//			printf("SLowTrash: %hx, SHighTrash: %hx, KLowTrash: %hx, KHighTrash: %hx, SigmaRowTrash: %hx, OccupancyThresh: %hx\n",
	//						SLowTrash[tdr], SHighTrash[tdr], KLowTrash[tdr], KHighTrash[tdr], SigmaRowTrash[tdr], OccupancyThresh[tdr]);//only for debug
	refmask = inutile;
	//		printf("Tdr number %d : %d, %d; refmask : %hx\n", a, tmp, mode[tdr], refmask);//only for debug    
	if(tmp) refmask|=1<<tdr;
	tmp=0;
      }
      
      LPRINTF("TDR program: 0x%04x\n", TDRflash);
      LPRINTF("JINF program: 0x%04x\n", JINFflash);
      
    }
    
    else PRINTF("The configuration file isn't named with 'JLV1' or 'JINF'\n");
  }
  
  fclose(file);
  return ret;
}

//--------------------------------------------------------------
JinjSlave::JinjSlave(char* name,char* conf_file,int address,AMSWcom* node_in){
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
  jinfevents[0]=0;
  jinfevents[1]=0;
  for (int ii=0;ii<NTDRS;ii++)tdrevents[0][ii]=0;
  for (int ii=0;ii<NTDRS;ii++)tdrevents[1][ii]=0;
}
//--------------------------------------------------------------
JinjSlave::~JinjSlave(){
  
  if(CPars) delete CPars;
}
