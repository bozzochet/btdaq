//  $Id: ntuple.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMSNTUPLE__
#define __AMSNTUPLE__



const int NBRANCHES = 1;    // number of branches

namespace root{
const int MAXBETA02    = 20;
const int MAXCHARGE02  = 20;
const int MAXPART02    =  10;
const int MAXTOF     =    48;
const int MAXTOFRAW  =    48;
const int MAXTOFRAWS  =   70;
const int MAXTOFMC   =   100;
const int MAXANTICL  =    8;
const int MAXANTIMC  =   100;
const int MAXTRCL    =   200;
const int MAXTRRAW   =   300;
const int MAXTRCLMC  =   200;
const int MAXTRRH02    =   666;
const int MAXTRTR02    =   100;
const int MAXVTX02    =   3;
const int MAXMCG02     =    100;
const int MAXECSHOW =    5;
const int MAXECCLUST =    60;
const int MAXEC2DCLUST =    10;
const int MAXECHITS  =   450;
const int MAXLVL3    =     2;
const int MAXLVL1    =     2;
const int MAXRICMC   =   250;
const int MAXRICHITS =   10880;
const int MAXRICHRIN =   10;
const int MAXTRDCLMC   =   200;
const int MAXTRDRHT   =   200;
const int MAXTRDCL   =   100;
const int MAXTRDSEG   =   100;
const int MAXTRDTRK   =   40;
const int MAXMCVOL   =   500;
 const int NUMAMS     =   40;
};
using namespace root;

#ifndef __ROOTSHAREDLIBRARY__
#include "node.h"
#include "tkdbc.h"
#include "trddbc.h"
#include "ecaldbc.h"
#include "antidbc02.h"
#include "tofdbc02.h"
#ifdef __WRITEROOT__
#include "root.h"
#include <TObjString.h>
#endif

class BetaNtuple02 {
public:
  int Nbeta;
  int Status[MAXBETA02];
  int Pattern[MAXBETA02];
  float Beta[MAXBETA02];
  float BetaC[MAXBETA02];
  float Error[MAXBETA02];
  float ErrorC[MAXBETA02];
  float Chi2[MAXBETA02];
  float Chi2S[MAXBETA02];
  int NTOF[MAXBETA02];
  int pTOF[MAXBETA02][4];
  int pTr[MAXBETA02];
friend class AMSBeta;
friend class AMSNtuple;
};

class EventNtuple02 {
public:
  int Eventno;
  int Run;
  int RunType;
  int Time[2];
  int RawWords;
  float RadS;
  float ThetaS;
  float PhiS;
  float Yaw;
  float Pitch;
  float Roll;
  float VelocityS;
  float VelTheta;
  float VelPhi;
  float ThetaM;
  float PhiM;
  float ISSEqAsc; // ISN 
  float ISSEqDec; // ISN 
  float ISSGalLong; // ISN 
  float ISSGalLat; // ISN 
  float AMSEqAsc; // ISN 
  float AMSEqDec; // ISN 
  float AMSGalLong; // ISN   
  float AMSGalLat; // ISN 
  int Tracks;
  int TrRecHits;
  int TrClusters;
  int TrRawClusters;
  int TrMCClusters;
  int TOFClusters;
  int TOFMCClusters;
  int AntiMCClusters;
  int TRDMCClusters;
  int AntiClusters;
   int EcalClusters;
  int EcalHits;
  int RICMCClusters;//CJM
  int RICHits;//CJM
  int TRDRawHits;
  int TRDClusters;
  int TRDSegments;
  int TRDTracks;
  int EventStatus[2];

friend class AMSEvent;
friend class AMSNtuple;
};

class ChargeNtuple02 {
public:
  int Ncharge;
  int Status[MAXCHARGE02];
  int BetaP[MAXCHARGE02];
  int RingP[MAXCHARGE02];
  int ChargeTOF[MAXCHARGE02];
  int ChargeTracker[MAXCHARGE02];
  int ChargeRich[MAXCHARGE02];
  float ProbTOF[MAXCHARGE02][4];
  int ChInTOF[MAXCHARGE02][4];
  float ProbTracker[MAXCHARGE02][4];
  int ChInTracker[MAXCHARGE02][4];
  float ProbRich[MAXCHARGE02][4];
  int ChInRich[MAXCHARGE02][4];
  float ProbAllTracker[MAXCHARGE02];
  float TrunTOF[MAXCHARGE02];
  float TrunTOFD[MAXCHARGE02];
  float TrunTracker[MAXCHARGE02];

friend class AMSCharge;
friend class AMSNtuple;
};

