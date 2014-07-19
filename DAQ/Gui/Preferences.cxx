#include "SetPreferences.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>


SetPreferences::SetPreferences(const TGWindow *p, const TGWindow *main, 
		     UInt_t w, UInt_t h) 
  : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);
#include "shm_definition.code"

  fMain=main;  


  fFrame1 = new TGHorizontalFrame(this, 60, 20, kFixedWidth);
  
  fOkButton = new TGTextButton(fFrame1, "&Ok", 1);
  fOkButton->Associate(this);
  fCancelButton = new TGTextButton(fFrame1, "&Cancel", 2);
  fCancelButton->Associate(this);
  
  fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
			  2, 2, 2, 2);
  fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);
  
  fFrame1->AddFrame(fOkButton, fL1);
  fFrame1->AddFrame(fCancelButton, fL1);
  
  fFrame1->Resize(150, fOkButton->GetDefaultHeight());
  AddFrame(fFrame1, fL2);
  

//   UInt_t fSPrefOpt = kVerticalFrame;
//   TGCompositeFrame *fSPref = new TGCompositeFrame(this,190,0,fSPrefOpt);
 
//   TGLabel *fSPrefName = new TGLabel(fSPref,"Preferences",ffontcon[1],ffont[1]);

//   fSPrefOpt = kHorizontalFrame;
//   TGCompositeFrame *fSPrefData = new TGCompositeFrame(fSPref,180,0,fSPrefOpt);

  fTab = new TGTab(this, 300, 300);
  fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);

  TGCompositeFrame *tf = fTab->AddTab("Colors");
  fF1 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
  tf->AddFrame(fF1, fL3);

  tf = fTab->AddTab("Hardware");
  tf->AddFrame(fF2, fL3);

  TGLayoutHints *fL5 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
                                          kLHintsExpandY, 2, 2, 5, 1);
   AddFrame(fTab, fL5);

   MapSubwindows();
   Resize();   // resize to default size

   // position relative to the parent's window
   CenterOnParent();

   SetWindowName("Dialog");

   MapWindow();
   //fClient->WaitFor(this); 
} 
