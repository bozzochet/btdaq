#include <qapplication.h>
#include "TDRGUI.h"
#include "ExitDialog.h"
#include "CDaqHold.h"


int main( int argc, char ** argv )
{
    //init my stuff
    CDaqHold::pDComm= new DaqClass("DAQ0","localhost",6666);
    //go with the GUI
    QApplication a( argc, argv );
    Daq0 w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
