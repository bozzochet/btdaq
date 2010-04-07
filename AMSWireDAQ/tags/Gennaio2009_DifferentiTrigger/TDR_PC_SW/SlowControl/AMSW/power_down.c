// file power_down.c
//
// Program to test "Power Down" and the "Boot". AB said there are pbms.
//
// A.Lebedev May-2008... 
//

#include "jlv1def.h"
#include "daqlib.h"

int TX = 1;                 // global variable
int RX = 1;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int i, k;
  int nev = 0;
  int Delay_1 = 0;
  int Delay_2 = 0;
  int16 err = 0;

//~---

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  set_AMSW_timeout(1.0);
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: \n");
  printf("  %s <TX&RX> {<Path>|0} [<Delay_1, us> [<Delay_2, us> [P]]]\n", argv[0]);

  k = 1;
  if (argc > k) TX = RX      = atoi(argv[k++]);
  if (argc > k) AMSW_PATH[0] = strtoul(argv[k++], NULL, 16);
  if (argc > k) Delay_1      = atoi(argv[k++]);
  if (argc > k) Delay_2      = atoi(argv[k++]);
  if (argc > k) P            = atoi(argv[k++]);
  printf("TX      = %d\n", TX);
  printf("RX      = %d\n", RX);
  printf("Path    = 0x%04X\n", AMSW_PATH[0]);
  printf("Delay_1 = %d us\n", Delay_1);
  printf("Delay_2 = %d us\n", Delay_2);
  printf("P     = %d\n", P);

  while (1) {
    set_JINF_power_down(&err);
    if (err) EXIT("set_JINF_power_down failed");
    delay((float)Delay_1/1000000.0);
//  get_JINF_status(NULL, NULL, NULL, NULL, NULL, &err);
//  if (err) EXIT("get_JINF_status failed (as it should be)");
    boot_JINF(0x0000, &err);
    if (err) EXIT("boot_JINF failed");
    nev = nev + 1;
    if (!(nev % 100)) printf("\r             %d\r", nev);
  }

  return 0;
}

//~============================================================================
