#include <iostream>
#include <ctime>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include "DecodeData.hh"

#include "TString.h"
#include "TRandom3.h"

using namespace std;

extern char progname[50];

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

inline bool file_exists(const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

TString stringtodump;

//=============================================================================================
DecodeData::DecodeData(char* ifname, char* caldir, int run, int ancillary, bool _kMC){

  kMC=_kMC;
  
  runn=run;

  ntdrRaw=0;
  ntdrCmp=0;
  memset(tdrMap, -1, NTDRS * sizeof(tdrMap[0]));
  
  memset(tdrAlign,-1,NTDRS*sizeof(tdrAlign[0]));//added by Viviana
  
  pri=0;
  //  pri=1;
  evpri=0;
  
  sprintf(type,"Jinf");

  OpenFile(ifname, caldir, run, ancillary);
  
  tdroffset=0;
  evenum=0;
  runn=run;

  shighthreshold=3.5;
  slowthreshold=1.0;
  khighthreshold=3.5;
  klowthreshold=1.0;

  kClusterize=false;
  cworkaround=0;


  // Create the ROOT run header
  rh = new RHClass();

  if(!kMC){
    //Read the run Header
    DumpRunHeader();      
    
    // Try to get some info on how many ladders there are in the data (RAW and CMP)
    long int pos=ftell(rawfile);
    out_flag=0;
    if (pri)
      printf("*************** Reading one event to understand things...\n");
    ReadOneEvent();
    if (pri) printf("*************** ONE event read...\n");
    // come back to the previous pos
    if(int ret0=fseek(rawfile,pos,SEEK_SET)){
      printf("Fatal: error during file skip  %d\n",ret0);
      exit(3);
    }
    
    out_flag=1;  
  }
  else {
    rh->SetRun(runn);
    time_t now=time(0);//MD: I don't like. Should be decided when simulating, not when analyzing!
    rh->SetDate(ctime(&now));
    nJinf=0;
    out_flag=0;
    ReadOneEvent();
    ntdrRaw=ntdrMC;//MD: seems ok. MC files are raw by default and we clusterize offline...
    out_flag=1;
  }

  //  // Create the ROOT Classes for the ROOT data format
  ev = new Event();
  
  mysort(tdrMap, ntdrRaw+ntdrCmp);
  // Update the ROOT run header
  rh->SetNJinfs(nJinf);
  rh->SetJinfMap(JinfMap);
  rh->SetNTdrsRaw(ntdrRaw);
  rh->SetNTdrsCmp(ntdrCmp);
  rh->SetTdrMap(tdrMap);
  
  if(pri) {
    printf("********* READVALS: %d %d %d %d \n", NTDRS, nJinf, ntdrRaw, ntdrCmp);      
    printf("Dumping the file headers that are going to be written in the ROOT files...\n");
  }
  rh->Print();
 
  //Find the Calibrations
  if(pri)
    printf("Finding a valid calibration...\n");
  FindCalibs();

  //added by Viviana
  int ntotdrs=ntdrRaw+ntdrCmp;
  //  for (int ii=0;ii<NTDRS;ii++) { 
  for (int ii=0;ii<ntotdrs;ii++) { // check if compatible with data struct
    calib* calt=&(cals[ii]);
    printf("CALTEST: TDR %d -> %f %f %d\n", ii, calt->ped[0],calt->sig[0],calt->status[0]);
  }
  //
  
  char name[255];
  for (int jj=0; jj<NJINF; jj++)
    {
      for (int hh=0; hh<NTDRS; hh++)
	{
	  sprintf(name,"occ_%d_%d",jj, hh);
	  //	  hocc[jj*NTDRS+hh]= new TH1F(name,name,1024,0,1024);
	  hocc[jj*NTDRS+hh]= new TH1F(name,name,4096,0,4096);
	  sprintf(name,"occseed_%d_%d",jj, hh);
	  //	  hoccseed[jj*NTDRS+hh]= new TH1F(name,name,1024,0,1024);
	  hoccseed[jj*NTDRS+hh]= new TH1F(name,name,4096,0,4096);
	  
	  sprintf(name,"qS_%d_%d", jj, hh);
	  hcharge[jj*NTDRS+hh][0]= new TH1F(name,name,1000,0,100);
	  sprintf(name,"qK_%d_%d", jj, hh);
	  hcharge[jj*NTDRS+hh][1]= new TH1F(name,name,1000,0,100);
	  
	  sprintf(name,"signalS_%d_%d", jj, hh);
	  hsignal[jj*NTDRS+hh][0]= new TH1F(name,name,4200,-100,4100);
	  sprintf(name,"signalK_%d_%d", jj, hh);
	  hsignal[jj*NTDRS+hh][1]= new TH1F(name,name,4200,-100,4100);
	  
	  sprintf(name,"q_vs_occ_%d_%d", jj, hh);
	  hchargevsocc[jj*NTDRS+hh]= new TH2F(name,name,4096,0,4096,1000,0,100);
	  
	  sprintf(name,"signal_vs_occ_%d_%d", jj, hh);
	  hsignalvsocc[jj*NTDRS+hh]= new TH2F(name,name,4096,0,4096,4200,-100,4100);
	  
	  sprintf(name,"sonS_%d_%d", jj, hh);
	  hson[jj*NTDRS+hh][0]= new TH1F(name,name,1000,0,100);
	  sprintf(name,"sonK_%d_%d", jj, hh);
	  hson[jj*NTDRS+hh][1]= new TH1F(name,name,1000,0,100);
	}
    }
}
//=============================================================================================

DecodeData::~DecodeData(){

  for (int jj=0; jj<NJINF; jj++){
    for (int hh = 0; hh < NTDRS; hh++) {
      //      printf("%d %d --> %f\n", jj, hh, hocc[jj*NTDRS+hh]->GetEntries());
      if (hocc[jj*NTDRS+hh]->GetEntries()<1.0) {
	//	printf("deleting hocc %d %d\n", jj, hh);
	delete hocc[jj*NTDRS+hh];
      }
      if (hoccseed[jj*NTDRS+hh]->GetEntries()<1.0) {
	//	printf("deleting hoccseed %d %d\n", jj, hh);
	delete hoccseed[jj*NTDRS+hh];
      }
      if (hchargevsocc[jj*NTDRS+hh]->GetEntries()<1.0) {
	//	  printf("deleting hchargevsocc %d %d\n", jj, hh);
	delete hchargevsocc[jj*NTDRS+hh];
      }
      if (hsignalvsocc[jj*NTDRS+hh]->GetEntries()<1.0) {
	//	  printf("deleting hsignalvsocc %d %d\n", jj, hh);
	delete hsignalvsocc[jj*NTDRS+hh];
      }

      for (int ss=0; ss<2; ss++) {
	//	printf("%d %d %d --> %f\n", jj, hh, ss, hcharge[jj*NTDRS+hh][ss]->GetEntries());
	if (hcharge[jj*NTDRS+hh][ss]->GetEntries()<1.0) {
	  //	  printf("deleting hcharge %d %d %d\n", jj, hh, ss);
	  delete hcharge[jj*NTDRS+hh][ss];
	}
	if (hsignal[jj*NTDRS+hh][ss]->GetEntries()<1.0) {
	  //	  printf("deleting hsignal %d %d %d\n", jj, hh, ss);
	  delete hsignal[jj*NTDRS+hh][ss];
	}
	if (hson[jj*NTDRS+hh][ss]->GetEntries()<1.0) {
	  //	  printf("deleting hson %d %d %d\n", jj, hh, ss);
	  delete hson[jj*NTDRS+hh][ss];
	}
      }
    }
  }
  
  if(pri)
    cout<< "In the destructor..." <<endl;
  if(rawfile)
    CloseFile();
  if (ev)
    delete ev;
  if(pri)
    cout<< "Destroyed." <<endl;


  if(kMC){
    delete mcf;
    delete mcrt;
    delete mcht;    
  }
  
  return;
}

//=============================================================================================

int DecodeData::FindPos(int tdrnum) {

  for (int ii = 0; ii < ntdrCmp+ntdrRaw; ii++)
    if (tdrMap[ii].first == tdrnum)
      return ii;
  
  return -1;
}
//=============================================================================================

int DecodeData::FindPosMC(int hitnum){
  
  // dummy map function -> to implement for realistic MC
  // return hvol[hitnum]; // does not work on old files
  // MD: see also if is really needed to have a separe FindPos for MC
  return hitnum;
}
//=============================================================================================

int DecodeData::FindCalPos(int tdrnum){

  for (int ii = 0; ii<ntdrCmp+ntdrRaw; ii++)
    if (tdrMap[ii].first == tdrnum)
      return ii;
  
  printf("DecodeData::FindCalPos:  Fatal Error can't find postion for TDR %d\n", tdrnum);
  
  exit(4);
}
//=============================================================================================

void DecodeData::OpenFile(char* ifname, char* caldir, int run, int ancillary){
  
  if (kMC) {
    //    OpenFile_mc(ifname, caldir, run, ancillary);
    OpenFile_mc(ifname);
  }
  else {
    OpenFile_data(ifname, caldir, run, ancillary);
  }
  
  sprintf(rawdir,"%s",ifname);
  sprintf(rawCaldir,"%s",caldir);
  
  rawfile = fopen(rawname,"r");
  if (rawfile == NULL) {
    printf ("Error file %s not found \n", rawname);
    exit(2);
  }
  
  return;
}

void DecodeData::OpenFile_data(char* ifname, char* caldir, int run, int ancillary){
  
  if (ancillary>=0) {
    sprintf(rawname, "%s/%06d_ANC_%d.dat", ifname, runn, ancillary);
  }
  else {
    sprintf(rawname, "%s/%06d.dat", ifname, runn);
    if (!file_exists(rawname)) {
      for (int ii=0; ii<10000; ii++) {
	sprintf(rawname, "%s/%06d_ANC_%d.dat", ifname, runn, ii);
	if (file_exists(rawname))
	  break;
      }
    }
  }
  
  return;
}

//void DecodeData::OpenFile_mc(char* ifname, char* caldir, int run, int ancillary){
void DecodeData::OpenFile_mc(char* ifname){
  sprintf(rawname, "%sanaOut.root",ifname);

  mcf=new TFile(rawname);
  mcrt=(TTree*)mcf->Get("runTree");
  mcrt->SetBranchAddress("nEvts",&gevt);
  mcrt->SetBranchAddress("gEne",&gene);
  mcrt->SetBranchAddress("nLayers",&nlayers);
  mcrt->SetBranchAddress("xyAlign",&tdrAlign);
  mcrt->GetEntry(0);
  printf("OPEN MC FILE %s -> SIM Events, energy[GeV]: %d %f\n", rawname, gevt, gene);
  mcht=(TTree*)mcf->Get("hitTree");
  int nent=mcht->GetEntries();
  printf("-> PProd Entries: %d\n", nent);
  
  return;
}

void DecodeData::CloseFile(){
  
  if(rawfile)
    fclose(rawfile);
  rawfile=NULL;
  
  return;
}

int DecodeData::EndOfFile(){

  int eff=0;
  if (kMC) {
    eff=EndOfFile_mc();
  }
  else {
    eff=EndOfFile_data();
  }
  return eff;
}

int DecodeData::EndOfFile_data(){

  int eff=0;
  unsigned short int dummy;
  
  if(rawfile) {
      dummy=0;//without the declaration of this variable it's not working (not finding the end of the file!!!!!)
    eff= feof(rawfile);
    //    if (eff) printf("EndOfFile: end of file !\n" ); 
    if (pri) printf("End of file? -> %d\n", eff);
  }
  else {
    printf("Error file pointer is NULL \n");
    exit(3);
  }
  
  return eff;
}

int DecodeData::EndOfFile_mc(){
  
  int eff=0;
  
  if(evenum==mcht->GetEntries())
    eff=-1;
  
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
  if (size != sizeof(hh))
    printf("!!!!!!!!!!!!!!!!!!! WRONG: Headersize = %hu (but sizeof(header) = %zu)\n", size, sizeof(hh));
  //	ReadFile(&hh, sizeof(header), 1, rawfile);//this should be fine also
  ReadFile(&hh, size, 1, rawfile);
  
  rh->SetRun(hh.run);
  rh->SetDate(hh.date);
  
  printf("Run: %d   Date: %s\n", hh.run, hh.date);
  if (pri) {
    for (int ii=0;ii<4;ii++)
      printf("Angle (%d) = %f\n", ii, hh.gonpar[ii]);
    printf("RefmaskJJ: 0x%x\n", hh.refmaskjj);
    for (int ii=0; ii<24; ii++)
      printf("Refmask (%d) = 0x%x\n", ii, hh.refmask[ii]);
  }
}

//=============================================================================================

int DecodeData::SkipOneEvent(int evskip){

  int ret;
  
  if (kMC) {
    ret = SkipOneEvent_mc(evskip);
  }
  else {
    ret = SkipOneEvent_data(evskip);
  }

  return ret;
}

int DecodeData::SkipOneEvent_data(int evskip){
  
  unsigned short int size;
  unsigned short int junk;
  
  printf (" Run %d SKIPPING %d events on DAQ %s\n", runn, evskip, type);
  for (int kk=0;kk<evskip;kk++){
    //Read The Event Size
    if (pri) printf("pos:%ld  ",ftell(rawfile)/2);
    ReadFile(&size,sizeof(size), 1, rawfile);
    
    if (pri) printf("%s size    %d \n",type,size);
    for (int ii=0; ii<size; ii++)  {
      ReadFile(&junk,sizeof(junk), 1, rawfile);
      if (pri) printf("%d:  %d\n", ii, junk);
      //printf("pos:%ld  ",ftell(rawfile)/2);
    }
  }
  
  return 0;
}

int DecodeData::SkipOneEvent_mc(int evskip){

  //TO BE IMPLEMENTED
  // MD: most-likely will never happen
  
  return 0;
}


//=============================================================================================

int DecodeData::ReadOneEvent(){

  int ret;
  
  if (kMC) {
    ret = ReadOneEvent_mc();
  }
  else {
    ret = ReadOneEvent_data();
  }
  
  return ret;
}

int DecodeData::ReadOneEvent_data(){
  
  // MD: Viviana moved here: I think is wrong
  // ev = new Event();
  
  unsigned short int size;
  unsigned int tdrnoeventmask; //two words before status
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

#ifdef CALOCUBE
  int header_size;
  fstat=ReadFile(&header_size,sizeof(int),1,rawfile);
  if(fstat!=1) 
    return 1;
  if(header_size != (int)sizeof(wholeheader)) {//not clear to me why he's always reading this kind of header: TIC data where collected with the other header...
    printf("Warning : the size of wholeheader is %d instead of the expected value %d\n", header_size, (int)sizeof(wholeheader));
    return 1;
  }

  wholeheader calocube_header;
  fstat = ReadFile(&calocube_header, sizeof(wholeheader), 1, rawfile);
  if (fstat == -1)
    return 1;
    /*
  cout << hex << endl;
  cout << "Header SIZE = " << calocube_header.SIZE << endl;
  cout << "Header RRRWNODETYPE = " << calocube_header.RRRWNODETYPE << endl;
  cout << "Header FBITAG = " << calocube_header.FBITAG << endl;
  cout << "Header TIMEMSB = " << calocube_header.TIMEMSB << endl;
  cout << "Header TIMELSB = " << calocube_header.TIMELSB << endl;
  cout << "Header JMDCSIZE = " << calocube_header.JMDCSIZE << endl;
  cout << "Header JMDCRRRWNODETYPE = " << calocube_header.JMDCRRRWNODETYPE << endl;
  cout << "Header RUNNUMMSB = " << calocube_header.RUNNUMMSB << endl;
  cout << "Header RUNNUMLSB = " << calocube_header.RUNNUMLSB << endl;
  cout << "Header RUNTAGMSB = " << calocube_header.RUNTAGMSB << endl;
  cout << "Header RUNTAGLSB = " << calocube_header.RUNTAGLSB << endl;
  cout << "Header EVTNUMMSB = " << calocube_header.EVTNUMMSB << endl;
  cout << "Header EVTNUMLSB = " << calocube_header.EVTNUMLSB << endl;
  cout << "Header JMDCTIMEMSB = " << calocube_header.JMDCTIMEMSB << endl;
  cout << "Header JMDCTIMELSB = " << calocube_header.JMDCTIMELSB << endl;
  cout << "Header JMDCTIMEFINEMSB = " << calocube_header.JMDCTIMEFINEMSB << endl;
  cout << "Header JMDCTIMEFINELSB = " << calocube_header.JMDCTIMEFINELSB << endl;
  int time_s = (int)(calocube_header.JMDCTIMEMSB*pow(2, 16)+calocube_header.JMDCTIMELSB);
  int time_u = (int)(calocube_header.JMDCTIMEFINEMSB*pow(2, 16)+calocube_header.JMDCTIMEFINELSB);
  time_t date_time = time_s;
  cout << "Equivalent to \n\t" << dec << asctime(localtime(&date_time)) <<
		  "\t\t and " << time_u << " usec " << hex << endl;
  cout << "Header GReservedGroups = " << calocube_header.GReservedGroups << endl;
  cout << "Header DSPSIZE = " << calocube_header.DSPSIZE << endl;
  cout << "Header DSPRRRWNODETYPE = " << calocube_header.DSPRRRWNODETYPE << endl;
  */
#endif
  
  fstat = ReadFile(&size,sizeof(size), 1, rawfile);
  if (fstat==-1)
    return 1; 
  
  long int base = ftell(rawfile)/2;
  
  //update the event counters
  evenum++;
  
  if(out_flag)
    ev->Evtnum=evenum;
  
  if(pri) printf("ReadOneEvent-AfterSizeRead) pos:%ld\n", ftell(rawfile)/2);
  
  //Read the Event Number
  fstat = ReadFile(&num,sizeof(num),1,rawfile);
  if (fstat==-1)
    return 1;
  
  if(pri)
    printf("Evenum: %d\n",num);
  if(out_flag)
    ev->Evtnum=num;
  if(pri || evpri)
    printf("\n-----------------------------> New event: %d\n",num);
  
  if(pri || evpri)
    printf("%s size: %d\n",type,size);
  
  //Store the position on the file
  long int pos=ftell(rawfile);
  
  // skip all the TDR data
  if(int ret0 = fseek(rawfile, (size - 2)*2, SEEK_CUR)){
    printf("Fatal: error during file skip ret0=%d \n", ret0);
    exit(3);
  }
  
  // read the last word of the event
  if(pri)
    printf("ReadOneEvent-LastWordOfEvent) pos:%ld\n", ftell(rawfile)/2);
  fstat=ReadFile(&status,sizeof(status), 1, rawfile);
  if (fstat==-1)
    return 1;
  
  if(pri || evpri)
    printf("%s Status %d: address=%02d  status=", type, status, status&0x1f); 
  
  int count=5;
  for (int dase=32; dase<0x8000; dase*=2)
    if(pri||evpri)printf(" %2d: %d |",count++,((status&dase)>0));
  if(pri||evpri)printf("\n");
  
  count=5;
  for (int dase=32; dase<0x8000; dase*=2){
    if((status&dase)>0)
      if(pri || evpri)
	printf(" %02d: %s\n", count, errmess[count]);
    count++;
  }
  if(pri || evpri) printf("\n\n");
  
  if(out_flag)
    ev->JINJStatus=status;
  
  if((status&0x400)>0){
  // if((status&0x200)>0||(status&0x400)>0){
    printf("TEST\n");
    printf("=======================> %s Error!!! Status %hx\n", type, status); 
    if (out_flag) printf("=======================> %s Error!!!  Skip this event: %d\n",type, ev->Evtnum); 
    printf("TEST\n");
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
    fstat=ReadFile(&dummy,sizeof(dummy), 1, rawfile);
    tdrnoeventmask=dummy<<16;
    dummy=0;
    fstat=ReadFile(&dummy,sizeof(dummy), 1, rawfile);
    tdrnoeventmask+=dummy;;
    if (fstat==-1)
      return 1; 
    if (pri || evpri)
      printf("Tdrs with no event Mask: %d\n", tdrnoeventmask); 
    for (int ii=0; ii<NTDRS; ii++){
      if (tdrnoeventmask&(1<<ii)){
	if (pri || evpri)
	  printf("A tdr (%02d) replied with no event...\n", ii);
	if (!out_flag) {
	  //	  printf("tdrMap[%d].first = %d\n", ntdrRaw+ntdrCmp, ii+100*0);
	  tdrMap[ntdrRaw+ntdrCmp].first = ii + 100 * 0; //In ReadOneJinf is 100*(status&0x1f) but here is in the case with just one Jinf...
	  tdrMap[ntdrRaw+ntdrCmp].second = 1;
	  ntdrCmp++;
	}
      }
      else if (pri || evpri) {
	int tdrnum = FindPos(ii);
        //	  printf("%d\n", tdrnum);
        if (tdrnum >= 0)
          printf("A tdr (%02d) replied...\n", ii);
	else
	  printf("A tdr (%02d) didn't replied...\n", ii);
      }
    }
  }
  
  // Reread the last word
  fstat=ReadFile(&status,sizeof(status), 1, rawfile);
  if (fstat==-1)
    return 1;
  
  if(pri || evpri)
    printf("%s status: %d\n",type,status);
  
  return 0;
}

int DecodeData::ReadOneEvent_mc(){

  TRandom3 rn;
  double dEdX2ADC=3.5e3;

  //// layer!=hits -->> to change for realistic mc configuration
  //int nhits=NTDRS;
  //int pphit;
  //int echx[24];
  //int echy[24];
  //double edep[24];
  
  mcht->SetBranchAddress("nHits",&nhits);
  //  mcht->SetBranchAddress("hVol",&hvol);
  
  mcht->SetBranchAddress("hVolZ",&hvolz);
  mcht->SetBranchAddress("echX",&echx);
  mcht->SetBranchAddress("echY",&echy);
  mcht->SetBranchAddress("ppHit",&pphit);

  //// for electron -->> to set for both electron and positron 
  mcht->SetBranchAddress("eeDep",&edep);
     
  mcht->GetEntry(evenum);  
  printf("ReadOneEventMC entry:%d %d %d %d %d\n",evenum, nhits, pphit, echx[pphit], echy[pphit]); 
  ntdrMC=nlayers; 

  
  if(!out_flag){

    for(int ij=0;ij<nJinf;ij++){
      JinfMap[ij]=0;
    }
    for (int ir=0;ir<nlayers;ir++){
      //      tdrRaw[ir]=ir;//Viviana
      tdrMap[ir].first = ir;//MD: to check
      tdrMap[ir].second = 0;//MD: RAW, and to check
      printf("ALIGNMENT %d %d\n", ir, tdrAlign[ir]);
    }
    /*
    // Viviana, but not needed anymore
    for (int ic=0;ic<ntdrCmp;ic++){
      tdrCmp[ic]=0;
    }
    */

    //printf("END FIRST ReadOneEventMC entry:%d %d\n",evenum,nhits);
    
  }
  else{

    //update the event counters
    evenum++;  

    ev->Evtnum=evenum;
    ev->JINJStatus=999;
    
    ev->JINFStatus[0]=999;
    
    printf("ReadOneEventMC store to event: %d %d %d %d %d %f %f \n",evenum,nhits,pphit,echx[pphit],echy[pphit],edep[pphit],edep[pphit]*dEdX2ADC/8.);
    int hitclcount=0;
    for(int nh=0;nh<nhits;nh++){ // nhits!=nlayers
      hvol[nh]=nh;      
       
      //calib* cal=&(cals[nh]);
      calib* cal=&(cals[hvol[nh]]);
      
      ev->TDRStatus[hvol[nh]]=999;
      ev->ReadTDR[hvol[nh]]=1;
      
      printf("ReadOneEventMC HIT %d: %d %f cm %d %f GeV %f \n",nh,hvol[nh],hvolz[nh],(tdrAlign[hvol[nh]]?echx[nh]:echy[nh]),edep[nh],edep[nh]*dEdX2ADC/8.);
      //// hardcoded number of channels
      for (int kk=0;kk<4096;kk++){
	//for (int kk=0;kk<1024;kk++){
	ev->CalPed[hvol[nh]][kk]=cal->ped[kk];
	ev->CalSigma[hvol[nh]][kk]=cal->sig[kk];
	
	ev->RawSignal[hvol[nh]][kk]=int(rn.Gaus(0.,5.));// simnoise --> OK?
	//	int chh=!tdrAlign[nh]?echx[nh]:echy[nh]; // X or Y according to alignment
	// shift to get 4096 strips
	int chh=!tdrAlign[hvol[nh]]?echx[nh]-225:echy[nh]-225; // X or Y according to alignment
	
	//	if(kk<640){	  //// was for getting 0 on the k side
	if (kk==chh){
	  ev->RawSignal[hvol[nh]][kk]=int(edep[nh]*dEdX2ADC);//
	  printf("ReadOneEventMC HITSIG %d: %d %f %f \n",kk,ev->RawSignal[hvol[nh]][kk],cal->ped[kk],cal->sig[kk]);
	}
	ev->RawSoN[hvol[nh]][kk]=(ev->RawSignal[hvol[nh]][kk]/8.0-cal->ped[kk])/cal->sig[kk];
	
	//printf("ReadOneEventMC SIGS %d: %d %f %f -> %f \n",kk,ev->RawSignal[nh][kk],cal->ped[kk],cal->sig[kk],ev->RawSoN[nh][kk]);
	/*}else{
	  ev->RawSignal[nh][kk]=0.;//	
	  ev->RawSoN[nh][kk]=0.;
	  }*/
	
      }
      
      hitclcount++;

      //// numnum is hvol[nhit] -> must be mapped to find the ntdr(=nlayer)
      int mtdrn=FindPosMC(nh);
      
      printf("ReadOneEventMC clusterize hit %d on vol %d of layer(tdr) %d, hcl:%d \n",nh, hvol[nh], mtdrn, hitclcount);

      Clusterize(mtdrn, 0, cal);
      
    }

  }// out_flag ==1
    
  return 0;   
}

//=============================================================================================

int DecodeData::ReadOneTDR(int Jinfnum){
  unsigned short int size;
  
  //read the size of the TDR events
  if(pri)
    printf("pos:%ld\n",ftell(rawfile)/2);
  ReadFile(&size,sizeof(size), 1, rawfile);
  
  if(pri)
    printf("TDRsize: %d \n",size);
  
  // allocate memory for the TDR event and read the event
  short int *array=(short int*)calloc(size, sizeof(short int));
  if(!array) {
    printf("Fatal error cannot alloc memeory.\n");
    exit(3);
  }
  
  ReadFile(array,size*sizeof(short int), 1, rawfile);
  
  //Decode the tdr number (the order of tdrs in the file may change event by event!!!)
  int tdrnum=-1;
  int numnum=((unsigned int)array[size-1])&0x1f;
  //  printf("JINF=%d, NUMNUM=%d\n", Jinfnum, numnum);
  if(out_flag) {
    tdrnum=FindPos(numnum+100*Jinfnum);
    //    printf("JINF=%d TDR=%d -> POS=%d\n", Jinfnum, numnum, tdrnum);
    if(tdrnum<0) {
      printf("WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("JINF=%d TDR=%d\n", Jinfnum, numnum);
      printf("DecodeData::ReadOneTDR::Cannot-Find-TDR-in-CMP-or-RAW\n");
      //      exit(4);
      return -1;
    }
  }
  
  if(pri)
    printf("==========> TDR %02d, Jinf %02d, Size=%4d, TDR Status=%04hx: ", numnum, Jinfnum, size, array[size-1]);
  if (!out_flag) {
    if ((array[size - 1] & 64) > 0) {
      //      printf("RAW) numnum=%d, Jinfnum=%d, ntdrRaw=%d, ntdrCmp=%d\n", numnum, Jinfnum, ntdrRaw, ntdrCmp);
      tdrMap[ntdrCmp+ntdrRaw].first = numnum + 100 * Jinfnum;
      tdrMap[ntdrCmp+ntdrRaw].second = 0;
      ntdrRaw++;
    }
    if ((array[size - 1] & 128) > 0) {
      //      printf("CMP) numnum=%d, Jinfnum=%d, ntdrRaw=%d, ntdrCmp=%d\n", numnum, Jinfnum, ntdrRaw, ntdrCmp);
      tdrMap[ntdrRaw+ntdrCmp].first = numnum + 100 * Jinfnum;
      tdrMap[ntdrRaw+ntdrCmp].second = 1;
      ntdrCmp++;
    }
  }
  
  int count=5;
  for (int base=32; base<0x8000; base*=2)
    if(pri || evpri)
      printf(" %2d: %d |",count++,((array[size-1]&base)>0));
  if(pri || evpri)
    printf("\n");
  
  count=5;
  for (int base=32; base<0x8000; base*=2){
    if((array[size-1]&base)>0)
      if(pri)
	printf(" %02d: %s\n",count,errmess[count]);
    count++;
  }
  
  if(pri)
    printf("\n");
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
  
  if(out_flag)
    ev->TDRStatus[tdrnum]=array[size-1];
  if(out_flag)
    ev->ReadTDR[tdrnum]=1;
  
  int RawOffset=0;
  if (TESTBIT(array[size-1],6) ){
    //    printf("size = %d\n", size);
    //    sleep(10);
    RawOffset=1024;// RAW data present
    if (pri)
      printf("|->RAW data present\n");
    if (pri && out_flag)
      printf("Filling Event and Histograms for JINF %d, TDR %d (RAW)\n", Jinfnum, numnum);
    int tdrnumraw=0;
    int count=0;
    if (out_flag){
      tdrnumraw=FindPos(numnum+100*Jinfnum);
      if(tdrnumraw<0) {
	printf("DecodeData::ReadOneTDR::Cannot-Find-TDR-%d-RAW\n",numnum+100*Jinfnum);
	exit(4);
      }
      calib* cal=&(cals[numnum+100*Jinfnum]);
      for (int kk=0;kk<320;kk++){
	ev->RawSignal[tdrnumraw][kk]=array[count];      //first ADC on S
	ev->RawSignal[tdrnumraw][320+kk]=array[count+1];//second ADC on S
	ev->RawSignal[tdrnumraw][640+kk]=array[count+2];//ADC on K
	//	printf("RAW %d %d  %d\n",kk,ev->RawSignal[FindTDRPos(tdrnum)][kk],array[count]);
	count+=3;
      }
      for (int kk=960;kk<1024;kk++){//remaining (320->384) on ADC on K
	ev->RawSignal[tdrnumraw][kk]=array[kk];
      }
      for (int cc=0; cc<1024; cc++) {
	//	printf("%04d) %f %f %f -> %f\n", cc, ((double)ev->RawSignal[tdrnumraw][cc])/8.0, cal->ped[cc], cal->sig[cc], (ev->RawSignal[tdrnumraw][cc]/8.0-cal->ped[cc])/cal->sig[cc]);
	ev->CalPed[tdrnumraw][cc] = cal->ped[cc];
	ev->CalSigma[tdrnumraw][cc] = cal->sig[cc];
	ev->CalStatus[tdrnumraw][cc] = cal->status[cc];
	if (cal->sig[cc]>0.125 && //not a dead channel
	    cal->sig[cc]<10.0) {//not a noisy channel
	  ev->RawSoN[tdrnumraw][cc] = (ev->RawSignal[tdrnumraw][cc]/8.0-cal->ped[cc])/cal->sig[cc];
	}
	else {
	  ev->RawSoN[tdrnumraw][cc] = 0.0;
	}
      }

      for (int cc=0; cc<1024; cc++) {
	//	if (!kClusterize) {//otherwise the histos will be filled better with the clusters
        if (!(                                 //reverse the following conditions/staments
	      TESTBIT(array[size - 1], 7) || //if we're purely RAW is better to fill the histograms also in this "naive" way (weighting of strip by signal). But if we're also in CMP (-> we're in MIXED), the compressed ones would be better
	      kClusterize                    //if we're clusterizing the RAW event is better to fill the histograms with the clusters
					       )) {
	  double threshold = shighthreshold;
	  if (cc>=640)
	    threshold = khighthreshold;
	  if (ev->RawSoN[tdrnumraw][cc]>threshold) {
	    //	    printf("%04d) %f %f %f -> %f\n", cc, ((double)ev->RawSignal[tdrnumraw][cc])/8.0, cal->ped[cc], cal->sig[cc], (ev->RawSignal[tdrnumraw][cc]/8.0-cal->ped[cc])/cal->sig[cc]);
	    // printf("%04d) %f\n", cc, ev->RawSoN[tdrnumraw][cc]);
	    // this fills the histogram for the raw events when NOT clustering, if kClusterize anyhow, ALL the histos as for the compressed data, will be filled
	    hocc[numnum+NTDRS*Jinfnum]->Fill(cc, ev->RawSoN[tdrnumraw][cc]);
	    // hoccseed not filled in this case...
	    // hcharge not filled in this case...
	    // hsignal not filled in this case...
	    // hson not filled in this case...
	  }
	}
      }
      
      //      printf("%d %f %f %f %f\n", kClusterize, shighthreshold, slowthreshold, khighthreshold, klowthreshold);  

      //this searches for clusters and if found Fill the histograms as in the CMP case
      if (kClusterize)
	Clusterize(numnum, Jinfnum, cal);
    }
    
  }
  
  if (TESTBIT(array[size-1],7)){    // Compressed data present
    if (pri)
      printf("|->Compressed data present\n");
    if (pri && out_flag)
      printf("Filling Event and Histograms for JINF %d, TDR %d (CMP)\n", Jinfnum, numnum);
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
      if(pri)
	printf("Cluster: add=%d  lenght=%d\n", clusadd, cluslen);
      for(int hh=0; hh<cluslen; hh++){
	if (pri)
	  printf("Signal: %d, Pos:%d\n", array[count], hh);
	if (hh<MAXLENGHT){
	  sig[hh]=array[count]/8.;
	  if (pri)
	    printf("        %f, Pos: %d\n", sig[hh], hh);
	}
	else
	  bad=1;
	count++;
      }
      
      if(out_flag){
	//	if (!kClusterize) {//what happens if is mixed mode? I would write the same cluster twice... So let's NOT write the "on-line" clusters in the root file but only the "offline" ones... This remove the "online" clusters even if is not 'mixed' but only 'compressed' but I think is 'safer'...
	if (
	    !(TESTBIT(array[size - 1], 6)) //we're only in compressed
	    ||                             // OR
	    (
	     TESTBIT(array[size - 1], 6) && //we're ALSO in RAW mode --> mixed. There's also the raw event, so this cluster would be potentially found also by the offline clusterization. This would cause a 'double counting'
	     !kClusterize)                  //we're not clusterizing offline, so is safe to AddCluster and is needed otherwise the cluster would be not present at all in the Tree
	    ) {
	  
	  AddCluster(numnum, Jinfnum, clusadd, cluslen, Sig2NoiStatus, CNStatus, PowBits, bad, sig);
	}
      }
    }
    
    //      int cc=1;
    //      //Read the  cnoise values
    //      for (int ii=size-17;ii<size-1;ii++){
    //        if(pri)printf("Cnoise %d:  %f \n",cc++,(array[ii])/8.);
    //        if(out_flag)ev->CNoise[tdrnum][ii-size+17]=(array[ii])/8.;
    //      }
  }
  if(pri)
    printf("\n");
  if(pri)
    printf("End of the TDR pos:%ld  \n",ftell(rawfile)/2);
  //free the memory for the next tdr
  free(array);
    
  return 0;
}

void DecodeData::AddCluster(int numnum, int Jinfnum, int clusadd, int cluslen, int Sig2NoiStatus, int CNStatus, int PowBits, int bad, float *sig, bool kRaw) {
  //  pri=1;
  
  static LadderConf *ladderconf = Event::GetLadderConf();
  int _bondingtype = 0;
  
  if (cworkaround != 0) {
    _bondingtype = cworkaround;
  }
  else {
    _bondingtype = ladderconf->GetBondingType(Jinfnum, numnum);
  }
  
  //  printf("bondingtype = %d\n", _bondingtype);
  
  int newclusadd = clusadd;
  
  if (!kRaw) { //otherwise the swap has been already done when clustering (if we were clustering, otherwise the cluster is not present and we never reach this function...)
    if (_bondingtype == 2) {
      // if (clusadd>=(3*64) && clusadd<(5*64)) newclusadd+=3*64;
      // if (clusadd>=(5*64) && clusadd<(8*64)) newclusadd-=2*64;
      // questo cura in parte dei problemi sui bordi ancora da capire
      if (clusadd >= (3 * 64) && (clusadd + cluslen - 1) < (5 * 64))
        newclusadd += 3 * 64;
      if ((clusadd + cluslen - 1) >= (5 * 64) && clusadd < (8 * 64))
        newclusadd -= 2 * 64;
    }
  }  
  
  int sid=0;
  if (!kMC && clusadd>=640)
    sid=1;  
  if (kMC)
    sid=!tdrAlign[numnum];// check alignment. MD: why we need this in the MC case?
  
  Cluster* pp= ev->AddCluster(numnum+100*Jinfnum,sid);
  calib* cal=&(cals[numnum+100*Jinfnum]);
  
  //  pp->Build(numnum+100*Jinfnum,sid,clusadd,cluslen,sig,&(cal->sig[clusadd]),
  //	    &(cal->status[clusadd]), Sig2NoiStatus, CNStatus, PowBits, bad);
  // ONLY the 3rd field should be changed (clusadd->newclusadd) to move the cluster.
  // The 'clusadd' passed to the array should be left as it is to read the same signal values
  // for the 'sig' array there's no problem since already starting from 0
  pp->Build(numnum + 100 * Jinfnum, sid, newclusadd, cluslen, sig, &(cal->sig[clusadd]),
            &(cal->status[clusadd]), Sig2NoiStatus, CNStatus, PowBits, bad);
  
  double cog = pp->GetCoG();
  double seedadd = pp->GetSeedAdd();

  hocc[numnum + NTDRS * Jinfnum]->Fill(cog);
  hoccseed[numnum + NTDRS * Jinfnum]->Fill(seedadd);
  //#define TOTCHARGE
#ifndef TOTCHARGE
  hcharge[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetSeedCharge());
  hsignal[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetSeedVal());
  hchargevsocc[numnum + NTDRS * Jinfnum]->Fill(cog, pp->GetSeedCharge());
  hsignalvsocc[numnum + NTDRS * Jinfnum]->Fill(cog, pp->GetSeedVal());
  hson[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetSeedSN());
#else
  hcharge[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetCharge());
  hsignal[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetTotSig());
  hchargevsocc[numnum + NTDRS * Jinfnum]->Fill(cog, pp->GetCharge());
  hsignalvsocc[numnum + NTDRS * Jinfnum]->Fill(cog, pp->GetTotSig());
  hson[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetTotSN());
#endif

  // if (pp->GetSeedSN()<3.5) {
  //     printf("%s", stringtodump.Data());
  //   for (int ii=0; ii<cluslen; ii++) {
  //     printf("AddCluster) %d %f\n", ii, sig[ii]);
  //   }
  //  printf("***** SoN: %f, Sig: %f, SeedAdd: %d side %d \n", pp->GetSeedSN(), pp->GetSeedVal(), pp->GetSeed(), pp->side);
     //   pp->Print();
     // sleep(3);
  // }
  // if (pp->GetSeedVal()<1.0) {
  //   printf("%s", stringtodump.Data());
  //   for (int ii=0; ii<cluslen; ii++) {
  //     printf("AddCluster) %d %f\n", ii, sig[ii]);
  //   }
  //   printf("***** SoN: %f, Sig: %f, SeedAdd: %d\n", pp->GetSeedSN(), pp->GetSeedVal(), pp->GetSeed());
  //   pp->Print();
  //   sleep(3);
  // }

  if (pri)
    pp->Print();
  
  return;
}

void DecodeData::Clusterize(int numnum, int Jinfnum, calib* cal) {

  // printf("numnum = %d\n", numnum);
  
  // pri=1;
  
  int _bondingtype=0;
  bool defaultThresholds = (shighthreshold == 3.5 && khighthreshold == 3.5 && slowthreshold == 1.0 && klowthreshold == 1.0);
  
  static LadderConf* ladderconf = Event::GetLadderConf();
  
  static bool printed = false;
  if (!printed){
    printf("\nClustering with:\n");
    printf("    %f %f for S-side\n", shighthreshold, slowthreshold);
    
    printf("    %f %f for K-side\n", khighthreshold, klowthreshold);
    if(!defaultThresholds){
      printf("    thresholds used FOR ALL THE LADDERS\n");
    }    
    
    if(defaultThresholds){
      printf("Except for: \n");
      
      for (int jj = 0; jj < NJINF; jj++) { 
	for (int tt = 0; tt < NTDRS; tt++) { 	    
	  double shithresh = ladderconf->GetSHiThreshold(jj,tt);
	  double khithresh = ladderconf->GetKHiThreshold(jj,tt);
	  double slothresh = ladderconf->GetSLoThreshold(jj,tt);
	  double klothresh = ladderconf->GetKLoThreshold(jj,tt);
	  
	  if(shithresh != 3.5 || khithresh != 3.5 || slothresh != 1.0 || klothresh != 1.0){
	    printf("    JINF=%d, TDR=%d\n", jj, tt);
	    printf("    %f %f for S-side\n", shithresh, slothresh);
	    printf("    %f %f for K-side\n", khithresh, klothresh);
	  }
	}
      }
    }
    
    if (cworkaround != 0) {
      printf("\n    %d as workaround FOR ALL THE LADDERS\n", cworkaround);
      _bondingtype = cworkaround;
    }
    else {
      printf("\n");
      for (int jj = 0; jj < NJINF; jj++) {
	for (int tt = 0; tt < NTDRS; tt++) {
	  if (ladderconf->GetBondingType(jj, tt) != 0)
	    printf("    %d as workaround for JINF=%d, TDR=%d\n", ladderconf->GetBondingType(jj, tt), jj, tt);
	}
      }
    }
    printed = true;
  }
  
  _bondingtype = ladderconf->GetBondingType(Jinfnum, numnum);
  
  int tdrnumraw=FindPos(numnum+100*Jinfnum);

  //// numnum -> mapped to find the ntdr(=nlayer)
  if(kMC)
    tdrnumraw=numnum;

  //// nvas were 16 total summing S and K
  //  int nvasS=10;
  //  int nvasK= 6;
  int nvasS=NVAS;// changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  int nvasK=NVAS;// changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  //// hardcoded nchannels per va was 64
  //  int nchavaS=64;
  //  int nchavaK=64;
  // defined in 
  int nchavaS=NCHAVA;// changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  int nchavaK=NCHAVA;// changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  if (_bondingtype==1) {
    nchavaS=32;
  }
  
  int nvas=NVAS;//changed by Viviana
  int nchava=NCHAVA;//changed by Viviana
  //// hardcoded nchannels per tdr was 1024
  short int array[nvas*nchava];//changed by Viviana
  float arraySoN[nvas*nchava];//changed by Viviana
  float pede[nvas*nchava];//changed by Viviana
  float sigma[nvas*nchava];//changed by Viviana
  int status[nvas*nchava];//changed by Viviana
  int arraysize=nvas*nchava;//changed by Viviana
  
  double highthreshold=3.5;
  double lowthreshold=1.0;

  int shift=0;

  TString headerstringtodump = "New clusterize:\n";
  bool added=false;//added by Viviana
  
  for (int side=0; side<2; side++) {
    if(added) continue;   
    if (side==0) {
      nvas=nvasS;
      nchava=nchavaS;
      if(defaultThresholds){
	highthreshold = ladderconf->GetSHiThreshold(Jinfnum,numnum);
	lowthreshold = ladderconf->GetSLoThreshold(Jinfnum,numnum);
      }
      else {
	highthreshold = shighthreshold;
	lowthreshold = slowthreshold;
      }
      
      shift = 0;
      if (_bondingtype==1) {
	arraysize=320;
	for (int cc=0; cc<arraysize; cc++) {
	  array[cc] = ev->RawSignal[tdrnumraw][cc*2];
	  arraySoN[cc] = ev->RawSoN[tdrnumraw][cc*2];
	  pede[cc] = cal->ped[cc*2];
	  sigma[cc] = cal->sig[cc*2];
	  status[cc] = cal->status[cc*2];
	}
      }
      else if (_bondingtype==2) {
	arraysize=384;
	int halfarraysize=((int)(arraysize/2));
	int shift=((int)(640/2));
	for (int cc=0; cc<halfarraysize; cc++) {
	  //	  printf("%d %d %d\n", cc, cc+halfarraysize, cc+shift);
	  array[cc] = ev->RawSignal[tdrnumraw][cc];
	  arraySoN[cc] = ev->RawSoN[tdrnumraw][cc];
	  pede[cc] = cal->ped[cc];
	  sigma[cc] = cal->sig[cc];
	  status[cc] = cal->status[cc];
	  array[cc+halfarraysize]    = ev->RawSignal[tdrnumraw][cc+shift];
	  arraySoN[cc+halfarraysize] = ev->RawSoN[tdrnumraw][cc+shift];
	  pede[cc+halfarraysize]     = cal->ped[cc+shift];
	  sigma[cc+halfarraysize]    = cal->sig[cc+shift];
	  status[cc+halfarraysize]   = cal->status[cc+shift];
	}
      }
      else {
	//arraysize=640;
	arraysize=nvas*nchava;//changed by Viviana
	memcpy(array, &(ev->RawSignal[tdrnumraw][0]), arraysize*sizeof(ev->RawSignal[tdrnumraw][0]));
	memcpy(arraySoN, &(ev->RawSoN[tdrnumraw][0]), arraysize*sizeof(ev->RawSoN[tdrnumraw][0]));
	memcpy(pede, &(cal->ped[0]), arraysize*sizeof(cal->ped[0]));
	memcpy(sigma, &(cal->sig[0]), arraysize*sizeof(cal->sig[0]));
	memcpy(status, &(cal->status[0]), arraysize*sizeof(cal->status[0]));
	if(kMC){//added by Viviana
	  added=true;
	}
      }
    } // side=0
    else {
      if (_bondingtype==2) {
	continue;
      }
      else {
	nvas=nvasK;
	nchava=nchavaK;

	if(defaultThresholds){
	  highthreshold = ladderconf->GetKHiThreshold(Jinfnum,numnum);
	  lowthreshold = ladderconf->GetKLoThreshold(Jinfnum,numnum);
	}
	else {        
	  highthreshold = khighthreshold;
	  lowthreshold = klowthreshold;
	}
	
	shift=!kMC?640:0;//changed by Viviana
	arraysize=!kMC?384:nvas*nchava;//changed by Viviana
	////arraysize=!kMC?384:1024;//changed by Viviana
	//the src is the same array as in the S-side case but passing the reference to the first element of K-side (640)
	
	memcpy(array, &(ev->RawSignal[tdrnumraw][shift]), arraysize*sizeof(ev->RawSignal[tdrnumraw][0]));
	memcpy(arraySoN, &(ev->RawSoN[tdrnumraw][shift]), arraysize*sizeof(ev->RawSoN[tdrnumraw][0]));
	memcpy(pede, &(cal->ped[shift]), arraysize*sizeof(cal->ped[0]));
	memcpy(sigma, &(cal->sig[shift]), arraysize*sizeof(cal->sig[0]));
	memcpy(status, &(cal->status[shift]), arraysize*sizeof(cal->sig[0]));
      }
    }
    
    double CN[nvas];
    
    for (int va=0; va<nvas; va++) {
      CN[va] = Event::ComputeCN(nchava, &(array[va * nchava]), &(pede[va * nchava]), &(arraySoN[va * nchava]), &(status[va * nchava]));
      //      printf("%d) %f\n", va, CN[va]);
      //      headerstringtodump += Form("CN[%d] = %f\n", va, CN[va]);
    }
    
    int bad=0;
    int cluslen=0;//to fill
    int Sig2NoiStatus=0;//boh
    int clusadd=0;//to fill
    int CNStatus=0;//boh
    int PowBits=0;//boh
    float sig[MAXLENGHT];

    bool firstfound=false;
    bool seedfound=false;
    
    float ssonmax = -999;
    int seedaddmax = -1;
    
    TString clusterstringtodump = "--> New cluster\n";
    //    printf("--> New cluster\n");
    
    for (int count=0; count<arraysize; count++) {

      if (status[count]) continue;
      
      // printf("count = %d\n", count);
      //      clusterstringtodump += Form("count = %d\n", count);
      
      int va = (int)(count/nchava);
      
      float ssun = (array[count]/8.0-pede[count]-CN[va])/sigma[count];
      //if (ssun>highthreshold) printf("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va], sigma[count], ssun);

      //printf("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va], sigma[count], ssun);
       
      //      clusterstringtodump += Form("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va], sigma[count], ssun);      

      if (ssun>=highthreshold) {//the seed that can also be the first of the cluster
	//	printf("%d) >high\n", count);
	//	clusterstringtodump += Form("%d) >high\n", count);
	//	clusterstringtodump += Form("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va], sigma[count], ssun);
	
	if (ssun>ssonmax) {
	  seedaddmax = count;
	  ssonmax = ssun;
	  //	  printf("%d) New max %d %f\n", count, seedaddmax, ssonmax);
	  //	  clusterstringtodump += Form("%d) New max %d %f\n", count, seedaddmax, ssonmax);
	}
	
	if (firstfound) { //this is the seed (maybe there was another seed previously, but doesn't matter...)
	  seedfound=true;
	  cluslen++;
	  //	  printf("%d) >high and first found already (cluslen=%d)\n", count, cluslen);
	  //	  clusterstringtodump += Form("%d) >high and first found already (cluslen=%d)\n", count, cluslen);
	}
	else {//is the seed but also the first of the cluster...
	  firstfound=true;
	  seedfound=true;
	  clusadd=count;
	  cluslen=1;
	  //	  printf("%d) >high, is the seed and also the first of the cluster (cluslen=%d)\n", count, cluslen);
	  //	  clusterstringtodump += Form("%d) >high, is the seed and also the first of the cluster (cluslen=%d)\n", count, cluslen);
	}
      }
      else if (ssun>=lowthreshold) { //potentially the start of a cluster, or maybe another neighbour...
	//	clusterstringtodump += Form("%d) >low\n", count);
	if (!firstfound) {//is the first of the potential cluster
	  firstfound=true;
	  clusadd=count;
	  cluslen=1;
	  //	  printf("%d) >low, is the first of the potential cluster (cluslen=%d)\n", count, cluslen);
	  //	  clusterstringtodump += Form("%d) >low, is the first of the potential cluster (cluslen=%d)\n", count, cluslen);
	}
	else {//there was already a 'first' so this can be a neighbour between the first and the seed or a neighbour after
	  cluslen++;
	  //	  printf("%d) >low but firstfound already (cluslen=%d)\n", count, cluslen);
	  //	  clusterstringtodump += Form("%d) >low but firstfound already (cluslen=%d)\n", count, cluslen);
	}
      }
      else if (ssun<lowthreshold || count==(arraysize-1)) { //end of a cluster or end of a "potential" cluster or simply nothing
	//	printf("%d) <low (cluslen=%d)\n", count, cluslen);
	//	clusterstringtodump += Form("%d) <low (cluslen=%d)\n", count, cluslen);
	if (seedfound) {//the cluster is done, let's save it!
	  //	  printf("%d) <low, seed found already: let's save the cluster (cluslen=%d)\n", count, cluslen);
	  //	  clusterstringtodump += Form("%d) <low, seed found already: let's save the cluster (cluslen=%d)\n", count, cluslen);
	  if (pri) printf("Cluster: add=%d  lenght=%d, seed=%d\n", clusadd+shift, cluslen, seedaddmax+shift);
	  //	  clusterstringtodump += Form("Cluster: add=%d  lenght=%d, seed=%d\n", clusadd+shift, cluslen, seedaddmax+shift);
	  for (int hh=clusadd; hh<(clusadd+cluslen); hh++){
	    int _va = (int)(hh/nchava);
	    float s = array[hh]/8.0-pede[hh]-CN[_va];
	    //	    clusterstringtodump += Form("Sig=%f from Array = %d, Ped=%f, CN=%f\n", s, array[hh], pede[hh], CN[_va]);
	    if (pri)
	      printf("Signal: %d, Pos:%d\n", (int)(8*s), hh+shift);
	    //	    clusterstringtodump += Form("Signal: %d, Pos:%d\n", (int)(8*s), hh+shift);
	    if ((hh-clusadd)<MAXLENGHT){
	      sig[hh-clusadd]=s;
	      if (pri)
		printf("        %f, Pos: %d\n", sig[hh-clusadd], hh+shift);
	      //	      clusterstringtodump += Form("        %f, Pos: %d\n", sig[hh-clusadd], hh+shift);
	    }
	    else
	      bad=1;
	  }
	  //	  clusterstringtodump += Form("Status[%d] = %d\n", seedaddmax+shift, status[seedaddmax]);
	  stringtodump = headerstringtodump + clusterstringtodump;
	  if (!(status[seedaddmax]&(1<<3))) {//if is not a bad cluster
	    //	    printf("numnum = %d\n", numnum);
	    AddCluster(numnum, Jinfnum, clusadd+shift, cluslen, Sig2NoiStatus, CNStatus, PowBits, bad, sig);
	  }
	  clusterstringtodump = "--> New cluster\n";
	}
	// there was no seed found: "potential" cluster not promoted or even "nothing"
	seedfound=false;
	firstfound=false;
	memset(sig, 0, MAXLENGHT*sizeof(sig[0]));
	ssonmax = -9999;
	seedaddmax = -1;
	stringtodump = headerstringtodump + clusterstringtodump;
	clusterstringtodump = "--> New cluster\n";
      }

    }
  }
  
  return;
}

