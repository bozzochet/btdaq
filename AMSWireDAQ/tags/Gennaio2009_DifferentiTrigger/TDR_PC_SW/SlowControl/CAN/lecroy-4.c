// file lecroy-4.c
//
// Program to check LeCroy commands in UG Crate
//
// A.Lebedev Jul-2008...

#include "uscmlib.h"

int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x78;                                // global variable
int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable
int P = 0;                                         // global variable

static struct {
  bool  use;
  int32 wri32;
  int   cnt;
} cmd[6][8][0x20];

//~----------------------------------------------------------------------------

char *module_name(int8 bus, int16 brd) {

  int i;

  static struct {
    char *txt;
    int8  bus;
    int16 brd;
  } table[] = {
    {"UGBS-H", 0, 1},
    {"UGBS-C", 2, 1},
    {"UGBC-H", 0, 2},
    {"UGBC-C", 2, 2},
    {"UGFV-H", 1, 1},
    {"UGFV-C", 3, 1}};

  static char *error = {"XXXX-X"};

  for (i=0; i<DIM(table); i++) {
    if (table[i].bus == bus && table[i].brd == brd) return table[i].txt;
  }
  
  return error;
}

//~----------------------------------------------------------------------------

void scan_UG_crate(void) {

  int8 bus, BUS;
  int r_w;
  int16 brd, adr, ext, wri;
  int32 wri32, rea32, m;
  bool chk;
  int16 err;
  int p;

  for (bus=0; bus<4; bus++) {
    for (brd=1; brd<3; brd++) {
      for (adr=0x00; adr<0x20; adr++) {
        cmd[bus][brd][adr].use = FALSE;
        cmd[bus][brd][adr].cnt = 0;
        r_w = READ;
        ext = 0;
        wri = 0x0001;
        wri32 =        0x80000000;
        pack32(&wri32, 0x00080000, r_w);
        pack32(&wri32, 0x0F800000, swapbits(adr,  5));
        pack32(&wri32, 0x00700000, swapbits(brd,  3));
        pack32(&wri32, 0x00070000, swapbits(ext,  3));
        pack32(&wri32, 0x0000FFFF, swapbits(wri, 16));
        for (m=0x80000000,p=1; m; m>>=1) if (wri32 & m) p ^= 1;
        if (p) wri32 ^= 0x40000000;
        cmd[bus][brd][adr].wri32 = wri32;
        BUS = (bus << 4) | bus;
        exec_LeCroy_cmd(1, &BUS, &wri32, &rea32, &chk, &err);
        if (!err && !chk) {
          printf("bus=%d brd=%d adr=%02hX chk=%d wri=%08X rea=%08X\n", 
                  bus,   brd,   adr,      chk,   wri32,   rea32);
          cmd[bus][brd][adr].use = TRUE;
        }
      }
    }
  }

  printf("Scan completed\n");
}

//~============================================================================

int main(int argc, char *argv[]) {

  int8 bus, BUS;
  int16 brd, adr;
  int32 wri32, rea32;
  bool chk;
  int16 err;
  int cnt = 0;
  int opt_P = 0;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n", argv[0]);
  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);
  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  scan_UG_crate();
  
  for (bus=0; bus<4; bus++) {
    for (brd=1; brd<3; brd++) {
      for (adr=0x00; adr<0x20; adr++) {
        cmd[bus][brd][adr].use = TRUE;
      }
    }
  }
  
  while(1) {  
    for (bus=0; bus<4; bus++) {
      for (brd=1; brd<3; brd++) {
        for (adr=0x00; adr<0x20; adr++) {
          wri32 = cmd[bus][brd][adr].wri32;
          if (!cmd[bus][brd][adr].use) continue;
          BUS = (bus << 4) | bus;
          exec_LeCroy_cmd(1, &BUS, &wri32, &rea32, &chk, &err);
          if (!err && chk) {
            cmd[bus][brd][adr].cnt++;
            if (opt_P) {
              printf("bus=%d brd=%d adr=%02hX chk=%d wri=%08X rea=%08X\n", 
                      bus,   brd,   adr,      chk,   wri32,   rea32);
            }
          }
        }
      }
    }
    cnt++;
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
      case '?':
        printf("*** cnt = %d\n", cnt);
        for (bus=0; bus<4; bus++) {
          printf("bus %d:\n", bus);
          for (brd=1; brd<3; brd++) {
            if (strcasestr(module_name(bus, brd), "XXXX")) continue;
            printf("brd %d: %s ", brd, module_name(bus, brd));
            for (adr=0x00; adr<0x20; adr++) {
              if (!cmd[bus][brd][adr].use) printf("    ");
              else                         printf("%4d", cmd[bus][brd][adr].cnt);
            }
            printf("\n");
          }
        }
      break;
      case 'P':
        opt_P = 1 - opt_P;
      break;
      }
    }
  }
  
  return 0;
}

//~============================================================================
