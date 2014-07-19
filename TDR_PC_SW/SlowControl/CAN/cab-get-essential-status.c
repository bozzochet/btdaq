// file cab-get-essential-status.c
//
//  Get CAB Essential Status and print it.
//
//  A.Lebedev Jul - 2008...
//

#include "cablib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x18;    // cab-slave program does not recognize Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

void print_CAB_essential_status(int8 *sta) {

  int i;

//~--

  printf("Essential Status:\n");
  for (i=0; i<56; i++) printf("%02hhX ", sta[i]);
  printf("\n");

//~--

  printf("SW_STATUS:\n");

  printf("  CPU_ID               = %02hhX ", sta[0]);
  if      (sta[0] == 0x01) printf("(Nominal CPU)\n");
  else if (sta[0] == 0x02) printf("(Redundant CPU)\n");
  else                     printf("(?)\n");

  printf("  RESET_CAUSE          = %02hhX ", sta[1]);
  if      (sta[1] == 0x01) printf("(Power_ON_Reset)\n");
  else if (sta[1] == 0x02) printf("(Watchdog_Reset)\n");
  else if (sta[1] == 0x04) printf("(Latch_UP_Reset)\n");
  else if (sta[1] == 0x08) printf("(Commanded_JMDC_Reset (to Start-up))\n");
  else if (sta[1] == 0x10) printf("(Invalid_Memory_Access_Reset)\n");
  else if (sta[1] == 0x20) printf("(Commanded_JMDC_Reset (to Active))\n");
  else                     printf("(?)\n");

  printf("  SW_STATE_CURRENT_CPU = %02hhX ", sta[2]);
  if      (sta[2] == 0x01) printf("(Start-Up)\n");
  else if (sta[2] == 0x02) printf("(Active)\n");
  else                     printf("(?)\n");

  printf("  SW_STATE_OTHER_CPU   = %02hhX ", sta[3]);
  if      (sta[3] == 0x01) printf("(Start-Up)\n");
  else if (sta[3] == 0x02) printf("(Active)\n");
  else                     printf("(?)\n");

  printf("  SW_SEQ_IN_COURSE     = %02hhX (", sta[4]);
  if (sta[4] & 0x01) printf("PTM_Acquisition ");
  if (sta[4] & 0x02) printf("DLCM_Acquisition ");
  if (sta[4] & 0x04) printf("Current_SetPoint_Programming ");
  if (sta[4] & 0x08) printf("Arm_Quench ");
  if (sta[4] & 0x10) printf("Current_Lead_SEL_Clear ");
  if (sta[4] & 0x20) printf("Magnet_Detector_SEL_Clear ");
  if (sta[4] & 0xC0) printf("? ");
  printf(")\n");

  printf("  SUSW_VERSION         = %02hhX\n", sta[5]);
  printf("  APSW_VERSION         = %02hhX\n", sta[6]);

  printf("  SW_CYCLE_COUNTER     = %02hhX%02hhX%02hhX%02hhX (%.0fsec)\n", 
          sta[7], sta[8], sta[9], sta[10], 
          0.0125*(((sta[7]*256+sta[8])*256+sta[9])*256+sta[10]));

//~--

  printf("ERROR_INFORMATION:\n");

  printf("  ERROR_COUNTER          = %02hhX%02hhX\n", sta[11], sta[12]);

  printf("  LAST_ENTRY_ERROR_TABLE = %02hhX %02hhX%02hhX%02hhX%02hhX %02hhX%02hhX ", 
          sta[13], sta[14], sta[15], sta[16], sta[17], sta[18], sta[19]);
  printf("(%s)\n", cab_err_message(sta[13]));

  printf("  CMD_REJECTED_COUNTER   = %02hhX%02hhX\n", sta[20], sta[21]);

  printf("  CAN_0_STATUS           = %02hhX\n", sta[22]);
  printf("  CAN_0_TX_STATUS        = %02hhX\n", sta[23]);
  printf("  CAN_0_RX_STATUS        = %02hhX\n", sta[24]);

  printf("  CAN_1_STATUS           = %02hhX\n", sta[25]);
  printf("  CAN_1_TX_STATUS        = %02hhX\n", sta[26]);
  printf("  CAN_1_RX_STATUS        = %02hhX\n", sta[27]);

//~--

  printf("START_UP_REPORT:\n");

  printf("  PROM_CRC_EXP          = %02hhX%02hhX\n", sta[28], sta[29]);
  printf("  PROM_CRC_CALC         = %02hhX%02hhX\n", sta[30], sta[31]);
  printf("  SW_PROGRAM_1_CRC_EXP  = %02hhX%02hhX\n", sta[32], sta[33]);
  printf("  SW_PROGRAM_1_CRC_CALC = %02hhX%02hhX\n", sta[34], sta[35]);
  printf("  SW_PROGRAM_2_CRC_EXP  = %02hhX%02hhX\n", sta[36], sta[37]);
  printf("  SW_PROGRAM_2_CRC_CALC = %02hhX%02hhX\n", sta[38], sta[39]);
  printf("  SW_TABLE_CRC_EXP      = %02hhX%02hhX\n", sta[40], sta[41]);
  printf("  SW_TABLE_CRC_CALC     = %02hhX%02hhX\n", sta[42], sta[43]);
  printf("  PROGRAM_RAM_CRC_CALC  = %02hhX%02hhX\n", sta[44], sta[45]);

  printf("  RAM_TEST              = %02hhX\n", sta[46]);
  printf("                          Data RAM result:         %s\n",
         (sta[46] & 0x01) ? "OK" : "Error");
  printf("                          Program Data RAM result: %s\n",
         (sta[46] & 0x02) ? "OK" : "Error");

//~--

  printf("CCSC_ESSENTIAL_STATUS:\n");

  printf("  CCSC_OTHER_DCDC_STATUS    = %02hhX ", sta[47]);
  if      (sta[47] == 0x5F) printf("(Converter is ON)\n");
  else if (sta[47] == 0x50) printf("(Converter is OFF)\n");
  else                      printf("(?)\n");

  printf("  LATCHUP_PROTECTION_STATUS = %02hhX ", sta[48]);
  if      (sta[48] == 0x5F) printf("(ENABLED)\n");
  else if (sta[48] == 0x50) printf("(DISABLED)\n");
  else                      printf("(?)\n");

//~--

  printf("PS_ESSENTIAL_STATUS:\n");

  printf("  PS_NCV_STATUS = %02hhX ", sta[49]);
  if      (sta[49] == 0x5F) printf("(OPEN)\n");
  else if (sta[49] == 0x50) printf("(CLOSED)\n");
  else                      printf("(?)\n");

  printf("  PS_RCV_STATUS = %02hhX ", sta[50]);
  if      (sta[50] == 0x5F) printf("(OPEN)\n");
  else if (sta[50] == 0x50) printf("(CLOSED)\n");
  else                      printf("(?)\n");

//~--

  printf("CSP_ESSENTIAL_STATUS:\n");

  printf("  CSP_CV_STATUS  = %02hhX\n", sta[51]);
  printf("                   CSP Magnet Detector Converter 1 Status: %s\n",
         (sta[51] & 0x01) ? "ON" : "OFF");
  printf("                   CSP Magnet Detector Converter 2 Status: %s\n",
         (sta[51] & 0x02) ? "ON" : "OFF");
  printf("                   CSP Magnet Detector Converter 3 Status: %s\n",
         (sta[51] & 0x04) ? "ON" : "OFF");
  printf("                   CSP Current Leads Converter 1 Status:   %s\n",
         (sta[51] & 0x08) ? "ON" : "OFF");
  printf("                   CSP Current Leads Converter 2 Status:   %s\n",
         (sta[51] & 0x10) ? "ON" : "OFF");
  printf("                   CSP Current Leads Converter 3 Status:   %s\n",
         (sta[51] & 0x20) ? "ON" : "OFF");
  printf("                   CSP Nominal Control Converter Status:   %s\n",
         (sta[51] & 0x40) ? "ON" : "OFF");
  printf("                   CSP Redundant Control Converter Status: %s\n",
         (sta[51] & 0x80) ? "ON" : "OFF");

  printf("  CSP_28V_STATUS = %02hhX\n",  sta[52]);
  printf("                   28V ISO LCL Nominal Battery Charger Status:   %s\n",
         (sta[52] & 0x01) ? "ON" : "OFF");
  printf("                   28V ISO LCL Redundant Battery Charger Status: %s\n",
         (sta[52] & 0x02) ? "ON" : "OFF");

  printf("  CSP_ALARM      = %02hhX ", sta[53]);
  if      (sta[53] == 0xFF) printf("(ALARM DETECTED)\n");
  else if (sta[53] == 0x00) printf("(NO ALARM DETECTED)\n");
  else                      printf("(?)\n");

//~--

  printf("CCS_ESSENTIAL_STATUS:\n");

  printf("  CCS_SEMIC_SW_STATUS = %02hhX ", sta[54]);
  if      (sta[54] == 0x5F) printf("(OPEN)\n");
  else if (sta[54] == 0x50) printf("(CLOSED)\n");
  else                      printf("(?)\n");

  printf("  CCS_OUTPUT_ENABLE   = %02hhX ", sta[55]);
  if      (sta[55] == 0x5F) printf("(ENABLED)\n");
  else if (sta[55] == 0x50) printf("(DISABLED)\n");
  else                      printf("(?)\n");

}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

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
  
  CAB_get_essential_status(sta, &err);
  printf("err = %04X", err);
  if (err) {
    if (err > 0x2000) printf(" = %s", cab_ack_message(err-0x2000));
    printf("\n");
  }
  else {
    printf("\n");
    print_CAB_essential_status(sta);
  }
  return 0;
}

//~----------------------------------------------------------------------------
