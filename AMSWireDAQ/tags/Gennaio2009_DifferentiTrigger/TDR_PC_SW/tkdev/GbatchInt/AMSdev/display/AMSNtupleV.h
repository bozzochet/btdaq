//  $Id: AMSNtupleV.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMSNtupleV__
#define __AMSNtupleV__
#include <TChain.h>
#include <TNamed.h>
#include <TH2.h>
#include <TH1.h>
#include <TFile.h>
#include <TPad.h>
#include <TView.h>
#include <TSystem.h>
#include <time.h>
#include <TMarker3DBox.h>
#include <TPolyLine3D.h>
#include <THelix.h>
#include "../online/AMSNtuple.h"
#include "AMSDisplay.h"
#include "TMarker3DCl.h"
class AMSDrawI{
public:
int fRef;   ///<  Reference to corr in element in stlvector
AMSEventR * fEv;

AMSDrawI(AMSEventR *ev=0,int ref=0):fRef(ref),fEv(ev){}
};

class AMS3DMarker: public TMarker3DBox{
public:
AMS3DMarker():TMarker3DBox(){};
Int_t DistancetoPrimitive(Int_t px, Int_t py){
   
   const Int_t numPoints = 8;
   Int_t dist = 9999;
   Double_t points[3*numPoints];
//   Float_t  points[3*numPoints];

   TView *view = gPad->GetView();
   if (!view) return dist;
   SetPoints(points);
   double  sum[3]={0,0,0};
   Double_t xndc[3];
//   Float_t xndc[3];
   for (int i = 0; i < 8; i++) {
      view->WCtoNDC(points+i*3, xndc);
      for(int k=0;k<3;k++)sum[k]+=xndc[k];
   }
   for(int k=0;k<3;k++)sum[k]/=8;
   double x=gPad->XtoAbsPixel(sum[0]);        
   double y=gPad->YtoAbsPixel(sum[1]);        
   dist=sqrt((x-px)*(x-px)+(y-py)*(y-py));
   if (dist < 5) {
      gPad->SetCursor(kCross);
   }
   return dist;
}
};

class  HeaderV: public   TPaveLabel, public AMSDrawI{
public:
HeaderV():AMSDrawI(NULL,-1),TPaveLabel(0.85+0.1*0.15,0.34*0.15,0.85+0.9*0.15,0.65*0.15,"Header","TR"){};
HeaderV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPaveLabel(0.8,-0.79,0.98,-0.71,"Header","TR"){};
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->fHeader.Info():0;}


};

class Trigger1V: public   TPaveLabel, public AMSDrawI{
public:
Trigger1V():AMSDrawI(NULL,-1),TPaveLabel(0.85+0.1*0.15,0.34*0.15,0.85+0.9*0.15,0.65*0.15,"Level1","TR"){};
Trigger1V(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPaveLabel(0.8,-0.89,0.98,-0.81,"Level1","TR"){};
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pLevel1(fRef)->Info(fRef):0;}


};

class Trigger3V: public   TPaveLabel, public AMSDrawI{
public:
Trigger3V():AMSDrawI(NULL,-1),TPaveLabel(0.85+0.1*0.15,0.34*0.15,0.85+0.9*0.15,0.65*0.15,"Level1","TR"){};
Trigger3V(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPaveLabel(0.8,-0.99,0.98,-0.91,"Level3","TR"){};
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pLevel3(fRef)->Info(fRef):0;}


};

class TofClusterV: public TMarker3DCl, public AMSDrawI{
protected:
public:
TofClusterV():AMSDrawI(NULL,-1),TMarker3DCl(){};
TofClusterV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
 TofClusterR *pcl=ev->pTofCluster(ref);
if(pcl){
  SetSize(pcl->ErrorCoo[0],pcl->ErrorCoo[1],sqrt(pcl->Edep/2)<8?sqrt(pcl->Edep/2):8);
  SetPosition(pcl->Coo[0],pcl->Coo[1],pcl->Coo[2]);
  SetDirection(0,0);
}
   SetLineWidth(3);
   SetLineColor(3);             // light green
   SetFillColor(3);
   SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)


}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pTofCluster(fRef)->Info(fRef):0;}


};
class TrRecHitV: public TMarker3DCl, public AMSDrawI{
protected:
public:
TrRecHitV():AMSDrawI(NULL,-1),TMarker3DCl(){};
TrRecHitV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
 TrRecHitR *pcl=ev->pTrRecHit(ref);
int size=gAMSDisplay->Focus()==0?2:1;
if(pcl){
  SetSize(pcl->EHit[0]<0.5?pcl->EHit[0]*100:pcl->EHit[0],pcl->EHit[1]*100,sqrt(pcl->Sum/10)<6?sqrt(pcl->Sum/10
):8);
  SetPosition(pcl->Hit[0],pcl->Hit[1],pcl->Hit[2]+fDz);
  SetDirection(0,0);
  if(gAMSDisplay->ShowTrClProfile()){
   float x[20];
  if(pcl->pTrCluster('y')){
    for (int k=0;k<pcl->pTrCluster('y')->Amplitude.size();k++)x[k]=pcl->pTrCluster('y')->Amplitude[k]/(pcl->pTrCluster('y')->Sum+1.e-20);
    SetProfileY(pcl->pTrCluster('y')->Amplitude.size(),x);
    SetShowProfileY(true);
  }
  if(pcl->pTrCluster('x')){
    for (int k=0;k<pcl->pTrCluster('x')->Amplitude.size();k++)x[k]=pcl->pTrCluster('x')->Amplitude[k]/(pcl->pTrCluster('x')->Sum+1.e-20);
    SetProfileX(pcl->pTrCluster('x')->Amplitude.size(),x);
    SetShowProfileX(true);
  }
  }
}
   
