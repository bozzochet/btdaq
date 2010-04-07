//  $Id: trrec.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
//
// Mar 20, 1997. ak. check if Pthit != NULL in AMSTrTrack::Fit
//
// Last Edit : Mar 20, 1997. ak
// 
#include "trrec.h"
#include "commons.h"
#include <math.h>
#include <limits.h>
#include "job.h"
#include "amsstl.h"
#include "trrawcluster.h"
#include "extC.h"
#include "upool.h"
#include <string.h>
#include "tofrec02.h"
#include "ntuple.h"
#include "cont.h"
#include "tkdbc.h"
//#include <trigger3.h>
#include "trigger302.h"
#include "tralig.h"
#include "mccluster.h"
#include "trdrec.h"
#include "vtx.h"
//

using namespace std;



integer AMSTrTrack::_RefitIsNeeded=0;
integer AMSTrTrack::_MarginPatternsNeeded=0;
integer AMSTrTrack::_max_ambigous_hits=2;

integer AMSTrCluster::build(integer refit){
  AMSlink * OriginalLast[2];
  integer OriginalNumber[2];
  AMSContainer * pct[2];
  geant Thr2R[2];
  integer ThrClNel[2];
  if(refit){
    AMSgObj::BookTimer.start("TrClusterRefit");
    // prepare some elements
    int i;
    for (i=0;i<2;i++){
      pct[i]=AMSEvent::gethead()->getC("AMSTrCluster",i);
      #ifdef __AMSDEBUG__
        assert(pct[i]!=NULL);
      #endif
      OriginalNumber[i]=pct[i]->getnelem();
      OriginalLast[i]=pct[i]->getlast();
      Thr2R[i]=TRCLFFKEY.Thr2R[i];
      ThrClNel[i]=TRCLFFKEY.ThrClNEl[i];
      TRCLFFKEY.Thr2R[i]=min((number)3.,Thr2R[i]*3.);
      TRCLFFKEY.ThrClNEl[i]=min(5,ThrClNel[i]+2);
    }
  }
 
  integer size=(TKDBc::maxstripsdrp()+1+
  2*max(TRCLFFKEY.ThrClNEl[0],TRCLFFKEY.ThrClNEl[1]))*sizeof(number);
  number *  adc  = (number*)UPool.insert(size); 
  AMSTrIdSoft id;
  for(int icll=0;icll<AMSTrIdSoft::ncrates();icll++){
   AMSTrRawCluster *p=(AMSTrRawCluster*)AMSEvent::gethead()->
   getheadC("AMSTrRawCluster",icll,1);
   VZERO(adc,size/4);
   while(p){
    // Unpack cluster into tmp space
    // find clusters
     id=AMSTrIdSoft(p->getid());
     integer ilay=id.getlayer();
     integer side=id.getside();
     // Very debug
     //          if(ilay==3 && side==0){p=p->next();continue;}
     p->expand(adc+TRCLFFKEY.ThrClNEl[side]/2);
    if(p->testlast()){
              
     // Circle buffer for x layers 1 && 6;
     // 
     if(side==0 && (ilay==1 || ilay==TKDBc::nlay())){
       for (int iloc=0;iloc<TRCLFFKEY.ThrClNEl[side]/2;iloc++){
        adc[iloc]=adc[id.getmaxstrips()+iloc];
        adc[iloc+id.getmaxstrips()+TRCLFFKEY.ThrClNEl[side]/2]=
        adc[iloc+TRCLFFKEY.ThrClNEl[side]/2];
       }  
     }
     number ref;
     ref=-FLT_MAX;
     number sum;
     number ssum;
     number pos;
     number rms;
     integer left,right,status,above,center;
     for (int i=TRCLFFKEY.ThrClNEl[side]/2;
     i<id.getmaxstrips()+TRCLFFKEY.ThrClNEl[side]/2+1;i++){
     if(adc[i]<ref){
      // cluster cand found
      if( adc[i]< adc[i+1] && adc[i+1]> TRCLFFKEY.Thr1A[side]){
      // "wide" cluster
      status=AMSDBc::WIDE;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-TRCLFFKEY.ThrClNEl[side]/2);
      center=i;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2,
            id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);  
      } 
      else if(adc[i+1]<adc[i+2] && adc[i+2]>TRCLFFKEY.Thr1A[side]){
       // two clusters near each other; take care about rightmost strip;
      status=AMSDBc::NEAR;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-1-TRCLFFKEY.ThrClNEl[side]/2);
      center=i-1;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2-1,
      id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);    
      }
      else{
       status=0;
       left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
       i-1-TRCLFFKEY.ThrClNEl[side]/2);
       center=i-1;
       right=min(i-1+TRCLFFKEY.ThrClNEl[side]/2,
       id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);   
      }
      sum=0;
      ssum=0;
      pos=0;
      rms=0;
      above=0;
      //
      // we don't know here the strip size (unfortunately...)
      // so put 1 instead ...
      // 
      int j;
      for (j=left;j<right+1;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
       if(!id.checkstatus(AMSDBc::BAD) && 
          adc[j]/id.getsig()>TRCLFFKEY.Thr3R[side]){
       }
       else adc[j]=0;
      }  

      for (j=left;j<center;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
        if(j-center< -1 && j-left==0 &&  
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<TRCLFFKEY.Thr2R[side])) {
          left++;
          continue;
        }
        if(j-center< 0 && j-left==0 &&
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.))){
          left++;
          continue;
        }
      }
      integer ro=right;
      for (j=right;j>center;j--){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
        if(j-center> 1 && j-right==0 &&  
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<TRCLFFKEY.Thr2R[side])) {
          right--;
          continue;
        }
        if(j-center> 0 && j-right==0 &&
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.))){
          right--;
          continue;
        }
      }

      for (j=left;j<right+1;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
       if(adc[j]>TRCLFFKEY.Thr2A[side])above++;
        if(j==ro+1 && status==AMSDBc::NEAR)sum+=adc[j]/2;
        else sum+=adc[j];
//        ssum=ssum+pow(id.getsig(),2.);
        ssum=ssum+id.getsig()*id.getsig();
        pos=pos+1*(j-center)*adc[j];
//        rms=rms+pow(1*(j-center),2)*adc[j];
        rms=rms+(j-center)*(j-center)*adc[j];
      }
       if(sum !=0){
        rms=sqrt(fabs(rms*sum-pos*pos))/sum; 
        pos=pos/sum+1*0.5;
        ssum=sqrt(ssum);
       }
      ref=-FLT_MAX;
      if(above >= TRCLFFKEY.ThrClNMin[side] && 
         sum> TRCLFFKEY.ThrClA[side] && ssum > 0 && 
         ssum < TRCLFFKEY.ThrClS[side] && sum/ssum > TRCLFFKEY.ThrClR[side]){
         id.upd(center-TRCLFFKEY.ThrClNEl[side]/2);
         if(right-left+1 > TRCLFFKEY.ThrClNEl[side]){
           if(adc[left]>adc[right])right--;
           else left++;
         }
         number eta=AMSTrCluster::_etacalc(left-center,right-center+1,adc+left);
         number cor=1.;
         if (TRCLFFKEY.EtaCor[side]) cor=AMSTrCluster::_etacor(side,eta);
         AMSTrCluster* pclnew=_addnext(
         id,status,left-center,right-center+1,cor*sum,cor*ssum,pos,rms,eta,adc+left);
           for (int j=left;j<right+1;j++){
            id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
             number addon=(id.getped()+id.getcmnnoise()+sum/64)*id.getavgain();
//            if(adc[j]*id.getgain()+addon>=30000){
//             cout <<"jopa "<<id.getgain()<<" "<<sum/8<<" "<<adc[j]*id.getgain()+addon<<" " <<addon<<endl;
//            }
            HF1(200107,adc[j]*id.getgain()+addon,1.);
            if(adc[j]*id.getgain()+addon>=TRMCFFKEY.adcoverflow){
             pclnew->setstatus(AMSDBc::AOVERFLOW);
            }
            if(j==right+1 && status==AMSDBc::NEAR)adc[j]=adc[j]/2;
            else adc[j]=0;
           }
      }                      
     }
     else{
      if(adc[i] > TRCLFFKEY.Thr1A[side]){
       // susp bump found
       id.upd(i-TRCLFFKEY.ThrClNEl[side]/2);
       if(id.checkstatus(AMSDBc::BAD)==0 && 
          id.getsig() < TRCLFFKEY.Thr1S[side] && 
          adc[i]/id.getsig() > TRCLFFKEY.Thr1R[side] )ref=adc[i];
      }
     }
     } 
     VZERO(adc,size/4);
    } 
     p=p->next();           
  }
  }
  UPool.udelete(adc);

  if(refit){
     int i;
     for (i=0;i<2;i++){
       pct[i]=AMSEvent::gethead()->getC("AMSTrCluster",i);
      #ifdef __AMSDEBUG__
        assert(pct[i]!=NULL);
      #endif

       // Restore thresholds

       TRCLFFKEY.Thr2R[i]=Thr2R[i];
       TRCLFFKEY.ThrClNEl[i]=ThrClNel[i];

       // Mark all new clusters to delete

       AMSTrCluster * pclnew=((AMSTrCluster*)OriginalLast[i])->next();
       while(pclnew){
         pclnew->setstatus(AMSDBc::DELETED);
         pclnew=pclnew->next();
       }

       // Find & mark corresponding elements

       AMSTrCluster *pcl=(AMSTrCluster*)pct[i]->gethead();
       while(pcl){
         if(pcl->checkstatus(AMSDBc::REFITTED)){
          pclnew=((AMSTrCluster*)OriginalLast[i])->next();
          AMSTrIdSoft pclid=pcl->getid();
          while(pclnew){
            if(pclnew->getid() == pclid){
              // mark
              pcl->setstatus(AMSDBc::DELETED);
              pclnew->clearstatus(AMSDBc::DELETED);
              pclnew->setstatus(AMSDBc::REFITTED);
            }     
            pclnew=pclnew->next();
          }
         }
         pcl=pcl->next();
       } 
       // Delete marked clusters
      pcl=(AMSTrCluster*)pct[i]->gethead();
      while(pcl && pcl->checkstatus(AMSDBc::DELETED)){
        pct[i]->removeEl(0,0);
        pcl=(AMSTrCluster*)pct[i]->gethead(); 
      }     
      while(pcl){
        while(pcl->next() && (pcl->next())->checkstatus(AMSDBc::DELETED))
        pct[i]->removeEl(pcl,0);
        pcl=pcl->next();
      }
      // Restore positions
     AMSlink * ptmp=pct[i]->gethead();
     integer ip=1;
     while(ptmp){
      ptmp->setpos(ip++);
      ptmp=ptmp->_next;
     }
     }
    AMSgObj::BookTimer.stop("TrClusterRefit");
  }
  return 1;
}



integer AMSTrCluster::buildWeak(integer refit){
  AMSlink * OriginalLast[2];
  integer OriginalNumber[2];
  AMSContainer * pct[2];
  geant Thr2R[2];
  integer ThrClNel[2];
  if(refit){
   return -1;
  }
 
  integer size=(TKDBc::maxstripsdrp()+1+
  2*max(TRCLFFKEY.ThrClNEl[0],TRCLFFKEY.ThrClNEl[1]))*sizeof(number);
  number *  adc  = (number*)UPool.insert(size); 
  AMSTrIdSoft id;
  // only x clusters can be weak
  for(int icll=0;icll<AMSTrIdSoft::ncrates();icll++){
   AMSTrRawCluster *p=(AMSTrRawCluster*)AMSEvent::gethead()->
   getheadC("AMSTrRawCluster",icll,1);
   VZERO(adc,size/4);
   while(p){
    // Unpack cluster into tmp space
    // find clusters
     id=AMSTrIdSoft(p->getid());
     integer ilay=id.getlayer(); 
     integer side=id.getside();
     if(side==0){
     // Very debug
     //         if(ilay==3 && side==0){p=p->next();continue;}
     p->expand(adc+TRCLFFKEY.ThrClNEl[side]/2);
    if(p->testlast()){

              
     // Circle buffer for x layers 1 && 6;
     // 
     if(side==0 && (ilay==1 || ilay==TKDBc::nlay())){
       for (int iloc=0;iloc<TRCLFFKEY.ThrClNEl[side]/2;iloc++){
        adc[iloc]=adc[id.getmaxstrips()+iloc];
        adc[iloc+id.getmaxstrips()+TRCLFFKEY.ThrClNEl[side]/2]=
        adc[iloc+TRCLFFKEY.ThrClNEl[side]/2];
       }  
     }

           // first (Unfortunately) find out and destroy all " normal clusters"
     {
     

     number ref;
     ref=-FLT_MAX;
     number sum;
     number ssum;
     number pos;
     number rms;
     integer left,right,status,above,center;
     for (int i=TRCLFFKEY.ThrClNEl[side]/2;
     i<id.getmaxstrips()+TRCLFFKEY.ThrClNEl[side]/2+1;i++){
     if(adc[i]<ref){
      // cluster cand found
      if( adc[i]< adc[i+1] && adc[i+1]> TRCLFFKEY.Thr1A[side]){
      // "wide" cluster
      status=AMSDBc::WIDE;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-TRCLFFKEY.ThrClNEl[side]/2);
      center=i;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2,
            id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);  
      } 
      else if(adc[i+1]<adc[i+2] && adc[i+2]>TRCLFFKEY.Thr1A[side]){
       // two clusters near each other; take care about rightmost strip;
      status=AMSDBc::NEAR;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-1-TRCLFFKEY.ThrClNEl[side]/2);
      center=i-1;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2-1,
      id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);    
      }
      else{
       status=0;
       left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
       i-1-TRCLFFKEY.ThrClNEl[side]/2);
       center=i-1;
       right=min(i-1+TRCLFFKEY.ThrClNEl[side]/2,
       id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);   
      }
      sum=0;
      ssum=0;
      pos=0;
      rms=0;
      above=0;
      //
      // we don't know here the strip size (unfortunately...)
      // so put 1 instead ...
      // 
      int j;
      for (j=left;j<right+1;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
       if(!id.checkstatus(AMSDBc::BAD) && 
          adc[j]/id.getsig()>TRCLFFKEY.Thr3R[side]){
       }
       else adc[j]=0;
      }  

      for (j=left;j<center;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
        if(j-center< -1 && j-left==0   ) {
          left++;
          continue;
        }
        if(j-center< 0 && j-left==0 &&
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.))){
          left++;
          continue;
        }
      }
      integer ro=right;
      for (j=right;j>center;j--){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
        if(j-center> 1 && j-right==0 ) {
          right--;
          continue;
        }
        if(j-center> 0 && j-right==0 &&
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.))){
          right--;
          continue;
        }
      }

      for (j=left;j<right+1;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
       if(adc[j]>TRCLFFKEY.Thr2A[side])above++;
        if(j==ro+1 && status==AMSDBc::NEAR)sum+=adc[j]/2;
        else sum+=adc[j];
//        ssum=ssum+pow(id.getsig(),2.);
        ssum=ssum+id.getsig()*id.getsig();
        pos=pos+1*(j-center)*adc[j];
//        rms=rms+pow(1*(j-center),2)*adc[j];
        rms=rms+(j-center)*(j-center)*adc[j];
      }


       if(sum !=0){
        rms=sqrt(fabs(rms*sum-pos*pos))/sum; 
        pos=pos/sum+1*0.5;
        ssum=sqrt(ssum);
       }
      ref=-FLT_MAX;
      if(above >= TRCLFFKEY.ThrClNMin[side] && 
         sum> TRCLFFKEY.ThrClA[side] && ssum > 0 && 
         ssum < TRCLFFKEY.ThrClS[side] && sum/ssum > TRCLFFKEY.ThrClR[side]){
         id.upd(center-TRCLFFKEY.ThrClNEl[side]/2);
         if(right-left+1 > TRCLFFKEY.ThrClNEl[side]){
           if(adc[left]>adc[right])right--;
           else left++;
         }
           for (int j=left;j<right+1;j++){
             if(j==right+1 && status==AMSDBc::NEAR)adc[j]=adc[j]/2;
             else adc[j]=0;
           }
      }                      
     }
     else{
      if(adc[i] > TRCLFFKEY.Thr1A[side]){
       // susp bump found
       id.upd(i-TRCLFFKEY.ThrClNEl[side]/2);
       if(id.checkstatus(AMSDBc::BAD)==0 && 
          id.getsig() < TRCLFFKEY.Thr1S[side] && 
          adc[i]/id.getsig() > TRCLFFKEY.Thr1R[side] )ref=adc[i];
      }
     }
     } 




     }
     // Now find "weak clusters"
     {




     number ref;
     ref=-FLT_MAX;
     number sum;
     number ssum;
     number pos;
     number rms;
     integer left,right,status,above,center;
     for (int i=TRCLFFKEY.ThrClNEl[side]/2;
     i<id.getmaxstrips()+TRCLFFKEY.ThrClNEl[side]/2+1;i++){
     if(adc[i]<ref){
      // cluster cand found
      if( adc[i]< adc[i+1] && adc[i+1]> TRCLFFKEY.Thr1A[side]){
      // "wide" cluster
      status= AMSDBc::WIDE;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-TRCLFFKEY.ThrClNEl[side]/2);
      center=i;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2,
            id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);  
      } 
      else if(adc[i+1]<adc[i+2] && adc[i+2]>TRCLFFKEY.Thr1A[side]){
       // two clusters near each other; take care about rightmost strip;
      status=AMSDBc::NEAR;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-1-TRCLFFKEY.ThrClNEl[side]/2);
      center=i-1;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2-1,
      id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);    
      }
      else{
       status=0;
       left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
       i-1-TRCLFFKEY.ThrClNEl[side]/2);
       center=i-1;
       right=min(i-1+TRCLFFKEY.ThrClNEl[side]/2,
       id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);   
      }
      sum=0;
      ssum=0;
      pos=0;
      rms=0;
      above=0;
      //
      // we don't know here the strip size (unfortunately...)
      // so put 1 instead ...
      // 
      for (int j=left;j<right+1;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
       if(!id.checkstatus(AMSDBc::BAD) && 
          adc[j]/id.getsig()>TRCLFFKEY.Thr3R[side]){
        if(j-center<= -TRCLFFKEY.ThrClNEl[side]/2 && 
           adc[j]/id.getsig()<TRCLFFKEY.Thr2R[side]){
           left++;
           j++;
           id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
           if(j-center< 0 && 
           adc[j]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.)){
            left++;
            continue;
           }
        }
        if(j-center>= TRCLFFKEY.ThrClNEl[side]/2 && 
           adc[j]/id.getsig()<TRCLFFKEY.Thr2R[side]){
           right--;
           id.upd(j-1-TRCLFFKEY.ThrClNEl[side]/2);
           if(j-1-center> 0 && 
           adc[j-1]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.)){
            right--;
           }
           
           continue;
        }
       if(adc[j]>TRCLFFKEY.Thr2A[side])above++;
        if(j==right+1 && status==AMSDBc::NEAR)sum+=adc[j]/2;
        else sum+=adc[j];
