//  $Id: commons.h,v 1.1 2008/11/06 22:22:10 zuccon Exp $
//  Author V. Choutko 24-may-1996
//
//  To developpers:
//  All new vars to cblocks defs: only at the very end !!!
///

#ifndef __AMSCOMMONS__
#define __AMSCOMMONS__
#include "typedefs.h"
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"



class TRMFFKEY_DEF{
public:
integer OKAY;
integer TIME;
};
#define TRMFFKEY COMMON_BLOCK(TRMFFKEY,trmffkey)
COMMON_BLOCK_DEF(TRMFFKEY_DEF,TRMFFKEY);

class MISCFFKEY_DEF{
public:
integer BeamTest;
integer G3On;
integer G4On;
integer dbwrbeg;
integer dbwrend;
};
COMMON_BLOCK_DEF(MISCFFKEY_DEF,MISCFFKEY);


class SELECTFFKEY_DEF {
public:
integer Run;
integer Event;
integer File[40];
integer RunE;
integer EventE;
};
#define SELECTFFKEY COMMON_BLOCK(SELECTFFKEY,selectffkey)
COMMON_BLOCK_DEF(SELECTFFKEY_DEF,SELECTFFKEY);

class STATUSFFKEY_DEF {
public:
uinteger status[34];
};
#define STATUSFFKEY COMMON_BLOCK(STATUSFFKEY,statusffkey)
COMMON_BLOCK_DEF(STATUSFFKEY_DEF,STATUSFFKEY);


class DAQCFFKEY_DEF {
public:
integer mode;
integer ifile[40];
integer ofile[40];
integer BTypeInDAQ[2];
integer Mode;
};
#define DAQCFFKEY COMMON_BLOCK(DAQCFFKEY,daqcffkey)
COMMON_BLOCK_DEF(DAQCFFKEY_DEF,DAQCFFKEY);



class LVL3SIMFFKEY_DEF{
public:
geant NoiseProb[2];
};
#define LVL3SIMFFKEY COMMON_BLOCK(LVL3SIMFFKEY,lvl3simffkey)
COMMON_BLOCK_DEF(LVL3SIMFFKEY_DEF,LVL3SIMFFKEY);

class LVL3EXPFFKEY_DEF{
public:
integer NEvents;
integer ToBad;
integer TryAgain;
geant   Range[3][2];
};
#define LVL3EXPFFKEY COMMON_BLOCK(LVL3EXPFFKEY,lvl3expffkey)
COMMON_BLOCK_DEF(LVL3EXPFFKEY_DEF,LVL3EXPFFKEY);



class LVL3FFKEY_DEF {
public:
integer MinTOFPlanesFired;
integer UseTightTOF;
geant TrTOFSearchReg; 
geant TrMinResidual;
geant TrMaxResidual[3];
integer TrMaxHits;
geant Splitting;
integer NRep;
integer Accept;
integer RebuildLVL3;
integer NoK;
geant TrHeavyIonThr;
integer SeedThr;
integer TRDHMulThr;
geant  TRDHMulPart;
integer Stat;
};
#define LVL3FFKEY COMMON_BLOCK(LVL3FFKEY,lvl3ffkey)
COMMON_BLOCK_DEF(LVL3FFKEY_DEF,LVL3FFKEY);



