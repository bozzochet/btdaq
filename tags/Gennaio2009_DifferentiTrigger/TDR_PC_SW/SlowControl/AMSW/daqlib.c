// file daqlib.c
//
// A.Lebedev Apr-2005...
//

//~----------------------------------------------------------------------------

#include "daqlib.h"

//~============================================================================

void read_JLV1_scalers(int16 *dat, int16 *err) {

  int32 RQ_typ = 0x11;
  int16 RQ_dat[2] = {0x4060, 0x2000};
  int32 RQ_cnt = sizeof(RQ_dat) / 2;
  int32 RP_siz = 0x60 + 2;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt,    dat,     err);
  
  if (*err) return;
  
  if (RP_cnt != RP_siz) {
    *err = 0x0200;
    return;
  }
}

//~----------------------------------------------------------------------------

void read_JLV1_FPGA_version_ID(int16 *dat, int16 *err) {

  int32 RQ_typ = 0x11;
  int16 RQ_dat[2] = {0x4002, 0x2060};
  int32 RQ_cnt = sizeof(RQ_dat) / 2;
  int32 RP_siz = 0x02 + 2;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt,    dat,     err);
  
  if (*err) return;
  
  if (RP_cnt != RP_siz) {
    *err = 0x0200;
    return;
  }
}

//~============================================================================

void exec_SDR2_SC_command(
     int R_W, int8 adr, int16 reg, int16 wri, int16 *rea, int16 *sta, int16 *err) {

  int32 RQ_typ = 0x09;
  int16 RQ_dat[2];
  int32 RQ_cnt;
  int16 RP_dat[100];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

  if ((R_W & 0x1) != R_W) {
    *err = 0x0100;
    return;
  }

  if ((adr & 0x07) != adr) {
    *err = 0x0110;
    return;
  }
  
  if ((reg & 0x0FFF) != reg) {
    *err = 0x0120;
    return;
  }
  
  RQ_dat[0]  = (int16)adr << 13;
  RQ_dat[0] |= (int16)(1 - R_W) << 12;
  RQ_dat[0] |= (int16)reg;
  
  if (R_W) {
    RQ_cnt = 2;
    RQ_dat[1] = wri;
  }
  else {
    RQ_cnt = 1;
  }

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (adr == 0 && RP_cnt != 5) {
    *err = 0x0200;
    return;
  }

  if (adr != 0 && RP_cnt != 6) {
    *err = 0x0210;
    return;
  }
  
  if (RQ_dat[0] != RP_dat[0]) {
    *err = 0x0220;
    return;
  }

  if (adr != 0 && RQ_dat[0] != RP_dat[2]) {
    *err = 0x0230;
    return;
  }
  
  if (sta) *sta = RP_dat[1];

  if (rea) *rea = adr ? RP_dat[3] : RP_dat[2];
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

//~============================================================================

void boot_JINF(int16 fh, int16 *err) {

  int32 RQ_typ = 0x00;
  int32 RQ_cnt;
  int16 RQ_dat[1];

  RQ_dat[0] = fh;
  
  if (fh) RQ_cnt = 1;
  else    RQ_cnt = 0;

//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);

  if (*err) return;
}

//~----------------------------------------------------------------------------

void load_JINF_FLASH_file(int16 fh, int16 *err) {

  int32 RQ_typ = 0x06;
  int32 RQ_cnt = 1;
  int16 RQ_dat[1];

  RQ_dat[0] = fh;

//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);

  if (*err) return;
}

//~----------------------------------------------------------------------------

void ping_JINF(int n, int16 *dat, int *nn, int16 *datdat, int16 *err) {

  int32 RQ_typ = 0x0D;
  int32 RQ_cnt = n;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,    dat, 0xFFFF, &RP_cnt, datdat,     err);

  if (*err) return;
  
  if (nn) *nn = MAX(0, RP_cnt-2);
}

//~----------------------------------------------------------------------------

void get_JINF_status(int *fmt, int16 *dat, 
                     int *n, int16 *datdat, int16 *sta, int16 *err) {

  int32 RQ_typ = 0x0C;
  int l = 10;
  int32 RQ_cnt = 0;
  int32 RP_cnt;
  int16 RP_dat[24*(l+2)+2];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int i, j, k;
  
//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;

  if ((RP_cnt - 2) == l) {
    if (fmt) *fmt = 0;
    if (dat) for (i=0; i<l; i++) dat[i] = RP_dat[i];
    return;
  }
  
  if (((RP_cnt - 2) % (l + 2)) == 0) {
    if (fmt) *fmt = 1;
    if (n && datdat && sta) {
      *n = (RP_cnt - 2) / (l + 2);
      for (j=k=0; j<*n; j++) {
        if (RP_dat[k++] != (l + 1)) {
          *err = 0x0120;
          return;
        }
        for (i=0; i<l; i++) datdat[l*j+i] = RP_dat[k++];
        sta[j] = RP_dat[k++];
      }
    }
    return;
  }
  
  *err = 0x0100;
}

