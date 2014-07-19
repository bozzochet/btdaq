// file uscmlib.c
//
// A.Lebedev Apr-2003...
//

//~----------------------------------------------------------------------------

#include "uscmlib.h"

bool ESPIONAGE = FALSE;
int8 CAB_CMD_ID;

//                            2400    4800    9600   19200
static int16 baud[2][4] = {{0xFF30, 0xFF98, 0xFFCC, 0xFFE6},
                           {0x9898, 0xCCCC, 0xE6E6, 0xF3F3}};

//~============================================================================
//
//                  BOOT COMMANDS
//
//~============================================================================

void reset_CPU(int16 *err) {

  int32 RQ_typ = 0x00;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1] = {0x00};

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================

void boot_from_OTP(int16 *err) {

  int32 RQ_typ = 0x00;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1] = {0x01};

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void boot_from_FLASH(int16 *err) {

  int32 RQ_typ = 0x00;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1] = {0x02};

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void boot_USCM(int8 from, int16 *err) {

  int32 RQ_typ = 0x00;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1];

  if (!(from < 6 || from == 0x8A || from == 0x8D)) {
    *err = 0x0100;
    return;
  }
  
  RQ_dat[0] = from;

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================
//
//                  "SYSTEM" COMMANDS
//
//~============================================================================

void clear_power_FF(int16 *err) {

  int32 RQ_typ = 0x00;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1] = {0x06};

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void open_USCM_OTP_backdoor(int16 *err) {

  int32 RQ_typ = 0x00;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1] = {0x8A};

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void close_USCM_OTP_backdoor(int16 *err) {

  int32 RQ_typ = 0x00;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1] = {0x8D};

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================
//
//                  USCM DEBUG COMMANDS
//
//~============================================================================

void read_debug_printout(int siz, int *n, int8 *dat, int16 *err) {

  int32 RQ_typ = 0x04;
  int32 RP_siz = siz;
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt,    dat,     err);

  if (*err || use_MEM) return;
  
  *n = RP_cnt;
}

//~----------------------------------------------------------------------------

void reset_debug_printout(int16 *err) {

  int32 RQ_typ = 0x04;

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ,      0,   NULL,      0,    NULL,   NULL,     err);
}

//~============================================================================
//
//                  PING/ECHO COMMAND
//
//~============================================================================

void ping_USCM(int n, int8 *dat, int *nn, int8 *datdat, int16 *err) {

  int32 RQ_typ = 0x01;
  int32 RQ_cnt = n;
  int32 RP_cnt;

  if (RQ_cnt > 8192) {
    *err = 0x0100;
    return;
  }

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt,    dat,   8192, &RP_cnt, datdat,     err);

  if (*err || use_MEM) return;
  
  if (nn) *nn = RP_cnt;
}

//~============================================================================
//
//                  STATUS COMMANDS
//
//~============================================================================

void get_USCM_status(int8 which, int8 *dat, int16 *err) {

  int32 RQ_typ = 0x02;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1];
  int8 *RP_dat = dat;
  int32 RP_siz;
  int32 RP_cnt;

  if (which > 1) {
    *err = 0x0100;
    return;
  }
  
  RQ_dat[0] = which;
  if (RQ_dat[0] == 0) RP_siz =  23;
  else                RP_siz = 727;
  
//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;
  
// Thank you, HBB & SXu
  if (!((which & (RP_cnt == 14 || RP_cnt == 727 || RP_cnt == 667 || RP_cnt == 663)) || 
       (!which & (RP_cnt ==  8 || RP_cnt ==  9)))) {
    *err = 0x0200;
    return;
  }
  
// Thank you, SXu
  if ((RP_dat[0] < 2) && (RP_dat[0] != RQ_dat[0])) {
    *err = 0x0300;
    return;
  }
}

//~----------------------------------------------------------------------------

void reset_USCM_status(int8 which, int16 *err) {

  int32 RQ_typ = 0x02;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1];

  if (which > 1) {
    *err = 0x0100;
    return;
  }
  
  RQ_dat[0] = which;
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void get_short_status(int8 *dat, int16 *err) {

  get_USCM_status(0, dat, err);
}

//~----------------------------------------------------------------------------

void reset_short_status(int16 *err) {

  reset_USCM_status(0, err);
}

//~----------------------------------------------------------------------------

void get_long_status(int8 *dat, int16 *err) {

  get_USCM_status(1, dat, err);
}

//~----------------------------------------------------------------------------

void reset_long_status(int16 *err) {

  reset_USCM_status(1, err);
}

//~============================================================================

void get_SXu_status(int8 *dat, int16 *err) {

  int32 RQ_typ = 0x02;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1] = {0};
  int8 *RP_dat = dat;
  int32 RP_siz = 14;
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;
  
  if (RP_cnt != RP_siz) *err  = 0x0100;
}

//~----------------------------------------------------------------------------

void reset_SXu_status(int16 *err) {

  int32 RQ_typ = 0x02;
  int32 RQ_cnt = 0;
  int8  RQ_dat[1] = {0};

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================
//
//                  CONFIGURATION FILE COMMANDS
//
//~============================================================================

void read_config_log(int siz, int *n, int8 *dat, int16 *err) {

  int32 RQ_typ = 0x03;
  int32 RP_siz = siz;
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt,    dat,     err);

  if (*err || use_MEM) return;
  
  *n = RP_cnt;
}

//~----------------------------------------------------------------------------

void exec_config_file(int16 *err) {

  int32 RQ_typ = 0x03;

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ,      0,   NULL,      0,    NULL,   NULL,     err);
}

//~============================================================================
//
//                  CONSOLE COMMANDS (for HBB software only)
//
//~============================================================================

void read_console(int *n, int8 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x04;
  int8  RP_dat[8];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  *n = 0;

  if (*err || use_MEM) return;

  *n = RP_cnt;
  for (i=0; i<*n; i++) {
    dat[i] = RP_dat[i];
  }
}

//~----------------------------------------------------------------------------

void write_console(int n, int8 *dat, int16 *err) {

  int32 RQ_typ = 0x04;

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ,      n,    dat,      0,    NULL,   NULL,     err);
}

//~============================================================================
//
//                  MEMORY COMMANDS
//
//~============================================================================

