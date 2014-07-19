// file amswlib-fake.c
//
// should be used in SlowControl/AMSW/design for xforms debug ONLY

#include "daqlib.h"

static bool timeout_set = FALSE;
static float timeout;

int P1 = 1;
int P2 = 1;
int P3 = 1;

//~============================================================================

void print_amsw_path(char *txt) {

  printf("CMD:: %s: PATH = %04X %04X %04X %04X %04X\n", txt,
    AMSW_PATH[0], AMSW_PATH[1], AMSW_PATH[2], AMSW_PATH[3], AMSW_PATH[4]);
}

//~============================================================================

void set_AMSW_timeout(float value) {

  timeout     = value;
  timeout_set = TRUE;
}

//~----------------------------------------------------------------------------

bool get_AMSW_timeout(float *value) {

  if (value) *value = timeout;
  return timeout_set;
}

//~============================================================================

bool initialize_EPP() {

  return 1;
}

//~-----------------------------------------------------------------------------

void generate_EPPCAN_box_trigger(int16 trig_msk, int16 *err) {

  *err = 0;
  
  if (P1) {
    printf("CMD:: generate_EPPCAN_box_trigger: trig_msk = 0x%04X\n", trig_msk);
  }
}

//~-----------------------------------------------------------------------------

void get_EPPCAN_box_busy(int16 *busy_sta, int16 *err) {

  *err = 0;
  
  if (P1) printf("CMD:: get_EPPCAN_box_busy\n");
}

//~============================================================================

void boot_JINF(int16 fh, int16 *err) {

  *err = 0;
  if (P1) {
    print_amsw_path("boot_JINF");
    printf("fh = 0x%08X\n", fh);
  }
}

//~-----------------------------------------------------------------------------

void load_JINF_FLASH_file(int16 fh, int16 *err) {

  *err = 0;
  if (P1) {
    print_amsw_path("load_JINF_FLASH_file");
    printf("fh = 0x%08X\n", fh);
  }
}

//~-----------------------------------------------------------------------------

void ping_JINF(int n, int16 *dat, int *nn, int16 *datdat, int16 *err) {

  *err = 0;
  if (P1) {
    print_amsw_path("ping_JINF");
    printf("n = %d\n", n);
  }
}

//~-----------------------------------------------------------------------------

void get_JINF_status(int *fmt, int16 *dat, 
                     int *n, int16 *datdat, int16 *sta, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: get_JINF_status\n");
}

//~-----------------------------------------------------------------------------

void get_JINF_FLASH_summary(int *fmt, int16 *dat, 
                            int *n, int16 *datdat, int16 *sta, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: get_JINF_FLASH_summary\n");
}

//~-----------------------------------------------------------------------------

void get_JINF_delay(int8 *delay, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: get_JINF_delay\n");
}

//~-----------------------------------------------------------------------------

void set_JINF_delay(int8  delay, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: set_JINF_delay: delay = 0x%02X = %2d\n", delay, delay);
}

//~-----------------------------------------------------------------------------

void get_JINF_event_nb(int16 *event_nb, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: get_JINF_event_nb\n");
}

//~-----------------------------------------------------------------------------

void reset_JINF_event_FIFO(int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: reset_JINF_event_FIFO\n");
}

//~-----------------------------------------------------------------------------

void get_JINF_SSF(int16 *mode, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: get_JINF_SSF\n");
}

//~-----------------------------------------------------------------------------

void set_JINF_SSF(int16  mode, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: set_JINF_SSF: %s\n", mode ? "ON" : "OFF");
}

//~-----------------------------------------------------------------------------

void get_JINF_busy_mask(int32 *mask, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: get_JINF_busy_mask\n");
}

//~-----------------------------------------------------------------------------

void set_JINF_busy_mask(int32  mask, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: set_JINF_busy_mask: mask = 0x%08X\n", mask);
}

//~-----------------------------------------------------------------------------

void get_JINF_busy_status(int32 *status, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: get_JINF_busy_status\n");
}

//~-----------------------------------------------------------------------------

void get_JINF_busy_errors(int32 *errors, int16 *err) {

  *err = 0;
  if (P1) {
    printf("CMD:: get_JINF_busy_errors\n");
  }
}


//~-----------------------------------------------------------------------------

