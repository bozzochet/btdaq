#include <iostream>
#include <stdio.h>
#include "SlowControl.h"
#include "amswcomtest.h"
#include "HELPgui.h"
#include "slowgui.h"


using namespace std;


SlowGui::SlowGui(const TGWindow *p, UInt_t w, UInt_t h, int tcrate, unsigned int jinfaddr, int port) {

  //XTANT=octant;
  TCrate = tcrate;

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


  fFrame1 = new TGHorizontalFrame(fMain, 100, 100, kFixedWidth);
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
  fMain->AddFrame(fFrame1,fL5);

  //Set a name to the main frame
  fMain->SetWindowName(Form("Slow Control, tcrate %d",TCrate) );
  //Map all subwindows of main frame
  fMain->MapSubwindows();

  //Initialize the laout algorithm
  fMain->Resize(550,300);

  //Map main frame
  fMain->MapWindow();

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

  
  Node=new AMSWcom();
  AMSWmode=kAMSW_JMDC;
  Node->SetHW(AMSWmode);
  Node->SetPort(port);
  Node->SetTimeOut(5000);
  if (Node->Init()==ERROR) {
    printf("init error\n");
    return;
  }

  CreateTbs(Node, jinfaddr);
  CreateTpsfe(Node, jinfaddr);
  CreateS9011AT(Node, jinfaddr);

  TimerCheck=new TTimer();
  //  TimerCheck->Connect("Timeout()","SlowGui",this,"CheckSlowControl()");
  TimerCheck->Connect("Timeout()","SlowGui",this,"RefreshAll()");
  
  TestAMSW(jinfaddr);
  
}


