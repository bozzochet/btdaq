// file lddr-test-2.c
//
// Program to test LAS QM (?) with external trigger.
// Following WK flow-chart.
//
// A.Lebedev Nov-2006...

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x1A7;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define PULSE_WIDTH__500     0x0001
#define PULSE_WIDTH_1000     0x0002
#define PULSE_WIDTH_2000     0x0004
#define PULSE_WIDTH_4000     0x0008
#define PULSE_WIDTH_MASK     0x000F
#define ENABLE_L0            0x0010
#define POWER_L0             0x0020
#define ENABLE_L1            0x0040
#define POWER_L1             0x0080

#define CONTINUOUS_OPERATION 0x0001
#define ENABLE_EXT_TRIGGER   0x0002
#define EXT_TRIGGER_PORT_A_B 0x0004
#define OPERATION_MODE_MASK  0x0007
#define FIRE_INT_TRIGGER     0x0008
#define BIAS_CURRENT_L0      0x0010
#define BIAS_CURRENT_L1      0x0020
#define POWER_L0_TRIP        0x0040
#define POWER_L1_TRIP        0x0080

#define SET_LCTL_PORT_A      0x08
#define SET_LCTL_PORT_B      0x10
#define TRIGGER_PULSE        0x20

#define LASER_REG_ERRORS      0
#define TRIGGER_REG_ERRORS    8
#define TRIPS                16
#define LASER_CURRENT_ERRORS 24
#define DALLAS_ERRORS        32
#define COMMUNICATION_ERRORS 40

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *config_filename;

char Operator_Name[80];
char Store_Directory[80];
int USCM_0, USCM_1;
int USCM_ON;
char LDDR_ON_bits[16][80], LDDR_OFF_bits[16][80], trigger_bits[8][80];
int16 trigger;
int16 LDDR_ON[2], LDDR_OFF[2];
int64 ID[2][10];
int P_Counter_Max;
int T_Counter_Max;
float Repetition_Time;
int LPT_Address;

struct {
  char *key;
  char *fmt;
  char *pc;
  int  *pin;
  int64 *p64;
  float *pfl;
  bool set;
} param[] = {

  {"Operator Name",   "%s", Operator_Name,   NULL,     NULL,    NULL},
  {"Store Directory", "%s", Store_Directory, NULL,     NULL,    NULL},
  
  {"USCM#0",          "%X", NULL,            &USCM_0,  NULL,    NULL},
  {"USCM#1",          "%X", NULL,            &USCM_1,  NULL,    NULL},
  {"USCM ON #",       "%d", NULL,            &USCM_ON, NULL,    NULL},
  
  {"LDDR ON: L0_Enable =",  "%s", LDDR_ON_bits[4],  NULL, NULL,  NULL},
  {"LDDR ON: L0_Power  =",  "%s", LDDR_ON_bits[5],  NULL, NULL,  NULL},
  {"LDDR ON: L1_Enable =",  "%s", LDDR_ON_bits[6],  NULL, NULL,  NULL},
  {"LDDR ON: L1_Power  =",  "%s", LDDR_ON_bits[7],  NULL, NULL,  NULL},
  {"LDDR ON: OFF/ON 0  =",  "%s", LDDR_ON_bits[12], NULL, NULL,  NULL},
  {"LDDR ON: OFF/ON 1  =",  "%s", LDDR_ON_bits[13], NULL, NULL,  NULL},

  {"LDDR OFF: L0_Enable =", "%s", LDDR_OFF_bits[4], NULL, NULL,  NULL},
  {"LDDR OFF: L0_Power  =", "%s", LDDR_OFF_bits[5], NULL, NULL,  NULL}, 
  {"LDDR OFF: L1_Enable =", "%s", LDDR_OFF_bits[6], NULL, NULL,  NULL},
  {"LDDR OFF: L1_Power  =", "%s", LDDR_OFF_bits[7], NULL, NULL,  NULL}, 
  {"LDDR OFF: OFF/ON 0  =", "%s", LDDR_OFF_bits[12],NULL, NULL,  NULL},
  {"LDDR OFF: OFF/ON 1  =", "%s", LDDR_OFF_bits[13],NULL, NULL,  NULL},

  {"T_Counter_Max =",  "%d",   NULL, &T_Counter_Max, NULL, NULL},
  {"Repeatition Time", "%f",   NULL, NULL,     NULL, &Repetition_Time},

  {"LPT Address =", "%X",      NULL, &LPT_Address,   NULL, NULL}};

