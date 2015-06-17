#include "slowgui.h"
#include "amswcomtest.h"
#include "SlowControl.h"




void SlowGui::CreateTBSFrame(TGCompositeFrame *frame) {

 // here we create two tabs, one for each TBS
  fTBSTab=new TGTab(frame, 200, 200);
  fTBSTab->Connect("Selected(Int_t)","SlowGui",this,"CheckTab(=0,\"TBS\")");

  for (Int_t i=0; i<NTBS; i++) {
    tTBSpage[i]=fTBSTab->AddTab(Form("TBS %d",i));
    TBSTabColor[i]=kMyYellow;
    FillTbsPage(i);
  }
  UpdateTabColor("TBS");


  frame->AddFrame(fTBSTab, fL5);

}


void SlowGui::FillTbsPage(Int_t page) {
  TGHorizontalFrame *hmain=new TGHorizontalFrame(tTBSpage[page],40,200);

  TGVerticalFrame *vframetools=new TGVerticalFrame(hmain,200,40);
  TGPictureButton *help=CreateHelpButton(vframetools,"TBS");
  TGPictureButton *refr=CreateRefreshButton(vframetools,"TBS");
  TGPictureButton *init=CreateInitLecroyButton(vframetools,"TBS",page);
  vframetools->AddFrame(refr, fL35);
  vframetools->AddFrame(init, fL35);
  vframetools->AddFrame(help, fL35);

  TGVerticalFrame *vframe=new TGVerticalFrame(hmain,200,40);
  TGHorizontalFrame *hframe=new TGHorizontalFrame(vframe,40,200);
  TGHorizontalFrame *hframe2=new TGHorizontalFrame(vframe,40,200);


  BuildActelDisplay(hframe, &(tTBSActelHotButton[page]), &(tTBSActelColdButton[page]), &(tTBSActelCombo[page]), &(tTBSTripIndicator[page]), &(tTBSPowerOnIndicator[page]), page, "TBS");



  TGVerticalFrame *vframe2;
  TGLabel *alabel;

  TGGroupFrame *agroup;
  for (Int_t grp=0; grp<2; grp++) {
    agroup=new TGGroupFrame(hframe2,Form("Group %d",grp+2*page),kHorizontalFrame);
    vframe2=new TGVerticalFrame(agroup,200,40);
    CreateTextButton(vframe2,&(tTBSLRhot[page][grp]),"Hot 80 V");
    tTBSLRhot[page][grp]->Connect("Clicked()","SlowGui",this,Form("ControlTbsLr(=%d,%d,0)",page,grp));
    CreateTextButton(vframe2,&(tTBSLRcold[page][grp]),"Cold 80 V");
    tTBSLRcold[page][grp]->Connect("Clicked()","SlowGui",this,Form("ControlTbsLr(=%d,%d,1)",page,grp));

    TGHorizontalFrame *hframe3=new TGHorizontalFrame(vframe2,40,200);
    tTBSVOLT60[page][grp]=new TGTextButton(hframe3,"60 V");
    tTBSVOLT60[page][grp]->Connect("Clicked()","SlowGui",this,Form("ControlTbsVoltage(=%d,%d,60)",page,grp));
    tTBSVOLT80[page][grp]=new TGTextButton(hframe3,"80 V");
    tTBSVOLT80[page][grp]->Connect("Clicked()","SlowGui",this,Form("ControlTbsVoltage(=%d,%d,80)",page,grp));
    hframe3->AddFrame(tTBSVOLT60[page][grp],fL74);
    hframe3->AddFrame(tTBSVOLT80[page][grp],fL74);
    vframe2->AddFrame(tTBSLRhot[page][grp],fL74);
    vframe2->AddFrame(tTBSLRcold[page][grp],fL74);
    //    vframe2->AddFrame(tTBSVOLT[page][grp],fL74);
    vframe2->AddFrame(hframe3,fL74);
    
    agroup->AddFrame(vframe2, fL73);

    vframe2=new TGVerticalFrame(agroup,200,40);
    tTBSADChot[page][grp]=new TGLabel(vframe2,"0x0000");
    SetLabelColor(tTBSADChot[page][grp], PixelColor[kMyGreen][0], PixelColor[kMyBlack][0]);
    tTBSADChot[page][grp]->SetMinHeight(30);
    tTBSADCcold[page][grp]=new TGLabel(vframe2,"0x0000");
    SetLabelColor(tTBSADCcold[page][grp], PixelColor[kMyGreen][0], PixelColor[kMyBlack][0]);
    tTBSGetVOLT[page][grp]=new TGTextButton(vframe2,"Get ADC");
    tTBSGetVOLT[page][grp]->Connect("Clicked()","SlowGui",this,Form("GetTbsADCBiasVoltage(=%d,%d,1)",page,grp));

    vframe2->AddFrame(tTBSADChot[page][grp],fL74);
    vframe2->AddFrame(tTBSADCcold[page][grp],fL74);
    vframe2->AddFrame(tTBSGetVOLT[page][grp],fL74);

    agroup->AddFrame(vframe2, fL74);

    hframe2->AddFrame(agroup, fL73);
  }
  
  ADCcurrentFrame[page]=new TGDockableFrame(vframe);
  ADCcurrentFrame[page]->Connect("ProcessedEvent(Event_t*)","SlowGui",this,"CheckTbsClickDockable(Event_t*)");
  ADCcurrentFrame[page]->Connect("Undocked()","SlowGui",this,"CheckTbsUndock()");
  ADCcurrentFrame[page]->Connect("Docked()","SlowGui",this,"CheckTbsDock()");
  ADCcurrentFrame[page]->SetWindowName(Form("TBS %d currents",page));

  TGGroupFrame *vframe3=new TGGroupFrame(ADCcurrentFrame[page],"Currents",kVerticalFrame);
  TGHorizontalFrame *hframe3=new TGHorizontalFrame(vframe3,40,200);
  for (Int_t grp=0; grp<2; grp++) {
    for (Int_t tdr=0; tdr<NTdrTPSFE; tdr+=2) {
      vframe2=new TGVerticalFrame(hframe3,200,40);
      tButTBSADCcurrent[page][tdr+grp*NTdrTPSFE]=new TGTextButton(vframe2,Form("TDR %d",TPSFEtoTDR(grp+2*page,tdr)));
      tButTBSADCcurrent[page][tdr+grp*NTdrTPSFE]->Connect("Clicked()","SlowGui",this,Form("GetTbsADCBiasCurrent(=%d,%d,1)",page,tdr+grp*NTdrTPSFE));
      vframe2->AddFrame(tButTBSADCcurrent[page][tdr+grp*NTdrTPSFE],fL74);
      tTBSADCcurrent[page][tdr+grp*NTdrTPSFE]=new TGLabel(vframe2,"0x0000");
      SetLabelColor(tTBSADCcurrent[page][tdr+grp*NTdrTPSFE], PixelColor[kMyGreen][0], PixelColor[kMyBlack][0]);
      vframe2->AddFrame(tTBSADCcurrent[page][tdr+grp*NTdrTPSFE],fL74);

      tTBSADCcurrent[page][tdr+1+grp*NTdrTPSFE]=new TGLabel(vframe2,"0x0000");
      SetLabelColor(tTBSADCcurrent[page][tdr+1+grp*NTdrTPSFE], PixelColor[kMyGreen][0], PixelColor[kMyBlack][0]);
      vframe2->AddFrame(tTBSADCcurrent[page][tdr+1+grp*NTdrTPSFE],fL74);
      tButTBSADCcurrent[page][tdr+1+grp*NTdrTPSFE]=new TGTextButton(vframe2,Form("TDR %d",TPSFEtoTDR(grp+2*page,tdr+1)));
      tButTBSADCcurrent[page][tdr+1+grp*NTdrTPSFE]->Connect("Clicked()","SlowGui",this,Form("GetTbsADCBiasCurrent(=%d,%d,1)",page,tdr+1+grp*NTdrTPSFE));
      printf("TDR %d, TDR %d\n", tdr+grp*NTdrTPSFE, tdr+1+grp*NTdrTPSFE);
      vframe2->AddFrame(tButTBSADCcurrent[page][tdr+1+grp*NTdrTPSFE],fL74);
      hframe3->AddFrame(vframe2,fL32);
    }
  }
  vframe3->AddFrame(hframe3,fL3);

  tButTBSADCcurrentsALL[page]=new TGTextButton(vframe3,"All");
  tButTBSADCcurrentsALL[page]->Connect("Clicked()","SlowGui",this,Form("GetTbsADCBiasCurrents(=%d,1)",page));

  TGTextButton *calbutton=new TGTextButton(vframe3,"Calibrate");
  calbutton->Connect("Clicked()","SlowGui",this,Form("CalibrateADCCurrent(=%d)",page));

  vframe3->AddFrame(tButTBSADCcurrentsALL[page],fL33);
  vframe3->AddFrame(calbutton,fL33);

  ADCcurrentFrame[page]->AddFrame(vframe3,fL3);
  vframe->AddFrame(hframe, fL3);
  vframe->AddFrame(hframe2,fL3);
  vframe->AddFrame(ADCcurrentFrame[page],fL3);


  //  tTBSpage[page]->AddFrame(vframe, fL3);

  hmain->AddFrame(vframe, fL3);
  hmain->AddFrame(vframetools, fL35);
  tTBSpage[page]->AddFrame(hmain, fL32);
}





