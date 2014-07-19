//  $Id: tralig.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "tralig.h"
#include "event.h"
#include <math.h>
#include "timeid.h"
#include "trrawcluster.h"
#include <float.h>
#include "mccluster.h"
#include "tkdbc.h"
#include "amsstl.h"
#include "event.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <signal.h>
using namespace trconst;
//PROTOCCALLSFSUB15(E04CCF,e04ccf,INT,DOUBLEV,DOUBLE,DOUBLE,INT,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,ROUTINE,ROUTINE,INT,INT)
//#define E04CCF(N,X,F,TOL,IW,W1,W2,W3,W4,W5,W6,ALFUN1,MONIT,MAXCAL,IFAIL) CCALLSFSUB15(E04CCF,e04ccf,INT,DOUBLEV,DOUBLE,DOUBLE,INT,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,ROUTINE,ROUTINE,INT,INT,N,X,F,TOL,IW,W1,W2,W3,W4,W5,W6,ALFUN1,MONIT,MAXCAL,IFAIL)

extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);

void AMSTrAligPar::_m2a(){

number l1=_Dir[0][0];
number m1=_Dir[0][1];
number n1=_Dir[0][2];
number l2=_Dir[1][0];
number m2=_Dir[1][1];
number n2=_Dir[1][2];
number l3=_Dir[2][0];
number m3=_Dir[2][1];
number n3=_Dir[2][2];
number roll,pitch,yaw;

if(sqrt(l1*l1+n1*n1)>FLT_MIN){

 roll=atan2(-m3,m2);
 pitch=atan2(n1,l1);
 yaw= fabs(cos(pitch))>FLT_MIN ? atan2(-m1,l1/cos(pitch)) :
                               atan2(-m1,n1/sin(pitch));
}
else{

cout <<" AMSTrAligPar::_m2a-W-MatrixSingular"<<endl;
yaw=atan2(-m1,0);
roll=0;
pitch=atan2(n2,l2); 
}
_Angles[0]=pitch;
_Angles[1]=yaw;
_Angles[2]=roll;

//Checkit

//#ifdef __AMSDEBUG__
_a2m();

if(fabs(l1-_Dir[0][0])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 0 0 "<<l1<<" " <<_Dir[0][0]<<endl;
if(fabs(m1-_Dir[0][1])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 0 1 "<<m1<<" " <<_Dir[0][1]<<endl;
if(fabs(n1-_Dir[0][2])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 0 2 "<<n1<<" " <<_Dir[0][2]<<endl;
if(fabs(l2-_Dir[1][0])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 1 0 "<<l2<<" " <<_Dir[1][0]<<endl;
if(fabs(m2-_Dir[1][1])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 1 1 "<<m2<<" " <<_Dir[1][1]<<endl;
if(fabs(n2-_Dir[1][2])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 1 2 "<<n2<<" " <<_Dir[1][2]<<endl;
if(fabs(l3-_Dir[2][0])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 2 0 "<<l3<<" " <<_Dir[2][0]<<endl;
if(fabs(m3-_Dir[2][1])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 2 1 "<<m3<<" " <<_Dir[2][1]<<endl;
if(fabs(n3-_Dir[2][2])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 2 2 "<<n3<<" " <<_Dir[2][2]<<endl;
//#endif

}

void AMSTrAligPar::_a2m(){
AMSPoint angle=_Angles;
number pitch=angle[0];
number yaw=angle[1];
number roll=angle[2];
  number cp=cos(pitch);
  number sp=sin(pitch);
  number cy=cos(yaw);
  number sy=sin(yaw);
  number cr=cos(roll);
  number sr=sin(roll);
  number l1=cy*cp;
  number m1=-sy;
  number n1=cy*sp;
  number l2=cr*sy*cp-sr*sp;
  number m2=cr*cy;
  number n2=cr*sy*sp+sr*cp;
  number l3=-sr*sy*cp-cr*sp;
  number m3=-sr*cy;
  number n3=-sr*sy*sp+cr*cp;
  _Dir[0]=AMSDir(l1,m1,n1);
  _Dir[1]=AMSDir(l2,m2,n2);
  _Dir[2]=AMSDir(l3,m3,n3);
}

void AMSTrAligPar::setpar(const AMSPoint & coo, const AMSPoint & angles){
_Coo=coo;
_Angles=angles;
// construct rot matrix
_a2m();
}


AMSTrAligPar::AMSTrAligPar(const AMSPoint & coo, const AMSPoint & angles){
_NEntries=0;
_Coo=coo;
_Angles=angles;
// construct rot matrix
_a2m();
}


AMSTrAligPar * AMSTrAligPar::SearchDB(uinteger address, integer & found, number db[2]){
  AMSTrAligPar::AMSTrAligDBEntry e(address);
  integer out=AMSbins(getdbtopp(),e,getdbentries());
  db[0]=0;
  db[1]=0;
  if(out >0){
    if((getdbtopp()+out-1)->status)found=1;
    else found=-1;
    db[0]=((getdbtopp()+out-1)->pav);
    db[1]=((getdbtopp()+out-1)->pav2);

    for(int i=0;i<trconst::maxlay;i++){
     *(getparp()+i)=AMSTrAligPar(getdbtopp()+out-1,i);
    }
    return getparp();
  }
  else {
    found=0;
    return 0;
  }
}

AMSTrAligPar::AMSTrAligPar(const AMSTrAligDBEntry * db, integer i){
_NEntries=0;
if(i>=0 && i<trconst::maxlay){
  _Coo=AMSPoint (db->coo[i][0],db->coo[i][1],db->coo[i][2]);
  _Angles=AMSPoint (db->angle[i][0],db->angle[i][1],db->angle[i][2]);
  _a2m();
}
else cerr <<"AMSTrAligPar::AMSTrAligPar-S-Wrong plane "<<i<<endl;
}




void AMSTrAligPar::UpdateDB(uinteger address,  AMSTrAligPar  o[], number fcni, number fcn, number pav, number pav2){
   
if(AMSFFKEY.Update){
    if(DbIsNotLocked())LockDB();
    // read latest db into memory
       AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVDB());
       time_t begin,end,insert;
       ptdv->gettime(insert,begin,end);
       ptdv->SetTime(insert,end,begin);
       time_t asktime;
       if(AMSEvent::gethead())asktime=AMSEvent::gethead()->gettime();
       else asktime=0;
       if(!ptdv->readDB(AMSDATADIR.amsdatabase,asktime)){
        cerr<<"AMSTrAligFit-F-ProblemToReadDB "<<*ptdv;
        UnlockDB();
        exit(1);
       }
}
 //  if (getdbentries()>= maxdbentries()){
   if (0){
     cerr<<"AMSTrAligPar::UpdateDB-E-DBIsFull "<<endl;
     return ;
   }
   else{
     // ItSelf
{
    AMSTrAligPar::AMSTrAligDBEntry e(address,1,o,fcni,fcn,pav,pav2);
    int out=AMSbins(getdbtopp(),e, getdbentries());
    int lexplicit=0;
    if(out>0)lexplicit=(getdbtopp()+out-1)->status;
    if(out>0 && TRALIG.ReWriteDB==0 && lexplicit==1  ){
     cerr<<"AMSTrAligPar::UpdateDB-E-ObjectAlreadyExists "<<address<<" "<<getdbentries()<<endl;
    }
    else if(out<=0){
     if(getdbentries()< maxdbentries()){
      *(getdbtopp()+getdbentries())=e;
      incdbentries();
      AMSsortNAGa(getdbtopp(),getdbentries());
      cout<<"AMSTrAligPar::UpdateDB-I-NewEntryAdded "<<address<<" "<<getdbentries()<<endl;
    }
    else{
     cerr<<"AMSTrAligPar::UpdateDB-E-DBIsFull "<<endl;
    }     
    }
    else{
     *(getdbtopp()+out-1)=e;
     cout<<"AMSTrAligPar::UpdateDB-I-EntryReplaced"<<address<<" "<<out-1<<endl;
    }
}
    // childs

    uinteger nchild;
    uinteger * pimplicit=AMSTrTrack::getchild(address,nchild);
    for(int i=0;i<nchild;i++){
     uinteger addr=pimplicit[i];
     AMSTrAligPar::AMSTrAligDBEntry ec(addr,0,o,fcni,fcn,pav,pav2);
    int out=AMSbins(getdbtopp(),ec, getdbentries());
    int lexplicit=0;
    if(out>0)lexplicit=(getdbtopp()+out-1)->status;
    if(out>0 && (TRALIG.ReWriteDB==0 || lexplicit==1)){
     cerr<<"AMSTrAligPar::UpdateDB-E-ImpObjectAlreadyExists "<<pimplicit[i]<<" "<<getdbentries()<<endl;
    }
    else if(out<=0){
     if(getdbentries()< maxdbentries()){
      *(getdbtopp()+getdbentries())=ec;
      incdbentries();
      AMSsortNAGa(getdbtopp(),getdbentries());
      cout<<"AMSTrAligPar::UpdateDB-I-NewImpEntryAdded "<<pimplicit[i]<<" "<<getdbentries()<<endl;
     }
     else {
      cerr<<"AMSTrAligPar::UpdateDB-E-DBIsFull "<<endl;
     }     
    }
    else{
     *(getdbtopp()+out-1)=ec;
     cout<<"AMSTrAligPar::UpdateDB-I-EntryReplaced"<<pimplicit[i]<<" "<<out-1<<endl;
    }

    }


   }
  // UpdateDB

    if (AMSFFKEY.Update ){
       AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVDB());
             ptdv->UpdCRC();
             if( !ptdv->write(AMSDATADIR.amsdatabase)){
                cerr <<"AMSTrAligPar::updateDB-F-ProblemtoUpdate "<<*ptdv;
                UnlockDB();
                exit(1);
             }              
     UnlockDB();
    }
   return ;
}


// Here AMSTrAligFit

AMSTrAligFit::AMSTrAligFit():_Address(0),_Pattern(0),_NData(0),_PositionData(0),
_pData(0),_PlaneNo(0),_HalfNo(0),_LadderNo(0),_ParNo(0),AMSNode(AMSID("AMSTrAligFit",0)){
  int i;
  for(i=0;i<trconst::maxlay;i++){
   _pParC[i]=AMSTrAligPar();
  }
  for(int k=0;k<sizeof(chi2)/sizeof(chi2[0]);k++){
   chi2[k]=chi2i[k]=0;
  }

}

AMSTrAligFit::AMSTrAligFit(uinteger address, integer pattern, integer ndatmax,
          integer alg, integer nodeno):_Address(address),_Pattern(pattern),
          _NData(ndatmax),_Algorithm(alg),_PositionData(0),AMSNode(AMSID("AMSTrAligFit",nodeno)){
          _pData=new AMSTrAligData[ndatmax];  
            assert(_pData !=  NULL);             
if(_Address==1){

// global Fit

  int i;
  for(i=0;i<maxlad;i++){
   for(int j=0;j<2;j++){
    for(int k=0;k<trconst::maxlay;k++){   
       if(TRALIG.LayersOnly)_pPargl[i][j][k].NEntries()=0;
       else _pPargl[i][j][k].NEntries()=-_gldb[i][j][k].nentries;
       if(_pPargl[i][j][k].NEntries())cout <<i<<" "<<j<<" "<<k<<" "<<_pPargl[i][j][k].NEntries()<<endl;
    }
   }
  }



_NoActivePar=0;
for( i=0;i<trconst::maxlay;i++){
  int nprp=0;
  for(int j=0;j<6;j++){
    if(TRALIG.ActiveParameters[i][j]){
      nprp++;
    }
  }
 _NoActivePar+=nprp;
}


// Find No of ladders
int nladders=0;
for(i=0;i<TKDBc::nlay();i++){
 for(int j=0;j<maxlad;j++){
  for(int s=0;s<2;s++){
    if(TKDBc::activeladdshuttle(i+1,j+1,s))nladders++;
  }
 }
}
_NoActivePar*=nladders;
_NoActivePar/=trconst::maxlay;
cout <<"GlobalFit-I-NoActivePar "<<_NoActivePar<<" "<<nladders<<endl;

_PlaneNo= new integer[_NoActivePar];
_ParNo= new integer[_NoActivePar];
_LadderNo=new integer[_NoActivePar];
_HalfNo=new integer[_NoActivePar];
_NoActivePar=0;

    for( i=0;i<trconst::maxlay;i++){
      for(int l=0;l<maxlad;l++){
       for(int m=0;m<2;m++){
         if(TKDBc::activeladdshuttle(i+1,l+1,m)){
          int nprp=0;
          for(int j=0;j<6;j++){
           if(TRALIG.ActiveParameters[i][j]){
            _PlaneNo[_NoActivePar+nprp]=i;
            _ParNo[_NoActivePar+nprp]=j;
            _LadderNo[_NoActivePar+nprp]=l;
            _HalfNo[_NoActivePar+nprp]=m;
            nprp++;
           }
          }
          _NoActivePar+=nprp;
         }
       }
      }
    }


}
else{
  int i;
  for(i=0;i<trconst::maxlay;i++){
   _pParC[i]=AMSTrAligPar();
  }
_NoActivePar=0;

for(i=0;i<trconst::maxlay;i++){
 for(int k=1;k<trconst::maxlay;k++){
 if(i==TKDBc::patconf(pattern,k)-1){
  int nprp=0;
  for(int j=0;j<6;j++){
    if(TRALIG.ActiveParameters[i][j]){
//       _PlaneNo[_NoActivePar+nprp]=i;
//       _ParNo[_NoActivePar+nprp]=j;
       nprp++;
    }
  }
 _NoActivePar+=nprp;
 break;
 }
}
}
_PlaneNo= new integer[_NoActivePar];
_ParNo= new integer[_NoActivePar];
_LadderNo=0;
_HalfNo=0;
_NoActivePar=0;
for(i=0;i<trconst::maxlay;i++){
 for(int k=1;k<trconst::maxlay;k++){
 if(i==TKDBc::patconf(pattern,k)-1){
  int nprp=0;
  for(int j=0;j<6;j++){
    if(TRALIG.ActiveParameters[i][j]){
       _PlaneNo[_NoActivePar+nprp]=i;
       _ParNo[_NoActivePar+nprp]=j;
       nprp++;
    }
  }
 _NoActivePar+=nprp;
 break;
 }
}
}
//cout << " pattern " <<pattern <<" "<<_NoActivePar<<endl;



}


}


AMSTrAligFit::~AMSTrAligFit(){
 delete [] _pData;
 delete [] _PlaneNo;
 delete [] _LadderNo;
 delete [] _HalfNo;
 delete [] _ParNo;
}

void AMSTrAligFit::Test(integer forced){

AMSNode *pnode =AMSJob::gethead()->getaligstructure();
AMSTrAligFit * pal = (AMSTrAligFit *)pnode->down();

if(forced==0){
 AMSParticle * ptr=0;
 AMSmceventg * ptrg=0;
  if(Select(ptr,ptrg,TRALIG.Algorithm)){
     AMSTrTrack * ptrack= ptr->getptrack();
     integer nnodes=0;
     integer found=0;
      while(pal){
        nnodes++;
        if(pal->AddressOK(ptrack->getaddress(),int(TRALIG.Cuts[9][0]))){
           if(pal->_PositionData<pal->_NData)(pal->_pData[(pal->_PositionData)++]).Init(ptr,ptrg);
           if(pal->AddressOK(ptrack->getaddress(),1))found=1;
           else if(found==0)found=-1;
        }
        pal=pal->next(); 
     }
     if(found==1)return; 
      if(nnodes<TRALIG.MaxPatternsPerJob){
           integer found=0;
           number db[2];
           AMSTrAligPar::SearchDB(ptrack->getaddress(),found,db);
           if(TRALIG.ReWriteDB  || found<=0){
            pal=new AMSTrAligFit(ptrack->getaddress(),
            ptrack->getpattern(), TRALIG.MaxEventsPerFit, TRALIG.Algorithm, nnodes+1);
            if(pal->_PositionData<pal->_NData)(pal->_pData[(pal->_PositionData)++]).Init(ptr,ptrg);
            pnode->add(pal);       
            cout <<"AMSTrAligFit::Test-I-NewPatternAdded " <<ptrack->getaddress()<<" "<<TRALIG.Algorithm<<endl;
           }
      //else cerr<<"AMSTrAligFit::Test-E-CouldNotAddPatternAlrdyExists " <<ptrack->getaddress()<<endl;
      }
      //else cerr<<"AMSTrAligFit::Test-E-CouldNotAddPatternMaxReached " <<ptrack->getaddress()<<endl;
  }
}


else {


   // Open Ntuple Fit/fill Ntuple & update Db

      while(pal){
         pal->Fit();
         pal=pal->next(); 
     }
  // close ntuple

  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//tralig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("tralig");
  CLOSEF(IOPA.hlun+1);




}
}




void AMSTrAligFit::Testgl(integer forced){
static int skip=0;

AMSNode *pnode =AMSJob::gethead()->getaligstructure();
AMSTrAligFit * pal = (AMSTrAligFit *)pnode->down();

if(forced==0){
 AMSParticle * ptr=0;
 AMSmceventg * ptrg=0;
  if(Select(ptr,ptrg,TRALIG.Algorithm)){
     AMSTrTrack * ptrack= ptr->getptrack();
     integer nnodes=0;
     integer found=0;
      while(pal){
        nnodes++;
        if(pal->AddressOK(1,1) ){
          int itst=GCFLAG.ITEST>10?GCFLAG.ITEST/10:1;        
           if(pal->_PositionData%itst==1)cout <<"AMSTrAligFit::Testgl-I "<<pal->_PositionData<<" events in memory "<<endl;
           if(pal->_PositionData%GCFLAG.ITEST==1){
             for(int ilay=0;ilay<maxlay;ilay++){
               for(int iside=0;iside<2;iside++){
                for(int ilad=0;ilad<maxlad;ilad++){
                 if(_pPargl[ilad][iside][ilay].NEntries()>0){
                   cout << ilay<<" "<<iside<<" "<<ilad<<" "<< _pPargl[ilad][iside][ilay].NEntries()<<endl;
                 }
                }
               }
             }
           }
           if(pal->_PositionData<pal->_NData){
            // UPdateGlobalParSpace;
             integer ladder[2][maxlay];
             AMSTrTrack::decodeaddress(ladder,ptrack->getaddress());
             int add=0;
             int i;
             for(i=0;i<maxlay;i++){
              if(ladder[0][i]){
                add+=_pPargl[ladder[0][i]-1][ladder[1][i]][i].AddOne();
              }
             }
             if(add)(pal->_pData[(pal->_PositionData)++]).Init(ptr,ptrg);
             else{
              for( i=0;i<maxlay;i++){
               if(ladder[0][i]){
                _pPargl[ladder[0][i]-1][ladder[1][i]][i].MinusOne();
               }
              }
             }
           }
           else if (TRALIG.LayersOnly){
              cout <<"AMSTrAligFit::Testgl-I-MaxEventNoReached.Terminating 0 "<<" " <<pal->_NData<<endl;
                Testgl(1);
           }
           else {
              cout <<"AMSTrAligFit::Testgl-I-MaxEventNoReached.Terminating 1 "<<pal->_NData<<endl;
              raise(SIGTERM);
           }
           if(skip++>TRALIG.EventsPerRun){
              //if(GCFLAG.IEORUN==0)GCFLAG.IEORUN=2;
              skip=0;
           }
           return;
        }
        pal=pal->next(); 
     }
      if(nnodes<TRALIG.MaxPatternsPerJob){
           integer found=0;
            pal=new AMSTrAligFit(1,0, TRALIG.MaxEventsPerFit, TRALIG.Algorithm, nnodes+1);
            if(pal->_PositionData<pal->_NData)(pal->_pData[(pal->_PositionData)++]).Init(ptr,ptrg);
            pnode->add(pal);       
            cout <<"AMSTrAligFit::TestGl-I-PatternAdded " <<1<<" "<<TRALIG.Algorithm<<endl;
           }
      }
  }



else {


   // Open Ntuple Fit/fill Ntuple & update Db

     if(pal){
         pal->Fitgl();
     }
     // remove node
     pnode->down()->remove();
     pnode->setDown(0);
if(forced>1){
 
  // close ntuple


  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//tralig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("tralig");
  CLOSEF(IOPA.hlun+1);

}


}
}



void AMSTrAligFit::Fit(){
    void (*palfun)(int &n, double x[], double &f, AMSTrAligFit *p)=&AMSTrAligFit::alfun;
    void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=
                                &AMSTrAligFit::monit;
      if(_PositionData<TRALIG.MinEventsPerFit){
        cerr << "AMSTrAligFit::Fit-E-TooFewEvents for "<<_Pattern<<" "<<_Address<<" "<<_PositionData<<endl;
        return;
      }
      cout << "AMSTrAligFit::Fit started for pattern "<<_Pattern<<" "<<_Address<<endl;
  // Fit Here
    const integer mp=40;
    number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];
  // number of parameters to fit
    integer n=_NoActivePar;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=1;
    number tol=2.99e-2;
    int i,j;
    for(i=0;i<mp;i++)x[i]=0;
    _flag=3;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
    for(i=0;i<mp;i++)x[i]=0;
     _pfitbefore=_tmppav;
     ifail=0;
     if(fabs(1-_pfitbefore)>TRALIG.GlobalGoodLimit){
      ifail=1;
      maxcal=2000;
      _flag=0;
      e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
      cout << "AMSTrAligFit::Fit finished "<<ifail<<" "<<f<<endl;
      if(ifail ==0 ){
       _flag=2;
       ifail=1;
       number fd;
       integer one(1);
       e04ccf_(n,x,fd,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);
       ifail=0;    
      }
     }
     if(ifail==0){
      AMSPoint outc[maxlay];
      AMSPoint outa[maxlay];
      for(i=0;i<trconst::maxlay;i++){
        outc[i]=0;
        outa[i]=0;
      }
      for(i=0;i<n;i++){
       int plane=_PlaneNo[i];
       int parno=_ParNo[i];
       if(parno<3)outc[plane][parno]=x[i];
       else outa[plane][parno-3]=x[i]*AMSDBc::pi/180.;
      }
 
      for(i=0;i<maxlay;i++){
        (_pParC[i]).setpar(outc[i],outa[i]);
      }
      _fcn=f;
      _fcnI=_tmp;
      _pfit=_tmppav;
       cout <<"tmpav "<<_tmppav<<" "<<_tmppsi<<endl;
      _pfits=_tmppsi;
      Anal();
     }
    else cerr <<"FIt-E-IfailNotZero "<<ifail<<endl;

 

}


void AMSTrAligFit::Fitgl(){
    void (*palfun)(int &n, double x[], double &f, AMSTrAligFit *p)=&AMSTrAligFit::alfungl;
    void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=
                                &AMSTrAligFit::monit;
      cout << "Global AMSTrAligFit::Fit started" <<endl;
  // Fit Here
  // number of parameters to fit
    const integer mp=999;
    number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];

   // rebuild noactivepar
    RebuildNoActivePar();


    integer n=_NoActivePar;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=25000;
    number tol=3.99e-2;
    int i,j;
    for(i=0;i<mp;i++)x[i]=0;
    _flag=3;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
    cout << "AMSTrAligFit::Fit finished "<<ifail<<" "<<f<<endl;
    if(ifail ==0 || ifail==2){
     _flag=2;
     ifail=1;
     number fd;
     integer one(1);
     e04ccf_(n,x,fd,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);
     AMSPoint outc[maxlad][2][maxlay];
     AMSPoint outa[maxlad][2][maxlay];
     for(i=0;i<maxlad;i++){ 
     for(j=0;j<2;j++){
     for(int k=0;k<maxlay;k++){
       outc[i][j][k]=AMSPoint(0,0,0);
       outa[i][j][k]=AMSPoint(0,0,0);
     }
     }
     }

     for(i=0;i<n;i++){
      int plane=_PlaneNo[i];
      int ladder=_LadderNo[i];
      int half=_HalfNo[i];
      int parno=_ParNo[i];
      if(parno<3)outc[ladder][half][plane][parno]=x[i];
      else outa[ladder][half][plane][parno-3]=x[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<maxlad;i++){
     for(j=0;j<2;j++){
     for(int k=0;k<trconst::maxlay;k++){
       (_pPargl[i][j][k]).setpar(outc[i][j][k],outa[i][j][k]);
     }
     }
     }
     _fcn=f;
     _fcnI=_tmp;
     _pfit=_tmppav;
      cout <<"tmpav "<<_tmppav<<" "<<_tmppsi<<endl;
     _pfits=_tmppsi;
     Analgl();
     }
    else cerr <<"FIt-E-IfailNotZero "<<ifail<<endl;

 

}


AMSTrAligPar::AMSTrAligDB AMSTrAligPar::_traldb;
AMSTrAligPar   AMSTrAligPar::par[trconst::maxlay];



void AMSTrAligFit::Anal(){

cout <<" AMSTrAligFit::Anal called for pattern "<<_Pattern<<" "<<_Address<<endl;
  static  AMSTrAligFit::TrAlig_def TRALIGG;
  static integer action=0;
  static integer upddone=0;
  if(action==0){
    action=1;
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);
    char filename[256];
    strcpy(filename,hfile);
    integer iostat;
    integer rsize=1024;
    char event[80];
    HROPEN(IOPA.hlun+1,"tralig",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening tralig ntuple file "<<filename<<endl;
    }
    else cout <<"tralig ntuple file "<<filename<<" opened."<<endl;

   HBNT(IOPA.ntuple+1,"Tracker Alignment"," ");
   HBNAME(IOPA.ntuple+1,"TrAlig",(int*)(&TRALIGG),"Pattern:I,Alg:I,Address:I,FCN:R,FCNI:R,CHI2(1000):R,CHI2I(1000):R,ndata:I,PFIT:R,PFITS:R,Coo(3,8):R,Angle(3,8):R");

  }
  int i,j,k,l;
      TRALIGG.Pattern=_Pattern;
      TRALIGG.Alg=_Algorithm;
      TRALIGG.Address=_Address;
      TRALIGG.FCN=_fcn;
      TRALIGG.FCNI=_fcnI;
      TRALIGG.Pfit=_pfit;
      TRALIGG.Pfitsig=_pfits;
    for(j=0;j<trconst::maxlay;j++){
      for(k=0;k<3;k++){
       TRALIGG.Coo[j][k]=_pParC[j].getcoo()[k];
       TRALIGG.Angle[j][k]=_pParC[j].getang()[k];
      }
    }
    HFNT(IOPA.ntuple+1);

   // updatedb
   // I need to convert _pParc to a local ladder level
   integer ladder[2][maxlay];
   AMSTrTrack::decodeaddress(ladder,_Address);
   for(int il=0;il<maxlay;il++){
    if(ladder[0][il]){
     integer sensor= ladder[1][il]==0? 1: TKDBc::nhalf(il+1,ladder[0][il])+1;
     AMSTrIdGeom amd(il+1,ladder[0][il],sensor,0,0);
     AMSgvolume * psen= AMSJob::gethead()->getgeom(amd.crgid());
     if(!psen){
       cerr<< " AMSTrAligPar::Anal-E-SensorNotfound "<<amd;
       return; 
     }
     else{
      psen=psen->up();
      AMSPoint coov;
      number coom[3][3];
      for (i=0;i<3;i++){
        coov[i]=psen->getcooA(i);
        for(j=0;j<3;j++){
         coom[i][j]=psen->getnrmA(i,j);
        }
      }

      number nrmN[3][3];
      AMSPoint cooN=_pParC[il].getcoo();
      for( j=0;j<3;j++){
        for( k=0;k<3;k++){
            AMSDir stipud_cxx=_pParC[il].getmtx(j);
            nrmN[j][k]=stipud_cxx[k];
        }
      }
      cooN=cooN+coov.mm3(nrmN);
      amsprotected::mm3(nrmN,coom,0);
      _pParC[il].setcoo(cooN);
      for( j=0;j<3;j++){
       _pParC[il].setmtx(j)= AMSDir(coom[j][0],coom[j][1],coom[j][2]);  
      }
     }
    }
    _pParC[il].updangles();
   }
   
   AMSTrAligPar::UpdateDB(_Address,_pParC,_fcnI,_fcn,_pfit,_pfitbefore);


}




void AMSTrAligFit::Analgl(){

cout <<" AMSTrAligFit::Analgl called for pattern "<<_Pattern<<" "<<_Address<<endl;
  static  AMSTrAligFit::TrAligg_def TRALIGG;
  static integer action=0;
  static integer upddone=0;
  if(action++==0){
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);
    char filename[256];
    strcpy(filename,hfile);
    integer iostat;
    integer rsize=1024;
    char event[80];
    HROPEN(IOPA.hlun+1,"tralig",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening tralig ntuple file "<<filename<<endl;
    }
    else cout <<"tralig ntuple file "<<filename<<" opened."<<endl;

   HBNT(IOPA.ntuple+1,"Tracker Alignment"," ");
   HBNAME(IOPA.ntuple+1,"TrAlig",(int*)(&TRALIGG),"Alg:I,Layer:I,Ladder:I,Side:I,FCN:R,FCNI:R,PFIT:R,PFITS:R,Coo(3):R,Angle(3):R,Stat:I");
 

  }
  int i,j,k,l;
   for(j=0;j<trconst::maxlay;j++){
     for(int nlad=0;nlad<maxlad;nlad++){
       for(int side=0;side<2;side++){
        if(TKDBc::activeladdshuttle(j+1,nlad+1,side )|| TRALIG.LayersOnly){
          TRALIGG.Alg=10000*_Algorithm+action-1;
          TRALIGG.Layer=j+1;
          TRALIGG.Ladder=nlad+1;
          TRALIGG.Side=side;
          TRALIGG.FCN=_fcn;
          TRALIGG.FCNI=_fcnI;
          TRALIGG.Pfit=_pfit;
          TRALIGG.Pfitsig=_pfits;
     for(int ll=0;ll<sizeof(chi2)/sizeof(chi2[0]);ll++){
      TRALIGG.CHI2[ll]=chi2[ll];
      TRALIGG.CHI2I[ll]=chi2i[ll];
     }
          TRALIGG.Stat=_pPargl[nlad][side][j].NEntries();
           for(k=0;k<3;k++){
            TRALIGG.Coo[k]=_pPargl[nlad][side][j].getcoo()[k];
            TRALIGG.Angle[k]=_pPargl[nlad][side][j].getang()[k];
           }
           HFNT(IOPA.ntuple+1);
          }
         }
        }
   }

  UpdateDBgl();


}




integer AMSTrAligFit::Select(AMSParticle * & ppart, AMSmceventg * & pmcg, integer alg){
      ppart=(AMSParticle*)AMSEvent::gethead()->getheadC("AMSParticle",0);
      pmcg=0;
      if(!ppart)return 0;      // Particle Should be Present always

  // Algorithm == 0 presumes momentum is knows,
  // pmcg may exist, if not create it from datacards
   if(alg==0){ 
         pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);  
         if(!pmcg){
             AMSPoint coo(CCFFKEY.coo[0],CCFFKEY.coo[1],CCFFKEY.coo[2]);
             AMSDir dir(CCFFKEY.dir[0],CCFFKEY.dir[1],CCFFKEY.dir[2]);
             pmcg=new AMSmceventg(GCKINE.ikine, CCFFKEY.momr[0], coo, dir);  
              AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), pmcg);   
         }
   }

   else if(alg==1){
// Algorithm == 1 presumes getting momentum from beta measurement & from gckine.ikine
 //    always create pmcg
      integer pid=ppart->getgpart();
      AMSBeta * pbeta=ppart->getpbeta();
      number beta=pbeta->getbeta();
       if(fabs(beta)<1. ){
         AMSPoint coo=ppart->gettofcoo(0);
         AMSDir dir(ppart->gettheta(),ppart->getphi());
         geant xmom=0;   
         pmcg= new AMSmceventg(GCKINE.ikine, xmom, coo, dir);  
         number gamma=1./sqrt(1-beta*beta);
         xmom=pmcg->getmass()*fabs(beta)*gamma;
         pmcg->setmom(xmom);
        AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), pmcg);  
       }
   }
   else if(alg==2){
    //  no magnetic field algo
   }
   else{
     cerr<<"AMSTrAligFit::Select-F-AlgNo "<<alg<<" IsNotSupportedYet"<<endl;
      exit(1);
   }
   // Apply Cuts   
   if(pmcg){
      if(alg==0){
        number charge=pmcg->getcharge();
        number momentum=pmcg->getmom();
        number beta=ppart->getpbeta()->getbeta();
        AMSDir dir=pmcg->getdir();
        number recharge=ppart->getcharge();
        if(ppart->getmomentum()<0)recharge=-recharge;
        if(recharge==charge){
           AMSTrTrack *ptr=ppart->getptrack();              
           number chi2_3d, chi2z, rid;
           ptr->getParSimpleFit(chi2_3d,chi2z,rid);
           number cth=cos(ppart->gettheta())*cos(dir.gettheta())+
           sin(ppart->gettheta())*sin(dir.gettheta())*cos(ppart->getphi()-dir.getphi());
           
                   if(chi2_3d<TRALIG.Cuts[0][1] && cth > TRALIG.Cuts[1][0] &&
                      fabs(ppart->getmomentum())>momentum*TRALIG.Cuts[2][0] &&
                      fabs(ppart->getmomentum())<momentum*TRALIG.Cuts[2][1]  ){
                      if(beta>TRALIG.Cuts[3][0] && beta< TRALIG.Cuts[3][1]){    
                       return 1;
                      }
                    }
        }
      }
      else {
          // Alg 1 cuts

        number pmass=ppart->getmass();
        number gmass=pmcg->getmass();
        number charge=pmcg->getcharge();
        number momentum=pmcg->getmom();
        number beta=ppart->getpbeta()->getbeta();
        number betaerror=ppart->getpbeta()->getebeta();
        AMSDir dir=pmcg->getdir();
        number recharge=ppart->getcharge();
        if(ppart->getmomentum()<0)recharge=-recharge;
        if(recharge==charge){
             static int tot2=0;
             static int tot3=0;
             static int tot4=0;
             static int tot5=0;
             static int tot6=0;
             static int tot7=0;
           AMSTrTrack *ptr=ppart->getptrack();              
           number chi2_3d, chi2z, rid;
           ptr->getParSimpleFit(chi2_3d,chi2z,rid);
           number cth=cos(ppart->gettheta())*cos(dir.gettheta())+
           sin(ppart->gettheta())*sin(dir.gettheta())*cos(ppart->getphi()-dir.getphi());
           
                  //cout <<"tot2 "<<++tot2<<" "<<chi2_3d<<" "<<cth;
                    //cout <<" "<<ppart->getmomentum()<<" "<<momentum<<endl;
                   if(chi2_3d<TRALIG.Cuts[0][1] && cth > TRALIG.Cuts[1][0] &&
                      fabs(ppart->getmomentum())>momentum*TRALIG.Cuts[2][0] &&
                      fabs(ppart->getmomentum())<momentum*TRALIG.Cuts[2][1]  ){
                  //cout <<"tot3 "<<++tot3<<" "<<beta<<" "<<endl;
                      if(beta>TRALIG.Cuts[3][0] && beta< TRALIG.Cuts[3][1]){                            integer betapat=ppart->getpbeta()->getpattern();
                      //   cout <<"tot4 "<<++tot4<<" "<<betapat<<" "<<endl;
                         if(betapat>=TRALIG.Cuts[4][0] && betapat>=TRALIG.Cuts[4][1]){
                        //   cout <<"tot5 "<<++tot5<<" "<<pmass<<" "<<pmass/gmass<<endl;
                           if(pmass/gmass> TRALIG.Cuts[5][0] && pmass/gmass <TRALIG.Cuts[5][1]){
                          cout <<"tot6 "<<++tot6<<" "<<betaerror<<" "<<endl;
                            if(betaerror < TRALIG.Cuts[6][1]){
                             return 1;
                            }
                          }
                         }
                      }
                    }
        }

      }
   }
   if(alg==2){
    number recharge=ppart->getcharge();
    if(recharge<3){
     AMSTrTrack *ptr=ppart->getptrack();              
    }
   }
   return 0;
}



