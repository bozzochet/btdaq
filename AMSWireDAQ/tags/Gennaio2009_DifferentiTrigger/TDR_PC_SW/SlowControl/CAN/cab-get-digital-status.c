// file cab-get-digital-status.c
//
//  Get CAB Digital Status and print it.
//
//  A.Lebedev Jul - 2008...
//

#include "cablib.h"

int JMDC_ID = 0x03;                                // global variable
int USCM_ID = 0x18;                                // global variable
int P = 0;                                         // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

void print_CAB_digital_status(int8 *sta) {

  int i;

//~--

  printf("Digital Status:\n");
  for (i=0; i<19; i++) printf("%02hhX ", sta[i]);
  printf("\n");

//~--

  printf("  SW_CYCLE_COUNTER = %02hhX%02hhX%02hhX%02hhX (%.0fsec)\n", 
          sta[0], sta[1], sta[2], sta[3], 
          0.0125*(((sta[0]*256+sta[1])*256+sta[2])*256+sta[3]));

//~--

  printf("CSC_STATUS:\n");

  printf("  CSC_CONV_CMD_STATUS    = %02hhX\n", sta[4]);
  printf("                           Converter 1 Status: %s\n",
         (sta[4] & 0x01) ? "ON" : "OFF");
  printf("                           Converter 2 Status: %s\n",
         (sta[4] & 0x02) ? "ON" : "OFF");
  printf("                           Converter 3 Status: %s\n",
         (sta[4] & 0x04) ? "ON" : "OFF");
  printf("                           Converter 4 Status: %s\n",
         (sta[4] & 0x08) ? "ON" : "OFF");
  printf("                           Converter 5 Status: %s\n",
         (sta[4] & 0x10) ? "ON" : "OFF");
  printf("                           Converter 6 Status: %s\n",
         (sta[4] & 0x20) ? "ON" : "OFF");

  printf("  CURRENT_1_CMD_SETTINGS = %02hhX%02hhX\n", sta[ 5], sta[ 6]);
  printf("  CURRENT_2_CMD_SETTINGS = %02hhX%02hhX\n", sta[ 7], sta[ 8]);
  printf("  CURRENT_3_CMD_SETTINGS = %02hhX%02hhX\n", sta[ 9], sta[10]);

  printf("  CCS_COMMANDING_STATUS  = %02hhX ", sta[11]);
  if      (sta[11] == 0x5F) printf("(ARMED)\n");
  else if (sta[11] == 0x50) printf("(DISARMED)\n");
  else                      printf("(?)\n");

//~--

  printf("STM_STATUS:\n");

  printf("  STM_STATUS = %02hhX\n", sta[12]);
  printf("               COLD_VALVE_CMD_STS: %s\n",
         (sta[12] & 0x01) ? "ON" : "OFF");
  printf("               COLD_VALVE_SW_STS:  %s\n",
         (sta[12] & 0x02) ? "ON" : "OFF");

//~--

  printf("PTM_STATUS:\n");

  printf("  PTM_STATUS = %02hhX\n", sta[13]);
  printf("               ON_OFFSET_CAL2_CMD_STS: %s\n",
         (sta[13] & 0x01) ? "ON" : "OFF");
  printf("               ON_OFFSET_CAL3_CMD_STS: %s\n",
         (sta[13] & 0x02) ? "ON" : "OFF");

//~--

  printf("PS_STATUS:\n");

  printf("  PS_CMD_SW_STATUS = %02hhX\n", sta[14]);
  printf("                     28V_ISO_CMD:         %s\n",
         (sta[14] & 0x01) ? "ON" : "OFF");
  printf("                     Vacuum Sensor:       %s\n",
         (sta[14] & 0x02) ? "POWERED" : "NOT POWERED");
  printf("                     Helium Tank Heater:  %s\n",
         (sta[14] & 0x04) ? "POWERED" : "NOT POWERED");
  printf("                     Pressure Sensor:     %s\n",
         (sta[14] & 0x08) ? "POWERED" : "NOT POWERED");
  printf("                     Status Conditioning: %s\n",
         (sta[14] & 0x10) ? "POWERED" : "NOT POWERED");
  printf("                     Pilot DV55:          %s\n",
         (sta[14] & 0x20) ? "OPENED" : "CLOSED");
  printf("                     Pilot DV57:          %s\n",
         (sta[14] & 0x40) ? "OPENED" : "CLOSED");

  printf("  PS_A_STATUS      = %02hhX\n", sta[15]);
  printf("                     Moog A Group: %s\n",
         (sta[15] & 0x01) ? "ON" : "OFF");
  printf("                     Moog B Group: %s\n",
         (sta[15] & 0x02) ? "ON" : "OFF");
  printf("                     Moog C Group: %s\n",
         (sta[15] & 0x04) ? "ON" : "OFF");
  printf("                     Moog D Group: %s\n",
         (sta[15] & 0x08) ? "ON" : "OFF");
  printf("                     Moog DV20A:   %s\n",
         (sta[15] & 0x10) ? "OPENED" : "CLOSED");
  printf("                     Moog DV20B:   %s\n",
         (sta[15] & 0x20) ? "OPENED" : "CLOSED");
  printf("                     Moog DV20C:   %s\n",
         (sta[15] & 0x40) ? "OPENED" : "CLOSED");
  printf("                     Moog DV20D:   %s\n",
         (sta[15] & 0x80) ? "OPENED" : "CLOSED");

  printf("  PS_B_STATUS      = %02hhX\n", sta[16]);
  printf("                     Moog Deco[0]: %s\n",
         (sta[16] & 0x01) ? "1" : "0");
  printf("                     Moog Deco[1]: %s\n",
         (sta[16] & 0x02) ? "1" : "0");
  printf("                     Moog Deco[2]: %s\n",
         (sta[16] & 0x04) ? "1" : "0");
  printf("                     Moog Deco[3]: %s\n",
         (sta[16] & 0x08) ? "1" : "0");
  printf("                     Warm Deco[0]: %s\n",
         (sta[16] & 0x10) ? "1" : "0");
  printf("                     Warm Deco[1]: %s\n",
         (sta[16] & 0x20) ? "1" : "0");
  printf("                     Warm Deco[2]: %s\n",
         (sta[16] & 0x40) ? "1" : "0");
  printf("                     Warm Deco[3]: %s\n",
         (sta[16] & 0x80) ? "1" : "0");

  printf("  PS_C_STATUS      = %02hhX\n", sta[17]);
  printf("                     Warm ON Group:  %s\n",
         (sta[17] & 0x01) ? "ON" : "OFF");
  printf("                     Warm OFF Group: %s\n",
         (sta[17] & 0x02) ? "ON" : "OFF");
  printf("                     Warm DV09A:     %s\n",
         (sta[17] & 0x04) ? "CLOSED" : "NOT CLOSED");
  printf("                     Warm DV09B:     %s\n",
         (sta[17] & 0x08) ? "CLOSED" : "NOT CLOSED");
  printf("                     Warm DV11A:     %s\n",
         (sta[17] & 0x10) ? "CLOSED" : "NOT CLOSED");
  printf("                     Warm DV11B:     %s\n",
         (sta[17] & 0x20) ? "CLOSED" : "NOT CLOSED");
  printf("                     Warm DV16A:     %s\n",
         (sta[17] & 0x40) ? "CLOSED" : "NOT CLOSED");
  printf("                     Warm DV16B:     %s\n",
         (sta[17] & 0x80) ? "CLOSED" : "NOT CLOSED");

  printf("  PS_D_STATUS      = %02hhX\n", sta[18]);
  printf("                     DLCM & Vacuum Group:   %s\n",
         (sta[18] & 0x01) ? "ON" : "OFF");
  printf("                     Vacuum Sensor:         %s\n",
         (sta[18] & 0x02) ? "ENERGIZED" : "NOT ENERGIZED");
  printf("                     Helium Tank Heater:    %s\n",
         (sta[18] & 0x04) ? "ENERGIZED" : "NOT ENERGIZED");
  printf("                     Pressure Sensor Group: %s\n",
         (sta[18] & 0x08) ? "ON" : "OFF");
  printf("                     Pressure Sensor:       %s\n",
         (sta[18] & 0x10) ? "ENERGIZED" : "NOT ENERGIZED");
  printf("                     PCold Group:           %s\n",
         (sta[18] & 0x20) ? "ON" : "OFF");
  printf("                     uSW Energization:      %s\n",
         (sta[18] & 0x40) ? "ENERGIZED" : "NOT ENERGIZED");
  printf("                     PS_CV:                 %s\n",
         (sta[18] & 0x80) ? "ON" : "OFF");
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int16 err;
  int8 staN[56], staR[56];
  
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

  CAB_get_digital_status(0, staN, &err);
  printf("err = %04X", err);
  if (err) {
    if (err > 0x2000) printf(" = %s", cab_ack_message(err-0x2000));
    printf("\n");
  }
  else {
    printf("\n");
    print_CAB_digital_status(staN);
  }
  
  CAB_get_digital_status(1, staR, &err);
  printf("err = %04X", err);
  if (err) {
    if (err > 0x2000) printf(" = %s", cab_ack_message(err-0x2000));
    printf("\n");
  }
  else {
    printf("\n");
    print_CAB_digital_status(staR);
  }
  
  return 0;
}

//~----------------------------------------------------------------------------
