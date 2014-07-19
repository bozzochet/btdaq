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
#include "ExitDialog.h"
#include "CDaqHold.h"
#include "errordiag.h"
#include "gonioadv.h"
#include "TDRGUI.h"

void CrystalGUI::filePrint()
{

}


void CrystalGUI::fileExit(){
  
      // Create a new dialog
      MyDialog dlg( this );
    
    // Show it and wait for Ok or Cancel
      if( dlg.exec() == QDialog::Accepted )
         exit(0);
}





void CrystalGUI::CrystalGUI_dockWindowPositionChanged( QDockWindow * )
{

}


void CrystalGUI::CrystalGUI_usesTextLabelChanged( bool )
{

}


void CrystalGUI::pbGonGet_clicked()
{
    if(CDaqHold::mdaq){
 GonClass* gg=CDaqHold::mdaq->Gon; 
                gg->GetPos();
                      UpdateGonDisplay();
    }
    else{
           ErrorDiag dlg( this );
               
    // Show it and wait for Ok or Cancel
                dlg.exec();  
    }
    
}
void CrystalGUI::Init(){
    UpdateGonDisplay();
   
}

void CrystalGUI::UpdateGonDisplay()
{
    QString aa;
    GonClass* gg=CDaqHold::mdaq->Gon; 
    if(rbGonRad->isOn()){
 aa=tlb1->text(); aa.replace("murad","radiant"); tlb1->setText(aa);
              aa=tlb1->text();aa.replace("grad","radiant");tlb1->setText(aa);
 aa=tlb1_2->text();aa.replace("murad","radiant");tlb1_2->setText(aa);
              aa=tlb1_2->text();aa.replace("grad","radiant");tlb1_2->setText(aa);

 LCDGon1->display(gg->ReadAlpha());
              LCDGon4->display(gg->ReadCraddle());
   }else if(rbGonGrad->isOn()){
              aa=tlb1->text();aa.replace("murad","grad");tlb1->setText(aa);
              aa=tlb1->text();aa.replace("radiant","grad");tlb1->setText(aa);
              aa=tlb1_2->text();aa.replace("murad","grad");tlb1_2->setText(aa);
              aa=tlb1_2->text();aa.replace("radiant","grad");tlb1_2->setText(aa);
       LCDGon1->display(gg->ReadAlphaG());
       LCDGon4->display(gg->ReadCraddleG());
   }
   else{
         aa=tlb1->text();aa.replace("radiant","murad");tlb1->setText(aa);
              aa=tlb1->text();aa.replace("grad","murad");tlb1->setText(aa);
              aa=tlb1_2->text();aa.replace("grad","murad");tlb1_2->setText(aa);
              aa=tlb1_2->text();aa.replace("radiant","murad");tlb1_2->setText(aa);
       LCDGon1->display(gg->ReadAlpha()*1E6);
       LCDGon4->display(gg->ReadCraddle()*1E6);
   }
   LCDGon2->display(gg->ReadLin1());
   LCDGon3->display(gg->ReadLin2());

   
}


void CrystalGUI::SetGonPos()
{
 GonClass* gg=CDaqHold::mdaq->Gon; 

 double a1=leGonAlpha->text().toDouble();
 double c1=leGonCrad->text().toDouble();
 double l1=leGonLin1->text().toDouble();
 double l2=leGonLin2->text().toDouble();
 if(rbGonRad->isOn())
    gg->SetPos(a1,c1,l1,l2);
 else if(rbGonGrad->isOn())
     gg->SetPosGrad(a1,c1,l1,l2);
 else
     gg->SetPos(a1*1E-6,c1*1E-6,l1,l2);
 if(rbAbs->isChecked())
 gg->MoveTo();
else
 gg->MoveToRel();
    
 gg->GetPos();
 UpdateGonDisplay();
}


void CrystalGUI::toolsGoniometer_AdvancedAction_activated()
{
  // Create a new dialog
      GonioAdv*  dlg=new GonioAdv( this );
    
    // Show it 
   dlg->show();
}


void CrystalGUI::rbGonMurad_toggled( bool )
{
UpdateGonDisplay();
}


void CrystalGUI::rbGonRad_toggled( bool )
{
UpdateGonDisplay();
}


void CrystalGUI::rbGonGrad_toggled( bool )
{
UpdateGonDisplay();
}
