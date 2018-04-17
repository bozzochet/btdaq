#ifndef JinjSlave_H
#define JinjSlave_H
#include "amswcom.h"

typedef unsigned short u_short;

#define NTDRS 24 //maximum number of TDRs (for Jinfs)
#define d_jinjslave_ip_size 16
#define d_jinjslave_port_size 6


class ConfPars{
 public:
  int type;
  u_short JLV1flash;
  u_short JINFflash;
  u_short TDRflash; 
  u_short delay; 
  char DATAPATH[255];
  char CALPATH[255];
  char REMOTEPATH[255];
  char dampeIP[d_jinjslave_ip_size], dampePORT[d_jinjslave_port_size];
  int Ntdrs;
  u_short mode[NTDRS];
  unsigned int refmask;
  int ReadConfig(char* filename);
  ConfPars();
  int SLowTrash[NTDRS];
  int SHighTrash[NTDRS];
  int KLowTrash[NTDRS];
  int KHighTrash[NTDRS];
  int SigmaRowTrash[NTDRS];
  int OccupancyThresh[NTDRS];
};


class JinjSlave{
 public:
  
  // FILE * logfile;
  char myname[20], myaction[20];
  int broadcastadd;
  int  selfaddress;
  int timestamp;
  AMSWcom *node;
  SlaveMask mask;
  JinjSlave(char* name,char* conf_file,int address,AMSWcom* node);
  ~JinjSlave();
  ConfPars *CPars;
  int jinfevents[2];
  int tdrevents[2][24];
  int jlv1events;
  virtual int Init(){};
  virtual int SetDelay(){};
  virtual int SetMode(int jinjslavemode){};
  virtual int GetEventNumber(){};
  virtual char* PrintAllEventNumber(int log=1,int jinjslavenum=0){};
  virtual int EventReset(){};
 private:
  
};

#endif
