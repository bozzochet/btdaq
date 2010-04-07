//  $Id: trcalib.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "trcalib.h"
#include "event.h"
#include <math.h>
#include "timeid.h"
#include "trrawcluster.h"
#include <float.h>
#include "mccluster.h"
#include "tkdbc.h"
#include "mceventg.h"
//PROTOCCALLSFSUB15(E04CCF,e04ccf,INT,DOUBLEV,DOUBLE,DOUBLE,INT,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,ROUTINE,ROUTINE,INT,INT)
//#define E04CCF(N,X,F,TOL,IW,W1,W2,W3,W4,W5,W6,ALFUN1,MONIT,MAXCAL,IFAIL) CCALLSFSUB15(E04CCF,e04ccf,INT,DOUBLEV,DOUBLE,DOUBLE,INT,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,ROUTINE,ROUTINE,INT,INT,N,X,F,TOL,IW,W1,W2,W3,W4,W5,W6,ALFUN1,MONIT,MAXCAL,IFAIL)

extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);

void AMSTrCalibPar::_a2m(){
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

void AMSTrCalibPar::setpar(const AMSPoint & coo, const AMSPoint & angles){
_Coo=coo;
_Angles=angles;
// construct rot matrix
_a2m();
}


AMSTrCalibPar::AMSTrCalibPar(const AMSPoint & coo, const AMSPoint & angles){
_Coo=coo;
_Angles=angles;
// construct rot matrix
_a2m();
}


integer AMSTrCalibData::Init(AMSBeta *pbeta, AMSTrTrack * ptrack, 
AMSmceventg *pmcg, integer pattern, integer alg){
// check if patterns match
  if(patpoints[alg][pattern]==0)return 0;
  int i,j;
  int k=0;
  for(i=0;i<patpoints[alg][pattern];i++){
    for(j=0;j<ptrack->getnhits();j++){
     AMSTrRecHit * ph= ptrack->getphit(j);
     if (ph->getLayer() == patconf[alg][pattern][i])k++;
    }
  }
  if(k !=patpoints[alg][pattern])return 0;
  if(alg<2){
   number beta=pbeta->getbeta();
    _ErrInvBeta=pbeta->getebeta();
    _InvBeta=1/beta;
   if(beta < 1){
    const number pmass=0.938;
    number gamma=1./sqrt(1.-beta*beta);
    _InvRigidity=1/beta/gamma/pmass;
    _ErrInvRigidity=gamma*_ErrInvBeta/pmass;
   }
   else{
    _InvRigidity=0;
    _ErrInvRigidity=0;
   }
  }
  else if(alg<4){
   _InvRigidity=pmcg->getcharge()/pmcg->getmom(); 
   _ErrInvRigidity=0.01*_InvRigidity;
  }
  else {
    cerr <<" AMSTrCalibData::Init-F-AlgNotSuppYet "<<alg<<endl;
    exit(1);
  }
  _NHits=patpoints[alg][pattern];
  _Hits= new AMSPoint[_NHits];
  _EHits=new AMSPoint[_NHits];
  for(i=0;i<patpoints[alg][pattern];i++){
    for(j=0;j<ptrack->getnhits();j++){
     AMSTrRecHit * ph= ptrack->getphit(j);
     if (ph->getLayer() == patconf[alg][pattern][i]){
      _Hits[i]=ph->getHit();
      _EHits[i]=ph->getEHit();

     }
    }
  }

  return 1;
}
integer  AMSTrCalibData::Select(integer alg, integer pattern){
  if(alg<2){
    AMSContainer * pcnt;
    int i,npart,ncharge;
    int nbeta=0; 
    geant nhits=0;
    for(i=0;;i++){
      pcnt=AMSEvent::gethead()->getC("AMSBeta",i);
      if(pcnt)nbeta+=pcnt->getnelem();
      else break;
    }
    for(i=0;;i++){
      pcnt=AMSEvent::gethead()->getC("AMSTrCluster",i);
      if(pcnt)nhits+=pcnt->getnelem();
      else break;
    }
      pcnt=AMSEvent::gethead()->getC("AMSCharge",0);
      ncharge=pcnt->getnelem();
      pcnt=AMSEvent::gethead()->getC("AMSParticle",0);
      npart=pcnt->getnelem();
      if(npart ==1 && ncharge==1 && nbeta==1){
       AMSParticle *ppart=(AMSParticle*)AMSEvent::gethead()->
       getheadC("AMSParticle",0);
       if(ppart){
         if(ppart->getcharge() == 1.){
           AMSBeta * pbeta=ppart->getpbeta();
           number beta=pbeta->getbeta();
           if(beta > TRCALIB.BetaCut[alg][0] &&              
              beta < TRCALIB.BetaCut[alg][1]){
            AMSTrTrack * ptrack=ppart->getptrack();
            if(nhits/ptrack->getnhits() < TRCALIB.HitsRatioCut[alg]){
              if(Init(pbeta,ptrack,0,pattern,alg)){
               AMSEvent::gethead()->addnext(AMSID("AMSTrCalibration",0),
               new AMSTrCalibration(pattern));
               return 1;
              }
            }
           }
         }
       }
      }
  }
  else {
    // Beam test algorithm
    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    if(!pmcg)return 0;
    number charge=pmcg->getcharge();    
    number momentum=pmcg->getmom();
    AMSDir dir=pmcg->getdir();
      integer npart=AMSEvent::gethead()->getC("AMSParticle",0)->getnelem();
      AMSParticle *ppart=(AMSParticle*)AMSEvent::gethead()->
       getheadC("AMSParticle",0);
      if(npart ==1 && ppart && ppart->getcharge()==charge ){
           AMSTrTrack *ptr=ppart->getptrack();
           for(int k=0;k<ptr->getnhits();k++){
            AMSTrRecHit * phit=ptr->getphit(k);
            int il=phit->getLayer();
            if(TRCALIB.Ladder[il-1]){
             AMSTrCluster *pycl=phit->getClusterP(1);
             if(((pycl->getid()).getdrp()!=TRCALIB.Ladder[il-1]/10)
             || ((pycl->getid()).gethalf()!=TRCALIB.Ladder[il-1]%10))return 0; 
            }           
           }      
            number chi2_3d, chi2z, rid;
           ptr->getParSimpleFit(chi2_3d,chi2z,rid);
            number cth=cos(ppart->gettheta())*cos(dir.gettheta())+
            sin(ppart->gettheta())*sin(dir.gettheta())*cos(ppart->getphi()-dir.getphi());
            if(chi2_3d<1. && cth > TRCALIB.HitsRatioCut[alg] &&
               ppart->getmomentum()>momentum*TRCALIB.MomentumCut[alg][0] &&
               ppart->getmomentum()<momentum*TRCALIB.MomentumCut[alg][1]  ){
              if(Init(ppart->getpbeta(),ppart->getptrack(),pmcg,pattern,alg)){
               AMSEvent::gethead()->addnext(AMSID("AMSTrCalibration",0),
               new AMSTrCalibration(pattern));
               return 1;
              }
             }
       }
     }
  return 0;
}

  integer AMSTrCalibData::patpoints[nalg][tkcalpat]={3,3,3,4,4,
                                                     3,3,3,3,0,
                                                     6,5,4,4,3,
                                                     6,5,4,4,3
                                                               };
  integer AMSTrCalibData::patconf[nalg][tkcalpat][6]={
                                                 1,2,4,0,0,0,
                                                 2,3,5,0,0,0,
                                                 3,4,6,0,0,0,
                                                 1,2,4,5,0,0,
                                                 2,3,5,6,0,0,
                                                 1,2,3,0,0,0,
                                                 2,3,4,0,0,0,
                                                 3,4,5,0,0,0,
                                                 4,5,6,0,0,0,
                                                 0,0,0,0,0,0,
                                                 1,2,3,4,5,6,
                                                 1,2,3,4,5,0,
                                                 1,2,3,4,0,0,
                                                 1,4,5,6,0,0,
                                                 1,5,6,0,0,0,
                                                 1,2,3,4,5,6,
                                                 1,2,3,4,5,0,
                                                 1,2,3,4,0,0,
                                                 1,4,5,6,0,0,
                                                 1,5,6,0,0,0
                                                              };


  AMSTrCalibFit * AMSTrCalibFit::_pCalFit[nalg][tkcalpat]={0,0,0,0,0,
                                                           0,0,0,0,0,
                                                           0,0,0,0,0,
                                                           0,0,0,0,0
  };




void AMSTrCalibFit::setHead(integer alg, integer pat, AMSTrCalibFit *ptr){
  if(alg>=0 && alg < nalg && pat >=0 && pat < tkcalpat)
  _pCalFit[alg][pat]=ptr;
}  
AMSTrCalibFit::AMSTrCalibFit():_pData(0),_PositionData(0),_Pattern(0),
  _NData(0),_NIter(0),_PositionIter(0),_fcn(0),_fcnI(0),_pfit(0),_pfits(0),_flag(0){
  int i;
  for(i=0;i<6;i++){
   _pParC[i]=0;
  }

  
}


AMSTrCalibFit::AMSTrCalibFit(integer pattern, integer data, integer iter, integer alg, integer ipart):
_PositionData(0),_PositionIter(0),_flag(0){
_Pattern=pattern;
_NData=data;
_NIter=iter;
assert(data > 0 && iter >0);
_pData= new AMSTrCalibData[data];
assert(_pData !=  NULL);
_Algorithm = alg;
_Pid=ipart;

integer npt=AMSTrCalibData::patpoints[alg][pattern];
int i,j;
for(i=0;i<6;i++){
 _pParC[i]= new AMSTrCalibPar[_NIter];
}
_fcn = new number[_NIter];
_fcnI = new number[_NIter];
_pfit = new number[_NIter];
_pfits = new number[_NIter];

_NoActivePar=0;
for(i=0;i<6;i++){
 for(int k=1;k<6;k++){
 if(i==AMSTrCalibData::patconf[alg][pattern][k]-1){
  int nprp=0;
  for(j=0;j<6;j++){
    if(TRCALIB.ActiveParameters[i][j]){
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
}

AMSTrCalibFit::~AMSTrCalibFit(){
 delete [] _pData;
 delete [] _fcn;
 delete [] _fcnI;
 delete [] _pfit;
 delete [] _pfits;
  int i;
integer npt=AMSTrCalibData::patpoints[_Algorithm][_Pattern];
  for(i=0;i<6;i++){
   delete [] _pParC[i];
  }
}

integer AMSTrCalibFit::Test(int forced){
static int skip=0;

if(forced && _PositionData>199){
 return 1;
}
if(TRCALIB.MultiRun){
 
 if(_Pattern==TRCALIB.PatStart){
  skip++;
  if(skip>TRCALIB.EventsPerRun){
    if(GCFLAG.IEORUN==0)GCFLAG.IEORUN=2;
    skip=0;
  }
 }
}

if(!forced && _pData[_PositionData].Select(_Algorithm, _Pattern)){
 _PositionData++;
if(TRCALIB.MultiRun){
 if(_Pattern==TRCALIB.PatStart){
  if( _PositionData%TRCALIB.EventsPerRun==TRCALIB.EventsPerRun-1){
    if(GCFLAG.IEORUN==0)GCFLAG.IEORUN=2;
    skip=0;
  }
 }
}
 if(_Pattern==TRCALIB.PatStart)skip=0;
}
if(_PositionData == _NData  ){
 return 1;
}
else if(_PositionData > _NData  ){
 cerr <<" AMSTrCalibFit::Test -E -PositionBeyondLimits "<<_PositionData<<" "<<_NData<<endl;
_PositionData=_NData;
 return 1;
}
else return 0;
}


void AMSTrCalibFit::Fit(){
    if(_PositionIter >= _NIter){
      cout << "AMSTrCalibFit::Fit Saturated for pattern "<<_Pattern<<" "<<_Algorithm<<" "<<    _PositionIter<<endl;
      return;
    }
    void (*palfun)(int &n, double x[], double &f, AMSTrCalibFit *p)=&AMSTrCalibFit::alfun;
    void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=
                                &AMSTrCalibFit::monit;
      cout << "AMSTrCalibFit::Fit started for pattern "<<_Pattern<<" "<<_Algorithm<<" "<<    _PositionIter<<endl;
  // Fit Here
    const integer mp=100;
    number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[2000];
  // number of parameters to fit
    integer npt=AMSTrCalibData::patpoints[_Algorithm][_Pattern];
    integer n=_NoActivePar;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=1000;
    number tol=2.5e-2;
    int i,j;
    for(i=0;i<mp;i++)x[i]=0;
    _flag=3;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
    cout << "AMSTrCalibFit::Fit finished "<<ifail<<" "<<f<<endl;
    if(ifail ==0 || ifail==2){
     _flag=2;
     ifail=1;
     number fd;
     integer one(1);
     e04ccf_(n,x,fd,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);
     AMSPoint outc[6];
     AMSPoint outa[6];
     for(i=0;i<6;i++){
       outc[i]=0;
       outa[i]=0;
     }
     for(i=0;i<n;i++){
      int plane=_PlaneNo[i];
      int parno=_ParNo[i];
      if(parno<3)outc[plane][parno]=x[i];
      else outa[plane][parno-3]=x[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<6;i++){
       (_pParC[i][_PositionIter]).setpar(outc[i],outa[i]);
     }
     _fcn[_PositionIter]=f;
     _fcnI[_PositionIter]=_tmp;
     _pfit[_PositionIter]=_tmppav;
      cout <<"tmpav "<<_tmppav<<" "<<_tmppsi<<endl;  
     _pfits[_PositionIter]=_tmppsi;
     _PositionIter++;
    }
    else cerr <<"FIt-E-IfailNotZero "<<ifail<<endl;
    _PositionData=0;
}

void AMSTrCalibFit::SAnal(){
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//tralig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("tralig");
  CLOSEF(IOPA.hlun+1);

}

void AMSTrCalibFit::Anal(){
cout <<" AMSTrCalibFit::Anal called for pattern "<<_Pattern<<" "<<_Algorithm<<endl;
  static  TrAlig_def TRALIG;
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
     exit(1);
    }
    else cout <<"tralig ntuple file "<<filename<<" opened."<<endl;

   HBNT(IOPA.ntuple+1,"Tracker Alignment"," ");
   HBNAME(IOPA.ntuple+1,"TrAlig",(int*)(&TRALIG),"Pattern:I,Alg:I,FCN:R,FCNI:R,PFIT:R,PFITS:R,Coo(3,6):R,Angle(3,6):R");

  }

  
  int i,j,k,l;
  AMSTrCalibPar x[6];
  AMSTrCalibPar x2[6];
  for(i=0;i<_PositionIter;i++){
      TRALIG.Pattern=_Pattern;
      TRALIG.Alg=_Algorithm;
      TRALIG.FCN=_fcn[i];
      TRALIG.FCNI=_fcnI[i];
      TRALIG.Pfit=_pfit[i];
      TRALIG.Pfitsig=_pfits[i];
    for(j=0;j<6;j++){
      for(k=0;k<3;k++){
       TRALIG.Coo[j][k]=_pParC[j][i].getcoo()[k]; 
       TRALIG.Angle[j][k]=_pParC[j][i].getang()[k]; 
      }
    }
    HFNT(IOPA.ntuple+1);
  }
  if(_PositionIter>0){
  for(j=0;j<6;j++){
   for(i=0;i<_PositionIter;i++){
   x[j]=x[j]+_pParC[j][i];
   x2[j]=x2[j]+_pParC[j][i]*_pParC[j][i];
   }
    x[j]=x[j]/_PositionIter;
    x2[j]=x2[j]/_PositionIter;
    x2[j]=x2[j]-x[j]*x[j];
    x2[j].sqr();
    cout <<"ANAL-I-Results "<<" Pattern "<<_Pattern<<" Alg "<<_Algorithm<<endl;
    cout <<j<<" "  <<x[j]<<" "<<x2[j]<<endl;
  }
   if(upddone==0 ){
    upddone=1;
    for(int i=0;i<6;i++){
     if(TRCALIB.Ladder[i]==0){
       //update layer
         integer status;
         integer rgid;
         geant coo[3];
         number nrm[3][3];
         TKDBc::GetLayer(i+1,status,coo,nrm,rgid);
         AMSPoint Coo(coo);
         number nrmN[3][3];
         x[i].updmtx();
         for(j=0;j<3;j++){
           for(k=0;k<3;k++){
            AMSDir stipud_cxx=x[i].getmtx(j);
            nrmN[j][k]=stipud_cxx[k];
           }
         }
         for(j=0;j<3;j++){
          coo[j]=x[i].getcoo()[j]+(x[i].getmtx(j)).prod(Coo);
         }
         amsprotected::mm3(nrmN,nrm,0);
         TKDBc::SetLayer(i+1,status,coo,nrm,rgid);
     }
     else {
       //update ladder
         integer status;
         integer rgid;
         geant coo[3];
         number nrm[3][3];
        
        TKDBc::GetLadder(i,TRCALIB.Ladder[i]/10-1,TRCALIB.Ladder[i]%10,status,
        coo,nrm,rgid);
         AMSPoint Coo(coo);
         number nrmN[3][3];
         x[i].updmtx();
         for(j=0;j<3;j++){
           for(k=0;k<3;k++){
            AMSDir stipud_cxx=x[i].getmtx(j);
            nrmN[j][k]=stipud_cxx[k];
           }
         }
         for(j=0;j<3;j++){
          coo[j]=x[i].getcoo()[j]+(x[i].getmtx(j)).prod(Coo);
         }
         amsprotected::mm3(nrmN,nrm,0);
        TKDBc::SetLadder(i,TRCALIB.Ladder[i]/10-1,TRCALIB.Ladder[i]%10,status,
        coo,nrm,rgid);



     }
    }
    TKDBc::write(3);
  }
}

  
}
integer AMSTrCalibFit::getlayer(integer c){
return AMSTrCalibData::patconf[_Algorithm][_Pattern][c];
}

void AMSTrCalibFit::alfun(integer &n, number xc[], number &fc, AMSTrCalibFit *p){
  integer i,niter;
  fc=0;
  for(i=0;i<n;i++){
    if(fabs(xc[i])>=0.199 ){
    fc=FLT_MAX;
    return;
    }
  }
  integer npt=AMSTrCalibData::patpoints[p->_Algorithm][p->_Pattern];
  const integer maxhits=10;
  static geant hits[maxhits][3];
  static geant hits1[maxhits][3];
  static geant sigma[maxhits][3];
  static geant sigma1[maxhits][3];
  static geant normal[maxhits][3];
  static integer layer[maxhits];
  integer ialgo=1;

  if(p->_Algorithm==2)ialgo=11;
  integer ims=0;
  geant out[9];
  for(i=0;i<npt;i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
  }
  integer npfit=0;
   // convert parameters
   AMSTrCalibPar par[6];
 {
     AMSPoint outc[6];
     AMSPoint outa[6];
     for(i=0;i<6;i++){
       outc[i]=0;
       outa[i]=0;
     }
     for(i=0;i<n;i++){
      int plane=p->_PlaneNo[i];
      int parno=p->_ParNo[i];
      if(parno<3)outc[plane][parno]=xc[i];
      else outa[plane][parno-3]=xc[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<6;i++){
       par[i].setpar(outc[i],outa[i]);
     }


 }
   number pav=0;
   number pav2=0;
   if(p->_flag)HBOOK1(101,"my distr",50,1.,11.,0.);
  for(niter=0;niter<p->_PositionData;niter++){
   for(i=0;i<npt;i++){
     int plane=AMSTrCalibData::patconf[p->_Algorithm][p->_Pattern][i]-1;
     for(int j=0;j<3;j++){
       hits[i][j]=(par[plane].getcoo())[j]+
       (par[plane].getmtx(j)).prod((p->_pData)[niter]._Hits[i]);
       sigma[i][j]=(par[plane].getmtx(j)).prod((p->_pData)[niter]._EHits[i]);
       //cout <<" hits i,j "<<i<<" "<<j<<" "<<hits[i][j]<<endl;
     }
   }
  hits[1][1]+=TRCALIB.HitsRatioCut[0];
   out[0]=p->_pData[niter]._InvRigidity;
   TKFITG(npt,hits,sigma,normal,p->_Pid,ialgo,ims,layer,out); 
   if(out[7]==0 && out[5]>p->_Algorithm!=0?
      TRCALIB.MomentumCut[p->_Algorithm][0]:
      TRCALIB.MomentumCut[p->_Algorithm][0]/p->_pData[niter]._InvRigidity
 &&                out[5]<p->_Algorithm!=0?
      TRCALIB.MomentumCut[p->_Algorithm][1]:
      TRCALIB.MomentumCut[p->_Algorithm][1]/p->_pData[niter]._InvRigidity
 ){
    if(ialgo/10 && p->_flag){
      geant out1[9];
      int ia=1;
      TKFITG(npt,hits,sigma,normal,p->_Pid,ia,ims,layer,out1); 
      out[5]=out1[5];
      geant xx=out[5];
      HF1(101,xx,1.);
    }
    pav+=out[5]*p->_pData[niter]._InvRigidity;
    pav2+=out[5]*out[5]*p->_pData[niter]._InvRigidity*p->_pData[niter]._InvRigidity;
    const number pm2=0.88;
    number energy=sqrt(out[5]*out[5]+pm2);
    number error=out[8];
//    error=0.1;
    //cout <<npfit<<" "<<out[5]<<" "<<out[6]<<" "<<error<<" "<<p->_pData[niter]._InvRigidity<<" "<<p->_pData[niter]._ErrInvRigidity<<" "<<niter<<endl;
    if(p->_Algorithm ==0)fc+=pow(energy/out[5]-(p->_pData)[niter]._InvBeta,double(2.))/
(((p->_pData)[niter]._ErrInvBeta)*((p->_pData)[niter]._ErrInvBeta)+(pm2/energy*out[8])*(pm2/energy*out[8]));    
    else if(p->_Algorithm<3)fc+=out[6];
    else {
        // Fit every combi from pattern
        if(npt==6){
          for(int j=22;j<42;j++){
            for(int k=0;k<3;k++){  
             for(int l=0;l<3;l++){  
              hits1[k][l]=hits[TKDBc::patconf(j,k)][l];
              sigma1[k][l]=sigma[TKDBc::patconf(j,k)][l];
              int n3=3;
              TKFITG(n3,hits1,sigma1,normal,p->_Pid,ialgo,ims,layer,out); 
              cout <<niter<<" "<<out[5]<<endl;
              fc+=(1/out[5]-p->_pData[niter]._InvRigidity)*(1/out[5]-p->_pData[niter]._InvRigidity)/(error*error+p->_pData[niter]._ErrInvRigidity*p->_pData[niter]._ErrInvRigidity);
             }
            }
          }
        fc=fc/21;
       }       
     else fc+=(1/out[5]-p->_pData[niter]._InvRigidity)*(1/out[5]-p->_pData[niter]._InvRigidity)/(error*error+p->_pData[niter]._ErrInvRigidity*p->_pData[niter]._ErrInvRigidity);
    }
    npfit++;
   }
  }
  if(npfit < n+2)fc=FLT_MAX;
  else fc=fc/(npfit-n);
  if(p->_flag)HPRINT(101);
  if(p->_flag==2 && npfit>1){
       pav/=npfit;
       pav2/=npfit;
       pav2=sqrt(fabs(pav2-pav*pav));
       p->_tmppav=pav;
       p->_tmppsi=pav2;
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<endl;
       p->_flag=0;        
  }
      if(p->_flag==3){
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<endl;
       p->_tmp=fc;
       p->_flag=0;
      }
}

void AMSTrCalibPar::sqr(){
int i,j;
for(i=0;i<3;i++)_Coo[i]=sqrt(fabs(_Coo[i]));
for(i=0;i<3;i++)_Coo[i]=sqrt(fabs(_Angles[i]));
_a2m();
}




void AMSTrIdCalib::_clear(){
int i;
for(i=0;i<getnchan();i++){
 _Count[i]=0;
 _ADC[i]=0;
 _ADC2[i]=0;
 _ADCRaw[i]=0;
 _ADC2Raw[i]=0;
 _BadCh[i]=0;
 for(int j=0;j<nrho;j++)_ADCRho[j][i]=0;
 _ADCMax[i]=-FLT_MAX;
}

for(i=0;i<10;i++){
 int j;
 for(j=0;j<trid::ms;j++)_CmnNoiseC[i][j]=0;
 for(j=0;j<trid::ms;j++)_CmnNoise[i][j]=0;
}
}

void AMSTrIdCalib::offmonhist(){
HBOOK1(500000+1,"Sigmas K Crate 32",200,0.,20.,0.);
HBOOK1(500000+2,"Sigmas S Crate 32",200,0.,20.,0.);
HBOOK1(500000+10+1,"Sigmas K Crate 72",200,0.,20.,0.);
HBOOK1(500000+10+2,"Sigmas S Crate 72",200,0.,20.,0.);
HBOOK1(600000+1,"Number of Bad Channels Side K Crate 32",70,0.5,70.5,0.);
HBOOK1(600000+2,"Number of Bad Channels Side S Crate 32",70,0.5,70.5,0.);
HBOOK1(600000+10+1,"Number of Bad Channels Side K Crate 72",70,0.5,70.5,0.);
HBOOK1(600000+10+2,"Number of Bad Channels Side S Crate 72",70,0.5,70.5,0.);
HBOOK1(700000+1,"CmnNoise K Crate 32",396,-0.5,395.5,0.);
HBOOK1(700000+2,"CmnNoise S Crate 32",660,-0.5,659.5,0.);
HBOOK1(700000+10+1,"CmnNoise K Crate 72",396,-0.5,395.5,0.);
HBOOK1(700000+10+2,"CmnNoise S Crate 72",660,-0.5,659.5,0.);
  geant badch[70];
  int i,j,k,l,m;
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<70;i++)badch[i]=0;
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft cid(i+1,j+1,k,l,0);
        if(cid.dead())continue;
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.checkstatus(AMSDBc::BAD))badch[i*10+j+1]++;
         HF1(500000+l+1+10*k,cid.getsig(),1.);
        }
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m+=64){
         cid.upd(m);
         //if(!l)cout <<i*11*(!l?6:10)+j*(!l?6:10)+(m/64)<<" "<<cid.getcmnnoise()<<endl;
         HF1(700000+l+1+10*k,i*11*(!l?6:10)+j*(!l?6:10)+(m/64),cid.getcmnnoise());
        }
       }
     }
      for(i=0;i<70;i++)HF1(600000+l+1+10*k,float(i),badch[i]);
    }
   }
   


}

