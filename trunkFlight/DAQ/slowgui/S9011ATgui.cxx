#include "slowgui.h"
#include "amswcomtest.h"
#include "SlowControl.h"





void SlowGui::CreateS9011ATFrame(TGCompositeFrame *frame) {

  TGHorizontalFrame *hmain=new TGHorizontalFrame(frame,40,200);

  TGVerticalFrame *vframetools=new TGVerticalFrame(hmain,200,40);
  TGPictureButton *help=CreateHelpButton(vframetools,"S9011AT");
  TGPictureButton *refr=CreateRefreshButton(vframetools,"S9011AT");
  TGPictureButton *init=CreateInitLecroyButton(vframetools,"S9011AT",0);

  vframetools->AddFrame(refr, fL35);
  vframetools->AddFrame(init, fL35);
  vframetools->AddFrame(help, fL35);

  TGVerticalFrame *vframe=new TGVerticalFrame(hmain,200,40);
  TGHorizontalFrame *hframe=new TGHorizontalFrame(vframe,40,200);
  TGHorizontalFrame *hframe2=new TGHorizontalFrame(vframe,40,200);

  BuildActelDisplay(hframe, &(tS9011ATActelHotButton), &(tS9011ATActelColdButton), &(tS9011ATActelCombo), &(tS9011ATTripIndicator), &(tS9011ATPowerOnIndicator), 0, "S9011AT");

  tS9053Enable=new TGCheckButton(hframe,"Enable S9053");
  tS9053Enable->Connect("Clicked()","SlowGui",this,Form("EnableS9053()"));
  tS9053Enable->SetOn(kFALSE);

  hframe->AddFrame(tS9053Enable, fL3);

  CreateS9051Page(hframe2);
  CreateS9053Page(hframe2);
  CreateS9055Page(hframe2);
  
  vframe->AddFrame(hframe, fL3);
  vframe->AddFrame(hframe2, fL3);

  hmain->AddFrame(vframe, fL3);
  hmain->AddFrame(vframetools, fL35);
  frame->AddFrame(hmain, fL32);

}


void SlowGui::CreateS9051Page(TGCompositeFrame *frame) {
  TGGroupFrame *s9051frame=new TGGroupFrame(frame,"S9051",kVerticalFrame);
  TGHorizontalFrame *hframe;
 
  for (int grp=0; grp<N9051; grp++) {
    hframe=new TGHorizontalFrame(s9051frame,40,200);
    CreateOvercurrentIndicator(hframe,&(tS9051OvercurrentLabel[grp][0]));
    CreateTripIndicator(hframe,&(tS9051TripLabel[grp][0]));
    CreateTextButton(hframe, &(tS9051DCDC[grp][0]), Form(" S%d ",grp));
    CreateTextButton(hframe, &(tS9051DCDC[grp][1]), Form(" K%d ",grp));
    tS9051DCDC[grp][0]->Connect("Clicked()","SlowGui",this,Form("ControlS9051(=%d,0)",grp));
    tS9051DCDC[grp][1]->Connect("Clicked()","SlowGui",this,Form("ControlS9051(=%d,1)",grp));
    CreateTripIndicator(hframe,&(tS9051TripLabel[grp][1]));
    CreateOvercurrentIndicator(hframe,&(tS9051OvercurrentLabel[grp][1]));
    hframe->AddFrame(tS9051OvercurrentLabel[grp][0], fL76);
    hframe->AddFrame(tS9051TripLabel[grp][0], fL76);
    hframe->AddFrame(tS9051DCDC[grp][0],fL7);
    hframe->AddFrame(tS9051DCDC[grp][1],fL7);
    hframe->AddFrame(tS9051TripLabel[grp][1], fL76);
    hframe->AddFrame(tS9051OvercurrentLabel[grp][1], fL76);
    s9051frame->AddFrame(hframe,fL7);
  }

  frame->AddFrame(s9051frame,fL7);
}



