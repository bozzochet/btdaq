
#include <cstdlib>

#include <time.h>

#include <cstdio>



int main(int argc,char **argv){
  tm date;
  date.tm_isdst = -1;
  date.tm_gmtoff=0;
  date.tm_zone=0;
  if (argc<6||argc>7) {
    printf("%s usage:  %s  yyyy mm dd hh mm [ss]\n",argv[0],argv[0]); 
    printf("   Output: unixtime\n");
    exit(2);
  }
  else if (argc==6)
    date.tm_sec=0;
  else if (argc==7)
    date.tm_sec   = atoi( argv[6]);
  
  date.tm_year  = atoi( argv[1])-1900;
  date.tm_mon   = atoi( argv[2])-1;
  date.tm_mday  = atoi( argv[3]);
  date.tm_hour  = atoi( argv[4]);
  date.tm_min   = atoi( argv[5]);
  time_t uxt=mktime(&date);
  if (uxt == -1)
     printf("%s is unknown \n",asctime(&date));
  else {
    printf("%s is %d\n",asctime(&date),uxt);
    
  }

  return 1;
}