void test_JINF_slaves(int16 *err) {

  *err = 0;
  if (P1) {
    print_amsw_path("test_JINF_slaves");
  }
}

//~-----------------------------------------------------------------------------

void get_JINF_slave_masks(int32 *masks, int16 *err) {

  *err = 0;
  if (P1) {
    print_amsw_path("get_JINF_slave_mask");
  }
}

//~-----------------------------------------------------------------------------

void set_JINF_slave_mask(int8 mask_ID, int32 mask, int16 *err) {

  *err = 0;
  if (P1) {
    print_amsw_path("set_JINF_slave_mask");
    printf("mask_ID = 0x%02X, mask = 0x%08X\n", mask_ID, mask);
  }
}

//~-----------------------------------------------------------------------------

void get_JINF_proc_mode(int16 *mode, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: get_JINF_proc_mode\n");
}

//~-----------------------------------------------------------------------------

void set_JINF_proc_mode(int16  mode, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: set_JINF_proc_mode: mode = 0x%04X\n", mode);
}

//~-----------------------------------------------------------------------------

bool get_JINF_event(int *n, int16 *event, int16 *err) {

  *err = 0;
  if (P1) printf("CMD:: get_JINF_event\n");
  
  return (rand() & 1);
}

//~-----------------------------------------------------------------------------

void init_LeCroy_bus(int8 bus, int16* err) {

//  sleep(1);

  if (rand() % 2) *err = 0;
  else            *err = 1;
*err=0;
  if (P1) {
    print_amsw_path("init_LeCroy_bus");
    printf("bus = %02X\n", bus);
  }
}

//~-----------------------------------------------------------------------------

void exec_LeCroy_cmd(int n, int8 *bus, int32 *wri, int32 *rea, bool *chk, int16 *err) {

//  sleep(1);
  
  if (rand() & 1) *err = 1;
  else            *err = 0;
  
  if (!*err && (rand() & 1)) *chk = 1;
  else                       *chk = 0;

  *err=0;*chk=0;
  *rea = 0x00000000; // needed for Brick
//*err = 0x0123;
  if (P1) {
    print_amsw_path("exec_LeCroy_bus");
    printf("n = %d bus = %02X wri = %08X err = 0x%04X(fake) chk = %d(fake)\n", 
           n, *bus, *wri, *err, *chk);
  }
}

//~============================================================================

void begin_IO_segment(int16 *seg) {

  int16 *p = seg;

  *p++ = 0x8000;
  *p++ = 0x0000;
}

//~----------------------------------------------------------------------------

void end_IO_segment(int16 *seg) {

  int16 *p = seg + 2 + (*seg & 0x3FFF);
  int16 FCS = 0xFFFF;
  int16 g   = 0x1021;
  int16 *pp, d, i;
  
  for (pp=seg+2; pp<p; pp++) {
    d = *pp;
    for (i=0; i<16; i++) {
      if ((FCS ^ d) & 0x8000) FCS = (FCS << 1) ^ g;
      else                    FCS = (FCS << 1);
      d <<= 1;
    }
  }

  *p++ = FCS;

  *seg = 0x8000 + (p - seg) - 2;
}

//~----------------------------------------------------------------------------

void add_IO_read(int16 *seg, int16 adr) {

  int16 *p = seg + 2 + (*seg & 0x3FFF);

  *p++ = 0x060C;
  *p++ = adr;
  *p++ = 0x040D;
  *p++ = 0x0000;
  *seg = 0x8000 + (p - seg) - 2;
}

//~----------------------------------------------------------------------------

void add_IO_write(int16 *seg, int16 adr, int16 dat) {

  int16 *p = seg + 2 + (*seg & 0x3FFF);

  *p++ = 0x060C;
  *p++ = adr;
  *p++ = 0x060D;
  *p++ = dat;
  *seg = 0x8000 + (p - seg) - 2;
}

//~----------------------------------------------------------------------------

void perform_IO_operation(int16 *wri, int *n_r, int16 *rea, int16 *err) {

  int i;
  
  if (rand() % 2) *err = 0;
  else            *err = 1;

*err = 0;

  if (P1) {
    print_amsw_path("perform_IO_operation");
    for (i=0; i<10; i++) printf("%04X ", wri[i]);
    printf("\n");
  }
}

