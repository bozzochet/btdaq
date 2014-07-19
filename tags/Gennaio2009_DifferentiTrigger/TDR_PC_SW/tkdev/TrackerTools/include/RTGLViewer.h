//  $Id: RTGLViewer.h,v 1.3 2008/12/03 21:02:03 haino Exp $
#ifndef _TDGLVIEWER_
#define _TDGLVIEWER_

#include "RTConnector.h"
#include "TGLViewer.h"
#include "TGFrame.h"
#include "TObjArray.h"

class RTConnector;
class TGMainFrame;
class TGButton;
class TGNumberEntry;
class TGTextView;
class TFile;
class TTree;
class Event;

class RTGLViewer : public TGLViewer
{
public:
  static Bool_t fIsStandAlone;

protected:
  RTConnector   *fConn;
  TGMainFrame   *fFrame;
  TGNumberEntry *fEntry;
  TGTextView    *fTview;
  TObjArray      fBarray;

  TFile *fFile;
  TTree *fTree;
  Event *fEvent;

  void CreateFrames();

  TGButton *GetButton(Int_t id);

  static RTGLViewer *fSelfPtr;

public:
  RTGLViewer(TVirtualPad *pad);
 ~RTGLViewer();

  static RTGLViewer *GetPtr(void) { return fSelfPtr; }

  RTConnector *GetConnector(void) { return fConn; }
  Int_t GetEntry(void);
  void  SetEntry(Int_t entry);
  void  SetMax  (Int_t max);

protected:
  void Show(void);
  void OpenFile(const char *fname);

  TTree *GetTree(void);
  void  DrawInfo(void);

public:
  void HandleButtons(Bool_t sw);

  static void Open (void);
  static void Print(void);
  static void Exit (void);
  static void Next (void);
  static void Prev (void);
  static void Draw (void);
  static void Entry(void);

  virtual void fileOpen  () { fConn->Emit("fileOpen()"  ); }  // *SIGNAL*
  virtual void eventEntry() { fConn->Emit("eventEntry()"); }  // *SIGNAL*

  ClassDef(RTGLViewer, 0) // Standalone GL viewer
};

#endif

