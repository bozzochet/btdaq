// file daqlib.h
//
// A.Lebedev Apr-2005...
//

//~----------------------------------------------------------------------------

#ifndef _DAQLIB_H
#define _DAQLIB_H

//~----------------------------------------------------------------------------

#include "amswlib.h"

//~----------------------------------------------------------------------------

void read_JLV1_scalers(int16 *dat, int16 *err);
void read_JLV1_FPGA_version_ID(int16 *dat, int16 *err);

void exec_SDR2_SC_command(
     int R_W, int8 adr, int16 reg, int16 wri, int16 *rea, int16 *sta, int16 *err);

void begin_IO_segment(int16 *seg);
void end_IO_segment(int16 *seg);
void add_IO_read(int16 *seg, int16 adr);
void add_IO_write(int16 *seg, int16 adr, int16 dat);

void boot_JINF(int16 fh, int16 *err);
void load_JINF_FLASH_file(int16 fh, int16 *err);
void ping_JINF(int n, int16 *dat, int *nn, int16 *datdat, int16 *err);
void get_JINF_status(int *fmt, int16 *dat, 
                     int *n, int16 *datdat, int16 *sta, int16 *err);
void get_JINF_FLASH_summary(int *fmt, int16 *dat, 
                            int *n, int16 *datdat, int16 *sta, int16 *err);
void set_JINF_power_down(int16 *err);

void get_JINF_delay(int8 *delay, int16 *err);
void set_JINF_delay(int8  delay, int16 *err);

void get_JINF_event_nb(int16 *event_nb, int16 *err);
void reset_JINF_event_FIFO(int16 *err);

void test_JINF_slaves(int16 *err);

void get_JINF_SSF(int16 *mode, int16 *err);
void set_JINF_SSF(int16  mode, int16 *err);

void get_JINF_busy_mask(int32 *mask, int16 *err);
void set_JINF_busy_mask(int32  mask, int16 *err);
void get_JINF_busy_status(int32 *status, int16 *err);
void get_JINF_busy_errors(int32 *errors, int16 *err);

void get_JINF_slave_masks(int32 *masks, int16 *err);
void set_JINF_slave_mask(int8 mask_ID, int32 mask, int16 *err);

void get_JINF_proc_mode(int16 *mode, int16 *err);
void set_JINF_proc_mode(int16  mode, int16 *err);

bool get_JINF_event(int *n, int16 *event, int16 *err);

void init_LeCroy_bus(int8 bus, int16 *err);
void exec_LeCroy_cmd(
     int n, int8 *bus, int32 *wri, int32 *rea, bool *chk, int16 *err);

void perform_IO_operation(int16 *wri, int *n_r, int16 *rea, int16 *err);

void get_EDR2_delay(int8 *delay, int16 *err);
void set_EDR2_delay(int8  delay, int16 *err);
void get_EDR2_DAC(int16 adr, int16 *DAC, int16 *err);
void set_EDR2_DAC(int16 adr, int16  DAC, int16 *err);
void get_EDR2_DACs(int16 *DAC1, int16 *DAC2, int16 *err);
void set_EDR2_DACs(int16  DAC1, int16  DAC2, int16 *err);
void get_EDR2_param(int8 *delay, int16 *DAC1, int16 *DAC2, int16 *err);
void set_EDR2_param(int8  delay, int16  DAC1, int16  DAC2, int16 *err);

//~============================================================================

#endif // _DAQLIB_H
