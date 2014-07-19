#include "slowgui2.h"
#include "amswcomtest.h"
#include "SlowControl.h"



void SlowGui::CreateTpsfe() {

  /*
  for (sTPSFEaddr i=kTPSFE0; i<NTPSFE; i++) {

    ActelTpsfe[i]=kActelHot; // preselection of Actels

    ActelTpsfeState[i][kActelHot]=kActelOn;
    ActelTpsfeState[i][kActelCold]=kActelOff;

    if (i==kTPSFE3) break;
  }
  */
}



void SlowGui::ControlTpsfe(Int_t Mode, Int_t Tpsfe, Int_t Tdr) {
  printf("TPSFE=%d, TDR=%2d, Mode=%d\n", Tpsfe,Tdr,Mode);

  switch (Mode) {
  case 0: // SSF
    printf("SSF mode\n");
    if (Tdr==0xff) SwitchTpsfeTdrAll(Tpsfe); else SwitchTpsfeTdr(Tpsfe, Tdr);
    break;
  case 1: // LRS
    printf("LRS mode\n");
    if (Tdr==0xff) SwitchTpsfeLrsAll(Tpsfe); else SwitchTpsfeLrs(Tpsfe, Tdr);
    break;
  case 2: // LRK
    printf("LRK mode\n");
    if (Tdr==0xff) SwitchTpsfeLrkAll(Tpsfe); else SwitchTpsfeLrk(Tpsfe, Tdr);
    break;
  case 3: // ALL
    printf("ALL mode\n");
    if (Tdr==0xff) SwitchTpsfeAllAll(Tpsfe); else SwitchTpsfeAll(Tpsfe, Tdr);
    break;
  default:
    printf("error: you should never arrive here...\n");
  }
  usleep(200000);
  GetTpsfeStatus(Tpsfe);
}




void SlowGui::SwitchTpsfeTdr(Int_t Tpsfe, Int_t Tdr) {

  ushort param=((Tpsfe+3)<<12) + 0x100 + Tdr;
  Node->SetParameter(AMSWaddr,0x1000,param,(TDRSSFstate[Tpsfe][Tdr]==kOn)?0:1);
  Node->SDProc(AMSWaddr,0x1003);

}


void SlowGui::SwitchTpsfeLrs(Int_t Tpsfe, Int_t Tdr) {

  ushort param=((Tpsfe+3)<<12) + 0x200 + Tdr;
  Node->SetParameter(AMSWaddr,0x1000,param,(TDRLRSstate[Tpsfe][Tdr]==kOn)?0:1);
  Node->SDProc(AMSWaddr,0x1003);

}


void SlowGui::SwitchTpsfeLrk(Int_t Tpsfe, Int_t Tdr) {

  ushort param=((Tpsfe+3)<<12) + 0x300 + Tdr;
  Node->SetParameter(AMSWaddr,0x1000,param,(TDRLRKstate[Tpsfe][Tdr]==kOn)?0:1);
  Node->SDProc(AMSWaddr,0x1003);

 }


void SlowGui::SwitchTpsfeAll(Int_t Tpsfe, Int_t Tdr) {

  if (TDRSSFstate[Tpsfe][Tdr]!=TDRLRSstate[Tpsfe][Tdr] || TDRLRSstate[Tpsfe][Tdr]!=TDRLRKstate[Tpsfe][Tdr]) return;

  int newstate=(TDRSSFstate[Tpsfe][Tdr]==kOn)?0:1;

  Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x100+Tdr,newstate);
  Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x200+Tdr,newstate);
  Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x300+Tdr,newstate);
  Node->SDProc(AMSWaddr,0x1003);

}


void SlowGui::SwitchTpsfeTdrAll(Int_t Tpsfe) {

  Int_t nOn=0, nOff=0;

  for (int i=0; i<NTdrTPSFE; i++) {
    nOn+=(TDRSSFstate[Tpsfe][i]==kOn);
    nOff+=(TDRSSFstate[Tpsfe][i]==kOff);
  }

  int newstate;
  if (nOn==NTdrTPSFE) newstate=0;
  else if (nOff==NTdrTPSFE) newstate=1;
  else return;

  for (int i=0; i<6; i++) Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x100+i,newstate);
  Node->SDProc(AMSWaddr,0x1003);


}


void SlowGui::SwitchTpsfeLrsAll(Int_t Tpsfe) {

  Int_t nOn=0, nOff=0;

  for (int i=0; i<NTdrTPSFE; i++) {
    nOn+=(TDRLRSstate[Tpsfe][i]==kOn);
    nOff+=(TDRLRSstate[Tpsfe][i]==kOff);
  }

  int newstate;
  if (nOn==NTdrTPSFE) newstate=0;
  else if (nOff==NTdrTPSFE) newstate=1;
  else return;

  for (int i=0; i<6; i++) Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x200+i,newstate);
  Node->SDProc(AMSWaddr,0x1003);

}