void AMSTrAligFit::alfun(integer &n, number xc[], number &fc, AMSTrAligFit *p){
  integer i,niter;
  fc=0;
  for(i=0;i<n;i++){
    if(fabs(xc[i])>=0.199 ){
    fc=FLT_MAX;
    return;
    }
  }
  const integer maxhits=10;
  static geant hits[maxhits][3];
  static geant hits1[maxhits][3];
  static geant sigma[maxhits][3];
  static geant sigma1[maxhits][3];
  static geant normal[maxhits][3];
  static integer layer[maxhits];
  integer ialgo=11;
  integer ims=0;
  geant out[9];
  for(i=0;i<trconst::maxlay;i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
  }
  integer npfit=0;
   // convert parameters
   AMSTrAligPar par[trconst::maxlay];
   static integer idontwanttofit=0;
   static number fool_e04ccf=0; 
 {
   if(p->_flag==0 && idontwanttofit){
     fc=fool_e04ccf;
     return;
    }     
     AMSPoint outc[trconst::maxlay];
     AMSPoint outa[trconst::maxlay];
     for(i=0;i<trconst::maxlay;i++){
       outc[i]=0;
       outa[i]=0;
     }
     for(i=0;i<n;i++){
      int plane=p->_PlaneNo[i];
      int parno=p->_ParNo[i];
      if(parno<3)outc[plane][parno]=xc[i];
      else outa[plane][parno-3]=xc[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<trconst::maxlay;i++){
       par[i].setpar(outc[i],outa[i]);
     }


 }
   number pav=0;
   number pav2=0;
   if(p->_flag)HBOOK1(p->_Address+p->_flag,"my distr",50,0.,2.,0.);
  for(niter=0;niter<p->_PositionData;niter++){
   integer npt=TKDBc::patpoints((p->_pData)[niter]._Pattern);
   for(i=0;i<npt;i++){
     int plane=TKDBc::patconf((p->_pData)[niter]._Pattern,i)-1;
     for(int j=0;j<3;j++){
       hits[i][j]=(par[plane].getcoo())[j]+
       (par[plane].getmtx(j)).prod((p->_pData)[niter]._Hits[i]);
       sigma[i][j]=(par[plane].getmtx(j)).prod((p->_pData)[niter]._EHits[i]);
     }
   }
   out[0]=p->_pData[niter]._InvRigidity;
   TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ialgo,ims,layer,out);
   if(out[7]==0 && out[5]!=0 ){
    if(ialgo/10 && p->_flag){
      geant out1[9];
      int ia=1;
      TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ia,ims,layer,out1);
      out[5]=out1[5];
      geant xx=out[5]*p->_pData[niter]._InvRigidity;
      HF1(p->_Address+p->_flag,xx,1.);
      pav+=1./out[5]/p->_pData[niter]._InvRigidity;
      pav2+=1./out[5]/out[5]/p->_pData[niter]._InvRigidity/p->_pData[niter]._InvRigidity;
    }
    number error=out[8];
    fc+=out[6];
    npfit++;
   }
  }
  if(npfit < n+2)fc=FLT_MAX;
  else fc=fc/(npfit-n);
  //cout <<" fc "<<fc<<" "<< npfit<<endl;
  if(p->_flag)HPRINT(p->_Address+p->_flag);
  if(p->_flag==2 && npfit>1){
       pav/=npfit;
       pav2/=npfit;
       pav2=sqrt(fabs(pav2-pav*pav));
       p->_tmppav=pav;
       p->_tmppsi=pav2;
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<" "<<n<<endl;
       p->_flag=0;
  }
      if(p->_flag==3){
       if(npfit>1){
        pav/=npfit;
        pav2/=npfit;
        pav2=sqrt(fabs(pav2-pav*pav));
       }
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<" "<<n<<endl;
       p->_tmp=fc;
       p->_flag=0;
       p->_tmppav=pav;
       p->_tmppsi=pav2;
       fool_e04ccf=fc;
//       if(fabs(1-pav)<TRALIG.GlobalGoodLimit)idontwanttofit=1;
//       else idontwanttofit=0;
      }
}

