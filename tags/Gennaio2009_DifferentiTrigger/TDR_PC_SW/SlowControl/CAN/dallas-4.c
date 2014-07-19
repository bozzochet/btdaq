// file dallas-4.c
//
// Program to read and print full DS1820 information.
//
// A.Lebedev - Aug-2006...

#include "uscmlib.h"

int P = 0;                                     // global variable
int USCM_ID = 0x15f;                           // global variable
int JMDC_ID = 0x00;                            // global variable

int BUS_FOR_REQUEST = 0;                       // global variable
int BUS_FOR_REPLY = 0;                         // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int8 ns[9], ni[9], nr[9];
_DS1820_data dat[9][64];

//~----------------------------------------------------------------------------
//
// "Old" DS1820 temperature conversion (stolen from SXu and reworked)
// (for family code 0x10 only)
//
// was proven to be wrong after CERN climate chamber tests
//

void convert_1(int8 lsb, int8 msb, int8 remain, int8 perc, 
               int8 *temp1, int16 *temp2) {

  short t1, t2;
  
  *temp1 = 0x80;
  *temp2 = 0x8000;

  if ((msb == 0x00) && (lsb == 0xAA)) return;  // power on
  if (perc == 0) return;
    
  if (msb) {
    if (lsb) {
      t2 = (128 * (lsb - 256)) + (192 - 256 * remain / perc);
      t1 = ((t2 % 128) > 64) ? (t2 / 128 + 1) : (t2 / 128);
      *temp1 = (t1 < (short)0xFF80) ? 0x80 : t1;
      *temp2 = t2;
    }
  }
  else {
    if (lsb == 0xFF) {
      *temp1 = 0x80;
      *temp2 = 0x7FFF;
    }
    else {
      t2 = (128 * lsb) + (192 - 256 * remain / perc);
      t1 = ((t2 % 128) > 64) ? (t2 / 128 + 1) : (t2 / 128);
      *temp1 = (t1 > (short)0x007F) ? 0x80 : t1;
      *temp2 = t2;
    }
  }
}

//~----------------------------------------------------------------------------
//
// "New" DS1820 temperature conversion (stolen from SXu and reworked)
// (for family code 0x10 only)
//
// was proven to be "correct" after CERN climate chamber tests
//

void convert_2(int8 lsb, int8 msb, int8 remain, int8 perc, 
               int8 *temp1, int16 *temp2) {

  short t1, t2;
  
  *temp1 = 0x80;
  *temp2 = 0x8000;

  lsb = lsb & 0xFE;
  
  if ((msb == 0x00) && (lsb == 0xAA)) return;  // power on
  if (perc == 0) return;
    
  if (msb) {
    if (lsb) {
      t2 = (128 * (lsb - 256)) + (192 - 256 * remain / perc);
      t1 = ((t2 % 128) > 64) ? (t2 / 128 + 1) : (t2 / 128);
      *temp1 = (t1 < (short)0xFF80) ? 0x80 : t1;
      *temp2 = t2;
    }
  }
  else {
    if (lsb == 0xFF) {
      *temp1 = 0x80;
      *temp2 = 0x7FFF;
    }
    else {
      t2 = (128 * lsb) + (192 - 256 * remain / perc);
      t1 = ((t2 % 128) > 64) ? (t2 / 128 + 1) : (t2 / 128);
      *temp1 = (t1 > (short)0x007F) ? 0x80 : t1;
      *temp2 = t2;
    }
  }
}

//~----------------------------------------------------------------------------
//
// DS1820 temperature conversion from Dallas/Maxim documents (as is)
//

void convert_3(int8 lsb, int8 msb, int8 remain, int8 perc, 
               int8 *temp1, int16 *temp2) {

  *temp1 = 0x80;
  *temp2 = 0x8000;

  if ((msb && lsb > 0x80) || (!msb && lsb <= 0x7F)) *temp1 = lsb;
  *temp2 = (int16)msb             << 15 | 
           (int16)(lsb & 0xFE)    <<  7 | 
           (int16)(perc - remain) <<  4;
}

//~----------------------------------------------------------------------------
//
// DS1820 temperature conversion from Dallas/Maxim documents (well, equivalent)
//

void convert_4(int8 lsb, int8 msb, int8 remain, int8 perc, 
               int8 *temp1, int16 *temp2) {

  *temp1 = 0x80;
  *temp2 = 0x8000;

  if ((msb && lsb > 0x80) || (!msb && lsb <= 0x7F)) *temp1 = lsb;
  *temp2 = (int16)msb                      << 15 | 
           (int16)lsb                      <<  7 | 
           (int16)((0x10 - remain) & 0x07) <<  4;
}

//~--------------------------------------------------------------------

void reboot_USCM_from_flash(void) {

  int i;
  int16 err;

  printf("Reboot started...\n");
  boot_from_FLASH(&err);
  for (i=0; i<3; i++) {
    if (err) {
      ping_USCM(0, NULL, NULL, NULL, &err);
    }
    else {
      printf("Reboot completed.\n");
      return;
    }
  }
  PANIC("reboot_USCM_from_flash FAILED\n");
}

