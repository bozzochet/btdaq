// file m-crate-1.c
//
// Waclaw complained on DAC/ADC mismatch
// this is a program to study it.
//
//
// A.Lebedev - Mar-2008...

#include "uscmlib.h"

int P = 0;                                     // global variable
int USCM_ID = 0x2D;                            // global variable
int JMDC_ID = 0x00;                            // global variable

int BUS_FOR_REQUEST = 0;                       // global variable
int BUS_FOR_REPLY = 0;                         // global variable

//~----------------------------------------------------------------------------

void write_DAC_read_DAC_ADC(int ind, int32 dac_value) {

  int i;
  int16 w_dac[16], r_dac[16], r_adc[32];
  float adc_value, diff;
  int16 err = 0x0000;

  for (i=0; i<16; i++) w_dac[i] = 0x8000;

  w_dac[ind] = (int32)((float)dac_value / 0.05 + 0.5) & 0x0FFF;
  write_DAC(w_dac, &err);
  if (err) EXIT("write_DAC");

  read_DAC(r_dac, &err);
  if (err) EXIT("read_DAC");
  if (r_dac[ind] != w_dac[ind]) {
    printf("w_dac[%d] = %04X, r_dac[%d] = %04X\n", ind, w_dac[ind], ind, r_dac[ind]);
  }

  read_ADC(r_adc, &err);
  if (err) EXIT("read_ADC");
  adc_value = 0.110 * (float)r_adc[ind] + 0.5;

  diff = (float)dac_value - adc_value;
  if (ABS(diff) >= 1.0) {
    printf("ind = %2d, (float)dac_value = %f, adc_value = %f, diff = %f\n",
            ind,       (float)dac_value,      adc_value,      diff);
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

  int ind;
  int32 dac_value;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<P>]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID     = 0x%02X\n", USCM_ID);
  printf("JMDC ID     = %d\n", JMDC_ID);
  printf("P           = %d\n", P);

  while (1) {
    for (ind=1; ind<2; ind++) {
      for (dac_value=0; dac_value<151; dac_value+=10) {
        write_DAC_read_DAC_ADC(ind, 0);
        write_DAC_read_DAC_ADC(ind, dac_value);
      }
    }
  }

  return 0;
}
