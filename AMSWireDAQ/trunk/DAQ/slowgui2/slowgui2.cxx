#include <iostream>
#include <stdio.h>
#include "SlowControl.h"
#include "amswcomtest.h"
#include "HELPgui2.h"
#include "slowgui2.h"


using namespace std;


SlowGui::SlowGui(const TGWindow *p, UInt_t w, UInt_t h, int tcrate, unsigned int jinfaddra, unsigned int jinfaddrb, int porta, int portb ) {

  TCrate=tcrate;

  InitStyle();
  OpenDataFile(tcrate);
  //RootFile=new TFile("out.root","recreate");
  RootFile->cd();

  TrackerMonitor();

  CanvasCount=0;

  fMain=new TGMainFrame(p,w,h);
  fMain->Connect("CloseWindow()","SlowGui",this,"Exit()");
  fMain->DontCallClose();
  //  c=new TCanvas();

  fFrame0 = new TGVerticalFrame(fMain,100,100,kFixedWidth);
  fFrame1 = new TGHorizontalFrame(fFrame0, 100, 100, kFixedWidth);
  fTab = new TGTab(fFrame1, 300, 300);
  fTab->Connect("Selected(Int_t)","SlowGui",this,"CheckMainTab(Int_t)");


  tS9011main = fTab->AddTab("S9011AT");
  tTPSFEmain = fTab->AddTab("TPSFE");
  tTBSmain = fTab->AddTab("TBS");
  tPlotTab=fTab->AddTab("Plots");
  tSettingsTab = fTab->AddTab("Settings");
 
  CreateS9011ATFrame(tS9011main);
  CreateTBSFrame(tTBSmain); 
  CreateTpsfeFrame(tTPSFEmain);
  CreatePlotFrame(tPlotTab);
  CreateSettingsFrame(tSettingsTab);

  fFrame1->AddFrame(fTab,fL5);
  fFrame0->AddFrame(fFrame1,fL51);

  fAMSWireStatus = new TGStatusBar(fFrame0);
  Int_t partsusBar[] = {10,45,45};
  fAMSWireStatus->SetParts(partsusBar,3);

  fFrame0->AddFrame(fAMSWireStatus, new TGLayoutHints(kLHintsBottom | kLHintsExpandX));

  fMain->AddFrame(fFrame0,fL5);

  //Set a name to the main frame
  fMain->SetWindowName(Form("Slow Control, tcrate %d",TCrate) );
  //Map all subwindows of main frame
  fMain->MapSubwindows();

  //Initialize the laout algorithm
  fMain->Resize(550,300);


  //Map main frame
  fMain->MapWindow();

  SetPosition(tcrate,0);

  //OpenDataFile();

  for (Int_t i=0; i<NTPSFE; i++) {
    TripFrame[i]->HideContainer();
    //cout << TripFrame[i]->EnableHide() << " " << TripFrame[i]->EnableUndock() << endl;;
  }

  for (Int_t i=0; i<NTBS; i++) {
    ADCcurrentFrame[i]->HideContainer();
  }

  ADCmode=1;

  InitGraphs();

  AMSWmode=kAMSW_JMDC;
  //  AMSWmode=kAMSW_EPP;

  Node=new AMSWcom();
  AMSWportA=porta;
  Node->SetHW(AMSWmode);
  Node->SetPort(AMSWportA);
  Node->SetTimeOut(5000);
  if (Node->Init()==ERROR) {
    printf("port A init error\n");
    return;
  }
  AMSWaddr=jinfaddra;

  NodeB=new AMSWcom();
  AMSWportB=portb;
  NodeB->SetHW(AMSWmode);
  NodeB->SetPort(AMSWportB);
  NodeB->SetTimeOut(5000);
  if (NodeB->Init()==ERROR) {
    printf("port B init error\n");
    return;
  }
  AMSWaddrB=jinfaddrb;

  CreateTbs();
  CreateTpsfe();
  CreateS9011AT();

  TimerCheck=new TTimer();
  //  TimerCheck->Connect("Timeout()","SlowGui",this,"CheckSlowControl()");
  TimerCheck->Connect("Timeout()","SlowGui",this,"RefreshAll()");

  JinftAver=0;
  JinftAsubver=0;
  JinftBver=0;
  JinftBsubver=0;

  TestAMSW();

}


void SlowGui::SetPosition(int crate, int type) {
  FILE *optf=0;
  optf=fopen(".slowgui2","rt");
  int cr,x0,y0,x1,y1;
  if (optf) {
    while(1) {
      fscanf(optf,"%d %d %d %d %d", &cr, &x0, &y0, &x1, &y1);
      if (feof(optf)) break;
      printf("%d %d %d %d %d\n",cr,x0,y0,x1,y1);
      if (cr==crate) { 
	if (type==0) fMain->Move(x0, y0);
	else fTrackerMonitorMainFrame->Move(x1,y1);
	break;
      }
    }
    fclose(optf);
  }
}

void SlowGui::UpdateAMSWireStatusBar() {

  fAMSWireStatus->SetText((AMSWmode==kAMSW_JMDC)?"JMDC":((AMSWmode==kAMSW_EPP)?"EPP":"PCI"),0);
  
  char text[200];
  sprintf(text,"Jinf A: port %d, addr. %04X, vers. %04X - %04X", AMSWportA, AMSWaddr, JinftAver, JinftAsubver);
  fAMSWireStatus->SetText(text,1);

  sprintf(text,"Jinf B: port %d, addr. %04X, vers. %04X - %04X", AMSWportB, AMSWaddrB, JinftBver, JinftBsubver);
  fAMSWireStatus->SetText(text,2);

}

