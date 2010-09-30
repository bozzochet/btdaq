#include "Ladder.h"

Ladder::Ladder(char aVoltage, char aSensors[3], char aAssembly, 
	       char aLayer  , char aSerial[4]){
  fVoltage  = aVoltage;
  strcpy(fSensors,aSensors);
  fAssembly = aAssembly;
  fLayer    = aLayer;
  strcpy(fSerial,aSerial);

  sprintf(fName,"%c%s%c%c%s",fVoltage,fSensors,fAssembly,fLayer,fSerial);

  memset(fNonGaussian,0,sizeof(fNonGaussian));

  memset(fGood,0,sizeof(fGood));
  fNevent=0;

  memset(fPedestal,0,sizeof(fPedestal));
  memset(fRefPedestal,0,sizeof(fRefPedestal));
  memset(fSigma,0,sizeof(fSigma));
  memset(fRefSigma,0,sizeof(fRefSigma));
  memset(fSigmaRaw,0,sizeof(fSigmaRaw));
  memset(fRefSigmaRaw,0,sizeof(fRefSigmaRaw));
  memset(fGainSlope,0,sizeof(fGainSlope));
  memset(fRefGainSlope,0,sizeof(fRefGainSlope));
  memset(fStatus,0,sizeof(fStatus));
  memset(fRefStatus,0,sizeof(fRefStatus));


  char file[200];
#include "msg_definition.code"
#include "shm_definition.code"

  memset(histo->fhRefPedestal,0,sizeof(histo->fhRefPedestal));
  memset(histo->fhDiffPedestal,0,sizeof(histo->fhDiffPedestal));
  memset(histo->fhStatus,0,sizeof(histo->fhStatus));
  
  
} 


Ladder::Ladder(char aName[15]){

  strcpy(fName,aName);
  memset(fStatus,0,sizeof(fStatus));
  memset(fNonGaussian,0,sizeof(fNonGaussian));

  fNevent=0;
  memset(fGood,0,sizeof(fGood));


  char file[200];
#include "msg_definition.code"
#include "shm_definition.code"
} 


Ladder::~Ladder(){
#include "shm_detach.code"
}

void Ladder::ReadEvent(int aSlot){
  memcpy(fRAW,data->Data[aSlot],sizeof(fRAW));
  fNevent++;
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      fReadOut[va][ch] = (double)fRAW[va][ch];
}

int Ladder::GetBin(int aHist, double aValue){
  int bin;
  if (aValue < histo->Minimum[aHist]) {
    bin = 0;
  } else if (aValue > histo->Maximum[aHist]) {
    bin = histo->Nbin[aHist]+1;
  } else {
    bin = (int) ( histo->Nbin[aHist] * (aValue - histo->Minimum[aHist]) /
  		  (histo->Maximum[aHist] - histo->Minimum[aHist]) ) + 1;
  }
  //print("aValue: %d  bin: %d",aValue,bin);
  return bin;
}

void Ladder::UpdateSignal(int dynped){
  SubstractPedestal();
  UpdatePedSub();
  ComputeCN2();
  SubstractCN();
  UpdateSignalX(dynped);

  //  memcpy(&histo->fhSignal[1],fReadOut,sizeof(fReadOut));
  //  for (int va=0;va<va_per_ladder;va++)
  //    for (int ch=0;ch<channel_per_va;ch++){
  //      if (fStatus[va][ch] == 0) 
  //	histo->fhSignal[va*channel_per_va+ch+1] = fReadOut[va][ch];
  //      else histo->fhSignal[va*channel_per_va+ch+1] = 0.0;
  //    }

  //  for (int va=0;va<va_per_ladder;va++)
  //  for (int ch=0;ch<channel_per_va;ch++)
  //    fSignalX[va][ch] = fReadOut[va][ch]-fPedestal[va][ch];
  //memcpy(&histo->fhSignal[1],fSignalX,sizeo);
}


void Ladder::UpdatePedSub(){
  memcpy(&histo->fhPedSub[1],fReadOut,sizeof(fReadOut));
}



void Ladder::UpdateSignalX(int dynped){

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++) {
      if (dynped) {
	if (fReadOut[va][ch]>0) fPedestal[va][ch]+=0.125; 
	else fPedestal[va][ch]-=0.125; 	  
      }
      if (fReadOut[va][ch]>(5.0*fSigma[va][ch])){
	fSignalHits[va][ch]=fSignalHits[va][ch]+1.0;
	if (fSignalHits[va][ch] > (double)fNevent/100.0){ 
	  fSignalMean[va][ch]=
	    fSignalMean[va][ch]*(fSignalHits[va][ch]-1)/fSignalHits[va][ch]+ 
	    fReadOut[va][ch]/fSignalHits[va][ch];}
	else {fSignalMean[va][ch]=0.0;}
      }
    }
  memcpy(&histo->fhSignal[1],fReadOut,sizeof(fReadOut));
  memcpy(&histo->fhSignalMean[1],fSignalMean,sizeof(fSignalMean));
  memcpy(&histo->fhSignalHits[1],fSignalHits,sizeof(fSignalHits));

  //  memcpy(histo->fhSignal,fReadOut,sizeof(fReadOut));

//    for (int va=0;va<va_per_ladder;va++)
//      for (int ch=0;ch<channel_per_va;ch++){
//        int bin = GetBin(kHISTO_SIGNAL,fReadOut[va][ch]);
//        histo->fhSignal[va][ch][bin]++;
//      }
}

void Ladder::UpdateScope(){
  //  cerr << fReadOut[10][20] << endl;
  memcpy(&histo->fhScope[1],fReadOut,sizeof(fReadOut));
}

