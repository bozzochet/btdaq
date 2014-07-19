// file mem-1.c
//
// To study writing in USCM flash memory
//
// A.Lebedev Aug-2006...

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x190;      // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

void erase_config_file(int16 *err) {

  int i;
  bool done;
  int sector = 7;

  boot_from_OTP(err);
  if (*err) {
    int i;
    for (i=0; i<3; i++) {
      ping_USCM(0, NULL, NULL, NULL, err);
      if (!*err) break;
    }
    if (P) printf("boot_from_OTP failed\n");
    return;
  }

  start_FLASH_erasure(sector, err);
  if (*err) {
    if (P) printf("start_FLASH_erasure failed\n");
    return;
  }
  if (P) printf("Waiting for erase sector %d ...", sector);
  for (i=0; i<50; i++) {
    usleep(100000);
    check_FLASH_erasure(&done, err);
    if (*err) {
      printf("\n check_FLASH_erasure failed\n");
      return;
    }
    if (done) break;
    if (P) printf(".");
  }
  if (!done) {
    if (P) printf("\nerasure of FLASH failed");
    return;
  }
  if (P) printf("\n");
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int n;
  int32 adr;
  int8 rea[8192];
  int8 wri[8192];
  int16 err;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
          argv[0]);
  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);
  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  erase_config_file(&err);
  if (err) {
    printf("erase_config_file failed\n");
    exit(1);
  }

  n   = 4096 + 1;
  adr = 0x38000;
  printf("n = %d, adr = 0x%06X\n", n,adr);
  
  write_USCM_memory(n, adr, wri, &err);
  if (err) {
    printf("write_USCM_memory... err = 0x%04X\n", err);
    exit(1);
  }

  read_USCM_memory(n, adr, rea, &err);
  if (err) {
    printf("read_USCM_memory... err = 0x%04X\n", err);
    exit(1);
  }

  printf("O.K.\n");
  return 0;
}

//~============================================================================