void AMSTrIdCalib::ntuple(integer s){
  if(IOPA.hlun){
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);  
    char filename[256];
    strcpy(filename,hfile);
    integer iostat;
    integer rsize=1024;
    char event[80];  
    sprintf(event,"%d",s);
    strcat(filename,event);
    HROPEN(IOPA.hlun+1,"trcalibration",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening trcalib ntuple file "<<filename<<endl;
     exit(1);
    }
    else cout <<"trcalib ntuple file "<<filename<<" opened."<<endl;

   TrCalib_def TRCALIB;
   HBNT(IOPA.ntuple,"Tracker Calibaration"," ");
   HBNAME(IOPA.ntuple,"TrCalib",(int*)(&TRCALIB),"PSLayer:I,PSLadder:I,PSHalf:I,PSSide:I, PSStrip:I,Ped:R,Sigma:R,BadCh:R,CmnNoise:R");
   int i,j,k,l,m;
    for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        if(id.dead())continue;
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
          id.upd(m);
          TRCALIB.Layer=i+1;
          TRCALIB.Ladder=j+1;
          TRCALIB.Half=k;
          TRCALIB.Side=l;
          TRCALIB.Strip=m;
          TRCALIB.Ped=id.getped();
          TRCALIB.Sigma=id.getsig();
          TRCALIB.BadCh=id.checkstatus(AMSDBc::BAD);
          TRCALIB.CmnNoise=id.getcmnnoise();
          HFNT(IOPA.ntuple);
         }
        }
       }
     }
    }
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//trcalibration";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (1, ICYCL, " ");
  HREND ("trcalibration");
  CLOSEF(IOPA.hlun+1);

}
}
  