void Ladder::Reset(int aDataMode){
  switch (aDataMode){
  case kDATAMODE_RAW:
    fNevent = 0;
    memset(fSignal,0,sizeof(fSignal));
    memset(fSignalMean,0,sizeof(fSignalMean));
    memset(fSignalHits,0,sizeof(fSignalHits));
    break;

  case kDATAMODE_FFT:
    fNevent = 0;
    memset(fSpectrumS1,0,sizeof(fSpectrumS1));
    memset(fSpectrumS2,0,sizeof(fSpectrumS2));
    memset(fSpectrumK,0,sizeof(fSpectrumK));
    break;

  case kDATAMODE_CALIBRATION:
    switch (control->CalStep) {
    case 0:
      fNevent = 0;
      memset(fPedcal,0,sizeof(fPedcal));
      memset(fSigcal,0,sizeof(fSigcal));
      break;
    case 1:
      fNevent = 0;
      memset(fPedestal,0,sizeof(fPedestal));
      memset(fGood,0,sizeof(fGood));
      for (int va=0;va<va_per_ladder;va++)
	for (int ch=0;ch<channel_per_va;ch++)
	  memset(histo->fhRawCal[va][ch],0,sizeof(histo->fhRawCal[va][ch]));
      memset(histo->fhPedestal,0,sizeof(histo->fhPedestal));
      memset(histo->fhPedSum,0,sizeof(histo->fhPedSum));
      break;
    case 2:
      fNevent = 0;
      memset(fSigmaRaw,0,sizeof(fSigmaRaw));
      memset(fGood,0,sizeof(fGood));
      memset(fNst,0,sizeof(fNst));
      memset(fSRmean,0,sizeof(fSRmean));
      memset(fSRsigma,0,sizeof(fSRsigma));
      memset(fStatus,0,sizeof(fStatus));
      memset(histo->fhSigmaRaw,0,sizeof(histo->fhSigmaRaw));
      memset(histo->fhSigRawSum,0,sizeof(histo->fhSigRawSum));
      break;
    case 3:
      fNevent = 0;
      memset(fSigma,0,sizeof(fSigma));
      memset(fGood,0,sizeof(fGood));
      memset(histo->fhSigma,0,sizeof(histo->fhSigma));
      memset(histo->fhSigSum,0,sizeof(histo->fhSigSum));
      for (int va=0;va<va_per_ladder;va++)
	memset(histo->fhCN[va],0,sizeof(histo->fhCN[va]));
      break;
    case 4:
      fNevent = 0;
      memset(fCNmean,0,sizeof(fCNmean));
      memset(fCNsigma,0,sizeof(fCNsigma));
      memset(fNonGaussian,0,sizeof(fNonGaussian));
      break;
    }

  case kDATAMODE_REDUCTION:
    fNevent = 0;
    memset(fSignal,0,sizeof(fSignal));
    memset(fSignalMean,0,sizeof(fSignalMean));
    memset(fSignalHits,0,sizeof(fSignalHits));
    break;

  case kDATAMODE_GAIN:
    fNevent = 0;
    if (control->CurrentGainStep == 0){
      memset(fGain,0,sizeof(fGain));
      memset(fSigmaGain,0,sizeof(fSigmaGain));
      memset(histo->fhGain,0,sizeof(histo->fhGain));
      for (int va=0;va<va_per_ladder;va++)
	for (int ch=0;ch<channel_per_va;ch++){
	  memset(histo->fhGainValue[va][ch],0,
		 sizeof(histo->fhGainValue[va][ch]));
	  memset(histo->fhMeanGain[va][ch],0,
		 sizeof(histo->fhMeanGain[va][ch]));
	  memset(histo->fhSigmaGain[va][ch],0,
		 sizeof(histo->fhSigmaGain[va][ch]));
	}
    }      
    break;

  case kDATAMODE_SCOPE:
    fNevent = 0;
    memset(histo->fhShaping,0,sizeof(histo->fhShaping));
  }
}


void Ladder::PassCal0(){              // First stimation on pedestal and sigma

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      fPedcal[va][ch] += fReadOut[va][ch];
      fSigcal[va][ch] += fReadOut[va][ch] * fReadOut[va][ch];
    }
  //fNevent++;
}
void Ladder::FinishPassCal0(){
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      fPedcal[va][ch] /= fNevent;
      fSigcal[va][ch] = sqrt(fSigcal[va][ch]/fNevent -
			     fPedcal[va][ch] * fPedcal[va][ch]);
    }  
}

void Ladder::PassCal1(){              //Compute pedestal
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      double fROmin = fPedcal[va][ch]-3*fSigcal[va][ch];
      double fROmax = fPedcal[va][ch]+3*fSigcal[va][ch];
      if ((fReadOut[va][ch] > fROmin) &&
	  (fReadOut[va][ch] < fROmax)) {
	fPedestal[va][ch] += fReadOut[va][ch];    
	fGood[va][ch]++;
      }
      // Fill Pedestal Histo       
      int bin = GetBin(kHISTO_RAWCAL,fReadOut[va][ch]);
      histo->fhRawCal[va][ch][bin]++;
  }
  //fNevent++;
}
void Ladder::FinishPassCal1(){
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      if (fGood[va][ch] != 0) fPedestal[va][ch] /= fGood[va][ch];
      else fPedestal[va][ch] = 0;
    }
  
  FillPedestal();
  FillDiffPedestal();
}


void Ladder::PassCal2(){              // Compute SigmaRaw

  SubstractPedestal();

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){

      double fROmin = -3.0*fSigcal[va][ch];
      double fROmax =  3.0*fSigcal[va][ch];
      if ((fReadOut[va][ch] > fROmin) &&
	  (fReadOut[va][ch] < fROmax) ) {
	fSigmaRaw[va][ch] += fReadOut[va][ch] * fReadOut[va][ch];
	fGood[va][ch]++;
      }
    }
  //fNevent++;
}

void Ladder::FinishPassCal2(){
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      if (fGood[va][ch] !=0) fSigmaRaw[va][ch] = 
			       sqrt(fSigmaRaw[va][ch]/fGood[va][ch]);
      else fSigmaRaw[va][ch] = 0;
      // int bin = va*channel_per_va+ch+1; 
      //histo->fhSigmaRaw[bin] = fSigmaRaw[va][ch];    
      fSRmean[va] += fSigmaRaw[va][ch];
      fSRsigma[va] += fSigmaRaw[va][ch] * fSigmaRaw[va][ch];
    }
  // Fill SigmaRaw Histo
  //  memcpy(&histo->fhSigmaRaw[1],fSigmaRaw,sizeof(fSigmaRaw));
  FillSigmaRaw();

  for (int va=0;va<va_per_ladder;va++){
    fSRmean[va] /= channel_per_va;
    fSRsigma[va] = sqrt(fSRsigma[va]/channel_per_va);
  }

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      if (fSigmaRaw[va][ch] < control->SRLow) SETBIT(fStatus[va][ch],0);
      if (fSigmaRaw[va][ch] > control->SRHigh)SETBIT(fStatus[va][ch],1);
      if ((fPedestal[va][ch]-3*fSigmaRaw[va][ch])<5)SETBIT(fStatus[va][ch],4);
      if ((fPedestal[va][ch]+3*fSigmaRaw[va][ch])>4090)
	SETBIT(fStatus[va][ch],5);
    } 

}

void Ladder::PassCal3(){

  SubstractPedestal();
  ComputeCN2();
  SubstractCN();

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){

    double fROmin = -3.0*fSigmaRaw[va][ch];
    double fROmax =  3.0*fSigmaRaw[va][ch];

    if ((fReadOut[va][ch] > fROmin) &&
   	(fReadOut[va][ch] < fROmax) ) {
      fSigma[va][ch] += fReadOut[va][ch] * fReadOut[va][ch]; // Compute Sigma
      fGood[va][ch]++;
    }
  }
  //fNevent++;
}
void Ladder::FinishPassCal3(){
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      if (fGood[va][ch]!=0)fSigma[va][ch]=sqrt(fSigma[va][ch]/fGood[va][ch]); 
      else fSigma[va][ch] = 0;
      //int bin = va*channel_per_va+ch+1;
      //histo->fhSigma[bin] = fSigma[va][ch];    
    }
      // Fill Sigma Histo
  //memcpy(&histo->fhSigma[1],fSigma,sizeof(fSigma));
  FillSigma();

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      if (fSigma[va][ch] < control->SigmaLow) SETBIT(fStatus[va][ch],2);
      if (fSigma[va][ch] > control->SigmaHigh)SETBIT(fStatus[va][ch],3);
    } 






  //  char aMsg[200];
  //  sprintf(aMsg,"Sigma ch640:%5.2f  ch896:%5.2f \n",fSigma[9][63],fSigma[12][63]);
  //  toLOG->Send(kDSP,aMsg,1);
}
  

