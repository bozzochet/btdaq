#include "Calib.hh"
#include <iostream>
#include <sys/stat.h>
#include <stdlib.h>


//=============================================================================================
Calib::Calib(int run){

  RunNum=run;
  sprintf(caldir,"./CalData/");

}

//=============================================================================================

int Calib::Calpos(int tdrnum, RHClass* rh){

  for (int ii=0;ii<rh->ntdrCmp;ii++)
    if(rh->tdrCmpMap[ii]==tdrnum)  return ii;
  printf("DecodeData::FindCalPos:  Fatal Error can't find position for TDR %d\n",tdrnum);
  exit(4);
}

//=============================================================================================

void Calib::RepererCalibs(RHClass *rh){

//   char name1[300];
//   FILE* calfile[TDRNUM];
  
//   if(rh->ntdrCmp<1){
//     printf("No TDR in CMP mode --> No DSP Calib can be found\n");
//     return;
//   }

//   for (int ii=0;ii<rh->ntdrCmp;ii++){
//     sprintf(name1,"%s_%04d.cal",caldir,rh->tdrCmpMap[ii]);
//     calfile[ii]=fopen(name1,"r");
//     if(!calfile[ii]){
//       printf("Cannot find the calib %s for the requested calib run %d\n",name1);
//       printf("I give up. Bye.\n");
//       exit(2);
//     }
//     printf("lire fichier de calibration %s\n",name1);
    
//     LireCalib(calfile[ii],&(cals[ii]),ii);
   
//   }
//   return;

  int pri=1;//printing debug infos

  struct stat buf;
  char name1[300];
  int run2;
  FILE* calfile[TDRNUM];
  
  if(rh->ntdrCmp<1){
    printf("No TDR in CMP mode --> No DSP Calib can be found\n");
    return;
  }

  
  for (run2=RunNum ;run2>0 ;run2--){
    sprintf(name1,"%s/%06d_%04d.cal",caldir,run2,rh->tdrCmpMap[0]);
    if (pri) printf("searching for %s \n",name1);
    if(stat(name1,&buf)==0){
      printf("First calib Found %s run %d\n",name1,run2);
      break;
    }
  }
  if( run2<40) {
    printf("Cannot find any calibration done before the requested run %d\n",RunNum);
    printf("I give up. Bye.\n");
    exit(2);
  }else 
    printf ("Searching for other calib files\n");
  

  for (int ii=0;ii<rh->ntdrCmp;ii++){
    sprintf(name1,"%s/%06d_%04d.cal",caldir,run2,rh->tdrCmpMap[ii]);
    calfile[ii]=fopen(name1,"r");
    if(!calfile[ii]){
      printf("Cannot find the calib %s for the requested calib run %d\n",name1,run2);
      printf("I give up. Bye.\n");
      exit(2);
    }
    printf("Found file %s\n",name1);
    
    LireCalib(calfile[ii],&(cals[ii]), ii); 
  }
  
}
//=============================================================================================

int  Calib::LireCalib(FILE* fil, calib* cal, int ntdr){
  int a,b,c;
  float d,e;
  for (int ii=0;ii<16;ii++)
    fscanf(fil,"%d, %f, %f",&a,&d,&e);

  for (int ii=0;ii<1024;ii++){
    fscanf(fil,"%d  %d  %d  %f %f %f %f %d  ",&a,&b,&c,
	   &(cal->ped[ii]),
	   &(cal->rsig[ii]),
	   &(cal->sig[ii]),
	   &d,
	   &(cal->status[ii]));
    //    printf("%d  %f %f %f %d\n",ii,cal->ped[ii],cal->rsig[ii],cal->sig[ii],cal->status[ii]);
    //    printf("retpied %f\n",ret_piedestal(ntdr,ii));
  }
  return 1;
}
