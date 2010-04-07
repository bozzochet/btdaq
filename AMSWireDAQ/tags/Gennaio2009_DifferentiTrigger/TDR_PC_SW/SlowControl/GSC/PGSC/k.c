#include <ams/types.h>
#include <ams/oiutcp.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>

#include <sys/mode.h>
#include <sys/types.h>
#include <sys/stat.h>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void main( void) {

  char IP_address[] = "localhost";		// boron at GSI
//char IP_address[] = "129.132.69.57";		// amsboron.ethz.ch
//char IP_address[] = "130.232.68.1";		// helium
//char IP_address[] = "130.232.68.2";		// ahelium
//char IP_address[] = "134.232.68.171";		// carbon
//char IP_address[] = "134.232.68.172";		// acarbon
  char serverName[256];
  char msg[256];

  if (openTCCconn( msg, IP_address, serverName) < 0) {
    printf( "Stopper FAILED\n");
    exit( 0);
  }
  printf( "Regular commanding stopped...\n");
  printf( "Press <Enter> to restart it!");
  gets( msg);
  exit( 0);
}
