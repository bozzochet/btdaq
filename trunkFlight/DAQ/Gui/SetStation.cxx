#include "SetStation.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>



SetStation::SetStation(const TGWindow *p, const TGWindow *main, 
			       UInt_t w, UInt_t h) 
    : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);
  // Creating share memory segments
  char file[200];
#include "shm_definition.code"


  fMain=main;
  fSS = new TGCompositeFrame(this,300,0,kVerticalFrame);
 
  TGLayoutHints *fSSNameLay = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
  TGLabel *fSSName = new TGLabel(fSLT,"Set Test Station Name",
				  ffontcon[1],ffont[1]);

  //Int_t  fSSTHints = kLHintsLeft;

  TGCompositeFrame *fSubA  =new TGCompositeFrame(fSLT, 290,0,kHorizontalFrame);
  TGCompositeFrame *fSub01 =new TGCompositeFrame(fSubA,200,0,kVerticalFrame);

  fSub01->SetLayoutManager(new TGMatrixLayout(fSub01,0,1,2,kLHintsLeft));
  fSSf11  = new TGLabel(fSub01,"Station:",ffontcon[2],ffont[2]);
    fSSf11->Resize(190,fSSf11->GetDefaultHeight());
    fSSf11->SetTextJustify(kTextLeft);

  fSub02->SetLayoutManager(new TGMatrixLayout(fSub02,0,1,2,kLHintsLeft));
  fSSf12 = new TGComboBox(fSub02,kSSCB_STATION);
    fSSf12->AddEntry("Geneve"          ,kSS_GENEVE);
    fSSf12->AddEntry("Perugia"         ,kSS_PERUGIA);
    fSSf12->AddEntry("G&A"             ,kSS_GYA);
    fSSf12->AddEntry("ETH/Prevessin"   ,kSS_ETH);
    fSSf12->AddEntry("Geneve-Eduardo"  ,kSS_EDUARDO);
    fSSf12->AddEntry("Perugia-Giovanni",kSS_GIOVANNI);
    fSSf12->Select(control->Station);
    fSSf12->Resize(100,20);
    fSSf12->Associate(this);
 





  fSLTTb12 = new TGTextBuffer(10);
  fSLTf12 = new TGTextEntry(fSub02,fSLTTb12);
    fSLTf12->Resize(35,fSLTf11->GetDefaultHeight());
    fSLTTb12->Clear();
    //char value[20];
    //sprintf(value,"%5.1f",0.0);
    //fSLTTb12->AddText(0,value);
    fSLTf12->Associate(this);
  fSLTTb22 = new TGTextBuffer(10);
  fSLTf22 = new TGTextEntry(fSub02,fSLTTb22);
    fSLTf22->Resize(35,fSLTf21->GetDefaultHeight());
    fSLTTb22->Clear();
    //sprintf(value,"%5.1f",0.0);
    //fSLTTb22->AddText(0,value);
    fSLTf22->Associate(this);
  fSLTTb32 = new TGTextBuffer(10);
  fSLTf32 = new TGTextEntry(fSub02,fSLTTb32);
    fSLTf32->Resize(35,fSLTf31->GetDefaultHeight());
    fSLTTb32->Clear();
    //sprintf(value,"%5.1f",0.0);
    //fSLTTb32->AddText(0,value);
    fSLTf32->Associate(this);


  fSub03->SetLayoutManager(new TGMatrixLayout(fSub03,0,1,2,kLHintsLeft));
  fSLTf13  = new TGLabel(fSub03," C",ffontcon[2],ffont[2]);
    fSLTf13->Resize(40,fSLTf11->GetDefaultHeight());
    fSLTf13->SetTextJustify(kTextLeft);
  fSLTf23  = new TGLabel(fSub03," uA",ffontcon[2],ffont[2]);
    fSLTf23->Resize(40,fSLTf21->GetDefaultHeight());
    fSLTf23->SetTextJustify(kTextLeft);
  fSLTf33  = new TGLabel(fSub03," uA",ffontcon[2],ffont[2]);
    fSLTf33->Resize(40,fSLTf31->GetDefaultHeight());
    fSLTf33->SetTextJustify(kTextLeft);


  TGCompositeFrame *fButtons = new TGCompositeFrame(fSLT,200,0,
						    kHorizontalFrame);
  fButtons->SetLayoutManager(new TGMatrixLayout(fButtons,1,0,5,
					     kLHintsLeft));


  fSLTButtonOK     = new TGTextButton(fButtons,"OK"    ,kSLTTB_OK);
  fSLTButtonOK->Resize(60,fSLTButtonOK->GetDefaultHeight());
  fSLTButtonOK->Associate(this);

  fSLTButtonSAVE   = new TGTextButton(fButtons,"SAVE"  ,kSLTTB_SAVE);
  fSLTButtonSAVE->Resize(60,fSLTButtonSAVE->GetDefaultHeight());
  fSLTButtonSAVE->Associate(this);

  fSLTButtonCANCEL = new TGTextButton(fButtons,"CANCEL",kSLTTB_CANCEL);
  fSLTButtonCANCEL->Resize(60,fSLTButtonCANCEL->GetDefaultHeight());
  fSLTButtonCANCEL->Associate(this);



  // Placing everything together
  //TGLayoutHints *fLay=new TGLayoutHints(kLHintsLeft|kLHintsExpandX,5,0,5,5);
  //TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsLeft,0,0,0,-15);


  fSub01->AddFrame(fSLTf11);
  fSub02->AddFrame(fSLTf12);
  fSub03->AddFrame(fSLTf13);   
  
  fSub01->AddFrame(fSLTf21); 
  fSub02->AddFrame(fSLTf22); 
  fSub03->AddFrame(fSLTf23);   

  fSub01->AddFrame(fSLTf31); 
  fSub02->AddFrame(fSLTf32); 
  fSub03->AddFrame(fSLTf33);   

  TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsLeft,0,0,0,0);
  fSubA->AddFrame(fSub01,fSubLay); 
  fSubA->AddFrame(fSub02,fSubLay); 
  fSubA->AddFrame(fSub03,fSubLay); 
  
  fButtons->AddFrame(fSLTButtonOK);
  fButtons->AddFrame(fSLTButtonSAVE);
  fButtons->AddFrame(fSLTButtonCANCEL);

  fSLT->AddFrame(fSLTName,fSLTNameLay);
  fSLT->AddFrame(fSubA,fSubLay);
  fSLT->AddFrame(fButtons);

  TGLayoutHints *fSLTLay = 
      new TGLayoutHints(kLHintsCenterX | kLHintsExpandX,0,0,0,0);

  AddFrame(fSLT,fSLTLay);

  MapSubwindows();
  SetWindowName("Set Temperature and Leakage");
  SetIconName("Set Temperature and Leakage");

  Layout();
  MapWindow();
  gClient->WaitFor(this);
}


