#include "slowgui.h"
#include "amswcomtest.h"
#include "SlowControl.h"



void SlowGui::CreateTpsfeFrame(TGCompositeFrame *frame) {
  // About the Connect syntax, have a look at the Root Guide...
  // The first value of CheckTab is dummy, as this will be the value
  // transmitted by Select(Int_t) i.e. the selected tab.
  // here we create four tabs, one for each TPSFE
  fTPSFETab=new TGTab(frame, 200, 200);
  fTPSFETab->Connect("Selected(Int_t)","SlowGui",this,"CheckTab(=0,\"TPSFE\")");

  for (Int_t i=0; i<NTPSFE; i++) {
    tTPSFEpage[i]=fTPSFETab->AddTab(Form("TPSFE %d",i));
    TPSFETabColor[i]=kMyYellow;
    FillTpsfePage(i);
  }
  UpdateTabColor("TPSFE");

  frame->AddFrame(fTPSFETab, fL5);

}



void SlowGui::FillTpsfePage(Int_t page) {
  TGHorizontalFrame *hmain=new TGHorizontalFrame(tTPSFEpage[page],40,200);

  TGVerticalFrame *vframetools=new TGVerticalFrame(hmain,200,40);
  TGPictureButton *help=CreateHelpButton(vframetools,"TPSFE");
  TGPictureButton *refr=CreateRefreshButton(vframetools,"TPSFE");
  TGPictureButton *init=CreateInitLecroyButton(vframetools,"TPSFE",page);
  vframetools->AddFrame(refr, fL35);
  vframetools->AddFrame(init, fL35);
  vframetools->AddFrame(help, fL35);

  //  TGVerticalFrame *vframe=new TGVerticalFrame(tTPSFEpage[page],200,40);
  TGVerticalFrame *vframe=new TGVerticalFrame(hmain,200,40);
  TGHorizontalFrame *hframe=new TGHorizontalFrame(vframe,40,200);
  TGHorizontalFrame *hframe2=new TGHorizontalFrame(vframe,40,200);

  BuildActelDisplay(hframe, &(tTPSFEActelHotButton[page]), &(tTPSFEActelColdButton[page]), &(tTPSFEActelCombo[page]), &(tTPSFETripIndicator[page]), &(tTPSFEPowerOnIndicator[page]), page, "TPSFE");


  // we go on with the TDR SSF LRS and LRK buttons
  TGVerticalFrame *vframetdr[NTdrTPSFE+1];

  TGVerticalFrame *avframe;
  TGLabel *alabel;

  for (int i=0; i<NTdrTPSFE; i++) {
    vframetdr[i]=new TGVerticalFrame(hframe2,200,40);

    tTPSFETdrLabel[page][i]=new TGLabel(vframetdr[i],Form("TDR %d",TPSFEtoTDR(page,i)));

    CreateTextButton(vframetdr[i], &(tTDRSSF[page][i]), Form("SSF %d",i));
    CreateTextButton(vframetdr[i], &(tTDRLRS[page][i]), Form("LRS %d",i));
    CreateTextButton(vframetdr[i], &(tTDRLRK[page][i]), Form("LRK %d",i));
    tTDRALL[page][i]=new TGTextButton(vframetdr[i],Form("ALL %d",i));

    tTDRSSF[page][i]->Connect("Clicked()","SlowGui",this,Form("ControlTpsfe(=0,%d,%d)",page,i));
    tTDRLRS[page][i]->Connect("Clicked()","SlowGui",this,Form("ControlTpsfe(=1,%d,%d)",page,i));
    tTDRLRK[page][i]->Connect("Clicked()","SlowGui",this,Form("ControlTpsfe(=2,%d,%d)",page,i));
    tTDRALL[page][i]->Connect("Clicked()","SlowGui",this,Form("ControlTpsfe(=3,%d,%d)",page,i));

    vframetdr[i]->AddFrame(tTPSFETdrLabel[page][i]);
    vframetdr[i]->AddFrame(tTDRSSF[page][i], fL7);
    vframetdr[i]->AddFrame(tTDRLRS[page][i], fL7);
    vframetdr[i]->AddFrame(tTDRLRK[page][i], fL7);
    vframetdr[i]->AddFrame(tTDRALL[page][i], fL7);

    hframe2->AddFrame(vframetdr[i], fL7);
  }

  // now we put the "all buttons"
  vframetdr[NTdrTPSFE]=new TGVerticalFrame(hframe2,200,40);
  tTPSFETdrLabel[page][NTdrTPSFE]=new TGLabel(vframetdr[NTdrTPSFE],"All");
  tTDRSSF[page][NTdrTPSFE]=new TGTextButton(vframetdr[NTdrTPSFE],"All SSF");
  tTDRLRS[page][NTdrTPSFE]=new TGTextButton(vframetdr[NTdrTPSFE],"All LRS");
  tTDRLRK[page][NTdrTPSFE]=new TGTextButton(vframetdr[NTdrTPSFE],"All LRK");
  tTDRALL[page][NTdrTPSFE]=new TGTextButton(vframetdr[NTdrTPSFE],"ALL");
  tTDRSSF[page][NTdrTPSFE]->Connect("Clicked()","SlowGui",this,Form("ControlTpsfe(=0,%d,%d)",page,0xff));
  tTDRLRS[page][NTdrTPSFE]->Connect("Clicked()","SlowGui",this,Form("ControlTpsfe(=1,%d,%d)",page,0xff));
  tTDRLRK[page][NTdrTPSFE]->Connect("Clicked()","SlowGui",this,Form("ControlTpsfe(=2,%d,%d)",page,0xff));
  tTDRALL[page][NTdrTPSFE]->Connect("Clicked()","SlowGui",this,Form("ControlTpsfe(=3,%d,%d)",page,0xff));
  vframetdr[NTdrTPSFE]->AddFrame(tTPSFETdrLabel[page][NTdrTPSFE]);
  vframetdr[NTdrTPSFE]->AddFrame(tTDRSSF[page][NTdrTPSFE], fL7);
  vframetdr[NTdrTPSFE]->AddFrame(tTDRLRS[page][NTdrTPSFE], fL7);
  vframetdr[NTdrTPSFE]->AddFrame(tTDRLRK[page][NTdrTPSFE], fL7);
  vframetdr[NTdrTPSFE]->AddFrame(tTDRALL[page][NTdrTPSFE], fL7);
  hframe2->AddFrame(vframetdr[NTdrTPSFE], fL7);

  vframe->AddFrame(hframe, fL3);
  vframe->AddFrame(hframe2, fL3);

  TripFrame[page]=new TGDockableFrame(vframe);
  TripFrame[page]->Connect("ProcessedEvent(Event_t*)","SlowGui",this,"CheckTpsfeClickDockable(Event_t*)");
  TripFrame[page]->EnableUndock(kFALSE);
  TGHorizontalFrame *hframe3=new TGHorizontalFrame(TripFrame[page],40,200);
  //TGLabel *alabel=new TGLabel(TripFrame,"Trip counters");
  //TripFrame->AddFrame(alabel,fL3);

  //TGVerticalFrame *avframe;
  //TGLabel *alabel;


  for (int i=0; i<NTdrTPSFE; i++) {
    avframe=new TGVerticalFrame(hframe3,200,40);
    alabel=new TGLabel(avframe,Form("TDR %d",TPSFEtoTDR(page,i)));
    TripLabelSSF[page][i]=new TGLabel(avframe,"0");
    TripLabelLRS[page][i]=new TGLabel(avframe,"0");
    TripLabelLRK[page][i]=new TGLabel(avframe,"0");
    SetLabelColor(TripLabelSSF[page][i], PixelColor[kMyGreen][0], PixelColor[kMyBlack][0]);
    SetLabelColor(TripLabelLRS[page][i], PixelColor[kMyGreen][0], PixelColor[kMyBlack][0]);
    SetLabelColor(TripLabelLRK[page][i], PixelColor[kMyGreen][0], PixelColor[kMyBlack][0]);
    avframe->AddFrame(alabel, fL7);
    avframe->AddFrame(TripLabelSSF[page][i], fL74);
    avframe->AddFrame(TripLabelLRS[page][i], fL74);
    avframe->AddFrame(TripLabelLRK[page][i], fL74);
    hframe3->AddFrame(avframe, fL3);
  }

  avframe=new TGVerticalFrame(hframe3,200,40);
  alabel=new TGLabel(avframe,"Auto off");  avframe->AddFrame(alabel,fL7);
  tAutoSSFButton[page]=new TGCheckButton(avframe,"SSF"); avframe->AddFrame(tAutoSSFButton[page], fL74);
  tAutoLRSButton[page]=new TGCheckButton(avframe,"LRS"); avframe->AddFrame(tAutoLRSButton[page], fL74);
  tAutoLRKButton[page]=new TGCheckButton(avframe,"LRK"); avframe->AddFrame(tAutoLRKButton[page], fL74);

  tAutoSSFButton[page]->Connect("Clicked()","SlowGui",this,Form("SwitchAutoMode(=%d,\"SSF\")",page));
  tAutoLRSButton[page]->Connect("Clicked()","SlowGui",this,Form("SwitchAutoMode(=%d,\"LRS\")",page));
  tAutoLRKButton[page]->Connect("Clicked()","SlowGui",this,Form("SwitchAutoMode(=%d,\"LRK\")",page));

  hframe3->AddFrame(avframe, fL3);

  TripFrame[page]->AddFrame(hframe3, fL3);

  vframe->AddFrame(TripFrame[page],fL3);

  //tTPSFEpage[page]->AddFrame(vframe, fL3);
  hmain->AddFrame(vframe, fL3);
  hmain->AddFrame(vframetools, fL35);
  tTPSFEpage[page]->AddFrame(hmain, fL32);
}


