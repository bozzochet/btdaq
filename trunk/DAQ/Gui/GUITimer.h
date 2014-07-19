#include "MainFrame.h"

class GUITimer : public TTimer {

 private:
  MainFrame *fMain;

 public:
  GUITimer(MainFrame *main, Long_t ms, Bool_t mode);
  ~GUITimer();
  Bool_t Notify();

};

