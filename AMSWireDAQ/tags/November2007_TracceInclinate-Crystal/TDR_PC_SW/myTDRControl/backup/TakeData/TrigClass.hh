#ifndef TrigClass_hh
#define TrigClass_hh

#include "CommClass.hh"

class TrigClass: public CommClass {

private:

  int number1;
  int number2;
  int number3;
  int counter;
  int Read(int noclose=0);
  int   Write(int bit2modify,int value2write);

public:

  TrigClass(char* address="localhost",int port=6666);
  virtual ~TrigClass(){};

  int   ReadCounter();
  void  ResetCounter();
  int   ReadInput();
  int   TriggerOn();
  int   TriggerOff();
  int   BeamTrig();
  int   CalibTrig();

};

#endif
