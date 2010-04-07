//  $Id: ControlFrame.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
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
#include <TGComboBox.h>
using namespace std;

class AMSControlFrame : public TGTransientFrame {
protected:
  TGButtonGroup  *_pmousefr;
  TGGroupFrame *_pviewfr, *_pcontrolfr,  *_pvisfr, *_pfocusfr;
  vector<TGCheckButton *> _pvis;
  TGComboBox *_pfocus;
  vector<TGTextButton *> _pcontrol;
  vector<TGTextButton *> _pview;
  vector<TGRadioButton *> _pmouse;
  TGCheckButton * _pauto;
  TGMenuBar           *fMenuBar;            // menubar
  TGPopupMenu         *fSaveAsMenu;        //  saveas
  TGPopupMenu         *fOptionMenu;     
  TGPopupMenu         *fHelpMenu;     
  
  TGLayoutHints       *fMenuBarLayout;      // menubar layout hints
  TGLayoutHints       *fMenuBarItemLayout;  // layout hints for menu in menubar
  TGLayoutHints       *fMenuBarHelpLayout;  // layout hint for help menu in menubar
  TGLayoutHints       *fL1;
  TGLayoutHints       *fL2;
  TGLayoutHints       *fL3;
  TGLayoutHints       *fL4;
  TGLayoutHints       *fL5;
  TGHSlider *fslider;
public:
  AMSControlFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
		  UInt_t options = kMainFrame | kVerticalFrame);
  virtual ~AMSControlFrame();
  virtual void CloseWindow();
  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};
#endif
