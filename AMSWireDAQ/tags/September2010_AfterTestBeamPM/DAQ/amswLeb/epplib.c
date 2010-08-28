// file epplib.c
//
// Provides a set of basic low-level functions to work with EPP-port.
//
// Software stolen from VK, AK and others was reworked and used here.
//
// A.Lebedev Mar-2005...
//

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "epplib.h"
#ifdef USE_CAN
#include "canlib.h"
#endif
#ifdef USE_AMSW
#include "amswlib.h"
#endif

//~----------------------------------------------------------------------------

static void terminate_EPP(void) {

//outb(0x00, LPT_ctrl);      // otherwise AB is not happy
}

//~----------------------------------------------------------------------------

static sighandler_t EPP_ctrl_C_handler(int signum) {

  exit(0);
}

//~----------------------------------------------------------------------------

static void EPP_exit_handler(void) {

  terminate_EPP();
}

//~----------------------------------------------------------------------------

static int initialize_and_test(void) {

  int w, r, n;
  int twice;

int ii;
for (ii=0; ii<2; ii++) {

  for (twice=0; twice<2; twice++) {
    inb(LPT_stat);
    inb(LPT_ctrl);
    inb(EPP_addr);
    inb(EPP_data);
    outb(0x01, LPT_stat);  // Reset timeout (some EPP need write 1)
    outb(0x00, LPT_stat);  // Reset timeout (some EPP need write 0)
    outb(0x04, LPT_ctrl);  // Reset EPP (Enable EPP mode?)
    outb(0x84, ECR_ctrl);  // some magics, helped Bilge, needs iopl(3)
  }

#if 0
  n = 0;
  for (w=0x00; w<=0xFF; w++) {
    outb(w, EPP_addr);         // write
    r = inb(EPP_addr);         //  read
    if (r != w) n++;           //   compare
  }
#endif

#if 1
  n = 0;
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
//printf("ii=%d n=%d\n", ii, n);
}
  
  return (n);
}

//~----------------------------------------------------------------------------

bool initialize_EPP(void) {

  int ntry = 3;
  int try, n[3], nn;
  static bool first = TRUE, found = FALSE;
  bool fail;
  int16 LPT_ports[3] = {0x0378, 0x0278, 0x03BC};

  if (first) {
#ifdef USE_CAN
    if (!get_command_timeout(NULL)) set_command_timeout(1.0);
    serial_RX_buffer_size = 4096;
#endif
#ifdef USE_AMSW
    if (!get_command_timeout(NULL)) set_command_timeout(9.9);
#endif
    if (use_PCIAMSW) {
      return TRUE;
    }
    else if (use_TCP || use_EAS) {
      return TRUE;
    }
    else {
      if (iopl(3)) {                    // get I/O permission
        perror("iopl: ");
        return FALSE;
      }
      if (atexit(EPP_exit_handler)) {
        printf("atexit(EPP_exit_handler) failed.\n");
        return FALSE;
      }
      if (signal(SIGINT, (sighandler_t)EPP_ctrl_C_handler) == SIG_ERR) {
        printf("signal(SIGINT...EPP_ctrl_C_handler) failed\n");
        return FALSE;
      }
    }
    first = FALSE;
  }

  if (!found) {
    for (try=0; try<ntry; try++) n[try] = 0;
    for (try=0; try<ntry; try++) {
      LPT_port = LPT_ports[try];
      LPT_data = LPT_port + 0x0000;
      LPT_stat = LPT_port + 0x0001;
      LPT_ctrl = LPT_port + 0x0002;
      EPP_addr = LPT_port + 0x0003;
      EPP_data = LPT_port + 0x0004;
      ECR_ctrl = LPT_port + 0x0402;
      n[try] = initialize_and_test();
      if (!n[try]) break;
    }
    fail = TRUE;
    for (try=0; try<ntry; try++) if (!n[try]) fail = FALSE;
    if (fail) {
      for (try=0; try<ntry; try++) {
        if (n[try]) printf("LPT port 0x%03X: %d errors\n", LPT_ports[try], n[try]);
      }
      terminate_EPP();
      return FALSE;
    }
    else {
      if (P) printf("will use LPT port 0x%03X\n", LPT_port);
      found = TRUE;
      return TRUE;
    }
  }
  else {
    nn = initialize_and_test();
    if (nn) {
      printf("LPT port 0x%03X: %d errors\n", LPT_port, nn);
      return FALSE;
    }
    else {
      return TRUE;
    }
  }
}

//~============================================================================
