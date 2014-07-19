#include "SetShapingTime.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>

#include <unistd.h>

SetShapingTime::SetShapingTime(const TGWindow *p, const TGWindow *main, 
			       UInt_t w, UInt_t h) 
    : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);
  char value[200];

  // Creating share memory segments
  char file[200];
#include "shm_definition.code"

 
  fMain=main;
  UInt_t fSSTOpt = kVerticalFrame;
  fSST = new TGCompositeFrame(this,300,0,fSSTOpt);
 
  TGLayoutHints *fSSTNameLay = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
  TGLabel *fSSTName = new TGLabel(fSST,"Set Shaping Time");//,ffontcon[1],ffont[1]);
  fSSTName->SetTextFont(DefineFonts::fFont[1]);

  fSSTOpt   = kHorizontalFrame;
  //Int_t  fSSTHints = kLHintsLeft;

  TGCompositeFrame *fSub01 = new TGCompositeFrame(fSST,200,0,fSSTOpt);
  //fSub01->SetLayoutManager(new TGMatrixLayout(fSub01,0,3,2,fSSTHints)); 


  fSSTf11  = new TGLabel(fSub01,"Shaping Time: ");//,ffontcon[2],ffont[2]);
  fSSTf11->SetTextFont(DefineFonts::fFont[2]);
  fSSTf11->Resize(100,fSSTf11->GetDefaultHeight());
  fSSTf11->SetTextJustify(kTextLeft);
  fSSTTb12 = new TGTextBuffer(1034);
  fSSTf12 = new TGTextEntry(fSub01,fSSTTb12);
  fSSTf12->Resize(35,fSSTf11->GetDefaultHeight());
  fSSTTb12->Clear();
  sprintf(value,"%3.1f",control->Shaping);
  fSSTTb12->AddText(0,value);
  fSSTf12->Associate(this);
  fSSTf13  = new TGLabel(fSub01," us");//,ffontcon[2],ffont[2]);
  fSSTf13->SetTextFont(DefineFonts::fFont[2]);
  fSSTf13->Resize(40,fSSTf11->GetDefaultHeight());
  fSSTf13->SetTextJustify(kTextLeft);
  

  TGCompositeFrame *fButtons = new TGCompositeFrame(fSST,200,0,
						    kHorizontalFrame);
  fButtons->SetLayoutManager(new TGMatrixLayout(fButtons,1,0,5,
					     kLHintsLeft));


  fSSTButtonOK     = new TGTextButton(fButtons,"OK"    ,kSSTTB_OK);
  fSSTButtonOK->Resize(60,fSSTButtonOK->GetDefaultHeight());
  fSSTButtonOK->Associate(this);

  fSSTButtonSAVE   = new TGTextButton(fButtons,"SAVE"  ,kSSTTB_SAVE);
  fSSTButtonSAVE->Resize(60,fSSTButtonSAVE->GetDefaultHeight());
  fSSTButtonSAVE->Associate(this);

  fSSTButtonCANCEL = new TGTextButton(fButtons,"CANCEL",kSSTTB_CANCEL);
  fSSTButtonCANCEL->Resize(60,fSSTButtonCANCEL->GetDefaultHeight());
  fSSTButtonCANCEL->Associate(this);



  // Placing everything together
  TGLayoutHints *fLay=new TGLayoutHints(kLHintsLeft | kLHintsExpandX,5,0,5,5);
  //TGLayoutHints *fSubLay = new TGLayoutHints(kLHintsLeft,0,0,0,-15);


  fSub01->AddFrame(fSSTf11);  
  fSub01->AddFrame(fSSTf12);    
  fSub01->AddFrame(fSSTf13);    

  fButtons->AddFrame(fSSTButtonOK);
  fButtons->AddFrame(fSSTButtonSAVE);
  fButtons->AddFrame(fSSTButtonCANCEL);

  fSST->AddFrame(fSSTName,fSSTNameLay);
  fSST->AddFrame(fSub01,fLay);
  fSST->AddFrame(fButtons);

  fSSTOpt = kLHintsCenterX | kLHintsExpandX;
  TGLayoutHints *fSSTLay = new TGLayoutHints(fSSTOpt,0,0,0,0);

  AddFrame(fSST,fSSTLay);

  MapSubwindows();
  SetWindowName("Set Shaping Time");
  SetIconName("Set Shaping Time");

  Layout();
  Resize();
  CenterOnParent();
  MapWindow();
  gClient->WaitFor(this);
 //  printf("after waitfor\n");
//   sleep(1);
//   printf("after 1 sec\n");
//   sleep(1);
//   printf("after 2 sec\n");
//   sleep(1);
//   printf("after 3 sec\n");
}


Bool_t SetShapingTime::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  //printf("welcome to process message of setshapingtime\n");
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



void SetShapingTime::ProcessTextButton(Long_t parm1, Long_t parm2){
  //printf("welcome to processtextbutton\n");
  switch (parm1) {
  case kSSTTB_OK:
  case kSSTTB_SAVE:
    Int_t aMessage;
    control->Shaping = atof(fSSTTb12->GetString());
    if (control->Shaping <  0.0) control->Shaping= 0.0;
    if (control->Shaping > 10.0) control->Shaping=10.0;
    aMessage=MK_MSG((EWidgetMessageTypes)kC_SETSHAPINGTIME,
                  (EWidgetMessageTypes)parm1);
    SendMessage(fMain,aMessage,0,0);
    CloseWindow();
    break;

  case kSSTTB_CANCEL:
    CloseWindow();
    break;
  }	
}

void SetShapingTime::CloseWindow(){

  //printf("before close\n");
  //TGTransientFrame::CloseWindow();
  DeleteWindow();
  //printf("after close\n");
  //sleep(1);
  //printf("after 1 sec\n");
}

SetShapingTime::~SetShapingTime()
{
#include "shm_detach.code"
//   delete fSSTf11; 
//   delete fSSTf12; delete fSSTTb12;
//   delete fSSTf13; 
//   delete fSSTButtonOK;
//   delete fSSTButtonSAVE;
//   delete fSSTButtonCANCEL;
//   delete fSST;       
//   printf("welcome to the setshaping time destructor !\n");
//   fflush(stdout);
//   printf("deleting...\n");
//   fflush(stdout);
  //#include "shm_detach.code"
//   delete fSSTf11; 
//   delete fSSTf12; delete fSSTTb12;
//   delete fSSTf13; 
//   delete fSSTButtonOK;
//   delete fSSTButtonSAVE;
//   delete fSSTButtonCANCEL;
//   delete fSST;       
//   delete fMain;
}










