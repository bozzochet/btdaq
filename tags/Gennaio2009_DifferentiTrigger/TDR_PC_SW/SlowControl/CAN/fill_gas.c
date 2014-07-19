// file fill_gas.c
//
// to replace can_pc scripts for gas mixing
//
// A.Lebedev - Mar-2005...

#include "ugbslib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int8 BUS  = 0<<4 | 0;                              // global variable
int  BRD  = 1;                                     // global variable
int  PP   = 1;                                     // global variable

int P = 0;                                         // global variable
int USCM_ID = 0x156;                               // global variable
int JMDC_ID = 0x00;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

void wait(int16 time) {

  int i;
  
  if (PP > 1) printf("   Waiting %2ds: ", time);

  for (i=0; i<time; i++) {
    sleep(1);
    if (PP > 1) printf("%d", (i+1)%10);
  }
  
  if (PP > 1) printf(" Done.\n");
}

//~----------------------------------------------------------------------------

void dallas(void) {

  int16 err;
  int8 bus;
  int8 n1[9];
  int8 n2;
  bool ok[64];
  float temp[64];
  int64 ID[64];
  int8 pri[64];
  int i;
  
  if (PP) printf("Started dallas procedure.\n");

  setup_DS1820_readout(0x00, 0x00, 0x00, 0x00, 1, &err);
  start_DS1820_scan(0xFF, &err);

  if (PP > 1) printf("   Scanning Dallas sensors:                   ...       ");

  wait(5);
  check_DS1820_scan(&bus, n1, &err);
  if (PP > 1) {
    printf("Found:");
    for (i=0; i<9; i++) printf(" %2d", n1[i]);
    printf(" err=0x%04X", err);
    printf(" Done.\n");
  }

  if (PP > 1) printf("   Started temperature measurements.\n");

  setup_DS1820_readout(0xFF, 0xFF, 0x00, 0x00, 1, &err);
  wait(5);
  printf("bus      Dallas ID      Temp\n");
  for (i=0; i<9; i++) {
    if (n1[i]) {
      int j;
      read_DS1820_table(i+1, &n1[i], ID, pri, &err);
//    read_DS1820_temp_short(i+1, &n2, ok, temp, &err);
      read_DS1820_temp_long(i+1, &n2, ok, temp, NULL, &err);
      for (j=0; j<n1[i]; j++) {
        printf(" %d  0x%016llX", i+1, ID[j]);
        if (ok[j]) printf(" %+6.2f\n", temp[j]);
        else       printf("\n");
      }
    }
  }

  if (PP) printf("Completed dallas procedure.\n");
}

//~----------------------------------------------------------------------------

void setup_LeCroy_bus(void) {

  bool  chk;
  int16 err;
  int32 rea;
  int32 wri = 0x80480000;
  
  if (PP) printf("Started setup_LeCroy_bus procedure.\n");

  if (PP > 1) printf("   Initializing LeCroy bus:                   ...        ");
  init_LeCroy_bus(BUS, &err);
  if (PP > 1) printf("                                 err=0x%04X", err);
  if (PP > 1) printf(" Done.\n");

  if (PP > 1) printf("   Executing LeCroy command:                  ...        ");
  exec_LeCroy_cmd(1, &BUS, &wri, &rea, &chk, &err);
  if (PP > 1) printf(" wri=%08X rea=%08X chk=%d err=0x%04X", wri, rea, chk, err);
  if (PP > 1) printf(" Done.\n");

  if (PP > 1) printf("   Executing LeCroy command:                  ...        ");
  exec_LeCroy_cmd(1, &BUS, &wri, &rea, &chk, &err);
  if (PP > 1) printf(" wri=%08X rea=%08X chk=%d err=0x%04X", wri, rea, chk, err);
  if (PP > 1) printf(" Done.\n");

  if (PP) printf("Completed setup_LeCroy_bus procedure.\n");
}

//~----------------------------------------------------------------------------

void ugas_middle_open_v5_5(void) {

  bool  chk;
  int16 err;
  
  if (PP) printf("Started ugas_middle_open_v5_5 procedure.\n");

  enable_regulators(_24V, &chk, &err);
  enable_valves(V5, &chk, &err);
  open_valve(V5, 20000, &chk, &err);
  wait(21);
  enable_valves(0, &chk, &err);             // this will close the valve!!!
  enable_regulators(_24V, &chk, &err);      // why again???

  if (PP) printf("Completed ugas_middle_open_v5_5 procedure.\n");
}

//~----------------------------------------------------------------------------

