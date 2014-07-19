// file sfet2-read.c
//
// Reads Thresholds from all S-Crates
//
// A.Lebedev May-2008...

#include "sfet2def.h"
#include "jinflib.h"

int TX = 1;                 // global variable
int RX = 1;                 // global variable
int P = 0;                  // global variable

int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

char CS_address[100] = {"127.0.0.1"};  // global variable
int  CS_port;                          // global variable
int16 NODE_ADR;                        // global variable

extern _scrate scrate[8];
extern float V_ref_int;
extern float V_ref_ext;
extern _b thr_msk;
extern _b ref_msk;

int fmt = 3;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void user_interface(int argc, char *argv[]);
static void read_threshold(int crt, int brd, int grp, int thr, int16 *cod, int16 *err);

//~----------------------------------------------------------------------------

static void user_interface(int argc, char *argv[]) {

  int k;
  FILE *out = stderr;

  setbuf(stdout, NULL);
  setup_command_path();
  use_AMSW = FALSE;

  fprintf(out, "Usage: \n");
  fprintf(out, "  %s [format [P]]\n", argv[0]);
  fprintf(out, "  %s 1553 Server [format [P]]\n", argv[0]);
  fprintf(out, "  %s HRDL Server [format [P]]\n", argv[0]);

  if (argc > 1 && !strcasecmp(argv[1], "1553")) {
    k = 2;
    if (argc <= k) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[k++], 80);
    CS_port = 61002;
    use_APID      = 0x0018;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    fprintf(out, "Server = %s\n",   CS_address);
    fprintf(out, "Port   = %d\n",   CS_port);
    fprintf(out, "Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
  }
  else if (argc > 1 && !strcasecmp(argv[1], "HRDL")) {
    k = 2;
    if (argc <= k) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[k++], 80);
    CS_port = 61001;
    use_APID      = 0x03D6;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    fprintf(out, "Server = %s\n",   CS_address);
    fprintf(out, "Port   = %d\n",   CS_port);
    fprintf(out, "Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
  }
  else {
    k = 1;
    use_AMSW = TRUE;
    fprintf(out, "Direct connection via EPP-CAN box will be used\n");
  }

  if (argc > k) fmt = atoi(argv[k++]);
  if (argc > k) P   = atoi(argv[k++]);
  fprintf(out, "format = %d\n", fmt);
  fprintf(out, "P      = %d\n", P);
}

//~----------------------------------------------------------------------------

static void read_threshold(int crt, int brd, int grp, int thr, int16 *cod, int16 *err) {

  int i, j;
  int adr;
  int cha = grp;
  int dac = thr;
  
  JINF_PATH = scrate[crt].lnk << 8 | 0x003F;
  NODE_ADR  = scrate[crt].adr;

  for (i=0, j=0; i<8; i++) {
    adr = sboard[i].adr;
    if (sboard[i].typ == 1) {
      if (j++ == brd) break;
    }
  }
  
  *err = 0x0000;

  exec_SDR2_SC_command(READ, adr, dac_adr[dac][cha], 0, cod, NULL, err);
//printf("crt=%d adr=%d dac=%d cha=%d cod=%04X     ", crt, adr, dac, cha, *cod);
}

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err;
  int crt, brd, grp, thr;
  int16 cod, val;
  float V_ref;
  
  struct {
    int   n;
    char *fmt;
    char *nam[16];
    int   use;
    bool  found;
    int   ind;
  } par[4] = {
    {8, "%3s ",  {"S0A", "S0B", "S1A", "S1B", "S2A", "S2B", "S3A", "S3B"}},
    {5, "%-7s ", {"SFET2-A", "SFET2-B", "SFET2-C", "SFET2-D", "SFEA2"}},
    {2, "%5s ",  {"GRP-0", "GRP-1"}},
    {3, "%-3s ", {"LT", "HT", "SHT"}}};
  int n = sizeof(par) / sizeof(par[0]);
        
  user_interface(argc, argv);

  set_AMSW_timeout(0.9);
  
  for (crt=0; crt<8; crt+=2) {
    for (brd=0; brd<5; brd++) {
      for (grp=0; grp<2; grp++) {
        for (thr=0; thr<3; thr++) {
          read_threshold(crt, brd, grp, thr, &cod, &err);
          V_ref = (cod & ref_msk.msk) ? V_ref_ext : V_ref_int;
          cod = cod & 0xFF;
          val = 2.0 * (float)cod / 256.0 * V_ref;
          switch (fmt) {
            case 1:
              printf("%2d %d %d %d %3d %4d\n", crt, brd, grp, thr, cod, val);
            break;
            case 2:
//            printf("%s %-7s %d %3d %4d\n", scrate[crt].nam, brd_nam[brd], thr, cod, val);
            break;
            case 3:
              printf("%s %-7s %s %-3s ", 
                     par[0].nam[crt], par[1].nam[brd], par[2].nam[grp], par[3].nam[thr]);
              if (!err) printf("%3d %4d\n", cod, val);
              else      printf(" *** Error err = 0x%04X\n", err);
            break;
          }
        }
      }
    }
  }
  return 0;
}

//~============================================================================
