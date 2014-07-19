//  $Id: AMSNtupleV.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#include "AMSNtupleV.h"
#include "TCONE.h"
#include "TNode.h"
char * AMSNtupleV::GetObjInfo(int px, int py){
static char* info=0;
int dist=99999;
info=0;



{
 int cand=-1;
 for(int i=0;i<fTrigger1V.size();i++){
   int current=fTrigger1V[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
  if(dist<7 && cand>=0)info=fTrigger1V[cand].GetObjectInfo(px,py);
}

{
 int cand=-1;
 for(int i=0;i<fTrigger3V.size();i++){
   int current=fTrigger3V[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
  if(dist<7 && cand>=0)info=fTrigger3V[cand].GetObjectInfo(px,py);
}
{
 int cand=-1;
 for(int i=0;i<fHeaderV.size();i++){
   int current=fHeaderV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
  if(dist<7 && cand>=0)info=fHeaderV[cand].GetObjectInfo(px,py);
}

  if(info)return info; 



{
 int cand=-1;
 for(int i=0;i<fTofClusterV.size();i++){
   int current=fTofClusterV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
  if(dist<7 && cand>=0)info=fTofClusterV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fTrRecHitV.size();i++){
   int current=fTrRecHitV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrRecHitV[cand].GetObjectInfo(px,py);
}

{
 int cand=-1;
 for(int i=0;i<fAntiClusterV.size();i++){
   int current=fAntiClusterV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fAntiClusterV[cand].GetObjectInfo(px,py);
}



{
 int cand=-1;
 for(int i=0;i<fTrMCClusterV.size();i++){
   int current=fTrMCClusterV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrMCClusterV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fTrdClusterV.size();i++){
   int current=fTrdClusterV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrdClusterV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fEcalShowerV.size();i++){
   int current=fEcalShowerV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fEcalShowerV[cand].GetObjectInfo(px,py);
}



{
 int cand=-1;
 for(int i=0;i<fRichHitV.size();i++){
   int current=fRichHitV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fRichHitV[cand].GetObjectInfo(px,py);
}




if(info)return info;
else dist=1000000;

{
 int cand=-1;
 for(int i=0;i<fEcalClusterV.size();i++){
   int current=fEcalClusterV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
   //cout <<"  cluster "<<i<<dist<<" "<<current<<endl;
  }
 }
 if(dist<7 && cand>=0)info=fEcalClusterV[cand].GetObjectInfo(px,py);
}





{
 int cand=-1;
 for(int i=0;i<fTrTrackV.size();i++){
   int current=fTrTrackV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrTrackV[cand].GetObjectInfo(px,py);
}



{
 int cand=-1;
 for(int i=0;i<fTrdTrackV.size();i++){
   int current=fTrdTrackV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrdTrackV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fRichRingV.size();i++){
   int current=fRichRingV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fRichRingV[cand].GetObjectInfo(px,py);
}
{
 int cand=-1;
 for(int i=0;i<fMCEventgV.size();i++){
   int current=fMCEventgV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fMCEventgV[cand].GetObjectInfo(px,py);
}

{
 int cand=-1;
 for(int i=0;i<fParticleV.size();i++){
   int current=fParticleV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fParticleV[cand].GetObjectInfo(px,py);
}




return info;



}


void AMSNtupleV::Prepare( EAMSType type){

if(type==kall || type==kusedonly || type==kanticlusters){
 fAntiClusterV.clear();
 if(gAMSDisplay->DrawObject(kanticlusters)){
  for(int i=0;i<NAntiCluster();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pAntiCluster(i)->Status)/32)%2)fAntiClusterV.push_back( AntiClusterV(this,i));
  }
 }
}

if(type==kall){
  fTrigger1V.clear();
  for(int i=0;i<NLevel1();i++){
   fTrigger1V.push_back( Trigger1V(this,i));
  }
  fTrigger3V.clear();
  for(int i=0;i<NLevel3();i++){
   fTrigger3V.push_back( Trigger3V(this,i));
  }
  fHeaderV.clear();
  for(int i=0;i<1;i++){
   fHeaderV.push_back( HeaderV(this,i));
  }

}

if(type==kall || type==kusedonly || type==ktofclusters){
 fTofClusterV.clear();
 if(gAMSDisplay->DrawObject(ktofclusters)){
  for(int i=0;i<NTofCluster();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTofCluster(i)->Status)/32)%2)fTofClusterV.push_back( TofClusterV(this,i));
  }
 }
}


if(type==kall || type==kusedonly || type==ktrclusters){
 fTrRecHitV.clear();
 if(gAMSDisplay->DrawObject(ktrclusters)){
  for(int i=0;i<NTrRecHit();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTrRecHit(i)->Status)/32)%2)fTrRecHitV.push_back( TrRecHitV(this,i));
  }
 }
}



if(type==kall || type==kusedonly || type==krichhits){
 fRichHitV.clear();
 if(gAMSDisplay->DrawObject(krichhits)){
  for(int i=0;i<NRichHit();i++){
   if(!gAMSDisplay->DrawUsedOnly() || (pRichHit(i)->Status)%1024)fRichHitV.push_back( RichHitV(this,i));
  }
 }
}


if(type==kall || type==kusedonly || type==ktrdclusters){
 fTrdClusterV.clear();
 if(gAMSDisplay->DrawObject(ktrdclusters)){
  for(int i=0;i<NTrdCluster();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTrdCluster(i)->Status)/32)%2)fTrdClusterV.push_back( TrdClusterV(this,i));
  }
 }
}

if(type==kall || type==kusedonly || type==kecalclusters){
 fEcalClusterV.clear();
 if(gAMSDisplay->DrawObject(kecalclusters)){
  for(int i=0;i<NEcalCluster();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pEcalCluster(i)->Status)/32)%2){
     if(pEcalCluster(i)->Edep>0)fEcalClusterV.push_back( EcalClusterV(this,i));
   }
  }
 }
}


