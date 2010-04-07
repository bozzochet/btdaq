//  $Id: ntuple.C,v 1.4 2007/11/14 14:21:17 zuccon Exp $
//
//  Jan 2003, A.Klimentov implement MemMonitor from S.Gerassimov
//
//  

#include "commons.h"
#include "node.h"
#include "ntuple.h"
#include "job.h"
#include "ecaldbc.h"


#include <iostream>
#include <iomanip>
#ifndef __MACOSX__
#include <malloc.h>
#endif
#include <time.h>
#include <strstream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef __CORBA__
#include "producer.h"
#endif

#ifdef __WRITEROOT__
#include "TBranch.h"
#include "TH1.h"


TTree* AMSNtuple::_tree=0;
TFile* AMSNtuple::_rfile=0;
TObjString AMSNtuple::_dc("");
const int branchSplit=1;

#endif

const int MEMUPD = 100; // update Memory Monitor Histograms each 
                        // MEMUPD events 

AMSNtuple::~AMSNtuple(){
#ifdef __WRITEROOT__
 if(_rfile){
   _rfile->Write();
   _rfile->Close();
   delete _rfile;
 }    
#endif
}

AMSNtuple::AMSNtuple(char* name) : AMSNode(AMSID(name,0)) {
  _lun=0;
//  initR(name);
}

AMSNtuple::AMSNtuple(integer lun, char* name) : AMSNode(AMSID(name,0)) {
  _lun = lun;
  init();
}