//        ssum=ssum+pow(id.getsig(),2.);
        ssum=ssum+id.getsig()*id.getsig();
        pos=pos+1*(j-center)*adc[j];
//        rms=rms+pow(1*(j-center),2)*adc[j];
        rms=rms+(j-center)*(j-center)*adc[j];
       }
       else adc[j]=0;

      }
       if(sum !=0){
        rms=sqrt(fabs(rms*sum-pos*pos))/sum; 
        pos=pos/sum+1*0.5;
        ssum=sqrt(ssum);
       }
      ref=-FLT_MAX;
      if(above >= TRCLFFKEY.ThrClNMin[side] && 
         sum> TRCLFFKEY.ThrClA[side] && ssum > 0 && 
         ssum < TRCLFFKEY.ThrClS[side] && sum/ssum > TRCLFFKEY.ThrClR[side]){
         id.upd(center-TRCLFFKEY.ThrClNEl[side]/2);
         if(right-left+1 > TRCLFFKEY.ThrClNEl[side]){
           if(adc[left]>adc[right])right--;
           else left++;
         }
         status=status | AMSDBc::WEAK;
         if(id.getsig()!=0 ) {
           number eta=AMSTrCluster::_etacalc(left-center,right-center+1,adc+left);
           number cor=1.;
           if (TRCLFFKEY.EtaCor[side]) cor=AMSTrCluster::_etacor(side,eta);
           AMSTrCluster* pclnew=_addnext(
           id,status,left-center,right-center+1,cor*sum,cor*ssum,pos,rms,eta,adc+left);
           for (int j=left;j<right+1;j++){
            id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
             number addon=(id.getped()+id.getcmnnoise()+sum/64)*id.getavgain();
            if(adc[j]*id.getgain()+addon>=TRMCFFKEY.adcoverflow){
             pclnew->setstatus(AMSDBc::AOVERFLOW);
            }
            if(j==right+1 && status==AMSDBc::NEAR)adc[j]=adc[j]/2;
            else adc[j]=0;
           }
         }
         status=status &  (~AMSDBc::WEAK);
      }                      
     }
     else{
      if(adc[i] > TRCLFFKEY.Thr1A[side]){
       // susp bump found
       id.upd(i-TRCLFFKEY.ThrClNEl[side]/2);
       if(id.checkstatus(AMSDBc::BAD)==0 && 
          id.getsig() < TRCLFFKEY.Thr1S[side] && 
          adc[i]/id.getsig() > TRMCFFKEY.thr1R[side] )ref=adc[i];
      }
     }
     }







     }


 
     VZERO(adc,size/4);
    }
     } 
     p=p->next();           
   }
  }
  UPool.udelete(adc);


    return 1;
}




number AMSTrCluster::getcofg(AMSTrIdGeom * pid){
  //
  // Here we are able to recalculate the center of gravity...
  //
#ifdef __AMSDEBUG__
  _Id.match(pid);
#endif
 integer side=_Id.getside();
number cofg=0;
number eval=0;
number smax=0;
number smt=0;
if(_pValues)smax=_pValues[-_NelemL];
integer i,error;
for(i=_NelemL;i<_NelemR;i++){
 cofg+=_pValues[i-_NelemL]*pid->getcofg(side,i,_Id.getstrip(),error);
 if(error==0)eval+=_pValues[i-_NelemL];
}
if(eval > 0){
cofg=cofg/eval;
smt=smax/eval;
}
else{
#ifdef __AMSDEBUG__
  cerr <<"AMSTrCluster::getcofg-E-NegativeClusterAmplitudeFound "<<eval<<endl;
#endif
cofg=0;
smt=0;
}
return cofg-cfgCorFun(smt,pid); 
}

number AMSTrCluster::getlcofg(AMSTrIdGeom * pid){

if(_pValues){
 int error;
 return getcofg(pid)-pid->getcofg(_Id.getside(),0,_Id.getstrip(),error);
}
else return 0;
}


number AMSTrCluster::cfgCorFun(number s, AMSTrIdGeom * pid){
integer side=_Id.getside();
integer strip=pid->getstrip(side);
integer l=pid->getlayer()-1;
if(strip == 0  ){
 if(s<TRCLFFKEY.CorFunParB[side][l]) return 0;
 else return TRCLFFKEY.CorFunParA[side][l]*
      atan(s-TRCLFFKEY.CorFunParB[side][l]);
}
else if(strip== pid->getmaxstrips(side)-1){
 if(s<TRCLFFKEY.CorFunParB[side][l]) return 0;
 else return -TRCLFFKEY.CorFunParA[side][l]*
      atan(s-TRCLFFKEY.CorFunParB[side][l]);
}
else return 0;
}
void AMSTrCluster::_ErrorCalc(){
#if 1
  // Temporary
integer side=_Id.getside();
if(side==1)_ErrorMean =TRCLFFKEY.ErrY;
else  _ErrorMean =TRCLFFKEY.ErrX;

#else
// Here is the right code

#endif
}

number AMSTrCluster::_etacalc(integer nelemL, integer nelemR, number val[]) {
  number eta=-1.;
  int i;

// no eta defined for 1 strip clusters
  int nstrp=-nelemL+nelemR;
  if (nstrp<2) return eta;

// get eta value
  int im=-1,in=-1;
  number mx=0.;
  for (i=0;i<nstrp;i++) {
    if (val[i]>mx) {
      mx=val[i];
      im=i;
    }
  }
  if (mx<=0. || im<0) {
    cout << " cluster strips: " << nstrp << endl;
    cout << " cluster amps: " <<val[0]<<" "<<val[1]<<" "<<val[2]<<" "
                              <<val[3]<<" "<<val[4]<< endl;
    cout << " max: " << im << " next: " << in << endl;
    return eta; 
  }
  if (im==0 || im<nstrp-1&&val[im+1]>val[im-1]) {
    in=im+1;
  }
  else in=im-1;

  eta=val[max(im,in)]/(val[im]+val[in]);
  if (eta<0. || eta>1.) eta=-1.;
  return eta;
}

number AMSTrCluster::_etacor(integer side, number eta) {
  static number etagk=.93078, etack[5]={1.5450,-5.3571,15.318,-20.002,10.048};
  number cor=1.;
  integer i;

// no eta correction for simulation
  if (!AMSJob::gethead()->isRealData()){
    static integer first=1;
    if(first){
      first=0;
      cout<<"AMSTrCluster::_etacor - No eta corrections on MC"<<endl;
    }
    return cor;
  }

// no eta correction for S side
  if (side==1) return cor;

// apply corrections
  if (eta>=0. && eta<=1.) {
    number sum=0.;
    for (i=0;i<5;i++) sum+=etack[i]*pow(eta,i);
    cor=etagk/sum;
  }
// cout<<" K side corrections applied. cor= "<<cor<<endl;
  return cor;
}

  AMSTrCluster * AMSTrCluster::_addnext(const AMSTrIdSoft & id, 
                          integer status, integer nelemL,integer nelemR, 
                         number sum, number ssum, number pos, number rms,
                         number eta, number val[]) {
    return (AMSTrCluster*)AMSEvent::gethead()->addnext(AMSID("AMSTrCluster",id.getside()),
    new AMSTrCluster(id,status,nelemL,nelemR,sum,ssum,pos,rms,eta,val));
}

AMSTrCluster::AMSTrCluster(const AMSTrIdSoft& id, integer status, 
                           integer nelemL, integer nelemR, number sum,
                           number ssum, number pos,number rms, number eta,
                           number val[]):
    AMSlink(status,0),_Id(id),_NelemL(nelemL), _NelemR(nelemR),_Sum(sum), 
    _Sigma(ssum), _Mean(pos), _Rms(rms), _Eta(eta){
 _ErrorCalc();
 if(getnelem()>0){
  _pValues=(number*)UPool.insert(getnelem()*sizeof(number));
  int i;
  for(i=0;i<getnelem();i++)_pValues[i]=val[i];
 }
}

void AMSTrCluster::_writeEl(){
// Fill the ntuple : check on IOPA.WriteAll%10 
  integer flag =    (IOPA.WriteAll%10==1)
                 || (IOPA.WriteAll%10==0 && checkstatus(AMSDBc::USED))
                 || (IOPA.WriteAll%10==2 && !checkstatus(AMSDBc::AwayTOF));

  if(AMSTrCluster::Out(flag) ){
    int i;
#ifdef __WRITEROOT__
     AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
    TrClusterNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trcl();

    if (TrN->Ntrcl>=MAXTRCL) return;

    TrN->Idsoft[TrN->Ntrcl]=_Id.cmpt();
    TrN->Status[TrN->Ntrcl]=_status;
    TrN->NelemL[TrN->Ntrcl]=_NelemL;
    TrN->NelemR[TrN->Ntrcl]=_NelemR;
    TrN->Sum[TrN->Ntrcl]=_Sum;
    TrN->Sigma[TrN->Ntrcl]=_Sigma;
    TrN->Mean[TrN->Ntrcl]=_Mean;
    TrN->RMS[TrN->Ntrcl]=_Rms;
    TrN->ErrorMean[TrN->Ntrcl]=_ErrorMean;
    for(i=0;i<min(5,getnelem());i++)TrN->Amplitude[TrN->Ntrcl][i]=_pValues[i]; 
    for(i=getnelem();i<5;i++)TrN->Amplitude[TrN->Ntrcl][i]=0;
    TrN->Ntrcl++;
  }
}

void AMSTrCluster::_copyEl(){
}

void AMSTrCluster::print(){
for(int i=0;i<2;i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSTrCluster",i);
 if(p)p->printC(cout);
}
}

AMSTrRecHit * AMSTrRecHit::_Head[trconst::maxlay]={0,0,0,0,0,0,0,0};


integer AMSTrRecHit::build(integer refit){
  if(refit){
    // Cleanup all  containers
    int i;
    for(i=0;;i++){
      AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrRecHit",i);
      if(pctr)pctr->eraseC();
      else break ;
    }
  } 
 AMSTrCluster *x;
 AMSTrCluster *y;
 AMSTrIdSoft idx;
 AMSTrIdSoft idy;
 integer ia,ib,ilay,nambig;
 y=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0); 
 while (y){
   idy=y->getid();
   ilay=idy.getlayer();
   if(y->checkstatus(AMSDBc::BAD)==0){
   x=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",0);
   while(x){
      idx=x->getid();  
      if(x->checkstatus(AMSDBc::BAD) ==0 ){
      if(idx.getlayer() == idy.getlayer() && 
         idx.getdrp() == idy.getdrp() && idx.gethalf() == idy.gethalf()
         && fabs(y->getVal()-x->getVal())/(y->getVal()+x->getVal()) < 
         TRCLFFKEY.ThrDSide){
        // make MANY points.... Unfortunately...
         AMSTrIdGeom *pid = idx.ambig(idy, nambig);
         for(int i=0;i<nambig;i++){
         AMSgSen *p;
         p=(AMSgSen*)AMSJob::gethead()->getgeomvolume((pid+i)->crgid());
         if(!p){
           if(TKDBc::GetStatus((pid+i)->getlayer()-1,(pid+i)->getladder()-1,
           (pid+i)->getsensor()-1))
           cerr << "AMSTrRecHitBuild-S-Sensor-Error "<<
             (pid+i)->crgid()<<endl;          
         }
         else{
          _addnext(p,0,ilay,x->getlcofg(pid+i),y->getlcofg(pid+i),x,y,
          p->str2pnt(x->getcofg(pid+i),y->getcofg(pid+i)),
          AMSPoint(x->getecofg(),y->getecofg(),(number)TRCLFFKEY.ErrZ));
         }
         } 
      } 
      }
    x=x->next();
   }
   }
   y=y->next();
 }  

// Mark hits away from TOF predictions
 if (TRFITFFKEY.TOFTracking!=0) markAwayTOFHits();

 return 1;

}



integer AMSTrRecHit::buildWeak(integer refit){
  if(refit){
   return -1;
  } 
 AMSTrCluster *x;
 AMSTrCluster *y;
 AMSTrIdSoft idx;
 AMSTrIdSoft idy;
 integer ia,ib,ilay,nambig;
 y=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0); 
 while (y){
   idy=y->getid();
   ilay=idy.getlayer();
   if(y->checkstatus(AMSDBc::BAD)==0 && 
      y->checkstatus(AMSDBc::WEAK)==0){
   x=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",0);
   while(x){
      idx=x->getid();  
      if(x->checkstatus(AMSDBc::BAD) ==0 &&
       x->checkstatus(AMSDBc::WEAK)){
      if(idx.getlayer() == idy.getlayer() && 
         idx.getdrp() == idy.getdrp() && idx.gethalf() == idy.gethalf()
         && fabs(y->getVal()-x->getVal())/(y->getVal()+x->getVal()) < 
         TRCLFFKEY.ThrDSide){
        // make MANY points.... Unfortunately...
         AMSTrIdGeom *pid = idx.ambig(idy, nambig);
         for(int i=0;i<nambig;i++){
         AMSgSen *p;
         p=(AMSgSen*)AMSJob::gethead()->getgeomvolume((pid+i)->crgid());
         if(!p){
           if(TKDBc::GetStatus((pid+i)->getlayer()-1,(pid+i)->getladder()-1,
           (pid+i)->getsensor()-1))
           cerr << "AMSTrRecHitBuild-S-Sensor-Error "<<
             (pid+i)->crgid()<<endl;          
         }
         else{
           //           cout <<" rec hit weak added "<<endl;
          _addnext(p,AMSDBc::WEAK,ilay,x->getlcofg(pid+i),y->getlcofg(pid+i),x,y,
          p->str2pnt(x->getcofg(pid+i),y->getcofg(pid+i)),
          AMSPoint(x->getecofg(),y->getecofg(),(number)TRCLFFKEY.ErrZ));
         }
         } 
      } 
      }
    x=x->next();
   }
   }
   y=y->next();
 }  

// Mark hits away from TOF predictions
 if (TRFITFFKEY.TOFTracking!=0) markAwayTOFHits();

 return 1;

}

/* This function checks if AMSTrRecHits are consistent with TOF information */
/* and marks them if it is not the case
                                     */