void AMSTrIdCalib::_hist(){
  // write down the difference
  HBOOK1(400000+1,"Peds Diff",200,-10.,10.,0.);
  HBOOK1(400010+1,"Peds Calcs",200,0.,2200.,0.);
  HBOOK1(400020+1,"Peds System",200,0.,2200.,0.);
  HBOOK1(400030+1,"CMNNoise",200,0.,100.,0.);
  HBOOK1(400000+2,"Sigmas Diff",200,-2.,8.,0.);
  HBOOK1(400010+2,"Sigmas Calcs",200,0.,20.,0.);
  HBOOK1(400020+2,"Sigmas System",200,0.,20.,0.);
  HBOOK1(400000+3,"Ped Accuracy Estimated",200,0.,1.,0.);
  HBOOK1(400100+1,"Peds Diff",200,-10.,10.,0.);
  HBOOK1(400110+1,"Peds Calcs",200,0.,2200.,0.);
  HBOOK1(400120+1,"Peds System",200,0.,2200.,0.);
  HBOOK1(400100+2,"Sigmas Diff",200,-2.,8.,0.);
  HBOOK1(400110+2,"Sigmas Calcs",200,0.,20.,0.);
  HBOOK1(400120+2,"Sigmas System",200,0.,20.,0.);
  HBOOK1(400100+3,"Ped Accuracy Estimated",200,0.,1.,0.);
  HBOOK1(400200+1,"Peds Diff",200,-10.,10.,0.);
  HBOOK1(400210+1,"Peds Calcs",200,0.,2200.,0.);
  HBOOK1(400220+1,"Peds System",200,0.,2200.,0.);
  HBOOK1(400200+2,"Sigmas Diff",200,-2.,8.,0.);
  HBOOK1(400210+2,"Sigmas Calcs",200,0.,20.,0.);
  HBOOK1(400220+2,"Sigmas System",200,0.,20.,0.);
  HBOOK1(400200+3,"Ped Accuracy Estimated",200,0.,1.,0.);
  PSStr_def PS;
  HBNAME(IOPA.ntuple,"PedsSigmas",(int*)(&PS),"PSLayer:I,PSLadder:I,PSHalf:I,PSSide:I, PSStrip:I,Ped:R,Sigma:R,BadCh:R,SigmaRaw:R,Rho(64):R");
  int i,j,k,l,m;
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        if(cid.dead())continue;
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()>1){
          int ch=cid.getchannel();
          PS.Layer=i+1;
          PS.Ladder=j+1;
          PS.Half=k;
          PS.Side=l;
          PS.Strip=m;
          PS.Ped=_ADC[ch];
          PS.Sigma=_ADC2[ch];
          PS.BadCh=cid.checkstatus(AMSDBc::BAD);
          PS.SigmaRaw=_ADC2Raw[ch];
          PS.Rho[0]=sigmas[ch];
          for(int kk=1;kk<nrho;kk++)PS.Rho[kk]=_ADCRho[kk][ch];
          HFNT(IOPA.ntuple);
          HF1(400000+1,_ADCRaw[ch]-cid.getped(),1.);
          HF1(400010+1,_ADCRaw[ch],1.);
          HF1(400020+1,cid.getped(),1.);
          HF1(400030+1,cid.getcmnnoise(),1.);
          HF1(400000+2,_ADC2[ch]-sigmas[ch],1.);
          HF1(400010+2,_ADC2[ch],1.);
          HF1(400020+2,sigmas[ch],1.);
          HF1(400000+3,_ADC2[ch]/sqrt(double(_Count[ch])),1.);

          HF1(400000+(l+1)*100+1,_ADCRaw[ch]-cid.getped(),1.);
          HF1(400010+(l+1)*100+1,_ADCRaw[ch],1.);
          HF1(400020+(l+1)*100+1,cid.getped(),1.);
          HF1(400000+(l+1)*100+2,_ADC2[ch]-sigmas[ch],1.);
          HF1(400010+(l+1)*100+2,_ADC2[ch],1.);
          HF1(400020+(l+1)*100+2,cid.getsig(),1.);
          HF1(400000+(l+1)*100+3,_ADC2[ch]/sqrt(double(_Count[ch])),1.);
         }
        }
       }
     }
    }
   }
   




  for(i=0;i<getnchan();i++){
    if(_Count[i] > 0){

    }
  }

}

