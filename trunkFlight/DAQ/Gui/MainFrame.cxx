#include "MainFrame.h"
#include "GUITimer.h"

// TDRGMainFrame
// Create (the) TDR Graphic User Interface Display.
//
// p = pointer to the client (not owner)
// w = width of TDRGMainFrame frame
// h = width of TDRGMainFrame frame
MainFrame::MainFrame(const TGWindow *p, UInt_t w, UInt_t h) : 
  TGMainFrame(p, w, h) {
  
  SetCleanup(kDeepCleanup);
  
  char file[200];

  // Creating queue messages  
#define Q_CTL
#define Q_LOG
#define Q_GUI
#undef  Q_RUN
#undef  Q_DSP
#include "msg_definition.code"

  // Creating share memory segments
#define S_CONTROL
#undef  S_DATA
#define S_HISTO
#include "shm_definition.code"

  DefineFonts *DefFonts=new DefineFonts();

  // GUI is alive!!! as ProcessGUI is asynchronous should be notified here
  toGUI->Wait(kCTL,&mtype,&par1,&par2);
  if (par1 == kWakeUp){
    toCTL->Send(kGUI,kReady,0);
  }

  cout << "CreateStyles" << endl;
  CreateStyles();
  cout << "after createstyles"<< endl;

  //TTF *fontset=new TTF();
  //fontset->SetSmoothing(kFALSE); // no antialiasing, hopefully this will speedup display

  // Define Timers
  GUITimer *ListenGUI = new GUITimer(this,100,kTRUE);
  gSystem->AddTimer(ListenGUI);

  for (int typ=0; typ<MAXTYP; typ++) 
    for (int copycnt=0; copycnt<SCOPECOPYMAX; copycnt++) fhh[typ][copycnt]=NULL;

  DefineHistos();

  // Define colors
  fClient->GetColorByName("green",fGreen);
  fClient->GetColorByName("red",fRed);
  fClient->GetColorByName("yellow",fYellow);

  //Work Area variables init
  sprintf(fOldLadderName,"none");
  fOldHisto=0;
  fOldva=0;
  fOldch=0,
  fOldfirstva=0;
  fOldlastva=0;
  fOldfirstch=0;
  fOldlastch=0;
  fOldMinimumX=0.0;
  fOldMaximumX=4096.0;
  fOldMinimumY=0.0;
  fOldMaximumY=4096.0;
  fOldRemain=0;

  fShowHisto=kHISTO_SCOPE;
  fMinimumX=histo->Minimum[fShowHisto];
  fMaximumX=histo->Maximum[fShowHisto];
  fMinimumY=histo->MinY[fShowHisto];
  fMaximumY=histo->MaxY[fShowHisto];



  // Define Layouts
  fL1 = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX,0,0,0,0);
  fL2 = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX,0,0,0,0);

  cout << "Menubar" << endl;
  MenuBar();
  AddFrame(fMenuBar, fL1);
  
  //  Title();
  //  AddFrame(fTitle, fL1);

  Work();
  AddFrame(fWork,fL2);

  SetWindowName(Form("AMS Tracker Test System version %s",control->Version));
  SetIconName("AMSt Test System");

  ////CheckProcess(kGUI);
  CheckProcess(kCTL);
  CheckProcess(kRUN);
  CheckProcess(kDSP);
  CheckProcess(kLOG);
  

  MapSubwindows();
  Layout();
  MapWindow();

  fHistCnt=-1;

  //SelectLadder *mySelectLadder;
  //mySelectLadder = new SelectLadder(gClient->GetRoot(),this,300,100);  
}

MainFrame::~MainFrame(){ }

//Bool_t TTimer::Notify(){
//  cerr << "TTIMER: FUNCIONA!!!" << endl;
//  Reset();
//  return kTRUE;
//}


