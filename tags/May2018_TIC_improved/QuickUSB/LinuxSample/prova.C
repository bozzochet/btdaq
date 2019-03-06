// Mainframe macro generated from application: /home/root/bin/root.exe
// By ROOT version 5.14/00 on 2007-07-13 13:41:24

#if !defined( __CINT__) || defined (__MAKECINT__)

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGuiBldHintsButton
#include "TGuiBldHintsButton.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGColorDialog
#include "TGColorDialog.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGTextEdit
#include "TGTextEdit.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGView
#include "TGView.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include "Riostream.h"

#endif

void prova()
{

   // main frame
   TGMainFrame *fMainFrame768 = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMainFrame768->SetLayoutBroken(kTRUE);

   TGFont *ufont;         // will reflect user font changes
   ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   TGGC   *uGC;           // will reflect user GC changes
   // graphics context changes
   GCValues_t valEntry626;
   valEntry626.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry626.fForeground);
   gClient->GetColorByName("#c0c0c0",valEntry626.fBackground);
   valEntry626.fFillStyle = kFillSolid;
   valEntry626.fFont = ufont->GetFontHandle();
   valEntry626.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry626, kTRUE);
   TGTextEntry *fTextEntry626 = new TGTextEntry(fMainFrame768, new TGTextBuffer(15),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kDoubleBorder | kOwnBackground);
   fTextEntry626->SetMaxLength(255);
   fTextEntry626->SetAlignment(kTextLeft);
   fTextEntry626->SetText("fTextEntry626");
   fTextEntry626->Resize(144,fTextEntry626->GetDefaultHeight());
   fMainFrame768->AddFrame(fTextEntry626, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextEntry626->MoveResize(96,96,144,22);
   TGTextEdit *fTextEdit637 = new TGTextEdit(fMainFrame768,320,240);
   fTextEdit637->LoadFile("TxtEdit637");
   fMainFrame768->AddFrame(fTextEdit637, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextEdit637->MoveResize(96,256,320,240);

   fMainFrame768->MapSubwindows();

   fMainFrame768->Resize(fMainFrame768->GetDefaultSize());
   fMainFrame768->MapWindow();
   fMainFrame768->Resize(705,520);
}  
