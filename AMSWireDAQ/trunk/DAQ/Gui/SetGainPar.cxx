#include "SetGainPar.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>



SetGainPar::SetGainPar(const TGWindow *p, const TGWindow *main, 
		     UInt_t w, UInt_t h) 
  : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);
  // Creating share memory segments
  char file[200];
#include "shm_definition.code"


  fMain=main;  

  UInt_t fSGPOpt = kVerticalFrame;
  TGCompositeFrame *fSGP = new TGCompositeFrame(this,190,0,fSGPOpt);
 
  TGLabel *fSGPName = new TGLabel(fSGP,"Gain Parameters");//,ffontcon[1],ffont[1]);
  fSGPName->SetTextFont(DefineFonts::fFont[1]);
  
  fSGPOpt = kHorizontalFrame;
  TGCompositeFrame *fSGPData = new TGCompositeFrame(fSGP,180,0,fSGPOpt);
  //  fSGPData->SetLayoutManager(new TGMatrixLayout(fSGPData,1,0,2,kLHintsLeft));
  
  fSGPOpt = kVerticalFrame;
  Int_t fSGPHints = kLHintsLeft | kLHintsExpandX;
  TGCompositeFrame *fSGP01 = new TGCompositeFrame(fSGPData,15,0,fSGPOpt);
  fSGP01->SetLayoutManager(new TGMatrixLayout(fSGP01,0,2,2,fSGPHints));
  TGCompositeFrame *fSGP02 = new TGCompositeFrame(fSGPData,10,0,fSGPOpt);
  fSGP02->SetLayoutManager(new TGMatrixLayout(fSGP02,0,1,2,fSGPHints));
  TGCompositeFrame *fSGP03 = new TGCompositeFrame(fSGPData,60,0,fSGPOpt);
  fSGP03->SetLayoutManager(new TGMatrixLayout(fSGP03,0,1,2,fSGPHints));
  TGCompositeFrame *fSGP04 = new TGCompositeFrame(fSGPData,60,0,fSGPOpt);
  fSGP04->SetLayoutManager(new TGMatrixLayout(fSGP04,0,1,2,fSGPHints));

    
  TGLayoutHints *fL1 = new TGLayoutHints(kLHintsLeft,1,1,1,1);
  TGLabel *Head01=new TGLabel(fSGP01," ");//,ffontcon[2],ffont[2]);
  Head01->SetTextFont(DefineFonts::fFont[2]);
  Head01->Resize(15,Head01->GetDefaultHeight());
  Head01->SetTextJustify(kTextLeft);
  fSGP01->AddFrame(Head01);
  TGLabel *Head02=new TGLabel(fSGP01," ");//,ffontcon[2],ffont[2]);
  Head02->SetTextFont(DefineFonts::fFont[2]);
  Head02->Resize(10,Head02->GetDefaultHeight());
  Head02->SetTextJustify(kTextLeft);
  fSGP01->AddFrame(Head02);
  TGLabel *Head03=new TGLabel(fSGP03,"DAC val");//,ffontcon[2],ffont[2]);
  Head03->SetTextFont(DefineFonts::fFont[2]);
  Head03->Resize(70,Head03->GetDefaultHeight());
  Head03->SetTextJustify(kTextLeft);
  fSGP03->AddFrame(Head03);
  TGLabel *Head04=new TGLabel(fSGP04,"Events");//,ffontcon[2],ffont[2]);
  Head04->SetTextFont(DefineFonts::fFont[2]);
  Head04->Resize(70,Head04->GetDefaultHeight());
  Head04->SetTextJustify(kTextLeft);
  fSGP04->AddFrame(Head04);
  

  char value[100];
  char DACnumber[20];
  for (int i=0;i<16;i++){
    sprintf(DACnumber,"%2d",i+1); 
    fSGPLabel[i]=new TGLabel(fSGP01,DACnumber);//,ffontcon[2],ffont[2]);
    fSGPLabel[i]->SetTextFont(DefineFonts::fFont[2]);
    fSGPLabel[i]->Resize(15,fSGPLabel[i]->GetDefaultHeight());
    fSGPLabel[i]->SetTextJustify(kTextLeft);
    fSGP01->AddFrame(fSGPLabel[i]);
    
    fSGPCB[i]=new TGCheckButton(fSGP01,"",-1);
    fSGPCB[i]->Resize(10,fSGPLabel[i]->GetDefaultHeight());
    if (control->DACmeasure[i]) fSGPCB[i]->SetState(kButtonDown);

    fSGPCB[i]->Associate(this);
    fSGP01->AddFrame(fSGPCB[i]);

    fSGPDACTB[i] = new TGTextBuffer(10);
    fSGPDAC[i] = new TGTextEntry(fSGP03,fSGPDACTB[i]);
    fSGPDAC[i]->Resize(70,fSGPLabel[i]->GetDefaultHeight());
    fSGPDACTB[i]->Clear();
    sprintf(value,"%d",control->DAC[i]);
    fSGPDACTB[i]->AddText(0,value);
    fSGPDAC[i]->Associate(this);
    fSGP03->AddFrame(fSGPDAC[i],fL1);

    fSGPEventTB[i] = new TGTextBuffer(10);
    fSGPEvent[i]=new TGTextEntry(fSGP04,fSGPEventTB[i]);
    fSGPEvent[i]->Resize(70,fSGPLabel[i]->GetDefaultHeight());
    fSGPEventTB[i]->Clear();
    sprintf(value,"%d",control->DACEvent[i]);
    fSGPEventTB[i]->AddText(0,value);
    fSGPEvent[i]->Associate(this);
    fSGP04->AddFrame(fSGPEvent[i],fL1);
  }
  fSGPData->AddFrame(fSGP01);
  //fSGPData->AddFrame(fSGP02);
  fSGPData->AddFrame(fSGP03);
  fSGPData->AddFrame(fSGP04);


  TGCompositeFrame *fCuts = new TGCompositeFrame(fSGP,200,0,kHorizontalFrame);
  fCuts->SetLayoutManager(new TGMatrixLayout(fCuts,1,0,5,kLHintsLeft));

  fSGPf11  = new TGLabel(fCuts,"Low Gain Cut");//,ffontcon[2],ffont[2]);
  fSGPf11->SetTextFont(DefineFonts::fFont[2]);
  fSGPf11->Resize(200,fSGPf11->GetDefaultHeight());
  fSGPf11->SetTextJustify(kTextLeft);
  fSGPTb12 = new TGTextBuffer(1034);
  fSGPf12 = new TGTextEntry(fCuts,fSGPTb12);
  fSGPf12->Resize(80,fSGPf11->GetDefaultHeight());
  fSGPTb12->Clear();
  sprintf(value,"%5.1f",control->GainLow);
  fSGPTb12->AddText(0,value);
  fSGPf12->Associate(this);

  fCuts->AddFrame(fSGPf11); 
  fCuts->AddFrame(fSGPf12); 


  TGCompositeFrame *fButtons=new TGCompositeFrame(fSGP,180,0,kHorizontalFrame);
  fButtons->SetLayoutManager(new TGMatrixLayout(fButtons,1,0,5,kLHintsLeft));

  fSGPButtonOK     = new TGTextButton(fButtons,"OK"    ,kSGPTB_OK);
  fSGPButtonOK->Resize(60,fSGPButtonOK->GetDefaultHeight());
  fSGPButtonOK->Associate(this);

  fSGPButtonSAVE   = new TGTextButton(fButtons,"SAVE"  ,kSGPTB_SAVE);
  fSGPButtonSAVE->Resize(60,fSGPButtonSAVE->GetDefaultHeight());
  fSGPButtonSAVE->Associate(this);

  fSGPButtonCANCEL = new TGTextButton(fButtons,"CANCEL",kSGPTB_CANCEL);
  fSGPButtonCANCEL->Resize(60,fSGPButtonCANCEL->GetDefaultHeight());
  fSGPButtonCANCEL->Associate(this);

  fButtons->AddFrame(fSGPButtonOK);
  fButtons->AddFrame(fSGPButtonSAVE);
  fButtons->AddFrame(fSGPButtonCANCEL);


  // Placing everything together

  TGLayoutHints *fSGPNameLay = new TGLayoutHints(kLHintsCenterX,1,1,1,1);
  TGLayoutHints *fSGPDataLay = new TGLayoutHints(kLHintsLeft,1,1,1,1);

  fSGP->AddFrame(fSGPName,fSGPNameLay);
  fSGP->AddFrame(fSGPData,fSGPDataLay);
  fSGP->AddFrame(fCuts);
  fSGP->AddFrame(fButtons);

  //fSGPOpt = kLHintsCenterX | kLHintsExpandX;
  fSGPOpt = kLHintsLeft;
  TGLayoutHints *fSGPLay = new TGLayoutHints(fSGPOpt,0,0,0,0);

  AddFrame(fSGP,fSGPLay);

  MapSubwindows();
  SetWindowName("Calibration Gain Parameters");
  SetIconName("Calibration Gain Parameters");

  Layout();
  Resize();
  CenterOnParent();
  MapWindow();
  gClient->WaitFor(this);
}


