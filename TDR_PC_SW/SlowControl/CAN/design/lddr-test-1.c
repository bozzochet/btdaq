// file lddr-test-1.c
//
// Program to test LAS QM (?) with internal trigger.
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

  {"Trigger:  L_DC    =",   "%s", trigger_bits[0],  NULL, NULL,  NULL},
  {"Trigger:  I_trg   =",   "%s", trigger_bits[1],  NULL, NULL,  NULL},
  {"Trigger:  Trg B/A =",   "%s", trigger_bits[2],  NULL, NULL,  NULL},

  {"LDDR#0 L0 Hot", "%016llX", NULL, NULL,     &ID[0][0], NULL},
  {"LDDR#0 L1 Hot", "%016llX", NULL, NULL,     &ID[0][1], NULL},
  {"LDDR#1 L0 Hot", "%016llX", NULL, NULL,     &ID[0][2], NULL},
  {"LDDR#1 L1 Hot", "%016llX", NULL, NULL,     &ID[0][3], NULL},
  {"LDDR#2 L0 Hot", "%016llX", NULL, NULL,     &ID[0][4], NULL},
  {"LDDR#2 L1 Hot", "%016llX", NULL, NULL,     &ID[0][5], NULL},
  {"LDDR#3 L0 Hot", "%016llX", NULL, NULL,     &ID[0][6], NULL},
  {"LDDR#3 L1 Hot", "%016llX", NULL, NULL,     &ID[0][7], NULL},
  {"LDDR#4 L0 Hot", "%016llX", NULL, NULL,     &ID[0][8], NULL},
  {"LDDR#4 L1 Hot", "%016llX", NULL, NULL,     &ID[0][9], NULL},

  {"LDDR#0 L0 Cld", "%016llX", NULL, NULL,     &ID[1][0], NULL},
  {"LDDR#0 L1 Cld", "%016llX", NULL, NULL,     &ID[1][1], NULL},
  {"LDDR#1 L0 Cld", "%016llX", NULL, NULL,     &ID[1][2], NULL},
  {"LDDR#1 L1 Cld", "%016llX", NULL, NULL,     &ID[1][3], NULL},
  {"LDDR#2 L0 Cld", "%016llX", NULL, NULL,     &ID[1][4], NULL},
  {"LDDR#2 L1 Cld", "%016llX", NULL, NULL,     &ID[1][5], NULL},
  {"LDDR#3 L0 Cld", "%016llX", NULL, NULL,     &ID[1][6], NULL},
  {"LDDR#3 L1 Cld", "%016llX", NULL, NULL,     &ID[1][7], NULL},
  {"LDDR#4 L0 Cld", "%016llX", NULL, NULL,     &ID[1][8], NULL},
  {"LDDR#4 L1 Cld", "%016llX", NULL, NULL,     &ID[1][9], NULL},

  {"P_Counter_Max =",  "%d",   NULL, &P_Counter_Max, NULL, NULL},
  {"T_Counter_Max =",  "%d",   NULL, &T_Counter_Max, NULL, NULL},
  {"Repeatition Time", "%f",   NULL, NULL,     NULL, &Repetition_Time},

  {"LPT Address =", "%X",      NULL, &LPT_Address,   NULL, NULL}};

int max_try = 3;

int board, current_wri, current_rea, width, P_Counter;
char default_report_filename[80] = "lddr-test-1-report.txt";
int16 zer[16], dac[16];
int16 LDDR_laser_wri, LDDR_laser_rea;
int16 LDDR_trigger_wri, LDDR_trigger_rea;
bool ok[64];
float tmp[64];
int16 age[64];
float temperature[2], max_temperature[2], min_temperature[2];
int current[2], max_current[2], min_current[2];
  
int errors[64];

int widths[4] = {500, 1000, 2000, 4000};
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

bool setup_DTS_readout(void) {

  int16 err;

  write_DS1820_table(1, 10, &ID[USCM_ON][0], NULL, &err);
  if (err) {
    errors[COMMUNICATION_ERRORS]++;
    printf("*** Failed to write DTS ID\n");
    return FALSE;
  }

  setup_DS1820_readout(0x01, 0x01, 0x00, 0x00, 1, &err);
  if (err) {
    errors[COMMUNICATION_ERRORS]++;
    printf("*** Failed to start DTS readout\n");
    return FALSE;
  }

  printf("DTS setup complete.\n\n");
  return TRUE;
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

  printf("\n");
  return (err ? FALSE : TRUE);
}

