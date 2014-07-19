#include "SelectLadder.h"
#include "Messages.h"

SelectLadder::SelectLadder(const TGWindow *p, const TGWindow *main, 
			   UInt_t w, UInt_t h) 
  : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);
  char file[200];


  // Creating share memory segments
#include "shm_definition.code"

  int LastIsNotTest=strcmp(control->fLadderNameAtExit,"testladder"); 

  fMain=main;
  UInt_t fSLOpt = kVerticalFrame;
  fSL = new TGCompositeFrame(this,250,0,fSLOpt);
 
  TGLayoutHints *fSLNameLay = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
  TGLabel *fSLName = new TGLabel(fSL,"Select Ladder");//,ffontcon[1],ffont[1]);
  fSLName->SetTextFont(DefineFonts::fFont[1]);


  TGCompositeFrame *fButtons = new TGCompositeFrame(fSL,200,0,kVerticalFrame);
  fButtons->SetLayoutManager(new TGMatrixLayout(fButtons,0,1,5,kLHintsLeft));



  fSLButtonTest     = new TGTextButton(fButtons,
			  "        Test Ladder        ",kSLTB_TEST);
  fSLButtonTest->Resize(fSLButtonTest->GetDefaultWidth(),
			fSLButtonTest->GetDefaultHeight());
  fSLButtonTest->Associate(this);

  fSLButtonAMS   = new TGTextButton(fButtons,
			  "        AMS Ladder         " ,kSLTB_AMS);
  fSLButtonAMS->Resize(fSLButtonTest->GetDefaultWidth(),
		       fSLButtonAMS->GetDefaultHeight());
  fSLButtonAMS->Associate(this);

  fSLButtonHybrid   = new TGTextButton(fButtons,
			  "        AMS Hybrid         " ,kSLTB_HYBRID);
  fSLButtonHybrid->Resize(fSLButtonTest->GetDefaultWidth(),
		       fSLButtonHybrid->GetDefaultHeight());
  fSLButtonHybrid->Associate(this);


  if (LastIsNotTest) {
    fSLButtonLast = new TGTextButton(fButtons,Form("        %s        ",control->fLadderNameAtExit),kSLTB_LAST);
    fSLButtonLast->Resize(fSLButtonTest->GetDefaultWidth(),
			  fSLButtonLast->GetDefaultHeight());
    fSLButtonLast->Associate(this);
  }

  // Placing everything together

  fSLOpt = kLHintsCenterX;
  TGLayoutHints *fSLLay = new TGLayoutHints(fSLOpt,0,0,0,0);

  if (LastIsNotTest) fButtons->AddFrame(fSLButtonLast,fSLLay);
  fButtons->AddFrame(fSLButtonTest,fSLLay);
  fButtons->AddFrame(fSLButtonAMS,fSLLay);
  fButtons->AddFrame(fSLButtonHybrid,fSLLay);

  fSL->AddFrame(fSLName,fSLNameLay);  
  fSL->AddFrame(fButtons,fSLLay);

  AddFrame(fSL,fSLLay);

  MapSubwindows();
  SetWindowName("Select Ladder");
  SetIconName("Select Ladder");

  Layout();
  Resize();
  CenterOnParent();
  MapWindow();
  gClient->WaitFor(this);
}


Bool_t SelectLadder::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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



void SelectLadder::ProcessTextButton(Long_t parm1, Long_t parm2){
  switch (parm1) {
  case kSLTB_LAST:
    control->LadderType=kLadderLast;
    break;
  case kSLTB_TEST:
    control->LadderType=kLadderTest;
    break;
  case kSLTB_AMS:
    control->LadderType=kLadderAMS;
    break;
  case kSLTB_HYBRID:
    control->LadderType=kLadderHybrid;
  }	
  //toCTL->Send(kGUI,kNewLadder,0);    
  CloseWindow();  
}

void SelectLadder::CloseWindow(){
  //TGTransientFrame::CloseWindow();
  DeleteWindow();
}

SelectLadder::~SelectLadder()
{
#include "shm_detach.code"
//   delete fSLButtonTest;
//   delete fSLButtonAMS;
//   delete fSLButtonHybrid;
//   delete fSL;       
//   delete fMain;
}