integer AMSTrIdCalib::CalcBadCh(integer half, integer side){
   int badch=0;
     for(int i=0;i<TKDBc::nlay();i++){
       for(int j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,half,side,0);
        if(id.dead())continue;
        for(int m=0;m<TKDBc::NStripsDrp(i+1,side);m++){
         id.upd(m);
         if(id.checkstatus(AMSDBc::BAD))badch++;  
        }
       }
     }
     return badch;
}

void AMSTrIdCalib::printbadchanlist(){
  if(TRCALIB.PrintBadChList){
  ofstream fcluster;
  ofstream fcommon;
  fcluster.open("../datacards/BadChannelList.Clusters",ios::out);
  fcommon.open("../datacards/BadChannelList.CommonNoise",ios::out);


 int i,j,k,l,m;
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        if(id.dead())continue;
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
         id.upd(m);
         if(!id.getsignsigraw())fcommon << hex<< id.gethaddr()<<endl;
         if(id.checkstatus(AMSDBc::BAD))fcluster << hex<< id.gethaddr()<<endl;
        }
       }
     }
    }
   }







  fcluster.close();
  fcommon.close();

    
  }
}


void AMSTrIdCalib::_calc(){
#ifdef __AMSBADLIST__
  ofstream fcluster;
  ofstream fcommon;
if(TRCALIB.Pass ==2){
  fcluster.open("../datacards/BadChannelList.Clusters",ios::out);
  fcommon.open("../datacards/BadChannelList.CommonNoise",ios::out);
}
#endif


 int i,j,k,l,m;
 integer badp[2]={0,0};
 integer bada[2]={0,0};
 integer bad[2]={0,0};
 integer bad2[2]={0,0};
 integer bad3[2]={0,0};
 integer bad4[2]={0,0};
 for(i=0;i<10;i++){
   for(j=0;j<trid::ms;j++){
     if(_CmnNoiseC[i][j]>0){
      _CmnNoise[i][j]=_CmnNoise[i][j]/_CmnNoiseC[i][j];
     }
   }
 }
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        if(cid.dead())continue;
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()>1){
          int ch=cid.getchannel();
          _ADC[ch]=(_ADC[ch]-_ADCMax[ch])/(_Count[ch]-1);
          _ADC2[ch]=(_ADC2[ch]-_ADCMax[ch]*_ADCMax[ch])/(_Count[ch]-1);
          _ADC2[ch]=sqrt(fabs(_ADC2[ch]-_ADC[ch]*_ADC[ch]));
          _ADCRaw[ch]=_ADCRaw[ch]/_Count[ch];
          _ADC2Raw[ch]=_ADC2Raw[ch]/_Count[ch];
          _BadCh[ch]=_BadCh[ch]/_Count[ch];
          _ADC2Raw[ch]=sqrt(fabs(_ADC2Raw[ch]-_ADCRaw[ch]*_ADCRaw[ch]));
          //          if(_ADC2[ch]/sqrt(_Count[ch]-1)>4*TRCALIB.PedAccRequired[l]){
          if(cid.checkstatus(AMSDBc::BAD))badp[l]++;
#ifdef __AMSBADLIST__
            if( TRCALIB.Pass == 2){
               // Put Here N. Produit chan
              if(!cid.getsignsigraw())fcommon << hex<< cid.gethaddr()<<endl;
            }
#endif             
          if( TRCALIB.Pass >= 2){
            geant thr=TRCALIB.BadChanThr[1]*_Count[ch];
            if(thr<4)thr=4;
            if(_BadCh[ch]*_Count[ch]>thr || cid.getsig()==0 ){ 
              cid.setstatus(AMSDBc::BAD);
#ifdef __AMSBADLIST__
            if( TRCALIB.Pass == 2){
               // Put Here N. Produit chan
              fcluster << hex<< cid.gethaddr()<<endl;
            }
#endif             
              bad[l]++;
            }
            else cid.clearstatus(AMSDBc::BAD);
            // special ladder#6 layer #3 side# 0
            //               if(cid.getlayer() == 3  && cid.getdrp() == 6 && cid.getside() == 0
            //                  && cid.gethalf()==0){
            //                   cid.setstatus(AMSDBc::BAD);            
            //#ifdef __AMSBADLIST__
            //            if( TRCALIB.Pass == 2){
            //               // Put Here N. Produit chan
            //              fcluster << hex <<cid.gethaddr()<<endl;
            //            }
            //#endif             

            //               }
            // special ladder#9 layer #2 side# 0 half# 1
               //               if(cid.getlayer() == 2  && cid.getdrp() == 9 && cid.getside() == 0
               //                  && cid.gethalf()==1)cid.setstatus(AMSDBc::BAD);            
          }
          if(cid.checkstatus(AMSDBc::BAD))bada[l]++;

          
         }
        }


        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()>1){
          int ch=cid.getchannel();
          number s1=_ADCRho[cid.getstripa()][ch]/_Count[ch];
          integer vamin=m-cid.getstripa();
          for (int kk=cid.getstripa();kk<nrho;kk++){
           _ADCRho[kk][ch]=_ADCRho[kk][ch]/_Count[ch];
           cid.upd(vamin+kk);
           int ch2=cid.getchannel();
           if(TRMCFFKEY.GenerateConst) {
             if(_ADC2Raw[ch] && _ADC2Raw[ch2])
              _ADCRho[kk][ch]=(_ADCRho[kk][ch]-_ADCRaw[ch]*_ADCRaw[ch2])/
                 _ADC2Raw[ch]/_ADC2Raw[ch2];
             else _ADCRho[kk][ch]=0;
           }
           else {
             number a1=(_ADC[ch]*(_Count[ch]-1)+_ADCMax[ch])/_Count[ch];
             number a2=(_ADC[ch2]*(_Count[ch2]-1)+_ADCMax[ch2])/_Count[ch2];
             number s2=ch2==ch?s1:_ADCRho[cid.getstripa()][ch2]/_Count[ch2];
             number s1a=sqrt(fabs(s1-a1*a1));
             number s2a=sqrt(fabs(s2-a2*a2));
                          
             if(s1a && s2a)_ADCRho[kk][ch]=(_ADCRho[kk][ch]-a1*a2)/s1a/s2a;
             else _ADCRho[kk][ch]=0;
           }
          }
         }
        }

        //        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
        //         cid.upd(m);
        //         if(cid.getcount()>1){
        //          int ch=cid.getchannel();
        //          _ADC[ch]=_ADC[ch]+cid.getcmnnoise();
        //         }
        //        }


       }
     }
    }