int max_try = 3;

int board, current_wri;
int16 zer[16], dac[16], pwr[16];
int16 LDDR_laser_wri;
int16 LDDR_trigger_wri;
int16 LCTL_trigger_wri;
  
int errors[64];

int8 adr[5] = {0x10, 0x12, 0x14, 0x16, 0x18};

//~----------------------------------------------------------------------------

bool read_configuration_file(char *filename) {

  FILE *file = NULL;
  char fmt[80];
  bool fileOK = TRUE;
  int i, k;
  
  file = fopen(filename, "r");
  if (!file) {
    printf("*** File %s was not found.\n", filename);
    return FALSE;
  }

  for (i=0; i<sizeof(param)/sizeof(param[0]); i++) param[i].set = FALSE;
  for (i=0; i<16; i++) LDDR_ON_bits[i][0]  = 0;
  for (i=0; i<16; i++) LDDR_OFF_bits[i][0] = 0;
  for (i=0; i<8; i++) trigger_bits[i][0]   = 0;
  for (i=0; i<10; i++) for (k=0; k<2; k++) ID[k][i] = 0x0000000000000000;
  
  while (1) {
    char b[8192];
    bool accepted = FALSE;
    
    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-1] = '\0';

    if (P) printf("***************************\nline = %s\n", b);

    for (i=0; i<sizeof(param)/sizeof(param[0]); i++) {
      int k = -1;

      if (param[i].set) continue;
      if (accepted) break;
      
      sprintf(fmt, "%s %s", param[i].key, param[i].fmt);
      if (P) printf("fmt = %s\n", fmt);

      if (param[i].pc)  k = sscanf(b, fmt, param[i].pc);
      if (param[i].pin) k = sscanf(b, fmt, param[i].pin);
      if (param[i].p64) k = sscanf(b, fmt, param[i].p64);
      if (param[i].pfl) k = sscanf(b, fmt, param[i].pfl);

      if (P) {
        sprintf(fmt, "%s\n", param[i].fmt);
        printf("i:%d:   k = %d, param = ", i, k);
        if (param[i].pc)  printf(fmt,  param[i].pc);
        if (param[i].pin) printf(fmt, *param[i].pin);
        if (param[i].p64) printf(fmt, *param[i].p64);
        if (param[i].pfl) printf(fmt, *param[i].pfl);
      }

      if (k == 1) {
        accepted = TRUE;
        param[i].set = TRUE;
        if (P) printf("ACCEPTED\n***************************\n");
      }
    }
  }

  fclose(file);
  
  USCM_ON = USCM_ON ? 1 : 0;

  printf("\nConfiguration Data:\n-------------------\n");
  for (i=0; i<sizeof(param)/sizeof(param[0]); i++) {
    printf("%s ", param[i].key);
    if (param[i].set) {
      sprintf(fmt, "%s", param[i].fmt);
      if (param[i].pc)  printf(fmt,  param[i].pc);
      if (param[i].pin) printf(fmt, *param[i].pin);
      if (param[i].p64) {
        printf(fmt, *param[i].p64);
        if (!DOWCRC(*param[i].p64)) printf(" *** ID CRC error");
      }
      if (param[i].pfl) printf(fmt, *param[i].pfl);
      printf("\n");
    }
    else {
      printf("*** NOT DEFINED ***\n");
      fileOK = FALSE;
    }
  }

  if (!fileOK) {
    printf("*** Configuration Data have errors.\n");
    return FALSE;
  }
  else {
    trigger = 0;
    for (k=0; k<8; k++) if (trigger_bits[k][0] == 'H') trigger |= 1 << k;
    for (i=0; i<2; i++) LDDR_ON[i] = 0;
    for (k=0; k<16; k++) if (LDDR_ON_bits[k][0] == 'H') LDDR_ON[k/8] |= 1 << (k % 8);
    LDDR_ON[1] |= trigger;
    for (i=0; i<2; i++) LDDR_OFF[i] = 0;
    for (k=0; k<16; k++) if (LDDR_OFF_bits[k][0] == 'H') LDDR_OFF[k/8] |= 1 << (k % 8);
    printf("LDDR ON:  Laser = %02hhX, Trigger = %02hhX\n", LDDR_ON[0], LDDR_ON[1]);
    printf("LDDR OFF: Laser = %02hhX, Trigger = %02hhX\n", LDDR_OFF[0], LDDR_OFF[1]);
    printf("\n");
    return TRUE;
  }
}

