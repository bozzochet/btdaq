// file cab-get-digital-status-2.c
//
//  Get CAB Digital Status and print it.
//  (sketches to be used in browsers etc)
//
//  A.Lebedev Nov - 2008...
//

#include "cablib.h"

int JMDC_ID = 0x03;                                // global variable
int USCM_ID = 0x18;                                // global variable
int P = 0;                                         // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

char b[2][100][80];

//~----------------------------------------------------------------------------

void _sprint_CAB_digital_status(int8 *sta, int *n, char b[][80]) {

  int i = 0;
  int k;

//~--

  sprintf(&b[i++][0],
          "SW_CYCLE_COUNTER = %02hhX%02hhX%02hhX%02hhX (%.0fsec)", 
          sta[0], sta[1], sta[2], sta[3], 
          0.0125*(((sta[0]*256+sta[1])*256+sta[2])*256+sta[3]));
//~--

  sprintf(&b[i++][0],"CSC_STATUS:");

  sprintf(&b[i++][0],"  CSC_CONV_CMD_STATUS    = %02hhX", sta[4]);
  sprintf(&b[i++][0],"    Converter 1 Status: %s", (sta[4] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Converter 2 Status: %s", (sta[4] & 0x02) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Converter 3 Status: %s", (sta[4] & 0x04) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Converter 4 Status: %s", (sta[4] & 0x08) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Converter 5 Status: %s", (sta[4] & 0x10) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Converter 6 Status: %s", (sta[4] & 0x20) ? "ON" : "OFF");

  sprintf(&b[i++][0],"  CURRENT_1_CMD_SETTINGS = %02hhX%02hhX", sta[ 5], sta[ 6]);
  sprintf(&b[i++][0],"  CURRENT_2_CMD_SETTINGS = %02hhX%02hhX", sta[ 7], sta[ 8]);
  sprintf(&b[i++][0],"  CURRENT_3_CMD_SETTINGS = %02hhX%02hhX", sta[ 9], sta[10]);

  k = sprintf(&b[i][0],"  CCS_COMMANDING_STATUS  = %02hhX ", sta[11]);
  if      (sta[11] == 0x5F) sprintf(&b[i++][0]+k,"(ARMED)");
  else if (sta[11] == 0x50) sprintf(&b[i++][0]+k,"(DISARMED)");
  else                      sprintf(&b[i++][0]+k,"(?)");

//~--

  sprintf(&b[i++][0],"STM_STATUS:");

  sprintf(&b[i++][0],"  STM_STATUS = %02hhX", sta[12]);
  sprintf(&b[i++][0],"    COLD_VALVE_CMD_STS: %s", (sta[12] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    COLD_VALVE_SW_STS:  %s", (sta[12] & 0x02) ? "ON" : "OFF");

//~--

  sprintf(&b[i++][0],"PTM_STATUS:");

  sprintf(&b[i++][0],"  PTM_STATUS = %02hhX", sta[13]);
  sprintf(&b[i++][0],"    ON_OFFSET_CAL2_CMD_STS: %s", (sta[13] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    ON_OFFSET_CAL3_CMD_STS: %s", (sta[13] & 0x02) ? "ON" : "OFF");

//~--

  sprintf(&b[i++][0],"PS_STATUS:");

  sprintf(&b[i++][0],"  PS_CMD_SW_STATUS = %02hhX", sta[14]);
  sprintf(&b[i++][0],"    28V_ISO_CMD:         %s", (sta[14] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Vacuum Sensor:       %s", (sta[14] & 0x02) ? "POWERED" : "NOT POWERED");
  sprintf(&b[i++][0],"    Helium Tank Heater:  %s", (sta[14] & 0x04) ? "POWERED" : "NOT POWERED");
  sprintf(&b[i++][0],"    Pressure Sensor:     %s", (sta[14] & 0x08) ? "POWERED" : "NOT POWERED");
  sprintf(&b[i++][0],"    Status Conditioning: %s", (sta[14] & 0x10) ? "POWERED" : "NOT POWERED");
  sprintf(&b[i++][0],"    Pilot DV55:          %s", (sta[14] & 0x20) ? "OPENED" : "CLOSED");
  sprintf(&b[i++][0],"    Pilot DV57:          %s", (sta[14] & 0x40) ? "OPENED" : "CLOSED");

  sprintf(&b[i++][0],"  PS_A_STATUS      = %02hhX", sta[15]);
  sprintf(&b[i++][0],"    Moog A Group: %s", (sta[15] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Moog B Group: %s", (sta[15] & 0x02) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Moog C Group: %s", (sta[15] & 0x04) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Moog D Group: %s", (sta[15] & 0x08) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Moog DV20A:   %s", (sta[15] & 0x10) ? "OPENED" : "CLOSED");
  sprintf(&b[i++][0],"    Moog DV20B:   %s", (sta[15] & 0x20) ? "OPENED" : "CLOSED");
  sprintf(&b[i++][0],"    Moog DV20C:   %s", (sta[15] & 0x40) ? "OPENED" : "CLOSED");
  sprintf(&b[i++][0],"    Moog DV20D:   %s", (sta[15] & 0x80) ? "OPENED" : "CLOSED");

  sprintf(&b[i++][0],"  PS_B_STATUS      = %02hhX", sta[16]);
  sprintf(&b[i++][0],"    Moog Deco[0]: %s", (sta[16] & 0x01) ? "1" : "0");
  sprintf(&b[i++][0],"    Moog Deco[1]: %s", (sta[16] & 0x02) ? "1" : "0");
  sprintf(&b[i++][0],"    Moog Deco[2]: %s", (sta[16] & 0x04) ? "1" : "0");
  sprintf(&b[i++][0],"    Moog Deco[3]: %s", (sta[16] & 0x08) ? "1" : "0");
  sprintf(&b[i++][0],"    Warm Deco[0]: %s", (sta[16] & 0x10) ? "1" : "0");
  sprintf(&b[i++][0],"    Warm Deco[1]: %s", (sta[16] & 0x20) ? "1" : "0");
  sprintf(&b[i++][0],"    Warm Deco[2]: %s", (sta[16] & 0x40) ? "1" : "0");
  sprintf(&b[i++][0],"    Warm Deco[3]: %s", (sta[16] & 0x80) ? "1" : "0");

  sprintf(&b[i++][0],"  PS_C_STATUS      = %02hhX", sta[17]);
  sprintf(&b[i++][0],"    Warm ON Group:  %s", (sta[17] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Warm OFF Group: %s", (sta[17] & 0x02) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Warm DV09A:     %s", (sta[17] & 0x04) ? "CLOSED" : "NOT CLOSED");
  sprintf(&b[i++][0],"    Warm DV09B:     %s", (sta[17] & 0x08) ? "CLOSED" : "NOT CLOSED");
  sprintf(&b[i++][0],"    Warm DV11A:     %s", (sta[17] & 0x10) ? "CLOSED" : "NOT CLOSED");
  sprintf(&b[i++][0],"    Warm DV11B:     %s", (sta[17] & 0x20) ? "CLOSED" : "NOT CLOSED");
  sprintf(&b[i++][0],"    Warm DV16A:     %s", (sta[17] & 0x40) ? "CLOSED" : "NOT CLOSED");
  sprintf(&b[i++][0],"    Warm DV16B:     %s", (sta[17] & 0x80) ? "CLOSED" : "NOT CLOSED");

  sprintf(&b[i++][0],"  PS_D_STATUS      = %02hhX", sta[18]);
  sprintf(&b[i++][0],"    DLCM & Vacuum Group:   %s", (sta[18] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Vacuum Sensor:         %s", (sta[18] & 0x02) ? "ENERGIZED" : "NOT ENERGIZED");
  sprintf(&b[i++][0],"    Helium Tank Heater:    %s", (sta[18] & 0x04) ? "ENERGIZED" : "NOT ENERGIZED");
  sprintf(&b[i++][0],"    Pressure Sensor Group: %s", (sta[18] & 0x08) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    Pressure Sensor:       %s", (sta[18] & 0x10) ? "ENERGIZED" : "NOT ENERGIZED");
  sprintf(&b[i++][0],"    PCold Group:           %s", (sta[18] & 0x20) ? "ON" : "OFF");
  sprintf(&b[i++][0],"    uSW Energization:      %s", (sta[18] & 0x40) ? "ENERGIZED" : "NOT ENERGIZED");
  sprintf(&b[i++][0],"    PS_CV:                 %s", (sta[18] & 0x80) ? "ON" : "OFF");

//~--

  *n = i;
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, k, n, lmax;
  int16 err;
  int8 sta[56];
  
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

  for (k=0; k<2; k++) {
    CAB_get_digital_status(k, sta, &err);
    printf("err = %04X", err);
    if (err) {
      if (err > 0x2000) printf(" = %s", cab_ack_message(err-0x2000));
      printf("\n");
    }
    else {
      printf("\n");
      _sprint_CAB_digital_status(sta, &n, b[0]);
      for (i=0,lmax=0; i<n; i++) lmax = MAX(lmax, strlen(b[0][i]));
      printf("k = %d, n = %d, lmax = %d\n", k, n, lmax);
      for (i=0; i<n; i++) printf("%s\n", b[0][i]);
    }
  }
  
  return 0;
}

//~----------------------------------------------------------------------------