void read_USCM_memory(int n, int32 adr, int8 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x06;
  int8  RQ_dat[5];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8  RP_dat[8192+5];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;
  int nn;
  int32 adradr;

  RQ_dat[0] = n >> 8;
  RQ_dat[1] = n & 0xFF;
  RQ_dat[2] = adr >> 16;
  RQ_dat[3] = adr >> 8;
  RQ_dat[4] = adr & 0xFF;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;

  nn     = (int16)RP_dat[0] << 8 | (int16)RP_dat[1];
  adradr = (int32)RP_dat[2] << 16 | (int32)RP_dat[3] << 8 |(int32)RP_dat[4];

  if (RP_cnt != nn + 5) {
    *err = 0x0300;
    return;
  }
  
  if (n != nn) {
    *err = 0x0400;
    return;
  }

  if (adr != adradr) {
    *err = 0x0500;
    return;
  }

  for (i=0; i<n; i++) {
    dat[i] = RP_dat[i+5];
  }
}

//~----------------------------------------------------------------------------

void write_USCM_memory(int n, int32 adr, int8 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x06;
  int8  RQ_dat[8192+5];
  int32 RQ_cnt;
  
  RQ_cnt = 5 + n;
  
  if (RQ_cnt > 8192+5) {
    *err = 0x0300;
    return;
  }

  RQ_dat[0] = n >> 8;
  RQ_dat[1] = n & 0xFF;
  RQ_dat[2] = adr >> 16;
  RQ_dat[3] = adr >> 8;
  RQ_dat[4] = adr & 0xFF;
  for (i=0; i<n; i++) {
    RQ_dat[i+5] = dat[i];
  }
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================

void start_FLASH_erasure(int8 sector, int16 *err) {

  int32 RQ_typ = 0x05;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1];

  RQ_dat[0] = sector;

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void check_FLASH_erasure(bool *done, int16 *err) {

  int32 RQ_typ = 0x05;
  int8  RP_dat[2];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;

  if (RP_cnt != 2) {
    *err = 0x0500;
    return;
  }

  if (RP_dat[1] == 0x00) *done = TRUE;
  else                   *done = FALSE;
}

//~----------------------------------------------------------------------------

void validate_FLASH(
     int32 begin, int32 *bgn, int32 *end, int16 *chk_1, int16 *chk_2, int16 *err) {

  int32 RQ_typ = 0x07;
  int32 RQ_cnt = 3;
  int8  RQ_dat[3];
  int8  RP_dat[10];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  RQ_dat[0] = begin >> 16;
  RQ_dat[1] = begin >>  8;
  RQ_dat[2] = begin >>  0;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;

  if ((RP_cnt != 8) && (RP_cnt != 10)) {     // Thank you, HBB & SXU
    *err = 0x0600;
    return;
  }

  if (bgn) *bgn = (int32)RP_dat[0] << 16 |
                  (int32)RP_dat[1] <<  8 |
                  (int32)RP_dat[2] <<  0;
  if (end) *end = (int32)RP_dat[3] << 16 |
                  (int32)RP_dat[4] <<  8 |
                  (int32)RP_dat[5] <<  0;
  if (chk_1) *chk_1 = (int16)RP_dat[6] << 8 |
                      (int16)RP_dat[7] << 0;
  if (RP_cnt == 10) {
    if (chk_2) *chk_2 = (int16)RP_dat[8] << 8 |
                        (int16)RP_dat[9] << 0;
  }
}

//~============================================================================
//
//                  SERIAL PORTS COMMANDS
//
//~============================================================================

void setup_serial_port(int8 port, int8  mode, int8  rate, int16 *err) {

  int32 RQ_typ = 0x0E;
  int8  RQ_dat[4];
  int32 RQ_cnt = sizeof(RQ_dat);

  if (port > 1) {
    *err = 0x0100;
    return;
  }
  
  if (mode & 0xF8) {
    *err = 0x0200;
    return;
  }
  
  RQ_dat[0] = port;
  RQ_dat[1] = mode;
//RQ_dat[2] = port ? 0x00 : 0xFF;             // Thank you, HBB!
//RQ_dat[2] = port ? baud[1][rate] : 0xFF;    // Thank you, SXu!
//RQ_dat[3] = baud[port][rate];
  RQ_dat[2] = unpack16(baud[port][rate], 0xFF00);
  RQ_dat[3] = unpack16(baud[port][rate], 0x00FF);
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void check_serial_port(int8 port, int8 *mode, int8 *rate, int16 *err) {

  int r;
  int32 RQ_typ = 0x0E;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8  RP_dat[4];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  if (port > 1) {
    *err = 0x0100;
    return;
  }

  RQ_dat[0] = port;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;
  
  if (RP_cnt != 4) {
    *err = 0x0400;
    return;
  }

  if (RP_dat[0] != port) {
    *err = 0x0500;
    return;
  }

  if (mode) *mode = RP_dat[1];

  if (RP_dat[1] & 0xF8) {
    *err = 0x0600;
    return;
  }

//if (RP_dat[2] != (port ? 0x00 : 0xFF)) {   // Thank you, HBB & SXu
//  *err = 0x0700;
//  return;
//}
  
  if (port == 0) {
    if (RP_dat[2] != 0xFF) {
      if (rate) *rate = 4;
      return;
    }
  }
  else {
    if ((RP_dat[2] != 0x00) && (RP_dat[2] != RP_dat[3])) {
      if (rate) *rate = 4;
      return;
    }
  }
  
  for (r=0; r<4; r++) {
    if (RP_dat[3] == unpack16(baud[port][r], 0x00FF)) break;
  }
  if (rate) *rate = r;
}

//~============================================================================

void enable_serial_port( int8 port, int16 *err) {

  setup_serial_port(port, 0x01, 3, err);
}

//~----------------------------------------------------------------------------

void disable_serial_port(int8 port, int16 *err) {

  setup_serial_port(port, 0x00, 3, err);
}

//~----------------------------------------------------------------------------

void flush_serial_port(int8 port, int16 *err) {

  write_serial_port(port, 0, NULL, err);
}

//~============================================================================

void read_serial_port( int8 port, int siz, int *n, int8 *dat, int16 *err) {

  int32 RQ_typ;
  int32 RP_siz = siz;
  int32 RP_cnt;

  if (port > 1) {
    *err = 0x0100;
    return;
  }

  RQ_typ = port ? 0x10 : 0x0F;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt,    dat,     err);

  if (*err || use_MEM) return;
  
  *n = RP_cnt;
}

//~----------------------------------------------------------------------------

int Read_serial_port( int8 port, int siz, int *n, int8 *dat, int16 *err) {

  int z = 0, r = 0, nz = 0, NZ = 4, free = siz;
  
  if (port > 1) {
    *err = 0x0100;
    return 0;
  }

  *n = 0;
  while (1) {
    read_serial_port(port, free, &r, dat+*n, err);
    if (*err) break;
    *n = *n + r;
    free = free - r;
    nz = r ? 0 : nz + 1;
    if (r) z = z + 1;
    if (nz > NZ) break;
  }

  return z;
}

//~============================================================================

void write_serial_port(int8 port, int  n, int8 *dat, int16 *err) {

  int32 RQ_typ;

  if (port > 1) {
    *err = 0x0100;
    return;
  }

  RQ_typ = port ? 0x10 : 0x0F;
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ,      n,    dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void Write_serial_port(int8 port, int  n, int8 *dat, int16 *err) {

  int8 *p = dat;
  int l = n;
  int s;
  
  if (port > 1) {
    *err = 0x0100;
    return;
  }

  while ((s = MIN(serial_RX_buffer_size,l))) {
    write_serial_port(port, s, p, err);
    if (*err) break;
    l = l - s;
    p = p + s;
  }
}

//~============================================================================
//
//                  ADC & DAC COMMANDS
//
//~============================================================================

void read_ADC(int16 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x11;
  int8  RP_dat[64];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;

  if (RP_cnt != 64) {
    *err = 0x0100;
    return;
  }

  for (i=0; i<32; i++) {
    dat[i] = (int16)RP_dat[2*i] << 8 | (int16)RP_dat[2*i+1];
    if (dat[i] & 0xF000) {
      *err = 0x0200;
      return;
    }
  }
}

//~----------------------------------------------------------------------------

void read_DAC(int16 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x12;
  int8  RP_dat[32];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;

  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }

  for (i=0; i<16; i++) {
    dat[i] = (int16)RP_dat[2*i] << 8 | (int16)RP_dat[2*i+1];
    if (dat[i] & 0xF000) {
      *err = 0x0200;
      return;
    }
  }
}

//~----------------------------------------------------------------------------

void write_DAC(int16 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x12;
  int8  RQ_dat[32];
  int32 RQ_cnt = sizeof(RQ_dat);

  for (i=0; i<16; i++) {
    RQ_dat[2*i  ] = dat[i] >> 8;
    RQ_dat[2*i+1] = dat[i] & 0x00FF;
  }

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================
//
//                  DALLAS TEMPERATURE SENSORS COMMANDS
//
//~============================================================================

void start_DS1820_scan(int8 bus, int16 *err) {

  int32 RQ_typ = 0x15;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);

  RQ_dat[0] = bus;

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void check_DS1820_scan(int8 *bus, int8 *n, int16 *err) {

  int i;
  int32 RQ_typ = 0x15;
  int8  RP_dat[10];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;
  
  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }
  
  if (bus) *bus = RP_dat[0];

  if (n) {
    for (i=0; i<9; i++) {
      n[i] = RP_dat[i+1];
    }
  }
}

//~----------------------------------------------------------------------------

void read_DS1820_table(int8 bus, int8 *n, int64 *ID, int8 *prio, int16 *err) {

  int i, j;
  int32 RQ_typ = 0x17;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8  RP_dat[577];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  RQ_dat[0] = bus;
 
//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;

  if (((RP_cnt - 1) % 9) != 0) {
    *err = 0x0100;
    return;
  }
  
  if (bus != RP_dat[0]) {
    *err = 0x0200;
    return;
  }

  *n = (RP_cnt - 1) / 9;

  for (i=0; i<*n; i++) {
    if (ID) {
      ID[i] = 0;
      for (j=0; j<8; j++) {
//      ID[i] = ID[i] << 8 | (int64)RP_dat[9*i+1+j];
        ID[i] = ID[i] << 8 | (int64)RP_dat[1+9*i+7-j];
      }
    }
    if (prio) prio[i] = RP_dat[1+9*i+8];
  }
}

//~----------------------------------------------------------------------------

void write_DS1820_table(int8 bus, int8 n, int64 *ID, int8 *prio, int16 *err) {

  int i, j;
  int32 RQ_typ = 0x17;
  int8  RQ_dat[577];
  int32 RQ_cnt = 1 + 9 * n;
  
  RQ_dat[0] = bus;
  
  if (n > 64) {
    *err = 0x0100;
    return;
  }
  
  for (i=0; i<n; i++) {
    for (j=0; j<8; j++) {
//    RQ_dat[9*i+1+j] = ID[i] >> (8 * (7 - j));
      RQ_dat[1+9*i+7-j] = ID[i] >> (8 * (7 - j));
    }
    if (prio) RQ_dat[1+9*i+8] = prio[i];
    else      RQ_dat[1+9*i+8] = 9;
  }

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================

void setup_DS1820_readout(
     int8 ena, int8 rst, int8 man, int8 mdw, int8 par, int16 *err) {

  int32 RQ_typ = 0x16;
  int8  RQ_dat[5];
  int32 RQ_cnt = sizeof(RQ_dat);

  RQ_dat[0] = ena;
  RQ_dat[1] = rst;
  RQ_dat[2] = man;
  RQ_dat[3] = mdw;
  RQ_dat[4] = par;

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void check_DS1820_readout(
     int8 *ena, int8 *par, int8 *sta, int8 *man, int8 *mdw, int8 *mdr, int16 *err) {

  int32 RQ_typ = 0x16;
  int8  RP_dat[8];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;

  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }
  
  if (ena) *ena = RP_dat[0];
  if (par) *par = RP_dat[1];
  if (sta) {
    sta[0] = unpack16((int16)RP_dat[2], 0x00C0);
    sta[1] = unpack16((int16)RP_dat[2], 0x0030);
    sta[2] = unpack16((int16)RP_dat[2], 0x000C);
    sta[3] = unpack16((int16)RP_dat[2], 0x0003);
    sta[4] = unpack16((int16)RP_dat[3], 0x00C0);
    sta[5] = unpack16((int16)RP_dat[3], 0x0030);
    sta[6] = unpack16((int16)RP_dat[3], 0x000C);
    sta[7] = unpack16((int16)RP_dat[3], 0x0003);
    sta[8] = unpack16((int16)RP_dat[4], 0x00C0);
  }
  if (man) *man = RP_dat[5];
  if (mdw) *mdw = RP_dat[6];
  if (mdr) *mdr = RP_dat[7];
}

//~============================================================================

void read_DS1820_temp_short(
     int8 bus, int8 *n, bool *ok, float *tmp, int16 *err) {

  int i;
  int32 RQ_typ = 0x18;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8  RP_dat[65];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  RQ_dat[0] = bus;
 
//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;

  if (bus != RP_dat[0]) {
    *err = 0x0200;
    return;
  }

  *n = RP_cnt - 1;

  for (i=0; i<*n; i++) {
    if (ok) {
      if (RP_dat[i+1] != 0x80) ok[i] = OK;
      else                     ok[i] = KO;
    }
    if (tmp) tmp[i] = (float)(char)RP_dat[i+1] / 2.0;
  }
}

//~----------------------------------------------------------------------------

void read_DS1820_temp_long(
     int8 bus, int8 *n, bool *ok, float *tmp, int16 *age, int16 *err) {

  int i;
  int32 RQ_typ = 0x19;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8  RP_dat[1+4*64];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  RQ_dat[0] = bus;
 
//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;

  if (bus != RP_dat[0]) {
    *err = 0x0200;
    return;
  }

  if ((RP_cnt - 1) % 4) {
    *err = 0x0300;
    return;
  }

  *n = (RP_cnt - 1) / 4;

  for (i=0; i<*n; i++) {
    int16 temp = (int16)RP_dat[1+4*i+0] << 8 | (int16)RP_dat[1+4*i+1];
    if (temp != 0x8000) {
      if (ok)  ok[i] = TRUE;
      if (tmp) tmp[i] = (float)(short)temp / 256.0 - 0.25;
    }
    else {
      if (ok) ok[i] = FALSE;
    }
    if (age) {
      age[i] = (int16)RP_dat[1+4*i+2] << 8 |
               (int16)RP_dat[1+4*i+3];
    }
  }
}

//~----------------------------------------------------------------------------

void read_DS1820_temp_raw(
     int8 bus, int8 *n, _DS1820_data *dat, int16 *err) {

  int i, j;
  int N = 10;
  int32 RQ_typ = 0x19;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8  RP_dat[1+N*64];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  RQ_dat[0] = bus | 0x80;
 
//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;

  if (RQ_dat[0] != RP_dat[0]) {
    *err = 0x0200;
    return;
  }

  if ((RP_cnt - 1) % N) {
    *err = 0x0300;
    return;
  }

  *n = (RP_cnt - 1) / N;

  for (i=0, j=1; i<*n; i++) {
    _DS1820_data *p = &dat[i];
    p->lsb    = RP_dat[j++];
    p->msb    = RP_dat[j++];
    p->cfg    = RP_dat[j++];
    p->remain = RP_dat[j++];
    p->perc   = RP_dat[j++];
    p->temp1  = RP_dat[j++];
    p->temp2  = (int16)RP_dat[j++] << 8;
    p->temp2 |= (int16)RP_dat[j++];
    p->age    = (int16)RP_dat[j++] << 8;
    p->age   |= (int16)RP_dat[j++];
    if (p->temp1 != 0x80) {
      p->ok_short = TRUE;
      p->temp_short = (float)(char)p->temp1 / 2.0;
    }
    else {
      p->ok_short = FALSE;
    }
    if (p->temp2 != 0x8000) {
      p->ok_long = TRUE;
      p->temp_long = (float)(short)p->temp2 / 256.0 - 0.25;
    }
    else {
      p->ok_long = FALSE;
    }
  }
}

//~============================================================================
//
//                  LVDS BUS COMMANDS
//
//~============================================================================

void read_LVDS_bus(int8 adr, int16 *dat, int16 *err) {

  int32 RQ_typ = 0x13;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1];
  int8  RP_dat[3];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  RQ_dat[0] = adr;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;
  
  if (RP_cnt != 3) {
    *err = 0x0400;
    return;
  }

  if (RQ_dat[0] != RP_dat[0]) {
    *err = 0x0500;
    return;
  }

  *dat = (int16)RP_dat[1]<<8 | (int16)RP_dat[2];
}

//~----------------------------------------------------------------------------

void Read_LVDS_bus(int8 adr, int n, int16 *dat, int16 *err) {

  int32 RQ_typ = 0x13;
  int8  RQ_dat[3];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8  RP_dat[1+2*1024];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;
  int i;

  RQ_dat[0] = adr;
  RQ_dat[1] = ((int16)n & 0xFF00) >> 8;
  RQ_dat[2] =  (int16)n & 0x00FF;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;
  
  if (RP_cnt != (1 + 2 * n)) {
    *err = 0x0400;
    return;
  }

  if (RQ_dat[0] != RP_dat[0]) {
    *err = 0x0500;
    return;
  }

  for (i=0; i<n; i++) {
    dat[i] = (int16)RP_dat[1+2*i]<<8 | (int16)RP_dat[1+2*i+1];
  }
}

//~----------------------------------------------------------------------------

void write_LVDS_bus(int8 adr, int16 dat, int16 *err) {

  int32 RQ_typ = 0x13;
  int32 RQ_cnt = 3;
  int8  RQ_dat[3];

  RQ_dat[0] = adr;
  RQ_dat[1] = dat >> 8;
  RQ_dat[2] = dat & 0x00FF;

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================
//
//                  LECROY COMMANDS
//
//~============================================================================

void init_LeCroy_bus(int8 bus, int16 *err) {

  int32 RQ_typ = 0x14;
  int32 RQ_cnt = 1;
  int8  RQ_dat[1];

  RQ_dat[0] = bus;

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void exec_LeCroy_cmd(
     int n, int8 *bus, int32 *wri, int32 *rea, bool *chk, int16 *err) {

  int n_max = 10;
  int32 RQ_typ = 0x14;
  int32 RQ_cnt = 5 * n;
  int8  RQ_dat[n_max*9];
  int8  RP_dat[n_max*9];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;
  int i;
  int8 *p;
  int8  busbus[n_max];
  int32 wriwri[n_max];

  if (n < 1 || n > n_max) {
    *err = 0x0300;
    return;
  }

  p = RQ_dat;
  for (i=0; i<n; i++) {
    *p++ = bus[i];
    *p++ = wri[i] >> 24;
    *p++ = wri[i] >> 16;
    *p++ = wri[i] >>  8;
    *p++ = wri[i];
  }

  if (ESPIONAGE) {
    int32 r_w, brd, adr, ext, dat;
    r_w = unpack32(wri[0], 0x00080000);
    brd = swapbits(unpack32(wri[0], 0x00700000),  3);
    adr = swapbits(unpack32(wri[0], 0x0F800000),  5);
    ext = swapbits(unpack32(wri[0], 0x00070000),  3);
    dat = swapbits(unpack32(wri[0], 0x0000FFFF), 16);
    printf(
      "\n*** RQ: bus=%02hX wri=%08X r_w=%d brd=%d adr=%2d ext=%d dat=%04X=%-5d\n",
      bus[0], wri[0], r_w, brd, adr, ext, dat, dat);
  }

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (ESPIONAGE) {
    printf("*** RP: err=%04X\n", *err);
  }

  if (*err || use_MEM) return;
  
  if (RP_cnt != 9 * n) {
    *err = 0x0400;
    return;
  }

  p = RP_dat;
  for (i=0; i<n; i++) {
    busbus[i] = *p++;
    if (bus[i] != (busbus[i] & 0x7F)) {
      *err = 0x0500;
      return;
    }
    wriwri[i]  = (int32)(*p++) << 24;
    wriwri[i] |= (int32)(*p++) << 16;
    wriwri[i] |= (int32)(*p++) <<  8;
    wriwri[i] |= (int32)(*p++);
    if (wri[i] != wriwri[i]) {
      *err = 0x0600;
      return;
    }
  }
  for (i=0; i<n; i++) {
    rea[i]  = (int32)(*p++) << 24;
    rea[i] |= (int32)(*p++) << 16;
    rea[i] |= (int32)(*p++) <<  8;
    rea[i] |= (int32)(*p++);
    chk[i] = busbus[i] >> 7;
  }

  if (ESPIONAGE) {
    int32 r_w, brd, adr, ext, dat;
    printf("*** RP: chk=%d\n", chk[0]);
    r_w = unpack32(rea[0], 0x00080000);
    brd = swapbits(unpack32(rea[0], 0x00700000),  3);
    adr = swapbits(unpack32(rea[0], 0x0F800000),  5);
    ext = swapbits(unpack32(rea[0], 0x00070000),  3);
    dat = swapbits(unpack32(rea[0], 0x0000FFFF), 16);
    printf(
      "*** RP: bus=%02hX rea=%08X r_w=%d brd=%d adr=%2d ext=%d dat=%04X=%-5d\n",
      bus[0], rea[0], r_w, brd, adr, ext, dat, dat);
  }
}

//~============================================================================
//
//                  CCEB SPECIFIC COMMANDS
//
//~============================================================================

void setup_CCBT_task(int8 ctl, int16 *err) {

  int32 RQ_typ = 0x1B;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);

  RQ_dat[0] = ctl;
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void read_CCBT_data(_CCBT_data *dat, int16 *err) {

  int i, j;
  int N = 1 + 1 + 2 + 2 * 32;
  int32 RQ_typ = 0x1B;
  int8  RP_dat[2*N];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;
  
  if (RP_cnt != 2*N) {
    *err = 0x0100;
    return;
  }

  for (j=0; j<2; j++) {
    _CCBT_data *p = &dat[j];
    p->use  = RP_dat[N*j+0];
    p->ok   = RP_dat[N*j+1];
    p->age  = (int16)RP_dat[N*j+2] << 8;
    p->age |= (int16)RP_dat[N*j+3] << 0;
    for (i=0; i<32; i++) {
      p->value[i]  = (int16)RP_dat[N*j+4+2*i+0] << 8;
      p->value[i] |= (int16)RP_dat[N*j+4+2*i+1] << 0;
    }
  }
}

//~============================================================================

void setup_16ms_task(int  n, __16ms_par *par, int16 *err) {

  int i, j;
  int32 RQ_typ = 0x1A;
  int8  RQ_dat[12*32];
  int32 RQ_cnt = 12 * n;

  if (n > 32) {
    *err = 0x0100;
    return;
  }

  for (i=0; i<n; i++) {
    __16ms_par *p = &par[i];
    RQ_dat[12*i+0] = p->cha;
    RQ_dat[12*i+1] = p->thr >> 8;
    RQ_dat[12*i+2] = p->thr;
    RQ_dat[12*i+3] = p->pro;
    for (j=0; j<8; j++) RQ_dat[12*i+4+j] = p->par[j];
  }
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void check_16ms_task(int *n, __16ms_par *par, int16 *err) {

  int i, j;
  int32 RQ_typ = 0x1A;
  int8  RP_dat[12*32];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;
  
  if (RP_cnt % 12) {
    *err = 0x0100;
    return;
  }

  if ((RP_cnt / 12) > 32) {
    *err = 0x0200;
    return;
  }

  *n = RP_cnt / 12;

  for (i=0; i<*n; i++) {
    __16ms_par *p = &par[i];
    p->cha  = RP_dat[12*i+0];
    p->thr  = (int16)RP_dat[12*i+1] << 8;
    p->thr |= (int16)RP_dat[12*i+2] << 0;
    p->pro  = RP_dat[12*i+3];
    for (j=0; j<8; j++) p->par[j] = RP_dat[12*i+4+j];
  }
}

//~----------------------------------------------------------------------------

void run_16ms_task_procedure(int8 pro, int8 *par, int16 *err) {

  int i;
  int32 RQ_typ = 0x1A;
  int8  RQ_dat[1+8];
  int32 RQ_cnt = sizeof(RQ_dat);

  RQ_dat[0] = pro;
  for (i=0; i<8; i++) RQ_dat[1+i] = par[i];

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================

#define DEBUG_THIS_FUNCTION FALSE

void write_CCEB_task_controls(
  int32 *fai_chk, int32 *ala_chk, int32 *fai_lim, int32 *ala_lim, int16 *err) {

  int i;
  int32 RQ_typ = 0x1C;
  int8  RQ_dat[1+4*(2+2+16/2+16/2)];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8 *p = RQ_dat;

  *p++ = 0x01;

  for (i=0; i<4; i++) {
    int j;
    *p++ = *fai_chk   >> 8;
    *p++ = *fai_chk++ >> 0;
    *p++ = *ala_chk   >> 8;
    *p++ = *ala_chk++ >> 0;
    for (j=0; j<8; j++) {
      *p     = (*fai_lim++ & 0x0F) << 4;
      *p++  |= (*fai_lim++ & 0x0F) << 0;
    }
    for (j=0; j<8; j++) {
      *p     = (*ala_lim++ & 0x0F) << 4;
      *p++  |= (*ala_lim++ & 0x0F) << 0;
    }
  }

#if DEBUG_THIS_FUNCTION
  printf("write_CCEB_task_controls_new\n");
  printf("sizeof(RQ_dat) = %d\n", sizeof(RQ_dat));
  printf("p-RQ_dat       = %d\n", p-RQ_dat);
  for (i=0; i<sizeof(RQ_dat); i++) {
    int j;
    for (j=0; j<4; j++) if (i==(1+j*(2+8+2+8))) printf("\n");
    printf("%02hX ", RQ_dat[i]);
  }
  printf("\n");
  *err = 0;
#else
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
#endif
}

#undef DEBUG_THIS_FUNCTION

//~----------------------------------------------------------------------------

void write_CCEB_task_target_values(int8 *phase, int32 *power, int16 *err) {

  int i;
  int32 RQ_typ = 0x1C;
  int8  RQ_dat[21];
  int32 RQ_cnt = sizeof(RQ_dat);

  RQ_dat[0] = 0x02;
  for (i=0; i<4; i++) {
    RQ_dat[5*i+1] = phase[i];
    RQ_dat[5*i+2] = power[i] >> 24;
    RQ_dat[5*i+3] = power[i] >> 16;
    RQ_dat[5*i+4] = power[i] >>  8;
    RQ_dat[5*i+5] = power[i] >>  0;
  }
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void write_CCEB_task_target_value(
     int8 cha, int8 phase, int32 power, int16 *err) {

  int32 RQ_typ = 0x1C;
  int8  RQ_dat[7];
  int32 RQ_cnt = sizeof(RQ_dat);

  RQ_dat[0] = 0x03;
  RQ_dat[1] = cha;
  RQ_dat[2] = phase;
  RQ_dat[3] = power >> 24;
  RQ_dat[4] = power >> 16;
  RQ_dat[5] = power >>  8;
  RQ_dat[6] = power >>  0;
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================

void send_CCEB_task_command(int8 cha, int8 cmd, int32 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x1C;
  int8  RQ_dat[13];
  int32 RQ_cnt;

  RQ_dat[0] = 0x00;
  
  RQ_dat[1] = cha;
  RQ_dat[2] = cmd;

  RQ_cnt = 3;
  if (dat) {
    for (i=0; i<5; i++) {
      RQ_dat[2*i+3] = dat[i] >> 8;
      RQ_dat[2*i+4] = dat[i];
      RQ_cnt = RQ_cnt + 2;
    }
  }
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void read_CCEB_task_status(int8 *dat, int16 *err) {

  int32 RQ_typ = 0x1C;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8 *RP_dat = dat;
  int32 RP_siz = 373;
  int32 RP_cnt;

  RQ_dat[0] = 0x01;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;

  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }
  if (RP_dat[0] != RQ_dat[0]) {
    *err = 0x0200;
    return;
  }
}

//~============================================================================
//
//      PDS SPECIFIC FUNCTIONS
//
//~============================================================================

void enable_CGS_serial_lines(int16 *err) {

  int8 dat = 0x00;
  
//write_USCM_memory(int n, int32 adr, int8 *dat, int16 *err);
  write_USCM_memory(    1,  0x400091,      &dat,        err);
}

//~----------------------------------------------------------------------------

void disable_CGS_serial_lines(int16 *err) {

  int8 dat = 0x02;
  
//write_USCM_memory(int n, int32 adr, int8 *dat, int16 *err);
  write_USCM_memory(    1,  0x400091,      &dat,        err);
}

//~----------------------------------------------------------------------------

void exec_PDS_cmd(int16 cmd, int8 adc_cha, int8 *dig, int16 *adc_dat, int16 *err) {

  exec_new_PDS_cmd(cmd, adc_cha, 0x01, dig, adc_dat, err);
}

//~----------------------------------------------------------------------------

void exec_new_PDS_cmd
      (int16 cmd, int8 adc_cha, int8 del, int8 *dig, int16 *adc_dat, int16 *err) {

  int32 RQ_typ = 0x0C;
  int8  RQ_dat[4];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8  RP_dat[7];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  RQ_dat[0] = cmd & 0x00FF;
  RQ_dat[1] = cmd >> 8;
  RQ_dat[2] = adc_cha;
  RQ_dat[3] = del;
  
//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;
  
  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }
  
  if (RP_dat[0] != RQ_dat[0] || RP_dat[1] != RQ_dat[1] || 
      RP_dat[2] != RQ_dat[2] || RP_dat[3] != RQ_dat[3]) {
    *err = 0x0200;
    return;
  }

  if (adc_dat) *adc_dat = (RP_dat[4] << 8) | RP_dat[5];

  if (dig) *dig = RP_dat[6] & 0x01;
}

//~============================================================================
//
//                  TTCE SPECIFIC COMMANDS
//
//~============================================================================

void check_TTCE_FLASH_erasure(int8 sector, bool *done, int16 *err) {

  int8 dat[2];
  int32 adr = 0x100000 + sector * 0x010000;
  
  read_USCM_memory(2, adr, dat, err);

  if (*err) return;
  
  *done = (dat[0] == dat[1]) ? TRUE : FALSE;
}


//~----------------------------------------------------------------------------

void setup_TTCE_28VDC_power(int8 *dat, int16 *err) {

  int32 RQ_typ = 0x07;
  int32 RQ_cnt = 8;
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt,    dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void check_TTCE_28VDC_power(int8 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x07;
  int8  RP_dat[5];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }
  
  if (dat) for (i=0; i<RP_cnt; i++) dat[i] = RP_dat[i];
}

//~============================================================================

void check_TTCE_DS1820_scan(int8 *bus, int8 *n, int16 *err) {

  int i;
  int32 RQ_typ = 0x15;
  int8  RP_dat[5];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err) return;
  
  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }
  
  if (bus) *bus = RP_dat[0];

  if (n) {
    for (i=0; i<4; i++) {
      n[i] = RP_dat[i+1];
    }
  }
}

//~----------------------------------------------------------------------------

void read_TTCE_DS1820_table(int8 bus, int8 *n, int64 *ID, int16 *err) {

  int i, j;
  int32 RQ_typ = 0x17;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8  RP_dat[513];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  RQ_dat[0] = bus;
 
//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;

  if (((RP_cnt - 1) % 8) != 0) {
    *err = 0x0100;
    return;
  }
  
  if (bus != RP_dat[0]) {
    *err = 0x0200;
    return;
  }

  *n = (RP_cnt - 1) / 8;

  for (i=0; i<*n; i++) {
    if (ID) {
      ID[i] = 0;
      for (j=0; j<8; j++) {
       ID[i] = ID[i] << 8 | (int64)RP_dat[1+8*i+7-j];
      }
    }
  }
}

//~----------------------------------------------------------------------------

void write_TTCE_DS1820_table(int8 bus, int8 n, int64 *ID, int16 *err) {

  int i, j;
  int32 RQ_typ = 0x17;
  int8  RQ_dat[513];
  int32 RQ_cnt = 1 + 8 * n;
  
  RQ_dat[0] = bus;
  
  if (n > 64) {
    *err = 0x0100;
    return;
  }
  
  for (i=0; i<n; i++) {
    for (j=0; j<8; j++) {
      RQ_dat[1+8*i+j] = ID[i] >> (8 * j);
    }
  }

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void setup_TTCE_DS1820_readout(int8 ena, int16 *err) {

  int32 RQ_typ = 0x16;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);

  RQ_dat[0] = ena;

//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void check_TTCE_DS1820_readout(int8 *ena, int16 *err) {

  int32 RQ_typ = 0x16;
  int8  RP_dat[1];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err) return;

  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }
  
  if (ena) *ena = RP_dat[0];
}

//~----------------------------------------------------------------------------

void read_TTCE_DS1820_temp(int8 bus, int8 *n, bool *ok, float *temp, int16 *err) {

  int i;
  int32 RQ_typ = 0x18;
  int8  RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat);
  int8  RP_dat[1+2*64];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  RQ_dat[0] = bus;


//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;

  if (bus != RP_dat[0]) {
    *err = 0x0200;
    return;
  }
  
  *n = (RP_cnt - 1) / 2;

  for (i=0; i<*n; i++) {
    if (ok) {
      if (RP_dat[2+2*i] & 0x80) ok[i] = KO;
      else                      ok[i] = OK;
    }
    if (temp) temp[i] = (char)RP_dat[1+2*i] + RP_dat[2+2*i] / 16.0 - 0.25;
  }
}

//~============================================================================

void write_TTCE_Pt1000_ctrl(int8 n, int8 *adr, int8 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x19;
  int8  RQ_dat[10];
  int32 RQ_cnt = n;
  
  if (n > 10) {
    *err = 0x0100;
    return;
  }
  
  for (i=0; i<n; i++) {
    if (adr[i] > 9) {
      *err = 0x0200;
      return;
    }
    if (dat[i] & 0xF8) {
      *err = 0x0300;
      return;
    }
    RQ_dat[i] = ((dat[i] & 0x03) << 4) | (adr[i] & 0x0F);
  }
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void read_TTCE_Pt1000_ADC(int16 *adc, int16 *err) {

  int i;
  int8 dat[144];

  read_USCM_memory(144, 0x50000, dat, err);

  if (*err) return;

  for (i=0; i<72; i++) {
    if (adc) adc[i] = (dat[2*i]<<8) | dat[2*i+1];
  }
}

//~----------------------------------------------------------------------------

void read_TTCE_Pt1000_temp(int8 *RC, int8 *VR, float *temp, int16 *err) {

  int i;
  int32 RQ_typ = 0x19;
  int32 RQ_cnt = 0;
  int8  RP_dat[44];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;

  for (i=0; i<22; i++) {
    if (temp) temp[i] = (signed char)RP_dat[2*i] + (RP_dat[2*i+1] & 0x0F)/16.0;
    if (RC)   RC[i]   = unpack16(RP_dat[2*i+1], 0x0030);
    if (VR)   VR[i]   = unpack16(RP_dat[2*i+1], 0x00C0);
  }
}

//~============================================================================

void read_TTCE_pressure(int16 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x1A;
  int32 RQ_cnt = 0;
  int8  RP_dat[8];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;

  if (RP_cnt != RP_siz) {
    *err = 0x0300;
    return;
  }

  for (i=0; i<RP_cnt/2; i++) {
    if (dat) dat[i] = ((int16)RP_dat[2*i] << 4) | (RP_dat[2*i+1] & 0x0F);
  }
}

//~============================================================================

void write_TTCE_pump_DAC(int8 *DAC, int16 *err) {

  int32 RQ_typ = 0x08;
  int32 RQ_cnt = 2;
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt,    DAC,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void read_TTCE_pump_data(int8 *DAC, int16 *RPM, int16 *err) {

  int32 RQ_typ = 0x08;
  int8  RP_dat[6];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }

  if (DAC) {
    DAC[0] = RP_dat[0];
    DAC[1] = RP_dat[1];
  }
  
  if (RPM) {
    RPM[0] = ((int16)RP_dat[2] << 4) | (RP_dat[3] & 0x0F);
    RPM[1] = ((int16)RP_dat[4] << 4) | (RP_dat[5] & 0x0F);
  }
}

//~============================================================================

void write_TTCE_PWM(int8 n, int8 *adr, int8 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x09;
  int8  RQ_dat[12];
  int32 RQ_cnt = 2 * n;
  
  for (i=0; i<n; i++) {
    RQ_dat[2*i+0] = adr[i];
    RQ_dat[2*i+1] = dat[i];
  }
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void read_TTCE_PWM(int8 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x09;
  int32 RQ_cnt = 0;
  int8  RP_dat[6];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;

  if (RP_cnt != RP_siz) {
    *err = 0x0300;
    return;
  }

  for (i=0; i<RP_cnt; i++) {
    if (dat) dat[i] = RP_dat[i];
  }
}

//~============================================================================

void write_TTCE_loop_ctrl(int8 n, int8 *adr, int8 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x0A;
  int8  RQ_dat[128];
  int32 RQ_cnt = 2 * n;
  
  for (i=0; i<n; i++) {
    RQ_dat[2*i+0] = adr[i];
    RQ_dat[2*i+1] = dat[i];
  }
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void read_TTCE_loop_ctrl(int8 *ctrl, int16 *err) {

  int i;
  int32 RQ_typ = 0x0A;
  int32 RQ_cnt = 0;
  int8  RP_dat[64];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;

  if (RP_cnt != RP_siz) {
    *err = 0x0300;
    return;
  }

  for (i=0; i<RP_cnt; i++) {
    if (ctrl) ctrl[i] = RP_dat[i];
  }
}

//~============================================================================
//
//                  CAB SPECIFIC COMMANDS
//
//~============================================================================

void exec_CAB_command(int8 *cmd, int8 *cnt, int8 *rep, int16 *err) {

  int32 RQ_typ = 0x0A;
  int8  RQ_dat[255];
  int32 RQ_cnt;
  int8  RP_dat[255];
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;
  int16 CRC;
  int8  n;
  
  RQ_cnt = cmd[2] - 1;
  memcpy(RQ_dat, &cmd[1], RQ_cnt);
  CRC = CRC_CCITT(cmd, RQ_cnt+1);

  CAB_CMD_ID = RQ_dat[0];
  
  RQ_dat[RQ_cnt++] = CRC >> 8;
  RQ_dat[RQ_cnt++] = CRC >> 0;
  
//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err) return;

  *err = check_CAB_reply(RQ_typ, RQ_dat, RP_cnt, RP_dat);

  if (*err || use_MEM) return;
  
  n = RP_dat[2] - 4;
  if (cnt) *cnt = n;
  if (rep) memcpy(rep, &RP_dat[4], n);
}

//~----------------------------------------------------------------------------

int16 check_CAB_reply(int32 RQ_typ, int8 *RQ_dat, int32 RP_cnt, int8 *RP_dat) {

  int8 CAN_BHDR = RP_dat[0];
  int8 RESP_ID  = RP_dat[1];
  int8 RESP_LEN = RP_dat[2];
  int8 CMD_ACK  = RP_dat[3];

  if (RP_cnt < 6)                                             return 0x0210;
  if (CRC_CCITT(RP_dat, RP_cnt))                              return 0x0220;
  if (CAN_BHDR != (0x80 | RQ_typ))                            return 0x0230;
  if (RESP_ID  != (((RQ_dat[0]>>4) | (RQ_dat[0]<<4)) & 0xFF)) return 0x0240;
  if (RP_cnt != RESP_LEN + 2)                                 return 0x0250;
  if (CMD_ACK)                                                return 0x2000+CMD_ACK;

  return 0x0000;
}

//~============================================================================
//
//                  MAIN INTERFACE FUNCTION TO USCM
//
//~============================================================================

void to_USCM(int R_W,
             int32 RQ_typ, int32  RQ_cnt, int8 *RQ_dat,
             int32 RP_siz, int32 *RP_cnt, int8 *RP_dat, int16 *RP_err) {

  static AMSBlock Request, *pRequest = &Request;
  static AMSBlock Reply,   *pReply   = &Reply;

  pRequest->BlockType    = R_W == WRITE ? 1 : 0;
  pRequest->NodeAddress  = (use_TCP || use_EAS) ? NODE_ADR : USCM_ID;
  pRequest->DataType     = RQ_typ;
  pRequest->DataCount    = RQ_cnt;
  pRequest->Data.p8      = RQ_dat;
  pRequest->Data.p16     = NULL;
  pack16(&pRequest->Source, 0xFF00, JMDC_ID);

  pReply->Error       = 0x0000;
  pReply->BufferSize  = RP_siz;
  pReply->Data.p8     = RP_dat;
  pReply->Data.p16    = NULL;
  pReply->BlockType   = 0x0002 | (pRequest->BlockType & 0x0001);
  pReply->NodeAddress = pRequest->NodeAddress;
  pReply->DataType    = pRequest->DataType;
  pReply->DataCount   = 0;
  pReply->Source      = pRequest->Source;

  if (P > 1) print_AMS_data_block("REQUEST:", pRequest);

//~---
  
  if (FALSE) {
  }
  
//~---

  else if (use_MEM) {
    int8 RQ_hdr = RQ_typ | ((R_W == WRITE) ? 0x40 : 0x00);
    int32 begin, end;
    packbytes(&begin, &uscm_mem[4], 4);
    packbytes(&end,   &uscm_mem[8], 4);
    if ((end - begin + 2 + 1 + RQ_cnt) > (0x4000 - 2)) pReply->Error = 0x1000;
    unpackbytes(&uscm_mem[end+1-begin], RQ_cnt+1, 2); 
    memcpy(&uscm_mem[end+3-begin], &RQ_hdr, 1);
    memcpy(&uscm_mem[end+4-begin], RQ_dat, RQ_cnt); 
    end = end + 2 + 1 + RQ_cnt;
    unpackbytes(&uscm_mem[8], end, 4);
  }

//~---

  else if (use_CAN) {
    CANStateMachine(START, pRequest, pReply);
    if (!pReply->Error) {
      while (CANStateMachine(CONTINUE, pRequest, pReply) != DONE);
    }
  }

//~---

  else if (use_TCP) {
    open_TCP_connection(CS_address, CS_port, &pReply->Error);
    if (!pReply->Error) TCP_SND_RCV(pRequest, pReply);
    close_TCP_connection();
  }

//~---

  else if (use_EAS) {
    if (P > 0) printf("PAD: Sending command...\n");
    eAssRequestWithReply(pRequest, pReply);
    pReply->Data.p8 = RP_dat;
    if (RP_dat) memcpy(RP_dat, pReply->Contents, pReply->DataCount);
  }

//~---

  if (RP_cnt) *RP_cnt = pReply->DataCount;
  if (RP_err) *RP_err = pReply->Error;
  if (P > 1) print_AMS_data_block("REPLY:  ", pReply);
}

//~============================================================================
