#include "MainFrame.h"
#include "Messages.h"
#include <TGView.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TFile.h>


TF1 *f1 = new TF1("f1","pol1"); // To be used in ShowHisto()

void MainFrame::CloseWindow()
{
  printf("bye bye !\n");
#include "shm_detach.code"
  toCTL->Send(kGUI,kTDRExit,0);
  sleep(1);
  printf("before closewindow\n");
  //TGMainFrame::CloseWindow();
  printf("Actions: closewindow, after TGMainFrame::CloseWindow()\n");
  gApplication->Terminate(0);
  printf("Actions: after Terminate(0)\n");
  //delete this;
  printf("Actions: after delete\n");
}

void MainFrame::CheckLadder()
{
  int retval1;
  fMsgIconType = kMBIconExclamation; 
  fMsgButtons = kMBYes | kMBNo;
  strcpy(fMsgTitle,"WARNING");
  sprintf(fMsgMessage,"%s%s%s\n%s","Ladder ",control->fLadderName, 
	  " still in use","Do you want to create a new one?");
  new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
	       fMsgIconType,fMsgButtons,&retval1);
  
  if (retval1 == kMBYes) {
    //int retval2;
    //fMsgIconType = kMBIconQuestion; 
    //fMsgButtons = kMBYes | kMBNo;
    //strcpy(fMsgTitle,"Save Ladder");
    //sprintf(fMsgMessage,"%s%s%s","Do you want to save ladder ", 
    //	    control->fLadderName,"?");
    //new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
    //		 fMsgIconType,fMsgButtons,&retval2);    
    //if (retval2 == kMBYes) toCTL->Send(kGUI,kSaveLadder,0);
    toCTL->Send(kGUI,kDeleteLadder,0);
    toCTL->Send(kGUI,kNewLadder,0);
  }
}

void MainFrame::UpdateMinimumX(float aMinimumX){
  char value[200];
  fMinimumX = SetBounds(aMinimumX,histo->Minimum[fShowHisto],fMaximumX);
  sprintf(value,"%5.1f",fMinimumX);
  fHOf32->SetText(value);
}
void MainFrame::UpdateMaximumX(float aMaximumX){
  char value[200];
  fMaximumX = SetBounds(aMaximumX,fMinimumX,histo->Maximum[fShowHisto]);
  sprintf(value,"%5.1f",fMaximumX);
  fHOf42->SetText(value);
}
void MainFrame::UpdateMinimumY(float aMinimumY){
  char value[200];
  fMinimumY = SetBounds(aMinimumY,histo->MinY[fShowHisto],fMaximumY);
  sprintf(value,"%5.1f",fMinimumY);
  fHOf52->SetText(value);
}
void MainFrame::UpdateMaximumY(float aMaximumY){
  char value[200];
  fMaximumY = SetBounds(aMaximumY,fMinimumY,histo->MaxY[fShowHisto]);
  sprintf(value,"%5.1f",fMaximumY);
  fHOf62->SetText(value);
}
void MainFrame::UpdateMainFrame(){
  static int icc;
  //  gSystem->ProcessEvents();

  switch (control->RunStatus) {
  case kRUNSTATUS_STOP:
    ShowHisto();
    break;

  case kRUNSTATUS_RUN:
    icc++;
    //if (icc%1 == 0)
    ShowHisto();
    //if (icc%5 == 0) 
    RefreshRunInfo();

    break;
  }
  gSystem->ProcessEvents();
}


void MainFrame::CheckProcess(int aProc){
  switch (aProc) {
  case kCTL:
    fPCf21->ChangeBackground(fYellow);
    gClient->NeedRedraw(fPCf21);
    break;
  case kDSP:
    fPCf31->ChangeBackground(fYellow);
    gClient->NeedRedraw(fPCf31);
    break;
  case kRUN:
    fPCf22->ChangeBackground(fYellow);
    gClient->NeedRedraw(fPCf22);
    break;
  case kLOG:
    fPCf32->ChangeBackground(fYellow);
    gClient->NeedRedraw(fPCf32);
    break;    
  }  
  gSystem->ProcessEvents();

  toCTL->Send(kGUI,kCheckProcess,aProc);
}