//~----------------------------------------------------------------------------

void get_JINF_FLASH_summary(int *fmt, int16 *dat, 
                            int *n, int16 *datdat, int16 *sta, int16 *err) {

  int32 RQ_typ = 0x07;
  int l = 32;
  int32 RQ_cnt = 0;
  int32 RP_cnt;
  int16 RP_dat[24*(l+2)+2];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int i, j, k;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if ((RP_cnt - 2) == l) {
    *fmt = 0;
    for (i=0; i<l; i++) dat[i] = RP_dat[i];
    return;
  }
  
  if (((RP_cnt - 2) % (l + 2)) == 0) {
    *fmt = 1;
    if (n && datdat && sta) {
      *n = (RP_cnt - 2) / (l + 2);
      for (j=k=0; j<*n; j++) {
        if (RP_dat[k++] != (l + 1)) {
          *err = 0x0120;
          return;
        }
        for (i=0; i<l; i++) datdat[l*j+i] = RP_dat[k++];
        sta[j] = RP_dat[k++];
      }
    }
    return;
  }
  
  *err = 0x0100;
}

//~----------------------------------------------------------------------------

void set_JINF_power_down(int16 *err) { 

  int32 RQ_typ = 0x0E; 
  
//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err); 
  to_AMSW(WRITE, RQ_typ,      0,   NULL,      0,    NULL,   NULL,     err); 

  if (*err) return; 
} 

//~============================================================================

void get_JINF_delay(int8 *delay, int16 *err) {

  int32 RQ_typ = 0x19;
  int32 RQ_cnt = 0;
  int16 RP_dat[3];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (RP_cnt != 3) {
    *err = 0x0100;
    return;
  }

  *delay = RP_dat[0];
}

//~----------------------------------------------------------------------------

void set_JINF_delay(int8  delay, int16 *err) {

  int32 RQ_typ = 0x19;
  int16 RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat) / 2;
  
  RQ_dat[0] = delay;
  
//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);

  if (*err) return;
}

//~============================================================================

void get_JINF_event_nb(int16 *event_nb, int16 *err) {

  int32 RQ_typ = 0x02;
  int32 RQ_cnt = 0;
  int16 RP_dat[3];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (RP_cnt != 3) {
    *err = 0x0100;
    return;
  }

  *event_nb = RP_dat[0];
}

//~----------------------------------------------------------------------------

void reset_JINF_event_FIFO(int16 *err) {

  int32 RQ_typ = 0x02;
  
//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(WRITE, RQ_typ,      0,   NULL,      0,    NULL,   NULL,     err);

  if (*err) return;
}

//~============================================================================

void test_JINF_slaves(int16 *err) {

  int32 RQ_typ = 0x16;
  
//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(WRITE, RQ_typ,      0,   NULL,      0,    NULL,   NULL,     err);

  if (*err) return;
}

//~============================================================================

void get_JINF_SSF(int16 *mode, int16 *err) {

  int32 RQ_typ = 0x1A;
  int32 RQ_cnt = 0;
  int16 RP_dat[3];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (RP_cnt != 3) {
    *err = 0x0100;
    return;
  }

  *mode = RP_dat[0];
}

//~----------------------------------------------------------------------------

void set_JINF_SSF(int16  mode, int16 *err) {

  int32 RQ_typ = 0x1A;
  int16 RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat[1]) / 2;
  
  RQ_dat[0] = mode;
  
//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);

  if (*err) return;
}

//~============================================================================

void get_JINF_busy_mask(int32 *mask, int16 *err) {

  int32 RQ_typ = 0x18;
  int32 RQ_cnt = 0;
  int16 RP_dat[4];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (RP_cnt != 4) {
    *err = 0x0100;
    return;
  }

  *mask = ((int32)RP_dat[0] << 16) | (int32)RP_dat[1];
}

//~----------------------------------------------------------------------------

void set_JINF_busy_mask(int32  mask, int16 *err) {

  int32 RQ_typ = 0x18;
  int16 RQ_dat[2];
  int32 RQ_cnt = sizeof(RQ_dat) / 2;
  
  RQ_dat[0] = mask >> 16;;
  RQ_dat[1] = mask & 0x0000FFFF;
  
//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);

  if (*err) return;
}

//~----------------------------------------------------------------------------

void get_JINF_busy_status(int32 *status, int16 *err) {

  int32 RQ_typ = 0x0A;
  int32 RQ_cnt = 0;
  int16 RP_dat[4];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (RP_cnt != 4) {
    *err = 0x0100;
    return;
  }

  *status = ((int32)RP_dat[0] << 16) | (int32)RP_dat[1];
}

//~----------------------------------------------------------------------------

void get_JINF_busy_errors(int32 *errors, int16 *err) {

  int32 RQ_typ = 0x0B;
  int32 RQ_cnt = 0;
  int16 RP_dat[4];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (RP_cnt != 4) {
    *err = 0x0100;
    return;
  }

  *errors = ((int32)RP_dat[0] << 16) | (int32)RP_dat[1];
}