   SetLineWidth(size);
   SetLineColor(4);             // blue
   SetFillColor(4);
   SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)
   SetFillStyle(1001);          // solid filling (not working now....)

}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pTrRecHit(fRef)->Info(fRef):0;}

};


class TrdClusterV: public TMarker3DCl, public AMSDrawI{
protected:
public:
TrdClusterV():AMSDrawI(NULL,-1),TMarker3DCl(){};
TrdClusterV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
 TrdClusterR *pcl=ev->pTrdCluster(ref);
if(pcl){
  SetPosition(pcl->Coo[0],pcl->Coo[1],pcl->Coo[2]);

  float ercoo[3];
  ercoo[pcl->Direction]=sqrt(pcl->EDep/2);
  ercoo[1-pcl->Direction]=2*pcl->ClSizeR*pcl->Multip;
  ercoo[2]=2*pcl->ClSizeR;
  SetSize(ercoo[0],ercoo[1],ercoo[2]);
 

  SetDirection(0,0);
}
   SetLineWidth(3);
   SetLineColor(46);             // dark red
   SetFillColor(46);
   SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)

}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pTrdCluster(fRef)->Info(fRef):0;}

};



class AntiClusterV: public TMarker3DCl, public AMSDrawI{
protected:
public:
AntiClusterV():AMSDrawI(NULL,-1),TMarker3DCl(){};
AntiClusterV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
 AntiClusterR *pcl=ev->pAntiCluster(ref);
if(pcl){
  double dr=180./3.1415926;
  double x=pcl->Coo[0]*cos(pcl->Coo[1]/dr);
  double y=pcl->Coo[0]*sin(pcl->Coo[1]/dr);
  SetPosition(x,y,pcl->Coo[2]);
  SetSize(fabs(y)*pcl->ErrorCoo[1]/dr,fabs(x)*pcl->ErrorCoo[1]/dr,pcl->ErrorCoo[2]);
  
  SetDirection(0,0);
}
   SetLineWidth(3);
   SetLineColor(30);             // purple
   SetFillColor(30);
   SetFillStyle(0);          // solid filling (not working now....)

}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pAntiCluster(fRef)->Info(fRef):0;}

};

class RichHitV: public TMarker3DCl, public AMSDrawI{
protected:
public:
RichHitV():AMSDrawI(NULL,-1),TMarker3DCl(){};
RichHitV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
 RichHitR *pcl=ev->pRichHit(ref);
 int size=gAMSDisplay->Focus()==0?2:1;
if(pcl){
  float error[3];
  error[0]=error[1]=0.85/2*size;  //tmp
  error[2]=sqrt(pcl->Npe);
  SetPosition(pcl->Coo[0],pcl->Coo[1],pcl->Coo[2]+error[2]);
  SetSize(error[0],error[1],error[2]);
//  cout <<"  rich hit "<<pcl->Coo[0]<<" "<<pcl->Coo[1]<<" "<<pcl->Coo[2]<<endl;
//  cout <<error[0]<<" "<<error[1]<<" "<<error[2]<<" "<<endl;
  SetDirection(0,0);
}
   SetLineWidth(size);
   SetLineColor(9);             // purple
   SetFillColor(9);
   SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)

}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pRichHit(fRef)->Info(fRef):0;}

};

