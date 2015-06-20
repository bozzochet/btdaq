#include <iostream>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include "DecodeData.hh"


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
DecodeData::DecodeData(char* ifname, char* caldir, int run, int ancillary){

  runn=run;
  ntdrRaw=0;
  memset(tdrRaw,-1,TDRNUM*sizeof(tdrRaw[0]));
  ntdrCmp=0;
  memset(tdrCmp,-1,TDRNUM*sizeof(tdrCmp[0]));
  pri=1;
  evpri=0;
  sprintf(type,"Jinf");
  if (ancillary < 0)
	  sprintf(rawname,"%s/%06d.dat",ifname,runn);
  else
	  sprintf(rawname, "%s/%06d_ANC_%d.dat", ifname, runn, ancillary);
  sprintf(rawdir,"%s",ifname);
  sprintf(rawCaldir,"%s",caldir);

  rawfile = fopen(rawname,"r");
  if (rawfile == NULL) {
	  printf ("Error file %s not found \n",rawname);
	  exit(2);
  }


  tdroffset=0;
  evenum=0;
  runn=run;

  // Create the ROOT run header
  rh= new RHClass();

  //Read the run Header
  DumpRunHeader();

  // Try to get some info on how many ladders there are in the data (RAW and CMP)
  long int pos=ftell(rawfile);
  out_flag=0;
  if (pri) printf("*************** Reading one event to understand things...\n");
  ReadOneEvent();
  // come back to the previous pos
  if(int ret0=fseek(rawfile,pos,SEEK_SET)){
    printf("Fatal: error during file skip  %d\n",ret0);
    exit(3);
  }
  out_flag=1;

  //  // Create the ROOT Classes for the ROOT data format
  ev = new Event(ntdrRaw);

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

  if(pri) printf("Dumping the file headers that are going to be written in the ROOT files...\n");
  rh->Print();
  
  //Find the Calibrations
  if(pri) printf("Finding a valid calibration...\n");
  FindCalibs();

  // for (int ii=0;ii<ntdrRaw;ii++) {
  //   printf("RAW: %d -> %d\n", ii, tdrRaw[ii]);
  // }

  // for (int ii=0;ii<ntdrCmp;ii++) {
  //   printf("CMP: %d -> %d\n", ii, tdrCmp[ii]);
  // }  
}
//=============================================================================================

DecodeData::~DecodeData(){
	if(pri) cout<< "In the destructor..." <<endl;
	if(rawfile) CloseFile();
	if (ev) delete ev;
	if(pri) cout<< "Destroyed." <<endl;
}
//=============================================================================================

int DecodeData::FindPos(int tdrnum){

  // for (int ii=0;ii<ntdrCmp;ii++) {
  //   printf("CMP: %d -> %d\n", ii, tdrCmp[ii]);
  // }
  
  for (int ii=0;ii<ntdrCmp;ii++)
    if(tdrCmp[ii]==tdrnum)  return ii;
  return -1;
}
//=============================================================================================

int DecodeData::FindPosRaw(int tdrnum){
  
  // for (int ii=0;ii<ntdrRaw;ii++) {
  //   printf("RAW: %d -> %d\n", ii, tdrRaw[ii]);
  // }
  
  for (int ii=0;ii<ntdrRaw;ii++)
    if(tdrRaw[ii]==tdrnum)  return ii;
  return -1;
}
//=============================================================================================

int DecodeData::FindCalPos(int tdrnum){

	for (int ii=0;ii<ntdrCmp;ii++)
		if(tdrCmp[ii]==tdrnum)  return ii;
	printf("DecodeData::FindCalPos:  Fatal Error can't find postion for TDR %d\n",tdrnum);
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
	unsigned short int dummy;

	if(rawfile) {
		dummy=0;//without the declaration of this variable it's not working (not finding the end of the file!!!!!)
		eff= feof(rawfile);
		if (eff) printf("EndOfFile: end of file !\n" ); 
		if (pri) printf("End of file? -> %d\n", eff);
	}
	else {
		printf("Error file pointer is NULL \n");
		exit(3);
	}

	return eff;
}

