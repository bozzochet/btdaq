// file cab-ptm-study.c
//
//  Test bench to understand how PTM works and should be used.
//
//  A.Lebedev Jan - 2009...
//

#include "cabdef.h"

int JMDC_ID = 0x03;                                // global variable
int USCM_ID = 0x18;                                // global variable
int P = 0;                                         // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

void study_ptm_timing() {

  bool debug = TRUE;
  int i;
  int16 err;
  int8 dat[256];
  static struct timeval now, then;

  PTM_calibration(0, &err);
  if (err) {
    if (err > 0x2000) printf("PTM_calibration: %s\n", cab_ack_message(err-0x2000));
    else              printf("err = %04X\n", err);
    exit(1);
  }

  while (1) {

    PTM_start_acq_seq(0, &err);
    if (err) {
      if (err > 0x2000) printf("PTM_start_acq_seq: %s\n", cab_ack_message(err-0x2000));
      else              printf("err = %04X\n", err);
      exit(1);
    }
    gettimeofday(&then, NULL);

    while (1) {
      int8 sta[56];
      CAB_get_essential_status(sta, &err);
      if (err) {
        if (err > 0x2000) printf("CAB_get_essential_status: %s\n", cab_ack_message(err-0x2000));
        else              printf("err = %04X\n", err);
        exit(1);
      }
      if (sta[4] & 0x01) {
        if (debug) printf("X");
      }
      else {
        break;
      }
    }
    gettimeofday(&now, NULL);
    if (debug) printf(": %f sec.  ", delta_t(&now, &then));

    PTM_read(0, dat, &err);
    if (err) {
      if (err > 0x2000) printf("PTM_read: %s\n", cab_ack_message(err-0x2000));
      else              printf("err = %04X\n", err);
      exit(1);
    }
//  for (i=0; i<20; i++) printf("%02hhX ", dat[i]);
//  printf("\n");
    for (i=0; i<12; i=i+2) printf("%5d ", ((int16)dat[4+i]<<8)+(int16)dat[5+i]);
    printf("   ");
    for (i=0; i<12; i=i+2) printf("%5d ", ((int16)dat[50+i]<<8)+(int16)dat[51+i]);
    printf("\n");
  }
}

//~----------------------------------------------------------------------------

/*
float convert_PTM_data(                 // ****** was moved to cablib ******
  _sta *p, int Dm, real Doffset_m, real D_Vref1, real D_Vref2, real D_Vref3) {

//~---

  float Rt;
  real Vref;
  real K_RANGE[] = {
    6.5196903786e-3,  // Superfluid Cooling Loop (100K)
    3.2533124334e-3,  // Persistent Switch       (200K)
    2.3256610970e-3,  // Cool Down Circuit & VCS (300K)
    1.9206302310e-3}; // Magnet Current Leads    (400K)
  real K1  = 107.4113856069;
  real K2  = 100.0;
  real K3  = 1.2462061251e-2;
  real KK1 = 53.7056928034;
  real KK2 = 50.0;
  real KK3 = 6.7985197536e-3;
  
//~---

  switch (p->F) {

    case 0: 
    break;

    case 1:
      Rt = (2.0 * D_Vref2 / 65536.0 - 1.0) * K_RANGE[p->K]
         / (2.0 * D_Vref1 / 65536.0 - 1.0) 
         / (((2.0 * (Dm - Doffset_m + 13002.1322645) / 65536.0) - 1.0)
         / 30100.0 + 1.0 / 49900.0);
    return Rt;

    case 2:
      Rt = (2.0 * D_Vref1 / 65536.0 - 1.0) / (2.0 * D_Vref2 / 65536.0 - 1.0) 
         * (K1  * (2.0 * (Dm - Doffset_m + 2260.992) / 65536.0 - 1.0) + K2 )
         / K3;
    return Rt;

    case 3:
      Rt = (2.0 * D_Vref1 / 65536.0 - 1.0) / (2.0 * D_Vref3 / 65536.0 - 1.0) 
         * (K1  * (2.0 * (Dm - Doffset_m + 2260.992) / 65536.0 - 1.0) + K2 )
         / K3;
    return Rt;

    case 4:
      Rt = (2.0 * D_Vref1 / 65536.0 - 1.0) / (2.0 * D_Vref3 / 65536.0 - 1.0) 
         * (KK1 * (2.0 * (Dm - Doffset_m + 2260.992) / 65536.0 - 1.0) + KK2)
         / KK3;
    return Rt;

    case 5:
      Vref = Dm * 10.0 / 65536.0 - 5.0;
    return Vref;
  }

  return 0xDEADBEAF;
}
*/


//~----------------------------------------------------------------------------

void study_conversion_formulae(void) {

  int i;
  int Dm, ind;
  real Rt, Doffset_m, D_Vref1, D_Vref2, D_Vref3, Rt_ref;
  struct {
    int ind;
    int Dm;
    real Doffset_m;
    real D_Vref1;
    real D_Vref2;
    real D_Vref3;
    real Rt_ref;
  } ref_data[] = {
    { 1, 13375, 13008.0, 62560.0, 62553.0,     0.0, 17517.62733},
    {22, 55660, 12962.0, 62560.0, 62553.0,     0.0,    75.13333},
    {19, 14097, 13007.0, 62560.0, 62553.0,     0.0,  2103.94463},
    {16, 13196, 13006.0, 62560.0, 62553.0,     0.0,  9967.91981},
    { 9, 27526,  2276.0, 62560.0, 62553.0,     0.0,  6643.14818},
    { 7,  9525,  2253.0, 62560.0,     0.0, 62564.0,  1912.52036},
    { 8, 35883,  2261.0, 62560.0,     0.0, 62564.0,  8104.40736},
    {23,  2979,     0.0,     0.0,     0.0,     0.0,    -4.545454},
    {46, 62557,     0.0,     0.0,     0.0,     0.0,     4.545454}};
  
  for (i=0; i<DIM(ref_data); i++) {
    Dm        = ref_data[i].Dm;
    ind       = ref_data[i].ind;
    Doffset_m = ref_data[i].Doffset_m;
    D_Vref1   = ref_data[i].D_Vref1;
    D_Vref2   = ref_data[i].D_Vref2;
    D_Vref3   = ref_data[i].D_Vref3;
    Rt_ref    = ref_data[i].Rt_ref;
    Rt = convert_PTM_data(&ptm_cha[ind], Dm, Doffset_m, D_Vref1, D_Vref2, D_Vref3);
    printf("Rt     = %Lf\nRt_ref = %Lf\n", Rt, Rt_ref);
  }
}

//~============================================================================

int main(int argc, char *argv[]) {
  
  setbuf(stdout, NULL);

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  setup_command_path_special();

  if (!initialize_EPP()) exit(1);

//~---

//study_ptm_timing();
  study_conversion_formulae();
  
  return 0;
}

//~============================================================================
