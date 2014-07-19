
#include "dcards.h"
#include <math.h>
#include "cern.h"
#include <float.h>

void TRMFFKEY_DEF::init(){
OKAY=0;
}
TRMFFKEY_DEF TRMFFKEY;

void STATUSFFKEY_DEF::init(){
  
  for(int i=0;i<32;i++){
    status[i]=0;
  }
  status[33]=10;
  
}
STATUSFFKEY_DEF STATUSFFKEY;

void SELECTFFKEY_DEF::init(){
  Run=0;
  Event=0;
  RunE=0;
  EventE=0;
  VBLANK(File,40);
}
SELECTFFKEY_DEF SELECTFFKEY;
//=============================
void AMSFFKEY_DEF::init(){
  Simulation=0; // Simulation
  Reconstruction=1; // Reconstruction
  Jobtype=0; // Default
  Debug=1;
  CpuLimit=15;
  Read=0;
  Write=1;
  Update=0;
  VBLANK(Jobname,40);
  VBLANK(Setupname,40);
  char amsetup[16]="AMS02";
  UCTOH(amsetup,Setupname,4,16);
  ZeroSetupOk=0;
}
AMSFFKEY_DEF AMSFFKEY;
//=============================
void IOPA_DEF::init(){
hlun=0;
VBLANK(hfile,40);
ntuple=1;
Portion=.1;
WriteAll=102;
VBLANK(TriggerC,40);
VBLANK(AMSFFKEY.TDVC,400);
char amsp[12]="AMSParticle";
UCTOH(amsp,TriggerC,4,12);
mode=0;
VBLANK(ffile,40);
MaxNtupleEntries=100000;
MaxFileSize=180000000;
MaxFileTime=86400*3;
BuildMin=-1;
WriteRoot=0;
VBLANK(rfile,40);
}
IOPA_DEF IOPA;
//=============================
void MISCFFKEY_DEF::init(){
  BTempCorrection=1;
BeamTest=0;
BZCorr=1;
G3On=1;
G4On=0;
dbwrbeg=0;//DBwriter UIC-time begin 
dbwrend=0;//DBwriter UIC-time end
}
MISCFFKEY_DEF MISCFFKEY;
//=============================

void G4FFKEY_DEF::init(){
 BFOrder=4;
 Delta=0.001;
 UniformMagField=0;
 Geant3CutsOn=1;
 PhysicsListUsed=1;
 LowEMagProcUsed=0;
}
G4FFKEY_DEF G4FFKEY;


void PRODFFKEY_DEF::init(){
  Debug=0;
}
PRODFFKEY_DEF PRODFFKEY;


//=============================
//    TRACKER SIMULATION
//=============================
void TKGEOMFFKEY_DEF::init() {
  ReadGeomFromFile=1;
  WriteGeomToFile=0;
  UpdateGeomFile=0;
  ZShift=0;
}
TKGEOMFFKEY_DEF TKGEOMFFKEY;
//=============================

void TRMCFFKEY_DEF::init(){
  alpha=250;
  beta=0.46;
  gamma=0.25;
  fastswitch=5.e-5;  // inverse linear density of primary electrons
  dedx2nprel=0.33e6;
  ped[0]=500;
  ped[1]=500;
  gain[0]=8;
  gain[1]=8;
  sigma[1]=55./14./sqrt(3.); // sig/noise ratio is about 14 for y
  sigma[0]=sigma[1]*1.41;   // x strip two times larger y
  delta[0]=-0.67;
  delta[1]=1.0;
  gammaA[0]=-0.3;
  gammaA[1]=0.1;
  NonGaussianPart[0]=0;
  NonGaussianPart[1]=0.1;
  BadCh[0]=0.01;
  BadCh[1]=0.01;

  cmn[0]=10;
  cmn[1]= 6;
  adcoverflow=32700;
  NoiseOn=1;
  sec[0]=0;
  sec[1]=0;
  min[0]=0;
  min[1]=0;
  hour[0]=0;
  hour[1]=0;
  day[0]=1;
  day[1]=1;
  mon[0]=0;
  mon[1]=0;
  year[0]=101;
  year[1]=110;
  GenerateConst=0;
  WriteHK=0;
  thr1R[0]=-2.75;
  thr1R[1]=3.5;
  thr2R[0]=1;
  thr2R[1]=1;
  neib[0]=0;
  neib[1]=1;
  CalcCmnNoise[0]=1;
  CalcCmnNoise[1]=0;
  {
    int i,j,k;
    for(i=0;i<8;i++){
      for(j=0;j<2;j++){
	for(k=0;k<32;k++)RawModeOn[i][j][k]=0;
      }
    }
  }
}
TRMCFFKEY_DEF TRMCFFKEY;
//=============================
void TRCALIB_DEF::init(){
  CalibProcedureNo=0;
  EventsPerCheck=100;
  PedAccRequired[0]=0.12;
  PedAccRequired[1]=0.09;
  Validity[0]=0;
  Validity[1]=36000;
  RhoThrA=0.7;
  RhoThrV=0.8;
  BadChanThr[0]=3.3;
  BadChanThr[1]=0.002;
  Method=2;
  Pass=1;
  DPS=1;
  UPDF=4;
  PrintBadChList=0;
  EventsPerIteration[0]=100;
  EventsPerIteration[1]=100;
  EventsPerIteration[2]=100;
  EventsPerIteration[3]=100;
  NumberOfIterations[0]=100;
  NumberOfIterations[1]=100;
  NumberOfIterations[2]=100;
  NumberOfIterations[3]=100;
  BetaCut[0][0]=0.7;
  BetaCut[0][1]=10.;
  BetaCut[1][0]=1;
  BetaCut[1][1]=10;
  BetaCut[2][0]=0.7;
  BetaCut[2][1]=1.4;
  BetaCut[3][0]=0.7;
  BetaCut[3][1]=1.4;
  HitsRatioCut[0]=2.2;
  HitsRatioCut[1]=2.2;
  HitsRatioCut[2]=0.998;
  HitsRatioCut[3]=0.998;
  MomentumCut[0][0]=-FLT_MAX;
  MomentumCut[0][1]=FLT_MAX;
  MomentumCut[1][0]=3;
  MomentumCut[1][1]=FLT_MAX;
  MomentumCut[2][0]=0.4;
  MomentumCut[2][1]=2.5;
  MomentumCut[3][0]=0.1;
  MomentumCut[3][1]=10;
  Chi2Cut[0]=3;
  Chi2Cut[1]=3;
  Chi2Cut[2]=100;
  Chi2Cut[3]=100;
  PatStart=0;
  MultiRun=0;
  EventsPerRun=10000001;
  int i;
  for(i=0;i<6;i++){
    Ladder[i]=0;
    ActiveParameters[i][0]=1;   // x
    ActiveParameters[i][1]=1;   // y
    ActiveParameters[i][2]=0;   // z
    ActiveParameters[i][3]=0;   // pitch  zx
    ActiveParameters[i][4]=1;   // yaw    xy
    ActiveParameters[i][5]=0;   // roll   yz
  }
}
TRCALIB_DEF TRCALIB;
//=============================
void TRALIG_DEF::init(){
  One=1.025;
  GlobalFit=0;
  InitDB=0;
  ReWriteDB=0;
  UpdateDB=0;
  MaxPatternsPerJob=50;
  MaxEventsPerFit=9999;
  MinEventsPerFit=999;
  for(int i=0;i<9;i++){
    Cuts[i][0]=0;
    Cuts[i][1]=0;
  }
  Cuts[0][1]=1;      // chi2 cut for alg=0
  Cuts[1][0]=0.992;   // cos  cut for alg =0 
  Cuts[2][0]=0.5;     //rel mom cut
  Cuts[2][1]=2;
  Cuts[3][0]=0;      // beta cut
  Cuts[3][1]=1.4;    
  Cuts[4][0]=0;      // beta pattern cut
  Cuts[4][1]=0;    
  Cuts[5][0]=0.5;      // mass cut
  Cuts[5][1]=2.0;    
  Cuts[6][0]=0.;         // betaerror cut
  Cuts[6][1]=0.04;   

  Algorithm=0;
  for( int i=0;i<6;i++){
    ActiveParameters[i][0]=1;   // x
    ActiveParameters[i][1]=1;   // y
    ActiveParameters[i][2]=1;   // z
    ActiveParameters[i][3]=1;   // pitch  zx
    ActiveParameters[i][4]=1;   // yaw    xy
    ActiveParameters[i][5]=1;   // roll   yz
  }
  EventsPerRun=1001;
  LayersOnly=0;
  GlobalGoodLimit=0.085;
  SingleLadderEntryLimit=10000;
}
TRALIG_DEF TRALIG;
//=============================