void MainFrame::ProcessOk(int aProc){
  switch (aProc) {
  case kCTL:
    fPCf21->ChangeBackground(fGreen);
    gClient->NeedRedraw(fPCf21);
    break;

  case kDSP:
    fPCf31->ChangeBackground(fGreen);
    gClient->NeedRedraw(fPCf31);
    break;

  case kRUN:
    fPCf22->ChangeBackground(fGreen);
    gClient->NeedRedraw(fPCf22);
    break;

  case kLOG:
    fPCf32->ChangeBackground(fGreen);
    gClient->NeedRedraw(fPCf32);
    break;
  }
  gSystem->ProcessEvents();
}

void MainFrame::ProcessNotOk(int aProc){
  switch (aProc) {
  case kCTL:
    fPCf21->ChangeBackground(fRed);
    gClient->NeedRedraw(fPCf21);
    break;

  case kDSP:
    fPCf31->ChangeBackground(fRed);
    gClient->NeedRedraw(fPCf31);
    break;

  case kRUN:
    fPCf22->ChangeBackground(fRed);
    gClient->NeedRedraw(fPCf22);
    break;

  case kLOG:
    fPCf32->ChangeBackground(fRed);
    gClient->NeedRedraw(fPCf32);
    break;
  }
  gSystem->ProcessEvents();
}

int MainFrame::SetBounds(int ava, int min, int max){
  ava = (ava<min) ? min : ava;
  ava = (ava>max) ? max : ava;
  return ava;
}
float MainFrame::SetBounds(float ava, float min, float max){
  ava = (ava<min) ? min : ava;
  ava = (ava>max) ? max : ava;
  return ava;
}

void MainFrame::UpdateHisto(double *aLink){
  //fh[fShowHisto]->Reset();
  //fh[fShowHisto]->FillN(histo->Nbin[fShowHisto]+2,fAxis[fShowHisto],aLink,1);

  for (int bin=1; bin<histo->Nbin[fShowHisto]+1; bin++) fh[fShowHisto]->SetBinContent(bin,aLink[bin]);

}



void MainFrame::UpdateHisto2(double *aLink){

  //fhh[HHConvert(fShowHisto)][(fHistCnt==-1)?0:fHistCnt%fRemain]->Reset();

  //fhh[HHConvert(fShowHisto)][(fHistCnt==-1)?0:fHistCnt%fRemain]->FillN(histo->Nbin[fShowHisto]+2,fAxis[fShowHisto],aLink,1);
  //printf("nbin=%d\n", histo->Nbin[fShowHisto]);
    for (int bin=1; bin<histo->Nbin[fShowHisto]+1; bin++) fhh[HHConvert(fShowHisto)][(fHistCnt==-1)?0:fHistCnt%fRemain]->SetBinContent(bin,aLink[bin]);


}