void SlowGui::CreateS9053Page(TGCompositeFrame *frame) {
  TGGroupFrame *s9053frame=new TGGroupFrame(frame,"S9053",kVerticalFrame);
  TGHorizontalFrame *hframe;
   
  for (int grp=0; grp<N9053; grp++) {
    hframe=new TGHorizontalFrame(s9053frame,40,200);
    CreateTripIndicator( hframe, &(tS9053TripLabel[grp][0]));
    CreateTextButton(hframe, &(tS9053DCDC[grp][0]), Form("Hot %d",grp));
    CreateOvercurrentIndicator(hframe,&(tS9053OvercurrentLabel[grp][0]));
    hframe->AddFrame(tS9053OvercurrentLabel[grp][0], fL72);
    hframe->AddFrame(tS9053DCDC[grp][0],fL73);
    hframe->AddFrame(tS9053TripLabel[grp][0], fL77);
    s9053frame->AddFrame(hframe,fL73);

    hframe=new TGHorizontalFrame(s9053frame,40,200);
    CreateTextButton(hframe, &(tS9053DCDC[grp][1]), Form("Cold %d",grp));
    CreateTripIndicator( hframe, &(tS9053TripLabel[grp][1]));
    CreateOvercurrentIndicator( hframe, &(tS9053OvercurrentLabel[grp][1]));
    hframe->AddFrame(tS9053OvercurrentLabel[grp][1], fL72);
    hframe->AddFrame(tS9053DCDC[grp][1],fL73);
    hframe->AddFrame(tS9053TripLabel[grp][1], fL77);
    s9053frame->AddFrame(hframe,fL73);

    tS9053DCDC[grp][0]->Connect("Clicked()","SlowGui",this,Form("ControlS9053(=%d,0)",grp));
    tS9053DCDC[grp][1]->Connect("Clicked()","SlowGui",this,Form("ControlS9053(=%d,1)",grp));
  }

  frame->AddFrame(s9053frame,fL7);
}



void SlowGui::CreateS9055Page(TGCompositeFrame *frame) {
  TGGroupFrame *s9055frame=new TGGroupFrame(frame,"S9055",kVerticalFrame);
  TGHorizontalFrame *hframe;
   
  for (int grp=0; grp<N9055; grp++) {
    hframe=new TGHorizontalFrame(s9055frame,40,200);
    CreateOvercurrentIndicator( hframe, &(tS9055OvercurrentLabel[grp][0]));
    CreateTripIndicator( hframe, &(tS9055TripLabel[grp][0]));
    CreateTextButton(hframe, &(tS9055DCDC[grp][0]), Form("Hot %d",grp));
    hframe->AddFrame(tS9055OvercurrentLabel[grp][0], fL72);
    hframe->AddFrame(tS9055DCDC[grp][0],fL73);
    hframe->AddFrame(tS9055TripLabel[grp][0], fL77);
    s9055frame->AddFrame(hframe,fL73);

    hframe=new TGHorizontalFrame(s9055frame,40,200);
    CreateTextButton(hframe, &(tS9055DCDC[grp][1]), Form("Cold %d",grp));
    CreateTripIndicator( hframe, &(tS9055TripLabel[grp][1]));
    CreateOvercurrentIndicator( hframe, &(tS9055OvercurrentLabel[grp][1]));
    hframe->AddFrame(tS9055OvercurrentLabel[grp][1], fL72);
    hframe->AddFrame(tS9055DCDC[grp][1],fL73);
    hframe->AddFrame(tS9055TripLabel[grp][1], fL77);
    s9055frame->AddFrame(hframe,fL73);

    tS9055DCDC[grp][0]->Connect("Clicked()","SlowGui",this,Form("ControlS9055(=%d,0)",grp));
    tS9055DCDC[grp][1]->Connect("Clicked()","SlowGui",this,Form("ControlS9055(=%d,1)",grp));
 
  }

  frame->AddFrame(s9055frame,fL7);
}
 
 
void SlowGui::EnableS9053() {

  for (int grp=0; grp<N9053; grp++) 
    for (int i=0; i<2; i++) {
      tS9053DCDC[grp][i]->SetEnabled((tS9053Enable->IsOn())?kTRUE:kFALSE);
      gClient->NeedRedraw(tS9053DCDC[grp][i]);
    }
}

 
void SlowGui::DisplayS9011atStatus() {

  Int_t mode=(ActelS9011ATState[kActelHot]==kActelOn);
  tS9011ATActelHotButton->SetBackgroundColor(PixelColor[(mode)?kMyGreen:kMyRed][0]);
  mode=(ActelS9011ATState[kActelCold]==kActelOn);
  tS9011ATActelColdButton->SetBackgroundColor(PixelColor[(mode)?kMyGreen:kMyRed][0]);
  gClient->NeedRedraw(tS9011ATActelHotButton);
  gClient->NeedRedraw(tS9011ATActelColdButton);

  ushort trip9051=MyS9011AT[ActelS9011AT]->GetTrip_S9051();
  ushort trip9053=MyS9011AT[ActelS9011AT]->GetTrip_S9053();
  ushort trip9055=MyS9011AT[ActelS9011AT]->GetTrip_S9055();

  Int_t over=0;

  for (Int_t i=0; i<N9051; i++) {
    ShowTripIndicator(tS9051TripLabel[i][0],1&(trip9051>>(i*2)));
    ShowTripIndicator(tS9051TripLabel[i][1],1&(trip9051>>(1+i*2)));

    over=(S9051overc[i][0]==kOff);
    mode=(S9051state[i][0]==kOn);
    ShowOvercurrentIndicator(tS9051OvercurrentLabel[i][0],over);
    SetS9011DCDCButton(over,mode,tS9051DCDC[i][0]);

    over=(S9051overc[i][1]==kOff);
    mode=(S9051state[i][1]==kOn);
    ShowOvercurrentIndicator(tS9051OvercurrentLabel[i][1],over);
    SetS9011DCDCButton(over,mode,tS9051DCDC[i][1]);
  }

  for (Int_t i=0; i<N9053; i++) {
    ShowTripIndicator(tS9053TripLabel[i][0],1&(trip9053>>(i*2)));
    ShowTripIndicator(tS9053TripLabel[i][1],1&(trip9053>>(1+i*2)));

    over=(S9053overc[i][0]==kOff);
    mode=(S9053state[i][0]==kOn);
    ShowOvercurrentIndicator(tS9053OvercurrentLabel[i][0],over);
    SetS9011DCDCButton(over,mode,tS9053DCDC[i][0]);

    over=(S9053overc[i][1]==kOff);
    mode=(S9053state[i][1]==kOn);
    ShowOvercurrentIndicator(tS9053OvercurrentLabel[i][1],over);
    SetS9011DCDCButton(over,mode,tS9053DCDC[i][1]);
  }

  for (Int_t i=0; i<N9055; i++) {
    ShowTripIndicator(tS9055TripLabel[i][0],1&(trip9055>>(i*2)));
    ShowTripIndicator(tS9055TripLabel[i][1],1&(trip9055>>(1+i*2)));

    over=(S9055overc[i][0]==kOff);
    mode=(S9055state[i][0]==kOn);
    ShowOvercurrentIndicator(tS9055OvercurrentLabel[i][0],over);
    SetS9011DCDCButton(over,mode,tS9055DCDC[i][0]);

    over=(S9055overc[i][1]==kOff);
    mode=(S9055state[i][1]==kOn);
    ShowOvercurrentIndicator(tS9055OvercurrentLabel[i][1],over);
    SetS9011DCDCButton(over,mode,tS9055DCDC[i][1]);
  }

  // now we should care about trips

  EnableS9053();

}