void SlowGui::TestAMSW() {
  //Node->LecroyWrite(addr,0);
  //printf("testamsw: erramsw=%x %d\n", Node->GetRX_DONE(), (short)Node->GetRX_DONE());
  int test1=TestJinf(Node, AMSWaddr, JinftAver, JinftAsubver);
  int test2=TestJinf(NodeB, AMSWaddrB, JinftBver, JinftBsubver);

  Offset=0;
  int test3=(JinftAver!=JinftBver);

  UpdateAMSWireStatusBar();

  if ((test1==0) && (test2==0) && (test3==0)) {
    // || (Node->GetHW()==kAMSW_EPP && Node->GetRX_DONE()==0x4180 ) || (Node->GetHW()==kAMSW_PCI && Node->GetRX_DONE()==0) || (Node->GetHW()==kAMSW_JMDC && Node->GetRX_DONE()==0)) 
    if (JinftAver>0xA802) Offset=1;
    RefreshAll();
    EnableS9053();

    TimerCheck->Start(60000,kFALSE);

    fTab->SetTab("S9011AT");
  } else {
    int reply;
    //new TGMsgBox(gClient->GetRoot(), fMain,"Communication problem", "Communication problem",kMBIconStop, kMBOk, &reply);
    fTab->SetTab("Settings");
  }

}


SlowGui::~SlowGui() {
  //Clean up used widgets: frame, buttons, layouthints

  fMain->Cleanup();
  if (fTrackerMonitorMainFrame) fTrackerMonitorMainFrame->Cleanup();
  delete fMain;
}


void SlowGui::CheckSlowControl() {
  for (int i=0; i<NTPSFE; i++) {
  TPSFETabColor[i]=7*gRandom->Rndm();

  }
  printf("hello\n");
  UpdateTabColor("TPSFE");

}


void SlowGui::CheckTab(Int_t val, Char_t *FrameName) {
  // Ok, here we do not care yet about the selected tab,
  // but we need to take it anyway...
  //printf("check tab\n");
  UpdateTabColor(FrameName);
  
  if (!strcmp(FrameName,"TPSFE")) {
    //printf("tab TPSFE val = %d\n",val);
    GetTpsfeStatus(val);
  }
  else if(!strcmp(FrameName,"TBS")) {
    //printf("tab TBS val = %d\n",val);
    GetTbsStatus(val);
  }
}


void SlowGui::CheckMainTab(Int_t val) {
  //fMain->Resize();
//   printf("main tab: %d\n", val);
//   if (val==1) {
//     printf("tpfse tab\n");
//     printf("pad selected: %d\n", fTPSFETab->GetCurrent());
//     printf("dockable is: %d\n", TripFrame[fTPSFETab->GetCurrent()]->IsHidden());
//     fMain->Resize();
//     //    if (TripFrame[fTPSFETab->GetCurrent()]->IsHidden()==0) fMain->Resize();//  fMain->Resize(400,400);
//   }

  Int_t current=0;

  int ret=0;
  TGMsgBox *aMsgBox=0;
  int reply=0;

  if (val==0) {
    // S9011
    //printf("S9011 tab\n");
    ret=GetS9011atStatus();
  }
  else if (val==1) {
    // TPSFE
    current=fTPSFETab->GetCurrent();
    ret=GetTpsfeStatus(current);
  } 
  else if (val==2) {
    // TBS
    current=fTBSTab->GetCurrent();
    ret=GetTbsStatus(current);
  } else if (val==4) {
    UpdateSettings();
  }

  if (ret) {
    TimerCheck->Stop();
    aMsgBox=new TGMsgBox(gClient->GetRoot(), fMain,"AMSWire problem","Wrong AMSWire reply",kMBIconStop, kMBOk, &reply,kVerticalFrame,kTextLeft | kTextCenterY);
    fTab->SetTab("Settings");
  }

}


void SlowGui::RefreshAll() {
  int ret=0;

  TGMsgBox *aMsgBox=0;
  int reply=0;


  ret=GetS9011atStatus();
  //printf("s9011, ret=%d\n",ret);
  if (!ret) ret=GetTpsfeStatus(0);
  //printf("tpsfe0, ret=%d\n",ret);
  if (!ret) ret=GetTpsfeStatus(1);
  //printf("tpsfe1, ret=%d\n",ret);
  if (!ret) ret=GetTpsfeStatus(2);
  //printf("tpsfe2, ret=%d\n",ret);
  if (!ret) ret=GetTpsfeStatus(3);
  //printf("tpsfe3, ret=%d\n",ret);
  if (!ret) ret=GetTbsStatus(0);
  //printf("tbs0, ret=%d\n",ret);
  if (!ret) ret=GetTbsStatus(1);
  //printf("tbs1, ret=%d\n",ret);

  if (ret) {
    TimerCheck->Stop();
    aMsgBox=new TGMsgBox(gClient->GetRoot(), fMain,"AMSWire problem","Wrong AMSWire reply",kMBIconStop, kMBOk, &reply,kVerticalFrame,kTextLeft | kTextCenterY);
    fTab->SetTab("Settings");
  }
}




void SlowGui::Exit() {

  /* Useful constants to know for a message box...
     kMBIconStop
     kMBIconQuestion
     kMBIconExclamation
     kMBIconAsterisk
     kMBYes
     kMBNo
     kMBOk
     kMBApply
     kMBRetry
     kMBIgnore
     kMBCancel
     kMBClose
     kMBDismiss
  */
  int reply;
  new TGMsgBox(gClient->GetRoot(), fMain,"Quit ?", "You are about to leave slow control, proceed ?",kMBIconExclamation, kMBYes | kMBNo, &reply);


  if (reply==kMBYes) {
    TimerCheck->Stop();
    //CloseDataFile();
  RootFile->Write();
  printf("rootfile list:\n");
  //RootFile->ls();
  RootFile->Close();
    gApplication->Terminate(0);
  }
}


void SlowGui::SetTabColor(TGTab *aTab, Char_t *name,  Pixel_t color) {
   TGTabElement *tabel = aTab->GetTabTab(name);
   tabel->ChangeBackground(color);
}


void SlowGui::SetTabColor(TGTab *aTab, Int_t index,  Pixel_t color) {
   TGTabElement *tabel = aTab->GetTabTab(index);
   tabel->ChangeBackground(color);
   gClient->NeedRedraw(tabel);
}


