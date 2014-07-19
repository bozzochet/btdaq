// file gps-sim.c
//
// Program to control GPS Simulator - A.Lebedev Oct-2005...
// Sends a PPS and "correct" STIME TM
//
// A.Lebedev Dec-2005...
//
// Stolen from Volker and reworked

#include "gpslib.h"

int COM1 = 0x03F8;

static char *rates[4] = {"2400", "4800", "9600", "19200"};
static int8 baud[4]   = {  0x30,   0x18,   0x0C,   0x06};

//~----------------------------------------------------------------------------

void outp(int16 adr, int8 dat) {

  outb(dat, adr);
}

//~----------------------------------------------------------------------------

int8 inp(int16 adr) {

  return inb(adr);
}

//~---------------------------------------------------------------------------- 

bool setup_PC_serial_port(int rate) {

  if (iopl(3)) {
    perror("iopl: ");
    return FALSE;
  }

  printf("serial port setting was:\n");
  outp(COM1+3, 0x83);  // set DLAB bit to get access to the BAUD rate register
  printf("LS = 0x%02X\n", inp(COM1));    // read LS part
  printf("MS = 0x%02X\n", inp(COM1+1));  // read MS part

  outp(COM1+3, 0x83);  // set DLAB bit to get access to the BAUD rate register
  outp(COM1,   baud[rate]);   // load LS value for BAUD rate
  outp(COM1+1, 0x00);         // load MS value for BAUD rate
  
  printf("serial port setting now:\n");
  printf("LS = 0x%02X\n", inp(COM1));    // read LS part
  printf("MS = 0x%02X\n", inp(COM1+1));  // read MS part

  outp(COM1+3, 0x03);     // reset DLAB bit and set:
                          // 8 bit word length, one stop bit, no parity

  return TRUE;
}

//~----------------------------------------------------------------------------

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

//~--------------------------------------------------------------------------------------

void prepare_STIME_message(int *n, int8 *b) {

  int16 week;
  int32 sec;
  time_t now;

  now = time(NULL) - 0x12D53D80;
  week = now / (7 * 24 * 60 * 60);
  sec  = now % (7 * 24 * 60 * 60);
  
//printf("now=0x%08X, week=%d, sec=%d\n", now, week, sec);

  create_STIME(week, sec, n, b);
}

//~======================================================================================

int main(int argc, char* argv[]) {

  int n;
  int rate, period;
  int8 b[512];
  char *tst = {"abcdefg0123456789"};

  setbuf(stdout, NULL);
  printf("Usage: %s [<Baud Rate (0..3)> [<Period]]]\n",argv[0]);

  rate = 2;
  if (argc > 1) rate = atoi(argv[1]);
  if (rate < 0 || rate > 3) exit(1);

  period = 1;
  if (argc > 2) period = atoi(argv[2]);
  
  printf("Baud Rate = %d (%s)\n", rate, rates[rate]);
  printf("Period    = %d sec\n", period);
  
  if (!setup_PC_serial_port(rate)) exit(1);

  n = strlen(tst);
  memcpy(b, tst, n);
  
  while (1) {
    int i;
    prepare_STIME_message(&n, b);
    syncronize(period);     // sync with computer clock "seconds"
    outp(COM1+4, 0x0B);     // set RTS = 1 ==> set PPS to high
    delay(0.050);           // length of PPS pulse (GPSE needs leading edge only)
    outp(COM1+4, 0x09);     // set RTS = 0 ==> set PPS to low
//  printf("PPS sent... %08X\n", (int32)time(NULL));
    for (i=0; i<n; i++) {
      while (!(inp(COM1+5) & 0x20));
      outp(COM1, b[i]);
    }
//  delay(1.0-0.050);         // this delay will simulate the one second interval
  }
}

//~======================================================================================