if(type==kall || type==kusedonly || type==ktrtracks){
 fTrTrackV.clear();
 if(gAMSDisplay->DrawObject(ktrtracks)){
  for(int i=0;i<NTrTrack();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTrTrack(i)->Status)/32)%2){
     if(pTrTrack(i)->IsGood()){
       fTrTrackV.push_back( TrTrackV(this,i));
     }
   }
  }
  for(int i=0;i<NTrTrack();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTrTrack(i)->Status)/32)%2){
     if(!pTrTrack(i)->IsGood())fTrTrackV.push_back( TrTrackV(this,i));
   }
  }

 }
}


if(type==kall || type==kusedonly || type==ktrdtracks){
 fTrdTrackV.clear();
 if(gAMSDisplay->DrawObject(ktrdtracks)){
  for(int i=0;i<NTrdTrack();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTrdTrack(i)->Status)/32)%2)fTrdTrackV.push_back( TrdTrackV(this,i));
  }
 }
}

if(type==kall || type==kusedonly || type==kecalshowers){
 fEcalShowerV.clear();
 if(gAMSDisplay->DrawObject(kecalshowers)){
  for(int i=0;i<NEcalShower();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pEcalShower(i)->Status)/32)%2)fEcalShowerV.push_back( EcalShowerV(this,i));
  }
 }
}

if(type==kall || type==kusedonly || type==krichrings){
 fRichRingV.clear();
 if(gAMSDisplay->DrawObject(krichrings)){
  for(int i=0;i<NRichRing();i++){
   if( (!gAMSDisplay->DrawUsedOnly() || ((pRichRing(i)->Status)/32)%2)){
    fRichRingV.push_back( RichRingV(this,i,false));
    if(gAMSDisplay->DrawRichRingsFromPlex())fRichRingV.push_back( RichRingV(this,i,true));
   }
  }
 }
}

if(type==kall ||  type==kmcinfo){
 fTrMCClusterV.clear();
 if(gAMSDisplay->DrawObject(kmcinfo)){
  for(int i=0;i<NTrMCCluster();i++){
    fTrMCClusterV.push_back( TrMCClusterV(this,i));
  }
 }
}

if(type==kall ||  type==kmcinfo){
 fMCEventgV.clear();
 if(gAMSDisplay->DrawObject(kmcinfo)){
  for(int i=0;i<NMCEventg();i++){
    fMCEventgV.push_back( MCEventgV(this,i));
  }
 }
}



if(type==kall || type==kparticles){
 fParticleV.clear();
 if(gAMSDisplay->DrawObject(kparticles)){
  for(int i=0;i<NParticle();i++){
   fParticleV.push_back( ParticleV(this,i));
  }
 }
}


}