//~----------------------------------------------------------------------------

bool setup_USCM() {

  int i;
  int16 err;

  USCM_ID = USCM_ON ? USCM_1 : USCM_0;

  boot_from_FLASH(&err);
  
  if (err) {
    for (i=0; i<3; i++) {
      ping_USCM(0, NULL, NULL, NULL, &err);
      if (!err) break;
    }
  }

  if (err) {
    errors[COMMUNICATION_ERRORS]++;
    printf("*** Failed to setup USCM %X.\n", USCM_ID);
    return FALSE;
  }
  else {
    printf("USCM %X setup complete.\n\n", USCM_ID);
    return TRUE;
  }
}

//~----------------------------------------------------------------------------

bool set_LDDR_board_to_OFF_state(int board) {

  int16 err;
  
  LDDR_laser_wri   = LDDR_OFF[0];
  LDDR_trigger_wri = LDDR_OFF[1];

  err = 0x0000;
  if (!err) {
    write_LVDS_bus(adr[board], LDDR_laser_wri, &err);
    if (err) {
      errors[COMMUNICATION_ERRORS]++;
      printf("*** LDDR #%d Laser Reg write error (1)\n", board);
    }
  }
  if (!err) {
    write_LVDS_bus(adr[board]+1, LDDR_trigger_wri, &err);
    if (err) {
      errors[COMMUNICATION_ERRORS]++;
      printf("*** LDDR #%d Trigger Reg write error (1)\n", board);
    }
  }
  if (!err) {
    int i;
    for (i=0; i<16; i++) zer[i] = i < 10 ? 0x0000 : 0x8000;
    write_DAC(zer, &err);
    if (err) {
      errors[COMMUNICATION_ERRORS]++;
      printf("*** DAC write error (1)\n");
    }
  }
  if (!err) {
    printf("LDDR #%d is set to OFF state\n", board);
  }

  return (err ? FALSE : TRUE);
}

//~----------------------------------------------------------------------------

void set_laser_current(void) {

  int i;

  current_wri = 75;

  for (i=0; i<16; i++) zer[i] = (i/2 == board) ? 0x0000 : 0x8000;
  for (i=0; i<16; i++) dac[i] = (i/2 == board) ? 20*current_wri : 0x8000;
}

//~----------------------------------------------------------------------------

void initialize_report(void) {

  int i;

  for (i=0; i<64; i++) errors[i] = 0;
}

//~----------------------------------------------------------------------------

bool set_LDDR_board_to_ON_state(int board, char c) {

  int try;
  int16 err;

  LDDR_laser_wri   = LDDR_ON[0];
  LDDR_laser_wri  |= PULSE_WIDTH_2000;
  LDDR_trigger_wri  = LDDR_ON[1];
  LDDR_trigger_wri |= ENABLE_EXT_TRIGGER;
  if (toupper(c) == 'A') LDDR_trigger_wri &= ~EXT_TRIGGER_PORT_A_B;
  else                   LDDR_trigger_wri |=  EXT_TRIGGER_PORT_A_B;

  for (try=0; try<max_try; try++) {
    err = 0x0000;
    if (!err) {
      write_LVDS_bus(adr[board], LDDR_laser_wri, &err);
      if (err) {
        errors[COMMUNICATION_ERRORS]++;
        printf("*** LDDR #%d Laser Reg write error (2)\n", board);
      }
    }
    if (!err) {
      write_LVDS_bus(adr[board]+1, LDDR_trigger_wri, &err);
      if (err) {
        errors[COMMUNICATION_ERRORS]++;
        printf("*** LDDR #%d Trigger Reg write error (2)\n", board);
      }
    }
    if (!err) {
      write_DAC(dac, &err);
      if (err) {
        errors[COMMUNICATION_ERRORS]++;
        printf("*** DAC write error (2)\n");
      }
    }
    if (!err) {
      printf("LDDR #%d is set to ON state\n\n", board);
      return TRUE;
    }
  }

  printf("*** LDDR #%d failed to be set to ON state\n\n", board);
  return FALSE;
}

//~----------------------------------------------------------------------------