class AMSFFKEY_DEF {
public:
integer Simulation;
integer Reconstruction;
integer Jobtype;
integer Debug;
geant   CpuLimit;
integer Read;
integer Write;
integer Jobname[40];
integer Setupname[40];
integer ZeroSetupOk;
integer Update;
integer TDVC[400];
};
#define AMSFFKEY COMMON_BLOCK(AMSFFKEY,amsffkey)
COMMON_BLOCK_DEF(AMSFFKEY_DEF,AMSFFKEY);
//==============================================================
class IOPA_DEF {
public:
integer hlun;
integer hfile[40];
integer ntuple;
integer WriteAll;
float   Portion;
integer TriggerC[40];
integer mode;
integer ffile[40];
integer MaxNtupleEntries;
integer WriteRoot;
integer rfile[40];
uinteger MaxFileSize;
integer MaxFileTime;
integer BuildMin;
  void init();
};
#define IOPA COMMON_BLOCK(IOPA,iopa)
COMMON_BLOCK_DEF(IOPA_DEF,IOPA);
//==============================================================
class TFMCFFKEY_DEF {
public:
geant TimeSigma;
geant sumHTdel;
geant TimeProbability2;
geant dzconv;
geant Thr;
//
integer mcprtf[5];     // TOF MC-print flag (=0 -> no_printing)
integer trlogic[2]; // to control trigger logic
integer fast;    // 1/0 fast/slow algo;
integer daqfmt;  // 0/1 raw/reduced TDC format for DAQ
integer birks;     // 0/1  not apply/apply birks corrections
integer mcseedo;   // 0/1->use MCCalib/RealData files as MCseed
geant blshift;     // base line shift
geant hfnoise;     // high freq. noise
integer ReadConstFiles;
integer addpeds;
integer calvern;//  TofCflistMC-file version number (file extention)
integer tdclin;//flag to activate(if =1) TDC-linearity logic for MC
integer tdcovf;//flag to activate(if =1) TDC-ovfl protection logic
};
#define TFMCFFKEY COMMON_BLOCK(TFMCFFKEY,tfmcffkey)
COMMON_BLOCK_DEF(TFMCFFKEY_DEF,TFMCFFKEY);
//==============================================================


// class MAGSFFKEY_DEF {
// public:
//   integer magstat; ///  -1/0/1->warm/cold_OFF/cold_ON 
//   geant fscale;    /// rescale factor (wrt nominal field) (if any)  (not implemented yet)
//   geant ecutge;    /// e/g ener.cut for tracking in magnet materials(Gev) 
//   integer BTempCorrection; // Temperature correction flag (parameters are hardcoded)
//   float   BZCorr; /// B field Z correction factor
//   void init();
// };
// #define MAGSFFKEY COMMON_BLOCK(MAGSFFKEY,magsffkey)
// COMMON_BLOCK_DEF(MAGSFFKEY_DEF,MAGSFFKEY);
//==============================================================


class TRDMCFFKEY_DEF{
public:
integer mode; // 0: saveliev 1: saveliev+Pai 2: garibyan + Pai
integer g3trd;  // g3identifier for trd aware media
float cor;     // saveliev parameter
float alpha;    // garibyan pars
float beta;    //  garibyan par
geant ped;     
geant pedsig;     
geant sigma;
geant gain;
geant cmn;
geant   f2i;
integer adcoverflow;
integer NoiseOn;
float GeV2ADC;
float Thr1R;
integer GenerateConst;
geant NoiseLevel;
geant DeadLevel;
integer sec[2];
integer min[2];
integer hour[2];
integer day[2];
integer mon[2];
integer year[2];
integer multiples;
};
#define TRDMCFFKEY COMMON_BLOCK(TRDMCFFKEY,trdmcffkey)
COMMON_BLOCK_DEF(TRDMCFFKEY_DEF,TRDMCFFKEY);


class TRDFITFFKEY_DEF{
public:
  float Chi2StrLine;     //  fit max chi2
  float ResCutStrLine;   // Max acc distance between point and fitted curve 
  float SearchRegStrLine;  
  int MinFitPoints;
  float TwoSegMatch;
  float MaxSegAllowed; 
};
#define TRDFITFFKEY COMMON_BLOCK(TRDFITFFKEY,trdfitffkey)
COMMON_BLOCK_DEF(TRDFITFFKEY_DEF,TRDFITFFKEY);