void Ladder::PassCal4(){
  SubstractPedestal();
  ComputeCN2();
  SubstractCN();

  for (int va=0;va<va_per_ladder;va++){
    fCNmean[va] += fCN[va];             // Compute Mean
    fCNsigma[va]  += fCN[va] * fCN[va]; // Compute Sigma
  }
  
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      if (fabs(fReadOut[va][ch]) > control->GASigmaCut*fSigma[va][ch]) 
	fNonGaussian[va][ch]++;
    
  //fNevent++;
}

void Ladder::FinishPassCal4(){
  for (int va=0;va<va_per_ladder;va++){
    fCNmean[va] /= fNevent;                    
    fCNsigma[va] = sqrt(fCNsigma[va]/fNevent -
			fCNmean[va] * fCNmean[va]);
    //fCNsigma[va] = sqrt(fCNsigma[va]/fNevent); // Assume fCNmean = 0  
  }
  
  float EventsOut = control->GAOut*fNevent;
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      if (fNonGaussian[va][ch] > EventsOut) SETBIT(fStatus[va][ch],7);

  FillSummaryPlots();
  FillStatus(); // let's forget about gain status...
}


void Ladder::Gain(){
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      if ( (va>=0 && va< 5 && control->ActiveS1) ||
	   (va>=5 && va<10 && control->ActiveS2) ||
	   (va>=10 && va_per_ladder && control->ActiveK) ){  
	int bin = GetBin(kHISTO_GAIN_VALUE,fReadOut[va][ch]);
	histo->fhGainValue[va][ch][bin]++;
	int curval = control->CurrentGainStep;
	fGain[curval][va][ch] += fReadOut[va][ch];
	fSigmaGain[curval][va][ch] += fReadOut[va][ch] * fReadOut[va][ch];
      }  
    }
}

void Ladder::FinishGain(){ 
  int curval = control->CurrentGainStep;
  int curevent = control->DACEvent[control->GainIndex];

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      if ( (va>=0 && va< 5 && control->ActiveS1) ||
	   (va>=5 && va<10 && control->ActiveS2) ||
	   (va>=10 && va_per_ladder && control->ActiveK) ){  
        fGain[curval][va][ch] /= curevent;
	histo->fhMeanGain[va][ch][curval] = fGain[curval][va][ch];
	histo->fhSigmaGain[va][ch][curval] = 
	    sqrt(fSigmaGain[curval][va][ch]/curevent -
		 fGain[curval][va][ch] * fGain[curval][va][ch]);
      }
    }

}


void Ladder::SaveGain(){
  FILE *outFile;  
  char filename[100];
  printf("%s/%s_%05d.gain",control->fDirLadder,fName,control->NRun);
  sprintf(filename,"%s/%s_%05d.gain",control->fDirLadder,fName,control->NRun);
  outFile=fopen(filename,"w");
  
  fprintf(outFile,"            "); 
  for (int curval=0;curval<control->MaxGainStep;curval++){
    fprintf(outFile,"%8.1f  ",histo->fhDACGain[curval]);
  }
  fprintf(outFile,"\n");

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      fprintf(outFile,"%5d %2d %2d ",va*64+ch+1,va+1,ch+1);
      for (int curval=0;curval<control->MaxGainStep;curval++){
	fprintf(outFile,"%8.1f  ",histo->fhMeanGain[va][ch][curval]);
      }
      fprintf(outFile,"\n");
    }
  fclose(outFile);
}


void Ladder::Channel(){
  int decal=5*channel_per_va*control->DACchain;
  for (int i=0;i<90;i++){
    int j=i+decal;  
    int va=Getva(j);
    int ch=Getch(j);
    histo->fhShaping[i+1]=fReadOut[va][ch];
  }
}


