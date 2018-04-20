#define PUTIL_SOURCE
#include "PUtil.h"




int GETRXDONE(AMSWcom *node) {
  int error=0;
  if (node->GetHW()==kAMSW_EPP) { 
    // LPRINTF("EPP rxdone = %04x\n",node->GetRX_DONE());
    if (node->GetRX_DONE()!=0x4180) error=1; 
  }
  if (node->GetHW()==kAMSW_PCI) {
    // LPRINTF("PCI rxdone = %d\n",(short) node->GetRX_DONE());
    if (node->GetRX_DONE()!=0) error=1; 
  }

  return error;
}

int PrintRXDONE(AMSWcom *node) {
  int error=0;
  if (node->GetHW()==kAMSW_EPP) { 
    LPRINTF("EPP rxdone = %04x\n",node->GetRX_DONE());
    if (node->GetRX_DONE()!=0x4180) error=1; 
  }
  if (node->GetHW()==kAMSW_PCI) {
    LPRINTF("PCI rxdone = %d\n",(short) node->GetRX_DONE());
    if (node->GetRX_DONE()!=0) error=1; 
  }

  return error;
}

char* TakeTime() {

  static char ret[100];

  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  sprintf(ret, "%s", asctime(timeinfo));
  ret[strlen(ret)-1]='\0';

  return ret;
}
