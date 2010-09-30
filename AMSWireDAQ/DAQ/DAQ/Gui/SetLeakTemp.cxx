#include "SetLeakTemp.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>



SetLeakTemp::SetLeakTemp(const TGWindow *p, const TGWindow *main, 
			       UInt_t w, UInt_t h) 
    : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);
  // Creating share memory segments
  char file[200];
#include "shm_definition.code"


  fMain=main;
  fSLT = new TGCompositeFrame(this,300,0,kVerticalFrame);
 
  TGLayoutHints *fSLTNameLay = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
  TGLabel *fSLTName = new TGLabel(fSLT,"Set room temp and leakage current");//,ffontcon[1],ffont[1]);
  fSLTName->SetTextFont(DefineFonts::fFont[1]);

  //Int_t  fSSTHints = kLHintsLeft;

  TGCompositeFrame *fSubA  =new TGCompositeFrame(fSLT, 290,0,kHorizontalFrame);
  TGCompositeFrame *fSub01 =new TGCompositeFrame(fSubA,200,0,kVerticalFrame);
  TGCompositeFrame *fSub02 =new TGCompositeFrame(fSubA,45,0,kVerticalFrame);
  TGCompositeFrame *fSub03 =new TGCompositeFrame(fSubA,50,0,kVerticalFrame);

  fSub01->SetLayoutManager(new TGMatrixLayout(fSub01,0,1,2,kLHintsLeft));
  fSLTf11  = new TGLabel(fSub01,"Room Temperature:");//,ffontcon[2],ffont[2]);
  fSLTf11->SetTextFont(DefineFonts::fFont[2]);
  fSLTf11->Resize(190,fSLTf11->GetDefaultHeight());
  fSLTf11->SetTextJustify(kTextLeft);
  fSLTf21  = new TGLabel(fSub01,"Leakage Current @80V:");//,ffontcon[2],ffont[2]);
  fSLTf21->SetTextFont(DefineFonts::fFont[2]);
  fSLTf21->Resize(190,fSLTf21->GetDefaultHeight());
  fSLTf21->SetTextJustify(kTextLeft);
  fSLTf31  = new TGLabel(fSub01,"Leakage Current @90V:");//,ffontcon[2],ffont[2]);
  fSLTf31->SetTextFont(DefineFonts::fFont[2]);
  fSLTf31->Resize(190,fSLTf31->GetDefaultHeight());
  fSLTf31->SetTextJustify(kTextLeft);
  
  fSub02->SetLayoutManager(new TGMatrixLayout(fSub02,0,1,2,kLHintsLeft));
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
  fSLTf13  = new TGLabel(fSub03," C");//,ffontcon[2],ffont[2]);
  fSLTf13->SetTextFont(DefineFonts::fFont[2]);
  fSLTf13->Resize(40,fSLTf11->GetDefaultHeight());
  fSLTf13->SetTextJustify(kTextLeft);
  fSLTf23  = new TGLabel(fSub03," uA");//,ffontcon[2],ffont[2]);
  fSLTf23->SetTextFont(DefineFonts::fFont[2]);
  fSLTf23->Resize(40,fSLTf21->GetDefaultHeight());
  fSLTf23->SetTextJustify(kTextLeft);
  fSLTf33  = new TGLabel(fSub03," uA");//,ffontcon[2],ffont[2]);
  fSLTf33->SetTextFont(DefineFonts::fFont[2]);
  fSLTf33->Resize(40,fSLTf31->GetDefaultHeight());
  fSLTf33->SetTextJustify(kTextLeft);



    //TGCompositeFrame *fEdit = new TGCompositeFrame(fSLT,290,0,kVerticalFrame);
    //if (control->AMSTest>='A') {
    //  fSLTf41  = new TGLabel(fEdit,"Comments (max 5 lines)",
    //			     ffontcon[2],ffont[2]);
    //fSLTf41->SetTextJustify(kTextLeft);
    //fSLTf42 = new TGTextEdit(fEdit,290,100,kSunkenFrame | kDoubleBorder);
    //}

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


  //if (control->AMSTest>='A') {
  //  fEdit->AddFrame(fSLTf41);
  //  fEdit->AddFrame(fSLTf42);
  //}
  TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsLeft,0,0,0,0);
  fSubA->AddFrame(fSub01,fSubLay); 
  fSubA->AddFrame(fSub02,fSubLay); 
  fSubA->AddFrame(fSub03,fSubLay); 


  
  fButtons->AddFrame(fSLTButtonOK);
  fButtons->AddFrame(fSLTButtonSAVE);
  fButtons->AddFrame(fSLTButtonCANCEL);

  fSLT->AddFrame(fSLTName,fSLTNameLay);
  fSLT->AddFrame(fSubA,fSubLay);
  //if (control->AMSTest>='A') fSLT->AddFrame(fEdit);
  fSLT->AddFrame(fButtons);

  TGLayoutHints *fSLTLay = 
      new TGLayoutHints(kLHintsCenterX | kLHintsExpandX,0,0,0,0);

  AddFrame(fSLT,fSLTLay);

  MapSubwindows();
  SetWindowName("Set Temperature and Leakage");
  SetIconName("Set Temperature and Leakage");

  Layout();
  Resize();
  MapWindow();
  gClient->WaitFor(this);
  //printf("after waitfor\n");
  //gSystem->ProcessEvents();
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
  char filename[100];
  switch (parm1) {
  case kSLTTB_OK:
  case kSLTTB_SAVE:
    control->RoomTemp = atof(fSLTTb12->GetString());
    control->Ileak80  = atof(fSLTTb22->GetString());
    control->Ileak90  = atof(fSLTTb32->GetString());
    sprintf(filename,"%s/%s.com",control->fDirLadder,control->fLadderName); 
    //fSLTf42->SaveFile(filename,kFALSE);
    //usleep(100000);
    //strcpy(control->Comment, (char *) fSLTf42->GetText());
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
  printf("welcome to the delete step\n");
#include "shm_detach.code"
  printf("after detach\n");
//   delete fSLTf11; 
//   printf("after 1st delete\n");
//   delete fSLTf12; delete fSLTTb12;
//   delete fSLTf13; 
//   delete fSLTf21; 
//   delete fSLTf22; delete fSLTTb22;
//   delete fSLTf23; 
//   delete fSLTf31; 
//   delete fSLTf32; delete fSLTTb32;
//   delete fSLTf33; 
//   delete fSLTf41; 
//   delete fSLTf42; 
//   delete fSLTButtonOK;
//   delete fSLTButtonSAVE;
//   delete fSLTButtonCANCEL;
//   delete fSLT;       
//   delete fMain;
}










