#include "amswcomtest.h"
#include "slowgui2.h"
#include "SlowControl.h"



void SlowGui::CreateSettingsFrame(TGCompositeFrame *frame) {

  //  TGHorizontalFrame *hframe=new TGHorizontalFrame(frame,40,200);
  //  TGVerticalFrame *vframeLabels=new TGVerticalFrame(hframe,200,40);
  //  TGVerticalFrame *vframeItems=new TGVerticalFrame(hframe,200,40);


  frame->SetLayoutManager(new TGMatrixLayout(frame, 0, 2, 4)); // 0 means unlimited number of rows


  TGLabel *alabel=new TGLabel(frame,"Communication mode:");
  frame->AddFrame(alabel);

  aAMSWmodeCombo=new TGComboBox(frame);
  aAMSWmodeCombo->AddEntry("PCI",kAMSW_PCI);
  aAMSWmodeCombo->AddEntry("EPP",kAMSW_EPP);
  aAMSWmodeCombo->AddEntry("JMDC",kAMSW_JMDC);
  aAMSWmodeCombo->Select(AMSWmode);
  aAMSWmodeCombo->Resize(70,20);
  aAMSWmodeCombo->Connect("Selected(Int_t)","SlowGui",this,"SelectCommunicationMode(Int_t)");
  frame->AddFrame(aAMSWmodeCombo);

  alabel=new TGLabel(frame,"Port A:");
  alabel->SetHeight(20);
  alabel->SetTextJustify(kTextCenterY);
  frame->AddFrame(alabel);

  aAMSWportCombo=new TGComboBox(frame);
  for (int i=0; i<4; i++) aAMSWportCombo->AddEntry(Form("%d",i),i);
  aAMSWportCombo->Select(AMSWportA);
  aAMSWportCombo->Resize(70,20);
  aAMSWportCombo->Connect("Selected(Int_t)","SlowGui",this,"SelectCommunicationPort(Int_t)");
  frame->AddFrame(aAMSWportCombo);
 
  alabel=new TGLabel(frame,"JinfA address:");
  alabel->SetHeight(20);
  alabel->SetTextJustify(kTextCenterY);
  frame->AddFrame(alabel);

  tAMSWaddr=new TGNumberEntry(frame,AMSWaddr,5);
  tAMSWaddr->SetFormat(TGNumberFormat::kNESHex, TGNumberFormat::kNEANonNegative);
  tAMSWaddr->SetLimits(TGNumberFormat::kNELLimitMinMax,0,0xffff);
  tAMSWaddr->Resize(70,20);
  //  aNumberEntry->Connect("ValueSet(Long_t)","SlowGui",this,"SetAMSWaddr(unsigned int)");
  frame->AddFrame(tAMSWaddr);

  alabel=new TGLabel(frame,"Port B:");
  alabel->SetHeight(20);
  alabel->SetTextJustify(kTextCenterY);
  frame->AddFrame(alabel);

  aAMSWportBCombo=new TGComboBox(frame);
  for (int i=0; i<4; i++) aAMSWportBCombo->AddEntry(Form("%d",i),i);
  aAMSWportBCombo->Select(AMSWportB);
  aAMSWportBCombo->Resize(70,20);
  aAMSWportBCombo->Connect("Selected(Int_t)","SlowGui",this,"SelectCommunicationPortB(Int_t)");
  frame->AddFrame(aAMSWportBCombo);

  alabel=new TGLabel(frame,"JinfB address:");
  alabel->SetHeight(20);
  alabel->SetTextJustify(kTextCenterY);
  frame->AddFrame(alabel);

  tAMSWaddrB=new TGNumberEntry(frame,AMSWaddrB,5);
  tAMSWaddrB->SetFormat(TGNumberFormat::kNESHex, TGNumberFormat::kNEANonNegative);
  tAMSWaddrB->SetLimits(TGNumberFormat::kNELLimitMinMax,0,0xffff);
  tAMSWaddrB->Resize(70,20);
  //  aNumberEntry->Connect("ValueSet(Long_t)","SlowGui",this,"SetAMSWaddr(unsigned int)");
  frame->AddFrame(tAMSWaddrB);

  /*
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
  */

  alabel=new TGLabel(frame,"ADC data display:");
  alabel->SetHeight(20);
  alabel->SetTextJustify(kTextCenterY);
  frame->AddFrame(alabel);

  TGComboBox *aCombo=new TGComboBox(frame);
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
  frame->AddFrame(abutton, fL74);
  //  abutton=new TGTextButton(frame,"Cancel");
  //  frame->AddFrame(abutton);

}


void SlowGui::UpdateSettings() {
  printf("welcome to update settings\n");
  aAMSWmodeCombo->Select(AMSWmode);
  aAMSWportCombo->Select(AMSWportA);
  aAMSWportBCombo->Select(AMSWportB);
  tAMSWaddr->SetHexNumber(AMSWaddr);
  tAMSWaddrB->SetHexNumber(AMSWaddrB);
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
  printf("setting port A to %d\n", port);
  // AMSWnode->SetPort(port);
  AMSWportA=port;

}

void SlowGui::SelectCommunicationPortB(Int_t port) {
  printf("setting port B to %d\n", port);
  // AMSWnode->SetPort(port);
  AMSWportB=port;

}

void SlowGui::SelectADCmode(Int_t mode) {

  ADCmode=mode;
  //  printf("ADCmode is %d\n",ADCmode);
}


void SlowGui::ApplyAMSWire() {

  TimerCheck->Stop();

  printf("Shutting amsw access\n");
  Node->Shut();
  NodeB->Shut();

  Node->SetHW(AMSWmode);
  NodeB->SetHW(AMSWmode);

  Node->SetPort(AMSWportA);
  NodeB->SetPort(AMSWportB);
  //  printf("AMSWmode=%d\n", AMSWmode);

  printf("Initializing amsw access\n");
  if (Node->Init()==ERROR) {
    printf("init port A error\n");
    return;
  }

 if (NodeB->Init()==ERROR) {
    printf("init port B error\n");
    return;
  }

  //printf("setting address to %x\n",addr);
  //  if (MyS9011AT[kActelHot]) MyS9011AT[kActelHot]->SetAMSWaddr(addr);
  //  if (MyS9011AT[kActelCold]) MyS9011AT[kActelCold]->SetAMSWaddr(addr);

  /*
  for (sTPSFEaddr i=kTPSFE0; i<NTPSFE; i++) {
    MyTpsfe[i][kActelHot]->SetAMSWaddr(addr);
    MyTpsfe[i][kActelCold]->SetAMSWaddr(addr);

    if (i==kTPSFE3) break;
  }
  */

  /*
  for (sTBSaddr i=kTBS0; i<NTBS; i++) {
    MyTbs[i][kActelHot]->SetAMSWaddr(addr);
    MyTbs[i][kActelCold]->SetAMSWaddr(addr);

    if (i==kTBS1) break;
  }
  */
  unsigned int addr=tAMSWaddr->GetNumber();
  unsigned int addrb=tAMSWaddrB->GetNumber();


  AMSWaddr = addr;
  AMSWaddrB = addrb;
  TestAMSW();
}