void SlowGui::DisplayTbsStatus(Int_t Tbs) {
  // first Actel status
  Int_t mode=(ActelTbsState[Tbs][kActelHot]==kActelOn);
  tTBSActelHotButton[Tbs]->SetBackgroundColor(PixelColor[(mode)?kMyGreen:kMyRed][0]);
  mode=(ActelTbsState[Tbs][kActelCold]==kActelOn);
  tTBSActelColdButton[Tbs]->SetBackgroundColor(PixelColor[(mode)?kMyGreen:kMyRed][0]);
  gClient->NeedRedraw(tTBSActelHotButton[Tbs]);
  gClient->NeedRedraw(tTBSActelColdButton[Tbs]);

  // now the linear regulator states
  Int_t modecold0=0, modecold1=0;
  mode=(TBSlrState[Tbs][k0Hot]==kOn);
  //  modehot=mode;
  tTBSLRhot[Tbs][0]->SetBackgroundColor(PixelColor[(mode)?kMyGreen:kMyRed][0]);
  mode=(TBSlrState[Tbs][k0Cold]==kOn);
  modecold0=mode;
  tTBSLRcold[Tbs][0]->SetBackgroundColor(PixelColor[(mode)?kMyGreen:kMyRed][0]);

  mode=(TBSlrState[Tbs][k1Hot]==kOn);
  //modehot=mode;
  tTBSLRhot[Tbs][1]->SetBackgroundColor(PixelColor[(mode)?kMyGreen:kMyRed][0]);
  mode=(TBSlrState[Tbs][k1Cold]==kOn);
  modecold1=mode;
  tTBSLRcold[Tbs][1]->SetBackgroundColor(PixelColor[(mode)?kMyGreen:kMyRed][0]);

  // linear regulator voltages
  tTBSLRhot[Tbs][0]->SetText(Form("Hot %d V",(Voltages[Tbs][k0Hot]==k80V)?80:60));
  tTBSLRcold[Tbs][0]->SetText(Form("Cold %d V",(Voltages[Tbs][k0Cold]==k80V)?80:60));
  tTBSLRhot[Tbs][1]->SetText(Form("Hot %d V",(Voltages[Tbs][k1Hot]==k80V)?80:60));
  tTBSLRcold[Tbs][1]->SetText(Form("Cold %d V",(Voltages[Tbs][k1Cold]==k80V)?80:60));

  tTBSLRhot[Tbs][0]->SetEnabled(kTRUE);
  tTBSLRcold[Tbs][0]->SetEnabled(kTRUE);
  tTBSLRhot[Tbs][1]->SetEnabled(kTRUE);
  tTBSLRcold[Tbs][1]->SetEnabled(kTRUE);

  gClient->NeedRedraw(tTBSLRhot[Tbs][0]); 
  gClient->NeedRedraw(tTBSLRcold[Tbs][0]);
  gClient->NeedRedraw(tTBSLRhot[Tbs][1]);
  gClient->NeedRedraw(tTBSLRcold[Tbs][1]);

  // now the voltage adcs

  UpdateTbsBiasVoltage(Tbs,0);
  UpdateTbsBiasVoltage(Tbs,1);
  UpdateTKBiasVoltage(Tbs,0,modecold0);
  UpdateTKBiasVoltage(Tbs,1,modecold1);
  // now the current adcs
  UpdateTbsBiasCurrents(Tbs);

}