//=============================
//    TRACKER Recon
//=============================

void TRCLFFKEY_DEF::init(){

  //number fac=AMSTrRawCluster::ADC2KeV()*0.46/0.4;
  ThrClA[0]=10.;
  ThrClA[1]=10.;

  ThrClS[0]=20;
  ThrClS[1]=15;

  ThrClR[0]=3.;
  ThrClR[1]=4.;

  Thr1A[0] =8.;
  Thr1A[1] =8.;

  Thr2A[0] =4.;
  Thr2A[1] =3.;

  Thr1S[0] =20;
  Thr1S[1] =15;

  Thr2S[0] =15;
  Thr2S[1] =10;

  Thr1R[0] =-2.75;
  Thr1R[1] =3.5;

  ThrClS[0]=20;
  Thr1S[0] =15;
  Thr2S[0] =15;

  Thr2R[0] =1.;
  Thr2R[1] =3.1;  // should be around 1 if ThrClNEl[1]=3;
  // should be around 3 if ThrClNEl[1]=5;
  Thr3R[0] =-2.;
  Thr3R[1] =-2.;

  ThrClNMin[0]=1;
  ThrClNMin[1]=1;

  ThrClNEl[0]=3;
  ThrClNEl[1]=5;

  ErrX=34.e-4;
  ErrY=20.e-4;
  ErrZ=34.e-4;
  ThrDSide=1.;

  CorFunParA[0][0]=400e-4;
  //CorFunParB[0][0]=0.85;
  CorFunParB[0][0]=1.;
  CorFunParA[0][5]=400e-4;
  CorFunParB[0][5]=1.;
  int k;
  for (k=1;k<5;k++){
    CorFunParA[0][k]=400e-4;
    CorFunParB[0][k]=1.;
  }
  for ( k=0;k<6;k++){
    CorFunParA[1][k]=65e-4;
    // CorFunParB[1][k]=0.5;
    CorFunParB[1][k]=1.;
  }

  for(k=0;k<6;k++){
    for(int l=0;l<3;l++){
      ResFunX[l][k]=0;
      ResFunY[l][k]=0;
    }
  }
  CommonGain[0]=1.;
  CommonGain[1]=1.;
  CommonShift[0]=0.;
  CommonShift[1]=0.;

  EtaCor[0]=1;
  EtaCor[1]=0;
}
TRCLFFKEY_DEF TRCLFFKEY;

//=============================
void TRFITFFKEY_DEF::init(){

  // Fit Par
  {
    for( int k=0;k<sizeof(patternp)/sizeof(patternp[0]);k++)patternp[k]=0;
  }
  UseTRD=1;
  UseTOF=2;
  Chi2FastFit=2000;
  Chi2StrLine=10;
  Chi2WithoutMS=5;
  ResCutFastFit=0.5;
  ResCutStrLine=0.5;
  ResCutCircle=0.5;
  SearchRegFastFit=1;
  SearchRegStrLine=1.5;
  SearchRegCircle=10.;
  RidgidityMin=0.02;
  FullReco=0;
  MinRefitCos[0]=0.7;
  MinRefitCos[1]=0.5;
  FastTracking=1;
  WeakTracking=1;
  FalseXTracking=1;
  Chi2FalseX=3.;
  ForceFalseTOFX=1;
  FalseTOFXTracking=1;
  TOFTracking=1;
  ForceAdvancedFit=1;
  ThrClA[0]=0.;
  ThrClA[1]=0.;
  ThrClR[0]=0.;
  ThrClR[1]=0.;
  MaxTrRecHitsPerLayer=250;
  LowMargin=0;
  OnlyGammaVtx=0;
  UseGeaneFitting=0;
  OldTracking=0;
}
TRFITFFKEY_DEF TRFITFFKEY;

//=============================
//  Event generator
//=============================
void CCFFKEY_DEF::init(){
  coo[0]=-1.e10;
  coo[1]=-1.e10;
  coo[2]=-1.e10;
  coo[3]=1.e10;
  coo[4]=1.e10;
  coo[5]=1.e10;
  dir[0]=-1.;
  dir[1]=-1.;
  dir[2]=-1.;
  dir[3]=1.;
  dir[4]=1.;
  dir[5]=1.;
  momr[0]=-1.e10;
  momr[1]=1.e10;
  fixp=0;
  albedor=0.;
  albedocz=0.05;
  npat=1;
  run=100;
  low=5;
  earth=0;
  theta=51.;
  phi=290.;
  polephi=108.392;
  begindate=2062004;
  enddate=1012008;
  begintime=170000;
  endtime=0;
  oldformat=0;
  sdir=1;
  Fast=0;
  StrCharge=2;
  StrMass=-1;
  SpecialCut=0;
  VBLANK(FluxFile,40);
  curtime=0;
}
CCFFKEY_DEF CCFFKEY;
//=============================
void GMFFKEY_DEF::init(){
  GammaSource=0; //ISN  
  SourceCoo[0]=1.46; //RA
  SourceCoo[1]=0.384; //DEC default:Crab
  SourceVisib=0.873; //50 degrees
  GammaBg=0;
  BgAngle=0.087; //5 degrees
}
GMFFKEY_DEF GMFFKEY; //ISN
//=============================

