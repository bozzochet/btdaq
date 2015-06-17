#include "SetCalPar.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>



SetCalPar::SetCalPar(const TGWindow *p, const TGWindow *main, 
		     UInt_t w, UInt_t h) 
  : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);
  char value[200];

  // Creating share memory segments
  char file[200];
#include "shm_definition.code"


  fMain=main;
  UInt_t fSCPOpt = kVerticalFrame;
  fSCP = new TGCompositeFrame(this,300,0,fSCPOpt);
 
  TGLayoutHints *fSCPNameLay = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
  TGLabel *fSCPName = new TGLabel(fSCP,"Calibration Parameters");//,ffontcon[1],ffont[1]);
  fSCPName->SetTextFont(DefineFonts::fFont[1]->GetFontStruct());

  char Title[50];
  UInt_t fSCPPassOpt   = kHorizontalFrame;
  Int_t  fSCPPassHints = kLHintsLeft | kLHintsExpandX;

  strcpy(Title,"Pass 0 - Skip");
  fSCPPass0 = new TGGroupFrame(fSCP,Title,fSCPPassOpt);//,ffontcon[2],ffont[2]);
  fSCPPass0->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());
  TGCompositeFrame *fSub01 = new TGCompositeFrame(fSCPPass0,150,0,fSCPPassOpt);
  fSub01->SetLayoutManager(new TGMatrixLayout(fSub01,0,1,2,fSCPPassHints));
  TGCompositeFrame *fSub02 = new TGCompositeFrame(fSCPPass0,100,0,fSCPPassOpt);
  fSub02->SetLayoutManager(new TGMatrixLayout(fSub02,0,1,2,fSCPPassHints));

  fSCP0f11  = new TGLabel(fSub01,"# Events           ");//,ffontcon[2],ffont[2]);
  fSCP0f11->SetTextFont(DefineFonts::fFont[2]);
  fSCP0f11->Resize(200,fSCP0f11->GetDefaultHeight());
  fSCP0f11->SetTextJustify(kTextLeft);
  fSCP0Tb12 = new TGTextBuffer(1034);
  fSCP0f12 = new TGTextEntry(fSub02,fSCP0Tb12);
  fSCP0f12->Resize(80,fSCP0f11->GetDefaultHeight());
  fSCP0Tb12->Clear();
  sprintf(value,"%d",control->EventsStep[0]);
  fSCP0Tb12->AddText(0,value);
  fSCP0f12->Associate(this);






  strcpy(Title,"Pass 1 - Pedestal");
  fSCPPass1 = new TGGroupFrame(fSCP,Title,fSCPPassOpt);//,ffontcon[2],ffont[2]);
  fSCPPass1->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());
  TGCompositeFrame *fSub11 = new TGCompositeFrame(fSCPPass1,150,0,fSCPPassOpt);
  fSub11->SetLayoutManager(new TGMatrixLayout(fSub11,0,1,2,fSCPPassHints));
  TGCompositeFrame *fSub12 = new TGCompositeFrame(fSCPPass1,100,0,fSCPPassOpt);
  fSub12->SetLayoutManager(new TGMatrixLayout(fSub12,0,1,2,fSCPPassHints));

  fSCP1f11  = new TGLabel(fSub11,"# Events           ");//,ffontcon[2],ffont[2]);
  fSCP1f11->SetTextFont(DefineFonts::fFont[2]);
  fSCP1f11->Resize(200,fSCP1f11->GetDefaultHeight());
  fSCP1f11->SetTextJustify(kTextLeft);
  fSCP1Tb12 = new TGTextBuffer(1034);
  fSCP1f12 = new TGTextEntry(fSub12,fSCP1Tb12);
  fSCP1f12->Resize(80,fSCP1f11->GetDefaultHeight());
  fSCP1Tb12->Clear();
  sprintf(value,"%d",control->EventsStep[1]);
  fSCP1Tb12->AddText(0,value);
  fSCP1f12->Associate(this);



  strcpy(Title,"Pass 2 - SigmaRaw");
  fSCPPass2 = new TGGroupFrame(fSCP,Title,fSCPPassOpt);//,ffontcon[2],ffont[2]);
  fSCPPass2->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());
  TGCompositeFrame *fSub21 = new TGCompositeFrame(fSCPPass2,150,0,fSCPPassOpt);
  fSub21->SetLayoutManager(new TGMatrixLayout(fSub21,0,1,2,fSCPPassHints));
  TGCompositeFrame *fSub22 = new TGCompositeFrame(fSCPPass2,100,0,fSCPPassOpt);
  fSub22->SetLayoutManager(new TGMatrixLayout(fSub22,0,1,2,fSCPPassHints));

  fSCP2f11  = new TGLabel(fSub21,"# Events           ");//,ffontcon[2],ffont[2]);
  fSCP2f11->SetTextFont(DefineFonts::fFont[2]);
  fSCP2f11->Resize(200,fSCP2f11->GetDefaultHeight());
  fSCP2f11->SetTextJustify(kTextLeft);
  fSCP2Tb12 = new TGTextBuffer(1034);
  fSCP2f12 = new TGTextEntry(fSub22,fSCP2Tb12);
  fSCP2f12->Resize(80,fSCP2f11->GetDefaultHeight());
  fSCP2Tb12->Clear();
  sprintf(value,"%d",control->EventsStep[2]);
  fSCP2Tb12->AddText(0,value);
  fSCP2f12->Associate(this);

  fSCP2f21  = new TGLabel(fSub21,"Sigma Cut      ");// ,ffontcon[2],ffont[2]);
  fSCP2f21->SetTextFont(DefineFonts::fFont[2]);
  fSCP2f21->Resize(200,fSCP2f21->GetDefaultHeight());
  fSCP2f21->SetTextJustify(kTextLeft);
  fSCP2Tb22 = new TGTextBuffer(1034);
  fSCP2f22 = new TGTextEntry(fSub22,fSCP2Tb22);
  fSCP2f22->Resize(80,fSCP2f21->GetDefaultHeight());
  fSCP2Tb22->Clear();
  sprintf(value,"%5.2f",control->SRSigmaCut);
  fSCP2Tb22->AddText(0,value);
  fSCP2f22->Associate(this);

  fSCP2f31  = new TGLabel(fSub21,"N. bins");// ,ffontcon[2],ffont[2]);
  fSCP2f31->SetTextFont(DefineFonts::fFont[2]);
  fSCP2f31->Resize(200,fSCP2f31->GetDefaultHeight());
  fSCP2f31->SetTextJustify(kTextLeft);
  fSCP2Tb32 = new TGTextBuffer(1034);
  fSCP2f32 = new TGTextEntry(fSub22,fSCP2Tb32);
  fSCP2f32->Resize(80,fSCP2f31->GetDefaultHeight());
  fSCP2Tb32->Clear();
  sprintf(value,"%d",control->SRBin);
  fSCP2Tb32->AddText(0,value);
  fSCP2f32->Associate(this);

  fSCP2f41  = new TGLabel(fSub21,"Low noise cut");// ,ffontcon[2],ffont[2]);
  fSCP2f41->SetTextFont(DefineFonts::fFont[2]);
  fSCP2f41->Resize(200,fSCP2f41->GetDefaultHeight());
  fSCP2f41->SetTextJustify(kTextLeft);
  fSCP2Tb42 = new TGTextBuffer(1034);
  fSCP2f42 = new TGTextEntry(fSub22,fSCP2Tb42);
  fSCP2f42->Resize(80,fSCP2f41->GetDefaultHeight());
  fSCP2Tb42->Clear();
  sprintf(value,"%5.1f",control->SRLow);
  fSCP2Tb42->AddText(0,value);
  fSCP2f42->Associate(this);

  fSCP2f51  = new TGLabel(fSub21,"High noise cut");// ,ffontcon[2],ffont[2]);
  fSCP2f51->SetTextFont(DefineFonts::fFont[2]);
  fSCP2f51->Resize(200,fSCP2f51->GetDefaultHeight());
  fSCP2f51->SetTextJustify(kTextLeft);
  fSCP2Tb52 = new TGTextBuffer(1034);
  fSCP2f52 = new TGTextEntry(fSub22,fSCP2Tb52);
  fSCP2f52->Resize(80,fSCP2f51->GetDefaultHeight());
  fSCP2Tb52->Clear();
  sprintf(value,"%5.1f",control->SRHigh);
  fSCP2Tb52->AddText(0,value);
  fSCP2f52->Associate(this);


  strcpy(Title,"Pass 3 - Sigma");
  fSCPPass3 = new TGGroupFrame(fSCP,Title,fSCPPassOpt);//,ffontcon[2],ffont[2]);
  fSCPPass3->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());
  TGCompositeFrame *fSub31 = new TGCompositeFrame(fSCPPass3,150,0,fSCPPassOpt);
  fSub31->SetLayoutManager(new TGMatrixLayout(fSub31,0,1,2,fSCPPassHints));
  TGCompositeFrame *fSub32 = new TGCompositeFrame(fSCPPass3,100,0,fSCPPassOpt);
  fSub32->SetLayoutManager(new TGMatrixLayout(fSub32,0,1,2,fSCPPassHints));

  fSCP3f11  = new TGLabel(fSub31,"# Events           ");//,ffontcon[2],ffont[2]);
  fSCP3f11->SetTextFont(DefineFonts::fFont[2]);
  fSCP3f11->Resize(200,fSCP3f11->GetDefaultHeight());
  fSCP3f11->SetTextJustify(kTextLeft);
  fSCP3Tb12 = new TGTextBuffer(1034);
  fSCP3f12 = new TGTextEntry(fSub32,fSCP3Tb12);
  fSCP3f12->Resize(80,fSCP3f11->GetDefaultHeight());
  fSCP3Tb12->Clear();
  sprintf(value,"%d",control->EventsStep[3]);
  fSCP3Tb12->AddText(0,value);
  fSCP3f12->Associate(this);

  fSCP3f21  = new TGLabel(fSub31,"Low noise cut");// ,ffontcon[2],ffont[2]);
  fSCP3f21->SetTextFont(DefineFonts::fFont[2]);
  fSCP3f21->Resize(200,fSCP3f21->GetDefaultHeight());
  fSCP3f21->SetTextJustify(kTextLeft);
  fSCP3Tb22 = new TGTextBuffer(1034);
  fSCP3f22 = new TGTextEntry(fSub32,fSCP3Tb22);
  fSCP3f22->Resize(80,fSCP3f21->GetDefaultHeight());
  fSCP3Tb22->Clear();
  sprintf(value,"%5.1f",control->SigmaLow);
  fSCP3Tb22->AddText(0,value);
  fSCP3f22->Associate(this);

  fSCP3f31  = new TGLabel(fSub31,"High noise cut");// ,ffontcon[2],ffont[2]);
  fSCP3f31->SetTextFont(DefineFonts::fFont[2]);
  fSCP3f31->Resize(200,fSCP3f31->GetDefaultHeight());
  fSCP3f31->SetTextJustify(kTextLeft);
  fSCP3Tb32 = new TGTextBuffer(1034);
  fSCP3f32 = new TGTextEntry(fSub32,fSCP3Tb32);
  fSCP3f32->Resize(80,fSCP2f51->GetDefaultHeight());
  fSCP3Tb32->Clear();
  sprintf(value,"%5.1f",control->SigmaHigh);
  fSCP3Tb32->AddText(0,value);
  fSCP3f32->Associate(this);




  strcpy(Title,"Pass 4 - Gaussian");
  fSCPPass4 = new TGGroupFrame(fSCP,Title,fSCPPassOpt);//,ffontcon[2],ffont[2]);
  fSCPPass4->SetTextFont(DefineFonts::fFont[2]->GetFontStruct());
  TGCompositeFrame *fSub41 = new TGCompositeFrame(fSCPPass4,150,0,fSCPPassOpt);
  fSub41->SetLayoutManager(new TGMatrixLayout(fSub41,0,1,2,fSCPPassHints));
  TGCompositeFrame *fSub42 = new TGCompositeFrame(fSCPPass4,100,0,fSCPPassOpt);
  fSub42->SetLayoutManager(new TGMatrixLayout(fSub42,0,1,2,fSCPPassHints));

  fSCP4f11  = new TGLabel(fSub41,"# Events           ");//,ffontcon[2],ffont[2]);
  fSCP4f11->SetTextFont(DefineFonts::fFont[2]);
  fSCP4f11->Resize(200,fSCP4f11->GetDefaultHeight());
  fSCP4f11->SetTextJustify(kTextLeft);
  fSCP4Tb12 = new TGTextBuffer(1034);
  fSCP4f12 = new TGTextEntry(fSub42,fSCP4Tb12);
  fSCP4f12->Resize(80,fSCP4f11->GetDefaultHeight());
  fSCP4Tb12->Clear();
  sprintf(value,"%d",control->EventsStep[4]);
  fSCP4Tb12->AddText(0,value);
  fSCP4f12->Associate(this);

  fSCP4f21  = new TGLabel(fSub41,"Sigma Cut  ");//,ffontcon[2],ffont[2]);
  fSCP4f21->SetTextFont(DefineFonts::fFont[2]);
  fSCP4f21->Resize(200,fSCP4f21->GetDefaultHeight());
  fSCP4f21->SetTextJustify(kTextLeft);
  fSCP4Tb22 = new TGTextBuffer(1034);
  fSCP4f22 = new TGTextEntry(fSub42,fSCP4Tb22);
  fSCP4f22->Resize(80,fSCP4f21->GetDefaultHeight());
  fSCP4Tb22->Clear();
  sprintf(value,"%f",control->GASigmaCut);
  fSCP4Tb22->AddText(0,value);
  fSCP4f22->Associate(this);

  fSCP4f31  = new TGLabel(fSub41,"% events out");//,ffontcon[2],ffont[2]);
  fSCP4f31->SetTextFont(DefineFonts::fFont[2]);
  fSCP4f31->Resize(200,fSCP4f31->GetDefaultHeight());
  fSCP4f31->SetTextJustify(kTextLeft);
  fSCP4Tb32 = new TGTextBuffer(1034);
  fSCP4f32 = new TGTextEntry(fSub42,fSCP4Tb32);
  fSCP4f32->Resize(80,fSCP4f31->GetDefaultHeight());
  fSCP4Tb32->Clear();
  sprintf(value,"%f",control->GAOut);
  fSCP4Tb32->AddText(0,value);
  fSCP4f32->Associate(this);



  TGCompositeFrame *fButtons = new TGCompositeFrame(fSCP,200,0,
						    kHorizontalFrame);
  fButtons->SetLayoutManager(new TGMatrixLayout(fButtons,1,0,5,
					     kLHintsLeft));


  fSCPButtonOK     = new TGTextButton(fButtons,"OK"    ,kSCPTB_OK);
  fSCPButtonOK->Resize(60,fSCPButtonOK->GetDefaultHeight());
  fSCPButtonOK->Associate(this);

  fSCPButtonSAVE   = new TGTextButton(fButtons,"SAVE"  ,kSCPTB_SAVE);
  fSCPButtonSAVE->Resize(60,fSCPButtonSAVE->GetDefaultHeight());
  fSCPButtonSAVE->Associate(this);

  fSCPButtonCANCEL = new TGTextButton(fButtons,"CANCEL",kSCPTB_CANCEL);
  fSCPButtonCANCEL->Resize(60,fSCPButtonCANCEL->GetDefaultHeight());
  fSCPButtonCANCEL->Associate(this);



  // Placing everything together
  TGLayoutHints *fLay=new TGLayoutHints(kLHintsLeft | kLHintsExpandX,5,0,5,5);
  TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsLeft,0,0,0,-15);

  fSub01->AddFrame(fSCP0f11);  fSub02->AddFrame(fSCP0f12);    
  fSCPPass0->AddFrame(fSub01,fSubLay); 
  fSCPPass0->AddFrame(fSub02,fSubLay); 

  fSub11->AddFrame(fSCP1f11);  fSub12->AddFrame(fSCP1f12);    
  fSCPPass1->AddFrame(fSub11,fSubLay); 
  fSCPPass1->AddFrame(fSub12,fSubLay); 

  fSub21->AddFrame(fSCP2f11);  fSub22->AddFrame(fSCP2f12);    
  fSub21->AddFrame(fSCP2f21);  fSub22->AddFrame(fSCP2f22);    
  fSub21->AddFrame(fSCP2f31);  fSub22->AddFrame(fSCP2f32);    
  fSub21->AddFrame(fSCP2f41);  fSub22->AddFrame(fSCP2f42);    
  fSub21->AddFrame(fSCP2f51);  fSub22->AddFrame(fSCP2f52);    
  fSCPPass2->AddFrame(fSub21,fSubLay); 
  fSCPPass2->AddFrame(fSub22,fSubLay); 


  fSub31->AddFrame(fSCP3f11);  fSub32->AddFrame(fSCP3f12);    
  fSub31->AddFrame(fSCP3f21);  fSub32->AddFrame(fSCP3f22);    
  fSub31->AddFrame(fSCP3f31);  fSub32->AddFrame(fSCP3f32);    
  fSCPPass3->AddFrame(fSub31,fSubLay); 
  fSCPPass3->AddFrame(fSub32,fSubLay); 


  fSub41->AddFrame(fSCP4f11);  fSub42->AddFrame(fSCP4f12);    
  fSub41->AddFrame(fSCP4f21);  fSub42->AddFrame(fSCP4f22);    
  fSub41->AddFrame(fSCP4f31);  fSub42->AddFrame(fSCP4f32);    
  fSCPPass4->AddFrame(fSub41,fSubLay); 
  fSCPPass4->AddFrame(fSub42,fSubLay); 



  fButtons->AddFrame(fSCPButtonOK);
  fButtons->AddFrame(fSCPButtonSAVE);
  fButtons->AddFrame(fSCPButtonCANCEL);

  fSCP->AddFrame(fSCPName,fSCPNameLay);
  fSCP->AddFrame(fSCPPass0,fLay);
  fSCP->AddFrame(fSCPPass1,fLay);
  fSCP->AddFrame(fSCPPass2,fLay);
  fSCP->AddFrame(fSCPPass3,fLay);
  fSCP->AddFrame(fSCPPass4,fLay);
  fSCP->AddFrame(fButtons);

  fSCPOpt = kLHintsCenterX | kLHintsExpandX;
  TGLayoutHints *fSCPLay = new TGLayoutHints(fSCPOpt,0,0,0,0);

  AddFrame(fSCP,fSCPLay);

  MapSubwindows();
  SetWindowName("Calibration Parameters");
  SetIconName("Calibration Parameters");

  Layout();
  Resize();
  CenterOnParent();
  MapWindow();
  gClient->WaitFor(this);
}