void SlowGui::ShowTbsEmpty(Int_t Tbs) {
  // first Actel status
  tTBSActelHotButton[Tbs]->SetBackgroundColor(PixelColor[kMyYellow][0]);
  tTBSActelColdButton[Tbs]->SetBackgroundColor(PixelColor[kMyYellow][0]);
  tTBSActelHotButton[Tbs]->SetEnabled(kFALSE);
  tTBSActelColdButton[Tbs]->SetEnabled(kFALSE);

  gClient->NeedRedraw(tTBSActelHotButton[Tbs]);
  gClient->NeedRedraw(tTBSActelColdButton[Tbs]);

  // now the linear regulator states
  tTBSLRhot[Tbs][0]->SetBackgroundColor(PixelColor[kMyYellow][0]);
  tTBSLRcold[Tbs][0]->SetBackgroundColor(PixelColor[kMyYellow][0]);
  tTBSLRhot[Tbs][1]->SetBackgroundColor(PixelColor[kMyYellow][0]);
  tTBSLRcold[Tbs][1]->SetBackgroundColor(PixelColor[kMyYellow][0]);
  tTBSLRhot[Tbs][0]->SetEnabled(kFALSE);
  tTBSLRcold[Tbs][0]->SetEnabled(kFALSE);
  tTBSLRhot[Tbs][1]->SetEnabled(kFALSE);
  tTBSLRcold[Tbs][1]->SetEnabled(kFALSE);
  
  gClient->NeedRedraw(tTBSLRhot[Tbs][0]); 
  gClient->NeedRedraw(tTBSLRcold[Tbs][0]);
  gClient->NeedRedraw(tTBSLRhot[Tbs][1]);
  gClient->NeedRedraw(tTBSLRcold[Tbs][1]);

  // now the voltage adcs
  
  UpdateTbsBiasVoltage(Tbs,0,1);
  UpdateTbsBiasVoltage(Tbs,1,1);
  
  // now the current adcs
  UpdateTbsBiasCurrents(Tbs,1);


}


