//  $Id: time.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <iostream.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <math.h>
main(){
time_t Time;
tm begin;
begin.tm_sec=0;
begin.tm_min=12;
begin.tm_hour=15;
begin.tm_mday=16;
begin.tm_mon=2;
begin.tm_year=98;
begin.tm_isdst=0;
Time=mktime(&begin);
cout <<ctime(&Time)<<endl;

cout <<Time<<endl;
time(&Time);
cout <<Time<<endl;
return 0;
}
