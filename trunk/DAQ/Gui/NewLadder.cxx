#include "NewLadder.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>



NewLadder::NewLadder(int LoH, const TGWindow *p, const TGWindow *main, 
		     UInt_t w, UInt_t h) 
  : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);
  // Creating share memory segments
  char file[200];
#include "shm_definition.code"

  fMain=main;

  switch (LoH) {
    case 1: ANewLadder(); break;
    case 2: ANewHybrid(); break;
  }

  MapSubwindows();
  switch (LoH) {
    case 1:
      SetWindowName("New Ladder");
      SetIconName("New Ladder");
      break;
    case 2:
      SetWindowName("New Hybrid");
      SetIconName("New Hybrid");
      break;
  }

  Layout();
  Resize();
  CenterOnParent();
  MapWindow();
  gClient->WaitFor(this);
}


void NewLadder::ANewLadder(){
  TGLayoutHints *fNLLay=new TGLayoutHints(kLHintsCenterX|kLHintsExpandX);
  TGCompositeFrame *fNL = new TGCompositeFrame(this,200,0,kVerticalFrame);

  TGLayoutHints *fNameLay = new TGLayoutHints(kLHintsCenterX);
  TGLabel *fName = new TGLabel(fNL,"New Ladder");//,ffontcon[0],ffont[0]);
  fName->SetTextFont(DefineFonts::fFont[0]);

  TGLayoutHints *fWorkLay = new TGLayoutHints(kLHintsCenterX);
  TGCompositeFrame *fWork = new TGCompositeFrame(fNL,200,0,kHorizontalFrame);

  TGLayoutHints *fTitleLay = new TGLayoutHints(kLHintsLeft);
  TGCompositeFrame *fTitle = new TGCompositeFrame(fWork,85,0,kVerticalFrame);
  TGLabel *fTit1 = new TGLabel(fTitle,"Sensors:");// ,ffontcon[2],ffont[2]);
  TGLabel *fTit2 = new TGLabel(fTitle,"Assembly:");//,ffontcon[2],ffont[2]);
  TGLabel *fTit3 = new TGLabel(fTitle,"Layer:");//   ,ffontcon[2],ffont[2]);
  TGLabel *fTit4 = new TGLabel(fTitle,"Serial:");//  ,ffontcon[2],ffont[2]);
  fTit1->SetTextFont(DefineFonts::fFont[2]);
  fTit2->SetTextFont(DefineFonts::fFont[2]);
  fTit3->SetTextFont(DefineFonts::fFont[2]);
  fTit4->SetTextFont(DefineFonts::fFont[2]);
  
  TGLayoutHints *fValueLay  = new TGLayoutHints(kLHintsRight);
  TGLayoutHints *fValueLay2 = new TGLayoutHints(kLHintsLeft);
  TGCompositeFrame *fValue  = new TGCompositeFrame(fWork,100,0,kVerticalFrame);

  fVoltage='l';
  
  TGComboBox *fVal1 = new TGComboBox(fValue,kNLCB_SENSORS);
    fVal1->AddEntry("01", 1);  
    fVal1->AddEntry("02", 2);
    fVal1->AddEntry("03", 3);  
    fVal1->AddEntry("04", 4);
    fVal1->AddEntry("05", 5);  
    fVal1->AddEntry("06", 6);
    fVal1->AddEntry("07", 7);  
    fVal1->AddEntry("08", 8);
    fVal1->AddEntry("09", 9);  
    fVal1->AddEntry("10",10);
    fVal1->AddEntry("11",11);  
    fVal1->AddEntry("12",12);
    fVal1->AddEntry("13",13);  
    fVal1->AddEntry("14",14);
    fVal1->AddEntry("15",15);  
    fVal1->Select(12);
    fVal1->Resize(100,20);
    fVal1->Associate(this);
    strcpy(fSensors,"12");
  TGComboBox *fVal2 = new TGComboBox(fValue,kNLCB_ASSEMBLY);
    fVal2->AddEntry("Geneve" , 1);  
    fVal2->AddEntry("Perugia", 2);
    fVal2->AddEntry("G&A" , 3);  
    fVal2->AddEntry("Spare",4);
    fVal2->AddEntry("Russian",5);
    fVal2->Select(3);
    fVal2->Resize(100,20);
    fVal2->Associate(this);
    sprintf(fAssembly,"a");
  TGComboBox *fVal3 = new TGComboBox(fValue,kNLCB_LAYER);
    fVal3->AddEntry("I - Inner", 1);  
    fVal3->AddEntry("O - Outer", 2);
    fVal3->AddEntry("T- Test" , 3);
    fVal3->AddEntry("J - Inner upper holes",4);
    fVal3->AddEntry("H - Inner lower holes",5);
    fVal3->AddEntry("P - Outer upper holes",6);
    fVal3->AddEntry("N - Outer lower holes",7);
    fVal3->Select(2);
    fVal3->Resize(100,20);
    fVal3->Associate(this);
    fLayer='o';

    //fVal4TB = new TGTextBuffer(4);
  fVal4 = new TGTextEntry(fValue,new TGTextBuffer(4),kNLTE_SERIAL);
    fVal4->Resize(30,20);
    //fVal4TB->Clear();
    //fVal4TB->AddText(0,"000");
    fVal4->SetText("000");
    fVal4->Associate(this);
    strcpy(fSerial,fVal4->GetText());

    if (strcmp(fAssembly,"ru"))
      sprintf(fLadderName,"l%s%s%c%s",fSensors,fAssembly,fLayer,fSerial);
    else
      sprintf(fLadderName,"l%s%s%s",fSensors,fAssembly,fSerial);
  //TGLayoutHints *fShowNameLay = new TGLayoutHints(kLHintsCenterX,0,0,10,10);
  //fShowName = new TGLabel(fNL,new TGString(fLadderName),ffontcon[1],ffont[1]);

  TGLayoutHints *fButtonLay = new TGLayoutHints(kLHintsCenterX|kLHintsExpandX);
  TGCompositeFrame *fButton = new TGCompositeFrame(fNL,200,0,kHorizontalFrame);

  TGLayoutHints *fButtonOKLay = new TGLayoutHints(kLHintsLeft,10,0,20,0);
  TGTextButton *fButtonOK = 
    new TGTextButton(fButton,new TGHotString("OK"),kNLTB_OK);
  fButtonOK->Associate(this);

  TGLayoutHints *fButtonCancelLay = new TGLayoutHints(kLHintsRight,0,10,20,0);
  TGTextButton *fButtonCancel = 
    new TGTextButton(fButton,new TGHotString("CANCEL"),kNLTB_CANCEL);
  fButtonCancel->Associate(this);


  fTitle->AddFrame(fTit1,fTitleLay); fValue->AddFrame(fVal1,fValueLay);
  fTitle->AddFrame(fTit2,fTitleLay); fValue->AddFrame(fVal2,fValueLay);
  fTitle->AddFrame(fTit3,fTitleLay); fValue->AddFrame(fVal3,fValueLay);
  fTitle->AddFrame(fTit4,fTitleLay); fValue->AddFrame(fVal4,fValueLay2);

  fWork->AddFrame(fTitle,fTitleLay); fWork->AddFrame(fValue,fValueLay);


  fButton->AddFrame(fButtonOK,fButtonOKLay);
  fButton->AddFrame(fButtonCancel,fButtonCancelLay);
  
  fNL->AddFrame(fName,fNameLay);
  fNL->AddFrame(fWork,fWorkLay);
  //fNL->AddFrame(fShowName,fShowNameLay);
  
  fNL->AddFrame(fButton,fButtonLay);

  AddFrame(fNL,fNLLay);
}