void AMSTrAligFit::alfungl(integer &n, number xc[], number &fc, AMSTrAligFit *p){
if(MAGSFFKEY.magstat>0){

  integer i,niter;
  fc=0;
  for(i=0;i<n;i++){
    if(fabs(xc[i])>=0.199 ){
    fc=FLT_MAX;
    return;
    }
  }
  static number fool_e04ccf=0;
  if(!p->_flag){
    int allzero=1;
    for(int k=0;k<n;k++){
     if(xc[k]!=0){
       allzero=0;
       break;
     }
    }
    if(allzero){
     fc=fool_e04ccf;
     return;
    }
  }
  const integer maxhits=10;
  static geant hits[maxhits][3];
  static geant hits1[maxhits][3];
  static geant sigma[maxhits][3];
  static geant sigma1[maxhits][3];
  static geant normal[maxhits][3];
  static integer layer[maxhits];
  integer ialgo=11;
  integer ims=0;
  geant out[9];
  for(i=0;i<maxlay;i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
  }
  integer npfit=0;
   // convert parameters
   AMSTrAligPar par[maxlad][2][maxlay];
 {
     AMSPoint outc[maxlad][2][maxlay];
     AMSPoint outa[maxlad][2][maxlay];
     for(i=0;i<maxlad;i++){
     for(int j=0;j<2;j++){
     for(int k=0;k<maxlay;k++){
       outc[i][j][k]=0;
       outa[i][j][k]=0;
     }
     }
     }
     for(i=0;i<n;i++){
      int plane=p->_PlaneNo[i];
      int parno=p->_ParNo[i];
      int ladder=p->_LadderNo[i];
      int half=p->_HalfNo[i];
      if(parno<3)outc[ladder][half][plane][parno]=xc[i];
      else outa[ladder][half][plane][parno-3]=xc[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<maxlad;i++){
     for(int j=0;j<2;j++){
     for(int k=0;k<maxlay;k++){
       par[i][j][k].setpar(outc[i][j][k],outa[i][j][k]);
     }
     }
     }

 }
   number pav=0;
   number pav2=0;
   if(p->_flag)HBOOK1(p->_Address+p->_flag,"my distr",50,0.,2.,0.);
  for(niter=0;niter<p->_PositionData;niter++){
   integer npt=TKDBc::patpoints((p->_pData)[niter]._Pattern);
   uint address=(p->_pData)[niter]._Address;
   integer lad[2][maxlay];
   AMSTrTrack::decodeaddress(lad,address);
   for(i=0;i<npt;i++){
     int plane=TKDBc::patconf((p->_pData)[niter]._Pattern,i)-1;
     int ladno;
     int halfno;
     if(TRALIG.LayersOnly){
       ladno=0;
       halfno=0;
     }
     else{
      ladno=lad[0][plane]-1;
      halfno=lad[1][plane];
     }
     if(!_pPargl[ladno][halfno][plane].NEntries() ){
        out[7]=1;
        goto mbreak;
     }
     for(int j=0;j<3;j++){
       hits[i][j]=(par[ladno][halfno][plane].getcoo())[j]+
       (par[ladno][halfno][plane].getmtx(j)).prod((p->_pData)[niter]._Hits[i]);
       sigma[i][j]=(par[ladno][halfno][plane].getmtx(j)).prod((p->_pData)[niter]._EHits[i]);
     }
   }
   out[0]=p->_pData[niter]._InvRigidity;
   TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ialgo,ims,layer,out);
mbreak: 
   if(out[7]==0 && out[5]!=0 ){
    if(ialgo/10 && p->_flag){
       
      geant out1[9];
      int ia=1;
      TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ia,ims,layer,out1);
      out[5]=out1[5];
      geant xx=out[5]*p->_pData[niter]._InvRigidity;;
      HF1(p->_Address+p->_flag,xx,1.);
    }
    pav+=1/out[5]/p->_pData[niter]._InvRigidity;
    pav2+=1/out[5]/out[5]/p->_pData[niter]._InvRigidity/p->_pData[niter]._InvRigidity;
    number error=out[8];
    fc+=out[6];
    npfit++;
   }
   
  }
  if(npfit < n+2)fc=FLT_MAX;
  else fc=fc/(npfit-n);
  if(p->_flag)HPRINT(p->_Address+p->_flag);
  if(p->_flag==2 && npfit>1){
       pav/=npfit;
       pav2/=npfit;
       pav2=sqrt(fabs(pav2-pav*pav));
       p->_tmppav=pav;
       p->_tmppsi=pav2;
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<" "<<n<<endl;
       p->_flag=0;

  }
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<" "<<n<<endl;
      if(p->_flag==3){
       p->_tmp=fc;
       p->_flag=0;
       fool_e04ccf=fc;
      }
}
else{
// no magnetic field;  str line fit
  integer i,niter;
  fc=0;
  for(i=0;i<n;i++){
    if(fabs(xc[i])>=0.199 ){
    fc=FLT_MAX;
    return;
    }
  }
  static number fool_e04ccf=0;
  if(!p->_flag){
    int allzero=1;
    for(int k=0;k<n;k++){
     if(xc[k]!=0){
       allzero=0;
       break;
     }
    }
    if(allzero){
     fc=fool_e04ccf;
     return;
    }
  }
  const integer maxhits=10;
  static geant hits[maxhits][3];
  static geant hits1[maxhits][3];
  static geant sigma[maxhits][3];
  static geant sigma1[maxhits][3];
  static geant normal[maxhits][3];
  static integer layer[maxhits];
  integer ialgo=11;
  integer ims=0;
  geant out[9];
  for(i=0;i<trconst::maxlay;i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
  }
  integer npfit=0;
   // convert parameters
   AMSTrAligPar par[maxlad][2][trconst::maxlay];
 {
     AMSPoint outc[maxlad][2][trconst::maxlay];
     AMSPoint outa[maxlad][2][trconst::maxlay];
     for(i=0;i<maxlad;i++){
     for(int j=0;j<2;j++){
     for(int k=0;k<trconst::maxlay;k++){
       outc[i][j][k]=0;
       outa[i][j][k]=0;
     }
     }
     }
     for(i=0;i<n;i++){
      int plane=p->_PlaneNo[i];
      int parno=p->_ParNo[i];
      int ladder=p->_LadderNo[i];
      int half=p->_HalfNo[i];
      if(parno<3)outc[ladder][half][plane][parno]=xc[i];
      else outa[ladder][half][plane][parno-3]=xc[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<maxlad;i++){
     for(int j=0;j<2;j++){
     for(int k=0;k<trconst::maxlay;k++){
       par[i][j][k].setpar(outc[i][j][k],outa[i][j][k]);
     }
     }
     }

 }
   number pav=0;
   number pav2=0;
   if(p->_flag)HBOOK1(p->_Address+p->_flag,"my distr",50,0.,2.,0.);
  for(niter=0;niter<p->_PositionData;niter++){
   integer npt=TKDBc::patpoints((p->_pData)[niter]._Pattern);
   uint address=(p->_pData)[niter]._Address;
   integer lad[2][maxlay];
   AMSTrTrack::decodeaddress(lad,address);
   for(i=0;i<npt;i++){
     int plane=TKDBc::patconf((p->_pData)[niter]._Pattern,i)-1;
     int ladno;
     int halfno;
     if(TRALIG.LayersOnly){
       ladno=0;
       halfno=0;
     }
     else{
      ladno=lad[0][plane]-1;
      halfno=lad[1][plane];
     }
     if(!_pPargl[ladno][halfno][plane].NEntries() ){
        out[7]=1;
        goto mbreak2;
     }
     for(int j=0;j<3;j++){
       hits[i][j]=(par[ladno][halfno][plane].getcoo())[j]+
       (par[ladno][halfno][plane].getmtx(j)).prod((p->_pData)[niter]._Hits[i]);
       sigma[i][j]=(par[ladno][halfno][plane].getmtx(j)).prod((p->_pData)[niter]._EHits[i]);
     }
   }
   out[0]=p->_pData[niter]._InvRigidity;
   TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ialgo,ims,layer,out);
mbreak2: 
   if(out[7]==0 ){
    fc+=out[6];
    if(p->_flag==3){
     //p->chi2[npfit]=0;
     p->chi2i[npfit]=out[6];
    }
    else{
     p->chi2[npfit]=out[6];
     //p->chi2i[npfit]=0;
    }
    npfit++;
   }
   
  }
  if(npfit < n+2)fc=FLT_MAX;
  else fc=fc/(npfit-n);
  if(p->_flag)HPRINT(p->_Address+p->_flag);
  if(p->_flag==2 && npfit>1){
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<endl;
       p->_flag=0;

  }
       for(i=0;i<8;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<" "<<n<<endl;
      if(p->_flag==3){
       p->_tmp=fc;
       p->_flag=0;
       fool_e04ccf=fc;
      }


}
}



