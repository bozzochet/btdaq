// file gpslib.h
//
// see gpslib.c
//
// A.Lebedev Nov-2005...
// C.Zurbach May-2007...
//

#ifndef _GPSLIB_H
#define _GPSLIB_H

#include "uscmlib.h"

typedef struct {
  int  cnt;
  int8 dat[256];
} _tc;

typedef struct __tc_par {
  char *nam;
  int   bgn;
  int   end;
  char *fmt;
  int   def;
} _tc_par;

typedef struct __TC {
  char  *nam;
  int16  ID;
  int    len;
  _tc_par   par[100];
} _TC;

typedef struct __tm_par {
  char *nam;
  int   bgn;
  int   end;
  char *fmt;
  char *unit;
  float mult;
} _tm_par;

typedef struct __TM {
  char *nam;
  struct {
    int16 RS422;
    int16 _1553;
  } ID;
  int   len;
  void (*call)(struct __TM*);
  char *desc;
  _tm_par  par[100];
  int8 *msg;
} _TM;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void to_GPS(int port, _tc *tc, int16 *err);
void to_GPS_sim(int port, _tc *tc, int16 *err);

int32 checksum(int n, int8 *b);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_COA(int32 COA_mgt, _tc *tc);

void create_GALEPH(int32 alm_eph, long PRN, _tc *tc);
void create_GDUMP(int32 start_add, long block_num, _tc *tc);
void create_GMESS(int32 TM_ID, long period, _tc *tc);

void create_LINIT(int32 address, long lenght, _tc *tc);
void create_LREPROM(_tc *tc);
void create_LRPACK(int32 address, int8 *RAM_dat, _tc *tc);
void create_LSYSTEM(void);

void create_NECO(short day, double day_fraction, int8 cov_orbit,
     int8 cov_clock, int8 cov_ind, int8 cov_type, float sma_axis,
     float ex, float ey, float hx, float hy, float position,
     float clock_bias, float clock_drift, float clock_drate,
     float coefficient, _tc *tc);
void create_NEMCO(void);
void create_NEXT(void);
void create_NINIT(short day, double day_fraction, int8 func, _tc *tc);
void create_NMANO(short day, double day_fraction, int8 thrust_error, 
     int8 thrust_type, long thrust_duration,
     float axis_1st, float axis_2nd, float axis_3rd, 
     float mass, float impulse, 
     float cmaxis_1st, float cmaxis_2nd, float cmaxis_3rd,
     float cmmaxis_1st, float cmmaxis_2nd, float cmmaxis_3rd, _tc *tc);
void create_NMOTE(void);
void create_NORBIT(short day, double day_fraction, double axis, double ex, 
     double ey, double hx, double hy, double orbit, _tc *tc);
void create_NPOL(void);

void create_RALEPH(_tc *tc);
void create_RATT(int16 frame, short q1, short q2, short q3, 
     short attitude_FOM, int8 attitude_val, _tc *tc);
void create_RCACQ(int16 field_val, short ID_algo, short acq_thresh,
     long sweep_rate, short intg_time, short search_range, _tc *tc);
void create_RCANT(_tc *tc);
void create_RCCS(int16 cmd, short from, short to,
     int8 *sat_ID, _tc *tc);
void create_RCEM(int16 field_val, short mask_angle, short mask_radius,
     int8 source, short pattern_A, short pattern_n, _tc *tc);
void create_RCFLAG(_tc *tc);
void create_RCTR(int16 field_val, short *code_K, short *carrier_K,
     short *interf_K, int8 correl, int8 chip, short track_01,
     short track_02, short track_03, _tc *tc);
void create_RNAV(short week, long second, long sec_fract,
     long pos_x, long pos_y, long pos_z, long vel_x, long vel_y, long vel_z,
     long drift, long pos_FOM, long vel_FOM, long drift_FOM, long time_FOM,
     _tc *tc);
void create_RPSEUD(int16 cha_ID, int16 cmd, int16 sig_src, 
     short sig_src_PRN, short doppler, _tc *tc);
void create_RSYSTEM(void);
void create_RTIME(int16 typ, short week, long second, _tc *tc);

void create_WMODE(int16 cmd, bool PPS, _tc *tc);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_STIME(int32 week, int32 second, int *TM_cnt, int8 *TM_dat);
void create_SDUMP(int *TM_cnt, int8 *TM_dat);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void process_ANYTM(_TM *TM);

//~--- Operational TM ---

void process_STIME(_TM *TM);
void process_SNAVS(_TM *TM);
void process_SPHINT(_TM *TM);
void process_SVELINT(_TM *TM);
void process_SCOSDIR(_TM *TM);
void process_SRECEIVST(_TM *TM);
void process_SDUMP(_TM *TM);

//~--- Control TM ---

void process_SNAVI(_TM *TM);
void process_SNAVN(_TM *TM);
void process_SALEPH(_TM *TM);
void process_S1RAWD(_TM *TM);
void process_S2RAWD(_TM *TM);
void process_S3RAWD(_TM *TM);
void process_S1CHANST(_TM *TM);
void process_S2CHANST(_TM *TM);

//~--- Init and software-uploading TM ---

void process_LSTATUS(_TM *TM);

//~--- Reserved TM ---

void process_NS1VECT(_TM *TM);
void process_NS2VECT(_TM *TM);
void process_NS3VECT(_TM *TM);
void process_NDIAG(_TM *TM);
void process_NRES(_TM *TM);
void process_S1SYSTEM(_TM *TM);
void process_S2SYSTEM(_TM *TM);
void process_S3SYSTEM(_TM *TM);

void process_SPVTDAT(_TM *TM);

//~============================================================================

#endif // _GPSLIB_H
