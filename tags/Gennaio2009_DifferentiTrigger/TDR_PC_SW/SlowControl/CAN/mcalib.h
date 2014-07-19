// file mcalib.h
//
// see mcalib.c
//
// A.Lebedev Mar-2005...

#ifndef _MCALIB_H
#define _MCALIB_H

#include "uscmlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void to_MCA(char *cmd, int16 siz, int16 *cnt, int8 *dat, int16 *err);

void power_MCA_up(int16 *err);
void power_MCA_down(int16 *err);
void start_MCA_DAQ(int16 *err);
void stop_MCA_DAQ(int16 *err);
void clear_MCA_memory(int16 *err);
void set_MCA_runtime_10_sec(int16 *err);
void set_MCA_runtime_100_sec(int16 *err);
void set_MCA_runtime_1000_sec(int16 *err);
void read_MCA_channel(int16 *chan, int32 *cont, int16 *err);
void read_MCA_all_channels(int8 *chan, int32 *cont, int16 *err);
void read_MCA_50_channels(int16 *chan, int32 *cont, int16 *err);
void read_MCA_50_channels16(int16 *chan, int16 *cont, int16 *err);
void reset_MCA_counter(int16 *err);
void transfer_MCA_data(int16 *err);
void read_MCA_status(int8 *sta, int16 *err);
void enable_pedestal_readout_01Hz(int16 *err);  // FSpada 22/1/08
void enable_pedestal_readout_1Hz(int16 *err);   // FSpada 22/1/08
void enable_pedestal_readout_10Hz(int16 *err);  // FSpada 22/1/08
void disable_pedestal_readout(int16 *err);      // FSpada 22/1/08

//~============================================================================

#endif // _MCALIB_H
