// file sfet2-write.c
//
// Writes Thresholds to S-Crates following config file
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

char *filename = {"Q"};
FILE *file = NULL;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void user_interface(int argc, char *argv[]);
static void write_threshold(int crt, int brd, int grp, int thr, int16 cod, int16 *err);
static bool write_SFET_SFEA_thresholds(FILE *file, bool ignore_errors);

//~----------------------------------------------------------------------------

static void user_interface(int argc, char *argv[]) {

  int k;
  FILE *out = stderr;

  setbuf(stdout, NULL);
  setup_command_path();
  use_AMSW = FALSE;

  fprintf(out, "Usage: \n");
  fprintf(out, "  %s filename [P]\n", argv[0]);
  fprintf(out, "  %s 1553 Server filename [P]\n", argv[0]);
  fprintf(out, "  %s HRDL Server filename [P]\n", argv[0]);

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

  if (argc <= k) PANIC("Too few arguments.\n");
  filename        = argv[k++];
  if (argc > k) P = atoi(argv[k++]);
  fprintf(out, "filename = %s\n", filename);
  fprintf(out, "P        = %d\n", P);
}

//~----------------------------------------------------------------------------

static void write_threshold(int crt, int brd, int grp, int thr, int16 cod, int16 *err) {

  int i, j;
  int adr;
  int cha = grp;
  int dac = thr;
  int16 wri = (cod & 0x00FF) | (cha * 0x0400) | 0x0300;
  
  JINF_PATH = scrate[crt].lnk << 8 | 0x003F;
  NODE_ADR  = scrate[crt].adr;

  for (i=0, j=0; i<8; i++) {
    adr = sboard[i].adr;
    if (sboard[i].typ == 1) {
      if (j++ == brd) break;
    }
  }

  *err = 0x0000;

  exec_SDR2_SC_command(WRITE, adr, dac_adr[dac][cha], wri, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

static bool write_SFET_SFEA_thresholds(FILE *file, bool ignore_errors) {

  int i, j;
  bool ok = TRUE;
  int16 err = 0x0000;
  float V_ref = V_ref_int;

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
        
  for (j=0; j<n; j++) {
    par[j].found = FALSE;
    par[j].use   = par[j].n;
    par[j].ind   = par[j].n;
  }
  
  while (1) {
    int16 cod = 0x0000;
    int16 val;
    bool cod_found = FALSE;
    char b[8192];
    int k;

    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-1] = '\0';

    if (P) printf("Input  = %s\n", b);

    for (k=0; k<n+1; k++) {
      char *q[1];
      char *token;
      if (k) token = strtok(NULL, "\n ");
      else   token = strtok(b,    "\n ");
      if (!token) break;
      if (P) printf("   token = %-10s ", token);
    
      if (!strcmp(token, "//")) break;
      for (j=0; j<n; j++) {
        for (i=0; i<par[j].n; i++) {
          if (!strcmp(token, par[j].nam[i])) {
            par[j].ind   = i;
            par[j].found = TRUE;
            break;
          }
        }
      }
      if (!cod_found) {
        cod = strtoul(token, q, 10);
        if (!**q) cod_found = TRUE;
      }
      if (P) printf("cod=%4d, **q=%08X\n", cod, **q);
    }
    if (P > 1) printf("\n");
    
    if (!cod_found) {
      for (j=0; j<n; j++) if (par[j].found) par[j].use = par[j].ind;
      continue;
    }

    for (j=0; j<n; j++) {
      if (!par[j].found) {
        if (!(par[j].use < par[j].n)) continue;
        par[j].ind = par[j].use;
      }
    }
    val = 2.0 * (float)cod / 256.0 * V_ref;
    if (P > 1) printf("Output = ");
    for (j=0; j<n; j++) printf(par[j].fmt, par[j].nam[par[j].ind]);
    printf("%3d %4d\n", cod, val);
    if (P > 1) printf("\n");

//  write_threshold(par[0].ind, par[1].ind, par[2].ind, par[3].ind, cod, &err);
    if (err) {
      ok = FALSE;
      printf("*** Error: write_SFET_SFEA_thresholds err = %04X\n", err);
      if (!ignore_errors) break;
    }
  }

  return ok;
}

//~============================================================================

int main(int argc, char *argv[]) {

  user_interface(argc, argv);

  set_AMSW_timeout(0.9);
  
  if (!file) file = fopen(filename, "r");
  if (!file) {
    printf("File %s absent.\n", filename);
    exit(1);
  }
  
  if (!write_SFET_SFEA_thresholds(file, TRUE)) exit(1);

  return 0;
}

//~============================================================================
