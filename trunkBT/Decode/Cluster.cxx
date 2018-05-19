#include "Cluster.hh"
#include <cmath>
#include <string.h>
#include "Event.hh"

ClassImp(Cluster);


Cluster::Cluster(){
  address=0;
  length=0;
  memset(Signal,0,MAXLENGHT*sizeof(Signal[0]));
  memset(Noise,0,MAXLENGHT*sizeof(Noise[0]));
  memset(Status,0,MAXLENGHT*sizeof(Status[0]));
  memset(SignalVAEqualized,0,MAXLENGHT*sizeof(SignalVAEqualized[0]));
  memset(NoiseVAEqualized,0,MAXLENGHT*sizeof(NoiseVAEqualized[0]));
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
    SignalVAEqualized[ii]=orig.SignalVAEqualized[ii];
    NoiseVAEqualized[ii]=orig.NoiseVAEqualized[ii];
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

  //  this->ApplyVAEqualization();

  return;
}

void Cluster::Clear(){
  address=0;
  length=0;
  memset(Signal,0,MAXLENGHT*sizeof(Signal[0]));
  memset(Noise,0,MAXLENGHT*sizeof(Noise[0]));
  memset(Status,0,MAXLENGHT*sizeof(Status[0]));
  memset(SignalVAEqualized,0,MAXLENGHT*sizeof(SignalVAEqualized[0]));
  memset(NoiseVAEqualized,0,MAXLENGHT*sizeof(NoiseVAEqualized[0]));
  bad=0;
  ladder=-1;
  side=-1;

}

int Cluster::GetAddress(){
  return address;
}

int Cluster::GetSeed(){
  float max=-1000;
  int seed=-1;
  for (int ii=0;ii<length;ii++){
    if (Noise[ii]!=0 && Signal[ii]/Noise[ii]>=max && Status[ii]==0){
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

float par0=Event::GetGainCorrectionPar(GetJinf(), GetTDR(), GetVA(aa), 0);
//float par1=Event::GetGainCorrectionPar(GetJinf(), GetTDR(), GetVA(aa), 1);
//float par2=Event::GetGainCorrectionPar(GetJinf(), GetTDR(), GetVA(aa), 2);

//float correctSignal = (Signal[aa]*par2*(1-par1)+par0*par2);
float correctSignal = (Signal[aa]+par0);

 return  correctSignal;
}

//why not simply 'return lenght'?
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


float Cluster::GetSig(){
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


float Cluster::GetTotSig(){
  float val=0;
  for (int ii=0; ii<length; ii++) {
    val += GetCSignal(ii);
  }
  return val;
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


int Cluster::GetVA(int strip_address){
  int vanum=0;

  vanum=strip_address/64;

  return vanum;
}

double Cluster::GetPitch(int side){
  return GetPitch(GetJinf(), GetTDR(), side);
};

double Cluster::GetNominalResolution(int side){
  return GetNominalResolution(GetJinf(), GetTDR(), side);
};

double Cluster::GetPitch(int jinfnum, int tdrnum, int side){
  // printf("Called Cluster::GetPitch(%d, %d, %d) = %lf\n", jinfnum, tdrnum, side, Event::GetLadderConf()->GetPitch(jinfnum, tdrnum, side));
  return Event::GetLadderConf()->GetPitch(jinfnum, tdrnum, side);
};

double Cluster::GetNominalResolution(int jinfnum, int tdrnum, int side){
  // printf("Called Cluster::GetNominalResolution(%d, %d, %d) = %lf\n", jinfnum, tdrnum, side, Event::GetLadderConf()->GetResolution(jinfnum, tdrnum, side));
  return Event::GetLadderConf()->GetResolution(jinfnum, tdrnum, side);
};


void Cluster::ApplyVAEqualization(){

  int jinfnum=GetJinf();
  int tdrnum=GetTDR();
  for(int ii=0;ii<length;ii++){
    int vanum=GetVA(address+ii);
    SignalVAEqualized[ii]=
      Signal[ii]*Event::GetGainCorrectionPar(jinfnum,tdrnum,vanum,0)
      *Event::GetGainCorrectionPar(jinfnum,tdrnum,vanum,1);
  }
  return;
}


//everything I'm making here is based on the cog but should have been done on the single strip address and only then the cog should have been performed
double Cluster::GetAlignedPosition(int mult){
  double align_shift = Event::GetAlignPar(GetJinf(), GetTDR(), side);
  float cog = GetCoG();
  float cog2=cog;
  double mult_shift = 0.0;
  float pitchcorr = 0.0;
  if (side==0) {//S
    // The gaps between 0 and 1 and 638 to 639 are doubled
    if(cog2>=0.5) pitchcorr+=1.0;
    if(cog2>=638.5) pitchcorr+=1.0;
    if( Event::GetLadderConf()->GetStripMirroring(GetJinf(), GetTDR(), side) ){
      cog2 = 639-cog2; //If the ladder is mirrored, reverse position
    }
  }
  else {// K (K5; K7 is not implemented)
    cog2 -= GetNChannels(0);//N channels of S --> cog in [0, 383]
    int sensor=(int)((cog2+mult*GetNChannels(1))/GetReadChannelK());//cast to int but essentially is also used as 'floor'
    bool multflip = Event::GetMultiplicityFlip(GetJinf(), GetTDR());
    // printf("VF: multflip = %d\n", multflip);
    if (multflip && sensor%2) {//if sensor is odd (DISPARO)
      sensor-=2;//move 'back' of two sensors...
    }
    mult_shift = GetSensPitchK()*sensor;
    if(cog2>191.5) cog2-=192.0;//--> cog in [0, 191]
    if (cog2>190.5) pitchcorr = 0.5;//last strip of the sensor is half pitch more far
    if( Event::GetLadderConf()->GetStripMirroring(GetJinf(), GetTDR(), side) ){
      cog2 = 383-cog2; //If the ladder is mirrored, reverse position
    }
  }
  return (cog2+pitchcorr)*GetPitch(side)+mult_shift-align_shift;
}

double Cluster::GetZPosition(){
  return Event::GetAlignPar(GetJinf(), GetTDR(), 2);
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
