#include "Event.hh"
#include "Cluster.hh"

ClassImp(Event);

Event::Event(int irawtdrnum){

  rawtdrnum=irawtdrnum;
  Evtnum=0;
  JINJStatus=0;
  for (int ii=0;ii<NJINF;ii++)
    JINFStatus[ii]=0;
  
  for(int ii=0;ii<NTDRS;ii++){ 
    ReadTDR[ii]=0;
    TDRStatus[ii]=31;
    for(int jj=0;jj<16;jj++)    
      CNoise[ii][jj]=0;
    NClus[ii][0]=0;
    NClus[ii][1]=0;
  }
  for(int ii=0;ii<1024;ii++)
    for(int kk=0;kk<8;kk++)
      Signal[kk][ii]=0;
  
  //  RawLadder = new TClonesArray("RawData",rawtdrnum);
  
  NClusTot=0;
  notgood=0;
  Cls = new TClonesArray("Cluster",2);
  Cls->SetOwner();
  
}

Event::~Event(){
  if(Cls) {Cls->Delete(); delete Cls;}  
}

void Event::Clear(){
  JINJStatus=0;
  for (int ii=0;ii<NJINF;ii++)
    JINFStatus[ii]=0;
  
  NClusTot=0;

  for(int ii=0;ii<NTDRS;ii++){ 
    ReadTDR[ii]=0;
    TDRStatus[ii]=31;
    for(int jj=0;jj<16;jj++)    
      CNoise[ii][jj]=0;
    NClus[ii][0]=0;
    NClus[ii][1]=0; 
  }

  for(int ii=0;ii<8;ii++){ 
    for(int kk=0;kk<1024;kk++)
      Signal[ii][kk]=0;
  }

  if(Cls)  Cls->Delete();
  
  
  //   for (int ii=Cls->GetEntries();ii>-1;ii--){
  //     Cluster* ff=(Cluster*) Cls->RemoveAt(ii);
  //     if(ff) delete ff;
  //   }
  
  return;
}

Cluster* Event::AddCluster(int lad,int side){
  Cluster* pp=(Cluster*)Cls->New(NClusTot);
  
  NClus[lad][side]++;
  NClusTot++;
  return pp;
}


Cluster* Event::GetCluster(int ii){
  return (Cluster*)Cls->At(ii);
}

int Event::NGoldenClus(int lad, int side){
  int num=0;
  for (int ii=0;ii<NClusTot;ii++){
    Cluster* cl=GetCluster(ii);
    if(cl->ladder==lad&&cl->side==side&&cl->golden==1) num++;
  }
  return num;
}


//-------------------------------------------------------------------------------------


ClassImp(RHClass);

RHClass::RHClass(){
  Run=0;
  ntdrRaw=0;
  ntdrCmp=0;
  nJinf=0;
  sprintf(date," ");
  memset(JinfMap,-1,NJINF*sizeof(JinfMap[0]));
  memset(tdrRawMap,-1,NTDRS*sizeof(tdrRawMap[0]));
  memset(tdrCmpMap,-1,NTDRS*sizeof(tdrCmpMap[0]));
  
  for (int ii=0;ii<NTDRS;ii++)
    for (int jj=0;jj<16;jj++){
      CNMean[ii][jj]=0.;
      CNSigma[ii][jj]=0.;
    }
}

void RHClass::Print(){
  printf("---------------------------------------------\n");
  printf("The header says:\n");
  printf("Run: %d Date: %s\n",Run,date);
  printf("# Jinf = %d\n",nJinf);
  for (int ii=0;ii<nJinf;ii++)
    printf("Jinf Map pos: %d Jinf num: %d\n",ii,JinfMap[ii]);
  
  printf("# TDR RAW = %d\n",ntdrRaw);
  for (int ii=0;ii<ntdrRaw;ii++)
    printf("TDR RAW Map pos: %d tdrnum: %d\n",ii,tdrRawMap[ii]);
  
  printf("# TDR CMP = %d\n",ntdrCmp);
  for (int ii=0;ii<ntdrCmp;ii++)
    printf("TDR CMP Map pos: %d tdrnum: %d\n",ii,tdrCmpMap[ii]);
  //   for (int ii=0;ii<NTDRS;ii++){
  //     printf("TDR: %d\n",ii);
  //     for (int jj=0;jj<16;jj++)
  //       printf(" %6.2f ",CNMean[ii][jj]);
  //     printf(" \n");
  //     for (int jj=0;jj<16;jj++)
  //       printf(" %6.2f ",CNSigma[ii][jj]);
  //     printf(" \n");
  //     printf(" \n");
  //   }
  return;
}