//=============================
//  TOF + ANTI Simulation
//=============================
void TFMCFFKEY_DEF::init(){

  TimeSigma=0.240; //(1) side time resolution(ns,=CounterResol(0.17)*sqrt(2)) 
  TimeSigma2=0.45; //(2)
  TimeProbability2=0.035;//(3)
  padl=11.5;        //(4) not used now (spare)
  Thr=0.1;          //(5) Sc.bar Edep-thresh.(Mev) to participate in Simul.   
//
  mcprtf[0]=0;     //(6) TOF MC print flag for init arrays(=1/2-> SEspecra/PMpulseShape)
  mcprtf[1]=0;     //(7) TOF MC print flag for MC PM-pulses(1/2-> fine/coarse scale)
  mcprtf[2]=0;     //(8) ...................... histograms
  mcprtf[3]=0;     //(9) Debug-printing
  mcprtf[4]=0;     //(10) spare
  trlogic[0]=1; //(11) spare 
  trlogic[1]=0; //(12) spare 
  fast=0;       //(13) 0/1/2->off/on fast generation in mceventg.C(2-> EC requirement)
  daqfmt=0;     //(14) 0/1-> raw/reduced TDC format for DAQ simulation
  birks=1;      //(15) 0/1->  not apply/apply birks corrections
  mcseedo=0;    //(16) 1/0->Use RealDataCalib_Copy/MCCalib files as MCSeed
  blshift=0.;   //(17) base line shift at fast discr.input (mv)
  hfnoise=5.;   //(18) high freq. noise .......   
//     
  ReadConstFiles=0;//(19)PTS(P=PedsMC,T=TimeDistr,S=MCCalibSeeds);P(T,S)=0/1->DB/RawFiles
//
  addpeds=0;//(20) add peds into empty(no MC dE/dX) channels
//
}
TFMCFFKEY_DEF TFMCFFKEY;
//=============================
void ATGEFFKEY_DEF::init(){
  scradi=54.55;   // internal radious of ANTI sc. cylinder (cm)
  scinth=0.8;     // thickness of scintillator (cm)
  scleng=82.65;  //(ams02) scintillator paddle length (glob. Z-dim)
  wrapth=0.04;   // wrapper thickness (cm)
  groovr=0.4;   // groove radious (bump_rad = groove_rad-pdlgap)
  pdlgap=0.1;   // inter paddle gap (cm)(2*wrapth+extra)
  stradi=54.235; // inner radious of supp. tube
  stleng=85.;    // (ams02)length of supp. tube
  stthic=0.12;   // thickness of supp. tube
}
ATGEFFKEY_DEF ATGEFFKEY;
//=============================
void ATMCFFKEY_DEF::init(){
  ATMCFFKEY.mcprtf=0;//(1)print-flag(0/1/2/3->print:no/histogr/PulseSh_arr/print_pulse)
  ATMCFFKEY.LZero=0; // (2)spare
  ATMCFFKEY.LSpeed=14.7;// (3)Eff. light speed in anti-paddle (cm/ns)
  ATMCFFKEY.ReadConstFiles=0;//(4)Sp|Rp(Seed|Real MCPeds), S,R=0/1-> read from DB/RawFiles
}
ATMCFFKEY_DEF ATMCFFKEY;

//=============================
//=============================

//=============================
//  TOF + ANTI Recon 
//=============================

void TFREFFKEY_DEF::init(){
// 
  Thr1=0.2;//(1) Bar threshold (mev) in tof-cluster algorithm
  ThrS=0.2; //(2) Threshold (mev) on total cluster energy(Not used now!)
//
  reprtf[0]=0; //(3) RECO print flag for statistics 
  reprtf[1]=0; //(4) print flag for DAQ (1/2-> print for decoding/dec+encoding)
  reprtf[2]=0; //(5) print flag for histograms
  reprtf[3]=0; //(6) print flag for TDC-hit multiplicity histograms 
  reprtf[4]=0; //(7) print flag for Debug needs  
//
  relogic[0]=0;//(8) 0/1/2/3/4/5/6/7 ->normal/STRR+AVSD/TDIF/TZSL/AMPL/PEDScl/ds/OnBoardTable-calibr. run. 
  relogic[1]=1;//(9) 1/0-> use/not SumHTchannel for matching with LTtime-channel 
  relogic[2]=0;//(10) 0/1-> force 1-side suppression(useful for MC processing)
  relogic[3]=0;//(11) 1/0->Do/not recovering of missing side 
  relogic[4]=1;//(12) 1/0->create(+write)/not TOF2RawSideObject-info into ntuple
//
  daqthr[0]=30.;//(13)tempor Anode low discr.thresh(30mV) for fast/slow_TDC 
  daqthr[1]=70.;//(14)tempor Anode high discr.thresh(100mV) for FT-trigger (z>=1)  
  daqthr[2]=250.;//(15)tempor Anode superhigh discr.thresh(mV) for  "z>=2"-trig  
  daqthr[3]=2.5;//(16) Anode-ADC-readout threshold in DAQ (in PedSigmas)    
  daqthr[4]=2.5;//(17) Dynode-ADC-readout threshold in DAQ (in PedSigmas)
//
  cuts[0]=10.;//(18) window(ns) to find "corresponding" hits in LT-/sumHT-channels
  cuts[1]=2000.;//(19)"befor"-cut in time history (ns)(max. integr.time?)
  cuts[2]=100.;//(20)"after"-cut in time history (ns)
  cuts[3]=2.8; //(21) error(cm) in longitudinal coordinate (for mip in single TOF bar)
  cuts[4]=40.;//(22) min(fixed) FT delay(ns)
  cuts[5]=40.;//(23) spare 
  cuts[6]=0.6;//(24) 2-bars assim.cut in TOFCluster energy calculation
  cuts[7]=8.;// (25) internal longit.coo matching cut ...Not used (spare)
  cuts[8]=50.;//(26) spare 
  cuts[9]=0.;// (27) 
//
  ReadConstFiles=100;//(28) QDPC(Q->ChargeCalib(mc/rd),D->ThrCuts-set(datacards),P->Peds(rd),
//                                                                       C->CalibConst(rd/mc));
// Q=1/0->Take ChargeCalibDensFunctions from RawFiles/DB
// D=1/0->Take ThreshCuts-set from DataCards/DB,
// P=1/0->Take Peds(rd) from RawFiles/DB,
// C=1/0->Take PaddleCalibrConst from RawFiles/DB
//
  sec[0]=0;//(29) 
  sec[1]=0;
  min[0]=0;
  min[1]=0;
  hour[0]=0;
  hour[1]=0;
  day[0]=1;
  day[1]=1;
  mon[0]=0;
  mon[1]=0;
  year[0]=101;
  year[1]=110;//(40)
}
TFREFFKEY_DEF TFREFFKEY;

