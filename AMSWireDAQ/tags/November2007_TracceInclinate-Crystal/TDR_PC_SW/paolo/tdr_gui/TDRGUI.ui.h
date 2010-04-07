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
#include <iostream>
#include <qfile.h>
#include <qerrormessage.h>
#include "errordialog.h"
#include "CDaqHold.h"

using namespace std;

bool Daq0::CheckConn(){
  if(!CDaqHold::pDComm){
    ErrorDialog* err= new ErrorDialog(); 
    err->setError("The Daq Class has not been initalizated.\n I give up!"); 
    err->show();
    return 0;
  } else  if(CDaqHold::pDComm->TestConnection()!=1){
    ErrorDialog* err= new ErrorDialog(); 
    err->setError("The Daq Class Cannot talk to the real Daq.\n I give up!"); 
    err->show();
    return 0;
  }
  return 1;
    
}

bool Daq0::pbGetLog_clicked(){
  char outb[255];

  ErrorDialog* err;
  if(CheckConn()){
    CDaqHold::pDComm->GetLog("pippo.log");
  } 
  return 1;
}


void Daq0::UpdateLog()
{
  QString fname="pippo.log";
  QFile file(fname);
  
  if ( file.open( IO_ReadOnly ) ) {
        QTextStream stream( &file );
        textEdit2->setText( stream.read() );
    }

  textEdit2->scrollToBottom ();
   
  file.close();
}
 



void Daq0::init(){
  if(CheckConn()){
    txLaTitle->setText(CDaqHold::pDComm->DaqName);
    textEdit2->setTextFormat(LogText);
    textEdit2->clear();
    textEdCount->setTextFormat(LogText);
    textEdCount->clear();
  }
}


void Daq0::pbClear_clicked()
{
  textEdit2->clear();
}


void Daq0::pbInit_clicked()
{
  if(CheckConn())
    CDaqHold::pDComm->Init();

}


void Daq0::pbStart_clicked()
{
  if(CheckConn()){
    GonClass* gonio=0;
    ErrorDialog* err;
    if(!gonio){
      err= new ErrorDialog(); err->setError("The Gon Comm Class has not been initalizated\n I will go on but you are warned!"); err->show(); 
    }
  
    if( gonio){ 
      CDaqHold::pDComm->StartDaq(leRun->text().toInt(),leEvMax->text().toInt(),gonio);
    }else if(CDaqHold::pDComm){ 
      CDaqHold::pDComm->StartDaq(leRun->text().toInt(),leEvMax->text().toInt(),0);
    }
  }
}

void Daq0::UpdateCount(){
  
  if(CheckConn()){
    
    char pippo[255];
    CDaqHold::pDComm->GetProcEvents(pippo);
    textEdCount->clear();
    printf("%s",pippo);
    QString jline(pippo);
    textEdCount->append(jline);


  }
}





void Daq0::pbStopLog_clicked()
{
  if(CheckConn())
    CDaqHold::pDComm->StopLog();
 
  return ;
}


void Daq0::pbUpdate_clicked()
{
  UpdateLog();
  UpdateCount();
}


void Daq0::pbCalib_clicked()
{
  if(CheckConn()){
    
      CDaqHold::pDComm->Calibrate(leRun->text().toInt());
    }
  
}


void Daq0::pbNewLog_clicked()
{
 if(CheckConn())
     CDaqHold::pDComm->NewLog();
   return;
}
