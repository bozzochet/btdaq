#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstring>
#include <stdio.h>

#include "TrigClass.h"


int TrigClass::ReadCounter(){
  int ret=0;
	if(!_PJLV1) {
		PRINTF("This is no real Trigger Class, so there's no counter...\n"); 
		return -9999;
	}
	else ret=_PJLV1->GetEventNumber();
	return ret;
}

int TrigClass::ResetCounter(){
  int ret=0;
	if(!_PJLV1) {
		PRINTF("This is no real Trigger Class, so there's no counter...\n");
		return ret;
	}
  else ret=_PJLV1->EventReset();
	return ret;
}

int TrigClass::TriggerOn(){
  int ret=0;
	if(!_PJLV1) {
		char reply;
		PRINTF("Please turn on the trigger production and then press RETURN!\n");
		reply = getchar();
		return ret;
	}
 	else ret=_PJLV1-> TriggerOn();
  return ret;
}

int TrigClass::TriggerOff(){
  int ret=0;
	if(!_PJLV1) {
		char reply;
		PRINTF("Please turn off the trigger production and then press RETURN!\n");
		reply = getchar(); 
		return ret;
	}
	else ret=_PJLV1-> TriggerOff();
  return ret;
}

int TrigClass::CalibTriggerOn(){
  int ret=0;
	if(!_PJLV1) {
		char reply;
		PRINTF("Please turn on calibration trigger and then press RETURN!\n");
		reply = getchar();
		return ret;
	}
  else ret=_PJLV1-> CalibTriggerOn();
  return ret;
}

int TrigClass::CalibTriggerOff(){
  int ret=0;
	if(!_PJLV1) {
		char reply;
		PRINTF("Please turn off calibration trigger and then press RETURN!\n");
		reply = getchar();
		return ret;
	}
  else ret=_PJLV1-> CalibTriggerOff();
  return ret;
}

int TrigClass::BeamTrig(){
  int ret=0;
	if(!_PJLV1) {
		char reply;
		PRINTF("Please switch to beam trigger and then press RETURN!\n")
		reply = getchar();
		return ret;
	}
// else ret=_PJLV1-> BeamTrigg();
  return ret;
}