void MainFrame::DefineHistos(){

  CreateHisto2(kHISTO_SCOPE,HISTO_SCOPE_BIN,
	      0.5,1024.5,0.0,5000.0,"Scope");
  CreateHisto2(kHISTO_SIGNAL,HISTO_SIGNAL_BIN,
	      0.5,1024.5,-400.0,4000.0,"Signal");
  CreateHisto(kHISTO_SIGNAL_MEAN,HISTO_SIGNAL_MEAN_BIN,
	      0.5,1024.5,-20.0,5000.0,"Signal Mean");
  CreateHisto(kHISTO_SIGNAL_HITS,HISTO_SIGNAL_HITS_BIN,
	      0.5,1024.5,-20.0,10000000.0,"Signal Hits");
  CreateHisto(kHISTO_RAWCAL,HISTO_RAWCAL_BIN,
	      0.0,1600.0,0.0,10000000.0,"Raw Calibration");
  CreateHisto(kSAVE_HISTO_RAWCAL,HISTO_RAWCAL_BIN,
	      0.0,1600.0,0.0,10000000.0,"Raw Calibration Save");
  CreateHisto(kHISTO_PEDESTAL,HISTO_PEDESTAL_BIN,
	      0.5,1024.5,0.0,5000.0,"Pedestal");
  CreateHisto(kHISTO_PEDSUM,HISTO_PEDSUM_BIN,
	      0.5,2048.5,0.0,1024.0,"Pedestal Summary");
  CreateHisto(kHISTO_SIGMARAW,HISTO_SIGMARAW_BIN,
	      0.5,1024.5,0.0,1024.0,"Sigma Raw");
  CreateHisto(kHISTO_SIGRAWSUM,HISTO_SIGRAWSUM_BIN,
	      0.5,60.5,0.0,1024.0,"Sigma Raw Summary");
  CreateHisto(kHISTO_CN,HISTO_CN_BIN,
	      -100.0,100.0,0.0,10000000.0,"Common Noise");
  CreateHisto(kHISTO_SIGMA,HISTO_SIGMA_BIN,
	      0.5,1024.5,0.0,1024.0,"Sigma");
  CreateHisto(kHISTO_SIGSUM,HISTO_SIGSUM_BIN,
	      0.5,20.5,0.0,1024.0,"Sigma Summary");
  CreateHisto(kHISTO_GAIN,HISTO_GAIN_BIN,
	      0.5,1024.5,0.0,50.0,"Gain Summary");
  CreateHisto(kHISTO_GAIN_VALUE,HISTO_GAIN_VALUE_BIN,
	      0.5,5000.5,0.0,5000.0,"Gain Values");
  CreateHisto(kHISTO_GAINPLOT,HISTO_GAINPLOT_BIN,
	      0.0,256.0,0.0,5096.0,"Gain Plot");
  CreateHisto(kHISTO_SHAPING,HISTO_SHAPING_BIN,
	      0.0,18.0,0.0,5000.0,"Shaping Time");
  CreateHisto(kHISTO_CLUSTER_LENGTH_S,HISTO_CLUSTER_LENGTH_S_BIN,
	      0.5,10.5,0.0,10000000.0,"Cluster Length S side");
  CreateHisto(kHISTO_CLUSTER_LENGTH_K,HISTO_CLUSTER_LENGTH_K_BIN,
	      0.5,10.5,0.0,10000000.0,"Cluster Length K side");
  CreateHisto(kHISTO_CLUSTER_NUMBER_S,HISTO_CLUSTER_NUMBER_S_BIN,
	      0.5,20.5,0.0,10000000.0,"Cluster Number S side");
  CreateHisto(kHISTO_CLUSTER_NUMBER_K,HISTO_CLUSTER_NUMBER_K_BIN,
	      0.5,20.5,0.0,10000000.0,"Cluster Number K side");
  CreateHisto(kHISTO_CLUSTER_COG,HISTO_CLUSTER_COG_BIN,
	      0.0,1024.0,0.0,10000000.0,"Cluster cog");
  CreateHisto(kHISTO_CLUSTER_MAXLOC,HISTO_CLUSTER_MAXLOC_BIN,
	      -10.0,10.0,0.0,10000000.0,"Cluster Maxloc-Cog");
  CreateHisto(kHISTO_CLUSTER_INTEGRAL_S,HISTO_CLUSTER_INTEGRAL_S_BIN,
	      0.5,300.5,0.0,10000000.0,"Cluster Integral S side");
  CreateHisto(kHISTO_CLUSTER_INTEGRAL_K,HISTO_CLUSTER_INTEGRAL_K_BIN,
	      0.5,300.5,0.0,10000000.0,"Cluster Integral K side");
  CreateHisto(kHISTO_CLUSTER_SOVERN_S,HISTO_CLUSTER_SOVERN_S_BIN,
	      0.5,30.5,0.0,10000000.0,"Cluster Signal/Noise S side");
  CreateHisto(kHISTO_CLUSTER_SOVERN_K,HISTO_CLUSTER_SOVERN_K_BIN,
	      0.5,30.5,0.0,10000000.0,"Cluster Signal/Noise K side");
  CreateHisto2(kHISTO_PEDSUB,HISTO_PEDSUB_BIN,
	      0.5,1024.5,-400.0,400.0,"Ped subtr");
  CreateHisto(kHISTO_PEDREF,HISTO_PEDESTAL_BIN,
	      0.5,1024.5,0.0,5000.0,"Reference Pedestal");
  CreateHisto(kHISTO_PEDDIFF,HISTO_PEDESTAL_BIN,
	      0.5,1024.5,-200.0,200.0,"Pedestal Difference with ref");
  CreateHisto(kHISTO_SIGMACOMP,HISTO_SIGMA_BIN,
	      0.5,1024.5,-20.0,20.0,"Sigma Comparison with ref");
  CreateHisto(kHISTO_SIGMARAWCOMP,HISTO_SIGMA_BIN,
	      0.5,1024.5,-50.0,50.0,"Sigma Raw Comparison with ref");
  CreateHisto(kHISTO_SPECTRUM_S1, HISTO_SPECTRUMS_BIN,
	      0,control->ChannelFrequency,0,1e9,"Frequency spectrum (MHz) S1");
  CreateHisto(kHISTO_SPECTRUM_S2, HISTO_SPECTRUMS_BIN,
	      0,control->ChannelFrequency,0,1e9,"Frequency spectrum (MHz) S2");
  CreateHisto(kHISTO_SPECTRUM_K, HISTO_SPECTRUMK_BIN,
	      0,control->ChannelFrequency,0,1e9,"Frequency spectrum (MHz) K");
}



