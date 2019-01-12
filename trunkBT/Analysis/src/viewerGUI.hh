#include <TQObject.h>
#include <RQ_OBJECT.h>

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

class MyMainFrame {
  RQ_OBJECT("MyMainFrame")
private:
  TGMainFrame         *fMain;
  TRootEmbeddedCanvas *fEcanvas;
  TGNumberEntry       *fNumber, *fNumber2;
  TGHorizontalFrame   *fHor0, *fHor0b, *fHor1, *fHor2,*fHor3;
  TGVerticalFrame     *fVer0, *fVer1;
  TGTextButton        *fExit, *fDraw, *fOpen;
  TGLabel             *evtLabel, *tdrLabel, *fileLabel;
  TGTextView          *fStatusBar;

public:
  MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
  virtual ~MyMainFrame();
  void DoDraw();
  void DoOpen();
//  ClassDef(MyMainFrame, 0)
};
