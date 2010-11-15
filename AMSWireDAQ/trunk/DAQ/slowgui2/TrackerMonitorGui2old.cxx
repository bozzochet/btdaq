#include "slowgui2.h"
//#include "TrackerLadders.h"


void SlowGui::TrackerMonitor(int tcrate, int octant) {
  ladmax=0;
  zoomcounter=0, zoommin=0;
  //readlist();

  InnerTracker.ReadFile("TrackerLadders/ladlist.txt");

  fTrackerMonitorMainFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
  fTrackerMonitorMainFrame->SetWindowName(Form("Tracker Monitor tcrate %d, octant %d",tcrate, octant));
  //fMainFrame971->SetLayoutBroken(kTRUE);
  
  TGFont *ufont, *progressfont;         // will reflect user font changes
  //   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*");$
  ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*");
  progressfont = gClient->GetFont("-*-helvetica-bold-r-*-*-11-*-*-*-*-*-*-*");
  
  TGGC   *uGC, *uGC2;           // will reflect user GC changes
  // graphics context changes
  GCValues_t valpFrame, valpFrame2;
  valpFrame.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
  gClient->GetColorByName("#000000",valpFrame.fForeground);
  gClient->GetColorByName("#c0c0c0",valpFrame.fBackground);
  valpFrame.fFillStyle = kFillSolid;
  valpFrame.fFont = ufont->GetFontHandle();
  valpFrame.fGraphicsExposures = kFALSE;
  uGC = gClient->GetGC(&valpFrame, kTRUE);
  
  valpFrame2.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
  gClient->GetColorByName("#000000",valpFrame2.fForeground);
  gClient->GetColorByName("#c0c0c0",valpFrame2.fBackground);
  valpFrame2.fFillStyle = kFillSolid;
  valpFrame2.fFont = progressfont->GetFontHandle();
  valpFrame2.fGraphicsExposures = kFALSE;
  uGC2 = gClient->GetGC(&valpFrame2, kTRUE);
  
  ULong_t ucolor;        // will reflect user color changes
  gClient->GetColorByName("#aeee95",ucolor);
  ULong_t ucolor2;
  gClient->GetColorByName("#9da1ff",ucolor2);

  gClient->GetColorByName("#8DFF85",PwgCol[0]); // green
  gClient->GetColorByName("#85BEFF",PwgCol[1]); // blue
  gClient->GetColorByName("#FFB585",PwgCol[2]); // red
  gClient->GetColorByName("#FFFA85",PwgCol[3]); // yellow
  
  TGVerticalFrame *vertframe=new TGVerticalFrame(fTrackerMonitorMainFrame);
  TGHorizontalFrame *horiframe[2];
  horiframe[0]=new TGHorizontalFrame(vertframe);
  horiframe[1]=new TGHorizontalFrame(vertframe);
  
  for (int pwg=0; pwg<4; pwg++) {
    
    TGVerticalFrame *vframe=new TGVerticalFrame(horiframe[(pwg>1)]);
    vframe->SetBackgroundColor(PwgCol[pwg]);
    int pwgpos=0;       
    for (int lay=0; lay<3; lay++) {
      TGHorizontalFrame *hframe=new TGHorizontalFrame(vframe);
      TGGroupFrame *fGroupFrame[2];
      hframe->SetBackgroundColor(PwgCol[pwg]);
      for (int i=0; i<2; i++) {
	
	//	int lad=getladder(X,XTANT,pwg,pwgpos);
	int lad=InnerTracker.FindLadderIndex(XTANT,pwg,pwgpos);
	TkLadder *ladder=InnerTracker.GetLadder(lad);
	
	// "L 09 GI 010" group frame
	fGroupFrame[i] = new TGGroupFrame(hframe,(lad==MAXLAD)?"":Form("%s - TDR %d",ladder->GetName(), ladder->GetTdr()),kVerticalFrame ,uGC->GetGC(),ufont->GetFontStruct(),PwgCol[pwg]);
	//	fGroupFrame[i] = new TGGroupFrame(hframe,Form("%s - TDR %d",ladder->GetName(), ladder->GetTdr()),kVerticalFrame ,uGC->GetGC(),ufont->GetFontStruct(),PwgCol[pwg]);
	fGroupFrame[i]->SetTitlePos(TGGroupFrame::kCenter);
	//fGroupFrame[i]->SetLayoutBroken(kTRUE);
	
	TGHorizontalFrame *myhframe=new TGHorizontalFrame(fGroupFrame[i]);
	TGVerticalFrame *myvframe=new TGVerticalFrame(myhframe);
	TGHorizontalFrame *powerstat=new TGHorizontalFrame(myvframe);
	
	myhframe->SetBackgroundColor(PwgCol[pwg]);
	myvframe->SetBackgroundColor(PwgCol[pwg]);
	powerstat->SetBackgroundColor(PwgCol[pwg]);
	

	//TGLabel *fLabelPWG = new TGLabel(myvframe,(lad==MAXLAD)?"---":Form("PWG %d, %d",pwg,pwgpos));
	//fLabelPWG->SetBackgroundColor(PwgCol[pwg]);
	//fLabelPWG->SetTextJustify(36);

	if (lad!=MAXLAD) {
	  TGLabel *pwgposlabel=new TGLabel(powerstat,Form(" %d ",pwgpos));
	  pwgposlabel->SetBackgroundColor(PwgCol[pwg]);
	  fSlabel[pwg][pwgpos]=new TGLabel(powerstat,"S");
	  fSlabel[pwg][pwgpos]->SetBackgroundColor(0xffff00);
	  fKlabel[pwg][pwgpos]=new TGLabel(powerstat,"K");
	  fKlabel[pwg][pwgpos]->SetBackgroundColor(0xffff00);
	  fTlabel[pwg][pwgpos]=new TGLabel(powerstat,"T");
	  fTlabel[pwg][pwgpos]->SetBackgroundColor(0xffff00);

	  powerstat->AddFrame(pwgposlabel,new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	  powerstat->AddFrame(fSlabel[pwg][pwgpos],new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	  powerstat->AddFrame(fKlabel[pwg][pwgpos],new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	  powerstat->AddFrame(fTlabel[pwg][pwgpos],new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	  
	} else {
	  fSlabel[pwg][pwgpos]=0;
	  fKlabel[pwg][pwgpos]=0;
	  fTlabel[pwg][pwgpos]=0;
	  TGLabel *emptylab=new TGLabel(powerstat,"---");
	  emptylab->SetBackgroundColor(PwgCol[pwg]);
	  powerstat->AddFrame(emptylab,new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0)); 
	}


	myvframe->AddFrame(powerstat, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	//fLabel798->MoveResize(16,32,88,24);
	
	//	 TGLabel *fLabelTDR = new TGLabel(myvframe,(lad==132)?"---":Form("TDR %d",ladder[lad].tdr));
	//fLabelTDR->SetBackgroundColor(PwgCol[pwg]);
	//fLabelTDR->SetTextJustify(36);
	//myvframe->AddFrame(fLabelTDR, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	
	TGHorizontalFrame *my2ndhframe=new TGHorizontalFrame(myvframe);

	if (lad!=MAXLAD) {
	  fLabelTripS[pwg][pwgpos] = new TGLabel(my2ndhframe,"S");
	  fLabelTripS[pwg][pwgpos]->SetTextJustify(36);
	  fLabelTripS[pwg][pwgpos]->SetForegroundColor(PwgCol[pwg]);
	  fLabelTripS[pwg][pwgpos]->SetBackgroundColor(PwgCol[pwg]);
	  
	  fLabelTripScnt[pwg][pwgpos] = new TGLabel(my2ndhframe,"D");
	  fLabelTripScnt[pwg][pwgpos]->SetTextJustify(36);
	  fLabelTripScnt[pwg][pwgpos]->SetBackgroundColor(PwgCol[pwg]);
	  fLabelTripScnt[pwg][pwgpos]->SetForegroundColor(PwgCol[pwg]);
	  
	  fLabelTripK[pwg][pwgpos] = new TGLabel(my2ndhframe,"K");
	  fLabelTripK[pwg][pwgpos]->SetTextJustify(36);
	  fLabelTripK[pwg][pwgpos]->SetBackgroundColor(PwgCol[pwg]);
	  fLabelTripK[pwg][pwgpos]->SetForegroundColor(PwgCol[pwg]);
	  
	  fLabelTripKcnt[pwg][pwgpos] = new TGLabel(my2ndhframe,"D");
	  fLabelTripKcnt[pwg][pwgpos]->SetTextJustify(36);
	  fLabelTripKcnt[pwg][pwgpos]->SetBackgroundColor(PwgCol[pwg]);
	  fLabelTripKcnt[pwg][pwgpos]->SetForegroundColor(PwgCol[pwg]);
	  
	  fLabelTripT[pwg][pwgpos] = new TGLabel(my2ndhframe,"T");
	  fLabelTripT[pwg][pwgpos]->SetTextJustify(36);
	  fLabelTripT[pwg][pwgpos]->SetBackgroundColor(PwgCol[pwg]);
	  fLabelTripT[pwg][pwgpos]->SetForegroundColor(PwgCol[pwg]);
	  
	  fLabelTripTcnt[pwg][pwgpos] = new TGLabel(my2ndhframe,"D");
	  fLabelTripTcnt[pwg][pwgpos]->SetTextJustify(36);
	  fLabelTripTcnt[pwg][pwgpos]->SetBackgroundColor(PwgCol[pwg]);
	  fLabelTripTcnt[pwg][pwgpos]->SetForegroundColor(PwgCol[pwg]);
	  
	  my2ndhframe->AddFrame(fLabelTripS[pwg][pwgpos],new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	  my2ndhframe->AddFrame(fLabelTripScnt[pwg][pwgpos],new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	  my2ndhframe->AddFrame(fLabelTripK[pwg][pwgpos],new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	  my2ndhframe->AddFrame(fLabelTripKcnt[pwg][pwgpos],new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	  my2ndhframe->AddFrame(fLabelTripT[pwg][pwgpos],new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	  my2ndhframe->AddFrame(fLabelTripTcnt[pwg][pwgpos],new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	} else {
	  fLabelTripS[pwg][pwgpos]=0;
	  fLabelTripScnt[pwg][pwgpos]=0;
	  fLabelTripK[pwg][pwgpos]=0;
	  fLabelTripKcnt[pwg][pwgpos]=0;
	  fLabelTripT[pwg][pwgpos]=0;
	  fLabelTripTcnt[pwg][pwgpos]=0;

	  TGLabel *emptylab=new TGLabel(my2ndhframe,"---");
	  emptylab->SetBackgroundColor(PwgCol[pwg]);
	  //emptylab->SetForegroundColor(PwgCol[pwg]);
	  my2ndhframe->AddFrame(emptylab,new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0)); 
	}	

	myvframe->AddFrame(my2ndhframe, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	myhframe->AddFrame(myvframe, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

	if (lad!=MAXLAD) {
	  TRootEmbeddedCanvas *fRootEmbeddedCanvas = new TRootEmbeddedCanvas(Form("ce%03d",lad),myhframe,50,40);
	  Int_t wfRootEmbeddedCanvas = fRootEmbeddedCanvas->GetCanvasWindowId();
	  LadCanZoom[pwg][pwgpos]=0;
	  LadCan[pwg][pwgpos] = new TCanvas(Form("c%03d",lad), 10, 10, wfRootEmbeddedCanvas);
	  LadCan[pwg][pwgpos]->Connect("Selected(TVirtualPad*,TObject*,Int_t)","SlowGui",this,"ProcessCanvas(TVirtualPad*,TObject*,Int_t)");
	  //	   gPad->cd();
	  //	   gPad->DrawFrame(0,0,10,10);
	  //	   fun->Draw();

	  LadCan[pwg][pwgpos]->cd();
	  TH1F *frame=LadCan[pwg][pwgpos]->DrawFrame(0,0,10,10);
	  frame->SetXTitle("Time");
	  frame->SetYTitle("Leakage current (#muA)");
	  frame->GetXaxis()->SetTimeDisplay(1);
	  frame->GetXaxis()->SetTimeFormat("%H:%M");
	  frame->SetTitle(ladder->GetName());
	  fRootEmbeddedCanvas->AdoptCanvas(LadCan[pwg][pwgpos]);
	  myhframe->AddFrame(fRootEmbeddedCanvas, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));

	  InitGraph(pwg,pwgpos);
	  //LadGraph[pwg][pwgpos]->Draw("l");
	  LadCan[pwg][pwgpos]->Update();
	}


	fGroupFrame[i]->AddFrame(myhframe, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	 
	//fGroupFrame[i]->SetLayoutManager(new TGVerticalLayout(fGroupFrame[i]));
	 
	fHProgressLeak[pwg][pwgpos]=new TGHProgressBar(fGroupFrame[i], 120, TGProgressBar::kProgressBarStandardWidth, 0xc0c0c0, TGFrame::GetDefaultSelectedBackground(), uGC2->GetGC(), progressfont->GetFontStruct(),kDoubleBorder|kSunkenFrame);
	 
	fHProgressLeak[pwg][pwgpos]->SetBarType(TGProgressBar::kStandard);
	fHProgressLeak[pwg][pwgpos]->SetFillType(TGProgressBar::kSolidFill);
	fHProgressLeak[pwg][pwgpos]->ShowPosition(kTRUE,kFALSE,"%.2f");
	fHProgressLeak[pwg][pwgpos]->SetMin(0);
	fHProgressLeak[pwg][pwgpos]->SetMax(20.0);
	fHProgressLeak[pwg][pwgpos]->SetPosition((lad==MAXLAD)?0.0:10.0);
	fHProgressLeak[pwg][pwgpos]->SetBarColor("red");
	fHProgressLeak[pwg][pwgpos]->SetForegroundColor(0xffff00);
	fHProgressLeak[pwg][pwgpos]->SetBackgroundColor((lad==MAXLAD)?0xc0c0c0:0x010b6c);
	fHProgressLeak[pwg][pwgpos]->Format((lad==MAXLAD)?"":"%2.2f uA");
	fGroupFrame[i]->AddFrame(fHProgressLeak[pwg][pwgpos], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	 
	hframe->AddFrame(fGroupFrame[i], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	//fGroupFrame[i]->MoveResize(312,24,120,184);
	 
	pwgpos++;
      }
       
      vframe->AddFrame(hframe, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
     
    }
     
    //TGStatusBar *fStatusPwg=new TGStatusBar(vframe,4,2,kHorizontalFrame | kSunkenFrame,PwgCol[pwg]);
    fStatusPwg[pwg]=new TGStatusBar(vframe);
    Int_t partsusBar[] = {50,50};
    fStatusPwg[pwg]->SetParts(partsusBar,2);
    fStatusPwg[pwg]->SetText(Form("Pwg group %d",pwg),0);
    fStatusPwg[pwg]->SetText("Bias voltage: 00.0 V",1);
    fStatusPwg[pwg]->SetBackgroundColor(PwgCol[pwg]);
    (fStatusPwg[pwg]->GetBarPart(0))->SetBackgroundColor(PwgCol[pwg]);
    (fStatusPwg[pwg]->GetBarPart(1))->SetBackgroundColor(PwgCol[pwg]);

    vframe->AddFrame(fStatusPwg[pwg], new TGLayoutHints(kLHintsBottom | kLHintsExpandX));
     
    horiframe[(pwg>1)]->AddFrame(vframe, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  }
   
  vertframe->AddFrame(horiframe[0], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  vertframe->AddFrame(horiframe[1], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   

  fTrackerMonitorMainFrame->AddFrame(vertframe, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  fTrackerMonitorMainFrame->MapSubwindows();

  fTrackerMonitorMainFrame->Resize(fTrackerMonitorMainFrame->GetDefaultSize());
  fTrackerMonitorMainFrame->MapWindow();
  //fMainFrame971->Resize(490,372);

  //TTimer *TimerCheck=new TTimer();
  //TimerCheck->Connect("Timeout()","SlowGui",this,"GenerateCurrent()");
  //TimerCheck->Start(1000,kFALSE);
 
}  


void SlowGui::UpdateTKTrips(int pwg, int pwgpos, int typ, int cnt) {

  TGLabel *lab1=0, *lab2=0;

  if (typ==0) {
    lab1=fLabelTripS[pwg][pwgpos];
    lab2=fLabelTripScnt[pwg][pwgpos];
  } else if (typ==1) {
    lab1=fLabelTripK[pwg][pwgpos];
    lab2=fLabelTripKcnt[pwg][pwgpos];
  } else if (typ==2) {
    lab1=fLabelTripT[pwg][pwgpos];
    lab2=fLabelTripTcnt[pwg][pwgpos];
  }

  if ((lab1==0) || (lab2==0)) return;

  lab1->SetBackgroundColor((cnt)?0:PwgCol[pwg]);
  lab1->SetForegroundColor((cnt)?0xffffff:PwgCol[pwg]);
  lab2->SetBackgroundColor((cnt)?0:PwgCol[pwg]);
  lab2->SetForegroundColor((cnt)?0xff0000:PwgCol[pwg]);
  lab2->SetText(Form("%X",cnt));

  gClient->NeedRedraw(lab1);
  gClient->NeedRedraw(lab2);
}


void SlowGui::UpdateTKTPSFE(int pwg, int pwgpos, int typ, int on) {
  TGLabel *lab=0;

  if (typ==0) {
    lab=fSlabel[pwg][pwgpos];
  } else if (typ==1) {
    lab=fKlabel[pwg][pwgpos];
  } else if (typ==2) {
    lab=fTlabel[pwg][pwgpos];
  }
  
  if (lab==0) return;
  
  lab->SetBackgroundColor((on)?0x00ff00:0xff0000);
  lab->SetForegroundColor((on)?0:0xffff00);
  
  gClient->NeedRedraw(lab);
}


void SlowGui::DisplayTKVoltages(int pwg, double voltage) {

  fStatusPwg[pwg]->SetText(Form("Bias voltage: %5.2lf V",voltage),1);

  gClient->NeedRedraw(fStatusPwg[pwg]);
}


// void SlowGui::ProcessCanvas(TVirtualPad *pad,TObject *obj,Int_t event) {

//   //  printf("hello event=%d\n",event);
//   char canname[200];
//   sprintf(canname,"%s", pad->GetName());

//   int lad=atoi(&canname[1]);

//   Ladder *ladder=InnerTracker.GetLadder(lad);

//   TCanvas *c=new TCanvas(Form("ccc%d",zoomcounter),Form("ccc%d",zoomcounter),600,400);
//   zoomcounter++;

//   pad->GetListOfPrimitives()->ls();

//   //  TH1F *frame=(TH1F*)pad->FindObject("hframe");
//   TH2F *htemp=(TH2F*)pad->FindObject("htemp");
//   if (htemp) printf("process canvas: htemp found\n");
//   else return;
//   //TTree *gr=(TTree*)pad->FindObject(Form("graph_%02d_%02d",ladder->GetPwg(),ladder->GetPwgpos()));
//   TGraph *gr=(TGraph*)pad->FindObject("Graph");
//   if (gr) printf("process canvas: Graph found\n");
//   else return;
//   TLatex *arrow=(TLatex*)pad->FindObject("lc80ref");
//   //frame->SetBit(kCanDelete,kFALSE);
//   c->cd();
//   htemp->Draw();
//   gr->Draw("l");  
//   arrow->Draw();
//   c->Update();

// }



void SlowGui::ProcessCanvas(TVirtualPad *pad,TObject *obj,Int_t event) {

  char canname[200];
  sprintf(canname,"%s", pad->GetName());

  int lad=atoi(&canname[1]);

  TkLadder *ladder=InnerTracker.GetLadder(lad);

  int pwg=ladder->GetPwg();
  int pwgpos=ladder->GetPwgpos();

  TCanvas *c=(TCanvas*)(gROOT->FindObject(Form("ccc%d",lad)));  
  if (c==0) {
    //gROOT->SetStyle("StyleWhite");
    LadCanZoom[pwg][pwgpos]=new TCanvas(Form("ccc%d",lad),Form("ccc%d",lad),600,400);
    LadCanZoom[pwg][pwgpos]->cd();
    LadGraph[pwg][pwgpos]->Draw("current:time","","l");
    LadCanZoom[pwg][pwgpos]->Modified();
    LadCanZoom[pwg][pwgpos]->Update();
    TH2F *htemp=(TH2F*)LadCanZoom[pwg][pwgpos]->FindObject("htemp");
    htemp->GetXaxis()->SetTimeDisplay(1);
    htemp->GetXaxis()->SetTimeFormat("%H:%M");
    htemp->SetTitle(ladder->GetName());
    //position.Draw();
    LadCanZoom[pwg][pwgpos]->Modified();
    LadCanZoom[pwg][pwgpos]->Update();
    //gROOT->SetStyle("StyleBlack");
  }

  

}


void SlowGui::UpdateZoomCanvases() {
  //printf("zoomcounter=%d\n",zoomcounter);
  for (int i=zoommin; i<zoomcounter; i++) {
    TCanvas *c=(TCanvas*)(gROOT->FindObject(Form("ccc%d",i)));
    //cout << c << endl;
    if (c) {
      c->Modified();
      c->Update();
    } else if ((i==zoommin) && (i<zoomcounter-1)) zoommin++;
  }

}



void SlowGui::InitGraph(int pwg, int pwgpos) {
  
  //LadGraph[pwg][pwgpos]=new TGraph();
  //  RootFile->cd();
  LadGraph[pwg][pwgpos]=new TTree(Form("graph_%02d_%02d",pwg,pwgpos),Form("graph_%02d_%02d",pwg,pwgpos));
  LadGraph[pwg][pwgpos]->Branch("time",&Tim[pwg][pwgpos],"time/I");
  LadGraph[pwg][pwgpos]->Branch("current",&Current[pwg][pwgpos],"current/D");
  LadGraph[pwg][pwgpos]->SetLineColor(kRed);
  LadGraph[pwg][pwgpos]->SetDirectory(RootFile);
  //LadGraph[pwg][pwgpos]->SetName(Form("graph_%02d_%02d",pwg,pwgpos));
  //LadGraph[pwg][pwgpos]->Write();
}



void SlowGui::FillTKGraph(int pwg, int pwgpos, double val) {
  
  time_t tim;
  time(&tim);

  Double_t val0=0, dum=0;

  Tim[pwg][pwgpos]=tim;

  if (LadGraph[pwg][pwgpos]==0) return;

  Int_t npoints=LadGraph[pwg][pwgpos]->GetEntries();

  //  if (npoints==0) {
  TkLadder *ladder=InnerTracker.FindLadder(XTANT,pwg,pwgpos);
  val0=ladder->GetLc80();
    //  } 
  //else {
  //  LadGraph[pwg][pwgpos]->GetPoint(npoints-1, dum,val);
    //val+=0.1*(gRandom->Rndm()-0.49);
  //}

  //  LadGraph[pwg][pwgpos]->Set(1+npoints);
  //LadGraph[pwg][pwgpos]->SetPoint(npoints,tim, val);

  Current[pwg][pwgpos]=val;  

  LadGraph[pwg][pwgpos]->Fill();

  UpdateCurrent2(pwg,pwgpos,val);

  if (npoints==0) return;

  if (npoints>0) {
    //printf("npoints==1\n");
    LadCan[pwg][pwgpos]->cd();
    LadGraph[pwg][pwgpos]->Draw("current:time","","l");
    //TLatex *position=new TLatex(tim,val0,"#leftarrow");
    TLatex position(tim,val0,"#leftarrow");
    position.SetTextSize(0.2);
    position.SetName("lc80ref");
    position.SetTextAlign(12);
    position.SetTextColor(kGreen); 
    position.Draw();
    LadCan[pwg][pwgpos]->Update();


    TCanvas *c=(TCanvas*)(gROOT->FindObject(Form("ccc%d",InnerTracker.FindLadderIndex(XTANT,pwg,pwgpos))));  
    if (c) {
      //gROOT->SetStyle("StyleWhite");
      LadCanZoom[pwg][pwgpos]->cd();
      LadGraph[pwg][pwgpos]->Draw("current:time","","l");
      LadCanZoom[pwg][pwgpos]->Modified();
      LadCanZoom[pwg][pwgpos]->Update();
      TH2F *htemp=(TH2F*)LadCanZoom[pwg][pwgpos]->FindObject("htemp");
      htemp->GetXaxis()->SetTimeDisplay(1);
      htemp->GetXaxis()->SetTimeFormat("%H:%M");
      htemp->SetTitle(ladder->GetName());
      position.Draw();
      LadCanZoom[pwg][pwgpos]->Modified();
      LadCanZoom[pwg][pwgpos]->Update();
      //gROOT->SetStyle("StyleBlack");
    }

  } else 
    RefreshCanvas(pwg,pwgpos);

  RootFile->Write(0,TObject::kOverwrite);

}


void SlowGui::UpdateCurrent2(int pwg, int pwgpos, double current) {

  //current+=2; // test
  int max=current/20;

  TkLadder *ladder=InnerTracker.FindLadder(XTANT,pwg,pwgpos);
  double lc80=ladder->GetLc80();

  fHProgressLeak[pwg][pwgpos]->SetBarColor((current>1.5*lc80 || current>5+lc80)?0xff0000:0x59d454);
  fHProgressLeak[pwg][pwgpos]->SetMin(max*20.0);
  fHProgressLeak[pwg][pwgpos]->SetMax((max+1)*20.0);
  fHProgressLeak[pwg][pwgpos]->SetPosition(current);

  gClient->NeedRedraw(fHProgressLeak[pwg][pwgpos]);

}


void SlowGui::RefreshCanvas(int pwg, int pwgpos) {
  // checks the limits of the graphs inside the canvas, and update
  // the frame dimentions

  printf("welcome to refreshcanvas...\n");

  Double_t minx=0, miny=0, maxx=0, maxy=0;
  Double_t x0,y0,x1,y1, width, height;

  TCanvas *aCanvas=LadCan[pwg][pwgpos]; 

  TH1F *frame=0;
  frame=(TH1F*)aCanvas->FindObject("hframe");
  if (!frame) return;

  printf("frame found\n");

  TLatex *arrow=(TLatex*)aCanvas->FindObject("lc80ref");
  if (!arrow) return;

  printf("arrow found\n");

  Int_t cnt=0;
  TTree *gr=LadGraph[pwg][pwgpos];
  if (!gr) return; // non-present graph
  if (gr->GetEntries()<2) return; // we need at least 2 points to use the next lines
  maxx=gr->GetMaximum("time");
  minx=gr->GetMinimum("time");
  maxy=gr->GetMaximum("current");
  miny=gr->GetMinimum("current");
  if (!cnt) { x0=minx; x1=maxx; y0=miny; y1=maxy; cnt=1;}
  else {
    x0=TMath::Min(x0,minx);
    x1=TMath::Max(x1,maxx);
    y0=TMath::Min(y0,miny);
    y1=TMath::Max(y1,maxy);
    cnt++;
  }
  
  if (!cnt) return; // no filled graphs found
  //  printf("I am here\n");

  arrow->SetX(x1);

  width=0.1*(x1-x0); height=0.1*(y1-y0);
  x0-=width; x1+=2.5*width; y0-=height; y1+=height; // for x1, we give more space for the legend

  frame->SetBins(frame->GetNbinsX(),x0,x1);
  frame->SetMinimum(y0);
  //  frame->SetMinimum(0);
  frame->SetMaximum(y1);

  aCanvas->Modified();
  gr->Draw("current:time","","l same");
  aCanvas->Update();

  
}


// void SlowGui::GenerateCurrent() {
  
//   for(int pwg=0; pwg<4; pwg++) {
//     for (int pwgpos=0; pwgpos<6; pwgpos++) {
//       FillGraph(pwg,pwgpos);
//     }
//   }

//   UpdateZoomCanvases();


// }


// void SlowGui::readlist() {


//   FILE *f=fopen("ladlist.txt","r");

//   if (f==0) return;

//   char *dum[200];

//   for (int i=0; i<10; i++) fscanf(f,"%s",dum); // skipping the first line with column names
  
//   int lad=0;
//   do {
//     char pos[10], name[20];
//     int x,layer,slot,xtant,pwg,pwgpos,tdr;
//     double lc80,lc90;
//     fscanf(f,"%s %d %d %s %d %d %d %d %lf %lf",&pos,&layer,&slot,&name,&xtant,&pwg,&pwgpos,&tdr,&lc80,&lc90);

//     if (feof(f)) break;

//     if ((strstr(name,"one"))) continue; // we skip the "None" line

//     printf("%s\n", name);
//     char *temp=strrchr(name,'\"');
//     *temp='\0';



//     ladder[lad].x=(strcmp("\"M\"",pos))?1:-1;
//     ladder[lad].layer=layer;
//     ladder[lad].slot=slot;
//     sprintf(ladder[lad].name,"%s",&name[1]);
//     ladder[lad].xtant=xtant;
//     ladder[lad].pwg=pwg;
//     ladder[lad].pwgpos=pwgpos;
//     ladder[lad].tdr=tdr;
//     ladder[lad].lc80=lc80;
//     ladder[lad].lc90=lc90;


//     lad++;

//   } while(1);

//   printf("%d ladders found\n", lad);

//   for (int i=0; i<lad; i++) {

//     printf("%d %d %d %s %d %d %d %d %lf %lf\n", ladder[i].x, ladder[i].layer, ladder[i].slot, ladder[i].name, ladder[i].xtant, ladder[i].pwg, ladder[i].pwgpos, ladder[i].tdr, ladder[i].lc80, ladder[i].lc90);


//   }

//   ladmax=lad;

//   ladder[132].x=0;
//   ladder[132].layer=0;
//   ladder[132].slot=0;
//   ladder[132].xtant=0;
//   ladder[132].pwg=0;
//   ladder[132].pwgpos=0;
//   ladder[132].lc80=0.0;
//   ladder[132].lc90=0.0;
//   sprintf(ladder[132].name,"Empty");

// }



// int SlowGui::getladder(int x, int xtant, int pwg, int pwgpos) {
//   for (int lad=0; lad<ladmax; lad++) {
//     if ((ladder[lad].x==x) && (ladder[lad].xtant==xtant) && (ladder[lad].pwg==pwg) && (ladder[lad].pwgpos==pwgpos)) return lad;
//   }

//   return 132;
// }

