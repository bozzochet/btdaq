#ifndef NIDAQCLASS_H
#define NIDAQCLASS_H

#include "NIDAQmxBase.h"

class NIDaqClass{
  
private:
  TaskHandle  taskHandleDO_0;
  TaskHandle  taskHandleDO_1;
  TaskHandle  taskHandleDI;
  TaskHandle  taskHandleC;
  char chan1[120];
  char chan2[120];
  char chan3[120];
  uInt32  r_dataDI [2];
  uInt32 num_portsDI ;

  uInt32 Counts;

  uInt32  r_dataDO_0 [2];
  uInt32  r_dataDO_1 [2];
  uInt32 num_portsDO_0 ;
  uInt32 num_portsDO_1 ;
  int debug;
  int Stop();



public:
  NIDaqClass();
  ~NIDaqClass();
  int Init();
  void ProcessError(int error);
  void Print();
  char* PrintString(char *pp);
  int Read();
  int Write(uInt32  a, uInt32 b);
  int ResetCounter();
};
#endif
