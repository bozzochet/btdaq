
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "trcaldb.h"
#include "TFile.h"

typedef  unsigned short int ushort;
char rrtype[2][10]={"Request","Reply"};
char rwtype[2][20]={"Write","Write and Read"};

class Block_ph{
public:
  Block_ph(){Clear();}
  void Clear(){bsize=0;node=0;type=0;rrflag=-1;rwflag=-1; nwords=0; status=0; tag=0;time=0;}
  int bsize;
  int node;
  int type;
  int rrflag;
  int rwflag;
  int nwords;
  int status;
  int tag;
  time_t time;
  void Print(){
    printf("Size: %5d Type:  %d| %s| %s|\nNode: %4d ",
	   bsize,type,rrtype[rrflag],rwtype[rwflag],node);
    if(time) printf("Status: %d Tag: %d Time: %s ",status,tag,ctime(&time));
    else  printf("\n");
  }
};

Block_ph bph;

CaloutDSP cal;

TrCalDB* caldb;

int readBlockFirstHeader(FILE * rawfile,int second=1);


int readOneWord(ushort* size,FILE* rawfile);
int readJMDCSubBlock(FILE * rawfile);
int readDetSubBlock(FILE * rawfile);


int readsize;
int main(int argc, char ** argv){ 
  
  
   char fname[120]; 
   if(argc<2) { 
     printf("Usage: \n \t %s <filename> \n",argv[0]); 
     exit(1); 
       
       }
   
   sprintf(fname,"%s",argv[1]); 
   
   //int readcalibration(char *fname){

  // Create and Init the Tracker Database
  TKDBc::CreateTKDBc();
  TKDBc::Head->init();

  // Open the output root file
  TFile f("CalDB.root","RECREATE");

  //Create a Calibration database
  caldb= new TrCalDB();
  caldb->init();


  

  FILE * rawfile= fopen(fname,"r");
 
  ushort size;
  int aa;
  int realsize;
  int count=-1;
   while(!feof(rawfile)){
     // while(count<10){
    count++;
     printf("-----------------------------------------------------\n");
//     printf("---------        BLOCK #%d   ------------------------\n",count);
//     printf("-----------------------------------------------------\n");
    readsize=0;
      
//     printf("\nDumping the Det SubBlock\n");
    
//     if(!readOneWord(&size,rawfile)) return -1; readsize++;
//     int bsize=(size&0x7fff)/2;
//     printf(" Size: %d\n",bsize);
    
//     if(!readOneWord(&size,rawfile)) return -1;  readsize++;
//     if(size>>15) printf("Block type is Reply\n");
//     else printf("Block type is Request\n");
    
//     if(size>>14&1) printf("Write only\n");
//     else printf("Write an then Read\n");
//     int nodenum=size>>5 & 0x1FF;
//     int type = size & 0x1F;
//     printf("Node: %6d  Type:  %d\n",nodenum,type);
//     if(bsize==4109)   
//       readDetSubBlock(rawfile,nodenum); 	
//   else{
    printf("Block %3d ",count);
    aa=readBlockFirstHeader(rawfile);
    if(aa<0) break;
    bph.Print();
    printf ("aa %d  %d\n",aa,bph.bsize);
    readJMDCSubBlock(rawfile);    
    if(bph.node>281&& bph.node<474)
      readDetSubBlock(rawfile);
    else{
      printf("Read %d to skip this block  \n",(bph.bsize-aa));
      for (int ii=0;ii<(bph.bsize-aa);ii++)
	if(!readOneWord(&size,rawfile)) return -1;  readsize++;
    }
    
    
    if(feof(rawfile)) break;
    printf("\n");
  }
  fclose(rawfile);
  caldb->Write();
  f.Close();
  return 0;

}



int readBlockFirstHeader(FILE * rawfile,int second){
  ushort size;
  ushort size2;
  int outsize=0;
  bph.nwords=0;
  //printf("Dumping the Primary Header\n"); 
  if(!readOneWord(&size,rawfile)) return -1; readsize++;
  bph.bsize=size;
  if(size>>15&1){
    if(!readOneWord(&size,rawfile)) return -1; readsize++;
    bph.bsize=bph.bsize<<16; 
    bph.bsize+=size;
  }
  bph.bsize/=2;
  
  if(!readOneWord(&size,rawfile)) return -1; readsize++;
  outsize++;
  if(size>>15) bph.rrflag=1;
  else         bph.rrflag=0;
  
  if(size>>14&1) bph.rwflag=1;
  else           bph.rwflag=1;

  bph.node=size>>5 & 0x1FF;
  bph.type = size & 0x1F;
  if (bph.type == 0x1F) {
    if(!readOneWord(&size,rawfile)) return -1;  readsize++;
    bph.type= size;
    outsize++;
  }
  if(!second) return outsize;
  //printf("Dumping the Secondary Header\n");
  
  if(!readOneWord(&size,rawfile)) return -1;  readsize++;
  outsize++;

  bph.status=(size>>12)&0xf;
  bph.tag=size&0xfff;

  if(!readOneWord(&size,rawfile)) return -1; readsize++;
  outsize++;
  bph.time=size;
  bph.time=bph.time<<16;
  if(!readOneWord(&size,rawfile)) return -1; readsize++;
  outsize++;
  bph.time+=size;
  
  return     outsize;
} 


