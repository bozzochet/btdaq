#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstring>
#include <stdio.h>

#include "TrigClass.h"

//JLV1 case & fake case
TrigClass::TrigClass(JLV1* pointer):CommClass(NULL,0){
  _PJLV1 = pointer;
  if (!pointer){//fake case
    fake=1;
  }
}

//NI_USB case
TrigClass::TrigClass(const char* address, int port):CommClass(address,port){
  //initializing...
  this->TestConnection();
  _PJLV1 = NULL;
  number1=0;
  number2=0;
  number3=0;
  counter=0;
  fake=0;
}

int TrigClass::ReadCounter(){
  int ret=0;
  if(!_PJLV1) {
    if (!fake){
      Read();
      return counter;
    }
    else{
      ////PRINTF("This is no real Trigger Class, so there's no counter...\n"); 
      return -9999;
    }
  }
  else ret=_PJLV1->GetEventNumber();
  return ret;
}

int TrigClass::ResetCounter(){
  int ret=0;
  if(!_PJLV1) {
    if (!fake){ 
      char command[255];
      char answer[255];
      sprintf(command,"RESETCOUNTER");
      SendCommand(command,answer,10);
      usleep(300000);
    }
    else{
      //PRINTF("This is no real Trigger Class, so there's no counter...\n");
    }
    return ret;
  }
  else ret=_PJLV1->EventReset();
  return ret;
}

int TrigClass::TriggerOn(){
  LPRINTF("TriggerOn...\n");
  int ret=0;
  if(!_PJLV1) {
    if (!fake){
      ret=Write(1,0);//switching off calibration trigger for precaution
      ret+=Write(0,1);
      if (ret<0) return ret;
      else ret=0;
    }
    else{
      char reply;
      PRINTF("Please press RETURN and then turn ON the trigger production!\n");
      //printf("HACK FOR NOW: NO NEED TO PRESS, JUST WAIT 3 SECONDS!\n");
      //sleep(3);
      reply = getchar();
    }
    
    return ret;
  }
  else ret=_PJLV1-> TriggerOn();
  return ret;
}

int TrigClass::TriggerOff(){
  LPRINTF("TriggerOff...\n");
  int ret=0;
  if(!_PJLV1) {
    if (!fake){
      ret=Write(1,0);//switching off calibration trigger for precaution
      ret+=Write(0,0);
      if (ret<0) return ret;
      else ret=0;
    }
    else{
      char reply;
      PRINTF("Please turn OFF the trigger production and then press RETURN!\n");
      //printf("HACK FOR NOW: NO NEED TO PRESS, JUST WAIT 3 SECONDS!\n");
      //sleep(3);
      reply = getchar();
    }
    
    return ret;
  }
  else ret=_PJLV1-> TriggerOff();
  return ret;
}

int TrigClass::CalibTriggerOn(){
  LPRINTF("CalibTriggerOn...\n");
  int ret=0;
  if(!_PJLV1) {
    if (!fake){
      ret=Write(0,0);//switching off data trigger for precaution
      ret+=Write(1,1);
      if (ret<0) return ret;
      else ret=0;
    }
    else{
      char reply;
      PRINTF("Please press RETURN and then turn ON the trigger production!\n");
      //printf("HACK FOR NOW: NO NEED TO PRESS, JUST WAIT 3 SECONDS!\n");
      //sleep(3);
      reply = getchar();
    }
    
    return ret;
  }
  else ret=_PJLV1-> CalibTriggerOn();
  return ret;
}

int TrigClass::CalibTriggerOff(){
  LPRINTF("CalibTriggerOff...\n");
  int ret=0;
  if(!_PJLV1) {
    if (!fake){
      ret=Write(0,0);//switching off data trigger for precaution
      ret+=Write(1,0);
      if (ret<0) return ret;
      else ret=0;
    }
    else{
      char reply;
      PRINTF("Please turn OFF calibration trigger and then press RETURN!\n");
      //printf("HACK FOR NOW: NO NEED TO PRESS, JUST WAIT 3 SECONDS!\n");
      //sleep(3);
      reply = getchar();
    }
    
    return ret;
  }
  else ret=_PJLV1-> CalibTriggerOff();
  return ret;
}

