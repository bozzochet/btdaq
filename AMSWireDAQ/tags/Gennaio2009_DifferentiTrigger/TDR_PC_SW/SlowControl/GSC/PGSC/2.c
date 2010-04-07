// 2.c
//  reads and checks the file with MCC ping result ( see 1.c )

//-------------------------------------------------------------------

#include <stdio.h>
void main( int argc, char *argv[]) {

  FILE *file;
  int id, n, n_old = 0;
  char b[100];

  setbuf( stdout, NULL);	// set "no buffering" for stdout
  printf( "Big Brother will watch you!\n");
  
  file = fopen( "watch", "r");
  while (1) {
    if (fgets( b, 100, file) == NULL) break;
    sscanf( b, "%x %x", &id, &n);
//  printf( "%d\n", n);
    if ((n_old + 1) != n) printf( "error: n, n_old = %d %d\n", n, n_old);
    n_old = n;
  }
}

