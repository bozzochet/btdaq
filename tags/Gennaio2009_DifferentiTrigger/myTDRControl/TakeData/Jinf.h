#ifndef JINF_H
#define JINF_H
#include "amswcom.h"

#include "JinjSlave.h"


class Jinf : public JinjSlave{

public:
  Jinf(char* name,char* conf_file,int address,AMSWcom* node);
  ~Jinf();
  int Init();
  int ReadSlaveMask();
  int SetMode(int modein);
  int SetModeTdrs(int modein=-1);
  int GetEventNumber();
  char* PrintAllEventNumber(int log=1,int Jinfnum=0);
  int EventReset();
  int ShowConnect(SlaveMask mask);
  int SetDelay();
  int OverWriteDelay(u_short del);
	int GenAddress(int tdrnum);
	int WriteCalibPar(AMSWcom* node, int tdrnum);
	int ReadCalibPar(AMSWcom* node, int tdrnum);
};

#endif
