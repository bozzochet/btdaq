// file serialwrite.c
//
// Program to study serial communications - A.Lebedev Feb-2005...
// FS mod 12 feb 2008

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x14E;    // 1xx stands for Newborn ID
int P = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

bool use_CAN = TRUE;                               // global variable
bool use_MEM = FALSE;                              // global variable
bool use_UDP = FALSE;                              // global variable
bool use_TCP = FALSE;                              // global variable
bool use_Peter_TCP = FALSE;                        // global variable
bool use_PCIAMSW = FALSE;                          // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int l;
  char *txt0 = {"fa freddoooooo\n\r"};
  char *txt1 = {"test1\n\r"};
  int16 err;
  int8  mode, rate;

  setbuf(stdout, NULL);
  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n", argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  enable_serial_port(0, &err);
  if (err) {
    printf("enable_serial_port(0,..: err = %04X\n", err);
    exit(1);
  }
  printf("serial port 0 enabled.\n");

  check_serial_port(0, &mode, &rate, &err);
  if (err) {
    printf("check_serial_port(0,..: err = %04X\n", err);
    exit(1);
  }
  printf("serial port 0: mode = 0x%02X, rate = %d\n", mode, rate);

  enable_serial_port(1, &err);
  if (err) {
    printf("enable_serial_port(1,..: err = %04X\n", err);
    exit(1);
  }
  printf("serial port 1 enabled.\n");

  check_serial_port(1, &mode, &rate, &err);
  if (err) {
    printf("check_serial_port(1,..: err = %04X\n", err);
    exit(1);
  }
  printf("serial port 1: mode = 0x%02X, rate = %d\n", mode, rate);

  while (1) {
    l = strlen(txt0);
    write_serial_port(0, l, (int8*)txt0, &err);
    if (err) {
      printf("write_serial_port(0,..: err = %04X\n", err);
      exit(1);
    }
    
    l = strlen(txt1);
    write_serial_port(1, l, (int8*)txt1, &err);
    if (err) {
      printf("write_serial_port(1,..: err = %04X\n", err);
      exit(1);
    }
  }
  
    
  return 0;
}

//~============================================================================
