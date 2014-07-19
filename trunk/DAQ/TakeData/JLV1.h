#ifndef JLV1_H
#define JLV1_H
#include "amswcom.h"

#include "JinjSlave.h"


class JLV1 : public JinjSlave{

private:

public:
  JLV1(char* name,char* conf_file,int address,AMSWcom* node);
  ~JLV1();
  int Init();
  int SetMode(int modein);
  int GetEventNumber();
  char* PrintAllEventNumber(int log=1,int JLV1num=0);
  int EventReset();
  int SetDelay();
  int OverWriteDelay(u_short del);
  int PulseTrigger();
  int TriggerOn();
  int TriggerOff();
  int CalibTriggerOn();
  int Calib2PassTriggerOn(int dtime);
  int CalibTriggerOff();
  int WriteBusyMask(ushort busy0x12=0x0000, ushort busy0x13=0x0000, ushort busy0x14=0x0000);

};

#endif
