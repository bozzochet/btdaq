#include "MainFrame.h"
#include "Messages.h"
#include <iostream>
////////////////////////////////////////////////////////////////////////////
// Work
// Create a Work Frame where there will be placed the Control and Display Areas
////////////////////////////////////////////////////////////////////////////
void MainFrame::Work(){

 cout << DefineFonts::fFont[0] << " " << DefineFonts::fFont[1] << " " << DefineFonts::fFont[2] << " " << DefineFonts::fFont[3] <<endl;
  

  UInt_t fWorkOpt = kVerticalFrame | kRaisedFrame;
  fWork = new TGCompositeFrame(this,800,0,fWorkOpt);  

  UInt_t fSubOpt = kHorizontalFrame;
  TGCompositeFrame *fSub = new TGCompositeFrame(fWork,800,0,fSubOpt); 

  Control();
  UInt_t fControlOpt = kLHintsLeft;
  TGLayoutHints *fControlLay = new TGLayoutHints(fControlOpt,0, 0, 0, 0);

  Display();
  UInt_t fDisplayOpt = kLHintsLeft;
  TGLayoutHints *fDisplayLay = new TGLayoutHints(fDisplayOpt,0, 0, 0, 0);  

  Text();
  UInt_t fTextOpt = kLHintsLeft;
  TGLayoutHints *fTextLay = new TGLayoutHints(fTextOpt,0, 0, 0, 0);  
  

  fSub->AddFrame(fControl,fControlLay);
  fSub->AddFrame(fDisplay,fDisplayLay);
  fWork->AddFrame(fSub);
  fWork->AddFrame(fText  , fTextLay);


}

void MainFrame::Control(){
  UInt_t fControlOpt = kVerticalFrame | kFixedWidth;
  fControl = new TGCompositeFrame(fWork,230,520,fControlOpt);

  TGLayoutHints *fLay=new TGLayoutHints(kLHintsLeft | kLHintsExpandX,5,0,4,4);

  // Define Ladder Info Area
  LadderInfo();
  fControl->AddFrame(fId,fLay);

  // Define Run Info Area
  RunInfo();
  fControl->AddFrame(fRunInfo,fLay);

  // Define Run Control Area
  RunControl();
  fControl->AddFrame(fRunControl,fLay);

  // Define Run Control Area
  ProcessControl();
  fControl->AddFrame(fProcessControl,fLay);

}



void MainFrame::LadderInfo(){    // LADDER INFO GROUP
  UInt_t fIdOpt = kVerticalFrame;
  fId = new TGGroupFrame(fControl,"Ladder Control",fIdOpt);//,ffontcon[2],ffont[2]);
  fId->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());

  TGCompositeFrame *fSubA = new TGCompositeFrame(fId,185,0,kHorizontalFrame);
  TGCompositeFrame *fSub01= new TGCompositeFrame(fSubA, 85,0,kVerticalFrame); 
  TGCompositeFrame *fSub02= new TGCompositeFrame(fSubA,100,0,kVerticalFrame);

  TGCompositeFrame *fSubB = new TGCompositeFrame(fId,185,0,kHorizontalFrame);
  TGCompositeFrame *fSub03= new TGCompositeFrame(fSubB, 85,0,kVerticalFrame); 
  TGCompositeFrame *fSub04= new TGCompositeFrame(fSubB, 70,0,kVerticalFrame);
  TGCompositeFrame *fSub05= new TGCompositeFrame(fSubB, 40,0,kVerticalFrame);

  TGCompositeFrame *fSubC = new TGCompositeFrame(fId ,185,0,kHorizontalFrame);
 
  fSub01->SetLayoutManager(new TGMatrixLayout(fSub01,0,1,0,kLHintsLeft));
  fIdf11 = new TGLabel(fSub01,"Ladder:    ");//,ffontcon[2],ffont[2]);
  fIdf11->SetTextFont(DefineFonts::fFont[2]);
  fIdf11->Resize(85,fIdf11->GetDefaultHeight());
  fIdf11->SetTextJustify(kTextLeft);
  fIdf21 = new TGLabel(fSub01,"Log Status:");//,ffontcon[2],ffont[2]);
  fIdf21->SetTextFont(DefineFonts::fFont[2]);
  fIdf21->Resize(85,fIdf21->GetDefaultHeight());
  fIdf21->SetTextJustify(kTextLeft);
  fIdf31 = new TGLabel(fSub01,"Mode:      ");//,ffontcon[2],ffont[2]);
  fIdf31->SetTextFont(DefineFonts::fFont[2]);
  fIdf31->Resize(85,fIdf31->GetDefaultHeight());
  fIdf31->SetTextJustify(kTextLeft);
  fIdf41 = new TGLabel(fSub01,"Trigger:   ");//,ffontcon[2],ffont[2]);
  fIdf41->SetTextFont(DefineFonts::fFont[2]);
  fIdf41->Resize(85,fIdf41->GetDefaultHeight());
  fIdf41->SetTextJustify(kTextLeft);
  fIdf51 = new TGLabel(fSub01,"Source:    ");//,ffontcon[2],ffont[2]);
  fIdf51->SetTextFont(DefineFonts::fFont[2]);
  fIdf51->Resize(85,fIdf51->GetDefaultHeight());
  fIdf51->SetTextJustify(kTextLeft);
  TGLabel *alabel=new TGLabel(fSub01,"Crate:");//,ffontcon[2],ffont[2]);
  alabel->SetTextFont(DefineFonts::fFont[2]);
  alabel->Resize(85,alabel->GetDefaultHeight());
  alabel->SetTextJustify(kTextLeft);
  TGLabel *alabel2=new TGLabel(fSub01,"DSP code:");//,ffontcon[2],ffont[2]);
  alabel2->SetTextFont(DefineFonts::fFont[2]);
  alabel2->Resize(85,alabel2->GetDefaultHeight());
  alabel2->SetTextJustify(kTextLeft);
  TGLabel *alabel3=new TGLabel(fSub01,"Master addr:");//,ffontcon[2],ffont[2]);
  alabel3->SetTextFont(DefineFonts::fFont[2]);
  alabel3->Resize(85,alabel3->GetDefaultHeight());
  alabel3->SetTextJustify(kTextLeft);


  fSub02->SetLayoutManager(new TGMatrixLayout(fSub02,0,1,0,kLHintsLeft));
  fIdf12 = new TGLabel(fSub02,"XXXXXXXX");//,ffontcon[3],ffont[3]);
  fIdf12->SetTextFont(DefineFonts::fFont[3]);
  fIdf12->SetTextColor(0xff0000);
  fIdf12->Resize(100,fIdf12->GetDefaultHeight());
  fIdf12->SetTextJustify(kTextLeft);
  fIdf22 = new TGComboBox(fSub02,kCB_LOG_STATUS);
  fIdf22->AddEntry("Not Logging",kLOGSTATUS_NOTLOG);  
  fIdf22->AddEntry("Log binary file",kLOGSTATUS_BINARY);
  fIdf22->AddEntry("Log ASCII file",kLOGSTATUS_ASCII);
  fIdf22->Select(kLOGSTATUS_NOTLOG);
  fIdf22->Resize(100,fIdf12->GetDefaultHeight());
  fIdf22->Associate(this);  
  //toCTL->Send(kGUI,kSetLogStatus,kLOGSTATUS_NOTLOG);
  control->LogStatus=kLOGSTATUS_NOTLOG;
  fIdf32 = new TGComboBox(fSub02,kCB_DATA_MODE);
  fIdf32->AddEntry("Raw Data"       ,kDATAMODE_RAW);  
  fIdf32->AddEntry("Cal - Octant"   ,kOCTANTCALIBRATION);
  fIdf32->AddEntry("Cal - All"      ,kFULLCALIBRATION);
  fIdf32->AddEntry("Cal - DSP"      ,kDSPCALIBRATION);
  fIdf32->AddEntry("Cal - Pedestal" ,cPASS_ALL);
  fIdf32->AddEntry("Cal - Gain"     ,kDATAMODE_GAIN);
  fIdf32->AddEntry("TDR Data"       ,kDATAMODE_REDUCTION);
  fIdf32->AddEntry("FFT"            ,kDATAMODE_FFT);
  fIdf32->AddEntry("Read Channel"   ,kDATAMODE_SCOPE);
  fIdf32->AddEntry("Cosmics"        ,kDATAMODE_COSMIC);
  fIdf32->Select(kDATAMODE_RAW);
  fIdf32->Resize(100,fIdf12->GetDefaultHeight());
  fIdf32->Associate(this);  
  //toCTL->Send(kGUI,kSetDataMode,kDATAMODE_RAW);
  control->DataMode=kDATAMODE_RAW;
  fIdf42 = new TGComboBox(fSub02,kCB_TRIGGER);
  fIdf42->AddEntry("Random"  ,kTRIGGER_RANDOM);  
  fIdf42->AddEntry("External",kTRIGGER_EXTERNAL);
  fIdf42->Select(kTRIGGER_EXTERNAL);
  fIdf42->Resize(100,fIdf12->GetDefaultHeight());
  fIdf42->Associate(this);
  //toCTL->Send(kGUI,kSetTrigger,kTRIGGER_RANDOM);
  control->Trigger=kTRIGGER_EXTERNAL;
  fIdf52 = new TGComboBox(fSub02,kCB_SOURCE);
  fIdf52->AddEntry("Simulation",kSOURCE_SIMULATION);  
  fIdf52->AddEntry("Real Data" ,kSOURCE_REALDATA);
  fIdf52->AddEntry("Binary File" ,kSOURCE_BINARYFILE);
  fIdf52->AddEntry("ASCII File" ,kSOURCE_ASCIIFILE);
  fIdf52->AddEntry("Beam Test X5" ,kSOURCE_BEAMTESTX5);
  fIdf52->AddEntry("TDR" ,kSOURCE_TDR);
  fIdf52->AddEntry("JINF" ,kSOURCE_JINF);
  fIdf52->Select(kSOURCE_JINF);
  fIdf52->Resize(100,fIdf12->GetDefaultHeight());
  fIdf52->Associate(this);  
  //toCTL->Send(kGUI,kSetSource,kSOURCE_REALDATA);
  control->Source=kSOURCE_JINF;

  fComboXtant=new TGComboBox(fSub02,kCB_XTANT);
  fComboXtant->AddEntry("0",1);
  fComboXtant->AddEntry("1",2);
  fComboXtant->AddEntry("2",3);
  fComboXtant->AddEntry("3",4);
  fComboXtant->AddEntry("4",5);
  fComboXtant->AddEntry("5",6);
  fComboXtant->AddEntry("6",7);
  fComboXtant->AddEntry("7",8);
  fComboXtant->Select(1+control->TrackerXtant);
  fComboXtant->Resize(100,alabel->GetDefaultHeight());
  fComboXtant->Associate(this);

  fNEtdrdsp=new TGNumberEntry(fSub02,control->TDRprog,5,kTE_TDRDSP_CODE);
  fNEtdrdsp->SetFormat(TGNumberFormat::kNESHex, TGNumberFormat::kNEANonNegative);
  fNEtdrdsp->SetLimits(TGNumberFormat::kNELLimitMinMax,0,0xffff);
  fNEtdrdsp->Resize(100,alabel2->GetDefaultHeight());
  fNEtdrdsp->Associate(this);

  fNEamswsl=new TGNumberEntry(fSub02,control->JINFaddr,5,kTE_AMSWSL);
  fNEamswsl->SetFormat(TGNumberFormat::kNESHex, TGNumberFormat::kNEANonNegative);
  fNEamswsl->SetLimits(TGNumberFormat::kNELLimitMinMax,0,0xffff);
  fNEamswsl->Resize(100,alabel3->GetDefaultHeight());
  fNEamswsl->Associate(this);
  
  fSubC->SetLayoutManager(new TGMatrixLayout(fSubC,0,3,0,kLHintsLeft));
  fIdbutS1= new TGTextButton(fSubC,"     S1     ",kTB_ANALOG_S1);
  fIdbutS1->ChangeBackground(fGreen);
  fIdbutS1->SetState(kButtonUp);
  fIdbutS1->Resize(50, fIdbutS1->GetDefaultHeight());
  fIdbutS1->Associate(this);
  fIdbutS2= new TGTextButton(fSubC,"     S2     ",kTB_ANALOG_S2);
  fIdbutS2->ChangeBackground(fGreen);
  fIdbutS2->SetState(kButtonUp);
  fIdbutS2->Resize(50, fIdbutS2->GetDefaultHeight());
  fIdbutS2->Associate(this);
  fIdbutK= new TGTextButton(fSubC, "     K     ",kTB_ANALOG_K);
  fIdbutK->ChangeBackground(fGreen);
  fIdbutK->SetState(kButtonUp);
  fIdbutK->Resize(50, fIdbutK->GetDefaultHeight());
  fIdbutK->Associate(this);


  fSub01->AddFrame(fIdf11); fSub02->AddFrame(fIdf12);    
  fSub01->AddFrame(fIdf21); fSub02->AddFrame(fIdf22); 
  fSub01->AddFrame(fIdf31); fSub02->AddFrame(fIdf32);
  fSub01->AddFrame(fIdf41); fSub02->AddFrame(fIdf42);   
  fSub01->AddFrame(fIdf51); fSub02->AddFrame(fIdf52);   
  fSub01->AddFrame(alabel); fSub02->AddFrame(fComboXtant);   
  fSub01->AddFrame(alabel2); fSub02->AddFrame(fNEtdrdsp);   
  fSub01->AddFrame(alabel3); fSub02->AddFrame(fNEamswsl);   


  //  fSub03->AddFrame(fIdf71); fSub04->AddFrame(fIdf72); fSub05->AddFrame(fIdf73);
  //  fSub03->AddFrame(fIdf81); fSub04->AddFrame(fIdf82); fSub05->AddFrame(fIdf83);
  //  fSub03->AddFrame(fIdf91); fSub04->AddFrame(fIdf92);
  //  fSub03->AddFrame(fIdfA1); fSub04->AddFrame(fIdfA2);

  fSubC->AddFrame(fIdbutS1); 
  fSubC->AddFrame(fIdbutS2); 
  fSubC->AddFrame(fIdbutK);

  TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsLeft,0,0,5,0);
  fSubA->AddFrame(fSub01,fSubLay);
  fSubA->AddFrame(fSub02,fSubLay);

  //  fSubB->AddFrame(fSub03,fSubLay);
  //  fSubB->AddFrame(fSub04,fSubLay);
  //  fSubB->AddFrame(fSub05,fSubLay);
 

  fId->AddFrame(fSubA,fSubLay);
  fId->AddFrame(fSubB,fSubLay);
  //fId->AddFrame(fSub3,fSubLay);

  fSubLay = new TGLayoutHints(kLHintsLeft,2,2,10,2);
  fId->AddFrame(fSubC,fSubLay);



}


