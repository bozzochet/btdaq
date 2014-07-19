#include "slowgui.h"
#include "amswcomtest.h"
#include "SlowControl.h"



void SlowGui::CreateTbs(AMSWcom* node, ushort amswaddr) {

  for (sTBSaddr i=kTBS0; i<NTBS; i++) {
    MyTbs[i][kActelHot]=new TBS(node, amswaddr, kActelHot,i);
    MyTbs[i][kActelCold]=new TBS(node, amswaddr, kActelCold,i);
    MyTbs[i][kActelHot]->SetDebug(1);
    MyTbs[i][kActelCold]->SetDebug(1);

    ActelTbs[i]=kActelHot; // preselection of Actels

    ActelTbsState[i][kActelHot]=kActelOn;
    ActelTbsState[i][kActelCold]=kActelOff;

    Voltages[i][k0Hot]=k80V;
    Voltages[i][k1Hot]=k80V;
    Voltages[i][k0Cold]=k80V;
    Voltages[i][k1Cold]=k80V;

    //GetTbsStatus(i);

    if (i==kTBS1) break;
  }

  memset(CorrADC,0,sizeof(CorrADC));
  InitCorrCurrent();

}



void SlowGui::GetTbsADCBiasVoltage(Int_t Tbs, Int_t group, Int_t update) {
  TBS *tbs=MyTbs[Tbs][ActelTbs[Tbs]];

  printf("getting voltage adc for tbs %d, group %d\n",Tbs,group);
  
  if (group) {
    tbs->GetBiasVoltage(k1Hot);
    tbs->GetBiasVoltage(k1Cold);
  } else {
    tbs->GetBiasVoltage(k0Hot);
    tbs->GetBiasVoltage(k0Cold);
  }
  
  if (update) GetTbsStatus(Tbs);//UpdateTbsBiasVoltage(Tbs, group);
}


void SlowGui::GetTbsADCBiasCurrent(Int_t Tbs, ushort Tdr, Int_t update) {
  if (Tdr>NTdrTBS-1) return;

  printf("getting the adc current for TBS %d, TDR %d\n", Tbs, Tdr);

  TBS *tbs=MyTbs[Tbs][ActelTbs[Tbs]];
  tbs->GetBiasCurrent((sTBSTDR)Tdr);

  if (update) UpdateTbsBiasCurrent(Tbs, Tdr);
}


void SlowGui::GetTbsADCBiasCurrents(Int_t Tbs, Int_t update) {

  printf("Get all currents from TBS %d\n", Tbs);

  TBS *tbs=MyTbs[Tbs][ActelTbs[Tbs]];
  tbs->GetBiasCurrents();

  if (update) UpdateTbsBiasCurrents(Tbs);
}


void SlowGui::GetTbsStatus(Int_t Tbs) {

  TBS *tbs=MyTbs[Tbs][ActelTbs[Tbs]];
  TBS *tbs_brother=MyTbs[Tbs][1-ActelTbs[Tbs]];

 
  tbs->GetActel();
  if (tbs->GetErrLecroy()) { 
    ShowTbsEmpty(Tbs);
    ActelTbsState[Tbs][ActelTbs[Tbs]]=kActelOff;  // actel does not reply, he is probably off
    return;
  }
  tbs->GetLRState();
  tbs->GetBiasVoltages();
  tbs->GetBiasCurrents();
 
  // we rely on various parameters...
  // first, the Actel
  ActelTbsState[Tbs][ActelTbs[Tbs]]=kActelOn; // if we are here, it normally means that the actel replied to the commands...
  //  ActelTbsState[Tbs][1-ActelTbs[Tbs]]=(tbs->GetControl_Brother())?kActelOff:kActelOn;
  tbs_brother->GetActel();
  ActelTbsState[Tbs][1-ActelTbs[Tbs]]=(tbs_brother->GetErrLecroy())?kActelOff:kActelOn;

  ShowBrotherTrip(tbs);
  ShowPowerOn(tbs);
  // then the LR
  ushort lrstat=tbs->GetGControlLR();
  for (Int_t i=0; i<4; i++) {
    TBSlrState[Tbs][i]=((lrstat>>i)&1)?kOff:kOn;
    Voltages[Tbs][i]=((lrstat>>(i+4))&1)?k60V:k80V;
  }

  if (TBSlrState[Tbs][k0Hot]==kOn) TBSlrState[Tbs][k0Cold]=kOff;
  if (TBSlrState[Tbs][k1Hot]==kOn) TBSlrState[Tbs][k1Cold]=kOff;
  // if (TBSlrState[Tbs][0]==kOff && TBSlrState[Tbs][2]==kOn;
  printf("lr states: %d %d %d %d\n",TBSlrState[Tbs][k0Hot],TBSlrState[Tbs][k1Hot],TBSlrState[Tbs][k0Cold],TBSlrState[Tbs][k1Cold]);

  GetTbsADCBiasVoltage(Tbs,0,0);
  GetTbsADCBiasVoltage(Tbs,1,0);
  GetTbsADCBiasCurrents(Tbs,0);
  // ok, now everything is ready, we display the results

  DisplayTbsStatus(Tbs);

}



