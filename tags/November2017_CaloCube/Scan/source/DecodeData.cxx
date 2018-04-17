#include "DecodeData.hh"
#include <iostream>
#include <sys/stat.h>


using namespace std;

extern char progname[50];


// "DATA bit, set by master when assembling group reply"
  // "END bit / CRC error (if DATA=1), set by master"
  //
  // 
  // 
  // 
  // 
  // 
  // 
  // 
char errmess[16][80]={"","","","","","replying node is CDP",
		      " RAW events",
		      " COMPRESSED events",
		      " cumulative status",
		      "build errors (sequencer errors for event building",
		      "build conditions (FE power fault for event building",
		      "NEXT bit / timeout (if DATA=1), set by master"	      ,
		      "ERROR bit / AMSW error (if DATA=1), set by master",
		      "ABORT bit / assembly error (if DATA=1), set by master",
		      "END bit / CRC error (if DATA=1), set by master",
		      "DATA bit, set by master when assembling group reply"};

//=============================================================================================
DecodeData::DecodeData( char * ifname,char* caldir,int run){

  runn=run;
  ntdrRaw=0;
  memset(tdrRaw,-1,TDRNUM*sizeof(tdrRaw[0]));
  ntdrCmp=0;
  memset(tdrCmp,-1,TDRNUM*sizeof(tdrCmp[0]));
  pri=0;
  evpri=0;
  sprintf(type,"JinJ");
  
  sprintf(rawname,"%s/%06d_%04d.dat",ifname,runn,0);
  sprintf(rawdir,"%s",ifname);
  sprintf(rawCaldir,"%s",caldir);

  rawfile = fopen(rawname,"r");
  if (rawfile == NULL) {printf ("Error file %s not found \n",rawname);exit(2);}


  tdroffset=0;
  evenum=0;
  runn=run;

  // Create the ROOT run header
  rh= new RHClass();


  //Read the run Header

  DumpRunHeader();

  if(runn!=rh->Run){
    printf("===========================================================\n");
    printf("DecodeData: Something strange is happening\n");
    printf("            File nameis : %s/run_%05d.dat\n",ifname,runn);
    printf("            But RunHeader says the run is: %d \n\n",rh->Run);
    printf("            I WILL STOP NOW!!!! \n");
    printf("===========================================================\n");    
  }
  
  // Try to get some info on how many ladders there are in the data (RAW and CMP)
  long int pos=ftell(rawfile);
  out_flag=0;
  ReadOneEvent();
  // come back to the previous pos
  if(int ret0=fseek(rawfile,pos,SEEK_SET)){
    printf("Fatal: error during file skip  %d\n",ret0);
    exit(3);
  }
  out_flag=1;

  //  // Create the ROOT Classes for the ROOT data format
  ev= new Event(ntdrRaw);  


  mysort(tdrRaw,ntdrRaw);
  mysort(tdrCmp,ntdrCmp);
   // Update the ROOT run header
  rh->nJinf=nJinf;
  for (int ii=0;ii<nJinf;ii++)
    rh->JinfMap[ii]=JinfMap[ii];

  rh->ntdrRaw=ntdrRaw;
  for (int ii=0;ii<ntdrRaw;ii++)
    rh->tdrRawMap[ii]=tdrRaw[ii];
  
  rh->ntdrCmp=ntdrCmp;
  for (int ii=0;ii<ntdrCmp;ii++)
    rh->tdrCmpMap[ii]=tdrCmp[ii];
  
  rh->Print();
  
  //Find the Calibrations

  FindCalibs ();
}
//=============================================================================================

DecodeData::~DecodeData(){
  if(pri)  cout<< "in the destructor"<<endl;
  if(rawfile) CloseFile();
  if (ev) delete ev;
  if(pri)cout<< " destroyed"<<endl;
}
//=============================================================================================