void AMSNtuple::init(){
  _Nentries=0;
  char *a=new char[strlen(getname())+1];
  strcpy(a,getname());
  HBNT(_lun, a," ");
  delete []a;
// -----> Station :
//
  HBNAME(_lun,"EventH",&_event02.Eventno,
 
         "eventno:I,run:I,runtype:I,time(2):I,RawWords:I,RadS:R,ThetaS:R,PhiS:R,YawS:R,PitchS:R,RollS:R,VelocityS:R,VelTheta:R,VelPhi:R,ThetaM:R,PhiM:R,ISSEqAsc:R,ISSEqDec:R,ISSGalLong:R,ISSGalLat:R,AMSEqAsc:R,AMSeqDec:R,AMSGalLat:R,AMSGalLong:R,TrTracks[0,1000]:I,TrRecHits[0,32766]:I,TrClusters[0,10000]:I,TrRawClusters[0,10000]:I,TrMCClusters[0,10000]:I,TOFClusters[0,1000]:I,TOFMCClusters[0,10000]:I,AntiMCClusters[0,10000]:I,TRDMCClusters[0,2000]:I,AntiClusters[0,100]:I,EcalClusters[0,500]:I,EcalHits[0,5000]:I," //ISN
"RICMCClusters[0,10000]:I,RICHits[0,10000]:I,TRDRawHits[0,2000]:I,TRDClusters[0,1000]:I,TRDSegments[0,500]:I,TRDTracks[0,255]:I,EventStatus(2):I"); 

  HBNAME(_lun,"Beta",&_beta02.Nbeta,
      "nbeta[0,20],betastatus(nbeta):I,betapattern(nbeta)[0,100]:I,beta(nbeta),betac(nbeta),betaerror(nbeta),betaerrorc(nbeta),betachi2(nbeta),betachi2s(nbeta),betantof(nbeta)[0,4]:I,betaptof(4,nbeta)[-1,1000]:I,betaptr(nbeta)[-1,1000]:I");

  HBNAME(_lun,"Charge",&_charge02.Ncharge,
    "ncharge[0,20],chargestatus(ncharge):I,chargebetap(ncharge)[-1,30000]:I,chargeringp(ncharge)[-1,30000]:I,chargetof(ncharge)[0,100]:I,chargetracker(ncharge)[0,100]:I,chargerich(ncharge)[0,100]:I,probtof(4,ncharge),chintof(4,ncharge)[0,100]:I,probtracker(4,ncharge),chintracker(4,ncharge)[0,100]:I,probrich(4,ncharge),chinrich(4,ncharge)[0,100]:I,proballtracker(ncharge),truntof(ncharge),truntofd(ncharge),truntracker(ncharge)");

  HBNAME(_lun,"TRDMCCl",&_trdclmc.Ntrdclmc,
  "ntrdmccl[0,200],trdlayermc(ntrdmccl)[0,20]:I,trdladdermc(ntrdmccl)[0,40]:I,trdtubemc(ntrdmccl)[0,100]:I,trdpartmc(ntrdmccl)[-1000,1000]:I,trdedepmc(ntrdmccl),trdekinmc(ntrdmccl),trdxglmc(3,ntrdmccl),trdstepmc(ntrdmccl)");

  HBNAME(_lun,"TRDRHT",&_trdht.Ntrdht,
  "ntrdht[0,200],trdhtlayer(ntrdht)[0,19]:I,trdhtladder(ntrdht)[0,17]:I,trdhttube(ntrdht)[0,15]:I,trdampht(ntrdht)");

  HBNAME(_lun,"TRDSEG",&_trdseg.Ntrdseg,
  "ntrdseg[0,100],trdsegstatus(ntrdseg):I,trdsegor(ntrdseg)[0,10]:I,trdsegfpar(2,ntrdseg),trdsegchi2(ntrdseg),trdsegpat(ntrdseg)[0,100]:I,trdsegnh(ntrdseg)[0,12]:I,trdsegpcl(12,ntrdseg)[0,1000]:I");

  HBNAME(_lun,"TRDTRK",&_trdtrk.Ntrdtrk,
  "ntrdt[0,40],trdtstatus(ntrdt):I,trdtcoo(3,ntrdt),trdtercoo(3,ntrdt),trdtphi(ntrdt),trdttheta(ntrdt),trdtchi2(ntrdt),trdtns(ntrdt)[0,5]:I,trdtpat(ntrdt)[0,100]:I,trdtps(5,ntrdt):I");

  HBNAME(_lun,"TRDClu",&_trdcl.Ntrdcl,
  "ntrdcl[0,100],trdclstatus(ntrdcl):I,trdclcoo(3,ntrdcl),trdlayer(ntrdcl)[0,20]:I,trddir(3,ntrdcl),trdmul(ntrdcl)[0,31]:I,trdhmul(ntrdcl)[0,31]:I,trdedep(ntrdcl),ptrdrht(ntrdcl)[0,1000]:I");

  HBNAME(_lun,"Particle",&_part02.Npart,
 
"npart[0,10],pstatus(npart):I,pbetap(npart)[-1,254]:I,pchargep(npart)[-1,254]:I,ptrackp(npart)[-1,254]:I,ptrdp(npart)[-1,254]:I,pvertp(npart)[-1,7]:I,prichp(npart)[-1,254]:I,pecalp(npart)[-1,254]:I,pid(npart)[0,1000]:I,pidvice(npart)[0,1000]:I,probpid(2,npart),fitmom(npart),pmass(npart),perrmass(npart),pmom(npart),perrmom(npart),pbeta(npart),perrbeta(npart),pcharge(npart),ptheta(npart),pphi(npart),thetagl(npart),phigl(npart),pcoo(3,npart),cutoff(npart),cootof(3,4,npart),cooanti(3,2,npart),cooecal(3,3,npart),cootr(3,8,npart),cootrd(3,npart),coorich(3,2,npart),pathrich(2,npart):R,pathrichb(2,npart):R,lengthrich(npart):R,trdlikelihood(npart):R,local(8,npart):R");
//
  HBNAME(_lun,"TOFClust",&_tof.Ntof,
 
"ntof[0,48],TOFStatus(ntof):I,TOFpln(ntof)[0,4]:I,TOFbar(ntof)[0,12]:I,TOFmem(ntof)[0,3]:I,TOFp2m(3,ntof)[0,48]:I,TOFEdep(ntof),TOFEdepd(ntof),TOFTime(ntof),TOFETime(ntof),TOFCoo(3,ntof),TOFErCoo(3,ntof)");
//

  HBNAME(_lun,"EcalClus",&_ecclust.Neccl,
 "neccl[0,60],EcclStatus(neccl):I,EcclProj(neccl)[0,1]:I,EcclPlane(neccl)[0,20]:I,EcclLeft(neccl)[0,72]:I,EcclCenter(neccl)[0,72]:I,EcclRight(neccl)[0,72]:I,EcclEdep(neccl),ecclsleak(neccl),eccldleak(neccl),EcclCoo(3,neccl),ecclpleft(neccl):I,ecclnhits(neccl)[0,72]:I");

  HBNAME(_lun,"Ecal2dCl",&_ec2dclust.Nec2dcl,
 "nec2d[0,10],ec2dstatus(nec2d):I,Ec2dProj(nec2d)[0,1]:I,Ec2dNmemb(nec2d):I,Ec2dEdep(nec2d),Ec2dCoo(nec2d),ec2dtan(nec2d),ec2dchi2(nec2d),ec2dpcl(18,nec2d)[0,1000]:I");

  HBNAME(_lun,"EcalShow",&_ecshow.Necsh,
 "necsh[0,5],ecshstatus(necsh):I,Ecshdir(3,necsh),ecshemdir(3,necsh),Ecshentry(3,necsh),Ecshexit(3,necsh),Ecshcofg(3,necsh),EcshErdir(necsh),ecshchi2dir(necsh),ecshfrontedep(necsh),EcshEn(necsh),ecsherat(3,necsh),Ecshener(necsh),ecshdifosum(necsh),EcshsLeak(necsh),EcshrLeak(necsh),EcshdLeak(necsh),EcshaLeak(necsh),EcshnLeak(necsh),EcshoLeak(necsh),ecsho2de(necsh),ecshchi2P(necsh),ecshparP(4,necsh),ecshchi2T(necsh),ecshspev(3,necsh),ecshncl(necsh)[0,7]:I,ecshpcl(2,necsh):I");
//
  HBNAME(_lun,"EcalHits",&_ecalhit.Necht,
 
"necht[0,450],EchtStatus(necht):I,EchtIdsoft(necht):I,EchtProj(necht)[0,1]:I,EchtPlane(necht)[0,20]:I,EchtCell(necht)[0,80]:I,EchtEdep(necht),EchtECor(necht),echtattc(necht),EchtCoo(3,necht),EchtADC(3,necht),echtped(3,necht),echtgain(necht)");
//

  HBNAME(_lun,"TOFMCClu",&_tofmc.Ntofmc,
  "ntofmc[0,100],TOFMCIdsoft(ntofmc):I,TOFMCXcoo(3,ntofmc),TOFMCtof(ntofmc),TOFMCedep(ntofmc)");
  HBNAME(_lun,"TrCluste",&_trcl.Ntrcl, 
  "Ntrcl[0,200],Idsoft(ntrcl):I,Statust(ntrcl):I,NelemL(ntrcl)[-100,100]:I,NelemR(ntrcl)[0,100]:I,Sumt(ntrcl),Sigmat(ntrcl),Meant(ntrcl):R,RMSt(ntrcl),ErrorMeant(ntrcl),Amplitude(5,ntrcl):R");
  HBNAME(_lun,"TrMCClus",&_trclmc.Ntrclmc,
  "ntrclmc[0,200],IdsoftMC(ntrclmc),Itra(ntrclmc),Left(2,ntrclmc):I,Center(2,ntrclmc):I,Right(2,ntrclmc):I,ss(5,2,ntrclmc),xca(3,ntrclmc),xcb(3,ntrclmc),xgl(3,ntrclmc),summc(ntrclmc)");
  HBNAME(_lun,"TrRecHit",&_trrh02.Ntrrh,
  "ntrrh[0,666],px(ntrrh)[-1,30000]:I,py(ntrrh)[-1,30000]:I,statusr(ntrrh):I,Layer(ntrrh)[1,10]:I,hitr(3,ntrrh),ehitr(3,ntrrh),sumr(ntrrh),difosum(ntrrh),cofgx(ntrrh),cofgy(ntrrh),Bfield(3,ntrrh)");

  HBNAME(_lun,"Vertex",&_vtx02.Nvtx,
  "nvtx[0,3],vtxMomentum(nvtx):R,vtxErrMomentum(nvtx):R,vtxTheta(nvtx):R,vtxPhi(nvtx):R,vtxMass(nvtx):R,vtxCharge(nvtx)[-7,7]:I,vtxStatus(nvtx):I,vtxChi2(nvtx):R,vtxNdof(nvtx):I,vtxNtracks(nvtx)[0,7]:I,vtxPtrTrack(3,nvtx):I,vtxVertex(3,nvtx):R");

  HBNAME(_lun,"TrTrack",&_trtr02.Ntrtr,
  "ntrtr[0,100],trstatus(ntrtr):I,pattern(ntrtr)[-1,100]:I,address(ntrtr):I,nhits(ntrtr)[0,8],phits(8,ntrtr)[-1,30000]:I,LocDBAver(ntrtr):R,GeaneFitDone(ntrtr)[0,1000]:I,AdvFitDone(ntrtr)[0,1000]:I,Chi2StrLine(ntrtr),Chi2WithoutMS(ntrtr),RigidityWithoutMS(ntrtr),Chi2FastFit(ntrtr),Rigidity(ntrtr),ErrRig(ntrtr),Theta(ntrtr),phi(ntrtr),p0(3,ntrtr),gchi2(ntrtr),grig(ntrtr),gerrrig(ntrtr),hchi2(2,ntrtr),HRigidity(2,ntrtr),HErrRigidity(2,ntrtr),htheta(2,ntrtr),hphi(2,ntrtr),hp0(3,2,ntrtr),fchi2ms(ntrtr),pirigerr(ntrtr),rigms(ntrtr),pirig(ntrtr)");

  HBNAME(_lun,"MCEventG",&_mcg02.Nmcg,
  "nmcg[0,100],nskip(nmcg):I,Particle(nmcg)[-200,500]:I,coo(3,nmcg),dir(3,nmcg),momentum(nmcg):R,mass(nmcg):R,charge(nmcg):R");

  HBNAME(_lun,"MCTrack",&_mct.Nmct,
  "nmct[0,500],radl(nmct),absl(nmct),pos(3,nmct),vname(nmct):I");

  HBNAME(_lun,"AntiClus",&_anti.Nanti,
 "nanti[0,8],AntiStatus(nanti):I,AntiSector(nanti)[0,8]:I,AnNtimes(nanti)[0,16]:I,AnNpairs(nanti)[0,8]:I,AnTimes(16,nanti),AnTimesE(16,nanti),AntiEdep(nanti),AntiCoo(3,nanti),AntiErCoo(3,nanti)");

  HBNAME(_lun,"AntiMCCl",&_antimc.Nantimc,
  "nantimc[0,100],AMCIdsoft(nantimc):I,AMCXcoo(3,nantimc),AMCtof(nantimc),AMCedep(nantimc)");

  HBNAME(_lun,"LVL3",&_lvl302.Nlvl3,
"nlvl3[0,2],LVL3TOF(nlvl3)[-1,255],LVL3TRD(nlvl3)[0,15],LVL3Tr(nlvl3)[0,15],LVL3Main(nlvl3),LVL3Dir(nlvl3)[-1,1],LVL3NTrHits(nlvl3)[0,1000],LVL3NPat(nlvl3)[0,10],LVL3Pat(2,nlvl3)[-1,250],LVL3Res(2,nlvl3):R,LVL3Time(nlvl3):R,LVL3ELoss(nlvl3):R,LVL3TRDHits(nlvl3)[0,63],LVL3HMult(nlvl3)[0,31],LVL3TRDPar(2,nlvl3):R,LVL3emag(nlvl3)[-1,1],LVL3ECmat(nlvl3)[-1,1],LVL3ECTRKcr(4,nlvl3):R");

  HBNAME(_lun,"LVL1",&_lvl102.Nlvl1,

"nlvl1[0,2],LVL1PhysBPatt(nlvl1),LVL1JMembPatt(nlvl1),LVL1Flag1(nlvl1)[-10,20],LVL1Flag2(nlvl1)[-10,20],LVL1TOFPatt1(4,nlvl1),LVL1TOFPatt2(4,nlvl1),LVL1AntiPatt(nlvl1),LVL1ECALflag(nlvl1),LVL1ECALpatt(3,6,nlvl1),LVL1ECtrsum(nlvl1):R,LVL1LiveTime(nlvl1):R,LVL1TrigRates(6,nlvl1):R");

  HBNAME(_lun,"TrRawCl",&_trraw.Ntrraw,
  "ntrraw[0,300],rawaddress(ntrraw):I,rawlength(ntrraw)[-1,30000]:I,s2n(ntrraw):R");


  HBNAME(_lun,"TOFRawCl",&_tofraw.Ntofraw,"ntofraw[0,48],tofrstatus(ntofraw):I,tofrplane(ntofraw)[0,4]:I,tofrbar(ntofraw)[0,12]:I,tofradca(2,ntofraw),tofradcd(2,ntofraw),tofradcdr(3,2,ntofraw),tofrsdtm(2,ntofraw),tofreda(ntofraw),tofredd(ntofraw),tofrtm(ntofraw),tofrcoo(ntofraw)");
 
 
 
 
HBNAME(_lun,"TofRawSd",&_tofraws.Ntofraws,"ntofraws[0,70],tofrsswid(ntofraws):I,tofrshwid(ntofraws):I,tofrsnftdc(ntofraws)[0,8]:I,tofrsftdc(8,ntofraws),tofrsnstdc(ntofraws)[0,16]:I,tofrsstdc(16,ntofraws),tofrsnsumh(ntofraws)[0,16]:I,tofrssumh(16,ntofraws),tofrsnsumsh(ntofraws)[0,16]:I,tofrssumsh(16,ntofraws),tofrsadca(ntofraws),tofrsnadcd(ntofraws)[0,3]:I,tofrsadcd(3,ntofraws),tofrstemp(ntofraws)");
 
  HBNAME(_lun,"RICMCCl",&_richmc.NMC,
  	 "nsignals[0,250],sid(nsignals):I,"
  	 "rimcorg(3,nsignals),rimcdir(3,nsignals),rimcstatus(nsignals):I,"
         "rimcnphg:I,rimcphit(nsignals):I");

  HBNAME(_lun,"RICEvent",&_richevent.Nhits,
    	 "Rhits[0,100]:I,Rchtch(Rhits):I,Rchtadc(Rhits):I,"
  	 "Rnpe(Rhits):R,Rchtx(Rhits),Rchty(Rhits),Rhstatus(Rhits):I");

  HBNAME(_lun,"Ring",&_ring.NRings,
  	 "nrings[0,10]:I,rctrkn(nrings):I,"
  	 "rcrihu(nrings):I,rcrimhu(nrings):I,"
	 "rcribeta(nrings),rcriebeta(nrings),"
	 "rcrichi2(nrings),rcristatus(nrings):I,"
	 "rcriprob(nrings):R,rcrinpexp(nrings):R,rcricnpe(nrings):R,"
         "rcriliphu(nrings):I,rcrilipthc(nrings):R,rcrilipbeta(nrings):R,"
         "rcrilipebeta(nrings):R,"
         "rcriliplikep(nrings):R,rcrilipchi2(nrings):R,rcriliprprob(nrings):R");
  
}
void AMSNtuple::reset(int full){
#ifdef __WRITEROOT__
   _evroot02.clear();
   _evroot02.SetCont();
#endif
   _beta02.Nbeta= 0;
   _charge02.Ncharge = 0;
   _part02.Npart = 0;
   _tof.Ntof = 0;
   _ecclust.Neccl = 0;
   _ec2dclust.Nec2dcl = 0;
   _ecshow.Necsh = 0;
   _ecalhit.Necht = 0;
   _tofmc.Ntofmc = 0;
   _trcl.Ntrcl = 0;
   _trdclmc.Ntrdclmc=0;
   _trdcl.Ntrdcl=0;
   _trdht.Ntrdht=0;
   _trdtrk.Ntrdtrk=0;
   _trdseg.Ntrdseg=0;
    VZERO(&_trclmc.Ntrclmc,(sizeof(_trclmc))/sizeof(integer));
   _trrh02.Ntrrh = 0;
   _trtr02.Ntrtr = 0;
   _mcg02.Nmcg = 0;
   _mct.Nmct = 0;   
   _anti.Nanti = 0;
   _antimc.Nantimc = 0;
   _lvl302.Nlvl3 = 0;
   _lvl102.Nlvl1 = 0;
   _trraw.Ntrraw = 0;
   _tofraw.Ntofraw = 0;
   _tofraws.Ntofraws = 0;
   _richmc.NMC=0;
   _richevent.Nhits=0;
   _ring.NRings=0;
   _vtx02.Nvtx=0;
}

