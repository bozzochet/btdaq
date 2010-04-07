// file rns.c
//
// Get JINF Node Status ("read node status")
//
// A.Lebedev Dec-2005...

#include "daqlib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable

int16 dat[10];
int n;
int16 datdat[24][10];
int16 sta[24];

//~----------------------------------------------------------------------------

  int N = 14;

  struct {
    char *ta;
    char *tb;
    int   i;
    int16 m;
    char *fmt;
    char *txt[16];
  } *q, Q[14] = {
  
  {"      ",   "Vers  ",  1, 0xFFFF, "%04X  ",{NULL}},
  {"      ",   "Pgm   ",  2, 0xF000, "%3s   ",{"ROM", "DAQ"}},
  {"      ",   "Node  ",  2, 0x0F00, "%4s  ", {"?",   "CDP", "CDDC"}},
  {"Node  ",   "Type  ",  2, 0x00F0, "%4s  ", {"CDDC","TDR", "UDR", "RDR", "EDR", "SDR", "LV1"}},
  {"Ports ",   "to M  ",  2, 0x000F, "%3d   ",{NULL}},
  {"SubD  ",   "Vers  ",  3, 0xFFFF, "%04X  ",{NULL}},
  {"Errors  ", "E",       6, 0x4000, "%d",    {NULL}},
  { NULL,      "A",       6, 0x2000, "%d",    {NULL}},
  { NULL,      "F",       6, 0x1000, "%d",    {NULL}},
  { NULL,      "B",       6, 0x0800, "%d",    {NULL}},
  { NULL,      "P",       6, 0x0400, "%d",    {NULL}},
  { NULL,      "D",       6, 0x0200, "%d",    {NULL}},
  { NULL,      "T",       6, 0x0100, "%d",    {NULL}},
  {"   Time",  "     sec",4,      0, "%8d",   {NULL}}};


void print_Node_Status(int lnk, int16 *status) {

  int i;
  int32 sta[14];
    
  if (status) {
    if (lnk >= 0) printf("%2d: ", lnk);
    for (i=0; i<N; i++) {
      q = &Q[i];
      if (q->m) sta[i] = unpack32((int32)status[(q->i)-1], (int32)q->m);
      else      sta[i] = ((int32)status[q->i] << 16 | (int32)status[q->i-1]) / 100;
      if (q->txt[0]) printf(q->fmt, q->txt[sta[i]]);
      else           printf(q->fmt,        sta[i]);
    }
    printf("\n");
  }
  else {
    if (lnk >= 0) printf("    ");
    for (i=0; i<N; i++) if (Q[i].ta) printf("%s", Q[i].ta);
    printf("\n");
    if (lnk >= 0) printf("Lnk ");
    for (i=0; i<N; i++) if (Q[i].tb) printf("%s", Q[i].tb);
    printf("\n");
  }
}

//~----------------------------------------------------------------------------

void print_Node_Status_1(void) {

  int i, j;
  
  for (j=0; j<n; j++) {
    printf("sta = 0x%04X: ", sta[j]);
    for (i=0; i<10; i++) printf("0x%04X ", datdat[j][i]);
    printf("\n");
  }
}

//~----------------------------------------------------------------------------

void print_Node_Status_0(void) {

  int i;
  
  for (i=0; i<10; i++) printf("0x%04X ", dat[i]);
  printf("\n");
}

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int fmt;
  int16 err;
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s [<TX&RX> [<Path>|0 [<Debug Printout Level>]]]\n", argv[0]);
  if (argc > 1) TX           = RX = atoi(argv[1]);
  if (argc > 2) AMSW_PATH[0] = strtoul(argv[2], NULL, 16);
  if (argc > 3) P            = atoi(argv[3]);
  printf("TX    = %d\n", TX);
  printf("RX    = %d\n", RX);
  printf("Path  = 0x%04X\n", AMSW_PATH[0]);
  printf("P     = %d\n", P);

  get_JINF_status(&fmt, &dat[0], &n, &datdat[0][0], sta, &err);

  if (err) {
    printf("*** Error 0x%04X\n", err);
    exit(1);
  }

  if (!fmt) print_Node_Status_0();
  else      print_Node_Status_1();

  printf("\n");
  if (!fmt) {
    print_Node_Status(-1, NULL);
    print_Node_Status(-1, dat);
  }
  else {
    print_Node_Status(0, NULL);
    for (i=0; i<n; i++) print_Node_Status(sta[i] & 0x001F, &datdat[i][0]);
  }

  return 0;
}

//~============================================================================
