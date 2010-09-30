#include <TROOT.h>
#include <TApplication.h> 
#include <TGClient.h>

#include "MainFrame.h"

extern void InitGui(); 
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };

TROOT root("GUI", "GUI test environment",initfuncs);

int main(int argc, char **argv) 
{ 
  TApplication theApp("App", &argc, argv);
  MainFrame mainWin(gClient->GetRoot(), 960,720);
  //MainFrame mainWin(gClient->GetRoot(), gClient->GetDisplayWidth(),gClient->GetDisplayHeight());
  theApp.Run();
  return 0;
}
