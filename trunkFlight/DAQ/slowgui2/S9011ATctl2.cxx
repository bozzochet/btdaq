#include "slowgui2.h"
#include "amswcomtest.h"
#include "SlowControl.h"


void SlowGui::CreateS9011AT() {

  //  MyS9011AT[kActelHot]=new S9011AT(node, amswaddr, kActelHot,0);
  //  MyS9011AT[kActelCold]=new S9011AT(node, amswaddr, kActelCold,0);

  //MyS9011AT[kActelHot]->SetDebug(1);
  //MyS9011AT[kActelCold]->SetDebug(1);

  //ActelS9011AT=kActelHot;
  //ActelS9011ATState[kActelHot]=kActelOn;
  //ActelS9011ATState[kActelCold]=kActelOff;

  //GetS9011atStatus();

}



int SlowGui::GetS9011atStatus() {

  Node->SDprocRead(AMSWaddr,0x1001);
  if (TestAMSWError(Node)) return 1;
  Node->SDprocRead(AMSWaddr,0x1011);  
  if (TestAMSWError(Node)) return 1;

  
  //  ActelS9011ATState[1-ActelS9011AT]=0?kActelOff:kActelOn;
  
  ushort error=FindParameter(0x2400);
  ushort acteltrip=(error>>13) & 1;
  ushort poweron=(error>>15) & 1;
  ushort trips=FindParameter(0x2401);
  ActelMask=FindParameter(0x9000);

  // from here we can understand which actel is up and running

  // ActelS9011ATState[kActelHot] = (actelconfig & 5) ? kActelOn:kActelOff;
  //ActelS9011ATState[kActelCold] = (actelconfig & 0xA) ? kActelOn:kActelOff;

  ShowBrotherTrip(2,acteltrip);
  ShowPowerOn(2,poweron);

  for (Int_t i=0, cnt=0; i<N9051; i++) {
    S9051state[i][0]=(FindParameter(0x2100+cnt)?kOn:kOff);
    S9051overc[i][0]=((1<<cnt)&trips)?kOff:kOn; // ???
    cnt++;
    S9051state[i][1]=(FindParameter(0x2100+cnt)?kOn:kOff);
    S9051overc[i][1]=((1<<cnt)&trips)?kOff:kOn; // ???
    cnt++;
  }


  for (Int_t i=0, cnt=0; i<N9053; i++) {
    S9053state[i][0]=(FindParameter(0x2200+cnt)?kOn:kOff);
    S9053overc[i][0]=((1<<(cnt+8))&trips)?kOff:kOn; // ???
    cnt++;
    S9053state[i][1]=(FindParameter(0x2200+cnt)?kOn:kOff);
    S9053overc[i][1]=((1<<(cnt+8))&trips)?kOff:kOn; // ???
    cnt++;
  }


  for (Int_t i=0, cnt=0; i<N9055; i++) {
    S9055state[i][0]=(FindParameter(0x2300+cnt)?kOn:kOff);
    S9055overc[i][0]=((1<<(cnt+12))&trips)?kOff:kOn; // ???
    cnt++;
    S9055state[i][1]=(FindParameter(0x2300+cnt)?kOn:kOff);
    S9055overc[i][1]=((1<<(cnt+12))&trips)?kOff:kOn; // ???
    cnt++;
  }


  //printf("about to display results\n");
  DisplayS9011atStatus();

  return 0;
}



void SlowGui::ControlS9051(Int_t group, Int_t side) {

  ushort param=0x2100+2*group+side;
  Node->SetParameter(AMSWaddr,0x1000,param,(S9051state[group][side]==kOn)?0:1);
  Node->SDProc(AMSWaddr,0x1003);

  usleep(200000);
  GetS9011atStatus();

}



void SlowGui::ControlS9053(Int_t group, Int_t side) {

  ushort param=0x2200+2*group+side;
  Node->SetParameter(AMSWaddr,0x1000,param,(S9053state[group][side]==kOn)?0:1);
  Node->SDProc(AMSWaddr,0x1110,1);
  Node->SDProc(AMSWaddr,0x1003);
  Node->SDProc(AMSWaddr,0x1110,0);

  sleep(3);

  tS9053Enable->SetOn(kFALSE);
  GetS9011atStatus();

}


void SlowGui::ControlS9055(Int_t group, Int_t side) {

  ushort param=0x2300+2*group+side;
  Node->SetParameter(AMSWaddr,0x1000,param,(S9055state[group][side]==kOn)?0:1);
  Node->SDProc(AMSWaddr,0x1003);

  usleep(200000);
  GetS9011atStatus();

}