//~----------------------------------------------------------------------------

void input_LDDR_board_number(void) {

  int k;
  char b[8192];

  while (1) {
    printf("Input LDDR # Board (0..4) <- ");
    fgets(b, sizeof(b), stdin);
    b[strlen(b)-1] = '\0';
    k = sscanf(b, "%d", &board);
    if ((k == 1) && (board >= 0) && (board < 5)) break;
    printf("*** Error in input\n");
  }
}

//~----------------------------------------------------------------------------

void input_laser_current(void) {

  int k;
  char b[8192];

  while (1) {
    printf("Input Laser Current (25, 75, 125) <- ");
    fgets(b, sizeof(b), stdin);
    b[strlen(b)-1] = '\0';
    k = sscanf(b, "%d",  &current_wri);
    if ((k == 1) && ((current_wri == 25) || (current_wri == 75) || (current_wri == 125))) {
      int i;
      for (i=0; i<16; i++) zer[i] = (i/2 == board) ? 0x0000 : 0x8000;
      for (i=0; i<16; i++) dac[i] = (i/2 == board) ? 20*current_wri : 0x8000;
      break;
    }
    printf("*** Error in input\n");
  }
}

//~----------------------------------------------------------------------------

bool wait_for_DTS_to_startup(void) {

  int i, k;
  int8 n, n_OK;
  int16 err;

  printf("\nWaiting for Dallas sensors to startup\n");
  
  for (i=0; i<10; i++) {
    read_DS1820_temp_long(1, &n, ok, tmp, age, &err);
    if (err) {
      errors[COMMUNICATION_ERRORS]++;
      printf("*** Dallas sensor readout error\n");
    }
    else {
      n_OK = 0;
      for (k=0; k<10; k++) if (ok[k]) n_OK++;
      if (n_OK == 10) {
        printf("All 10 Dallas sensors working OK.\n\n");
        return TRUE;
      }
    }
    sleep(1);
  }

  printf("*** (Some) Dallas sensors are not working.\n\n");
  return FALSE;
}

//~----------------------------------------------------------------------------

void initialize_report(void) {

  int i;

  for (i=0; i<64; i++) errors[i] = 0;
  for (i=0; i<2; i++) max_temperature[i] = -300.00;
  for (i=0; i<2; i++) min_temperature[i] =  300.00;
  for (i=0; i<2; i++) max_current[i] = 0.0;
  for (i=0; i<2; i++) min_current[i] = 4095.0 / 10.0;
}

//~----------------------------------------------------------------------------

bool set_LDDR_board_to_ON_state(int board) {

  int try;
  int16 err;

  LDDR_laser_wri   = LDDR_ON[0];
  LDDR_trigger_wri = LDDR_ON[1];
  if (width == 0) LDDR_laser_wri |= PULSE_WIDTH__500;
  if (width == 1) LDDR_laser_wri |= PULSE_WIDTH_1000;
  if (width == 2) LDDR_laser_wri |= PULSE_WIDTH_2000;
  if (width == 3) LDDR_laser_wri |= PULSE_WIDTH_4000;

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
      printf("LDDR #%d is set to ON state. ", board);
      printf("Pulse Length = %dns, Laser Current = %dmA\n\n",
              widths[width], current_wri);
      return TRUE;
    }
  }

  printf("*** LDDR #%d failed to be set to ON state\n\n", board);
  printf("\n");
  return FALSE;
}

//~----------------------------------------------------------------------------

bool trigger_LDDR_board(void) {

  int try;
  int16 err;

  for (try=0; try<max_try; try++) {
    err = 0x0000;
    if (!err) {
      LDDR_trigger_wri &= ~FIRE_INT_TRIGGER;
      write_LVDS_bus(adr[board]+1, LDDR_trigger_wri, &err);
      if (err) {
        errors[COMMUNICATION_ERRORS]++;
        printf("*** LDDR #%d Trigger Reg write error (1)\n", board);
      }
    }
    if (!err) {
      LDDR_trigger_wri |=  FIRE_INT_TRIGGER;
      write_LVDS_bus(adr[board]+1, LDDR_trigger_wri, &err);
      if (err) {
        errors[COMMUNICATION_ERRORS]++;
        printf("*** LDDR #%d Trigger Reg write error (2)\n", board);
      }
    }
    if (!err) {
      LDDR_trigger_wri &= ~FIRE_INT_TRIGGER;
      write_LVDS_bus(adr[board]+1, LDDR_trigger_wri, &err);
      if (err) {
        errors[COMMUNICATION_ERRORS]++;
        printf("*** LDDR #%d Trigger Reg write error (3)\n", board);
      }
    }
    if (!err) {
      printf("LDDR #%d is triggered (%d)\n", board, P_Counter);
      return TRUE;
    }
  }

  printf("*** LDDR #%d failed to be triggered (%d)\n\n", board, P_Counter);
  printf("\n");
  return FALSE;
}