void MainFrame::ShowHisto(){
  char Name[100];
  TGraphErrors *gr=NULL;
  TGraph *grsigma=NULL, *grsigmaref=NULL, *grsigmaraw=NULL, *grsigmarawref=NULL;


  //TCanvas *c1 = fCanvas->GetCanvas();
  //fCanvas->GetCanvas()->SetEditable(kFALSE);
  fCanvas->GetCanvas()->cd();

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  //  fCanvas->GetCanvas()->SetGrid();

  if ((fShowHisto != fOldHisto)    || (fRemain   != fOldRemain)   ||
      (fva        != fOldva)       || (fch       != fOldch)       ||
      (ffirstva   != fOldfirstva)  || (flastva   != fOldlastva)   || 
      (ffirstch   != fOldfirstch)  || (flastch   != fOldlastch)   ||
      (fMinimumX  != fOldMinimumX) || (fMaximumX != fOldMaximumX) ||
      (fMinimumY  != fOldMinimumY) || (fMaximumY != fOldMaximumY) ||
      (strcmp(fOldLadderName,control->fLadderName)) ||
      (control->TDRref      != fOldTDRref) ||
      (control->DACValue    != fOldDACValue)){

//     if (fShowHisto != fOldHisto) printf("histo\n");
//     if (fRemain   != fOldRemain) printf("remain\n");
//     if (fva        != fOldva) printf("va\n");
//     if (fch       != fOldch) printf("channel\n");
//     if (ffirstva   != fOldfirstva) printf("firstva\n");
//     if (flastva   != fOldlastva) printf("lastva\n");
//     if  (ffirstch   != fOldfirstch) printf("firstch\n");
//     if (flastch   != fOldlastch) printf("lastch\n");
//     if (fMinimumX  != fOldMinimumX) printf("minx\n");
//     if (fMaximumX != fOldMaximumX) printf("maxx\n");
//     if (fMinimumY  != fOldMinimumY) printf("miny\n");
//     if (fMaximumY != fOldMaximumY) printf("maxy\n");
//     if (strcmp(fOldLadderName,control->fLadderName)) printf("ladnam\n");
//     if (control->DACValue    != fOldDACValue) printf("dacval\n");
//     if (control->TDRref != fOldTDRref) printf("TDRref\n");

    if (fva >=1 && fva<=5){
      control->DACchain=0;
      control->DACpos=(fva-1)*channel_per_va + fch;
    } 
    if (fva >=6 && fva<=10){
      control->DACchain=1; 
      control->DACpos=(fva-6)*channel_per_va + fch;
    } 
    if (fva >=11 && fva<=16){
      control->DACchain=2; 
      control->DACpos=(fva-11)*channel_per_va + fch; 
    }

    if (fShowHisto== kSAVE_HISTO_RAWCAL) control->SaveHistoRawcal=1;
  

    if (fShowHisto != fOldHisto || fva != fOldva || fch != fOldch){
      UpdateMinimumX(histo->Minimum[fShowHisto]);
      UpdateMaximumX(histo->Maximum[fShowHisto]);
      UpdateMinimumY(histo->MinY[fShowHisto]);
      UpdateMaximumY(histo->MaxY[fShowHisto]);
    }

    fOldHisto=fShowHisto;
    fOldva=fva;
    fOldch=fch;
    fOldfirstva=ffirstva;
    fOldlastva=flastva;
    fOldfirstch=ffirstch;
    fOldlastch=flastch;
    fOldMinimumX=fMinimumX;
    fOldMaximumX=fMaximumX;
    fOldMinimumY=fMinimumY;
    fOldMaximumY=fMaximumY;
    sprintf(fOldLadderName,"%s",control->fLadderName);
    fOldDACValue=control->DACValue;
    fOldTDRref=control->TDRref;
    fOldRemain=fRemain;
    fHistCnt=-1;
  }

  TH1F *his=fh[fShowHisto];
  int DrawGrid = 0, SpecialHis=0, va=1;

  switch (fShowHisto) {     // Redefine histo          
  case kHISTO_SCOPE:
    sprintf(Name,"%s - Run %05d Scope",control->fLadderName,control->NRun);
    UpdateHisto2(histo->fhScope); 
    //UpdateMaximumY(4096);    
    DrawGrid=1;
    SpecialHis=1;
    his=fhh[HHConvert(fShowHisto)][(fHistCnt==-1)?0:fHistCnt%fRemain];
    //cout << fhh[HHConvert(fShowHisto)][(fHistCnt==-1)?0:fHistCnt%fRemain] << endl;
    //cout << his << endl;
    break;
  case kHISTO_SIGNAL:
    sprintf(Name,"%s - Run %05d Signal",control->fLadderName,control->NRun);
    UpdateHisto2(histo->fhSignal); 
    //UpdateMaximumY(fMaximumY);
    DrawGrid=1;
    SpecialHis=1;
    his=fhh[HHConvert(fShowHisto)][(fHistCnt==-1)?0:fHistCnt%fRemain];
    break;
  case kHISTO_PEDSUB:
    sprintf(Name,"%s - Run %05d Pedestal subtracted",control->fLadderName,control->NRun);
    UpdateHisto2(histo->fhPedSub); 
    //UpdateMaximumY(fMaximumY);
    DrawGrid=1;
    SpecialHis=1;
    his=fhh[HHConvert(fShowHisto)][(fHistCnt==-1)?0:fHistCnt%fRemain];
    break;
  case kHISTO_SIGNAL_MEAN:
    sprintf(Name,"%s - Run %05d Signal-Mean",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhSignalMean); 
    //UpdateMaximumY(fMaximumY);
    DrawGrid=1;
    break;
  case kHISTO_SIGNAL_HITS:
    sprintf(Name,"%s - Run %05d Signal-Hits",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhSignalHits); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    DrawGrid=1;
    break;
  case kHISTO_RAWCAL:
    sprintf(Name,"%s - Run %05d Raw Calibration VA-%02d Ch-%02d",
	    control->fLadderName,control->NRun,fva,fch);
    UpdateHisto(histo->fhRawCal[fva-1][fch-1]); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    fh[fShowHisto]->Fit("gaus");
    break;

  case kSAVE_HISTO_RAWCAL:
    if (control->SaveHistoRawcal) {
      TFile *f=new TFile("historawcal.root","recreate");
      TH1F *hcopy=0;

      for(int va=1; va<2; va++) for(int ch=1; ch<65; ch++) {
	sprintf(Name,"%s - Run %05d Raw Calibration VA-%02d Ch-%02d",
		control->fLadderName,control->NRun,va,ch);
	UpdateHisto(histo->fhRawCal[va-1][ch-1]);
	fh[fShowHisto]->SetTitle(Name);
	fh[fShowHisto]->SetMaximum(-1111);
	UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
	fh[fShowHisto]->Fit("gaus");
	hcopy=new TH1F();
	fh[fShowHisto]->Copy(*hcopy);
	hcopy->SetName(Name);
	hcopy->Fit("gaus");
	hcopy->Write();
	delete hcopy;
	hcopy=0;
      }
      control->SaveHistoRawcal=0;
      f->Close();
    }

    

    break;

  case kHISTO_PEDESTAL:
    sprintf(Name,"%s - Run %05d Pedestal",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhPedestal); 
    fh[fShowHisto]->SetMaximum(-1111);
    //UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    //UpdateMaximumY(5000);
    DrawGrid=1; 
    break;
  case kHISTO_PEDREF:
    sprintf(Name,"%s - Reference Pedestal",
	    control->fLadderName);
    UpdateHisto(histo->fhRefPedestal); 
    fh[fShowHisto]->SetMaximum(-1111);
    //UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    //UpdateMaximumY(5000);
    DrawGrid=1; 
    break;
  case kHISTO_PEDDIFF:
    sprintf(Name,"%s - Run %05d Pedestal variation with ref",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhDiffPedestal); 
    fh[fShowHisto]->SetMaximum(-1111);
    //UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    //UpdateMaximumY(5000);
    DrawGrid=1; 
    break;
  case kHISTO_PEDSUM:
    sprintf(Name,"%s - Run %05d Pedestal Summary",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhPedSum); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_SIGMARAW:
    sprintf(Name,"%s - Run %05d Sigma Raw",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhSigmaRaw); 
    //UpdateMaximumY(4096);    
    DrawGrid=1;
    break;
  case kHISTO_SIGRAWSUM:
    sprintf(Name,"%s - Run %05d Sigma Raw Summary",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhSigmaRaw); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_CN:
    sprintf(Name,"%s - Run %05d Common Noise VA-%02d",
	    control->fLadderName,control->NRun,fva);
    UpdateHisto(histo->fhCN[fva-1]); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    fh[fShowHisto]->Fit("gaus");
    break;
  case kHISTO_SIGMA:
    sprintf(Name,"%s - Run %05d Sigma",control->fLadderName,control->NRun);
    UpdateHisto(histo->fhSigma); 
    //UpdateMaximumY(100);    
    DrawGrid=1;
    break;	
  case kHISTO_SIGSUM:
    sprintf(Name,"%s - Run %05d Sigma Summary",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhSigSum); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_GAIN:
    sprintf(Name,"%s - Run %05d Gain",control->fLadderName,control->NRun);
    UpdateHisto(histo->fhGain); 
    DrawGrid=1;
    break;	
  case kHISTO_GAIN_VALUE:
    sprintf(Name,"%s - Run %05d Gain VA-%02d Ch-%02d",
	    control->fLadderName,control->NRun,fva,fch);
    UpdateHisto(histo->fhGainValue[fva-1][fch-1]); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());    
    break;	
  case kHISTO_GAINPLOT:
    sprintf(Name,"%s - Run %05d Gain ADC vs DAC VA-%02d Ch-%02d",
	    control->fLadderName,control->NRun,fva,fch);
    UpdateMaximumY(4096.0);    
    if (control->MaxGainStep > 0){
      if (gr) delete gr; // before creating a new object, let's cleanup the old one
      gr = new TGraphErrors(control->MaxGainStep,
			    histo->fhDACGain,
			    histo->fhMeanGain[fva-1][fch-1],
			    histo->fhDACGainError,
			    histo->fhSigmaGain[fva-1][fch-1]);
      gr->SetMarkerStyle(20);
      gr->Draw("P");
      f1->SetRange(histo->fhDACGain[histo->fhGainMin[fva-1][fch-1]],
		   histo->fhDACGain[histo->fhGainMax[fva-1][fch-1]]);
      gr->Fit("f1","R");

    }
    break;	
  case kHISTO_SIGMACOMP:
    sprintf(Name,"%s - Run %05d Sigma Comparison with ref",
	    control->fLadderName,control->NRun);
    if (grsigma) delete grsigma;
    grsigma=new TGraph(1024);

    if (grsigmaref) delete (grsigmaref);
    grsigmaref=new TGraph(1024);

    for (int chan=1; chan<1025; chan++) {
      grsigma->SetPoint(chan,chan,histo->fhSigma[chan]);
      grsigmaref->SetPoint(chan,chan,-histo->fhRefSigma[chan]);
    }
    DrawGrid=1;
    break;
  case kHISTO_SIGMARAWCOMP:
    sprintf(Name,"%s - Run %05d Sigma Raw Comparison with ref",
	    control->fLadderName,control->NRun);
    if (grsigmaraw) delete grsigmaraw;
    grsigmaraw=new TGraph(1024);

    if (grsigmarawref) delete (grsigmarawref);
    grsigmarawref=new TGraph(1024);

    for (int chan=1; chan<1025; chan++) {
      grsigmaraw->SetPoint(chan,chan,histo->fhSigmaRaw[chan]);
      grsigmarawref->SetPoint(chan,chan,-histo->fhRefSigmaRaw[chan]);
    }
    DrawGrid=1;
    break;
  case kHISTO_SHAPING:
    sprintf(Name,"%s - Run %05d Shaping Time VA-%02d Ch-%02d DAC=%3d ",
	    control->fLadderName,control->NRun,fva,fch,control->DACValue);
    UpdateHisto(histo->fhShaping); 
    //UpdateMaximumY(4096.0);    
    break;	
  case kHISTO_CLUSTER_LENGTH_S:
    sprintf(Name,"%s - Run %05d Cluster Length (S side)",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhClusLength_S); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_CLUSTER_LENGTH_K:
    sprintf(Name,"%s - Run %05d Cluster Length (K side)",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhClusLength_K); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_CLUSTER_NUMBER_S:
    sprintf(Name,"%s - Run %05d Number of Clusters (S side)",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhClusNumber_S); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_CLUSTER_NUMBER_K:
    sprintf(Name,"%s - Run %05d Number of Clusters (K side)",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhClusNumber_K); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_CLUSTER_COG:
    sprintf(Name,"%s - Run %05d Cluster Cog",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhClusCog); 
    fh[fShowHisto]->SetMaximum(-1111);    
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    DrawGrid=1;
    break;
  case kHISTO_CLUSTER_MAXLOC:
    sprintf(Name,"%s - Run %05d Cluster Maxloc",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhClusMaxloc); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_CLUSTER_INTEGRAL_S:
    sprintf(Name,"%s - Run %05d Cluster Integral (S side)",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhClusIntegral_S); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_CLUSTER_INTEGRAL_K:
    sprintf(Name,"%s - Run %05d Cluster Integral (K side)",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhClusIntegral_K); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_CLUSTER_SOVERN_S:
    sprintf(Name,"%s - Run %05d Cluster Signal/Noise (S side)",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhClusSovern_S); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_CLUSTER_SOVERN_K:
    sprintf(Name,"%s - Run %05d Cluster Signal/Noise (K side)",
	    control->fLadderName,control->NRun);
    UpdateHisto(histo->fhClusSovern_K); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_SPECTRUM_S1:
    sprintf(Name,"%s - Run %05d Frequency spectrum (ADC S1) MHz", control->fLadderName,control->NRun);
    UpdateHisto(histo->fhSpectrumS1);
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_SPECTRUM_S2:
    sprintf(Name,"%s - Run %05d Frequency spectrum (ADC S2) MHz", control->fLadderName,control->NRun);
    UpdateHisto(histo->fhSpectrumS2); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  case kHISTO_SPECTRUM_K:
    sprintf(Name,"%s - Run %05d Frequency spectrum (ADC K) MHz", control->fLadderName,control->NRun);
    UpdateHisto(histo->fhSpectrumK); 
    fh[fShowHisto]->SetMaximum(-1111);
    UpdateMaximumY(1.1*fh[fShowHisto]->GetMaximum());
    break;
  }
  
  his->SetTitle(Name);
  his->SetMinimum(fMinimumY);
  his->SetMaximum(fMaximumY);
  his->SetAxisRange(fMinimumX,fMaximumX);

  if (SpecialHis) {
    //printf("SpecialHis\n");
    if (fHistCnt==-1) {
      for (int i=0;i<fRemain; i++) { // initialization
	//printf("fRemain=%d ... about to reset hist %d\n",fRemain,i);
	fhh[HHConvert(fShowHisto)][i]->Reset();
	//printf("reset done\n");
	fhh[HHConvert(fShowHisto)][i]->Draw((i)?"same":"");
	if (i==0) ShowBad();
	//printf("histo drawn\n");
      }
    }
  }
  

  if (fShowHisto==kHISTO_SPECTRUM_S1 || fShowHisto==kHISTO_SPECTRUM_S2 || fShowHisto==kHISTO_SPECTRUM_K) his->DrawNormalized();
  else {
    if (!SpecialHis) { his->Draw(); ShowBad(); his->Draw("same");} 
    else if (fRemain==1) { his->Draw(); ShowBad(); his->Draw("same");}
  }
  
  fHistCnt++;
 
  if (fShowHisto == kHISTO_GAINPLOT) gr->Draw("P");
  if (fShowHisto == kHISTO_SIGMACOMP) {
    grsigma->Draw("l");
    grsigmaref->Draw("l");
  }
  if (fShowHisto == kHISTO_SIGMARAWCOMP) {
    grsigmaraw->Draw("l");
    grsigmarawref->Draw("l");
  }
  if (DrawGrid) { 
    //ShowBad();
    GridVA(); 
  } 
  
  fCanvas->GetCanvas()->Modified();
  fCanvas->GetCanvas()->Update();
  if (control->RecordGifMovie) fCanvas->GetCanvas()->Print(Form("%s_%05d.gif+10",control->fLadderName,control->NRun));
  //  data->fOut++;
  gSystem->ProcessEvents();
}
 

