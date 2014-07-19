#include "Cluster.hh"
#include <cmath>


ClassImp(Cluster);


Cluster::Cluster(){
  address=0;
  length=0;
  memset(Signal,0,MAXLENGTH*sizeof(Signal[0]));
  memset(Noise,0,MAXLENGTH*sizeof(Noise[0]));
  memset(Status,0,MAXLENGTH*sizeof(Status[0]));
  bad=0;
  npiste=0;
  golden=0;
  ladder=-1;
  side=-1;
}

Cluster::Cluster(Cluster & orig):TObject(orig){

  ladder=orig.ladder;
  side=orig.side;
  address=orig.address;
  length=orig.length;
  for (int ii=0;ii<MAXLENGTH;ii++){
    Signal[ii]=orig.Signal[ii];
    Noise[ii]=orig.Noise[ii];
    Status[ii]=orig.Status[ii];
  }
  bad=orig.bad;
  golden=orig.golden;
}
void Cluster::Build(int lad, int sid,int add,int len,
		    float* sig,float* noi,int* stat,int badin){

  ladder=lad;
  side=sid;
  address=add;
  if(len>MAXLENGTH) length=MAXLENGTH;
  else length=len;
  for (int ii=0;ii<length;ii++){
    Signal[ii]=sig[ii];
    Noise[ii]=noi[ii];
    Status[ii]=stat[ii];
  }
  bad=badin;
  
  return;
}

