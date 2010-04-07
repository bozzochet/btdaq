// file basili-6.c
//
// This is a workbench for HPTDC control studies.
// Function jtag_step was stolen from CAEN by AB and reworked.
//
// A.Lebedev Mar-2007...

#include "daqlib.h"
#include "sdr2def.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

int8 board;                 // global variable

//~----------------------------------------------------------------------------

int jtag_step(int TMS, int TDI) {

  int16 wri, rea;
  int16 err;
  int16 data_out, data_in, reset, frame, clock;

  wri = ((TDI & 1) << 3) | ((TMS & 1) << 1) | 0;
  data_in  = unpack16(wri, 0x0008);
  reset    = unpack16(wri, 0x0004);
  frame    = unpack16(wri, 0x0002);
  clock    = unpack16(wri, 0x0001);
  if (P) printf("W   %d    %d    %d    %d\n", data_in, reset, frame, clock);
  exec_SDR2_SC_command(WRITE, board, 0x703, wri, NULL, NULL, &err);
  if (err) PANIC("Failed to write with TCK=0.\n");

  wri = ((TDI & 1) << 3) | ((TMS & 1) << 1) | 1;
  data_in  = unpack16(wri, 0x0008);
  reset    = unpack16(wri, 0x0004);
  frame    = unpack16(wri, 0x0002);
  clock    = unpack16(wri, 0x0001);
  if (P) printf("W   %d    %d    %d    %d\n", data_in, reset, frame, clock);
  exec_SDR2_SC_command(WRITE, board, 0x703, wri, NULL, NULL, &err);
  if (err) PANIC("Failed to write with TCK=1.\n");

  exec_SDR2_SC_command(READ,  board, 0x703, 0, &rea, NULL, &err);
  if (err) PANIC("Failed to read.\n");
  data_out = unpack16(rea, 0x0010);
  data_in  = unpack16(rea, 0x0008);
  reset    = unpack16(rea, 0x0004);
  frame    = unpack16(rea, 0x0002);
  clock    = unpack16(rea, 0x0001);
  if (P) printf("R   %d    %d    %d    %d    %d\n", data_in, reset, frame, clock, data_out);

  if (P) printf("\n");
  
  return ((rea >> 4) & 1);
}

//~============================================================================

int main(int argc, char *argv[]) {

  int NC = 1024;
  int NL = 10240;
  int i, nc, nl;
  FILE *file;
  char *filename;
  int16 err;
  int command[NC], parity[NC];
  char note[NC][80];
  struct {
    int  len;
    int  TDI[80];
    int  TDO[80];
    char note[80];
  } line[NL];
  int TMS;
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s <TX&RX> <filename> <board> [<Debug Printout Level>]\n", argv[0]);
  if (argc < 4) {
    printf("Too few arguments.\n");
    exit(1);
  }
  TX = RX = atoi(argv[1]);
  if (TX < 0 || TX > 3) {
    printf("Wrong TX&RX port %d, must be 0..3.\n", TX);
    exit(1);
  }
  filename = argv[2];
  board = atoi(argv[3]);
  if (argc > 4) P  = atoi(argv[4]);
  
  printf("TX&RX    = %d\n", TX);
  printf("filename = %s\n", filename);
  printf("board    = %d\n", board);
  printf("P        = %d\n", P);
  printf("\n");

  file = fopen(filename, "r");
  if (!file) {
    printf("*** File %s was not found.\n", filename);
    exit(1);
  }

  nl = 0;
  for (i=0; i<NL; i++) line[i].len = 0;

  nc = 0;
  for (i=0; i<NC; i++) command[i] = 0;
  for (i=0; i<NC; i++) parity[i]  = 0;
  
  while (1) {
    char b[8192];
    bool cmd;
    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-1] = '\0';

    if (!strncmp(b, "COMMAND:", 8)) cmd = TRUE;
    else                            cmd = FALSE;

    if (cmd) {
      int i, ii = 0;
      int my_parity = 0;
      for (i=8; i<strlen(b); i++) {
        if ((b[i] == ' ') || (b[i] == '\t')) continue;
        if ((b[i] == '0') || (b[i] == '1')) {
          ii++;
          if (ii < 5) {
            command[nc] = (command[nc] << 1) | (b[i] - '0');
            my_parity   =  my_parity         ^ (b[i] - '0');
            continue;
          }
          else {
            parity[nc]  =                      (b[i] - '0');
            continue;
          }
        }
        if (ii == 5) {
          strncpy(&note[nc][0], &b[i], 80);
          printf("nc = %d, ii = %d, command = %X, parity = %X, my_parity = %X, note = %s\n",
                  nc,      ii,      command[nc],  parity[nc],  my_parity,      note[nc]);
          nc++;
          break;
        }
      }
    }
    else {
      int i;
      for (i=0; i<strlen(b); i++) {
        int j;
        if ((b[i] == ' ') || (b[i] == '\t')) continue;
        if ((b[i] == '0') || (b[i] == '1')) {
          line[nl].TDI[line[nl].len] = b[i] - '0';
          line[nl].len++;
          continue;
        }
        strncpy(&line[nl].note[0], &b[i], 80);
        printf("nl = %d, len = %d, TDI =", nl, line[nl].len);
        for (j=0; j<line[nl].len; j++) printf(" %X", line[nl].TDI[j]);
        printf(", note = %s\n", line[nl].note);
        if (line[nl].len) nl++;
        break;
      }
    }
  }
  
  exec_SDR2_SC_command(WRITE, board, 0x702, 1, NULL, NULL, &err);
  if (err) PANIC("Failed to enable.\n");