void Ladder::WriteSummary(int aSaveTest){
  FILE *outFile;  
  char filename[100],filename2[100];
  char command[250];
  sprintf(filename,"%s/%s.sum",control->fDirLadder,fName);
  outFile=fopen(filename,"w");  
  char sep[100];
  sprintf(sep,
   "+--+------+------+------+------+------+------+------+--+--+--+--+--+--+--+--|\n");
  //  if (control->OnlyHybrid == 1) 
  //    fprintf(outFile,"Hybrid test\n");
  /// else
    
  fprintf(outFile,"%s\n",fName);

  fprintf(outFile,sep);
  fprintf(outFile,
   "|VA|  CN  | PED  | sPED | SIG  | sSIG | GAIN | sGAIN|rl|rh|sl|sh|un|ov|ng|gl|\n");
  fprintf(outFile,sep);


  memset(fmeanped,0,sizeof(fmeanped));
  memset(fsigped,0,sizeof(fsigped));
  memset(fmeansig,0,sizeof(fmeansig));
  memset(fsigsig,0,sizeof(fsigsig));
  memset(fmeangan,0,sizeof(fmeangan));
  memset(fsiggan,0,sizeof(fsiggan));
  memset(fSRawlow,0,sizeof(fSRawlow));   
  memset(fSRawhigh,0,sizeof(fSRawhigh));
  memset(fSigmalow,0,sizeof(fSigmalow));   
  memset(fSigmahigh,0,sizeof(fSigmahigh));
  memset(funder,0,sizeof(funder));
  memset(fover,0,sizeof(fover));
  memset(fnongaus,0,sizeof(fnongaus));
  memset(fgainlow,0,sizeof(fgainlow));

  int testva[va_per_ladder];
  memset(testva,0,sizeof(testva));

  if (control->ActiveS1) {
    testva[0]=1; testva[1]=1; testva[2]=1; testva[3]=1; testva[4]=1;
    CalcSummary( 0, 4,outFile); 
    fprintf(outFile,sep);}
  if (control->ActiveS2) {
    testva[5]=1; testva[6]=1; testva[7]=1; testva[8]=1; testva[9]=1;
    CalcSummary( 5, 9,outFile); 
    fprintf(outFile,sep);}
  if (control->ActiveK)  {
    testva[10]=1; testva[11]=1; testva[12]=1; testva[13]=1; testva[14]=1;
    testva[15]=1;
    CalcSummary(10,15,outFile); 
    fprintf(outFile,sep);}

  //Here we should check if there is good ladder

  for (int va=0;va<va_per_ladder;va++){
    fSRawlow[va_per_ladder]   += fSRawlow[va];
    fSRawhigh[va_per_ladder]  += fSRawhigh[va];
    fSigmalow[va_per_ladder]  += fSigmalow[va];
    fSigmahigh[va_per_ladder] += fSigmahigh[va];
    funder[va_per_ladder]     += funder[va];
    fover[va_per_ladder]      += fover[va];
    fnongaus[va_per_ladder]   += fnongaus[va];
    fgainlow[va_per_ladder]   += fgainlow[va];

    if (va < 10 ) {
      fSRawlow[va_per_ladder+1]   += fSRawlow[va];
      fSRawhigh[va_per_ladder+1]  += fSRawhigh[va];
      fSigmalow[va_per_ladder+1]  += fSigmalow[va];
      fSigmahigh[va_per_ladder+1] += fSigmahigh[va];
      funder[va_per_ladder+1]     += funder[va];
      fover[va_per_ladder+1]      += fover[va];
      fnongaus[va_per_ladder+1]   += fnongaus[va];
      fgainlow[va_per_ladder+1]   += fgainlow[va];
    } else {
      fSRawlow[va_per_ladder+2]   += fSRawlow[va];
      fSRawhigh[va_per_ladder+2]  += fSRawhigh[va];
      fSigmalow[va_per_ladder+2]  += fSigmalow[va];
      fSigmahigh[va_per_ladder+2] += fSigmahigh[va];
      funder[va_per_ladder+2]     += funder[va];
      fover[va_per_ladder+2]      += fover[va];
      fnongaus[va_per_ladder+2]   += fnongaus[va];
      fgainlow[va_per_ladder+2]   += fgainlow[va];

    }
  }
  if (fSRawlow[va_per_ladder] != 0)
  fprintf(outFile,
	  "Total number of Sigma Raw low noise ch: %3d  %3d on S  %3d on K\n",
	  fSRawlow[va_per_ladder],
	  fSRawlow[va_per_ladder+1],
	  fSRawlow[va_per_ladder+2]);
  if (fSRawhigh[va_per_ladder] != 0)
  fprintf(outFile,
	  "Total number of Sigma Raw high noise ch: %3d  %3d on S  %3d on K\n",
	  fSRawhigh[va_per_ladder],
	  fSRawhigh[va_per_ladder+1],
	  fSRawhigh[va_per_ladder+2]);
  if (fSigmalow[va_per_ladder] != 0)
  fprintf(outFile,
	  "Total number of Sigma low noise ch: %3d  %3d on S  %3d on K\n",
	  fSigmalow[va_per_ladder],
	  fSigmalow[va_per_ladder+1],
	  fSigmalow[va_per_ladder+2]);
  if (fSigmahigh[va_per_ladder] != 0)
  fprintf(outFile,
	  "Total number of Sigma high noise ch: %3d  %3d on S  %3d on K\n",
	  fSigmahigh[va_per_ladder],
	  fSigmahigh[va_per_ladder+1],
	  fSigmahigh[va_per_ladder+2]);
  if (funder[va_per_ladder] != 0)
  fprintf(outFile,"Total number of low  signal  ch: %3d  %3d on S  %3d on K\n",
	  funder[va_per_ladder],
	  funder[va_per_ladder+1],
	  funder[va_per_ladder+2]);
  if (fover[va_per_ladder] != 0)
  fprintf(outFile,"Total number of high signal  ch: %3d  %3d on S  %3d on K\n",
	  fover[va_per_ladder],
	  fover[va_per_ladder+1],
	  fover[va_per_ladder+2]);
  if (fnongaus[va_per_ladder] != 0)
  fprintf(outFile,"Total number of Non gaussian ch: %3d  %3d on S  %3d on K\n",
	  fnongaus[va_per_ladder],
	  fnongaus[va_per_ladder+1],
	  fnongaus[va_per_ladder+2]);
  if (fgainlow[va_per_ladder] != 0)
  fprintf(outFile,"Total number of Low Gain ch: %3d  %3d on S  %3d on K\n",
	  fgainlow[va_per_ladder],
	  fgainlow[va_per_ladder+1],
	  fgainlow[va_per_ladder+2]);
   

  fbadSRawlow[0]   = 3; fbadSRawlow[1]   = 6;
  fbadSRawhigh[0]  = 3; fbadSRawhigh[1]  = 6;
  fbadSigmalow[0]  = 3; fbadSigmalow[1]  = 6;
  fbadSigmahigh[0] = 3; fbadSigmahigh[1] = 6;
  fbadunder[0]     = 3; fbadunder[1]     = 6;
  fbadover[0]      = 3; fbadover[1]      = 6;
  fbadnongaus[0]   = 3; fbadnongaus[1]   = 6;
  fbadgainlow[0]   = 3; fbadgainlow[1]   = 6;
  memset(fGoodLadder,0,sizeof(fGoodLadder));



  if (control->ActiveS1) QualityLadder(0,4);
  if (control->ActiveS2) QualityLadder(5,9);
  if (control->ActiveK)  QualityLadder(10,15);

  if (fSRawlow[va_per_ladder]>fbadSRawlow[1])
    SETBIT(fGoodLadder[va_per_ladder],0);
  if (fSRawhigh[va_per_ladder]>fbadSRawhigh[1]) 
    SETBIT(fGoodLadder[va_per_ladder],1);
  if (fSigmalow[va_per_ladder]>fbadSigmalow[1])
    SETBIT(fGoodLadder[va_per_ladder],2);
  if (fSigmahigh[va_per_ladder]>fbadSigmahigh[1]) 
    SETBIT(fGoodLadder[va_per_ladder],3);
  if (funder[va_per_ladder]>fbadunder[1]) 
    SETBIT(fGoodLadder[va_per_ladder],4);
  if (fover[va_per_ladder]>fbadover[1]) 
    SETBIT(fGoodLadder[va_per_ladder],5);
  if (fnongaus[va_per_ladder]>fbadnongaus[1]) 
    SETBIT(fGoodLadder[va_per_ladder],6);
  if (fgainlow[va_per_ladder]>fbadgainlow[1]) 
    SETBIT(fGoodLadder[va_per_ladder],7);




  if (fGoodLadder[va_per_ladder] != 0) {
    fprintf(outFile,"Ladder is *NOT* ok\n");
  } else {
    fprintf(outFile,"Ladder is ok\n");
  }

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      if (READBIT(fStatus[va][ch],0)) 
	fprintf(outFile,
		"   va=%02d  ch=%02d has Sigma Raw low noise\n",va+1,ch+1);
  for (int va=0;va<va_per_ladder;va++)
    if (READBIT(fGoodLadder[va],0) && testva[va]) 
      fprintf(outFile,
	      " va %02d: has a lot of Sigma Raw low noise channels\n",va+1);
  if (READBIT(fGoodLadder[va_per_ladder],0)) 
    fprintf(outFile," Ladder has a lot of Sigma Raw low noise channels\n");
 

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      if (READBIT(fStatus[va][ch],1))	
	  fprintf(outFile,
		  "   va=%02d  ch=%02d has Sigma Raw high noise\n",va+1,ch+1);
  for (int va=0;va<va_per_ladder;va++)
    if (READBIT(fGoodLadder[va],1) && testva[va]) 
      fprintf(outFile,
	      " va %02d: has a lot of SIgma Raw high noise channels\n",va+1);
  if (READBIT(fGoodLadder[va_per_ladder],1)) 
    fprintf(outFile," Ladder has a lot of Sigma Raw high noise channels\n");


  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      if (READBIT(fStatus[va][ch],2)) 
	fprintf(outFile,
		"   va=%02d  ch=%02d has Sigma low noise\n",va+1,ch+1);
  for (int va=0;va<va_per_ladder;va++)
    if (READBIT(fGoodLadder[va],2) && testva[va]) 
      fprintf(outFile,
	      " va %02d: has a lot of Sigma low noise channels\n",va+1);
  if (READBIT(fGoodLadder[va_per_ladder],2)) 
    fprintf(outFile," Ladder has a lot of Sigma low noise channels\n");
 

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      if (READBIT(fStatus[va][ch],3))	
	  fprintf(outFile,
		  "   va=%02d  ch=%02d has Sigma high noise\n",va+1,ch+1);
  for (int va=0;va<va_per_ladder;va++)
    if (READBIT(fGoodLadder[va],3) && testva[va]) 
      fprintf(outFile,
	      " va %02d: has a lot of Sigma high noise channels\n",va+1);
  if (READBIT(fGoodLadder[va_per_ladder],3)) 
    fprintf(outFile," Ladder has a lot of Sigma high noise channels\n");



  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      if (READBIT(fStatus[va][ch],4))	
	  fprintf(outFile,"   va=%02d  ch=%02d underflow\n",va+1,ch+1); 
  for (int va=0;va<va_per_ladder;va++)
    if (READBIT(fGoodLadder[va],4) && testva[va]) 
      fprintf(outFile," va %02d: has a lot of underflow channels\n",va+1);
  if (READBIT(fGoodLadder[va_per_ladder],4)) 
    fprintf(outFile," Ladder has a lot of underflow channels\n");

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      if (READBIT(fStatus[va][ch],5))	
	  fprintf(outFile,"   va=%02d  ch=%02d overflow\n",va+1,ch+1);
  for (int va=0;va<va_per_ladder;va++)
    if (READBIT(fGoodLadder[va],5) && testva[va]) 
      fprintf(outFile," va %02d: has a lot of overflow channels\n",va+1);
  if (READBIT(fGoodLadder[va_per_ladder],5)) 
    fprintf(outFile," Ladder has a lot of overflow channels\n");

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      if (READBIT(fStatus[va][ch],6))	
	  fprintf(outFile,"   va=%02d  ch=%02d is non gaussian\n",va+1,ch+1);
  for (int va=0;va<va_per_ladder;va++)
    if (READBIT(fGoodLadder[va],6) && testva[va]) 
      fprintf(outFile," va %02d: has a lot of non gaussian channels\n",va+1); 
  if (READBIT(fGoodLadder[va_per_ladder],6)) 
    fprintf(outFile," Ladder has a lot of non gaussian channels\n");  

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      if (READBIT(fStatus[va][ch],7))	
	  fprintf(outFile,"   va=%02d  ch=%02d has low gain\n",va+1,ch+1);
  for (int va=0;va<va_per_ladder;va++)
    if (READBIT(fGoodLadder[va],7) && testva[va]) 
      fprintf(outFile," va %02d: has a lot of low gain channels\n",va+1); 
  if (READBIT(fGoodLadder[va_per_ladder],7)) 
    fprintf(outFile," Ladder has a lot of low gain channels\n");  

  fclose(outFile);

// copy this file to other place if neccesary

  sprintf(filename2,"%s/%s_%05d.sum",control->fDirLadder,fName,control->NRun);
  sprintf(command,"cp %s %s",filename,filename2);
  system(command);

  if (aSaveTest != 0) { 
    sprintf(filename2,"%s/%s-%c-%s-%02d.sum",
	    control->fDirTest,fName,control->AMSTest,
	    control->StationCode,control->AMSTestNumber);
    sprintf(command,"cp %s %s",filename,filename2);
    system(command);
  }

}


