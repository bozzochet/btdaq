// file gpslib.c
//
// A library of GPS functions
//
// Claude Zurbach & A.Lebedev - Nov 2005...
//

#include "gpslib.h"

//~============================================================================
//
//                  COMMON FUNCTIONS
//
//~============================================================================

void to_GPS(int port, _tc *tc, int16 *err) {

  unpackbytes(&tc->dat[0+4], 0x00000000, 4);
  unpackbytes(&tc->dat[0+4], checksum(tc->cnt, (int8*)tc->dat), 4);
  write_serial_port(port, tc->cnt, tc->dat, err);
}

//~----------------------------------------------------------------------------

void to_GPS_sim(int port, _tc *tc, int16 *err) {

  int i;
  
  unpackbytes(&tc->dat[0+4], 0x00000000, 4);
  unpackbytes(&tc->dat[0+4], checksum(tc->cnt, (int8*)tc->dat), 4);
  printf("to_GPS_sim: port=%d tc->cnt=%d\n", port, tc->cnt);
  for (i=0; i<tc->cnt; i++) printf("%02hhX ", tc->dat[i]);
  printf("\n");
}

//~============================================================================

void print_TM_message(_TM *TM) {

  int i;

  printf("len = %d\n", TM->len);
  for (i=0; i<64; i++) {
    printf("%02hhX ", TM->msg[i]);
  }
  printf("\n");
}

//~============================================================================

int32 checksum(int n, int8 *b) {

  int i;
  int32 w;
  int32 s = 0;

  for (i=8; i<n-8;) {
//printf("i = %2d\n", i);
    w  = (int32)b[i++] << 24;
    w += (int32)b[i++] << 16;
    w += (int32)b[i++] <<  8;
    w += (int32)b[i++] <<  0;
//printf("w = 0x%08X\n", w);
    s = s + w;
  }

  return s;
}

//~============================================================================
//
//                  TELECOMMANDS
//
//~============================================================================

//~----------------------------------------------------------------------------
// COA TC: to stop DEMETR telemetry (a.k.a. ROFF003 or S3SYSTEM)

void create_COA(int32 COA_mgt, _tc *tc) {

  int8 msg[] = {
// COA Telecommand
// header
  0x9C, 0x3E, 0x1F, 0x18,  // preamble & identifier
  0x00, 0x00, 0x00, 0x00,  // place for checksum                         <<<===
//
  0x00,                    // reserved
  0xFF,                    // validity                                   <<<===
  0x11, 0x00, 0x00, 0x00,  // specific ID for COA
  0x00, 0x00, 0x00, 0x00,  // permit(00010001)/inhibit(00010000) COA     <<<===
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00, 0x00,              // reserved
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 6], COA_mgt,                    4);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ : 28.11.06 - GALEPH to collect Almanac or Ephemeris (two parameters)