void SlowGui::UpdateTabColor(const Char_t *type) {
  Int_t max=0;
  TGTab *aTab=NULL;
  Int_t *colors;
  Int_t currenttab=0;

  if (!strcmp(type,"TPSFE")) { max=NTPSFE; aTab=fTPSFETab; colors=TPSFETabColor; }
  if (!strcmp(type,"TBS")) { max=NTBS; aTab=fTBSTab; colors=TBSTabColor; }

  currenttab=aTab->GetCurrent();

  for (int i=0; i<max; i++) SetTabColor(aTab, i, PixelColor[colors[i]][i!=currenttab]);
}



void SlowGui::InitStyle() {
  gROOT->SetStyle("Plain");

  TStyle *myStyle[2], *tempo;
  myStyle[0]=new TStyle("StyleWhite", "white");
  myStyle[1]=new TStyle("StyleBlack", "black");

  tempo=gStyle;
  Int_t linecol, bkgndcol, histcol;
 
  for(Int_t style=0; style<2; style++) {

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
    myStyle[style]->SetHistFillColor((style)?bkgndcol:kYellow);
    myStyle[style]->SetOptDate(22);
    myStyle[style]->GetAttDate()->SetTextColor(linecol);
    myStyle[style]->GetAttDate()->SetTextAngle(90);
    myStyle[style]->GetAttDate()->SetTextSize(0.01);
    myStyle[style]->SetDateY(1);
    myStyle[style]->SetDateX(1);
  }

  myStyle[1]->cd();

  gROOT->ForceStyle();

  // now we setup the pixel colors
  PixelColor[kMyWhite][0]=0xffffff;
  PixelColor[kMyWhite][1]=0xdfdfdf;
  PixelColor[kMyBlack][0]=0x000000;
  PixelColor[kMyBlack][1]=0x202020;
  PixelColor[kMyRed][0]=0xff0000;
  PixelColor[kMyRed][1]=0xdf0000;
  PixelColor[kMyGreen][0]=0x00ff00;
  PixelColor[kMyGreen][1]=0x00df00;
  PixelColor[kMyBlue][0]=0x0000ff;
  PixelColor[kMyBlue][1]=0x0000df;
  PixelColor[kMyYellow][0]=0xffff00;
  PixelColor[kMyYellow][1]=0xdfdf00;
  PixelColor[kMyMagenta][0]=0xff00ff;
  PixelColor[kMyMagenta][1]=0xdf00df;
  PixelColor[kMyCyan][0]=0x00ffff;
  PixelColor[kMyCyan][1]=0x00dfdf;

  //  Yellow[0]=0xffff00; Yellow[1]=0xdfdf00;
  //  White[0]=0xffffff; White[1]=0xdfdfdf;
  //  Black[0]=0x000000; Black[1]=0x101010;
  //  Green[0]=0x00ff00; Green[1]=0x00df00;
  //  Red[0]=0xff0000; Red[1]=0xdf0000;
  //  Blue[0]=0x0000ff; Blue[1]=0x0000df;
  //  Cyan[0]=0x00ffff; Cyan[1]=0x00dfdf;
  //  Magenta[0]=0xffff00; Magenta[1]=0xdfdf00;

  // Defining some layout hints:

  // trackerdisplay colors
  gClient->GetColorByName("#8DFF85",PwgCol[0]); // green
  gClient->GetColorByName("#85BEFF",PwgCol[1]); // blue
  gClient->GetColorByName("#FFB585",PwgCol[2]); // red
  gClient->GetColorByName("#FFFA85",PwgCol[3]); // yellow


  fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
  fL31 = new TGLayoutHints(kLHintsExpandY | kLHintsLeft, 5, 5, 5, 5);
  fL32 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);
  fL33 = new TGLayoutHints(kLHintsExpandY | kLHintsExpandX, 5, 5, 5, 5);
  fL34 = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 5, 5, 5, 5);
  fL35 = new TGLayoutHints(kLHintsTop | kLHintsRight, 5, 5, 5, 5);
  fL5 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY, 2, 2, 5, 1);
  fL51 = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 2, 2, 5, 1);
  fL7 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2);
  fL71 = new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2);
  //  fL72 = new TGLayoutHints(kLHintsCenterX | kLHintsLeft, 2, 2, 2, 2);
  fL72 = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2);
  fL73 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2);
  fL74 = new TGLayoutHints(kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 2);
  fL75 = new TGLayoutHints(kLHintsCenterY, 2, 2, 2, 2);
  fL76 = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 2, 2, 2, 2);
  fL77 = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 2, 2);
  fL78 = new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 2, 2, 2);
  fL79 = new TGLayoutHints(kLHintsExpandY | kLHintsRight, 2, 2, 2, 2);

}



void SlowGui::SetLabelColor(TGLabel *label, Pixel_t fore, Pixel_t back) {
  label->SetTextColor(fore);
  label->SetBackgroundColor(back);
}



void SlowGui::ShowBrotherTrip(int group, int trip) {

  TGLabel *alabel;

  if (group==7 || group==8) { // TBS
    alabel=tTBSTripIndicator[group-7];
  } 
  else if (group>2 && group<7) { // TPSFE
    alabel=tTPSFETripIndicator[group-3];
  }
  else if (group==2) { // S9011
    alabel=tS9011ATTripIndicator;
  }
  else return;

  ShowTripIndicator(alabel,trip);
}



void SlowGui::ShowPowerOn(int group, int pwon) {
  TGLabel *alabel;

  if (group==7 || group==8) { // TBS
    alabel=tTBSPowerOnIndicator[group-7];
  } 
  else if (group>2 && group<7) { // TPSFE
    alabel=tTPSFEPowerOnIndicator[group-3];
  }
  else if (group==2) { // S9011
    alabel=tS9011ATPowerOnIndicator;
  }
  else return;

  ShowPowerOnIndicator(alabel, pwon);
}