//=============================
void TFCAFFKEY_DEF::init(){
//    defaults for calibration:
// TZSL-calibration:
  pcut[0]=5.;// (1)track mom. low limit (gev/c) (prot, put 0.75 for mu)
  pcut[1]=100.;// (2)track mom. high limit
  bmeanpr=0.996;// (3)mean prot. velocity in the above range
  tzref[0]=0.;//(4)T0 for ref. counter
  tzref[1]=0.;//(5) spare
  fixsl=5.;// (6)def. slope
  bmeanmu=0.997;// (7)mean muon velocity in the above range
  idref[0]=104;//(8)LBB for first ref. counter 
  idref[1]=0;//(9)0/1/2->FitAll/IgnorTrapezCount/FitTrapezCount&FixOthers
  ifsl=1;//(10) 0/1 to fix/release slope param.
//
  caltyp=0;// (11) 0/1->space/earth calibration
//
// AMPL-calibration:
  truse=1; // (12) 1/-1-> to use/not tracker
  plhc[0]=0.5;// (13) track mom. low limit(gev/c) for space calibr
  plhc[1]=47.;// (14) track mom. high limit(gev/c) ..............
  minev=80;// (15)min.events needed for measurement in channel or bin
  trcut=0.92;// (16) cut to use for "truncated average" calculation
  spares[0]=0;//spare integers
  spares[1]=0;//spare integers
  spares[2]=0;//spare integers
  spares[3]=0;//spare integers
  adc2q=1.;//(21)adc->charge conv.factor(pC/ADCch, hope = for all ADC chips)
  plhec[0]=0.5;//(22)plow-cut for earth calibration
  plhec[1]=10.;//(23)phigh-cut ...................
  bgcut[0]=0.58; //(24) beta*gamma low-cut to be around mip-region(abs.calib)
  bgcut[1]=50.;//(25) beta*gamma high-cut ..............................
//
  tofcoo=0; // (26) 0/1-> use transv/longit coord. from TOF 
  dynflg=0; // (27)  not used now
  cfvers=2; // (28) 1-999 -> vers.number for tof2cvlistNNN.dat file
  cafdir=0;// (29) 0/1-> use official/private directory for calibr.files
  mcainc=0;// (30) spare
  tofbetac=0.5;// (31) if nonzero->low beta cut (own TOF measurements !!!)
  pedcpr[0]=0.01; // (32) PedCalibJobClass: portion of highest adcs to remove for ped-calc
  pedcpr[1]=0.1;  // (33) PedCalibJobDScal: portion of highest adcs to remove for ped-calc
  pedoutf=2;      // (34)  --//-- outp.flag: 0/1/2-> HistosOnly/PedWr2DB+File/PedWr2File
//
}
TFCAFFKEY_DEF TFCAFFKEY;

//=============================
void ATREFFKEY_DEF::init(){
  reprtf[0]=0;//(1) Reco print_hist flag (0/1->no/yes)
  reprtf[1]=0;//(2) DAQ-print (1/2->print for decoding/decoding+encoding)
  relogic=0;  //(3) =0/1/2/3/4->Normal/AbsCal/PedCal_Clas(randTrg)/PedCal_DwnScal(onData)/PedCal_OnBoardTable
  Edthr=0.1;  //(4) threshold to create Cluster(Paddle) object (mev)
  zcerr1=10.; //(5) Err(cm).in longit.coord. when 2-sides times are known 
  daqthr=3.;  //(6) spare
  ftdel=50.;  //(7) FT-delay wrt correlated Anti history-pulse
  ftwin=70.;  //(8) window to check Hist-hit/FT coincidence(+- around FT-delay corrected value)
//
  ReadConstFiles=0;//(9)PVS(RD_Peds,VariabCalibPar(mc/rd),StabCalibPar(mc/rd)), P(V,S)=0/1-> DB/RawFiles
//  
  sec[0]=0;//(10) 
  sec[1]=0;//(11)
  min[0]=0;//(12)
  min[1]=0;//(13)
  hour[0]=0;//(14)
  hour[1]=0;//(15)
  day[0]=1;
  day[1]=1;
  mon[0]=0;
  mon[1]=0;
  year[0]=101;//(20)
  year[1]=110;
}
ATREFFKEY_DEF ATREFFKEY;

//=============================
void ATCAFFKEY_DEF::init(){
// defaults for calibration:
  ATCAFFKEY.cfvers=4; //(1) (001-999) vers.number NN for antiverlistNN.dat file
  ATCAFFKEY.cafdir=0;// (2)  0/1-> use official/private directory for calibr.files
  ATCAFFKEY.pedcpr[0]=0.01; // (3) PedCalibJobRandom(classic): portion of highest adcs to remove
  ATCAFFKEY.pedcpr[1]=0.035;// (4) PedCalibJobDownScaled(in trig): portion of highest adcs to remove
  ATCAFFKEY.pedoutf=2;      // (5)  --//-- outp.flag: 0/1/2-> HistosOnly/PedWr2DB+File/PedWr2File
//
}
ATCAFFKEY_DEF ATCAFFKEY;







