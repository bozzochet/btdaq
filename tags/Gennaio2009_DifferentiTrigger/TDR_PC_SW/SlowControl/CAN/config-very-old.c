// file config-very-old.c
//
// Configuration file working test bench (old and obsolete)
//
// A.Lebedev Aug-2006...
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x196;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int8  n[8];
int64 ID[8][64];
int8  pri[8][64];

int8 set;
int8 ena, rst, man, mdw, par;

bool was_scan;
bool was_reset;
int8 sca;

bool was_dac;
int16 dac[16];

int lvds_n, lvds_adr[256], lvds_dat[256];

char *filename;
FILE *file = NULL;

int16 err;

//~----------------------------------------------------------------------------

void open_config_file(void) {

  int n = 0x4000;
  static bool first = TRUE;
  int32 not_used = {0xCCCCCCCC};
  int32 begin    = {0x00038000};
  int32 end      = {0x0003800B};

  use_MEM = TRUE;

  if (first) {
    uscm_mem = malloc(n);
    first = FALSE;
  }
  memset(uscm_mem, 0xFF, n);
  
  unpackbytes(&uscm_mem[ 0], not_used, 4);
  unpackbytes(&uscm_mem[ 4], begin,    4);
  unpackbytes(&uscm_mem[ 8], end,      4);
}

//~----------------------------------------------------------------------------

void close_config_file(void) {

  int i;
  int32 begin;
  int32 end;
  int16 CKS;
  
  packbytes(&begin, &uscm_mem[4], 4);
  packbytes(&end,   &uscm_mem[8], 4);

  for (CKS=0, i=begin; i<=end; i++) CKS += (int16)uscm_mem[i-begin];
//printf("CKS = %04X\n", CKS);

  unpackbytes(&uscm_mem[end+1-begin], CKS, 2);

  use_MEM = FALSE;
}

//~----------------------------------------------------------------------------

void write_config_file(int16 *err) {

  int32 begin, end, addr;
  int16 size;
  
  packbytes(&begin, &uscm_mem[4], 4);
  packbytes(&end,   &uscm_mem[8], 4);

  size = end - begin + 3;
  addr = begin;
  if (P) printf("begin = 0x%06X, end = 0x%06X, size = %d\n", begin, end, size);
  while (size) {
    int n = MIN(1024, size);
    if (P) printf("write_USCM_memory(%4d, 0x%06X, ...\n", n, addr);
    write_USCM_memory(n, addr, &uscm_mem[addr-begin], err);
    if (*err) return;
    addr = addr + n;
    size = size - n;
  }
}

//~----------------------------------------------------------------------------

void erase_config_file(int16 *err) {

  int i;
  bool done;
  int sector = 7;

  boot_from_OTP(err);
  if (*err) {
    int i;
    for (i=0; i<3; i++) {
      ping_USCM(0, NULL, NULL, NULL, err);
      if (!*err) break;
    }
    if (P) printf("boot_from_OTP failed\n");
    return;
  }

  start_FLASH_erasure(sector, err);
  if (*err) {
    if (P) printf("start_FLASH_erasure failed\n");
    return;
  }
  if (P) printf("Waiting for erase sector %d ...", sector);
  for (i=0; i<50; i++) {
    usleep(100000);
    check_FLASH_erasure(&done, err);
    if (*err) {
      printf("\n check_FLASH_erasure failed\n");
      return;
    }
    if (done) break;
    if (P) printf(".");
  }
  if (!done) {
    if (P) printf("\nerasure of FLASH failed");
    return;
  }
  if (P) printf("\n");
}

//~============================================================================

void create_test_config_file(void) {

  int i;
  int8 echo[3] = {0x11, 0x22, 0x33};

  open_config_file();
  boot_USCM(3, &err);
  boot_USCM(4, &err);
  ping_USCM(3, echo, NULL, NULL, &err);
  close_config_file();
  
  for (i=0; i<30; i++) printf("%02hhX ", uscm_mem[i]);
  printf("\n");
}

//~----------------------------------------------------------------------------

