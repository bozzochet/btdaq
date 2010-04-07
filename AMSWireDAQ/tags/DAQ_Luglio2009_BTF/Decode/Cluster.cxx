#include "Cluster.hh"
#include <cmath>
#include <string.h>

ClassImp(Cluster);


Cluster::Cluster(){
  address=0;
  length=0;
  memset(Signal,0,MAXLENGHT*sizeof(Signal[0]));
  memset(Noise,0,MAXLENGHT*sizeof(Noise[0]));
  memset(Status,0,MAXLENGHT*sizeof(Status[0]));
  bad=0;
  golden=0;
  ladder=-1;
  side=-1;
}

Cluster::Cluster(Cluster & orig):TObject(orig){

  ladder=orig.ladder;
  side=orig.side;
  address=orig.address;
  length=orig.length;
  for (int ii=0;ii<MAXLENGHT;ii++){
    Signal[ii]=orig.Signal[ii];
    Noise[ii]=orig.Noise[ii];
    Status[ii]=orig.Status[ii];
  }
  bad=orig.bad;
  golden=orig.golden;
}


void Cluster::Build(int lad, int sid, int add, int len, float* sig, float* noi, int* stat, int Sig2NoiRatio, int CNStatus, int PowBits, int badin){

  ladder=lad;
  side=sid;
  address=add;
  if(len>MAXLENGHT) length=MAXLENGHT;
  else length=len;
  for (int ii=0;ii<length;ii++){
    Signal[ii]=sig[ii];
    Noise[ii]=noi[ii];
    Status[ii]=stat[ii];
  }
  snratio=Sig2NoiRatio;
  CNstatus=CNStatus;
  powbits=PowBits;
  bad=badin;
  
  return;
}

void Cluster::Clear(){
  address=0;
  length=0;
  memset(Signal,0,MAXLENGHT*sizeof(Signal[0]));
  memset(Noise,0,MAXLENGHT*sizeof(Noise[0]));
  memset(Status,0,MAXLENGHT*sizeof(Status[0]));
  bad=0;
  ladder=-1;
  side=-1;

}


int Cluster::GetSeed(){
  float max=-1000;
  int seed=-1;
  for (int ii=0;ii<length;ii++){
    if (Signal[ii]/Noise[ii]>=max&&Status[ii]==0
	&&!(ladder==5&&(ii+address)==380)
	&&!(ladder==5&&(ii+address)==381)
	){
      max=Signal[ii]/Noise[ii];
      seed=ii;
    }
  }
  return seed;

}


int Cluster::GetSeedAdd(){
  return address+GetSeed();
}

float Cluster::GetCSignal(int aa){
 //  int stadd=address+aa;
//   int vanum=stadd/64;
//   float c1= Signal[aa];
//   float corr=0.;
//   if(aa>0){
//     int stadd2=address+aa-1;
//     int vanum2=stadd2/64;
//     float c2= Signal[aa-1];
//     corr=c2*0.;
//   }
//   if (side==1) return c1-corr;
//   else return c1;
  return  Signal[aa];
}



int Cluster::GetLenght(float val){
  int se=GetSeed();
  int myle=1;
  for (int ii=se-1;ii>=0;ii--)
    if(GetCSignal(ii)/Noise[ii]>val) myle++;
    else break;
  for (int ii=se+1;ii<length;ii++)
    if(GetCSignal(ii)/Noise[ii]>val) myle++;
    else break;
  return myle;
  
}


float Cluster::GetTotNoise(){
  int se=GetSeed();
  int se_r=se+1;
  int se_l=se-1;
  float val_l=0;
  float val_r=0;
  if (se_l>=0)     val_l = Noise[se_l];
  else val_l=0;
  if (se_r<length) val_r = Noise[se_r];
  else val_r=0;
  if(val_l>val_r) return sqrt(val_l*val_l+Noise[se]*Noise[se]);
  else return sqrt(val_r*val_r+Noise[se]*Noise[se]);
}



float Cluster::GetTotSig(){
  int se=GetSeed();
  int se_r=se+1;
  int se_l=se-1;
  float val_l=0;
  float val_r=0;
  if (se_l>=0)     val_l = GetCSignal(se_l);
  else val_l=0;
  if (se_r<length) val_r = GetCSignal(se_r);
  else val_r=0;
  if(val_l>val_r) return val_l+GetCSignal(se);
  else return val_r+GetCSignal(se);
}


float Cluster::GetEtaRaw(){
  float ee;
  int se=GetSeed();
  int se_r=se+1;
  int se_l=se-1;
  float val_l=0;
  float val_r=0;
  float val_seed=GetCSignal(se);
  if (se_l>=0)     val_l = GetCSignal(se_l);
  if (se_r<length) val_r = GetCSignal(se_r);

  if(val_l>val_r && val_l/Noise[se_l]>0)      ee=(val_seed*se+val_l*se_l)/(val_l+val_seed);
  else if(val_r>val_l && val_r/Noise[se_r]>0) ee=(val_r*se_r+val_seed*se)/(val_r+val_seed);
  else return -3;

  return ee;
}


float Cluster::GetEta(){
  float ee=GetEtaRaw();
  if(ee<0) return -3;
  else return ee-trunc(ee);
}

float Cluster::GetCoG(){
  int se=GetSeed();
  float ee=GetEtaRaw();
  if(ee<0) return address+se;
  else return address+ee;
}

float Cluster::GetSeedVal(){
  return GetCSignal(GetSeed());
}

float Cluster::GetSeedSN(){
  return GetCSignal(GetSeed())/Noise[GetSeed()];
}

float Cluster::GetTotSN(){

return GetTotSig()/GetTotNoise();
}

int Cluster::GetLength(){
  return length;
}


void Cluster::Print(){

  printf(" ladd: %d  side: %d add: %d len: %d bad: %d \n",
         ladder,side,address,length,bad);
  printf(" Seed: %d SeedAddress: %d Seed:val %f SeedSN: %f \n",GetSeed(),GetSeedAdd(),GetSeedVal(),GetSeedSN());
  printf(" Strip:       Signal:     Noise:       Status: \n");
  for(int ii=0;ii<length;ii++){
    printf("    %d      %f       %f       %d \n",
	   ii, GetCSignal(ii), Noise[ii], Status[ii]);
  }
  return;
}


int Cluster::GoldRegion(){
  int lowgold[6][2]={{280,720},{50,650},{150,660},{160,660},{150,860},{220,660}};
  int  upgold[6][2]={{340,830},{150,850},{400,710},{460,710},{450,920},{480,710}};

  if(GetCoG()>lowgold[ladder][side]&&
     GetCoG()<=upgold[ladder][side]) return 1;
  else return 0;
}