int DecodeData::FindPos(int tdrnum){

  for (int ii=0;ii<ntdrCmp;ii++)
    if(tdrCmp[ii]==tdrnum)  return ii;
  printf("DecodeData::FindPos:  Fatal Error can't find position for TDR %d\n",tdrnum);
  exit(4);
}
//=============================================================================================

int DecodeData::FindPosRaw(int tdrnum){

  for (int ii=0;ii<ntdrRaw;ii++)
    if(tdrRaw[ii]==tdrnum)  return ii;
  printf("DecodeData::FindRawPos:  Fatal Error can't find position for TDR %d\n",tdrnum);
  exit(4);
}
//=============================================================================================

int DecodeData::FindCalPos(int tdrnum){

  for (int ii=0;ii<ntdrCmp;ii++)
    if(tdrCmp[ii]==tdrnum)  return ii;
  printf("DecodeData::FindCalPos:  Fatal Error can't find position for TDR %d\n",tdrnum);
  exit(4);
}
//=============================================================================================


void DecodeData::CloseFile(){
  if(rawfile) fclose(rawfile);
  rawfile=NULL;
  return;
}
//=============================================================================================

int DecodeData::EndOfFile(){
  int eff=0;
  if(rawfile) eff= feof(rawfile);
  else {printf("Error file pointer is NULL \n");exit(3);}
  
  return eff;
}

//=============================================================================================

void DecodeData::DumpRunHeader(){
  unsigned short int size;  
  unsigned short int dummy;  


  // For the moment just skip the event header


  header hh;
  //Read The Header Size
 //  if(pri) printf("%ld  ",ftell(rawfile)/2);
  ReadFile(&size,sizeof(size),1,rawfile);

  if(pri) printf("Headersize    %d \n",size);
  
 //  for (int ii=0;ii<size;ii++){
//     ReadFile(&dummy,sizeof(dummy),1,rawfile);
//     if(pri) printf("pos:%ld  ",ftell(rawfile)/2);
//     if(pri) printf("   %d \n",dummy);
//   }
   
   ReadFile(&hh,sizeof(hh),1,rawfile);
  
   printf("run: %d   date: %s\n",hh.run,hh.date);
  
   rh->Run=hh.run;
   sprintf(rh->date,"%s",hh.date);
//   rh->Time=hh.time;
//   sprintf(rh->Trigger,"%s",hh.trigger);
//   sprintf(rh->SysName,"%s",hh.systemname);
//   char gg[30];
//   char ff[30];
//   for (int ii=0;ii<3;ii++)rh->GonPar[ii]=hh.gonpar[ii];
//   printf("Run: %d date: %s   hour:%s \n",hh.run,rh->GetDate(gg),rh->GetTime(ff));
//   printf("angle: %f lin1: %f lin2: %f \n",hh.gonpar[1],hh.gonpar[0],hh.gonpar[2]);
//   printf("Trigger typye: %s     Datastream: %s   \n",hh.trigger,hh.systemname);
//   if(pri) printf("%ld  \n",ftell(rawfile)/2);

}

//=============================================================================================

int DecodeData::SkipOneEvent(int evskip){
  unsigned short int size;
  unsigned short int junk;

  printf (" Run %d SKIPPING %d events on DAQ %s\n",runn,evskip,type);
  for (int kk=0;kk<evskip;kk++){
    //Read The Event Size
    if(pri) printf("pos:%ld  ",ftell(rawfile)/2);
    ReadFile(&size,sizeof(size),1,rawfile);

    if(pri) printf("%s size    %d \n",type,size);
    for (int ii=0;ii<size;ii++)  {
      ReadFile(&junk,sizeof(junk),1,rawfile);
      if(pri)printf("%d:  %d\n",ii, junk);
      //printf("pos:%ld  ",ftell(rawfile)/2);
    }
  }
  return 0;
}

//=============================================================================================

