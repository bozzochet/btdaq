/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#include <qfiledialog.h>
#include "hitdump.h"
#include "cldisplay.h"
#include "clrdisplay.h"
#include "RTDisplay3D.h"
#include "RTConnectorMQD.h"

void MQDisplay::init()
{
    new RTrackerDisplay();
    RTrackerDisplay::fSelfPtr->fCanvas=Canvas1->GetCanvas();
    RTrackerDisplay::fSelfPtr->PreparePlanes();
    fConn = 0;
}

void MQDisplay::fileNew()
{

}


void MQDisplay::fileOpen()
{
    QString s = QFileDialog::getOpenFileName(
       "./",
                    "Root file (*.root)",
                    this,
                    "open file dialog",
                    "Choose a file" );
    RTrackerDisplay::fSelfPtr->OpenFile(s.ascii());
    sbEvent->setMinValue(0);
    sbEvent->setMaxValue(RTrackerDisplay::fSelfPtr->GetEntries()-1);
    setMaxConn(RTrackerDisplay::fSelfPtr->GetEntries()-1);
    pbDraw_clicked();
}


void MQDisplay::fileSave()
{

}


void MQDisplay::fileSaveAs()
{

}


void MQDisplay::filePrint()
{

}


void MQDisplay::fileExit()
{
    emit quitApp();
}


void MQDisplay::editUndo()
{

}


void MQDisplay::editRedo()
{

}


void MQDisplay::editCut()
{

}


void MQDisplay::editCopy()
{

}


void MQDisplay::editPaste()
{

}


void MQDisplay::editFind()
{

}


void MQDisplay::pbPREV_clicked()
{
    sbEvent->stepDown();
}


void MQDisplay::pbNEXT_clicked()
{
    sbEvent->stepUp();
}


void MQDisplay::pbOPEN_clicked()
{

}


void MQDisplay::pbDraw_clicked()
{
    int bb=(sbEvent->text()).toInt();
    RTrackerDisplay::fSelfPtr->DrawEvent(bb);
    emit drawEvent();
    TextUpdate();
}


void MQDisplay::TextUpdate()
{
    if (!RTrackerDisplay::fSelfPtr->fEvent) return;
    RTrackerDisplay::fSelfPtr->fEvent->Info();
    QString aa(RTrackerDisplay::fSelfPtr->fEvent->sout);
    teOUT->setText(aa); 
}





void MQDisplay::pbAnalize_clicked()
{
    HitDump* hh=new HitDump();
    hh->show();
}


void MQDisplay::pbCLD_clicked()
{
    ClDisplay* hh=new ClDisplay();
    connect(this, SIGNAL(drawEvent()), hh, SLOT(UpdateEvent()));
    hh->show();
    hh->UpdateEvent();
}


void MQDisplay::pbRCLD_clicked()
{
    ClRDisplay* hh=new ClRDisplay();
    hh->show();
}


void MQDisplay::pb3DD_clicked()
{
    if (fConn) return;
    RTrackerDisplay3D::RunDisplay3D("rtogl");
    fConn = new RTConnectorMQD(this);
    setMaxConn(RTrackerDisplay::fSelfPtr->GetEntries()-1);
    
    fConn->DrawEvent();
}


void MQDisplay::SetFile( const char *fname )
{
    RTrackerDisplay::fSelfPtr->OpenFile(fname);
    sbEvent->setMinValue(0);
    sbEvent->setMaxValue(RTrackerDisplay::fSelfPtr->GetEntries()-1);
    setMaxConn(RTrackerDisplay::fSelfPtr->GetEntries()-1);
    pbDraw_clicked();
}


void MQDisplay::SetEntry( int entry)
{
    sbEvent->setValue(entry);
}

void MQDisplay::drawEventConn()
{
    fConn->DrawEvent();
}


void MQDisplay::setEntryConn( int entry )
{
    fConn->SetEntry(entry);
}


void MQDisplay::setMaxConn( int max )
{
    if (fConn) fConn->SetMax(max);
}
