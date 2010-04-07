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
#include "Event.h"
#include "RTDisplay.h"





void ClRDisplay::UpdateS()
{
teRSOUT->clear();

CanvasRS->GetCanvas()->Clear();

Event* ev=RTrackerDisplay::fSelfPtr->fEvent;


AMSTrRawCluster* cls=ev->GetTrRawCluster(sbRClS->text().toInt());


if(Ssig) delete Ssig;
if(Snoise) delete Snoise;
if(Sstat) delete Sstat;


if(cls){
     cls->Info();
     teRSOUT->append(QString(cls->sout));
    int infs   =cls->GetAddress(0)-10;
    int sups =cls->GetAddress(0)+10+cls->GetNelem();
    Ssig    = new TH1F("SignalRS","SignalS;Channel;ADC count",sups-infs,infs,sups);
    Snoise = new TH1F("NoiseRS","NoiseS;Channel;ADC count",sups-infs,infs,sups);
    Sstat   = new TH1F("StatusRS","StatusS;Channel;ADC count",sups-infs,infs,sups);
 
    for(int ii=0;ii<cls->GetNelem();ii++){
         Ssig->SetBinContent(ii+11,cls->GetSignal(ii));
         Snoise->SetBinContent(ii+11,cls->GetSigma(ii));
        if( cls->GetStatus(ii)!=0)Sstat->SetBinContent(ii+11,-2);
    }
    CanvasRS->GetCanvas()->cd();
    Ssig->Draw();
    Snoise->SetLineColor(4);
    Snoise->SetLineStyle(2);
    Snoise->Draw("same");
    Sstat->SetLineColor(2);
    Sstat->Draw("same");    
    CanvasRS->GetCanvas()->Update();
}

}





void ClRDisplay::init()
{
Ssig=0;
Snoise=0;
Sstat=0;
cls=0;
    
}