#ifdef __AMSBADLIST__
            if( TRCALIB.Pass == 2){
              fcluster.close();
              fcommon.close();
            }
#endif             

    if(bad[l])cout <<"AMSTrIdCalib::_calc-W-bad channels found for side "<<l<<" : "<<bad[l]<<endl;
    if(badp[l])cout <<"AMSTrIdCalib::_calc-W-badp channels found for side "<<l<<" : "<<badp[l]<<endl;
    if(bada[l])cout <<"AMSTrIdCalib::_calc-W-bada channels found for side "<<l<<" : "<<bada[l]<<endl;
   }
   if(TRMCFFKEY.GenerateConst){
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        if(cid.dead())continue;
        geant rhorho[nrho][nrho];
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
         integer mstr=(m+1)%nrho;
         integer istr=m%nrho;
         cid.upd(m);
         if(cid.getcount()>1){
         integer vamin=m-cid.getstripa();
         int ch=cid.getchannel();
         int kk;
         for(kk=istr+1;kk<nrho;kk++)rhorho[kk][istr]=_ADCRho[kk][ch];
         if(mstr==0){
           for(kk=0;kk<nrho;kk++){
            for(int kj=kk+1;kj<nrho;kj++)rhorho[kk][kj]=rhorho[kj][kk];
           } 
           integer xmc=0;
           for(kk=0;kk<nrho;kk++){
            geant xrho=0;
            cid.upd(vamin+kk);
            for(int kj=0;kj<nrho;kj++){
            if(kj!=kk)xrho+=rhorho[kk][kj];
             if(kj==kk || rhorho[kk][kj]< TRCALIB.RhoThrV)cid.clearrhomatrix(kj);
             else cid.setrhomatrix(kj);
            }
            xrho=xrho/(nrho-1);
            if(xrho < TRCALIB.RhoThrA ){
             cid.upd(vamin+kk);
             _ADC2Raw[cid.getchannel()]=-_ADC2Raw[cid.getchannel()];
             bad2[l]++;
            }
            else xmc++;
           }
           if(xmc<2)bad3[l]++;
         }         
         }
        }
       }
     }
    }
    if(bad2[l])cout <<"AMSTrIdCalib::_calc-W-noise channels found for side "
    <<l<<" : "<<bad2[l]<<endl;
    if(bad2[l])cout <<"AMSTrIdCalib::_calc-W-bad VA found for side "
    <<l<<" : "<<bad3[l]<<endl;
   }
   }
}