void SlowGui::ControlTbsVoltage(Int_t Tbs, Int_t group, Int_t voltage) {
  sVOLT newvolt;
  if (voltage==80) newvolt=k80V;
  else if (voltage==60) newvolt=k60V;
  else return;

  printf("TBS %d: setting  group %d at %d V\n",Tbs, group, voltage);

  
  sACTEL first=ActelTbs[Tbs];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;
  MyTbs[Tbs][first]->SetVoltage(group, newvolt); 
  if (ActelTbsState[Tbs][second]==kActelOn) MyTbs[Tbs][second]->SetVoltage(group, newvolt);
  
  usleep(300000); // give some time to the voltages to adapt
  GetTbsStatus(Tbs);
  // regarding cosmetics, I think it should be done somewhere else...

  // cosmetics
  //  if (Voltages[Tbs][group]==k80V) Voltages[Tbs][group]=k60V;
  //  else if (Voltages[Tbs][group]==k60V) Voltages[Tbs][group]=k80V;
  //  else return;
  //  printf("before change\n");
  //  tTBSLRhot[Tbs][group]->SetText(Form("Hot %d V",(Voltages[Tbs][group]==k80V)?80:60));
  //  tTBSLRcold[Tbs][group]->SetText(Form("Cold %d V",(Voltages[Tbs][group]==k80V)?80:60));
  //tTBSVOLT[Tbs][group]->SetText(Form("to %d V",(Voltages[Tbs][group]==k80V)?60:80));

}



void SlowGui::ControlTbsLr(Int_t Tbs, Int_t group, Int_t lr) {

  printf("TBS: %d: switching LR %d of group %d\n",Tbs,lr,group);

  sACTEL first=ActelTbs[Tbs];
  sACTEL second=(first==kActelHot)?kActelCold:kActelHot;

  TBS *tbs[2];

  tbs[0]=0; tbs[1]=0;
  tbs[0]=MyTbs[Tbs][first];
  if (ActelTbsState[Tbs][second]==kActelOn) tbs[1]=MyTbs[Tbs][second];

  // so first, we need to know in which state the system is supposed to be
  sDCDC state=TBSlrState[Tbs][group+lr*2];
  sDCDC newstate=kNoDCDCChange0;
  if (state==kOn) newstate=kOff;
  else if (state==kOff) newstate=kOn;
  else return;

  printf("lr=%d, newstate=%s\n",lr,(newstate==kOn)?"kOn":(newstate==kOff)?"kOff":"xxx");
  
  // the point is that things are not as simple as it looks to be...
  // if I want to switch off hot, I have to send off to both hot and cold,
  // else cold starts up.
  // If I want to switch on cold, it means switching off hot.
  // and hot + cold cannot be on at the same time.
  // also, if both Actels are on, we have to make sure that the OR combination 
  // will be the expected behaviour. Thus the easiest it to send the same command to both Actels.
  if (lr==0 && newstate==kOff) {
    for (int i=0; i<2;i++) if (tbs[i]) tbs[i]->SetLR(group, kOff, kOff);
  }
  else if (lr==0 && newstate==kOn) {
    for (int i=0; i<2;i++) if (tbs[i]) tbs[i]->SetLR(group, kOn, kOn); // it is more secure to set the on signal to everybody
  }
  else if (lr==1 && newstate==kOff) {
    for (int i=0; i<2;i++) if (tbs[i]) tbs[i]->SetLR(group, kOff, kOff);
  }
  else if (lr==1 && newstate==kOn) {
    for (int i=0; i<2;i++) if (tbs[i]) tbs[i]->SetLR(group, kOff, kOn); 
  }
  else return; // right now I see no reason to arrive here, but who knows...
  
  // now we need to update the status information.
  sleep(1); // give some time to the voltages to adapt
  GetTbsStatus(Tbs);
}



void SlowGui::CalibrateADCCurrent(Int_t Tbs) {
  memset(CorrADC[Tbs],0,sizeof(CorrADC[Tbs]));
  GetTbsADCBiasCurrents(Tbs,0);
  TBS *aTbs=MyTbs[Tbs][ActelTbs[Tbs]];

  for (Int_t i=0; i<NTdrTBS; i++) CorrADC[Tbs][i]=ADCtoCurrent(aTbs->BiasCurrent((sTBSTDR)i),Tbs,i,0);

  printf("TBS %d:\n", Tbs);
  for (Int_t i=0; i<NTdrTBS; i++) printf("%f ,",CorrADC[Tbs][i]);
  printf("\n");

  // now we save the values into a file

  char fname[100];
  sprintf(fname,"slowguidata/tcrate%d_tbs%d.txt",TCrate, Tbs);
  FILE *calibf=fopen(fname,"wt");
  for (Int_t i=0; i<NTdrTBS; i++) fprintf(calibf,"%f ",CorrADC[Tbs][i]);
  fprintf(calibf,"\n");
  fclose(calibf);
}


void SlowGui::ReadADCCurrentCalib(Int_t Tbs) {

  memset(CorrADC[Tbs],0,sizeof(CorrADC[Tbs]));

  char fname[100];
  sprintf(fname,"slowguidata/tcrate%d_tbs%d.txt",TCrate, Tbs);
  FILE *calibf=fopen(fname,"rt");
  if (calibf==0) { 
    printf("\033[1;31mcannot open TBS calibration file %s\033[0m\n", fname);
    return;
  }

  for (Int_t i=0; i<NTdrTBS; i++) fscanf(calibf,"%f ",&CorrADC[Tbs][i]);
  fclose(calibf);

}


void SlowGui::InitCorrCurrent() {

  const Float_t corr[2][12]=
    {
      {-0.385500 ,-0.606000 ,-1.855500 ,-0.091500 ,0.276000 ,0.570000 ,-2.149500 ,-1.414500 ,-0.826500 ,-0.606000 ,1.011000 ,1.084500 },
      {0.423000 ,0.349500 ,2.040000 ,1.084500 ,1.378500 ,0.423000 ,1.746000 ,0.570000 ,1.452000 ,1.525500 ,1.378500 ,3.142500}
    };

  for (int Tbs=0; Tbs<NTBS; Tbs++) for (int i=0; i<NTdrTBS; i++) CorrADC[Tbs][i]=corr[Tbs][i];


  ReadADCCurrentCalib(0);
  ReadADCCurrentCalib(1);
}