void SlowGui::BuildActelDisplay(TGCompositeFrame *hframe, TGTextButton **button1, TGTextButton **button2, TGComboBox **combo, TGLabel **label1, TGLabel **label2, Int_t page, const Char_t *type) {

  CreateTextButton(hframe, button1, "Actel Hot");

  (*button1)->Connect("Clicked()","SlowGui",this,Form("SwitchActel2(=0,%d,\"%s\")",page,type));


  CreateTextButton(hframe, button2, "Actel Cold");
  (*button2)->Connect("Clicked()","SlowGui",this,Form("SwitchActel2(=1,%d,\"%s\")",page,type));
 
  /*
  *combo=new TGComboBox(hframe);
  (*combo)->AddEntry("Actel Hot",0);
  (*combo)->AddEntry("Actel Cold",1);
  (*combo)->Select(0);
  (*combo)->Resize(100, 20);
  (*combo)->Connect("Selected(Int_t)","SlowGui",this,Form("SelectActel(=0,%d,\"%s\")",page,type));
  */

  CreateTripIndicator(hframe,label1);
  CreatePowerOnIndicator(hframe,label2);

  // now we place all these little things

  //  hframe->AddFrame(*combo, fL3);
  hframe->AddFrame(*button1, fL3);
  hframe->AddFrame(*button2, fL3);
  hframe->AddFrame(*label1, fL34);
  hframe->AddFrame(*label2, fL34);

}


void SlowGui::BuildActelDisplay2(TGCompositeFrame *hframe, TGTextButton **button1, TGTextButton **button2, TGTextButton **button3, TGTextButton **button4, TGLabel **label1, TGLabel **label2, Int_t page, const Char_t *type) {

  CreateTextButton(hframe, button1, "Actel H0");
  (*button1)->Connect("Clicked()","SlowGui",this,Form("SwitchActel2(=0,%d,\"%s\")",page,type));

  CreateTextButton(hframe, button2, "Actel C0");
  (*button2)->Connect("Clicked()","SlowGui",this,Form("SwitchActel2(=1,%d,\"%s\")",page,type));

  CreateTextButton(hframe, button3, "Actel H1");
  (*button3)->Connect("Clicked()","SlowGui",this,Form("SwitchActel2(=2,%d,\"%s\")",page,type));

  CreateTextButton(hframe, button4, "Actel C1");
  (*button4)->Connect("Clicked()","SlowGui",this,Form("SwitchActel2(=3,%d,\"%s\")",page,type)); 

  TGPictureButton *actelswitch=CreateS9011ActelSwitchButton(hframe,"S9011AT",0);

  CreateTripIndicator(hframe,label1);
  CreatePowerOnIndicator(hframe,label2);

  // now we place all these little things

  //  hframe->AddFrame(*combo, fL3);
  hframe->AddFrame(*button1, fL3);
  hframe->AddFrame(*button2, fL3);
  hframe->AddFrame(*button3, fL3);
  hframe->AddFrame(*button4, fL3);
  hframe->AddFrame(actelswitch, fL3);
  hframe->AddFrame(*label1, fL34);
  hframe->AddFrame(*label2, fL34);

}



void SlowGui::SwitchActel2(int Actel, Int_t ID, Char_t *type) {
  //printf("switch actel %s of %s %d\n", (Actel==0)?"hot":"cold", type, ID); 


  ushort mask=0, newmask=0, state=0;
  ushort select=(1<<Actel);
  int shift=0;

  if (!strcmp(type,"TBS")) {
    cout << "TBS\n";
    shift=(12+2*ID);
    mask=(ActelMask>>shift) & 3;
    newmask=0xffff-(3<<shift);
   }
  else if (!strcmp(type,"TPSFE")) {
    cout << "TPSFE\n";
    shift=(4+2*ID);
    mask=(ActelMask>>shift) & 3;
    newmask=0xffff-(3<<shift);
  }
  else if (!strcmp(type,"S9011AT")) {
    cout << "S9011AT\n";
    shift=0;
    mask=(ActelMask & 0xf);
    newmask=0xfff0;
  }
  else return;

  state=((1<<Actel)^mask); // we invert the state of the specified actel

  // some more checks for S9011
  if (!strcmp(type,"S9011AT")) {
    if (state==5) state=(Actel==0)?1:4; // 2 lines to Actel hot, we give priority to the one clicked on gui
    if (state==0xA) state=(Actel==1)?2:8; // 2 lines to Actel cold, we give priority to the one clicked on gui
  }

  if (state==0) return; // we cannot accept "all off" state

  newmask&=ActelMask;
  newmask|=(state<<shift);

  printf("new mask will be: 0x%04x\n",newmask);

  Node->SetParameter(AMSWaddr,0x1000,0x9000,newmask);

    if (!strcmp(type,"S9011AT")) Node->SDProc(AMSWaddr,0x1110,1); // also the 9053 settings must be copied to the newly switched on ACTEL
  Node->SDProc(AMSWaddr,0x1003); // here we change the actel configuration
    if (!strcmp(type,"S9011AT")) Node->SDProc(AMSWaddr,0x1110,0); // we re-activate the 9053 protection

  //  Node->SDProc(AMSWaddr,0x1003); // here we write into all actels the configuration

  sleep(1);

  if (!strcmp(type,"TBS")) GetTbsStatus(ID);
  else if (!strcmp(type,"TPSFE")) GetTpsfeStatus(ID);
  else if (!strcmp(type,"S9011AT")) GetS9011atStatus();

  /*
  slctl->GetActel();
  ushort brother=slctl->GetControl_Brother();
  ActelState[1-Actel]=(brother)?kActelOff:kActelOn;
  // then we need to update the display...
  */

}