void MainFrame::RefreshRunInfo(){
  char str1[15];

  sprintf(str1,"%d",control->NRun);
  fRIf12->SetText(new TGString(str1));

  sprintf(str1,"%d",control->NEvents);  // Update Run Number
  fRIf22->SetText(new TGString(str1));
  int seconds = (control->RunTime/1000)%60;
  int minutes = (control->RunTime/1000)/60;
  int hours   = minutes/60;
  minutes = minutes%60;
  sprintf(str1,"%02d:%02d:%02d",hours,minutes,seconds);
  fRIf32->SetText(new TGString(str1));

  //sprintf(str1,"%-6.1f%s",control->LiveTime," %");
  //fRIf42->SetText(new TGString(str1));
  
  sprintf(str1,"%-8.2f%s",control->EventRate," Hz");
  fRIf52->SetText(new TGString(str1));

  sprintf(str1,"%d",control->NBadPwrS);
  fRIf62->SetText(new TGString(str1));
  
  sprintf(str1,"%d",control->NBadPwrK);
  fRIf72->SetText(new TGString(str1)); 

  gClient->NeedRedraw(fRIf12);
  gClient->NeedRedraw(fRIf22);
  gClient->NeedRedraw(fRIf32);
  //gClient->NeedRedraw(fRIf42);
  gClient->NeedRedraw(fRIf52);
  gClient->NeedRedraw(fRIf62);
  gClient->NeedRedraw(fRIf72);

  gSystem->ProcessEvents();
}