void AMSTrIdCalib::_update(){
 int i,j,k,l,m;
   for(k=0;k<2;k++){
    int total=0;
     for(l=0;l<2;l++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        if(cid.dead())continue;
        number oldone=0;
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()){
          total++;
          int ch=cid.getchannel();
          cid.setped()=_ADCRaw[ch];
          cid.setsig()=_ADC2[ch];
          cid.setsigraw()=(_ADC2Raw[ch]==0?0:1./_ADC2Raw[ch]);
          cid.setindnoise()=oldone+
          AMSTrMCCluster::sitknoiseprob(cid ,cid.getsig()*TRMCFFKEY.thr1R[l]);
          oldone=cid.getindnoise();

         }
        }
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m+=64){
          number avs=0;
          number navs=0;
          for(int mm=0;mm<64;mm++){
           cid.upd(mm);
           if(cid.getcount()){
           int ch=cid.getchannel();
            avs+=_ADC2[ch]*_ADC2[ch];
            navs++;
           }
           if(navs)avs=avs/navs;
           cid.setcmnnoise()=sqrt(cid.getcmnnoise()-avs);
          }
        }



       }
     }
     }
    if (AMSFFKEY.Update){
    AMSTimeID *ptdv;
     time_t begin,end,insert;
    if(total){
     if(TRMCFFKEY.GenerateConst){
      ptdv = AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVrawsigma(k));
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      ptdv->SetTime(insert,getrun()-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
      cout <<" Tracker H/K  info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);



     ptdv = AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVrhomatrix(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     ptdv->SetTime(insert,getrun()-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);



     }
     if(TRCALIB.Pass == 1 || TRMCFFKEY.GenerateConst ){  

     ptdv = 
     AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVped(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     ptdv->SetTime(insert,getrun()-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);

     ptdv = AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVsigma(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     ptdv->SetTime(insert,getrun()-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);
     }
     if(TRCALIB.Pass ==1 && k==1 && !TRMCFFKEY.GenerateConst){

      ptdv = 
      AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVCmnNoise());
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      ptdv->SetTime(insert,getrun()-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
      cout <<" Tracker H/K  info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);


      //      ptdv = 
      //      AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVIndNoise());
      //      ptdv->UpdateMe()=1;
      //      ptdv->UpdCRC();
      //      time(&insert);
      //      ptdv->SetTime(insert,getrun()-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
      //      cout <<" Tracker H/K  info has been updated for "<<*ptdv;
      //      ptdv->gettime(insert,begin,end);
      //      cout <<" Time Insert "<<ctime(&insert);
      //      cout <<" Time Begin "<<ctime(&begin);
      //      cout <<" Time End "<<ctime(&end);


     }
     if(TRCALIB.Pass ==2){

     ptdv = AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVstatus(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     ptdv->SetTime(insert,getrun()-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);

     }






    cout << "AMSTrIdCalib::_update-I-total of "<<total<<" channels updated"<<endl;
    }
   }




        // Here update dbase

     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
      if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
      cerr <<"AMSJob::_timeinitjob-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    }



    _BeginTime=_CurTime;

   }


integer * AMSTrIdCalib::_Count=0;
geant * AMSTrIdCalib::_BadCh=0;
number  *  AMSTrIdCalib::_ADC=0;
number *  AMSTrIdCalib::_ADC2=0;
number  *  AMSTrIdCalib::_ADCRaw=0;
number *  AMSTrIdCalib::_ADC2Raw=0;
number * AMSTrIdCalib::_ADCRho[nrho];
number *  AMSTrIdCalib::_ADCMax=0;
integer  AMSTrIdCalib::_CmnNoiseC[10][trid::ms];
geant  AMSTrIdCalib::_CmnNoise[10][trid::ms];


void AMSTrIdCalib::initcalib(){
_Count= new integer[getnchan()];
_BadCh= new geant[getnchan()];
_ADC= new number[getnchan()];
_ADC2= new number[getnchan()];
_ADCRaw= new number[getnchan()];
_ADC2Raw= new number[getnchan()];
for(int i=0;i<nrho;i++)_ADCRho[i]=new number[getnchan()];
_ADCMax= new number[getnchan()];

assert (_Count && _ADC && _ADC2 && _ADCMax && _ADC2Raw && _ADCRaw && _BadCh);
_clear();


  {
    for (int i=0;i<6;i++){
     for(int j=1;j<11;j++){
      HBOOK1(10000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(20000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(30000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(60000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(40000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(80000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(110000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(120000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(130000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(160000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(140000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(180000+i*10+j,"Noise",200,-10.,10.,0.);
     }
    }
  }


}


void AMSTrIdCalib::check(integer forcedw){
static integer counter=0;
static integer hist=0;
if(++counter%TRCALIB.EventsPerCheck == 0 || forcedw){
 int i,j,k,l,m;
    number acc[2]={0,0};
    number cnt[2]={0,0};
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        if(cid.dead())continue;
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()){
          int ch=cid.getchannel();
          acc[l]=acc[l]+
          sqrt(fabs(_ADC2[ch]-_ADC[ch]*_ADC[ch]/_Count[ch]))/_Count[ch];
          cnt[l]=cnt[l]+1;
         }
        }
       }
     }
    }
   }

   if(cnt[0]>0 && cnt[1]>0){
     if(!forcedw && (acc[0]/cnt[0] < TRCALIB.PedAccRequired[0] &&  acc[1]/cnt[1] < TRCALIB.PedAccRequired[1])){
       cout << "AMSTrIdCalib::check-I-peds & sigmas succesfully calculated with accuracies "<<
         acc[0]/cnt[0]<<" "<<acc[1]/cnt[1]<<" ( "<<counter<<" ) events."<<endl;
       cout << "AMSTrIdCalib::check-I-peds & sigmas succesfully calculated for  "<< cnt[0]+cnt[1]<< " Channels"<<endl;
       _calc();
       _hist();
       _update();
       _clear();
       counter=0;
       hist=1;
     }
     else if(forcedw && hist==0){
       cout << "AMSTrIdCalib::check-I-peds & sigmas succesfully calculated with accuracies "<<
         acc[0]/cnt[0]<<" "<<acc[1]/cnt[1]<<" ( "<<counter<<" ) events."<<endl;
       cout << "AMSTrIdCalib::check-I-peds & sigmas succesfully calculated for  "<< cnt[0]+cnt[1]<< " Channels"<<endl;
       _calc();
       _hist();
       _update();
       _clear();
       counter=0;
     }
   }
}
}


void AMSTrIdCalib::buildSigmaPedB(integer n, int16u *p){
   _CurTime=AMSEvent::gethead()->gettime();
   //if(_CurRun > 0 && _CurRun != AMSEvent::gethead()->getrun())check(1);
   _CurRun=AMSEvent::gethead()->getrun();
  integer static first=0;
  if(first++ == 0){
    _BeginTime=AMSEvent::gethead()->gettime();
  }

  integer const ms=640;
  integer len;
  static geant id[ms];
  //VZERO(id,ms*sizeof(id[0])/sizeof(integer));
  int i,j,k;
  integer ic=AMSTrRawCluster::checkdaqidRaw(*p)-1;
  integer mixed=0;
  if(ic<0){
      ic=AMSTrRawCluster::checkdaqidMixed(*p)-1;
      mixed=2;
      static int mess=1;
      if(mess){
        cout <<"AMSTrIdCalib::buildSigmaPedB-I-MixedModeFound"<<endl;
        mess=0;
      }
  }
  int16u * ptr=p+1;
  // Main loop
  integer oldformat=1;
  while (ptr<p+n){
    // Read two tdrs
    uinteger subl=*ptr;
    if(subl ==0){
      cerr <<"AMSTrRawCluster::buildsigmaspedb-E-SubLengthZero, skipped event"<<endl;
      return;
    }

    integer ntdr = *(ptr+1) & 31;
    int16u *ptro=ptr;
    if(subl != 3084 || mixed){
      // Probably new format
      oldformat=0; 
    }
    ptr+=2;
    for(i=0;i<ntdr;i++){
     int16u tdrn=*ptr;
     int16u lrec=*(ptr+1);
     ptr++;
     if(tdrn < 16){
       // S side
       len=640;
       for(j=0;j<2;j++){
         int16u conn,tdrs;
         tdrs=tdrn/2;
         if(tdrn%2==0){
          if(j==0)conn=0;
          else conn=1;
         }
         else {
          if(j==0)conn=2;
          else conn=3;
         }
         int16u haddr=(conn<<10) | (tdrs <<12);
         AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<320;k++){
           idd.upd(k);
           id[k]=float(*(ptr+mixed+2+k+j*(640+128*oldformat))); 
          }
          for(k=320;k<640;k++){
           idd.upd(k);
           id[k]=float(*(ptr+mixed+2+k+64*oldformat+j*(640+128*oldformat))); 
          }
          buildpreclusters(idd,len,id);
         }
       }
     }
     else if(tdrn<24){
       // K Side
       len=384;
       for(j=0;j<4;j++){
          int16u conn, tdrk;
          if(tdrn%2 ==0){
            if(j<2)conn=j;
            else conn=j;
          }
          else {
           if(j<2)conn=j;
           else conn=j;
           conn+= 4;
          }
          tdrk=(tdrn-16)/2;
          int16u haddr=(10<<6) |( conn<<10) | (tdrk <<13);
          AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<384;k++){
           idd.upd(k);
          id[k]=float(*(ptr+mixed+2+k+j*384)); 
          }
          buildpreclusters(idd,len,id);
         }
       }
     }
     else{
       cerr<<"AMSTrIdCalib::buildSigmaPedB-E-TDRNOutOfRange "<<tdrn<<endl;
     }
     ptr+=lrec;
    }
    if(!oldformat)ptr=ptro+subl;
  }





}


void AMSTrIdCalib::buildSigmaPed(integer n, int16u *p){
   _CurTime=AMSEvent::gethead()->gettime();
   _CurRun=AMSEvent::gethead()->getrun();
  integer static first=0;
  if(first++ == 0){
    _BeginTime=AMSEvent::gethead()->gettime();
  }
  integer const maxva=64;
  integer const mss=640;
  static geant id[mss];
  static geant idlocal[maxva];
  VZERO(id,trid::ms*sizeof(id[0])/sizeof(integer));
  int i,j,k,l;
  integer ic=AMSTrRawCluster::checkdaqidRaw(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
     AMSTrIdSoft idd(ic,int16u(*(ptr)));
     AMSTrIdCalib cid(idd);
     //aux loop thanks to data format to calculate corr length
     int16u * paux;
     int len=0;
     int16u bit15=1<<15;
     for(paux=ptr+1;paux<p+n;paux++){
      if( !(bit15 & *paux))break;
      else len++;
     }
     if(len > mss ){
       cerr <<" AMSTrIdCalib::buildSigmaPed-S-LengthError Max is "<<mss <<" Current is "<<len<<endl;
      len=mss;
     }
     if(!idd.dead()){
     for(j=0;j<len;j++){
      cid.upd(j);
      id[j]=float((*(ptr+1+j)) & 4095);
     }
     // calc cmn noise
      integer vamin,vamax,l;
      for (j=0;j<len;j+=maxva){
         cid.upd(j);
         vamin=j-cid.getstripa();
         vamax=j+maxva-cid.getstripa();
         geant cmn=0;
         geant avsig=0;
         for (l=vamin;l<vamax;l++){
           cid.upd(l);
           cmn+=(id[l]-cid.getped())*cid.getsignsigraw();
           avsig+=cid.getsignsigraw();
         }
         if(avsig>1)cmn=cmn/avsig;
         cid.updcmnnoise(cmn);
         cid.updcmnnoiseC();
         for(l=vamin;l<vamax;l++){
           cid.upd(l);
           cid.updADCRaw(id[l]);         
           cid.updADC2Raw(id[l]);         
           if(TRMCFFKEY.GenerateConst){
             integer vamin=l-cid.getstripa();
             for(int kk=cid.getstripa();kk<nrho;kk++)
             cid.updADCRho(id[l],id[vamin+kk],kk);
           }
           id[l]+=-cmn;
         }         
      }
      for(j=0;j<len;j++){
       cid.upd(j);
       integer ids=(cid.getlayer()-1)*10+cid.getdrp();
       if(cid.getsig()!=0 && TRCALIB.Pass>1){
          geant r=(id[j]-cid.getped())/cid.getsig();
          if(r<-TRCALIB.BadChanThr[0])cid.updBadCh();
          if(TRCALIB.Pass >= 2){
           HF1(100000*cid.gethalf()+(cid.getside()+1)*10000+ids,r,1.);
           if(cid.checkstatus(AMSDBc::BAD)){
              HF1(100000*cid.gethalf()+(cid.getside()+1)*40000+ids,r,1.);
           }
           else HF1(100000*cid.gethalf()+(cid.getside()+1)*30000+ids,r,1.);
          }
       }
       cid.updADC(id[j]);
       cid.updADC2(id[j]);
       cid.updADCMax(id[j]);
       if(!TRMCFFKEY.GenerateConst && TRCALIB.Pass==1){
           integer vamin=j-cid.getstripa();
          for(int kk=cid.getstripa();kk<nrho;kk++)
           cid.updADCRho(id[j],id[vamin+kk],kk);
       }
       cid.updcounter();
      }
     }
    ptr=ptr+len+1;

  }
}



void AMSTrIdCalib::buildSigmaPedA(integer n, int16u *p){
   _CurTime=AMSEvent::gethead()->gettime();
   _CurRun=AMSEvent::gethead()->getrun();
  integer static first=0;
  if(first++ == 0){
    _BeginTime=AMSEvent::gethead()->gettime();
  }
  integer const maxva=64;
  integer const mss=640;
  static geant id[mss];
  static geant idlocal[maxva];
  VZERO(id,trid::ms*sizeof(id[0])/sizeof(integer));
  int i,j,k,l;
  integer ic=AMSTrRawCluster::checkdaqidRaw(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
     AMSTrIdSoft idd(ic,int16u(*(ptr)));
     AMSTrIdCalib cid(idd);
     //aux loop thanks to data format to calculate corr length
     int16u * paux;
     int len=0;
     int16u bit15=1<<15;
     for(paux=ptr+1;paux<p+n;paux++){
      if( !(bit15 & *paux))break;
      else len++;
     }
     if(len > mss ){
       cerr <<" AMSTrIdCalib::buildSigmaPedA-S-LengthError Max is "<<mss <<" Current is "<<len<<endl;
      len=mss;
     }
     if(!idd.dead()){
     for(j=0;j<len;j++){
      cid.upd(j);
      id[j]=float((*(ptr+1+j)) & 4095);
     }
       buildpreclusters(idd,len,id);
       
     }
    ptr=ptr+len+1;

  }
}



time_t AMSTrIdCalib::_BeginTime=0;
time_t AMSTrIdCalib::_CurTime=0;
uinteger AMSTrIdCalib::_CurRun=0;



void AMSTrIdCalib::buildpreclusters(AMSTrIdSoft & idd, integer len, geant id[]){
     // calc cmn noise
  integer const maxva=64;
  static geant idlocal[maxva];
  integer ic=idd.gethalf();
      int i,j,k;
      AMSTrIdCalib cid(idd);

     // calc cmn noise
      integer vamin,vamax,l;
      for (j=0;j<len;j+=maxva){
         cid.upd(j);
         vamin=j-cid.getstripa();
         vamax=j+maxva-cid.getstripa();
           geant cmn=0;
           geant avsig=0;
         for (l=vamin;l<vamax;l++){
           cid.upd(l);
           cmn+=(id[l]-cid.getped())*cid.getsignsigraw();
           avsig+=cid.getsignsigraw();
           idlocal[l-vamin]=id[l]-cid.getped();
         }
         if(avsig>1)cmn=cmn/avsig;
         cid.updcmnnoise(cmn);
         cid.updcmnnoiseC();
         for (l=vamin;l<vamax;l++){
           cid.upd(l);
           cid.updADCRaw(id[l]);         
           cid.updADC2Raw(id[l]);         
           if(TRMCFFKEY.GenerateConst){
             integer vamin=l-cid.getstripa();
             for(int kk=cid.getstripa();kk<nrho;kk++)
             cid.updADCRho(id[l],id[vamin+kk],kk);
           }
           if(!cid.getsignsigraw() ){
            geant cmn=0;
            geant avsig=0;
           for(int kk=0;kk<maxva;kk++){
            if(cid.getrhomatrix(kk)){
             cid.upd(kk+vamin);
                          cmn+=idlocal[kk]*fabs(cid.getsigraw());
                          //             cmn+=idlocal[kk];
             avsig++;
             cid.upd(l);
            }
           }
            if(avsig>1 && cid.getsigraw()!=0){
                           cmn=cmn/avsig/fabs(cid.getsigraw());
                           //             cmn=cmn/avsig;
            }          
            id[l]+=-cmn;
           }
           else id[l]+=-cmn;
         }

         for(l=vamin;l<vamax;l++){
          cid.upd(l);
       integer ids=(cid.getlayer()-1)*10+cid.getdrp();
       if(cid.getsig()!=0 && TRCALIB.Pass>1){
          geant r=(id[l]-cid.getped())/cid.getsig();
          if(r<-TRCALIB.BadChanThr[0])cid.updBadCh();
          if(TRCALIB.Pass >= 2) {
           HF1(100000*cid.gethalf()+(cid.getside()+1)*10000+ids,r,1.);
           if(cid.checkstatus(AMSDBc::BAD)){
               HF1(100000*cid.gethalf()+(cid.getside()+1)*40000+ids,r,1.);
           }
           else HF1(100000*cid.gethalf()+(cid.getside()+1)*30000+ids,r,1.);
          }
       }
          cid.updADC(id[l]);
          cid.updADC2(id[l]);
          cid.updADCMax(id[l]);
          if(!TRMCFFKEY.GenerateConst && TRCALIB.Pass ==1){
            integer vamin=l-cid.getstripa();
            for(int kk=cid.getstripa();kk<nrho;kk++)
             cid.updADCRho(id[l],id[vamin+kk],kk);
          }
          cid.updcounter();

        }
      }


      // Dynamically Update pedestals if needed
      if(TRCALIB.DPS && TRCALIB.Pass>=2){
       for(j=0;j<len;j++){
          idd.upd(j);
          if(fabs(id[j]-idd.getped())<TRCALIB.DPS*idd.getsig()){
           idd.setped()=idd.getped()+(id[j]-idd.getped())/TRCALIB.UPDF;
         }           
       }
      }




}



void AMSTrIdCalib::addonemorecalib(){


   int i,j,k,l,m;
    for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft cid(i+1,j+1,k,l,0);
        AMSTrIdCalib id(cid);
        if(id.dead())continue;
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
          id.upd(m);
          id.updADC(id.getped());
          id.updADCRaw(id.getsig());
          if(id.checkstatus(AMSDBc::BAD))id.updBadCh();
          id.updcounter();
          if(m%64==0){
           id.updcmnnoise(id.getcmnnoise());
           id.updcmnnoiseC();
          }
         }
        }
       }
     }
    }



}