void SlowGui::UpdateTKBiasVoltage(Int_t Tbs, ushort group, int cold) {

  //hot=1 means hot is on, cold is off
  //hot=0 means hot is off, cold is on

  if (group>1) return;

  sTBSLR tbslr=(sTBSLR)(2*cold+group);

  TBS *aTbs=MyTbs[Tbs][ActelTbs[Tbs]];

  ushort adcvoltage=aTbs->BiasVoltage(tbslr);
  double voltage=ADCtoVolt(adcvoltage,group,cold);

  int pwg=group+2*Tbs;
  DisplayTKVoltages(pwg, voltage);
}


void SlowGui::UpdateTbsBiasVoltage(Int_t Tbs, ushort group, Int_t empty) {
  if (group>1) return;

  TBS *aTbs=MyTbs[Tbs][ActelTbs[Tbs]];
  sTBSLR lrhot, lrcold;

  lrhot=(group)?k1Hot:k0Hot;
  lrcold=(group)?k1Cold:k0Cold;

  tTBSADChot[Tbs][group]->SetText((empty)?"---":(ADCmode)?Form("0x%04X",aTbs->BiasVoltage(lrhot)):Form("%3.1f",ADCtoVolt(aTbs->BiasVoltage(lrhot),group,0)));
  tTBSADCcold[Tbs][group]->SetText((empty)?"---":(ADCmode)?Form("0x%04X",aTbs->BiasVoltage(lrcold)):Form("%3.1f",ADCtoVolt(aTbs->BiasVoltage(lrcold),group,1)));
  gClient->NeedRedraw(tTBSADChot[Tbs][group]);
  gClient->NeedRedraw(tTBSADCcold[Tbs][group]);  

  
}


