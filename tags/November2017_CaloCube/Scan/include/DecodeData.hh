#ifndef DecodeData_h
#define DecodeData_h
#include "Event.hh"
#include "TH1F.h"
#include <cstdio>

#define TDRNUM 24
#define NJINF 2

typedef struct calib{
  float ped[1024];
  float rsig[1024];
  float sig[1024];
  int status[1024];

} calib;


typedef struct header {
  int run;    // run number
  char date[50];        // date
  double gonpar[4];// goniometer parameters
  unsigned int refmaskjj;
  unsigned int refmask[24];
} header;




class DecodeData {

 private:
  FILE * rawfile;
  char rawname[300];
  char rawdir[300];
  char rawCaldir[300];
  int Xstant;
  int pri;
  int evpri;
  int runn;
  int tdroffset;
  calib cals[TDRNUM];

  int nJinf;
  int JinfMap[NJINF];
  int ntdrRaw;
  int tdrRaw[TDRNUM];
  int ntdrCmp;
  int tdrCmp[TDRNUM];
  int out_flag;
  char type[10]; //JinF or JinJ
void FindCalibs ();
  int  ReadCalib(FILE * fil,calib* cal);
  int FindPos(int tdrnum);
  int FindPosRaw(int tdrnum);
  int FindCalPos(int tdrnum);
  int ReadFile(void *ptr, size_t size, size_t nitems,
		       FILE * stream);
  void mysort(int* aa,int nel);
public:
  Event * ev;
  RHClass *rh;

  int evenum;

  void DumpRunHeader();

 public:
  DecodeData( char * ifname,char* caldir,int run);
  ~DecodeData();
  
  int SkipOneEvent(int evskip=1);
  int ReadOneEvent();
  int ReadOneTDR(int jinf=0);
  int ReadOneJINF();
  void CloseFile();
  int EndOfFile();
  void SetPrintOff(){pri=0;}
  void SetPrintOn(){pri=1;}
  void SetEvPrintOff(){evpri=0;}
  void SetEvPrintOn(){evpri=1;}
};

#endif