/*
double DecodeData::ComputeCN(int size, short int* RawSignal, float* pede, float* RawSoN, double threshold){

   double mean=0.0;
  int n=0;
  
  for (int ii=0; ii<size; ii++) {
    if (RawSoN[ii]<threshold) {//to avoid real signal...
      n++;
      //      printf("    %d) %f %f\n", ii, RawSignal[ii]/8.0, pede[ii]);
      mean+=(RawSignal[ii]/8.0-pede[ii]);
    }
  }
  if (n>1) {
    mean/=n;
  }
  else { //let's try again with an higher threshold
    mean = ComputeCN(size, RawSignal, pede, RawSoN, threshold+1.0);
  }
  //  printf("    CN = %f\n", mean);

  return mean;
}*/

//=============================================================================================

void DecodeData::FindCalibs(){

  struct stat buf;
  char name1[300];
  char nameA[300];
  char nameB[300];
  char nameMC[300];
  int run2;
  int runA;
  int runB;
  int runMC=0;// added by Viviana. MD: see if really needed
  
  FILE *calfile[NTDRS];
  int old = pri;
  bool afterclose = false;

  if(!kMC){ 
    if ((ntdrCmp + ntdrRaw) < 1) {
      printf("No TDR in CMP or RAW mode --> No DSP Calib can be found\n");
      return;
    }
    
    int tdrprobe = tdrMap[0].first;
    
    for (runB = runn; runB > 0; runB--) {
      sprintf(nameB, "%s/%06d_%04d.cal", rawCaldir, runB, tdrprobe);
      //    printf("%s\n", nameB);
      if (stat(nameB, &buf) == 0) {
	//      printf("First calib Found %s run %d\n", name1, run2);
	break;
      }
    }
    
    for (runA = runn; runA < (runn + (runn - runB)); runA++) {
      sprintf(nameA, "%s/%06d_%04d.cal", rawCaldir, runA, tdrprobe);
      //    printf("%s\n", nameA);
      if (stat(nameA, &buf) == 0) {
	//      printf("First calib Found %s run %d\n", name1, run2);
	afterclose = true;
	break;
      }
    }
    
    if (afterclose) {
      run2 = runA;
      sprintf(name1, "%s/%06d_%04d.cal", rawCaldir, runA, tdrprobe);
    }
    else {
      run2 = runB;
      sprintf(name1, "%s/%06d_%04d.cal", rawCaldir, runB, tdrprobe);
    }
    
    printf("Closest calib Found %s run %d\n", name1, run2);

    if (run2 < 40) {
      printf("Cannot find any calibration done before the requested run %d\n", runn);
      printf("I give up. Bye.\n");
      exit(2);
    }
    else
      printf("Searching the calib files for the other TDRs, if any\n");
    
    for (int ii = 0; ii < ntdrCmp+ntdrRaw; ii++) {
      int Jinfnum = tdrMap[ii].first / 100;
      int tdrnum = tdrMap[ii].first - Jinfnum * 100;
      int index = Jinfnum * 100 + tdrnum;
      sprintf(name1, "%s/%06d_%02d%02d.cal", rawCaldir, run2, Jinfnum, tdrnum);
      calfile[index] = fopen(name1, "r");
      if (!calfile[index]) {
	printf("Cannot find the calib %s for the requested calib run %d\n", name1, run2);
	printf("I give up. Bye.\n");
	exit(2);
      }
      printf("Found file %s\n",name1);
      //    printf("---- %d\n", index);
      ReadCalib(calfile[index],&(cals[index]));
    }
  }// end if !kMC
  else {// for MC
    printf("%scalMC_%04d.cal TDRS %d\n", rawCaldir,runMC, ntdrMC);      
    for (int iic=0;iic<ntdrMC;iic++){
      //sprintf(nameMC,"%s/calMC_%04d.cal", rawCaldir, iic);
      sprintf(nameMC,"%scalMC_0000.cal", rawCaldir);
      printf("%scalMC_0000.cal \n", rawCaldir);      
      calfile[iic]=fopen(nameMC,"r");
      printf("Found file %s for MCTDR %d \n",nameMC,iic);
      ReadCalib_mc(calfile[iic],&(cals[iic]));
    }
  }
  
  pri=old;
}