//------
integer AMSTrRecHit::markAwayTOFHits(){

    int i;

      TriggerLVL302 *plvl3;
      plvl3 = (TriggerLVL302*)AMSEvent::gethead()->getheadC("TriggerLVL3",0);
// LVL3 required if existing
      if (plvl3==NULL) {
// #ifdef __AMSDEBUG__
//       cout << "markAwayTOFHits: No Level3 Trigger existing" << endl;
// #endif
        return 1;
      }
      if ( plvl3->skip() ) return 1;
      AMSTOFCluster *phit[4], *ploop;
// There should be one and only one AMSTOFCluster on planes 1, 4
// according to LVL3 trigger
      phit[0] = AMSTOFCluster::gethead(0);
      if ( (phit[0] == NULL) || (phit[0]->next()) ) return 1;
      phit[3] = AMSTOFCluster::gethead(3);
      if ( (phit[3] == NULL) || (phit[3]->next()) ) return 1;

// Initial straight line from planes 1 and 4 for ZX projection
      number slope_x= (phit[3]->getcoo()[0] - phit[0]->getcoo()[0]) /
                 (phit[3]->getcoo()[2] - phit[0]->getcoo()[2]);
      number intercept_x= phit[0]->getcoo()[0] - slope_x*phit[0]->getcoo()[2];

// Look for the best AMSTOFCluster on plane 2 within errors
// to improve the X prediction on the tracker
      number resmax2=999.;
      phit[1]=NULL;
      for (ploop = AMSTOFCluster::gethead(1); ploop ; ploop=ploop->next() ){
        number resx2 = fabs(ploop->getcoo()[0] 
                            - slope_x*ploop->getcoo()[2] - intercept_x);
        if (resx2<resmax2) {
          resmax2 = resx2;
          phit[1] = ploop;
        }
      }

// Look for the best AMSTOFCluster on plane 3 within errors
// to improve the X prediction
      number resmax3=999.;
      phit[2]=NULL;
      for (ploop = AMSTOFCluster::gethead(2); ploop ; ploop=ploop->next() ){
        number resx3 = fabs(ploop->getcoo()[0] 
                            - slope_x*ploop->getcoo()[2] - intercept_x);
        if (resx3<resmax3) {
          resmax3 = resx3;
          phit[2] = ploop;
        }
      }
// We require at least 3 AMSTOFClusters
    if ( (phit[1]==NULL) && (phit[2]==NULL) ) return 1;
//-----------
// Straight line parameters for the ZX plane 
// Use only Pad information (planes2,3: TOF calib. independent)
// unless they are missing
    number sw=0, sz=0, sx=0, sxz=0, szz=0;
    for (i=0; i<4; i++){
      if (phit[i]==NULL) continue;
      if (i==0 && phit[1]!=NULL) continue;
      if (i==3 && phit[2]!=NULL) continue;
      number w = phit[i]->getecoo()[0]; if (w<=0.) continue; w = 1./w/w;
      number x = phit[i]->getcoo()[0];
      number z = phit[i]->getcoo()[2];
      sw += w;
      sx += x*w;
      sz += z*w;
      sxz += x*z*w;
      szz += z*z*w;
    }
    number determinant = szz*sw-sz*sz;
    slope_x = (sxz*sw-sx*sz)/determinant;
    intercept_x = (szz*sx-sz*sxz)/determinant;
    number covss = sw/determinant;
    number covsi = -sz/determinant;
    number covii = szz/determinant;

// Straight line parameters for the ZY plane 
// (Only planes 1,4 => TOFCalib independent, but robust and enough)
    number slope_y= (phit[3]->getcoo()[1] - phit[0]->getcoo()[1]) /
               (phit[3]->getcoo()[2] - phit[0]->getcoo()[2]);
    number intercept_y= phit[0]->getcoo()[1] - slope_y*phit[0]->getcoo()[2];

// First mark all TRClusters as "away from TOF" by default
    AMSTrCluster* pclus;
    for (i=0;i<1;i++) {
      pclus = (AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",i);
      for (; pclus!=NULL; pclus=pclus->next()) {
        pclus->setstatus(AMSDBc::AwayTOF);
      }
    }

{
// Mark AMSTrRecHits which are outside the TOF path
    AMSTrRecHit * ptrhit;
    AMSPoint hit;
    geant searchregtof;
    searchregtof=TOF2DBc::plnstr(5)+2.*TOF2DBc::plnstr(8);
    for (i=0;i<TKDBc::nlay();i++) {
      for (ptrhit=AMSTrRecHit::gethead(i); ptrhit!=NULL; ptrhit=ptrhit->next()){
        hit = ptrhit->getHit();
        number xres = fabs(hit[0]-intercept_x - slope_x*hit[2]);
        number yres = fabs(hit[1]-intercept_y - slope_y*hit[2]);
        if (    xres > searchregtof
             || yres > searchregtof    ) {
          ptrhit->setstatus(AMSDBc::AwayTOF);
        }
        else {
          pclus = ptrhit->getClusterP(0);
          if (pclus) pclus->clearstatus(AMSDBc::AwayTOF);
          pclus = ptrhit->getClusterP(1);
          if (pclus) pclus->clearstatus(AMSDBc::AwayTOF);
        }
      }
    }

}
// Now we want add same for MC Tr Hits (V. Choutko 3/8/1999)

    AMSTrMCCluster * ptrhit=
    (AMSTrMCCluster*)AMSEvent::gethead()->getheadC("AMSTrMCCluster",0);    
    AMSPoint hit;
    geant searchregtof;
    searchregtof=TOF2DBc::plnstr(5)+2.*TOF2DBc::plnstr(8);
      while(ptrhit){
        hit = ptrhit->getHit();
        number xres = fabs(hit[0]-intercept_x - slope_x*hit[2]);
        number yres = fabs(hit[1]-intercept_y - slope_y*hit[2]);
        if (    xres < searchregtof
             && yres < searchregtof    ) {
           ptrhit->clearstatus(AMSDBc::AwayTOF);
        }
        else if(yres< searchregtof && ptrhit->IsNoise()){
           ptrhit->clearstatus(AMSDBc::AwayTOF);
        }
        else{
           ptrhit->setstatus(AMSDBc::AwayTOF);
        }
        ptrhit=ptrhit->next();
      }

      return 0;

}






 void AMSTrRecHit::_addnext(AMSgSen * pSen, integer status, integer layer, number cofgx, number cofgy, AMSTrCluster *x,
                            AMSTrCluster * y, const AMSPoint & hit,
                            const AMSPoint & ehit){
    number s1=0,s2=0;
    number ss1,ss2;
    integer GOOD=1;
    if(x){
      s1=x->getVal();
      x->setstatus(AMSDBc::USED);
      ss1=x->getsigma();
      if(s1<TRFITFFKEY.ThrClA[0] || (ss1 && s1/ss1 < TRFITFFKEY.ThrClR[0])){
       GOOD=0;
      }
    }
    if(y){
      s2=y->getVal();
      y->setstatus(AMSDBc::USED);
      ss2=y->getsigma();
      if(s2<TRFITFFKEY.ThrClA[1] || (ss2 && s2/ss2 < TRFITFFKEY.ThrClR[1])){
       GOOD=0;
      }
    }
    geant bfield[3], ghit[3];
    for (int j=0;j<3;j++) { ghit[j] = (float)hit[j]; }
    GUFLD(ghit, bfield);
    AMSlink * ptr = AMSEvent::gethead()->addnext(AMSID("AMSTrRecHit",layer-1),
    new     AMSTrRecHit(pSen, status,layer,cofgx,cofgy,x,y,hit,ehit,s1+s2,(s1-s2)/(s1+s2),(AMSPoint)bfield));
//    cout <<"cfgx  "<<cofgx<<" "<<ptr<<" "<<layer<<" "<<cofgy<<endl;
    if(ptr && GOOD)ptr->setstatus(AMSDBc::GOOD);  
}


void AMSTrRecHit::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (IOPA.WriteAll%10==0 && checkstatus(AMSDBc::USED))
                 || (IOPA.WriteAll%10==2 && !checkstatus(AMSDBc::AwayTOF));

  if(AMSTrRecHit::Out(flag) ){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
// Fill the ntuple 
 TrRecHitNtuple02* THN = AMSJob::gethead()->getntuple()->Get_trrh02();
  if (THN->Ntrrh>=root::MAXTRRH02) return;

    if(_Xcl)THN->pX[THN->Ntrrh]=_Xcl->getpos();
    else THN->pX[THN->Ntrrh]=-1;
    THN->pY[THN->Ntrrh]=_Ycl->getpos();
    int pat;
    pat=1;
    if(AMSTrCluster::Out(IOPA.WriteAll%10==1)){
      // Writeall
      for(int i=0;i<pat;i++){
          AMSContainer *pc=AMSEvent::gethead()->getC("AMSTrCluster",i);
           #ifdef __AMSDEBUG__
            assert(pc != NULL);
           #endif
           THN->pY[THN->Ntrrh]+=pc->getnelem();
      }
    }                                                        
    else if (AMSTrCluster::Out(IOPA.WriteAll%10==0)){
      //Write only USED hits
      for(int i=0;i<pat;i++){
        AMSTrCluster *ptr=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",i);
        while(ptr && ptr->checkstatus(AMSDBc::USED) ){
          THN->pY[THN->Ntrrh]++;
          ptr=ptr->next();
        }
      }
    }
    else if (AMSTrCluster::Out(IOPA.WriteAll%10==2)){
      //Write only hits consistent with TOF
      for(int i=0;i<pat;i++){
        AMSTrCluster *ptr=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",i);
        while(ptr && !(ptr->checkstatus(AMSDBc::AwayTOF)) ){
          THN->pY[THN->Ntrrh]++;
          ptr=ptr->next();
        }
      }
    }
    else return;
  
    if(!checkstatus(AMSDBc::FalseX) && !checkstatus(AMSDBc::FalseTOFX) &&
        ((_Xcl->getid()).getlayer() != _Layer) || 
       ((_Ycl->getid()).getlayer() != _Layer) ){
      cerr << "AMSTrRecHit-S-Logic Error "<<(_Xcl->getid()).getlayer()<<" "<<
        (_Ycl->getid()).getlayer()<<" "<<_Layer<<endl;
    }
    THN->Status[THN->Ntrrh]=_status;
    THN->Layer[THN->Ntrrh]=_Layer;
    int i;
    for(i=0;i<3;i++)THN->Hit[THN->Ntrrh][i]=_Hit[i];
    for(i=0;i<3;i++)THN->EHit[THN->Ntrrh][i]=_EHit[i];
    THN->Sum[THN->Ntrrh]=_Sum;
    THN->DifoSum[THN->Ntrrh]=_DifoSum;
//    cout <<" cofgx "<<_cofgx<<" "<<this<<" "<<_Layer<<" "<<_cofgy<<endl;
    THN->CofgX[THN->Ntrrh]=_cofgx;
    THN->CofgY[THN->Ntrrh]=_cofgy;
    for(i=0;i<3;i++)THN->Bfield[THN->Ntrrh][i]=_Bfield[i];
    THN->Ntrrh++;
  }
}

void AMSTrRecHit::_copyEl(){
#ifdef __WRITEROOT__
    if(PointerNotSet())return;
    TrRecHitR & trr=AMSJob::gethead()->getntuple()->Get_evroot02()->TrRecHit(_vpos);
    if (_Xcl) trr.fTrClusterX= _Xcl->GetClonePointer();
    else trr.fTrClusterX=-1;
    if (_Ycl) trr.fTrClusterY= _Ycl->GetClonePointer();
    else trr.fTrClusterY=-1;
#endif
}


void AMSTrRecHit::print(){
for(int i=0;i<TKDBc::nlay();i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSTrRecHit",i);
 if(p)p->printC(cout);
}
}

geant AMSTrTrack::_Time=0;
time_t AMSTrTrack::__Time=0;
geant AMSTrTrack::_TimeLimit=0;



integer AMSTrTrack::build(integer refit){

  {
    int nrh=0;
    for(int i=0;i<TKDBc::nlay();i++){
     nrh+= (AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
    }
    if(nrh>=min(TRFITFFKEY.MaxTrRecHitsPerLayer*TKDBc::nlay(),root::MAXTRRH02)){
    AMSlink *ptr=AMSEvent::gethead()->getheadC("TriggerLVL3",0);
    TriggerLVL302 *ptr302=dynamic_cast<TriggerLVL302*>(ptr);
//      cout <<" nrh "<<nrh<<" "<<ptr302->skip()<<" "<<ptr302->MainTrigger()<<endl;
//      if((!ptr3 || ptr3->skip()) && (!ptr302 || ptr302->skip())){
      if(!ptr302 || ptr302->skip()){ //tempor
       AMSEvent::gethead()->seterror();
       return 0;
      }
    }
  }

  integer NTrackFound=-1;
  // pattern recognition + fit
  if(refit){
    // Cleanup all track containers
    int i;
    for(i=0;;i++){
      AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrTrack",i);
      if(pctr)pctr->eraseC();
      else break ;
    }
  } 
  _RefitIsNeeded=0;
  _Start();
  // Add test here
   
  { 
    int xs=0; 
    for (int kk=0;kk<TKDBc::nlay();kk++){
    AMSTrRecHit * phit=AMSTrRecHit::gethead(kk);
    if(phit)xs++;
  }
    if(xs>3)AMSEvent::gethead()->addnext(AMSID("Test",0),new Test());
  }

  for (pat=0;pat<TKDBc::npat();pat++){
    if(!TKDBc::patallow(pat) && !_MarginPatternsNeeded)continue;
    if(TKDBc::patallowFalseX(pat)){
      int fp=TKDBc::patpoints(pat)-1;    
      // Try to make StrLine Fit
      integer first=TKDBc::patconf(pat,0)-1;
      integer second=TKDBc::patconf(pat,fp)-1;
      phit[0]=AMSTrRecHit::gethead(first);
      while( phit[0]){
       if(phit[0]->Good()){
       phit[fp]=AMSTrRecHit::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good()){
        par[0][0]=(phit[fp]-> getHit()[0]-phit[0]-> getHit()[0])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[0][1]=phit[0]-> getHit()[0]-par[0][0]*phit[0]-> getHit()[2];
        par[0][2]=sqrt(1+par[0][0]*par[0][0]);
        par[1][0]=(phit[fp]-> getHit()[1]-phit[0]-> getHit()[1])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[1][1]=phit[0]-> getHit()[1]-par[1][0]*phit[0]-> getHit()[2];
        par[1][2]=sqrt(1+par[1][0]*par[1][0]);
        if(NTrackFound<0)NTrackFound=0;
        // Search for others
        //  add try due to icc bug
        try{
        integer npfound=_TrSearcher(1);
        if(npfound){
           NTrackFound++;
         if(TKDBc::patallow(pat)){
            // we don't want three points any more
            _MarginPatternsNeeded=0;
         }
         goto out;
        }
        }
        catch (...){
         throw;
        }
        }         
        phit[fp]=phit[fp]->next();
       }
       }  
out:
       phit[0]=phit[0]->next();
      }
      
    }
  }
return NTrackFound;
}