//~============================================================================

void get_JINF_slave_masks(int32 *masks, int16 *err) {

  int32 RQ_typ = 0x17;
  int32 RQ_cnt = 0;
  int16 RP_dat[26];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;
  int i, j;
  int16 kunin[24];

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (RP_cnt != 26) {
    *err = 0x0100;
    return;
  }

  for (i=0; i<24; i++) kunin[i] = RP_dat[i];

  for (i=0; i<16; i++) {
    masks[i] = 0x00000000;
    for (j=0; j<24; j++) {
      pack32(&masks[i], 1<<j, unpack16(kunin[j], 1<<i));
    }
  }
}

//~----------------------------------------------------------------------------

void set_JINF_slave_mask(int8 mask_ID, int32 mask, int16 *err) {

  int32 RQ_typ = 0x17;
  int16 RQ_dat[2];
  int32 RQ_cnt = sizeof(RQ_dat) / 2;

  RQ_dat[0] = ((int16)mask_ID << 8) | ((mask & 0x00FF0000) >> 16);
  RQ_dat[1] = mask & 0x0000FFFF;
  
//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);

  if (*err) return;
}

//~============================================================================

void get_JINF_proc_mode(int16 *mode, int16 *err) {

  int32 RQ_typ = 0x12;
  int32 RQ_cnt = 0;
  int16 RP_dat[3];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;
  
  if (RP_cnt != 3) {
    *err = 0x0100;
    return;
  }

  *mode = RP_dat[0];
}

//~----------------------------------------------------------------------------

void set_JINF_proc_mode(int16  mode, int16 *err) {

  int32 RQ_typ = 0x12;
  int16 RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat[1]) / 2;
  
  RQ_dat[0] = mode;
  
//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);

  if (*err) return;
}

//~============================================================================

bool get_JINF_event(int *n, int16 *event, int16 *err) {

  int32 RQ_typ = 0x01;
  int32 RQ_cnt = 0;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,   NULL,  16384, &RP_cnt,  event,     err);

  if (*err) return FALSE;
  
  if (RP_cnt) {
    *n = RP_cnt - 1;
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//~============================================================================

void init_LeCroy_bus(int8 bus, int16 *err) {

  int32 RQ_typ = 0x1D;
  int32 RQ_cnt = 1;
  int16 RQ_dat[1];

  RQ_dat[0] = (int16)bus <<  4;

//to_AMSW(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~----------------------------------------------------------------------------

void exec_LeCroy_cmd(
     int n, int8 *bus, int32 *wri, int32 *rea, bool *chk, int16 *err) {

  int i, j, nn;
  int32 RQ_typ = 0x1D;
  int32 RQ_cnt = 0;
  int16 RQ_dat[1025];      // calculate and put here correct array dimension
  int16 RP_dat[1023];      // calculate and put here correct array dimension
  int32 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;

  RQ_dat[0] = n;
  for (i=0; i<n; i++) {
    RQ_dat[1+4*i+0] = 0;                         // delay will come here later
    RQ_dat[1+4*i+1] = (int16)bus[i] << 4;
    RQ_dat[1+4*i+2] = wri[i] >> 16;
    RQ_dat[1+4*i+3] = wri[i] & 0x0000FFFF;
  }
  RQ_cnt = 1 + 4 * n;;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;

   nn = (RP_cnt - 2) / 7;
   if (RP_cnt != nn * 7 + 2) {
     *err = 0x0400;
     return;
   }
   if (nn != n) {
     *err = 0x0500;
     return;
   }
   for (i=0; i<nn; i++) {
     for (j=0; j<4; j++) {
       if (RP_dat[1+7*i+j] != RQ_dat[1+4*i+j]) {
         *err = 0x0600;
         return;
       }
     }
   }

  for (i=0; i<nn; i++) {
    rea[i] = ((int32)RP_dat[1+7*i+4] << 16) | (int32)RP_dat[1+7*i+5];
    chk[i] = ((rea[i] ^ wri[i]) & 0x7FFF0000) ? 1 : 0;
  }
}
//~----------------------------------------------------------------------------

void perform_IO_operation(int16 *wri, int *n_r, int16 *rea, int16 *err) {

  int32 RQ_typ = 0x0F;
  int32 RQ_cnt = (*wri & 0x3FFF) + 2;
  int32 RP_cnt;

  if ((RQ_cnt + 1) % 2) {
    *err = 0x0100;
    return;
  }
  
//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,    wri, RQ_cnt, &RP_cnt,    rea,     err);

  if (*err) return;

  if (RP_cnt != RQ_cnt) {
    *err = 0x0200;
    return;
  }

  if (n_r) *n_r = RP_cnt - 2;
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
    dat = pack16(&dat, msk, DAC0 & 0x7FFF);
    add_IO_write(request, bias+0, dat);
  }
  if (!(DAC1 & 0x8000)) {
    int16 dat = 0x4000;
    dat = pack16(&dat, msk, DAC1 & 0x7FFF);
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
