// file serial-4.c
//
// Program to check USCM serial input
//  - uses PC serial port to transmit data to USCM serial port.
//
// A.Lebedev Mar-2006...
//
// Stolen from Volker and reworked (see also gps-sim)

#include <sys/io.h>
#include "mylib.h"
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

//~--------------------------------------------------------------------------------------

void syncronize(int period) {

  static bool first = TRUE;
  time_t now, then;
  
  if (first || !period) {
    first = FALSE;
    then = time(NULL);
    while (((now=time(NULL)) - then) < 1);
    then = now;
  }
  else {
    then = time(NULL);
    while (((now=time(NULL)) - then) < period);
    then = now;
  }
}

//~======================================================================================

int main(int argc, char* argv[]) {

  int i, n = 512;
  int rate, period;
  int8 b[10000];

  setbuf(stdout, NULL);
  printf("Usage: %s [<Baud Rate (0..3)> [<Period]]]\n",argv[0]);

  rate = 2;
  if (argc > 1) rate = atoi(argv[1]);
  if (rate < 0 || rate > 3) exit(1);

  period = 1;
  if (argc > 2) period = atoi(argv[2]);
  
  printf("Baud Rate = %d (%s)\n", rate, rates[rate]);
  printf("Period    = %d sec\n", period);
  
  if (iopl(3)) {
    perror("iopl: ");
    exit(1);
  }

  outp(COM1+3, 0x83);         // set DLAB bit to get access to the BAUD rate register
  outp(COM1,   baud[rate]);   // load LS value for BAUD rate
  outp(COM1+1, 0x00);         // load MS value for BAUD rate
  
  printf("LS = 0x%02X\n", inp(COM1));    // read LS part
  printf("MS = 0x%02X\n", inp(COM1+1));  // read MS part

  outp(COM1+3, 0x03);     // reset DLAB bit and set:
                          // 8 bit word length, one stop bit, no parity

  for (i=0; i<10000; i++) b[i] = (int8)i;
  
  while (1) {
    int i;
//  syncronize(period);     // sync with computer clock "seconds"
    sleep(period);
//  printf("PPS sent... %08X\n", (int32)time(NULL));
    for (i=0; i<n; i++) {
      while (!(inp(COM1+5) & 0x20));
      outp(COM1, b[i]);
    }
  }
}

//~======================================================================================