void AMSNtuple::write(integer addentry){
  if(_lun){
     HFNT(_lun);
     _Nentries++;
  }
}

void AMSNtuple::endR(){
#ifdef __WRITEROOT__
   if(_rfile){
     _rfile->Write();
     _rfile->Close();
     delete _rfile;
   }
   _rfile=0;
#endif
}



void AMSNtuple::initR(char* fname){
#ifdef __WRITEROOT__
  static TROOT _troot("S","S");
   cout << "Initializing tree...\n"<<endl;
  _Nentries=0;
   if(_rfile){
     _rfile->Write();
     _rfile->Close();
     delete _rfile;
   }
   _rfile= new TFile(fname,"RECREATE");
#ifdef __CORBA__
   _dc.SetString(AMSProducer::GetDataCards());
   //cout <<_dc.GetString()<<endl;
#endif
   _dc.Write("DataCards");
   if(!_rfile || _rfile->IsZombie())throw amsglobalerror("UnableToOpenRootFile",3);

   cout<<"Set Compress Level ..."<<IOPA.WriteRoot-1<<endl;
   cout<<"Set Split Level ..."<<branchSplit<<endl;

   _rfile->SetCompressionLevel(IOPA.WriteRoot-1);
    cout<<"AMSNtuple::initR -I- create branches"<<endl;
   _tree= new TTree("AMSRoot","AMS Ntuple Root");
     _evroot02.CreateBranch(_tree,branchSplit);
//    static void *pev1=(void*)&_evroot02;
//   TBranch *b1=_tree->Branch(AMSEventR::BranchName(),"AMSEventR",&pev1,64000,branchSplit);
//    AString  bhead=AMSEventR::_Name;
//    bhead+="Header";
//     static void *pev2=(void*)&_evroot02.fHeader;
//   TBranch *b2=_tree->Branch((const char*)bhead,"AMSEventHeaderR",&pev2,64000,1); 
#endif
#ifndef __WRITEROOT__
cerr <<" RootFileOutput is Not supported in this version "<<endl;
exit(1);
#endif

#ifdef __MEMMONITOR__
   int NEVENTS = GCFLAG.NEVENT;
   MemMonitor(MEMUPD, NEVENTS);
#endif
}
void AMSNtuple::writeR(){
#ifdef __WRITEROOT__
    _evroot02.SetCont();
    if(_tree){
    if(!_lun )_Nentries++;
     _tree->Fill();
    }
#endif
#ifdef __MEMMONITOR__
    int NEVENTS = GCFLAG.NEVENT;
    MemMonitor(MEMUPD,NEVENTS);
#endif
}