int MainFrame::HHConvert(int aHisto) {
  int ret=0;
  if (aHisto==kHISTO_SCOPE) ret=0; 
  if (aHisto==kHISTO_SIGNAL) ret=1; 
  if (aHisto==kHISTO_PEDSUB) ret=2; 
  if (aHisto==kHISTO_SHAPING) ret=3; 

  return ret;
}


void MainFrame::CreateHisto(int aHisto,int aNbin, 
			    float aMinX, float aMaxX, 
			    float aMinY, float aMaxY, const char *aName){
  float dif;
  char Name[200];
  histo->Nbin[aHisto]    = aNbin;
  histo->Minimum[aHisto] = aMinX;
  histo->Maximum[aHisto] = aMaxX;
  histo->MinY[aHisto] = aMinY; 
  histo->MaxY[aHisto] = aMaxY; 
  dif = (histo->Maximum[aHisto]-histo->Minimum[aHisto])/histo->Nbin[aHisto];
  for (int i=0;i<histo->Nbin[aHisto]+1;i++) { 
    fAxis[aHisto][i]=histo->Minimum[aHisto]+(i-1)*dif;
    if (aHisto == kHISTO_SHAPING) fAxis[aHisto][i]+= 0.01;
  }
  sprintf(Name,"%s",aName);
  fh[aHisto] = new TH1F(aName,aName,histo->Nbin[aHisto],
			histo->Minimum[aHisto],histo->Maximum[aHisto]);   
}