void Cluster::Clear(){
  address=0;
  length=0;
  memset(Signal,0,MAXLENGTH*sizeof(Signal[0]));
  memset(Noise,0,MAXLENGTH*sizeof(Noise[0]));
  memset(Status,0,MAXLENGTH*sizeof(Status[0]));
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

int Cluster::GetSeedStatus() {
  return Status[GetSeedAdd()];
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



int Cluster::GetLargeur(float val){
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


float Cluster::GetTotNoise(float seuil){
  int se=GetSeed();
  float val = Noise[se]*Noise[se];
  for (int ii=se-1;ii>=0;ii--)
    if(GetCSignal(ii)/Noise[ii]>=seuil) val+=(Noise[ii]*Noise[ii]);
    else break;
  for (int ii=se+1;ii<length;ii++)
    if(GetCSignal(ii)/Noise[ii]>=seuil) val+=(Noise[ii]*Noise[ii]);
    else break;
  if (val > 0.) val=sqrt(val);
  return(val);
}



float Cluster::GetTotSig(float seuil){
  int se=GetSeed();
  float val = GetCSignal(se);
  for (int ii=se-1;ii>=0;ii--)
    if(GetCSignal(ii)/Noise[ii]>=seuil) val+=GetCSignal(ii);
    else break;
  for (int ii=se+1;ii<length;ii++)
    if(GetCSignal(ii)/Noise[ii]>=seuil) val+=GetCSignal(ii);
    else break;
  return(val);
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

float Cluster::GetEtaTrue(){
  float ee;
  int se=GetSeed();
  int se_r=se+1;
  int se_l=se-1;
  float val_l=0;
  float val_r=0;
  float sb_l=0;
  float sb_r=0;
  float val_seed=GetCSignal(se);
  //  float sb_seed = val_seed/Noise[se];
  if (se_l>=0) {
     val_l = GetCSignal(se_l);
     sb_l = GetCSignal(se_l)/Noise[se_l];
  }
  if (se_r<length) {
     val_r = GetCSignal(se_r);
     sb_r = GetCSignal(se_r)/Noise[se_r];
  }

  //  printf("EtaTrue: se_l %d se %d se_r %d\n",se_l,se,se_r);
  //  printf("   addr: se_l %d se %d se_r %d\n",address+se_l,address+se,address+se_r);
  //  printf("         val_l %f val_seed %f val_r %f\n",val_l,val_seed,val_r);
  //  printf("         sb_l %f sb_seed %f sb_r %f\n",sb_l,sb_seed,sb_r);
  if(sb_l>sb_r && val_l>0) {
     ee=val_seed/(val_l+val_seed);
     piste_eta = se_l;
  }
  else if(sb_r>sb_l && val_r>0) {
     ee=val_r/(val_r+val_seed);
     piste_eta = se;
  }
  else return -3;
  //  printf("(sb) piste_eta %d ee %f\n",piste_eta,ee);
  /*  if(val_l>val_r && val_l>0) {
     ee=val_seed/(val_l+val_seed);
     piste_eta = se_l;
  }
  else if(val_r>val_l && val_r>0) {
     ee=val_r/(val_r+val_seed);
     piste_eta = se;
  }
  printf("(s) piste_eta %d ee %f\n",piste_eta,ee); */
  return ee;
}

float Cluster::GetEta3(){
  float ee3 = -1.;
  piste_eta3 = -999;
  int se=GetSeed();
  int se_r=se+2;
  int se_l=se-2;
  float val_l[10];
  for (int i=0; i<10; i++) val_l[i]=0.;
  float val_r[10];
  for (int i=0; i<10; i++) val_r[i]=0.;
  float sb_l[2];
  for (int i=0; i<2; i++) sb_l[i]=0.;
  float sb_r[2];
  for (int i=0; i<2; i++) sb_r[i]=0.;
  float val_seed=GetCSignal(se);
  float sb_seed = val_seed/Noise[se];
  //  printf("\n");
  //  printf("se_l %d se_r %d se %d length %d\n",se_l,se_r,se,length);
  while (se_l < se) {
     if (se_l>=0) {
       //        printf("while se_l %d\n",se_l);
        val_l[se_l-se+2] = GetCSignal(se_l);
        sb_l[se_l-se+2] = GetCSignal(se_l)/Noise[se_l];
     }
     se_l++;
  }
  while (se_r > se) {
    if (se_r < length) {
      //       printf("while se_r %d\n",se_r);
       val_r[se_r-se-1] = GetCSignal(se_r);
       sb_r[se_r-se-1] = GetCSignal(se_r)/Noise[se_r];
    }
    se_r--;
  }
  //  printf("val0 %f val1 %f valse %f val0 %f val1 %f\n",val_l[0],val_l[1],val_seed,val_r[0],val_r[1]);
  //  printf("sb0 %f sb1 %f sbse %f sb0 %f sb1 %f\n",sb_l[0],sb_l[1],sb_seed,sb_r[0],sb_r[1]);
  if(sb_l[1] > sb_r[0] && val_l[1]>0) {
    if (sb_l[0] > sb_r[0] && val_l[0]>0) { 
     ee3 = (val_l[1]+ 2*val_seed)/(val_l[0]+ val_l[1] + val_seed);
     piste_eta3 = se - 2;
    } 
    else if (sb_r[0] > sb_l[0] && val_r[0]>0) {
     ee3 = (val_seed+ 2*val_r[0])/(val_l[1]+ val_seed + val_r[0]);
     piste_eta3 = se - 1;
    }
  }
  else if(sb_r[0] > sb_l[1] && val_r[0]>0) {
    if (sb_r[1] > sb_l[1] && val_r[1]>0) { 
     ee3 = (val_r[0]+ 2*val_r[1])/(val_seed + val_r[0]+ val_r[1]);
     piste_eta3 = se;
    } 
    else if (sb_l[1] > sb_r[1] && val_l[1]>0) {
     ee3 = (val_seed+ 2*val_r[0])/(val_l[1]+ val_seed + val_r[0]);
     piste_eta3 = se - 1;
    }
  }

  //  printf("EtaTrue: se_l %d se %d se_r %d\n",se_l,se,se_r);
  //  printf("   addr: se_l %d se %d se_r %d\n",address+se_l,address+se,address+se_r);
  //  printf("         val_l %f val_seed %f val_r %f\n",val_l,val_seed,val_r);
  //  printf("         sb_l %f sb_seed %f sb_r %f\n",sb_l,sb_seed,sb_r);
  //  printf("(sb) piste_eta %d ee %f\n",piste_eta,ee);
  /*  if(val_l>val_r && val_l>0) {
     ee=val_seed/(val_l+val_seed);
     piste_eta = se_l;
  }
  else if(val_r>val_l && val_r>0) {
     ee=val_r/(val_r+val_seed);
     piste_eta = se;
     } */
  //  printf("piste_eta3 %d ee3 %f\n",piste_eta3,ee3);
  return ee3;
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

float Cluster::GetCoGN( int n){
  int se=GetSeed();
  //  float ee=GetEtaRaw();

  if (n > length) n=length;
  int span = n/2;
  int ind_bas = -1;
  int ind_haut = 0;
  //  printf("--> CogN: larg %d n %d seed %d span %d entre %d et %d\n",length,n,se,span,se-span,se+span);
  for (int i=(se-span); i<=(se+span); i++) {
    if (i >= 0 && ind_bas == -1) ind_bas=i;
    else if (i < length && ind_bas > -1) ind_haut=i;
  }
  //  printf("    ind %d %d\n",ind_bas,ind_haut);
  double pair = fmod(((double) n),2.);
  //  printf("pair %f\n",pair);
  if (pair == 0.) {
    //    printf(" bas sn %f haut sn %f\n",Signal[ind_bas]/Noise[ind_bas],Signal[ind_haut]/Noise[ind_haut]);
    if ((Signal[ind_bas]/Noise[ind_bas]) > (Signal[ind_haut]/Noise[ind_haut])) ind_haut--;
	else ind_bas++;
    //    printf("    ind %d %d\n",ind_bas,ind_haut);
  }
  //  printf("npiste %d\n",npiste);
  float cog = 0.;
  float stot = 0.;
  for (int i=ind_bas; i<=ind_haut; i++) {
    if (Signal[i] > 0.) {
      cog+=((address+i)*Signal[i]);
      stot+=Signal[i];
      npiste++;
    }
  }
  cog/=stot;
  //  printf("npiste %d\n",npiste);
  //  float cogeta = 0.;
  //  if (ee<0) cogeta = address+se;
  //  else cogeta = address+ee;
  //  printf("cog %f cogeta %f\n",cog,cogeta);

  return cog;
}

float Cluster::GetPositionEtaP(Align* alg, int det) {
  int se=GetSeed();
  float ee=GetEtaTrue();
  if(ee<0) {
    npiste = 1;
    return address+se;
  }
  else {
    int nbins = alg->feta_p[det]->GetNbinsX();
    int ib = 0;
    while (ib < nbins) {
      double val_min = alg->feta_p[det]->GetBinCenter(ib) - alg->feta_p[det]->GetBinWidth(ib);
      double val_max = alg->feta_p[det]->GetBinCenter(ib) + alg->feta_p[det]->GetBinWidth(ib);
      if (ee >= val_min && ee <= val_max) {
	 npiste = 2;
         return (address+piste_eta)+alg->feta_p[det]->GetBinContent(ib);
      }
      ib++; 
    }
  }
  return -999.;
}

float Cluster::GetPositionEtaN(Align* alg, int det) {
  int se=GetSeed();
  float ee=GetEtaTrue();
  if(ee<0) {
    npiste = 1;
    return address+se;
  }
  else {
    int nbins = alg->feta_n[det]->GetNbinsX();
    //    printf("nbins %d\n",nbins);
    int ib = 0;
    while (ib < nbins) {
      double val_min = alg->feta_n[det]->GetBinCenter(ib) - alg->feta_n[det]->GetBinWidth(ib);
      double val_max = alg->feta_n[det]->GetBinCenter(ib) + alg->feta_n[det]->GetBinWidth(ib);
      //      printf("ib %d ee %f min %f max %f feta %f pos %f\n",ib,ee,val_min,val_max,alg->feta_n[det]->GetBinContent(ib),
      //	     (address+se-1.)+alg->feta_n[det]->GetBinContent(ib));
      if (ee >= val_min && ee <= val_max) {
 	 npiste = 2;
         return (address+piste_eta)+alg->feta_n[det]->GetBinContent(ib);
      }
      ib++; 
    }
  }
  return -999.;
}

float Cluster::GetPositionEta3N(Align* alg, int det) {
  int se=GetSeed();
  float ee=GetEta3();
  if(ee<0) {
    npiste = 1;
    return address+se;
  }
  else {
    int nbins = alg->feta3_n[det]->GetNbinsX();
    //    printf("eta3 nbins %d\n",nbins);
    int ib = 0;
    while (ib < nbins) {
      double val_min = alg->feta3_n[det]->GetBinCenter(ib) - alg->feta3_n[det]->GetBinWidth(ib);
      double val_max = alg->feta3_n[det]->GetBinCenter(ib) + alg->feta3_n[det]->GetBinWidth(ib);
      //      printf("ib %d ee %f min %f max %f feta %f pos %f\n",ib,ee,val_min,val_max,alg->feta_n[det]->GetBinContent(ib),
      //	     (address+se-1.)+alg->feta_n[det]->GetBinContent(ib));
      if (ee >= val_min && ee <= val_max) {
 	 npiste = 3;
         return (address+piste_eta3+alg->bord_eta3_n[det])+alg->feta3_n[det]->GetBinContent(ib);
      }
      ib++; 
    }
  }
  return -999.;
}

float Cluster::GetSeedVal(){
  return GetCSignal(GetSeed());
}

float Cluster::GetSeedSN(){
  return GetCSignal(GetSeed())/Noise[GetSeed()];
}

float Cluster::GetTotSN(float seuil){
return GetTotSig(seuil)/GetTotNoise(seuil);
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