uinteger AMSNtuple::getrun(){
    return _event02.Run;
}

// Author S.Gerassimov TUM/COMPASS (sergei.gerassimov@cern.ch)
// adapted Jan 21, 2003. ak.
//
// Fill memory consumption histograms
// every n-th call up to N
// Example:
// if to call every event MemMonitor(100, 100000);
// at the end you will get histogram with memory consumption evolution
// 'till event # 100000, sampled every 100 events

void AMSNtuple::MemMonitor(const int n, int N = 0)
{
#if !defined(sun) && !defined(__MACOSX__)

  static int nevt    =0;
  static int nperiod =0;
  static int nmaximum=0;

  struct mallinfo m;

#ifdef __WRITEROOT__
  static TH1D* mm [5];
#endif

  if(nevt == 0) {      // booking
    nperiod = n;
    nmaximum= N;
    int nbins = N/n;
    cout<<"PaUtils::MemMonitor ==> Book histograms "<<endl;
    // Jan 21. 03 explanation for pedestrians (a.k.)
    // arena    - This is the total size of memory allocated with `sbrk'
    // hblkhd   - This is the total size of memory allocated with `mmap'
    // uordblks - This is the total size of memory occupied by chunks 
    // fordblks - This is the total size of memory occupied by free chanks
    // keepcost - This is the size of the top-most releasable chunk that
    //            normally borders the end of the heap (i.e. the high end of
    //            the virtual address space's data segment).

    // 
#ifdef __WRITEROOT__
    mm[0]=new TH1D( "MemMon_00","'malloc'      memory VS event number", nbins,  0, N);
    mm[1]=new TH1D( "MemMon_01","'mmap'        memory VS event number", nbins,  0, N);
    mm[2]=new TH1D( "MemMon_02","'used chunks' memory VS event number", nbins,  0, N);
    mm[3]=new TH1D( "MemMon_03","'free chunks' memory VS event number", nbins,  0, N);
    mm[4]=new TH1D( "MemMon_04","'keepcost'    memory VS event number", nbins,  0, N);
#else
    char cdir[256];
    HCDIR(cdir,"R");
    HMDIR("//PAWC/MEMMON","S");

    HBOOK1(9795,"'malloc'      memory VS event number",nbins,0,N,0);
    HBOOK1(9796,"'mmap'      memory VS event number",nbins,0,N,0);
    HBOOK1(9797,"'used chunks'      memory VS event number",nbins,0,N,0);
    HBOOK1(9798,"'free chunks'      memory VS event number",nbins,0,N,0);
    HBOOK1(9799,"'keepcost'      memory VS event number",nbins,0,N,0);

    char hp[9]="//PAWC";
    HCDIR(hp," ");
    HCDIR (cdir, " ");

#endif
}
  if(++nevt >= nmaximum) return; // out of histogramm limit

  if(nevt%nperiod == 0) {     // filling
    m=mallinfo();
#ifdef __WRITEROOT__
    mm[0]->Fill(nevt, double(m.arena   /1048576.));
    mm[1]->Fill(nevt, double(m.hblkhd  /1048576.));
    mm[2]->Fill(nevt, double(m.uordblks/1048576.));
    mm[3]->Fill(nevt, double(m.fordblks/1048576.));
    mm[4]->Fill(nevt, double(m.keepcost/1048576.));
#else
    char cdir[256];
    char *BLANK=" ";
    char memdir[256]="//PAWC/MEMMON";
    HCDIR(cdir,"R");
    HCDIR(memdir,BLANK);

    HF1(9795,float(nevt), float(m.arena   /1048576.));   
    HF1(9796,float(nevt), float(m.hblkhd  /1048576.));
    HF1(9797,float(nevt), float(m.uordblks/1048576.));
    HF1(9798,float(nevt), float(m.fordblks/1048576.));
    HF1(9797,float(nevt), float(m.keepcost/1048576.));

    char hp[9]="//PAWC";
    HCDIR(hp," ");
    HCDIR (cdir, " ");
#endif
  }
#endif
  return;
}







