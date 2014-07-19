//  $Id: ControlFrame.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#ifndef __CONTROLFRAME__
#define __CONTROLFRAME__
#include <stdlib.h>
#include <iostream>
#include <TROOT.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGFileDialog.h>
#include <vector>
#include <TGProgressBar.h>
#include <TGSlider.h>
using namespace std;

class AMSControlFrame : public TGTransientFrame {
protected:
  TGGroupFrame *_pbutfr, *_pcontrolfr, *_pcombofr, *_pcyclefr;
  TGGroupFrame* _plogfr;
  TGCheckButton *_plogx;
  TGCheckButton *_plogy;
  TGCheckButton *_plogz;
  vector<TGCheckButton *> _pcycle;
  vector<TGTextButton *> _pbutton;
  vector<TGTextButton *> _pcontrol;
  TGMenuBar           *fMenuBar;            // menubar
  TGPopupMenu         *fDetMenu;
  vector<TGPopupMenu*>         fSubDetMenu;      //sub det
  vector<TGPopupMenu*>   fSubDetCascadeMenu;
  TGPopupMenu         *fSaveAsMenu;        //  saveas
    
  
  TGLayoutHints       *fMenuBarLayout;      // menubar layout hints
  TGLayoutHints       *fMenuBarItemLayout;  // layout hints for menu in menubar
  TGLayoutHints       *fMenuBarHelpLayout;  // layout hint for help menu in menubar
  TGLayoutHints       *fL1;
  TGLayoutHints       *fL2;
  TGLayoutHints       *fL3;
  TGLayoutHints       *fL4;
  TGLayoutHints       *fL5;
  TGHProgressBar *fbar;
  TGHSlider *fslider;
public:
  AMSControlFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
		  UInt_t options = kMainFrame | kVerticalFrame);
  virtual ~AMSControlFrame();
  void ShowPosition(){if(fbar)fbar->ShowPosition();}
  void SetPosition(float t){if(fbar)fbar->SetPosition(t);}
  virtual void CloseWindow();
  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
ClassDef(AMSControlFrame,0)
};
#endif