void MainFrame::RefreshId(){
  fIdf12->SetText(new TGString(control->fLadderName));
  fIdf22->Select(control->LogStatus);
  fIdf32->Select(control->DataMode);
  fIdf42->Select(control->Trigger);
  fIdf52->Select(control->Source);

  gClient->NeedRedraw(fIdf11); gClient->NeedRedraw(fIdf12);
  gClient->NeedRedraw(fIdf21); gClient->NeedRedraw(fIdf22);
  gClient->NeedRedraw(fIdf31); gClient->NeedRedraw(fIdf32);
  gClient->NeedRedraw(fIdf41); gClient->NeedRedraw(fIdf42);
  gClient->NeedRedraw(fIdf51); gClient->NeedRedraw(fIdf52);

  gClient->NeedRedraw(fIdbutS1);
  gClient->NeedRedraw(fIdbutS2);
  gClient->NeedRedraw(fIdbutK);

  gClient->NeedRedraw(fId);

  gClient->NeedRedraw(fControl);

  gSystem->ProcessEvents();
}


void MainFrame::RefreshLog(){
  fText->LoadFile("Log/TDR.log"); 
  fText->SetVsbPosition(fText->ReturnLineCount()-5);
}

void MainFrame::RefreshLog2(char *message){
  if (strlen(message)==0) return;
  char tosend[200];
  for (int i=0,j=0; i<strlen(message); i++) {
    tosend[j]=message[i];
    j++;
    if (message[i]=='\n') {
      tosend[j-1]='\0';
      //printf("%s\n",tosend);
      fText->AddLine(tosend);
      fText->SetVsbPosition(fText->ReturnLineCount()-5);
      j=0;
    }
  }
  //  fText->AddLine(message);
  // fText->SetVsbPosition(fText->ReturnLineCount()-5);
}