void create_killer_config_file(void) {

#if 0
  int i;
  int8 echo[8192];

  for (i=0; i<8192; i++) echo[i] = i;
  open_config_file();
  for (i=0; i<2; i++) {
    ping_USCM(8182, echo, NULL, NULL, &err);
    if (err) {
      printf("ping_USCM failed, i = %d, err = 0x%04X\n", i, err);
      exit(1);
    }
  }
  close_config_file();
#endif

#if 1
  int i;
  int8 n;
  int64 ID[64];

  open_config_file();
  write_DS1820_table(1, 64, ID, NULL, &err);
  if (err) {
    printf("write_DS1820_table failed, err = 0x%04X\n", err);
    exit(1);
  }
  for (i=0; i<100; i++) {
    read_DS1820_table(1, &n, ID, NULL, &err);
    if (err) {
      printf("read_DS1820_table failed, i = %d, err = 0x%04X\n", i, err);
      exit(1);
    }
  }
  close_config_file();
#endif
}

//~============================================================================

bool convert_config_file(char *filename) {

#define DALLAS_BUS   1
#define DALLAS_SCAN  2
#define DALLAS_SETUP 3
#define DAC          4
#define LVDS         5

#define SETUP_ENABLE   1
#define SETUP_RESET    2
#define SETUP_MANUAL   3
#define SETUP_MODE     4
#define SETUP_PARALLEL 5

  int i, j;
  int swtc1 = 0, swtc2 = 0;
  
//~---

  if (!file) file = fopen(filename, "r");
  if (!file) {
    printf("File %s absent.\n", filename);
    return FALSE;
  }

  open_config_file();

  for (j=0; j<8; j++) {
    n[j] = -1;
    for (i=0; i<64; i++) {
      ID[j][i]  = 0;
      pri[j][i] = 0;
    }
  }

  set = 0x00;
  was_scan  = FALSE;
  was_reset = FALSE;
  
  was_dac = FALSE;
  for (i=0; i<16; i++) dac[i] = 0x8000;

//~------

  while (1) {
    char b[8192];
    int k;
    char key[80];
    int bu;
    static int bus = 100000;
    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-1] = '\0';
    if (P) printf(b);

    k = sscanf(b, "Dallas %s %d", key, &bu);
    if (k == 2) {
      if (!strncmp(key, "Bus", 3)) {
        if (bu > 0 && bu < 9) {
          bus = bu;
          n[bus-1] = 0;
          if (P) printf(":: OK, switched to Dallas Bus %d\n", bus);
          swtc1 = DALLAS_BUS;
          continue;
        }
        else {
          if (P) printf(":: bus %d is invalid\n", bu);
          swtc1 = 0;
          continue;
        }
      }
    }

    k = sscanf(b, "Dallas %s", key);
    if (k == 1) {
      if (!strncmp(key, "Scan", 5)) {
        if (P) printf(":: OK, switched to Dallas Scan\n");
        swtc1 = DALLAS_SCAN;
        continue;
      }
      if (!strncmp(key, "Setup", 5)) {
        if (P) printf(":: OK, switched to Dallas Setup\n");
        swtc1 = DALLAS_SETUP;
        continue;
      }
    }

    k = sscanf(b, "DAC %s", key);
    if (k == 1) {
      if (!strncmp(key, "Data", 4)) {
        if (P) printf(":: OK, switched to DAC data\n");
        swtc1 = DAC;
        continue;
      }
    }

    k = sscanf(b, "LVDS Adr %s", key);
    if (k == 1) {
      if (!strncmp(key, "Data", 4)) {
        if (P) printf(":: OK, switched to LVDS\n");
        swtc1 = LVDS;
        continue;
      }
    }

    k = sscanf(b, "Reset %s", key);
    if (k == 1) {
      if (!strncmp(key, "CPU", 4)) {
        if (P) printf(":: OK\n");
        was_reset = TRUE;
        swtc1 = 0;
        continue;
      }
    }

//~---

    switch (swtc1) {
      int   i;
      int64 id;
      int  prio;
      int16 cod;
      char bit[8];
      int8 byte, para;

      case DALLAS_BUS:
        k = sscanf(b, "%d %16llX %d", &i, &id, &prio);
        if (k == 3) {
          if      (i < 0 || i > 63) {
            if (P) printf(":: number %d is invalid\n", i);
          }
          else if (!DOWCRC(id)) {
            if (P) printf(":: ID CRC error\n");
          }
          else if (prio < 0 || prio > 9) {
            if (P) printf(":: priority %d is invalid\n", prio);
          }
          else {
            n[bus-1]      = MAX(n[bus-1], i+1);
            ID[bus-1][i]  = id;
            pri[bus-1][i] = prio;
            if (P) printf(":: OK\n");
          }
        }
        else {
          if (P) printf("\n");
        }
      break;

      case DAC:
        k = sscanf(b, "%d %hX", &i, &cod);
        if (k == 2) {
          if      (i < 0 || i > 15) {
            if (P) printf(":: channel %d is invalid\n", i);
          }
          else if (cod > 0xFFF) {
            if (P) printf(":: code %X is invalid\n", cod);
          }
          else {
            was_dac = TRUE;
            dac[i] = cod;
            if (P) printf(":: OK\n");
          }
        }
        else {
          if (P) printf("\n");
        }
      break;

      case DALLAS_SCAN:
        k = sscanf(b, "Scan %c %c %c %c %c %c %c %c",
            &bit[7], &bit[6], &bit[5], &bit[4],
            &bit[3], &bit[2], &bit[1], &bit[0]);
        if (k == 8) {
          for (byte=0x00, i=0; i<8; i++) {
            if (bit[i] == '1') byte = byte | (1 << i);
          }
          was_scan = TRUE;
          sca = byte;
          if (P) printf(":: OK\n");
        }
        else {
          if (P) printf("\n");
        }
      break;

      case DALLAS_SETUP:
        swtc2 = 0;
        k = sscanf(b, "Enable %c %c %c %c %c %c %c %c",
            &bit[7], &bit[6], &bit[5], &bit[4],
            &bit[3], &bit[2], &bit[1], &bit[0]);
        if (k == 8) swtc2 = SETUP_ENABLE;
        k = sscanf(b, "Reset %c %c %c %c %c %c %c %c",
            &bit[7], &bit[6], &bit[5], &bit[4],
            &bit[3], &bit[2], &bit[1], &bit[0]);
        if (k == 8) swtc2 = SETUP_RESET;
        k = sscanf(b, "Manual %c %c %c %c %c %c %c %c",
            &bit[7], &bit[6], &bit[5], &bit[4],
            &bit[3], &bit[2], &bit[1], &bit[0]);
        if (k == 8) swtc2 = SETUP_MANUAL;
        k = sscanf(b, "Mode %c %c %c %c %c %c %c %c",
            &bit[7], &bit[6], &bit[5], &bit[4],
            &bit[3], &bit[2], &bit[1], &bit[0]);
        if (k == 8) swtc2 = SETUP_MODE;
        k = sscanf(b, "Parallel %hhd", &para);
        if (k == 1) swtc2 = SETUP_PARALLEL;
        if (swtc2) {
          for (byte=0x00, i=0; i<8; i++) {
            if (bit[i] == '1' || bit[i] == 'P') byte = byte | (1 << i);
          }
          switch (swtc2) {
            case SETUP_ENABLE:
              ena = byte;
              set |= 0x01;
              if (P) printf(":: OK\n");
            break;
            case SETUP_RESET:
              rst = byte;
              set |= 0x02;
              if (P) printf(":: OK\n");
            break;
            case SETUP_MANUAL:
              man = byte;
              set |= 0x04;
              if (P) printf(":: OK\n");
            break;
            case SETUP_MODE:
              mdw = byte;
              set |= 0x08;
              if (P) printf(":: OK\n");
            break;
            case SETUP_PARALLEL:
              if (para <= 64) {
                par = para;
                set |= 0x10;
                if (P) printf(":: OK\n");
              }
              else {
                if (P) printf(":: number is invalid\n");
              }
            break;
            default:
              if (P) printf("\n");
            break;
          }
        }
        else {
          if (P) printf("\n");
        }
      break;
      default:
        if (P) printf("\n");
      break;
    }
  }

