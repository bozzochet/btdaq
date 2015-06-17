#define PUTIL_SOURCE
#include "PUtil.h"


int GETRXDONE(AMSWcom *node) {
  return node->GETRXDONE();
}

int PrintRXDONE(AMSWcom *node) {
  char message[256];
  int ret=node->PrintRXDONE(message);
  LPRINTF("%s",message);
  return ret;
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