//        va=control->ShowHisto[1];
//        ch=control->ShowHisto[2];
//        h1->Set(histo->Nbin[3]+2,histo->fhPedestal[2][5]);
//        h1->Draw();
//        control->ShowHisto[0]=kHISTO_SCOPE;


void MainFrame::GridVA(){
  TLine *line = new TLine();

  double xgr; 
  line->SetLineStyle(2);
  line->SetLineColor(kBlue);
  for (int i=1;i<max_number_va;i++){    
    xgr = i*channel_per_va+0.5;
    if (xgr >= fMinimumX && xgr <= fMaximumX) {
      if (i==10) line->SetLineStyle(1);
      if (i>10) line->SetLineColor(kRed);
      line->DrawLine(xgr,fMinimumY,xgr,fMaximumY);
      if (i==10) line->SetLineStyle(2);
    }
  }
  delete line;
}



void MainFrame::ShowBad() {

  if (!(control->ShowBadChannels)) return;

  TBox *box=new TBox();
  box->SetFillStyle(3001);
  box->SetFillColor(kMagenta);
  
  for (int i=0; i<1024; i++) {
    if (histo->fhStatus[i+1]) box->DrawBox(i+0.5,fMinimumY,i+1.5,fMaximumY);
  }

  delete box;
}



void MainFrame::SetFile(int aSource){
  char fff[200];
  switch (aSource) {
  case kSOURCE_BINARYFILE:
    FileDialog(aSource,kFDOpen);
    break;
  case kSOURCE_ASCIIFILE:
    FileDialog(aSource,kFDOpen);
    break;
  case kSOURCE_BEAMTESTX5:
    FileDialog(aSource,kFDOpen);
    break;
  case kSOURCE_TDRCDP:
    FileDialog(aSource,kFDOpen);
    break;
  }

  if (control->fFileName == NULL) {
    toCTL->Send(kGUI,kSetSource,kSOURCE_JINF);
  } else {
    control->LogStatus = kLOGSTATUS_NOTLOG;
    strcpy(fff,control->fFileName);
    sprintf(control->fFileName,"%s%s",control->fDirRawData,fff);
  }


}