Bool_t SetGainPar::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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



void SetGainPar::ProcessTextButton(Long_t parm1, Long_t parm2){
  switch (parm1) {
  case kSGPTB_OK:
  case kSGPTB_SAVE:
    Int_t aMessage;
    for (int i=0;i<16;i++){
      control->DACmeasure[i]=0;
      if (fSGPCB[i]->GetState() == kButtonDown) {
	control->DACmeasure[i] = 1;
	control->DAC[i]        = atoi(fSGPDACTB[i]->GetString());
	control->DACEvent[i]   = atoi(fSGPEventTB[i]->GetString());
      }
     }
    control->GainLow = atof(fSGPTb12->GetString());

    aMessage=MK_MSG((EWidgetMessageTypes)kC_SETGAINPAR,
                  (EWidgetMessageTypes)parm1);
    SendMessage(fMain,aMessage,0,0);
    CloseWindow();
    break;

  case kSGPTB_CANCEL:
    CloseWindow();
    break;
  }	
}


void SetGainPar::CloseWindow(){
  //TGTransientFrame::CloseWindow();
  DeleteWindow();
}

SetGainPar::~SetGainPar()
{
#include "shm_detach.code"
//   delete fSGPLabel; 
//   delete fSGPCB;
//   delete fSGPDAC; delete fSGPDACTB;
//   delete fSGPEvent; delete fSGPEventTB;
//   delete fSGPButtonOK;
//   delete fSGPButtonSAVE;
//   delete fSGPButtonCANCEL;
//   delete fMain;
}
