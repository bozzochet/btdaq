#ifndef PUTIL_H
#define PUTIL_H
#include <stdio.h>
#include <cstring> // gcc 4.3: needed to add this
#include "amswcom.h"
#include "amsw_lib.h"


int PrintRXDONE(AMSWcom *node);
int GETRXDONE(AMSWcom *node);
char* TakeTime();

#define LPRINTF(a...) { fprintf(logfile,"%s: ",TakeTime()); fprintf(logfile,a); fflush(logfile); }
#define SLPRINTF(a...) { fprintf(shortlogfile,"%s: ",TakeTime()); fprintf(shortlogfile,a); fflush(shortlogfile); }
#define PRINTF(a...) { LPRINTF(a); printf(a); fflush(stdout); }
#define TESTRXDONE(a) { ret=PrintRXDONE(a); if (ret) return ret; }

#ifdef PUTIL_SOURCE
FILE * logfile;
FILE * shortlogfile;

#else
extern FILE * logfile;
extern FILE * shortlogfile;
#endif

#endif
