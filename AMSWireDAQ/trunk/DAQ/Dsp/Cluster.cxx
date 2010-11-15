#include "Ladder.h"

void Ladder::FreeCluster(cluster **aCluster){
  cluster *actual,*save;
  actual=*aCluster;
  *aCluster=NULL;
  while (actual != NULL) {
    save=actual->next;
    FreeChannel(&actual->channel);
    delete actual;
    actual=save;
  }
}

void Ladder::FreeChannel(channel **aChannel){
  channel *actual,*save;
  actual=*aChannel;
  *aChannel=NULL;
  while (actual != NULL) {
    save=actual->next;
    delete actual;
    actual=save;
  }
}


//void Ladder::SetClusterization(float aCut1, float aCut2, 
//			       float aMinInt, int aMask){
//  fCut1 = aCut1;
//  fCut2 = aCut2;
//  fMinInt = aMinInt;
//  fMaskClu = aMask;
//}
void Ladder::DoCluster(char aSorK, cluster **aCluster){
  int i=0;
  int clusmin,clusmax;
  int ifirst=0,ilast=0;
  int numva=0;
   
  boolean noclusters = true;
  cluster *clusnow=NULL;
  channel *channow=NULL;

  int firstva=0,lastva=0;

  FreeCluster(aCluster);

//    numva=va_per_ladder;
//    firstva=0;
//    lastva=va_per_ladder;
//    ifirst=firstva*channel_per_va;
//    ilast =lastva*channel_per_va;

  if (aSorK == 'S'){
    numva=10;
    firstva=0;
    lastva=10;
    ifirst=firstva*channel_per_va;
    ilast =lastva*channel_per_va;
  }

  if (aSorK == 'K'){
    numva=6;
    firstva=10;
    lastva=16;
    ifirst=firstva*channel_per_va;
    ilast =lastva*channel_per_va;
  }



  int nclus = 0;
  
  i=ifirst;
  while (i<ilast) {
    int va = Getva(i);
    int ch = Getch(i);
    //    printf("i=%3d %3d %3d %6.1f %6.1f %d\n",i,va,ch,
    //	      fReadOut[va][ch],control->fCut1*fSigma[va][ch],
    //	      fStatus[va][ch]);
    if ( fReadOut[va][ch] <= control->fCut1*fSigma[va][ch] || 
	(fStatus[va][ch] & control->fMaskClu) !=0 ){
      i++;
      continue;
    }
 
    nclus++;

    clusmin=i-1;
    va=Getva(clusmin);
    ch=Getch(clusmin);
    if (clusmin>=ifirst){
      while (clusmin>=ifirst && 
	     fReadOut[va][ch] > control->fCut2*fSigma[va][ch] &&
	     (fStatus[va][ch] & control->fMaskClu) == 0) {
	clusmin--;
	va=Getva(clusmin);
	ch=Getch(clusmin);
      }
    }
    clusmin++;

    clusmax=i+1;
    if (clusmax<ilast) {
      va=Getva(clusmax);
      ch=Getch(clusmax);
      while (clusmax<ilast && 
	     fReadOut[va][ch] > control->fCut2*fSigma[va][ch] &&
	     (fStatus[va][ch] & control->fMaskClu) == 0) {
	clusmax++; 
	va=Getva(clusmax);
	ch=Getch(clusmax);
      }
    }
    clusmax--;
    i=clusmax+1;

    float aInt=0.0;
    for (int l=clusmin;l<=clusmax;l++){
      va=Getva(l);
      ch=Getch(l);
      aInt += fReadOut[va][ch];
    }

    if (aInt < control->fMinInt) continue;

    if (noclusters) {
      *aCluster=new cluster;
      clusnow=*aCluster;
      noclusters=false;
    } else {
      clusnow->next=new cluster;
      clusnow=clusnow->next;
    }

    clusnow->first=clusmin;
    clusnow->length=clusmax-clusmin+1;
    clusnow->cog=0.0;
    clusnow->maxloc=0;
    clusnow->maxval=0.0;
    clusnow->integral=0.0;
    clusnow->sovern1=0.0;
    clusnow->sovern2=0.0;
    clusnow->sovern3=0.0;
    clusnow->next=NULL;
    
    for (int l=clusmin;l<=clusmax;l++){
      va=Getva(l);
      ch=Getch(l);
//        if (strcmp(fName,"amsS2")==0) 
//  	printf("  ch:%4d %10.2f %10.2f %10.2f\n",l,fReadOut[va][ch],
//  	       fSigma[va][ch],fCN[va]);
      if (fReadOut[va][ch]>clusnow->maxval) {
	clusnow->maxval=fReadOut[va][ch];
	clusnow->maxloc=l;
      }      
      clusnow->integral += fReadOut[va][ch];
      clusnow->cog += fReadOut[va][ch]*(l+1);
      if (fSigma[va][ch]>0) { 
        clusnow->sovern1+=fSigma[va][ch]*fSigma[va][ch];
        clusnow->sovern2+=fSigma[va][ch];
	clusnow->sovern3+=fReadOut[va][ch]/fSigma[va][ch]*
	                  fReadOut[va][ch]/fSigma[va][ch];
      }
    }
    clusnow->cog/=clusnow->integral;
    clusnow->cog-=1.0;
    if (clusnow->sovern1>0) 
      clusnow->sovern1=clusnow->integral/sqrt(clusnow->sovern1);
    if (clusnow->sovern2>0)
      clusnow->sovern2=clusnow->length*clusnow->integral/clusnow->sovern2;
    clusnow->sovern3=sqrt(clusnow->sovern3);

//      if (strcmp(fName,"amsS2")==0) 
//        printf("clus:%10.2f %4d %4d\n",clusnow->integral,
//  	     clusnow->first,clusnow->length);

// piece of code to include highest strip if clulen=1
// does not work like it is!

    int loval=clusmin, hival=clusmax;

// Ni puta idea de porque hice esto, pero crea dos canales cuando length=1
// asi que lo comento y ya veremos si hay algun problema 
//      if (clusmax==clusmin) {
//        if (clusmax<ilast) {
//          if (clusmin>ifirst) {
//            if (fReadOut[Getva(clusmin-1)][Getch(clusmin-1)]>
//                fReadOut[Getva(clusmax+1)][Getch(clusmax+1)]) {
//              loval=clusmin-1;}
//            else {
//              hival=clusmax+1;
//            }
//          } else {
//            hival=clusmax+1;
//          }
//        } else {
//          loval=clusmin-1;
//        }
//      }


    for (int l=loval;l<=hival;l++){
      va=Getva(l);
      ch=Getch(l);
      if (l == loval) {
	clusnow->channel=new channel;
	channow=clusnow->channel;
      } else {
	channow->next=new channel;
	channow=channow->next;
      }
        
      channow->Slot = l;
      channow->ADC = fReadOut[va][ch];
      channow->CN = fCN[va];
      channow->Status = fStatus[va][ch];     
      channow->next = NULL;
    }
  }
}