//=============================
// ECAL Simulation
//=============================
void ECMCFFKEY_DEF::init(){
  fastsim=0;     //(1) 1/0-> fast/slow simulation algorithm(missing fast TBD)
  mcprtf=0;       //(2) print_hist flag (0/1->no/yes)
  cutge=0.0005;    //(3) cutgam=cutele cut for EC_volumes
  silogic[0]=0;   //(4) SIMU logic flag =0/1/2->peds+noise/no_noise/no_peds
  silogic[1]=0;   //(5) 1/0-> to use RealDataCopy(sd)/MC(mc) RLGA/FIAT-files as MCSeeds
  mev2mev=59.27/1.007;  //(6) Geant dE/dX(MeV)->MCEmeas(MeV) conv.factor(at EC-center)  ! corrected for  500 kev geant3 cut
  mev2adc=0.873;  //(7) MCEmeas(MeV)->ADCch factor(MIP-m.p.->10th channel)(...)
  safext=-10.;    //(8) Extention(cm) of EC transv.size when TFMC 13=2 is used
  mev2pes=55.;    //(9) PM ph.electrons/Mev(dE/dX)(8000*0.0344*0.2)
  pmseres=0.8;    //(10)PM single-electron spectrum resolution
  adc2q=0.045;    //(11)Anode(H,L) ADCch->Q(pC) conv.factor(pC/adc)
  fendrf=0.3;     //(12) fiber end-cut reflection factor
  physa2d=6.;     //(13) physical(for trigger) an/dyn ratio(mv/mv) 
  hi2low=36.;     //(14) not used now 
  sbcgn=1.;       //(15) not used now 
  pedh=150;       //(16)Ped-HiCh    
  pedvh=30;       //(17)ch-to-ch variation(%)     
  pedl=150;       //(18)Ped-LoCh    
  pedvl=30;       //(19)ch-to-ch variations(%)
  pedsh=0.55;     //(20)PedSig-HiCh     
  pedsvh=30;      //(21)ch-to-ch variation(%)     
  pedsl=0.55;     //(22)PedSig-LoCh    
  pedsvl=30;      //(23)ch-to-ch variation(%)
  pedd=150;       //(24)Ped-DyCh    
  peddv=30;       //(25)ch-to-ch variation(%)     
  pedds=0.55;     //(26)PedSig-DyCh    
  peddsv=30;      //(27)ch-to-ch variation(%)
//     
  ReadConstFiles=0;//(28)CP=CalibrMCSeeds|Peds:C=1/0->Read MSCalibFile/DB
//                                          P=1/0->ReadFromFile/ReadFromDB
}
ECMCFFKEY_DEF ECMCFFKEY;

//=============================
// ECAL Recon
//=============================
void ECREFFKEY_DEF::init(){

  reprtf[0]=0;   // (1) print_hist flag (0/1->no/yes)
  reprtf[1]=0;   // (2) print_profile flag (0/1->no/yes)
  reprtf[2]=0;   // (3) DAQ-debug prints if =1
//
  relogic[0]=1;  // (4) 1/0->write/not EcalHits into Ntuple
  relogic[1]=0;  // (5) 0/1/2/3/4/5/6->norm/RLGA/RLGA+FIAT/ANOR/PedClassic/PedDowdScaled/OnBoardPedTable_calib
  relogic[2]=0;  // (6) 0/1/2->multipl/Ebackgr/Both_type cuts in ANOR_calib logic
  relogic[3]=0;  // (7) 
  relogic[4]=0;  // (8) spare
//
// Run-time DAQ/trig/RECO-thresholds/cuts(time dependent):
  thresh[0]=3.;     // (9)  Anode(High/low-gain chan) readout threshold(in sigmas)
  thresh[1]=120.;   // (10) Etot "mip"-trig.thresh(mev tempor)
  thresh[2]=0.;     // (11) spare
  thresh[3]=0.;     // (12) spare
  thresh[4]=3.;     // (13) Dynode chan. readout thershold(in sigmas)
  thresh[5]=1.;     // (14) Trig. PMDynThr sl1(y)
  thresh[6]=70.;    // (15) Trig. PMDynThr sl2(x)
  thresh[7]=100.;   // (16) Trig. PMDynThr sl3(y)
  thresh[8]=80.;    // (17) Trig. PMDynThr sl4(x)
  thresh[9]=60.;    // (18) Trig. PMDynThr sl5(y)
  thresh[10]=40.;   // (19) Trig. PMDynThr sl6(x)
  thresh[11]=40.;   // (20) Trig. PMDynThr sl7(y)
  thresh[12]=1.;    // (21) Trig. PMDynThr sl8(x) 
  thresh[13]=1.;    // (22) Trig. PMDynThr sl9(y) 
  thresh[14]=0.;    // (23) spare
// 
  cuts[0]=1.15;   // (24) Pisa Trig-algorithm: AngleCut1 
  cuts[1]=2.15;   // (25)                      AngleCut1
  cuts[2]=5.;     // (26)                      Xmult-boundary
  cuts[3]=6.;     // (27)                      Ymult-boundary
  cuts[4]=2.;     // (28) min.layers(out of 3 per proj) with at least 1 pm having Adynode > thr
  cuts[5]=0.;     // (29)
  cuts[6]=0.;     // (30)
  cuts[7]=0.;     // (31)
  cuts[8]=0.;     // (32)
  cuts[9]=0.65;   // (33) LVL3-trig. EC-algorithm: "peak"/"average" methode boundary
//
  ReadConstFiles=100;//(34)DCP (ThreshCuts-set | Calib.const(MC/RD) / RDpeds)
//                            D=1/0-> Take from DataCards/DB
//                            C=1/0-> Take from CalibFiles/DB
//                            P=1/0-> Take from CalibFiles/DB
  Thr1DSeed=10;//(35) this and below is for Vitali's clust. algorithm
  Thr1DRSeed=0.18;
  Cl1DLeakSize=9;
  Cl1DCoreSize=2;
  Thr2DMax=1.2;  //max tan(theta)
  Length2DMin=3;
  Chi22DMax=1000;
  PosError1D=0.1;
  Chi2Change2D=0.33;
  TransShowerSize2D=10;
  SimpleRearLeak[0]=-0.015;
  SimpleRearLeak[1]=0.952e-3;
  SimpleRearLeak[2]=3.1;
  SimpleRearLeak[3]=0.9984e-3;  //  ==  [1]/(1-abs([0])*[2])
  CalorTransSize=32;
  EMDirCorrection=1.03;
  HiEnThr=1650;
  HiEnCorFac=0.225;
//  
  sec[0]=0;//53 
  sec[1]=0;//
  min[0]=0;//
  min[1]=0;//
  hour[0]=0;//
  hour[1]=0;//
  day[0]=1;//
  day[1]=1;//
  mon[0]=0;//
  mon[1]=0;//
  year[0]=101;//
  year[1]=110;//64
}
ECREFFKEY_DEF ECREFFKEY;

