#ifndef DecodeData_h
#define DecodeData_h
#include "Event.hh"
#include "TH1F.h"
#include <cstdio>

#define TDRNUM 24
#define NJINF 1

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
  int Xstant;
  int runn;
  int tdroffset;
  calib cals[NJINF*TDRNUM];
  int pri;
  int evpri;
  int nJinf;
  int JinfMap[NJINF];
  int ntdrRaw;
  int tdrRaw[NJINF*TDRNUM];
  int ntdrCmp;
  int tdrCmp[NJINF*TDRNUM];
  int out_flag;
  char type[10]; //JinF or JinJ
  void FindCalibs ();
  int  ReadCalib(FILE * fil,calib* cal);
  int FindPos(int tdrnum);
  int FindPosRaw(int tdrnum);
  int FindCalPos(int tdrnum);
  int ReadFile(void *ptr, size_t size, size_t nitems, FILE * stream);
  void mysort(int* aa,int nel);
public:
  Event * ev;
  RHClass *rh;
  
  int evenum;
  TH1F* hmio[NJINF*TDRNUM];
  
  void DumpRunHeader();
  
public:
  DecodeData( char * ifname,char* caldir,int run,int ancillary);
  ~DecodeData();
  
  int SkipOneEvent(int evskip=1);
  int ReadOneEvent();
  int ReadOneTDR(int jinf=0);
  int ReadOneJINF();
  void AddCluster(int numnum, int Jinfnum, int clusadd, int cluslen, int Sig2NoiStatus, int CNStatus, int PowBits, int bad, float* sig);
  void Clusterize(int numnum, int Jinfnum, short int Signal[1024]);

  void CloseFile();
  int EndOfFile();
  void SetPrintOff(){pri=0;}
  void SetPrintOn(){pri=1;}
  void SetEvPrintOff(){evpri=0;}
  void SetEvPrintOn(){evpri=1;}
};

#endif