void create_GALEPH(int32 alm_eph, long PRN, _tc *tc) {

  int8 msg[] = {
// GALEPH Telecommand
// header
  0x9C, 0x3E, 0x19, 0x44,      // preamble & identifier (0x1944)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00, 0x00, 0x00,      // type of data (almanach : 0x11)         <<<===
  0x00, 0x00, 0x00, 0x00,      // GPS PRN : 0 = all                      <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], alm_eph,                    4);
  unpackbytes(&msg[8+ 8], PRN,                        4);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ : 03.05.07 - GDUMP to request the dump of a RAM memory area

void create_GDUMP(int32 start_add, long block_num, _tc *tc) {

  int8 msg[] = {
// GDUMP Telecommand
// header
  0x9C, 0x3E, 0x1F, 0x0F,      // preamble & identifier (0x1F0F)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // starting address of dump               <<<===
  0x00, 0x00, 0x00, 0x00,      // number of 16-byte block to dump        <<<===
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 8], start_add,                  4);
  unpackbytes(&msg[8+12], block_num,                  4);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// AL : GMESS - demand of a periodic message

void create_GMESS(int32 TM_ID, long period, _tc *tc) {

  int8 msg[] = {
// GMESS Telecommand:
// header
  0x9C, 0x3E, 0x19, 0x1E,      // preamble & identifier
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00, 0x00, 0x00,      // TM ID                                  <<<===
  0x00, 0x00, 0x00, 0x00,      // period of transmission                 <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], TM_ID,                      4);
  unpackbytes(&msg[8+ 8], period,                     4);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ - 02.05.07 : LINIT - to initialise the software-upload procedure

void create_LINIT(int32 address, long length, _tc *tc) {

  int8 msg[] = {
// LINIT Telecommand:
// header
  0x9C, 0x3E, 0x10, 0x3F,      // preamble & identifier
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00, 0x00, 0x00,      // address                                <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // length                                 <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00,                             // reserved
  0x00, 0x00, 0x00, 0x00,      // CRC                                    <<<===
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], address,                    4);
  unpackbytes(&msg[8+12], length,                     4);

  // treatement CRC Data ?

  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// LREPROM TC: Copy in FPROM

void create_LREPROM(_tc *tc) {

  int8 msg[] = {
// LREPROM Telecommand:
// header
  0x9C, 0x3E, 0x10, 0x5A,      // preamble & identifier (0x105A)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00, 0x00, 0x00,      // Address of source                      <<<===
  0x00, 0x00, 0x00, 0x00,      // Address of destination                 <<<===
  0x00, 0x00, 0x00, 0x00,      // Length of the area                     <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00,                             // reserved
  0x00, 0x00, 0x00, 0x00,      // CRC                                    <<<===
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ - 03.05.07 : LRPACK - to initialise the software-upload procedure

void create_LRPACK(int32 address, int8 *RAM_dat, _tc *tc) {

  int8 msg[] = {
// LRPACK Telecommand:
// header
  0x9C, 0x3E, 0x10, 0x99,      // preamble & identifier
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00, 0x00, 0x00,      // address                                <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // data            <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // data            <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // data            <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // data            <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // data            <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // data            <<<===
  0x00, 0x00, 0x00, 0x00,      // data                                   <<<===
  0x00, 0x00, 0x00, 0x00,      // CRC                                    <<<===
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], address,                    4);
  memcpy(&msg[     8+ 8], RAM_dat,                   52);

  // treatement CRC Data ?

  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// LSYSTEM TC: Used for on-ground tests

void create_LSYSTEM(void) {
}

//~----------------------------------------------------------------------------
// CZ : 02.05.07 - NECO to describe a manoeuvre to the N2G

void create_NECO(short day, double day_fraction, int8 cov_orbit,
     int8 cov_clock, int8 cov_ind, int8 cov_type, float sma_axis,
     float ex, float ey, float hx, float hy, float position,
     float clock_bias, float clock_drift, float clock_drate,
     float coefficient, _tc *tc) {

  int8 msg[] = {
// NECO Telecommand
// header
  0x9C, 0x3E, 0x1A, 0x50,      // preamble & identifier (0x1A50)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
  //
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
//
// Time of application
  0x00, 0x00,                  // day number                             <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // day fraction        <<<===
//
// Indicators
  0x00,                        // covariance for orbital parameters      <<<===
  0x00,                        // covariance of local clock              <<<===
  0x00,                        // indicator of covariance                <<<===
  0x00,                        // type of covariance                     <<<===
//
// Covariance of orbital parameters
  0x00, 0x00, 0x00, 0x00,      // semi-major axis                        <<<===
  0x00, 0x00, 0x00, 0x00,      // Ex                                     <<<===
  0x00, 0x00, 0x00, 0x00,      // Ey                                     <<<===
  0x00, 0x00, 0x00, 0x00,      // Hx                                     <<<===
  0x00, 0x00, 0x00, 0x00,      // Hy                                     <<<===
  0x00, 0x00, 0x00, 0x00,      // Position orbit                         <<<===
//
// Covariance of local clock
  0x00, 0x00, 0x00, 0x00,      // clock bias                             <<<===
  0x00, 0x00, 0x00, 0x00,      // clock drift                            <<<===
  0x00, 0x00, 0x00, 0x00,      // clock drift rate                       <<<===
  0x00, 0x00,                  // reserved
//
// Covariance of CMS, or DRAG, or HILL
  0x00, 0x00, 0x00, 0x00,      // solar pressure coeff.                  <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], day,                        2);
  unpackbytes(&msg[8+ 6], day_fraction,               8);
  unpackbytes(&msg[8+14], cov_orbit,                  1);
  unpackbytes(&msg[8+15], cov_clock,                  1);
  unpackbytes(&msg[8+16], cov_ind,                    1);
  unpackbytes(&msg[8+17], cov_type,                   1);
  unpackbytes(&msg[8+18], sma_axis,                   1);
  unpackbytes(&msg[8+22], ex,                         4);
  unpackbytes(&msg[8+26], ey,                         4);
  unpackbytes(&msg[8+30], hx,                         4);
  unpackbytes(&msg[8+34], hy,                         4);
  unpackbytes(&msg[8+38], position,                   4);
  unpackbytes(&msg[8+42], clock_bias,                 4);
  unpackbytes(&msg[8+46], clock_drift,                4);
  unpackbytes(&msg[8+50], clock_drate,                4);
  unpackbytes(&msg[8+56], coefficient,                4);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// NEMCO TC: Modification of variance of the Navigator measurements

void create_NEMCO(void) {
}

//~----------------------------------------------------------------------------
// NEXT TC: Used for on-ground tests

void create_NEXT(void) {
}

//~----------------------------------------------------------------------------
// CZ : 25.04.07 - NINIT to reset the N2G navigator

void create_NINIT(short day, double day_fraction, int8 func, _tc *tc) {

  int8 msg[] = {
// NINIT Telecommand
// header
  0x9C, 0x3E, 0x1A, 0x0A,      // preamble & identifier (0x1A0A)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
  //
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
//
// Time of application
  0x00, 0x00,                  // day number                             <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // day fraction        <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // function                               <<<===
//
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00,                  // reserved
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], day,                        2);
  unpackbytes(&msg[8+ 6], day_fraction,               8);
  unpackbytes(&msg[8+16], func,                       1);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ : 02.05.07 - NMANO to describe a manoeuvre to the N2G

void create_NMANO(short day, double day_fraction, int8 thrust_error, 
     int8 thrust_type, long thrust_duration,
     float axis_1st, float axis_2nd, float axis_3rd, 
     float mass, float impulse, 
     float cmaxis_1st, float cmaxis_2nd, float cmaxis_3rd,
     float cmmaxis_1st, float cmmaxis_2nd, float cmmaxis_3rd, _tc *tc) {

  int8 msg[] = {
// NINIT Telecommand
// header
  0x9C, 0x3E, 0x1A, 0x47,      // preamble & identifier (0x1A47)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
  //
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
//
// Time of application
  0x00, 0x00,                  // day number                             <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // day fraction        <<<===
//
// Thrust features
  0x00,                        // estimation thrust error                <<<===
  0x00,                        // type of thrust                         <<<===
  0x00, 0x00, 0x00, 0x00,      // duration of thrust                     <<<===
  0x00, 0x00, 0x00, 0x00,      // 1st reference axis                     <<<===
  0x00, 0x00, 0x00, 0x00,      // 2sd reference axis                     <<<===
  0x00, 0x00, 0x00, 0x00,      // 3th reference axis                     <<<===
  0x00, 0x00, 0x00, 0x00,      // initial mass                           <<<===
  0x00, 0x00, 0x00, 0x00,      // specific impulse                       <<<===
//
// Covariance of manoeuvre
  0x00, 0x00, 0x00, 0x00,      // 1st reference axis                     <<<===
  0x00, 0x00, 0x00, 0x00,      // 2sd reference axis                     <<<===
  0x00, 0x00, 0x00, 0x00,      // 3th reference axis                     <<<===
//
// Covariance of model of manoeuvre if estimation requested
  0x00, 0x00, 0x00, 0x00,      // 1st reference axis                     <<<===
  0x00, 0x00, 0x00, 0x00,      // 2sd reference axis                     <<<===
  0x00, 0x00, 0x00, 0x00,      // 3th reference axis                     <<<===
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], day,                        2);
  unpackbytes(&msg[8+ 6], day_fraction,               8);
  unpackbytes(&msg[8+14], thrust_error,               1);
  unpackbytes(&msg[8+15], thrust_type,                1);
  unpackbytes(&msg[8+16], thrust_duration,            1);
  unpackbytes(&msg[8+20], axis_1st,                   4);
  unpackbytes(&msg[8+24], axis_2nd,                   4);
  unpackbytes(&msg[8+28], axis_3rd,                   4);
  unpackbytes(&msg[8+32], mass,                       4);
  unpackbytes(&msg[8+36], impulse,                    4);
  unpackbytes(&msg[8+40], cmaxis_1st,                 4);
  unpackbytes(&msg[8+44], cmaxis_2nd,                 4);
  unpackbytes(&msg[8+48], cmaxis_3rd,                 4);
  unpackbytes(&msg[8+52], cmmaxis_1st,                4);
  unpackbytes(&msg[8+56], cmmaxis_2nd,                4);
  unpackbytes(&msg[8+60], cmmaxis_3rd,                4);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// NMOTE TC: Modification of propagation model of the Navigator

void create_NMOTE(void) {
}

//~----------------------------------------------------------------------------
// CZ : 05.04.07 - NORBIT to initialise the state vector of N2G

void create_NORBIT(short day, double day_fraction, double axis, double ex, 
     double ey, double hx, double hy, double orbit, _tc *tc) {

  int8 msg[] = {
// NORBIT Telecommand
// header
  0x9C, 0x3E, 0x1A, 0x21,      // preamble & identifier (0x1A21)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
//
// Time of application
  0x00, 0x00,                  // day number                             <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // day fraction        <<<===
  0x00, 0x00,                  // reserved
//
// Orbital parameters in VEIS 50 frame
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // semi-major axis     <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Ex                  <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Ey                  <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Hx                  <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Hy                  <<<===
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Position on orbit   <<<===
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], day,                        2);
  unpackbytes(&msg[8+ 6], day_fraction,               8);
  unpackbytes(&msg[8+16], axis,                       8);
  unpackbytes(&msg[8+24], ex,                         8);
  unpackbytes(&msg[8+32], ey,                         8);
  unpackbytes(&msg[8+40], hx,                         8);
  unpackbytes(&msg[8+48], hy,                         8);
  unpackbytes(&msg[8+56], orbit,                      8);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// NPOL TC: Modification of location of Earth pole used by the Navigator

void create_NPOL(void) {
}

//~----------------------------------------------------------------------------
// RALEPH TC: Transmission of almanac/ephemeris to the receiver

void create_RALEPH(_tc *tc) {

  int8 msg[] = {
// RALEPH Telecommand:
// header
  0x9C, 0x3E, 0x13, 0x72,      // preamble & identifier (0x1372)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00,                        // ID of signal source                    <<<===
// Health status
  0x00,                        // Health status and type                 <<<===
  0x00,                        // IODE(eph) or reserved(alm)             <<<===
  0x00,                        // URA and satellite present              <<<===
// Reference time of Almanac or Ephemerides
  0x00, 0x00,                  // week number                            <<<===
  0x00, 0x00,                  // Toc(eph) or reserved(alm)              <<<===
  0x00, 0x00,                  // Toe(eph) or Toa(alm)                   <<<===
// Main orbital parameters
  0x00, 0x00, 0x00, 0x00,      // mean anomaly at reference time         <<<===
  0x00, 0x00, 0x00, 0x00,      // eccentricity                           <<<===
  0x00, 0x00, 0x00, 0x00,      // square root of semimajor axis          <<<===
  0x00, 0x00, 0x00, 0x00,      // longitude of ascendant node            <<<===
  0x00, 0x00, 0x00, 0x00,      // inclination angle                      <<<===
  0x00, 0x00, 0x00, 0x00,      // argument of perigee                    <<<===
  0x00, 0x00, 0x00, 0x00,      // rate of right ascension                <<<===
// Clock parameters
  0x00, 0x00, 0x00, 0x00,      // Af0                                    <<<===
  0x00, 0x00,                  // Af1                                    <<<===
  0x00, 0x00,                  // Af2                                    <<<===
//Secondary orbital parameters
  0x00, 0x00,                  // IDOT: rate of inclination angle        <<<===
  0x00, 0x00,                  // mean motion difference                 <<<===
  0x00, 0x00,                  // C_uc                                   <<<===
  0x00, 0x00,                  // C_us                                   <<<===
  0x00, 0x00,                  // C_ic                                   <<<===
  0x00, 0x00,                  // C_is                                   <<<===
  0x00, 0x00,                  // C_rc                                   <<<===
  0x00, 0x00,                  // C_rs                                   <<<===
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ-AL: 05.04.07 - RATT to transmitt attitude spacecraft to GPS receiver

void create_RATT(int16 frame, short q1, short q2, short q3, 
     short attitude_FOM, int8 attitude_val, _tc *tc) {

  int8 msg[] = {
// RATT Telecommand:
// header
  0x9C, 0x3E, 0x15, 0x05,      // preamble & identifier (0x1505)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00,                  // Reference Frame                        <<<===
  0x00, 0x00,                  // q1                                     <<<===
  0x00, 0x00,                  // q2                                     <<<===
  0x00, 0x00,                  // q3                                     <<<===
  0x00, 0x00,                  // Attitude FOM                           <<<===
  0x00,                        // reserved
  0x00,                        // Validity of attitude aiding data       <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 6], frame,                      2);
  unpackbytes(&msg[8+ 8], q1,                         2);
  unpackbytes(&msg[8+10], q2,                         2);
  unpackbytes(&msg[8+12], q3,                         2);
  unpackbytes(&msg[8+14], attitude_FOM,               2);
  unpackbytes(&msg[8+17], attitude_val,               1);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ: 03.05.07 RCACQ - to modify the acquisition parameters

void create_RCACQ(int16 field_val, short ID_algo, short acq_thresh,
     long sweep_rate, short intg_time, short search_range, _tc *tc) {

  int8 msg[] = {
// RCACQ Telecommand
// header
  0x9C, 0x3E, 0x1C, 0xCF,      // preamble & identifier 0x1CCF
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // validity of field                      <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00,                  // ID acquisition algorithm               <<<===
  0x00, 0x00,                  // acquisition threshold                  <<<===
  0x00, 0x00, 0x00, 0x00,      // sweeping rate                          <<<===
  0x00, 0x00,                  // integration time                       <<<===
  0x00, 0x00,                  // code search range                      <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], field_val,                  2);
  unpackbytes(&msg[8+12], ID_algo,                    2);
  unpackbytes(&msg[8+14], acq_thresh,                 2);
  unpackbytes(&msg[8+16], sweep_rate,                 4);
  unpackbytes(&msg[8+20], intg_time,                  2);
  unpackbytes(&msg[8+22], search_range,               2);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// RCANT TC: Configuration of antennas

void create_RCANT(_tc *tc) {

  int8 msg[] = {
// RCANT Telecommand:
// header
  0x9C, 0x3E, 0x15, 0x48,      // preamble & identifier (0x1548)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // Validity of the fields                 <<<===
  0x00,                        // reserved
//
// Antenna Configuration
  0x00,                        // Antenna ID                             <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // Antenna masking angle                  <<<===
  0x00, 0x00, 0x00, 0x00, 0x00,// reserved
  0x00,                        // Antenna by-default orientation         <<<===
  0x00, 0x00,                  // reserved
//
// Position of the center of phase of the antenna in the body frame
  0x00, 0x00,                  // X                                      <<<===
  0x00, 0x00,                  // Y                                      <<<===
  0x00, 0x00,                  // Z                                      <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
//
// Direction cosines of the boresight of the antenna in the body frame
  0x00, 0x00,                  // cos(alpha) (along X axis)              <<<===
  0x00, 0x00,                  // cos(beta)  (along Y axis)              <<<===
  0x00, 0x00,                  // cos(gamma) (along Z axis)              <<<===
//
// Radiation pattern of the antenna. Group delay of the RF set.
  0x00, 0x00,                  // A (antenna power gain in boresight)    <<<===
  0x00, 0x00,                  // N (antenna directivity)                <<<===
  0x00, 0x00,                  // Propagation group delay                <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00,                  // reserved
//
// Configuration of the baseline
  0x00, 0x00,                  // Antenna indicator 1                    <<<===
  0x00, 0x00,                  // Antenna indicator 2                    <<<===
  0x00, 0x00,                  // Antenna indicator 3                    <<<===
  0x00, 0x00,                  // Antenna indicator 4                    <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ: 03.05.07 RCCS - to define strategy for cold start

void create_RCCS(int16 cmd, short from, short to,
     int8 *sat_ID, _tc *tc) {

  int8 msg[] = {
// RCCS Telecommand
// header
  0x9C, 0x3E, 0x1C, 0xA9,      // preamble & identifier
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // command word                           <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // Lower bound of Doppler range           <<<===
  0x00, 0x00,                  // Upper bound of Doppler range           <<<===
  0x00, 0x00, 0x00,            // reserved
  0x01,                        // Positioning system GPS                 <<<===
  0x00, 0x00, 0x00, 0x00,      // ID of satellites                       <<<===
  0x00, 0x00, 0x00, 0x00,      // ID of satellites                       <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  int i;

  unpackbytes(&msg[8+ 4], cmd,                        2);
  unpackbytes(&msg[8+12], from,                       2);
  unpackbytes(&msg[8+14], to,                         2);
  for (i=0; i<8; i++) if (sat_ID) unpackbytes(&msg[8+20+i], sat_ID[i], 1);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ: 03.05.07 RCEM - to define parameters relative to the positioning system

void create_RCEM(int16 field_val, short mask_angle, short mask_radius,
     int8 source, short pattern_A, short pattern_n, _tc *tc) {

  int8 msg[] = {
// RCEM Telecommand
// header
  0x9C, 0x3E, 0x1C, 0x1B,      // preamble & identifier 0x1C1B
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // validity of field                      <<<===
  0x01,                        // positioning system (GPS)
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // masking angle                          <<<===
  0x00, 0x00, 0x00,            // reserved
  0x00, 0x00,                  // radius maskig sphere                   <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00,                        // source boresight orientation           <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // radiation pattern A                    <<<===
  0x00, 0x00,                  // radiation pattern n                    <<<===
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], field_val,                  2);
  unpackbytes(&msg[8+10], mask_angle,                 2);
  unpackbytes(&msg[8+14], mask_radius,                2);
  unpackbytes(&msg[8+33], source,                     1);
  unpackbytes(&msg[8+36], pattern_A,                  2);
  unpackbytes(&msg[8+38], pattern_n,                  2);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ : 28.11.06 - RCFLAG to reset status fields in SRECEIVST (no parameter)

void create_RCFLAG(_tc *tc) {

  int8 msg[] = {
// RCFLAG Telecommand
// header
  0x9C, 0x3E, 0x16, 0xB4,      // preamble & identifier (0x16B4)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // Command Word                           <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00,                  // reserved
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ: 03.05.07 RCTR - to modify the tracking parameters

void create_RCTR(int16 field_val, short *code_K, short *carrier_K,
     short *interf_K, int8 correl, int8 chip, short track_01,
     short track_02, short track_03, _tc *tc) {

  int8 msg[] = {
// RCTR Telecommand
// header
  0x9C, 0x3E, 0x1C, 0xCF,      // preamble & identifier 0x1CCF
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00,                  // reserved
  0x00, 0x00,                  // validity of field                      <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00,                  // reserved
//
// Tracking parameters
  0x00, 0x00,                  // code loop parameters K1                <<<===
  0x00, 0x00,                  // code loop parameters K2                <<<===
  0x00, 0x00,                  // code loop parameters K3                <<<===
  0x00, 0x00,                  // carrier loop parameters K1             <<<===
  0x00, 0x00,                  // carrier loop parameters K2             <<<===
  0x00, 0x00,                  // carrier loop parameters K3             <<<===
  0x00, 0x00,                  // interferometric loop parameters K1     <<<===
  0x00, 0x00,                  // interferometric loop parameters K2     <<<===
  0x00, 0x00,                  // interferometric loop parameters K3     <<<===
  0x00,                        // correlator type                        <<<===
  0x00,                        // chip spacing                           <<<===
  0x00, 0x00,                  // Tracking threshold O1                  <<<===
  0x00, 0x00,                  // Tracking threshold O2                  <<<===
  0x00, 0x00,                  // Tracking threshold O3                  <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00,                  // reserved
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  int i;
  
  unpackbytes(&msg[8+ 4], field_val,                  2);
  for (i=0; i<3; i++) unpackbytes(&msg[8+12+2*i], code_K[i],    2);
  for (i=0; i<3; i++) unpackbytes(&msg[8+18+2*i], carrier_K[i], 2);
  for (i=0; i<3; i++) unpackbytes(&msg[8+24+2*i], interf_K[i],  2);
  unpackbytes(&msg[8+31], chip,                       1);
  unpackbytes(&msg[8+32], track_01,                   2);
  unpackbytes(&msg[8+34], track_02,                   2);
  unpackbytes(&msg[8+36], track_03,                   2);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ : 05.04.07 - RNAV to provide navigation aiding data PVT to the receiver

void create_RNAV(short week, long second, long sec_fract,
     long pos_x, long pos_y, long pos_z, long vel_x, long vel_y, long vel_z,
     long drift, long pos_FOM, long vel_FOM, long drift_FOM, long time_FOM,
     _tc *tc) {

  int8 msg[] = {
// RNAV Telecommand
// header
  0x9C, 0x3E, 0x13, 0x3C,      // preamble & identifier (0x133C)
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
//
// GPS Time of the data
  0x00, 0x00,                  // week number                            <<<===
  0x00, 0x00, 0x00, 0x00,      // Second in the week                     <<<===
  0x00, 0x00, 0x00, 0x00,      // Second fraction                        <<<===
//
// Position in WGS84 ECEF
  0x00, 0x00, 0x00, 0x00,      // X                                      <<<===
  0x00, 0x00, 0x00, 0x00,      // Y                                      <<<===
  0x00, 0x00, 0x00, 0x00,      // Z                                      <<<===
//
// Velocity in WGS84 ECEF
  0x00, 0x00, 0x00, 0x00,      // X                                      <<<===
  0x00, 0x00, 0x00, 0x00,      // Y                                      <<<===
  0x00, 0x00, 0x00, 0x00,      // Z                                      <<<===
//
// Local clock drift
  0x00, 0x00, 0x00, 0x00,      // Local clock drift                      <<<===
//
// Figure of merit
  0x00, 0x00, 0x00, 0x00,      // Position FOM                           <<<===
  0x00, 0x00, 0x00, 0x00,      // Velocity FOM                           <<<===
  0x00, 0x00, 0x00, 0x00,      // Local clock drift FOM                  <<<===
  0x00, 0x00, 0x00, 0x00,      // Time FOM                               <<<===
//
  0x00, 0x00, 0x00, 0x00,      // reserved
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 6], week,                       2);
  unpackbytes(&msg[8+ 8], second,                     4);
  unpackbytes(&msg[8+12], sec_fract,                  4);
  unpackbytes(&msg[8+16], pos_x,                      4);
  unpackbytes(&msg[8+20], pos_y,                      4);
  unpackbytes(&msg[8+24], pos_z,                      4);
  unpackbytes(&msg[8+28], vel_x,                      4);
  unpackbytes(&msg[8+32], vel_y,                      4);
  unpackbytes(&msg[8+36], vel_z,                      4);
  unpackbytes(&msg[8+40], drift,                      4);
  unpackbytes(&msg[8+44], pos_FOM,                    4);
  unpackbytes(&msg[8+48], vel_FOM,                    4);
  unpackbytes(&msg[8+52], drift_FOM,                  4);
  unpackbytes(&msg[8+56], time_FOM,                   4);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// CZ: 03.05.07 - RPSEUD to command the signal processing channels

void create_RPSEUD(int16 cha_ID, int16 cmd, int16 sig_src, 
     short sig_src_PRN, short doppler, _tc *tc) {

  int8 msg[] = {
// RPSEUD Telecommand
// header
  0x9C, 0x3E, 0x16, 0x88,      // preamble & identifier
  0x00, 0x00, 0x00, 0x00,      // place for checksum                     <<<===
//
  0x00,                        // reserved
  0xFF,                        // validity                               <<<===
  0x00, 0x00, 0x00, 0x00,      // reserved
  0x00, 0x00,                  // channel ID                             <<<===
  0x00, 0x00,                  // command word                           <<<===
  0x00, 0x00,                  // signal source type                     <<<===
  0x00, 0x00,                  // signal source PRN                      <<<===
  0x00, 0x00,                  // Doppler                                <<<===
//
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 6], cha_ID,                     2);
  unpackbytes(&msg[8+ 8], cmd,                        2);
  unpackbytes(&msg[8+10], sig_src,                    2);
  unpackbytes(&msg[8+12], sig_src_PRN,                2);
  unpackbytes(&msg[8+14], doppler,                    2);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// RSYSTEM TC: Used for on-ground tests

void create_RSYSTEM(void) {
}

//~----------------------------------------------------------------------------
// RTIME TC: Transmittion of time to the receiver

void create_RTIME(int16 type, short week, long second, _tc *tc) {
     
  int8 msg[] = {
// RTIME Telecommand
// header
  0x9C, 0x3E, 0x13, 0x00,     // preamble & identifier
  0x00, 0x00, 0x00, 0x00,     // place for checksum
//
  0x00,                       // reserved
  0xFF,                       // validity                                <<<===
  0x00, 0x00,                 // reserved
  0x00, 0x01,                 // type of data                            <<<===
#if 1
// GPS absolute / relative time
  0x00, 0x00,                 // week number                             <<<===
  0x00, 0x00, 0x00, 0x00,     // seconds in the week                     <<<===
  0x00, 0x00, 0x00, 0x00,     // fraction of second                      <<<===
  0x02, 0x00, 0x00, 0x00,     // time FOM                                <<<===
#endif
#if 0
// Delta GPS Time - UTC
  0x00, 0x00,                 // reserved
  0x00, 0x02, 0xF1, 0x78,     // seconds                                 <<<===
  0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00,     // reserved
#endif
#if 0
// Local clock drift
  0x00, 0x00,                 // reserved
  0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x02, 0xF1, 0x78,     // local clock drift                       <<<===
  0x02, 0x00, 0x00, 0x00,     // local clock drift FOM                   <<<===
#endif
//
  0x00, 0x00, 0x00, 0x00,     // reserved
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 4], type,                       2);
  unpackbytes(&msg[8+ 6], week,                       2);
  unpackbytes(&msg[8+ 8], second,                     4);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------
// WMODE TC: Selection of the functional mode of the receiver

void create_WMODE(int16 cmd, bool PPS, _tc *tc) {

  int8 msg[] = {
// WMODE Telecommand
// header
  0x9c, 0x3E, 0x10, 0x14,     // preamble & identifier
  0x00, 0x00, 0x00, 0x00,     // place for checksum
// control of modes
  0x00,                       // reserved
  0xFF,                       // validity
  0x12, 0x34,                 // type of command                         <<<===
  0x02, 0x00, 0x12, 0x38,     // address of start
  0x00, 0x00, 0x00, 0x00,     // reserved
// control of configuration
  0x02, 0x00, 0xB8, 0x00,     // address of configuration zone
  0x13, 0x00, 0x12, 0x28,     // address of ZPMEM
  0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x01,                 // request for change
  0x00, 0x00, 0x00,           // reserved
  0x03,                       // configuration for localisation
  0x00,                       // reserved
  0x00,                       // configuration for attitude
  0x00, 0x00, 0x00,           // reserved
  0x01,                       // configuration for cold start
  0x00,                       // reserved
  0x01,                       // config for PPS (0=free, 1=locked)       <<<===
  0x00,                       // reserved
  0x00,                       // Y2015 extension
  0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00,     // reserved 
  0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00, 0x00, 0x00,     // reserved
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}; 

  unpackbytes(&msg[8+ 2], cmd,                        2);
  unpackbytes(&msg[8+37], PPS,                        1);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(tc->dat, msg, sizeof(msg));
  tc->cnt = sizeof(msg);
}

//~============================================================================
//
//                  SIMULATION TOOLS
//
//~============================================================================

void create_STIME(int32 week, int32 second, int *TM_cnt, int8 *TM_dat) {

  int8 msg[] = {
// header
  0x9C, 0x3E, 0x23, 0x00,     // preamble & identifier
  0x00, 0x00, 0x00, 0x00,     // place for checksum
//
  0x00,                       // reserved
  0x00,                       // validity                                <<<===
  0x00, 0x00, 0x00,           // reserved
  0x00,                       // Source of GPS time                      <<<===
// GPS time of PPS
  0x00, 0x00,                 // week number                             <<<===
  0x00, 0x00, 0x00, 0x00,     // seconds in week                         <<<===
  0x00, 0x00, 0x00, 0x00,     // fraction of second                      <<<===
  0x00, 0x00, 0x00, 0x00,     // time FOM                                <<<===
// UTC time of PPS
  0x00, 0x00,                 // week number                             <<<===
  0x00, 0x00, 0x00, 0x00,     // seconds in week                         <<<===
  0x00, 0x00, 0x00, 0x00,     // fraction of second                      <<<===
  0x00, 0x00, 0x00, 0x00,     // reserved
  0x00, 0x00,                 // reserved
// Local time
  0x00, 0x00, 0x00, 0x00,     // local time                              <<<===
  0x00,                       // reserved
// GPS time of syncronized message (1553 only)
  0xFF,                       // validity                                <<<===
  0x00, 0x00,                 // week number                             <<<===
  0x00, 0x00, 0x00, 0x00,     // seconds in week                         <<<===
  0x00, 0x00, 0x00, 0x00,     // fraction of second                      <<<===
  0x00, 0x00, 0x00, 0x00,     // identifier of the syncronisation message
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[8+ 6], week,                       2);
  unpackbytes(&msg[8+ 8], second,                     4);
  unpackbytes(&msg[8+20], week,                       2);
  unpackbytes(&msg[8+22], second,                     4);
  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(TM_dat, msg, sizeof(msg));
  *TM_cnt = sizeof(msg);
}

//~----------------------------------------------------------------------------

void create_SDUMP(int *TM_cnt, int8 *TM_dat) {

  int8 msg[] = {
// header
  0x9C, 0x3E, 0x2F, 0x0F,     // preamble & identifier
  0x00, 0x00, 0x00, 0x00,     // place for checksum
//
  0x00,                       // reserved
  0x00,                       // validity                                <<<===
  0x00, 0x00,                 // reserved
  0xDE, 0xAD, 0xBE, 0xEF,     // Starting address of DUMP                <<<===
  0x00, 0x00, 0x00, 0x07,     // Number of 16-byte block to dump         <<<===
// Block of 16 Bytes (1/3)
  0x01, 0x02, 0x03, 0x04,     //                                         <<<===
  0x05, 0x06, 0x07, 0x08,     //                                         <<<===
  0x09, 0x0A, 0x0B, 0x0C,     //                                         <<<===
  0x0D, 0x0E, 0x0F, 0xAA,     //                                         <<<===
// Block of 16 Bytes (2/3)
  0xAA, 0xBB, 0xCC, 0xDD,     //                                         <<<===
  0x01, 0x02, 0x03, 0x04,     //                                         <<<===
  0x10, 0x20, 0x30, 0x40,     //                                         <<<===
  0xDE, 0xAD, 0xBE, 0xEF,     //                                         <<<===
// Block of 16 Bytes (3/3)
  0x00, 0x11, 0x22, 0x33,     //                                         <<<===
  0x44, 0x55, 0x66, 0x77,     //                                         <<<===
  0x88, 0x88, 0xAA, 0xBB,     //                                         <<<===
  0xCC, 0xDD, 0xEE, 0xFF,     //                                         <<<===
// Checksum
  0x01, 0x23, 0x45, 0x67,     //                                         <<<===
// terminator
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

  unpackbytes(&msg[0+ 4], checksum(sizeof(msg), msg), 4);

  memcpy(TM_dat, msg, sizeof(msg));
  *TM_cnt = sizeof(msg);
}

//~============================================================================
//
//                  PRINT TELEMETRY INFORMATION IN "ALCATEL" FORMAT
//
//~============================================================================

void process_ANYTM(_TM *TM) {

  int i, j;
  
  printf("\n*** %-10s *** %s\n", TM->nam, TM->desc);

//print_TM_message(TM);

  for (i=0; i<100; i++) {
    int bgn = TM->par[i].bgn;
    int end = TM->par[i].end;
    int siz = end - bgn + 1;
    char fmt[80];
    int64 q = 0;
    if (!TM->par[i].nam) break;
    printf("%-30s ", TM->par[i].nam);
    sprintf(fmt, "%s", TM->par[i].fmt);
    for (j=0; j<siz; j++) q = (q << 8) | TM->msg[bgn+j];
    if (index(fmt, 'f') || index(fmt, 'e')) {
      if (siz == 4) printf(fmt, *(float*)&q);
      else          printf(fmt, *(double*)&q);
    }
    else {
      if (siz != 16) {
        if (index(fmt, 'd')) printf(fmt, *(signed*)&q);
        else                 printf(fmt,            q);
      }
      else {
        for (j=0; j<16; j++) printf("%02X %s", TM->msg[bgn+j], j==15?"\n":"");
      }
    }
    if (TM->par[i].unit) printf(" %s", TM->par[i].unit);
    if (TM->par[i].mult) printf(" %f", TM->par[i].mult);
    printf("\n");
  }
}
  
//~============================================================================
//
//                  OPERATIONAL TELEMETRY
//
//~============================================================================

//~----------------------------------------------------------------------------
// STIME TM: GPS/UTC Time

void process_STIME(_TM *TM) {

  int16 GPS_val, UTC_val, GPS_src;
  int16 GPS_week, UTC_week;
  int32 GPS_sec, GPS_frac, GPS_FOM, UTC_sec, UTC_frac, loc_time;
  time_t t;
  struct tm r;
  char GPS_time[100], UTC_time[100];

  GPS_val = TM->msg[1] & 0x0F;
  UTC_val = TM->msg[1] >> 4;
  
  GPS_src = TM->msg[5];

  GPS_week =                   TM->msg[ 6];
  GPS_week = (GPS_week << 8) + TM->msg[ 7];
  GPS_sec  =                   TM->msg[ 8];
  GPS_sec  = (GPS_sec  << 8) + TM->msg[ 9];
  GPS_sec  = (GPS_sec  << 8) + TM->msg[10];
  GPS_sec  = (GPS_sec  << 8) + TM->msg[11];
  GPS_frac =                   TM->msg[12];
  GPS_frac = (GPS_frac << 8) + TM->msg[13];
  GPS_frac = (GPS_frac << 8) + TM->msg[14];
  GPS_frac = (GPS_frac << 8) + TM->msg[15];
  GPS_FOM  =                   TM->msg[16];
  GPS_FOM  = (GPS_FOM  << 8) + TM->msg[17];
  GPS_FOM  = (GPS_FOM  << 8) + TM->msg[18];
  GPS_FOM  = (GPS_FOM  << 8) + TM->msg[19];

  UTC_week =                   TM->msg[20];
  UTC_week = (UTC_week << 8) + TM->msg[21];
  UTC_sec  =                   TM->msg[22];
  UTC_sec  = (UTC_sec  << 8) + TM->msg[23];
  UTC_sec  = (UTC_sec  << 8) + TM->msg[24];
  UTC_sec  = (UTC_sec  << 8) + TM->msg[25];
  UTC_frac =                   TM->msg[26];
  UTC_frac = (UTC_frac << 8) + TM->msg[27];
  UTC_frac = (UTC_frac << 8) + TM->msg[28];
  UTC_frac = (UTC_frac << 8) + TM->msg[29];

  loc_time =                   TM->msg[36];
  loc_time = (loc_time << 8) + TM->msg[37];
  loc_time = (loc_time << 8) + TM->msg[38];
  loc_time = (loc_time << 8) + TM->msg[39];

  t = 0x12D53D80 + 604800 * GPS_week + GPS_sec;
  localtime_r(&t, &r);
  sprintf(GPS_time, "%02d/%02d/%02d %02d:%02d:%02d.%09d", 
          r.tm_mday, 1+r.tm_mon, 1900+r.tm_year,
          r.tm_hour, r.tm_min, r.tm_sec, GPS_frac);

  t = 0x12D53D80 + 604800 * UTC_week + UTC_sec;
  localtime_r(&t, &r);
  sprintf(UTC_time, "%02d/%02d/%02d %02d:%02d:%02d.%09d", 
          r.tm_mday, 1+r.tm_mon, 1900+r.tm_year,
          r.tm_hour, r.tm_min, r.tm_sec, UTC_frac);

  printf("%s%s: ", timestamp(0), TM->nam);
  printf("GPS: %X %s  UTC: %X %s  Local=%d\n", 
          GPS_val, GPS_time, UTC_val, UTC_time, loc_time);
}

//~----------------------------------------------------------------------------
// SNAVS TM: Position and velocity

void process_SNAVS(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// SPHINT TM: interferometric carrier phase measurements

void process_SPHINT(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// SVELINT TM: interferometric carrier phase velocity

void process_SVELINT(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// SCOSDIR TM: direction cosines of tracked satellites

void process_SCOSDIR(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// CZ : 20.06.07 - TM for RECEIVST (Receiver status)

void process_SRECEIVST(_TM *TM) {

  int i;
  int16 SREC_val, SREC_localt, SREC_pegase, SREC_sigproc;
  int16 SREC_hardw, SREC_procmem, SREC_localis, SREC_coldstr;
  int16 SREC_pps, SREC_y2015, SREC_antenna, SREC_angle;
  int16 SREC_snapres, SREC_n2gres, SREC_almanac, SREC_satselect, SREC_visib;
  int16 SREC_snapst, SREC_n2gst, SREC_receivtc, SREC_standby;

  SREC_val     =                         TM->msg[ 1];
  SREC_localt  =                         TM->msg[ 2];
  SREC_localt  = (SREC_localt << 8)    | TM->msg[ 3];
  SREC_pegase  =                         TM->msg[ 4];
  SREC_pegase  = (SREC_pegase << 8)    | TM->msg[ 5];
  SREC_sigproc =                         TM->msg[ 8];
  SREC_sigproc = (SREC_sigproc << 8)   | TM->msg[ 9];
  SREC_hardw   =                         TM->msg[10];
  SREC_hardw   = (SREC_hardw << 8)     | TM->msg[11];
  SREC_procmem =                         TM->msg[12];
  SREC_procmem = (SREC_hardw << 8)     | TM->msg[13];
  SREC_localis =                         TM->msg[17];
  SREC_coldstr =                         TM->msg[23];
  SREC_pps     =                         TM->msg[25];
  SREC_y2015   =                         TM->msg[27];
  SREC_antenna =                         TM->msg[28];
  SREC_antenna = (SREC_antenna << 8)   | TM->msg[29];
  SREC_angle   =                         TM->msg[36];
  SREC_angle   = (SREC_angle << 8)     | TM->msg[37];
  SREC_snapres =                         TM->msg[44];
  SREC_n2gres  =                         TM->msg[45];
  SREC_almanac =                         TM->msg[47];
  SREC_satselect =                       TM->msg[48];
  SREC_visib   =                         TM->msg[49];
  SREC_snapst  =                         TM->msg[50];
  SREC_n2gst   =                         TM->msg[51];
  SREC_receivtc =                        TM->msg[60];
  SREC_receivtc = (SREC_receivtc << 8) | TM->msg[61];
  SREC_standby =                         TM->msg[62];

  printf("%s%s: ", timestamp(0), TM->nam);
  printf("VALIDITY: %X: LOCTIME: %d LOCALISATION: %d PPS: %d SNAPSHOT: %d N2G: %X ALMANAC: %d VISIBILITY %d\n",
          SREC_val,    SREC_localt, SREC_localis,    SREC_pps, SREC_snapres, SREC_n2gres, SREC_almanac, SREC_visib);

  for (i=2; i<61; i++) {
    printf("%X:", TM->msg[i]);
  }
  printf("\n");
}

//~----------------------------------------------------------------------------
// SDUMP TM: Dump results

void process_SDUMP(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~============================================================================
//
//                  CONTROL TELEMETRY
//
//~============================================================================

//~----------------------------------------------------------------------------
// SNAVI TM: Position and velocity of SNAPSHOT

void process_SNAVI(_TM *TM) {

  int i;
  int16 SNAVI_val, SNAVI_localt, SNAVI_weekn;
  int32 SNAVI_locdft, SNAVI_pfom, SNAVI_vfom, SNAVI_lcdfom, SNAVI_tfom;
  int32 SNAVI_secondw, SNAVI_secondf, SNAVI_px, SNAVI_py, SNAVI_pz;
  int32 SNAVI_vx, SNAVI_vy, SNAVI_vz;

  SNAVI_val     =                         TM->msg[ 1];
  SNAVI_localt  =                         TM->msg[ 2];
  SNAVI_localt  = (SNAVI_localt << 8)   | TM->msg[ 3];
  SNAVI_weekn   =                         TM->msg[ 6];
  SNAVI_weekn   = (SNAVI_weekn << 8)    | TM->msg[ 7];
  SNAVI_secondw =                         TM->msg[ 8];
  SNAVI_secondw = (SNAVI_secondw << 8)  | TM->msg[ 9];
  SNAVI_secondw = (SNAVI_secondw << 8)  | TM->msg[10];
  SNAVI_secondw = (SNAVI_secondw << 8)  | TM->msg[11];
  SNAVI_secondf =                         TM->msg[12];
  SNAVI_secondf = (SNAVI_secondf << 8)  | TM->msg[13];
  SNAVI_secondf = (SNAVI_secondf << 8)  | TM->msg[14];
  SNAVI_secondf = (SNAVI_secondf << 8)  | TM->msg[15];
  SNAVI_px      =                         TM->msg[16];
  SNAVI_px      = (SNAVI_px << 8)       | TM->msg[17];
  SNAVI_px      = (SNAVI_px << 8)       | TM->msg[18];
  SNAVI_px      = (SNAVI_px << 8)       | TM->msg[19];
  SNAVI_py      =                         TM->msg[20];
  SNAVI_py      = (SNAVI_py << 8)       | TM->msg[21];
  SNAVI_py      = (SNAVI_py << 8)       | TM->msg[22];
  SNAVI_py      = (SNAVI_py << 8)       | TM->msg[23];
  SNAVI_pz      =                         TM->msg[24];
  SNAVI_pz      = (SNAVI_pz << 8)       | TM->msg[25];
  SNAVI_pz      = (SNAVI_pz << 8)       | TM->msg[26];
  SNAVI_pz      = (SNAVI_pz << 8)       | TM->msg[27];
  SNAVI_vx      =                         TM->msg[28];
  SNAVI_vx      = (SNAVI_vx << 8)       | TM->msg[29];
  SNAVI_vx      = (SNAVI_vx << 8)       | TM->msg[30];
  SNAVI_vx      = (SNAVI_vx << 8)       | TM->msg[31];
  SNAVI_vy      =                         TM->msg[32];
  SNAVI_vy      = (SNAVI_vy << 8)       | TM->msg[33];
  SNAVI_vy      = (SNAVI_vy << 8)       | TM->msg[34];
  SNAVI_vy      = (SNAVI_vy << 8)       | TM->msg[35];
  SNAVI_vz      =                         TM->msg[36];
  SNAVI_vz      = (SNAVI_vz << 8)       | TM->msg[37];
  SNAVI_vz      = (SNAVI_vz << 8)       | TM->msg[38];
  SNAVI_vz      = (SNAVI_vz << 8)       | TM->msg[39];
  SNAVI_locdft  =                         TM->msg[40];
  SNAVI_locdft  = (SNAVI_locdft << 8)   | TM->msg[41];
  SNAVI_locdft  = (SNAVI_locdft << 8)   | TM->msg[42];
  SNAVI_locdft  = (SNAVI_locdft << 8)   | TM->msg[43];
  SNAVI_pfom    =                         TM->msg[44];
  SNAVI_pfom    = (SNAVI_pfom << 8)     | TM->msg[45];
  SNAVI_pfom    = (SNAVI_pfom << 8)     | TM->msg[46];
  SNAVI_pfom    = (SNAVI_pfom << 8)     | TM->msg[47];
  SNAVI_vfom    =                         TM->msg[48];
  SNAVI_vfom    = (SNAVI_vfom << 8)     | TM->msg[49];
  SNAVI_vfom    = (SNAVI_vfom << 8)     | TM->msg[50];
  SNAVI_vfom    = (SNAVI_vfom << 8)     | TM->msg[51];
  SNAVI_lcdfom  =                         TM->msg[52];
  SNAVI_lcdfom  = (SNAVI_lcdfom << 8)   | TM->msg[53];
  SNAVI_lcdfom  = (SNAVI_lcdfom << 8)   | TM->msg[54];
  SNAVI_lcdfom  = (SNAVI_lcdfom << 8)   | TM->msg[55];
  SNAVI_tfom    =                         TM->msg[56];
  SNAVI_tfom    = (SNAVI_tfom << 8)     | TM->msg[57];
  SNAVI_tfom    = (SNAVI_tfom << 8)     | TM->msg[58];
  SNAVI_tfom    = (SNAVI_tfom << 8)     | TM->msg[59];

  printf("%s%s: ", timestamp(0), TM->nam);
  printf("VALIDITY: %X: LOCTIME: %d WEEK: %X: SECOND: %d FRACTION: %d PFOM: %d VFOM: %d LCDFOM: %X TFOM: %d\n", 
         SNAVI_val, SNAVI_localt, SNAVI_weekn, SNAVI_secondw, SNAVI_secondf, SNAVI_pfom, SNAVI_vfom, SNAVI_lcdfom, SNAVI_tfom);

  for (i=2; i<59; i++) {
    printf("%X:", TM->msg[i]);
  }
  printf("\n");
}

//~----------------------------------------------------------------------------
// SNAVN TM: Position and velocity of the Navigator

void process_SNAVN(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// SALEPH TM: Almanac or Ephemeris

void process_SALEPH(_TM *TM) {

  int i;

  printf("%s%s\n", timestamp(0), TM->nam);

  for (i=2; i<64; i++) {
    printf("%X:", TM->msg[i]);
  }
  printf("\n");
}

//~----------------------------------------------------------------------------
// S1RAWD TM: Code and carrier raw measurements

void process_S1RAWD(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// S2RAWD TM: Code and carrier raw measurements

void process_S2RAWD(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// S3RAWD TM: Code and carrier raw measurements

void process_S3RAWD(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// S1CHANST TM: Status of the channels

void process_S1CHANST(_TM *TM) {

  int i;
  int16 val;
  float loc_time;
  
  printf("%s%s: ", timestamp(0), TM->nam);
  
  val = TM->msg[1];

  loc_time = (float)((TM->msg[2] << 8) + TM->msg[3]) / 10.0;
  
  printf("Validity = %X, ", val);
  printf("Local time = %6.1f\n", loc_time);

  for (i=8; i<64; i+=4) {
    int j = (i - 8) / 4 + 1;
    printf("%2d:%02hhX:%05.2f:%d%d%d:%02d ", 
            j, TM->msg[i], (float)TM->msg[i+1]*0.25, 
            TM->msg[i+2]&0x7, (TM->msg[i+2]>>4)&0x3, (TM->msg[i+2]>>6)&0x3,
            TM->msg[i+3]);
    if ((j == 7) || (j == 14)) printf("\n");
  }
}

//~----------------------------------------------------------------------------
// S2CHANST TM: Status of the channels

void process_S2CHANST(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~============================================================================
//
//                  INIT ANS SOFTWARE UPLOADING TELEMETRY
//
//~============================================================================

//~----------------------------------------------------------------------------
// LSTATUS TM: Init status and progress status of software upload

void process_LSTATUS(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~============================================================================
//
//                  RESERVED TELEMETRY
//
//~============================================================================


//~----------------------------------------------------------------------------
// NS1VECT TM: Parameters of state vector of the Navigator

void process_NS1VECT(_TM *TM) {

  int i;
  int16 NS1V_val, NS1V_localt, NS1V_vald;
  long NS1V_sma, NS1V_ex;

  printf("%s%s: ", timestamp(0), TM->nam);

  NS1V_val     =                         TM->msg[ 1];
  NS1V_localt  =                         TM->msg[ 2];
  NS1V_localt  = (NS1V_localt << 8)    | TM->msg[ 3];
  NS1V_vald    =                         TM->msg[15];
  NS1V_sma     =                         TM->msg[16];
  NS1V_sma     = (NS1V_sma << 8)       | TM->msg[17];
  NS1V_sma     = (NS1V_sma << 8)       | TM->msg[18];
  NS1V_sma     = (NS1V_sma << 8)       | TM->msg[19];
  NS1V_sma     = (NS1V_sma << 8)       | TM->msg[20];
  NS1V_sma     = (NS1V_sma << 8)       | TM->msg[21];
  NS1V_sma     = (NS1V_sma << 8)       | TM->msg[22];
  NS1V_sma     = (NS1V_sma << 8)       | TM->msg[23];
  NS1V_ex      =                         TM->msg[24];
  NS1V_ex      = (NS1V_ex << 8)        | TM->msg[25];
  NS1V_ex      = (NS1V_ex << 8)        | TM->msg[26];
  NS1V_ex      = (NS1V_ex << 8)        | TM->msg[27];
  NS1V_ex      = (NS1V_ex << 8)        | TM->msg[28];
  NS1V_ex      = (NS1V_ex << 8)        | TM->msg[29];
  NS1V_ex      = (NS1V_ex << 8)        | TM->msg[30];

  for (i=1; i<30; i++) {
    printf("%X:", TM->msg[i]);
  }
  printf("\n");
}

//~----------------------------------------------------------------------------
// NS2VECT TM: Pole, Thrust, Drag, Hill, CMS of the Navigator

void process_NS2VECT(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}
  
//~----------------------------------------------------------------------------
// NS3VECT TM: Data and clock data of state vector of the Navigator

void process_NS3VECT(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// NDIAG TM: Diagonal of covariance matrix of the Navigator

void process_NDIAG(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// NRES TM: Residuals of measurements of the Navigator

void process_NRES(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// S1SYSTEM TM: Used for on-ground tests

void process_S1SYSTEM(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// S2SYSTEM TM: Used for on-ground tests

void process_S2SYSTEM(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// S3SYSTEM TM: Used for on-ground tests

void process_S3SYSTEM(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~----------------------------------------------------------------------------
// SPVTDAT TM: Position, velocity and datation data (DEMETER specific TM)

void process_SPVTDAT(_TM *TM) {

  printf("%s%s\n", timestamp(0), TM->nam);
}

//~============================================================================
