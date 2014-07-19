// 11.c
// test bench for a time bomb

#include <signal.h>
#include <sys/time.h>
void time_bomb( void) {
  printf( "\nNo interactive activity observed - program terminated!\n\n");
  exit( 0);
}
void activate_time_bomb( int mins) {
  struct itimerval it;
  memset( &it, 0, sizeof(it));
  it.it_value.tv_sec = 60 * mins;
  signal( SIGALRM, time_bomb);
  setitimer( ITIMER_REAL, &it, (struct itimer *) 0);
}
main() {
  activate_time_bomb( 1);
  while(1);
}
