// Mainframe macro generated from application: /home/prova02/root/bin/root.exe
// By ROOT version 5.14/00 on 2007-06-07 16:43:53

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
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
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
#ifndef ROOT_TGFontDialog
#include "TGFontDialog.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
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
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include "Riostream.h"

#endif

void TestGui()
{

   // main frame
   TGMainFrame *fMainFrame2396 = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMainFrame2396->SetLayoutBroken(kTRUE);
   TGHorizontal3DLine *fHorizontal3DLine515 = new TGHorizontal3DLine(fMainFrame2396,416,8);
   fMainFrame2396->AddFrame(fHorizontal3DLine515, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fHorizontal3DLine515->MoveResize(8,40,416,8);

   // tab widget
   TGTab *fTab528 = new TGTab(fMainFrame2396,408,408);

   // container of "General"
   TGCompositeFrame *fCompositeFrame531;
   fCompositeFrame531 = fTab528->AddTab("General");
   fCompositeFrame531->SetLayoutManager(new TGVerticalLayout(fCompositeFrame531));


   // container of "Reg Test"
   TGCompositeFrame *fCompositeFrame533;
   fCompositeFrame533 = fTab528->AddTab("Reg Test");
   fCompositeFrame533->SetLayoutManager(new TGVerticalLayout(fCompositeFrame533));
   fCompositeFrame533->SetLayoutBroken(kTRUE);

   // "Register I/O" group frame
   TGGroupFrame *fGroupFrame577 = new TGGroupFrame(fCompositeFrame533,"Register I/O");
   fGroupFrame577->SetLayoutBroken(kTRUE);
   TGTextButton *fTextButton664 = new TGTextButton(fGroupFrame577,"Read Reg");
   fTextButton664->SetTextJustify(36);
   fTextButton664->Resize(63,22);
   fGroupFrame577->AddFrame(fTextButton664, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,-2,2));
   fTextButton664->MoveResize(16,72,63,22);
   TGTextButton *fTextButton701 = new TGTextButton(fGroupFrame577,"Write Reg");
   fTextButton701->SetTextJustify(36);
   fTextButton701->Resize(62,22);
   fGroupFrame577->AddFrame(fTextButton701);
   fTextButton701->MoveResize(16,136,62,22);

   TGFont *ufont;         // will reflect user font changes
   ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   TGGC   *uGC;           // will reflect user GC changes
   // graphics context changes
   GCValues_t valEntry1003;
   valEntry1003.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry1003.fForeground);
   gClient->GetColorByName("#c0c0c0",valEntry1003.fBackground);
   valEntry1003.fFillStyle = kFillSolid;
   valEntry1003.fFont = ufont->GetFontHandle();
   valEntry1003.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry1003, kTRUE);
   TGTextEntry *fTextEntry1003 = new TGTextEntry(fGroupFrame577, new TGTextBuffer(15),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kDoubleBorder | kOwnBackground);
   fTextEntry1003->SetMaxLength(255);
   fTextEntry1003->SetAlignment(kTextLeft);
   fTextEntry1003->SetText("0");
   fTextEntry1003->Resize(112,fTextEntry1003->GetDefaultHeight());
   fGroupFrame577->AddFrame(fTextEntry1003);
   fTextEntry1003->MoveResize(88,72,112,22);

   ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   // graphics context changes
   GCValues_t valEntry732;
   valEntry732.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry732.fForeground);
   gClient->GetColorByName("#c0c0c0",valEntry732.fBackground);
   valEntry732.fFillStyle = kFillSolid;
   valEntry732.fFont = ufont->GetFontHandle();
   valEntry732.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry732, kTRUE);
   TGTextEntry *fTextEntry732 = new TGTextEntry(fGroupFrame577, new TGTextBuffer(15),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kDoubleBorder | kOwnBackground);
   fTextEntry732->SetMaxLength(255);
   fTextEntry732->SetAlignment(kTextLeft);
   fTextEntry732->SetText("0");
   fTextEntry732->Resize(112,fTextEntry732->GetDefaultHeight());
   fGroupFrame577->AddFrame(fTextEntry732, new TGLayoutHints(kLHintsRight | kLHintsTop,2,1,2,2));
   fTextEntry732->MoveResize(88,136,112,22);

   ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   // graphics context changes
   GCValues_t valEntry783;
   valEntry783.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry783.fForeground);
   gClient->GetColorByName("#c0c0c0",valEntry783.fBackground);
   valEntry783.fFillStyle = kFillSolid;
   valEntry783.fFont = ufont->GetFontHandle();
   valEntry783.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry783, kTRUE);
   TGTextEntry *fTextEntry783 = new TGTextEntry(fGroupFrame577, new TGTextBuffer(15),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kDoubleBorder | kOwnBackground);
   fTextEntry783->SetMaxLength(255);
   fTextEntry783->SetAlignment(kTextLeft);
   fTextEntry783->SetText("0");
   fTextEntry783->Resize(112,fTextEntry783->GetDefaultHeight());
   fGroupFrame577->AddFrame(fTextEntry783, new TGLayoutHints(kLHintsRight | kLHintsTop,2,-1,12,2));
   fTextEntry783->MoveResize(232,136,112,22);

   fGroupFrame577->SetLayoutManager(new TGVerticalLayout(fGroupFrame577));
   fGroupFrame577->Resize(384,200);
   fCompositeFrame533->AddFrame(fGroupFrame577, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   fGroupFrame577->MoveResize(8,0,384,200);

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas620 = new TRootEmbeddedCanvas(0,fCompositeFrame533,384,150);
   Int_t wfRootEmbeddedCanvas620 = fRootEmbeddedCanvas620->GetCanvasWindowId();
   TCanvas *c123 = new TCanvas("c123", 10, 10, wfRootEmbeddedCanvas620);
   fRootEmbeddedCanvas620->AdoptCanvas(c123);
   fCompositeFrame533->AddFrame(fRootEmbeddedCanvas620, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRootEmbeddedCanvas620->MoveResize(8,208,384,150);


   // container of "Loop Test"
   TGCompositeFrame *fCompositeFrame576;
   fCompositeFrame576 = fTab528->AddTab("Loop Test");
   fCompositeFrame576->SetLayoutManager(new TGVerticalLayout(fCompositeFrame576));
   fCompositeFrame576->SetLayoutBroken(kTRUE);

   // "Command" group frame
   TGGroupFrame *fGroupFrame1562 = new TGGroupFrame(fCompositeFrame576,"Command");
   fGroupFrame1562->SetLayoutBroken(kTRUE);

   ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   // graphics context changes
   GCValues_t valButton1581;
   valButton1581.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#ffffff",valButton1581.fForeground);
   gClient->GetColorByName("#c0c0c0",valButton1581.fBackground);
   valButton1581.fFillStyle = kFillSolid;
   valButton1581.fFont = ufont->GetFontHandle();
   valButton1581.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valButton1581, kTRUE);

   ULong_t ucolor;        // will reflect user color changes
   gClient->GetColorByName("#0000ff",ucolor);
   TGTextButton *fTextButton1581 = new TGTextButton(fGroupFrame1562,"Start",-1,uGC->GetGC());
   fTextButton1581->SetTextJustify(36);
   fTextButton1581->Resize(99,24);

   fTextButton1581->ChangeBackground(ucolor);
   fGroupFrame1562->AddFrame(fTextButton1581, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton1581->MoveResize(24,32,99,24);

   ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   // graphics context changes
   GCValues_t valButton1647;
   valButton1647.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#ffffff",valButton1647.fForeground);
   gClient->GetColorByName("#c0c0c0",valButton1647.fBackground);
   valButton1647.fFillStyle = kFillSolid;
   valButton1647.fFont = ufont->GetFontHandle();
   valButton1647.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valButton1647, kTRUE);

   TGTextButton *fTextButton1647 = new TGTextButton(fGroupFrame1562,"Pause",-1,uGC->GetGC());
   fTextButton1647->SetTextJustify(36);
   fTextButton1647->Resize(99,24);

   fTextButton1647->ChangeBackground(ucolor);
   fGroupFrame1562->AddFrame(fTextButton1647, new TGLayoutHints(kLHintsRight | kLHintsTop,2,-1,12,2));
   fTextButton1647->MoveResize(24,56,99,24);

   ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   // graphics context changes
   GCValues_t valButton1687;
   valButton1687.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#ffffff",valButton1687.fForeground);
   gClient->GetColorByName("#c0c0c0",valButton1687.fBackground);
   valButton1687.fFillStyle = kFillSolid;
   valButton1687.fFont = ufont->GetFontHandle();
   valButton1687.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valButton1687, kTRUE);

   TGTextButton *fTextButton1687 = new TGTextButton(fGroupFrame1562,"Stop",-1,uGC->GetGC());
   fTextButton1687->SetTextJustify(36);
   fTextButton1687->Resize(99,24);

   fTextButton1687->ChangeBackground(ucolor);
   fGroupFrame1562->AddFrame(fTextButton1687, new TGLayoutHints(kLHintsRight | kLHintsTop,2,-1,12,2));
   fTextButton1687->MoveResize(24,80,99,24);

   ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   // graphics context changes
   GCValues_t valEntry1888;
   valEntry1888.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry1888.fForeground);
   gClient->GetColorByName("#c0c0c0",valEntry1888.fBackground);
   valEntry1888.fFillStyle = kFillSolid;
   valEntry1888.fFont = ufont->GetFontHandle();
   valEntry1888.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry1888, kTRUE);
   TGTextEntry *fTextEntry1888 = new TGTextEntry(fGroupFrame1562, new TGTextBuffer(15),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kDoubleBorder | kOwnBackground);
   fTextEntry1888->SetMaxLength(255);
   fTextEntry1888->SetAlignment(kTextCenterX);
   fTextEntry1888->SetText("1000");
   fTextEntry1888->Resize(93,fTextEntry1888->GetDefaultHeight());
   fGroupFrame1562->AddFrame(fTextEntry1888, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextEntry1888->MoveResize(264,32,93,22);

   fGroupFrame1562->SetLayoutManager(new TGVerticalLayout(fGroupFrame1562));
   fGroupFrame1562->Resize(384,152);
   fCompositeFrame576->AddFrame(fGroupFrame1562, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fGroupFrame1562->MoveResize(8,8,384,152);

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas1947 = new TRootEmbeddedCanvas(0,fCompositeFrame576,382,192);
   Int_t wfRootEmbeddedCanvas1947 = fRootEmbeddedCanvas1947->GetCanvasWindowId();
   TCanvas *c124 = new TCanvas("c124", 10, 10, wfRootEmbeddedCanvas1947);
   fRootEmbeddedCanvas1947->AdoptCanvas(c124);
   fCompositeFrame576->AddFrame(fRootEmbeddedCanvas1947, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRootEmbeddedCanvas1947->MoveResize(10,176,382,192);


   fTab528->SetTab(2);

   fTab528->Resize(fTab528->GetDefaultSize());
   fMainFrame2396->AddFrame(fTab528, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTab528->MoveResize(8,48,408,408);
   TGPictureButton *fPictureButton1991 = new TGPictureButton(fMainFrame2396,gClient->GetPicture("mb_question_s.xpm"));
   fMainFrame2396->AddFrame(fPictureButton1991, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fPictureButton1991->MoveResize(16,8,33,24);

   fMainFrame2396->MapSubwindows();

   fMainFrame2396->Resize(fMainFrame2396->GetDefaultSize());
   fMainFrame2396->MapWindow();
   fMainFrame2396->Resize(432,491);
}  