integer AMSTrTrack::buildPathIntegral(integer refit){

  {
    int nrh=0;
    for(int i=0;i<TKDBc::nlay();i++){
     nrh+= (AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
    }
    if(nrh>=min(TRFITFFKEY.MaxTrRecHitsPerLayer*TKDBc::nlay(),root::MAXTRRH02)){
    AMSlink *ptr=AMSEvent::gethead()->getheadC("TriggerLVL3",0);
    TriggerLVL302 *ptr302=dynamic_cast<TriggerLVL302*>(ptr);
//      cout <<" nrh "<<nrh<<" "<<ptr302->skip()<<" "<<ptr302->MainTrigger()<<endl;
//      if((!ptr3 || ptr3->skip()) && (!ptr302 || ptr302->skip())){
      if(!ptr302 || ptr302->skip()){ //tempor
       AMSEvent::gethead()->seterror();
       return 0;
      }
    }
  }

  integer NTrackFound=0;
  // pattern recognition + fit
  if(refit){
    // Cleanup all track containers
    int i;
    for(i=0;;i++){
      AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrTrack",i);
      if(pctr)pctr->eraseC();
      else break ;
    }
  } 
  _RefitIsNeeded=0;
  _Start();
  // Add test here
   
  { 
    int xs=0; 
    for (int kk=0;kk<TKDBc::nlay();kk++){
            AMSTrRecHit * phit=AMSTrRecHit::gethead(kk);
            if(phit)xs++;
    }
    if(xs>3)AMSEvent::gethead()->addnext(AMSID("Test",0),new Test());
  }

  static AMSTrRecHit * phit2;
  static AMSTrRecHit * phit1;
  static AMSTrRecHit * phit0;
  static AMSTrRecHit * phitl;
  number gers=0.03;
  AMSPoint hit_err = AMSPoint(gers,gers,gers);

  // Count rec hits on each layer
  int nelem[trconst::maxlay];
  for (int i=0; i<trconst::maxlay; i++) { 
     nelem[i] = (AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
  }
 
  while (1) {

      if (NTrackFound>=Vtxconst::maxtr) break;

      AMSTrTrack* ptrack = NULL;
	AMSTrTrack ptest;
      number minchi2 = TRFITFFKEY.Chi2WithoutMS;

      for (int pat=0;pat<TKDBc::npat();pat++){
            if(!TKDBc::patallow(pat) && !_MarginPatternsNeeded)continue;
            if(!TKDBc::patallowFalseX(pat)) continue;

            int fp=TKDBc::patpoints(pat)-1;    

            // Skip pattern if any layer has zero or too many points on it
            for (int i=0; i<=fp; i++) {
              if (AMSTrRecHit::firstgood(pat,i)==NULL) goto next_pattern;
              int il = TKDBc::patconf(pat,i)-1;
              if (nelem[il]>TRFITFFKEY.MaxTrRecHitsPerLayer) goto next_pattern;
            }

            ptest._NHits = 0;
            ptest._Pattern = pat;
            for (phit2=AMSTrRecHit::firstgood(pat,fp);
                              phit2; phit2=phit2->nextgood()){
               AMSPoint x2 = phit2->getHit();
               for (phit0=AMSTrRecHit::firstgood(pat,fp-2);
                              phit0; phit0=phit0->nextgood()){

                  AMSPoint x0 = phit0->getHit();
	            number par[2][3];
                  par[0][0]=(x2[0] - x0[0])/(x2[2] - x0[2]);
                  par[0][1]=x0[0] - par[0][0]*x0[2];
                  par[0][2]=sqrt(1+par[0][0]*par[0][0]);
                  par[1][0]=(x2[1] - x0[1])/(x2[2] - x0[2]);
                  par[1][1]=x0[1] - par[1][0]*x0[2];
                  par[1][2]=sqrt(1+par[1][0]*par[1][0]);
       
                  for (phit1=AMSTrRecHit::firstgood(pat,fp-1);
                              phit1; phit1=phit1->nextgood_path(par)){
                        ptest._Pthit[2] = phit2;
                        ptest._Pthit[1] = phit1;
                        ptest._Pthit[0] = phit0;
                        ptest._NHits = 3;
                        ptest.SimpleFit(hit_err);
	                  if (ptest._Chi2WithoutMS>TRFITFFKEY.Chi2WithoutMS) continue;

                        for (int j=fp-3; j>=0; j--) {
                          ptest._NHits++;
                          for (int i=ptest._NHits-1; i>=1; i--) {
                              ptest._Pthit[i] = ptest._Pthit[i-1];
                          }
                          AMSTrRecHit* pbest = NULL;
                          double chi2tmp = TRFITFFKEY.Chi2WithoutMS;
                          for (phitl=AMSTrRecHit::firstgood(pat,j);
                                   phitl; phitl=phitl->nextgood_path(par)){
                              ptest._Pthit[0] = phitl;
                              ptest.SimpleFit(hit_err);
	                        if (ptest._Chi2WithoutMS<chi2tmp) {
                                    chi2tmp = ptest._Chi2WithoutMS;
                                    pbest = phitl;
                              }
                          }
                          if (!pbest) goto next_3;
                          ptest._Pthit[0] = pbest;

                          if (_NoMoreTime()) { 
                              remove_track(ptrack); 
                              if (AMSEvent::debug){
                                cout << " buildPathIntegral Cpulimit Exceeded!!!! " << endl;
                              }
                              throw AMSTrTrackError(" Cpulimit Exceeded ");
                              return NTrackFound;
	                    }

                          // Build final track
	                    if (j==0){
                              ptest.SimpleFit(hit_err);
                              if (ptest._Chi2WithoutMS<minchi2
                                  && ptest.Fit(0)<TRFITFFKEY.Chi2FastFit &&
                                  ptest.TOFOK()) {
                                    minchi2 = ptest._Chi2WithoutMS;
                                    if (ptrack) delete ptrack;
                                    ptrack = ptest.CloneIt();
                              }
	                    }  
	                  }  
next_3:
                  continue;
                  }
               }
            }
        

next_pattern:
            // Check next pattern only if no tracks has been found
            if (ptrack) break;

      }

      // Best track hopefully found; _addnextR ptrack if succesful
      if (ptrack){ 
          // Get pattern and hits
          int pat = ptrack->getpattern();
          int nhits = ptrack->getnhits();
          for (int i=0;i<nhits;i++){
                  phit[i] = ptrack->getphit(i);
          }
          // Fit track; add it to the container list
          ptrack->Fit(0);
          if (_addnext(pat, nhits, phit)) {
            NTrackFound++;
            if(AMSEvent::debug){
                  cout << " AMSTrTrack::buildPathIntegral track>>>>>> " << endl;
                  cout << "### Run " << AMSEvent::gethead()->getrun();
                  cout << " Event " << AMSEvent::gethead()->getEvent() << endl;
                  cout << "# hits " << nhits;
                  cout << " Rigidity: " << ptrack->getrid() << endl;
                  cout << " chi2/ndof: " << ptrack->getchi2withoutMS() << endl;
                  for (int i=0;i<ptrack->getnhits();i++){
                        cout << "        " << ptrack->getphit(i)->getHit()[0];
                        cout << ", " << ptrack->getphit(i)->getHit()[1];
                        cout << ", " << ptrack->getphit(i)->getHit()[2];
                        cout << endl;
                  }
            }
            remove_track(ptrack); 
          } else {
            // Get out if we awere not succesful (but why?)
            remove_track(ptrack); 
            return NTrackFound;
          }
      } else {

            // Get out if nothing has been found
            return NTrackFound;

      }

  }
  return NTrackFound;

}


integer AMSTrTrack::buildWeak(integer refit){
  {
    int nrh=0;
    for(int i=0;i<TKDBc::nlay();i++){
     nrh+= (AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
    }
    if(nrh>=min(TRFITFFKEY.MaxTrRecHitsPerLayer*TKDBc::nlay(),root::MAXTRRH02)){
    AMSlink *ptr=AMSEvent::gethead()->getheadC("TriggerLVL3",0);
    TriggerLVL302 *ptr302=dynamic_cast<TriggerLVL302*>(ptr);
      if(!ptr302 || ptr302->skip()){
       AMSEvent::gethead()->seterror();
       return 0;
      }
    }
  }

  integer NTrackFound=-1;
  // pattern recognition + fit
  if(refit){
   return NTrackFound;
  } 
  _RefitIsNeeded=0;
  _Start();
   

  for (pat=0;pat<TKDBc::npat();pat++){
    // Only 4 points patterns used
      int fp=TKDBc::patpoints(pat)-1;    
    if(TKDBc::patallow(pat) && fp<=TKDBc::nlay()-3){
      // Try to make StrLine Fit
      integer first=TKDBc::patconf(pat,0)-1;
      integer second=TKDBc::patconf(pat,fp)-1;
      phit[0]=AMSTrRecHit::gethead(first);
      while( phit[0]){
       if(phit[0]->Good() && phit[0]->checkstatus(AMSDBc::WEAK)==0){
       phit[fp]=AMSTrRecHit::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good() && phit[fp]->checkstatus(AMSDBc::WEAK)==0){
        par[0][0]=(phit[fp]-> getHit()[0]-phit[0]-> getHit()[0])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[0][1]=phit[0]-> getHit()[0]-par[0][0]*phit[0]-> getHit()[2];
        par[1][0]=(phit[fp]-> getHit()[1]-phit[0]-> getHit()[1])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[1][1]=phit[0]-> getHit()[1]-par[1][0]*phit[0]-> getHit()[2];
        // Search for others
        if(NTrackFound<0)NTrackFound=0;
        // Search for others
        //  add try due to icc bug
        try{
        integer npfound=_TrSearcher(1);
        if(npfound){
           NTrackFound++;
         goto out;
        }
        }
        catch (...){
         throw;
        }

        }         
        phit[fp]=phit[fp]->next();
       }
       }  
out:
       phit[0]=phit[0]->next();
      }
      
    }
  }
return NTrackFound;
}


integer AMSTrTrack::buildWeakPathIntegral(integer refit){
  {
    int nrh=0;
    for(int i=0;i<TKDBc::nlay();i++){
     nrh+= (AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
    }
    if(nrh>=min(TRFITFFKEY.MaxTrRecHitsPerLayer*TKDBc::nlay(),root::MAXTRRH02)){
    AMSlink *ptr=AMSEvent::gethead()->getheadC("TriggerLVL3",0);
    TriggerLVL302 *ptr302=dynamic_cast<TriggerLVL302*>(ptr);
      if(!ptr302 || ptr302->skip()){
       AMSEvent::gethead()->seterror();
       return 0;
      }
    }
  }

  integer NTrackFound=0;
  // pattern recognition + fit
  if(refit){
   return NTrackFound;
  } 
  _RefitIsNeeded=0;
  _Start();
   
  static AMSTrRecHit * phit2;
  static AMSTrRecHit * phit1;
  static AMSTrRecHit * phit0;
  static AMSTrRecHit * phitl;
  number gers=0.03;
  AMSPoint hit_err = AMSPoint(gers,gers,gers);
      
  // Count rec hits on each layer
  int nelem[trconst::maxlay];
  for (int i=0; i<trconst::maxlay; i++) { 
     nelem[i] = (AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
  }
 
  while (1) {

      if (NTrackFound>=Vtxconst::maxtr) break;

      AMSTrTrack* ptrack = NULL;
	AMSTrTrack ptest;
      number minchi2 = TRFITFFKEY.Chi2WithoutMS;

      for (int pat=0;pat<TKDBc::npat();pat++){
            if (!TKDBc::patallow(pat)) continue;

            int fp=TKDBc::patpoints(pat)-1;    

            // Skip pattern if any layer has zero or too many points on it
            for (int i=0; i<=fp; i++) {
              if (AMSTrRecHit::firstgood_WEAK(pat,i)==NULL) goto next_pattern;
              int il = TKDBc::patconf(pat,i)-1;
              if (nelem[il]>TRFITFFKEY.MaxTrRecHitsPerLayer) goto next_pattern;
            }

            ptest._NHits = 0;
            ptest._Pattern = pat;
            for (phit2=AMSTrRecHit::firstgood_WEAK(pat,fp);
                              phit2; phit2=phit2->nextgood_WEAK()){
               AMSPoint x2 = phit2->getHit();
               for (phit0=AMSTrRecHit::firstgood_WEAK(pat,fp-2);
                              phit0; phit0=phit0->nextgood_WEAK()){

                  AMSPoint x0 = phit0->getHit();
	            number par[2][3];
                  par[0][0]=(x2[0] - x0[0])/(x2[2] - x0[2]);
                  par[0][1]=x0[0] - par[0][0]*x0[2];
                  par[0][2]=sqrt(1+par[0][0]*par[0][0]);
                  par[1][0]=(x2[1] - x0[1])/(x2[2] - x0[2]);
                  par[1][1]=x0[1] - par[1][0]*x0[2];
                  par[1][2]=sqrt(1+par[1][0]*par[1][0]);
       
                  for (phit1=AMSTrRecHit::firstgood_WEAK(pat,fp-1);
                              phit1; phit1=phit1->nextgood_WEAK_path(par)){
                        ptest._Pthit[2] = phit2;
                        ptest._Pthit[1] = phit1;
                        ptest._Pthit[0] = phit0;
                        ptest._NHits = 3;
                        ptest.SimpleFit(hit_err);
	                  if (ptest._Chi2WithoutMS>TRFITFFKEY.Chi2WithoutMS) continue;

                        for (int j=fp-3; j>=0; j--) {
                          ptest._NHits++;
                          for (int i=ptest._NHits-1; i>=1; i--) {
                              ptest._Pthit[i] = ptest._Pthit[i-1];
                          }
                          AMSTrRecHit* pbest = NULL;
                          double chi2tmp = TRFITFFKEY.Chi2WithoutMS;
                          for (phitl=AMSTrRecHit::firstgood_WEAK(pat,j);
                                   phitl; phitl=phitl->nextgood_WEAK_path(par)){
                              ptest._Pthit[0] = phitl;
                              ptest.SimpleFit(hit_err);
	                        if (ptest._Chi2WithoutMS<chi2tmp) {
                                    chi2tmp = ptest._Chi2WithoutMS;
                                    pbest = phitl;
                              }
                          }
                          if (!pbest) goto next_3;
                          ptest._Pthit[0] = pbest;

                          if (_NoMoreTime()) { 
                              remove_track(ptrack); 
                              if (AMSEvent::debug){
                                cout << " buildWEAKPathIntegral Cpulimit Exceeded!!!! " << endl;
                              }
                              throw AMSTrTrackError(" Cpulimit Exceeded ");
                              return NTrackFound;
	                    }

                          // Build final track
	                    if (j==0){
                              ptest.SimpleFit(hit_err);
                              if (ptest._Chi2WithoutMS<minchi2
                                  && ptest.Fit(0)<TRFITFFKEY.Chi2FastFit &&
                                  ptest.TOFOK()) {
                                    minchi2 = ptest._Chi2WithoutMS;
                                    if (ptrack) delete ptrack;
                                    ptrack = ptest.CloneIt();
                              }
	                    }  
	                  }  
next_3:
                  continue;
                  }
               }
            }
        

next_pattern:
            // Check next pattern only if no tracks has been found
            if (ptrack) break;

      }

      // Best track hopefully found; _addnextR ptrack if succesful
      if (ptrack){ 
          // Get pattern and hits
          int pat = ptrack->getpattern();
          int nhits = ptrack->getnhits();
          for (int i=0;i<nhits;i++){
                  phit[i] = ptrack->getphit(i);
          }
          // Fit track; add it to the container list
          ptrack->Fit(0);
          if (_addnext(pat, nhits, phit)) {
            NTrackFound++;
            if(AMSEvent::debug){
                  cout << " AMSTrTrack::buildWEAKPathIntegral track>>>>>> " << endl;
                  cout << "### Run " << AMSEvent::gethead()->getrun();
                  cout << " Event " << AMSEvent::gethead()->getEvent() << endl;
                  cout << "# hits " << nhits;
                  cout << " Rigidity: " << ptrack->getrid() << endl;
                  cout << " chi2/ndof: " << ptrack->getchi2withoutMS() << endl;
                  for (int i=0;i<ptrack->getnhits();i++){
                        cout << "        " << ptrack->getphit(i)->getHit()[0];
                        cout << ", " << ptrack->getphit(i)->getHit()[1];
                        cout << ", " << ptrack->getphit(i)->getHit()[2];
                        cout << endl;
                  }
            }
            remove_track(ptrack); 
          } else {
            // Get out if we awere not succesful (but why?)
            remove_track(ptrack); 
            return NTrackFound;
          }
      } else {

            // Get out if nothing has been found
            return NTrackFound;

      }

  }
  return NTrackFound;

}

integer AMSTrTrack::buildFalseX(integer nptmin){
  {
    int nrh=0;
    for(int i=0;i<TKDBc::nlay();i++){
     nrh+= (AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
    }
    if(nrh>=min(TRFITFFKEY.MaxTrRecHitsPerLayer*TKDBc::nlay(),root::MAXTRRH02)){
    AMSlink *ptr=AMSEvent::gethead()->getheadC("TriggerLVL3",0);
    TriggerLVL302 *ptr302=dynamic_cast<TriggerLVL302*>(ptr);
      if(!ptr302 || ptr302->skip()){
       AMSEvent::gethead()->seterror();
       return 0;
      }
    }
  }

  integer NTrackFound=-1;
  // pattern recognition + fit
  _RefitIsNeeded=0;
  _Start();
 AMSgObj::BookTimer.start("TrFalseX");

  for (pat=0;pat<TKDBc::npat();pat++){
    if(TKDBc::patpoints(pat)<=nptmin && TKDBc::patallowFalseX(pat)){
      int fp=TKDBc::patpoints(pat)-1;    
      // Try to make StrLine Fit
      integer first=TKDBc::patconf(pat,0)-1;
      integer second=TKDBc::patconf(pat,fp)-1;
      phit[0]=AMSTrRecHit::gethead(first);
      while( phit[0]){
       if(phit[0]->Good() && phit[0]->checkstatus(AMSDBc::WEAK)==0 && phit[0]->checkstatus(AMSDBc::FalseX)==0){
       phit[fp]=AMSTrRecHit::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good() && phit[fp]->checkstatus(AMSDBc::WEAK)==0 && phit[fp]->checkstatus(AMSDBc::FalseX)==0){
        par[0][0]=(phit[fp]-> getHit()[0]-phit[0]-> getHit()[0])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[0][1]=phit[0]-> getHit()[0]-par[0][0]*phit[0]-> getHit()[2];
        par[0][2]=sqrt(1+par[0][0]*par[0][0]);
        par[1][0]=(phit[fp]-> getHit()[1]-phit[0]-> getHit()[1])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[1][1]=phit[0]-> getHit()[1]-par[1][0]*phit[0]-> getHit()[2];
        par[1][2]=sqrt(1+par[1][0]*par[1][0]);
        // Search for others
        if(NTrackFound<0)NTrackFound=0;

        //  add try due to icc bug
        try{
        integer npfound=_TrSearcherFalseX(1);
        if(npfound){
           if(npfound>0)NTrackFound++;
           goto out;
        }
        }
        catch (...){
         throw;
        }

        }         
        phit[fp]=phit[fp]->next();
       }
       }  
out:
       phit[0]=phit[0]->next();
      }
      
    }
  }
        AMSgObj::BookTimer.stop("TrFalseX");
return NTrackFound;
}


/*
integer AMSTrTrack::Distance(number par[2][3], AMSTrRecHit *ptr){
const integer freq=10;
static integer trig=0;
trig=(trig+1)%freq;
           if(trig==0 && _NoMoreTime()){
            throw AMSTrTrackError(" Cpulimit Exceeded ");
           }
   return fabs(par[0][1]+par[0][0]*ptr->getHit()[2]-ptr->getHit()[0]) > TRFITFFKEY.SearchRegStrLine*par[0][2] ||
          fabs(par[1][1]+par[1][0]*ptr->getHit()[2]-ptr->getHit()[1])
           > TRFITFFKEY.SearchRegCircle*par[1][2];
}


*/
integer AMSTrTrack::_addnext(integer pat, integer nhit, AMSTrRecHit* pthit[trconst::maxlay]){

#ifdef __UPOOL__
    AMSTrTrack track(pat, nhit ,pthit);
    AMSTrTrack *ptrack=   &track;
#else
    AMSTrTrack *ptrack=   new AMSTrTrack(pat, nhit ,pthit);
#endif

    number gers=0.03;
    if(TRFITFFKEY.OldTracking)ptrack->VerySimpleFit(AMSPoint(gers,gers,gers));
    else ptrack->SimpleFit(AMSPoint(gers,gers,gers));
    if(ptrack->_Chi2StrLine< TRFITFFKEY.Chi2StrLine){
     if(ptrack->_Chi2WithoutMS< TRFITFFKEY.Chi2WithoutMS && 
      fabs(ptrack->_RigidityWithoutMS)>TRFITFFKEY.RidgidityMin ){
       if( (  (ptrack->Fit(0) < 
            TRFITFFKEY.Chi2FastFit)) && ptrack->TOFOK() ){
         // permanently add;
#ifdef __UPOOL__
          ptrack=new AMSTrTrack(track);
#endif
          _addnextR(ptrack, pat, nhit, pthit);
          return 1;
       }
     }
    }
#ifndef __UPOOL__
    delete ptrack;  
#endif   
    return 0;
}




void AMSTrTrack::_addnextR(AMSTrTrack *ptrack, integer pat, integer nhit, AMSTrRecHit* pthit[trconst::maxlay]){

         int i;
         // Mark hits as USED
         for( i=0;i<nhit;i++){
           if(pthit[i]->checkstatus(AMSDBc::USED))
            pthit[i]->setstatus(AMSDBc::AMBIG);
           else pthit[i]->setstatus(AMSDBc::USED);
           if(pthit[i]->checkstatus(AMSDBc::FalseX))
            ptrack->setstatus(AMSDBc::FalseX);
           if(pthit[i]->checkstatus(AMSDBc::WEAK))
            ptrack->setstatus(AMSDBc::WEAK);
           if(pthit[i]->checkstatus(AMSDBc::FalseTOFX))
            ptrack->setstatus(AMSDBc::FalseTOFX);
           /*
           // Mark also hits sharing the same cluster in the bending plane
           AMSTrCluster* py = pthit[i]->getClusterP(1);
           if (py){
             AMSTrRecHit* paux = AMSTrRecHit::gethead(pthit[i]->getLayer()-1);
             while (paux) {
                  if (py==paux->getClusterP(1)) paux->setstatus(AMSDBc::S_AMBIG);
                  paux = paux->next();
             }
           }
           */
         }

          number dc[2];
          dc[0]=fabs(sin(ptrack->gettheta())*cos(ptrack->getphi()));
          dc[1]=fabs(sin(ptrack->gettheta())*sin(ptrack->getphi()));
          int n;
          for(n=0;n<2;n++){
           if(dc[n] > TRFITFFKEY.MinRefitCos[n]){
             for( i=0;i<nhit;i++){
              AMSTrCluster *pcl= pthit[i]->getClusterP(n);
              if(pcl)pcl->setstatus(AMSDBc::REFITTED);
              _RefitIsNeeded++;
             }
           }
          }
         // Make sure that the path integral fit is always tried
         ptrack->Fit(5,2);
         // permanently add;
          AMSEvent::gethead()->addnext(AMSID("AMSTrTrack",0),ptrack);
}





integer AMSTrTrack::_addnextFalseX(integer pat, integer nhit, AMSTrRecHit* pthit[]){
//AMSgObj::BookTimer.start("TrFalseX");
#ifdef __UPOOL__
    AMSTrTrack track(pat, nhit ,pthit);
    AMSTrTrack *ptrack=   &track;
#else
    AMSTrTrack *ptrack=   new AMSTrTrack(pat, nhit ,pthit);
#endif

    number gers=0.03;
    if(TRFITFFKEY.OldTracking)ptrack->VerySimpleFit(AMSPoint(gers,gers,gers));
    else ptrack->SimpleFit(AMSPoint(gers,gers,gers));
    if(ptrack->_Chi2StrLine< TRFITFFKEY.Chi2StrLine){
     if(ptrack->_Chi2WithoutMS< TRFITFFKEY.Chi2WithoutMS && 
      fabs(ptrack->_RigidityWithoutMS)>TRFITFFKEY.RidgidityMin ){
      if( (  (ptrack->Fit(0) < TRFITFFKEY.Chi2FastFit))
           && ptrack->TOFOK()){
        // Here we should add at least one point and fit 
        // First determine which planes are missed and interpolate to them,
        // find corresponding sensor ID
        // Then make additional  hit(s) and include them into fitting 
        // finally change pattern according to new track and
        // add "FalseX" bit into track status word

          integer pointfound=0;
          for(int i=nhit;i<TKDBc::nlay();i++){
            integer ladder=8;
            integer sensor=5;
             AMSTrIdGeom id(TKDBc::patmiss(pat,i),ladder,sensor,0,0);
             AMSgvolume* p= AMSJob::gethead()->getgeomvolume(id.crgid());
            if(p){
              AMSPoint  P1;
                { 
                  AMSDir pntdir(0,0,1.);
                  AMSPoint pntplane(p->getcooA(0),p->getcooA(1),p->getcooA(2));
                  number theta,phi,length;
                  ptrack->interpolate(pntplane,pntdir,P1,theta,phi,length); 
                }
              
              AMSTrCluster * py=
                (AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0); 
              integer yfound=0;  
              while(py){
                AMSTrIdSoft idy=py->getid();
                if(idy.getlayer()==id.getlayer()){
                  if((P1[0]<0 && idy.gethalf()==0) || (P1[0]>0 && idy.gethalf()==1))yfound++;
                }
                py=py->next();
              }
              
              if(yfound){
                // Now 2nd pass : find corresponding ladder & sensor
                AMSgSen * pls=0;
                AMSPoint PS(p->getpar(0),p->getpar(1),p->getpar(2));
                if(id.FindAtt(P1,PS))pls=(AMSgSen*) AMSJob::gethead()->getgeomvolume(id.crgid());
                if(pls){
                  // Here Search for corresponding y-hit in the container 
                  AMSDir pntdir(pls->getnrmA(0,2),pls->getnrmA(1,2),pls->getnrmA(2,2));
                  AMSPoint pntplane(pls->getcooA(0),pls->getcooA(1),pls->getcooA(2));
                  number theta,phi,length;
                  ptrack->interpolate(pntplane,pntdir,P1,theta,phi,length);
                  // Now check for the corr y-hit
                  
                  
                  py=
                    (AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0); 
                  while(py){
                    AMSTrIdSoft idy=py->getid();
                    if(idy.getlayer()==id.getlayer() && abs(idy.getdrp()-id.getladder())<1){
//                      if((P1[0]<0 && idy.gethalf()==0) || (P1[0]>0 && idy.gethalf()==1)){   // This is sometimes  wrong and somewhen
//                                                                                            //   should be replaced
//                                                                                            // on something more robust 
//                                                                                            //  but I am somewhere lazy
                        if(idy.gethalf()==id.gethalf()){
                        //  Create False RawHit and put it in the corr container 
                        AMSPoint loc=pls->gl2loc(P1);
                        id.R2Gy(idy.getstrip());
                        AMSPoint hit=pls->str2pnt(loc[0]+PS[0],py->getcofg(&id)); 
                        AMSPoint Err(TRFITFFKEY.ResCutStrLine,
                                     TRFITFFKEY.ResCutStrLine,TRFITFFKEY.ResCutStrLine);
                        if((hit-P1).abs() < Err){
//                          cout <<id.getlayer()<<" "<<P1<<"  " <<hit<<" "<<idy.gethalf()<<endl;
                          AMSTrRecHit::_addnext(pls,AMSDBc::FalseX,id.getlayer(),-1,-1,0,py,hit,
                                                AMSPoint((number)TRCLFFKEY.ErrZ*2,py->getecofg(),(number)TRCLFFKEY.ErrZ));
                          pointfound++;
                        } 
                        
                      }
                    }
                    py=py->next();
                  }
                    if(pointfound>0){
#ifndef __UPOOL__
                      delete ptrack;  
#endif   
//                      AMSgObj::BookTimer.stop("TrFalseX");
                      return 1;
                    }
                }
              }
            }
#ifdef __AMSDEBUG__
            else  cerr <<" AMSTrTrack::_addnextFalseX-E-NoSensorsForPlaneFound "<<
                    TKDBc::patmiss(pat,i)<<endl;
#endif
            
          }

                    if(TRFITFFKEY.OldTracking && TKDBc::patallow(pat)){
#ifdef __UPOOL__
                      ptrack=new AMSTrTrack(track);
#endif
                      _addnextR(ptrack, pat, nhit, pthit);
//                      AMSgObj::BookTimer.stop("TrFalseX");
                      return -1;
                    }
                    else {
#ifndef __UPOOL__
                      delete ptrack;  
#endif   
//                      AMSgObj::BookTimer.stop("TrFalseX");
                      return 0;
                    }




      }
     }
    }
#ifndef __UPOOL__
    delete ptrack;  
#endif   
//    AMSgObj::BookTimer.stop("TrFalseX");
    return 0;
}





void AMSTrTrack::AdvancedFit(){
    if(TKDBc::patpoints(_Pattern)>3){
      Fit(1);
      Fit(2);
    }
    Fit(4);
    Fit(5);
}

integer AMSTrTrack::TOFOK(){
    if(TRFITFFKEY.UseTRD &&(TRFITFFKEY.UseTRD>1 || TKDBc::ambig(_Pattern) || 
                              checkstatus(AMSDBc::FalseX) ||
                              checkstatus(AMSDBc::FalseTOFX))){
    AMSContainer *pc=AMSEvent::gethead()->getC("AMSTRDTrack",0);
    if(pc){
      AMSTRDTrack * ptrd = (AMSTRDTrack*)pc->gethead();
      number SearchReg(4);
      integer trdf=0;
      while(ptrd){
       trdf++;
       number theta,phi,sleng;
       AMSDir s(ptrd->gettheta(),ptrd->getphi());
       if(s[2]!=0){
         number x=ptrd->getCooStr()[0]+s[0]/s[2]*(_P0[2]-ptrd->getCooStr()[2]);
         if(fabs(x-_P0[0])<SearchReg)return 1;       
       }
/*
       AMSPoint Res;
       interpolate(ptrd->getCooStr() ,AMSPoint(0,0,1), Res, theta, phi, sleng);
       if( ((ptrd->getCooStr()-Res)/ptrd->getECooStr()).abs()< SearchReg){
         return 1;
       }
*/
       ptrd=ptrd->next();
      }
      if(trdf)return 0;
    }
    }
    if (TRFITFFKEY.UseTOF && (TKDBc::ambig(_Pattern) || 
                              checkstatus(AMSDBc::FalseX) ||
                              checkstatus(AMSDBc::FalseTOFX))){
   // Cycle thru all TOF clusters;
   // at least UseTOF of them should be matched with the track
   integer i;
   integer matched=0;
   // Take cut from beta defs
   AMSPoint SearchReg(BETAFITFFKEY.SearchReg[0],BETAFITFFKEY.SearchReg[1],
   BETAFITFFKEY.SearchReg[2]);

   for(i=0;i<4;i++){
    AMSTOFCluster * phit = AMSTOFCluster::gethead(i);
    AMSPoint Res;
    number theta,phi,sleng;
    if(phit){
     interpolate(phit->getcoo() ,AMSPoint(0,0,1), Res, theta, phi, sleng);
    }
    while (phit){
     if( ((phit->getcoo()-Res)/phit->getecoo()).abs()< SearchReg){
     if(phit->getntof() < 3)matched+=1;  
     else matched+=10;
     }
     phit=phit->next();
    }  
   }
   switch (TRFITFFKEY.UseTOF){
   case 0:
    return 1;
   case 1:
    if(matched/10 <1 && matched%10 <1)return 0;
    break;
   case 2:
    if(matched/10 <1 || matched%10<1)return 0;
    break;
   case 3:
    if(matched/10 <2 && matched%10<2)return 0;
    break;
   case 4:
    if(matched/10 <2 || matched%10<2)return 0;
    break;
   }
   return 1;  
    }
  else return 1;
}

void AMSTrTrack::SimpleFit(){
 AMSPoint ehit(0.03,0.03,0.03);
 SimpleFit(ehit);
}

void AMSTrTrack::SimpleFit(AMSPoint ehit){

// Consistency check on the number of hits
  if (_NHits<3 || _NHits>trconst::maxlay) {
    _Chi2WithoutMS = FLT_MAX;
    _Chi2StrLine = FLT_MAX;
#ifdef __AMSDEBUG__
    cout << " In SimpleFit, nhits= " << _NHits << endl;
#endif
    return;
  }

// Get hit positions and uncertainties
// Scale errors (we will use sigmas in microns)
  AMSPoint hits[trconst::maxlay];
  AMSPoint b[trconst::maxlay];
  AMSPoint sigma[trconst::maxlay];
  for (int i=0;i<_NHits;i++){
    hits[i] = _Pthit[i]->getHit();
    b[i] = _Pthit[i]->getBfield();
    sigma[i] = ehit*1.e4;
#ifdef __AMSDEBUG__
//    cout << " i, hits, sigma: " << i << ",";
//    for (int j=0;j<3;j++){ cout << " " << hits[i][j];}
//    for (int j=0;j<3;j++){ cout << " " << 1.e-4*sigma[i][j];}
//    cout << endl;
//    cout << " i, bfield: " << i << ",";
//    for (int j=0;j<3;j++){ cout << " " << b[i][j];}
//    cout << endl;
#endif
  }

// Lenghts
  float len[trconst::maxlay];
  for (int i=0;i<_NHits;i++){
    if (i==0){
      len[i] = 0.;
    }
    else {
      len[i] = sqrt( (hits[i][0]-hits[i-1][0])*(hits[i][0]-hits[i-1][0])
                    +(hits[i][1]-hits[i-1][1])*(hits[i][1]-hits[i-1][1])
                    +(hits[i][2]-hits[i-1][2])*(hits[i][2]-hits[i-1][2]) );
    }
  }

// Calculate path integrals
  number PathIntegral_x[trconst::maxlay][3];
  number PathIntegral_u[trconst::maxlay][3];

  for (int j=0;j<3;j++){
        PathIntegral_x[0][j] = 0.;
        PathIntegral_u[0][j] = 0.;
  }

  for (int i=1;i<_NHits;i++){
     
    if (MAGSFFKEY.magstat>0) {

      number u[3], bave[3];
      for (int j=0;j<3;j++){
          u[j] = (hits[i][j]-hits[i-1][j])/len[i];
          bave[j] = (b[i-1][j]+b[i][j])/2;
      }

      PathIntegral_x[i][0] = (u[1]*b[i-1][2]-u[2]*b[i-1][1])/2;
      PathIntegral_x[i][1] = (u[2]*b[i-1][0]-u[0]*b[i-1][2])/2;
      PathIntegral_x[i][2] = (u[0]*b[i-1][1]-u[1]*b[i-1][0])/2;

      PathIntegral_u[i][0] = u[1]*bave[2]-u[2]*bave[1];
      PathIntegral_u[i][1] = u[2]*bave[0]-u[0]*bave[2];
      PathIntegral_u[i][2] = u[0]*bave[1]-u[1]*bave[0];

    } else {
      for (int j=0;j<3;j++){
        PathIntegral_x[i][j] = 0.;
        PathIntegral_u[i][j] = 0.;
      }

    }

  }

// F and G matrices
  number d[2*trconst::maxlay][5];
  for (int i=0;i<_NHits;i++) {
    int ix = i;
    int iy = i+_NHits;
    for (int j=0;j<5;j++) { d[ix][j] = 0; d[iy][j] = 0;}
    d[ix][0] = 1.;
    d[iy][1] = 1.;
    for (int k=0;k<=i;k++) {
	d[ix][2] += len[k];
	d[iy][3] += len[k];
      if (MAGSFFKEY.magstat>0) {
         for (int l=0;l<=k;l++) {
            if (l==k) {
                  d[ix][4] += len[k]*len[k]*PathIntegral_x[k][0];
                  d[iy][4] += len[k]*len[k]*PathIntegral_x[k][1];
            } else {
                  d[ix][4] += len[k]*len[l]*PathIntegral_u[l][0];
                  d[iy][4] += len[k]*len[l]*PathIntegral_u[l][1];
            }
	   }
      }
    }
  }

// F*S_x*x + G*S_y*y
  number dx[5];
  for (int j=0;j<5;j++) {
    dx[j] = 0.;
    for (int l=0;l<_NHits;l++) {
      dx[j] += d[l][j]/sigma[l][0]/sigma[l][0]*hits[l][0];
      dx[j] += d[l+_NHits][j]/sigma[l][1]/sigma[l][1]*hits[l][1];
    }
  }

// (F*S_x*F + G*S_y*G)
  
  int idim = 5;
  int idim_eff;
  if (MAGSFFKEY.magstat>0) {
      idim_eff = 5;
  } else {
      idim_eff = 4;
  }
  number Param[5];
  number Covariance[5][5];
  for (int j=0;j<5;j++) {
    for (int k=0;k<5;k++) {
      if (MAGSFFKEY.magstat>0) {
            Covariance[j][k] = 0.;
      } else if (k==4 && j==4) {
            Covariance[j][k] = 1.;
            continue;
      } else if (k==4 || j==4) {
            Covariance[j][k] = 0.;
            continue;
      } else {
            Covariance[j][k] = 0.;
      }
      for (int l=0;l<_NHits;l++) {
        Covariance[j][k] += d[l][j]/sigma[l][0]/sigma[l][0]*d[l][k];
        Covariance[j][k] += d[l+_NHits][j]/sigma[l][1]/sigma[l][1]*d[l+_NHits][k];
      } 
    }
  }
        
// (F*S_x*F + G*S_y*G)**{-1}
  int ifail;
  INVERTMATRIX((double*)Covariance, idim, idim, ifail);
  if (ifail) {
    _Chi2WithoutMS = FLT_MAX;
    _Chi2StrLine = FLT_MAX;
#ifdef __AMSDEBUG__
    cout << " What ?? Singular matrix!!" << endl;
#endif
    return;
  }

// Solution
 for (int k=0;k<idim_eff;k++) {
      Param[k] = 0.;
      for (int i=0;i<idim_eff;i++) {
            Param[k] += Covariance[k][i]*dx[i];
      }
  }
  
// Chi2 (xl and yl in microns, since sigmas are in microns too)
  _Chi2WithoutMS = 0.;
  _Chi2StrLine = 0.;
  for (int l=0;l<_NHits;l++) {
    number xl = hits[l][0]*1.e4;
    number yl = hits[l][1]*1.e4;
    for (int k=0;k<idim_eff;k++) {
      xl -= d[l][k]*Param[k]*1.e4;
      yl -= d[l+_NHits][k]*Param[k]*1.e4;
    }
    _Chi2WithoutMS += xl/sigma[l][0]/sigma[l][0]*xl + yl/sigma[l][1]/sigma[l][1]*yl;
    _Chi2StrLine += xl/sigma[l][0]/sigma[l][0]*xl;
  }

// Return Chi2/Ndof
  if (MAGSFFKEY.magstat <= 0) {
      _Chi2WithoutMS /= (2.*_NHits-4.);
      _Chi2StrLine /= (_NHits-2.);
      _RigidityWithoutMS = FLT_MAX;
  } else if (Param[4]!=0.0) {
      _Chi2WithoutMS /= (2.*_NHits-5.);
      _Chi2StrLine /= (_NHits-2.);
      _RigidityWithoutMS = 2.997E-4/Param[4];
  } else {
      _Chi2WithoutMS = FLT_MAX;
      _Chi2StrLine = FLT_MAX;
      _RigidityWithoutMS = FLT_MAX;
#ifdef __AMSDEBUG__
      cout << " What ?? Param=0!!" << endl;
#endif
      return;
  }

}


void AMSTrTrack::VerySimpleFit(AMSPoint ehit){

integer ifit=0;
integer npt=_NHits;
const integer maxhits=10;
assert(npt < maxhits && npt > 2);
geant x[maxhits];
geant y[maxhits];
geant wxy[maxhits];
geant z[maxhits];
geant ssz[maxhits];
geant ressz[maxhits];
geant resxy[2*maxhits];
geant spcor[maxhits];
number work[maxhits];
geant chixy;
geant chiz;
geant xmom,dip,phis,exmom;
integer iflag=0;
geant p0[3];
for (int i=0;i<npt;i++){
 z[i]=_Pthit[i]->getHit()[0];
 x[i]=_Pthit[i]->getHit()[1];
 y[i]=_Pthit[i]->getHit()[2];
 wxy[i]=(ehit[0]*ehit[0]+ehit[1]*ehit[1]);
 if(wxy[i]==0)wxy[i]=
 (_Pthit[i]->getEHit()[1] * _Pthit[i]->getEHit()[1]+
  _Pthit[i]->getEHit()[2] * _Pthit[i]->getEHit()[2]);
  wxy[i]=1/wxy[i];
 ssz[i]=ehit[0];
 if(ssz[i]==0)ssz[i]=_Pthit[i]->getEHit()[0];
 ssz[i]=1/ssz[i]; 
}

TRAFIT(ifit,x,y,wxy,z,ssz,npt,resxy,ressz,iflag,spcor,work,chixy,chiz,xmom,
       exmom,p0,dip,phis);
if(iflag/1000 == 0)_Chi2WithoutMS=chixy;
else _Chi2WithoutMS=FLT_MAX;
if(iflag%1000 ==0)_Chi2StrLine=chiz;
else _Chi2StrLine=FLT_MAX;
_RigidityWithoutMS=xmom;
}

number AMSTrTrack::Fit(integer fits, integer ipart){

  // fit =0  fit pattern
  // fit =1  fit 1st part if pat=0,1,2,3, ... etc  
  // fit =2  fit 2nd half if pat=0,1,2,3  ... etc + interpolate to beg of 1st
  // fit =3  Geanefit pattern
  // fit =4  fast fit with ims=0

// ! Changed
  // fit =5  Juans fit with ims=1


   // Create Proper Hit/Ehit things
   _crHit();

  // Protection from too low mass
    if(ipart==2)ipart=5;
    if(ipart==3)ipart=6;
  integer npt=_NHits;
  const integer maxhits=10;
  assert(npt < maxhits);
  static geant hits[maxhits][3];
  static geant sigma[maxhits][3];
  static geant normal[maxhits][3];
  static integer layer[maxhits];
  integer ialgo=1;
  geant out[9];
  integer i;
    integer fit =abs(fits);
    if(fit==3 )ialgo=TRFITFFKEY.UseGeaneFitting?3:5;      
    else if(fit==5)ialgo=4;
    for(i=0;i<_NHits;i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=fits<0?1:-1;
    }
  if(fit == 0 || fit==3 || fit==4 || fit==5){
    for(i=0;i<_NHits;i++){
     layer[i]=_Pthit[i]->getLayer();
     for(int j=0;j<3;j++){
      if (_Pthit[i]) {
       hits[i][j]=getHit(i,fits<0)[j];
       sigma[i][j]=getEHit(i)[j];
      } else {
        cout <<"AMSTrTrack::Fit -W- _Pthit["<<i<<"] = NULL, j"<<j<<endl;
      }
     }
    }
  }
  else if(fit ==1){
     npt=(TKDBc::patpoints(_Pattern)+1)/2;
     if(npt<3)npt=3;
     if(TKDBc::patpoints(_Pattern)%2==0 && TKDBc::patpoints(_Pattern)>4){
      // fit 1,,,n/2-1,n/2+1
       int j,k;
       for(k=0;k<npt;k++){
        layer[k]=_Pthit[k]->getLayer();
        for(j=0;j<3;j++){
        hits[k][j]=getHit(k<npt-1?k:k+1,fits<0)[j];
        sigma[k][j]=getEHit(k<npt-1?k:k+1)[j];
        }
       }      
     }
     else if(TKDBc::patpoints(_Pattern)>3){
       // fit 1,,,,n/2
       int j,k;
       for(k=0;k<npt;k++){
       layer[k]=_Pthit[k]->getLayer();
        for(j=0;j<3;j++){
        hits[k][j]=getHit(k,fits<0)[j];
        sigma[k][j]=getEHit(k)[j];
        }
       }      
     }
     else{
      _HChi2[0]=FLT_MAX;
      return _HChi2[0];       
     }    
  }  
  else if(fit ==2){
     npt=(TKDBc::patpoints(_Pattern)+1)/2;
     if(npt<3)npt=3;
     if(TKDBc::patpoints(_Pattern)%2==0 && TKDBc::patpoints(_Pattern)>4){
      // fit 1,,,n/2-1,n/2+1
       int j,k;
       for(k=0;k<npt;k++){
        layer[k]=_Pthit[k]->getLayer();
        for(j=0;j<3;j++){
        hits[k][j]=getHit((k==0?-1:k)+TKDBc::patpoints(_Pattern)-npt,fits<0)[j];
        sigma[k][j]=getEHit((k==0?-1:k)+TKDBc::patpoints(_Pattern)-npt)[j];
        }
       }      
     }
     else if(TKDBc::patpoints(_Pattern)>3){
       // fit 1,,,,n/2
       int j,k;
       for(k=0;k<npt;k++){
        layer[k]=_Pthit[k]->getLayer();
        for(j=0;j<3;j++){
        hits[k][j]=getHit(TKDBc::patpoints(_Pattern)-npt+k,fits<0)[j];
        sigma[k][j]=getEHit(TKDBc::patpoints(_Pattern)-npt+k)[j];
        }
       }      
     }
     else{
      _HChi2[0]=FLT_MAX;
      return _HChi2[0];       
     }    



  }
  else{
     return FLT_MAX;       
  }   
integer ims; 
if(fit==4)ims=0;
else ims=1;
    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    if(pmcg){
     number charge=pmcg->getcharge();    
     number momentum=pmcg->getmom();
     out[0]=charge/momentum;
    }
TKFITG(npt,hits,sigma,normal,ipart,ialgo,ims,layer,out);
if(fit==0){
_FastFitDone=1;
_Chi2FastFit=out[6];
if(out[7] != 0)_Chi2FastFit=FLT_MAX;
_Ridgidity=out[5];
_ErrRidgidity=out[8];
_Theta=out[3];
_Phi=out[4];
_P0=AMSPoint(out[0],out[1],out[2]);
 
}
else if(fit==1){
_HChi2[0]=out[6];
if(out[7] != 0)_HChi2[0]=FLT_MAX;
_HRidgidity[0]=out[5];
_HErrRidgidity[0]=out[8];
_HTheta[0]=out[3];
_HPhi[0]=out[4];
_HP0[0]=AMSPoint(out[0],out[1],out[2]);




}
else if(fit==2){
_AdvancedFitDone=1;
_HChi2[1]=out[6];
if(out[7] != 0)_HChi2[1]=FLT_MAX;
_HRidgidity[1]=out[5];
_HErrRidgidity[1]=out[8];
_HTheta[1]=out[3];
_HPhi[1]=out[4];
_HP0[1]=AMSPoint(out[0],out[1],out[2]);


// interpolate if fit == 2;

AMSTrRecHit *phit=_Pthit[0];
#ifdef __AMSDEBUG__
  assert(phit!= NULL);
#endif

 AMSDir SenDir((phit->getpsen())->getnrmA(2,0),
        (phit->getpsen())->getnrmA(2,1),(phit->getpsen())->getnrmA(2,2) );
 AMSPoint SenPnt=phit->getHit();
 number sleng;
 interpolate(SenPnt, SenDir, _HP0[1], _HTheta[1], _HPhi[1], sleng);
 if(_HTheta[1]<AMSDBc::pi/2)_HTheta[1]=AMSDBc::pi-_HTheta[1];
  
}
else if(fit==3){
#ifdef __AMSDEBUG__
int i,j;
for(i=0;i<npt;i++){
  for(j=0;j<3;j++){
    if(sigma[i][j]==0){
      cerr<<"AMSTrTrack::Fit-F-Some Errors are zero "<<i<<" "<<j<<endl;
      exit(1);
    }
  }
}
#endif     
_GeaneFitDone=ipart;
_GChi2=out[6];
_GRidgidity=out[5];
_GErrRidgidity=out[8];
_GTheta=out[3];
_GPhi=out[4];
_GP0=AMSPoint(out[0],out[1],out[2]);
if(out[7] !=0 || _GChi2>TRFITFFKEY.Chi2FastFit){
  _GeaneFitDone=0;
  if(out[7]){
    _GChi2=FLT_MAX;
    _GErrRidgidity=FLT_MAX;
    _GRidgidity=FLT_MAX;
  }
}  
}

else if(fit==4){
_Chi2MS=out[6];
if(out[7] != 0)_Chi2MS=FLT_MAX;
_RidgidityMS=out[5];
}
else if(fit==5){
_PIErrRigidity=out[8];
if(out[7] != 0)_PIErrRigidity=-out[7];
_PIRigidity=out[5];
_PITheta=out[3];
_PIPhi=out[4];
_PIP0=AMSPoint(out[0],out[1],out[2]);
_PIChi2=out[6];
}

return _Chi2FastFit;
}




void AMSTrTrack::getParFastFit(number&  Chi2, number& Rid, number&  Err,
number&  Theta, number&  Phi, AMSPoint&  X0)const
{Chi2=_Chi2FastFit;Rid=_Ridgidity;Theta=_Theta;Phi=_Phi;X0=_P0;Err=_ErrRidgidity;}

void AMSTrTrack::getParSimpleFit(number & Chi2_3D, number &Chi2sz, number & Rid)const
{Chi2_3D=_Chi2WithoutMS;Chi2sz=_Chi2StrLine;Rid=_RigidityWithoutMS;}

void AMSTrTrack::getParAdvancedFit(number&   GChi2, number&  GRid, number&  GErr,
number&  GTheta, number&  GPhi, AMSPoint&  GP0,
number HChi2[2], number HRid[2], number HErr[2], number HTheta[2],
                              number HPhi[2], AMSPoint  HP0[2] ) const {
 GChi2=_GChi2; GRid=_GRidgidity; GErr=_GErrRidgidity; GTheta=_GTheta;
GPhi=_GPhi;GP0=_GP0;
for(int i=0;i<2;i++){
 HChi2[i]=_HChi2[i];
 HRid[i]=_HRidgidity[i];
 HErr[i]=_HErrRidgidity[i];
 HTheta[i]=_HTheta[i];
 HPhi[i]=_HPhi[i];
 HP0[i]=_HP0[i];
}
}

void AMSTrTrack::_writeEl(){
  if(AMSTrTrack::Out(1)){
    int i;
#ifdef __WRITEROOT__
    // Root related part in ../CC/root.C 
    // TrTrackR::TrTrackR(AMSTrTrack *ptr)
    // no check for _AdvancedFitDone && _GeaneFitDone
    // in TrTrackR constructor
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  TrTrackNtuple02* TrTN = AMSJob::gethead()->getntuple()->Get_trtr02();
  if (TrTN->Ntrtr>=MAXTRTR02) return;

// Fill the ntuple 
    TrTN->Status[TrTN->Ntrtr]=_status;
    TrTN->Pattern[TrTN->Ntrtr]=_Pattern;
    TrTN->NHits[TrTN->Ntrtr]=_NHits;
    TrTN->Address[TrTN->Ntrtr]=_Address;
//    for(i=0;i<2;i++)TrTN->Dbase[TrTN->Ntrtr][i]=_Dbase[i];
    
    int k;
    for(k=_NHits;k<TKDBc::nlay();k++)TrTN->pHits[TrTN->Ntrtr][k]=0;
    for(k=0;k<_NHits;k++){
     TrTN->pHits[TrTN->Ntrtr][k]=_Pthit[k]->getpos();
      int pat;
      pat=_Pthit[k]->getLayer()-1;
      if (AMSTrRecHit::Out(IOPA.WriteAll%10==1)){
        // Writeall
        for(i=0;i<pat;i++){
          AMSContainer *pc=AMSEvent::gethead()->getC("AMSTrRecHit",i);
           #ifdef __AMSDEBUG__
            assert(pc != NULL);
           #endif
           TrTN->pHits[TrTN->Ntrtr][k]+=pc->getnelem();
        }
      }                                                        
      else if (AMSTrRecHit::Out(IOPA.WriteAll%10==0)){
      //WriteUsedOnly
        for(i=0;i<pat;i++){
          AMSTrRecHit *ptr=(AMSTrRecHit*)AMSEvent::gethead()->getheadC("AMSTrRecHit",i);
          while(ptr && ptr->checkstatus(AMSDBc::USED)){
            TrTN->pHits[TrTN->Ntrtr][k]++;
            ptr=ptr->next();
          }
        }
      }
      else if (AMSTrRecHit::Out(IOPA.WriteAll%10==2)){
      //WriteUsedOnly
        for(i=0;i<pat;i++){
          AMSTrRecHit *ptr=(AMSTrRecHit*)AMSEvent::gethead()->getheadC("AMSTrRecHit",i);
          while(ptr && !(ptr->checkstatus(AMSDBc::AwayTOF)) ){
            TrTN->pHits[TrTN->Ntrtr][k]++;
            ptr=ptr->next();
          }
        }
      }
      else return;
    }
  
  
    TrTN->LocDBAver[TrTN->Ntrtr]=_Dbase[0];
    TrTN->GeaneFitDone[TrTN->Ntrtr]=_GeaneFitDone;
    TrTN->AdvancedFitDone[TrTN->Ntrtr]=_AdvancedFitDone;
    TrTN->Chi2StrLine[TrTN->Ntrtr]=geant(_Chi2StrLine);
    TrTN->Chi2WithoutMS[TrTN->Ntrtr]=geant(_Chi2WithoutMS);
    TrTN->RigidityWithoutMS[TrTN->Ntrtr]=(geant)_RigidityWithoutMS;
    TrTN->Chi2FastFit[TrTN->Ntrtr]=(geant)_Chi2FastFit;
    TrTN->Ridgidity[TrTN->Ntrtr]=(geant)_Ridgidity;
    TrTN->ErrRidgidity[TrTN->Ntrtr]=(geant)_ErrRidgidity;
    TrTN->Theta[TrTN->Ntrtr]=(geant)_Theta;
    TrTN->Phi[TrTN->Ntrtr]=(geant)_Phi;
    for(i=0;i<3;i++)TrTN->P0[TrTN->Ntrtr][i]=(geant)_P0[i];
    if(_GeaneFitDone){ 
     TrTN->GChi2[TrTN->Ntrtr]=(geant)_GChi2;
     TrTN->GRidgidity[TrTN->Ntrtr]=(geant)_GRidgidity;
     TrTN->GErrRidgidity[TrTN->Ntrtr]=(geant)_GErrRidgidity;
    }
    else{
     TrTN->GChi2[TrTN->Ntrtr]=-1;
     TrTN->GRidgidity[TrTN->Ntrtr]=0;
     TrTN->GErrRidgidity[TrTN->Ntrtr]=0;
    } 
    if(_AdvancedFitDone){
     for(i=0;i<2;i++)TrTN->HChi2[TrTN->Ntrtr][i]=(geant) _HChi2[i];
     for(i=0;i<2;i++)TrTN->HRidgidity[TrTN->Ntrtr][i]=(geant)_HRidgidity[i];
     for(i=0;i<2;i++)TrTN->HErrRidgidity[TrTN->Ntrtr][i]=(geant)_HErrRidgidity[i];
     for(i=0;i<2;i++)TrTN->HTheta[TrTN->Ntrtr][i]=(geant)_HTheta[i];
     for(i=0;i<2;i++)TrTN->HPhi[TrTN->Ntrtr][i]=(geant)_HPhi[i];
     for(i=0;i<3;i++)TrTN->HP0[TrTN->Ntrtr][0][i]=(geant)_HP0[0][i];
     for(i=0;i<3;i++)TrTN->HP0[TrTN->Ntrtr][1][i]=(geant)_HP0[1][i];
    }
    else{
     for(i=0;i<2;i++)TrTN->HChi2[TrTN->Ntrtr][i]=-1;
     for(i=0;i<2;i++)TrTN->HRidgidity[TrTN->Ntrtr][i]=0;
     for(i=0;i<2;i++)TrTN->HErrRidgidity[TrTN->Ntrtr][i]=0;
     for(i=0;i<2;i++)TrTN->HTheta[TrTN->Ntrtr][i]=0;
     for(i=0;i<2;i++)TrTN->HPhi[TrTN->Ntrtr][i]=0;
     for(i=0;i<3;i++)TrTN->HP0[TrTN->Ntrtr][0][i]=0;
     for(i=0;i<3;i++)TrTN->HP0[TrTN->Ntrtr][1][i]=0;
    }
    TrTN->FChi2MS[TrTN->Ntrtr]=(geant)_Chi2MS;
    TrTN->PiErrRig[TrTN->Ntrtr]=(geant)_PIErrRigidity;
    TrTN->RidgidityMS[TrTN->Ntrtr]=(geant)_RidgidityMS;
    TrTN->PiRigidity[TrTN->Ntrtr]=(geant)_PIRigidity;
    TrTN->Ntrtr++;
  }
}
void AMSTrTrack::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
TrTrackR & trr=AMSJob::gethead()->getntuple()->Get_evroot02()->TrTrack(_vpos);
    for (int i=0; i<_NHits; i++) {
      if(_Pthit[i])trr.fTrRecHit.push_back(_Pthit[i]->GetClonePointer());
    }
#endif
}


void AMSTrTrack::print(){
for(int i=0;i<TKDBc::npat();i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSTrTrack",i);
 if(p && TKDBc::patallow(i))p->printC(cout);
}
}

void AMSTrTrack::interpolate(AMSPoint  pntplane, AMSDir dirplane,AMSPoint & P1,
                             number & theta, number & phi, number & length, int icase){

  // interpolates track to plane (pntplane, dirplane)
  // and calculates the track parameters(P1,theta,phi) and total track length

  geant out[7];
  static number m55[5][5];
  geant init[7];
  geant point[6];
  geant charge=1;
  AMSDir dir;
  if(icase==0){
   AMSDir  dir(_Theta,_Phi);
   init[0]=_P0[0];
   init[1]=_P0[1];
   init[2]=_P0[2];
   init[3]=dir[0];
   init[4]=dir[1];
   init[5]=dir[2];
   init[6]=_Ridgidity;
  }
  else if (icase==1){
   AMSDir  dir(_GTheta,_GPhi);
   init[0]=_GP0[0];
   init[1]=_GP0[1];
   init[2]=_GP0[2];
   init[3]=dir[0];
   init[4]=dir[1];
   init[5]=dir[2];
   init[6]=_GRidgidity;
  }
  else {
   AMSDir  dir(_PITheta,_PIPhi);
   init[0]=_PIP0[0];
   init[1]=_PIP0[1];
   init[2]=_PIP0[2];
   init[3]=dir[0];
   init[4]=dir[1];
   init[5]=dir[2];
   init[6]=_PIRigidity;
  }
  point[0]=pntplane[0];
  point[1]=pntplane[1];
  point[2]=pntplane[2];
  point[3]=dirplane[0];
  point[4]=dirplane[1];
  point[5]=dirplane[2];
  geant slength;
  TKFITPAR(init, charge,  point,  out,  m55, slength);
  P1[0]=out[0];
  P1[1]=out[1];
  P1[2]=out[2];
  theta=acos(out[5]);
  phi=atan2(out[4],out[3]);
  length=slength;  
}

void AMSTrTrack::interpolateCyl(AMSPoint CylCenter, AMSDir CylAxis,
                                number CylRadius, number idir, AMSPoint & P1,
                                number & theta, number & phi, number & length){

  // interpolates track to cylinder (Cylpar = x_cen,y_cen, radius)
  //in the direction idir  
  // and calculates the track parameters(P1,theta,phi) and total track length

  geant out[7];
  static number m55[5][5];
  geant init[7];
  geant point[7];
  geant charge=1;
  point[0]=CylCenter[0];
  point[1]=CylCenter[1];
  point[2]=CylCenter[2];
  point[3]=CylAxis[0];
  point[4]=CylAxis[1];
  point[5]=CylAxis[2];
  point[6]=CylRadius;
  geant slength;
  number s2=(CylCenter-_P0).prod(CylCenter-_P0);
  number s1=(CylCenter-_P0).prod(CylAxis);
  number sdist=CylRadius-sqrt(s2-s1*s1);
  number local;
  if(sdist<0){
    // interpolate to 2nd ladder first;
   integer ok=intercept(P1,2,theta,phi,local);
   AMSDir dir(idir*sin(theta)*cos(phi),
             idir*sin(theta)*sin(phi),
             idir*cos(theta));
   init[0]=P1[0];
   init[1]=P1[1];
   init[2]=P1[2];
   init[3]=dir[0];
   init[4]=dir[1];
   init[5]=dir[2];
  }
  else {
   AMSDir dir(idir*sin(_Theta)*cos(_Phi),
             idir*sin(_Theta)*sin(_Phi),
             idir*cos(_Theta));
   init[0]=_P0[0];
   init[1]=_P0[1];
   init[2]=_P0[2];
   init[3]=dir[0];
   init[4]=dir[1];
   init[5]=dir[2];
  }
   init[6]=_Ridgidity;
   TKFITPARCYL(init, charge,  point,  out,  m55, slength);
  P1[0]=out[0];
  P1[1]=out[1];
  P1[2]=out[2];
  theta=acos(out[5]);
  phi=atan2(out[4],out[3]);
  length=slength;  
}


void AMSTrTrack::init(AMSTrRecHit * phit[] ){
int i;
for( i=0;i<_NHits;i++)_Pthit[i]=phit[i];
for(i=_NHits;i<trconst::maxlay;i++)_Pthit[i]=0;
 _GChi2=-1;
 _GRidgidity=0;
 _GErrRidgidity=0;
 _GTheta=0;
 _GPhi=0;
 _GP0=0;
 _PIP0=0;
 _Chi2MS=-1;
 _PIErrRigidity=-1;
 _RidgidityMS=0;
 _PIRigidity=0;
 _PITheta=0;
 _PIPhi=0;
 for(i=0;i<2;i++){
 _HRidgidity[i]=0;
 _HErrRidgidity[i]=0;
 _HTheta[i]=0;
 _HPhi[i]=0;
 _HP0[i]=0;
 }

}

AMSTrTrackError::AMSTrTrackError(char * name){
  if(name){
    integer n=strlen(name)+1;
    if(n>255)n=255;
    strncpy(msg,name,n);
  }
  AMSgObj::BookTimer.stop("TrTrack",1);
  AMSgObj::BookTimer.stop("RETKEVENT",1);
  AMSgObj::BookTimer.stop("REAMSEVENT",1);
  AMSEvent::gethead()->seterror(2);
}
char * AMSTrTrackError::getmessage(){return msg;}

integer AMSTrCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTrCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}