void SlowGui::CheckTpsfeClickDockable(Event_t *event) {
  //  printf("tpsfe... dock\n");

  //  GetTpsfeStatus(0);
  //  GetTpsfeStatus(3);


  fMain->Resize();
}




Int_t SlowGui::TPSFEtoTDR(Int_t tpsfe, Int_t tdr) {
  // 0  4  8   12 16 20   22 18 14   10  6  2
  // 1  5  9   13 17 21   23 19 15   11  7  3

  Int_t TPSFETdrIndex[NTPSFE][NTdrTPSFE]={0,1,4,5,8,9,
					  12,13,16,17,20,21,
					  22,23,18,19,14,15,
					  10,11,6,7,2,3 };
  return TPSFETdrIndex[tpsfe][tdr];
}



void SlowGui::DisplayTpsfeStatus(Int_t Tpsfe) {

  Int_t mode=(ActelTpsfeState[Tpsfe][kActelHot]==kActelOn);
  tTPSFEActelHotButton[Tpsfe]->SetBackgroundColor(PixelColor[(mode)?kMyGreen:kMyRed][0]);
  mode=(ActelTpsfeState[Tpsfe][kActelCold]==kActelOn);
  tTPSFEActelColdButton[Tpsfe]->SetBackgroundColor(PixelColor[(mode)?kMyGreen:kMyRed][0]);
  gClient->NeedRedraw(tTPSFEActelHotButton[Tpsfe]);
  gClient->NeedRedraw(tTPSFEActelColdButton[Tpsfe]);

  for (int i=0; i<NTdrTPSFE; i++) {
    tTDRSSF[Tpsfe][i]->SetBackgroundColor(PixelColor[(TDRSSFstate[Tpsfe][i]==kOff)?kMyRed:kMyGreen][0]);   // SSF: 1=off, 0=on
    tTDRLRS[Tpsfe][i]->SetBackgroundColor(PixelColor[(TDRLRSstate[Tpsfe][i]==kOff)?kMyRed:kMyGreen][0]); // LRS: 1=off, 0=on
    tTDRLRK[Tpsfe][i]->SetBackgroundColor(PixelColor[(TDRLRKstate[Tpsfe][i]==kOff)?kMyRed:kMyGreen][0]); // LRK: 1=off, 0=on

    tTDRSSF[Tpsfe][i]->SetEnabled(kTRUE);
    tTDRLRS[Tpsfe][i]->SetEnabled(kTRUE);
    tTDRLRK[Tpsfe][i]->SetEnabled(kTRUE);

    gClient->NeedRedraw(tTDRSSF[Tpsfe][i]);
    gClient->NeedRedraw(tTDRLRS[Tpsfe][i]);
    gClient->NeedRedraw(tTDRLRK[Tpsfe][i]);

    UpdateTKTPSFE(Tpsfe, i, 2, (TDRSSFstate[Tpsfe][i]==kOn));
    UpdateTKTPSFE(Tpsfe, i, 0, (TDRLRSstate[Tpsfe][i]==kOn));
    UpdateTKTPSFE(Tpsfe, i, 1, (TDRLRKstate[Tpsfe][i]==kOn));
  }

  ShowTpsfeTrips(Tpsfe);


}


