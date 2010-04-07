#include "trdrec.h"
#include "event.h"
#include "ntuple.h"
#include "extC.h"
#include"trigger302.h"
AMSTRDCluster * AMSTRDCluster::_Head[trdconst::maxlay]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
integer AMSTRDCluster::build(int rerun){
    number Thr1A=TRDCLFFKEY.Thr1A/TRDCLFFKEY.ADC2KeV;
    number Thr1S=TRDCLFFKEY.Thr1S/TRDCLFFKEY.ADC2KeV;
    number Thr1R=TRDCLFFKEY.Thr1R;
    number Thr1H=TRDCLFFKEY.Thr1H/TRDCLFFKEY.ADC2KeV;
    number adc[trdconst::maxtube+3];
    VZERO(adc,sizeof(adc)/sizeof(integer));
    for (int n=0;n<AMSTRDIdSoft::ncrates();n++){
     AMSTRDRawHit * ptr=
     (AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",n,2);  
     while(ptr){
       AMSTRDIdSoft id(ptr->getidsoft());
       integer ilay=id.getlayer();
       integer ilad=id.getladder();
       if(ptr->testlast()){
        adc[ptr->getidsoft().gettube()]=ptr->Amp()/(id.getgain()>0?id.getgain():1);
#ifdef __AMSDEBUG__
        if(id.getgain()<=0){
         cerr <<"AMSTRDCluster::build-E-zero gain for "<<id<<" "<<id.getgain()<<endl;
        }
#endif
        number ref=-FLT_MAX;
        int status=0;
        for(int i=0;i<trdconst::maxtube+1;i++){
         if(adc[i]<ref){
          if( adc[i]< adc[i+1] && adc[i+1]> Thr1A){
             status=AMSDBc::WIDE;
          }
          else if(adc[i+1]<adc[i+2] && adc[i+2]>Thr1A){
             status=AMSDBc::NEAR;
          }
          else {
            status=0;
          }
          // find left & right
          int hmul=0;
          int center=i-1;
          int left=0;
          int right=trdconst::maxtube;
          for(int k=center-1;k>=0;k--){
           id.upd(k);
           if(adc[k]<=0 || id.checkstatus(AMSDBc::BAD) || center-k>TRDCLFFKEY.MaxHitsInCluster){
            left=k+1;
            break;
           }
          }
          if(status==AMSDBc::WIDE || status==AMSDBc::NEAR){
            right=adc[i]==0?center:center+1;
          }
          else right=center+2;
          if(right>trdconst::maxtube-1)right=trdconst::maxtube-1;
          for( int k=right;k>center;k--){
           id.upd(k);
           if(adc[k]<=0 || id.checkstatus(AMSDBc::BAD)|| k-left>TRDCLFFKEY.MaxHitsInCluster)right--;
           else break;
          }
          i=right+1;
          number sum=0;
          number ssum=0;
          number pos=0;
          number rms=0;
          for (int j=left;j<right+1;j++){
           id.upd(j);
           if(status==AMSDBc::WIDE && j==right){
            if(adc[j]/2>Thr1H)hmul++;
            sum+=adc[j]/2;
            ssum+=id.getsig()*id.getsig();
            pos+=(j-center)*adc[j]/2;
            rms+=(j-center)*(j-center)*adc[j]/2;
            adc[j]=adc[j]/2;
           }            
           else{
            if(adc[j]>Thr1H)hmul++;
              sum+=adc[j];
              ssum+=id.getsig()*id.getsig();
              pos+=(j-center)*adc[j];
              rms+=(j-center)*(j-center)*adc[j];
              adc[j]=0;
           }
          }
          if(sum !=0){
           rms=sqrt(fabs(rms*sum-pos*pos))/sum; 
           pos=pos/sum;
           ssum=sqrt(ssum);
          }
          ref=-FLT_MAX;
          id.upd(center);
          AMSTRDIdGeom idg(id);
          AMSgvolume * pv=AMSJob::gethead()->getgeomvolume(idg.crgid());
          if(pv){
           AMSPoint coo(pv->getcooA(0),pv->getcooA(1),pv->getcooA(2));
           number rad=pv->getpar(1);
           number z=pv->getpar(2);
           AMSDir xdir(fabs(pv->getnrmA(0,0)),fabs(pv->getnrmA(1,0)),fabs(pv->getnrmA(2,0)));
           AMSDir ydir(fabs(pv->getnrmA(0,1)),fabs(pv->getnrmA(1,1)),fabs(pv->getnrmA(2,1)));
           AMSDir cdir=xdir[2]<ydir[2]?xdir:ydir;
           coo=coo+cdir*pos;           
           AMSDir zdir(fabs(pv->getnrmA(0,2)),fabs(pv->getnrmA(1,2)),fabs(pv->getnrmA(2,2)));
           ptr->setstatus(AMSDBc::USED);
           status=status | AMSDBc::GOOD;
           AMSEvent::gethead()->addnext(AMSID("AMSTRDCluster",ilay),new AMSTRDCluster(status,ilay,coo,rad,z,zdir,sum*TRDCLFFKEY.ADC2KeV,right-left+1,hmul,ptr)); 
          }
          else {
           cerr<< "AMSTRDCluster::build-S-Nogeomvolumefound "<<idg.crgid()<<endl;
          }          
         }
         else if(adc[i]>Thr1A){
          id.upd(i);
          if(id.checkstatus(AMSDBc::BAD)==0 && 
           id.getsig() < Thr1S && 
           adc[i]/id.getsig() >Thr1R){
           ref=adc[i];
          }
         }
       }
         VZERO(adc,sizeof(adc)/sizeof(integer));
      }
      else adc[ptr->getidsoft().gettube()]=ptr->Amp()/(id.getgain()>0?id.getgain():1);
        ptr=ptr->next();
      }
    }
return 1;
}



void AMSTRDCluster::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (checkstatus(AMSDBc::USED));
  if(AMSTRDCluster::Out(flag) ){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  TRDClusterNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trdcl();

   if (TrN->Ntrdcl>=MAXTRDCL) return;
   TrN->Status[TrN->Ntrdcl]=_status;
   TrN->Layer[TrN->Ntrdcl]=_layer;
   for(int i=0;i<3;i++)TrN->Coo[TrN->Ntrdcl][i]=_Coo[i];
   for(int i=0;i<3;i++)TrN->CooDir[TrN->Ntrdcl][i]=_CooDir[i];
   TrN->Multip[TrN->Ntrdcl]=_Multiplicity;
   TrN->HMultip[TrN->Ntrdcl]=_HighMultiplicity;
   TrN->EDep[TrN->Ntrdcl]=_Edep;
   TrN->pRawHit[TrN->Ntrdcl]=_pmaxhit->getpos();
   if(AMSTRDRawHit::Out(IOPA.WriteAll%10==1)){
      for(int i=0;i<_pmaxhit->getidsoft().getcrate();i++){
        AMSContainer *pc=AMSEvent::gethead()->getC("AMSTRDRawHit",i);
        TrN->pRawHit[TrN->Ntrdcl]+=pc->getnelem();
      }
   }
    else{
      //Write only USED hits
      for(int i=0;i<_pmaxhit->getidsoft().getcrate();i++){
        AMSTRDRawHit *ptr=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",i);
        while(ptr && ptr->checkstatus(AMSDBc::USED) ){
         TrN->pRawHit[TrN->Ntrdcl]++;
          ptr=ptr->next();
        }
      }
    }
   TrN->Ntrdcl++;
  }


}

