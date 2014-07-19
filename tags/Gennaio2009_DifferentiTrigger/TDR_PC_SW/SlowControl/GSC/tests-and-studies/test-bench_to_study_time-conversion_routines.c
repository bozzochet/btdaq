// 1.c
//  test-bench to study time-conversion routines

#include <stdio.h>
#include <time.h>
#include <string.h>
main() {
  struct tm local_tm;
  struct tm GMT_tm;
  time_t now, local_t, GMT_t;
  double diff;
  char *status;
  char b[256];
  while (1) {
    printf( "---------------------------------\n");
    now = time( NULL);
    printf( "now        = %8.8X   ", now);
    printf( " %s", ctime( &now));
    printf( "Enter time (GMT) <- ");
    gets( b);
    memcpy( (char *)&local_tm, (char *)localtime( &now), sizeof( local_tm));
    diff = difftime( mktime( localtime( &now)), mktime( gmtime( &now)));
    status = strptime( b, "%H:%M", &local_tm);
    if (status == NULL) printf( "NULL\n");
    local_t = mktime( &local_tm);
//  GMT_t = (double) local_t + diff;
//  GMT_t = (double) mktime( &local_tm) + diff;
    GMT_t = mktime( &local_tm) + mktime( localtime( &now)) - mktime( gmtime( &now));
    printf( "local_time = %8.8X   ", local_t);
    printf( " %s", ctime( &local_t));
    printf( "GMT_time   = %8.8X   ", GMT_t);
    printf( " %s", ctime( &GMT_t));
  }
}
/*
  The mktime function converts the tm structure pointed to by the timeptr
  parameter to a time_t variable.  The function uses the following members of
  the tm structure:

       tm_year
       tm_mon
       tm_mday
       tm_hour
       tm_min
       tm_sec
       tm_isdst
*/