void MainFrame::CreateDirectory(char *aDirectory){
  char command[200];
  sprintf(command,"mkdir %s",aDirectory);
  system(command);
}


void MainFrame::SetLastCalibrationAsDefault() {

  char command[200];

  sprintf(command,"cp -f %s/%s.cal %s/%s.refcal",control->fDirLadder,control->fLadderName,control->fDirLadder,control->fLadderName);
  system(command);

}



void MainFrame::SetDefaultCalibration(){
  char oldPath[200];
  char command[200];
  const char *ftcal[]       = { "Cal files"   , "*.cal",
				"All files"   , "*",
				0,0};
  TGFileInfo fi;
  fi.fFileTypes = ftcal;
  //fi.fFileTypes = (char **)ftcal;
  
  sprintf(oldPath,"%s",gSystem->WorkingDirectory());

  gSystem->ChangeDirectory(control->fDirLadder);      
  new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&fi);
  sprintf(command,"rm -f Default.cal"); system(command);
  sprintf(command,"rm -f Default.sum"); system(command);
  sprintf(command,"ln -s %s Default.cal",fi.fFilename); system(command);

  //sprintf(command,"ln -s %s Default.sum",fi.fFilename); system(command);
  gSystem->ChangeDirectory(oldPath);      
}

void MainFrame::AMSTest(char *TestDesc){
  char title[10];

  sprintf(title,"TEST %c",control->AMSTest);

  char MsgMessage[200];
  sprintf(MsgMessage,"     AMS Tracker TEST %c \n",control->AMSTest);
  strcat(MsgMessage, "     ");
  strcat(MsgMessage, TestDesc);  
  strcat(MsgMessage, "\n\n");
  strcat(MsgMessage, "  Do you want to continue??\n");

  int retval;
  new TGMsgBox(fClient->GetRoot(),this,title,MsgMessage,
	       kMBIconExclamation,kMBYes|kMBNo,&retval);
  
  if (retval == kMBYes) {
    toCTL->Send(kGUI,kReadTemp,0);
    usleep(1000);
    mySetLeakTemp = new SetLeakTemp(fClient->GetRoot(),this,300,250); 
    usleep(500000); // Needed to avoid a crash (????)
  } else {
    control->AMSTest='@';
  }
}