/*
void SlowGui::SwitchActel(sACTEL Brother, Int_t ID, Char_t *type) {
  printf("switch actel %s of %s %d\n", (Brother==kActelHot)?"hot":"cold", type, ID); 

  sActelMode *ActelState;
  //  SlowControl *slctl;
  sACTEL Actel=(Brother==kActelHot)?kActelCold:kActelHot;

  if (!strcmp(type,"TBS")) {
    cout << "TBS\n";
    ActelState=ActelTbsState[ID];
    //slctl=MyTbs[ID][Actel];
  }
  else if (!strcmp(type,"TPSFE")) {
    cout << "TPSFE\n";
    ActelState=ActelTpsfeState[ID];
    //slctl=MyTpsfe[ID][Actel];
  }
  else if (!strcmp(type,"S9011AT")) {
    cout << "S9011AT\n";
    //    ActelState=ActelS9011ATState;
    //slctl=MyS9011AT[Actel];
    // for now line 0 is selected
    //mode=ActelState[Brother];
    //newmode=(mode==kActelOn)?kActelOff:kActelOn;
    //if (newmode==kActelOff) && (ActelStat
    //int mask = 
  }
  else return;

  sActelMode mode,newmode;
  mode=ActelState[Brother];
  newmode=(mode==kActelOn)?kActelOff:kActelOn;
  //slctl->SetActelBrother(newmode);



  sleep(1);

  if (!strcmp(type,"TBS")) GetTbsStatus(ID);
  else if (!strcmp(type,"TPSFE")) GetTpsfeStatus(ID);
  else if (!strcmp(type,"S9011AT")) GetS9011atStatus();

  
  //slctl->GetActel();
  //ushort brother=slctl->GetControl_Brother();
  //ActelState[1-Actel]=(brother)?kActelOff:kActelOn;
  //// then we need to update the display...
  

}
*/

/*
void SlowGui::SelectActel(sACTEL Actel, Int_t ID, Char_t *type) {
  TGTextButton *hot,*cold;
  sACTEL *selection;

  if (!strcmp(type,"TBS")) {
    cout << "TBS " << ID << endl;
    hot=tTBSActelHotButton[ID]; cold=tTBSActelColdButton[ID]; selection=ActelTbs;
  }
  else if (!strcmp(type,"TPSFE")) {
    cout << "TPSFE " << ID << endl;
    hot=tTPSFEActelHotButton[ID]; cold=tTPSFEActelColdButton[ID]; selection=ActelTpsfe;
  }
  //else if (!strcmp(type,"S9011AT")) {
  //  cout << "S9011AT " << ID << endl;
    //hot=tS9011ATActelHotButton; cold=tS9011ATActelColdButton; selection=&ActelS9011AT;
  //}
  else return;

  selection[ID]=Actel;
  if (Actel==kActelHot) {
    hot->SetEnabled(kFALSE);
    cold->SetEnabled(kTRUE);
  } else {
    hot->SetEnabled(kTRUE);
    cold->SetEnabled(kFALSE);
  }

  if (!strcmp(type,"TBS")) {
    GetTbsStatus(ID);
  }
  else if (!strcmp(type,"TPSFE")) {
    GetTpsfeStatus(ID);
  }
  else if (!strcmp(type,"S9011AT")) {
   GetS9011atStatus();
  }


}
*/


void SlowGui::CreateOvercurrentIndicator(TGCompositeFrame *frame, TGLabel **overlabel) {
  *overlabel=new TGLabel(frame," O ");
  Pixel_t color=(*overlabel)->GetDefaultFrameBackground();
  SetLabelColor(*overlabel, color, LightColor(color));
}


void SlowGui::ShowOvercurrentIndicator(TGLabel *alabel, Int_t show) {
  Pixel_t color=alabel->GetDefaultFrameBackground();
  Pixel_t text=(show)?PixelColor[kMyYellow][0]:color;
  Pixel_t back=(show)?PixelColor[kMyRed][0]:color;
  SetLabelColor(alabel,text, back);
  gClient->NeedRedraw(alabel);
}


void SlowGui::CreateTripIndicator(TGCompositeFrame *frame, TGLabel **triplabel) {
  *triplabel=new TGLabel(frame," T ");
  Pixel_t color=(*triplabel)->GetDefaultFrameBackground();
  SetLabelColor(*triplabel, color, LightColor(color));
  //  ShowTripIndicator(*triplabel, 1);
}


void SlowGui::ShowTripIndicator(TGLabel *alabel, Int_t show) {
  Pixel_t color=alabel->GetDefaultFrameBackground();
  Pixel_t text=(show)?PixelColor[kMyRed][0]:color;
  Pixel_t back=(show)?PixelColor[kMyYellow][0]:color;
  SetLabelColor(alabel,text, back);
  gClient->NeedRedraw(alabel);
}


void SlowGui::CreatePowerOnIndicator(TGCompositeFrame *frame, TGLabel **pwonlabel) {
  *pwonlabel=new TGLabel(frame," P ");
  Pixel_t color=(*pwonlabel)->GetDefaultFrameBackground();
  SetLabelColor(*pwonlabel,color,LightColor(color));
  //ShowPowerOnIndicator(*pwonlabel, 1);
}


void SlowGui::ShowPowerOnIndicator(TGLabel *alabel, Int_t show) {
  Pixel_t color=alabel->GetDefaultFrameBackground();
  Pixel_t text=(show)?PixelColor[kMyGreen][0]:color;
  Pixel_t back=(show)?PixelColor[kMyBlue][1]:color;
  SetLabelColor(alabel,text, back);
  gClient->NeedRedraw(alabel);
}


void SlowGui::CreateTextButton(TGCompositeFrame *frame, TGTextButton **button, const Char_t *text) {
    *button=new TGTextButton(frame,text);
    (*button)->SetBackgroundColor(PixelColor[kMyYellow][0]);
}



TGPictureButton* SlowGui::CreateHelpButton(TGCompositeFrame *hmain, const Char_t *item) {

  TGPictureButton *help = new TGPictureButton(hmain,gClient->GetPicture(HELPICON));
  help->Connect("Clicked()","SlowGui",this,Form("DisplayHelp(=\"%s\")",item));
  help->SetToolTipText(Form("Some info about %s",item));
  return help;
}


TGPictureButton* SlowGui::CreateRefreshButton(TGCompositeFrame *hmain, const Char_t *item) {

  Int_t val=0;

  if (!strcmp("S9011AT",item)) val=0;
  else if (!strcmp("TPSFE",item)) val=1;
  else if (!strcmp("TBS",item)) val=2;
  else return NULL;

  TGPictureButton *refresh = new TGPictureButton(hmain,gClient->GetPicture(REFRESHICON));
  refresh->Connect("Clicked()","SlowGui",this,Form("CheckMainTab(=%d)",val));
  refresh->SetToolTipText("Refresh...");
  return refresh;
}