class ParticleNtuple02 {
public:
  int Npart;
  int   Status[MAXPART02];
  int   BetaP[MAXPART02];
  int   ChargeP[MAXPART02];
  int   TrackP[MAXPART02];
  int   TRDP[MAXPART02];
  int   VertexP[MAXPART02];
  int   RICHP[MAXPART02];
  int   EcalP[MAXPART02];
  int   Particle[MAXPART02];
  int   ParticleVice[MAXPART02];
  float Prob[MAXPART02][2];
  float FitMom[MAXPART02];
  float Mass[MAXPART02];
  float ErrMass[MAXPART02];
  float Momentum[MAXPART02];
  float ErrMomentum[MAXPART02];
  float Beta[MAXPART02];
  float ErrBeta[MAXPART02];
  float Charge[MAXPART02];
  float Theta[MAXPART02];
  float Phi[MAXPART02];
  float ThetaGl[MAXPART02];
  float PhiGl[MAXPART02];
  float Coo[MAXPART02][3];
  float Cutoff[MAXPART02];
  float TOFCoo[MAXPART02][4][3];
  float AntiCoo[MAXPART02][2][3];
  float EcalCoo[MAXPART02][3][3];
  float TrCoo[MAXPART02][trconst::maxlay][3];
  float TRDCoo[MAXPART02][3];
  float RichCoo[MAXPART02][2][3];
  float RichPath[MAXPART02][2];
  float RichPathBeta[MAXPART02][2];
  float RichLength[MAXPART02];
  float TRDLikelihood[MAXPART02];
  float Local[MAXPART02][trconst::maxlay];
friend class AMSParticle;
friend class AMSNtuple;
};

//-------------------------
class TOFClusterNtuple {
public:
  int Ntof;
  int Status[MAXTOF];
  int Layer[MAXTOF];
  int Bar[MAXTOF];
  int Nmemb[MAXTOF];
  int P2memb[MAXTOF][3];
  float Edep[MAXTOF];
  float Edepd[MAXTOF];
  float Time[MAXTOF];
  float ErrTime[MAXTOF];
  float Coo[MAXTOF][3];
  float ErrorCoo[MAXTOF][3];

friend class AMSTOFCluster;
friend class AMSNtuple;
};

class EcalShowerNtuple {
public:
  int Necsh;
  int   Status[MAXECSHOW];
  float Dir[MAXECSHOW][3];
  float EMDir[MAXECSHOW][3];
  float Entry[MAXECSHOW][3];
  float Exit[MAXECSHOW][3];
  float CofG[MAXECSHOW][3];
  float ErTheta[MAXECSHOW];
  float Chi2Dir[MAXECSHOW];
  float FirstLayerEdep[MAXECSHOW];
  float EnergyC[MAXECSHOW];
  float Energy3C[MAXECSHOW][3];
  float ErEnergyC[MAXECSHOW];
  float DifoSum[MAXECSHOW];
  float SideLeak[MAXECSHOW];
  float RearLeak[MAXECSHOW];
  float DeadLeak[MAXECSHOW];
  float AttLeak[MAXECSHOW];
  float NLinLeak[MAXECSHOW];
  float OrpLeak[MAXECSHOW];
  float Orp2DEnergy[MAXECSHOW];
  float Chi2Profile[MAXECSHOW];
  float ParProfile[MAXECSHOW][4];
  float Chi2Trans[MAXECSHOW];
//  float TransProfile[MAXECSHOW][3];
  float SphericityEV[MAXECSHOW][3];
  int   N2DCl[MAXECSHOW];
  int   p2DCl[MAXECSHOW][2];
friend class AMSEcalShower;
friend class AMSNtuple;
};