class TRDRECFFKEY_DEF{
public:
  float Chi2StrLine;     //  fit max chi2
  float ResCutStrLine;   // Max acc distance between point and fitted curve 
  float SearchRegStrLine;  
};
#define TRDRECFFKEY COMMON_BLOCK(TRDRECFFKEY,trdrecffkey)
COMMON_BLOCK_DEF(TRDRECFFKEY_DEF,TRDRECFFKEY);
class TRDCLFFKEY_DEF{
public:
  float ADC2KeV;         //
  float Thr1A;           // cluster threshold amp kev
  float Thr1S;           // cluster threshold sigma kev
  float Thr1R;           // cluster threshold ratio kev
  float Thr1H;           // cluster threshold high amp kev
  int   MaxHitsInCluster; 
};
#define TRDCLFFKEY COMMON_BLOCK(TRDCLFFKEY,trdclffkey)
COMMON_BLOCK_DEF(TRDCLFFKEY_DEF,TRDCLFFKEY);


//==================================================================
class ECMCFFKEY_DEF {
public:
integer fastsim;  //1/0-> fast/slow simulation
integer mcprtf;   //1/0-> print/not mc-histogr.
geant cutge;// cutgam=cutele cut in EC_radiator
integer silogic[2];// SIMU logic flags
geant mev2mev;
geant mev2adc;
geant safext;     // Extention(cm) of EC transv.size when TFMC 13=2 is used
geant mev2pes;
geant pmseres;
geant adc2q;//adc->q conv.factor(pC/adc)
geant fendrf;//fiber end-cut reflection factor
geant physa2d;//physical an/dyn ratio(mv/mv)
geant hi2low;//not used now
geant sbcgn;//not used now
geant pedh; //Ped-HiCh    
geant pedvh;//ch-to-ch variation(%)     
geant pedl; //Ped-LoCh    
geant pedvl;//ch-to-ch variations(%)
geant pedsh;//PedSig-HiCh     
geant pedsvh;//ch-to-ch variation(%)     
geant pedsl;//PedSig-LoCh    
geant pedsvl;//ch-to-ch variation(%)
geant pedd; //Ped-DyCh    
geant peddv;//ch-to-ch variation(%)     
geant pedds;//PedSig-DyCh    
geant peddsv;//ch-to-ch variation(%)
integer ReadConstFiles;
integer calvern;     
};
#define ECMCFFKEY COMMON_BLOCK(ECMCFFKEY,ecmcffkey)
COMMON_BLOCK_DEF(ECMCFFKEY_DEF,ECMCFFKEY);
//---
class ECREFFKEY_DEF {
public:
  integer reprtf[3];   //reco print-flags
  integer relogic[5];  //reco logic-flags
  geant thresh[15];       //Time dependent DAQ/Trig-thresholds
  geant cuts[10];       // ........ RECO cuts (clust.thr.,...)
  integer ReadConstFiles;
  integer calutc;//EcalCflistRD-file extention(utc-time)
//
  float Thr1DSeed;        // Threshold for 1d cluster search
  float Thr1DRSeed;        // Threshold for 1d cluster search
  int   Cl1DCoreSize;
  int   Cl1DLeakSize; 
  float Thr2DMax;
  float Length2DMin;
  float Chi22DMax;
  float PosError1D;
  float Chi2Change2D;
  float TransShowerSize2D;
  float SimpleRearLeak[4];
  float CalorTransSize;
  float EMDirCorrection;
  float HiEnThr;
  float HiEnCorFac;
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define ECREFFKEY COMMON_BLOCK(ECREFFKEY,ecreffkey)
COMMON_BLOCK_DEF(ECREFFKEY_DEF,ECREFFKEY);
//---
class ECCAFFKEY_DEF {
public:
  integer cfvers;// not used now
  integer cafdir;// 0/1-> use officical/private directory for calib.files
  integer truse; // 1/0-> use/not tracker info
  integer refpid;// ref.pm id
//   RLGA+FIAT part
  geant trmin;// presel.cut on min. rigidity of the track
  geant adcmin;//cut on min ADC-value of indiv.sub-cell
  geant adcpmx;//cut on max ADC-value of indiv.PM
  integer ntruncl;// remove this number of highest layers
  geant trxac;// TRK->EC extrapolation accuracy in X-proj
  geant tryac;// TRK->EC extrapolation accuracy in Y-proj
  geant mscatp;// EC.mult.scat.param.
  integer nortyp;// normalization type
  integer badplmx;// max. accept. bad sc-planes 
  geant etrunmn;  // Min ECenergy (Etrunc in mev) to select particle(p or He ...)
  geant etrunmx;  // Max ECenergy (Etrunc in mev) ...............................
  geant nsigtrk;  // safety gap in TRK-accur. units for crossing check procedure
//   ANOR part
  geant pmin;       // presel-cut on min. mom. of the track(gev/c) 
  geant pmax;       // presel-cut on max. mom. of the track 
  geant scmin;      // min ADC for indiv. SubCell (to remove ped,noise)
  geant scmax;      // max ADC .................. (to remove sparks,ovfl,...)
  integer spikmx;   // max SC's(spikes) with ADC>above max
  integer nhtlmx[6];   // max hits in sc-plane 1-6(to remove early showering)
  integer lmulmx;      // max hits/sc-plane (to remove events with abn.multiplicity)
  integer nholmx[6];   // max holes(betw.fired cells) in sc-plane 1-6(early show.prot)
  integer nbplmx;      // max bad sc-planes (with spikes or high(abn) multiplicity)
  geant edtmin;        // min Etot(mev) to remove MIP
  geant esleakmx;      // max Eleak(side)/Etot to remove energy side leak
  geant ebleakmx;      // max Eleak(back)/Etot
  geant edfrmn;        // min Efront(mev)
  geant edt2pmx;       // max Etail/Epeak
  geant ed2momc;       // Edep(EC)/Mom(TRK)-1 cut
  geant cog1cut;       // Track-ScPlaneCOG mismatch cut(cm) for the 1st t6 SC-planes.
  geant scdismx[6];    // max sc-track dist. to consider hit as backgroubd(pl 1-6) 
  geant scdisrs;       // as above for all other planes(not used really)
  geant b2scut[6];     // max backgr/signal energy(bound.from above) for pl 1-6
  geant pedcpr;        //PedCal: def portion of highest amplitude to remove for ped calc.
  integer pedoutf;     //        PedOutpFlag 
  geant pedlim[2];     // Ped-limits for PedCalibJobs
  geant siglim[2];     // PedSig-limits .............
//g.chen
  integer ecshswit;      // 0/1 switch to/(not to)  use shower info in calibration
  geant chi2dirmx;     // max chi2 of shower dir fit
  geant prchi2mx;      // max chi2 of shower profile fit
  geant trchi2mx;      // max chi2 of shower trans. fit
  geant eshsleakmx;    // max Eleak(shower side)/Etot
  geant eshrleakmx;    // max Eleak(shower back)/Etot
  geant eshdleakmx;    // max Eleak(shower dead)/Etot
  geant esholeakmx;    // max Eleak(shower orph)/Etot
  geant difsummx;      // cut of (E_x-E_y)/(E_x+E_y)
  geant trentmax[2];   // cut of distance between track and shower entry
  geant trextmax[2];   // cut of distance between track and shower exit
};
#define ECCAFFKEY COMMON_BLOCK(ECCAFFKEY,eccaffkey)
COMMON_BLOCK_DEF(ECCAFFKEY_DEF,ECCAFFKEY);

//===================================================================
class ATGEFFKEY_DEF {
public:
  integer nscpad;  // number of scintillator paddles 
  geant scradi;    // internal radious of ANTI sc. cylinder (cm)
  geant scinth;    // thickness of scintillator (cm)
  geant scleng;    // scintillator paddle length (glob. Z-dim)
  geant wrapth;    // wrapper thickness (cm)
  geant groovr;    // groove radious (bump_rad = groove_rad - pdlgap)
  geant pdlgap;    // inter paddle gap (cm)
  geant stradi;    // support tube intern.radious
  geant stleng;    // support tube length
  geant stthic;    // support tube thickness
};
#define ATGEFFKEY COMMON_BLOCK(ATGEFFKEY,atgeffkey)
COMMON_BLOCK_DEF(ATGEFFKEY_DEF,ATGEFFKEY);
//===================================================================
class ATMCFFKEY_DEF {
public:
integer mcprtf;// hist. print flag
geant FTdel;
geant LSpeed;
//
integer ReadConstFiles;
integer calvern;
//
};
#define ATMCFFKEY COMMON_BLOCK(ATMCFFKEY,atmcffkey)
COMMON_BLOCK_DEF(ATMCFFKEY_DEF,ATMCFFKEY);
//===================================================================
class ATREFFKEY_DEF {
public:
  integer reprtf[2];//  print flag
  integer relogic;// reco logic
  geant Edthr; // threshold (mev) to create Cluster-object
  geant zcerr1;// long.coo error(when 2-sides times are known) 
  geant daqthr; //DAQ-readout threshold(SigmaPed units) 
  geant ftdel; // FT-delay wrt hist-TDC hit 
  geant ftwin; // t-window(ns) for Hist-TDC hit coinc.with FT 
//
  integer ReadConstFiles;
  uinteger calutc;
//
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define ATREFFKEY COMMON_BLOCK(ATREFFKEY,atreffkey)
COMMON_BLOCK_DEF(ATREFFKEY_DEF,ATREFFKEY);
//================================================================
class ATCAFFKEY_DEF {
  public:
  integer cfvers; // spare
  integer cafdir; // use official/private directory for calib.file
  geant pedcpr[2]; //PedCalibJob: Class/DownScaled: portion of highest adcs to remove for ped-calc
  integer pedoutf;//              PedOutputFlag
  geant pedlim[2];// Ped-limits for PedCalibJobs
  geant siglim[2];// PedSig-limits .............
};
#define ATCAFFKEY COMMON_BLOCK(ATCAFFKEY,atcaffkey)
COMMON_BLOCK_DEF(ATCAFFKEY_DEF,ATCAFFKEY);
//================================================================
class TFREFFKEY_DEF {
public:
geant Thr1;  // limit on max
geant ThrS;  // limit on sum
//
  integer reprtf[5]; //RECO print flag 
  integer relogic[5];//RECO logic flag
  geant daqthr[5];// daq thresholds
  geant cuts[10];// cuts 
//
  integer ReadConstFiles;
//
  integer TempHandlMode;
  uinteger calutc;
//
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define TFREFFKEY COMMON_BLOCK(TFREFFKEY,tfreffkey)
COMMON_BLOCK_DEF(TFREFFKEY_DEF,TFREFFKEY);
//===================================================================
class TFCAFFKEY_DEF {
public:
// TZSL-calibration :
geant pcut[2];//low/high limits on momentum of calibr. events
geant bmeanpr;// mean proton velocity in this mom. range
geant tzref[2];// def. T0 for reference counter + spare
geant fixsl;// def. for slope
geant bmeanmu;// mean muon velocity in this mom. range
integer idref[2];// LBB for ref.counter and fix/release flag for trapez.counters
integer ifsl;// 0/1 to fix/release slope param.
integer caltyp;// 0/1 to select space/earth calibration
// AMPL-calibration :
integer truse;// 1/0 to use/not tracker
geant plhc[2];//low/high cuts on tracker mom. for space calibration
integer minev;// min.events needed for measurement in channel or bin
geant trcut;// cut to use for "truncated average" calculation (0.85)
integer spares[4];//
geant adc2q;//adc->charge conv.factor(pC/ADCch)(hope = for all ADC chips)
geant plhec[2];//low/high cuts on tracker mom. for earth calibration
geant bgcut[2];// beta*gamma low/high cuts for mip in abs.calibration
integer tofcoo;// 0/1-> use transv/longit coord. from TOF
geant tofbetac;// if !=0 -> low beta cut (own TOF measurement !!!) 
//                to use when previous calibration suppose to be good enought
integer cfvers;// spare
integer cafdir;// 0/1->use officical/private directory for calib.files
integer mcainc;// spare
geant pedcpr[2];//PedCalJob: portion of highest adcs to remove for ped-calc(Class/DScal)
integer pedoutf;//           PedOutputFlag
geant pedlim[2];// Ped-limits for PedCalibJobs
geant siglim[2];// PedSig-limits .............
// TOFTdcCalib (LINC)
integer minstat;//min.stat/ch
integer tdccum;//tdc-calib usage mode
};
#define TFCAFFKEY COMMON_BLOCK(TFCAFFKEY,tfcaffkey)
COMMON_BLOCK_DEF(TFCAFFKEY_DEF,TFCAFFKEY);
//===================================================================
class TGL1FFKEY_DEF {
public:
integer trtype;
integer toflc;
integer tofsc;
integer toflcsz;
integer toflcz;
integer tofextwid;
integer antismx[2];
integer antisc;
integer cftmask;
integer RebuildLVL1;
geant MaxScalersRate;
geant MinLifeTime;
geant TheMagCut;
integer ecorand;
integer ecprjmask;
integer Lvl1ConfMCVers;
integer Lvl1ConfRDVers;
integer Lvl1ConfRead;
integer printfl;
integer Lvl1ConfSave;
//
integer sec[2];
integer min[2];
integer hour[2];
integer day[2];
integer mon[2];
integer year[2];
};
#define TGL1FFKEY COMMON_BLOCK(TGL1FFKEY,tgl1ffkey)
COMMON_BLOCK_DEF(TGL1FFKEY_DEF,TGL1FFKEY);
//================================================================



//
const integer npatb=10;
class BETAFITFFKEY_DEF {
public:
  integer pattern[npatb];  //patterns  to fit; Priority decreases with number
  //    Number              Descr             Points      Default
  //    0                   1234              4           on
  //    1                   123               3           on
  //    2                   124               3           on
  //    3                   134               3           on
  //    4                   234               3           on
  //    5                   13                2           on
  //    6                   14                2           on
  //    7                   23                2           on
  //    8                   24                2           on
  //    9                   12                2           off
  geant Chi2;  //  Max acceptable chi2 for betafit
  geant SearchReg[3]; // Max distance between track & tof hit
  geant LowBetaThr;   // Threshold below refit should be done if possible 
                           // using 2 & 3 (comb # 7)i
  integer FullReco;
  geant Chi2S;  // Max acceptable chi2 for space fit
  integer MassFromBetaRaw;
};
//



#define BETAFITFFKEY COMMON_BLOCK(BETAFITFFKEY,betafitffkey)
COMMON_BLOCK_DEF(BETAFITFFKEY_DEF,BETAFITFFKEY);

class CHARGEFITFFKEY_DEF {
public:
  integer NmembMax;
  integer Tracker;
  geant EtaMin[2];
  geant EtaMax[2];
  geant ProbTrkRefit;
  geant ResCut[2];
  geant SigMin;
  geant SigMax;
  integer PdfNorm;
  integer TrMeanRes;
  geant ProbMin;
  integer TrackerOnly;
  integer ChrgMaxAnode;
  integer BetaPowAnode;
  integer TrackerForceSK;
  geant TrackerKSRatio;
  integer TrackerProbOnly;
  integer TrkPDFileMCVers;
  integer TrkPDFileRDVers;
  integer TrkPDFileRead;
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
//



#define CHARGEFITFFKEY COMMON_BLOCK(CHARGEFITFFKEY,chargefitffkey)
COMMON_BLOCK_DEF(CHARGEFITFFKEY_DEF,CHARGEFITFFKEY);

//

//

class CCFFKEY_DEF {
public:
  geant coo[6];       //1-6
  geant dir[6];       //7-12
  geant momr[2];      //13-14
  integer fixp;       //15
  geant albedor;      //16
  geant albedocz;     //17
  integer npat;       //18
  integer run;
  integer low;
  integer earth;      // earth mafnetic field modulation
  geant theta;         // station theta at begin
  geant phi;           // station phi at begin
  geant polephi;       // north pole phi
  integer begindate;      // time begin format ddmmyyyy
  integer begintime;      // time begin format hhmmss
  integer enddate;        // time end   format ddmmyyyy
  integer endtime;        // time end   format hhmmss
  integer sdir;            // direction + 1 to higher -1 to lower theta
  integer oldformat;
  integer Fast;            // fast generation
  geant   StrCharge;
  geant   StrMass; 
  integer SpecialCut;     // special cut to accept
                          //  1:  focus on ecal 
  integer FluxFile[40];   // FluxFile 
  integer curtime;        // current event time
  integer DirFilePositions[2];
  integer DirFile[40];
  float Angle;
  void init();
};
//
#define CCFFKEY COMMON_BLOCK(CCFFKEY,ccffkey)
COMMON_BLOCK_DEF(CCFFKEY_DEF,CCFFKEY);

//
//ISN 
class GMFFKEY_DEF {
public:
integer GammaSource; // gamma source flag/identifier. 1 for user defined
geant SourceCoo[2]; // user defined source equatorial coordinates (GammaSource=1)
geant SourceVisib; // maximum zenith angle (to speed up simulation)
integer GammaBg; // diffuse background flag 
geant BgAngle; // angle for background integration
};
#define GMFFKEY COMMON_BLOCK(GMFFKEY,gmffkey)
COMMON_BLOCK_DEF(GMFFKEY_DEF,GMFFKEY);







/*
const integer mmax=nx*ny*nz;
const integer lrq=10*mmax+7;
const integer liq=2*mmax+1;

class TKFIELDADDON_DEF{
public:
int    iqx[liq];
int    iqy[liq];
int    iqz[liq];
geant  rqx[lrq][2];
geant  rqy[lrq][2];
geant  rqz[lrq][2];

};
#define TKFIELDADDON COMMON_BLOCK(TKFIELDADDON,tkfieldaddon)
COMMON_BLOCK_DEF(TKFIELDADDON_DEF,TKFIELDADDON);
*/






class CALIB_DEF{
public:
integer InsertTimeProc;             // 0 (default) by current time ; 1 by run time
integer Ntuple; 
};
#define CALIB COMMON_BLOCK(CALIB,calib)
COMMON_BLOCK_DEF(CALIB_DEF,CALIB);





class TRDCALIB_DEF{
public:
integer CalibProcedureNo;
integer EventsPerCheck;
geant PedAccRequired; 
integer Validity[2];
geant BadChanThr;
};
#define TRDCALIB COMMON_BLOCK(TRDCALIB,trdcalib)
COMMON_BLOCK_DEF(TRDCALIB_DEF,TRDCALIB);





class G4FFKEY_DEF{
public:
int BFOrder;
float Delta;
int UniformMagField;
int Geant3CutsOn;
int PhysicsListUsed;
int LowEMagProcUsed;
};
#define G4FFKEY COMMON_BLOCK(G4FFKEY,g4ffkey)
COMMON_BLOCK_DEF(G4FFKEY_DEF,G4FFKEY);


// RICH Geometry card: currently unused

class 
RICGEOM_DEF{
public:
// Mirrors
geant top_radius; 
geant bottom_radius;
geant hole_radius;
geant height;
geant inner_mirror_height;
// Radiator
geant radiator_radius;
geant radiator_height;
geant radiator_box_length; // The aerogel is an array of this length
// lighguides and PMT
geant light_guides_height; // Unused
geant light_guides_length;
};
#define RICGEOM COMMON_BLOCK(RICGEOM,ricgeom)
COMMON_BLOCK_DEF(RICGEOM_DEF,RICGEOM);

class RICCONTROLFFKEY_DEF{
public:
integer iflgk_flag;
integer tsplit; // Allows task time spliting at the end of the job for OPTIMIZATION
integer pmttables[50];     // Precomputed input PMT tables if any
integer pmttables_out[50]; // Output of precomputed PMT tables if any
};
#define RICCONTROLFFKEY COMMON_BLOCK(RICCONTROLFFKEY,riccontrolffkey)
COMMON_BLOCK_DEF(RICCONTROLFFKEY_DEF,RICCONTROLFFKEY);


class RICRADSETUPFFKEY_DEF{
public:
integer setup;  // Allows to choose among several differen geometries
integer tables_in[50];     // Input file of radiator tables
integer tables_out[50];    // Output file of radiator tables
};
#define RICRADSETUPFFKEY COMMON_BLOCK(RICRADSETUPFFKEY,ricradsetupffkey)
COMMON_BLOCK_DEF(RICRADSETUPFFKEY_DEF,RICRADSETUPFFKEY);


class RICRECFFKEY_DEF{
public:
integer recon[2];
float   pars0[5];
float   pars1[5];
float   pars2[5];
float   pars3[5];
};
#define RICRECFFKEY COMMON_BLOCK(RICRECFFKEY,ricrecffkey)
COMMON_BLOCK_DEF(RICRECFFKEY_DEF,RICRECFFKEY);



class RICFFKEY_DEF{
 public:  
  integer ReadFile;  
  integer sec[2];        // calibration validation
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
  integer fname_in[200];
  integer fname_out[200];

};
#define RICFFKEY COMMON_BLOCK(RICFFKEY,ricffkey)
COMMON_BLOCK_DEF(RICFFKEY_DEF,RICFFKEY);



class
RICGTKOV_DEF{
public:
geant usrcla;
geant scatprob;
geant scatang;
};

#define RICGTKOV COMMON_BLOCK(RICGTKOV,gtckovext)
COMMON_BLOCK_DEF(RICGTKOV_DEF,RICGTKOV);

class
PRODFFKEY_DEF{
public:
integer Debug;
};

#define PRODFFKEY COMMON_BLOCK(PRODFFKEY,prodffkey)
COMMON_BLOCK_DEF(PRODFFKEY_DEF,PRODFFKEY);

//+LIP
// commons

// geometry
class LIPGEO_DEF{
 public:
  // dimensions