//~----------------------------------------------------------------------------

void find_Dallas_sensors(void) {

  int i;
  int8 bus = 0xFF;
  int8 bus_old;
  int16 err;

  start_DS1820_scan(0xFF, &err);
  if (err) PANIC("find_Dallas_sensor_ID FAILED (1)\n");

  printf("Scan started...\n");

  bus_old = 0xFF;
  while (bus) {
    sleep(3);
    check_DS1820_scan(&bus, ns, &err);
    if (err) PANIC("find_Dallas_sensor_ID FAILED (2)\n");
    if (bus == bus_old) PANIC("find_Dallas_sensor_ID FAILED (3)\n");
    bus_old = bus;
  }

  for (i=0; i<8; i++) {
    ni[i] = 0;
    if (ns[i]) read_DS1820_table(i+1, &ni[i], NULL, NULL, &err);
    if (err) PANIC("find_Dallas_sensor_ID FAILED (4)\n");
    if (ns[i] != ni[i]) PANIC("find_Dallas_sensor_ID FAILED (5)\n");
  }
  
  printf("Scan completed:\n");
  printf("Bus #            ");
  for (i=0; i<8; i++) printf(" %2d",    7-i+1);
  printf("\n");
  printf("Sensors found:   ");
  for (i=0; i<8; i++) printf(" %2d", ns[7-i]);
  printf("\n");
  printf("Sensors to read: ");
  for (i=0; i<8; i++) printf(" %2d", ni[7-i]);
  printf("\n");
}

//~----------------------------------------------------------------------------

void start_Dallas_sensors(void) {

  int i;
  int8 ena, rst, man, mdw, par;
  int16 err;

  ena = 0x00;
  for (i=0; i<8; i++) if (ni[i]) ena |= 1 << i;
  rst = 0xFF;
//  man = 0xFF;
  man = 0x00;
  mdw = 0x00;
  par = 1;

  if (!ena) PANIC("No sensors to read!\n");
  
  setup_DS1820_readout(ena, rst, man, mdw, par, &err);
  if (err) PANIC("start_Dallas_sensors FAILED\n");
  printf("Readout started...\n");
}

//~----------------------------------------------------------------------------

void read_Dallas_sensors(void) {

  int i;
  int16 err;

  for (i=0; i<8; i++) {
    if (ni[i]) {
      read_DS1820_temp_raw(i+1, &nr[i], &dat[i][0], &err);
      if (err) PANIC("read_Dallas_sensors FAILED\n");
    }
  }
}

//~----------------------------------------------------------------------------

void print_Dallas_information() {

  int i, j;
  int8 temp1;
  int16 temp2;
  bool ok1, ok2;
  
  for (j=0; j<8; j++) {
    if (nr[j]) {
      printf("%s Bus %d: Nb LSB MSB REM PER TMP1 TMP2 short    long\n",
              timestamp(1), j+1);
      for (i=0; i<nr[j]; i++) {
        _DS1820_data *p = &dat[j][i];
        printf("                       ");
        printf("     %2d", i);
        printf("  %02hhX",  p->lsb);
        printf("  %02hhX",  p->msb);
        printf("  %02hhX",  p->remain);
        printf("  %02hhX",  p->perc);
        printf("   %02hhX", p->temp1);
        printf(" %04X",     p->temp2);
        if (p->ok_short) printf(" %+5.1f", p->temp_short);
        else             printf("      ");
        if (p->ok_long) printf(" %+7.2f",  p->temp_long);
        else            printf("        ");
        convert_3(p->lsb, p->msb, p->remain, p->perc, &temp1, &temp2);
        printf("   %02hhX", temp1);
        printf(" %04X",     temp2);
        ok1 = (temp1 == 0x80)   ? FALSE : TRUE;
        ok2 = (temp2 == 0x8000) ? FALSE : TRUE;
        if (ok1) printf(" %+5.1f", (float)(char)temp1/2.0);
        else     printf("      ");
        if (ok2) printf(" %+7.2f", (float)(short)temp2/256.0-0.25);
        else     printf("        ");
        if ((p->ok_short && ok1 && (p->temp1 != temp1)) ||
            (p->ok_long  && ok2 && (p->temp2 != temp2))) printf("  X");
        printf("\n");
      }
      delay(2.0);
    }
  }
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID     = 0x%02X\n", USCM_ID);
  printf("JMDC ID     = %d\n", JMDC_ID);
  printf("P           = %d\n", P);

  if (!initialize_EPP()) exit(1);

  reboot_USCM_from_flash();
  find_Dallas_sensors();
  start_Dallas_sensors();
  
  while (1) {
    read_Dallas_sensors();
    print_Dallas_information();
  }

  return 0;
}

//~============================================================================
