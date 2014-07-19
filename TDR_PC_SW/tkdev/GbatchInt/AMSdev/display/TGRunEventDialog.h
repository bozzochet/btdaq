//  $Id: TGRunEventDialog.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
 
// Copyright (C) 1998 CodeCERN. All rights reserved.
 
#ifndef ROOT_TGRunEventDialog
#define ROOT_TGRunEventDialog
 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGRunEventDialog                                                     //
//                                                                      //
// This class creates a event-changing  dialog. It contains two enter   //
// regions to input run number and event number of an event. If run     //
// =-9, then the event number is taken as for the file, not for a run.  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
#ifndef ROOT_TGFrame
#include <TGFrame.h>
#endif
 
 
class TGTextView;
class TGLabel;
class TGTextBuffer;
class TGTextEntry;
class TGTextButton;
 
 
class TGRunEventDialog : public TGTransientFrame {
 
protected:
   Int_t             *fRun;      // pointer to run number
   Int_t             *fEvent;    // pointer to event number
   const static char *fLines;    // lines in TGTextFrame fText
   TGTextView        *fText;     // TGTextView to post a note
   TGLabel           *fLrun;     // "Run" label
   TGLabel           *fLevt;     // "Event" label
   TGTextBuffer      *fTbrun;    // text buffer for fTrun
   TGTextBuffer      *fTbevt;    // text buffer for fTevt
   TGTextEntry       *fTrun;     // text entry for run number
   TGTextEntry       *fTevt;     // text entry for event number
   TGTextButton      *fOk;       // button "Ok"
   TGTextButton      *fClear;    // button "Clear"
   TGTextButton      *fCancel;   // button "Cancel"
   TGVerticalFrame   *fVenter;   // frame to contain fHrun and fHevt
   TGHorizontalFrame *fHrun;     // frame to contain fLrun and fTrun
   TGHorizontalFrame *fHevt;     // frame to contain fLevt and fTevt
   TGHorizontalFrame *fHb;       // frame to contain buttons
   TGLayoutHints     *fLmain;    // layout in main frame
   TGLayoutHints     *fLv;       // layout in fVenter
   TGLayoutHints     *fLhl;      // layout for textLabel of run and event     
   TGLayoutHints     *fLht;      // layout for textEntry of run and event
   TGLayoutHints     *fLhb;      // layout for buttons in fHb

public:
   TGRunEventDialog(const TGWindow *p, const TGWindow *main,
                Int_t *pRun, Int_t *pEvent);
   virtual ~TGRunEventDialog();
 
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   virtual void CloseWindow();

};
 
#endif