class EcalClusterNtuple {
public:
  int Neccl;
  int Status[MAXECCLUST];
  int Proj[MAXECCLUST];
  int Plane[MAXECCLUST];
  int Left[MAXECCLUST];
  int Center[MAXECCLUST];
  int Right[MAXECCLUST];
  float Edep[MAXECCLUST];
//  float RMS[MAXECCLUST];
  float SideLeak[MAXECCLUST];
  float DeadLeak[MAXECCLUST];
  float Coo[MAXECCLUST][3];
  int pLeft[MAXECCLUST];
  int NHits[MAXECCLUST];
friend class Ecal1DCluster;
friend class AMSNtuple;
};

class Ecal2DClusterNtuple {
public:
  int Nec2dcl;
  int Status[MAXEC2DCLUST];
  int Proj[MAXEC2DCLUST];
  int Nmemb[MAXEC2DCLUST];
  float Edep[MAXEC2DCLUST];
//  float SideLeak[MAXEC2DCLUST];
//  float DeadLeak[MAXEC2DCLUST];
  float Coo[MAXEC2DCLUST];
  float Tan[MAXEC2DCLUST];
  float Chi2[MAXEC2DCLUST];
  int   pCl[MAXEC2DCLUST][18];
friend class AMSEcal2DCluster;
friend class AMSNtuple;
};

//--------------------------
class EcalHitNtuple {
public:
  int Necht;
  int Status[MAXECHITS];
  int Idsoft[MAXECHITS];
  int Proj[MAXECHITS];
  int Plane[MAXECHITS];
  int Cell[MAXECHITS];
  float Edep[MAXECHITS];
  float EdCorr[MAXECHITS];
  float AttCor[MAXECHITS];
  float Coo[MAXECHITS][3];
  float ADC[MAXECHITS][3];
  float Ped[MAXECHITS][3];
  float Gain[MAXECHITS];
friend class AMSEcalHit;
friend class AMSNtuple;
};

//-------------------------- 
class TOFMCClusterNtuple {
public:
  int Ntofmc;
  int Idsoft[MAXTOFMC];
  float Coo[MAXTOFMC][3];
  float TOF[MAXTOFMC];
  float Edep[MAXTOFMC];

friend class AMSTOFMCCluster;
friend class AMSNtuple;
};

//--------------------------
class TrClusterNtuple {
public:
  int Ntrcl;
  int Idsoft[MAXTRCL];
  int Status[MAXTRCL];
  int NelemL[MAXTRCL];
  int NelemR[MAXTRCL];
  float Sum[MAXTRCL];
  float Sigma[MAXTRCL];
  float Mean[MAXTRCL];
  float RMS[MAXTRCL];
  float ErrorMean[MAXTRCL];
  float Amplitude[MAXTRCL][5];

friend class AMSTrCluster;
friend class AMSNtuple;
};

class TrMCClusterNtuple {
public:
  int Ntrclmc;
  int Idsoft[MAXTRCLMC];
  int TrackNo[MAXTRCLMC];
  int Left[MAXTRCLMC][2];
  int Center[MAXTRCLMC][2];
  int Right[MAXTRCLMC][2];
  float SS[MAXTRCLMC][2][5];
  float Xca[MAXTRCLMC][3];
  float Xcb[MAXTRCLMC][3];
  float Xgl[MAXTRCLMC][3];
  float Sum[MAXTRCLMC];

friend class AMSTrMCCluster;
friend class AMSNtuple;
};

class TRDMCClusterNtuple {
public:
  int Ntrdclmc;
  int Layer[MAXTRDCLMC];
  int Ladder[MAXTRDCLMC];
  int Tube[MAXTRDCLMC];
  int ParticleNo[MAXTRDCLMC];
//  int TrackNo[MAXTRDCLMC];
  float Edep[MAXTRDCLMC];
  float Ekin[MAXTRDCLMC];
  float Xgl[MAXTRDCLMC][3];
  float Step[MAXTRDCLMC];
 
friend class AMSTRDMCCluster;
friend class AMSNtuple;
};