void NewLadder::ANewHybrid(){

  TGLayoutHints *fNLLay=new TGLayoutHints(kLHintsCenterX|kLHintsExpandX);
  TGCompositeFrame *fNL = new TGCompositeFrame(this,200,0,kVerticalFrame);

  TGLayoutHints *fNameLay = new TGLayoutHints(kLHintsCenterX);
  TGLabel *fName = new TGLabel(fNL,"New Hybrid");//,ffontcon[0],ffont[0]);
  fName->SetTextFont(DefineFonts::fFont[0]);

  TGLayoutHints *fWorkLay = new TGLayoutHints(kLHintsCenterX);
  TGCompositeFrame *fWork = new TGCompositeFrame(fNL,200,0,kHorizontalFrame);

  TGLayoutHints *fTitleLay = new TGLayoutHints(kLHintsLeft);
  TGCompositeFrame *fTitle = new TGCompositeFrame(fWork,85,0,kVerticalFrame);
  TGLabel *fTit1 = new TGLabel(fTitle,"S or K:");// ,ffontcon[2],ffont[2]);
  TGLabel *fTit2 = new TGLabel(fTitle,"Batch:");//,ffontcon[2],ffont[2]);
  TGLabel *fTit3 = new TGLabel(fTitle,"Serial:");//   ,ffontcon[2],ffont[2]);
  fTit1->SetTextFont(DefineFonts::fFont[2]);
  fTit2->SetTextFont(DefineFonts::fFont[2]);
  fTit3->SetTextFont(DefineFonts::fFont[2]);
  
  TGLayoutHints *fValueLay  = new TGLayoutHints(kLHintsRight);
  TGLayoutHints *fValueLay2 = new TGLayoutHints(kLHintsLeft);
  TGCompositeFrame *fValue  = new TGCompositeFrame(fWork,100,0,kVerticalFrame);

  TGComboBox *fVal1 = new TGComboBox(fValue,kNLCB_SorK);
    fVal1->AddEntry("S", 1);  
    fVal1->AddEntry("K", 2);
    fVal1->Select(1);
    strcpy(fHybridType,"2024");
    fVal1->Resize(100,20);
    fVal1->Associate(this);

  fVal5 = new TGTextEntry(fValue,new TGTextBuffer(4),kNLTE_BATCH);
    fVal5->Resize(30,20);
    //fVal4TB->Clear();
    //fVal4TB->AddText(0,"000");
    fVal5->SetText("");
    strcpy(fHybridBatch,"1");
    fVal5->Associate(this);

  fVal6 = new TGTextEntry(fValue,new TGTextBuffer(4),kNLTE_HYB_SERIAL);
    fVal6->Resize(30,20);
    //fVal4TB->Clear();
    //fVal4TB->AddText(0,"000");
    fVal6->SetText("");
    strcpy(fHybridSerial,"1");
    fVal6->Associate(this);

  TGLayoutHints *fButtonLay = new TGLayoutHints(kLHintsCenterX|kLHintsExpandX);
  TGCompositeFrame *fButton = new TGCompositeFrame(fNL,200,0,kHorizontalFrame);

  TGLayoutHints *fButtonOKLay = new TGLayoutHints(kLHintsLeft,10,0,20,0);
  TGTextButton *fButtonOK = 
    new TGTextButton(fButton,new TGHotString("OK"),kNLTB_HYB_OK);
  fButtonOK->Associate(this);

  TGLayoutHints *fButtonCancelLay = new TGLayoutHints(kLHintsRight,0,10,20,0);
  TGTextButton *fButtonCancel = 
    new TGTextButton(fButton,new TGHotString("CANCEL"),kNLTB_CANCEL);
  fButtonCancel->Associate(this);

  fTitle->AddFrame(fTit1,fTitleLay); fValue->AddFrame(fVal1,fValueLay);
  fTitle->AddFrame(fTit2,fTitleLay); fValue->AddFrame(fVal5,fValueLay);
  fTitle->AddFrame(fTit3,fTitleLay); fValue->AddFrame(fVal6,fValueLay);


  fWork->AddFrame(fTitle,fTitleLay); fWork->AddFrame(fValue,fValueLay);


  fButton->AddFrame(fButtonOK,fButtonOKLay);
  fButton->AddFrame(fButtonCancel,fButtonCancelLay);
  
  fNL->AddFrame(fName,fNameLay);
  fNL->AddFrame(fWork,fWorkLay);
  //fNL->AddFrame(fShowName,fShowNameLay);
  
  fNL->AddFrame(fButton,fButtonLay);

  AddFrame(fNL,fNLLay);

}

