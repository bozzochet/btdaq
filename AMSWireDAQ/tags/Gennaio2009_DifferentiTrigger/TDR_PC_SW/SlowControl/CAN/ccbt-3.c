// file ccbt-3.c
//
// Test bench to develop USCM task to get data from CCBT boards,
// should run in CCEB USCM later on.
//
// Unlike ccbt-2.c this version tries to use USCM-like functions
//
// A.Lebedev - Dec 2006...
//

#include "uscmlib.h"

int8  CanRepBlkDat[4096];
int16 CanRepBlkLen;

struct _CCBT_data {
  bool  ok;
  int16 tstamp;
  int16 value[32];
} CCBT_data[2];

//~-----------------------------------------------------------------------------

int32 TimeGet(void) {

  struct timeval now;
  
  gettimeofday(&now, NULL);

  return (1000 * (now.tv_sec & 0x003FFF) + now.tv_usec / 1000);
}

//~-----------------------------------------------------------------------------

int32 TimeLapse(int32 tstamp) {

  struct timeval now;
  
  gettimeofday(&now, NULL);

  return (1000 * (now.tv_sec & 0x003FFF) + now.tv_usec / 1000 - tstamp);
}

//~-----------------------------------------------------------------------------

void CCBT_task(int ind) {

#define CCBT_START 1
#define CCBT_READ  2

  int16 err;
  int16 status;
  static int state[2];
  static int32 then[2];
  int16 data[32];

  const int8 sta_adr[2] = {0x02, 0x06};
  const int8 dat_adr[2] = {0x03, 0x07};

//~--
  
  if (ind < 0) {
    int j;
    for (j=0; j<2; j++) {
      state[j] = CCBT_START;
      CCBT_data[j].ok = FALSE;
    }
  }
  
  switch (state[ind]) {
  
    case CCBT_START:
      then[ind] = TimeGet();
      write_LVDS_bus(sta_adr[ind], 0x0001, &err);
//printf("START:   ind=%d, then[ind] = %d, err=%04X\n", ind, then[ind], err);
      if (!err) {
        state[ind] = CCBT_READ;
        return;
      }
    break;
    
    case CCBT_READ:
      if (TimeLapse(then[ind]) > 5000) {
        state[ind] = CCBT_START;
        return;
      }
      read_LVDS_bus(sta_adr[ind], &status, &err);
//printf("CHECK:   ind=%d, err=%04X\n", ind, err);
      if (!err) {
        int i;
        if (status & 0x0001) {
          for (i=0; i<32; i++) {
            read_LVDS_bus(dat_adr[ind], &data[i], &err);
if (!i)
printf("    READ:    ind=%d, err=%04X, dt=%4dms\n", ind, err, TimeLapse(then[ind]));
            if (err) {
              state[ind] = CCBT_START;
              return;
            }
          }
          CCBT_data[ind].ok = TRUE;
          CCBT_data[ind].tstamp = TimeGet();
          for (i=0; i<32; i++) CCBT_data[ind].value[i] = data[i];
          state[ind] = CCBT_START;
          return;
        }
      }
      else {
        CCBT_data[ind].ok = FALSE;
        state[ind] = CCBT_START;
        return;
      }
    break;
  }
}

//~-----------------------------------------------------------------------------

void Read_CCBT_data(void) {

  int i, j;
  int16 age;
  
  CanRepBlkLen = 0;

  for (j=0; j<2; j++) {
    struct _CCBT_data *p = &CCBT_data[j];
    CanRepBlkDat[CanRepBlkLen++] = p->ok;
    age = TimeLapse(p->tstamp) / 1000;
    CanRepBlkDat[CanRepBlkLen++] = age >> 8;
    CanRepBlkDat[CanRepBlkLen++] = age & 0xFF;
    for (i=0; i<32; i++) {
      CanRepBlkDat[CanRepBlkLen++] = p->value[i] >> 8;
      CanRepBlkDat[CanRepBlkLen++] = p->value[i] & 0xFF;
    }
  }
}

//~=============================================================================

int main(int argc, char *argv[]) {

  bool iopt_P = FALSE;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [USCM_ID [JMDC_ID [P]]]]\n",argv[0]);

  if (argc > 1) USCM_ID  = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID  = strtoul(argv[2], NULL, 16);
  if (argc > 3) P        = atoi(argv[3]);

  printf("USCM_ID = %03X\n", USCM_ID);
  printf("JMDC_ID = %d\n",   JMDC_ID);
  printf("P       = %d\n",   P);
  
  if (!initialize_EPP()) exit(1);

  CCBT_task(-1);

  while (1) {
    CCBT_task(0);
    CCBT_task(1);
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
        case 'Q':
          exit(0);
        break;
        case 'P':
          iopt_P = iopt_P ? FALSE : TRUE;
        break;
        case 'R':
          Read_CCBT_data();
          if (iopt_P) {
            int i;
            printf("\n");
            for (i=0; i<2*(2+2*32); i++) printf("%02hhX ", CanRepBlkDat[i]);
            printf("\n");
          }
        break;
      }
    }
  }
}

//~=============================================================================