void AMSTrIdCalib::getaverage(){


    //get average
   int i,j,k,l,m;

    for(i=0;i<10;i++){
      for(j=0;j<trid::ms;j++){
       if(_CmnNoiseC[i][j]>0){
        _CmnNoise[i][j]=sqrt(_CmnNoise[i][j]/_CmnNoiseC[i][j]);
       }
      }
    }

    for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft cid(i+1,j+1,k,l,0);
        AMSTrIdCalib id(cid);
        if(id.dead())continue;
        geant xn=0;
        geant localbad[640];
        geant * localbada[640];
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
          id.upd(m);
          if(id.getcount())id.setped()=id.getADC()/id.getcount();
          if(id.getcount())id.setsig()=id.getADCRaw()/id.getcount();
          localbada[m]=localbad+m;
          if(id.getcount()){
            localbad[m]=id.getBadCh()/id.getcount();
            xn+=localbad[m];
          }
          else localbad[m]=0;
          id.clearstatus(AMSDBc::BAD);
          if(m%64==0){
           id.setcmnnoise()=id.getcmnnoise();
          }
         }
        // now set up bad channels
        int nbad=floor(xn+0.5);
        cout <<"nbad "<<nbad<<endl;;
        // now order the localbad
        AMSsortNAG(localbada,TKDBc::NStripsDrp(i+1,l));
        for(m=TKDBc::NStripsDrp(i+1,l)-1;m>TKDBc::NStripsDrp(i+1,l)-1-nbad;m--){
          integer la=localbada[m]-localbad;
          cout <<la<<" "<<localbad[la]<<endl;
          id.upd(la);
          id.setstatus(AMSDBc::BAD);
        }
       }
      }
     }
    }





   //update db
    AMSTimeID *ptdv;
     time_t begin,end,insert;
     for( k=0;k<2;k++){
      ptdv = 
      AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVped(k));
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      begin=SELECTFFKEY.Run;
      end=TRCALIB.MultiRun;
      ptdv->SetTime(insert,begin,end);

      ptdv = 
      AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVsigma(k));
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      begin=SELECTFFKEY.Run;
      end=TRCALIB.MultiRun;
      ptdv->SetTime(insert,begin,end);
      ptdv = 
      AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVCmnNoise());
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      begin=SELECTFFKEY.Run;
      end=TRCALIB.MultiRun;
      ptdv->SetTime(insert,begin,end);
      ptdv = 
      AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVstatus(k));
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      begin=SELECTFFKEY.Run;
      end=TRCALIB.MultiRun;
      ptdv->SetTime(insert,begin,end);
     }

    if (AMSFFKEY.Update==2 ){
     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
      if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
      cerr <<"AMSJob::_dbendjob-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    }

   // write ntuple
  if(1){
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);  
    char filename[256];
    strcpy(filename,hfile);
    integer iostat;
    integer rsize=1024;
    char event[80];  
    HROPEN(IOPA.hlun+1,"trcalibration",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening trcalib ntuple file "<<filename<<endl;
     exit(1);
    }
    else cout <<"trcalib ntuple file "<<filename<<" opened."<<endl;

   TrCalib_def TRCALIB;
   HBNT(IOPA.ntuple,"Tracker Calibaration"," ");
   HBNAME(IOPA.ntuple,"TrCalib",(int*)(&TRCALIB),"PSLayer:I,PSLadder:I,PSHalf:I,PSSide:I, PSStrip:I,Ped:R,Sigma:R,BadCh:R,CmnNoise:R");
   int i,j,k,l,m;
    for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        if(id.dead())continue;
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
          id.upd(m);
          TRCALIB.Layer=i+1;
          TRCALIB.Ladder=j+1;
          TRCALIB.Half=k;
          TRCALIB.Side=l;
          TRCALIB.Strip=m;
          TRCALIB.Ped=id.getped();
          TRCALIB.Sigma=id.getsig();
          TRCALIB.BadCh=id.checkstatus(AMSDBc::BAD);
          TRCALIB.CmnNoise=id.getcmnnoise();
          HFNT(IOPA.ntuple);
         }
        }
       }
     }
    }
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//trcalibration";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (1, ICYCL, " ");
  HREND ("trcalibration");
  CLOSEF(IOPA.hlun+1);

}
  

}