int DecodeData::ReadOneEvent(){
  unsigned short int size;
  unsigned short int status;
  unsigned short int num;
  if(!out_flag){ntdrRaw=0;ntdrCmp=0;nJinf=0;}

  //Read The Event Size
  if(pri) printf("pos:%ld  ",ftell(rawfile)/2);
  
  ReadFile(&size,sizeof(size),1,rawfile);

  long int base=ftell(rawfile)/2;

  //update the event counters
  evenum++;

  if(out_flag) ev->Evtnum=evenum;


  if(pri) printf("pos:%ld  \n",ftell(rawfile)/2);

  //Read the Event Number
  ReadFile(&num,sizeof(num),1,rawfile);

  if(pri) printf("Evenum    %d \n",num);
  if(out_flag)ev->Evtnum=num;
  if(pri||evpri)printf("\n -----------------------------> New event: %d   \n",num);

  if(pri||evpri)printf("%ssize    %d \n",type,size);
  
  //Store the position on the file
  long int pos=ftell(rawfile);
  
  // skip all the TDR data
  if(int ret0=fseek(rawfile,(size-2)*2,SEEK_CUR)){
    printf("Fatal: error during file skip ret0=%d \n",ret0);
    exit(3);
  }
  
  // read the last word of the event
  if(pri) printf("pos:%ld  ",ftell(rawfile)/2);
  ReadFile(&status,sizeof(status),1,rawfile);

  if(pri||evpri)printf("%s status    %d : address %02d  status ",type,status,status&0x1f); 
  
  int count=5;
  for (int dase=32;dase<0x8000;dase*=2)
    if(pri||evpri)printf(" %2d %d",count++,((status&dase)>0));
  if(pri||evpri)printf("\n");
 
  count=5;
  for (int dase=32;dase<0x8000;dase*=2){
    if((status&dase)>0)
      if(pri||evpri)printf("%02d: %s\n",count,errmess[count]);
    count++;
  }
  if(pri||evpri)printf("\n \n");

  if(out_flag)ev->JINJStatus=status;

  if((status&0x200)>0||(status&0x400)>0){
    printf("=======================> %s Error!!!  Skip this event: %d\n",type,ev->Evtnum); 
    count=5;
    for (int dase=32;dase<0x8000;dase*=2){
      if((status&dase)>0)
	printf("%02d: %s\n",count,errmess[count]);
      count++;
    }
   printf("Uscita da OneEvent\n");
    return 10;
  }

// come back to the previous pos
  if(int ret0=fseek(rawfile,pos,SEEK_SET)){
    printf("Fatal: error during file skip %d\n",ret0);
    exit(3);
  }

  int ret=0;
  
  // Read the TDRs or the JINFs
  if(!out_flag){ntdrRaw=0;ntdrCmp=0;}

  if(strcmp(type,"JinJ")==0)
    while ( ftell(rawfile)/2< base+size-1 )  {
      ret=ReadOneJINF();
      if (ret) {
        printf("Uscita da OneEvent (Jinf)\n");
	return ret;
      }
    }
  else
    while ( ftell(rawfile)/2< base+size-1 )  {
      ret=ReadOneTDR(0);
      if (ret==10) {
       printf("Uscita da OneEvent (Tdr)\n");
       return ret;
      }
    }

  // Reread the last word
  ReadFile(&status,sizeof(status),1,rawfile);

  if(pri||evpri)printf("%s status    %d \n ",type,status); 
  return 0;
}


//=============================================================================================