void vent_mixing_volume(void) {

  bool  chk;
  int16 err;
  
  if (PP) printf("Started vent_mixing_volume procedure.\n");

  enable_regulators(_24V, &chk, &err);
  enable_valves(V5, &chk, &err);
  open_valve(V5, 60000, &chk, &err);   wait(62);
  open_valve(V5, 60000, &chk, &err);   wait(62);
  open_valve(V5, 60000, &chk, &err);   wait(62);
  open_valve(V5, 60000, &chk, &err);   wait(62);
  open_valve(V5, 60000, &chk, &err);   wait(62);
  enable_valves(0, &chk, &err);
  
  if (PP) printf("Completed vent_mixing_volume procedure.\n");
}

//~----------------------------------------------------------------------------

void sensors(void) {

  bool  chk;
  int16 err;
  int16 dat;
  float p2a = 0.0, p2b = 0.0;
  int i, N = 10;
  int16 error = 0;  

  printf("Sensors (P2b-P2a averaged): ");
  for (i=0; i<N; i++) {
    read_value(P2a, &dat, &chk, &err);
    p2a = p2a + dat;
    error |= chk;
    error |= err;
    read_value(P2b, &dat, &chk, &err);
    p2b = p2b + dat;
    error |= chk;
    error |= err;
  }
  if (!error) printf("%5.1f\n", (p2b-p2a)*5.0/4095.0*60.0/(float)N);
  else        printf("Failed.\n");
}

//~----------------------------------------------------------------------------

void sensors2(void) {

  bool  chk;
  int16 err;
  int16 dat;
  int i;

  printf("Sensors2: ");
  for (i=P1a; i<=T2b; i++) {
    read_value(i, &dat, &chk, &err);
    if (err || chk) printf("0xXXXX ");
    else            printf("0x%04X ", dat);
  }
  printf("\n");
}

//~----------------------------------------------------------------------------

void fill_co2(void) {

  bool  chk;
  int16 err;

  if (PP) printf("Started fill_co2 procedure.\n");
  
  enable_regulators(_5V+_24V, &chk, &err);  wait( 1);
  enable_valves(V1b,          &chk, &err);  wait( 1);
  open_valve(   V1b,  2000,   &chk, &err);  wait( 4);
  enable_valves(V2b,          &chk, &err);  wait( 1);
  open_valve(   V2b,  4000,   &chk, &err);  wait( 6);
  enable_valves(V3b,          &chk, &err);  wait( 1);
  open_valve(   V3b, 60000,   &chk, &err);  wait(62);

  if (PP) printf("Completed fill_co2 procedure.\n");
}

//~----------------------------------------------------------------------------

void fill_ar(void) {

  bool  chk;
  int16 err;
  
  if (PP) printf("Started fill_ar procedure.\n");

  enable_regulators(_5V+_24V, &chk, &err);  wait( 1);
  enable_valves(V1a,          &chk, &err);  wait( 1);
  open_valve(   V1a,  1000,   &chk, &err);  wait( 3);
  enable_valves(V2a,          &chk, &err);  wait( 1);
  open_valve(   V2a,  2000,   &chk, &err);  wait( 4);
  enable_valves(V3a,          &chk, &err);  wait( 1);
  open_valve(   V3a, 30000,   &chk, &err);  wait(32);

  if (PP) printf("Completed fill_ar procedure.\n");
}

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int8 dat[8];
  int16 err;

  setbuf(stdout, NULL);
  setup_command_path();

  printf(
  "Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level> [<Debug Printout Level>]]]]\n",
  argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);
  if (argc > 4) PP = atoi(argv[4]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);
  printf("PP      = %d\n", PP);

//~--- Reboot USCM from FLASH Memory ---
//     (otherwise some commands will not work)

  printf("Waiting for reboot...");
  boot_from_FLASH(&err);
  for (i=0; i<3; i++) {
    printf(".");
    get_short_status(dat, &err);
    if (!err) {
      printf("Done\n");
      break;
    }
  }
  if (err) {
    printf("Failed\n");
    exit(1);
  }

//~--- Gas Mixing Procedure Starts Here ---


  setup_LeCroy_bus();

  sensors();
  dallas();
  sensors();
  
  sensors();
//vent_mixing_volume();
  ugas_middle_open_v5_5();
  sensors();
  
  fill_co2(); sensors();
  fill_co2(); sensors();
//fill_ar();  sensors();
//fill_ar();  sensors();
//fill_ar();  sensors();

  return 0;
}

//~============================================================================
