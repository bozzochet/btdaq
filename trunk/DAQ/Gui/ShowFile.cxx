#include "ShowFile.h"
#include "Messages.h"
#include <iostream>
ShowFile::ShowFile(const TGWindow *p, const TGWindow *main, 
		     UInt_t w, UInt_t h, const char *Title, const char *FileName) 
  : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);
  //cerr << FileName << endl;

  fMain=main;
  UInt_t fSFOpt = kVerticalFrame;
  fSF = new TGCompositeFrame(this,w,h,fSFOpt);

  h = (int) (0.90*h);
  fSFText = new TGTextView(fSF,w,h,kSunkenFrame | kDoubleBorder);
  fSFText->LoadFile(FileName);



  fSFButtonDISMISS     = new TGTextButton(fSF,"DISMISS",kSFTB_DISMISS);
  fSFButtonDISMISS->Resize(60,fSFButtonDISMISS->GetDefaultHeight());
  fSFButtonDISMISS->Associate(this);
  TGLayoutHints *fSFButtonLay = new TGLayoutHints(kLHintsCenterX,5,5,10,10);


  // Placing everything together

  fSF->AddFrame(fSFText);
  fSF->AddFrame(fSFButtonDISMISS,fSFButtonLay);
  AddFrame(fSF);

  MapSubwindows();
  SetWindowName(Title);
  SetIconName(Title);

  Layout();
  Resize();
  CenterOnParent();
  MapWindow();

}


Bool_t ShowFile::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
    case kCM_BUTTON:
      switch (parm1) {
      case kSFTB_DISMISS:
	CloseWindow();
	break;
      }
      break;
    }
  }
  return kTRUE;
}




void ShowFile::CloseWindow(){
  //TGTransientFrame::CloseWindow();
  DeleteWindow();
}

ShowFile::~ShowFile()
{
//   delete fSFText;
//   delete fSFButtonDISMISS;
//   delete fSF;       
//   delete fMain;
}
