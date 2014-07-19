//  $Id: TGTextEntry.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
//*CMZ :  2.00/00 23/02/98  16.13.28  by  Fons Rademakers
//*-- Author :    Fons Rademakers   08/01/98

//*KEEP,CopyRight,T=C.
/*************************************************************************
 * Copyright(c) 1995-1998, The ROOT System, All rights reserved.         *
 * Authors: Rene Brun, Nenad Buncic, Valery Fine, Fons Rademakers.       *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/
//*KEEP,CopyLeft.
/**************************************************************************

    This source is based on Xclass95, a Win95-looking GUI toolkit.
    Copyright (C) 1996, 1997 David Barth, Ricky Ralston, Hector Peraza.

    Xclass95 is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

**************************************************************************/
//*KEND.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGTextEntry                                                          //
//                                                                      //
// A TGTextEntry is a one line text input widget.                       //
//                                                                      //
// Changing text in the text entry widget will generate the event:      //
// kC_TEXTENTRY, kTE_TEXTCHANGED, widget id, 0.                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//*KEEP,TGTextEntry.
#include <TGTextEntry.h>
//*KEEP,TSystem.
#include <TSystem.h>
//*KEEP,TMath.
#include <TMath.h>
//*KEEP,TTimer,T=C++.
#include <TTimer.h>
//*KEND.
#include <iostream.h>


//______________________________________________________________________________
class TBlinkTimer : public TTimer {
private:
   TGTextEntry   *fTextEntry;
public:
   TBlinkTimer(TGTextEntry *t, Long_t ms) : TTimer(ms, kTRUE) { fTextEntry = t; }
   Bool_t Notify();
};

//______________________________________________________________________________
Bool_t TBlinkTimer::Notify()
{
   fTextEntry->HandleTimer(0);
   Reset();
   return kFALSE;
}



ClassImp(TGTextEntry)

//______________________________________________________________________________
TGTextEntry::TGTextEntry(const TGWindow *p, TGTextBuffer *text, Int_t id,
                         GContext_t norm, FontStruct_t font, UInt_t options,
                         ULong_t back) : TGFrame(p, 1, 1, options, back)
{
   // Create a text entry widget. It will adopt the TGTextBuffer object
   // (i.e. the text buffer will be deleted by the text entry widget).

   fWidgetId    = id;
   fWidgetType  = "TGTextEntry";
   fWidgetFlags = kWidgetWantFocus;
   fMsgWindow   = p;

   fNormGC     = norm;
   fSelGC      = fgDefaultSelectedGC;
   fSelbackGC  = fgDefaultSelectedBackgroundGC;
   fFontStruct = font;
   fText       = text;

   int tw, max_ascent, max_descent;
   tw = gVirtualX->TextWidth(fFontStruct, fText->GetString(), fText->GetTextLength());
   gVirtualX->GetFontProperties(fFontStruct, max_ascent, max_descent);

   Resize(tw + 8, max_ascent + max_descent + 7);

   fCursorX     = 4;
   fCursorIX    = fStartIX = fEndIX = 0;
   fSelectionOn = fCursorOn = kFALSE;
   fCurBlink    = 0;

   gVirtualX->SetCursor(fId, fgDefaultCursor);

   gVirtualX->GrabButton(fId, kAnyButton, kAnyModifier,
                    kButtonPressMask | kButtonReleaseMask | kPointerMotionMask,
                    kNone, kNone);

   gVirtualX->SelectInput(fId, kKeyPressMask | kExposureMask | kFocusChangeMask);
}

//______________________________________________________________________________
TGTextEntry::~TGTextEntry()
{
   // Delete a text entry widget.

   delete fText;
   delete fCurBlink;
}

//______________________________________________________________________________
void TGTextEntry::DoRedraw()
{
   // Draw the text entry widget.

   int x, y, max_ascent, max_descent;

   TGFrame::DoRedraw();

   x = 4; // do not center text
   y = 4;

   gVirtualX->GetFontProperties(fFontStruct, max_ascent, max_descent);
   gVirtualX->DrawString(fId, fNormGC, x, y + max_ascent,
                    fText->GetString(), fText->GetTextLength());

   if (fCursorOn) {
      gVirtualX->DrawLine(fId, fgBlackGC, fCursorX, 3,
                     fCursorX, max_ascent + max_descent + 3);
//  } else {
//     gVirtualX->DrawLine(fId, fgWhiteGC, fCursorX, 4,
//                    fCursorX, max_ascent + max_descent + 1);
   }

   if (fSelectionOn) {
      int xs, ws, ixs, iws;

      xs  = TMath::Min(fStartX, fEndX);
      ws  = TMath::Abs(fEndX - fStartX);
      ixs = TMath::Min(fStartIX, fEndIX);
      iws = TMath::Abs(fEndIX - fStartIX);

      gVirtualX->FillRectangle(fId, fSelbackGC, x+xs, y-1,
                          ws, max_ascent + max_descent +1);

      // fText->Draw(fId, fNormGC, x, y + max_ascent);

      gVirtualX->DrawString(fId, fSelGC, x+xs, y + max_ascent,
                       (fText->GetString())+ixs, iws);
   }

   // hmmm...
   DrawBorder();
}

