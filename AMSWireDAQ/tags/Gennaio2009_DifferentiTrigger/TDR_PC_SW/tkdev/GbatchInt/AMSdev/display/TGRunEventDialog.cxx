//  $Id: TGRunEventDialog.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#include "TGRunEventDialog.h"
#include <TGTextView.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
enum {
   kIDF_OK,
   kIDF_CLEAR,
   kIDF_CANCEL
};

const char *TGRunEventDialog::fLines =
"Note: goto the event with run# and >=event#\n      if run# empty, take the current run#\n      when run#=-9, event# is for the tree";


//_____________________________________________________________________________

TGRunEventDialog::TGRunEventDialog(const TGWindow *p, const TGWindow *main,
                   Int_t *pRun, Int_t *pEvent) :
   TGTransientFrame(p, main, 10, 10, kMainFrame | kVerticalFrame)
{
   fRun = pRun; fEvent = pEvent;

   //
   // 3 frames in global window
   //
   fLmain  = new TGLayoutHints( kLHintsTop | kLHintsExpandX, 4, 4, 5, 5);

   fText   = new TGTextView(this, 10, 10, -1,TGView::kNoHSB | TGView::kNoVSB, 0);
   fText->ChangeOptions(0);
   fText->LoadBuffer(fLines);
   fText->Resize(350, 50);

   fVenter = new TGVerticalFrame(this, 10, 10);
   fHb     = new TGHorizontalFrame(this, 10, 10);

   AddFrame(fText, fLmain);
   AddFrame(fVenter, fLmain); 
   AddFrame(fHb, fLmain); 

   //
   // 2 horizonal frames in fVenter
   //
   fLv   = new TGLayoutHints( kLHintsLeft | kLHintsCenterY | kLHintsExpandX);

   fHrun = new TGHorizontalFrame(fVenter, 10, 10);
   fHevt = new TGHorizontalFrame(fVenter, 10, 10);

   fVenter->AddFrame(fHrun, fLv);
   fVenter->AddFrame(fHevt, fLv);

   //
   // 2 frames of Label and TextEntry in fHrun
   //
   fLhl   = new TGLayoutHints( kLHintsLeft | kLHintsCenterY, 5, 5, 2, 2);
   fLht   = new TGLayoutHints( kLHintsRight | kLHintsCenterY, 0, 20, 2, 2);

   fLrun  = new TGLabel(fHrun, new TGHotString("&Run#:"));
   fTbrun = new TGTextBuffer(100);
   fTrun  = new TGTextEntry(fHrun, fTbrun);
   fTrun->Resize(260, fTrun->GetDefaultHeight()); 

   fHrun->AddFrame(fLrun, fLhl);
   fHrun->AddFrame(fTrun, fLht);

   //
   // 2 frames of Label and TextEntry in fHevt
   //
   fLevt  = new TGLabel(fHevt, new TGHotString("E&vent#:"));
   fTbevt = new TGTextBuffer(100);
   fTevt  = new TGTextEntry(fHevt, fTbevt);
   fTevt->Resize(260, fTevt->GetDefaultHeight()); 

   fHevt->AddFrame(fLevt, fLhl);
   fHevt->AddFrame(fTevt, fLht);

   //
   // 3 buttons in fHb
   //
   fLhb    = new TGLayoutHints( kLHintsTop | kLHintsExpandX, 20, 20, 2, 2);

   fOk     = new TGTextButton(fHb, new TGHotString("&Ok"), kIDF_OK);
   fClear  = new TGTextButton(fHb, new TGHotString("C&lear"), kIDF_CLEAR);
   fCancel = new TGTextButton(fHb, new TGHotString("&Cancel"), kIDF_CANCEL);

   UInt_t width = fCancel->GetDefaultWidth()+20;
   UInt_t height = fCancel->GetDefaultHeight()+10;
   fOk->Resize(width, height);
   fClear->Resize(width, height);
   fCancel->Resize(width, height);

   fHb->AddFrame(fOk, fLhb);
   fHb->AddFrame(fClear, fLhb);
   fHb->AddFrame(fCancel, fLhb);

   //
   // associate buttons to the frame
   //
   fOk->Associate(this);
   fClear->Associate(this);
   fCancel->Associate(this);

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

   SetWMSize(size.fWidth, size.fHeight);
   SetWMSizeHints(size.fWidth, size.fHeight, size.fWidth, size.fHeight, 0, 0);

   const char *wname = "Goto Run/Event";
   SetWindowName(wname);
   SetIconName(wname);
   SetClassHints("RunEventDialog", "RunEventDialog");

   SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
                              kMWMDecorMinimize | kMWMDecorMenu,
               kMWMFuncAll |  kMWMFuncResize    | kMWMFuncMaximize |
                              kMWMFuncMinimize,
               kMWMInputModeless);

   MapWindow();
   fClient->WaitFor(this);
}

//_____________________________________________________________________________
TGRunEventDialog::~TGRunEventDialog()
{
   // delete RunEvent dialog

   delete fOk;  delete fClear; delete fCancel;
   delete fLhb; delete fHb;

   delete fTevt; delete fLevt; delete fHevt;
   delete fTrun; delete fLrun; delete fHrun;
   delete fLhl;  delete fLht;  delete fLv;   delete fVenter;

   delete fText;

   delete fLmain;
}

//_____________________________________________________________________________
void TGRunEventDialog::CloseWindow()
{
   // close RunEvent dialog
   *fRun = 0; *fEvent = 0;
   delete this;
}

//_____________________________________________________________________________
Bool_t TGRunEventDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
            case kCM_BUTTON:
               switch (parm1) {
                  case kIDF_OK:
                     if (fTbrun->GetTextLength() == 0) *fRun = 0;
                     else *fRun = atoi(fTbrun->GetString());
                     if (fTbevt->GetTextLength() == 0) *fEvent = 0;
                     else *fEvent = atoi(fTbevt->GetString());
                     cout << "Run =" << *fRun << "\tEvent =" << *fEvent << endl;
                     delete this;
                     break;

                  case kIDF_CLEAR:
                     fTbrun->Clear();
                     fTbevt->Clear();
                     fClient->NeedRedraw(fTrun);
                     fClient->NeedRedraw(fTevt);
//                     cout << "Clear" << endl;
                     break;

                  case kIDF_CANCEL:
//                     cout << "Cancel" << endl;
                     delete this;
                     break;

                  default:
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