void AMSTrAligData::Init(AMSParticle *ppart, AMSmceventg *pmcg){
    AMSTrTrack * ptrack=ppart->getptrack();
    integer pattern=ptrack->getpattern();
   _Pattern=pattern;
   _Address=ptrack->getaddress();
   _InvRigidity=pmcg->getcharge()/pmcg->getmom();
   _ErrInvRigidity=0.01*_InvRigidity;
   _Pid=pmcg->pid();
  _NHits=TKDBc::patpoints(pattern);
  _Hits= new AMSPoint[_NHits];
  _EHits=new AMSPoint[_NHits];
  for(int i=0;i<TKDBc::patpoints(pattern);i++){
    for(int j=0;j<ptrack->getnhits();j++){
     AMSTrRecHit * ph= ptrack->getphit(j);
     if (ph->getLayer() == TKDBc::patconf(pattern,i)){
      _Hits[i]=ph->getHit();
      _EHits[i]=ph->getEHit();
     }
    }
  }

  return ;
}

AMSID AMSTrAligPar::getTDVDB(){
return AMSID("TrAligDB",AMSJob::gethead()->isRealData());

}
AMSID AMSTrAligFit::getTDVGLDB(){
return AMSID("TrAligglDB",AMSJob::gethead()->isRealData());

}