integer AMSTrRecHit::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTrRecHit",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}



integer AMSTrTrack::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTrTrack",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}

/* This function creates a false AMSTrRecHit for each S-AMSTrCluster */
/* consistent with TOF information. The X coordinate is imposed from */
/* the straight line fit to >=3 TOF planes in the XZ projection      */

integer AMSTrTrack::makeFalseTOFXHits(){

    int i;

    TriggerLVL302 *plvl3;
    plvl3 = (TriggerLVL302*)AMSEvent::gethead()->getheadC("TriggerLVL3",0);
// LVL3 required if existing
    if ( plvl3==NULL) {
// #ifdef __AMSDEBUG__
//    cout << "makeFalseTOFXHits: No Level3 Trigger existing" << endl;
// #endif
      return 1;
    }
    if ( plvl3->skip() ) return 1;

    AMSTOFCluster *phit[4], *ploop;

// There should be one and only one AMSTOFCluster on planes 1, 4
// according to LVL3 trigger
    phit[0] = AMSTOFCluster::gethead(0);
    if ( (phit[0] == NULL) || (phit[0]->next()) ) return 1;
    phit[3] = AMSTOFCluster::gethead(3);
    if ( (phit[3] == NULL) || (phit[3]->next()) ) return 1;

// Initial straight line from planes 1 and 4 for ZX projection
    number slope_x= (phit[3]->getcoo()[0] - phit[0]->getcoo()[0]) /
               (phit[3]->getcoo()[2] - phit[0]->getcoo()[2]);
    number intercept_x= phit[0]->getcoo()[0] - slope_x*phit[0]->getcoo()[2];

// Look for the closest AMSTOFCluster on plane 2
    number resmax2=999;
    phit[1]=NULL;
    for (ploop = AMSTOFCluster::gethead(1); ploop ; ploop=ploop->next() ){
      number resx2 = fabs(ploop->getcoo()[0] 
                          - slope_x*ploop->getcoo()[2] - intercept_x);
      if (resx2<resmax2) {
        resmax2 = resx2;
        phit[1] = ploop;
      }
    }

// Look for the closest AMSTOFCluster on plane 3
// to improve the X prediction
    number resmax3=999;
    phit[2]=NULL;
    for (ploop = AMSTOFCluster::gethead(2); ploop ; ploop=ploop->next() ){
      number resx3 = fabs(ploop->getcoo()[0] 
                          - slope_x*ploop->getcoo()[2] - intercept_x);
      if (resx3<resmax3) {
        resmax3 = resx3;
        phit[2] = ploop;
      }
    }

// We require at least 3 AMSTOFClusters
    if ( (phit[1]==NULL) && (phit[2]==NULL) ) return 1;

// Straight line parameters for the ZX plane 
// Use only Pad information -planes 2 or 3- if not consistent with planes 
// 1 or 4, respectively (info from planes 1,4 is TOF-calibration dependent)
    number sw=0, sz=0, sx=0, sxz=0, szz=0;
    geant searchregtof;
    searchregtof=TOF2DBc::plnstr(5)+2.*TOF2DBc::plnstr(8);
    for (i=0; i<4; i++){
      if (phit[i]==NULL) continue;
      if (i==0 && phit[1]!=NULL && resmax2>searchregtof) continue;
      if (i==3 && phit[2]!=NULL && resmax3>searchregtof) continue;
      number w = phit[i]->getecoo()[0]; if (w<=0.) continue; w = 1./w/w;
      number x = phit[i]->getcoo()[0];
      number z = phit[i]->getcoo()[2];
      sw += w;
      sx += x*w;
      sz += z*w;
      sxz += x*z*w;
      szz += z*z*w;
    }
    number determinant = szz*sw-sz*sz;
    slope_x = (sxz*sw-sx*sz)/determinant;
    intercept_x = (szz*sx-sz*sxz)/determinant;
    number covss = sw/determinant;
    number covsi = -sz/determinant;
    number covii = szz/determinant;

// Straight line parameters for the ZY plane 
// (Only planes 1,4 => TOFCalib independent, but robust and enough)
    number slope_y= (phit[3]->getcoo()[1] - phit[0]->getcoo()[1]) /
               (phit[3]->getcoo()[2] - phit[0]->getcoo()[2]);
    number intercept_y= phit[0]->getcoo()[1] - slope_y*phit[0]->getcoo()[2];

// Create Fake AMSTrRecHits on all tracker planes containing sensible Y clusters
    AMSTrCluster *py;
    for (py=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0);
         py != NULL ; py=py->next()) {
// Do nothing for bad clusters
      if (py->checkstatus(AMSDBc::BAD) != 0) continue;
// SoftId, approximate IdGeom and approximate gSensor
      AMSTrIdSoft idsoft = py->getid();
      integer layer = idsoft.getlayer();
      integer ladder = idsoft.getdrp();
      integer half = idsoft.gethalf();
      integer sensor;
      if (half==0) 
                   sensor = 5; 
      else 
                   sensor = TKDBc::nsen(layer,ladder) - 5;
      AMSTrIdGeom idgeom(layer,ladder,sensor,0,0);
      AMSgSen *psensor=(AMSgSen*)AMSJob::gethead()->getgeomvolume(idgeom.crgid());
      idgeom.R2G(idsoft);
// Get the approximate global y coordinate
      AMSPoint glopos = psensor->str2pnt(0.,py->getcofg(&idgeom));
// Impose the X global coordinate from TOF
      glopos[0] = intercept_x + slope_x*glopos[2];
// Do not use if it is far away from TOF prediction
      geant searchregtof;
      searchregtof=TOF2DBc::plnstr(5)+2.*TOF2DBc::plnstr(8);
      if (fabs(glopos[1] - intercept_y - slope_y*glopos[2])
               > searchregtof) continue; // 1/2 SC bar
// Find now the right sensor number on the ladder
// And do not use it if TOF prediction is far away from the ladder
      AMSPoint sensor_size(   psensor->getpar(0)
                              , psensor->getpar(1)
                              , psensor->getpar(2));
      if(!idgeom.FindAtt(glopos,sensor_size) 
      || idgeom.gethalf()!=idsoft.gethalf()
      || idgeom.getladder()!=idsoft.getdrp()) continue;
// Go to local coordinates to get a better global position
      psensor=(AMSgSen*)AMSJob::gethead()->getgeomvolume(idgeom.crgid());
      AMSPoint locpos = psensor->gl2loc(glopos);
      glopos = psensor->str2pnt(locpos[0]+sensor_size[0],py->getcofg(&idgeom));
// Error
      AMSPoint gloerr;
      gloerr[0] = sqrt(covii+2*glopos[2]*covsi+glopos[2]*glopos[2]*covss);
      gloerr[1] = py->getecofg();
      gloerr[2] = (number)TRCLFFKEY.ErrZ;

// Create a new Fake hit with TOF on X
      AMSTrRecHit::_addnext(
        psensor,
        AMSDBc::FalseTOFX,
        idsoft.getlayer(),
        -1,
        -1,
        0,
        py,
        glopos,
        gloerr );
//         cout <<idsoft.getlayer()<<" "<<idsoft.gethalf()<<" "<<glopos<<" "<<idgeom.getsensor()<<" "<<idsoft.gethalf()<<" "<<idgeom.gethalf()<<endl;
        // cout << "makeFalseTOFXHits: New Hit done at plane " << 
        //           idsoft.getlayer() << glopos << gloerr << endl;
    }
    return 0;
}

