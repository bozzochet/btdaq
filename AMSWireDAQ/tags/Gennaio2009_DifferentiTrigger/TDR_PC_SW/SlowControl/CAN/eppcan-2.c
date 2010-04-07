// file eppcan-2.c
//
//  to understand EPP port
//
//  A.Lebedev Aug 2006...
//

#include "epplib.h"

int P = 3;

//~----------------------------------------------------------------------------

void print_LPT_registers(char *txt) {

  printf("0x%03X: %s\n", LPT_port, txt);
//printf("   LPT_data = %02hhX\n", inb(LPT_data));
//printf("   LPT_stat = %02hhX\n", inb(LPT_stat));
//printf("   LPT_ctrl = %02hhX\n", inb(LPT_ctrl));
//printf("   EPP_addr = %02hhX\n", inb(EPP_addr));
//printf("   EPP_data = %02hhX\n", inb(EPP_data));
//printf("   ECR_ctrl = %02hhX\n", inb(ECR_ctrl));
  printf("\n");
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int n, w, r;
  int16 LPT_ports[3] = {0x0378, 0x0278, 0x03BC};

  setbuf(stdout, NULL);
//printf("Usage: %s [code]\n", argv[0]);
//if (argc > 1) code = strtoul(argv[1], NULL, 16);
//printf("code = 0x%02X\n", code);

  LPT_port = LPT_ports[0];
  LPT_data = LPT_port + 0x0000;
  LPT_stat = LPT_port + 0x0001;
  LPT_ctrl = LPT_port + 0x0002;
  EPP_addr = LPT_port + 0x0003;
  EPP_data = LPT_port + 0x0004;
  ECR_ctrl = LPT_port + 0x0402;

//if (ioperm(LPT_port, 8, 1)) {     // get port permission
  if (iopl(3)) {                    // get I/O permission
    perror("iopl: ");
    exit(1);
  }

  print_LPT_registers("Begin");
  
  outb(0x00, LPT_ctrl);
  outb(0x01, LPT_stat);             // reset time-out
  print_LPT_registers("After 00");
  usleep(1000);
  print_LPT_registers("After usleep");
  outb(0x04, LPT_ctrl);             // reset EPP
  outb(0x84, ECR_ctrl);             // some magics, helped Bilge, needs iopl(3)

  print_LPT_registers("After 04");

  n = 0;
#if 1
  for (w=0x00; w<=0xFF; w++) {
    outb(w, EPP_addr);         // write
    r = inb(EPP_addr);         //  read
    if (r != w) n++;           //   compare
  }
#endif

#if 0
  for (w=0x00; w<=0xFF; w++) {
    outb(0x20, EPP_addr);
    outb(0x17, EPP_data);
    outb(0x21, EPP_addr);
    outb(w,    EPP_data);     // write
    outb(0x20, EPP_addr);
    outb(0x17, EPP_data);
    outb(0x21, EPP_addr);
    r = inb(EPP_data);        //  read
    if (r != w) n++;          //   compare
  }
#endif

  print_LPT_registers("After testing");

  if (n) {
    printf("\nLPT port 0x%03X: EPP address access failure(s) detected %d time(s)\n",
              LPT_ports[0], n);
  }
  else {
    exit(0);
  }

  return 0;
}

//~============================================================================
