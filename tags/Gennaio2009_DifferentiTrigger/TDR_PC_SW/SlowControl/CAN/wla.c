// file wla.c
//
// Write Logical Address into USCM
//
// A.Lebedev Aug-2004...
//

#include "uscmlib.h"

int JMDC_ID = 0;
int USCM_ID = 0x15E;
int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int P = 0;                                         // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i;
  int16 ID1, ID2, logical_ID;
  int error = FALSE;
  int8 done;

  int32 RQ_typ    = 0x06;
  int16 RQ_cnt    = 7;
  int8  RQ_dat[7] = {0x00, 0x02, 0x03, 0xFF, 0xFE};
  int16 err;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s <USCM_NB_ID> <USCM_LOG_ID> [<JMDC_ID> [<Printout Level>]]\n", argv[0]);
  if (argc < 3) {
    printf("<USCM_NB_ID> and <USCM_LOG_ID> are mandatory\n");
    exit(1);
  }

  ID1 = strtoul(argv[1], NULL, 16);
  ID2 = strtoul(argv[2], NULL, 16);

  if      (((ID1 & 0xFF00) == 0x0100) && ((ID2 & 0xFF00) == 0x0000)) {
    USCM_ID    = ID1;
    logical_ID = ID2;
  }
  else if (((ID2 & 0xFF00) == 0x0100) && ((ID1 & 0xFF00) == 0x0000)) {
    USCM_ID    = ID2;
    logical_ID = ID1;
  }
  else {
    printf("<USCM_NB_ID> must be 0x100 + USCM_ID\n");
    exit(1);
  }

  printf("Logical ID 0x%02X will be written to USCM with newborn ID 0x%02X\n", 
          logical_ID, USCM_ID&0x00FF);

  if (argc > 3) JMDC_ID = strtoul(argv[3], NULL, 16);
  if (argc > 4) P = atoi(argv[4]);

  RQ_dat[5] = logical_ID;
  RQ_dat[6] = ~RQ_dat[5];

  if (!error) {
    if (!initialize_EPP()) {
      printf("Failed to initialize the EPPCAN box\n");
      error = TRUE;
    }
  } 

  if (!error) {
    printf("Waiting for reboot...");
    boot_from_OTP(&err);
    for (i=0; i<3; i++) {
      printf(".");
      ping_USCM(0, NULL, NULL, NULL, &err);
      if (!err) {
        break;
      }
    }
    if (err) {
      printf("Failed (may be OTP software is out of date)\n");
//    error = TRUE;          allow for very old OTP software
    }
    else {
      printf("Done\n");
    }
  }

  if (!error) {
    start_FLASH_erasure(8, &err);
    if (err) {
      printf("erase FLASH sector 8 failed\n");
      error = TRUE;
    }
    printf("Waiting for erase sector 8 ...");
    for (i=50; i>0; i--) {
      usleep(100000);
      check_FLASH_erasure(&done, &err);
      if (err) {
        printf("\nquery FLASH sector 8 status failed\n");
        error = TRUE;
      }
      if (done) break;
      printf(".");
    }
    if (!done) {
      printf("\nerase FLASH failed");
      error = TRUE;
    }
    printf("\n");
  }

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,    &err);

  if (error) printf("WRITING FAILED.\n");
  else       printf("WRITING SUCCEEDED.\n");

  if (!error) {
    printf("Waiting for reboot...");
    boot_from_OTP(&err);
    for (i=0; i<3; i++) {
      printf(".");
      ping_USCM(0, NULL, NULL, NULL, &err);
      if (!err) {
        break;
      }
    }
    if (err) {
      printf("Failed (may be OTP software is out of date)\n");
//    error = TRUE;          allow for very old OTP software
    }
    else {
      printf("Done\n");
    }
  }

//~-- ping and print result

  return 0;
}

//~============================================================================
