#ifndef TrigClass_h
#define TrigClass_h

#include "PUtil.h"
#include "JLV1.h"

class TrigClass{

private:
	JLV1* _PJLV1;
	
public:

  TrigClass(JLV1* pointer=0):_PJLV1(pointer){}
	~TrigClass(){ _PJLV1=0;}

  JLV1* GetPointer(){return _PJLV1;}
	void SetPointer(JLV1* pointer){_PJLV1 = pointer;}

  int	ReadCounter();
  int ResetCounter();
  int TriggerOn();
  int TriggerOff();
  int BeamTrig();
  int CalibTriggerOn();
	int CalibTriggerOff();

};

#endif