//=============================================================================================

void DecodeData::DumpRunHeader(){
  unsigned short int size;
  header hh;
  
  //Read The Header Size
  ReadFile(&size, sizeof(size), 1, rawfile);
  
  size*=2;//this is the size stored but for some reason we decided (in TakeData) to store in units of word, so divided by 2.0
  
  if(pri) printf("Headersize: %d\n", size);
  /* check the header size */
  if (size!=sizeof(hh)) printf("!!!!!!!!!!!!!!!!!!! WRONG: Headersize = %zu (but sizeof(header) = %zu)\n", size, sizeof(hh));
  //	ReadFile(&hh, sizeof(header), 1, rawfile);//this should be fine also
  ReadFile(&hh, size, 1, rawfile);
  
  rh->Run=hh.run;
  sprintf(rh->date, "%s", hh.date);
  
  printf("Run: %d   Date: %s\n",hh.run,hh.date);
  for (int ii=0;ii<4;ii++)
    printf("Angle (%d) = %f\n", ii, hh.gonpar[ii]);
  printf("RefmaskJJ: 0x%x\n", hh.refmaskjj);
  for (int ii=0;ii<24;ii++)
    printf("Refmask (%d) = 0x%x\n", ii, hh.refmask[ii]);
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
  unsigned int tdrnoeventmask;//two words before status
  unsigned short int status;
  unsigned short int num;
  
  int fstat=0;
  
  if(!out_flag){
    ntdrRaw=0;
    ntdrCmp=0;
    nJinf=0;
  }
  
  //Read The Event Size
  if(pri) printf("ReadOneEvent) pos:%ld\n ",ftell(rawfile)/2);
  
  fstat=ReadFile(&size,sizeof(size),1,rawfile);
  if (fstat==-1) return 1; 
  
  long int base=ftell(rawfile)/2;
  
  //update the event counters
  evenum++;
  
  if(out_flag) ev->Evtnum=evenum;
  
  if(pri) printf("ReadOneEvent-AfterSizeRead) pos:%ld\n",ftell(rawfile)/2);
  
  //Read the Event Number
  fstat=ReadFile(&num,sizeof(num),1,rawfile);
  if (fstat==-1) return 1;
  
  if(pri) printf("Evenum: %d\n",num);
  if(out_flag)ev->Evtnum=num;
  if(pri||evpri)printf("\n-----------------------------> New event: %d\n",num);
  
  if(pri||evpri)printf("%s size: %d\n",type,size);
  
  //Store the position on the file
  long int pos=ftell(rawfile);
  
  // skip all the TDR data
  if(int ret0=fseek(rawfile,(size-2)*2,SEEK_CUR)){
    printf("Fatal: error during file skip ret0=%d \n",ret0);
    exit(3);
  }
  
  // read the last word of the event
  if(pri) printf("ReadOneEvent-LastWordOfEvent) pos:%ld\n",ftell(rawfile)/2);
  fstat=ReadFile(&status,sizeof(status),1,rawfile);
  if (fstat==-1) return 1;
  
  if(pri||evpri)printf("%s Status %d: address=%02d  status=",type,status,status&0x1f); 
  
  int count=5;
  for (int dase=32; dase<0x8000; dase*=2)
    if(pri||evpri)printf(" %2d: %d |",count++,((status&dase)>0));
  if(pri||evpri)printf("\n");
  
  count=5;
  for (int dase=32;dase<0x8000;dase*=2){
    if((status&dase)>0)
      if(pri||evpri)printf(" %02d: %s\n",count,errmess[count]);
    count++;
  }
  if(pri||evpri)printf("\n\n");
  
  if(out_flag) ev->JINJStatus=status;
  
  //  if((status&0x400)>0){
  if((status&0x200)>0||(status&0x400)>0){
    printf("=======================> %s Error!!! Status %hx\n", type, status); 
    if (out_flag) printf("=======================> %s Error!!!  Skip this event: %d\n",type, ev->Evtnum); 
    count=5;
    for (int dase=32;dase<0x8000;dase*=2){
      if((status&dase)>0)
	printf(" %02d: %s\n",count,errmess[count]);
      count++;
    }
    return 10;
  }
  
  // come back to the previous pos (i.e. to the first sub-block)
  if(int ret0=fseek(rawfile,pos,SEEK_SET)){
    printf("Fatal: error during file skip %d\n",ret0);
    exit(3);
  }
  
  // Read the TDRs or the JINFs
  if(!out_flag){
    ntdrRaw=0;
    ntdrCmp=0;
  }
  
  if(strcmp(type,"JinJ")==0){
    while ( ftell(rawfile)/2< base+size-1 ){
      ReadOneJINF();
    }
  }
  else {
    while ( ftell(rawfile)/2< base+size-3 ){
      if(pri) 
	printf("ReadOneTDR from file position: %ld\n", ftell(rawfile)/2);
      ReadOneTDR(0);
    }
    // Read the last 2 words before status
    unsigned short int dummy;
    fstat=ReadFile(&dummy,sizeof(dummy),1,rawfile);
    tdrnoeventmask=dummy<<16;
    dummy=0;
    fstat=ReadFile(&dummy,sizeof(dummy),1,rawfile);
    tdrnoeventmask+=dummy;;
    if (fstat==-1) return 1;
    if(pri||evpri) printf("Tdrs with no event Mask: %d\n", tdrnoeventmask); 
    for (int ii=0; ii<NTDRS; ii++){
      if (tdrnoeventmask&(1<<ii)){
	if(pri||evpri) printf("A tdr (%d) replied with no event...\n",ii);
	if(!out_flag)
	  tdrCmp[ntdrCmp++]=ii+100*0;
      } else if (pri||evpri) 
	printf("A tdr (%d) didn't replied...\n",ii);
    }
  }
  
  // Reread the last word
  fstat=ReadFile(&status,sizeof(status),1,rawfile);
  if (fstat==-1) return 1;
  
  if(pri||evpri) printf("%s status: %d\n",type,status); 
  return 0;
}


