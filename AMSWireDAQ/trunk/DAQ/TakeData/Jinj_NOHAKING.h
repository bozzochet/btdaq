#ifndef JINJ_H
#define JINJ_H
#include "amswcom.h"
#include "Jinf.h"
#include "JLV1.h"
#include "JinjSlave.h"

#define NSLAVE 24 //maximum number of Slaves


class ConfParsJ{

public:
  u_short JINJflash;
  char DATAPATH[255];
  char CALPATH[255];
  int Nslave;
  u_short mode[NSLAVE];
  char SlaveConfFile[24][100];
  unsigned int refmask;
  ConfParsJ();
  ~ConfParsJ(){}
  int ReadConfig(const char* filename); // gcc 4.3: added 'const'
};


class Jinj{

public:
  char myname[20];
  int broadcastadd;
  int  selfaddress;
  AMSWcom *node;
  ConfParsJ *CPars;
  SlaveMask mask;
  int jinjevents;
  int jinfevents[24];
  int NSlave;
  JinjSlave* Slave[24];
  int SlaveAdd[24];
  Jinj(const char* name,const char* conf_file,int address,AMSWcom* node); // gcc 4.3: added 'const'
  ~Jinj();
  int SelfInit();
	int InitJinfs();
  int ReadSlaveMask();
  int EventReset();
  int SetModeJinj(int modein);
  int SetModeSlaves(int jinjslavemode=1,int tdrmode=-1);
  int GetEventNumber();
  char* PrintAllEventNumber(int log=0);
  int ShowConnect(SlaveMask mask);
	int SetDelay();
	JLV1* GetJLV1Pointer();
	JinjSlave* GetSlavePointer(int id);
  int GenAddress(int tdrnum);
  int SwitchOffNode(int jinjport);
};


#endif