TGPictureButton* SlowGui::CreateInitLecroyButton(TGCompositeFrame *hmain, const Char_t *item, Int_t val2) {

  Int_t val=0;

  if (!strcmp("S9011AT",item)) val=0;
  else if (!strcmp("TPSFE",item)) val=1;
  else if (!strcmp("TBS",item)) val=2;
  else return NULL;

  TGPictureButton *initlecroy = new TGPictureButton(hmain,gClient->GetPicture(INITLECROYICON));
  initlecroy->Connect("Clicked()","SlowGui",this,Form("InitLecroy(=%d,%d)",val,val2));
  initlecroy->SetToolTipText("Init communication...");
  return initlecroy;
}


TGPictureButton* SlowGui::CreateReInitDCDCButton(TGCompositeFrame *hmain, const Char_t *item, Int_t val2) {

  Int_t val=0;

  if (!strcmp("S9011AT",item)) val=0;
  else if (!strcmp("TPSFE",item)) val=1;
  else if (!strcmp("TBS",item)) val=2;
  else return NULL;

  TGPictureButton *reinitdcdc = new TGPictureButton(hmain,gClient->GetPicture(REINITDCDC));
  reinitdcdc->Connect("Clicked()","SlowGui",this,Form("ReInitDCDC(=%d,%d)",val,val2));
  reinitdcdc->SetToolTipText("Re-init DC-DC converters to their default settings...");
  return reinitdcdc;
}


TGPictureButton* SlowGui::CreateAnalogicalSwitchoffButton(TGCompositeFrame *hmain, const Char_t *item, Int_t val2) {

  Int_t val=0;

  if (!strcmp("S9011AT",item)) val=0;
  else if (!strcmp("TPSFE",item)) val=1;
  else if (!strcmp("TBS",item)) val=2;
  else return NULL;

  TGPictureButton *off = new TGPictureButton(hmain,gClient->GetPicture(ANALOGOFF));
  off->Connect("Clicked()","SlowGui",this,Form("SwitchOffAnalogical(=%d,%d)",val,val2));
  off->SetToolTipText("Switch-off all the analogical DC-DC converters...");
  return off;
}


TGPictureButton* SlowGui::CreateS9053HotColdButton(TGCompositeFrame *hmain, const Char_t *item, Int_t val2) {

  Int_t val=0;

  if (!strcmp("S9011AT",item)) val=0;
  else if (!strcmp("TPSFE",item)) val=1;
  else if (!strcmp("TBS",item)) val=2;
  else return NULL;

  TGPictureButton *s9053hotcold = new TGPictureButton(hmain,gClient->GetPicture(S9053HTOCICON));
  s9053hotcold->Connect("Clicked()","SlowGui",this,Form("S9053HotToCold(=%d,%d)",val,val2));
  s9053hotcold->SetToolTipText("Performs the S9053 hot to cold transition...");
  return s9053hotcold;
}

TGPictureButton* SlowGui::CreateS9053HotCold2Button(TGCompositeFrame *hmain, const Char_t *item, Int_t val2) {

  Int_t val=0;

  if (!strcmp("S9011AT",item)) val=0;
  else if (!strcmp("TPSFE",item)) val=1;
  else if (!strcmp("TBS",item)) val=2;
  else return NULL;

  TGPictureButton *s9053hotcold2 = new TGPictureButton(hmain,gClient->GetPicture(S9053HTOCICON));
  s9053hotcold2->Connect("Clicked()","SlowGui",this,Form("S9053HotToCold2(=%d,%d)",val,val2));
  s9053hotcold2->SetToolTipText("Performs the S9053 hot to cold transition LR method...");
  return s9053hotcold2;
}


TGPictureButton* SlowGui::CreateS9011ActelSwitchButton(TGCompositeFrame *hmain, const Char_t *item, Int_t val2) {

  Int_t val=0;

  if (!strcmp("S9011AT",item)) val=0;
  else if (!strcmp("TPSFE",item)) val=1;
  else if (!strcmp("TBS",item)) val=2;
  else return NULL;

  TGPictureButton *button = new TGPictureButton(hmain,gClient->GetPicture(SWITCHACTEL));
  button->Connect("Clicked()","SlowGui",this,Form("S9011SwitchActel(=%d,%d)",val,val2));
  button->SetToolTipText("Performs the Actel H to C or C to H transition");
  return button;
}


int SlowGui::TestAMSWError(AMSWcom *node) {

  TGMsgBox *aMsgBox=0;
  int reply=0;

  if (!((node->GetHW()==kAMSW_EPP && node->GetRX_DONE()==0x4180 ) || (node->GetHW()==kAMSW_PCI && node->GetRX_DONE()==0) || (node->GetHW()==kAMSW_JMDC && node->GetRX_DONE()==0))) {
    //aMsgBox=new TGMsgBox(gClient->GetRoot(), fMain,"AMSWire problem",Form("Wrong reply from port %d", node->GetPort()),kMBIconStop, kMBOk, &reply,kVerticalFrame,kTextLeft | kTextCenterY);
    return 1;
  } 
}