//=============================================================================================

int DecodeData::ReadOneTDR(int Jinfnum){
  unsigned short int size;
  
  //read the size of the TDR events
  if(pri) printf("pos:%ld\n",ftell(rawfile)/2);
  ReadFile(&size,sizeof(size),1,rawfile);
  
  if(pri) printf("TDRsize: %d \n",size);
  
  // allocate memory for the TDR event and read the event
  short int *array=(short int*)calloc(size,sizeof(short int));
  if(!array) {
    printf("Fatal error cannot alloc memeory.\n");
    exit(3);
  }
  
  ReadFile(array,size*sizeof(short int),1,rawfile);
  
  // for (int ii=0;ii<ntdrRaw;ii++) {
  //   printf("RAW: %d -> %d\n", ii, tdrRaw[ii]);
  // }

  // for (int ii=0;ii<ntdrCmp;ii++) {
  //   printf("CMP: %d -> %d\n", ii, tdrCmp[ii]);
  // }

  //Decode the tdr number (the order of tdrs in the file may change event by event!!!)
  int tdrnum=-1;
  int numnum=((unsigned int)array[size-1])&0x1f;
  //  printf("JINF=%d, NUMNUM=%d\n", Jinfnum, numnum);
  if(out_flag) {
    tdrnum=FindPos(numnum+100*Jinfnum);
    //    printf("%d\n", tdrnum);
    if(tdrnum<0) tdrnum=FindPosRaw(numnum+100*Jinfnum);
    if(tdrnum<0) {
      printf("DecodeData::ReadOneTDR::Cannot-Find-TDR-in-CMP-or-RAW\n");
      exit(4);
    }
  }
  
  if(pri) printf("==========> TDR %02d, Jinf %02d, Size=%4d, TDR Status=%04hx: ", numnum, Jinfnum, size, array[size-1]);
  if(!out_flag) if((array[size-1]&64)>0)  tdrRaw[ntdrRaw++]=numnum+100*Jinfnum;
  if(!out_flag) if((array[size-1]&128)>0) tdrCmp[ntdrCmp++]=numnum+100*Jinfnum;
  
  int count=5;
  for (int base=32;base<0x8000;base*=2)
    if(pri||evpri)
      printf(" %2d: %d |",count++,((array[size-1]&base)>0));
  if(pri||evpri)printf("\n");
  
  count=5;
  for (int base=32;base<0x8000;base*=2){
    if((array[size-1]&base)>0)
      if(pri)printf(" %02d: %s\n",count,errmess[count]);
    count++;
  }
  
  if(pri) printf("\n");
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
    RawOffset=1024;// RAW data present
    if (pri) printf("|->RAW data present\n");
    if (pri) printf("Filling Event and Histograms for JINF %d, TDR %d (RAW)\n", Jinfnum, numnum);
    int tdrnumraw=0;
    int count=0;
    if (out_flag){
      tdrnumraw=FindPosRaw(numnum+100*Jinfnum);
      if(tdrnumraw<0) { printf("DecodeData::ReadOneTDR::Cannot-Find-TDR-%d-RAW\n",numnum+100*Jinfnum); exit(4); }
      calib* cal=&(cals[numnum+100*Jinfnum]);
      for (int kk=0;kk<320;kk++){
	ev->Signal[tdrnumraw][kk]=array[count];//first ADC on S
	ev->Signal[tdrnumraw][320+kk]=array[count+1];//second ADC on S
	ev->Signal[tdrnumraw][640+kk]=array[count+2];//ADC on K
	//	printf("RAW %d %d  %d\n",kk,ev->Signal[FindTDRPos(tdrnum)][kk],array[count]);
	count+=3;
      }
      for (int kk=960;kk<1024;kk++){//remaining (320->384) on ADC on K
	ev->Signal[tdrnumraw][kk]=array[kk];
      }
      for (int cc=0; cc<1024; cc++) {
	//	printf("%04d) %f %f %f -> %f\n", cc, ((double)ev->Signal[tdrnumraw][cc])/8.0, cal->ped[cc], cal->sig[cc], (ev->Signal[tdrnumraw][cc]/8.0-cal->ped[cc])/cal->sig[cc]);
	ev->SoN[tdrnumraw][cc] = (ev->Signal[tdrnumraw][cc]/8.0-cal->ped[cc])/cal->sig[cc];
	if (ev->SoN[tdrnumraw][cc]>3.5) hmio[numnum+100*Jinfnum]->Fill(cc, ev->SoN[tdrnumraw][cc]);//TEMPORARY!!! Should be replaced by the one filled with the off-line clusterization
      }

      Clusterize(numnum, Jinfnum, ev->Signal[tdrnumraw]);
    }
    
  }  
  
  if (TESTBIT(array[size-1],7)){    // Compressed data present
    if (pri) printf("|->Compressed data present\n");
    if (pri) printf("Filling Event and Histograms for JINF %d, TDR %d (CMP)\n", Jinfnum, numnum);
    //dump clusters
    int count=RawOffset;
    while (count<(size-1)){
      int bad=0;
      int lenword=(array[count++]);
      int cluslen=(lenword&0x7F) +1;
      int Sig2NoiStatus=(lenword&0xFF80)>>7;
      int addword=(array[count++]);
      int clusadd=addword&0x3FF;
      int CNStatus=(addword&0x3C00)>>10;
      int PowBits=(addword&0xC000)>>14;
      float sig[MAXLENGHT];
      if(pri) printf("Cluster: add=%d  lenght=%d\n", clusadd, cluslen);
      for(int hh=0; hh<cluslen; hh++){
	if(pri)printf("Signal: %d, Pos:%d\n", array[count], hh);
	if(hh<MAXLENGHT){
	  sig[hh]=array[count]/8.;
	  if(pri)printf("        %f, Pos: %d\n", sig[hh], hh);
	}
	else bad=1;
	count++;
      }
      
      if(out_flag){
	AddCluster(numnum, Jinfnum, clusadd, cluslen, Sig2NoiStatus, CNStatus, PowBits, bad, sig);
      }
    }
    
    //      int cc=1;
    //      //Read the  cnoise values
    //      for (int ii=size-17;ii<size-1;ii++){
    //        if(pri)printf("Cnoise %d:  %f \n",cc++,(array[ii])/8.);
    //        if(out_flag)ev->CNoise[tdrnum][ii-size+17]=(array[ii])/8.;
    //      }
  }
  if(pri)printf("\n");
  if(pri) printf("End of the TDR pos:%ld  \n",ftell(rawfile)/2);
  //free the memory for the next tdr
  free(array);
  
  
  return 0;
}