//~============================================================================

void get_EDR2_delay(int8 *delay, int16 *err) {

  int16 adr = 0;
  int16 request[100];
  int16 reply[100];
  
  begin_IO_segment(request);
  add_IO_read(request, adr);
  end_IO_segment(request);
  perform_IO_operation(request, NULL, reply, err);

*err=rand() & 1;

  if (*err) return;
  
  *delay = reply[3];
}

//~----------------------------------------------------------------------------

void set_EDR2_delay(int8  delay, int16 *err) {

  int16 adr = 0;
  int16 request[100];
  int16 reply[100];

  if (delay > 0x7F) {
    *err = 0x1000;
    return;
  }
  
  begin_IO_segment(request);
  add_IO_write(request, adr, delay);
  end_IO_segment(request);
  perform_IO_operation(request, NULL, reply, err);

*err=rand() & 1;

}

//~============================================================================

void get_EDR2_DAC(int16 adr, int16 *DAC, int16 *err) {

  int16 bias = 1;
  int16 msk = 0x1FFE;
  int16 request[100];
  int16 reply[100];
  
  if (adr > 1) {
    *err = 0x1000;
    return;
  }

  begin_IO_segment(request);
  add_IO_read(request, adr+bias);
  end_IO_segment(request);
  perform_IO_operation(request, NULL, reply, err);

  if (*err) return;
  
  *DAC = unpack16(reply[3], msk);
}

//~----------------------------------------------------------------------------

void set_EDR2_DAC(int16 adr, int16 DAC, int16 *err) {

  int16 bias = 1;
  int16 msk = 0x1FFE;
  int16 dat;
  int16 request[100];
  int16 reply[100];

  if (P1) printf("CMD:: set_EDR2_DAC: adr = %2d, DAC = %04X = %4d\n", adr, DAC, DAC);
  if (adr > 1) {
    *err = 0x1000;
    return;
  }

  if (DAC > 0x0FFF) {
    *err = 0x1100;
    return;
  }
  
  begin_IO_segment(request);
  dat = 0x4000;
  pack16(&dat, msk, DAC);
  add_IO_write(request, adr+bias, dat);
  end_IO_segment(request);
  perform_IO_operation(request, NULL, reply, err);
}

//~----------------------------------------------------------------------------

void get_EDR2_DACs(int16 *DAC0, int16 *DAC1, int16 *err) {

  int16 bias = 1;
  int16 msk = 0x1FFE;
  int16 request[100];
  int16 reply[100];
  
  begin_IO_segment(request);
  add_IO_read(request, bias+0);
  add_IO_read(request, bias+1);
  end_IO_segment(request);
  perform_IO_operation(request, NULL, reply, err);

  if (*err) return;
  
  if (DAC0) *DAC0 = unpack16(reply[3], msk);
  if (DAC1) *DAC1 = unpack16(reply[7], msk);
}

//~----------------------------------------------------------------------------

void set_EDR2_DACs(int16  DAC0, int16  DAC1, int16 *err) {

  int16 bias = 1;
  int16 msk = 0x1FFE;
  int16 request[100];
  int16 reply[100];

  if (((DAC0 & 0x7FFF) > 0x0FFF) || ((DAC1 & 0x7FFF) > 0x0FFF)) {
    *err = 0x1000;
    return;
  }
  
  begin_IO_segment(request);
  if (!(DAC0 & 0x8000)) {
    int16 dat = 0x4000;
    dat = pack16(&dat, msk, DAC0&0x7FFF);
    add_IO_write(request, bias+0, dat);
  }
  if (!(DAC1 & 0x8000)) {
    int16 dat = 0x4000;
    dat = pack16(&dat, msk, DAC1&0x7FFF);
    add_IO_write(request, bias+1, dat);
  }
  end_IO_segment(request);
  perform_IO_operation(request, NULL, reply, err);
}

//~----------------------------------------------------------------------------

void get_EDR2_param(int8 *delay, int16 *DAC1, int16 *DAC2, int16 *err) {
}

//~----------------------------------------------------------------------------

void set_EDR2_param(int8  delay, int16  DAC1, int16  DAC2, int16 *err) {
}

//~============================================================================