void SlowGui::ShowTpsfeEmpty(Int_t Tpsfe) {

  tTPSFEActelHotButton[Tpsfe]->SetBackgroundColor(PixelColor[kMyYellow][0]);
  tTPSFEActelColdButton[Tpsfe]->SetBackgroundColor(PixelColor[kMyYellow][0]);
  tTPSFEActelHotButton[Tpsfe]->SetEnabled(kFALSE);
  tTPSFEActelColdButton[Tpsfe]->SetEnabled(kFALSE);
  gClient->NeedRedraw(tTPSFEActelHotButton[Tpsfe]);
  gClient->NeedRedraw(tTPSFEActelColdButton[Tpsfe]);

  for (int i=0; i<NTdrTPSFE; i++) {
    tTDRSSF[Tpsfe][i]->SetBackgroundColor(PixelColor[kMyYellow][0]);   // SSF: 1=off, 0=on
    tTDRLRS[Tpsfe][i]->SetBackgroundColor(PixelColor[kMyYellow][0]); // LRS: 1=off, 0=on
    tTDRLRK[Tpsfe][i]->SetBackgroundColor(PixelColor[kMyYellow][0]); // LRK: 1=off, 0=on
    tTDRSSF[Tpsfe][i]->SetEnabled(kFALSE);
    tTDRLRS[Tpsfe][i]->SetEnabled(kFALSE);
    tTDRLRK[Tpsfe][i]->SetEnabled(kFALSE);

    gClient->NeedRedraw(tTDRSSF[Tpsfe][i]);
    gClient->NeedRedraw(tTDRLRS[Tpsfe][i]);
    gClient->NeedRedraw(tTDRLRK[Tpsfe][i]);
  }

  ShowTpsfeTrips(Tpsfe,1);


}



