
void Ladder::FreeCluster(cluster **aCluster){
  cluster *actual,*save;
  actual=*aCluster;
  *aCluster=NULL;
  while (actual != NULL) {
    save=actual->next;
    FreeChannel(actual->channel);
    delete actual;
    actual=save;
  }
}

void Ladder::FreeChannel(channel *aChannel){
  channel *actual,*save;
  actual=aChannel;
  aChannel=NULL;
  while (actual != NULL) {
    save=actual->next;
    delete actual;
    actual=save;
  }
}


void Ladder::SetClusterization(float aCut1, float aCut2, 
			       float aMinInt, int aMask){
  fCut1 = aCut1;
  fCut2 = aCut2;
  fMinInt = aMinInt;
  fMaskClu = aMask;
}
void Ladder::DoCluster(cluster **aCluster){
  int i=0;
  int clusmin,clusmax;
  int ifirst=0,ilast=0;
  int numva=0;
   
  boolean noclusters = true;
  cluster *clusnow=NULL;
  channel *channow=NULL;

  int firstva=0,lastva=0;

  FreeCluster(aCluster);

  numva=va_per_ladder;
  firstva=0;
  lastva=va_per_ladder;
  ifirst=firstva*channel_per_va;
  ilast =lastva*channel_per_va;


  int nclus = 0;
  
  i=ifirst;
  while (i<ilast) {
    int va = Getva(i);
    int ch = Getch(i);
    if ( fReadOut[va][ch] <= fCut1*fSigma[va][ch] || 
	(fStatus[va][ch] & fMaskClu) !=0 ){
      i++;
      continue;
    }
 
    nclus++;

    clusmin=i-1;
    va=Getva(clusmin);
    ch=Getch(clusmin);
    if (clusmin>=ifirst){
      while (clusmin>=ifirst && 
	     fReadOut[va][ch] > fCut2*fSigma[va][ch] &&
	     (fStatus[va][ch] & fMaskClu) == 0) {
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
	     fReadOut[va][ch] > fCut2*fSigma[va][ch] &&
	     (fStatus[va][ch] & fMaskClu) == 0) {
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

    if (aInt < fMinInt) continue;

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
    if (clusmax==clusmin) {
      if (clusmax<ilast) {
        if (clusmin>ifirst) {
          if (fReadOut[Getva(clusmin-1)][Getch(clusmin-1)]>
              fReadOut[Getva(clusmax+1)][Getch(clusmax+1)]) {
            loval=clusmin-1;}
          else {
            hival=clusmax+1;
          }
        } else {
          hival=clusmax+1;
        }
      } else {
        loval=clusmin-1;
      }
    }


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
    }
  }
}

void Ladder::Reduction(cluster **aCluster){
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
  DoCluster(aCluster);

  //  int nclus=0;
  //  cluster *actual;
//    if (strcmp(fName,"telX4")==0){
//      cout << endl << "New event" << endl;
//      actual=*aCluster;
//      while (actual != NULL){
//        nclus++;
//        // fill clusters histos
//        printf("%s%4i%4i%4i%9.1f\n","Cluster: ",nclus,actual->first,
//  	     actual->length,actual->integral);
//        actual=actual->next;
//      }
//    }

  //fill number of clusters histos

}































































