void AMSNtupleV::Draw( EAMSType type){

 for(int i=0;i<fTrigger1V.size();i++){
   fTrigger1V[i].AppendPad();
  }
 for(int i=0;i<fTrigger3V.size();i++){
   fTrigger3V[i].AppendPad();
  }

 for(int i=0;i<fHeaderV.size();i++){
   fHeaderV[i].AppendPad();
  }




 for(int i=0;i<fTrTrackV.size();i++){
   fTrTrackV[i].AppendPad();
  }

 for(int i=0;i<fTrdTrackV.size();i++){
   fTrdTrackV[i].AppendPad();
  }

 for(int i=0;i<fParticleV.size();i++){
  fParticleV[i].AppendPad();
 }

 for(int i=0;i<fEcalClusterV.size();i++){
   fEcalClusterV[i].AppendPad();
  }

 for(int i=0;i<fRichRingV.size();i++){
   fRichRingV[i].AppendPad();
  }

 for(int i=0;i<fEcalShowerV.size();i++){
   fEcalShowerV[i].AppendPad();
  }

 for(int i=0;i<fTofClusterV.size();i++){
   fTofClusterV[i].AppendPad();
  }

 for(int i=0;i<fAntiClusterV.size();i++){
   fAntiClusterV[i].AppendPad();
  }

 for(int i=0;i<fTrdClusterV.size();i++){
   fTrdClusterV[i].AppendPad();
  }


 for(int i=0;i<fRichHitV.size();i++){
   fRichHitV[i].AppendPad();
  }


 for(int i=0;i<fTrRecHitV.size();i++){
   fTrRecHitV[i].AppendPad();
  }

 for(int i=0;i<fTrMCClusterV.size();i++){
   fTrMCClusterV[i].AppendPad();
  }

 for(int i=0;i<fMCEventgV.size();i++){
   fMCEventgV[i].AppendPad();
  }

}


bool AMSNtupleV::GetEvent(unsigned int run, unsigned int event){
int entry=0;
if(Run()==run && Event() <event)entry=fCurrentEntry;
while(ReadOneEvent(entry++)!=-1){
 if(Run() == run && Event()>=event)return true;
}
return false;
}


#include "TRotation.h"