void Ladder::CalcSummary(int firstva, int lastva, FILE *outFile){
  for (int va=firstva;va<=lastva;va++){
    for (int ch=0;ch<channel_per_va;ch++){
      fmeanped[va] += fPedestal[va][ch];
      fsigped[va]  += fPedestal[va][ch] * fPedestal[va][ch];
      fmeansig[va] += fSigma[va][ch];
      fsigsig[va]  += fSigma[va][ch] * fSigma[va][ch];
      fmeangan[va] += fGainSlope[va][ch];
      fsiggan[va]  += fGainSlopeErr[va][ch] * fGainSlopeErr[va][ch];
      fSRawlow[va]   += READBIT(fStatus[va][ch],0);
      fSRawhigh[va]  += READBIT(fStatus[va][ch],1);
      fSigmalow[va]  += READBIT(fStatus[va][ch],2);
      fSigmahigh[va] += READBIT(fStatus[va][ch],3);
      funder[va]     += READBIT(fStatus[va][ch],4);
      fover[va]      += READBIT(fStatus[va][ch],5);
      fnongaus[va]   += READBIT(fStatus[va][ch],6);
      fgainlow[va]   += READBIT(fStatus[va][ch],7);
    }
    fmeanped[va] /= channel_per_va;
    fsigped[va] = sqrt(fabs(fsigped[va]/channel_per_va -
			    fmeanped[va]*fmeanped[va])+1e-8);
    fmeansig[va] /= channel_per_va;
    fsigsig[va] = sqrt(fabs(fsigsig[va]/channel_per_va - 
			    fmeansig[va]*fmeansig[va])+1e-8);
    fmeangan[va] /= channel_per_va;
    fsiggan[va] = sqrt(fabs(fsiggan[va]/channel_per_va));

    fprintf(outFile,
      "|%2d|%6.1f|%6.1f|%6.1f|%6.2f|%6.2f|%6.2f|%6.2f|%2d|%2d|%2d|%2d|%2d|%2d|%2d|%2d|\n",
	    va+1,fCNsigma[va],fmeanped[va],fsigped[va],fmeansig[va],
	    fsigsig[va],fmeangan[va],fsiggan[va],fSRawlow[va],
	    fSRawhigh[va],fSigmalow[va],fSigmahigh[va],
	    funder[va],fover[va],fnongaus[va],fgainlow[va]);	     
  }

}
void Ladder::QualityLadder(int firstva, int lastva){

  for (int va=firstva;va<=lastva;va++){
    if (fSRawlow[va]>fbadSRawlow[0]) {
      SETBIT(fGoodLadder[va],0);
      SETBIT(fGoodLadder[va_per_ladder],8);
    }
    if (fSRawhigh[va]>fbadSRawhigh[0]) {
      SETBIT(fGoodLadder[va],1);
      SETBIT(fGoodLadder[va_per_ladder],9);
    }
    if (fSigmalow[va]>fbadSigmalow[0]) {
      SETBIT(fGoodLadder[va],2);
      SETBIT(fGoodLadder[va_per_ladder],10);
    }
    if (fSigmahigh[va]>fbadSigmahigh[0]) {
      SETBIT(fGoodLadder[va],3);
      SETBIT(fGoodLadder[va_per_ladder],11);
    }
    if (funder[va]>fbadunder[0]) {
     SETBIT(fGoodLadder[va],4);
     SETBIT(fGoodLadder[va_per_ladder],12);
    }
    if (fover[va]>fbadover[0]) {
      SETBIT(fGoodLadder[va],5);
      SETBIT(fGoodLadder[va_per_ladder],13);
    }
    if (fnongaus[va]>fbadnongaus[0]) {
      SETBIT(fGoodLadder[va],6);
      SETBIT(fGoodLadder[va_per_ladder],14);
    }
    if (fgainlow[va]>fbadgainlow[0]) {
      SETBIT(fGoodLadder[va],7);
      SETBIT(fGoodLadder[va_per_ladder],15);
    }
  }
}