bool setup_LCTL_board(char c) {

  int try;
  int16 err;
  int16 V = toupper(c) == 'A' ? 2200 : 4095;
  
  for (try=0; try<max_try; try++) {
    err = 0x0000;
    if (!err) {
      int i;
      for (i=0; i<16; i++) pwr[i] = (i == 14) ? 0x8000 + V : 0x0000;
      write_DAC(zer, &err);
      if (err) {
        errors[COMMUNICATION_ERRORS]++;
        printf("*** DAC write error (3)\n");
      }
      if (!err) {
        printf("LCTL DAC is set to %5.3fV\n", (float)V/1000.0);
        return TRUE;
      }
    }
  }

  printf("*** LCTL failed to be set to %c state\n\n", toupper(c));
  return FALSE;
}

//~----------------------------------------------------------------------------

void trigger_routine(char c) {

  int T_Counter;

  LCTL_trigger_wri  = board;
  LCTL_trigger_wri |= toupper(c) == 'A' ? SET_LCTL_PORT_A : SET_LCTL_PORT_B;

  for (T_Counter=0; T_Counter<T_Counter_Max; T_Counter++) {
    if (LPT_Address) {
      outb(LCTL_trigger_wri,                 LPT_Address);
      outb(LCTL_trigger_wri | TRIGGER_PULSE, LPT_Address);
      outb(LCTL_trigger_wri,                 LPT_Address);
      printf("Triggered (%d)\n", T_Counter);
    }
    delay(Repetition_Time);
  }
}

//~----------------------------------------------------------------------------

bool ask(char c) {

  int k;
  char b[8192];
  char y_n;
  
  while (1) {
    printf("Test LDDR#%d with trigger port %c? (y/n) <- ", board, toupper(c));
    fgets(b, sizeof(b), stdin);
    b[strlen(b)-1] = '\0';
    k = sscanf(b, " %c", &y_n);
    if (k == 1 && y_n == 'y') break;
    if (k == 1 && y_n == 'n') {
      printf("\n");
      return FALSE;
    }
    printf("*** Error in input\n");
  }
  return TRUE;
}

//~----------------------------------------------------------------------------

void printf_report(FILE *file) {

  setbuf(file, NULL);

  fprintf(file, "\n");
  fprintf(file, "Results:\n");
  fprintf(file, "--------\n");
  
  fprintf(file, "Date & Time:          %s\n", timestamp(1));
  fprintf(file, "Operator Name:        %s\n", Operator_Name);
  fprintf(file, "LDDR Board:           %d\n", board);
  fprintf(file, "USCM Board:           %X\n", USCM_ID);
  fprintf(file, "Trigger-Mode:         External\n");
  fprintf(file, "Nb of Measurements:   %d\n", P_Counter_Max);
  fprintf(file, "Current Set Value:    %d\n", current_wri);
  fprintf(file, "\n");
  fprintf(file, "Failure Report:\n");
  fprintf(file, "---------------\n");
  
  fprintf(file, "-------------------------------------------------\n");
  fprintf(file, "Communication Errors:                   %4d\n", errors[COMMUNICATION_ERRORS]);
  fprintf(file, "-------------------------------------------------\n");
  fprintf(file, "\n");
}

//~----------------------------------------------------------------------------

void terminate_program(void) {

  printf_report(stdout);
  exit(1);
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s <config-file-name> [P]\n",argv[0]);

  if (argc < 2) {
    printf("Configuration File Name is missing.\n");
    exit(1);
  }
  config_filename = argv[1];
  if (argc > 2) P = atoi(argv[2]);

  printf("\nLAS test with EXTERNAL trigger");
  printf("\n===============================\n");
  printf("\nInput Parameters:");
  printf("\n-----------------\n");
  printf("Configuration File = %s\n", argv[1]);
  printf("P                  = %d\n", P);
  
  if (!read_configuration_file(config_filename)) exit(1);

  if (!initialize_EPP()) exit(1);

  if (!setup_USCM()) exit(1);
  
  
  for (board=0; board<4; board++) {
    initialize_report();
    set_laser_current();
    for (i=0; i<5; i++) set_LDDR_board_to_OFF_state(i);
    if (LPT_Address) outb((int8)board, LPT_Address);
    if (ask('A')) {
      if (!setup_LCTL_board('A')) terminate_program();
      if (!set_LDDR_board_to_ON_state(board, 'A')) terminate_program();
      trigger_routine('A');
    }
    if (ask('B')) {
      if (!setup_LCTL_board('B')) terminate_program();
      if (!set_LDDR_board_to_ON_state(board, 'B')) terminate_program();
      trigger_routine('B');
    }
    printf_report(stdout);
  }
  for (i=0; i<5; i++) set_LDDR_board_to_OFF_state(i);
  
  return 0;
}

//~============================================================================