RichRingV::RichRingV(AMSEventR *ev,int ref, bool drawplex):AMSDrawI(ev,ref),TPolyLine3D(){
 RichRingR *pcl=ev->pRichRing(ref);
//
// at the moment only rich rings ass with particles will be drawn
//
//  This should go to RichRingR  as soon as the latter will be updated
//  (added theta, radiator etc) by Carlos
//
static TNode *mirror=gAMSDisplay->GetGeometry()->GetNode("OMIR1");
static TNode *rich=gAMSDisplay->GetGeometry()->GetNode("RICH1");
           TCONE * pcone= (TCONE*)mirror->GetShape();
           double r1=pcone->GetRmin2();
           double r2=pcone->GetRmin();
           double dz=pcone->GetDz();      
           double xc=mirror->GetX();
           double yc=mirror->GetY();
           double zc=mirror->GetZ();
           double tc=(r1-r2)/2/dz;
           zc+=rich->GetZ();
           double  z2=zc-dz;
/*           cout <<" tc "<<tc<<" "<<z2<<endl;
             cout <<r1<<endl;           
             cout <<r2<<endl;           
             cout <<dz<<endl;           
             cout <<xc<<endl;           
             cout <<yc<<endl;           
             cout <<zc<<endl;           
             cout <<tc<<endl;           
*/
  const int npointm=360/5+1;
  int npoint=npointm;
  float array[3*npointm];
  double rad_thick;
  const double rad_length=-3;
  double n_aero;
  if(ev->Version()<130){
    n_aero=1.02998;
  }
  else n_aero=1.0529;
  const double n_naf=1.33;
  const double n_naf_Spread=0.01; 
   if(ev->Version() <89){
  double refi;
   double rad_posz;
for(int i=0;i<ev->nParticle();i++){
 if( ev->pParticle(i)->iRichRing() == ref){
  if(fabs(ev->pParticle(i)->RichCoo[0][0])<11.3*3/2 && fabs(ev->pParticle(i)->RichCoo[0][1])<11.3*3/2){
    refi=n_naf;
    rad_posz=-2.5;
    rad_thick=-0.5;
  }
  else {
   refi=n_aero;
   rad_posz=0;
   rad_thick=-3;
  }
  double    cc=1./ev->pRichRing(ref)->Beta/refi;
//   cout <<" refi "<<refi<<" "<<cc<<" "<<ev->pParticle(i)->RichCoo[0][0]<<" "<<ev->pParticle(i)->RichCoo[0][1]<<" "<<ev->pParticle(i)->RichCoo[0][2]<<" "<<ev->pParticle(i)->RichCoo[1][2]<<" "<<ev->pRichRing(ref)->Beta<<endl;
  if(cc<1){
   double theta=acos(cc);
    TVector3 z(ev->pParticle(i)->RichCoo[1][0]-ev->pParticle(i)->RichCoo[0][0],ev->pParticle(i)->RichCoo[1][1]-ev->pParticle(i)->RichCoo[0][1],ev->pParticle(i)->RichCoo[1][2]-ev->pParticle(i)->RichCoo[0][2]);
    double rcoo[3];
    TRotation r;
    r.SetZAxis(z);
   double dphi=2*3.1415926/(npoint-1);
   double phi=-dphi;
   for( int k=0;k<npoint;k++){
   double posz=rad_posz+rad_thick*float(rand())/RAND_MAX;
   double thick=rad_length-rad_posz;  
    rcoo[0]=ev->pParticle(i)->RichCoo[0][0]+z.X()/z.Z()*posz;
    rcoo[1]=ev->pParticle(i)->RichCoo[0][1]+z.Y()/z.Z()*posz;
    rcoo[2]=ev->pParticle(i)->RichCoo[0][2]+posz;
    phi+=dphi;
    double u=sin(theta)*cos(phi);
    double v=sin(theta)*sin(phi);
    double w=cos(theta);
    TVector3 ray(u,v,w);
    ray.Transform(r);

    rcoo[0]+=ray.X()/ray.Z()*thick;
    rcoo[1]+=ray.Y()/ray.Z()*thick;
    rcoo[2]+=ray.Z()/ray.Z()*thick;
//  Now refraction to the plex
    double plex_thick =0.1;
    const double n_plex=1.49;
    {
     double st=refi*sin(ray.Theta())/n_plex;
     double u1=st*cos(ray.Phi());
     double v1=st*sin(ray.Phi());
     double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     // propagate
     rcoo[0]+=ray.X()/ray.Z()*plex_thick;
     rcoo[1]+=ray.Y()/ray.Z()*plex_thick;
     rcoo[2]+=ray.Z()/ray.Z()*plex_thick;
    }
//  Now refraction
    double st=n_plex*sin(ray.Theta());
    if(st>=1){
//      cerr<< "full refl "<<st<<endl;
      k=k-1;
      if(!(npoint--))break;
      continue;
    }
     
    double u1=st*cos(ray.Phi());
    double v1=st*sin(ray.Phi());
    double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     double zl=ev->pParticle(i)->RichCoo[1][2]-rcoo[2]+3.2;
//    cout <<" after "<<ray.X()<<" "<<ray.Y()<<" "<<ray.Z()<<" "<<ray.Phi()<<" "<<ray.Theta()<<endl;
    array[k*3+0]=rcoo[0]+ray.X()/ray.Z()*zl;
    array[k*3+1]=rcoo[1]+ray.Y()/ray.Z()*zl;
    array[k*3+2]=ev->pParticle(i)->RichCoo[1][2]+3.2;
    double rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    double rc=sqrt(xc*xc+yc*yc)+r2+(array[k*3+2]-z2)*tc;
    while(rp>rc   && ray.Z()<0){
 
      zl=ev->pParticle(i)->RichCoo[1][2]-rcoo[2]+3.2;
      // take iterations
       double rp2=rp;
       double rc2=rc;
       double eps=1.e-2;
       while(fabs(rp2-rc2)>eps && fabs(zl)>eps){       
        zl=zl/2;
        rcoo[0]+=ray.X()/ray.Z()*zl;
        rcoo[1]+=ray.Y()/ray.Z()*zl;
        rcoo[2]+=zl;
        rp2=sqrt(rcoo[0]*rcoo[0]+rcoo[1]*rcoo[1]);
        rc2=sqrt(xc*xc+yc*yc)+r2+(rcoo[2]-z2)*tc;
        if(rp2>rc2 && zl<0)zl=-zl;
        else if(rp2<rc2 && zl>0)zl=-zl;
      }           
//      cout <<"  got iteration "<<rcoo[0]<< " "<<rcoo[1]<<" "<<rcoo[2]<< " "<<rp2 <<" "<<rc2<<endl;
     // get norm vector to cone
     double cw=sin(atan(-tc));
     double phin=atan2(rcoo[1],rcoo[0]);
     double cu=cos(atan(-tc))*cos(phin);
     double cv=cos(atan(-tc))*sin(phin);
//     cout <<"  normal to cone "<<cu<<" "<<cv<<" "<<cw<<endl;
     //  reflect
     double cc=ray.X()*cu+ray.Y()*cv+ray.Z()*cw;
     double ru=ray.X()-2*cc*cu;
     double rv=ray.Y()-2*cc*cv;
     double rw=ray.Z()-2*cc*cw;
     ray=TVector3(ru,rv,rw);
     array[k*3+0]=rcoo[0]+ru/rw*(ev->pParticle(i)->RichCoo[1][2]+3.2-rcoo[2]);
     array[k*3+1]=rcoo[1]+rv/rw*(ev->pParticle(i)->RichCoo[1][2]+3.2-rcoo[2]);
     rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    }
     if(ray.Z()>0){
      k--;
      if(!(npoint--))break;
//      cout <<"removing point point "<<k<<" "<<theta<<" "<<phi<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1])<<endl;
      continue;
     }
     const double hole=32;
     if(fabs(array[k*3+0])<hole && fabs(array[k*3+1])<hole){
       // in the hole
      k--;
      if(!(npoint--))break;
      continue;
     }
//    cout <<" ray "<<ray.X()<<" "<<ray.Y()<<" "<<ray.Z()<<endl;
//    cout <<"point "<<k<<" "<<theta<<" "<<phi<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1])<<endl;
   }   

   SetPolyLine(npoint,array);
   SetLineColor(6);
   int size=gAMSDisplay->Focus()==0?2:1;
   SetLineWidth(size*2);
   SetLineStyle(1);
   return;
   }
   }
   }
   return;
   }
   else{
    const double plex_thick =0.1;
    const double n_plex=1.49;
    double shift=0;
   double refi=pcl->Beta*cos(pcl->Theta);
    refi=1/refi;
  if(refi>1.1){
    rad_thick=-0.5;
  }
  else {
  if(ev->Version()<130){
   rad_thick=-3;
  }
  else rad_thick=-2.5;
  }
  if(drawplex){
      refi=n_plex;
      shift=-rad_thick/2; 
      rad_thick=-0.1;
      shift+=-rad_thick/2; 
  }    

   double theta=pcl->Theta;
    //cout <<" theta "<<pcl->Theta<<"  " <<refi<<endl;
    TVector3 z(pcl->TrPMTPos[0]-pcl->TrRadPos[0],pcl->TrPMTPos[1]-pcl->TrRadPos[1],pcl->TrPMTPos[2]-pcl->TrRadPos[2]);
    //cout << pcl->TrRadPos[2]<<"  "<<pcl->TrPMTPos[2]<<endl;
    double rcoo[3];
    TRotation r;
    r.SetZAxis(z);
   double dphi=2*3.1415926/(npoint-1);
   double phi=-dphi;
   for( int k=0;k<npoint;k++){
    //cout << "  k npoint "<<k<<" "<<npoint<<endl;
   double posz=rad_thick*(float(rand())/RAND_MAX-0.5);
   double thick=rad_thick*0.5-posz;  
    rcoo[0]=pcl->TrRadPos[0]+z.X()/z.Z()*posz;
    rcoo[1]=pcl->TrRadPos[1]+z.Y()/z.Z()*posz;
    rcoo[2]=pcl->TrRadPos[2]+posz;
    phi+=dphi;
    double u=sin(theta)*cos(phi);
    double v=sin(theta)*sin(phi);
    double w=cos(theta);
    TVector3 ray(u,v,w);
    ray.Transform(r);

    rcoo[0]+=ray.X()/ray.Z()*thick;
    rcoo[1]+=ray.Y()/ray.Z()*thick;
    rcoo[2]+=ray.Z()/ray.Z()*thick;
//  Now refraction to the plex
    if( !drawplex){
     double st=refi*sin(ray.Theta())/n_plex;
     double u1=st*cos(ray.Phi());
     double v1=st*sin(ray.Phi());
     double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     // propagate
     rcoo[0]+=ray.X()/ray.Z()*plex_thick;
     rcoo[1]+=ray.Y()/ray.Z()*plex_thick;
     rcoo[2]+=ray.Z()/ray.Z()*plex_thick;
    }
//  Now refraction
    double st=n_plex*sin(ray.Theta());
    if(st>=1){
//      cerr<< "full refl "<<st<<endl;
            k--;
      if(!(npoint--))break;
      else continue;
    }
     
    double u1=st*cos(ray.Phi());
    double v1=st*sin(ray.Phi());
    double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     double addon=4;
     if(ev->Version()>160){
        addon=0;
     }
    double rp=sqrt(rcoo[0]*rcoo[0]+rcoo[1]*rcoo[1]);
    double rc=sqrt(xc*xc+yc*yc)+r2+(rcoo[2]-z2)*tc;
    if(rp>rc){
      k--;
      if(!(npoint--))break;
      else continue;
    }
    double zl=pcl->TrPMTPos[2]-rcoo[2]+addon;
    array[k*3+0]=rcoo[0]+ray.X()/ray.Z()*zl;
    array[k*3+1]=rcoo[1]+ray.Y()/ray.Z()*zl;
    array[k*3+2]=pcl->TrPMTPos[2]+addon;
    rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    rc=sqrt(xc*xc+yc*yc)+r2+(array[k*3+2]-z2)*tc;
    while(rp>rc  && ray.Z()<0){
      zl=pcl->TrPMTPos[2]-rcoo[2]+addon;
      //cout <<"  mirror found "<<k<<" "<<rp<<" "<<rc<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<zl<<endl;
      // take iterations
       double rp2=rp;
       double rc2=rc;
       double eps=1.e-2;
       while(fabs(rp2-rc2)>eps && fabs(zl)>eps){       
        zl=zl/2;
        rcoo[0]+=ray.X()/ray.Z()*zl;
        rcoo[1]+=ray.Y()/ray.Z()*zl;
        rcoo[2]+=zl;
        rp2=sqrt(rcoo[0]*rcoo[0]+rcoo[1]*rcoo[1]);
        rc2=sqrt(xc*xc+yc*yc)+r2+(rcoo[2]-z2)*tc;
        if(rp2>rc2 && zl<0)zl=-zl;
        else if(rp2<rc2 && zl>0)zl=-zl;
        
      }           
     // get norm vector to cone
     double cw=sin(atan(-tc));
     double phin=atan2(rcoo[1],rcoo[0]);
     double cu=cos(atan(-tc))*cos(phin);
     double cv=cos(atan(-tc))*sin(phin);
     //  reflect
     double cc=ray.X()*cu+ray.Y()*cv+ray.Z()*cw;
     double ru=ray.X()-2*cc*cu;
     double rv=ray.Y()-2*cc*cv;
     double rw=ray.Z()-2*cc*cw;
     ray=TVector3(ru,rv,rw);
     array[k*3+0]=rcoo[0]+ru/rw*(pcl->TrPMTPos[2]+addon-rcoo[2]);
     array[k*3+1]=rcoo[1]+rv/rw*(pcl->TrPMTPos[2]+addon-rcoo[2]);
     rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    }
     if(ray.Z()>0){
            k--;
      if(!(npoint--))break;
      else continue;
     }
     const double hole=32;
     if(fabs(array[k*3+0])<hole && fabs(array[k*3+1])<hole){
       // in the hole
            k--;
      if(!(npoint--))break;
      else continue;
     }
//    cout <<"point "<<k<<" "<<theta<<" "<<phi<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1])<<endl;
   }   
}
   SetPolyLine(npoint,array);
   SetLineColor(6);
   int size=gAMSDisplay->Focus()==0?2:1;
   SetLineWidth(size*2);
   SetLineStyle(1);
   if(drawplex)SetLineStyle(2);
   return;
}