  geant ztoprad_ams;
  geant zpmtdet_c;
  geant hmir_c;
  geant rtmir_c;
  geant rbmir_c;
  geant emcxlim_c;
  geant emcylim_c;
  geant ztmirgap_c;
  geant zbmirgap_c;
  geant reflec_c;
  // light guides
  geant lg_length_c;
  geant lg_height_c;
  // pmt
  geant pmt_suptk_c;
  geant pmt_shdtk_c;
  geant pmt_sidel_c;
};
#define LIPGEO COMMON_BLOCK(LIPGEO,lipgeo)
COMMON_BLOCK_DEF(LIPGEO_DEF,LIPGEO);

// radiator
class LIPRAD_DEF{
 public:

  // radiator

  geant hrad_c;
  geant refindex_c;
  geant clarity_c;
  int radtype_c;
  geant rrad_c;
  geant ltile_c;
  int nabslen_c;
  geant labsrad_c[44];
  // foil
  geant hpgl_c;
  geant pglix_c;
};
#define LIPRAD COMMON_BLOCK(LIPRAD,liprad)
COMMON_BLOCK_DEF(LIPRAD_DEF,LIPRAD);


// hit parameters

class LIPDAT_DEF{
 public:
  int     nbhitsmax_c;
  int     nbhits_c;
  int     hitsadc_c[16000];
  geant   hitsnpe_c[16000];
  geant   hitscoo_c[16000][3];
  int     hitshid_c[16000];
};
#define LIPDAT COMMON_BLOCK(LIPDAT,lipdat)
COMMON_BLOCK_DEF(LIPDAT_DEF,LIPDAT);

// track parameters
class LIPTRK_DEF{
 public:
  geant pimp_c[3];
  geant pmom_c;
  geant pthe_c;
  geant pphi_c;
  geant pcoopmt_c[3];
  geant cerang_c;
  geant pbeta_c;
  geant pchg_c;
};
#define LIPTRK COMMON_BLOCK(LIPTRK,liptrk)
COMMON_BLOCK_DEF(LIPTRK_DEF,LIPTRK);

// output parameters
class LIPVAR_DEF{
public:
int   liphused;
geant lipthc;
geant lipbeta;
geant lipebeta;
geant liplikep;
geant lipchi2;
geant liprprob;
};
#define LIPVAR COMMON_BLOCK(LIPVAR,lipvar)
COMMON_BLOCK_DEF(LIPVAR_DEF,LIPVAR);
// ENDofLIP





#endif