Bool_t NewLadder::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
    case kCM_COMBOBOX:
      ProcessComboBox(parm1,parm2); 
      break;
    case kCM_BUTTON:
      ProcessTextButton(parm1,parm2);
      break;
    }
    break;
  case kC_TEXTENTRY:
    switch (GET_SUBMSG(msg)) {
    case kTE_TEXTCHANGED:
    case kTE_ENTER:
      ProcessTextEntry(parm1,parm2);
      break;
    }
    break;
  }

  if (strcmp(fAssembly,"ru"))
    sprintf(fLadderName,"%c%s%s%c%s",fVoltage,fSensors,fAssembly,fLayer,fSerial);
  else
    sprintf(fLadderName,"%c%s%s%s",fVoltage,fSensors,fAssembly,fSerial);
  //  sprintf(fLadderName,"%c%s%s%c%s",
  //  fVoltage,fSensors,fAssembly,fLayer,fSerial); 
  sprintf(fHybridName,"%s-%s-%s",
	  fHybridType,fHybridBatch,fHybridSerial); 
  //fShowName->SetText(new TGString(fLadderName));
  //gClient->NeedRedraw(fShowName);
  //gSystem->ProcessEvents();

  return kTRUE;
}

void NewLadder::ProcessComboBox(Long_t parm1, Long_t parm2){
  switch (parm1) {
  case kNLCB_SENSORS:
    switch (parm2) {
    case  1: strcpy(fSensors,"01");  break;
    case  2: strcpy(fSensors,"02");  break;
    case  3: strcpy(fSensors,"03");  break;
    case  4: strcpy(fSensors,"04");  break;
    case  5: strcpy(fSensors,"05");  break;
    case  6: strcpy(fSensors,"06");  break;
    case  7: strcpy(fSensors,"07");  break;
    case  8: strcpy(fSensors,"08");  break;
    case  9: strcpy(fSensors,"09");  break;
    case 10: strcpy(fSensors,"10");  break;
    case 11: strcpy(fSensors,"11");  break;
    case 12: strcpy(fSensors,"12");  break;
    case 13: strcpy(fSensors,"13");  break;
    case 14: strcpy(fSensors,"14");  break;
    case 15: strcpy(fSensors,"15");  break;
    }
    break;
  case kNLCB_ASSEMBLY:
    switch (parm2) {
    case 1: sprintf(fAssembly,"g");  break;
    case 2: sprintf(fAssembly,"p");  break;
    case 3: sprintf(fAssembly,"a");  break;
    case 4: sprintf(fAssembly,"s");  break;
    case 5: sprintf(fAssembly,"ru"); break;
    }
    break;
  case kNLCB_LAYER:
    switch (parm2) {
    case 1: fLayer='i';  break;
    case 2: fLayer='o';  break;
    case 3: fLayer='t';  break;
    case 4: fLayer='j';  break;
    case 5: fLayer='h';  break;
    case 6: fLayer='p';  break;
    case 7: fLayer='n';  break;
    }
    break;
  case kNLCB_SorK:
    switch (parm2) {
    case 1: sprintf(fHybridType,"2024"); break;
    case 2: sprintf(fHybridType,"2025"); break;
    }
    break;

  }	
}


