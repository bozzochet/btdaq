// file load_all_jlv1_registers.c
//
// reads "configuration" file (format is as JLV1-2-dat.configfile)
// and writes all registers of JLV1
//
// A.Lebedev Oct-2006...

#include "jlv1def.h"
#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

//~----------------------------------------------------------------------------

int16 err;

char *filename;

int n_reg = 64;
_w w_boot_adr;
_w w_mask;
_w w_period;
_w w_epp_trig_msk;
_w w_proc_mode;
_w w_PPS_msk;
_ww ww_reg;
_ww w_cfg_file;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void invalidate_data(void);
void read_config_file(char *name);
void write_config_file(char *name);
void write_all_registers(void);

//~============================================================================

void invalidate_data(void) {

  static bool first = TRUE;

//~-- "all registers" initialization must be the first

  invalidate_ww(first, &ww_reg, n_reg, 0x0000);

//~--

  invalidate_w(&w_boot_adr, 1, 0x0000);
  invalidate_w(&w_epp_trig_msk, 1, 0x0000);
  invalidate_w(&w_proc_mode, 1, 0x0000);
  w_cfg_file.unk = TRUE;
  if (first) w_cfg_file.inp = malloc(4);
  if (first) w_cfg_file.wri = malloc(4);
  w_cfg_file.inp[0] = 0;

  read_config_file(filename);
  first = FALSE;
}

//~----------------------------------------------------------------------------

void read_config_file(char *name) {

  FILE *file;
  
  file = fopen(name, "r");
  if (file != NULL) {
    fread(&w_boot_adr.inp,     4,     1, file);
    fread(&w_proc_mode.inp,    4,     1, file);
    fread(&w_epp_trig_msk.inp, 4,     1, file);
    fread(ww_reg.inp,          4, n_reg, file);
    fread(w_cfg_file.inp,      4,     1, file);
    fclose(file);
  }
  else {
    printf("File %s is absent\n", name);
    exit(1);
  }
}

//~----------------------------------------------------------------------------

void write_config_file(char *name) {

  FILE *file;

  file = fopen(name, "w");
  if (file != NULL) {
    fwrite(&w_boot_adr.inp,     4,     1, file);
    fwrite(&w_proc_mode.inp,    4,     1, file);
    fwrite(&w_epp_trig_msk.inp, 4,     1, file);
    fwrite(ww_reg.inp,          4, n_reg, file);
    fwrite(w_cfg_file.inp,      4,     1, file);
    fclose(file);
  }
}

//~-------------------------------------------------------------------------------

void write_all_registers(void) {

  _ww *w = &ww_reg;
  int i;
  int16 request[1000];
  int16 reply[1000];
  
  for (i=0; i<n_reg; i++) w->inp[i] &= 0x0000FFFF;

  begin_IO_segment(request);
  for (i=0; i<n_reg; i++) add_IO_write(request, i, w->inp[i]);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);
  
  if (err) {
    printf("Register writing failed, err = 0x%04X\n", err);
    exit(1);
  }
}

//~-------------------------------------------------------------------------------

void create_main_form(void) {
}

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s <TX&RX> {<Path> | 0} <filename> [<Debug Printout Level>]\n",
          argv[0]);
  if (argc < 4) {
    printf("Too few parameters\n");
    exit(1);
  }
  TX = RX         = atoi(   argv[1]);
  AMSW_PATH[0]    = strtoul(argv[2], NULL, 16);
  filename        =         argv[3];
  if (argc > 4) P = atoi(   argv[4]);
  printf("TX    = %d\n", TX);
  printf("RX    = %d\n", RX);
  printf("Path  = 0x%04X\n", AMSW_PATH[0]);
  printf("File  = %s\n", filename);
  printf("P     = %d\n", P);

  set_AMSW_timeout(1.0);

  invalidate_data();
//write_config_file("check.file");
  write_all_registers();
  
  printf("All done.\n");
  
  return 0;
}

//~============================================================================