void Ladder::SaveCalibration(int aSaveTest){
  FILE *outFile;  
  char filename[100],filename2[100];
  char command[250];
  char ext[10];

  //sprintf(ext,"%s",(control->DataMode==kDSPCALIBRATION)?"dsp":"cal"); 
  sprintf(ext,"cal"); 
  sprintf(filename,"%s/%s.%s",control->fDirLadder,fName,ext);
  outFile = fopen(filename,"w");          // Open file

  for (int va=0;va<va_per_ladder;va++)
    fprintf(outFile,"%4d,%10.2f,%10.2f\n",va,fCNmean[va],fCNsigma[va]);
  

  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      int channel = va*channel_per_va+ch+1;
      fprintf(outFile,"%5d%4d%4d%10.2f%10.2f%10.2f%10.2f%4d\n",
	      channel,va+1,ch+1,fPedestal[va][ch],fSigmaRaw[va][ch],
	      fSigma[va][ch],fGainSlope[va][ch],fStatus[va][ch]);
    } 
  fclose(outFile);

  sprintf(filename2,"%s/%s_%05d.%s",control->fDirLadder,fName,control->NRun,ext);
  sprintf(command,"cp %s %s",filename,filename2);
  system(command);

  if (aSaveTest != 0) { 
    sprintf(filename2,"%s/%s-%c-%s-%02d.%s",
	    control->fDirTest,fName,control->AMSTest,
	    control->StationCode,control->AMSTestNumber,ext);
    sprintf(command,"cp %s %s",filename,filename2);
    system(command);
  }
  // Eventually here will go the filling of summary calib histograms
}

int Ladder::ReadRefCalibration(){
  FILE *ifcal;
  char filename[200];
  int dum1,dum2,dum3;

  sprintf(filename,"%s/%s.refcal",control->fDirLadder,fName);

  printf("Opening calibration file: %s\n",filename);

  ifcal = fopen(filename,"rt");          // Open file

  if (!ifcal) return 1; // file problem

  for (int va=0;va<va_per_ladder;va++) { // reading CN values
    fscanf(ifcal,"%d,%lf,%lf",&dum1,&fCNmean[va],&fCNsigma[va]);
    //    printf("%d %lf %lf\n",dum1,fCNmean[va],fCNsigma[va]);

  }

  for (int va=0;va<va_per_ladder;va++)  // reading channel constants 
    for (int ch=0;ch<channel_per_va;ch++){
      fscanf(ifcal,"%d%d%d%lf%lf%lf%lf%d",&dum1,&dum2,&dum3,
	     &fRefPedestal[va][ch],&fRefSigmaRaw[va][ch],
	     &fRefSigma[va][ch],&fRefGainSlope[va][ch],&fRefStatus[va][ch]);
    }
  fclose(ifcal);  
  //printf("Calibration constants loaded\n");

  //  FillPedestal();
  FillRefPedestal();
  FillRefSigma();
  FillRefSigmaRaw();
  return 0;
}


int Ladder::ReadCalibration(){
  FILE *ifcal;
  char filename[200];
  int dum1,dum2,dum3;

  sprintf(filename,"%s/%s.cal",control->fDirLadder,fName);

  printf("Opening calibration file: %s\n",filename);

  ifcal = fopen(filename,"rt");          // Open file

  if (!ifcal) return 1; // file problem

  for (int va=0;va<va_per_ladder;va++) { // reading CN values
    fscanf(ifcal,"%d,%lf,%lf",&dum1,&fCNmean[va],&fCNsigma[va]);
    //    printf("%d %lf %lf\n",dum1,fCNmean[va],fCNsigma[va]);

  }

  for (int va=0;va<va_per_ladder;va++)  // reading channel constants 
    for (int ch=0;ch<channel_per_va;ch++){
      fscanf(ifcal,"%d%d%d%lf%lf%lf%lf%d",&dum1,&dum2,&dum3,
	     &fPedestal[va][ch],&fSigmaRaw[va][ch],
	     &fSigma[va][ch],&fGainSlope[va][ch],&fStatus[va][ch]);
    }
  fclose(ifcal);  
  //printf("Calibration constants loaded\n");

  //  FillPedestal();
  FillPedestal();
  FillSigma();
  FillSigmaRaw();
  FillStatus();
  FillDiffPedestal();
  FillSummaryPlots();
  return 0;
}



void Ladder::SubstractPedestal(){
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      fReadOut[va][ch] -= fPedestal[va][ch];
}

void Ladder::ComputeCN1(){
  memset(fCN,0,sizeof(fCN));
  memset(fNst,0,sizeof(fNst));

  int mask_channelOK = 511; //  511=111111111 508=111111100   (old 91=1011011) 
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      CLRBIT(fStatus[va][ch],8);
      if ((fReadOut[va][ch] < -3.0*fSigmaRaw[va][ch]) ||
	  (fReadOut[va][ch] >  3.0*fSigmaRaw[va][ch])) 
	SETBIT(fStatus[va][ch],8); 
    }
    
    
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      if ( !(fStatus[va][ch] & mask_channelOK) ){
	fCN[va] += fReadOut[va][ch]; // Compute CN
	fNst[va] +=1;
      }
    }

  for (int va=0;va<va_per_ladder;va++){
    if (fNst[va] > 0 ){
      fCN[va] /= fNst[va];
    } else {
      fCN[va]=0;
    }
    int bin = GetBin(kHISTO_CN,fCN[va]);
    histo->fhCN[va][bin] += 1;
  }
}


void Ladder::ComputeCN2(){ /// Falta modificar el status
  memset(fCN,0,sizeof(fCN));
  memset(fNst,0,sizeof(fNst));

  boolean converge=false;
  int Nstall=0;

  float fMaj=0.6;
  float aMaj=0.0;

  int mask_channelOK = 511; // 511=111111111  508=111111100   (old 91=1011011) 
  for (int va=0;va<va_per_ladder;va++){
    int chref=1;
    converge=false;
    while (!converge && chref<channel_per_va){
      fNst[va]=0;
      fCN[va]=0;
      Nstall=0;
      if ( fStatus[va][chref] & mask_channelOK ) {
       	converge=false;
	chref++;	  
      } else {
	double aRef=fReadOut[va][chref];
	for (int ch=0;ch<channel_per_va;ch++){
	  CLRBIT(fStatus[va][ch],8);
	  if ( !(fStatus[va][ch] & mask_channelOK) ) {
	    Nstall++;
	    if ((fabs(fReadOut[va][ch]-aRef)<3.0*fSigmaRaw[va][chref])){
	      fNst[va]++;
	      fCN[va] += fReadOut[va][ch];
	    }
	  }
	}
	aMaj = (float)fNst[va]/(float)Nstall;
	if ( aMaj > fMaj){
	  converge = true;
	  fCN[va] /= fNst[va]; 
	} else {	  
	  chref++;
	  fCN[va]=0.0;
	  Nstall=0;
          fNst[va]=0;
	}
      }
    }
    
    int bin = GetBin(kHISTO_CN,fCN[va]);
    histo->fhCN[va][bin] += 1;
    //    printf("CN2 info: va=%02d first=%2d Nst=%2d All=%2d Maj=%6.4f\n"
    //    	   ,va,chref,fNst[va],Nstall,aMaj);
    //if (!converge) printf("Sorry no convergence\n");      
  }


}

void Ladder::SubstractCN(){
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++)
      fReadOut[va][ch] -= fCN[va];  
}

void Ladder::FillPedestal(){
  int i;
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      i=va*channel_per_va+ch+1;
      histo->fhPedestal[i] = fPedestal[va][ch];
    }
}

void Ladder::FillSigma() {
  memcpy(&histo->fhSigma[1],fSigma,sizeof(fSigma));
}