void MainFrame::CreateHisto2(int aHisto,int aNbin, 
			    float aMinX, float aMaxX, 
			    float aMinY, float aMaxY, const char *aName){
  //printf("welcome to createhisto2\n");
  float dif;
  char Name[200];
  histo->Nbin[aHisto]    = aNbin;
  histo->Minimum[aHisto] = aMinX;
  histo->Maximum[aHisto] = aMaxX;
  histo->MinY[aHisto] = aMinY; 
  histo->MaxY[aHisto] = aMaxY; 
  dif = (histo->Maximum[aHisto]-histo->Minimum[aHisto])/histo->Nbin[aHisto];
  for (int i=0;i<histo->Nbin[aHisto]+1;i++) { 
    fAxis[aHisto][i]=histo->Minimum[aHisto]+(i-1)*dif;
    if (aHisto == kHISTO_SHAPING) fAxis[aHisto][i]+= 0.01;
  }
  sprintf(Name,"%s",aName);
  for (int cnt=0; cnt<SCOPECOPYMAX; cnt++)
    fhh[HHConvert(aHisto)][cnt] = new TH1F(Form("%s_%03d",aName,cnt),aName,histo->Nbin[aHisto],
			histo->Minimum[aHisto],histo->Maximum[aHisto]);
  //cout << fhh[HHConvert(aHisto)][0] << endl;   
}




void MainFrame::CreateStyles() { //
  cout << "before plain"<< endl;
  gROOT->SetStyle("Plain");
  cout << "after plain"<< endl;
  //TCanvas *c=new TCanvas("c","c",600,600);
  TStyle *myStyle[2], *tempo;
  //myStyle[0]=0;
  //myStyle[1]=0;
  cout << "before style pointers new operation"<< endl;
  myStyle[0]=new TStyle("StyleWhite", "white");
  cout << "style 0 created" << endl;
  myStyle[1]=new TStyle("StyleBlack", "black");
  cout << "after new styles"<< endl;

  tempo=gStyle;
  Int_t linecol, bkgndcol, histcol;


  for(Int_t style=0; style<2; style++) {
    cout << "Defining style "<< style << endl;
    linecol=kWhite*style+kBlack*(1-style);
    bkgndcol=kBlack*style+kWhite*(1-style);
    histcol=kYellow*style+kBlack*(1-style); // was 95

    myStyle[style]->Copy(*tempo);

    myStyle[style]->SetCanvasBorderMode(0);
    myStyle[style]->SetCanvasBorderSize(1);
    myStyle[style]->SetFrameBorderSize(1);
    myStyle[style]->SetFrameBorderMode(0);
    myStyle[style]->SetPadBorderSize(1);
    myStyle[style]->SetStatBorderSize(1);
    myStyle[style]->SetTitleBorderSize(1);
    myStyle[style]->SetPadBorderMode(0);
    myStyle[style]->SetPalette(1,0);
    myStyle[style]->SetPaperSize(20,27);	
    myStyle[style]->SetFuncColor(kRed);
    myStyle[style]->SetFuncWidth(1);
    myStyle[style]->SetLineScalePS(1); 
    myStyle[style]->SetCanvasColor(bkgndcol);
    myStyle[style]->SetAxisColor(linecol,"XYZ");
    myStyle[style]->SetFrameFillColor(bkgndcol);
    myStyle[style]->SetFrameLineColor(linecol);
    myStyle[style]->SetLabelColor(linecol,"XYZ");
    myStyle[style]->SetPadColor(bkgndcol);
    myStyle[style]->SetStatColor(bkgndcol);
    myStyle[style]->SetStatTextColor(linecol);
    myStyle[style]->SetTitleColor(linecol,"XYZ");
    myStyle[style]->SetTitleFillColor(bkgndcol);
    myStyle[style]->SetTitleTextColor(linecol);
    myStyle[style]->SetLineColor(linecol);
    myStyle[style]->SetMarkerColor(histcol);
    myStyle[style]->SetTextColor(linecol);
    
    myStyle[style]->SetGridColor((style)?13:kBlack);
    myStyle[style]->SetHistFillStyle(1001*(1-style));
    myStyle[style]->SetHistLineColor(histcol);
    myStyle[style]->SetHistFillColor(bkgndcol);
    myStyle[style]->SetOptDate(22);
    myStyle[style]->GetAttDate()->SetTextColor(linecol);
    myStyle[style]->GetAttDate()->SetTextAngle(90);
    myStyle[style]->GetAttDate()->SetTextSize(0.01);
    myStyle[style]->SetDateY(1);
    myStyle[style]->SetDateX(1);


  }

  myStyle[0]->cd(); // Goes to StyleWhite :-(
  //  myStyle[1]->cd(); // Goes to StyleBlack :-)
  
  gROOT->ForceStyle();
  

}