int DecodeData::ReadOneTDR(int Jinfnum){
  unsigned short int size;
  unsigned short int status;


    //read the size of the TDR events
  if(pri) printf("pos:%ld  ",ftell(rawfile)/2);
  ReadFile(&size,sizeof(size),1,rawfile);

  if(pri)printf("TDRsize    %d \n",size);
  
  // allocate memory for the TDR event and read the event
  short int *array=(short int*)calloc(size,sizeof(short int));
  if(!array) {printf("Fatal error cannot alloc memeory./n"); exit(3);}

  ReadFile(array,size*sizeof(short int),1,rawfile);
  
  //checking TDR status (spero...)
  status=array[size-1];

  if(pri||evpri)printf("TDR status    %d : address %02d  status ",status,status&0x1f); 

  int TDRnum=((unsigned int)status)&0x1f;


  int count=1;
  for (int base=32;base<0x8000;base*=2)
    if(pri||evpri)printf(" %2d %d",count++,((status&base)>0));
  if(pri||evpri)printf("\n");
 
  count=5;
  for (int base=32;base<0x8000;base*=2){
    if((status&base)>0)
      if(pri||evpri)printf("%02d: %s\n",count,errmess[count]);
    count++;
  }
  if(pri||evpri)printf("\n \n");

  if(out_flag)ev->TDRStatus[TDRnum]=status;
  
  if((status&0x200)>0||(status&0x400)>0){
    printf("=======================> TDR Error!!!  Skip this event: %d\n",ev->Evtnum);
    count=5;
    for (int base=32;base<0x8000;base*=2){
      if((status&base)>0)
	printf("%02d: %s\n",count,errmess[count]);
      count++;
    }
    printf("uscita da oneTDR\n");
    return 10;
  }
  
  //Decode the tdr number (the order of tdrs in the file may change event by event!!!)
  int tdrnum=-1;
  int numnum=((unsigned int)array[size-1])&0x1f;
  if(out_flag)tdrnum=FindPos(numnum+100*Jinfnum);

  if(pri)printf("==========> tdrnum %02d Jinf %02d  size %4d status  %d ",numnum,Jinfnum,size,array[size-1]);
  if(!out_flag)if((array[size-1]&64)>0)  tdrRaw[ntdrRaw++]=numnum+100*Jinfnum;
  if(!out_flag)if((array[size-1]&128)>0) tdrCmp[ntdrCmp++]=numnum+100*Jinfnum;

  /*int*/ count=5;
  for (int base=32;base<0x8000;base*=2)
    if(pri||evpri)printf(" %2d %d",count++,((array[size-1]&base)>0));
  if(pri||evpri)printf("\n");
  
  count=5;
  for (int base=32;base<0x8000;base*=2){
    if((array[size-1]&base)>0)
      if(pri)printf("%02d: %s \n ",count,errmess[count]);
    count++;
  }
  if(pri)printf("\n ");
  // if(pri)for(int ii=0;ii<size;ii++) printf("arr: %d =  %d\n",ii,array[ii]);
  
  // - bit 15 { DATA bit, set by master when assembling group reply;
  // - bit 14 { END bit / CRC error (if DATA=1), set by master;
  // - bit 13 { ABORT bit / assembly error (if DATA=1), set by master;
  // - bit 12 { ERROR bit / AMSW error (if DATA=1), set by master;
  // - bit 11 { NEXT bit / timeout (if DATA=1), set by master;
  // - bit 10 { build conditions (FE power fault for event building);
  // - bit 9 { build errors (sequencer errors for event building);
  // - bit 8 { cumulative node status;
  // - bit 7 { if the node is building COMPRESSED events;
  // - bit 6 { if the node is building RAW events;
  // - bit 5 { if replying node is CDP;
  // - bits 4-0 { slave ID, set by master;
  
  if(out_flag)ev->TDRStatus[tdrnum]=array[size-1];
  if(out_flag)ev->ReadTDR[tdrnum]=1;
  
  int RawOffset=0;
  if (TESTBIT(array[size-1],6) ){
    RawOffset=1024;   // RAW data present
    if(pri)printf("  RAW data present   ");
    int tdrnumraw=0;
    int count=0;
    if(out_flag){
      tdrnumraw=FindPosRaw(numnum+100*Jinfnum);
      for (int kk=0;kk<320;kk++){
	ev->Signal[tdrnumraw][kk]=array[count];
	ev->Signal[tdrnumraw][320+kk]=array[count+1];
	ev->Signal[tdrnumraw][640+kk]=array[count+2];
	//	printf("RAW %d %d  %d\n",kk,ev->Signal[FindTDRPos(tdrnum)][kk],array[count]);
	count+=3;
       }
      for (int kk=960;kk<1024;kk++){
	ev->Signal[tdrnumraw][kk]=array[kk];
      }
    }
    
  }  
  
  

  
  if (TESTBIT(array[size-1],7)){    // Compressed data present
    if(pri)printf("  Compressed data present \n");
    //dump clusters
    int  count=RawOffset;
     while (count<(size-17)){
      int bad=0;
      int cluslen=array[count++]+1;
      int clusadd=array[count++];
      float sig[MAXLENGHT];
      if(pri)printf("Cluster: add: %d  lenght: %d\n",clusadd,cluslen);
      for(int hh=0;hh<cluslen;hh++){
	if(pri)printf("signal: %d %d\n",hh,array[count]);
	if(hh<MAXLENGHT){
	  sig[hh]=array[count]/8.;
	  if(pri)printf("        %d %f\n",hh,sig[hh]);
	}
	else bad=1;
	count++;
      }
      
      int sid=0;
      if(clusadd>640) sid=1;
      if(out_flag){
	Cluster* pp= ev->AddCluster(numnum+100*Jinfnum,sid);
	calib* cal=&(cals[tdrnum]);
 	pp->Build(numnum+100*Jinfnum,sid,clusadd,cluslen,sig,&(cal->sig[clusadd]),
 		  &(cal->status[clusadd]),bad);
	if(pri)pp->Print();
      }
    }
     
     int cc=1;
     //Read the  cnoise values
     for (int ii=size-17;ii<size-1;ii++){
       if(pri)printf("Cnoise %d:  %f \n",cc++,(array[ii])/8.);
       if(out_flag)ev->CNoise[tdrnum][ii-size+17]=(array[ii])/8.;
     }
  }
  if(pri)printf("\n");
  if(pri) printf("End op the TDR pos:%ld  \n",ftell(rawfile)/2);
  //free the memory for the next tdr
  free(array);
  
  
  return 0;
}


