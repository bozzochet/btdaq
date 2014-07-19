// file bus_550_alpha.c

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include <ams/types.h>
#include <ams/oiutcp.h>
#include <time.h>

#include "bc.h"
#include "commander.h"
#define FALSE 0
#define TRUE  1
#define FAIL 0
#define OK   1

#define N 255

/*
#define DELAY_BETWEEN_COMMANDS 0	// seconds
// char IP_address[] = "134.232.68.171";	// helium
// char IP_address[] = "134.232.68.172";	// ahelium
char IP_address[] = "129.132.69.57";		// amsboron.ethz.ch
char serverName[N];
*/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int enable_bus_controller( void);
void disable_bus_controller( void);
void send_PSP( unsigned char *PSP_frame, unsigned short *PSP_status);
int send_command( void);
int send_command_fast( void);

//--------------------------------------------------------------------------
int enable_bus_controller( void) {
  PSP_status  = 1;
  PSP_request = 0;
  return 1;
}
//--------------------------------------------------------------------------
void disable_bus_controller( void) {
}
//--------------------------------------------------------------------------
void send_PSP( unsigned char *PSP_frame, unsigned short *PSP_status) {
#ifndef send_commands_fast
  if (send_command()) *PSP_status = 1;
#else
  if (send_command_fast()) *PSP_status = 1;
#endif
  else                *PSP_status = 0;
}
//--------------------------------------------------------------------------
int send_command_fast( void) {
//char *CIPsim_name = "tx5.srdl";
  char *CIPsim_name = "localhost";
  char msg[100], serverName[50];
  unsigned char reply;
  char *replies[] = { "OK", "FRWD FAIL", "LEN FAIL", "SIZE FAIL"};
  int i, len;
  PSP_frame[PSP_frame[1]] = 0x07;         /* AMS routing code 07 */
  PSP_frame[PSP_frame[1]+1] = 0xFF;
  for (i=0; i<PSP_frame[1]+1; i++) {
    PSP_frame[PSP_frame[1]+1] ^= PSP_frame[i];  /* parity */
  }
//sleep( 1);
  usleep( (useconds_t) 150000);
//if (rand() % 2) return FAIL;
//return OK;
  if (openOIUconn( msg, CIPsim_name, serverName) < 0) {
    printf( "openOIUconn = %s\n", msg);
    goto FAILURE;
  }
//printf( "openOIUconn =  %s\n", msg);
  usleep( (useconds_t) 150000);
  switch ( OIUsend( PSP_frame[1]+2, PSP_frame, msg)) {
  case -1:
//  printf( "OIUsend = %s\n", msg);
    goto FAILURE;
  case  0:
//  printf( "OIUsend = %s\n", msg);
    goto FAILURE;
  default:
    while (!(len = OIUrecv( &reply))) {
      if (len < 0) {
//      printf( "OIUrecv = FATAL error\n");
        goto FAILURE;
      }
    }
//  printf( "OIUrecv = %s\n", replies[(int)reply]);
    if (reply != 0) goto FAILURE;
    goto SUCCESS;
  }  
FAILURE:
  closeOIUconn();
//printf( "closeOIUconn = Connection closed\n", msg); 
  return FAIL;
SUCCESS:
  closeOIUconn();
//printf( "closeOIUconn = Connection closed\n", msg);
  write_commanding_log_file();
  return OK;
}
//--------------------------------------------------------------------------
