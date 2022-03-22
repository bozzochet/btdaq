#ifndef TrigClass_h
#define TrigClass_h

#include "PUtil.h"
#include "JLV1.h"//JLV1 case
#include "CommClass.hh"//NI-USB case

class TrigClass: public CommClass {

private:
  //JLV1 case
  JLV1* _PJLV1;

  //NI-USB case
  int number1;
  int number2;
  int number3;
  int counter;
  int fake;
  int Read(int noclose=0);
  int Write(int bit2modify,int value2write);
	
public:

  //JLV1 case & fake case
  TrigClass(JLV1* pointer=0);
  ~TrigClass(){ _PJLV1=0;}

  JLV1* GetPointer(){return _PJLV1;}
  void SetPointer(JLV1* pointer){_PJLV1 = pointer;}

  //NI-USB case
  TrigClass(const char* address, int port);

  int ReadCounter();
  int ResetCounter();
  int TriggerOn();
  int TriggerOff();
  int CalibTriggerOn();
  int CalibTriggerOff();

  //only NI-USB case
  int ReadInput();
  int Init();

};

#endif
