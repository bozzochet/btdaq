// file convert_jlv1_registers_configuration_file.c
//
// Reads old (Oct 2006) "configuration" file for JLV1-2 program
// and writes new (Jul 2007) "configuration" file for JLV1-2 program.
//
// See files load_all_jlv1_registers.c-old and load_all_jlv1_registers.c
// for more details.
//
// A.Lebedev Jul-2007...

#include "jlv1def.h"
#include "template.h"

//~----------------------------------------------------------------------------

int16 err;

char *old_file_name, *new_file_name;

int n_reg = 64;
_w w_boot_adr;
_w w_mask;
_w w_period;
_w w_trig_msk;
#define w_epp_trig_msk w_trig_msk
_w w_proc_mode;
_w w_PPS_msk;
_ww ww_reg;
_ww w_cfg_file;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void invalidate_data(void);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~============================================================================

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  w_boot_adr.inp = 0x0;
  w_boot_adr.unk = TRUE;

  w_mask.inp = 0x0;
  w_mask.unk = TRUE;
  
  w_period.inp = 0x0000;
  w_period.unk = TRUE;
  
  w_trig_msk.inp = 0x0;
  w_trig_msk.unk = TRUE;
  
  w_proc_mode.inp = 0x0000;
  w_proc_mode.unk = TRUE;
  
  ww_reg.unk = TRUE;
  if (first) ww_reg.inp = malloc(64*4);
  if (first) ww_reg.wri = malloc(64*4);
  for (i=0; i<64; i++) ww_reg.inp[i] = 0x0000;

  w_cfg_file.unk = TRUE;
  if (first) w_cfg_file.inp = malloc(4);
  if (first) w_cfg_file.wri = malloc(4);
  w_cfg_file.inp[0] = 0;

  read_dat_config_file(old_file_name);
  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  
  file = fopen(filename, "r");
  if (file != NULL) {
    fread(&w_mask.inp,      sizeof(&w_mask.inp),       1, file);
    fread(&w_period.inp,    sizeof(&w_period.inp),     1, file);
    fread(&w_boot_adr.inp,  sizeof(&w_boot_adr.inp),   1, file);
    fread(&w_proc_mode.inp, sizeof(&w_proc_mode.inp),  1, file);
    fread(&w_trig_msk.inp,  sizeof(&w_trig_msk.inp),   1, file);
    fread(ww_reg.inp, 4, 64, file);
    fread(w_cfg_file.inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  else {
    printf("File %s is absent\n", filename);
    exit(1);
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(&w_boot_adr.inp,     4,     1, file);
    fwrite(&w_proc_mode.inp,    4,     1, file);
    fwrite(&w_epp_trig_msk.inp, 4,     1, file);
    fwrite(ww_reg.inp,          4, n_reg, file);
    fwrite(w_cfg_file.inp,      4,     1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream

  printf("Usage: %s <old config filename> <new_config_file_name\n",
          argv[0]);
  if (argc < 2) {
    printf("Too few parameters\n");
    exit(1);
  }
  old_file_name = argv[1];
  new_file_name = argv[2];
  printf("Old Config File Name = %s\n", old_file_name);
  printf("New Config File Name = %s\n", new_file_name);

  invalidate_data();
  write_dat_config_file(new_file_name);

  printf("All done.\n");
  
  return 0;
}

//~============================================================================