void MainFrame::RunInfo(){
  UInt_t fRIOpt = kHorizontalFrame;
  fRunInfo = new TGGroupFrame(fControl,"Run Info",fRIOpt);//,ffontcon[2],ffont[2]);
  fRunInfo->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());

  UInt_t fSubOpt = kVerticalFrame;// | kFixedSize ;
  TGCompositeFrame *fSub1 = new TGCompositeFrame(fRunInfo,100,0,fSubOpt); 
  TGCompositeFrame *fSub2 = new TGCompositeFrame(fRunInfo, 85,0,fSubOpt);
 
  fSub1->SetLayoutManager(new TGMatrixLayout(fSub1,0,1,0,kLHintsLeft));
  fRIf11 = new TGLabel(fSub1,"Run:        ");//,ffontcon[2],ffont[2]);
  fRIf11->SetTextFont(DefineFonts::fFont[2]);
  fRIf11->Resize(85,fRIf11->GetDefaultHeight());
  fRIf11->SetTextJustify(kTextLeft);
  fRIf21 = new TGLabel(fSub1,"Event:      ");//,ffontcon[2],ffont[2]);
  fRIf21->SetTextFont(DefineFonts::fFont[2]);
  fRIf21->Resize(85,fRIf21->GetDefaultHeight());
  fRIf21->SetTextJustify(kTextLeft);
  fRIf31 = new TGLabel(fSub1,"Run Time:   ");//,ffontcon[2],ffont[2]);
  fRIf31->SetTextFont(DefineFonts::fFont[2]);
  fRIf31->Resize(85,fRIf31->GetDefaultHeight());
  fRIf31->SetTextJustify(kTextLeft);

  fRIf51 = new TGLabel(fSub1,"Rate:       ");//,ffontcon[2],ffont[2]);
  fRIf51->SetTextFont(DefineFonts::fFont[2]);
  fRIf51->Resize(85,fRIf51->GetDefaultHeight());
  fRIf51->SetTextJustify(kTextLeft);
  fRIf61 = new TGLabel(fSub1,"Bad pwr S:  ");//,ffontcon[2],ffont[2]);
  fRIf61->SetTextFont(DefineFonts::fFont[2]);
  fRIf61->Resize(85,fRIf61->GetDefaultHeight());
  fRIf61->SetTextJustify(kTextLeft);
  fRIf71 = new TGLabel(fSub1,"Bad pwr K:  ");//,ffontcon[2],ffont[2]);
  fRIf71->SetTextFont(DefineFonts::fFont[2]);
  fRIf71->Resize(85,fRIf71->GetDefaultHeight());
  fRIf71->SetTextJustify(kTextLeft);

  fSub2->SetLayoutManager(new TGMatrixLayout(fSub2,0,1,0,kLHintsLeft));
  fRIf12 = new TGLabel(fSub2,"0");//,ffontcon[3],ffont[3]);
  fRIf12->SetTextFont(DefineFonts::fFont[3]);
  fRIf12->SetTextColor(0xff0000);
  fRIf12->Resize(100,fRIf12->GetDefaultHeight());
  fRIf12->SetTextJustify(kTextLeft);
  fRIf22 = new TGLabel(fSub2,"0");//,ffontcon[3],ffont[3]);
  fRIf22->SetTextFont(DefineFonts::fFont[3]);
  fRIf22->SetTextColor(0xff0000);
  fRIf22->Resize(100,fRIf22->GetDefaultHeight());
  fRIf22->SetTextJustify(kTextLeft);
  fRIf32 = new TGLabel(fSub2,"00:00:00  ");//,ffontcon[3],ffont[3]);
  fRIf32->SetTextFont(DefineFonts::fFont[3]);
  fRIf32->SetTextColor(0xff0000);
  fRIf32->Resize(100,fRIf32->GetDefaultHeight());
  fRIf32->SetTextJustify(kTextLeft);

  fRIf52 = new TGLabel(fSub2,"0000.00 Hz");//,ffontcon[3],ffont[3]);
  fRIf52->SetTextFont(DefineFonts::fFont[3]);
  fRIf52->SetTextColor(0xff0000);
  fRIf52->Resize(100,fRIf52->GetDefaultHeight());
  fRIf52->SetTextJustify(kTextLeft);
  fRIf62 = new TGLabel(fSub2,"0");//,ffontcon[3],ffont[3]);
  fRIf62->SetTextFont(DefineFonts::fFont[3]);
  fRIf62->SetTextColor(0xff0000);
  fRIf62->Resize(100,fRIf62->GetDefaultHeight());
  fRIf62->SetTextJustify(kTextLeft);
  fRIf72 = new TGLabel(fSub2,"0");//,ffontcon[3],ffont[3]);
  fRIf72->SetTextFont(DefineFonts::fFont[3]);
  fRIf72->SetTextColor(0xff0000);
  fRIf72->Resize(100,fRIf72->GetDefaultHeight());
  fRIf72->SetTextJustify(kTextLeft);

  fSub1->AddFrame(fRIf11);  fSub2->AddFrame(fRIf12);    
  fSub1->AddFrame(fRIf21);  fSub2->AddFrame(fRIf22); 
  fSub1->AddFrame(fRIf31);  fSub2->AddFrame(fRIf32);
  //    fSub1->AddFrame(fRIf41);  fSub2->AddFrame(fRIf42);   
  fSub1->AddFrame(fRIf51);  fSub2->AddFrame(fRIf52);   
  fSub1->AddFrame(fRIf61);  fSub2->AddFrame(fRIf62);   
  fSub1->AddFrame(fRIf71);  fSub2->AddFrame(fRIf72);   

  TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsLeft,0,0,5,0);
  fRunInfo->AddFrame(fSub1,fSubLay);
  fRunInfo->AddFrame(fSub2,fSubLay);
}