//=============================================================================================

int  DecodeData::ReadCalib(FILE * fil,calib* cal){
  
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
    if (pri)
      printf("%d  %f %f %f %d\n",ii,cal->ped[ii],cal->rsig[ii],cal->sig[ii],cal->status[ii]);
  }
  
  return 1;
}

//=============================================================================================

int  DecodeData::ReadCalib_mc(FILE * fil,calib* cal){
  // for now keeping kind of data like calibration file.. 
  
  //int a,b,c;
  //	float d,e;
  int old=1;
  old=pri;
  pri=0;
  
  /* for (int ii=0;ii<24;ii++)
     fscanf(fil,"%d, %f, %f",&a,&d,&e);
  */
  
  ////for (int ii=0;ii<1024;ii++){  
  for (int ii=0;ii<4096;ii++){
    
    /*		
		fscanf(fil,"%d  %d  %d  %f %f %f %f %d  ",&a,&b,&c,
		&(cal->ped[ii]),
		&(cal->rsig[ii]),
		&(cal->sig[ii]),
		&d,
		&(cal->status[ii]));
    */
    
    //// fix values for now
    cal->ped[ii]=420.;
    cal->rsig[ii]=1.250;
    cal->sig[ii]=1.125;
    cal->status[ii]=0;
    
    //if(pri)printf("%d  %f %f %f %d\n",ii,cal->ped[ii],cal->rsig[ii],cal->sig[ii],cal->status[ii]);
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
  
  //Read The Event Size
  if (pri)
    printf("pos:%ld  ",ftell(rawfile)/2);
  ReadFile(&size,sizeof(size),1,rawfile);
  if(pri)
    printf("JINFsize    %d \n",size);
  
  long int base=ftell(rawfile)/2;
  
  if (pri)
    printf("pos:%ld  \n",ftell(rawfile)/2);
  
  //Store the position on the file
  long int pos=ftell(rawfile);
  
  // skip all the TDR data
  if(int ret0=fseek(rawfile, (size-1)*2, SEEK_CUR)) {
    printf("Fatal: error during file skip ret0=%d \n",ret0);
    exit(3);
  }
  
  // read the last word of the JINF event
  if(pri)
    printf("pos:%ld  ",ftell(rawfile)/2);
  
  ReadFile(&status,sizeof(status), 1, rawfile);
  
  if (pri || evpri)
    printf("Jinf status    %d : address %02d  status ", status, status&0x1f); 
  
  int Jinfnum=((unsigned int)status)&0x1f;//e questo numero se non c'è un jinj master non sò chi glielo potrebbe aver messo (però se sta nello status che comunque ci sarà, al limite sarà 0...) 
  
  int count=1;
  for (int base=32;base<0x8000;base*=2)
    if (pri || evpri)
      printf(" %2d %d",count++,((status&base)>0));
  if (pri || evpri)
    printf("\n");
  
  count=5;
  for (int base=32;base<0x8000;base*=2){
    if ((status&base)>0)
      if (pri || evpri)
	printf("%02d: %s\n",count,errmess[count]);
    count++;
  }
  if( pri || evpri)
    printf("\n \n");
  
  if (out_flag)
    ev->JINFStatus[Jinfnum]=status;
  
  //  if((status&0x400)>0){
  if ((status&0x200)>0||(status&0x400)>0){
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
  if (int ret0=fseek(rawfile,pos,SEEK_SET)){
    printf("Fatal: error during file skip %d\n",ret0);
    exit(3);
  }
  
  if (!out_flag)
    JinfMap[nJinf++]=status&0x1f;
  
  // Read the TDR
  while ( ftell(rawfile)/2< base+size-1 )
    ReadOneTDR(status&0x1f);
  // Read the last 2 words before status
  ReadFile(&noknown,sizeof(noknown),1,rawfile);
  ReadFile(&tdrnoeventmask,sizeof(tdrnoeventmask),1,rawfile);
  if (pri || evpri)
    printf("Tdrs with no event Mask: %d\n", tdrnoeventmask); 
  for (int ii=0; ii<NTDRS; ii++){
    if (tdrnoeventmask&(1<<ii)){
      if (pri || evpri)
	printf("A tdr (%d) replied with no event...\n",ii);
      if(!out_flag) {
	tdrMap[ntdrRaw+ntdrCmp].first = ii + 100 * (status & 0x1f);
	tdrMap[ntdrRaw+ntdrCmp].second = 1;
	ntdrCmp++;
      }
    }
  }
  
  // Reread the last word
  ReadFile(&status,sizeof(status),1,rawfile);
  if( pri || evpri)
    printf("Jinf status    %d \n ",status);
  
  return 0;
}

int DecodeData::GetTdrNum(int pos) {
  if (pos > NJINF * NTDRS)
    {
      printf("Pos %d not allowed. Max is %d\n", pos, NJINF * NTDRS);
      return -9999;
    }
  return tdrMap[pos].first;
}

int DecodeData::GetTdrType(int pos){
  if (pos > NJINF * NTDRS)
    {
      printf("Pos %d not allowed. Max is %d\n", pos, NJINF * NTDRS);
      return -9999;
    }
  return tdrMap[pos].second;
}

//=============================================================================================

int DecodeData::ReadFile(void * ptr, size_t size, size_t nitems, FILE * stream){
  
  int ret=0;
  ret=fread(ptr,size,nitems,stream);
  if (feof(stream)) {
    printf("End of File \n");
    return -1;
  }
  if (ferror(stream)) {
    printf("Error reading \n");
    return -2;
  }
  
  return ret;
}

void DecodeData::mysort(laddernumtype *aa, int nel){
  
  laddernumtype* bb = new laddernumtype[nel];
  
  int min = 10000;
  int max = -1;
  for (int count = 0; count < nel; count++) {
    for (int ii = 0; ii < nel; ii++) {
      if (aa[ii].first < min && aa[ii].first > max) {
	min = aa[ii].first;
	bb[count] = aa[ii];
      }
    }
    max = bb[count].first;
    min = 10000;
  }
  
  for (int count = 0; count < nel; count++)
    aa[count] = bb[count];
  
  delete[] bb;
}