class EcalClusterV: public TMarker3DCl, public AMSDrawI{
protected:
public:
EcalClusterV():AMSDrawI(NULL,-1),TMarker3DCl(){};
EcalClusterV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
 EcalClusterR *pcl=ev->pEcalCluster(ref);
if(pcl){
  SetPosition(pcl->Coo[0],pcl->Coo[1],pcl->Coo[2]);
  float error[3];
  const float cellsize=0.9;
  error[pcl->Proj]=cellsize*pcl->NEcalHit();  //tmp
  error[2]=cellsize/2;         //tmp;
  error[1-pcl->Proj]=log(1.+pcl->Edep<0?0:pcl->Edep);
  if(error[1-pcl->Proj]<cellsize)error[1-pcl->Proj]=cellsize;
//  cout <<pcl->Plane<<" "<<pcl->Coo[0]<<" "<<pcl->Coo[1]<<" "<<pcl->Coo[2]<<endl;
//  cout <<pcl->Plane<<" "<<error[0]<<" "<<error[1]<<" "<<error[2]<<" "<<endl;
  SetSize(error[0],error[1],error[2]);
  SetDirection(0,0);
}
   SetLineWidth(gAMSDisplay->Focus()==0?1:2);
   SetLineColor(7);             // purple
   SetFillColor(7);
   SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)

}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pEcalCluster(fRef)->Info(fRef):0;}

};



class TrMCClusterV: public TMarker3DCl, public AMSDrawI{
protected:
public:
TrMCClusterV():AMSDrawI(NULL,-1),TMarker3DCl(){};
TrMCClusterV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
 TrMCClusterR *pcl=ev->pTrMCCluster(ref);
if(pcl){
  SetPosition(pcl->Xgl[0],pcl->Xgl[1],pcl->Xgl[2]);
  SetSize(0.2,0.2,sqrt(pcl->Sum*1000)*6);
  SetDirection(0,0);
}
   SetLineWidth(1);
   SetLineColor(5);   // yellow
   SetFillColor(5);
   SetFillStyle(0);          // solid filling (not working now....)

}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pTrMCCluster(fRef)->Info(fRef):0;}

};



class ParticleV: public TPolyLine3D, public AMSDrawI{
protected:
public:
ParticleV():AMSDrawI(NULL,-1),TPolyLine3D(){};
ParticleV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
 ParticleR *pcl=ev->pParticle(ref);
if(pcl){
 const int npoint=2+2+8+2+2+3;
 float array[3*npoint];
  int old=0;
  if(pcl->TRDCoo[1][2]>90 && pcl->TRDCoo[1][2]<150)old=1; 
  for(int k=0;k<3;k++)array[k]=pcl->TRDCoo[old][k];
  for(int k=0;k<3;k++)array[3+k]=pcl->TRDCoo[0][k];
  for(int k=0;k<3;k++)array[3+3+k]=pcl->TOFCoo[0][k];
  for(int k=0;k<3;k++)array[3+3*2+k]=pcl->TOFCoo[1][k];
  for(int i=0;i<8;i++){
   for(int k=0;k<3;k++)array[3+3*3+3*i+k]=pcl->TrCoo[i][k];
  } 
  for(int k=0;k<3;k++)array[3+3*11+k]=pcl->TOFCoo[2][k];
  for(int k=0;k<3;k++)array[3+3*12+k]=pcl->TOFCoo[3][k];
  for(int k=0;k<3;k++)array[3+3*13+k]=pcl->RichCoo[0][k];
  for(int k=0;k<3;k++)array[3+3*14+k]=pcl->RichCoo[1][k];
  for(int i=0;i<3;i++){
   for(int k=0;k<3;k++)array[3+3*15+3*i+k]=pcl->EcalCoo[i][k];
  } 
   SetPolyLine(npoint,array);
   SetLineColor(2);
   SetLineWidth(1);
   SetLineStyle(1);
   if(pcl->Charge==0)SetLineStyle(2);
}
}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pParticle(fRef)->Info(fRef):0;}


};

class TrdTrackV: public TPolyLine3D, public AMSDrawI{
protected:
public:
TrdTrackV():AMSDrawI(NULL,-1),TPolyLine3D(){};
TrdTrackV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
 TrdTrackR *pcl=ev->pTrdTrack(ref);
if(pcl){
 const int npoint=2;
 float array[3*npoint];
 double u[3];
  u[0]=sin(pcl->Theta)*cos(pcl->Phi);
  u[1]=sin(pcl->Theta)*sin(pcl->Phi);
  u[2]=cos(pcl->Theta);
  float z1=85;   //tmp
  float x1=pcl->Coo[0]+u[0]/u[2]*(z1-pcl->Coo[2]);
  float y1=pcl->Coo[1]+u[1]/u[2]*(z1-pcl->Coo[2]);
 float z2=150; 
  float x2=pcl->Coo[0]+u[0]/u[2]*(z2-pcl->Coo[2]);
  float y2=pcl->Coo[1]+u[1]/u[2]*(z2-pcl->Coo[2]);
  array[0]=x1;
  array[1]=y1;
  array[2]=z1;
  array[3]=x2;
  array[4]=y2;
  array[5]=z2;
   SetPolyLine(npoint,array);
}
   SetLineColor(28);
   SetLineWidth(1);
   SetLineStyle(1);

}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pTrdTrack(fRef)->Info(fRef):0;}


};