AMSTrAligPar  AMSTrAligFit::_pPargl[17][2][maxlay];

integer AMSTrAligFit::AddressOK(uinteger address, integer strict){
  
  if(strict || TKDBc::patpoints(_Pattern)<5)return address==_Address;
  integer lad1[2][maxlay];
  integer lad2[2][maxlay];
  AMSTrTrack::decodeaddress(lad1,address);
   AMSTrTrack::decodeaddress(lad2,_Address);
  for(int i=0;i<maxlay;i++){
     if(lad1[0][i]!=0){
      for(int j=0;j<2;j++){
       if(lad1[j][i]!=lad2[j][i]){
         return 0;
       }
      }
     }   
  }
  return 1;
}

void AMSTrAligPar::LockDB(){

_lockunlock(1);
}



void AMSTrAligPar::UnlockDB(){

_lockunlock(0);

}


void AMSTrAligPar::_lockunlock(integer lock){

char fnam[256]="";
strcpy(fnam,AMSDATADIR.amsdatabase);
strcat(fnam,".AMSTrAligDB.lock");
ofstream ftxt;
int ntry=0;
const int maxtry=100;
again:
ftxt.open(fnam);
if(ftxt){
 ftxt <<lock<<endl;
 ftxt.close();
 cout <<"AMSTrAligPar::_lockunlock-I-lockstatus "<<lock<<endl;
}
else{
 cerr<<" AMSTrAligPar::_lockunlock-F-UnableToOpenLockFile "<<fnam<<endl;
  if(ntry++<maxtry){
   ftxt.close();
   goto again;
  }
   exit(1);
}


}

