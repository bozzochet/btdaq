// file blocktest.c
//
// Example of AMS-02 data and commands recording
//
// Everything stolen from Jinghui Zhang and reworked
//
// A.Lebedev, Aug-2007...
//

#include "recorder.h"

int16 apid_can  = APID_CAN;
int16 apid_amsw = APID_AMSW;

int CANFrameSize  = 1000;
int AMSWFrameSize = 1000;

//                                                File  Dir   File  
//                               File  Dir  File  Name  Name  Inter  Timer
//                    Directory   Nb   Nb    fd   len   len   val     ON
MyFile can  = {"/Data/EPP/CAN/",  -1,   -1,  -1,    3,    4,    60,    0};
MyFile amsw = {"/Data/EPP/AMSW/", -1,   -1,  -1,    3,    4,    60,    0};

//~============================================================================

int main() {

  int i, n = 0;
  int8 Buf[800];
  AMSBlock Request, *pRequest = &Request;

  pRequest->BlockType   = 0x01;
  pRequest->NodeAddress = 0x03;
  pRequest->DataType    = 0x1E & ~0x40;
  pRequest->DataCount   = 80;
  pRequest->Data.p8     = Buf;

  RecordTimer(&can,  RECORD_TIMER_START);
  RecordTimer(&amsw, RECORD_TIMER_START);
   
  while(1) {
    Buf[0] = n++;
    for (i=1; i<800; i++) Buf[i] = i;
    RecordAMSBlock(&can, pRequest, apid_can, CANFrameSize);
  }

  return 1;
}
