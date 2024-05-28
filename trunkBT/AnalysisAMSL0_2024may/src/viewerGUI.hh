#include <TQObject.h>
#include <RQ_OBJECT.h>
#include "TGraph.h"

class TGWindow;
class TGMainFrame;
class TRootEmbeddedCanvas;
class TGNumberEntry;
class TGHorizontalFrame;
class TGVerticalFrame;
class TGTextButton;
class TGLabel;
class TGTextView;
class TGLayout;
class TGFrame;
class TGFileDialog;
class TGCheckButton;

class MyMainFrame
{
  RQ_OBJECT("MyMainFrame")
private:
  TGMainFrame *fMain;
  TRootEmbeddedCanvas *fEcanvas;
  TGNumberEntry *fNumber, *fNumber2;
  TGHorizontalFrame *fHor0, *fHor0b, *fHor1, *fHor2, *fHor3, *fHor4;
  TGVerticalFrame *fVer0, *fVer1;
  TGTextButton *fExit, *fDraw, *fOpen, *fSave;
  TGLabel *evtLabel, *tdrLabel, *fileLabel, *pedLabel;
  TGTextView *fStatusBar;
  TGCheckButton *fPed;
  TGraph *gr_eventS = new TGraph();
  TGraph *gr_eventK = new TGraph();
  TGraph *line[14];

public:
  MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~MyMainFrame();
  void DoDraw();
  void DoOpen();
  void DoSave();
  void viewer(int tdr, int evt, char filename[200]);
  void savetofile(int tdr, int evt, char filename[200]);
  ClassDef(MyMainFrame, 0)
};
