#include "SetPrinter.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>



SetPrinter::SetPrinter(const TGWindow *p, const TGWindow *main, UInt_t w, 
		       UInt_t h, TRootEmbeddedCanvas *fCanvas,
		       const char *fName) 
  : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);

  char value[200];

  // Creating share memory segments
  char file[200];
  #include "shm_definition.code"


  fMain=main;
  myCanvas = fCanvas;
  fSPr = new TGCompositeFrame(this,300,0,kVerticalFrame);
 
  TGLabel *fSPrName = new TGLabel(fSPr,"Print Canvas");//,ffontcon[1],ffont[1]);
  fSPrName->SetTextFont(DefineFonts::fFont[1]);

  TGCompositeFrame *fSub1 = new TGCompositeFrame(fSPr,200,0,kHorizontalFrame);
  fSPrf11=new TGCheckButton(fSub1,"Printer Command  ",-1);
    fSPrf11->Resize(100,fSPrf11->GetDefaultHeight());
    fSPrf11->SetState(kButtonDown);
  fSPrTb12 = new TGTextBuffer(1034);
  fSPrf12 = new TGTextEntry(fSub1,fSPrTb12,-1);
    fSPrf12->Resize(150,fSPrf12->GetDefaultHeight());
    fSPrTb12->Clear();
    //    sprintf(value,"lpr -P%s",getenv("PRINTER"));
    sprintf(value,"%s %s %s", control->PrintCmd,(strcmp(control->PrintOpt,"EMPTY_OPTION"))?control->PrintOpt:"", (strcmp(control->Printer,"EMPTY_OPTION"))?control->Printer:"");
    fSPrTb12->AddText(0,value);
    fSPrf12->Associate(this);



  TGCompositeFrame *fSub2 = new TGCompositeFrame(fSPr,200,0,kHorizontalFrame);

  fSPrf21=new TGCheckButton(fSub2,"File                    ",-1);
    fSPrf21->Resize(100,fSPrf21->GetDefaultHeight());
    fSPrf21->SetState(kButtonDown);
 
  fSPrTb22 = new TGTextBuffer(1034);
  fSPrf22 = new TGTextEntry(fSub2,fSPrTb22,-1);
    fSPrf22->Resize(150,fSPrf22->GetDefaultHeight());
    fSPrTb22->Clear();
    sprintf(value,"%s",fName);
    fSPrTb22->AddText(0,value);
    fSPrf22->Associate(this);

  TGCompositeFrame *fSub3 = new TGCompositeFrame(fSPr,200,0,kHorizontalFrame);

  fSPrf31 = new TGLabel(fSub3,"                         ");//,ffontcon[2],ffont[2]);
  fSPrf31->SetTextFont(DefineFonts::fFont[2]);
  fSPrf31->Resize(100,fSPrf31->GetDefaultHeight());
  fSPrf31->SetTextJustify(kTextLeft);
  
  fSPrf32=new TGCheckButton(fSub3,"ps",-1);
  fSPrf32->Resize(50,fSPrf32->GetDefaultHeight());
  fSPrf32->SetState(kButtonUp);

  fSPrf33=new TGCheckButton(fSub3,"eps",-1);
    fSPrf33->Resize(50,fSPrf32->GetDefaultHeight());
    fSPrf33->SetState(kButtonDown);
 
  TGCompositeFrame *fButtons = new TGCompositeFrame(fSPr,200,0,
						    kHorizontalFrame);
  fButtons->SetLayoutManager(new TGMatrixLayout(fButtons,1,0,5,5));


  fSPrButtonOK     = new TGTextButton(fButtons,"OK"    ,kSPr_OK);
  fSPrButtonOK->Resize(60,fSPrButtonOK->GetDefaultHeight());
  fSPrButtonOK->Associate(this);


  fSPrButtonCANCEL = new TGTextButton(fButtons,"CANCEL",kSPr_CANCEL);
  fSPrButtonCANCEL->Resize(60,fSPrButtonCANCEL->GetDefaultHeight());
  fSPrButtonCANCEL->Associate(this);



  // Placing everything together
  TGLayoutHints *fLay=new TGLayoutHints(kLHintsLeft | kLHintsExpandX,5,0,5,5);
  TGLayoutHints *fLayButtons=new TGLayoutHints(kLHintsCenterX,
					       5,0,5,5);

  fSub1->AddFrame(fSPrf11); fSub1->AddFrame(fSPrf12);    
  fSub2->AddFrame(fSPrf21); fSub2->AddFrame(fSPrf22);    
  fSub3->AddFrame(fSPrf31); fSub3->AddFrame(fSPrf32); 
                             fSub3->AddFrame(fSPrf33);   

  fButtons->AddFrame(fSPrButtonOK);
  fButtons->AddFrame(fSPrButtonCANCEL);
			     

  fSPr->AddFrame(fSPrName,fLay);			     
  fSPr->AddFrame(fSub1,fLay);
  fSPr->AddFrame(fSub2,fLay);
  fSPr->AddFrame(fSub3,fLay);
  fSPr->AddFrame(fButtons,fLayButtons);

  TGLayoutHints *fSPrLay = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX,
					     0,0,0,0);

  AddFrame(fSPr,fSPrLay);

  MapSubwindows();
  SetWindowName("Print Canvas");
  SetIconName("Print Canvas");

  Layout();
  Resize();
  CenterOnParent();
  MapWindow();
  gClient->WaitFor(this);
}