void MainFrame::RunControl(){

  UInt_t fRCOpt = kVerticalFrame;
  fRunControl = new TGGroupFrame(fControl,"Run Control",fRCOpt);//, ffontcon[2],ffont[2]);
  fRunControl->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());

  TGHorizontalFrame *myhframe=new TGHorizontalFrame(fRunControl);

  fRCf11 = new TGTextButton(myhframe,"Start Run",kTB_START_RUN);
  fRCf11->ChangeBackground(fGreen);
  fRCf11->SetState(kButtonUp);
  fRCf11->Resize(90,fRCf11->GetDefaultHeight());
  fRCf11->Associate(this);
  control->Enable_START_RUN=1; 

  fRCf12 = new TGTextButton(myhframe, "Stop Run",kTB_STOP_RUN);
  fRCf12->ChangeBackground(fRed);
  fRCf12->SetState(kButtonDown);
  fRCf12->Resize(90,fRCf12->GetDefaultHeight());
  fRCf12->Associate(this);
  control->Enable_STOP_RUN=0;

  myhframe->AddFrame(fRCf11, new TGLayoutHints(kLHintsExpandX,2,2,2,2)); 
  myhframe->AddFrame(fRCf12, new TGLayoutHints(kLHintsExpandX,2,2,2,2));

  TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsExpandX,2,2,2,2);
  fRunControl->AddFrame(myhframe,fSubLay);


}


void MainFrame::ProcessControl(){
  UInt_t fPCOpt = kVerticalFrame;
  fProcessControl = new TGGroupFrame(fControl,"Process Control",fPCOpt);//,ffontcon[2],ffont[2]);
  fProcessControl->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());

  TGHorizontalFrame *myhframe=new TGHorizontalFrame(fProcessControl);

  fPCf11 = new TGTextButton(myhframe,"Check All",kTB_CHECK_ALL);
  fPCf11->Resize(90,fPCf11->GetDefaultHeight());
  fPCf11->Associate(this);

  myhframe->AddFrame(fPCf11, new TGLayoutHints(kLHintsLeft,2,2,2,2));

  TGHorizontalFrame *myhframe2=new TGHorizontalFrame(fProcessControl);

  fPCf21 = new TGTextButton(myhframe2,"Control",kTB_CHECK_CONTROL);
  fPCf21->Resize(90,fPCf21->GetDefaultHeight());
  fPCf21->Associate(this);

  fPCf22 = new TGTextButton(myhframe2,"Run",kTB_CHECK_RUN);
  fPCf22->Resize(90,fPCf22->GetDefaultHeight());
  fPCf22->Associate(this);

  myhframe2->AddFrame(fPCf21, new TGLayoutHints(kLHintsExpandX,2,2,2,2));
  myhframe2->AddFrame(fPCf22, new TGLayoutHints(kLHintsExpandX,2,2,2,2));

  TGHorizontalFrame *myhframe3=new TGHorizontalFrame(fProcessControl);

  fPCf31 = new TGTextButton(myhframe3,"DSP",kTB_CHECK_DSP);
  fPCf31->Resize(90,fPCf31->GetDefaultHeight());
  fPCf31->Associate(this);

  fPCf32 = new TGTextButton(myhframe3,"Log",kTB_CHECK_LOG);
  fPCf32->Resize(90,fPCf32->GetDefaultHeight());
  fPCf32->Associate(this);

  myhframe3->AddFrame(fPCf31, new TGLayoutHints(kLHintsExpandX,2,2,2,2));
  myhframe3->AddFrame(fPCf32, new TGLayoutHints(kLHintsExpandX,2,2,2,2));

  TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsExpandX,2,2,2,2);
  fProcessControl->AddFrame(myhframe,fSubLay);
  fProcessControl->AddFrame(myhframe2,fSubLay);
  fProcessControl->AddFrame(myhframe3,fSubLay);





}



void MainFrame::Display(){
  UInt_t fOpt = kVerticalFrame;
  fDisplay = new TGCompositeFrame(fWork,500,500,fOpt);

  // Define Selection Area
  Selection();
  fOpt = kLHintsLeft; 
  TGLayoutHints *fSelLay = new TGLayoutHints(fOpt,0,0,0,0);
  //TDRSelection();
  // Define Canvas Area
  Canvas();
  fOpt = kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY;
  TGLayoutHints *fCanvasLay = new TGLayoutHints(fOpt,5,0,0,2);

  fDisplay->AddFrame(fSel,fSelLay);
  //fDisplay->AddFrame(fSelectTDR,fSelLay);
  fDisplay->AddFrame(fCanvas,fCanvasLay);
}


void MainFrame::TDRSelection() {

  UInt_t fOpt = kHorizontalFrame;
  UInt_t fOpt2 = kVerticalFrame;

  const int tdrplace[2][12]={
    { 0, 4, 8, 12, 16, 20, 22, 18, 14, 10, 6, 2},
    { 1, 5, 9, 13, 17, 21, 23, 19, 15, 11, 7, 3}
  };

  fSelectTDR = new TGGroupFrame(fDisplay,"Select TDR",fOpt2);//,ffontcon[2],ffont[2]);
  fSelectTDR->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());

  for (int i=0; i<4; i++)  fTDRSel[i] = new TGCompositeFrame(fSelectTDR,600,0,fOpt);
  

  TGLayoutHints *fLay=new TGLayoutHints(kLHintsLeft | kLHintsTop,1,0,0,1);
  TGLayoutHints *fLay2=new TGLayoutHints(kLHintsLeft,1,0,10,1);

  TGLayoutHints *fLay1=new TGLayoutHints(kLHintsLeft | kLHintsTop,5,0,0,1);
  //  TGLayoutHints *fLay21=new TGLayoutHints(kLHintsLeft,2,0,10,1);
  

  for (int gr=0; gr<4; gr++) {
    for (int cnt=0; cnt<6; cnt++) {
      int tdr=tdrplace[gr%2][cnt+(gr>1)*6];
      fTDRButton[tdr]=new TGTextButton(fTDRSel[gr],Form("%02d",tdr),kTB_TDRSEL_0+tdr);
      //fTDRButton[tdr]->ChangeBackground(fYellow);
      fTDRButton[tdr]->SetState(kButtonUp);
      fTDRButton[tdr]->Associate(this);
      
      unsigned int bheight=10;//0.8*fTDRButton[tdr]->GetHeight();
      fTDRButton[tdr]->Resize(fTDRButton[tdr]->GetWidth(),bheight);
      
      fTDRButton[tdr]->SetToolTipText(control->LadderNameOctant[tdr],250); //250 ms delay, is the shortest possible according to the doc
      fTDRSel[gr]->AddFrame(fTDRButton[tdr],(cnt==3)?fLay1:fLay);
      //cout << "button " << tdr << " " << fTDRButton[tdr] << endl;
    }
    fSelectTDR->AddFrame(fTDRSel[gr],(gr%2)?fLay:fLay2);
  }
  
  
  TGHorizontalFrame *myhframe=new TGHorizontalFrame(fSelectTDR);
  TGLabel *alabel=new TGLabel(myhframe,"PWG mode:");//,ffontcon[2],ffont[2]);
  alabel->SetTextFont(DefineFonts::fFont[2]);
  myhframe->AddFrame(alabel, new TGLayoutHints(kLHintsLeft,2,2,2,2));

  fChk2 = new TGCheckButton(myhframe, "", kCHB_PWG_MODE);//,ffontcon[2],ffont[2]);
  fChk2->SetFont(DefineFonts::fFont[2]->GetFontStruct());
  fChk2->Resize(140,alabel->GetDefaultHeight());
  fChk2->SetState((control->ReadoutPWG)?kButtonDown:kButtonUp);
  fChk2->Associate(this);
  myhframe->AddFrame(fChk2, new TGLayoutHints(kLHintsLeft,2,2,2,2));


  fSelectTDR->AddFrame(myhframe, new TGLayoutHints(kLHintsExpandX | kLHintsBottom ,2,2,10,0));



}