void DecodeData::AddCluster(int numnum, int Jinfnum, int clusadd, int cluslen, int Sig2NoiStatus, int CNStatus, int PowBits, int bad, float* sig) {

  int sid=0;
  if (clusadd>640) sid=1;
  Cluster* pp= ev->AddCluster(numnum+100*Jinfnum,sid);
  calib* cal=&(cals[numnum+100*Jinfnum]);
  hmio[numnum+100*Jinfnum]->Fill(clusadd);
  pp->Build(numnum+100*Jinfnum,sid,clusadd,cluslen,sig,&(cal->sig[clusadd]),
	    &(cal->status[clusadd]),Sig2NoiStatus, CNStatus, PowBits, bad);
  if(pri) pp->Print();

  return;
}

void DecodeData::Clusterize(int numnum, int Jinfnum, short int Signal[1024]) {
  
  /*
    int bad=0;
    int lenword=(array[count++]);
    int cluslen=(lenword&0x7F) +1;
    int Sig2NoiStatus=(lenword&0xFF80)>>7;
    int addword=(array[count++]);
    int clusadd=addword&0x3FF;
    int CNStatus=(addword&0x3C00)>>10;
    int PowBits=(addword&0xC000)>>14;
    float sig[MAXLENGHT];
    if(pri) printf("Cluster: add=%d  lenght=%d\n", clusadd, cluslen);
    for(int hh=0; hh<cluslen; hh++){
    if(pri)printf("Signal: %d, Pos:%d\n", array[count], hh);
    if(hh<MAXLENGHT){
    sig[hh]=array[count]/8.;
    if(pri)printf("        %f, Pos: %d\n", sig[hh], hh);
    }
    else bad=1;
    count++;
    }
    
    AddCluster(numnum, Jinfnum, clusadd, cluslen, Sig2NoiStatus, CNStatus, PowBits, bad, sig);
  */
  
  return;
}

