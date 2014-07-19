//  $Id: Help.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
 
// Copyright (C) 1998 CodeCERN. All rights reserved.
 
#ifndef ROOT_AMSHelp
#define ROOT_AMSHelp
 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSHelp                                                     //
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
class TGText;
class TGTextBuffer;
class TGTextEntry;
class TGTextButton;
 
 
class AMSHelp : public TGTransientFrame {
 
protected:
   TGTextView        *fText;     // TGTextView to post a note
   TGText            *fTextb;    // text buffer for
   TGTextButton      *fOk;       // button "Ok"
   TGLayoutHints     *fLmain;    // layout in main frame
   TGLayoutHints     *fLok;    // layout in main frame

public:
   AMSHelp(const TGWindow *p, const TGWindow *main,
                char *fname);
   virtual ~AMSHelp();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   virtual void CloseWindow();

};
 
#endif