void Ladder::Reduction(int dynped){
  int bin;
  int nclus_S=0;
  int nclus_K=0;
  cluster *actual;

  header *h = new header;
  cluster_write *actual_write = new cluster_write;
  channel *actual_channel;
  channel_write *actual_channel_write = new channel_write;

  SubstractPedestal();

  
//    if (strcmp(fName,"telX4")==0) {
//      cout << endl;
//      ComputeCN2();   
//      printf(" CN:%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f\n",
//      	   fCN[0],fCN[1],fCN[2],fCN[3],fCN[4],
//      	   fCN[5],fCN[6],fCN[7],fCN[8],fCN[9]);
//    }

  ComputeCN2(); 
//    if (strcmp(fName,"telX4")==0) {
//      printf("CN2:%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f\n",
//    	   fCN[0],fCN[1],fCN[2],fCN[3],fCN[4],
//    	   fCN[5],fCN[6],fCN[7],fCN[8],fCN[9]);
//    }

  SubstractCN();

  UpdateSignalX(dynped);

  DoCluster('S',&Scluster);
  DoCluster('K',&Kcluster);



  actual=Scluster;
  while (actual != NULL){
    nclus_S++;
    bin = GetBin(kHISTO_CLUSTER_LENGTH_S,actual->length);
    histo->fhClusLength_S[bin]++;
    bin = GetBin(kHISTO_CLUSTER_COG,actual->cog);
    histo->fhClusCog[bin]++;
    bin = GetBin(kHISTO_CLUSTER_MAXLOC,actual->maxloc-actual->cog);
    histo->fhClusMaxloc[bin]++;
    bin = GetBin(kHISTO_CLUSTER_INTEGRAL_S,actual->integral);
    histo->fhClusIntegral_S[bin]++;
    bin = GetBin(kHISTO_CLUSTER_SOVERN_S,actual->sovern1);
    histo->fhClusSovern_S[bin]++;
    actual=actual->next;
  }
  bin = GetBin(kHISTO_CLUSTER_NUMBER_S,nclus_S);
  histo->fhClusNumber_S[bin]++;

  actual=Kcluster;
  while (actual != NULL){
    nclus_K++;
    bin = GetBin(kHISTO_CLUSTER_LENGTH_K,actual->length);
    histo->fhClusLength_K[bin]++;
    bin = GetBin(kHISTO_CLUSTER_COG,actual->cog);
    histo->fhClusCog[bin]++;
    bin = GetBin(kHISTO_CLUSTER_MAXLOC,actual->maxloc-actual->cog);
    histo->fhClusMaxloc[bin]++;
    bin = GetBin(kHISTO_CLUSTER_INTEGRAL_K,actual->integral);
    histo->fhClusIntegral_K[bin]++;
    bin = GetBin(kHISTO_CLUSTER_SOVERN_K,actual->sovern1);
    histo->fhClusSovern_K[bin]++;
    actual=actual->next;
  }
  bin = GetBin(kHISTO_CLUSTER_NUMBER_K,nclus_K);
  histo->fhClusNumber_K[bin]++;


  if (control->DataMode == kDATAMODE_SOURCE && control->SourceSequence > 0){
    h->NRun = control->NRun;
    h->NEvt = control->NEvents;
    h->S_clus = nclus_S;
    h->K_clus = nclus_K;
    fwrite(h,sizeof(struct header),1,fTDRDataFile);
    
    actual=Scluster; 
    while (actual != NULL){
      memcpy(actual_write,actual,sizeof(struct cluster_write));
      fwrite(actual_write,sizeof(struct cluster_write),1,fTDRDataFile);
      actual_channel = actual->channel;
      while (actual_channel != NULL) {
	memcpy(actual_channel_write,actual_channel,
	       sizeof(struct channel_write));
	fwrite(actual_channel_write,sizeof(struct channel_write),1,
	       fTDRDataFile);
	actual_channel = actual_channel->next;
      }
      actual=actual->next;
    }

    actual=Kcluster; 
    while (actual != NULL){
      memcpy(actual_write,actual,sizeof(struct cluster_write));
      fwrite(actual_write,sizeof(struct cluster_write),1,fTDRDataFile);
      actual_channel = actual->channel;
      while (actual_channel != NULL) {
	memcpy(actual_channel_write,actual_channel,
	       sizeof(struct channel_write));
	fwrite(actual_channel_write,sizeof(struct channel_write),1,
	       fTDRDataFile);
	actual_channel = actual_channel->next;
      }
      actual=actual->next;
    }
  }


  //while (actual != NULL){
  //  nclus_S++;
  //  printf("Run:%05d Evt:%06d S-%03d%5d%5d%9.1f%9.1f%9.1f%9.1f\n",
  //	   control->NRun,control->NEvents,nclus_S,actual->first,
  //   actual->length,actual->integral,actual->sovern1,
  //   actual->sovern2,actual->sovern3);
  //actual=actual->next;
  //}

  //actual=Kcluster; nclus_K=0;
  //while (actual != NULL){
  // nclus_K++;
  //printf("Run:%05d Evt:%06d K-%03d%5d%5d%9.1f%9.1f%9.1f%9.1f\n",
  //   control->NRun,control->NEvents,nclus_K,actual->first,
  //   actual->length,actual->integral,actual->sovern1,
  //   actual->sovern2,actual->sovern3);
  //actual=actual->next;
  //}
}