void SlowGui::UpdateTbsBiasCurrent(Int_t Tbs, ushort Tdr) {
  if (Tdr>NTdrTBS-1) return;

  TBS *aTbs=MyTbs[Tbs][ActelTbs[Tbs]];
  tTBSADCcurrent[Tbs][Tdr]->SetText((ADCmode)?Form("0x%04X",aTbs->BiasCurrent((sTBSTDR)Tdr)):Form("%3.1f",ADCtoCurrent(aTbs->BiasCurrent((sTBSTDR)Tdr),Tbs, Tdr,1 )));
  gClient->NeedRedraw(tTBSADCcurrent[Tbs][Tdr]);
}


void SlowGui::UpdateTbsBiasCurrents(Int_t Tbs, Int_t empty) {
  TBS *aTbs=MyTbs[Tbs][ActelTbs[Tbs]];

  for (sTBSTDR tdr=kTDR0; tdr<=kTDR11; tdr++) {
    tTBSADCcurrent[Tbs][tdr]->SetText((empty)?"---":(ADCmode)?Form("0x%04X",aTbs->BiasCurrent((sTBSTDR)tdr)):Form("%3.1f",ADCtoCurrent(aTbs->BiasCurrent((sTBSTDR)tdr), Tbs, tdr,1)));
    gClient->NeedRedraw(tTBSADCcurrent[Tbs][tdr]);

    int pwg=((int)tdr)/6;
    int pwgpos=((int)tdr)-pwg*6;
    pwg+=2*Tbs;
    
    FillTKGraph(pwg, pwgpos, ADCtoCurrent(aTbs->BiasCurrent((sTBSTDR)tdr), Tbs, tdr,1));

    if (tdr==kTDR11) break;
  }
  UpdateZoomCanvases();


}


void SlowGui::CheckTbsClickDockable(Event_t *event) {
  //  printf("tbs dock: %d\n",event->fType);

  fMain->Resize();
}


void SlowGui::CheckTbsUndock() {
  printf("undocked\n");
}
 
 void SlowGui::CheckTbsDock() {
   printf("docked\n");
   fMain->Resize();
 }
 
 
 Float_t SlowGui::ADCtoVolt(ushort adc, ushort group, ushort cold) {
   // to be filled here...

   Float_t par[4][2][2]={
     {{276.900000, 41.450000},{262.100000, 41.550000}},
     {{273.200000, 41.600000},{268.100000, 41.550000}},
     {{268.000000, 41.500000},{279.100000, 41.050000}},
     {{263.300000, 41.650000},{272.500000, 41.250000}}
   };

   Float_t val=(adc-par[group][cold][0])/par[group][cold][1];
   if (val<0) val=0;
   return val+2.0;
   
 }


 Float_t SlowGui::ADCtoCurrent(ushort adc, ushort Tbs, ushort Tdr, Int_t correct) {
   if (correct) correct=1;
   Float_t val=(249.0-0.0735*adc-correct*CorrADC[Tbs][Tdr]);
   //if (correct && val<0) val=0;
   return val;  // approximative formula
 }