void MainFrame::Selection(){
  UInt_t fOpt = kHorizontalFrame;

  fSel = new TGCompositeFrame(fDisplay,600,0,fOpt);

  TGLayoutHints *fLay=new TGLayoutHints(kLHintsLeft | kLHintsExpandY,5,0,5,0);

  TDRSelection();
  fSel->AddFrame(fSelectTDR,fLay);


  SelectHisto();
  fSel->AddFrame(fSelectHisto,fLay);

  HistoOptions();
  fSel->AddFrame(fHistoOptions,fLay);
}

void MainFrame::SelectHisto(){  

  UInt_t fOpt = kVerticalFrame;
  fSelectHisto = new TGGroupFrame(fSel,"Select Histogram",fOpt);//,ffontcon[2],ffont[2]);
  fSelectHisto->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());

  TGHorizontalFrame *myhframe=new TGHorizontalFrame(fSelectHisto);

  fOpt = kVerticalFrame; 
  TGCompositeFrame *fSub1 = new TGCompositeFrame(myhframe, 85,0,fOpt);
  fSub1->SetLayoutManager(new TGMatrixLayout(fSub1,0,1,2,kLHintsLeft));
  fSHf11 = new TGLabel(fSub1,"Histogram:");//,ffontcon[2],ffont[2]);
  fSHf11->SetTextFont(DefineFonts::fFont[2]);
  fSHf11->Resize(85,fSHf11->GetDefaultHeight());
  fSHf11->SetTextJustify(kTextLeft);
  //  fSHf71 = new TGLabel(fSub1,"TDR:",ffontcon[2],ffont[2]);
  //  fSHf71->Resize(85,fSHf71->GetDefaultHeight());
  //  fSHf71->SetTextJustify(kTextLeft);
  fSHf21 = new TGLabel(fSub1,"VA:");//,ffontcon[2],ffont[2]);
  fSHf21->SetTextFont(DefineFonts::fFont[2]);
  fSHf21->Resize(85,fSHf21->GetDefaultHeight());
  fSHf21->SetTextJustify(kTextLeft);
  fSHf31 = new TGLabel(fSub1,"Channel:");//,ffontcon[2],ffont[2]);
  fSHf31->SetTextFont(DefineFonts::fFont[2]);
  fSHf31->Resize(85,fSHf31->GetDefaultHeight());
  fSHf31->SetTextJustify(kTextLeft);
  fSHf61 = new TGLabel(fSub1,"Persist:");//,ffontcon[2],ffont[2]);
  fSHf61->SetTextFont(DefineFonts::fFont[2]);
  fSHf61->Resize(85,fSHf61->GetDefaultHeight());
  fSHf61->SetTextJustify(kTextLeft);
  //  fSHf41 = new TGLabel(fSub1,"DACchan:",ffontcon[2],ffont[2]);
  //    fSHf41->Resize(85,fSHf41->GetDefaultHeight());
  //    fSHf41->SetTextJustify(kTextLeft);
  fSHf51 = new TGLabel(fSub1,"DACval:");//,ffontcon[2],ffont[2]);
  fSHf51->SetTextFont(DefineFonts::fFont[2]);
  fSHf51->Resize(85,fSHf51->GetDefaultHeight());
  fSHf51->SetTextJustify(kTextLeft);

  TGCompositeFrame *fSub2 = new TGCompositeFrame(myhframe,100,0,fOpt);
  fSub2->SetLayoutManager(new TGMatrixLayout(fSub2,0,1,2,kLHintsLeft));
  fSHf12 = new TGComboBox(fSub2,kCB_HISTOS_HISTOGRAM);
  fSHf12->AddEntry("Scope", kHISTO_SCOPE); 
  fSHf12->AddEntry("Signal", kHISTO_SIGNAL); 
  fSHf12->AddEntry("Ped subtracted", kHISTO_PEDSUB);
  fSHf12->AddEntry("Pedestal", kHISTO_PEDESTAL); 
  fSHf12->AddEntry("Sigma", kHISTO_SIGMA); 
  fSHf12->AddEntry("Sigma Raw", kHISTO_SIGMARAW); 
  fSHf12->AddEntry("Ref Pedestal", kHISTO_PEDREF);
  fSHf12->AddEntry("Ped. diff. with ref", kHISTO_PEDDIFF);  
  fSHf12->AddEntry("Sigma comp. with ref", kHISTO_SIGMACOMP);  
  fSHf12->AddEntry("Sigma Raw comp. with ref", kHISTO_SIGMARAWCOMP);  
  fSHf12->AddEntry("Signal-Mean", kHISTO_SIGNAL_MEAN); 
  fSHf12->AddEntry("Signal-Hits", kHISTO_SIGNAL_HITS); 
  fSHf12->AddEntry("Raw Calibration",kHISTO_RAWCAL);
  fSHf12->AddEntry("Save Raw Calibration",kSAVE_HISTO_RAWCAL);
  fSHf12->AddEntry("Pedestal Summary", kHISTO_PEDSUM); 
  fSHf12->AddEntry("SigRaw Summary", kHISTO_SIGRAWSUM); 
  fSHf12->AddEntry("Common Noise", kHISTO_CN); 
  fSHf12->AddEntry("Sigma Summary", kHISTO_SIGSUM);
  fSHf12->AddEntry("Gain", kHISTO_GAIN); 
  fSHf12->AddEntry("Gain-Raw values", kHISTO_GAIN_VALUE); 
  fSHf12->AddEntry("Gain-ADC vs DAC", kHISTO_GAINPLOT);
  fSHf12->AddEntry("Shaping Time", kHISTO_SHAPING); 
  fSHf12->AddEntry("Cl Length(S)", kHISTO_CLUSTER_LENGTH_S); 
  fSHf12->AddEntry("Cl Length(K)", kHISTO_CLUSTER_LENGTH_K); 
  fSHf12->AddEntry("Cl Number(S)", kHISTO_CLUSTER_NUMBER_S); 
  fSHf12->AddEntry("Cl Number(K)", kHISTO_CLUSTER_NUMBER_K); 
  fSHf12->AddEntry("Cl Cog", kHISTO_CLUSTER_COG); 
  fSHf12->AddEntry("Cl Maxloc", kHISTO_CLUSTER_MAXLOC); 
  fSHf12->AddEntry("Cl Integral(S)", kHISTO_CLUSTER_INTEGRAL_S); 
  fSHf12->AddEntry("Cl Integral(K)", kHISTO_CLUSTER_INTEGRAL_K); 
  fSHf12->AddEntry("Cl Sovern(S)", kHISTO_CLUSTER_SOVERN_S); 
  fSHf12->AddEntry("Cl Sovern(K)", kHISTO_CLUSTER_SOVERN_K); 
  fSHf12->AddEntry("Spectrum S1", kHISTO_SPECTRUM_S1);
  fSHf12->AddEntry("Spectrum S2", kHISTO_SPECTRUM_S2);
  fSHf12->AddEntry("Spectrum K", kHISTO_SPECTRUM_K);
  fSHf12->Select(kHISTO_SCOPE);
  fSHf12->Resize(100,fSHf11->GetDefaultHeight());
  fSHf12->Associate(this);
  fShowHisto = kHISTO_SCOPE;
  char value[200];
  //  fSHTb72 = new TGTextBuffer(1034);
  //  fSHf72 = new TGTextEntry(fSub2,fSHTb72,kTE_HISTOS_TDR);
  //  fSHf72->Resize(50,fSHf71->GetDefaultHeight());
  //  fSHTb72->Clear();
  //sprintf(value,"%d",control->TDRref);
  //  fSHTb72->AddText(0,value);
  //  fSHf72->Associate(this);
  fSHf22 = new TGComboBox(fSub2,kCB_HISTOS_VA);
  fSHf22->AddEntry("01", 1); fSHf22->AddEntry("02", 2);
  fSHf22->AddEntry("03", 3); fSHf22->AddEntry("04", 4);
  fSHf22->AddEntry("05", 5); fSHf22->AddEntry("06", 6);
  fSHf22->AddEntry("07", 7); fSHf22->AddEntry("08", 8);
  fSHf22->AddEntry("09", 9); fSHf22->AddEntry("10",10);
  fSHf22->AddEntry("11",11); fSHf22->AddEntry("12",12);
  fSHf22->AddEntry("13",13); fSHf22->AddEntry("14",14);
  fSHf22->AddEntry("15",15); fSHf22->AddEntry("16",16);
  fSHf22->Select(1);
  fSHf22->Resize(50,fSHf21->GetDefaultHeight());
  fSHf22->Associate(this);
  fva=1;
  fSHf32 = new TGComboBox(fSub2,kCB_HISTOS_CHANNEL);
  fSHf32->AddEntry("01", 1); fSHf32->AddEntry("02", 2);
  fSHf32->AddEntry("03", 3); fSHf32->AddEntry("04", 4);
  fSHf32->AddEntry("05", 5); fSHf32->AddEntry("06", 6);
  fSHf32->AddEntry("07", 7); fSHf32->AddEntry("08", 8);
  fSHf32->AddEntry("09", 9); fSHf32->AddEntry("10",10);
  fSHf32->AddEntry("11",11); fSHf32->AddEntry("12",12);
  fSHf32->AddEntry("13",13); fSHf32->AddEntry("14",14);
  fSHf32->AddEntry("15",15); fSHf32->AddEntry("16",16);
  fSHf32->AddEntry("17",17); fSHf32->AddEntry("18",18);
  fSHf32->AddEntry("19",19); fSHf32->AddEntry("20",20);
  fSHf32->AddEntry("21",21); fSHf32->AddEntry("22",22);
  fSHf32->AddEntry("23",23); fSHf32->AddEntry("24",24);
  fSHf32->AddEntry("25",25); fSHf32->AddEntry("26",26);
  fSHf32->AddEntry("27",27); fSHf32->AddEntry("28",28);
  fSHf32->AddEntry("29",29); fSHf32->AddEntry("30",30);
  fSHf32->AddEntry("31",31); fSHf32->AddEntry("32",32);
  fSHf32->AddEntry("33",33); fSHf32->AddEntry("34",34);
  fSHf32->AddEntry("35",35); fSHf32->AddEntry("36",36);
  fSHf32->AddEntry("37",37); fSHf32->AddEntry("38",38);
  fSHf32->AddEntry("39",39); fSHf32->AddEntry("40",40);
  fSHf32->AddEntry("41",41); fSHf32->AddEntry("42",42);
  fSHf32->AddEntry("43",43); fSHf32->AddEntry("44",44);
  fSHf32->AddEntry("45",45); fSHf32->AddEntry("46",46);
  fSHf32->AddEntry("47",47); fSHf32->AddEntry("48",48);
  fSHf32->AddEntry("49",49); fSHf32->AddEntry("50",50);
  fSHf32->AddEntry("51",51); fSHf32->AddEntry("52",52);
  fSHf32->AddEntry("53",53); fSHf32->AddEntry("54",54);
  fSHf32->AddEntry("55",55); fSHf32->AddEntry("56",56);
  fSHf32->AddEntry("57",57); fSHf32->AddEntry("58",58);
  fSHf32->AddEntry("59",59); fSHf32->AddEntry("60",60);
  fSHf32->AddEntry("61",61); fSHf32->AddEntry("62",62);
  fSHf32->AddEntry("63",63); fSHf32->AddEntry("64",64);
  fSHf32->Select(1);
  fSHf32->Resize(50,fSHf31->GetDefaultHeight());
  fSHf32->Associate(this);
  fch=1;

  fSHf62 = new TGComboBox(fSub2,kCB_HISTOS_REMAIN);
  fSHf62->AddEntry("1", 1); fSHf62->AddEntry("10", 10);
  fSHf62->AddEntry("20", 20); fSHf62->AddEntry("50", 50);
  fSHf62->Select(1);
  fSHf62->Resize(50,fSHf61->GetDefaultHeight());
  fSHf62->Associate(this);
  fRemain=1;


  //fSHTb42 = new TGTextBuffer(1034);
  //fSHf42 = new TGTextEntry(fSub2,fSHTb42,kTE_HISTOS_DACCHAN);
  //  fSHf42->Resize(50,fSHf41->GetDefaultHeight());
  //  fSHTb42->Clear();
  //  sprintf(value,"%d",control->DACChannel);
  //  fSHTb42->AddText(0,value);
  //  fSHf42->Associate(this);


  fSHTb52 = new TGTextBuffer(1034);
  fSHf52 = new TGTextEntry(fSub2,fSHTb52,kTE_HISTOS_DACVAL);
  fSHf52->Resize(50,fSHf51->GetDefaultHeight());
  fSHTb52->Clear();
  sprintf(value,"%d",control->DACValue);
  fSHTb52->AddText(0,value);
  fSHf52->Associate(this);

  TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsLeft,0,0,0,0);//-15);

  fSub1->AddFrame(fSHf11);  fSub2->AddFrame(fSHf12); 
  //fSub1->AddFrame(fSHf71);  fSub2->AddFrame(fSHf72); 
  fSub1->AddFrame(fSHf21);  fSub2->AddFrame(fSHf22); 
  fSub1->AddFrame(fSHf31);  fSub2->AddFrame(fSHf32); 
  fSub1->AddFrame(fSHf61);  fSub2->AddFrame(fSHf62); 
  //fSub1->AddFrame(fSHf41);  fSub2->AddFrame(fSHf42); 
  fSub1->AddFrame(fSHf51);  fSub2->AddFrame(fSHf52); 
  myhframe->AddFrame(fSub1);
  myhframe->AddFrame(fSub2);//,fSubLay);
  fSelectHisto->AddFrame(myhframe, new TGLayoutHints(kLHintsLeft,2,2,10,2));

}