class MCEventgV: public TPolyLine3D, public AMSDrawI{
protected:
public:
MCEventgV():AMSDrawI(NULL,-1),TPolyLine3D(){};
MCEventgV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
 MCEventgR *pcl=ev->pMCEventg(ref);
if(pcl){
 const int npoint=2;
 float array[3*npoint];
  array[0]=pcl->Coo[0];
  array[1]=pcl->Coo[1];
  array[2]=pcl->Coo[2];
  MCEventgR *pcln=ev->pMCEventg(ref+1);
  if(pcln && pcl->Particle>0)array[5]=pcln->Coo[2];
  else array[5]=pcl->Coo[2]+10;
  array[3]=pcl->Coo[0]+pcl->Dir[0]/pcl->Dir[2]*(array[5]-pcl->Coo[2]);
  array[4]=pcl->Coo[1]+pcl->Dir[1]/pcl->Dir[2]*(array[5]-pcl->Coo[2]);
  SetPolyLine(npoint,array);

   SetLineColor(5);
   SetLineWidth(1);
   SetLineStyle(1);
   if(pcl->Charge==0)SetLineStyle(2);
   else SetLineWidth(fabs(pcl->Charge));
}
}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pMCEventg(fRef)->Info(fRef):0;}


};




class TrTrackV: public THelix, public AMSDrawI{
protected:
public:
TrTrackV():AMSDrawI(NULL,-1),THelix(){};
TrTrackV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),THelix(){

 TrTrackR *pcl=ev->pTrTrack(ref);
if(pcl){
   Double_t Bfield = -0.8;	// in minus-x direction of AMS
   Double_t P0[3];
   Double_t V0[3];
   Double_t Axis[3]={-1,0,0};
   Double_t Range[2]={-60,60};
     for(int i=0;i<3;i++)P0[i]=pcl->P0[i];
     
     V0[0]=pcl->Rigidity * sin(pcl->Theta) * cos(pcl->Phi);
     V0[1]=pcl->Rigidity * sin(pcl->Theta) * sin(pcl->Phi);
     V0[2]=pcl->Rigidity * cos(pcl->Theta);
     THelix::SetHelix(P0,V0,0.3*Bfield/100,Range,kHelixX,Axis);
}
   SetLineColor(14);
   SetLineWidth(1);
   SetLineStyle(1);

}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pTrTrack(fRef)->Info(fRef):0;}


};




class EcalShowerV: public TPolyLine3D, public AMSDrawI{
protected:
public:
EcalShowerV():AMSDrawI(NULL,-1),TPolyLine3D(){};
EcalShowerV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
 EcalShowerR *pcl=ev->pEcalShower(ref);
if(pcl){
 const int npoint=2;
 float array[3*npoint];
  array[0]=pcl->Entry[0];
  array[1]=pcl->Entry[1];
  array[2]=pcl->Entry[2];
  array[3]=pcl->Exit[0];
  array[4]=pcl->Exit[1];
  array[5]=pcl->Exit[2];
   SetPolyLine(npoint,array);
   int lw=sqrt(pcl->EnergyC);
   if(lw<1)lw=1;
   SetLineWidth(lw);
}
   SetLineColor(12);
   SetLineStyle(1);

}
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pEcalShower(fRef)->Info(fRef):0;}


};




class RichRingV: public TPolyLine3D, public AMSDrawI{
protected:
public:
RichRingV():AMSDrawI(NULL,-1),TPolyLine3D(){};
RichRingV(AMSEventR *ev,int ref, bool drawplex=false);
char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pRichRing(fRef)->Info(fRef):0;}


};





class AMSNtupleV : public AMSNtupleR{
protected:

// Drawable things 
vector<AntiClusterV> fAntiClusterV;
vector<TofClusterV> fTofClusterV;
vector<TrdClusterV> fTrdClusterV;
vector<RichHitV> fRichHitV;
vector<TrMCClusterV> fTrMCClusterV;
vector<EcalClusterV> fEcalClusterV;
vector<TrRecHitV> fTrRecHitV;
vector<TrTrackV> fTrTrackV;
vector<TrdTrackV> fTrdTrackV;
vector<EcalShowerV> fEcalShowerV;
vector<RichRingV> fRichRingV;
vector<ParticleV> fParticleV;
vector<MCEventgV> fMCEventgV;
vector<Trigger1V> fTrigger1V;
vector<Trigger3V> fTrigger3V;
vector<HeaderV> fHeaderV;


public:
AMSNtupleV(TChain *chain):AMSNtupleR(chain){};
void Draw(EAMSType type=kall);
void Prepare(EAMSType type=kall);
bool GetEvent(unsigned int run, unsigned int event);
char * GetObjInfo(int px,int py);
ClassDef(AMSNtupleV,1)           // Ntuple Drawable
};
#endif