Bool_t SetCalPar::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
    case kCM_BUTTON:
      ProcessTextButton(parm1,parm2);
      break;
    }
  }
  return kTRUE;
}



void SetCalPar::ProcessTextButton(Long_t parm1, Long_t parm2){
  switch (parm1) {
  case kSCPTB_OK:
  case kSCPTB_SAVE:
    Int_t aMessage;

    control->EventsStep[0] = atoi(fSCP0Tb12->GetString());
    control->EventsStep[1] = atoi(fSCP1Tb12->GetString());
    control->EventsStep[2] = atoi(fSCP2Tb12->GetString());
    control->SRSigmaCut          = atof(fSCP2Tb22->GetString());
    control->SRBin               = atoi(fSCP2Tb32->GetString());
    control->SRLow               = atof(fSCP2Tb42->GetString());
    control->SRHigh              = atof(fSCP2Tb52->GetString());
    control->EventsStep[3] = atoi(fSCP3Tb12->GetString());
    control->SigmaLow            = atof(fSCP3Tb22->GetString());
    control->SigmaHigh           = atof(fSCP3Tb32->GetString());
    control->EventsStep[4] = atoi(fSCP4Tb12->GetString());
    control->GASigmaCut          = atof(fSCP4Tb22->GetString());
    control->GAOut               = atof(fSCP4Tb32->GetString()); 

    //    if (parm1 == kSCPTB_SAVE) { } Save Calibration numbers on file
    aMessage=MK_MSG((EWidgetMessageTypes)kC_SETCALPAR,
                  (EWidgetMessageTypes)parm1);
    SendMessage(fMain,aMessage,0,0);
    CloseWindow();
    break;

  case kSCPTB_CANCEL:
    CloseWindow();
    break;
  }	
}

