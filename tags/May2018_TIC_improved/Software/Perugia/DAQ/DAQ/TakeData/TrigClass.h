#ifndef TrigClass_h
#define TrigClass_h

#include "PUtil.h"
#include "JLV1.h"//JLV1 case
#include "CommClass.hh"//NI-USB case

/* C++98 <-> C++11 portability */
#if __cplusplus > 199711
#define STATIC_CONST static constexpr
#else
#define STATIC_CONST static const
#endif

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
  TrigClass(char* address, int port);

  int ReadCounter();
  int ResetCounter();
  int TriggerOn(bool apply=true, bool delay=false);
  int TriggerOff(bool apply=true, bool delay=false);
  int CalibTriggerOn(bool apply=true, bool delay=false);
  int CalibTriggerOff(bool apply=true, bool delay=false);

  //only NI-USB case
  int ReadInput();
  int Init();

  STATIC_CONST int wait_nsec=3; //s
  STATIC_CONST int ntry=60; //attempt one per s
};

#endif