void SlowGui::SetS9011DCDCButton(Int_t over, Int_t mode, TGTextButton *button) {
  Int_t fcolor=0, bcolor=0;
    if (over && mode) { bcolor=kMyCyan; }
    else { 
      //fcolor=kBlack;
      bcolor=(mode)?kMyGreen:kMyRed;
    }
    button->SetBackgroundColor(PixelColor[bcolor][0]);
    button->SetEnabled(kTRUE);
    //SetButtonColor(button,PixelColor[fcolor][0],PixelColor[bcolor][0]);
    gClient->NeedRedraw(button);
}


void SlowGui::ShowS9011atEmpty() {

  tS9011ATActelHotButton->SetBackgroundColor(PixelColor[kMyYellow][0]);
  tS9011ATActelColdButton->SetBackgroundColor(PixelColor[kMyYellow][0]);
  tS9011ATActelHotButton->SetEnabled(kFALSE);
  tS9011ATActelColdButton->SetEnabled(kFALSE);
  gClient->NeedRedraw(tS9011ATActelHotButton);
  gClient->NeedRedraw(tS9011ATActelColdButton);

  for (Int_t i=0; i<N9051; i++) {
    tS9051DCDC[i][0]->SetBackgroundColor(PixelColor[kMyYellow][0]);
    tS9051DCDC[i][1]->SetBackgroundColor(PixelColor[kMyYellow][0]);
    tS9051DCDC[i][0]->SetEnabled(kFALSE);
    tS9051DCDC[i][1]->SetEnabled(kFALSE);
    gClient->NeedRedraw(tS9051DCDC[i][0]);
    gClient->NeedRedraw(tS9051DCDC[i][1]);
  }

  for (Int_t i=0; i<N9053; i++) {
    tS9053DCDC[i][0]->SetBackgroundColor(PixelColor[kMyYellow][0]);
    tS9053DCDC[i][1]->SetBackgroundColor(PixelColor[kMyYellow][0]);
    tS9053DCDC[i][0]->SetEnabled(kFALSE);
    tS9053DCDC[i][1]->SetEnabled(kFALSE);
    gClient->NeedRedraw(tS9053DCDC[i][0]);
    gClient->NeedRedraw(tS9053DCDC[i][1]);
  }

  for (Int_t i=0; i<N9055; i++) {
    tS9055DCDC[i][0]->SetBackgroundColor(PixelColor[kMyYellow][0]);
    tS9055DCDC[i][1]->SetBackgroundColor(PixelColor[kMyYellow][0]);
    tS9055DCDC[i][0]->SetEnabled(kFALSE);
    tS9055DCDC[i][1]->SetEnabled(kFALSE);
    gClient->NeedRedraw(tS9055DCDC[i][0]);
    gClient->NeedRedraw(tS9055DCDC[i][1]);
  }

  // now we should care about trips


}
