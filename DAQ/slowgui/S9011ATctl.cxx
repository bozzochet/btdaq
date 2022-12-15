#include "slowgui.h"
#include "amswcomtest.h"
#include "SlowControl.h"


void SlowGui::CreateS9011AT(AMSWcom* node, ushort amswaddr) {

  MyS9011AT[kActelHot]=new S9011AT(node, amswaddr, kActelHot,0);
  MyS9011AT[kActelCold]=new S9011AT(node, amswaddr, kActelCold,0);

  MyS9011AT[kActelHot]->SetDebug(1);
  MyS9011AT[kActelCold]->SetDebug(1);

  ActelS9011AT=kActelHot;
  ActelS9011ATState[kActelHot]=kActelOn;
  ActelS9011ATState[kActelCold]=kActelOff;

  //GetS9011atStatus();

}



void SlowGui::GetS9011atStatus() {

  S9011AT *s9011=MyS9011AT[ActelS9011AT];
  S9011AT *s9011_brother=MyS9011AT[1-ActelS9011AT];
  
  s9011->GetActel();
  //  printf("errlecroy=0x%x\n", s9011->GetErrLecroy());
  if (s9011->GetErrLecroy()) { 
    ShowS9011atEmpty();
    ActelS9011ATState[ActelS9011AT]=kActelOff; // actel does not reply, he is probably off
    return;
  }
  s9011->GetS9051();
  s9011->GetS905355();
  s9011->GetGlobalControl();
  s9011->GetTripStatus();

  ActelS9011ATState[ActelS9011AT]=kActelOn; // if we arrive here, it is because the actel is alive...
  // ActelS9011ATState[1-ActelS9011AT]=(s9011->GetControl_Brother())?kActelOff:kActelOn;
  s9011_brother->GetActel();
  //  printf("errlecroy=0x%x\n", s9011->GetErrLecroy());
  ActelS9011ATState[1-ActelS9011AT]=(s9011_brother->GetErrLecroy())?kActelOff:kActelOn;




  ShowBrotherTrip(s9011);
  ShowPowerOn(s9011);

  ushort gcontrol=s9011->GetGControl_S9051();
  ushort status=s9011->GetStatus_S9051();
  for (Int_t i=0, cnt=0; i<N9051; i++) {
    S9051state[i][0]=((gcontrol>>cnt)&1)?kOff:kOn;
    S9051overc[i][0]=((status>>cnt)&1)?kOff:kOn;
    cnt++;
    S9051state[i][1]=((gcontrol>>cnt)&1)?kOff:kOn;
    S9051overc[i][1]=((status>>cnt)&1)?kOff:kOn;
    cnt++;
  }

  gcontrol=s9011->GetGControl_S9053();
  status=s9011->GetStatus_S9053();
  for (Int_t i=0, cnt=0; i<N9053; i++) {
    S9053state[i][0]=((gcontrol>>cnt)&1)?kOff:kOn;
    S9053overc[i][0]=((status>>cnt)&1)?kOff:kOn;
    cnt++;
    S9053state[i][1]=((gcontrol>>cnt)&1)?kOn:kOff; // inversion of kOn and kOff is not an error
    S9053overc[i][1]=((status>>cnt)&1)?kOff:kOn; // for overcurrent, this is the standard convention
    cnt++;
  }

  gcontrol=s9011->GetGControl_S9055();
  status=s9011->GetStatus_S9055();
  for (Int_t i=0, cnt=0; i<N9055; i++) {
    S9055state[i][0]=((gcontrol>>cnt)&1)?kOff:kOn;
    S9055overc[i][0]=((status>>cnt)&1)?kOff:kOn;
    cnt++;
    S9055state[i][1]=((gcontrol>>cnt)&1)?kOff:kOn;
    S9055overc[i][1]=((status>>cnt)&1)?kOff:kOn;
    cnt++;
  }



  DisplayS9011atStatus();
}



void SlowGui::ControlS9051(Int_t group, Int_t side) {

  sACTEL first=ActelS9011AT;
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  S9011AT *s9011[2];

  s9011[0]=0; s9011[1]=0;
  s9011[0]=MyS9011AT[first];
  if (ActelS9011ATState[second]==kActelOn) s9011[1]=MyS9011AT[second];

  for (int i=0; i<2;i++) if (s9011[i]) s9011[i]->SetS9051(group,side,(S9051state[group][side]==kOn)?kOff:kOn);

  usleep(200000);
  GetS9011atStatus();

}



void SlowGui::ControlS9053(Int_t group, Int_t side) {

  sACTEL first=ActelS9011AT;
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  S9011AT *s9011[2];

  s9011[0]=0; s9011[1]=0;
  s9011[0]=MyS9011AT[first];
  if (ActelS9011ATState[second]==kActelOn) s9011[1]=MyS9011AT[second];
  
  for (int i=0; i<2;i++) if (s9011[i]) s9011[i]->SetS9053(group,side,(S9053state[group][side]==kOn)?kOff:kOn);

  sleep(3);
  tS9053Enable->SetOn(kFALSE);
  GetS9011atStatus();

}


void SlowGui::ControlS9055(Int_t group, Int_t side) {

  sACTEL first=ActelS9011AT;
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  S9011AT *s9011[2];

  s9011[0]=0; s9011[1]=0;
  s9011[0]=MyS9011AT[first];
  if (ActelS9011ATState[second]==kActelOn) s9011[1]=MyS9011AT[second];

  for (int i=0; i<2;i++) if (s9011[i]) s9011[i]->SetS9055(group,side,(S9055state[group][side]==kOn)?kOff:kOn);

  usleep(200000);
  GetS9011atStatus();

}