class TRDRawHitNtuple {
public:
  int Ntrdht;
  int Layer[MAXTRDRHT];
  int Ladder[MAXTRDRHT];
  int Tube[MAXTRDRHT];
  float Amp[MAXTRDRHT];
friend class AMSTRDRawHit;
friend class AMSNtuple;
};


class TRDClusterNtuple {
public:
  int Ntrdcl;
  int   Status[MAXTRDCL];
  float Coo[MAXTRDCL][3];
  int    Layer[MAXTRDCL];
  float CooDir[MAXTRDCL][3];
  int Multip[MAXTRDCL];
  int HMultip[MAXTRDCL];
  float EDep[MAXTRDCL];
  int   pRawHit[MAXTRDCL];
 
friend class AMSTRDCluster;
friend class AMSNtuple;
};


class TRDSegmentNtuple {
public:
  int Ntrdseg;
  int   Status[MAXTRDSEG];
  int   Orientation[MAXTRDSEG];
  float FitPar[MAXTRDSEG][2];
  float Chi2[MAXTRDSEG];
  int Pattern[MAXTRDSEG];
  int Nhits[MAXTRDSEG];
  int PCl[MAXTRDSEG][trdconst::maxhits];

friend class AMSTRDSegment;
friend class AMSNtuple;
};

class TRDTrackNtuple {
public:
  int Ntrdtrk;
  int   Status[MAXTRDTRK];
  float Coo[MAXTRDTRK][3];
  float ErCoo[MAXTRDTRK][3];
  float Phi[MAXTRDTRK];
  float Theta[MAXTRDTRK];
  float Chi2[MAXTRDTRK];
  int   NSeg[MAXTRDTRK];
  int   Pattern[MAXTRDTRK];
  int   pSeg[MAXTRDTRK][trdconst::maxseg];
 
friend class AMSTRDTrack;
friend class AMSNtuple;
};

class TrRecHitNtuple02 {
public:
  int Ntrrh;
  int pX[MAXTRRH02];
  int pY[MAXTRRH02];
  int Status[MAXTRRH02];
  int Layer[MAXTRRH02];
  float Hit[MAXTRRH02][3];
  float EHit[MAXTRRH02][3];
  float Sum[MAXTRRH02];
  float DifoSum[MAXTRRH02];
  float CofgX[MAXTRRH02];
  float CofgY[MAXTRRH02];
  float Bfield[MAXTRRH02][3];

friend class AMSTrRecHit;
friend class AMSNtuple;
};

class VtxNtuple{
public:
  int Nvtx;
  float Momvtx[MAXVTX02];
  float ErrMomvtx[MAXVTX02];
  float Thetavtx[MAXVTX02];
  float Phivtx[MAXVTX02];
  float Massvtx[MAXVTX02];
  int Chargevtx[MAXVTX02];
  int Statusvtx[MAXVTX02];
  float Chi2vtx[MAXVTX02];
  int Ndofvtx[MAXVTX02];
  int Ntrkvtx[MAXVTX02];
  int PtrTrack[MAXVTX02][3];
  float Vertex[MAXVTX02][3];

friend class AMSVtx;
friend class AMSTrTrack;
friend class AMSNtuple;
};

class TrTrackNtuple02 {
public:
  int Ntrtr;
  int Status[MAXTRTR02];
  int Pattern[MAXTRTR02];
  int Address[MAXTRTR02];
  int NHits[MAXTRTR02];
  int pHits[MAXTRTR02][trconst::maxlay];
  float LocDBAver[MAXTRTR02];
  int GeaneFitDone[MAXTRTR02];
  int AdvancedFitDone[MAXTRTR02];
  float Chi2StrLine[MAXTRTR02];
  float Chi2WithoutMS[MAXTRTR02];
  float RigidityWithoutMS[MAXTRTR02];
  float Chi2FastFit[MAXTRTR02];
  float Ridgidity[MAXTRTR02];
  float ErrRidgidity[MAXTRTR02];
  float Theta[MAXTRTR02];
  float Phi[MAXTRTR02];
  float P0[MAXTRTR02][3];
  float GChi2[MAXTRTR02];
  float GRidgidity[MAXTRTR02];
  float GErrRidgidity[MAXTRTR02];
//  float GTheta[MAXTRTR02];
//  float GPhi[MAXTRTR02];
//  float GP0[MAXTRTR02][3];
  float HChi2[MAXTRTR02][2];
  float HRidgidity[MAXTRTR02][2];
  float HErrRidgidity[MAXTRTR02][2];
  float HTheta[MAXTRTR02][2];
  float HPhi[MAXTRTR02][2];
  float HP0[MAXTRTR02][2][3];
  float FChi2MS[MAXTRTR02];
  float PiErrRig[MAXTRTR02];
  float RidgidityMS[MAXTRTR02];
  float PiRigidity[MAXTRTR02];

friend class AMSTrTrack;
friend class AMSNtuple;
};

