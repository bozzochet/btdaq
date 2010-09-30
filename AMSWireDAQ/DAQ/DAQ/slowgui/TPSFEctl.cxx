#include "slowgui.h"
#include "amswcomtest.h"
#include "SlowControl.h"



void SlowGui::CreateTpsfe(AMSWcom* node, ushort amswaddr) {

  for (sTPSFEaddr i=kTPSFE0; i<NTPSFE; i++) {
    MyTpsfe[i][kActelHot]=new TPSFE(node, amswaddr, kActelHot,i);
    MyTpsfe[i][kActelCold]=new TPSFE(node, amswaddr, kActelCold,i);

    MyTpsfe[i][kActelHot]->SetDebug(1);
    MyTpsfe[i][kActelCold]->SetDebug(1);

    ActelTpsfe[i]=kActelHot; // preselection of Actels

    ActelTpsfeState[i][kActelHot]=kActelOn;
    ActelTpsfeState[i][kActelCold]=kActelOff;

    //GetTpsfeStatus(i);

    if (i==kTPSFE3) break;
  }

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

  sACTEL first=ActelTpsfe[Tpsfe];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  TPSFE *tpsfe[2];

  tpsfe[0]=0; tpsfe[1]=0;
  tpsfe[0]=MyTpsfe[Tpsfe][first];
  if (ActelTpsfeState[Tpsfe][second]==kActelOn) tpsfe[1]=MyTpsfe[Tpsfe][second];

  // first we check if there is a trip problem, i.e. counter is 15
  // actually it seems that it is not possible to reset the SSF control
  // by sending an off-on sequence...
  //ushort cnt=tpsfe[0]->GetCntTDR(Tdr);
  //  printf("counter is 0x%x\n",cnt);
  //  if (cnt==0xf && TDRSSFstate[Tpsfe][Tdr]==kOff) {
  //    printf("here we are\n");
  //    for (int i=0; i<2;i++) if (tpsfe[i]) tpsfe[i]->SetTDR(Tdr,kOff);
  //    for (int i=0; i<2;i++) if (tpsfe[i]) tpsfe[i]->SetTDR(Tdr,kOn);
  //
  //  } else

  for (int i=0; i<2;i++) if (tpsfe[i]) tpsfe[i]->SetTDR(Tdr,(TDRSSFstate[Tpsfe][Tdr]==kOn)?kOff:kOn); 
}


void SlowGui::SwitchTpsfeLrs(Int_t Tpsfe, Int_t Tdr) {

  sACTEL first=ActelTpsfe[Tpsfe];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  TPSFE *tpsfe[2];

  tpsfe[0]=0; tpsfe[1]=0;
  tpsfe[0]=MyTpsfe[Tpsfe][first];
  if (ActelTpsfeState[Tpsfe][second]==kActelOn) tpsfe[1]=MyTpsfe[Tpsfe][second];

  for (int i=0; i<2;i++) if (tpsfe[i]) tpsfe[i]->SetLRS(Tdr,(TDRLRSstate[Tpsfe][Tdr]==kOn)?kOff:kOn);
}


void SlowGui::SwitchTpsfeLrk(Int_t Tpsfe, Int_t Tdr) {

  sACTEL first=ActelTpsfe[Tpsfe];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  TPSFE *tpsfe[2];

  tpsfe[0]=0; tpsfe[1]=0;
  tpsfe[0]=MyTpsfe[Tpsfe][first];
  if (ActelTpsfeState[Tpsfe][second]==kActelOn) tpsfe[1]=MyTpsfe[Tpsfe][second];

  for (int i=0; i<2;i++) if (tpsfe[i]) tpsfe[i]->SetLRK(Tdr,(TDRLRKstate[Tpsfe][Tdr]==kOn)?kOff:kOn);
}


void SlowGui::SwitchTpsfeAll(Int_t Tpsfe, Int_t Tdr) {

  sACTEL first=ActelTpsfe[Tpsfe];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  TPSFE *tpsfe[2];

  tpsfe[0]=0; tpsfe[1]=0;
  tpsfe[0]=MyTpsfe[Tpsfe][first];
  if (ActelTpsfeState[Tpsfe][second]==kActelOn) tpsfe[1]=MyTpsfe[Tpsfe][second];

  if (TDRSSFstate[Tpsfe][Tdr]!=TDRLRSstate[Tpsfe][Tdr] || TDRLRSstate[Tpsfe][Tdr]!=TDRLRKstate[Tpsfe][Tdr]) return;

  sDCDC newstate=(TDRSSFstate[Tpsfe][Tdr]==kOn)?kOff:kOn;

  for (int i=0; i<2;i++) if (tpsfe[i]) {
    tpsfe[i]->SetTDR(Tdr,newstate);
    tpsfe[i]->SetLRS(Tdr,newstate);
    tpsfe[i]->SetLRK(Tdr,newstate);
  }
}


