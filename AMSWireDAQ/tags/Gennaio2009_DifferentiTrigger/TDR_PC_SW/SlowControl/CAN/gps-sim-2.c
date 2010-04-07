// file gps-sim-2.c
//
// Reads a file with GPS TM and sends it to GPSE simulating GPS
//
// A.Lebedev Apr-2006...
//
// Stolen from Volker and reworked

#include "gpslib.h"

int COM1 = 0x03F8;

static char *rates[4] = {"2400", "4800", "9600", "19200"};
static int8 baud[4]   = { 0x30,   0x18,   0x0C,    0x06};

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

  outp(COM1+3, 0x83);         // set DLAB bit to get access to the BAUD rate register
  outp(COM1,   baud[rate]);   // load LS value for BAUD rate
  outp(COM1+1, 0x00);         // load MS value for BAUD rate
  
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

//~============================================================================

int main(int argc, char* argv[]) {

  int n;
  int rate, period;
  char b[8192];

  setbuf(stdout, NULL);
  printf("Usage: %s <file-name> [<Baud Rate (0..3)> [<Period]]]\n",argv[0]);

  if (argc < 2) exit(1);
  
  rate = 2;
  if (argc > 2) rate = atoi(argv[2]);
  if (rate < 0 || rate > 3) exit(1);

  period = 1;
  if (argc > 3) period = atoi(argv[3]);
  
  printf("Filename  = %s\n", argv[1]);
  printf("Baud Rate = %d (%s)\n", rate, rates[rate]);
  printf("Period    = %d sec\n", period);
  
  if (!setup_PC_serial_port(rate)) exit(1);
  
  while (1) {
    int i;
    static FILE *file = NULL;
    int j;
    if (!file) file = fopen(argv[1], "r");
    if (!file) exit(1);
    if (!fgets(b, sizeof(b), file)) exit(0);
    n = 0;
    sscanf(b, "Length = %d", &n);
    for (j=0; j<n; j++) fscanf(file, "%02hhX", &b[j]);
    syncronize(period);     // sync with computer clock "seconds"
    for (i=0; i<n; i++) {
      while (!(inp(COM1+5) & 0x20));
      outp(COM1, b[i]);
    }
  }
}

//~============================================================================