void SlowGui::SwitchTpsfeLrkAll(Int_t Tpsfe) {

  Int_t nOn=0, nOff=0;

  for (int i=0; i<NTdrTPSFE; i++) {
    nOn+=(TDRLRKstate[Tpsfe][i]==kOn);
    nOff+=(TDRLRKstate[Tpsfe][i]==kOff);
  }

  int newstate;
  if (nOn==NTdrTPSFE) newstate=0;
  else if (nOff==NTdrTPSFE) newstate=1;
  else return;

  for (int i=0; i<6; i++) Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x300+i,newstate);
  Node->SDProc(AMSWaddr,0x1003);

}


void SlowGui::SwitchTpsfeAllAll(Int_t Tpsfe) {

  Int_t nOn=0, nOff=0;

  for (int i=0; i<NTdrTPSFE; i++) {
    nOn+=(TDRSSFstate[Tpsfe][i]==kOn);
    nOff+=(TDRSSFstate[Tpsfe][i]==kOff);
    nOn+=(TDRLRSstate[Tpsfe][i]==kOn);
    nOff+=(TDRLRSstate[Tpsfe][i]==kOff);
    nOn+=(TDRLRKstate[Tpsfe][i]==kOn);
    nOff+=(TDRLRKstate[Tpsfe][i]==kOff);
  }
 

  int newstate;
  if (nOn==3*NTdrTPSFE) newstate=0;
  else if (nOff==3*NTdrTPSFE) newstate=1;
  else return;

  for (int Tdr=0; Tdr<6; Tdr++) {
    Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x100+Tdr,newstate);
    Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x200+Tdr,newstate);
    Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x300+Tdr,newstate);
  }
  Node->SDProc(AMSWaddr,0x1003);
}



int SlowGui::GetTpsfeStatus(Int_t Tpsfe) {

  Node->SDprocRead(AMSWaddr,0x1001);
  if (TestAMSWError(Node)) return 1;
  Node->SDprocRead(AMSWaddr,0x1011); 
  if (TestAMSWError(Node)) return 1;

  ushort group=(3+Tpsfe)<<12;

  ushort state;
  for (int i=0; i<NTdrTPSFE; i++) {
    state=FindParameter(group+0x100+i);
    TDRSSFstate[Tpsfe][i]=(state)?kOn:kOff;
    state=FindParameter(group+0x200+i);
    TDRLRSstate[Tpsfe][i]=(state)?kOn:kOff;
    state=FindParameter(group+0x300+i);
    TDRLRKstate[Tpsfe][i]=(state)?kOn:kOff;
  }

  AutoSSF[Tpsfe]=(FindParameter(group+0x106))?kOff:kOn; // too keep compatibilty with previous version. only for that....
  AutoLRS[Tpsfe]=(FindParameter(group+0x206))?kOff:kOn;
  AutoLRK[Tpsfe]=(FindParameter(group+0x306))?kOff:kOn;


  ActelMask=FindParameter(0x9000);

  // ActelTpsfeState[Tpsfe][ActelTpsfe[Tpsfe]]=kActelOn; // if arrive here, it is normally because the Actel is on...
  //  ActelTpsfeState[Tpsfe][1-ActelTpsfe[Tpsfe]]=0?kActelOff:kActelOn;

  ushort acteltrip=(FindParameter(group+0x800)>0);
  ushort poweron=(FindParameter(group+0x400)>>15) & 1;
  // consider the trip bit, the power on bit
  ShowBrotherTrip(Tpsfe+3,acteltrip);
  ShowPowerOn(Tpsfe+3,poweron);
  DisplayTpsfeStatus(Tpsfe);

  return 0;
} 



void SlowGui::SwitchAutoMode(Int_t Tpsfe, Char_t *type) {

    if (!strcmp(type,"SSF")) Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x106, (AutoSSF[Tpsfe]==kOn)?1:0);
    else if (!strcmp(type,"LRS")) Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x206,(AutoLRS[Tpsfe]==kOn)?1:0);
    else if (!strcmp(type,"LRK")) Node->SetParameter(AMSWaddr,0x1000,((Tpsfe+3)<<12)+0x306,(AutoLRK[Tpsfe]==kOn)?1:0);

    Node->SDProc(AMSWaddr,0x1111,1); // enable auto-off control
    Node->SDProc(AMSWaddr,0x1003);
    Node->SDProc(AMSWaddr,0x1111,0); // disable auto-off control

    GetTpsfeStatus(Tpsfe);
}