class MCTrackNtuple {
public:
int Nmct;
float radl[MAXMCVOL];
float absl[MAXMCVOL];
float pos[MAXMCVOL][3];
char  vname[MAXMCVOL][4];
};


class MCEventGNtuple02 {
public:
  int Nmcg;
  int Nskip[MAXMCG02];
  int Particle[MAXMCG02];
  float Coo[MAXMCG02][3];
  float Dir[MAXMCG02][3];
  float Momentum[MAXMCG02];
  float Mass[MAXMCG02];
  float Charge[MAXMCG02];

friend class AMSmceventg;
friend class AMSNtuple;
};


class AntiClusterNtuple {
public:
  int Nanti;
  int Status[MAXANTICL];
  int Sector[MAXANTICL];
  int   Ntimes[MAXANTICL];
  int   Npairs[MAXANTICL];
  float Times[MAXANTICL][32];
  float Timese[MAXANTICL][32];
  float Edep[MAXANTICL];
  float Coo[MAXANTICL][3];   // R, phi, Z
  float ErrorCoo[MAXANTICL][3];

friend class AMSAntiCluster;
friend class AMSNtuple;
};

class ANTIMCClusterNtuple {
public:
  int Nantimc;
  int Idsoft[MAXANTIMC];
  float Coo[MAXANTIMC][3];
  float TOF[MAXANTIMC];
  float Edep[MAXANTIMC];

friend class AMSAntiMCCluster;
friend class AMSNtuple;
};

class LVL3Ntuple02 {
public:
  int Nlvl3;
  int TOFTr[MAXLVL3];
  int TRDTr[MAXLVL3];
  int TrackerTr[MAXLVL3];
  int MainTr[MAXLVL3];
  int Direction[MAXLVL3];
  int NTrHits[MAXLVL3];
  int NPatFound[MAXLVL3];
  int Pattern[MAXLVL3][2];
  float Residual[MAXLVL3][2];
  float Time[MAXLVL3];
  float ELoss[MAXLVL3];
  int   TRDHits[MAXLVL3];
  int   HMult[MAXLVL3];
  float TRDPar[MAXLVL3][2];
  int ECemag[MAXLVL3];
  int ECmatc[MAXLVL3];
  float ECTOFcr[MAXLVL3][4];
friend class TriggerLVL302;
friend class AMSNtuple;
};



class LVL1Ntuple02 {
public:
  int Nlvl1;
  int PhysBPatt[MAXLVL1];
  int JMembPatt[MAXLVL1];
  int TOFlag1[MAXLVL1];
  int TOFlag2[MAXLVL1];
  int TOFPatt1[MAXLVL1][4];
  int TOFPatt2[MAXLVL1][4];
  int AntiPatt[MAXLVL1];
  int ECALflag[MAXLVL1];
  int ECALpatt[MAXLVL1][6][3];
  float ECALtrsum[MAXLVL1];
  float LiveTime[MAXLVL1];
  float TrigRates[MAXLVL1][6];

friend class Trigger2LVL1;
friend class AMSNtuple;
};

class TrRawClusterNtuple {
public:
  int Ntrraw;
  int address[MAXTRRAW];
  int nelem[MAXTRRAW];
  float s2n[MAXTRRAW];
friend class AMSTrRawCluster;
friend class AMSNtuple;
};