void Ladder::FillSigmaRaw() {
  memcpy(&histo->fhSigmaRaw[1],fSigmaRaw,sizeof(fSigmaRaw));
}

void Ladder::FillRefPedestal(){
  int i;
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      i=va*channel_per_va+ch+1;
      histo->fhRefPedestal[i] = fRefPedestal[va][ch];
    }
}


void Ladder::FillRefSigma(){
  int i;
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      i=va*channel_per_va+ch+1;
      histo->fhRefSigma[i] = fRefSigma[va][ch];
    }
}


void Ladder::FillRefSigmaRaw(){
  int i;
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      i=va*channel_per_va+ch+1;
      histo->fhRefSigmaRaw[i] = fRefSigmaRaw[va][ch];
    }
}


void Ladder::ReadDSPCalib() {
  int i;
  for (int va=0;va<va_per_ladder;va++) {
    fCNmean[va]=0;
    fCNsigma[va]=0;
    for (int ch=0;ch<channel_per_va;ch++){
      i=va*channel_per_va+ch+1;
      //      histo->fhRefPedestal[i] = fRefPedestal[va][ch];
      fPedestal[va][ch]=histo->fhDSPPedestal[i];
      fSigma[va][ch]=histo->fhDSPSigma[i];
      fSigmaRaw[va][ch]=histo->fhDSPSigmaRaw[i];
      fStatus[va][ch]=histo->fhDSPStatus[i];
    }
  }
  FillPedestal();
  FillSigma();
  FillSigmaRaw();
  FillStatus();
  FillDiffPedestal();
  FillSummaryPlots();
}



void Ladder::FillDiffPedestal(){
  int i;
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      i=va*channel_per_va+ch+1;
      histo->fhDiffPedestal[i] = fPedestal[va][ch]-fRefPedestal[va][ch];
    }
}


void Ladder::FillStatus() {
  int i;
  memset((histo->fhStatus),0,sizeof(histo->fhStatus));
  for (int va=0;va<va_per_ladder;va++) {
    if ((control->ActiveS1==0) && (va<5)) continue;
    if ((control->ActiveS2==0) && (va>4) && (va<10)) continue;
    if ((control->ActiveK==0) && (va>9)) continue;
    for (int ch=0;ch<channel_per_va;ch++){
      i=va*channel_per_va+ch+1;
      histo->fhStatus[i] = fStatus[va][ch];
    }
  }
}

void Ladder::FillSummaryPlots(){
 int bin;
 int ichain=0;
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){

      bin = GetBin(kHISTO_PEDSUM,fPedestal[va][ch]);
      histo->fhPedSum[bin]++;

      bin = GetBin(kHISTO_SIGRAWSUM,fSigmaRaw[va][ch]);
      histo->fhSigRawSum[bin]++;

      bin = GetBin(kHISTO_SIGSUM,fSigma[va][ch]);
      histo->fhSigSum[bin]++;
    }
}


void Ladder::FillRawCal(){
  for (int va=0;va<va_per_ladder;va++)
    for (int ch=0;ch<channel_per_va;ch++){
      // Fill Pedestal Histo       
      int bin = GetBin(kHISTO_PEDESTAL,fReadOut[va][ch]);
      histo->fhRawCal[va][ch][bin]++;
    }
}

int Ladder::Getva(int aChannel){
  int va = aChannel/channel_per_va;
  if (va<0)va=0;
  if (va>=va_per_ladder) va = va_per_ladder;
  return va;
} 

int Ladder::Getch(int aChannel){
  int ch = aChannel%channel_per_va;
  if (ch<0)ch=0;
  if (ch>=channel_per_va) ch = channel_per_va;
  return ch;
} 
 

void Ladder::OpenOutputFile(){
  char comment[10];
  switch (control->LogStatus) {
  case kLOGSTATUS_BINARY:
    switch (control->DataMode) {
    case kDATAMODE_SOURCE:
      switch (control->SourceSequence) {
      case 0: sprintf(comment,"cal"); break;
      case 1: sprintf(comment,"2b"); break;
      case 2: sprintf(comment,"2c"); break;
      case 3: sprintf(comment,"2d"); break;
      }
      sprintf(control->fFileName,"%s/%s-%c-%s-%02d-%s.raw",
	      control->fDirTest,fName,control->AMSTest,
	      control->StationCode,control->AMSTestNumber,comment);
      fRawDataFile=fopen(control->fFileName,"wb");
      if (control->SourceSequence > 0) {
	sprintf(control->fFileName,"%s/%s-%c-%s-%02d-%s.tdr",
		control->fDirTest,fName,control->AMSTest,
		control->StationCode,control->AMSTestNumber,comment);
	fTDRDataFile=fopen(control->fFileName,"wb");
      }
      break;
    case kDATAMODE_REDUCTION:
      sprintf(control->fFileName,"%s/%s_%05d.binary",
	      control->fDirLadder,fName,control->NRun);
      fRawDataFile=fopen(control->fFileName,"wb");

      sprintf(control->fFileName,"%s/%s_%05d.tdr",
	      control->fDirLadder,fName,control->NRun);
      fTDRDataFile=fopen(control->fFileName,"wb");
      break;
    default:
      sprintf(control->fFileName,"%s/%s_%05d.binary",
	      control->fDirLadder,fName,control->NRun);
      fRawDataFile=fopen(control->fFileName,"wb");
    }
    break;
  case kLOGSTATUS_ASCII:
    sprintf(control->fFileName,"%s/%s_%05d.ascii",
	    control->fDirLadder,fName,control->NRun);
    fRawDataFile=fopen(control->fFileName,"w");
    break;
  case kLOGSTATUS_TEST:
    sprintf(control->fFileName,"%s/%s-%c-%s-%02d.raw",
	    control->fDirTest,fName,control->AMSTest,
	    control->StationCode,control->AMSTestNumber);
    fRawDataFile=fopen(control->fFileName,"wb");

  }
}

void Ladder::CloseOutputFile(){
  fclose(fRawDataFile);
  if ((control->DataMode == kDATAMODE_SOURCE && control->SourceSequence > 0) ||
        (control->DataMode == kDATAMODE_REDUCTION && 
         control->LogStatus != kLOGSTATUS_NOTLOG)) 
  fclose(fTDRDataFile);
  strcpy(control->fFileName," ");
}

void Ladder::SaveEvent(){
  switch (control->LogStatus) {
  case kLOGSTATUS_BINARY:
    fwrite(fRAW,sizeof(fRAW),1,fRawDataFile);
    break;
  case kLOGSTATUS_ASCII:
    for (int va=0;va<va_per_ladder;va++){
      for (int ch=0;ch<channel_per_va;ch++){
        fprintf(fRawDataFile,"%4d ",(unsigned short)fRAW[va][ch]);
      }
      fprintf(fRawDataFile,"\n");
    }
    break;
  case kLOGSTATUS_TEST:
    fwrite(fRAW,sizeof(fRAW),1,fRawDataFile);
    break;
  }
}