//=============================================================================================

void DecodeData::FindCalibs (){
  struct stat buf;
  char name1[300];
  int run2;
  FILE* calfile[TDRNUM];
  int old=pri;
  
  if(ntdrCmp<1){
    printf("No TDR in CMP mode --> No DSP Calib can be found\n");
    return;
  }
  
  for ( run2=runn;run2>0 ;run2--){
    sprintf(name1,"%s/%06d_%04d.cal",rawCaldir,run2,tdrCmp[0]);
    printf("searching for %s \n",name1);
    if(stat(name1,&buf)==0){
      printf("First calib Found %s run %d\n",name1,run2);
      break;
    }
  }
  if( run2<40) {
    printf("Cannot find any calibration done before the requested run %d\n",runn);
    printf("I give up. Bye.\n");
    exit(2);
  }else 
    printf ("Searching for other calib files\n");
  

  for (int ii=0;ii<ntdrCmp;ii++){
    int Jinfnum=tdrCmp[ii]/100;
    int tdrnum=tdrCmp[ii]-Jinfnum*100;
    sprintf(name1,"%s/%06d_%02d%02d.cal",rawCaldir,run2,Jinfnum,tdrnum);
    calfile[ii]=fopen(name1,"r");
    if(!calfile[ii]){
      printf("Cannot find the calib %s for the requested calib run %d\n",name1,run2);
      printf("I give up. Bye.\n");
      exit(2);
    }
    printf("Found file %s\n",name1);
    
    ReadCalib(calfile[ii],&(cals[ii]));
   
  }
  
 
  pri=old;
}

//=============================================================================================

int  DecodeData::ReadCalib(FILE * fil,calib* cal){
  int a,b,c;
  float d,e;
  int old=1;
  old=pri;
  pri=0;
  for (int ii=0;ii<16;ii++)
    fscanf(fil,"%d, %f, %f",&a,&d,&e);

  for (int ii=0;ii<1024;ii++){
    fscanf(fil,"%d  %d  %d  %f %f %f %f %d  ",&a,&b,&c,
	   &(cal->ped[ii]),
	   &(cal->rsig[ii]),
	   &(cal->sig[ii]),
	   &d,
	   &(cal->status[ii]));
    if(pri)printf("%d  %f %f %f %d\n",ii,cal->ped[ii],cal->rsig[ii],cal->sig[ii],cal->status[ii]);
  }
  pri=old;
  return 1;
}