class TOFRawClusterNtuple {
public:
  int Ntofraw;
  int Status[MAXTOFRAW];
  int Layer[MAXTOFRAW];
  int Bar[MAXTOFRAW];
  float adca[MAXTOFRAW][2];
  float adcd[MAXTOFRAW][2];
  float adcdr[MAXTOFRAW][2][3];
  float sdtm[MAXTOFRAW][2];
  float edepa[MAXTOFRAW];
  float edepd[MAXTOFRAW];
  float time[MAXTOFRAW];
  float cool[MAXTOFRAW];

friend class TOF2RawCluster;
friend class AMSNtuple;
};


class TofRawSideNtuple {
public:
  int Ntofraws;
  int swid[MAXTOFRAWS];
  int hwid[MAXTOFRAWS];
  int stat[MAXTOFRAWS];
  int nftdc[MAXTOFRAWS];
  int ftdc[MAXTOFRAWS][8];
  int nstdc[MAXTOFRAWS];
  int stdc[MAXTOFRAWS][16];
  int nsumh[MAXTOFRAWS];
  int sumht[MAXTOFRAWS][16]; 
  int nsumsh[MAXTOFRAWS];
  int sumsht[MAXTOFRAWS][16];
  float adca[MAXTOFRAWS];
  int nadcd[MAXTOFRAWS];
  float adcd[MAXTOFRAWS][3];
  float temp[MAXTOFRAWS];

friend class TOF2RawSide;
friend class AMSNtuple;
};

class RICMCNtuple {
public:
  int NMC;                     // Number of detected photons
  int id[MAXRICMC];            // Particle id.
  float origin[MAXRICMC][3];   // Particle origin
  float direction[MAXRICMC][3];// Original direction
  int status[MAXRICMC];        // Status=10*number of reflections+(have it rayleigh?1:0)
  int numgen;                  // Number of generated photons
  int eventpointer[MAXRICMC];  // Pointer to detected hit
friend class AMSRichMCHit;
friend class AMSNtuple;
};

class RICEventNtuple {
public:
  int Nhits;
  int channel[MAXRICHITS];
  int adc[MAXRICHITS];
  float npe[MAXRICHITS];
  float x[MAXRICHITS];
  float y[MAXRICHITS];
  integer status[MAXRICHITS];

friend class AMSRichRawEvent;
friend class AMSNtuple;
};

class RICRing {
public:
  int NRings;
  int track[MAXRICHRIN];
  int used[MAXRICHRIN];
  int mused[MAXRICHRIN];
  float beta[MAXRICHRIN];
  float errorbeta[MAXRICHRIN];
  float quality[MAXRICHRIN];
  unsigned int status[MAXRICHRIN];
  float probkl[MAXRICHRIN];
  float npexp[MAXRICHRIN];
  float collected_npe[MAXRICHRIN];
  //+LIP 
  // variables
  int   liphused[MAXRICHRIN];     //nr of hits used=10000 + nr hits mirror
  float lipthc[MAXRICHRIN];       // rec. Cerenkov angle
  float lipbeta[MAXRICHRIN];      // rec. beta 
  float lipebeta[MAXRICHRIN];    // error on rec. beta
  float liplikep[MAXRICHRIN];     // likelihood prob. 
  float lipchi2[MAXRICHRIN];      // chi2 of the fit 
  float liprprob[MAXRICHRIN];     // ring prob.
  //ENDofLIP 

/*
  float npexpg[MAXRICHRIN];
  float npexpr[MAXRICHRIN];
  float npexpb[MAXRICHRIN];
  int rad,bas;  /// CORTAR AQUI
*/



friend class AMSRichRing;
friend class AMSNtuple;
}; 