//only NI-USB case
int TrigClass::ReadInput(){
  Read();
  return number3;
}

int TrigClass::Read(int noclose){
  char command[255];
  char answer[255];
  sprintf(command,"READ");
  SendCommand(command,answer,10,noclose);
  if(noclose==0) usleep(300000);
  sscanf(answer,"%X %X %X %d\n",&number1,&number2,&number3,&counter);
  return number3;
}

int TrigClass::Init(){
  int ret=0;

  for (int i=0; i<16; i++){
    ret=Write(i,0);
    if (ret<0) return ret;
    else ret=0;
    usleep(300000);
  }

  return ret;
}

int TrigClass::Write(int bit2modify,int value2write){
  //  PRINTF("Modifying bit : %d with value %d\n", bit2modify, value2write);//only for debug
  int ret=0;
  char command[255];
  char answer[255];
  int temp1=0;
  int temp2=0;
  int bit2modify1=0;
  int bit2modify2=0;
  int OR1=0;
  int OR2=0;
  int AND1=255;
  int AND2=255;
  int ANDvalue2write1=255;
  int ANDvalue2write2=255;
  int ORvalue2write1=0;
  int ORvalue2write2=0;
  if (bit2modify<8) {
    bit2modify1=bit2modify;
    AND1=0;
    OR1=255;
  }
  else if (bit2modify<16) {
    bit2modify2=bit2modify-8;
    AND2=0;
    OR2=255;
  }
  else {
    ret=1;
    PRINTF("Error: we have only 2 ports with each 8 channels to write (0-15)...\n");
    return ret;
  }
  if (value2write==0) {
    if (bit2modify<8) {
      ORvalue2write1=0;
      ANDvalue2write1=255 - (1<<bit2modify);
    }
    else if (bit2modify<16) {
      ORvalue2write2=0;
      ANDvalue2write2=255 - (1<<(bit2modify-8));
    }
  }
  else {//any passed number except 0 is took like 1...
    if (bit2modify<8) {
      ORvalue2write1=0 + (1<<bit2modify);
      ANDvalue2write1=255;
    }
    else if (bit2modify<16) {
      ORvalue2write2=0 + (1<<(bit2modify-8));
      ANDvalue2write2=255;
    }
  }
  Read(1);
  int n1=  number1 & (ANDvalue2write1|AND1) | (ORvalue2write1&OR1);
  int n2=  number2 & (ANDvalue2write2|AND2) | (ORvalue2write2&OR2);
  //  PRINTF("n1= %d=( ( %d & ( (%d|%d)=%d ) )=%d | (%d&%d)=%d )\n", n1, number1, ANDvalue2write1, AND1, (ANDvalue2write1|AND1), number1 & (ANDvalue2write1|AND1), ORvalue2write1, OR1, (ORvalue2write1&OR1));//only for debug
  //  PRINTF("n2= %d=(( ( %d & ( (%d|%d)=%d ) )=%d | (%d&%d)=%d ))\n", n2, number2, ANDvalue2write2, AND2, (ANDvalue2write2|AND2), ( number2 & (ANDvalue2write2|AND2) ), ORvalue2write2, OR2, (ORvalue2write2&OR2));//only for debug
  LPRINTF("WRITE %d %d\n", n1, n2 );
  sprintf(command,"WRITE %d %d",n1,n2 );
  ret=SendCommand(command,answer,10,1);
  Read();
  
  if ( !( number1==n1	 &&	 number2==n2 ) ) {
    PRINTF("Error writing trigger settings on NI...\n");
    ret+=1;
  }
  return ret;
}