//=============================================================================================

void DecodeData::FindCalibs(){

  struct stat buf;
  char name1[300];
  int run2;
  FILE* calfile[TDRNUM];
  int old=pri;
  
  if((ntdrCmp+ntdrRaw)<1){
    printf("No TDR in CMP or RAW mode --> No DSP Calib can be found\n");
    return;
  }
  
  for (run2=runn ;run2>0 ;run2--){
    sprintf(name1,"%s/%06d_%04d.cal",rawCaldir,run2,tdrCmp[0]);
    if(stat(name1,&buf)==0){
      printf("First calib Found %s run %d\n",name1,run2);
      break;
    }
  }
  
  if (run2<40) {
    printf("Cannot find any calibration done before the requested run %d\n",runn);
    printf("I give up. Bye.\n");
    exit(2);
  }
  else 
    printf ("Searching for other calib files\n");
  
  for (int ii=0;ii<ntdrCmp;ii++){
    int Jinfnum=tdrCmp[ii]/100;
    int tdrnum=tdrCmp[ii]-Jinfnum*100;
    int index = Jinfnum*100+tdrnum;
    sprintf(name1,"%s/%06d_%02d%02d.cal", rawCaldir, run2, Jinfnum, tdrnum);
    calfile[index]=fopen(name1,"r");
    if(!calfile[index]){
      printf("Cannot find the calib %s for the requested calib run %d\n",name1,run2);
      printf("I give up. Bye.\n");
      exit(2);
    }
    printf("Found file %s\n",name1);
    //    printf("---- %d\n", index);
    ReadCalib(calfile[index],&(cals[index]));
  }

  for (int ii=0;ii<ntdrRaw;ii++){
    int Jinfnum=tdrRaw[ii]/100;
    int tdrnum=tdrRaw[ii]-Jinfnum*100;
    int index = Jinfnum*100+tdrnum;
    sprintf(name1,"%s/%06d_%02d%02d.cal", rawCaldir, run2, Jinfnum, tdrnum);
    calfile[index]=fopen(name1,"r");
    if(!calfile[index]){
      printf("Cannot find the calib %s for the requested calib run %d\n",name1,run2);
      printf("I give up. Bye.\n");
      exit(2);
    }
    printf("Found file %s\n",name1);
    //    printf("---- %d\n", index);
    ReadCalib(calfile[index],&(cals[index]));
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
  unsigned short int noknown;
  unsigned short int tdrnoeventmask;
  unsigned short int status;
  
  int old=1;
  old=pri;
  
  //Read The Event Size
  if(pri)
    printf("pos:%ld  ",ftell(rawfile)/2);
  ReadFile(&size,sizeof(size),1,rawfile);
  if(pri)
    printf("JINFsize    %d \n",size);
  
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
  
  int Jinfnum=((unsigned int)status)&0x1f;//e questo numero se non c'è un jinj master non sò chi glielo potrebbe aver messo (però se sta nello status che comunque ci sarà, al limite sarà 0...) 
  
  
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
  
  if(out_flag) ev->JINFStatus[Jinfnum]=status;
  
  //  if((status&0x400)>0){
  if((status&0x200)>0||(status&0x400)>0){
    printf("=======================> JINF Error!!!  Skip this event: %d\n",ev->Evtnum); 
    count=5;
    for (int base=32;base<0x8000;base*=2){
      if((status&base)>0)
	printf("%02d: %s\n",count,errmess[count]);
      count++;
    }
    return 10;
  }
  // come back to the previous pos
  if(int ret0=fseek(rawfile,pos,SEEK_SET)){
    printf("Fatal: error during file skip %d\n",ret0);
    exit(3);
  }
  
  
  if(!out_flag)JinfMap[nJinf++]=status&0x1f;
  
  // Read the TDR
  while ( ftell(rawfile)/2< base+size-1 )  ReadOneTDR(status&0x1f);
  // Read the last 2 words before status
  ReadFile(&noknown,sizeof(noknown),1,rawfile);
  ReadFile(&tdrnoeventmask,sizeof(tdrnoeventmask),1,rawfile);
  if(pri||evpri) printf("Tdrs with no event Mask: %d\n", tdrnoeventmask); 
  for (int ii=0; ii<NTDRS; ii++){
    if (tdrnoeventmask&(1<<ii)){
      if(pri||evpri) printf("A tdr (%d) replied with no event...\n",ii);
      if(!out_flag) tdrCmp[ntdrCmp++]=ii+100*(status&0x1f);
    }
  }
  // Reread the last word
  ReadFile(&status,sizeof(status),1,rawfile);
  if(pri||evpri)printf("Jinf status    %d \n ",status); 
  return 0;
  
  pri=old;
}


//=============================================================================================

int DecodeData::ReadFile(void * ptr, size_t size, size_t nitems, FILE * stream){

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