void AMSTRDCluster::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
   TrdClusterR & ptr=AMSJob::gethead()->getntuple()->Get_evroot02()->TrdCluster(_vpos);
    if (_pmaxhit) ptr.fTrdRawHit= _pmaxhit->GetClonePointer();
    else ptr.fTrdRawHit=-1;
#endif
}

void AMSTRDCluster::_printEl(ostream &o){
o<<_layer<<" "<<"Multiplicity "<<_Multiplicity<<" Ampl "<<_Edep<<" Dir&Coo "<<_CooDir<<_Coo<<endl;
}







integer AMSTRDCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}


number AMSTRDCluster::getHit(uinteger orr){
// or - 0 - perp to tube orientation
// or - 1 - z
if(orr==0)return _Coo[0]*_CooDir[1]-_Coo[1]*_CooDir[0];
else return _Coo[2];
}


  number AMSTRDSegment::par[2]={0,0};
 AMSTRDCluster * AMSTRDSegment::phit[trdconst::maxhits];

integer AMSTRDSegment::build(int rerun){
  int nseg=0;
for(int iseg=0;iseg<TRDDBc::nlayS();iseg++){  
  bool      WeakCaseWanted=true;
  integer NTrackFound=-1;
  for ( pat=0;pat<TRDDBc::npatH(iseg);pat++){
    if(TRDDBc::patallowH(pat,iseg) || (TRDDBc::patallow2H(pat,iseg) && WeakCaseWanted)){
      int fp=TRDDBc::patpointsH(pat,iseg)-1;    
      // Try to make StrLine Fit
      integer first=TRDDBc::patconfH(pat,0,iseg)-1;
      integer second=TRDDBc::patconfH(pat,fp,iseg)-1;
      phit[0]=AMSTRDCluster::gethead(first);
      while( phit[0]){
       if(phit[0]->Good()){
       phit[fp]=AMSTRDCluster::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good()){
        par[0]=(phit[fp]-> getHit(0)-phit[0]-> getHit(0))/
               (phit[fp]-> getHit(1)-phit[0]-> getHit(1));
        par[1]=phit[0]-> getHit(0)-par[0]*phit[0]-> getHit(1);
        if(NTrackFound<0)NTrackFound=0;
        // Search for others
        integer npfound=_TrSearcher(1,iseg);
        if(npfound){
           NTrackFound++;
           if(TRDDBc::patallowH(pat,iseg))WeakCaseWanted=false;
         goto out;
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
if(NTrackFound>0)nseg++;
}

return nseg;
}







integer AMSTRDSegment::pat=0;
integer AMSTRDSegment::_TrSearcher(int icall,uinteger iseg){
           
           phit[icall]=AMSTRDCluster::gethead(TRDDBc::patconfH(pat,icall,iseg)-1);
           while(phit[icall]){
             if(phit[icall]->Good() && !Distance1D(par,phit[icall])){
              if(TRDDBc::patpointsH(pat,iseg) >icall+2){         
                return _TrSearcher(++icall,iseg); 
              }                
              else {
                // icall+2 point combination found
                if(_addnext(pat,TRDDBc::patpointsH(pat,iseg),iseg,phit)){
                  return TRDDBc::patpointsH(pat,iseg);
                }
              }
             }
             phit[icall]=phit[icall]->next();
           }
           return 0;


}


integer AMSTRDSegment::_addnext(integer pat, integer nhit, uinteger iseg, AMSTRDCluster* pthit[]){

#ifdef __UPOOL__
cerr<< "AMSTRSegment::_addnext-F-UPOOL not supported yet"<<endl;
abort();
#else
    AMSTRDSegment *ptrack=   new AMSTRDSegment(iseg,pat, nhit ,TRDDBc::oriseg(iseg),pthit);
#endif

    ptrack->Fit();
    if(ptrack->_Chi2< TRDRECFFKEY.Chi2StrLine){
          ptrack->_addnextR(iseg);
          return 1;
       }
    delete ptrack;  
    return 0;
}



void AMSTRDSegment::_addnextR(uinteger iseg){

         int i;
         // Mark hits as USED
         for( i=0;i<_NHits;i++){
           if(_pCl[i]->checkstatus(AMSDBc::USED))
            _pCl[i]->setstatus(AMSDBc::AMBIG);
           else _pCl[i]->setstatus(AMSDBc::USED);
         }
         // permanently add;
          AMSEvent::gethead()->addnext(AMSID("AMSTRDSegment",iseg),this);
}


bool AMSTRDSegment::Distance1D(number par[2], AMSTRDCluster *ptr){

   number maxdist=ptr->getEHit()*3;
   return fabs(par[1]+par[0]*ptr->getHit(1)-ptr->getHit(0))/
            sqrt(1+par[0]*par[0]) > ptr->getEHit()*3;
}


void AMSTRDSegment::Fit(){

number x=0;
number x2=0;
number y=0;
number xy=0;
for (int i=0;i<_NHits;i++){
 x+=_pCl[i]->getHit(1);
 y+=_pCl[i]->getHit(0);
 xy+=_pCl[i]->getHit(1)*_pCl[i]->getHit(0);
 x2+=_pCl[i]->getHit(1)*_pCl[i]->getHit(1);
}
 x/=_NHits;
 y/=_NHits;
 xy/=_NHits;
 x2/=_NHits;
 _FitPar[0]=(xy-x*y)/(x2-x*x);
 _FitPar[1]=y-_FitPar[0]*x;
 _Chi2=0;
 for(int i=0;i<_NHits;i++){
  number d=(_pCl[i]->getHit(0)-_FitPar[0]*_pCl[i]->getHit(1)-_FitPar[1]);
  _Chi2+=d*d/_pCl[i]->getEHit()/_pCl[i]->getEHit();
 }
 _Chi2/=_NHits-1;
}



void AMSTRDSegment::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (checkstatus(AMSDBc::USED));
  if(Out(flag) ){
#ifdef __WRITEROOT__
   AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
   TRDSegmentNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trdseg();
   if (TrN->Ntrdseg>=MAXTRDSEG) return;
   TrN->Status[TrN->Ntrdseg]=_status;
   TrN->Orientation[TrN->Ntrdseg]=_Orientation;
   TrN->Chi2[TrN->Ntrdseg]=_Chi2;
   TrN->Pattern[TrN->Ntrdseg]=_Pattern;
   TrN->Nhits[TrN->Ntrdseg]=_NHits;
   for(int i=0;i<2;i++)TrN->FitPar[TrN->Ntrdseg][i]=_FitPar[i];

   for (int i=0;i<trdconst::maxhits;i++)TrN->PCl[TrN->Ntrdseg][i]=0;
   for (int i=0;i<_NHits;i++){
    TrN->PCl[TrN->Ntrdseg][i]=_pCl[i]->getpos();
   if(AMSTRDCluster::Out(IOPA.WriteAll%10==1)){
      for(int j=0;j<_pCl[i]->getlayer();j++){
        AMSContainer *pc=AMSEvent::gethead()->getC("AMSTRDCluster",j);
        TrN->PCl[TrN->Ntrdseg][i]+=pc->getnelem();
      }
   }
    else{
      //Write only USED hits
      for(int j=0;j<_pCl[i]->getlayer();j++){
        AMSTRDCluster *ptr=(AMSTRDCluster*)AMSEvent::gethead()->getheadC("AMSTRDCluster",j);
        while(ptr && ptr->checkstatus(AMSDBc::USED) ){
         TrN->PCl[TrN->Ntrdseg][i]++;
          ptr=ptr->next();
        }
      }
    }
   }
   TrN->Ntrdseg++;
  }

}

void AMSTRDSegment::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
  TrdSegmentR & ptr =AMSJob::gethead()->getntuple()->Get_evroot02()->TrdSegment(_vpos); 
   
    for (int i=0; i<_NHits; i++) {
      if (_pCl[i]) ptr.fTrdCluster.push_back(_pCl[i]->GetClonePointer());
    }
#endif
}

void AMSTRDSegment::_printEl(ostream &o){
o <<_Pattern<<" Nhits "<<_NHits<<endl;
}

integer AMSTRDSegment::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDSegment",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}


 AMSTRDCluster * AMSTRDCluster::gethead(uinteger i){
   if(i<TRDDBc::nlay()){
    if(!_Head[i])_Head[i]=(AMSTRDCluster*)AMSEvent::gethead()->getheadC("AMSTRDCluster",i,1);
    return _Head[i];
   }
   else {
#ifdef __AMSDEBUG__
    cerr <<"AMSTRDCluster:gethead-S-Wrong Head "<<i;
#endif
    return 0;
   }   
}

 AMSTRDSegment * AMSTRDSegment::gethead(uinteger i){
   if(i<TRDDBc::nlayS()){
    return (AMSTRDSegment*)AMSEvent::gethead()->getheadC("AMSTRDSegment",i,1);
   }
   else {
#ifdef __AMSDEBUG__
    cerr <<"AMSTRDSegment:gethead-S-Wrong Head "<<i;
#endif
    return 0;
   }   
}


integer AMSTRDTrack::build(int rerun){

_Start();


  
    int nrh=0;
    for(int i=0;i<TRDDBc::nlayS();i++){
     nrh+= (AMSEvent::gethead()->getC("AMSTRDSegment",i))->getnelem();
    }
    if(nrh>=TRDFITFFKEY.MaxSegAllowed){
      cout <<" Too many segments "<<nrh<<endl;
     TriggerLVL302 *plvl3;
     plvl3 = (TriggerLVL302*)AMSEvent::gethead()->getheadC("TriggerLVL3",0);
      if(!plvl3 || plvl3->skip()){
       AMSEvent::gethead()->seterror();
       return 0;
     }
      cout <<" but lvl3 says continue "<<endl;
    }
   
  

integer NTrackFound=-1;
bool      WeakCaseWanted=false;
for (pat=0;pat<TRDDBc::npatS();pat++){
    if(TRDDBc::patallowS(pat) || (TRDDBc::patallow2S(pat) && WeakCaseWanted)){
      int fp=TRDDBc::patpointsS(pat)-1;    
      // Try to make StrLine Fit
      integer first=TRDDBc::patconfS(pat,0)-1;
      integer second=TRDDBc::patconfS(pat,fp)-1;
      phit[0]=AMSTRDSegment::gethead(first);
      while( phit[0]){
       if(phit[0]->Good()){
       phit[fp]=AMSTRDSegment::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good()){
        if(NTrackFound<0)NTrackFound=0;
        // Search for others
        integer npfound=_TrSearcher(1);
        if(npfound){
           NTrackFound++;
           if(TRDDBc::patallowS(pat))WeakCaseWanted=false;
         goto out;
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
   //  Add proj coord to corr trd clusters
    for(  AMSTRDTrack* ptr=(AMSTRDTrack*)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,0);ptr;ptr=ptr->next()){
      for(int i=0;i<ptr->_Base._NHits;i++){
        int proj=ptr->_Base._PCluster[i]->getCooDir()[0]>0.9?0:1;
        AMSDir dir(ptr->_Real._Theta,ptr->_Real._Phi);
//        cout <<  proj<< "  "<<ptr->_Base._PCluster[i]->getCoo()[0]<<" "<<ptr->_Base._PCluster[i]->getCoo()[1]<<" ";
        ptr->_Base._PCluster[i]->getCoo()[proj]=ptr->_Real._Coo[proj]+dir[proj]/dir[2]*(ptr->_Base._PCluster[i]->getCoo()[2]-ptr->_Real._Coo[2]);
//        cout <<ptr->_Base._PCluster[i]->getCoo()[proj]<<endl;
      }
    }

return NTrackFound;
}



void AMSTRDTrack::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (checkstatus(AMSDBc::USED));
  if(Out(flag) ){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  TRDTrackNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trdtrk();

  if (TrN->Ntrdtrk>=MAXTRDTRK) return;
   TrN->Status[TrN->Ntrdtrk]=_status;
   for (int i=0;i<3;i++)TrN->Coo[TrN->Ntrdtrk][i]=_StrLine._Coo[i];
   for (int i=0;i<3;i++)TrN->ErCoo[TrN->Ntrdtrk][i]=_StrLine._ErCoo[i];
   TrN->Theta[TrN->Ntrdtrk]=_StrLine._Theta;
   TrN->Phi[TrN->Ntrdtrk]=_StrLine._Phi;
   TrN->Chi2[TrN->Ntrdtrk]=_StrLine._Chi2;
   TrN->NSeg[TrN->Ntrdtrk]=_BaseS._NSeg;
   TrN->Pattern[TrN->Ntrdtrk]=_BaseS._Pattern;

   for (int i=0;i<TRDDBc::nlayS();i++)TrN->pSeg[TrN->Ntrdtrk][i]=0;
   for (int i=0;i<_BaseS._NSeg;i++){
    TrN->pSeg[TrN->Ntrdtrk][i]=_BaseS._PSeg[i]->getpos();
   if(AMSTRDSegment::Out(IOPA.WriteAll%10==1)){
      for(int j=0;j<_BaseS._PSeg[i]->getslayer();j++){
        AMSContainer *pc=AMSEvent::gethead()->getC("AMSTRDSegment",j);
        TrN->pSeg[TrN->Ntrdtrk][i]+=pc->getnelem();
      }
   }
    else{
      //Write only USED hits
      for(int j=0;j<_BaseS._PSeg[i]->getslayer();j++){
        AMSTRDSegment *ptr=(AMSTRDSegment*)AMSEvent::gethead()->getheadC("AMSTRDSegment",j);
        while(ptr && ptr->checkstatus(AMSDBc::USED) ){
         TrN->pSeg[TrN->Ntrdtrk][i]++;
          ptr=ptr->next();
        }
      }
    }
   }
   TrN->Ntrdtrk++;
  }

}

void AMSTRDTrack::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
  TrdTrackR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->TrdTrack(_vpos);
    for (int i=0; i<_BaseS._NSeg; i++) {
       if(_BaseS._PSeg[i])ptr.fTrdSegment.push_back(_BaseS._PSeg[i]->GetClonePointer()); 
    }
#endif
}

void AMSTRDTrack::_printEl(ostream &o){
o <<_Base._Pattern<<" Nhits "<<_Base._NHits<<endl;
}

integer AMSTRDTrack::pat=0;
 AMSTRDSegment * AMSTRDTrack::phit[trdconst::maxseg];

integer AMSTRDTrack::_TrSearcher(int icall){
           if( _NoMoreTime()){
            throw amsglobalerror(" AMSTRDTrack::_TrSearcher-E-Cpulimit Exceeded ",2);
           }
           
           if(TRDDBc::patpointsS(pat)> icall+1)phit[icall]=AMSTRDSegment::gethead(TRDDBc::patconfS(pat,icall)-1);
           while(phit[icall]){
             if(phit[icall]->Good() ){
              if(TRDDBc::patpointsS(pat) >icall+2){         
                return _TrSearcher(++icall); 
              }                
              else{  
                // icall+2 point combination found
                if(_addnext(pat,TRDDBc::patpointsS(pat),phit)){
                  return TRDDBc::patpointsS(pat);
                }
                else if(TRDDBc::patpointsS(pat) <icall+2)return 0;
              }
             }
             phit[icall]=phit[icall]->next();
           }
           return 0;


}


integer AMSTRDTrack::_addnext(integer pat, integer nhit, AMSTRDSegment* pthit[]){

#ifdef __UPOOL__
cerr<< "AMSTRDTrack::_addnext-F-UPOOL not supported yet"<<endl;
abort();
#else
    AMSTRDTrack::TrackBaseS s(pat,nhit,pthit);
    AMSTRDTrack *ptrack=   new AMSTRDTrack(s);
#endif

    ptrack->StrLineFit();
    if(ptrack->_StrLine._FitDone && ptrack->_StrLine._Chi2< 2*TRDFITFFKEY.Chi2StrLine){
          ptrack->_addnextR();
          return 1;
       }
    delete ptrack;  
    return 0;
}



void AMSTRDTrack::_addnextR(){

         int i;
         // Mark hits as USED
         for( i=0;i<_BaseS._NSeg;i++){
           if(_BaseS._PSeg[i]->checkstatus(AMSDBc::USED))
            _BaseS._PSeg[i]->setstatus(AMSDBc::AMBIG);
           else _BaseS._PSeg[i]->setstatus(AMSDBc::USED);
         }
         RealFit();                  
         // permanently add;
          AMSEvent::gethead()->addnext(AMSID("AMSTRDTrack",0),this);
}


number AMSTRDTrack::Distance3D(AMSPoint p, AMSDir d, AMSTRDCluster *ptr){
  return 1.;
}



integer AMSTRDTrack::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDTrack",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}


extern "C" void e04ccf_(int &n, number xc[], number &fc, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);

void AMSTRDTrack::StrLineFit(){

    void (*palfun)(int &n, double xc[], double &fc, AMSTRDTrack *p)=&AMSTRDTrack::alfun;
    void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=&AMSTRDTrack::monit;
    _Base._NHits=0;    
     _StrLine._FitDone=false;
  // Fit Here
    const integer mp=40;
    number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];
    number yy=0;
    number xx=0;
    number yz=0;
    number xz=0;
    number yz2=0;
    number xz2=0;
    number nyz=0;
    number nxz=0;
   if(_BaseS._NSeg>2){
    for (int i=0;i<_BaseS._NSeg;i++){
     for(int k=0;k<_BaseS._PSeg[i]->getNHits();k++){
      if(fabs(_BaseS._PSeg[i]->getpHit(k)->getCooDir()[0])>0.9){
        yz+=-_BaseS._PSeg[i]->getFitPar(0);      
        yy+=-_BaseS._PSeg[i]->getFitPar(1);      
        yz2+=(_BaseS._PSeg[i]->getFitPar(0))*(_BaseS._PSeg[i]->getFitPar(0));          nyz++;
      }
      else if(fabs(_BaseS._PSeg[i]->getpHit(k)->getCooDir()[1])>0.9){
        xz+=_BaseS._PSeg[i]->getFitPar(0);      
        xx+=_BaseS._PSeg[i]->getFitPar(1);      
        xz2+=(_BaseS._PSeg[i]->getFitPar(0))*(_BaseS._PSeg[i]->getFitPar(0));          nxz++;
      }
      _Base._PCluster[_Base._NHits++]=_BaseS._PSeg[i]->getpHit(k);
     }
    } 
    if(_Base._NHits<TRDFITFFKEY.MinFitPoints)return;
    if(nxz){
     xz/=nxz;
     xx/=nxz;
     xz2/=nxz;
     if(sqrt(xz2-xz*xz)>TRDFITFFKEY.TwoSegMatch)return ;
    }     
    if(nyz){
     yz/=nyz;
     yy/=nyz;
     yz2/=nyz;
     if(sqrt(yz2-yz*yz)>TRDFITFFKEY.TwoSegMatch)return ;
    }     

    if(!nyz || !nxz){
     cerr<<"AMSTRDTrack::StrLineFit-F-TRDIsRotated-FindSmarterAlgorithmPlease"<<endl;
     abort();     
    }
   }

else{
     number yz,yy,xz,xx;
    for (int i=0;i<_BaseS._NSeg;i++){
      if(fabs(_BaseS._PSeg[i]->getpHit(0)->getCooDir()[0])>0.9){
        yz=-_BaseS._PSeg[i]->getFitPar(0);      
        yy=-_BaseS._PSeg[i]->getFitPar(1);      
      }
      else if(fabs(_BaseS._PSeg[i]->getpHit(0)->getCooDir()[1])>0.9){
        xz=_BaseS._PSeg[i]->getFitPar(0);      
        xx=_BaseS._PSeg[i]->getFitPar(1);      
      }
     for(int k=0;k<_BaseS._PSeg[i]->getNHits();k++){
      _Base._PCluster[_Base._NHits++]=_BaseS._PSeg[i]->getpHit(k);
     }
    } 
    if(_Base._NHits<TRDFITFFKEY.MinFitPoints)return;
}
    AMSPoint _Exit(0,0,_Base._PCluster[0]->getCoo()[2]);
    AMSPoint _Entry(0,0,_Base._PCluster[_Base._NHits-1]->getCoo()[2]);
    _Entry[0]=xx+xz*_Entry[2];
    _Exit[0]=xx+xz*_Exit[2];
    _Entry[1]=yy+yz*_Entry[2];
    _Exit[1]=yy+yz*_Exit[2];
    AMSDir Dir=_Exit-_Entry;
    AMSPoint Center=_Exit+_Entry;
    Center=Center*0.5;   
    x[0]=Center[0];
    x[1]=Center[1];
    x[2]=Center[2];
    x[3]=Dir.getphi();
    x[4]=Dir.gettheta();
      
  // number of parameters to fit
    integer n=5;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=1000;
    number tol=2.99e-2;
    int i,j;
    _update=false;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,(void*)this);
    if(ifail==0){
     _update=true;
     _StrLine._Coo[0]=x[0];
     _StrLine._Coo[1]=x[1];
     _StrLine._Coo[2]=x[2];
     alfun(n,x,f,this);
     _StrLine._Chi2=f;
     for(int i=0;i<3;i++)_StrLine._ErCoo[i]=_Base._PCluster[0]->getEHit()*sqrt
(_StrLine._Chi2/(_Base._NHits-2)+1.);
      AMSDir s(x[4],x[3]);
//    Default go in
     if(s[2]>0)s=s*(-1);
      number z2=(_Base._PCluster[0]->getCoo()[2]+_Base._PCluster[_Base._NHits-1]->getCoo()[2])/2;
      _StrLine._Coo=_StrLine._Coo+s*(z2-_StrLine._Coo[2])/s[2];
     _StrLine._Phi=s.getphi();
     _StrLine._Theta=s.gettheta();
     _StrLine._InvRigidity=0;
     _StrLine._FitDone=true;
    }
    else{
     _StrLine._Chi2=FLT_MAX;
    }
}


void AMSTRDTrack::alfun(integer &n, number xc[], number &fc, AMSTRDTrack *p){
  fc=0;
/*
  for (int i=3;i<6;i++){
   if(fabs(xc[i]>1)){
    fc=FLT_MAX;
    return;
   }
  }
*/
  integer nh=0;
  AMSDir sdir(xc[4],xc[3]);
  AMSPoint sp(xc[0],xc[1],xc[2]);
  for (int i=0;i<p->_Base._NHits;i++){
   if(!p->_Base._PCluster[i]->checkstatus(AMSDBc::DELETED)){
    nh++;
     if(!p->_update){
      AMSPoint dc=sp-p->_Base._PCluster[i]->getCoo();
      AMSDir gamma=sdir.cross(p->_Base._PCluster[i]->getCooDir());
     number d;
     if(gamma.norm()!=0){
      d=dc.prod(gamma);
     }
     else{
       AMSPoint beta= sdir.crossp(dc);
       d=beta.norm();
     }
      fc+=d*d/p->_Base._PCluster[i]->getEHit()/p->_Base._PCluster[i]->getEHit();
    }
    else{
     AMSPoint dc=sp-p->_Base._PCluster[i]->getCoo();
     AMSPoint alpha=sdir.crossp(p->_Base._PCluster[i]->getCooDir());
     AMSPoint beta= sdir.crossp(dc);
     number t=alpha.prod(beta)/alpha.prod(alpha);
     if(t>p->_Base._PCluster[i]->getHitL())t=p->_Base._PCluster[i]->getHitL();
     else if (t<-p->_Base._PCluster[i]->getHitL())t=-p->_Base._PCluster[i]->getHitL();
      p->_Base._Hit[i]=(p->_Base._PCluster[i]->getCoo()+p->_Base._PCluster[i]->getCooDir()*t);
      p->_Base._EHit[i]=AMSPoint(p->_Base._PCluster[i]->getEHit(),p->_Base._PCluster[i]->getEHit(),p->_Base._PCluster[i]->getEHit());
      AMSPoint dc2=sp-p->_Base._Hit[i];
      number d1=sdir.prod(dc2);
      d1=dc2.prod(dc2)-d1*d1;
      fc+=d1/p->_Base._PCluster[i]->getEHit()/p->_Base._PCluster[i]->getEHit();
      
    }
    }   
  }
  if(nh>1)fc/=nh-1;
}

void AMSTRDTrack::RealFit(){
_Real._FitDone=false;
integer fit=0;
integer ims=0;
  integer npt=_Base._NHits;
  const integer maxhits=trdconst::maxlay;
   geant hits[maxhits][3];
  geant sigma[maxhits][3];
   geant normal[maxhits][3];
   integer layer[maxhits];
  integer ialgo=1;
  geant out[9];
    for(int i=0;i<npt;i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
     layer[npt-1-i]=_Base._PCluster[i]->getlayer();
     for(int j=0;j<3;j++){
      hits[npt-1-i][j]=_Base._Hit[i][j];
      sigma[npt-1-i][j]=_Base._EHit[i][j];
     }
    }
int ipart=14;
TKFITG(npt,hits,sigma,normal,ipart,ialgo,ims,layer,out);
_Real._FitDone=true;
_Real._Chi2=out[6];
if(out[7] != 0)_Real._Chi2=FLT_MAX;
else{
_Real._FitDone=true;
_Real._Chi2=out[6];
}
_Real._InvRigidity=1/out[5];
_Real._ErrInvRigidity=out[8];
_Real._Theta=out[3];
_Real._Phi=out[4];
_Real._Coo=AMSPoint(out[0],out[1],out[2]);


}

geant AMSTRDTrack::_Time=0;
geant AMSTRDTrack::_TimeLimit=0;


bool AMSTRDTrack::_NoMoreTime(){
return _TimeLimit>0? _CheckTime()>_TimeLimit: _CheckTime()>AMSFFKEY.CpuLimit;
}



bool AMSTRDTrack::IsEcalCandidate(){

// very stupid function;
// should be replaced by smth more reasonable
   int hmul=0;
   int mul=0;
   for(int i=0;i<_Base._NHits;i++){
    mul+=_Base._PCluster[i]->getmult();
    hmul+=_Base._PCluster[i]->gethmult();
   }
   if(hmul>trdconst::maxlay/5 && hmul<trdconst::maxlay*4/5+1)return true;
   else return false;   

}


bool AMSTRDTrack::IsHighGammaTrack(){
 uinteger hmul=0;
 uinteger threshold=3;
 for (int i=0;i<_Base._NHits;i++){
  if(_Base._PCluster[i]->getmult()==1)hmul+=_Base._PCluster[i]->gethmult();
 }
 return hmul>threshold;
}




bool AMSTRDTrack::Veto(int last){
 for (int i=0;i<_Base._NHits;i++){
  if(_Base._PCluster[i]->getlayer()>last)return false;
 }
 return true;
}