//______________________________________________________________________________
void TGTextEntry::DrawBorder()
{
   // Draw the border of the text entry widget.

   switch (fOptions & (kSunkenFrame | kRaisedFrame | kDoubleBorder)) {
      case kSunkenFrame | kDoubleBorder:
         gVirtualX->DrawLine(fId, fgShadowGC, 0, 0, fWidth-2, 0);
         gVirtualX->DrawLine(fId, fgShadowGC, 0, 0, 0, fHeight-2);
         gVirtualX->DrawLine(fId, fgBlackGC, 1, 1, fWidth-3, 1);
         gVirtualX->DrawLine(fId, fgBlackGC, 1, 1, 1, fHeight-3);

         gVirtualX->DrawLine(fId, fgHilightGC, 0, fHeight-1, fWidth-1, fHeight-1);
         gVirtualX->DrawLine(fId, fgHilightGC, fWidth-1, fHeight-1, fWidth-1, 0);
         gVirtualX->DrawLine(fId, fgBckgndGC,  1, fHeight-2, fWidth-2, fHeight-2);
         gVirtualX->DrawLine(fId, fgBckgndGC,  fWidth-2, 1, fWidth-2, fHeight-2);
         break;

      default:
         TGFrame::DrawBorder();
         break;
   }
}

//______________________________________________________________________________
Int_t TGTextEntry::GetCharacterIndex(Int_t xcoord)
{
   // Get character postion at pixel position xcoord.

   int tw, ix, up, down, len;

   // check for out of boundaries first...
   len = fText->GetTextLength();
   tw = gVirtualX->TextWidth(fFontStruct, fText->GetString(), len);
   if (xcoord < 0) return 0;
   if (xcoord > tw) return len; // len-1

   // do a binary approximation
   up = len; //-1
   down = 0;
   while (up-down > 1) {
      ix = (up+down) >> 1;
      tw = gVirtualX->TextWidth(fFontStruct, fText->GetString(), ix);
      if (tw > xcoord)
         up = ix;
      else
         down = ix;
      if (tw == xcoord) break;
   }
   ix = down;

   // safety check...
   ix = TMath::Max(ix, 0);
   ix = TMath::Min(ix, len); // len-1

   return ix;
}

