#include "slowgui2.h"
#include "amswcomtest.h"
#include "SlowControl.h"



void SlowGui::CreateTbs() {

  for (sTBSaddr i=kTBS0; i<NTBS; i++) {
    /*
    ActelTbs[i]=kActelHot; // preselection of Actels

    ActelTbsState[i][kActelHot]=kActelOn;
    ActelTbsState[i][kActelCold]=kActelOff;
    */
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



void SlowGui::GetTbsADCBiasVoltage(Int_t Tbs, Int_t update) {

  //printf("getting voltage adc for tbs %d, group %d\n",Tbs,group);

  int pos=((Tbs)?18:1);
  
  TbsVolt[Tbs][0][0]=Node->Event[pos++]; // Hot
  TbsVolt[Tbs][0][1]=Node->Event[pos++]; // Cold
  TbsVolt[Tbs][1][0]=Node->Event[pos++]; // Hot
  TbsVolt[Tbs][1][1]=Node->Event[pos++]; // Cold

  for(int i=0; i<NTdrTBS; i++) TbsCurrent[Tbs][i] = Node->Event[pos++];
  //  for(int i=0; i<NTdrTBS; i++) printf("tbs %d tdr %d: 0x%04x\n",Tbs,i,TbsCurrent[Tbs][i]);

  //  printf("TbsVolt[%d][0][0]=0x%04x\n",Tbs,TbsVolt[Tbs][0][0]);
  //  printf("TbsVolt[%d][0][1]=0x%04x\n",Tbs,TbsVolt[Tbs][0][1]);
  //  printf("TbsVolt[%d][1][0]=0x%04x\n",Tbs,TbsVolt[Tbs][1][0]);
  //  printf("TbsVolt[%d][1][1]=0x%04x\n",Tbs,TbsVolt[Tbs][1][1]);

  if (update) GetTbsStatus(Tbs);
}



int SlowGui::GetTbsStatus(Int_t Tbs) {
  Node->SDprocRead(AMSWaddr,0x1001);
  if (TestAMSWError(Node)) return 1;
  Node->SDprocRead(AMSWaddr,0x1011); 
  if (TestAMSWError(Node)) return 1;

  // we rely on various parameters...
  // first, the Actel
  //ActelTbsState[Tbs][ActelTbs[Tbs]]=kActelOn; // if we are here, it normally means that the actel replied to the commands...

  // tbs_brother->GetActel();
  //ActelTbsState[Tbs][1-ActelTbs[Tbs]]=0?kActelOff:kActelOn;

  ushort group=(Tbs+7)<<12;
  ushort error=FindParameter(group+0x200);
  ushort trips=(error>>13) & 1;
  ushort poweron=(error>>15) & 1;
  ushort LR0Hoff=(error>>8) & 1;
  ushort LR1Hoff=(error>>9) & 1;
  ActelMask=FindParameter(0x9000);

  ShowBrotherTrip(Tbs+7, trips);
  ShowPowerOn(Tbs+7, poweron);

  // LR hot
  TBSlrState[Tbs][0]=(((1-LR0Hoff)*FindParameter(group+0x100))?kOn:kOff);
  TBSlrState[Tbs][1]=(((1-LR1Hoff)*FindParameter(group+0x102))?kOn:kOff);
  // LR cold
  TBSlrState[Tbs][2]=((LR0Hoff*FindParameter(group+0x101))?kOn:kOff);
  TBSlrState[Tbs][3]=((LR1Hoff*FindParameter(group+0x103))?kOn:kOff);

  for(int i=0,cnt=0; i<2; i++) {
    Voltages[Tbs][cnt++]=(FindParameter(group+0x104+i)?k80V:k60V);
    Voltages[Tbs][cnt++]=(FindParameter(group+0x104+2+i)?k80V:k60V);
  }


 
 
  // GetTbsADCBiasCurrents(Tbs,0);
  
  // ok, now everything is ready, we display the results

  Node->SDprocRead(AMSWaddr,0x1012); // getting raw values, no other way for now to get adc values
  if (TestAMSWError(Node)) return 1;

  GetTbsADCBiasVoltage(Tbs,0);
 
  DisplayTbsStatus(Tbs);

  return 0;
}



void SlowGui::ControlTbsVoltage(Int_t Tbs, Int_t group, Int_t voltage) {
  int newvolt;
  if (voltage==80) newvolt=1;
  else if (voltage==60) newvolt=0;
  else return;

  printf("TBS %d: setting  group %d at %d V\n",Tbs, group, voltage);

  ushort param=((Tbs+7)<<12)+0x104+2*group;
  Node->SetParameter(AMSWaddr,0x1000,param,newvolt);
  Node->SetParameter(AMSWaddr,0x1000,param+1,newvolt);
  Node->SDProc(AMSWaddr,0x1003);
  
  usleep(300000); // give some time to the voltages to adapt
  GetTbsStatus(Tbs);

}



void SlowGui::ControlTbsLr(Int_t Tbs, Int_t group, Int_t lr) {

  printf("TBS: %d: switching LR %d of group %d\n",Tbs,lr,group);

  // so first, we need to know in which state the system is supposed to be
  sDCDC state=TBSlrState[Tbs][group+lr*2];
  sDCDC newstate=kNoDCDCChange0;
  if (state==kOn) newstate=kOff;
  else if (state==kOff) newstate=kOn;
  else return;

  ushort param=((Tbs+7)<<12)+0x100+2*group;

  printf("lr=%d, newstate=%s\n",lr,(newstate==kOn)?"kOn":(newstate==kOff)?"kOff":"xxx");
  
  // the point is that things are not as simple as it looks to be...
  // if I want to switch off hot, I have to send off to both hot and cold,
  // else cold starts up.
  // If I want to switch on cold, it means switching off hot.
  // and hot + cold cannot be on at the same time.
  // also, if both Actels are on, we have to make sure that the OR combination 
  // will be the expected behaviour. Thus the easiest it to send the same command to both Actels.
  if (lr==0 && newstate==kOff) {
    Node->SetParameter(AMSWaddr,0x1000,param,0);
    Node->SetParameter(AMSWaddr,0x1000,param+1,0);
  }
  else if (lr==0 && newstate==kOn) {
    Node->SetParameter(AMSWaddr,0x1000,param,1);
    Node->SetParameter(AMSWaddr,0x1000,param+1,1);   
  }
  else if (lr==1 && newstate==kOff) {
    Node->SetParameter(AMSWaddr,0x1000,param,0);
    Node->SetParameter(AMSWaddr,0x1000,param+1,0);
  }
  else if (lr==1 && newstate==kOn) {
    Node->SetParameter(AMSWaddr,0x1000,param,0);
    Node->SetParameter(AMSWaddr,0x1000,param+1,1);
  }
  else return; // right now I see no reason to arrive here, but who knows...

  Node->SDProc(AMSWaddr,0x1003);  
  sleep(1); // give some time to the voltages to adapt
  GetTbsStatus(Tbs);
}



void SlowGui::CalibrateADCCurrent(Int_t Tbs) {
  memset(CorrADC[Tbs],0,sizeof(CorrADC[Tbs]));
  //GetTbsADCBiasCurrents(Tbs,0);

  Node->SDprocRead(AMSWaddr,0x1002); // getting raw values, no other way for now to get adc values
  GetTbsADCBiasVoltage(Tbs,0);

  //TBS *aTbs=MyTbs[Tbs][ActelTbs[Tbs]];

  for (Int_t i=0; i<NTdrTBS; i++) CorrADC[Tbs][i]=ADCtoCurrent(TbsCurrent[Tbs][i],Tbs,i,0);

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
    printf("\033[1;31mcannot open file %s\033[0m\n", fname);
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