integer AMSTrTrack::buildFalseTOFX(integer refit){
  {
    int nrh=0;
    for(int i=0;i<TKDBc::nlay();i++){
     nrh+= (AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
    }
    if(nrh>=min(TRFITFFKEY.MaxTrRecHitsPerLayer*TKDBc::nlay(),root::MAXTRRH02)){
    AMSlink *ptr=AMSEvent::gethead()->getheadC("TriggerLVL3",0);
    TriggerLVL302 *ptr302=dynamic_cast<TriggerLVL302*>(ptr);
      if(!ptr302 || ptr302->skip()){
       AMSEvent::gethead()->seterror();
       return 0;
      }
    }
  }

  integer NTrackFound=-1;
  // pattern recognition + fit
  if(refit){
    // Cleanup all track containers
    int i;
    for(i=0;;i++){
      AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrTrack",i);
      if(pctr)pctr->eraseC();
      else break ;
    }
  } 
  _RefitIsNeeded=0;
  _Start();
  // Add test here

// Build Fake Clusters with TOF
  integer hitstatus = makeFalseTOFXHits();
  if (hitstatus!=0) return -1;
   
  { 
    int xs=0; 
    for (int kk=0;kk<TKDBc::nlay();kk++){
      AMSTrRecHit * pphit;
      for (pphit=AMSTrRecHit::gethead(kk); pphit!=NULL; pphit=pphit->next()){
        if (pphit->Good() && pphit->checkstatus(AMSDBc::FalseTOFX)) {
          xs++; 
          break;
        }
      }
    }
    if(xs>3)AMSEvent::gethead()->addnext(AMSID("Test",0),new Test());
    else return -1;
  }
          integer       ThreePointNotWanted=0;

  for (pat=0;pat<TKDBc::npat();pat++){
    if(TKDBc::patallow(pat)){
    if(TKDBc::patpoints(pat)==3 && ThreePointNotWanted)continue;
      int fp=TKDBc::patpoints(pat)-1;    
      // Try to make StrLine Fit
      integer first=TKDBc::patconf(pat,0)-1;
      integer second=TKDBc::patconf(pat,fp)-1;
      phit[0]=AMSTrRecHit::gethead(first);
      while( phit[0]){
       if(phit[0]->Good() && phit[0]->checkstatus(AMSDBc::FalseTOFX)!=0){
       phit[fp]=AMSTrRecHit::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good() && phit[fp]->checkstatus(AMSDBc::FalseTOFX)!=0){
        par[0][0]=(phit[fp]-> getHit()[0]-phit[0]-> getHit()[0])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[0][2]=sqrt(1+par[0][0]*par[0][0]);
        par[0][1]=phit[0]-> getHit()[0]-par[0][0]*phit[0]-> getHit()[2];
        par[1][0]=(phit[fp]-> getHit()[1]-phit[0]-> getHit()[1])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[1][1]=phit[0]-> getHit()[1]-par[1][0]*phit[0]-> getHit()[2];
        par[1][2]=sqrt(1+par[1][0]*par[1][0]);
        if(NTrackFound<0)NTrackFound=0;
        // Search for others
        //  add try due to icc bug
        try{
        integer npfound=_TrSearcherFalseTOFX(1);
        if(npfound){
           NTrackFound++;
         if(npfound>4){
            // we don't want three points any more
            ThreePointNotWanted=1;
         }
         goto out;
        }
        }
        catch (...){
         throw;
        }

        }         
        phit[fp]=phit[fp]->next();
       }
       }  
out:
       phit[0]=phit[0]->next();
      }
      
    }
  }
