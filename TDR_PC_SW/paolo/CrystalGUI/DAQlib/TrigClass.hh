#ifndef TrigClass_hh
#define TrigClass_hh

#include "CommClass.hh"

class TrigClass: public CommClass {

private:

  int number1;
	int number2;
	int number3;
	int counter;

public:

  TrigClass(char* address="localhost",int port=6666);
  virtual ~TrigClass(){};

	int TrigClass::Write(int bit2modify,int value2write);
	int TrigClass::ReadCounter();
	int TrigClass::ReadInput();
	int TrigClass::TriggerOn();
	int TrigClass::TriggerOff();
	int TrigClass::BeamTrig();
	int TrigClass::CalibTrig();
	int IsTriggerRunning(){return 1;}

};

#endif