void MainFrame::HistoOptions(){
  UInt_t fOpt = kVerticalFrame | kFixedWidth;
  fHistoOptions = new TGGroupFrame(fDisplay,"Histogram Options",fOpt);//,ffontcon[2],ffont[2]);
  fHistoOptions->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());
  fHistoOptions->SetWidth(300);

  TGHorizontalFrame *myhframe=new TGHorizontalFrame(fHistoOptions);
  TGVerticalFrame *myvframe=new TGVerticalFrame(myhframe);

  fHOf11 = new TGLabel(myvframe,"First VA:");//,ffontcon[2],ffont[2]);
  fHOf11->SetTextFont(DefineFonts::fFont[2]);
  fHOf11->Resize(85,fHOf11->GetDefaultHeight());
  fHOf11->SetTextJustify(kTextLeft);
  fHOf21 = new TGLabel(myvframe,"Last VA:");//,ffontcon[2],ffont[2]);
  fHOf21->SetTextFont(DefineFonts::fFont[2]);
  fHOf21->Resize(85,fHOf21->GetDefaultHeight());
  fHOf21->SetTextJustify(kTextLeft);

  myvframe->AddFrame(fHOf11, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myvframe->AddFrame(fHOf21, new TGLayoutHints(kLHintsExpandX,0,0,0,0));


  TGVerticalFrame *myvframe2=new TGVerticalFrame(myhframe);

  ffirstva=1;  
  fHOf12 = new TGComboBox(myvframe2,kCB_HISTOS_FIRST_VA);
  for (int i=1; i<17; i++) fHOf12->AddEntry(Form("%02d",i), i);
  fHOf12->Select(ffirstva);
  fHOf12->Resize(50,fHOf11->GetDefaultHeight());
  fHOf12->Associate(this);

  flastva=16;
  fHOf22 = new TGComboBox(myvframe2,kCB_HISTOS_LAST_VA);
  for (int i=1; i<17; i++) fHOf22->AddEntry(Form("%02d",i), i);
  fHOf22->Select(flastva);
  fHOf22->Resize(50,fHOf21->GetDefaultHeight());
  fHOf22->Associate(this);

  myvframe2->AddFrame(fHOf12, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myvframe2->AddFrame(fHOf22, new TGLayoutHints(kLHintsExpandX,0,0,0,0));


  TGVerticalFrame *myvframe3=new TGVerticalFrame(myhframe);

  fHOf13 = new TGLabel(myvframe3,"First Ch:");//,ffontcon[2],ffont[2]);
  fHOf13->SetTextFont(DefineFonts::fFont[2]);
  fHOf13->Resize(85,fHOf13->GetDefaultHeight());
  fHOf13->SetTextJustify(kTextLeft);
  fHOf23 = new TGLabel(myvframe3,"Last Ch: ");//,ffontcon[2],ffont[2]);
  fHOf23->SetTextFont(DefineFonts::fFont[2]);
  fHOf23->Resize(85,fHOf23->GetDefaultHeight());
  fHOf23->SetTextJustify(kTextLeft);

  myvframe3->AddFrame(fHOf13, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myvframe3->AddFrame(fHOf23, new TGLayoutHints(kLHintsExpandX,0,0,0,0));


  TGVerticalFrame *myvframe4=new TGVerticalFrame(myhframe);

  ffirstch=1;
  fHOf14 = new TGComboBox(myvframe4,kCB_HISTOS_FIRST_CHANNEL);
  for (int i=1; i<65; i++)   fHOf14->AddEntry(Form("%02d",i), i);
  fHOf14->Select(ffirstch);
  fHOf14->Resize(50,fHOf13->GetDefaultHeight());
  fHOf14->Associate(this);
  flastch=64;
  fHOf24 = new TGComboBox(myvframe4,kCB_HISTOS_LAST_CHANNEL);
  for (int i=1; i<65; i++)   fHOf24->AddEntry(Form("%02d",i), i);
  fHOf24->Select(flastch);
  fHOf24->Resize(50,fHOf23->GetDefaultHeight());
  fHOf24->Associate(this);

  myvframe4->AddFrame(fHOf14, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myvframe4->AddFrame(fHOf24, new TGLayoutHints(kLHintsExpandX,0,0,0,0));

  myhframe->AddFrame(myvframe, new TGLayoutHints(kLHintsExpandX,2,2,0,0));
  myhframe->AddFrame(myvframe2, new TGLayoutHints(kLHintsExpandX,2,8,0,0));
  myhframe->AddFrame(myvframe3, new TGLayoutHints(kLHintsExpandX,8,2,0,0));
  myhframe->AddFrame(myvframe4, new TGLayoutHints(kLHintsExpandX,2,2,0,0));



  TGHorizontalFrame *myhframe2=new TGHorizontalFrame(fHistoOptions);
  TGVerticalFrame *myvframe5=new TGVerticalFrame(myhframe2);

  TGHorizontalFrame *myhframe3=new TGHorizontalFrame(myvframe5);
  fHOf31 = new TGLabel(myhframe3,"Min X:");//,ffontcon[2],ffont[2]);
  fHOf31->SetTextFont(DefineFonts::fFont[2]);
  fHOf31->Resize(140,fHOf31->GetDefaultHeight());
  fHOf31->SetTextJustify(kTextLeft); 
  char value[200];
  fHOTb32 = new TGTextBuffer(1034);
  fHOf32 = new TGTextEntry(myhframe3,fHOTb32,kTE_HISTOS_MINX);
  //  fHOf32->SetFont(ffont[4],kTRUE);
  fHOf32->Resize(50,fHOf31->GetDefaultHeight());
  fHOTb32->Clear();
  sprintf(value,"%5.1f",fMinimumX);
  fHOTb32->AddText(0,value);
  fHOf32->Associate(this);
  unsigned int bheight=20;
  fHOf33 = new TGTextButton(myhframe3,"x2",kTB_HISTOS_MINX_MUL2);
  fHOf33->ChangeOptions(fHOf33->GetOptions() | kFixedSize); 
  fHOf33->Resize(20,bheight);
  fHOf33->Associate(this);
  fHOf34 = new TGTextButton(myhframe3,"/2",kTB_HISTOS_MINX_DIV2);
  fHOf34->ChangeOptions(fHOf34->GetOptions() | kFixedSize); 
  fHOf34->Resize(20,bheight);
  fHOf34->Associate(this);

  myhframe3->AddFrame(fHOf31, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myhframe3->AddFrame(fHOf32, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myhframe3->AddFrame(fHOf33, new TGLayoutHints(kLHintsLeft,0,0,0,0));
  myhframe3->AddFrame(fHOf34, new TGLayoutHints(kLHintsLeft,0,0,0,0));

  TGHorizontalFrame *myhframe4=new TGHorizontalFrame(myvframe5);
  fHOf41 = new TGLabel(myhframe4,"Max X: ");//,ffontcon[2],ffont[2]);
  fHOf41->SetTextFont(DefineFonts::fFont[2]);
  fHOf41->Resize(140,fHOf41->GetDefaultHeight());
  fHOf41->SetTextJustify(kTextLeft);
  fHOTb42 = new TGTextBuffer(1034);
  fHOf42 = new TGTextEntry(myhframe4,fHOTb42,kTE_HISTOS_MAXX);
  fHOf42->Resize(50,fHOf41->GetDefaultHeight());
  fHOTb42->Clear();
  sprintf(value,"%5.1f",fMaximumX);
  fHOTb42->AddText(0,value);
  fHOf42->Associate(this);
  fHOf43 = new TGTextButton(myhframe4,"x2",kTB_HISTOS_MAXX_MUL2);
  fHOf43->ChangeOptions(fHOf43->GetOptions() | kFixedSize); 
  fHOf43->Resize(20,bheight);
  fHOf43->Associate(this);
  fHOf44 = new TGTextButton(myhframe4,"/2",kTB_HISTOS_MAXX_DIV2);
  fHOf44->ChangeOptions(fHOf44->GetOptions() | kFixedSize); 
  fHOf44->Resize(20,bheight);
  fHOf44->Associate(this);

  myhframe4->AddFrame(fHOf41, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myhframe4->AddFrame(fHOf42, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myhframe4->AddFrame(fHOf43, new TGLayoutHints(kLHintsLeft,0,0,0,0));
  myhframe4->AddFrame(fHOf44, new TGLayoutHints(kLHintsLeft,0,0,0,0));


  TGHorizontalFrame *myhframe5=new TGHorizontalFrame(myvframe5);
  fHOf51 = new TGLabel(myhframe5,"Min Y:");//,ffontcon[2],ffont[2]);
  fHOf51->SetTextFont(DefineFonts::fFont[2]);
  fHOf51->Resize(140,fHOf51->GetDefaultHeight());
  fHOf51->SetTextJustify(kTextLeft);
  fHOTb52 = new TGTextBuffer(1034);
  fHOf52 = new TGTextEntry(myhframe5,fHOTb52,kTE_HISTOS_MINY);
  fHOf52->Resize(50,fHOf51->GetDefaultHeight());
  fHOTb52->Clear();
  sprintf(value,"%5.1f",fMinimumY);
  fHOTb52->AddText(0,value);
  fHOf52->Associate(this);
  fHOf53 = new TGTextButton(myhframe5,"x2",kTB_HISTOS_MINY_MUL2);
  fHOf53->ChangeOptions(fHOf53->GetOptions() | kFixedSize); 
  fHOf53->Resize(20,bheight);
  fHOf53->Associate(this);
  fHOf54 = new TGTextButton(myhframe5,"/2",kTB_HISTOS_MINY_DIV2); 
  fHOf54->ChangeOptions(fHOf54->GetOptions() | kFixedSize); 
  fHOf54->Resize(20,bheight);
  fHOf54->Associate(this);

  myhframe5->AddFrame(fHOf51, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myhframe5->AddFrame(fHOf52, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myhframe5->AddFrame(fHOf53, new TGLayoutHints(kLHintsLeft,0,0,0,0));
  myhframe5->AddFrame(fHOf54, new TGLayoutHints(kLHintsLeft,0,0,0,0));


  TGHorizontalFrame *myhframe6=new TGHorizontalFrame(myvframe5);
  fHOf61 = new TGLabel(myhframe6,"Max Y:");//,ffontcon[2],ffont[2]);
  fHOf61->SetTextFont(DefineFonts::fFont[2]);
  fHOf61->Resize(140,fHOf61->GetDefaultHeight());
  fHOf61->SetTextJustify(kTextLeft);
  fHOTb62 = new TGTextBuffer(1034);
  fHOf62 = new TGTextEntry(myhframe6,fHOTb62,kTE_HISTOS_MAXY);
  fHOf62->Resize(50,fHOf61->GetDefaultHeight());
  fHOTb62->Clear();
  sprintf(value,"%5.1f",fMaximumY);
  fHOTb62->AddText(0,value);
  fHOf62->Associate(this);
  fHOf63 = new TGTextButton(myhframe6,"x2",kTB_HISTOS_MAXY_MUL2);
  fHOf63->ChangeOptions(fHOf63->GetOptions() | kFixedSize); 
  fHOf63->Resize(20,bheight);
  fHOf63->Associate(this);
  fHOf64 = new TGTextButton(myhframe6,"/2",kTB_HISTOS_MAXY_DIV2);
  fHOf64->ChangeOptions(fHOf64->GetOptions() | kFixedSize); 
  fHOf64->Resize(20,bheight);
  fHOf64->Associate(this);

  myhframe6->AddFrame(fHOf61, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myhframe6->AddFrame(fHOf62, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myhframe6->AddFrame(fHOf63, new TGLayoutHints(kLHintsLeft,0,0,0,0));
  myhframe6->AddFrame(fHOf64, new TGLayoutHints(kLHintsLeft,0,0,0,0));

  myvframe5->AddFrame(myhframe3, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myvframe5->AddFrame(myhframe4, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myvframe5->AddFrame(myhframe5, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  myvframe5->AddFrame(myhframe6, new TGLayoutHints(kLHintsExpandX,0,0,0,0));


  TGVerticalFrame *myvframe6=new TGVerticalFrame(myhframe2);
  TGHorizontalFrame *myhframe7=new TGHorizontalFrame(myvframe6);
  fHOf45=new TGLabel(myhframe7,"DynPed:");//,ffontcon[2],ffont[2]);
  fHOf45->SetTextFont(DefineFonts::fFont[2]);
  fHOf45->Resize(fHOf45->GetDefaultWidth()*1.5,fHOf41->GetDefaultHeight());
  fChk4 = new TGCheckButton(myhframe7 , "", kCHB_DYN_PED);//,ffontcon[2],ffont[2]);
  fChk4->SetFont(DefineFonts::fFont[2]->GetFontStruct());
  fChk4->Resize(140,fHOf41->GetDefaultHeight());
  fChk4->SetState((control->DynPed)?kButtonDown:kButtonUp);
  fChk4->Associate(this);

  myhframe7->AddFrame(fHOf45, new TGLayoutHints(kLHintsLeft,0,0,0,0));
  myhframe7->AddFrame(fChk4, new TGLayoutHints(kLHintsRight,0,0,0,0));


  TGHorizontalFrame *myhframe8=new TGHorizontalFrame(myvframe6);
  fHOf55=new TGLabel(myhframe8,"Record:");//,ffontcon[2],ffont[2]);
  fHOf55->SetTextFont(DefineFonts::fFont[2]);
  fHOf55->Resize(fHOf55->GetDefaultWidth()*1.5,fHOf51->GetDefaultHeight());
  fChk3 = new TGCheckButton(myhframe8, "", kCHB_RECORD_GIF);//,ffontcon[2],ffont[2]);
  fChk3->SetFont(DefineFonts::fFont[2]->GetFontStruct());
  fChk3->Resize(140,fHOf51->GetDefaultHeight());
  fChk3->SetState((control->RecordGifMovie)?kButtonDown:kButtonUp);
  fChk3->Associate(this);

  myhframe8->AddFrame(fHOf55, new TGLayoutHints(kLHintsLeft,0,0,0,0));
  myhframe8->AddFrame(fChk3, new TGLayoutHints(kLHintsRight,0,0,0,0));


  TGHorizontalFrame *myhframe9=new TGHorizontalFrame(myvframe6);
  fHOf65=new TGLabel(myhframe9,"Show bad:");//,ffontcon[2],ffont[2]);
  fHOf65->SetTextFont(DefineFonts::fFont[2]);
  fHOf65->Resize(fHOf65->GetDefaultWidth()*1.5,fHOf61->GetDefaultHeight());
  fChk1 = new TGCheckButton(myhframe9, "", kCHB_SHOW_BAD);//,ffontcon[2],ffont[2]);
  fChk1->SetFont(DefineFonts::fFont[2]->GetFontStruct());
  fChk1->Resize(140,fHOf61->GetDefaultHeight());
  fChk1->SetState((control->ShowBadChannels)?kButtonDown:kButtonUp);
  fChk1->Associate(this);

  myhframe9->AddFrame(fHOf65, new TGLayoutHints(kLHintsLeft,0,0,0,0));
  myhframe9->AddFrame(fChk1, new TGLayoutHints(kLHintsRight,0,0,0,0));


  myvframe6->AddFrame(myhframe7, new TGLayoutHints(kLHintsRight,0,0,0,0));
  myvframe6->AddFrame(myhframe8, new TGLayoutHints(kLHintsRight,0,0,0,0));
  myvframe6->AddFrame(myhframe9, new TGLayoutHints(kLHintsRight,0,0,0,0));

  myhframe2->AddFrame(myvframe5, new TGLayoutHints(kLHintsLeft,0,0,0,0));
  myhframe2->AddFrame(myvframe6, new TGLayoutHints(kLHintsRight,0,0,0,0));
  
  fHistoOptions->AddFrame(myhframe,  new TGLayoutHints(kLHintsExpandX,0,0,10,0));
  fHistoOptions->AddFrame(myhframe2,  new TGLayoutHints(kLHintsExpandX,0,0,10,0));



  /*
  fOpt = kVerticalFrame | kFixedWidth; 
  TGCompositeFrame *fFrame2 = new TGCompositeFrame(fHistoOptions,275,0,fOpt);

  fOpt = kHorizontalFrame; 
  char value[200];
  TGCompositeFrame *fSub5 = new TGCompositeFrame(fFrame2,180,0,fOpt);
  TGCompositeFrame *fLab5 = new TGCompositeFrame(fSub5,55,0,kFixedWidth);
  //  fSub5->SetLayoutManager(new TGMatrixLayout(fSub5,0,1,2,kLHintsLeft));
  fHOf31 = new TGLabel(fLab5,"Min X:");//,ffontcon[2],ffont[2]);
  fHOf31->SetTextFont(DefineFonts::fFont[2]);
  fHOf31->Resize(140,fHOf31->GetDefaultHeight());
  fHOf31->SetTextJustify(kTextLeft);
  fHOTb32 = new TGTextBuffer(1034);
  fHOf32 = new TGTextEntry(fSub5,fHOTb32,kTE_HISTOS_MINX);
  //  fHOf32->SetFont(ffont[4],kTRUE);
  fHOf32->Resize(50,fHOf31->GetDefaultHeight());
  fHOTb32->Clear();
  sprintf(value,"%5.1f",fMinimumX);
  fHOTb32->AddText(0,value);
  fHOf32->Associate(this);
  fHOf33 = new TGTextButton(fSub5,"x2",kTB_HISTOS_MINX_MUL2);
  //  fHOf33->SetFont(ffont[4]);
  //    fHOf33->SetTextJustify(kTextLeft | kTextBottom);
  //    fHOf33->ChangeBackground(fGreen);
  //    fHOf33->SetState(kButtonUp);
  fHOf33->Resize(10,0.5*fHOf31->GetDefaultHeight());
  //  fHOf33->SetHeight(fHOf31->GetDefaultHeight());
  fHOf33->Associate(this);
  fHOf34 = new TGTextButton(fSub5,"/2",kTB_HISTOS_MINX_DIV2);
  //fHOf34->SetFont(ffont[4]);
  //    fHOf34->ChangeBackground(fGreen);
  //    fHOf34->SetTextJustify(kTextLeft);
  //    fHOf34->SetState(kButtonUp);
  fHOf34->Resize(20,0.8*fHOf31->GetDefaultHeight());
  fHOf34->Associate(this);



  TGCompositeFrame *fSub6 = new TGCompositeFrame(fFrame2,280,0,fOpt); 
  TGCompositeFrame *fLab6 = new TGCompositeFrame(fSub6,55,0,kFixedWidth);
  //  fSub6->SetLayoutManager(new TGMatrixLayout(fLab6,0,1,2,kLHintsLeft));
  fHOf41 = new TGLabel(fLab6,"Max X: ");//,ffontcon[2],ffont[2]);
  fHOf41->SetTextFont(DefineFonts::fFont[2]);
  fHOf41->Resize(140,fHOf41->GetDefaultHeight());
  fHOf41->SetTextJustify(kTextLeft);
  fHOTb42 = new TGTextBuffer(1034);
  fHOf42 = new TGTextEntry(fSub6,fHOTb42,kTE_HISTOS_MAXX);
  fHOf42->Resize(50,fHOf41->GetDefaultHeight());
  fHOTb42->Clear();
  sprintf(value,"%5.1f",fMaximumX);
  fHOTb42->AddText(0,value);
  fHOf42->Associate(this);
  fHOf43 = new TGTextButton(fSub6,"x2",kTB_HISTOS_MAXX_MUL2);
  fHOf43->Resize(20,0.8*fHOf41->GetDefaultHeight());
  fHOf43->Associate(this);
  fHOf44 = new TGTextButton(fSub6,"/2",kTB_HISTOS_MAXX_DIV2);
  //    fHOf44->ChangeBackground(fGreen);
  //    fHOf44->SetTextJustify(kTextLeft);
  //    fHOf44->SetState(kButtonUp);
  fHOf44->Resize(20,0.8*fHOf41->GetDefaultHeight());
  fHOf44->Associate(this);

  fHOf45=new TGLabel(fSub6,"       DynPed:");//,ffontcon[2],ffont[2]);
  fHOf45->SetTextFont(DefineFonts::fFont[2]);
  fHOf45->Resize(fHOf45->GetDefaultWidth()*1.5,fHOf41->GetDefaultHeight());
  fChk4 = new TGCheckButton(fSub6, "", kCHB_DYN_PED);//,ffontcon[2],ffont[2]);
  fChk4->SetFont(DefineFonts::fFont[2]->GetFontStruct());
  fChk4->Resize(140,fHOf41->GetDefaultHeight());
  fChk4->SetState((control->DynPed)?kButtonDown:kButtonUp);
  fChk4->Associate(this);

  TGCompositeFrame *fSub7 = new TGCompositeFrame(fFrame2,280,0,fOpt);
  TGCompositeFrame *fLab7 = new TGCompositeFrame(fSub7,55,0,kFixedWidth);
  //  fSub7->SetLayoutManager(new TGMatrixLayout(fSub7,0,1,2,kLHintsLeft));
  fHOf51 = new TGLabel(fLab7,"Min Y:");//,ffontcon[2],ffont[2]);
  fHOf51->SetTextFont(DefineFonts::fFont[2]);
  fHOf51->Resize(140,fHOf51->GetDefaultHeight());
  fHOf51->SetTextJustify(kTextLeft);
  fHOTb52 = new TGTextBuffer(1034);
  fHOf52 = new TGTextEntry(fSub7,fHOTb52,kTE_HISTOS_MINY);
  fHOf52->Resize(50,fHOf51->GetDefaultHeight());
  fHOTb52->Clear();
  sprintf(value,"%5.1f",fMinimumY);
  fHOTb52->AddText(0,value);
  fHOf52->Associate(this);
  fHOf53 = new TGTextButton(fSub7,"x2",kTB_HISTOS_MINY_MUL2);
  //    fHOf53->SetTextJustify(kTextLeft | kTextBottom);
  //    fHOf53->ChangeBackground(fGreen);
  //    fHOf53->SetState(kButtonUp);
  fHOf53->Resize(20,0.8*fHOf51->GetDefaultHeight());
  fHOf53->Associate(this);
  fHOf54 = new TGTextButton(fSub7,"/2",kTB_HISTOS_MINY_DIV2); 
  //    fHOf54->ChangeBackground(fGreen);
  //    fHOf54->SetTextJustify(kTextLeft);
  //    fHOf54->SetState(kButtonUp);
  fHOf54->Resize(20,0.8*fHOf51->GetDefaultHeight());
  fHOf54->Associate(this);

  fHOf55=new TGLabel(fSub7,"       Record:");//,ffontcon[2],ffont[2]);
  fHOf55->SetTextFont(DefineFonts::fFont[2]);
  fHOf55->Resize(fHOf55->GetDefaultWidth()*1.5,fHOf51->GetDefaultHeight());
  fChk3 = new TGCheckButton(fSub7, "", kCHB_RECORD_GIF);//,ffontcon[2],ffont[2]);
  fChk3->SetFont(DefineFonts::fFont[2]->GetFontStruct());
  fChk3->Resize(140,fHOf51->GetDefaultHeight());
  fChk3->SetState((control->RecordGifMovie)?kButtonDown:kButtonUp);
  fChk3->Associate(this);


  TGCompositeFrame *fSub8 = new TGCompositeFrame(fFrame2,280,0,fOpt);
  TGCompositeFrame *fLab8 = new TGCompositeFrame(fSub8,55,0,kFixedWidth);
  //  fSub8->SetLayoutManager(new TGMatrixLayout(fSub8,0,1,2,kLHintsLeft));
  fHOf61 = new TGLabel(fLab8,"Max Y:       ");//,ffontcon[2],ffont[2]);
  fHOf61->SetTextFont(DefineFonts::fFont[2]);
  fHOf61->Resize(140,fHOf61->GetDefaultHeight());
  fHOf61->SetTextJustify(kTextLeft);
  fHOTb62 = new TGTextBuffer(1034);
  fHOf62 = new TGTextEntry(fSub8,fHOTb62,kTE_HISTOS_MAXY);
  fHOf62->Resize(50,fHOf61->GetDefaultHeight());
  fHOTb62->Clear();
  sprintf(value,"%5.1f",fMaximumY);
  fHOTb62->AddText(0,value);
  fHOf62->Associate(this);
  fHOf63 = new TGTextButton(fSub8,"x2",kTB_HISTOS_MAXY_MUL2);
  //    fHOf63->SetTextJustify(kTextLeft | kTextBottom);
  //    fHOf63->ChangeBackground(fGreen);
  //    fHOf63->SetState(kButtonUp);
  fHOf63->Resize(20,0.8*fHOf61->GetDefaultHeight());
  fHOf63->Associate(this);
  fHOf64 = new TGTextButton(fSub8,"/2",kTB_HISTOS_MAXY_DIV2);
  //    fHOf64->ChangeBackground(fGreen);
  //    fHOf64->SetTextJustify(kTextLeft);
  //    fHOf64->SetState(kButtonUp);
  fHOf64->Resize(20,0.8*fHOf61->GetDefaultHeight());
  fHOf64->Associate(this);
  fHOf65=new TGLabel(fSub8,"    Show bad:");//,ffontcon[2],ffont[2]);
  fHOf65->SetTextFont(DefineFonts::fFont[2]);
  fHOf65->Resize(fHOf65->GetDefaultWidth()*1.5,fHOf61->GetDefaultHeight());
  fChk1 = new TGCheckButton(fSub8, "", kCHB_SHOW_BAD);//,ffontcon[2],ffont[2]);
  fChk1->SetFont(DefineFonts::fFont[2]->GetFontStruct());
  fChk1->Resize(140,fHOf61->GetDefaultHeight());
  fChk1->SetState((control->ShowBadChannels)?kButtonDown:kButtonUp);
  fChk1->Associate(this);


  TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsLeft,0,0,0,-15);
  TGLayoutHints *fLay0 = new TGLayoutHints(kLHintsLeft|kLHintsExpandX,1,1,1,1);
  TGLayoutHints *fLay1 = new TGLayoutHints(kLHintsLeft,1,1,1,1);
  TGLayoutHints *fLay11 = new TGLayoutHints(kLHintsTop,1,1,1,1);

  //fSub1->AddFrame(fHOf11); fSub2->AddFrame(fHOf12);  
  //fSub3->AddFrame(fHOf13); fSub4->AddFrame(fHOf14);
  //fSub1->AddFrame(fHOf21); fSub2->AddFrame(fHOf22); 
  //fSub3->AddFrame(fHOf23); fSub4->AddFrame(fHOf24);
  //fFrame1->AddFrame(fSub1);
  //fFrame1->AddFrame(fSub2);
  //fFrame1->AddFrame(fSub3);
  //fFrame1->AddFrame(fSub4,fSubLay);

  fLab5->AddFrame(fHOf31);
  fLab6->AddFrame(fHOf41);
  fLab7->AddFrame(fHOf51);
  fLab8->AddFrame(fHOf61);
  fSub5->AddFrame(fLab5,fLay0);  fSub5->AddFrame(fHOf32,fLay1);
  fSub5->AddFrame(fHOf33,fLay1); fSub5->AddFrame(fHOf34,fLay1);
  fSub6->AddFrame(fLab6,fLay0);  fSub6->AddFrame(fHOf42,fLay1);
  fSub6->AddFrame(fHOf43,fLay1); fSub6->AddFrame(fHOf44,fLay1);
  fSub6->AddFrame(fHOf45,fLay1); fSub6->AddFrame(fChk4,fLay1);
  fSub7->AddFrame(fLab7,fLay0);  fSub7->AddFrame(fHOf52,fLay1);
  fSub7->AddFrame(fHOf53,fLay1); fSub7->AddFrame(fHOf54,fLay1);
  fSub7->AddFrame(fHOf55,fLay1); fSub7->AddFrame(fChk3,fLay1);
  fSub8->AddFrame(fLab8,fLay0);  fSub8->AddFrame(fHOf62,fLay1); 
  fSub8->AddFrame(fHOf63,fLay1); fSub8->AddFrame(fHOf64,fLay1);
  fSub8->AddFrame(fHOf65,fLay1); fSub8->AddFrame(fChk1,fLay1);
  fFrame2->AddFrame(fSub5);
  fFrame2->AddFrame(fSub6);
  fFrame2->AddFrame(fSub7);
  fFrame2->AddFrame(fSub8);

  //fHistoOptions->AddFrame(fFrame1);
  fHistoOptions->AddFrame(fFrame2);//,fSubLay);

*/

}



void MainFrame::Canvas(){
  fCanvas = new TRootEmbeddedCanvas("fCanvas-0", fDisplay, 560, 400);
  fCanvas->GetCanvas()->SetBorderMode(0);
  fCanvas->GetCanvas()->Update();

}


void MainFrame::Text(){
  fText = new TGTextView(fWork,950,90,kSunkenFrame | kDoubleBorder);

  //  fText->LoadFile("Log/TDR.log");
  //  fTextCanvas->SetTopLine(fText->GetTextFrame()->GetLines() -
  //				    fText->GetTextFrame()->GetVisibleLines());
}