return NTrackFound;
}

integer AMSTrTrack::DistanceTOF(number par[2][3], AMSTrRecHit *ptr){
/*
const integer freq=10;
static integer trig=0;
trig=(trig+1)%freq;
           if(trig==0 && _NoMoreTime()){
            throw AMSTrTrackError(" Cpulimit Exceeded ");
           }
*/
   geant searchregtof;
   searchregtof=TOF2DBc::plnstr(5)+2.*TOF2DBc::plnstr(8);
   
   return fabs(par[0][1]+par[0][0]*ptr->getHit()[2]-ptr->getHit()[0])
            > searchregtof*par[0][2] ||
          fabs(par[1][1]+par[1][0]*ptr->getHit()[2]-ptr->getHit()[1])>
            TRFITFFKEY.SearchRegCircle*par[1][2];
}


integer AMSTrTrack::intercept(AMSPoint &P1,integer layer, number &theta, number &phi, number & local, integer icase){
  AMSTrRecHit * phit=0;  
  for(int i=0;i<_NHits;i++){
   if(_Pthit[i]->getLayer()-1 == layer){
    phit=_Pthit[i];
    break;
   } 
  }
  AMSPoint xloc;
  AMSgSen *pls=0;
  if(phit){
    pls=phit->getpsen();
    AMSDir pntdir(pls->getnrmA(0,2),pls->getnrmA(1,2),pls->getnrmA(2,2));
    AMSPoint pntplane(phit->getHit());
    number length;
    interpolate(pntplane,pntdir,P1,theta,phi,length,icase);
    xloc=pls->gl2loc(P1);
  }    
  else{
    // no hits but still track extrap needed
    AMSTrIdGeom g(layer+1,8,4,0,0);    // reference
    AMSgvolume *p =AMSJob::gethead()->getgeomvolume(g.crgid());
    if(p){
      AMSDir pntdir(p->getnrmA(0,2),p->getnrmA(1,2),p->getnrmA(2,2));
      AMSPoint pntplane(p->getcooA(0),p->getcooA(1),p->getcooA(2));
      number length;
      interpolate(pntplane,pntdir,P1,theta,phi,length,icase);
    }
    else {
      cerr <<"AMSTrTrack::intercept-E-NoGeomVolumeFound"<<g.crgid()<<endl;
      P1=0;
      theta=0;
      phi=0;
      local=0;
      return 0;
    }
      // Interpolate to certain sensor if any;
     AMSPoint PS(p->getpar(0),p->getpar(1),p->getpar(2));
      if(g.FindAtt(P1,PS)){
         pls=(AMSgSen*) AMSJob::gethead()->getgeomvolume(g.crgid());
         AMSDir pntdir(pls->getnrmA(0,2),pls->getnrmA(1,2),pls->getnrmA(2,2));
         AMSPoint pntplane(pls->getcooA(0),pls->getcooA(1),pls->getcooA(2));
         number theta,phi,length;
         interpolate(pntplane,pntdir,P1,theta,phi,length,icase);
         xloc=pls->gl2loc(P1);
      }
      else{
       local=0;
       return -1;
      }

  }
  AMSPoint PS(pls->getpar(0),pls->getpar(1),pls->getpar(2));
  xloc=(xloc/PS).abs();
   if(xloc[0]>xloc[1])local=1-xloc[0];
   else local=1-xloc[1];
  return 1;
}

void AMSTrTrack::_crHit(){
 // build address

 _buildaddress();
 //decodeaddress();
 integer found=0;
 AMSTrAligPar * par(0);
 if(AMSTrAligFit::glDBOK(_Address))setstatus(AMSDBc::GlobalDB);
// if(!TRALIG.UpdateDB &&  !checkstatus(AMSDBc::GlobalDB))par=AMSTrAligPar::SearchDB(_Address, found,_Dbase);
 if(!TRALIG.UpdateDB )par=AMSTrAligPar::SearchDB(_Address, found,_Dbase);
  if(found && fabs(_Dbase[1]-TRALIG.One)>TRALIG.GlobalGoodLimit && 
     fabs(_Dbase[0]-1.025)<TRALIG.GlobalGoodLimit){
   for(int i=0;i<_NHits;i++){
    int plane=TKDBc::patconf(_Pattern,i)-1;
    for(int j=0;j<3;j++){
     _Hit[i][j]=(par[plane].getcoo())[j]+
      (par[plane].getmtx(j)).prod(_Pthit[i]->getlocHit());
     _EHit[i][j]=fabs((par[plane].getmtx(j)).prod(_Pthit[i]->getEHit()));
//     cout <<i<<" "<<j<<" "<<_Hit[i][j]<<" "<<_Pthit[i]->getHit()[j]<<endl; 
    }
   }
   setstatus(AMSDBc::LocalDB);
  }
  else{
   _Dbase[0]=0;
   _Dbase[1]=0;
   for(int i=0;i<_NHits;i++){
    int plane=TKDBc::patconf(_Pattern,i)-1;
    for(int j=0;j<3;j++){
     _Hit[i][j]=_Pthit[i]->getHit()[j];
     _EHit[i][j]=_Pthit[i]->getEHit()[j];
    }
   }
  }
}

void AMSTrTrack::_buildaddress(){
  _Address=0;
  for(int i=0;i<_NHits;i++){
    AMSTrIdSoft id = _Pthit[i]->getClusterP(1)->getid();
    int layer=id.getlayer();
    int ladder=id.getdrp();
    int half=id.gethalf();
    _Address+=TKDBc::Cumulus(layer)*(ladder+half*(TKDBc::nlad(layer)+1));
  } 

}

void AMSTrTrack::decodeaddress(integer ladder[2][trconst::maxlay], uinteger _Address){
   for(int i=0;i<TKDBc::nlay();i++){
    uinteger lad=(_Address/TKDBc::Cumulus(i+1))%(2*TKDBc::nlad(i+1)+2);
    ladder[0][i]=lad%(TKDBc::nlad(i+1)+1);
    ladder[1][i]=lad/(TKDBc::nlad(i+1)+1);
    //cout <<i+1<< " "<<ladder[0][i]<<" "<<ladder[1][i]<<endl; 
   }
}

uinteger * AMSTrTrack::getchild(uinteger address, uinteger & nchild){
   // suppress childs;
    nchild=0;
    return 0;

    const int maxchld=21;
    static uinteger achld[maxchld];
    integer lad[2][trconst::maxlay];    
    integer lad1[2][trconst::maxlay];    
    integer npt=0;
    decodeaddress(lad,address);
    int xnpt=0;
    int i;
    for(i=0;i<TKDBc::nlay();i++){
     if(lad[0][i])xnpt++;
    }
    nchild=0;
    if(xnpt>=5){
     for(i=0;i<TKDBc::nlay();i++){
      if(lad[0][i]){
       int tmp=lad[0][i];
       lad[0][i]=0;
       achld[nchild++]=encodeaddress(lad);     
       lad[0][i]=tmp;
      }
     }
     //if(xnpt==6){
     if(xnpt>6){
      for(i=0;i<TKDBc::nlay();i++){
       int tmpi=lad[0][i];
       lad[0][i]=0;
       for(int j=i+1;j<TKDBc::nlay();j++){
        int tmpj=lad[0][j];
        lad[0][j]=0;
        achld[nchild++]=encodeaddress(lad);     
        lad[0][j]=tmpj;
        if(nchild>maxchld){
         cerr<< "AMSTrTrack::getchild-F-TooManyChilds "<<nchild<<endl;
         exit(1);         
        }
       }
       lad[0][i]=tmpi;
      }
     }
     return achld; 
    }
    else{
     return 0;
    } 
}


