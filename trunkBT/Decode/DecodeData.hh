#ifndef DecodeData_h
#define DecodeData_h
#include "Event.hh"
#include "TBranch.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include <cstdio>
#include <vector>

//(could be defined by Makefile)
//#define CALOCUBE

// TODO:
// template <size_t NCh> class calib {
// public:
//  float ped[NCh];
//  float rsig[NCh];
//  float sig[NCh];
//  int status[NCh];
//};

class calib {
public:
  float *ped;
  float *rsig;
  float *sig;
  int *status;

  calib() {
    ped = new float[Event::NVAS * Event::NCHAVA];  // was [1024]: is backward compatible?
    rsig = new float[Event::NVAS * Event::NCHAVA]; // was [1024]: is backward compatible?
    sig = new float[Event::NVAS * Event::NCHAVA];  // was [1024]: is backward compatible?
    status = new int[Event::NVAS * Event::NCHAVA]; // was [1024]: is backward compatible?
  };
};

#pragma pack(push, 1)

// typedef struct header { // gcc 4.3, considers 'typedef' useless // (what??)
struct header {  // for file writing NOT in AMSBlock
  int run;       // run number
  char date[50]; // date // why we put 50! In the RHClass is 30! Should be shorter, but anyhow it will be truncated when
                 // passed to RHClass
  double gonpar[4];       // goniometer parameters
  unsigned int refmaskjj; // 16/08/2014 - On Mac this is seen as long 8 (instead of 4) and the reader is read wrongly
  unsigned int refmask[24];
};

#pragma pack(pop)

#pragma pack(push, 1)

struct wholeheader {
  //---- Primary and secondary header ---------------------//
  unsigned short SIZE;
  unsigned short RRRWNODETYPE;
  unsigned short FBITAG;
  unsigned short TIMEMSB;
  unsigned short TIMELSB;
  //---- JMDC data block ----------------------------------//
  unsigned short JMDCSIZE;
  unsigned short JMDCRRRWNODETYPE;

  unsigned short RUNNUMMSB;
  unsigned short RUNNUMLSB;
  unsigned short RUNTAGMSB;
  unsigned short RUNTAGLSB;
  unsigned short EVTNUMMSB;
  unsigned short EVTNUMLSB;
  unsigned short JMDCTIMEMSB;
  unsigned short JMDCTIMELSB;
  unsigned short JMDCTIMEFINEMSB;
  unsigned short JMDCTIMEFINELSB;
  unsigned short GReservedGroups;
  //---- DSP Slave Format ----------------------------------//
  unsigned short DSPSIZE;
  unsigned short DSPRRRWNODETYPE;
};

#pragma pack(pop)

class DecodeData {

protected:
  FILE *rawfile;

  int runn;
  calib *cals;
  bool pri = false;
  bool evpri = false;
  int ntdrRaw = 0;
  int ntdrCmp = 0;
  laddernumtype *tdrMap;
  int *tdrAlign; // added by Viviana, credo...
  double m_adcUnits = 8.0;
  unsigned int m_defaultShift = 640;
  unsigned int m_defaultArraySize = 384;

  int FindPos(int tdrnum);
  int FindCalPos(int tdrnum);
  int ReadFile(void *ptr, size_t size, size_t nitems, FILE *stream);

  bool kMC = false;

public:
  Event *ev;
  // TODO: put in DecodeDataAMS
  RHClass *rh;

  TH1F **hocc;
  TH1F **hoccseed;
  TH1F ***hcharge;
  TH1F ***hsignal;
  TH2F **hchargevsocc;
  TH2F **hsignalvsocc;
  TH1F ***hson;

  double shighthreshold;
  double slowthreshold;
  double khighthreshold;
  double klowthreshold;

  bool kClusterize;
  int cworkaround;

  virtual void DumpRunHeader() = 0;

public:
  DecodeData() = default;
  virtual ~DecodeData() = default;

  // generic
  void AddCluster(int numnum, int Jinfnum, int clusadd, int cluslen, int Sig2NoiStatus, int CNStatus, int PowBits,
                  int bad, float *sig, bool kRaw = false);
  virtual void Clusterize(int numnum, int Jinfnum, calib *cal);

  virtual inline int GetNTdrRaw() { return ntdrRaw; }
  virtual inline int GetNTdrCmp() { return ntdrCmp; }
  virtual int GetTdrNum(int pos) = 0;
  virtual int GetTdrType(int pos) = 0;

  virtual int SkipOneEvent(int evskip = 1) = 0;
  virtual int ReadOneEvent() = 0;

  virtual void OpenFile(const char *ifname, const char *caldir, int run, int ancillary) = 0;
  virtual void CloseFile();
  virtual int EndOfFile();

  // mc
  void SetPrintOff() { pri = 0; }
  void SetPrintOn() { pri = 1; }
  void SetEvPrintOff() { evpri = 0; }
  void SetEvPrintOn() { evpri = 1; }

  virtual TTree *GetMCTruth() { return nullptr; }; // CB
};

#endif