//=============================
void ECCAFFKEY_DEF::init(){
  // REUN-Calibration  parameters:
// RLGA/FIAT part:
  cfvers=4;     // (1) 1-999 -> vers.number for ecalcvlistNNN.dat file
  cafdir=0;     // (2) 0/1-> use official/private directory for calibr.files
  truse=1;      // (3) (1)/0-> use He4/proton tracks for calibration
  refpid=118;   // (4) ref.pm ID (SPP-> S=SupLayer, PP=PM number) 
  trmin=4.;     // (5) presel-cut on min. rigidity of the track(gv) 
  adcmin=3.;    // (6) min ADC cut for indiv. SubCell (to remove noise)
  adcpmx=1000.; // (7) max ADC cut for indiv SC to consider Plane as bad(non PunchThrough)
  ntruncl=1;    // (8) remove this number of scPlanes with highest Edep
  trxac=0.022;  // (9) TRK->EC extrapolation accuracy in X-proj(cm)
  tryac=0.019;  //(10) TRK->EC extrapolation accuracy in Y-proj............
  mscatp=1.;    //(11) EC mult.scatt. fine tuning parameter
  nortyp=0;     //(12) normaliz.type 0/1-> by crossed/fired counters
  badplmx=0;   // (13) Accept max. bad sc-planes(>2 fired sc, high sc Ed, separated sc)
  etrunmn=60.;  //(14) Min ECenergy (Etrunc in mev) to select particle(He)
  etrunmx=160.; //(15) Max ECenergy (Etrunc in mev) ......................
  nsigtrk=1.5;  //(16) Safety gap param. for crossing check(-> ~2 sigma of TRK accur.)
// ANOR part:
  pmin=3.;        // (17) presel-cut on min. mom. of the track(gev/c) 
  pmax=15.;       // (18) presel-cut on max. mom. of the track 
  scmin=3.;       // (19) min ADC for indiv. SubCell (to remove ped,noise)
  scmax=1500.;    // (20) max ADC .................. (to remove sparks,ovfl,...)
  spikmx=0;       // (21) max SC's(spikes) with ADC>max  (to remove sparks,ovfl,...)
  nhtlmx[0]=3;    // (22) max hits in 1st sc-plane (to remove early showering)
  nhtlmx[1]=4;    // (23) max hits in 2nd sc-plane (to remove early showering)
  nhtlmx[2]=4;    // (24) max hits in 3rd sc-plane (to remove early showering)
  nhtlmx[3]=5;    // (25) max hits in 4th sc-plane (to remove early showering)
  nhtlmx[4]=7;    // (26) max hits in 5th sc-plane (to remove early showering)
  nhtlmx[5]=7;    // (27) max hits in 6th sc-plane (to remove early showering)
  lmulmx=20;      // (28) max hits/sc-plane (to remove events with abn.multiplicity)
  nholmx[0]=2;    // (29) max holes(betw.fired cells) in 1st sc-plane(early show.prot)
  nholmx[1]=2;    // (30) max holes(betw.fired cells) in 2nd sc-plane(early show.prot)
  nholmx[2]=3;    // (31) max holes(betw.fired cells) in 3rd sc-plane(early show.prot)
  nholmx[3]=3;    // (32) max holes(betw.fired cells) in 4th sc-plane(early show.prot)
  nholmx[4]=3;    // (33) max holes(betw.fired cells) in 5th sc-plane(early show.prot)
  nholmx[5]=4;    // (34) max holes(betw.fired cells) in 6th sc-plane(early show.prot)
  nbplmx=0;       // (35) max bad sc-planes (with spikes or high multiplicity)
  edtmin=800.;    // (36) min Etot(mev) to remove MIP
  esleakmx=0.01;  // (37) max Eleak(side)/Etot to remove energy side leak
  ebleakmx=0.02;  // (38) max Eleak(back)/Etot
  edfrmn=300.;    // (39) min Efront(mev)
  edt2pmx=0.2;    // (40) max Etail/Epeak
  ed2momc=0.4;    // (41) Edep(EC)/Mom(TRK)-1 cut
  cog1cut=0.6;    // (42) Track-SCPlaneCOG mismatch cut(cm) for the 1st 6 SC-planes.
  scdismx[0]=0.8; // (43) max sc-track dist. to consider hit as backgroubd(pl-1) 
  scdismx[1]=1.;  // (44) max sc-track dist. to consider hit as backgroubd(pl-2) 
  scdismx[2]=1.6; // (45) max sc-track dist. to consider hit as backgroubd(pl-3)
  scdismx[3]=1.8; // (46) max sc-track dist. to consider hit as backgroubd(pl-4)
  scdismx[4]=3.;  // (47) max sc-track dist. to consider hit as backgroubd(pl-5)
  scdismx[5]=3.2; // (48) max sc-track dist. to consider hit as backgroubd(pl-6)
  scdisrs=8.;     // (49) as above for all other planes(not used really)
  b2scut[0]=0.1;  // (50) max backgr/signal energy(bound.from above) for pl-1 
  b2scut[1]=0.1;  // (51) max backgr/signal energy(bound.from above) for pl-2 
  b2scut[2]=0.2;  // (52) max backgr/signal energy(bound.from above) for pl-3 
  b2scut[3]=0.2;  // (53) max backgr/signal energy(bound.from above) for pl-4 
  b2scut[4]=0.15; // (54) max backgr/signal energy(bound.from above) for pl-5 
  b2scut[5]=0.15; // (55) max backgr/signal energy(bound.from above) for pl-6
  pedcpr=0.04;    // (56) PedCal : portion of highest amplitudes to remove for ped/sig calc
  pedoutf=2;      // (57)  --//-- outp.flag: 0/1/2-> HistosOnly/PedWr2DB+File/PedWr2File
//g.chen
  ecshswit=0;        // (58) 0/1 switch to/(not to) use shower info in ANOR calibration
  chi2dirmx=10.;     //     max chi2 of shower dir fit
  prchi2mx=5.;       //     max chi2 of shower profile fit
  trchi2mx=10.;      //     max chi2 of shower trans. fit
  eshsleakmx=0.01;   //     max Eleak(shower side)/Etot
  eshrleakmx=0.05;;  //     max Eleak(shower back)/Etot
  eshdleakmx=0.02;   //     max Eleak(shower dead)/Etot
  esholeakmx=0.1;;   //     max Eleak(shower orph)/Etot
  difsummx=0.2;      //     cut of (E_x-E_y)/(E_x+E_y)
  trentmax[0]=0.5;   //     cut of distance between track and shower entry x-proj
  trentmax[1]=0.5;   //     cut of distance between track and shower entry y-proj
  trextmax[0]=1.0;   //     cut of distance between track and shower enit x-proj
  trextmax[1]=1.0;   // (70) cut of distance between track and shower exit y-proj
}
ECCAFFKEY_DEF ECCAFFKEY;
//=============================

//=============================


//=============================
//  TDR Simulation
//=============================
void TRDMCFFKEY_DEF::init(){
  mode=-1;
  g3trd=123654;
  cor=0.68;
  alpha=0;
  beta=0;
  gain=1;
  f2i=8;
  adcoverflow=4095;
  ped=355;
  pedsig=102;
  sigma=6;
  cmn=15;
  NoiseOn=1;
  GeV2ADC=1.e8;
  Thr1R=3.5;
  sec[0]=0;
  sec[1]=0;
  min[0]=0;
  min[1]=0;
  hour[0]=0;
  hour[1]=0;
  day[0]=1;
  day[1]=1;
  mon[0]=0;
  mon[1]=0;
  year[0]=101;
  year[1]=110;
  GenerateConst=0;
  NoiseLevel=0.01;
  DeadLevel=0.015;
  multiples=0;
}
TRDMCFFKEY_DEF TRDMCFFKEY;

