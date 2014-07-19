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

char* DaqClass::GetProcEvents(char * chout){

  if(!chout) return NULL;
  char answer[255];
   char funame[255];
   int ret;
   char* ff;
   char **endptr;
   int count,njinj;
   int Jinfnum1=0,Jinfcount1=0;
   int Jinfnum2=0,Jinfcount2=0;
   int tdnum[24];
   int tdcount[24];
   memset(tdnum,0,24*sizeof(tdnum[0]));
   memset(tdcount,0,24*sizeof(tdcount[0]));
   sprintf(funame,"GetProcEvents");
    
   ret=SendCommand("GETEVTNUM\n",answer,10);
   if(ret<0) {
     if(pri) printf(" %s problem sending the command\n",funame);
     return NULL;
   }
   if(strstr(answer,"ERROR")) return NULL;
   int ii=0;
   printf("answer is %s \n",answer);
   sscanf(answer," %d ||     %d  |%02d      %d |%04d      %d |%04d      %d |%04d      %d |%04d      %d |%02d      %d |%04d      %d |%04d      %d",&count,&njinj,&Jinfnum1,&Jinfcount1,&tdnum[0],&tdcount[0],&tdnum[1],&tdcount[1],&tdnum[2],&tdcount[2],&tdnum[3],&tdcount[3],&Jinfnum2,&Jinfcount2,&tdnum[4],&tdcount[4],&tdnum[5],&tdcount[5]);
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

   // printf("%s  DaqEvents: %d Jinj: %d ",DaqName,nevents_count,nevents_Jinj);
//    for (int ii=1;ii<ntot_tdrs;ii++)
//      printf(" Jinf%02d: %d, ",jinfnum[ii],nevents_Jinf[ii]);

//    for (int ii=1;ii<ntot_tdrs;ii++)
//      printf(" TRD%02d: %d, ",tdrnum[ii],nevents[ii]);
//    printf("\n");

   sprintf(chout,"%s NI Count: %d Jinj: %d \n",DaqName,count,njinj);
   sprintf(chout,"%s Jinf%02d: %d, \n Jinf%02d: %d, \n",chout,Jinfnum1,Jinfcount1,Jinfnum2,Jinfcount2);
   
   for (int ii=0;ii<6;ii++)
     sprintf(chout,"%s  TRD%02d: %d, \n",chout,tdnum[ii],tdcount[ii]);

   return chout;
  
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

  sprintf(command,"CALIBRATE %d",run);
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
int DaqClass::NewLog(float timeout){
  char answer[255];
  int ret=0;
  char *ff;
  ret=SendCommand("NEWLOG",answer,timeout);
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
