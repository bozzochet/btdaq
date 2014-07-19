#include <qapplication.h>
#include "Crystal.h"
#include "ExitDialog.h"
#include "CDaqHold.h"


int main( int argc, char ** argv )
{
    //init my stuff
    CDaqHold::mdaq= new MainDaq();
    CDaqHold::mdaq->Gon->PrintOn();
    //go with the GUI
    QApplication a( argc, argv );
    CrystalGUI w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