void NewLadder::ProcessTextEntry(Long_t parm1, Long_t parm2){
  switch (parm1) {
  case kNLTE_SERIAL:
    strcpy(fSerial,fVal4->GetText());
    break;
  case kNLTE_BATCH:
    strcpy(fHybridBatch,fVal5->GetText());
    break;
  case kNLTE_HYB_SERIAL:
    strcpy(fHybridSerial,fVal6->GetText());
    break;
  }
}

void NewLadder::ProcessTextButton(Long_t parm1, Long_t parm2){
  Int_t aMessage;
  switch (parm1) {
  case kNLTB_OK:
      //control->fVoltage = fVoltage;
      //sprintf(control->fSensors,fSensors);
      //control->fAssembly=fAssembly;
      //control->fLayer=fLayer;
      //sprintf(control->fSerial,fSerial);
    sprintf(control->fLadderName,fLadderName);
    aMessage=MK_MSG((EWidgetMessageTypes)kC_NEWLADDER,
		    (EWidgetMessageTypes)kNLTB_OK);
    SendMessage(fMain,aMessage,0,0);
    CloseWindow();
    break;

  case kNLTB_HYB_OK:  
      //sprintf(control->fHybridType,fHybridType);
      //sprintf(control->fHybridBatch,fHybridBatch);
      //sprintf(control->fHybridSerial,fHybridSerial);
    sprintf(control->fLadderName,fHybridName);
    aMessage=MK_MSG((EWidgetMessageTypes)kC_NEWLADDER,
		    (EWidgetMessageTypes)kNLTB_HYB_OK);
    SendMessage(fMain,aMessage,0,0);
    CloseWindow();
    break;
  case kNLTB_CANCEL:
    CloseWindow();
    break;
  }	
}

void NewLadder::CloseWindow(){
  //TGTransientFrame::CloseWindow(); 
  DeleteWindow();
}

NewLadder::~NewLadder()
{
#include "shm_detach.code"
//   delete fVal4;
//   delete fVal5;
//   delete fVal6;
//   //delete fShowName; not used
//   delete fMain;
}



