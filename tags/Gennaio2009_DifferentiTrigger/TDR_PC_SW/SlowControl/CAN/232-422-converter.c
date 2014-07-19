// file 232-422-converter.c
//
// Program to send TC to GPS via GPS-simulator
// (GPS-simulator is used as RS232 <--> RS422 converter only)
//
// A.Lebedev Dec-2005...
//
// Stolen from Volker and reworked

#include "gpslib.h"

int COM1 = 0x03F8;

static char *rates[4] = {"2400", "4800", "9600", "19200"};
static int8 baud[4]   = {  0x30,   0x18,   0x0C,   0x06};

//~-------------------------------------------------------------------------------------- 

void outp(int16 adr, int8 dat) {

  outb(dat, adr);
}

//~--------------------------------------------------------------------------------------

int8 inp(int16 adr) {

  return inb(adr);
}

//~======================================================================================

int main(int argc, char* argv[]) {

  int i;
  int rate;
  _tc tc;

  setbuf(stdout, NULL);
  printf("Usage: %s [<Baud Rate (0..3)>]\n",argv[0]);

  rate = 2;
  if (argc > 1) rate = atoi(argv[1]);
  if (rate < 0 || rate > 3) exit(1);
  
  printf("Baud Rate = %d (%s)\n", rate, rates[rate]);
  
  if (iopl(3)) {
    perror("iopl: ");
    exit(1);
  }

  outp(COM1+3, 0x83); // set DLAB bit to get access to the BAUD rate register
  outp(COM1,   baud[rate]);   // load LS value for BAUD rate
  outp(COM1+1, 0x00);         // load MS value for BAUD rate
  
  printf("LS = 0x%02X\n", inp(COM1));    // read LS part
  printf("MS = 0x%02X\n", inp(COM1+1));  // read MS part

  outp(COM1+3, 0x03); // reset DLAB bit and set:
                      // 8 bit word length, one stop bit, no parity

  create_GMESS(3, 7, &tc);
  for (i=0; i<tc.cnt; i++) {
    while (!(inp(COM1+5) & 0x20));
    outp(COM1, tc.dat[i]);
  }

  while (1) {
    int8 ch;
    while (!(inp(COM1+5) & 0x01));
    ch = inp(COM1);
    printf("0x%02X ", ch);
  }
  return 0;
}

//~======================================================================================
