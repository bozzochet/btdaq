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
#include "CDaqHold.h"

void GonioAdv::pbGonSetPos_clicked()
{
  if(CDaqHold::mdaq){
     GonClass* gg=CDaqHold::mdaq->Gon; 
     if(gg) gg->SetZero();
 }
  return;
}


void GonioAdv::pbGonHwCal_clicked()
{
 if(CDaqHold::mdaq){
     GonClass* gg=CDaqHold::mdaq->Gon; 
     if(gg) gg->Reset();
 }
}


void GonioAdv::pbGonZero_clicked()
{
 if(CDaqHold::mdaq){
     GonClass* gg=CDaqHold::mdaq->Gon; 
     if(gg) gg->GotoZero();
 }
}



void GonioAdv::pbGonAbort_clicked()
{
 if(CDaqHold::mdaq){
     GonClass* gg=CDaqHold::mdaq->Gon; 
     if(gg) gg->Abort();
 }
}


void GonioAdv::pbGonSendRaw_clicked()
{
    
    if(CDaqHold::mdaq){
     GonClass* gg=CDaqHold::mdaq->Gon; 
     if(gg &&
 leGonRawCom->text().length()!=0) 
  gg->SendRawCommand(leGonRawCom->text().ascii());
     
 }
}




void GonioAdv::pbGonSwCal_clicked()
{
 if(CDaqHold::mdaq){
     GonClass* gg=CDaqHold::mdaq->Gon; 
     if(gg) gg->Calibrate();
 }
}