void MainFrame::SourceTest(char *TestDesc){
  char title[10];

  sprintf(title,"TEST %c",control->AMSTest);
  

  char MsgMessage[200];
  sprintf(MsgMessage,"     AMS Tracker TEST %c \n",control->AMSTest);
  strcat(MsgMessage, "     ");
  strcat(MsgMessage, TestDesc);  
  strcat(MsgMessage, "\n\n");
  strcat(MsgMessage, "  Do you want to continue??\n");
  strcat(MsgMessage, " Please be sure that the source is in position GARAGE"); 
  int retval;
  new TGMsgBox(fClient->GetRoot(),this,title,MsgMessage,
	       kMBIconExclamation,kMBYes|kMBNo,&retval);
  
  if (retval == kMBYes) {
    toCTL->Send(kGUI,kReadTemp,0);
    usleep(1000);
    mySetLeakTemp = new SetLeakTemp(fClient->GetRoot(),this,300,250); 
    usleep(500000); // Needed to avoid a crash (????)
  } else {
    control->AMSTest='@';
  }
  
}

void MainFrame::ShowTemp(){
  char aMessage[200];
  sprintf(aMessage,"%4.1f C",control->HybridTemp);    	
  fIdf72->SetText(aMessage);
  gClient->NeedRedraw(fIdf72);
  gSystem->ProcessEvents();
}