void SlowGui::SwitchTpsfeTdrAll(Int_t Tpsfe) {

  sACTEL first=ActelTpsfe[Tpsfe];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  TPSFE *tpsfe[2];

  tpsfe[0]=0; tpsfe[1]=0;
  tpsfe[0]=MyTpsfe[Tpsfe][first];
  if (ActelTpsfeState[Tpsfe][second]==kActelOn) tpsfe[1]=MyTpsfe[Tpsfe][second];


  Int_t nOn=0, nOff=0;

  for (int i=0; i<NTdrTPSFE; i++) {
    nOn+=(TDRSSFstate[Tpsfe][i]==kOn);
    nOff+=(TDRSSFstate[Tpsfe][i]==kOff);
  }

  sDCDC newstate;
  if (nOn==NTdrTPSFE) newstate=kOff;
  else if (nOff==NTdrTPSFE) newstate=kOn;
  else return;

  for (int i=0; i<2;i++) if (tpsfe[i]) tpsfe[i]->SetTDR(newstate,newstate,newstate,newstate,newstate,newstate,kNoDCDCChange0);
}


void SlowGui::SwitchTpsfeLrsAll(Int_t Tpsfe) {

  sACTEL first=ActelTpsfe[Tpsfe];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  TPSFE *tpsfe[2];

  tpsfe[0]=0; tpsfe[1]=0;
  tpsfe[0]=MyTpsfe[Tpsfe][first];
  if (ActelTpsfeState[Tpsfe][second]==kActelOn) tpsfe[1]=MyTpsfe[Tpsfe][second];

  Int_t nOn=0, nOff=0;

  for (int i=0; i<NTdrTPSFE; i++) {
    nOn+=(TDRLRSstate[Tpsfe][i]==kOn);
    nOff+=(TDRLRSstate[Tpsfe][i]==kOff);
  }

  sDCDC newstate;
  if (nOn==NTdrTPSFE) newstate=kOff;
  else if (nOff==NTdrTPSFE) newstate=kOn;
  else return;

  for (int i=0; i<2;i++) if (tpsfe[i]) tpsfe[i]->SetLRS(newstate,newstate,newstate,newstate,newstate,newstate,kNoDCDCChange0);
}


void SlowGui::SwitchTpsfeLrkAll(Int_t Tpsfe) {

  sACTEL first=ActelTpsfe[Tpsfe];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  TPSFE *tpsfe[2];

  tpsfe[0]=0; tpsfe[1]=0;
  tpsfe[0]=MyTpsfe[Tpsfe][first];
  if (ActelTpsfeState[Tpsfe][second]==kActelOn) tpsfe[1]=MyTpsfe[Tpsfe][second];

  Int_t nOn=0, nOff=0;

  for (int i=0; i<NTdrTPSFE; i++) {
    nOn+=(TDRLRKstate[Tpsfe][i]==kOn);
    nOff+=(TDRLRKstate[Tpsfe][i]==kOff);
  }

  sDCDC newstate;
  if (nOn==NTdrTPSFE) newstate=kOff;
  else if (nOff==NTdrTPSFE) newstate=kOn;
  else return;

   for (int i=0; i<2;i++) if (tpsfe[i]) tpsfe[i]->SetLRK(newstate,newstate,newstate,newstate,newstate,newstate,kNoDCDCChange0);
}


void SlowGui::SwitchTpsfeAllAll(Int_t Tpsfe) {

  sACTEL first=ActelTpsfe[Tpsfe];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  TPSFE *tpsfe[2];

  tpsfe[0]=0; tpsfe[1]=0;
  tpsfe[0]=MyTpsfe[Tpsfe][first];
  if (ActelTpsfeState[Tpsfe][second]==kActelOn) tpsfe[1]=MyTpsfe[Tpsfe][second];

  Int_t nOn=0, nOff=0;

  for (int i=0; i<NTdrTPSFE; i++) {
    nOn+=(TDRSSFstate[Tpsfe][i]==kOn);
    nOff+=(TDRSSFstate[Tpsfe][i]==kOff);
    nOn+=(TDRLRSstate[Tpsfe][i]==kOn);
    nOff+=(TDRLRSstate[Tpsfe][i]==kOff);
    nOn+=(TDRLRKstate[Tpsfe][i]==kOn);
    nOff+=(TDRLRKstate[Tpsfe][i]==kOff);
  }

  sDCDC newstate;
  if (nOn==3*NTdrTPSFE) newstate=kOff;
  else if (nOff==3*NTdrTPSFE) newstate=kOn;
  else return;

  for (int i=0; i<2;i++) if (tpsfe[i]) {
    tpsfe[i]->SetTDR(newstate,newstate,newstate,newstate,newstate,newstate,kNoDCDCChange0);
    tpsfe[i]->SetLRS(newstate,newstate,newstate,newstate,newstate,newstate,kNoDCDCChange0);
    tpsfe[i]->SetLRK(newstate,newstate,newstate,newstate,newstate,newstate,kNoDCDCChange0);
  }
}



