#include <TROOT.h>
#include <TApplication.h> 
#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGString.h>
#include <TGLabel.h>
#include <TGTextView.h>

#include "DefineFonts.h"


class ShowFile : public TGTransientFrame{

private:
  const TGWindow *fMain;
  TGCompositeFrame *fSF;
  TGTextView *fSFText;
  TGTextButton *fSFButtonDISMISS; 


public:
  ShowFile(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, 
	   const char *Title, const char *FileName);
  ~ShowFile();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void CloseWindow();
};