void SlowGui::TestAMSW(unsigned int addr) {
  Node->LecroyWrite(addr,0);
  printf("testamsw: erramsw=%x %d\n", Node->GetRX_DONE(), (short)Node->GetRX_DONE());
  if ((Node->GetHW()==kAMSW_EPP && Node->GetRX_DONE()==0x4180 ) || (Node->GetHW()==kAMSW_PCI && Node->GetRX_DONE()==0) || (Node->GetHW()==kAMSW_JMDC && Node->GetRX_DONE()==0)) {
    RefreshAll();
    EnableS9053();

    //TimerCheck->Start(60000,kFALSE);

    fTab->SetTab("S9011AT");
  } else {
    int reply;
    new TGMsgBox(gClient->GetRoot(), fMain,"Communication problem", "Communication problem",kMBIconStop, kMBOk, &reply);
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
  printf("check tab\n");
  UpdateTabColor(FrameName);
  
  if (!strcmp(FrameName,"TPSFE")) {
    printf("tab TPSFE val = %d\n",val);
    GetTpsfeStatus(val);
  }
  else if(!strcmp(FrameName,"TBS")) {
    printf("tab TBS val = %d\n",val);
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

  if (val==0) {
    // S9011
    printf("S9011 tab\n");
    GetS9011atStatus();
  }
  else if (val==1) {
    // TPSFE
    current=fTPSFETab->GetCurrent();
    GetTpsfeStatus(current);
  } 
  else if (val==2) {
    // TBS
    current=fTBSTab->GetCurrent();
    GetTbsStatus(current);
  } 
}


void SlowGui::RefreshAll() {

   GetS9011atStatus();
   GetTpsfeStatus(0);
   GetTpsfeStatus(1);
   GetTpsfeStatus(2);
   GetTpsfeStatus(3);
   GetTbsStatus(0);
   GetTbsStatus(1);
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



void SlowGui::ShowBrotherTrip(SlowControl *slowctl) {

  Int_t Id=slowctl->GetID();
  ushort trip=slowctl->GetTrip_Brother();
  TGLabel *alabel;

  if (!strcmp(slowctl->GetCardType(),"TBS")) {
    alabel=tTBSTripIndicator[Id];
  } 
  else if (!strcmp(slowctl->GetCardType(),"TPSFE")) {
    alabel=tTPSFETripIndicator[Id];
  }
  else if (!strcmp(slowctl->GetCardType(),"S9011AT")) {
    alabel=tS9011ATTripIndicator;
  }
  else return;

  //  Pixel_t color=alabel->GetDefaultFrameBackground();
  //  Pixel_t text=(trip)?PixelColor[kRed][0]:color;
  //  Pixel_t back=(trip)?PixelColor[kYellow][0]:color;
  //  SetLabelColor(alabel,text, back);
  //  gClient->NeedRedraw(alabel);
  ShowTripIndicator(alabel,trip);
}


void SlowGui::ShowPowerOn(SlowControl *slowctl) {

  Int_t Id=slowctl->GetID();
  ushort pwon=slowctl->GetPowerOn();
  TGLabel *alabel;

  cout << slowctl->GetCardType() << " Id: " << Id << endl;

  if (!strcmp(slowctl->GetCardType(),"TBS")) {
    alabel=tTBSPowerOnIndicator[Id];
  } 
  else if (!strcmp(slowctl->GetCardType(),"TPSFE")) {
    alabel=tTPSFEPowerOnIndicator[Id];
  }
  else if (!strcmp(slowctl->GetCardType(),"S9011AT")) {
    alabel=tS9011ATPowerOnIndicator;
  }
  else return;

//   Pixel_t color=alabel->GetDefaultFrameBackground();
//   Pixel_t text=(trip)?PixelColor[kGreen][0]:color;
//   Pixel_t back=(trip)?PixelColor[kBlue][1]:color;
//   SetLabelColor(alabel,text, back);
//   gClient->NeedRedraw(alabel);
  ShowPowerOnIndicator(alabel, pwon);
}



void SlowGui::BuildActelDisplay(TGCompositeFrame *hframe, TGTextButton **button1, TGTextButton **button2, TGComboBox **combo, TGLabel **label1, TGLabel **label2, Int_t page, const Char_t *type) {
//   TGTextButton *button1, *button2;
//   TGComboBox *combo;
//   TGLabel *label1, *label2;



  CreateTextButton(hframe, button1, "Actel Hot");
  //  *button1=new TGTextButton(hframe,"Actel Hot");
  (*button1)->Connect("Clicked()","SlowGui",this,Form("SwitchActel(=0,%d,\"%s\")",page,type));
  //  (*button1)->SetBackgroundColor(PixelColor[kYellow][0]);
  (*button1)->SetEnabled(kFALSE);

  CreateTextButton(hframe, button2, "Actel Cold");
  //  *button2=new TGTextButton(hframe,"Actel Cold");
  (*button2)->Connect("Clicked()","SlowGui",this,Form("SwitchActel(=1,%d,\"%s\")",page,type));
  //  (*button2)->SetBackgroundColor(PixelColor[kYellow][0]);
 

  *combo=new TGComboBox(hframe);
  (*combo)->AddEntry("Actel Hot",0);
  (*combo)->AddEntry("Actel Cold",1);
  (*combo)->Select(0);
  (*combo)->Resize(100, 20);
  (*combo)->Connect("Selected(Int_t)","SlowGui",this,Form("SelectActel(=0,%d,\"%s\")",page,type));
  //    *label1=new TGLabel(hframe," T ");
  //    SetLabelColor(*label1,PixelColor[kRed][0], PixelColor[kYellow][0]);
  CreateTripIndicator(hframe,label1);
  //  *label2=new TGLabel(hframe," P ");
  //  SetLabelColor(*label2,PixelColor[kGreen][0], PixelColor[kBlue][1]);
  CreatePowerOnIndicator(hframe,label2);

  // now we place all these little things

  hframe->AddFrame(*combo, fL3);
  hframe->AddFrame(*button1, fL3);
  hframe->AddFrame(*button2, fL3);
  hframe->AddFrame(*label1, fL34);
  hframe->AddFrame(*label2, fL34);

}



void SlowGui::SwitchActel(sACTEL Brother, Int_t ID, Char_t *type) {
  printf("switch actel %s of %s %d\n", (Brother==kActelHot)?"hot":"cold", type, ID); 

  sActelMode *ActelState;
  SlowControl *slctl;
  sACTEL Actel=(Brother==kActelHot)?kActelCold:kActelHot;

  if (!strcmp(type,"TBS")) {
    cout << "TBS\n";
    ActelState=ActelTbsState[ID];
    slctl=MyTbs[ID][Actel];
  }
  else if (!strcmp(type,"TPSFE")) {
    cout << "TPSFE\n";
    ActelState=ActelTpsfeState[ID];
    slctl=MyTpsfe[ID][Actel];
  }
  else if (!strcmp(type,"S9011AT")) {
    cout << "S9011AT\n";
    ActelState=ActelS9011ATState;
    slctl=MyS9011AT[Actel];
  }
  else return;

  sActelMode mode,newmode;
  mode=ActelState[Brother];
  newmode=(mode==kActelOn)?kActelOff:kActelOn;
  slctl->SetActelBrother(newmode);

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
  else if (!strcmp(type,"S9011AT")) {
    cout << "S9011AT " << ID << endl;
    hot=tS9011ATActelHotButton; cold=tS9011ATActelColdButton; selection=&ActelS9011AT;
  }
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
  initlecroy->SetToolTipText("Init Lecroy...");
  return initlecroy;
}


void SlowGui::InitLecroy(Int_t val1, Int_t val2) {
  // we also init the parallel port
  printf("init port\n");
  Node->Shut();
  if (Node->Init()==ERROR) {
    printf("init error\n");
    return;
  }

  if (val2<0) return;
  if (val1<0 || val1>2) return;

  SlowControl *actel;

  if (val1==0) actel=MyS9011AT[ActelS9011AT];
  if (val1==1 && val2<NTPSFE) actel=MyTpsfe[val2][ActelTpsfe[val2]];
  if (val1==2 && val2<NTBS) actel=MyTbs[val2][ActelTbs[val2]];

  actel->InitLecroy();
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
