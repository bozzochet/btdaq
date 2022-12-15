#include "amswcomtest.h"
#include "slowgui.h"
#include "SlowControl.h"



void SlowGui::CreateSettingsFrame(TGCompositeFrame *frame) {

  //  TGHorizontalFrame *hframe=new TGHorizontalFrame(frame,40,200);
  //  TGVerticalFrame *vframeLabels=new TGVerticalFrame(hframe,200,40);
  //  TGVerticalFrame *vframeItems=new TGVerticalFrame(hframe,200,40);


  frame->SetLayoutManager(new TGMatrixLayout(frame, 7, 2, 4));


  TGLabel *alabel=new TGLabel(frame,"Communication mode:");
  frame->AddFrame(alabel);

  TGComboBox *aCombo=new TGComboBox(frame);
  aCombo->AddEntry("PCI",kAMSW_PCI);
  aCombo->AddEntry("EPP",kAMSW_EPP);
  aCombo->AddEntry("JMDC",kAMSW_JMDC);
  aCombo->Select(kAMSW_JMDC);
  aCombo->Resize(70,20);
  aCombo->Connect("Selected(Int_t)","SlowGui",this,"SelectCommunicationMode(Int_t)");
  frame->AddFrame(aCombo);

  alabel=new TGLabel(frame,"Communication port:");
  alabel->SetHeight(20);
  alabel->SetTextJustify(kTextCenterY);
  frame->AddFrame(alabel);

  aCombo=new TGComboBox(frame);
  for (int i=0; i<4; i++) aCombo->AddEntry(Form("%d",i),i);
  aCombo->Select(0);
  aCombo->Resize(70,20);
  aCombo->Connect("Selected(Int_t)","SlowGui",this,"SelectCommunicationPort(Int_t)");
  frame->AddFrame(aCombo);
 
  alabel=new TGLabel(frame,"Jinf address:");
  alabel->SetHeight(20);
  alabel->SetTextJustify(kTextCenterY);
  frame->AddFrame(alabel);

  tAMSWaddr=new TGNumberEntry(frame,0xffff,5);
  tAMSWaddr->SetFormat(TGNumberFormat::kNESHex, TGNumberFormat::kNEANonNegative);
  tAMSWaddr->SetLimits(TGNumberFormat::kNELLimitMinMax,0,0xffff);
  tAMSWaddr->Resize(70,20);
  //  aNumberEntry->Connect("ValueSet(Long_t)","SlowGui",this,"SetAMSWaddr(unsigned int)");
  frame->AddFrame(tAMSWaddr);

  alabel=new TGLabel(frame,"S9011 line:");
  alabel->SetHeight(20);
  alabel->SetTextJustify(kTextCenterY);
  frame->AddFrame(alabel);

  aCombo=new TGComboBox(frame);
  for (int i=0; i<2; i++) aCombo->AddEntry(Form("%d",i),i);
  aCombo->Select(0);
  aCombo->Resize(70,20);
  //  aCombo->Connect("Selected(Int_t)","SlowGui",this,"SelectS9011ATHalf(Int_t)");
  frame->AddFrame(aCombo);


  alabel=new TGLabel(frame,"ADC data display:");
  alabel->SetHeight(20);
  alabel->SetTextJustify(kTextCenterY);
  frame->AddFrame(alabel);

  aCombo=new TGComboBox(frame);
  aCombo->AddEntry("Converted",0);
  aCombo->AddEntry("Raw",1);
  aCombo->Select(1);
  aCombo->Resize(70,20);
  aCombo->Connect("Selected(Int_t)","SlowGui",this,"SelectADCmode(Int_t)");
  frame->AddFrame(aCombo);
 
  alabel=new TGLabel(frame,"Timer delay (min., 0=never):");
  alabel->SetHeight(20);
  alabel->SetTextJustify(kTextCenterY);
  frame->AddFrame(alabel);

  tTimerDel=new TGNumberEntry(frame,0,5);
  tTimerDel->SetFormat(TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  tTimerDel->SetLimits(TGNumberFormat::kNELLimitMinMax,0,60);
  tTimerDel->Resize(70,20);
  tTimerDel->Connect("ValueSet(Long_t)","SlowGui",this,"SetTimerDelay(Int_t)");
  frame->AddFrame(tTimerDel);


  TGTextButton *abutton=new TGTextButton(frame,"Apply");
  abutton->Connect("Clicked()","SlowGui",this,"ApplyAMSWire()");
  frame->AddFrame(abutton);
  //  abutton=new TGTextButton(frame,"Cancel");
  //  frame->AddFrame(abutton);

}


void SlowGui::SetTimerDelay(Int_t val) {

  TimerCheck->Stop();

  int timmin=tTimerDel->GetNumber();
  printf("changing timer value to %d minutes\n",timmin);
  if (timmin) TimerCheck->Start(60000*timmin,kFALSE);


}


void SlowGui::SelectCommunicationMode(Int_t mode) {
  if (mode==kAMSW_PCI || mode==kAMSW_EPP || mode==kAMSW_JMDC) {
    AMSWmode=mode;
    printf("setting mode to %d\n", mode);
    // AMSWnode->SetHW(mode);
  }

}



void SlowGui::SelectCommunicationPort(Int_t port) {
  printf("setting port to %d\n", port);
  // AMSWnode->SetPort(port);
  AMSWport=port;

}


void SlowGui::SelectADCmode(Int_t mode) {

  ADCmode=mode;
  //  printf("ADCmode is %d\n",ADCmode);
}


void SlowGui::ApplyAMSWire() {

  TimerCheck->Stop();

  printf("Shutting amsw access\n");
  Node->Shut();

  Node->SetHW(AMSWmode);
  Node->SetPort(AMSWport);
  printf("AMSWmode=%d\n", AMSWmode);

  printf("Initializing amsw access\n");
  if (Node->Init()==ERROR) {
    printf("init error\n");
    return;
  }

  unsigned int addr=tAMSWaddr->GetNumber();
  printf("setting address to %x\n",addr);
  if (MyS9011AT[kActelHot]) MyS9011AT[kActelHot]->SetAMSWaddr(addr);
  if (MyS9011AT[kActelCold]) MyS9011AT[kActelCold]->SetAMSWaddr(addr);

  for (sTPSFEaddr i=kTPSFE0; i<NTPSFE; i++) {
    MyTpsfe[i][kActelHot]->SetAMSWaddr(addr);
    MyTpsfe[i][kActelCold]->SetAMSWaddr(addr);

    if (i==kTPSFE3) break;
  }



  for (sTBSaddr i=kTBS0; i<NTBS; i++) {
    MyTbs[i][kActelHot]->SetAMSWaddr(addr);
    MyTbs[i][kActelCold]->SetAMSWaddr(addr);

    if (i==kTBS1) break;
  }

  TestAMSW(addr);

}