//=============================
void TRDFITFFKEY_DEF::init(){
  Chi2StrLine=3;
  ResCutStrLine=1;
  SearchRegStrLine=3;
  MinFitPoints=6;
  TwoSegMatch=0.1;
  MaxSegAllowed=50;
}
TRDFITFFKEY_DEF TRDFITFFKEY;

//=============================
void TRDRECFFKEY_DEF::init(){
  TRDRECFFKEY.Chi2StrLine=5;
  TRDRECFFKEY.ResCutStrLine=1;
  TRDRECFFKEY.SearchRegStrLine=3;
}
TRDRECFFKEY_DEF TRDRECFFKEY;

//=============================
void TRDCLFFKEY_DEF::init(){
  
  ADC2KeV=1.e6/TRDMCFFKEY.GeV2ADC/TRDMCFFKEY.gain;
  Thr1S=0.11;
  Thr1A=0.33;
  Thr1R=6;
  Thr1H=5.8;
  MaxHitsInCluster=3;
  LVL3FFKEY.TRDHMulThr=Thr1H/ADC2KeV*TRDMCFFKEY.f2i;
  LVL3FFKEY.TRDHMulPart=0.249;
}
TRDCLFFKEY_DEF TRDCLFFKEY;






//=============================
//   Trigger Simulation
//=============================
void TGL1FFKEY_DEF::init(){
  //
  TGL1FFKEY.trtype=-99; //(1) Requested PhysBranches(pattern) for LVL1 global OR(see datacards.doc)
  //exapmles: 1->unbTOF;2->Z>=1;4->Z>=2;8->SlowZ>=2;16->e;32->gamma;64->EC(FTE);128->ext
  // --> TOF :
  // 
  TGL1FFKEY.toflc=-99; //(2)required TOF-FastTrigger(Z>=1) LAYERS configuration
  //                     (=0/1/2/3-> accept at least ANY3of4/ALL4/ANYTopBot2of4/LUT1+Top12 layer coincidence)
  //                      Logically define(if>=0) short set of possible LUT1+LUT2 settings; "-" means
  //                      usage of exactly defined DB settings of LUT1+LUT2
  TGL1FFKEY.tofsc=-99; //(3)required TOF-FT Plane-SIDEs configuration MN
  //                                 (m(n)=0/1-> plane two-sides-AND/OR selection, n->Z>=1, m->Z>=2)
  TGL1FFKEY.toflcsz=-99;//(4)required TOF-FTZ(slowZ>=2) LAYERS configuration IJ(J=0/1/2/3-> 
  //                        ->topAND*botAND/topAND*botOR/topOR*botAND/topOR*botOR;
  //                        I=0/1 => "*"=AND/OR. "-" means usage of DB setting 
  TGL1FFKEY.toflcz=-99;//(5)required TOF-BZ(Z>=2 flag for LVL1) LAYERS configuration 
  //                     (=0/1/2-> accept at least ANY3of4/ALL4/ANYTopBot2of4 layer coincidence)
  //                      Logically define(if>=0) short set of possible LUT-BZ settings; "-" means
  //                      usage of exactly defined DB settings of LUT-BZ
  TGL1FFKEY.tofextwid=-99;//(6) ext.width(ns,top&bot) in SlowZ>=2 logic(if <0 => take indiv.values from DB)
  // 
  // --> ANTI :
  TGL1FFKEY.antismx[0]=-99; //(7) max. acceptable fired ANTI-paddles(logical, in equat.region)
  TGL1FFKEY.antismx[1]=-99; //(8) max. acceptable fired ANTI-paddles(logical, in polar.region)
  TGL1FFKEY.antisc=-99;     //(9) required ANTI-trigger SIDE configuration
  //                                 (=0/1-> two-sides-AND/OR selection)
  // --> FastTrigger :
  TGL1FFKEY.cftmask=-99;//(10) GlobalFTMask(i|j|k=>FTE|FTZ>=2|FTC>=1; i(j,k)=1(active)/0(disabled))
  //
  // For all above cards "-99" means usage of more detailed and up-to-date DB-settings
  //   To use manual(through data-card) control put necessary >=0 value !!! 
  //
  TGL1FFKEY.RebuildLVL1=0;//(11) (for RealData) rebuil lvl1-obj from subdet.RawEvent objects
  // 
  TGL1FFKEY.MaxScalersRate=20000;//(12)
  TGL1FFKEY.MinLifeTime=0.015;//(13)
  //
  // --> orbit:
  TGL1FFKEY.TheMagCut=0.7;// (14)geom.latitude cut for anti-cut selection(below-#5, above-#6)
  //
  // --> Ecal
  TGL1FFKEY.ecorand=-99;   //(15) 1/2=>OR/END of 2 proj. requirements on min. layer multiplicity
  TGL1FFKEY.ecprjmask=-99; //(16) proj.mask(lkji: ij=1/0->XYproj active/disabled in FTE; kl=same for LVL1(angle)
  //
  TGL1FFKEY.Lvl1ConfMCVers=1;//(17)MC def.version number of "lvl1conf***mc" -file
  TGL1FFKEY.Lvl1ConfRDVers=1;//(18)RD def.version number of "lvl1conf***rl" -file
  TGL1FFKEY.Lvl1ConfRead=0;  //(19) MN, N=0/1->read Lvl1Config-data from DB/raw_file
  //                                      M=0/1->read Lvl1Config raw-file from official/private dir
  TGL1FFKEY.printfl=0;      // (20) PrintControl=0/n>0 -> noPrint/PrintWithPriority(n=1 ->max)
  //
  TGL1FFKEY.sec[0]=0;//(21) 
  TGL1FFKEY.sec[1]=0;
  TGL1FFKEY.min[0]=0;
  TGL1FFKEY.min[1]=0;
  TGL1FFKEY.hour[0]=0;
  TGL1FFKEY.hour[1]=0;
  TGL1FFKEY.day[0]=1;
  TGL1FFKEY.day[1]=1;
  TGL1FFKEY.mon[0]=0;
  TGL1FFKEY.mon[1]=0;
  TGL1FFKEY.year[0]=101;
  TGL1FFKEY.year[1]=110;//(32)
  //
}
TGL1FFKEY_DEF TGL1FFKEY;
//=============================