//~------

  for (i=0; i<8; i++) {
    if (n[i] != 0xFF) {
      write_DS1820_table(i+1, n[i], &ID[i][0], &pri[i][0], &err);
      if (!err) {
        if (P) printf("-- Bus %d writing completed OK\n", i+1);
      }
      else {
        if (P) printf("-- Bus %d writing failed\n", i+1);
        break;
      }
    }
  }
  
  if (set == 0x1F) {
    setup_DS1820_readout(ena, rst, man, mdw, par, &err);
    if (!err) {
      if (P) printf("-- Setup writing completed OK\n");
    }
    else {
      if (P) printf("-- Setup writing failed\n");
    }
  }
  
  if (was_scan) {
    start_DS1820_scan(sca, &err);
    if (!err) {
      if (P) printf("-- Scan writing completed OK\n");
    }
    else {
      if (P) printf("-- Scan writing failed\n");
    }
  }

  if (was_dac) {
    write_DAC(dac, &err);
    if (!err) {
      if (P) printf("-- DAC writing completed OK\n");
    }
    else {
      if (P) printf("-- DAC writing failed\n");
    }
  }

  if (was_reset) {
    boot_USCM(0, &err);
    if (!err) {
      if (P) printf("-- Reset CPU writing completed OK\n");
    }
    else {
      if (P) printf("-- Reset CPU writing failed\n");
    }
  }
  