int SlowGui::TestJinf(AMSWcom *node, unsigned int addr, ushort &ver, ushort &subver) {
  TGMsgBox *aMsgBox=0;
  int reply=0;

  ver=0;
  subver=0;

  printf("testing port %d, address 0x%08x\n",node->GetPort(), addr);
  node->GetStatus(addr);
 
  if (!((node->GetHW()==kAMSW_EPP && node->GetRX_DONE()==0x4180 ) || (node->GetHW()==kAMSW_PCI && node->GetRX_DONE()==0) || (node->GetHW()==kAMSW_JMDC && node->GetRX_DONE()==0))) {
    aMsgBox=new TGMsgBox(gClient->GetRoot(), fMain,"Jinf problem",Form("Wrong reply from address %X of port %d",addr, node->GetPort()),kMBIconStop, kMBOk, &reply,kVerticalFrame,kTextLeft | kTextCenterY);
    return 1;
  } 
  //unsigned short subdver=0xff1f & Node->GetNodeStatus().SubdVer;
  ver=node->GetNodeStatus().Ver;
  subver=0xff1f & node->GetNodeStatus().SubdVer;
  unsigned short attr=node->GetNodeStatus().Attr;
  unsigned short nodetype=((attr & 0x0f00)>>8)-1;
  unsigned short subdet=(attr & 0x00f0)>>4;

  static char nodtyp[2][20]={"CDP\0","CDDC\0"};
  static char evbtyp[8][20]={"JINJ\0","Tracker\0","TRD\0","RICH\0","ECAL\0","TOF\0","LV1\0", "ECAL trig\0"};


  //printf("%d %d\n", nodetype,subdet);
  char instrument[100];
  sprintf(instrument, "It is a %s of type %s",nodtyp[nodetype], evbtyp[subdet]);

  //printf("node type=0x%04x, subdet type=0x%04x, subdet version=0x%04x\n", nodetype, subdet, subver); 

  int status=0;  
  char message[100];
  sprintf(message,"Address %X", addr);
  if (nodetype!=1) {
    status=1;
    sprintf(message,"%s is not a Jinf\n%s",message,instrument);
  } 
  else if (subdet!=1) {
    status=1;
    sprintf(message, "%s is not a Jinf-T\n%s",message, instrument);
  }
  else if (subver<0x9c0b) {
    status=1;
    sprintf(message,"%s is a Jint-T but has a too old subd version",message);
  }

  if (status) aMsgBox=new TGMsgBox(gClient->GetRoot(), fMain,"Jinf problem",message,kMBIconStop, kMBOk, &reply,kVerticalFrame,kTextLeft | kTextCenterY);

  return status;

}


void SlowGui::InitLecroy(Int_t val1, Int_t val2) {

  // we also include the port initialization
  printf("Shutting amsw access\n");
  Node->Shut();
  NodeB->Shut();
  printf("Node A initialization...\n");
  if (Node->Init()==ERROR) { 
    printf("init error\n");
    return;
  }
  printf("Node B initialization...\n");
  if (NodeB->Init()==ERROR) { 
    printf("init error\n");
    return;
  }


  // 


  //  if (val2<0) return;
  //  if (val1<0 || val1>2) return;

  //  SlowControl *actel;

  //if (val1==0) actel=MyS9011AT[ActelS9011AT];
  //if (val1==2 && val2<NTBS) actel=MyTbs[val2][ActelTbs[val2]];

  //actel->InitLecroy();
}


void SlowGui::ReInitDCDC(Int_t val1, Int_t val2) {

  //  Node->SDProc(AMSWaddr, 0x1001); // reset of original parameters, and activation of them, included s9053
  //  Node->SDProc(AMSWaddr, 0x1201); // activates actel brother, copies settings to him too
  Node->SDProc(AMSWaddr, 0x1004); // reset of parameters, without applying them
  Node->SDProc(AMSWaddr, 0x1003); // like this, the s9053 settings are not touched
  
  sleep(2);

  CheckMainTab(val1);
}


void SlowGui::SwitchOffAnalogical(Int_t val1, Int_t val2) {

  Node->SDProc(AMSWaddr, 0x1100); // analogical switchoff

  sleep(2);

  CheckMainTab(val1);
}


void SlowGui::S9011SwitchActel(Int_t val1, Int_t val2) {

  Node->SDProc(AMSWaddr, 0x1201); // switches on actel brother, copies settings into both actels
  Node->SDProc(AMSWaddr, 0x1202); // switches off original actel
  
  //  sleep(2);

  CheckMainTab(val1);
}


void SlowGui::S9053HotToCold(Int_t val1, Int_t val2) {

  printf("S9053 Hot to cold conversion\n");

  printf("A: Sets the default operation settings\n");
  Node->SDProc(AMSWaddr,0x1003);
  PrintRXDONE(Node); 

  printf("A: Hot to Cold transition of S9053_1\n");
  Node->SDProc(AMSWaddr,0x1200,1);
  PrintRXDONE(Node); 

  printf("A: Activates Actel Brother and applies the actual settings to both Actels.\n");
  Node->SDProc(AMSWaddr,0x1201);
  PrintRXDONE(Node); 

  printf("A: Switches off the Primary Actel\n");
  Node->SDProc(AMSWaddr,0x1202);
  PrintRXDONE(Node); 

   printf("B: Jinf B gets the actual settings of the power supplies\n");
  NodeB->SDprocRead(AMSWaddrB,0x1001);
  PrintRXDONE(NodeB); 
  PrintEvent(NodeB);

  printf("Jinf B takes as default the actual settings\n");
  NodeB->SDProc(AMSWaddrB,0x1005);
  PrintRXDONE(NodeB); 

  printf("B: Hot to Cold transition of S9053_0\n");
  NodeB->SDProc(AMSWaddrB,0x1200,0);
  PrintRXDONE(NodeB); 

  printf("A: Jinf A gets the actual settings of the power supplies\n");
  Node->SDprocRead(AMSWaddr,0x1001);
  PrintRXDONE(Node); 
  PrintEvent(Node);

  printf("A: Jinf A takes as default the actual settings\n");
  Node->SDProc(AMSWaddr,0x1005);
  PrintRXDONE(Node); 

  printf("A: Activates Actel Brother and applies the actual settings to both Actels\n");
  Node->SDProc(AMSWaddr,0x1201);
  PrintRXDONE(Node); 

  printf("A: Switches off Actel brother\n");
  Node->SDProc(AMSWaddr,0x1202);
  PrintRXDONE(Node); 

  printf("A: Jinf A takes as default the actual settings\n");
  Node->SDProc(AMSWaddr,0x1005);
  PrintRXDONE(Node); 

  CheckMainTab(val1);
}


void SlowGui::PrintLecroyError(unsigned int err) {

  err>>=16;
  if (!err) return;

  int retry=err&0xff;
  int init=err>>8;
  printf("LeCroy error: retry=%02d, init=%02d\n",retry,init);

}


