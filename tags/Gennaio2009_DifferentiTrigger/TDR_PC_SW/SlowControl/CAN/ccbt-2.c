// file ccbt-2.c
//
// Test bench to develop USCM task to get data from CCBT boards,
// should run in CCEB USCM later on.
//
// A.Lebedev - Nov 2006...
//

#include "uscmlib.h"

int16 CCBT_data[2][32];

//~-----------------------------------------------------------------------------

void CCBT_task(int ind) {

#define CCBT_START 1
#define CCBT_READ  2

  int16 err;
  int16 status;
  int16 data[32];
  static int state[2];
  static struct timeval now[2], then[2];
  const int8 sta_adr[2] = {0x02, 0x06};
  const int8 dat_adr[2] = {0x03, 0x07};
  
  if (ind < 0) state[0] = state[1] = CCBT_START;
  
  switch (state[ind]) {
  
    case CCBT_START:
      gettimeofday(&then[ind], NULL);
      write_LVDS_bus(sta_adr[ind], 0x0001, &err);
//printf("START:   ind=%d, err=%04X\n", ind, err);
      if (!err) {
        state[ind] = CCBT_READ;
        return;
      }
    break;
    
    case CCBT_READ:
      gettimeofday(&now[ind], NULL);
      if (delta_t(&now[ind], &then[ind]) > 5.0) {
        state[ind] = CCBT_START;
        return;
      }
      read_LVDS_bus(sta_adr[ind], &status, &err);
//printf("CHECK:   ind=%d, err=%04X\n", ind, err);
      if (!err) {
        int i;
        if (status & 0x0001) {
//        for (i=0; i<32; i++) {
          for (i=0; i<1; i++) {
            read_LVDS_bus(dat_adr[ind], &data[i], &err);
if (!i)
printf("    READ:    ind=%d, err=%04X, dt=%.1fs\n", ind, err, delta_t(&now[ind], &then[ind]));
            if (err) {
              state[ind] = CCBT_START;
              return;
            }
          }
          for (i=0; i<32; i++) CCBT_data[ind][i] = data[i];
          state[ind] = CCBT_START;
          return;
        }
      }
      else {
        state[ind] = CCBT_START;
        return;
      }
    break;
  }
}

//~=============================================================================

int main(int argc, char *argv[]) {

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
  }
}

//~=============================================================================