//~------

  close_config_file();

  return TRUE;
}

//~============================================================================

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage:\n");
  printf("%s <file-name> [<USCM_ID> [<JMDC_ID> [P]]]]\n",argv[0]);

  if (argc < 2) exit(1);
  filename = argv[1];
  if (argc > 2) USCM_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) JMDC_ID = atoi(argv[3]);
  if (argc > 4) P       = atoi(argv[4]);
  
  printf("USCM_ID = %02X\n", USCM_ID);
  printf("JMDC_ID = %d\n",   JMDC_ID);
  printf("P       = %d\n",   P);

#if 0
  create_test_config_file();
#endif

#if 0
  create_killer_config_file();
#endif

#if 1
  if (!convert_config_file(filename)) {
    printf("convert_config_file failed\n");
    exit(1);
  }
#endif

  if (!initialize_EPP()) exit(1);

  erase_config_file(&err);
  if (err) {
    printf("erase_config_file failed\n");
    exit(1);
  }
  write_config_file(&err);
  if (err) {
    printf("write_config_file failed\n");
    exit(1);
  }

  printf("Config file %s successfully written into USCM %X\n",
         filename, USCM_ID);
  return 0;
}

//~============================================================================

#if this_is_a_test_file // see also test-01.conf and many *.conf files

Dallas Bus 5
NB ---Dallas ID ---  P Comments
 0 A7000800AA9D6010  1 LDDR #0 Laser 0
 1 1C0008004C26B010  2 LDDR #0 Laser 1
 2 35000800AAACD410  3 LDDR #1 Laser 0
 3 60000800AB040A10  4 LDDR #1 Laser 1
 4 DB0008004C097A10  5 LDDR #2 Laser 0
 5 FE0008004C2C8610  6 LDDR #2 Laser 1
 6 C5000800AA939610  7 LDDR #3 Laser 0
 7 95000800AAB50110  8 LDDR #3 Laser 1
 8 D3000800504E1910  9 LDDR #4 Laser 0
 9 18000800AAB03710 10 LDDR #4 Laser 1
69 18000800AAB03710  9 LDDR #4 Laser 1
10 18000800AAB03711  9 LDDR #4 Laser 1

Dallas Bus 5
NB ---Dallas ID ---  P Comments
 0 A7000800AA9D6010  1 LDDR #0 Laser 0
 1 1C0008004C26B010  2 LDDR #0 Laser 1
 2 35000800AAACD410  3 LDDR #1 Laser 0
 3 60000800AB040A10  4 LDDR #1 Laser 1
 4 DB0008004C097A10  5 LDDR #2 Laser 0
Dallas Bus 15
 5 FE0008004C2C8610  6 LDDR #2 Laser 1
 6 C5000800AA939610  7 LDDR #3 Laser 0
 7 95000800AAB50110  8 LDDR #3 Laser 1
 8 D3000800504E1910  9 LDDR #4 Laser 0
 9 18000800AAB03710 10 LDDR #4 Laser 1
69 18000800AAB03710  9 LDDR #4 Laser 1
10 18000800AAB03711  9 LDDR #4 Laser 1

Dallas Scan 8 7 6 5 4 3 2 1
Scan        1 1 1 0 0 0 0 0

Dallas Setup 8 7 6 5 4 3 2 1
Enable       1 0 0 0 0 0 0 0
Reset        1 1 1 1 1 1 1 1
Manual       1 0 0 0 0 0 0 0
Mode         P N N N N N N N
Parallel 32

#endif

//~============================================================================