Bool_t SetPrinter::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  printf("welcome to process message\n");
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



void SetPrinter::ProcessTextButton(Long_t parm1, Long_t parm2){
  int PrintCanvas;
  int SaveCanvas;
  char PrinterCommand[100];
  char CanvasFile[100];
  int PsCanvasFile;
  int EpsCanvasFile;
  char command[200];

  switch (parm1) {
  case kSPr_OK:
    Int_t aMessage;

    PrintCanvas=0;
    SaveCanvas=0;
    sprintf(PrinterCommand," ");
    sprintf(CanvasFile," ");
    PsCanvasFile=0;
    EpsCanvasFile=0;
    
    if (fSPrf11->GetState() == kButtonDown){
      PrintCanvas=1;
      strcpy(PrinterCommand,fSPrTb12->GetString());
    }
    if (fSPrf21->GetState() == kButtonDown){
      SaveCanvas=1;
      strcpy(CanvasFile,fSPrTb22->GetString());
      if (fSPrf32->GetState()==kButtonDown) PsCanvasFile=1;
      if (fSPrf33->GetState()==kButtonDown) EpsCanvasFile=1;  
    }
    //    if (parm1 == kSCPTB_SAVE) { } Save Calibration numbers on file
    
    //TCanvas *c1 = myCanvas->GetCanvas();
    if (PrintCanvas != 0) { 
      //      TPostScript myprt("myfile.ps",112);
      //myCanvas->GetCanvas()->Draw();
      myCanvas->GetCanvas()->Print("myfile.ps");
      //myprt.Close();
      sprintf(command,"%s myfile.ps",PrinterCommand); 
      system(command);
    }
    if (SaveCanvas != 0) {
      if (PsCanvasFile != 0) {
	sprintf(command,"%s.ps",CanvasFile);
	//TPostScript myps(command,112);
	myCanvas->GetCanvas()->Print(command);
	//myps.Close();
      }
      if (EpsCanvasFile != 0) {
	sprintf(command,"%s.eps",CanvasFile);
	//TPostScript myeps(command,113);
	myCanvas->GetCanvas()->Print(command);
	//myeps.Close();
      }
    }

    aMessage=MK_MSG((EWidgetMessageTypes)kC_SETPRINTER,
                  (EWidgetMessageTypes)parm1);
    SendMessage(fMain,aMessage,0,0);
    CloseWindow();
    break;

  case kSPr_CANCEL:
    CloseWindow();
    break;
  }	
}

void SetPrinter::CloseWindow(){
  //TGTransientFrame::CloseWindow();
  DeleteWindow();
}

SetPrinter::~SetPrinter()
{
  #include "shm_detach.code"
//   delete fSPrf11; delete fSPrf12; delete fSPrTb12;
//   delete fSPrf21; delete fSPrf22; delete fSPrTb22;
//   delete fSPrf31; delete fSPrf32; delete fSPrf33;
//   delete fSPrButtonOK;
//   delete fSPrButtonCANCEL;
//   delete fSPr;       
//   delete fMain;
}