void SlowGui::ShowTpsfeTrips(Int_t Tpsfe, Int_t empty) {

  TPSFE *tpsfe=MyTpsfe[Tpsfe][ActelTpsfe[Tpsfe]];

  ushort cnt=0;

  for (int i=0; i<NTdrTPSFE; i++) {
    cnt=tpsfe->GetCntTDR(i);
    TripLabelSSF[Tpsfe][i]->SetTextColor(PixelColor[(cnt)?kMyRed:kMyGreen][0]);
    TripLabelSSF[Tpsfe][i]->SetText((empty)?"---":Form("%d",cnt));
    if (cnt==0xf) tTDRSSF[Tpsfe][i]->SetBackgroundColor(PixelColor[kMyMagenta][0]);
//SetButtonColor(tTDRSSF[Tpsfe][i],PixelColor[kMyYellow][0],PixelColor[kMyBlack][1]);
    UpdateTKTrips(Tpsfe, i, 2, cnt);

    cnt=tpsfe->GetCntLRS(i);
    TripLabelLRS[Tpsfe][i]->SetTextColor(PixelColor[(cnt)?kMyRed:kMyGreen][0]);
    TripLabelLRS[Tpsfe][i]->SetText((empty)?"---":Form("%d",cnt));
    UpdateTKTrips(Tpsfe, i, 0, cnt);

    cnt=tpsfe->GetCntLRK(i);
    TripLabelLRK[Tpsfe][i]->SetTextColor(PixelColor[(cnt)?kMyRed:kMyGreen][0]);
    TripLabelLRK[Tpsfe][i]->SetText((empty)?"---":Form("%d",cnt));
    UpdateTKTrips(Tpsfe, i, 1, cnt);

    gClient->NeedRedraw(TripLabelSSF[Tpsfe][i]);
    gClient->NeedRedraw(TripLabelLRS[Tpsfe][i]);
    gClient->NeedRedraw(TripLabelLRK[Tpsfe][i]);

  }
  
  tAutoSSFButton[Tpsfe]->SetState((empty)?kButtonUp:(AutoSSF[Tpsfe]==kOff)?kButtonDown:kButtonUp);
  tAutoLRSButton[Tpsfe]->SetState((empty)?kButtonUp:(AutoLRS[Tpsfe]==kOff)?kButtonDown:kButtonUp);
  tAutoLRKButton[Tpsfe]->SetState((empty)?kButtonUp:(AutoLRK[Tpsfe]==kOff)?kButtonDown:kButtonUp);

  // note: there are also the Actel Brother trips...
}




