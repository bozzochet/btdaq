// file gps-time-study.c
//
// GPS time conversion library studies
//
// A.Lebedev - Nov 2005...
//

#include "mylib.h"

//~============================================================================

int main(int argc, char* argv[]) {

  struct tm r;
  time_t    t, tt, unx, gps, now;
  int32 week, sec;
  char b[80];
  
  t = time(NULL);
  if (argc > 1) t = strtoul(argv[1], NULL, 16);
  printf("t  = %08X  ", (int32)t);
//gmtime_r(&t, &r);
  localtime_r(&t, &r);
  printf("%02d/%02d/%04d  ", r.tm_mday, 1+r.tm_mon, 1900+r.tm_year);
  printf("%02d:%02d:%02d\n", r.tm_hour, r.tm_min, r.tm_sec);
  tt = mktime(&r);
  printf("tt = %08X\n", (int32)tt);
  
  printf("\n");
  r.tm_year = 70;
  r.tm_mon  =  0;
  r.tm_mday =  1;
  r.tm_hour =  1;    // UTC
  r.tm_min  =  0;
  r.tm_sec  =  0;
  unx = mktime(&r);
  printf("unx = %08X\n", (int32)unx);

  printf("\n");
  r.tm_year = 80;
  r.tm_mon  =  0;
  r.tm_mday =  6;
  r.tm_hour =  1;    // UTC
  r.tm_min  =  0;
  r.tm_sec  =  0;
  gps = mktime(&r);
  printf("gps = %08X\n", (int32)gps);
  
  printf("\n");
  r.tm_year = 105;
  r.tm_mon  =  11;
  r.tm_mday =   4;
  r.tm_hour =   1;    // UTC
  r.tm_min  =   0;
  r.tm_sec  =   0;
  now = mktime(&r);
  printf("now = %08X\n", (int32)now);
  
  now = now - 0x12D53D80;
  week = now / 604800;
  sec  = now % 604800;
  printf("now, week, sec = %d %d %d (for check)\n", (int32)now, week, sec);

//~--

  printf("\n");
  now = time(NULL);
  printf("now = %08X\n", (int32)now);
  
  now  = now - 0x12D53D80;
  week = now / 604800;
  sec  = now % 604800;
  printf("now, week, sec = %d %d %d\n", (int32)now, week, sec);


  now = 0x12D53D80 + 604800 * week + sec;
  printf("now = %08X\n", (int32)now);
  ctime_r(&now, b);
  printf("          1111111111222222222233333333334444444444\n");
  printf("01234567890123456789012345678901234567890123456789\n");
  printf(b);
  
  return 0;
}

//~============================================================================