Bool_t SetLeakTemp::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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



void SetLeakTemp::ProcessTextButton(Long_t parm1, Long_t parm2){
  switch (parm1) {
  case kSLTTB_OK:
  case kSLTTB_SAVE:
    control->RoomTemp = atof(fSLTTb12->GetString());
    control->Ileak80  = atof(fSLTTb22->GetString());
    control->Ileak90  = atof(fSLTTb32->GetString());
    break;

  case kSLTTB_CANCEL:
    break;
  }	
  Int_t aMessage;
  aMessage=MK_MSG((EWidgetMessageTypes)kC_SETLEAKTEMP,
                  (EWidgetMessageTypes)parm1);
  SendMessage(fMain,aMessage,0,0);
  CloseWindow();
}

void SetLeakTemp::CloseWindow(){
  //TGTransientFrame::CloseWindow();
  DeleteWindow();
}

SetLeakTemp::~SetLeakTemp()
{
#include "shm_detach.code"
//   delete fSLTf11; 
//   delete fSLTf12; delete fSLTTb12;
//   delete fSLTf13; 
//   delete fSLTf21; 
//   delete fSLTf22; delete fSLTTb22;
//   delete fSLTf23; 
//   delete fSLTButtonOK;
//   delete fSLTButtonSAVE;
//   delete fSLTButtonCANCEL;
//   delete fSLT;       
//   delete fMain;
}










