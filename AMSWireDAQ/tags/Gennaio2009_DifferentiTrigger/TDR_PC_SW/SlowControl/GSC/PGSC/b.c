// file b.c
//
// AMS 1553 bus controller for STS-91 PGSC computer (PCMCIA 1553 interface only)
//
// - main program
//
// AMS PGSC BUS CONTROLLER program:
// - read file with command and send it to MCC
// - send GPC command to MCC
// - switch RT address and 1553 bus

#include "bc.h"

#define OK   1
#define FAIL 0
#define integer16 unsigned short

char *given_program_name;

static FILE *file;
static int index = 99;
static int n = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#define MAX_AMS_COMMAND_BLOCK_LENGTH 61         // 16-bit words

integer16                         AMS_command_block[MAX_AMS_COMMAND_BLOCK_LENGTH+2];
#define block_length              AMS_command_block[0]
#define block_ID                  AMS_command_block[1]
#define command_ID                AMS_command_block[2]
static integer16 *command_time = &AMS_command_block[3];
static integer16 *block_body   = &AMS_command_block[5];
#define block_CRC                 AMS_command_block[block_length+1]  // it is outside block !

//-------------------------------------------------------------------

/****************************************************************************/
/*                                                                          */
/*  cyclic redundancy check source was "borrowed" from VK (and "reworked")  */
/*                                                                          */
/****************************************************************************/

integer16 command_block_CRC( void) {
  integer16 *p, FCS, d, i;
  integer16 g = 0x1021;
  FCS = 0xFFFF;
  for (p=AMS_command_block; p<&block_CRC; p++) {
    d = *p;
    for (i=0; i<16; i++) {
      if ((FCS ^ d) & 0x8000)
        FCS = (FCS << 1) ^ g;
      else
        FCS = (FCS << 1);
      d <<= 1;
    }
  }
  return FCS;
}
//-------------------------------------------------------------------

void ctrlc_handler( void) {
  exit( 0);
}

//-------------------------------------------------------------------

void exit_handler( void) {
  disable_bus_controller(); 
  printf("\nLet us try to switch off this damned card\n");
}