class AMSNtuple : public AMSNode{
protected:
  integer _lun;
  integer _Nentries;
  EventNtuple02 _event02;
  BetaNtuple02 _beta02;
  ChargeNtuple02 _charge02;
  ParticleNtuple02 _part02;
  TOFClusterNtuple _tof;
  TOFMCClusterNtuple _tofmc;
  TrClusterNtuple _trcl;
  TrMCClusterNtuple _trclmc;
  TRDMCClusterNtuple _trdclmc;
  TRDClusterNtuple _trdcl;
  TRDSegmentNtuple _trdseg;
  TRDTrackNtuple _trdtrk;
  TRDRawHitNtuple _trdht;
  TrRecHitNtuple02 _trrh02;
  VtxNtuple _vtx02;
  TrTrackNtuple02 _trtr02;
  MCTrackNtuple _mct;// tempor 02??
  MCEventGNtuple02 _mcg02;
  AntiClusterNtuple _anti;
  ANTIMCClusterNtuple _antimc;
  LVL3Ntuple02 _lvl302;
  LVL1Ntuple02 _lvl102;
  TrRawClusterNtuple _trraw;
  TOFRawClusterNtuple _tofraw;
  TofRawSideNtuple _tofraws;
  EcalClusterNtuple _ecclust;
  Ecal2DClusterNtuple _ec2dclust;
  EcalShowerNtuple _ecshow;
  EcalHitNtuple _ecalhit;
  RICMCNtuple _richmc;
  RICEventNtuple _richevent;
  RICRing _ring;
#ifdef __WRITEROOT__
  AMSEventR   _evroot02;
  static TTree* _tree;  
  static TFile* _rfile;
  static TObjString _dc; 
#endif
  virtual void _init(){};

public:
  AMSNtuple():AMSNode(AMSID()),_lun(0){}
  AMSNtuple(char* name);
  ~AMSNtuple();
  AMSNtuple(integer lun, char* name);
  void init();
  static TString & DataCards()  {return _dc.String();}
  uinteger getrun();
  void reset(int i=0);
  void write(integer addentry=0);
  void writeR();
  void endR();
  void initR(char* name);
  integer getentries(){return _Nentries;}
  EventNtuple02* Get_event02() {return &_event02;}
  BetaNtuple02* Get_beta02() { return &_beta02;}
  ChargeNtuple02* Get_charge02() { return &_charge02;}
  ParticleNtuple02* Get_part02() {return &_part02;}
  TOFClusterNtuple* Get_tof() {return &_tof;}
  TOFMCClusterNtuple* Get_tofmc() {return &_tofmc;}
  TrClusterNtuple* Get_trcl() {return &_trcl;}
  TrMCClusterNtuple* Get_trclmc() {return &_trclmc;}
  TRDMCClusterNtuple* Get_trdclmc() {return &_trdclmc;}
  TRDClusterNtuple* Get_trdcl() {return &_trdcl;}
  TRDSegmentNtuple* Get_trdseg() {return &_trdseg;}
  TRDTrackNtuple* Get_trdtrk() {return &_trdtrk;}
  TRDRawHitNtuple* Get_trdht() {return &_trdht;}
  TrRecHitNtuple02* Get_trrh02() {return &_trrh02;}
  VtxNtuple* Get_vtx02() {return &_vtx02;}
  TrTrackNtuple02* Get_trtr02() {return &_trtr02;}
  MCTrackNtuple* Get_mct() {return &_mct;}
  MCEventGNtuple02* Get_mcg02() {return &_mcg02;}
  AntiClusterNtuple* Get_anti() {return &_anti;}
  ANTIMCClusterNtuple* Get_antimc() {return &_antimc;}
  LVL3Ntuple02* Get_lvl302() {return &_lvl302;}
  LVL1Ntuple02* Get_lvl102() {return &_lvl102;}
  TrRawClusterNtuple* Get_trraw() {return &_trraw;}
  TOFRawClusterNtuple* Get_tofraw() {return &_tofraw;}
  TofRawSideNtuple* Get_tofraws() {return &_tofraws;}
  EcalClusterNtuple* Get_ecclust() {return &_ecclust;}
  Ecal2DClusterNtuple* Get_ec2dclust() {return &_ec2dclust;}
  EcalShowerNtuple* Get_ecshow() {return &_ecshow;}
  EcalHitNtuple* Get_ecalhit() {return &_ecalhit;}
  RICMCNtuple* Get_richmc() {return &_richmc;}
  RICEventNtuple* Get_richevent() {return &_richevent;}
  RICRing* Get_ring(){return &_ring;}

  void MemMonitor(const int n, int N);

#ifdef __WRITEROOT__
  AMSEventR* Get_evroot02() {return &_evroot02;}
#endif

};

#endif
#endif