//~----------------------------------------------------------------------------

bool check_LDDR_laser_reg() {

  int i;
  int16 m;
  int try;
  int16 err;

  for (try=0; try<max_try; try++) {
    read_LVDS_bus(adr[board], &LDDR_laser_rea, &err);
    if (err) {
      errors[COMMUNICATION_ERRORS]++;
      printf("*** LDDR #%d Laser Reg read error\n", board);
    }
    else {
      break;
    }
  }
  
  if (err) {
    printf("*** LDDR #%d laser failed to be checked (1) (%d)\n\n", board, P_Counter);
    printf("\n");
    return FALSE;
  }
  
  m = (LDDR_laser_wri ^ LDDR_laser_rea) & 0x00FF;
  if (m) {
    for (i=0; i<8; i++) if (m & (1<<i)) errors[LASER_REG_ERRORS+i]++;
    for (try=0; try<max_try; try++) {
      write_LVDS_bus(adr[board], LDDR_laser_wri, &err);
      if (err) {
        errors[COMMUNICATION_ERRORS]++;
        printf("*** LDDR #%d Laser Reg write error (3)\n", board);
      }
      else {
        break;
      }
    }
    if (err) {
      printf("*** LDDR #%d laser failed to be checked (2) (%d)\n\n", board, P_Counter);
      printf("\n");
      return FALSE;
    }
  }

  return TRUE;
}

//~----------------------------------------------------------------------------

bool check_LDDR_trigger_reg() {

  int i;
  int16 m;
  int try;
  int16 err;

  for (try=0; try<max_try; try++) {
    read_LVDS_bus(adr[board]+1, &LDDR_trigger_rea, &err);
    if (err) {
      errors[COMMUNICATION_ERRORS]++;
      printf("*** LDDR #%d trigger Reg read error\n", board);
    }
    else {
      break;
    }
  }
  
  if (err) {
    printf("*** LDDR #%d trigger failed to be checked (1) (%d)\n\n", board, P_Counter);
    printf("\n");
    return FALSE;
  }
  
  m = (LDDR_trigger_wri ^ LDDR_trigger_rea) & 0x0007;
  if (m) {
    for (i=0; i<8; i++) if (m & (1<<i)) errors[TRIGGER_REG_ERRORS+i]++;
    for (try=0; try<max_try; try++) {
      write_LVDS_bus(adr[board]+1, LDDR_trigger_wri, &err);
      if (err) {
        errors[COMMUNICATION_ERRORS]++;
        printf("*** LDDR #%d trigger Reg write error (3)\n", board);
      }
      else {
        break;
      }
    }
    if (err) {
      printf("*** LDDR #%d trigger failed to be checked (2) (%d)\n\n", board, P_Counter);
      printf("\n");
      return FALSE;
    }
  }

  return TRUE;
}

//~----------------------------------------------------------------------------

bool check_LDDR_trips() {

  int try;
  bool ok = FALSE;
  int16 err;

  for (try=0; try<max_try; try++) {
    read_LVDS_bus(adr[board]+1, &LDDR_trigger_rea, &err);
    if (err) {
      ok = FALSE;
      errors[COMMUNICATION_ERRORS]++;
      printf("*** LDDR #%d Trigger Reg read error (2)\n", board);
    }
    else {
      ok = TRUE;
      break;
    }
  }
  
  if (!ok) return FALSE;
  
  if (LDDR_trigger_rea & (POWER_L0_TRIP || POWER_L1_TRIP)) {
    printf("Power Trip detected.\n");
    if (LDDR_trigger_rea & POWER_L0_TRIP) errors[TRIPS+0]++;
    if (LDDR_trigger_rea & POWER_L1_TRIP) errors[TRIPS+1]++;
    if (!set_LDDR_board_to_OFF_state(board)) return FALSE;
    if (!set_LDDR_board_to_ON_state(board)) return FALSE;
    printf("Trip fixed (?)\n");
  }

  return TRUE;
}