//______________________________________________________________________________
Bool_t TGTextEntry::HandleButton(Event_t *event)
{
   // Handle mouse button event in text entry widget.

   int x;

   if (event->fType == kButtonPress) {

      gVirtualX->SetInputFocus(fId);

      if (event->fCode == kButton1) {

         x = 4;
         fStartIX     = GetCharacterIndex(event->fX - x);
         fStartX      = gVirtualX->TextWidth(fFontStruct, fText->GetString(), fStartIX);
         fCursorIX    = fStartIX;
         fCursorX     = fStartX + x;
         fSelectionOn = kFALSE;
         DoRedraw();

      } else if (event->fCode == kButton2) {

         if (gVirtualX->GetPrimarySelectionOwner() == kNone) {
            // No primary selection, so use the cut buffer
            PastePrimary(fClient->GetRoot()->GetId(), kCutBuffer, kFALSE);
         } else {
            gVirtualX->ConvertPrimarySelection(fId, event->fTime);
         }

      }
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGTextEntry::HandleDoubleClick(Event_t *)
{
   // Handle double click event in the text entry widget.

   gVirtualX->SetInputFocus(fId);

   int x        = 4;
   fStartIX     = 0;
   fStartX      = gVirtualX->TextWidth(fFontStruct, fText->GetString(), fStartIX);
   fEndIX       = fText->GetTextLength();
   fEndX        = gVirtualX->TextWidth(fFontStruct, fText->GetString(), fEndIX);
   fCursorIX    = fEndIX;
   fCursorX     = fEndX + x;
   fSelectionOn = kTRUE;

   DoRedraw();

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGTextEntry::HandleMotion(Event_t *event)
{
   // Handle mouse motion event in the text entry widget.

   int x = 4;

   fEndIX       = GetCharacterIndex(event->fX - x); // + 1;
   fEndX        = gVirtualX->TextWidth(fFontStruct, fText->GetString(), fEndIX);
   fCursorIX    = fEndIX;
   fCursorX     = fEndX + x;
   fSelectionOn = kTRUE;

   DoRedraw();

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGTextEntry::HandleKey(Event_t *event)
{
   // Handle keyboard event in text entry widget.

   int    n, len;
   Bool_t text_changed = kFALSE;
   char   tmp[10];
   Int_t  keysym;

   len = fText->GetTextLength();
   gVirtualX->LookupString(event, tmp, sizeof(tmp), keysym);
   n = strlen(tmp);

   switch (keysym) {
      case kSymRight:
         if (fSelectionOn) fSelectionOn = kFALSE;
         if (fCursorIX < len) ++fCursorIX;
         break;

      case kSymLeft:
         if (fSelectionOn) fSelectionOn = kFALSE;
         if (fCursorIX > 0) --fCursorIX;
         break;

      case kSymHome:
         if (fSelectionOn) fSelectionOn = kFALSE;
         fCursorIX = 0;
         break;

      case kSymEnd:
         if (fSelectionOn) fSelectionOn = kFALSE;
         fCursorIX = len;
         break;

      case kSymDelete:
         if (fSelectionOn) {
            int start, ns;

            start = TMath::Min(fStartIX, fEndIX);
            ns = TMath::Abs(fEndIX - fStartIX);
            fText->RemoveText(start, ns);
            fSelectionOn = kFALSE;
            fCursorIX = start;
         } else {
            fText->RemoveText(fCursorIX, 1);
         }
         text_changed = kTRUE;
         break;

      case kSymBackSpace:
         if (fSelectionOn) {
            int start, ns;

            start = TMath::Min(fStartIX, fEndIX);
            ns = TMath::Abs(fEndIX - fStartIX);
            fText->RemoveText(start, ns);
            fSelectionOn = kFALSE;
            fCursorIX = start;
         } else {
            if (fCursorIX > 0) {
               --fCursorIX;
               fText->RemoveText(fCursorIX, 1);
            }
         }
         text_changed = kTRUE;
         break;

      case kSymUp:
      case kSymDown:
      case kSymEscape:
      case kSymExecute:
      case kSymReturn:
         break;

      default:
         if (fSelectionOn) {
            int start, ns;

            start = TMath::Min(fStartIX, fEndIX);
            ns = TMath::Abs(fEndIX - fStartIX);
            fText->RemoveText(start, ns);
            fSelectionOn = kFALSE;
            fCursorIX = start;
         }
         fText->AddText(fCursorIX, tmp);
         len = fText->GetTextLength();
         fCursorIX += n;
         if (fCursorIX > len) fCursorIX = len;
         text_changed = kTRUE;
         break;
   }
   if (text_changed || keysym==10){
      SendMessage(fMsgWindow, MK_MSG(kC_TEXTENTRY, kTE_TEXTCHANGED),
                  fWidgetId, keysym);
   }
   fCursorX = 4 + gVirtualX->TextWidth(fFontStruct, fText->GetString(), fCursorIX);
   fClient->NeedRedraw(this);

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGTextEntry::HandleFocusChange(Event_t *event)
{
   // Handle focus change event in text entry widget.

   // check this when porting to Win32
   if ((event->fCode == kNotifyNormal) && (event->fState != kNotifyPointer)) {
      if (event->fType == kFocusIn) {
         fCursorOn = kTRUE;
         if (!fCurBlink) fCurBlink = new TBlinkTimer(this, 500);
         fCurBlink->Reset();
         gSystem->AddTimer(fCurBlink);
      } else {
          fCursorOn    = kFALSE;
          fSelectionOn = kFALSE;
          if (fCurBlink) fCurBlink->Remove();
      }
      fClient->NeedRedraw(this);
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGTextEntry::HandleSelection(Event_t *event)
{
   // Handle text selection event.

   PastePrimary((Window_t)event->fUser[0], (Atom_t)event->fUser[3], kTRUE);

   return kTRUE;
}

//______________________________________________________________________________
void TGTextEntry::PastePrimary(Window_t wid, Atom_t property, Bool_t del)
{
   // Paste text from selection (either primary or cut buffer) into
   // text entry widget.

   TString data;
   Int_t   nchar;

   gVirtualX->GetPasteBuffer(wid, property, data, nchar, del);

   if (nchar) {
      // remove any previous selected text
      if (fSelectionOn) {
         int start, ns;

         start = TMath::Min(fStartIX, fEndIX);
         ns = TMath::Abs(fEndIX - fStartIX);
         fText->RemoveText(start, ns);
         fSelectionOn = kFALSE;
         fCursorIX = start;
      }

      fText->AddText(fCursorIX, data.Data(), nchar);
      int len = fText->GetTextLength();
      fCursorIX += nchar;
      if (fCursorIX > len) fCursorIX = len;
   }

   SendMessage(fMsgWindow, MK_MSG(kC_TEXTENTRY, kTE_TEXTCHANGED),
               fWidgetId, 0);
   fCursorX = 4 + gVirtualX->TextWidth(fFontStruct, fText->GetString(), fCursorIX);
   fClient->NeedRedraw(this);
}

//______________________________________________________________________________
Bool_t TGTextEntry::HandleTimer(TTimer *)
{
   // Handle cursor blink timer.

   fCursorOn = !fCursorOn;
   DoRedraw();

   return kTRUE;
}