void SetCalPar::CloseWindow(){
  //TGTransientFrame::CloseWindow();
  DeleteWindow();
}

SetCalPar::~SetCalPar()
{
#include "shm_detach.code"
//   delete fSCP0f11; delete fSCP0f12; delete fSCP0Tb12;
//   delete fSCP1f11; delete fSCP1f12; delete fSCP1Tb12;
//   delete fSCP2f11; delete fSCP2f12; delete fSCP2Tb12;
//   delete fSCP2f21; delete fSCP2f22; delete fSCP2Tb22;
//   delete fSCP2f31; delete fSCP2f32; delete fSCP2Tb32;
//   delete fSCP2f41; delete fSCP2f42; delete fSCP2Tb42;
//   delete fSCP2f51; delete fSCP2f52; delete fSCP2Tb52;
//   delete fSCP3f11; delete fSCP3f12; delete fSCP3Tb12;
//   delete fSCP4f11; delete fSCP4f12; delete fSCP4Tb12;
//   delete fSCP4f21; delete fSCP4f22; delete fSCP4Tb22;
//   delete fSCP4f31; delete fSCP4f32; delete fSCP4Tb32;
//   delete fSCPButtonOK;
//   delete fSCPButtonSAVE;
//   delete fSCPButtonCANCEL;
//   delete fSCPPass0;       
//   delete fSCPPass1;       
//   delete fSCPPass2;       
//   delete fSCPPass3;       
//   delete fSCPPass4;       
//   delete fSCP;       
//   delete fMain;
}

