// file jmdclib.h
//
// See jmdclib.c
//
// A.Lebedev, May-2008...
// A.Basili,  Aug-2008...

//~----------------------------------------------------------------------------

#ifndef _JMDCLIB_H
#define _JMDCLIB_H

//~----------------------------------------------------------------------------

#include "mylib.h"
#include "tcplib.h"
#include "easslib.h"

//~----------------------------------------------------------------------------

//~--- Q-List Commands ---

void write_JMDC_Q_list_status(int16 status, int16 *err);
void start_JMDC_Q_list(int16 *err);
void stop_JMDC_Q_list(int16 *err);
void check_JMDC_Q_list_status(int16 *status, int16 *err);
void read_JMDC_Q_list(int *n, int16 *dat, int16 *err);

//~--- System Command ----

void boot_JMDC(int16 *err);
void ping_JMDC(int n, int16 *dat, int *nn, int16 *datdat, int16 *err);

//~--- NASA Command ----

void set_owner(int16 i1553, int16 iMCC, int16 iHRDL, int16 iDAQ, int16 *err);

//~--- Interface Control Command ----

void hrdl_light(bool o_o, int16 *err);
void hrdl_data(bool o_o, int16 *err);
void RS232_data(bool o_o, int16 *err);
void RS422_data(bool o_o, int16 *err);
/* void open_envelop(int16 *dat); */
/* void close_envelop(int16 **dat, int16 *n); */
/* void print_envelop(int16 *dat, int16 n); */
/* void send_envelop(int16 *dat, int16 n, int16 *err); */

//~--- File Operation Command ----

void file_summary (int16 *err);
void load_image(char *filename, int16 *err);
void load_JMDC_FLASH_file(char filename[12], int16 *err);

//~--- Main Interface Function to JMDC ---

void to_JMDC(int R_W,
             int32 RQ_typ, int32  RQ_cnt, int16 *RQ_dat,
             int32 RP_siz, int32 *RP_cnt, int16 *RP_dat, int16 *RP_err);

//~============================================================================

#endif // _JMDCLIB_H
