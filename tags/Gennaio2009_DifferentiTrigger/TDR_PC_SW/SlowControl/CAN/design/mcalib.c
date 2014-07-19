// file mcalib.c
//
// A library of MCA functions (MCA is planned to be used in TRD-Gas system)
//
// A.Lebedev Mar-2005...

#include "mcalib.h"

int port;                                    // global variable
int PP;                                      // global variable

//~============================================================================

void to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {

  int i, l, n;
  int8 rea[512];
  
  l = strlen(cmd);
  if (l != 3) {
    *err = 0x0100;
    return;
  }

  flush_serial_port(port, err);
  if (*err) {
    if (PP) printf("flush_serial_port(%d,..: err = %04X\n", port, *err);
    return;
  }

  write_serial_port(port, l, (int8*) cmd, err);
  if (*err) {
    if (PP) printf("write_serial_port(%d,..: err = %04X\n", port, *err);
    return;
  }
  if (PP) {
    printf("send %d bytes: ", l);
    for (i=0; i<l; i++) printf("%02X ", cmd[i]);
    printf("[");
    for (i=0; i<l-1; i++) printf("%c", cmd[i]);
    printf("] ");
    printf("... ");
  }
  
  *cnt = 0;
  while (1) {
    read_serial_port(port, 512, &n, rea, err);
    if (*err) {
      if (PP) printf("read_serial_port(%d,..: err = %04X\n", port, *err);
      return;
    }
    if (n > 0) {
      if (*cnt + n > siz) {
        if (PP) printf("Too much!\n");
        *err = 0x0200;
        return;
      }
      if (dat) memcpy(&dat[*cnt], rea, n);
      *cnt = *cnt + n;
    }
    else {
      if (*cnt) {
        if (PP) printf("Done.\n");
        break;
      }
    }
  }
}

//~============================================================================

void power_MCA_up(int16 *err) {

  char *cmd = {"PU\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;

  if (PP > 1) printf("*** power_MCA_up::             ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);

  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~----------------------------------------------------------------------------

void start_MCA_DAQ(int16 *err) {

  char *cmd = {"ST\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;

  if (PP > 1) printf("*** start_MCA_DAQ::            ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);

  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~----------------------------------------------------------------------------

void stop_MCA_DAQ(int16 *err) {

  char *cmd = {"SP\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;

  if (PP > 1) printf("*** stop_MCA_DAQ::             ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);

  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~----------------------------------------------------------------------------

void clear_MCA_memory(int16 *err) {

  char *cmd = {"CL\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;

  if (PP > 1) printf("*** clear_MCA_memory::         ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);

  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~----------------------------------------------------------------------------

void set_MCA_runtime_10_sec(int16 *err) {

  char *cmd = {"R1\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;

  if (PP > 1) printf("*** set_MCA_runtime_10_sec::   ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);

  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~----------------------------------------------------------------------------

void set_MCA_runtime_100_sec(int16 *err) {

  char *cmd = {"R2\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;

  if (PP > 1) printf("*** set_MCA_runtime_100_sec::  ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);

  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~----------------------------------------------------------------------------

void set_MCA_runtime_1000_sec(int16 *err) {

  char *cmd = {"R3\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;

  if (PP > 1) printf("*** set_MCA_runtime_1000_sec:: ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);

  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~----------------------------------------------------------------------------

void read_MCA_channel(int16 *chan, int32 *cont, int16 *err) {

  char *cmd = {"RO\r"};
  int N = 1;
  int8 dat[4*N+2];
  int16 siz = sizeof(dat);
  int16 cnt;
  int i;

  if (PP > 1) printf("*** read_MCA_channel::         ");
  
//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,       dat,        err);
  
  if (*err) return;
  
  if (cnt != siz) {
    *err = 0x0500;
    return;
  }

  *chan = ((int16)dat[4*N+1]<<8) + dat[4*N];
  for (i=0; i<2*N; i+=2) {
    cont[i/2] = ((int32)dat[2*N+i+1]<<24) + 
                ((int32)dat[2*N+i  ]<<16) + 
                ((int32)dat[    i+1]<< 8) + 
                ((int32)dat[      i]<< 0);
  }
}

//~----------------------------------------------------------------------------

void read_MCA_all_channels(int8 *chan, int32 *cont, int16 *err) {

  if (PP > 1) printf("*** read_MCA_all_channels::    ");
}

//~----------------------------------------------------------------------------

void read_MCA_50_channels(int16 *chan, int32 *cont, int16 *err) {

  char *cmd = {"RB\r"};
  int N = 50;
  int8 dat[4*N+2];
  int16 siz = sizeof(dat);
  int16 cnt;
  int i;
  
  if (PP > 1) printf("*** read_MCA_50_channels::     ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,       dat,        err);
  
  if (*err) return;
  
  if (cnt != siz) {
    *err = 0x0500;
    return;
  }

  *chan = ((int16)dat[4*N+1]<<8) + dat[4*N];
  for (i=0; i<2*N; i+=2) {
    cont[i/2] = ((int32)dat[2*N+i+1]<<24) + 
                ((int32)dat[2*N+i  ]<<16) + 
                ((int32)dat[    i+1]<< 8) + 
                ((int32)dat[      i]<< 0);
  }
}

//~----------------------------------------------------------------------------

void read_MCA_50_channels16(int16 *chan, int16 *cont, int16 *err) {

  if (PP > 1) printf("*** read_MCA_50_channels16::   ");
}

//~----------------------------------------------------------------------------

void reset_MCA_counter(int16 *err) {

  char *cmd = {"SC\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;
  
  if (PP > 1) printf("*** reset_MCA_counter::        ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);
  
  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~----------------------------------------------------------------------------

void transfer_MCA_data(int16 *err) {

  char *cmd = {"RM\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;
  
  if (PP > 1) printf("*** transfer_MCA_data::        ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);
  
  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~----------------------------------------------------------------------------

void read_MCA_status(int8 *sta, int16 *err) {

  char *cmd = {"RS\r"};
  int16 siz = 20;
  int16 cnt;

  if (PP > 1) printf("*** read_MCA_status::          ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,       sta,        err);

  if (*err) return;

  if (cnt != siz) {
    *err = 0x0500;
    return;
  }
}

//~-------FSpada 22/1/08--------------------------------------------------------

void enable_pedestal_readout_01Hz(int16 *err) {

  char *cmd = {"PP\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;
  
  if (PP > 1) printf("*** enable_pedestal_readout_01Hz::        ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);
  
  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~--------FSpada 22/1/08-------------------------------------------------------

void enable_pedestal_readout_1Hz(int16 *err) {

  char *cmd = {"PQ\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;
  
  if (PP > 1) printf("*** enable_pedestal_readout_1Hz::        ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);
  
  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~--------FSpada 22/1/08-------------------------------------------------------

void enable_pedestal_readout_10Hz(int16 *err) {

  char *cmd = {"PR\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;
  
  if (PP > 1) printf("*** enable_pedestal_readout_10Hz::        ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);
  
  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~--------FSpada 22/1/08--------------------------------------------------------

void disable_pedestal_readout_01Hz(int16 *err) {

  char *cmd = {"PD\r"};
  int16 siz = 1;
  int16 cnt;
  int8 dat;
  
  if (PP > 1) printf("*** disable_pedestal_readout::        ");

//to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err) {  
  to_MCA(      cmd,       siz,       &cnt,      &dat,        err);
  
  if      (*err);
  else if (cnt != siz)  *err = 0x0500;
  else if (dat == 0x04) *err = 0x0600;
  else if (dat != 0x06) *err = 0x0700;
}

//~============================================================================
