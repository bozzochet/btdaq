// file 16.c
//
// Program to study CC 16ms task - A.Lebedev Jul-2006...

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x198;                     // 1xx stands for Newborn ID
int P = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, j, n;
  __16ms_par par[32];
  int16 err;
  int8 pro[3] = {'D', 'R', 'T'};

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n", argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  n = 32;
  for (i=0; i<n; i++) {
    __16ms_par *p = &par[i];
    p->cha = i;
    p->thr = 0xF000 + i;
    p->pro = pro[i%3];
    for (j=0; j<8; j++) p->par[j] = 16 * (i % 16) + j;
  }

  printf("setup: \n");
  for (i=0; i<n; i++) {
    __16ms_par *p = &par[i];
    printf("%2d) cha = %02hhX, thr = %04X, pro = %c, par = ", 
                 i, p->cha, p->thr, p->pro > ' ' ? p->pro : '?');
    for (j=0; j<8; j++) printf("%02hhX ", p->par[j]);
    printf("\n");
  }
  setup_16ms_task(n, par, &err);
  if (err) {
    printf("setup_16ms_task: err = %04X\n", err);
    exit(1);
  }
    
  check_16ms_task(&n, par, &err);
  if (err) {
    printf("check_16ms_task: err = %04X\n", err);
    exit(1);
  }
  printf("check:\n");
  for (i=0; i<n; i++) {
    __16ms_par *p = &par[i];
    printf("%2d) cha = %02hhX, thr = %04X, pro = %c, par = ", 
                 i, p->cha, p->thr, p->pro > ' ' ? p->pro : '?');
    for (j=0; j<8; j++) printf("%02hhX ", p->par[j]);
    printf("\n");
  }
  return 0;
}

//~============================================================================