integer AMSTrAligPar::DbIsNotLocked(integer delay){
int ntry=0;
const int maxtry=255;
char fnam[256]="";
strcpy(fnam,AMSDATADIR.amsdatabase);
strcat(fnam,".AMSTrAligDB.lock");
ifstream ftxt;
again:
ftxt.open(fnam);
if(ftxt){
 int lock=1;
 ftxt >>lock;
 if(lock && ntry<maxtry){
  cerr<< "AMSTrAligPar::DbIsNotLocked-W-DBIsLocked "<<endl;
  ftxt.close();
  sleep(delay);
  ntry++;
  goto again;
 }
 else if(lock==0){
  ftxt.close();
  return 1;
 }
 else{
  cerr<<" AMSTrAligPar::DbIsNotLocked-E-TooManyTries-GoAhead"<<endl;
  ftxt.close();
  return 0;  
 }
}
else{
 cerr<<" AMSTrAligPar::DbIsNotLocked-F-UnableToOpenLockFile "<<fnam<<" "<<ntry<<endl;
 if(ntry++<maxtry){
    ftxt.close();
    sleep(delay);
    goto again;
 }
 exit(1);
 return 0;
}


}

void AMSTrAligFit::UpdateDBgl(){

if(TRALIG.LayersOnly){
     for(int i=0;i<trconst::maxlay;i++){
       //update layer
         integer status;
         integer rgid;
         geant coo[3];
         number nrm[3][3];
         TKDBc::GetLayer(i+1,status,coo,nrm,rgid);
         AMSPoint Coo(coo);
         number nrmN[3][3];
         _pPargl[0][0][i].updmtx();
         int j;
         for(j=0;j<3;j++){
           for(int k=0;k<3;k++){
            AMSDir stipud_cxx=_pPargl[0][0][i].getmtx(j);
            nrmN[j][k]=stipud_cxx[k];
           }
         }
         for(j=0;j<3;j++){
          coo[j]=_pPargl[0][0][i].getcoo()[j]+(_pPargl[0][0][i].getmtx(j)).prod(Coo);
         }
         amsprotected::mm3(nrmN,nrm,0);
         TKDBc::SetLayer(i+1,status,coo,nrm,rgid);
      }
         TKDBc::write(4);

}
else{
         integer status;
         integer rgid;
         geant coo[3];
         number nrm[3][3];
int i;
for(i=0;i<maxlay;i++){
 integer statusy;
 integer rgidy;
 geant cooy[3];
 number nrmy[3][3];        
 TKDBc::GetLayer(i+1,statusy,cooy,nrmy,rgidy);
 number nrmyT[3][3];
 transpose(nrmy,nrmyT);
 AMSPoint Cooy(cooy);
 for(int nlad=0;nlad<maxlad;nlad++){
  for(int side=0;side<2;side++){
    if((TKDBc::activeladdshuttle(i+1,nlad+1,side))){
       TKDBc::GetLadder(i,nlad,side,status,coo,nrm,rgid);
       AMSPoint Coo(coo);
       number nrmN[3][3];
       _pPargl[nlad][side][i].updmtx();
       int j;
       for(j=0;j<3;j++){
           for(int k=0;k<3;k++){
            AMSDir stipud_cxx=_pPargl[nlad][side][i].getmtx(j);
            nrmN[j][k]=stipud_cxx[k];
           }
       }
       number nrmz0[3][3];
       number nrmz1[3][3];
       UCOPY(nrmN,nrmz0,sizeof(nrmz0)/sizeof(integer));
       amsprotected::mm3(nrmyT,nrmz0,0);
       UCOPY(nrmy,nrmz1,sizeof(nrmz0)/sizeof(integer));
       amsprotected::mm3(nrmN,nrmz1,0);
       amsprotected::mm3(nrmyT,nrmz1,0);
        
       for(j=0;j<3;j++){
          AMSDir d0(nrmyT[j][0],nrmyT[j][1],nrmyT[j][2]);
          AMSDir d1(nrmz0[j][0],nrmz0[j][1],nrmz0[j][2]);
          AMSDir d2(nrmz1[j][0],nrmz1[j][1],nrmz1[j][2]);
          coo[j]=d0.prod(_pPargl[nlad][side][i].getcoo()-Cooy)+d1.prod(Cooy)+d2.prod(Coo);
       }
       amsprotected::mm3(nrmy,nrm,0);
       amsprotected::mm3(nrmN,nrm,0);
       amsprotected::mm3(nrmyT,nrm,0);
       if(_pPargl[nlad][side][i].NEntries()>TRALIG.MinEventsPerFit){
         TKDBc::SetLadder(i,nlad,side,status,coo,nrm,rgid);
         _pPargl[nlad][side][i].NEntries()=-_pPargl[nlad][side][i].NEntries(); 
       }
       else{
        cerr<<"AMSTrAligFit::UpdateDBgl-W-NentriesTooLowParWillNotBeUpdated "<<
        _pPargl[nlad][side][i].NEntries()<<" "<<nlad<<" "<<side<<" "<<i<<endl;
         _pPargl[nlad][side][i].NEntries()=0;  
        
       }       
    }  
  }
 }
}

      // UpdateDB

     for(i=0;i<maxlay;i++){
      int j;
      for(j=0;j<maxlad;j++){
        for(int k=0;k<2;k++){
         _gldb[j][k][i].nentries=-_pPargl[j][k][i].NEntries();
         for(int l=0;l<3;l++){
          _gldb[j][k][i].coo[l]=_pPargl[j][k][i].getcoo()[l];
          _gldb[j][k][i].ang[l]=_pPargl[j][k][i].getang()[l];
         }
        }
      }
     }
    

    if (AMSFFKEY.Update ){
       AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVGLDB());
             ptdv->UpdCRC();
             if( !ptdv->write(AMSDATADIR.amsdatabase)){
                cerr <<"AMSTrAligPar::updateDB-F-ProblemtoUpdate "<<*ptdv;
                exit(1);
             }              
    }
    TKDBc::write(3);
}


}

