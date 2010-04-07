#ifndef JINF_H
#define JINF_H
#include "amswcom.h"

#define NTDRS 24 //maximum number of TDRs

typedef unsigned short u_short;

typedef struct datades {
  int size[NTDRS];
  int start[NTDRS];
  int mode[NTDRS]; // 1=raw, 2=red, 3=mix
  int error[NTDRS];
} datades;


class ConfPars{

public:
  u_short JINFflash;
  u_short TDRflash; 
  u_short delay; 
  char DATAPATH[255];
  char CALPATH[255];
  int Ntdrs;
  u_short mode[NTDRS];
  unsigned int refmask;
  ConfPars();
  int ReadConfig(char* filename);
};


class Jinf{

public:
  // FILE * logfile;
  char myname[20];
  int broadcastadd;
  int  selfaddress;
  AMSWcom *node;
  ConfPars *CPars;
  SlaveMask mask;
  int jinfevents;
  int tdrevents[24];

private:
  datades descr;
  int GenAddress(int tdrnum);

public:
  Jinf(char* name,char* conf_file,int address,AMSWcom* node);
  ~Jinf();
  int Init();
  int ReadSlaveMask();
  int SetModeJinf(int modein);
  int SetModeTdrs(int modein=-1);
  int GetEventNumber();
  char* PrintAllEventNumber(int log=1,int Jinfnum=0);
  int EventReset();
  int SaveCalibrations(int run_number,int Jinfnum,char* addprefix);
  int SaveOneCalibration(unsigned int tdr_number, char *calfileprefix);
  int CalIsDone();
  void ExtractEvent(int offset, int datasize);
  void CheckEvent(int evtnum);
  int ShowConnect(SlaveMask mask);
  int SetDelay();
  int OverWriteDelay(u_short del);
};





int ShowConnect(SlaveMask mask);
#endif
