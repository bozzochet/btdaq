#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "DaqClass.hh"

DaqClass::DaqClass(char *name,char* address,int port):CommClass(address,port)
{
  sprintf(DaqName,"%s",name);
  for (int ii=0;ii<25;ii++){
    nevents[ii]=0;
    tdrnum[ii]=0;
    jinfnum[ii]=0;
    nevents_Jinf[ii]=0;
  }
  ntot_tdrs=0;
  nevents_Jinj=0;;
  nevents_count=0;;
}
//---------------------------------------------------------------------------

int DaqClass::GetProcEvents(){

   char answer[255];
   char funame[255];
   int ret;
   char* ff;
   char **endptr;
   sprintf(funame,"GetProcEvents");
    
   ret=SendCommand("GETRUNSUM\n",answer,10);
   if(ret<0) {
     if(pri) printf(" %s problem sending the command\n",funame);
     return -2;
   }
   if(strstr(answer,"ERROR")) return -1;
   int ii=0;
   
   ff=strtok(answer," ");
   endptr=NULL; 
   nevents_count=atoi(ff);

   ff=strtok(NULL," ");
   nevents_Jinj=atoi(ff);
   for (int ii=0;ii<ntot_jinf;ii++){
     jinfnum[ii]=atoi(ff);
     ff=strtok(NULL," ");
     nevents_Jinf[ii]=atoi(ff);
   }
   while(ff=strtok(NULL," ")){
     ii++;  
     tdrnum[ii]=atoi(ff);
     ff=strtok(NULL," ");
     nevents[ii]=atoi(ff);
   }
   ntot_tdrs=ii;

   printf("%s  DaqEvents: %d Jinj: %d ",DaqName,nevents_count,nevents_Jinj);
   for (int ii=1;ii<ntot_tdrs;ii++)
     printf(" Jinf%02d: %d, ",jinfnum[ii],nevents_Jinf[ii]);

   for (int ii=1;ii<ntot_tdrs;ii++)
     printf(" TRD%02d: %d, ",tdrnum[ii],nevents[ii]);
   printf("\n");
   return nevents[0];
  
}
   
//---------------------------------------------------------------------------


int DaqClass::IsDaqRunning(){

  char command[255];
  
  sprintf(command,"STATUS");
  return SendCommandWithReply("STATUS","BUSY","OK","IsDaqRunning",3);
  
}

//---------------------------------------------------------------------------

int DaqClass::Init(float timeout){

  return SendCommandWithReply("INIT","READY","BUSY","Init",timeout);
  
}


//---------------------------------------------------------------------------
int DaqClass::CalibRaw(int run,float timeout){

  char command[255];

  sprintf(command,"CALIBRAW %d",run);
  return SendCommandWithReply(command,"events acquired","BUSY","Calibrate",timeout);
  
}

//---------------------------------------------------------------------------
int DaqClass::Calibrate(int run,float timeout){

  char command[255];

  sprintf(command,"CALIBRATE %d ",run);
  return SendCommandWithReply(command,"READY","BUSY","Calibrate",timeout);
  
}

//---------------------------------------------------------------------------
int DaqClass::StartDaq(int run,int maxev,GonClass* gon,float timeout){

  char command[255];
  //  Gon->GetPos();
  if(gon)
    sprintf(command,"START %d %d %f %f %f %f",run,maxev,gon->ReadAlpha(),gon->ReadCraddle(),gon->ReadLin1(),gon->ReadLin2());
  else 
    sprintf(command,"START %d %d ",run,maxev);
  //  sprintf(command,"START %d ",run);
  return SendCommandWithReply(command,"READY","BUSY","StartDaq",timeout);
  
}

//---------------------------------------------------------------------------
int DaqClass::StopDaq(float timeout){
  char answer[255];
  int ret=0;
  char *ff;
  ret=SendCommand("stop",answer,timeout);
  printf("%s \n",answer);
//   if(){
//     ff=strtok(answer," ");
//     endptr=NULL; 
//     nevent_tot=atoi(ff);
//   }

  return 0;
  
  
}

//---------------------------------------------------------------------------
int DaqClass::KillDaq(float timeout){

  return SendCommandWithReply("KILL","OK","ERROR","KillDaq",timeout);
}