//~----------------------------------------------------------------------------

bool read_temperatures() {

  int i;
  int try;
  int8 n;
  int16 err;

  for (try=0; try<max_try; try++) {
    read_DS1820_temp_long(1, &n, ok, tmp, age, &err);
    if (err) {
      errors[COMMUNICATION_ERRORS]++;
      printf("*** Dallas sensor readout error\n");
    }
    else {
      for (i=0; i<2; i++) {
        int k = 2 * board + i;
        if (ok[k]) {
          temperature[i]     = tmp[k];
          max_temperature[i] = MAX(max_temperature[i], tmp[k]);
          min_temperature[i] = MIN(min_temperature[i], tmp[k]);
        }
        else {
          errors[DALLAS_ERRORS+k]++;
        }
      }
      return TRUE;
    }
  }

  return FALSE;
}

//~----------------------------------------------------------------------------

bool read_laser_currents() {

  int i;
  int16 adc[32];
  int try;
  int16 err;

  for (try=0; try<max_try; try++) {
    read_ADC(adc, &err);
    if (err) {
      errors[COMMUNICATION_ERRORS]++;
      printf("*** ADC read error\n");
    }
    if (!err) {
      for (i=0; i<2; i++) {
        current_rea = (float)adc[2*board+i] / 10.0;
        max_current[i] = MAX(max_current[i], current_rea);
        min_current[i] = MIN(min_current[i], current_rea);
        if ((float)ABS(current_wri-current_rea)/(float)current_wri > 0.20) {
          errors[LASER_CURRENT_ERRORS+i]++;
        }
      }
      return TRUE;
    }
  }

  return FALSE;
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
  fprintf(file, "Trigger-Mode:         Internal\n");
  fprintf(file, "Nb of Measurements:   %d\n", P_Counter_Max);
  fprintf(file, "Current Set Value:    %d\n", current_wri);
  fprintf(file, "L0 Current: min, max: %d, %d\n", min_current[0], max_current[0]);
  fprintf(file, "L1 Current: min, max: %d, %d\n", min_current[1], max_current[1]);
  fprintf(file, "Temperatures:\n");
  if (min_temperature[0] < 200) {
    fprintf(file, "L0: ID, min, max:     %016llX, %+7.2f, %+7.2f\n",
            ID[USCM_ON][2*board+0], min_temperature[0], max_temperature[0]);
  }
  else {
    fprintf(file, "L0: ID, min, max:     %016llX\n",
            ID[USCM_ON][2*board+0]);
  }
  if (min_temperature[1] < 200) {
    fprintf(file, "L1: ID, min, max:     %016llX, %+7.2f, %+7.2f\n",
            ID[USCM_ON][2*board+1], min_temperature[1], max_temperature[1]);
  }
  else {
    fprintf(file, "L1: ID, min, max:     %016llX\n",
            ID[USCM_ON][2*board+1]);
  }
  
  fprintf(file, "\n");
  fprintf(file, "Failure Report:\n");
  fprintf(file, "---------------\n");
  
  fprintf(file, "-------------------------------------------------\n");
  fprintf(file, "Register   bit   function               failures#\n");
  fprintf(file, "-------------------------------------------------\n");
  fprintf(file, "Laser      D0    Pulse Duration 500ns   %4d\n", errors[LASER_REG_ERRORS+0]);
  fprintf(file, "           D1    Pulse Duration 1000ns  %4d\n", errors[LASER_REG_ERRORS+1]);
  fprintf(file, "           D2    Pulse Duration 2000ns  %4d\n", errors[LASER_REG_ERRORS+2]);
  fprintf(file, "           D3    Pulse Duration 4000ns  %4d\n", errors[LASER_REG_ERRORS+3]);
  fprintf(file, "           D4    Disable/Enable L0      %4d\n", errors[LASER_REG_ERRORS+4]);
  fprintf(file, "           D5    Power L0               %4d\n", errors[LASER_REG_ERRORS+5]);
  fprintf(file, "           D6    Disable/Enable L1      %4d\n", errors[LASER_REG_ERRORS+6]);
  fprintf(file, "           D7    Power L1               %4d\n", errors[LASER_REG_ERRORS+7]);
  fprintf(file, "\n");
  fprintf(file, "Trigger    D0    Laser pulsed/DC (0/1)  %4d\n", errors[TRIGGER_REG_ERRORS+0]);
  fprintf(file, "           D1    Trigger Ext/Int (1/0)  %4d\n", errors[TRIGGER_REG_ERRORS+1]);
  fprintf(file, "           D2    Trigger A/B (0/1)      %4d\n", errors[TRIGGER_REG_ERRORS+2]);
  fprintf(file, "           D3    Trigger ___---___      %4d\n", errors[TRIGGER_REG_ERRORS+3]);
  fprintf(file, "\n");
  fprintf(file, "           D6    L0 Trip ON/OFF (0/1)   %4d\n", errors[TRIGGER_REG_ERRORS+6]);
  fprintf(file, "           D7    L1 Trip ON/OFF (0/1)   %4d\n", errors[TRIGGER_REG_ERRORS+7]);
  fprintf(file, "-------------------------------------------------\n");
  fprintf(file, "\n");
  fprintf(file, "-------------------------------------------------\n");
  fprintf(file, "Dallas     L0    %016llX       %4d\n", ID[USCM_ON][2*board+0], errors[DALLAS_ERRORS+0]);
  fprintf(file, "Dallas     L1    %016llX       %4d\n", ID[USCM_ON][2*board+1], errors[DALLAS_ERRORS+1]);
  fprintf(file, "-------------------------------------------------\n");
  fprintf(file, "\n");
  fprintf(file, "-------------------------------------------------\n");
  fprintf(file, "Current    L0                           %4d\n",  errors[LASER_CURRENT_ERRORS+0]);
  fprintf(file, "Current    L1                           %4d\n",  errors[LASER_CURRENT_ERRORS+1]);
  fprintf(file, "-------------------------------------------------\n");
  fprintf(file, "\n");
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

void save_report(void) {

  int k;
  char b[8192];
  char y_n;
  char filename[80];
  char *report_filename;
  FILE *report_file;

  while (1) {
    printf("Store report? (y/n) <- ");
    fgets(b, sizeof(b), stdin);
    b[strlen(b)-1] = '\0';
    k = sscanf(b, " %c", &y_n);
    if (k == 1 && y_n == 'y') break;
    if (k == 1 && y_n == 'n') {
      printf("\n");
      return;
    }
    printf("*** Error in input\n");
  }

  printf("Input report filename <- ");
  fgets(b, sizeof(b), stdin);
  b[strlen(b)-1] = '\0';
  report_filename = sscanf(b, "%s", filename) == 1 ? filename : default_report_filename;

  if ((report_file = fopen(report_filename, "a+"))) {
    printf_report(report_file);
    printf("Writing report file %s completed.\n\n", report_filename);
    fclose(report_file);
  }
  else {
    printf("*** Writing report file failed.\n\n");
  }
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

  printf("\nLAS test with INTERNAL trigger");
  printf("\n==============================\n");
  printf("\nInput Parameters:");
  printf("\n-----------------\n");
  printf("Configuration File = %s\n", argv[1]);
  printf("P                  = %d\n", P);
  
  if (!read_configuration_file(config_filename)) exit(1);

  if (!initialize_EPP()) exit(1);

  if (!setup_USCM()) exit(1);
  if (!setup_DTS_readout()) exit(1);
  
  input_LDDR_board_number();
  input_laser_current();
  
  wait_for_DTS_to_startup();

  for (width=0; width<4; width++) {
    initialize_report();
    for (i=0; i<5; i++) set_LDDR_board_to_OFF_state(i);
    if (!set_LDDR_board_to_ON_state(board)) terminate_program();
    if (LPT_Address) outb((int8)board, LPT_Address);
    for (P_Counter=0; P_Counter<P_Counter_Max; P_Counter++) {
      if (!trigger_LDDR_board()) terminate_program();
      delay(Repetition_Time);
      if (!check_LDDR_laser_reg()) terminate_program();
      if (!check_LDDR_trigger_reg()) terminate_program();
      if (!check_LDDR_trips()) terminate_program();
      if (!read_temperatures()) terminate_program();
      if (!read_laser_currents()) terminate_program();
    }
    printf_report(stdout);
    save_report();
  }
  for (i=0; i<5; i++) set_LDDR_board_to_OFF_state(i);
  
  return 0;
}

//~============================================================================
