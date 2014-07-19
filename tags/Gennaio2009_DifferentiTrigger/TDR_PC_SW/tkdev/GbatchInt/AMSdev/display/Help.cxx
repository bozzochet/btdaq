//  $Id: Help.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#include "Help.h"
#include <TGTextView.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <stdlib.h>
#include <iostream>

//_____________________________________________________________________________

AMSHelp::AMSHelp(const TGWindow *p, const TGWindow *main,
                   char *fname) :
   TGTransientFrame(p, main, 10, 10, kMainFrame | kVerticalFrame)
{

   //
   // 3 frames in global window
   //
   fLmain  = new TGLayoutHints( kLHintsExpandY | kLHintsExpandX, 4, 4, 5, 5);
   fLok  = new TGLayoutHints(  kLHintsCenterX, 1, 1, 1, 1);

   fText   = new TGTextView(this, 10, 10, -1);
//   fText->ChangeOptions(0);
    bool succ=fText->LoadFile(fname);
   if(!succ){
     fText->LoadBuffer(" Ooops  ... No Help  ....");
   }
   fText->Resize(800, 600);




   AddFrame(fText, fLmain);
   fOk     = new TGTextButton(this, new TGHotString("&Close"), 0);
   AddFrame(fOk, fLok);
   fOk->Associate(this);


   // end of definition of frames

   MapSubwindows();
   TGDimension size = GetDefaultSize();
   Resize(size);

   //---- position relative to the parent's window

   if (main) {
      int      ax, ay;
      Window_t wdummy;
      gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                            (((TGFrame *) main)->GetWidth() - fWidth) >> 1,
                            (((TGFrame *) main)->GetHeight() - fHeight) >> 1,
                              ax, ay, wdummy);
      if (ax < 0) ax = 10;
      Move(ax, ay);
      SetWMPosition(ax, ay);
   }

   //---- make the message box non-resizable

//   SetWMSize(size.fWidth, size.fHeight);
//   SetWMSizeHints(size.fWidth, size.fHeight, size.fWidth, size.fHeight, 0, 0);

   const char *wname = "Help";
   SetWindowName(wname);
   SetIconName(wname);

// SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
//                              kMWMDecorMinimize | kMWMDecorMenu,
//               kMWMFuncAll |  kMWMFuncResize    | kMWMFuncMaximize |
//                              kMWMFuncMinimize,
//               kMWMInputModeless);

   MapWindow();
//   fClient->WaitFor(this);
}

//_____________________________________________________________________________
AMSHelp::~AMSHelp()
{
   // delete RunEvent dialog

   delete fOk;  


   delete fText;

   delete fLmain;
   delete fLok;
}

//_____________________________________________________________________________
void AMSHelp::CloseWindow()
{
   delete this;
}

//_____________________________________________________________________________
Bool_t AMSHelp::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
            case kCM_BUTTON:
               switch (parm1) {
                  case 0:
                     delete this;
                     break;
               } // switch(parm1)
               break;
            default:
               break;
         } // switch(GET_SUBMSG(msg))
         break;

      default:
         break;

   } // switch(GET_MSG(msg))

   return kTRUE;
}
