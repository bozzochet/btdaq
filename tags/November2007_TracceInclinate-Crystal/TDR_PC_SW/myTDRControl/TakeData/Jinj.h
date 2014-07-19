#ifndef JINJ_H
#define JINJ_H
#include "amswcom.h"
#include "Jinf.h"

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
  int ReadConfig(char* filename);
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
  Jinf* Slave[24];
  int SlaveAdd[24];
private:
  datades descr;
  int GenAddress(int tdrnum);
  Jinf* Jinj::GetSlavePointer(int id);

public:
  Jinj(char* name,char* conf_file,int address,AMSWcom* node);
  ~Jinj();
  int Init();
  int ReadSlaveMask();
  int EventReset();
  int SetModeJinj(int modein);
  int SetModeSlaves(int jinfmode=1,int tdrmode=-1);
  int GetEventNumber();
  char* PrintAllEventNumber(int log=0);
  void ExtractEvent(int offset,int datasize);
  void CheckEvent(int evtnum);
  int ShowConnect(SlaveMask mask);
  int SaveCalWhenDone(int run_number,char *addprefix);
	int SetDelay();
};


#endif