void SlowGui::S9053HotToCold2(Int_t val1, Int_t val2) {

  unsigned int err=0;

  printf("S9053 Hot to cold conversion (direct LeCroy commands)\n");

  printf("\n0. Actel Hot, JinfA: bus init\n");
  Node->LecroyWrite(AMSWaddr, 0x40);
  PrintRXDONE(Node); 

  printf("\n1. Actel Hot, JinfA: S9053_0 A on, B off; S9053_1 all off\n");
  err=Node->LecroyRead(AMSWaddr, 0x40, 0xE18A, 0x0065);
  PrintRXDONE(Node); 
  PrintLecroyError(err);

  sleep(1);

  printf("\n2. Actel Hot, JinfA: S9053_0 A on, B off; S9053_1 A off, B on\n");
  err=Node->LecroyRead(AMSWaddr, 0x40, 0xE18A, 0x00A5);
  PrintRXDONE(Node); 
  PrintLecroyError(err);

  sleep(1); // Actel cold needs time to be ready
  
  printf("\n3. Actel Cold, JinfA: bus init\n");
  Node->LecroyWrite(AMSWaddr, 0x50);
  PrintRXDONE(Node); 

  printf("\n4. Actel Cold, JinfA: S9053_0 A on, B off; S9053_1 A off, B on\n");
  err=Node->LecroyRead(AMSWaddr, 0x50, 0xE18A, 0x00A5);
  PrintRXDONE(Node); 
  PrintLecroyError(err);

  printf("\n5. Actel Cold, JinfB: S9053_0 all off; S9053_1 A off, B on\n");
  err=NodeB->LecroyRead(AMSWaddrB, 0x50, 0xE18A, 0x00A6);
  PrintRXDONE(NodeB); 
  PrintLecroyError(err);

  sleep(1);

  printf("\n6. Actel Cold, JinfB: S9053_0 A off, B on; S9053_1 A off, B on\n");
  err=NodeB->LecroyRead(AMSWaddrB, 0x50, 0xE18A, 0x00AA);
  PrintRXDONE(NodeB);
  PrintLecroyError(err);

  sleep(1); // actel hot needs time to be ready

  printf("\n7. Actel Hot, JinfB: bus init\n");
  NodeB->LecroyWrite(AMSWaddrB, 0x40);
  PrintRXDONE(Node); 

  printf("\n8. Actel Hot, JinfB: S9053_0 A off, B on; S9053_1 A off, B on\n");
  err=NodeB->LecroyRead(AMSWaddrB, 0x40, 0xE18A, 0x00AA);
  PrintRXDONE(NodeB);
  PrintLecroyError(err);

  sleep(2);

  CheckMainTab(val1);
}




void SlowGui::PrintRXDONE(AMSWcom *node) {
  if (node->GetHW()==kAMSW_EPP) printf("rxdone = %04x\n",node->GetRX_DONE());
  if (node->GetHW()==kAMSW_PCI) printf("rxdone = %d\n",(short) node->GetRX_DONE());
  if (node->GetHW()==kAMSW_JMDC) printf("rxdone = %d\n",(short) node->GetRX_DONE());
}


void SlowGui::PrintEvent(AMSWcom *node, int length) {
  if (length==-1) length=node->GetEventSize();
  for (int i=0; i<length; i++) {
    if (i%16==0) printf("%5d: ",i);
    printf("%04x ",node->Event[i]);
    if ((i+1)%16==0) printf("\n");
  }
  printf("\n");
  printf("Event size = %d\n", node->GetEventSize());
}


void SlowGui::DisplayHelp(Char_t *help) {

  Char_t filename[200], title[50];

  if (!strcmp(help,"TBS")) {
    sprintf(filename,"slowgui/tbshelp.txt");
    sprintf(title,"TBS help");
  }
  else if (!strcmp(help,"TPSFE")) {
    sprintf(filename,"slowgui/tpsfehelp.txt");
    sprintf(title,"TPSFE help");
  }
  else if (!strcmp(help,"S9011AT")) {
    sprintf(filename,"slowgui/s9011athelp.txt");
    sprintf(title,"S9011AT help");
  }
  else return;

  Editor *ed = new Editor(gClient->GetRoot(), 600, 400);
  ed->SetTitle(title);
  ed->LoadFile(filename);
  ed->Popup();
}


Pixel_t SlowGui::LightColor(Pixel_t color) {

  Int_t r,g,b,h,l,s;
  TColor::Pixel2RGB(color, r,g,b);
  TColor::RGB2HLS(r,g,b,h,l,s);
  if (l-10<245) l+=10;
  else l-=10;
  TColor::HLS2RGB(h,l,s,r,g,b);

  return TColor::RGB2Pixel(r,g,b);
}


void SlowGui::SetButtonColor(TGTextButton *button, Pixel_t fore, Pixel_t back) {

  button->SetTextColor(fore, kTRUE);
  button->SetBackgroundColor(back);

}


// int main(int argv, char** argc) {

//   TApplication *theApp;
//   theApp=new TApplication("App", &argv, argc);


//   new SlowGui(gClient->GetRoot(),400,400);
//   theApp->Run();
//   return 0;
// }



void SlowGui::OpenDataFile(int crate) {
  time_t tim;
  time(&tim);
  tm *ltim=localtime(&tim);
  Char_t time[50];
  //  strftime(time,50,"%d%m%Y%H%M%S",ltim);
  strftime(time,50,"%Y%m%d%H%M%S",ltim);
  //DataFile=fopen(Form("data_%s.txt",time),"wt");
  RootFile=new TFile(Form("slowguidata/data_crate%d_%s.root",crate,time),"recreate"); 
}


ushort SlowGui::FindParameter(int param) {
  int cnt=Offset+2;

  int len=Node->GetEventSize();
  len-=3;
  len/=2;

  ushort state=0;

  //printf("looking for parameter %04x ... ",param);

  for(int i=0; i<len; i++) {
    int parn=Node->Event[cnt++];
    state=Node->Event[cnt++];
    if (parn == param) break;
  }

  //  printf("%x\n",state);

  return state;

}