int readOneWord(ushort* sizeout,FILE* rawfile){
  int byteswap=0;
  int debug=0;
  ushort size;

  fread(&size,sizeof(size),1,rawfile);
  if(ferror(rawfile)) {printf("Error reading \n"); return 0;}
  if(feof(rawfile)) {printf("End of file \n"); return 0;}
  if(byteswap){
    
    ushort tmp=size;
    size=0;
    
    size=tmp>>8|tmp<<8;

    //  if (debug)printf("read: %d  %hX  swapped: %d  %hX \n",tmp,tmp,size,size);
  }
  


  if (debug)printf("pos: %ld  read: %d  %hX\n",ftell(rawfile)/2,size,size);
  *sizeout=size;
  
  return 1;
}





int readJMDCSubBlock(FILE * rawfile){
  ushort size;
  ushort size2;
  
  printf("Dumping the JMDC SubBlock\n");

  if(!readOneWord(&size,rawfile)) return -1; readsize++;
  printf("JMDC Size: %d\n",size&0x7fff);

  
  if(!readOneWord(&size,rawfile)) return -1;  readsize++;

  
  if(size>>15) printf("Block type is Reply\n");
  else printf("Block type is Request\n");
  
  if(size>>14&1) printf("Write only\n");
  else printf("Write an then Read\n");
  int nodenum=size>>5 & 0x1FF;
  int type = size & 0x1F;
  printf("Node: %6d  Type:  %d\n",nodenum,type);

  if(!readOneWord(&size,rawfile)) return -1;  readsize++;
  if(!readOneWord(&size2,rawfile)) return -1;   readsize++;
  
  int runnum=size<<16;
  runnum+=size2;


  if(!readOneWord(&size,rawfile)) return -1;   readsize++;
  if(!readOneWord(&size2,rawfile)) return -1;   readsize++;
  
  int runtag=size<<16;
  runtag+=size2;

  if(!readOneWord(&size,rawfile)) return -1;   readsize++;
  if(!readOneWord(&size2,rawfile)) return -1;   readsize++;
  
  int evnum=size<<16;
  evnum+=size2;
  printf("Run:  %d RunTag: %d Event: %d \n",runnum,runtag,evnum); 

  if(!readOneWord(&size,rawfile)) return -1;   readsize++;
  if(!readOneWord(&size2,rawfile)) return -1;   readsize++;
  
  int evtime=size<<16;
  evtime+=size2;
  
  time_t time2=evtime;
  printf("Time: %d  %s\n",evtime,ctime(&time2));


  if(!readOneWord(&size,rawfile)) return -1;  readsize++;
  if(!readOneWord(&size2,rawfile)) return -1; readsize++;
  
  int finetime=size<<16;
  finetime+=size2;
  
  printf("FineTime: %d  \n",finetime);
  
  if(!readOneWord(&size,rawfile)) return -1;
  if(size>>15){
    printf("GPS data present and skipped ..\n");
    if(!readOneWord(&size,rawfile)) return -1;  readsize++;
    if(!readOneWord(&size,rawfile)) return -1; readsize++;
    if(!readOneWord(&size,rawfile)) return -1; readsize++;
    if(!readOneWord(&size,rawfile)) return -1; readsize++;
    if(!readOneWord(&size,rawfile)) return -1; readsize++;
  }    
  return 0;
}


int readDetSubBlock(FILE * rawfile){
  ushort size;

  if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
  int cpar=size;
  printf("Command parameter is: %hX \n",size); 

  if(cpar&1){ //pedestals
    printf("Reading Pedestals\n");
    for (int ii=0;ii<1024;ii++){
      if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
      cal.ped[ii]=size;
    }
  }

  if(cpar&2){ //flags
    printf("Reading flags\n");
    for (int ii=0;ii<1024;ii++){
      if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
      cal.status[ii]=size;
    }
  }

  if(cpar&4){ //sigma
    printf("Reading Sigmas\n");
    for (int ii=0;ii<1024;ii++){
      if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
      cal.sig[ii]=size;
    }
  }

  if(cpar&8){ //rawsigma
    printf("Reading Raw Sigmas\n");
    for (int ii=0;ii<1024;ii++){
      if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
      cal.rsig[ii]=size;
    }
  }
 
  if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
  printf("Detector DSP version %hX\n",size);
  
  if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
  printf("Lowsigma factor %d\n",size);
  cal.lowthres=size;
  
  if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
  printf("Highsigma factor %d\n",size);
  cal.highthres=size;
  
  if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
  printf("sigmaraw factor %d\n",size);
  cal.sigrawthres=size;
  
  if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
  printf("Calibration Events %d\n",size);
  cal.calnum=size;
  
  
  if(!readOneWord(&size,rawfile)) return -1;  readsize++; 
  printf("Calibration status %d\n",size);
  cal.calstatus=size;
  
  
  //Fill the Calibration object
  TrLadCal* ladcal=caldb->FindCal_JMDC(bph.node);
  if(ladcal){
    ladcal->Fill(&cal);
    ladcal->PrintInfo();
  }else
    printf("-------------------> WARNIG I CANT FIND The calibration object to be filled\n");

  // skip the last two words
  if(!readOneWord(&size,rawfile)) return -1; readsize++;
  
  printf("Pos: %ld  Read:    %hX  %d\n",ftell(rawfile)/2, size&0x1f,size);

   if(!readOneWord(&size,rawfile)) return -1; readsize++;
  
  printf("Pos: %ld  Read:     %hX  %d\n",ftell(rawfile)/2, size&0x1f,size);
 
 
  
  return 1;
}
