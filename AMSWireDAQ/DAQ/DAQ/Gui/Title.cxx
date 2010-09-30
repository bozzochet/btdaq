#include "MainFrame.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////
// CreateTitle
// Create a Title Banner
////////////////////////////////////////////////////////////////////////////
void MainFrame::Title(){

  fTitleOpt = kHorizontalFrame | kRaisedFrame;
  fTitle = new TGCompositeFrame(this,800,100,fTitleOpt);  

  TString icon;
  icon.Append(getenv("PWD"));
  icon.Append("/GUI");
  icon.Append("/AMSlogo.xpm");
  fLogoUGLay = new TGLayoutHints(kLHintsLeft,0,0,0,0);
  fLogoUG = new TGPictureButton(fTitle,fClient->GetPicture(icon.Data()),1);
  fLogoUG->SetCommand("printf(\"Logo Universite de Geneve\\n\");");

  // Define Text attributes

  fTextFrameLay = 
    new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 0, 0, 0, 0);
  fTextFrame = new TGCompositeFrame(fTitle, 0, 0, kVerticalFrame);

  fTitleTextLay[0] = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
  fTitleText[0]    = new TGLabel(fTextFrame,"AMS tracker Test System");//,ffontcon[0],ffont[0]);
  fTitleText[0]->SetTextFont(DefineFonts::fFont[0]);
  fTitleTextLay[1] = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
  fTitleText[1]    = new TGLabel(fTextFrame,"AMS tracker Test System");//,ffontcon[1],ffont[1]);
  fTitleText[1]->SetTextFont(DefineFonts::fFont[1]);

  fTitle->AddFrame(fLogoUG, fLogoUGLay);
  fTextFrame->AddFrame(fTitleText[0], fTitleTextLay[0]);
  fTextFrame->AddFrame(fTitleText[1], fTitleTextLay[1]);
  fTitle->AddFrame(fTextFrame, fTextFrameLay);
}



