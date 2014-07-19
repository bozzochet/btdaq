// file ccbt-1.c
//
// Example of a program to check the multiple LVDS reads
// and check CCBT board logics
//
// A.Lebedev - May-2006...

#include "uscmlib.h"

int P = 0;                                     // global variable
int USCM_ID = 0x196;                           // global variable
int JMDC_ID = 0x00;                            // global variable

int BUS_FOR_REQUEST = 0;                       // global variable
int BUS_FOR_REPLY = 0;                         // global variable

//~----------------------------------------------------------------------------

void reboot_USCM_from_flash(void) {

  int i;
  int16 err;

  printf("Reboot started...\n");
  boot_from_FLASH(&err);
  for (i=0; i<3; i++) {
    ping_USCM(0, NULL, NULL, NULL, &err);
    if (!err) {
      printf("Reboot completed.\n");
      return;
    }
  }
  PANIC("reboot_USCM_from_flash FAILED\n");
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i;
  int16 err;
  int16 dac[16] = {0x8000, 0x8000, 0x8000, 0x8000,    //  1--4
                   0x8000, 0x8000, 0x8000, 0x8000,    //  5--8
                   0x8000, 0x8000, 0x0FFF, 0x8000,    //  9-12
                   0x8000, 0x8000, 0x8000, 0x8000};   // 13-16
  int16 lvds;
  int16 dat[32];
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID     = 0x%02X\n", USCM_ID);
  printf("JMDC ID     = %d\n", JMDC_ID);
  printf("P           = %d\n", P);

  if (!initialize_EPP()) exit(1);

  reboot_USCM_from_flash();
  write_DAC(dac, &err);
  if (err) PANIC("write_DAC FAILED\n");
  write_LVDS_bus(0x02, 0x01, &err);
  if (err) PANIC("write_LVDS_bus FAILED\n");
  while (1) {
    read_LVDS_bus(0x02, &lvds, &err);
    if (err) PANIC("read_LVDS_bus FAILED\n");
    if (lvds & 0x0001) break;
    printf("no DAV yet\n");
    delay(1.0);
  }
  printf("\n\nDAV!!!\n\n");
  Read_LVDS_bus(0x03, 32, dat, &err);
  if (err) PANIC("Read_LVDS_bus FAILED\n");
  for (i=0; i<32; i++) {
    printf("%2d: %04X\n", i, dat[i]);
  }
  
  return 0;
}

//~============================================================================
