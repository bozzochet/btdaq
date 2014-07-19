// file adc.c
//
// Read ADC to test interference with "16ms task" for CCEB
//
// A.Lebedev Jun-2006...

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x199;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int16 dat[32];

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int16 err;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: t [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n");

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  while (1) {
    read_ADC(dat, &err);
    if (err) {
      printf("err = 0x%04X\n", err);
    }
    else {
      if (dat[1] > 0x0960) printf("adc[1] = %04X\n", dat[1]);
    }
  }
  return 0;
}

//~============================================================================

#if 0

bool listen;

int new_n;
int8 new_adc[32];
int16 new_thr[32];

int n;
int8 adc[32];
int16 thr[32];

if (listen) {
  n = new_n;
  for (i=0; i<n; i++) {
    adc[i] = new_adc[i];
    thr[i] = new_thr[i];
  }
  listen = FALSE;
} 

for (i=0; i<n; i++) {
  val = read_ADC(adc[i]);
  if (val > thr[i]) print something;
}

//~----------------------------------------------------------------------------

bool listen;

int new_n;
int8 new_adc[32];
int16 new_thr[32];

int cmd_n;
int8 cmd_adc[32];
int16 cmd_thr[32];

listen = FALSE;
  
new_n = cmd_n;
for (i=0; i<new_n; i++) {
  new_adc[i] = cmd_adc[i];
  new_thr[i] = cmd_thr[i];
}

listen = TRUE;

while () {
  if (!listen) .... reply
}

//~----------------------------------------------------------------------------

#endif
 
