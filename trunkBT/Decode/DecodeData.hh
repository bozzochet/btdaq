#ifndef DecodeData_h
#define DecodeData_h
#include "Event.hh"
#include "TH1F.h"
#include "TH2F.h"
#include <cstdio>

typedef struct calib{
  float ped[1024];
  float rsig[1024];
  float sig[1024];
  int status[1024];

} calib;

#pragma pack(push, 1)

//typedef struct header { // gcc 4.3, considers 'typedef' useless // (what??)
struct header {			//for file writing NOT in AMSBlock
  int run;    			// run number
  char date[50];        	// date
  double gonpar[4];		// goniometer parameters
  unsigned int refmaskjj;//16/08/2014 - On Mac this is seen as long 8 (instead of 4) and the reader is read wrongly
  unsigned int refmask[24];
};

#pragma pack(pop)

class DecodeData {
  
private:
  FILE* rawfile;
  char rawname[300];
  char rawdir[300];
  char rawCaldir[300];
  //  int Xstant;
  int runn;
  int tdroffset;
  calib cals[NJINF*NTDRS];
  int pri;
  int evpri;
  int nJinf;
  int JinfMap[NJINF];
  int ntdrRaw;
  int tdrRaw[NJINF*NTDRS];
  int ntdrCmp;
  int tdrCmp[NJINF*NTDRS];
  int out_flag;
  char type[10]; //JinF or JinJ
  void FindCalibs ();
  int  ReadCalib(FILE * fil,calib* cal);
  int FindPos(int tdrnum);
  int FindPosRaw(int tdrnum);
  int FindCalPos(int tdrnum);
  int ReadFile(void *ptr, size_t size, size_t nitems, FILE * stream);
  void mysort(int* aa,int nel);

  bool kMC;
  
public:
  Event * ev;
  RHClass *rh;
  
  int evenum;
  TH1F* hocc[NJINF*NTDRS];
  TH1F* hoccseed[NJINF*NTDRS];
  TH1F* hcharge[NJINF*NTDRS][2];
  TH1F* hsignal[NJINF*NTDRS][2];
  TH2F* hchargevsocc[NJINF*NTDRS];
  TH2F* hsignalvsocc[NJINF*NTDRS];
  TH1F* hson[NJINF*NTDRS][2];

  double shighthreshold;
  double slowthreshold;
  double khighthreshold;
  double klowthreshold;

  bool kClusterize;
  int cworkaround;
  
  void DumpRunHeader();
  
public:
  DecodeData(char * ifname, char* caldir, int run, int ancillary, bool _kMC=false);
  ~DecodeData();

  //generic
  void AddCluster(int numnum, int Jinfnum, int clusadd, int cluslen, int Sig2NoiStatus, int CNStatus, int PowBits, int bad, float* sig);
  void Clusterize(int numnum, int Jinfnum, calib* cal);
  //moved to Event class
  //  double ComputeCN(int size, short int* Signal, float* pede, float* SoN, double threshold=3.0);

  int GetNTDRRaw() { return ntdrRaw;}
  int GetNTDRCmp() { return ntdrCmp;}
  int GetIdTDRRaw(int pos);
  int GetIdTDRCmp(int pos);
  
  int SkipOneEvent(int evskip=1);
  int ReadOneEvent();

  void OpenFile(char* ifname, char* caldir, int run, int ancillary);
  void CloseFile();
  int EndOfFile();

  //data
  void OpenFile_data(char* ifname, char* caldir, int run, int ancillary);
  int ReadOneEvent_data();
  int ReadOneTDR(int jinf=0);
  int ReadOneJINF();
  int SkipOneEvent_data(int evskip=1);
  
  //mc
  void OpenFile_mc(char* ifname, char* caldir, int run, int ancillary);
  int ReadOneEvent_mc();
  int SkipOneEvent_mc(int evskip=1);
  
  void SetPrintOff(){pri=0;}
  void SetPrintOn(){pri=1;}
  void SetEvPrintOff(){evpri=0;}
  void SetEvPrintOn(){evpri=1;}
};

#endif
