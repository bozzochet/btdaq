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
  int TriggerOn();
  int TriggerOff();
  int CalibTriggerOn();
  int CalibTriggerOff();
  
};

#endif