//~--- Write "command" in IR ---

//~-- initial sequence 0 1 1 0 0

  if (P) printf("  TDI TRST  TMS  TCK   TDO   initial sequence 0 1 1 0 0\n");

  jtag_step(0, 0);  // idle
  jtag_step(1, 0);  // select_dr_scan
  jtag_step(1, 0);  // select_ir_scan
  jtag_step(0, 0);  // capture_ir
  jtag_step(0, 0);  // shift_ir

  if (P) printf("-------------\n\n");

//~--- load command(s)

  for (i=0; i<nc; i++) {
    if (P) printf("  TDI TRST  TMS  TCK   TDO   load command #%d\n", i);

    jtag_step(0, ((command[i] >> 0) & 0x01));
    jtag_step(0, ((command[i] >> 1) & 0x01));
    jtag_step(0, ((command[i] >> 2) & 0x01));
    jtag_step(0, ((command[i] >> 3) & 0x01));
    if (i == nc-1) TMS = 1;
    else           TMS = 0;
    jtag_step(TMS, parity[i]);

    if (P) printf("-------------\n\n");
  }

//~--- final sequence 1 0

  if (P) printf("  TDI TRST  TMS  TCK   TDO   final sequence 1 0\n");

  jtag_step(1, 0);  // update_ir
  jtag_step(0, 0);  // idle

  if (P) printf("-------------\n\n");

//~--- Write "data" in DR:

//~-- initial sequence 0 1 0 0

  if (P) printf("  TDI TRST  TMS  TCK   TDO   initial sequence 0 1 0 0\n");

  jtag_step(0, 0);  // idle
  jtag_step(1, 0);  // select_dr_scan
  jtag_step(0, 0);  // capture_dr
  jtag_step(0, 0);  // shift_dr

  if (P) printf("-------------\n\n");

//~--- read TDO and write TDI

  if (P) printf("  TDI TRST  TMS  TCK   TDO   read TDO and write TDI:\n");

  for (i=0; i<nl; i++) {
    int j;
    if (P) printf("  TDI TRST  TMS  TCK   TDO   %s\n", line[i].note);
    for (j=0; j<line[i].len; j++) {
      int k = line[i].len - 1 - j;
      if ((i == nl-1) && (j == line[i].len-1)) TMS = 1;
      else                                     TMS = 0;
      line[i].TDO[k] = jtag_step(TMS, line[i].TDI[k]);
    }
    if (P) printf("-------------\n\n");
    for (j=0; j<line[i].len; j++) printf("%X", line[i].TDI[j]);
    printf("  ");
    for (j=0; j<line[i].len; j++) printf("%X", line[i].TDO[j]);
    printf("  %s\n", line[i].note);
  }

//~--- final sequence 1 0

  if (P) printf("  TDI TRST  TMS  TCK   TDO   final sequence 1 0\n");

  jtag_step(1, 0);  // update_dr
  jtag_step(0, 0);  // idle

  exec_SDR2_SC_command(WRITE, board, 0x702, 0, NULL, NULL, &err);
  if (err) PANIC("Failed to enable.\n");

  if (P) printf("-------------\n\n");

  return 0;
}

//~============================================================================