void AMSTrAligFit::RebuildNoActivePar(){
if(TRALIG.LayersOnly){

 _NoActivePar=0;
 for(int i=0;i<maxlay;i++){
  int ntr=0;
  for(int l=0;l<maxlad;l++){
   for(int m=0;m<2;m++){
    if(_pPargl[l][m][i].NEntries()>0)ntr+=_pPargl[l][m][i].NEntries();
   }
  }
  if(ntr>TRALIG.MinEventsPerFit/2){
  _pPargl[0][0][i].NEntries()=ntr;
  int nprp=0;
   for(int j=0;j<6;j++){ 
    if(TRALIG.ActiveParameters[i][j]){
      _PlaneNo[_NoActivePar+nprp]=i;
      _ParNo[_NoActivePar+nprp]=j;
      _LadderNo[_NoActivePar+nprp]=0;
      _HalfNo[_NoActivePar+nprp]=0;
      nprp++;
    }
   }
     _NoActivePar+=nprp;
   }
   else {
      cout <<" AMSTrAligFit::RebuildNoActivePar-I-TooFewEvents "<<ntr<<" "<<i<<endl;
      _pPargl[0][0][i].NEntries()=0;
   }
 }


}
else{
 _NoActivePar=0;
 for(int i=0;i<maxlay;i++){
  for(int l=0;l<maxlad;l++){
   for(int m=0;m<2;m++){
    if(_pPargl[l][m][i].NEntries()>TRALIG.MinEventsPerFit/2){
     int nprp=0;
     for(int j=0;j<maxlay;j++){ 
      if(TRALIG.ActiveParameters[i][j]){
        _PlaneNo[_NoActivePar+nprp]=i;
        _ParNo[_NoActivePar+nprp]=j;
        _LadderNo[_NoActivePar+nprp]=l;
        _HalfNo[_NoActivePar+nprp]=m;
        nprp++;
      }
     }
     _NoActivePar+=nprp;
    }
    else if(_pPargl[l][m][i].NEntries()>0){
      cout <<" AMSTrAligFit::RebuildNoActivePar-I-TooFewEvents "<<_pPargl[l][m][i].NEntries()<<" "<<l<<" "<<m << " " <<i<<endl;
      _pPargl[l][m][i].NEntries()=0;
    }
   }
  }
 }
}
cout <<"GlobalFit-I-NoActivePar "<<_NoActivePar<<endl;

}

void AMSTrAligFit::InitDB(){

  for(int i=0;i<maxlad;i++){
   for(int j=0;j<2;j++){
    for(int k=0;k<maxlay;k++){   
       _gldb[i][j][k].nentries=0;
         for(int l=0;l<3;l++){
          _gldb[j][k][i].coo[l]=0;
          _gldb[j][k][i].ang[l]=0;
         }
       _pPargl[i][j][k]=AMSTrAligPar(); 
    }
   }
  }

}

AMSTrAligFit::gldb_def AMSTrAligFit::_gldb[maxlad][2][maxlay];


integer AMSTrAligFit::glDBOK(uinteger address){
   integer ladder[2][maxlay];
   AMSTrTrack::decodeaddress(ladder,address);
   for(int i=0;i<maxlay;i++){
     if(ladder[0][i]){
       if(!_gldb[ladder[0][i]-1][ladder[1][i]][i].nentries)return 0;
     }
   }
   return 1;
  
}

integer AMSTrAligPar::AddOne(){

if(_NEntries>=0)_NEntries++;

if(_NEntries<TRALIG.SingleLadderEntryLimit)return _NEntries>=0;
else return 0;

}

void AMSTrAligPar::MinusOne(){

if(_NEntries>0)_NEntries--;


}