//------------------------------------------------------------------------------
int open_file( char *file_name) {    
  if ((file = fopen( file_name, "r")) == NULL) return FAIL;
  n = 0;
  index = 99;
  return OK;
}
//------------------------------------------------------------------------------
int read_file( integer16 *number) {
  char line[81];
  static integer16 v[20];
  int l;
  if (index >= n) {
    while (1) {
      if (fgets( line, 80, file) == NULL) {
        close_file();
        return FAIL;
      }
      l = strlen( line) - 1;
      if (l <= 0) continue;
      line[l] = '\0';
      if (line[0] == '/') continue;
      n = sscanf( line, 
        " %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx",
        &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8], &v[9],
        &v[10], &v[11], &v[12], &v[13], &v[14], &v[15], &v[16], &v[17], &v[18], &v[19]);
      index = 0;
      break;
    }
  }
  *number = v[index++];
  return OK;
}
//------------------------------------------------------------------------------
int close_file( void) {    
  fclose( file);
  n = 0;
  index = 99;
  return OK;
}
//------------------------------------------------------------------------------
int send_AMS_command() {
  time_t now;
  int i;
  PSP_frame[0] = 0;
  PSP_frame[1] = 2 * (block_length + 3);
  for (i=0; i<block_length+2; i++) {
    PSP_frame[2*i+2] = AMS_command_block[i] >> 8;
    PSP_frame[2*i+3] = AMS_command_block[i];
  }
  PSP_request = 1;
  now = time(NULL);
  while (time(NULL) - now < 5) {
    if (PSP_request && PSP_status) {
      send_PSP( PSP_frame, &PSP_status);
      PSP_request = 0;
    } 
    if (PSP_status) {
      return OK;
    }
  }
  PSP_status = 1;
  return FAIL; 
}
//------------------------------------------------------------------------------
int send_GPC_command( int cmd) {
  time_t now;
  int i;
  integer16 GPC_command[6] = { 0xA201, 0xA202, 0xA203, 0xA204, 0xA205, 0xA200};
  block_length = 1;
  block_ID     = GPC_command[cmd];
  block_CRC    = command_block_CRC();
  PSP_frame[0] = 0;
  PSP_frame[1] = 2 * (block_length + 3);
  for (i=0; i<block_length+2; i++) {
    PSP_frame[2*i+2] = AMS_command_block[i] >> 8;
    PSP_frame[2*i+3] = AMS_command_block[i];
  }
  PSP_request = 1;
  now = time(NULL);
  while (time(NULL) - now < 5) {
    if (PSP_request && PSP_status) {
      send_PSP( PSP_frame, &PSP_status);
      PSP_request = 0;
    } 
    if (PSP_status) {
      return OK;
    }
  }
  PSP_status = 1;
  return FAIL; 
}
//------------------------------------------------------------------------------
void main( int argc, char *argv[]) {
  int nb_of_commands;
  char line[81];
  char file_name[81];
  int opt, spec, RT, i, j, k, l, n;
  char *command_name[6] = {
    "POWER STEP 1",   "POWER STEP 2",    "POWER STEP 3", 
    "HEATERS ENABLE", "HEATERS DISABLE", "RESET"       };
  
  given_program_name = argv[0]; 
  setbuf( stdout, NULL);        // set "no buffering" for stdout stream

  signal( SIGINT, ctrlc_handler);
  if (atexit( exit_handler)) printf( "exit handler declaration error, too bad!\n");
  
  
  printf( "\n                 Hello Franklin & Janet!\n");

  if (enable_bus_controller()) {
    printf( "\n                    BUS CONTROLLER OK\n\n");
  }
  else {
    printf( "\n                    BUS CONTROLLER FAILED\n\n");
    exit( 0);
  }

AGAIN:

  printf( "MAIN MENU - Please select one of the following:\n");
  printf( "1.  Load software from file into AMS\n");
  printf( "2.  Send pseudo SPEC command to AMS\n");
  printf( "3.  Switch active RT or 1553 Bus\n");
  printf( "4.  Exit Bus Controller Program\n");
  printf( "MAIN: ");
  fgets( line, 80, stdin);
  opt = atoi( line);
  if (abs(opt) == 0 || abs(opt) > 4) {
    printf( "**** ERROR: invalid option.\n");
    goto AGAIN;
  }
  if (opt < 0) printf( "==== WARNING: you are in the expert mode ====\n");
  switch (abs(opt)) {
  case 1:
    printf( "LOAD MENU - Please specify full file name: ");
    fgets( file_name, 80, stdin);
    l = strlen( file_name) - 1;
    if (l <= 0) goto AGAIN;
    file_name[l] = '\0';
    if (!open_file( file_name)) {
      printf( "**** ERROR: file '%s' is not found or is invalid.\n", file_name);
      goto AGAIN;
    }
    printf( "---- File '%s' is located, start checking...\n", file_name);
    nb_of_commands = 0;
    while (1) {
      if (!read_file( &block_length)) break;
      if (block_length >= MAX_AMS_COMMAND_BLOCK_LENGTH) {
        printf( "**** FAILURE: file '%s' is invalid - command is tooo long.\n", 
                file_name);
        goto FAILURE;
      }
      nb_of_commands++;
      for (i=0; i<block_length+1; i++) {
        if (!read_file( &AMS_command_block[i+1])) {
          printf( "**** FAILURE: file '%s' is invalid - unexpected EOF.\n", 
                  file_name);
          goto FAILURE;
        }
      }
      if (block_CRC != command_block_CRC()) {
        if (opt > 0) {
          printf( "**** FAILURE: file '%s' is invalid - wrong CRC.\n", 
                  file_name);
          goto FAILURE;
        }
      }
    }
    close_file();
    if (nb_of_commands == 0) {
      printf( "**** FAILURE: file '%s' is invalid - containes no commands.\n", 
              file_name);
      goto FAILURE;
    }
    if (nb_of_commands == 1) printf( "---- File '%s' checked, it containes %d command.\n", 
                             file_name, nb_of_commands);
    else                     printf( "---- File '%s' checked, it containes %d commands.\n", 
                             file_name, nb_of_commands);
    printf( "---- Start loading software from file '%s' to AMS...\n", file_name);
    if (!open_file( file_name)) {
      printf( "**** FAILURE: file '%s' is now invalid.\n", file_name);
      goto FAILURE;
    }
    for (j=0; j<nb_of_commands; j++) {
      if (!read_file( &block_length) || block_length >= MAX_AMS_COMMAND_BLOCK_LENGTH) {
        printf( "**** FAILURE: file '%s' is now invalid - command is tooo long.\n", file_name);
        goto FAILURE;
      }
      for (i=0; i<block_length+1; i++) {
        if (!read_file( &AMS_command_block[i+1])) {
          printf( "**** FAILURE: file '%s' is now invalid - unexpected EOF.\n", file_name);
          goto FAILURE;
        }
      }
      if (block_CRC != command_block_CRC()) {
        if (opt > 0) {
          printf( "**** FAILURE: file '%s' is now invalid - wrong CRC.\n", 
                  file_name);
          goto FAILURE;
        }
        block_CRC = command_block_CRC();
      }
      if (send_AMS_command()) {
        printf( "\r---- %d/%d", j+1, nb_of_commands);
      }
      else {
        printf( "\n**** FAILURE: failed to send the command.\n");
        goto FAILURE;
      }
    }
    close_file();
    printf( "\n---- Finish loading software from file '%s' to AMS.\n", file_name);
    goto AGAIN;
FAILURE:
    close_file();
    printf( "**** FAILURE: software loading aborted.\n");
    goto AGAIN;
  case 2:
    printf( "SPEC MENU - Please select a command: \n");
    for (i=0; i<6; i++) {
      printf( "  %d. %s\n", i+1, command_name[i]);
    }
    printf( "SPEC: ");
    fgets( line, 80, stdin);
    spec = atoi( line);
    if (spec < 1 || spec > 6) {
      printf( "**** ERROR: invalid option.\n");
      goto AGAIN;
    }
    printf( "---- Start sending command '%s' to AMS...\n", command_name[spec-1]);
    if (send_GPC_command( spec-1)) {
      printf( "---- Command '%s' has been sent.\n", command_name[spec-1]);
    }
    else {
      printf( "**** FAILURE: command '%s' was not sent.\n", command_name[spec-1]);
    }
    break;
  case 3:
    printf( "RT MENU - Please select an RT or 1553 Bus:\n");
    printf( "4 - Select Primary RT #4\n");
    printf( "2 - Select Alternate RT #28\n");
    printf( "A - Select 1553 Bus A\n");
    printf( "B - Select 1553 Bus B\n");
    printf( "else exit this menu with no change\n");
    printf( "RT: ");
    fgets( line, 80, stdin);
    RT = atoi( line);
    if (RT == 4 || RT == 2 || RT == 28) {
      if (RT == 2) RT = 28;
      rt_addr = RT;
      printf( "---- RT address changed to %d.\n", RT);
      goto AGAIN;
    }
    if (toupper(line[0]) == 'A' || toupper(line[0]) == 'B') {
      if (toupper(line[0]) == 'A') bus_select = bus_A;
      else                         bus_select = bus_B;
      printf( "---- 1553 BUS changed to %c.\n", toupper(line[0]));
      goto AGAIN;
    }
    printf( "**** ERROR: invalid RT/BUS, no changes made.\n");
    goto AGAIN;
    break;
  case 4:
    printf( "\nThe Bus Controller program terminated.  You may turn on the OIU now.\n\n");
    printf( "                 Bye Franklin & Janet!\n\n");
    exit( 0);
    break;
  default:
    break;
  }
  
  goto AGAIN;
}