void SlowGui::GetTpsfeTrips(Int_t Tpsfe) {
  
  TPSFE *tpsfe=MyTpsfe[Tpsfe][ActelTpsfe[Tpsfe]];

  //  tpsfe->GetTripCounters();
  ShowTpsfeTrips(Tpsfe);
}



void SlowGui::GetTpsfeStatus(Int_t Tpsfe) {

  TPSFE *tpsfe=MyTpsfe[Tpsfe][ActelTpsfe[Tpsfe]];
  TPSFE *tpsfe_brother=MyTpsfe[Tpsfe][1-ActelTpsfe[Tpsfe]];
  
  tpsfe->GetActel();
  if (tpsfe->GetErrLecroy()) { 
    ShowTpsfeEmpty(Tpsfe);
    ActelTpsfeState[Tpsfe][ActelTpsfe[Tpsfe]]=kActelOff;
    return;
  }
  tpsfe->GetLRS();
  tpsfe->GetLRK();
  tpsfe->GetGControl();
  tpsfe->GetTripCounters();

  ushort state;
  for (int i=0; i<NTdrTPSFE; i++) {
    state=(1 & (tpsfe->GetStatusTDR()>>i));
    TDRSSFstate[Tpsfe][i]=(state)?kOff:kOn;
    state=(1 & (tpsfe->GetGControlLRS()>>i));
    TDRLRSstate[Tpsfe][i]=(state)?kOff:kOn;
    state=(1 & (tpsfe->GetGControlLRK()>>i));
    TDRLRKstate[Tpsfe][i]=(state)?kOff:kOn;

  }

  AutoSSF[Tpsfe]=(tpsfe->GetAutoModeTDR())?kOff:kOn;
  AutoLRS[Tpsfe]=(tpsfe->GetAutoModeLRS())?kOff:kOn;
  AutoLRK[Tpsfe]=(tpsfe->GetAutoModeLRK())?kOff:kOn;

  // we rely on various parameters...
  ActelTpsfeState[Tpsfe][ActelTpsfe[Tpsfe]]=kActelOn; // if arrive here, it is normally because the Actel is on...
  ActelTpsfeState[Tpsfe][1-ActelTpsfe[Tpsfe]]=(tpsfe->GetControl_Brother())?kActelOff:kActelOn;

  tpsfe_brother->GetActel();
  ActelTpsfeState[Tpsfe][1-ActelTpsfe[Tpsfe]]=(tpsfe_brother->GetErrLecroy())?kActelOff:kActelOn;


  // consider the trip bit, the power on bit
  ShowBrotherTrip(tpsfe);
  ShowPowerOn(tpsfe);
  DisplayTpsfeStatus(Tpsfe);
} 



void SlowGui::SwitchAutoMode(Int_t Tpsfe, Char_t *type) {

  sACTEL first=ActelTpsfe[Tpsfe];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  TPSFE *tpsfe[2];

  tpsfe[0]=0; tpsfe[1]=0;
  tpsfe[0]=MyTpsfe[Tpsfe][first];
  if (ActelTpsfeState[Tpsfe][second]==kActelOn) tpsfe[1]=MyTpsfe[Tpsfe][second];

  for (int i=0; i<2;i++) if (tpsfe[i]) {
    if (!strcmp(type,"SSF")) tpsfe[i]->SetAutoModeTDR((AutoSSF[Tpsfe]==kOn)?kOn:kOff); // looks strange ? Well it is REALLY like that !
    else if (!strcmp(type,"LRS")) tpsfe[i]->SetAutoModeLRS((AutoLRS[Tpsfe]==kOn)?kOn:kOff);
    else if (!strcmp(type,"LRK")) tpsfe[i]->SetAutoModeLRK((AutoLRK[Tpsfe]==kOn)?kOn:kOff);
  }

  GetTpsfeStatus(Tpsfe);
}