//=============================================================================================



int DecodeData::ReadOneJINF(){
  unsigned short int size;
  unsigned short int status;

  int old=1;
  old=pri;
    
 

  //Read The Event Size
  if(pri) printf("pos:%ld  ",ftell(rawfile)/2);
  ReadFile(&size,sizeof(size),1,rawfile);
  if(pri)printf("JINFsize    %d \n",size);

  long int base=ftell(rawfile)/2;

  if(pri) printf("pos:%ld  \n",ftell(rawfile)/2);


  //Store the position on the file
  long int pos=ftell(rawfile);
  
  // skip all the TDR data
  if(int ret0=fseek(rawfile,(size-1)*2,SEEK_CUR)){
    printf("Fatal: error during file skip ret0=%d \n",ret0);
    exit(3);
  }
  
  // read the last word of the JINF event
  if(pri) printf("pos:%ld  ",ftell(rawfile)/2);

  ReadFile(&status,sizeof(status),1,rawfile);

  if(pri||evpri)printf("Jinf status    %d : address %02d  status ",status,status&0x1f); 

  int Jinfnum=((unsigned int)status)&0x1f;


  int count=1;
  for (int base=32;base<0x8000;base*=2)
    if(pri||evpri)printf(" %2d %d",count++,((status&base)>0));
  if(pri||evpri)printf("\n");
 
  count=5;
  for (int base=32;base<0x8000;base*=2){
    if((status&base)>0)
      if(pri||evpri)printf("%02d: %s\n",count,errmess[count]);
    count++;
  }
  if(pri||evpri)printf("\n \n");

  if(out_flag)ev->JINFStatus[Jinfnum]=status;

  if((status&0x200)>0||(status&0x400)>0){
    printf("=======================> JINF Error!!!  Skip this event: %d\n",ev->Evtnum); 
    count=5;
    for (int base=32;base<0x8000;base*=2){
      if((status&base)>0)
	printf("%02d: %s\n",count,errmess[count]);
      count++;
    }
    printf("Uscita da OneJinf\n");
    return 10;
  }
// come back to the previous pos
  if(int ret0=fseek(rawfile,pos,SEEK_SET)){
    printf("Fatal: error during file skip %d\n",ret0);
    exit(3);
  }
  
  int ret;
  
  if(!out_flag)JinfMap[nJinf++]=status&0x1f;

  // Read the TDR
  while ( ftell(rawfile)/2< base+size-1 )  {
    ret=ReadOneTDR(status&0x1f);
    if (ret==10) {
      printf("Uscita da OneJinf (Tdr)\n");
      return ret;
    }
  }
  // Reread the last word
  ReadFile(&status,sizeof(status),1,rawfile);
  if(pri||evpri) printf("Jinf status    %d \n ",status); 
  return 0;

  pri=old;
}


//=============================================================================================

int DecodeData::ReadFile(void * ptr, size_t size, size_t nitems,
		     FILE * stream){

  int ret=0;
  ret=fread(ptr,size,nitems,stream);
  if(feof(stream)) {printf("End of File \n"); return -1;}
  if(ferror(stream)) {printf("Error reading \n"); return -2;}
  return ret;
}


void DecodeData::mysort(int* aa, int nel){

  int* bb= new int[nel];
  
  int min=10000;
  int max=-1;
  for (int count=0;count<nel;count++){  
    for (int ii=0;ii<nel;ii++){
      if(aa[ii]<min&&aa[ii]>max) {min=aa[ii]; bb[count]=aa[ii];}
    }
    max=bb[count];
    min=10000;
  }
  
  for (int count=0;count<nel;count++)
    aa[count]=bb[count];
  delete[] bb;
}
