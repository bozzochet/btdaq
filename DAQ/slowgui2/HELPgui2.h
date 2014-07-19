// directly copied from $ROOTSYS/test/guitest.cxx

#include "slowgui2.h"


class Editor : public TGTransientFrame {

private:
   TGTextEdit       *fEdit;   // text edit widget
   TGTextButton     *fOK;     // OK button
   TGLayoutHints    *fL1;     // layout of TGTextEdit
   TGLayoutHints    *fL2;     // layout of OK button

public:
   Editor(const TGWindow *main, UInt_t w, UInt_t h);
   virtual ~Editor();

   void   LoadBuffer(const char *buffer);
   void   LoadFile(const char *file);

   TGTextEdit *GetEditor() const { return fEdit; }

   void   SetTitle();
   void   SetTitle(Char_t *title);
   void   Popup();
   void   CloseWindow();
   Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};
