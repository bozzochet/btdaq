#include "TQApplication.h"
#include "TQRootApplication.h"
#include "mqdisplay.h"
#include "RTGLViewer.h"
#include "TkDBc.h"
#include "TROOT.h"
#include "TString.h"

int main( int argc, char ** argv )
{
   TQApplication app("uno",&argc,argv);
   TQRootApplication a( argc, argv, 0);

   TString fname;
   TString tkdbc;
   int entry = 0;
   for (int i = 0; i < argc; i++) {
     TString str = argv[i];
     if (str.Contains("entry:"))
       entry = TString(str(str.First(":")+1, str.Length())).Atoi();
     if (str.Contains("dst:"  )) fname = str(str.First(":")+1, str.Length());
     if (str.Contains("tkdbc:")) tkdbc = str(str.First(":")+1, str.Length());
   }

   TkDBc::CreateTkDBc();
   if (tkdbc != "") TkDBc::Head->init(tkdbc);
   else TkDBc::Head->init();

   RTGLViewer::fIsStandAlone = kFALSE;

   gROOT->SetStyle("Plain"); 
   MQDisplay *w = new MQDisplay();
   if (entry  >  0) w->SetEntry(entry);
   if (fname != "") w->SetFile (fname);
   w->show();

   a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
   a.connect(  w, SIGNAL( quitApp() ), &a, SLOT( quit() ) );
   a.exec();
   return 0;
}
 