void Ladder::ComputeGain(){
  double a,b,siga,sigb,chi2,q;  
  int aMin=0;
  int aMax=control->MaxGainStep;
  double qmax=1.0e10;

  if (control->MaxGainStep > 0){
    for (int va=0;va<va_per_ladder;va++){
      for (int ch=0;ch<channel_per_va;ch++){
	qmax=1.0e10;  
	for (int imin=0; imin<control->MaxGainStep-6;imin++){
          for (int imax=control->MaxGainStep; imax>imin+6;imax--){
	    fit(&histo->fhDACGain[imin],&histo->fhMeanGain[va][ch][imin],
		&histo->fhSigmaGain[va][ch][imin],imax-imin,
		&a,&b,&siga,&sigb,&chi2,&q);
	    if (chi2<qmax){qmax=chi2; aMin=imin; aMax=imax;}
	  }
	}
	histo->fhGainMin[va][ch]=aMin;
	histo->fhGainMax[va][ch]=aMax;
	fit(&histo->fhDACGain[aMin],&histo->fhMeanGain[va][ch][aMin],
	    &histo->fhSigmaGain[va][ch][aMin],aMax-aMin,
	    &a,&fGainSlope[va][ch],
	    &siga,&fGainSlopeErr[va][ch],&chi2,&q);
	histo->fhGain[va*channel_per_va+ch+1]=fGainSlope[va][ch];
      }
    }
  }
}


void Ladder::fit(double x[], double y[], double sig[], int ndata,
		 double *a,  double *b, double *siga, double *sigb, 
		 double *chi2, double *q)
{
  int i;
  float wt,t,sxoss,sx,sy,st2,ss;

  *b=0.0;
  sx=0.0; 
  sy=0.0; 
  ss=0.0;
  st2=0.0;

  for (i=0;i<ndata;i++) {
    sig[i] += 0.0000001;
    wt=1.0/(sig[i]*sig[i]);
    ss += wt;
    sx += x[i]*wt;
    sy += y[i]*wt;
  }
  sxoss=sx/ss;

  for (i=0;i<ndata;i++) {
    t=(x[i]-sxoss)/sig[i];
    st2 += t*t;
    *b += t*y[i]/sig[i];
  }


  *b /= st2;
  *a=(sy-sx*(*b))/ss;
  *siga=sqrt((1.0+sx*sx/(ss*st2))/ss);
  *sigb=sqrt(1.0/st2);
  *chi2=0.0;
	
  for (i=0;i<ndata;i++)
      *chi2 += ((y[i]-(*a)-(*b)*x[i])/sig[i])*((y[i]-(*a)-(*b)*x[i])/sig[i]);
  *q=gammq(0.5*(ndata-2),0.5*(*chi2));
      
}

double Ladder::gammln(double xx){
  double x,y,tmp,ser;
  static double cof[6]={76.18009172947146,-86.50532032941677,
			24.01409824083091,-1.231739572450155,
			0.1208650973866179e-2,-0.5395239384953e-5};
  int j;
  
  y=x=xx;
  tmp=x+5.5;
  tmp -= (x+0.5)*log(tmp);
  ser=1.000000000190015;
  for (j=0;j<=5;j++) ser += cof[j]/++y;
  return -tmp+log(2.5066282746310005*ser/x);
}




double Ladder::gammq(double a, double x){
  double gamser,gammcf,gln;
 
  //if (x < 0.0 || a <= 0.0) nrerror("Invalid arguments in routine gammq");
  if (x < (a+1.0)) {
    gser(&gamser,a,x,&gln);
    return 1.0-gamser;
  } else {
    gcf(&gammcf,a,x,&gln);
    return gammcf;
  }
}

 
void Ladder::gser(double *gamser, double a, double x, double *gln){
  int n;
  double sum,del,ap;
  int ITMAX=100;
  double EPS=3.0e-7;

  *gln=gammln(a);
  if (x <= 0.0) {
    *gamser=0.0;
    return;
  } else {
    ap=a;
    del=sum=1.0/a;
    for (n=1;n<=ITMAX;n++) {
      ++ap;
      del *= x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*EPS) {
	*gamser=sum*exp(-x+a*log(x)-(*gln));
	return;
      }
    }
    return;
  }
}


void Ladder::gcf(double *gammcf, double a, double x, double *gln){
  int i;
  double an,b,c,d,del,h;
  int ITMAX=100;
  double EPS=3.0e-7;
  double FPMIN=1.0e-30;
 
  *gln=gammln(a);
  b=x+1.0-a;
  c=1.0/FPMIN;
  d=1.0/b;
  h=d;
  for (i=1;i<=ITMAX;i++) {
    an = -i*(i-a);
    b += 2.0;
    d=an*d+b;
    if (fabs(d) < FPMIN) d=FPMIN;
    c=b+an/c;
    if (fabs(c) < FPMIN) c=FPMIN;
    d=1.0/d;
    del=d*c;
    h *= del;
    if (fabs(del-1.0) < EPS) break;
  }
  *gammcf=exp(-x+a*log(x)-(*gln))*h;
}



void Ladder::StartFFT() {
  SubstractPedestal();
  UpdatePedSub();
  ComputeCN2();
  SubstractCN();

  //printf("welcome to startfft\n");

  int nchanS=va_per_adc_S*channel_per_va, nchanK=va_per_adc_K*channel_per_va;
  int startK=2*nchanS;
  double adcS1[nchanS], adcS2[nchanS], adcK[nchanK];
  
  for(int chan=0; chan<nchanS; chan++) {
    adcS1[chan]=fReadOut[Getva(chan)][Getch(chan)];
    adcS2[chan]=fReadOut[va_per_adc_S+Getva(chan)][Getch(chan)];
  }
  DoFFT(adcS1, nchanS, fSpectrumS1);
  DoFFT(adcS2, nchanS, fSpectrumS2);
  
  for(int chan=startK; chan<startK+nchanK; chan++) adcK[chan-startK]=fReadOut[Getva(chan)][Getch(chan)];
  DoFFT(adcK, nchanK, fSpectrumK);

  //for (int chan=0; chan<320; chan++) printf("ch=%3d  adcS1=%3.1lf adcS2=%3.1lf adcK=%3.1lf specS1=%f\n",chan,adcS1[chan],adcS2[chan],adcK[chan],fSpectrumS1[chan]);

  UpdateSpectrum();

}


void Ladder::DoFFT(double* array, int nech, double* his) {
  fftw_complex *out;

  //for (int i=0; i<nech; i++)  printf("i=%3d  array=%lf\n",i,array[i]);

  //printf("this is %s\n", his->GetName());

  fftw_plan fftp;
  out=new fftw_complex[nech/2+1];
  fftp=fftw_plan_dft_r2c_1d(nech,array,out,FFTW_ESTIMATE);
  fftw_execute(fftp);

  float val;
  for (int i=0; i<(nech/2+1);i++) {
    val=(out[i][0]*out[i][0]+out[i][1]*out[i][1])/nech;
    his[1+i]+=val; // +1 because we then fill a histogram object
    //his->SetBinContent(i+1,his->GetBinContent(i+1)+val);
  }
  
  fftw_destroy_plan(fftp);
  delete out;
  
}


void Ladder::UpdateSpectrum() {
  memcpy(&histo->fhSpectrumS1[1],fSpectrumS1,sizeof(fSpectrumS1));
  memcpy(&histo->fhSpectrumS2[1],fSpectrumS2,sizeof(fSpectrumS2));
  memcpy(&histo->fhSpectrumK[1],fSpectrumK,sizeof(fSpectrumK));
}