uinteger AMSTrTrack::encodeaddress(integer ladder[2][trconst::maxlay]){
   uinteger address=0;
   for(int i=0;i<TKDBc::nlay();i++){
     address+= TKDBc::Cumulus(i+1)*(ladder[0][i]+ladder[1][i]*(TKDBc::nlad(i+1)+1));
   }
   return address;
}

AMSTrTrack::AMSTrTrack(AMSDir dir, AMSPoint point, number rig, number errig):AMSlink(0,0),
_Pattern(-1),_NHits(0),_GeaneFitDone(0),_AdvancedFitDone(1),
_Ridgidity(rig),_ErrRidgidity(errig),_Chi2FastFit(1000000){
 for(int i=0;i<trconst::maxlay;i++){
  _Pthit[i]=0;
 }
 _Theta=dir.gettheta();
 _Phi=dir.getphi();
 _P0=point;
}

AMSTrTrack::AMSTrTrack(number theta, number phi, AMSPoint point):AMSlink(0,0),
_Pattern(-1),_NHits(0),_GeaneFitDone(0),_AdvancedFitDone(1),
_Ridgidity(10000000),_ErrRidgidity(10000000),_Chi2FastFit(1000000){
 for(int i=0;i<trconst::maxlay;i++){
  _Pthit[i]=0;
 }
 _Theta=theta;
 _Phi=phi;
 _P0=point;
}



integer AMSTrTrack::_TrSearcher(int icall){
           if( _NoMoreTime()){
            throw AMSTrTrackError(" Cpulimit Exceeded ");
           }
            const int freq=1000 ;
            int trig=0;
           phit[icall]=AMSTrRecHit::gethead(TKDBc::patconf(pat,icall)-1);
           while(phit[icall]){
//             cout <<icall<<" "<<phit[icall]<<" "<<pat<<endl;
             if(phit[icall]->Good() && !Distance(par,phit[icall])){
              if(++trig%freq ==0){
                           if( _NoMoreTime()){
                           throw AMSTrTrackError(" Cpulimit Exceeded ");
           }
              }
              if(TKDBc::patpoints(pat) >icall+2){         
                return _TrSearcher(++icall); 
              }                
              else{  
                // icall+2 point combination found
                if(_addnext(pat,icall+2,phit)){
                  return icall+2;
                }
              }
             }
             phit[icall]=phit[icall]->next();
           }
           return 0;


}
integer AMSTrTrack::_TrSearcherFalseTOFX(int icall){
           if( _NoMoreTime()){
            throw AMSTrTrackError(" Cpulimit Exceeded ");
           }
           phit[icall]=AMSTrRecHit::gethead(TKDBc::patconf(pat,icall)-1);
           while(phit[icall]){
             if(phit[icall]->Good() &&  phit[icall]->checkstatus(AMSDBc::FalseTOFX) && !DistanceTOF(par,phit[icall])){
              if(TKDBc::patpoints(pat) >icall+2){         
                integer iret=_TrSearcherFalseTOFX(++icall); 
                return iret;
              }                
              else{  
                // icall+2 point combination found
                if(_addnext(pat,icall+2,phit)){
                  return icall+2;
                }
              }
             }
             phit[icall]=phit[icall]->next();
           }
           return 0;


}

integer AMSTrTrack::pat=0;

AMSTrRecHit* AMSTrTrack::phit[trconst::maxlay]={0,0,0,0,0,0,0,0};

number AMSTrTrack::par[2][3];








integer AMSTrTrack::_TrSearcherFalseX(int icall){
           if( _NoMoreTime()){
            throw AMSTrTrackError(" Cpulimit Exceeded ");
           }
           const int freq=1000;
           int trig=0;
           phit[icall]=AMSTrRecHit::gethead(TKDBc::patconf(pat,icall)-1);
           while(phit[icall]){
//             cout <<icall<<" "<<phit[icall]<<" "<<pat<<endl;
             if(phit[icall]->Good() &&   phit[icall]->checkstatus(AMSDBc::FalseX)==0 && !Distance(par,phit[icall]) ){
                            if(++trig%freq ==0){
                           if( _NoMoreTime()){
                           throw AMSTrTrackError(" Cpulimit Exceeded ");
           }
              }
              if(TKDBc::patpoints(pat) >icall+2){         
                integer iret=_TrSearcherFalseX(++icall); 
                return iret;
              }                
              else{  
                // icall+2 point combination found
                int f=_addnextFalseX(pat,icall+2,phit);
                if(f){
                  return f;
                }
              }
             }
             phit[icall]=phit[icall]->next();
           }
           return 0;


}

bool AMSTrTrack::_NoMoreTime(){
static unsigned int count=0;
if( (count++)%512==0 ){
 return _TimeLimit>0? _CheckTime()>_TimeLimit: _CheckTime()>AMSFFKEY.CpuLimit;
}
else return false;
}


AMSTrTrack::AMSTrTrack(integer nht, AMSTrRecHit * pht[], int FFD, int GFD, 
                       number chi2FF, number rigFF, number erigFF, number thetaFF, number phiFF, AMSPoint P0FF, 
                       number chi2G, number rigG, number erigG, number thetag, number phig, AMSPoint p0g, 
                       number chi2MS, number jchi2MS, number rigFMS, number grigms):
_NHits(nht),_FastFitDone(FFD),_GeaneFitDone(1),_Chi2FastFit(chi2FF),_Ridgidity(rigFF), _ErrRidgidity(erigFF),_Theta(thetaFF),_Phi(phiFF),_P0(P0FF),_GChi2(chi2G),_GRidgidity(grigms),_GErrRidgidity(erigG),_Chi2MS(chi2MS),_PIErrRigidity(jchi2MS),_RidgidityMS(rigFMS),_PIRigidity(grigms),_PITheta(thetag),_PIPhi(phig),_Address(0),_Pattern(-1),_AdvancedFitDone(0),_GPhi(phig),_GTheta(thetag),_GP0(p0g),_PIP0(p0g){
 
  for(int i=0;i<2;i++)_Dbase[i]=0;
  _Chi2StrLine=0;
_Chi2WithoutMS=0;
_RigidityWithoutMS=0;

for(int i=0;i<2;i++) _HChi2[i]=0;
for(int i=0;i<2;i++) _HRidgidity[i]=0;
for(int i=0;i<2;i++) _HErrRidgidity[i]=0;
for(int i=0;i<2;i++) _HTheta[i]=0;
for(int i=0;i<2;i++) _HPhi[i]=0;
for(int i=0;i<3;i++) _HP0[0][i]=0;
for(int i=0;i<3;i++) _HP0[1][i]=0;




for(int i=0;i<_NHits;i++)_Pthit[i]=pht[i];

}
         
///////////////////////////////////////////////////////////
AMSTrTrack* AMSTrTrack::CloneIt(){

  static AMSTrTrack* ptrout = NULL;
  if (this) {
          ptrout = new AMSTrTrack(_Pattern, _NHits, _Pthit);
          ptrout->_Chi2WithoutMS = _Chi2WithoutMS;
          ptrout->_Chi2StrLine = _Chi2StrLine;
          ptrout->_RigidityWithoutMS = _RigidityWithoutMS;
  }
  return ptrout;

}

///////////////////////////////////////////////////////////
void AMSTrTrack::remove_track(AMSTrTrack* &ptrack){
  if (ptrack) {
    for(int i=0;;i++){
      AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrTrack",i);
      if(pctr==NULL) break;
      for (AMSlink* ptr=pctr->gethead(); ptr!=NULL; ptr=ptr->next()){
        if (ptr->next()==ptrack) {
          pctr->removeEl(ptr,1);
          printf("DROPPING %x element in %x!!!\n",ptr,pctr);
          break; 
        }
      }
    }

    delete ptrack;  
    ptrack = NULL;
  }

}

///////////////////////////////////////////////////////////
integer AMSTrTrack::next_combination(
                int index_min, int index_max, number par[2][3]){
         if (index_min<0) return 0;
         if (index_max>=TKDBc::patpoints(_Pattern)) return 0;
	   int index_run = index_min;
next_index:
         _Pthit[index_run] = _Pthit[index_run]->nextgood_path(par);
         if (!_Pthit[index_run]){
	       if (index_run>=index_max) return 0;
             _Pthit[index_run] = AMSTrRecHit::firstgood_path(_Pattern,index_run,par);
             index_run++;
	       goto next_index;
         }

	   return 1;
}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSTrRecHit::firstgood(integer pattern, integer index){

	 integer layer = TKDBc::patconf(pattern,index)-1;
       AMSTrRecHit* phit = AMSTrRecHit::gethead(layer);

       for (;phit!=NULL;phit=phit->next()) {
         if (!phit->Good_PI()) continue;
         if (phit->checkstatus(AMSDBc::WEAK)) continue;
         //if (phit->checkstatus(AMSDBc::S_AMBIG) 
         if (phit->checkstatus(AMSDBc::USED) 
            && phit->getLayer()>AMSTrTrack::_max_ambigous_hits) 
                                                            continue;
         break;
       }

       return phit;

}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSTrRecHit::firstgood_path(integer pattern, integer index,number par[2][3]){

	 integer layer = TKDBc::patconf(pattern,index)-1;
       AMSTrRecHit* phit = AMSTrRecHit::gethead(layer);

       for (;phit!=NULL;phit=phit->next()) {
         if (!phit->Good_PI()) continue;
         if (phit->checkstatus(AMSDBc::WEAK)) continue;
         //if (phit->checkstatus(AMSDBc::S_AMBIG) 
         if (phit->checkstatus(AMSDBc::USED) 
            && phit->getLayer()>AMSTrTrack::_max_ambigous_hits) 
                                                            continue;
         if (!phit->is_in_path(par)) continue;
         break;
       }

       return phit;

}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSTrRecHit::nextgood(){

       AMSTrRecHit* phit = this;

       if (phit) phit = phit->next();
       for (;phit!=NULL;phit=phit->next()) {
         if (!phit->Good_PI()) continue;
         if (phit->checkstatus(AMSDBc::WEAK)) continue;
         //if (phit->checkstatus(AMSDBc::S_AMBIG) 
         if (phit->checkstatus(AMSDBc::USED) 
            && phit->getLayer()>AMSTrTrack::_max_ambigous_hits) 
                                                            continue;
         break;
       }

       return phit;

}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSTrRecHit::nextgood_path(number par[2][3]){

       AMSTrRecHit* phit = this;
       
       if (phit) phit = phit->next();
       for (;phit!=NULL;phit=phit->next()) {
         if (!phit->Good_PI()) continue;
         if (phit->checkstatus(AMSDBc::WEAK)) continue;
         //if (phit->checkstatus(AMSDBc::S_AMBIG) 
         if (phit->checkstatus(AMSDBc::USED) 
            && phit->getLayer()>AMSTrTrack::_max_ambigous_hits) 
                                                            continue;
         if (!phit->is_in_path(par)) continue;
         break;
       }

       return phit;

}

///////////////////////////////////////////////////////////
integer AMSTrTrack::next_combination_WEAK(
                int index_min, int index_max, number par[2][3]){
         if (index_min<0) return 0;
         if (index_max>=TKDBc::patpoints(_Pattern)) return 0;
	   int index_run = index_min;
next_index:
         _Pthit[index_run] = _Pthit[index_run]->nextgood_WEAK_path(par);
         if (!_Pthit[index_run]){
	       if (index_run>=index_max) return 0;
             _Pthit[index_run] = AMSTrRecHit::firstgood_WEAK_path(_Pattern,index_run,par);
             index_run++;
	       goto next_index;
         }

	   return 1;
}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSTrRecHit::firstgood_WEAK(integer pattern, integer index){

	 integer layer = TKDBc::patconf(pattern,index)-1;
       AMSTrRecHit* phit = AMSTrRecHit::gethead(layer);

       for (;phit!=NULL;phit=phit->next()) {
         if (!phit->checkstatus(AMSDBc::WEAK)) continue;
         if (!phit->Good_PI()) continue;
         //if (phit->checkstatus(AMSDBc::S_AMBIG) 
         if (phit->checkstatus(AMSDBc::USED) 
            && phit->getLayer()>AMSTrTrack::_max_ambigous_hits) 
                                                            continue;
         break;
       }

       return phit;

}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSTrRecHit::firstgood_WEAK_path(integer pattern, integer index,number par[2][3]){

	 integer layer = TKDBc::patconf(pattern,index)-1;
       AMSTrRecHit* phit = AMSTrRecHit::gethead(layer);

       for (;phit!=NULL;phit=phit->next()) {
         if (!phit->checkstatus(AMSDBc::WEAK)) continue;
         if (!phit->Good_PI()) continue;
         //if (phit->checkstatus(AMSDBc::S_AMBIG) 
         if (phit->checkstatus(AMSDBc::USED) 
            && phit->getLayer()>AMSTrTrack::_max_ambigous_hits) 
                                                            continue;
         if (!phit->is_in_path(par)) continue;
         break;
       }

       return phit;

}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSTrRecHit::nextgood_WEAK(){

       AMSTrRecHit* phit = this;

       if (phit) phit = phit->next();
       for (;phit!=NULL;phit=phit->next()) {
         if (!phit->checkstatus(AMSDBc::WEAK)) continue;
         if (!phit->Good_PI()) continue;
         //if (phit->checkstatus(AMSDBc::S_AMBIG) 
         if (phit->checkstatus(AMSDBc::USED) 
            && phit->getLayer()>AMSTrTrack::_max_ambigous_hits) 
                                                            continue;
         break;
       }

       return phit;

}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSTrRecHit::nextgood_WEAK_path(number par[2][3]){

       AMSTrRecHit* phit = this;
       
       if (phit) phit = phit->next();
       for (;phit!=NULL;phit=phit->next()) {
         if (!phit->checkstatus(AMSDBc::WEAK)) continue;
         if (!phit->Good_PI()) continue;
         //if (phit->checkstatus(AMSDBc::S_AMBIG) 
         if (phit->checkstatus(AMSDBc::USED) 
            && phit->getLayer()>AMSTrTrack::_max_ambigous_hits) 
                                                            continue;
         if (!phit->is_in_path(par)) continue;
         break;
       }

       return phit;

}

///////////////////////////////////////////////////////////
void AMSTrTrack::flag_golden_tracks(){

// Go ahead...
   int nfound = 0;
   AMSTrTrack* ptrack[Vtxconst::maxtr];

// Add up tracks
   int maxtracks=Vtxconst::maxtr;

//Clear S-ambiguities first
      AMSTrTrack *ptr = (AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",0);
      for (;ptr!=NULL && nfound<maxtracks;ptr = ptr->next()) {
        for (int i=0; i<ptr->getnhits(); i++){
           AMSTrRecHit* phit = ptr->getphit(i);
           if (!phit) continue;
           phit->clearstatus(AMSDBc::S_AMBIG);
        } 
      } 

// First pass (only tracks with beta)
   ptr = (AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",0);
   for (;ptr!=NULL && nfound<maxtracks;ptr = ptr->next()) {
      if (strstr(AMSJob::gethead()->getsetup(),"AMS02")) {
            if (ptr->checkstatus(AMSDBc::WEAK)) continue;
            if (ptr->checkstatus(AMSDBc::FalseTOFX)) continue;
      } else {
            //if (ptr->checkstatus(AMSDBc::WEAK)) continue;
            if (ptr->checkstatus(AMSDBc::FalseTOFX)) continue;
      }

//Beta check
      bool track_has_beta = false;
      for(int patb=0; patb<npatb; patb++){
        AMSBeta *pbeta = (AMSBeta*)AMSEvent::gethead()->getheadC("AMSBeta",patb);
        for (;pbeta!=NULL;pbeta = pbeta->next()) {
          if (pbeta->getptrack()==ptr) {
            track_has_beta = true;
            goto exit_beta;
          }
        }
      }
exit_beta:
      if (!track_has_beta) continue;

//Set S-ambiguities with respect to this track
      for (int i=0; i<ptr->getnhits(); i++){
         AMSTrRecHit* phit = ptr->getphit(i);
         if (!phit) continue;
         AMSTrCluster* py = phit->getClusterP(1);
         if (py){
           AMSTrRecHit* paux = AMSTrRecHit::gethead(phit->getLayer()-1);
           while (paux) {
                if (py==paux->getClusterP(1)) paux->setstatus(AMSDBc::S_AMBIG);
                paux = paux->next();
           }
         }
      }

      ptrack[nfound] = ptr;
      nfound++;
   }

// Second pass (recover non-ambiguous tracks without beta)
   ptr = (AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",0);
   for (;ptr!=NULL && nfound<maxtracks;ptr = ptr->next()) {
      if (strstr(AMSJob::gethead()->getsetup(),"AMS02")) {
            if (ptr->checkstatus(AMSDBc::WEAK)) continue;
            if (ptr->checkstatus(AMSDBc::FalseTOFX)) continue;
      } else {
            //if (ptr->checkstatus(AMSDBc::WEAK)) continue;
            if (ptr->checkstatus(AMSDBc::FalseTOFX)) continue;
      }

//Beta check
      bool track_has_beta = false;
      for(int patb=0; patb<npatb; patb++){
        AMSBeta *pbeta = (AMSBeta*)AMSEvent::gethead()->getheadC("AMSBeta",patb);
        for (;pbeta!=NULL;pbeta = pbeta->next()) {
          if (pbeta->getptrack()==ptr) {
            track_has_beta = true;
            goto exit_nobeta;
          }
        }
      }
exit_nobeta:
      if (track_has_beta) continue;

//Count S-ambiguities if no beta is found; reject track if "too ambiguous"
      int ns_amb = 0;
      for (int i=0; i<ptr->getnhits(); i++){
           AMSTrRecHit* phit = ptr->getphit(i);
           if (!phit) continue;
           if (phit->checkstatus(AMSDBc::S_AMBIG)) ns_amb++;
      } 
      if (ns_amb>AMSTrTrack::_max_ambigous_hits) continue;

//Set S-ambiguities with respect to this track
      for (int i=0; i<ptr->getnhits(); i++){
         AMSTrRecHit* phit = ptr->getphit(i);
         if (!phit) continue;
         AMSTrCluster* py = phit->getClusterP(1);
         if (py){
           AMSTrRecHit* paux = AMSTrRecHit::gethead(phit->getLayer()-1);
           while (paux) {
                if (py==paux->getClusterP(1)) paux->setstatus(AMSDBc::S_AMBIG);
                paux = paux->next();
           }
         }
      }

      ptrack[nfound] = ptr;
      nfound++;
   }

// Mark Golden tracks
   for (int i=0; i<nfound; i++) ptrack[i]->setstatus(AMSDBc::GOLDEN);

   return;

}