void LVL3FFKEY_DEF::init(){
  MinTOFPlanesFired=3;
  UseTightTOF=0;
  TrTOFSearchReg=6.;
  TrMinResidual=0.03;
  TrMaxResidual[0]=-1.2;
  TrMaxResidual[1]=-0.2;
  TrMaxResidual[2]=-0.3;
  TrMaxHits=30;
  Splitting=0.04;
  NRep=1;
  Accept=0;
  RebuildLVL3=2;
  NoK=1;
  TrHeavyIonThr=200;
  SeedThr=-32;
  Stat=0;
}
LVL3FFKEY_DEF LVL3FFKEY;
//=============================

void LVL3SIMFFKEY_DEF::init(){
  NoiseProb[1]=2.e-10;
  NoiseProb[0]=NoiseProb[1]*1.65;
}
LVL3SIMFFKEY_DEF LVL3SIMFFKEY;
//=============================

void LVL3EXPFFKEY_DEF::init(){
  NEvents=2048;
  ToBad=10;
  TryAgain=10;
  Range[0][0]=2.5;
  Range[0][1]=2.;
  Range[1][0]=1000000;
  Range[1][1]=3;
  Range[2][0]=2.;
  Range[2][1]=2.5;
}
LVL3EXPFFKEY_DEF LVL3EXPFFKEY;
//=============================




//=============================
//  RICH Simulation
//=============================
void RICGEOM_DEF::init(){
//
// Geometry defaults: unused
  /*************************************************
  RICGEOM.top_radius=RICHDB::top_radius;
  RICGEOM.bottom_radius=RICHDB::bottom_radius;
  RICGEOM.hole_radius=RICHDB::hole_radius;
  RICGEOM.height=RICHDB::rich_height;
  RICGEOM.radiator_radius=RICHDB::rad_radius;
  RICGEOM.radiator_height=RICHDB::rad_height;
  RICGEOM.radiator_box_length=RICHDB::rad_length;
  RICGEOM.light_guides_height=RICHDB::lg_height;
  RICGEOM.light_guides_length=RICHDB::lg_length;
  *******************************************************/
}
RICGEOM_DEF RICGEOM;


//=============================
void RICCONTROLFFKEY_DEF::init(){
  RICCONTROLFFKEY.iflgk_flag=0;  // This should be always zero
  RICCONTROLFFKEY.tsplit=0;      // Bo time splitting at the end 
}
RICCONTROLFFKEY_DEF RICCONTROLFFKEY;

//=============================
void RICFFKEY_DEF::init(){
  ReadFile=0;
  sec[0]=0;
  sec[1]=0;
  min[0]=0;
  min[1]=0;
  hour[0]=0;
  hour[1]=0;
  day[0]=1;
  day[1]=1;
  mon[0]=0;
  mon[1]=0;
  year[0]=101;
  year[1]=110;
}
RICFFKEY_DEF RICFFKEY;

//=============================
void RICRECFFKEY_DEF::init(){
  recon[0]=11;   //beta reconstruction
  recon[1]=1;   //Z reconstruction
}
RICRECFFKEY_DEF RICRECFFKEY;

//=============================
void RICRADSETUPFFKEY_DEF::init(){
  setup=1;
}
RICRADSETUPFFKEY_DEF RICRADSETUPFFKEY;




//=============================
// MAGNET Simulation
//=============================
void MAGSFFKEY_DEF::init(){
  magstat=1;    //(1) -1/0/1->warm/cold_OFF/cold_ON 
  fscale=1.;    //(2) rescale factor (wrt nominal field) (if any) 
  ecutge=0.001; //(3) e/g ener.cut for tracking in magnet materials(Gev) 
}
MAGSFFKEY_DEF MAGSFFKEY;

//=============================
//  Magnet Recon
//=============================
void  TKFIELD_DEF::init(){
  iniok=1;
  VBLANK(mfile,40);
  isec[0]=0;
  isec[1]=0;
  imin[0]=0;
  imin[1]=0;
  ihour[0]=0;
  ihour[1]=0;
  imon[0]=0;
  imon[1]=0;
  iyear[0]=0;
  iyear[1]=0;
}
//TKFIELD_DEF TKFIELD;


void DAQCFFKEY_DEF::init(){
  mode=0;
  OldFormat=0;
  LCrateinDAQ=1;
  SCrateinDAQ=-1;
  NoRecAtAll=0;
  TrFormatInDAQ=3;
  VBLANK(ifile,40);
  VBLANK(ofile,40);
}
DAQCFFKEY_DEF DAQCFFKEY;

//============================
//   Hi level recon
//============================

void CHARGEFITFFKEY_DEF::init(){
  // Fit beta & charge
  NmembMax=3;
  Tracker=1;
  EtaMin[0]=0.05;
  EtaMin[1]=0.00;
  EtaMax[0]=0.95;
  EtaMax[1]=1.00;
  ProbTrkRefit=0.01;
  ResCut[0]=4.;//-1. to switch OFF incomp.clus.exclusion(as for simulation default,
  ResCut[1]=4.;//  see charge.C code with tempor. MC settings
  SigMin=0.1;
  SigMax=0.3;
  PdfNorm=1;//not used now (pdf's are normalized automatically)
  TrMeanRes=1;//(13) use normal(0)/"-incomp.hit"(1)truncated mean
  ProbMin=0.0001;
  TrackerOnly=9;//above this value use Tracker only
  ChrgMaxAnode=9;//not used now
  BetaPowAnode=0;//not used now
  TrackerForceSK=0;//(18)
  TrackerKSRatio=0.67;//(19)
  TrackerProbOnly=9;
  TrkPDFileMCVers=1;//MC vers.number of Trk-ElosPDFFile(trkpdffNNNmc.dat)
  TrkPDFileRDVers=1;//RD vers.number of Trk-ElosPDFFile(trkpdffNNNrl.dat)
  TrkPDFileRead=0;//read TrkPDF-info from DB(0) OR RawFile (1)
  sec[0]=0; 
  sec[1]=0;
  min[0]=0;
  min[1]=0;
  hour[0]=0;
  hour[1]=0;
  day[0]=1;
  day[1]=1;
  mon[0]=0;
  mon[1]=0;
  year[0]=101;
  year[1]=110;
}
CHARGEFITFFKEY_DEF CHARGEFITFFKEY;
//=================================
void BETAFITFFKEY_DEF::init(){
  pattern[0]=1;
  pattern[1]=1;
  pattern[2]=1;
  pattern[3]=1;
  pattern[4]=1;
  pattern[5]=1;
  pattern[6]=1;
  pattern[7]=1;
  pattern[8]=1;
  Chi2=50;
  Chi2S=15;
  SearchReg[0]=3.;
  SearchReg[1]=3.;
  SearchReg[2]=3.;
  LowBetaThr=0.4;
  FullReco=0;
  MassFromBetaRaw=1;
}
BETAFITFFKEY_DEF BETAFITFFKEY;
