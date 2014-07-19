// file pds31.c
//
// Simple commanding program for PDS
//
// A.Lebedev Jun-2005...

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x131;      // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

void print_usage(char *name) {

  printf(
  "Usage: %s R {A|B} <board address> <channel address> [<ADC address>]\n",
   name);
  printf(
  "   or: %s W {A|B} <board address> <channel address> {0|1}\n", name);

  exit(1);
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int8 r_w, a_b, adr, cha, dat;
  bool show_adc;
  int16 cmd;
  int8 adc_cha;
  int8 dig;
  int16 adc_dat;
  int16 err;

  setbuf(stdout, NULL);
  setup_command_path();
  
  if (argc < 5)                                             print_usage(argv[0]);
  if (strcasecmp(argv[1], "R") && strcasecmp(argv[1], "W")) print_usage(argv[0]);
  if (strcasecmp(argv[2], "A") && strcasecmp(argv[2], "B")) print_usage(argv[0]);
  if (!strcasecmp(argv[1], "R")) {
    if (argc > 6)                                           print_usage(argv[0]);
    r_w = 0;
    dat = 0;
    if (argc == 5) {
      show_adc = FALSE;
      adc_cha = 1;
    }
    else {
      show_adc = TRUE;
      adc_cha = strtoul(argv[5], NULL, 16);
    }
  }
  else {
    r_w = 1;
    adc_cha = 1;
    show_adc = FALSE;
    if (argc != 6)                                          print_usage(argv[0]);
    dat = atoi(argv[5]);
    if ((dat != 0) && (dat != 1))                           print_usage(argv[0]);
  }

  a_b = strcasecmp(argv[2], "A") ? 1 : 0;
  adr = strtoul(argv[3], NULL, 16);
  cha = strtoul(argv[4], NULL, 16);
      
  if (!initialize_EPP()) exit(1);

  enable_CGS_serial_lines(&err);
  if (err) {
    printf("enable_CGS_serial_lines failed, err=0x%04X\n", err);
    exit(1);
  }
  
  cmd = 0;
  pack16(&cmd, 0x0800, dat);
  pack16(&cmd, 0x0400, r_w);
  pack16(&cmd, 0x0200, a_b);
  pack16(&cmd, 0x01E0, adr);
  pack16(&cmd, 0x001F, cha);

  exec_PDS_cmd(cmd, adc_cha, &dig, &adc_dat, &err);
  if (err) {
    printf("exec_PDS_cmd failed, err=0x%04X\n", err);
    exit(1);
  }

  disable_CGS_serial_lines(&err);
  if (err) {
    printf("disable_CGS_serial_lines failed, err=0x%04X\n", err);
    exit(1);
  }

  if (r_w) {
    printf("W: adr=0x%02X cha=0x%02X dig=%d\n", adr, cha, dat);
  }
  else {
    if (show_adc) {
      printf("R: adr=0x%02X cha=0x%02X adc=0x%02X --> adc_dat=0x%04X\n",
      adr, cha, adc_cha, adc_dat);
    }
    else {
      printf("R: adr=0x%02X cha=0x%02X --> dig_dat=%d\n",
      adr, cha, dig);
    }
  }
  
  return 0;
}

//~============================================================================
