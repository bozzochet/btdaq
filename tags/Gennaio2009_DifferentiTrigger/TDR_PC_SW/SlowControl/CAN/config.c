// file config.c
//
// Universal program to write config file into USCM
//
// A.Lebedev Jul-2008...

#include "ccebdef.h"
#include "ccebtasklib.h"

int JMDC_ID = 0x03;     // global variable
int USCM_ID = 0x1C;     // global variable
int P = 0;              // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

bool success = TRUE;
int16 err;
 
//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void (*parser)(int action, char *b, char *msg);

static void USCM_parser(int action, char *b, char *msg);
static void GPS_parser(int action, char *b, char *msg);
static void CC_parser(int action, char *b, char *msg);

struct {
  char *name;
  void (*parser)(int action, char *b, char *msg);
} System[] = {
  {"USCM::", USCM_parser},
  {"GPS::",  GPS_parser},
  {"CC::",   CC_parser}};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void open_config_file(void);
static void close_config_file(void);
static void erase_config_file(int16 *err);
static void write_config_file(int16 *err);

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

//~============================================================================

void erase_config_file(int16 *err) {

  int i;
  bool done;
  int sector = 7;

  boot_from_OTP(err);
  if (*err) {
    printf("*** Error: boot_from_OTP failed\n");
    return;
  }

  start_FLASH_erasure(sector, err);
  if (*err) {
    printf("*** Error: start_FLASH_erasure failed\n");
    return;
  }

  for (i=0; i<50; i++) {
    usleep(100000);
    check_FLASH_erasure(&done, err);
    if (*err) {
      printf("*** Error: check_FLASH_erasure failed\n");
      return;
    }
    if (done) break;
  }

  if (!done) printf("*** Error: erasure of FLASH failed\n");
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
    if (*err) {
      printf("*** Error: write_USCM_memory failed\n");
      return;
    }
    addr = addr + n;
    size = size - n;
  }
}

//~============================================================================

void USCM_parser(int action, char *b, char *msg) {

#define IDLE                0
#define WRITE_DALLAS_ID     1
#define SETUP_DALLAS        2
#define SCAN_DALLAS         3
#define WRITE_DAC           4
#define WRITE_LVDS          5
#define RESET_CPU           6
#define OPEN_BACKDOOR       7
#define CLOSE_BACKDOOR      8
#define RESET_STATUS        9
#define WRITE_MEMORY       10
#define SETUP_SERIAL_PORT  11
#define WRITE_SERIAL_PORT  12
#define INIT_LECROY_BUS    13
#define EXEC_LECROY_BUS    14
#define EXEC_WRITE_COMMAND 15

  struct {
    char *name;
    int  swtch;
    int  param;
  } Command[] = {
  {"WRITE_DALLAS_ID_BUS_1:", WRITE_DALLAS_ID,     1},
  {"WRITE_DALLAS_ID_BUS_2:", WRITE_DALLAS_ID,     2},
  {"WRITE_DALLAS_ID_BUS_3:", WRITE_DALLAS_ID,     3},
  {"WRITE_DALLAS_ID_BUS_4:", WRITE_DALLAS_ID,     4},
  {"WRITE_DALLAS_ID_BUS_5:", WRITE_DALLAS_ID,     5},
  {"WRITE_DALLAS_ID_BUS_6:", WRITE_DALLAS_ID,     6},
  {"WRITE_DALLAS_ID_BUS_7:", WRITE_DALLAS_ID,     7},
  {"WRITE_DALLAS_ID_BUS_8:", WRITE_DALLAS_ID,     8},
  {"SETUP_DALLAS:",          SETUP_DALLAS,       -1},
  {"SCAN_DALLAS:",           SCAN_DALLAS,        -1},
  {"WRITE_DAC:",             WRITE_DAC,          -1},
  {"WRITE_LVDS:",            WRITE_LVDS,         -1},
  {"RESET_CPU:",             RESET_CPU,          -1},
  {"OPEN_BACKDOOR:",         OPEN_BACKDOOR,      -1},
  {"CLOSE_BACKDOOR:",        CLOSE_BACKDOOR,     -1},
  {"RESET_STATUS:",          RESET_STATUS,       -1},
  {"WRITE_MEMORY:",          WRITE_MEMORY,       -1},
  {"SETUP_SERIAL_PORT_0:",   SETUP_SERIAL_PORT,   0},
  {"SETUP_SERIAL_PORT_1:",   SETUP_SERIAL_PORT,   1},
  {"WRITE_SERIAL_PORT_0:",   WRITE_SERIAL_PORT,   0},
  {"WRITE_SERIAL_PORT_1:",   WRITE_SERIAL_PORT,   1},
  {"INIT_LECROY_BUS:",       INIT_LECROY_BUS,    -1},
  {"EXEC_LECROY_BUS:",       EXEC_LECROY_BUS,    -1},
  {"EXEC_WRITE_COMMAND:",    EXEC_WRITE_COMMAND, -1}};

  int i, k;
  static int swtch, param;
  static char *cmdnm;
  static int swtch_new, param_new;
  static char *cmdnm_new;
  static bool switch_command;
  char token[80];
  
  static int8  n;
  static int64 ID[64];
  static int8  pri[64];

  static int8 setup_dallas;
  static int8 ena, rst, man, mdw, par;
  static int8 sca;
  
  static int16 dac[16];

  static int   lvds_n;
  static int8  lvds_adr[256];
  static int16 lvds_dat[256];

  static int8 setup_serial;
  static int8 mode, rate;
//~------

//printf("USCM_parser: action = %d\n", action);

  msg[0] = '\0';

//~------

  if (action == BEGIN) {
    swtch = IDLE;
    n = 0;
    for (i=0; i<64; i++) ID[i]  = 0;
    for (i=0; i<64; i++) pri[i] = 0;
    setup_dallas = 0x00;
    sca = 0x00;
    for (i=0; i<16; i++) dac[i] = 0x8000;
    lvds_n = 0;
    setup_serial = 0x00;
    return;
  }

//~------

  if (action == END) {
    switch_command = TRUE;
  }
  else {
    switch_command = FALSE;
    k = sscanf(b, "%s", token);
    if (k == 1) {
      for (i=0; i<DIM(Command); i++) {
        if (!strcmp(token, Command[i].name)) {
          switch_command = TRUE;
          cmdnm_new = Command[i].name;
          swtch_new = Command[i].swtch;
          param_new = Command[i].param;
          sprintf(msg, "*** OK");
          break;
        }
      }
    }
  }

//~------

  switch (swtch) {
    int ind, prio;
    int64 id;
    char bit[32];
    int8 byte;
    int16 code;

//~---

    case IDLE:
    break;

//~---

    case WRITE_DALLAS_ID:

      if (switch_command) {
        if (n) {
          write_DS1820_table(param, n, ID, pri, &err);
          if (err) {
            success = FALSE;
            sprintf(msg, "*** Error: failed to store previous command");
          }
        }
        n = 0;
        for (i=0; i<64; i++) ID[i]  = 0;
        for (i=0; i<64; i++) pri[i] = 0;
        break;
      }

      k = sscanf(b, "%d %16llX %d", &ind, &id, &prio);
      if (k == 3) {
        if      (ind < 0 || ind > 63) {
          success = FALSE;
          sprintf(msg, "*** Error: number %d is invalid", ind);
        }
        else if (!DOWCRC(id)) {
          success = FALSE;
          sprintf(msg, "*** Error: ID CRC error");
        }
        else if (prio < 0 || prio > 9) {
          success = FALSE;
          sprintf(msg, "*** Error: priority %d is invalid", prio);
        }
        else {
          n        = MAX(n, ind+1);
          ID[ind]  = id;
          pri[ind] = prio;
          sprintf(msg, "*** OK");
        }
      }

    break;

//~---

    case SETUP_DALLAS:

      if (switch_command) {
        if (setup_dallas == 0x1F) {
          setup_dallas = 0x00;
          setup_DS1820_readout(ena, rst, man, mdw, par, &err);
          if (err) {
            success = FALSE;
            sprintf(msg, "*** Error: failed to store previous command");
          }
        }
        break;
      }

      k = sscanf(b, "ENABLE %c %c %c %c %c %c %c %c",
                 &bit[7], &bit[6], &bit[5], &bit[4],
                 &bit[3], &bit[2], &bit[1], &bit[0]);
      if (k == 8) {
        for (byte=0x00, i=0; i<8; i++) {
          if (bit[i] == '1' || bit[i] == 'E') byte = byte | (1 << i);
        }
        ena = byte;
        setup_dallas |= 0x01;
        sprintf(msg, "*** OK");
        break;
      }

      k = sscanf(b, "RESET %c %c %c %c %c %c %c %c",
                 &bit[7], &bit[6], &bit[5], &bit[4],
                 &bit[3], &bit[2], &bit[1], &bit[0]);
      if (k == 8) {
        for (byte=0x00, i=0; i<8; i++) {
          if (bit[i] == '1' || bit[i] == 'R') byte = byte | (1 << i);
        }
        rst = byte;
        setup_dallas |= 0x02;
        sprintf(msg, "*** OK");
        break;
      }

      k = sscanf(b, "MANUAL %c %c %c %c %c %c %c %c",
                 &bit[7], &bit[6], &bit[5], &bit[4],
                 &bit[3], &bit[2], &bit[1], &bit[0]);
      if (k == 8) {
        for (byte=0x00, i=0; i<8; i++) {
          if (bit[i] == '1' || bit[i] == 'M') byte = byte | (1 << i);
        }
        man = byte;
        setup_dallas |= 0x04;
        sprintf(msg, "*** OK");
        break;
      }

      k = sscanf(b, "MODE %c %c %c %c %c %c %c %c",
                 &bit[7], &bit[6], &bit[5], &bit[4],
                 &bit[3], &bit[2], &bit[1], &bit[0]);
      if (k == 8) {
        for (byte=0x00, i=0; i<8; i++) {
          if (bit[i] == '1' || bit[i] == 'P') byte = byte | (1 << i);
        }
        mdw = byte;
        setup_dallas |= 0x08;
        sprintf(msg, "*** OK");
        break;
      }

      k = sscanf(b, "PARALLEL %hhd", &byte);
      if (k == 1) {
        if (byte > 64) {
          success = FALSE;
          sprintf(msg, "*** Error: number is invalid");
        }
        else {
          par = byte;
          setup_dallas |= 0x10;
          sprintf(msg, "*** OK");
          break;
        }
      }

    break;

//~---

    case SCAN_DALLAS:

      if (switch_command) {
        if (sca) {
          start_DS1820_scan(sca, &err);
          if (err) {
            success = FALSE;
            sprintf(msg, "*** Error: failed to store previous command");
          }
        }
        break;
      }

      k = sscanf(b, "SCAN %c %c %c %c %c %c %c %c",
                 &bit[7], &bit[6], &bit[5], &bit[4],
                 &bit[3], &bit[2], &bit[1], &bit[0]);
      if (k == 8) {
        for (byte=0x00, i=0; i<8; i++) {
          if (bit[i] == '1') byte = byte | (1 << i);
        }
        sca = byte;
        sprintf(msg, "*** OK");
        break;
      }

    break;

//~---

    case WRITE_DAC:

      if (switch_command) {
        for (i=0; i<16; i++) {
          if (!(dac[i] & 0x8000)) {
            write_DAC(dac, &err);
            if (err) {
              success = FALSE;
              sprintf(msg, "*** Error: failed to store previous command");
            }
            break;
          }
        }
        break;
      }

      k = sscanf(b, "%d %hX", &ind, &code);
      if (k == 2) {
        if      (ind < 0 || ind > 15) {
          success = FALSE;
          sprintf(msg, "*** Error: channel %d is invalid", ind);
        }
        else if (code > 0xFFF) {
          success = FALSE;
          sprintf(msg, "*** Error: code %X is invalid", code);
        }
        else {
          dac[ind] = code;
          sprintf(msg, "*** OK");
          break;
        }
      }

    break;

//~---

    case WRITE_LVDS:

      if (switch_command) {
        if (lvds_n) {
          for (i=0; i<lvds_n; i++) {
            write_LVDS_bus(lvds_adr[i], lvds_dat[i], &err);
            if (err) {
              success = FALSE;
              sprintf(msg, "*** Error: failed to store previous command");
            }
          }
        }
        break;
      }

      k = sscanf(b, "ADR %X DAT %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c",
                 &ind,
                 &bit[15], &bit[14], &bit[13], &bit[12],
                 &bit[11], &bit[10], &bit[ 9], &bit[ 8],
                 &bit[ 7], &bit[ 6], &bit[ 5], &bit[ 4],
                 &bit[ 3], &bit[ 2], &bit[ 1], &bit[ 0]);
      if (k == 17) {
        if (ind < 0 || ind > 255) {
          success = FALSE;
          sprintf(msg, "*** Error: address %X is invalid", ind);
        }
        else {
          lvds_adr[lvds_n] = ind;
          for (code=0x0000, i=0; i<16; i++) {
            if (bit[i] == '1') code = code | (1 << i);
          }
//printf("\nCase 1a: code = %04X\n", code);
          lvds_dat[lvds_n] = code;
          lvds_n = lvds_n + 1;
          sprintf(msg, "*** OK");
        }
        break;
      }
      
      k = sscanf(b, "ADR %X DAT %c %c %c %c %c %c %c %c",
                 &ind,
                 &bit[ 7], &bit[ 6], &bit[ 5], &bit[ 4],
                 &bit[ 3], &bit[ 2], &bit[ 1], &bit[ 0]);
      if (k == 9) {
        if (ind < 0 || ind > 255) {
          success = FALSE;
          sprintf(msg, "*** Error: address %X is invalid", ind);
        }
        else {
          lvds_adr[lvds_n] = ind;
          for (code=0x0000, i=0; i<8; i++) {
            if (bit[i] == '1') code = code | (1 << i);
          }
//printf("\nCase 2a: code = %04X\n", code);
          lvds_dat[lvds_n] = code;
          lvds_n = lvds_n + 1;
          sprintf(msg, "*** OK");
        }
        break;
      }
      
      k = sscanf(b, "ADR %X DAT %hX", &ind, &code);
      if (k == 2) {
        if (ind < 0 || ind > 255) {
          success = FALSE;
          sprintf(msg, "*** Error: address %X is invalid", ind);
        }
        else {
          lvds_adr[lvds_n] = ind;
//printf("\nCase 3a: code = %04X\n", code);
          lvds_dat[lvds_n] = code;
          lvds_n = lvds_n + 1;
          sprintf(msg, "*** OK");
        }
        break;
      }

      k = sscanf(b, " %X %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c",
                 &ind,
                 &bit[15], &bit[14], &bit[13], &bit[12],
                 &bit[11], &bit[10], &bit[ 9], &bit[ 8],
                 &bit[ 7], &bit[ 6], &bit[ 5], &bit[ 4],
                 &bit[ 3], &bit[ 2], &bit[ 1], &bit[ 0]);
      if (k == 17) {
        if (ind < 0 || ind > 255) {
          success = FALSE;
          sprintf(msg, "*** Error: address %X is invalid", ind);
        }
        else {
          lvds_adr[lvds_n] = ind;
          for (code=0x0000, i=0; i<16; i++) {
            if (bit[i] == '1') code = code | (1 << i);
          }
//printf("\nCase 1b: code = %04X\n", code);
          lvds_dat[lvds_n] = code;
          lvds_n = lvds_n + 1;
          sprintf(msg, "*** OK");
        }
        break;
      }
      
      k = sscanf(b, " %X %c %c %c %c %c %c %c %c",
                 &ind,
                 &bit[ 7], &bit[ 6], &bit[ 5], &bit[ 4],
                 &bit[ 3], &bit[ 2], &bit[ 1], &bit[ 0]);
      if (k == 9) {
        if (ind < 0 || ind > 255) {
          success = FALSE;
          sprintf(msg, "*** Error: address %X is invalid", ind);
        }
        else {
          lvds_adr[lvds_n] = ind;
          for (code=0x0000, i=0; i<8; i++) {
            if (bit[i] == '1') code = code | (1 << i);
          }
//printf("\nCase 2b: code = %04X\n", code);
          lvds_dat[lvds_n] = code;
          lvds_n = lvds_n + 1;
          sprintf(msg, "*** OK");
        }
        break;
      }

      k = sscanf(b, " %X %hX", &ind, &code);
      if (k == 2) {
        if (ind < 0 || ind > 255) {
          success = FALSE;
          sprintf(msg, "*** Error: address %X is invalid", ind);
        }
        else {
//printf("\nCase 3b: code = %04X\n", code);
          lvds_adr[lvds_n] = ind;
          lvds_dat[lvds_n] = code;
          lvds_n = lvds_n + 1;
          sprintf(msg, "*** OK");
        }
        break;
      }

    break;

//~---

    case RESET_CPU:

      if (switch_command) {
        boot_USCM(0, &err);
        break;
      }
    break;

//~---

    case OPEN_BACKDOOR:

      if (switch_command) {
        open_USCM_OTP_backdoor(&err);
        if (err) {
          success = FALSE;
          sprintf(msg, "*** Error: failed to store previous command");
        }
        break;
      }
    break;

//~---

    case CLOSE_BACKDOOR:

      if (switch_command) {
        close_USCM_OTP_backdoor(&err);
        if (err) {
          success = FALSE;
          sprintf(msg, "*** Error: failed to store previous command");
        }
        break;
      }
    break;

//~---

    case RESET_STATUS:

      if (switch_command) {
        reset_USCM_status(0, &err);
        if (err) {
          success = FALSE;
          sprintf(msg, "*** Error: failed to store previous command");
        }
        break;
      }
    break;

//~---

    case SETUP_SERIAL_PORT:

      if (switch_command) {
        if (setup_serial == 0x07) {
          setup_serial = 0x00;
          mode |= SERIAL_PORT_ENABLE;
printf("param = %d\n", param);
          setup_serial_port(param, mode, rate, &err);
          if (err) {
            success = FALSE;
            sprintf(msg, "*** Error: failed to store previous command");
          }
        }
        break;
      }

      k = sscanf(b, "TX_ROLL %c", &byte);
      if (k == 1) {
        if (byte == '1' || byte == 'E') mode |= SERIAL_TX_ROLL;
        setup_serial |= 0x01;
        sprintf(msg, "*** OK");
        break;
      }

      k = sscanf(b, "RX_ROLL %c", &byte);
      if (k == 1) {
        if (byte == '1' || byte == 'E') mode |= SERIAL_RX_ROLL;
        setup_serial |= 0x02;
        sprintf(msg, "*** OK");
        break;
      }

      k = sscanf(b, "BAUD_RATE %s", token);
      if (k == 1) {
        rate = 0xFF;
        if (!strcmp(token,  "2400")) rate = SERIAL_BAUD_RATE__2400;
        if (!strcmp(token,  "4800")) rate = SERIAL_BAUD_RATE__4800;
        if (!strcmp(token,  "9600")) rate = SERIAL_BAUD_RATE__9600;
        if (!strcmp(token, "19200")) rate = SERIAL_BAUD_RATE_19200;
        if (rate == 0xFF) {
          success = FALSE;
          sprintf(msg, "*** Error: Baud Rate is invalid");
        }
        else {
          setup_serial |= 0x04;
          sprintf(msg, "*** OK");
        }
        break;
      }
    break;

//~---

  }  // end-of-switch

//~------

  if (action != END) {
    if (switch_command) {
      switch_command = FALSE;
      cmdnm = cmdnm_new;
      swtch = swtch_new;
      param = param_new;
//printf("\n  FOUND COMMAND: %s - swtch = %d, param = %d\n", cmdnm, swtch, param);
    }
  }
}

//~============================================================================

void GPS_parser(int action, char *b, char *msg) {

  printf("GPS_parser: action = %d\n", action);
}

//~============================================================================

struct _ADC    *adc;
struct _cernox *cernox[1000];

uint8 cceb_model;
_osc OSC;

_cc cc_source[4], cc_converted[4];
_dallas_t_table dallas_t_table[2];
_cernox_t_table cernox_t_table[3];

//~----------------------------------------------------------------------------

void CC_parser(int action, char *b, char *msg) {

  int i, j;
  char fmt[80];

  char f1[] = "%hhd";
  static struct _p1 {
    char *key;
    int8 *adr;
    bool  set;
  } p1[] = {
     {"CCEB_MODEL", &cceb_model},
     {"OSCILLATOR", &OSC.selected}};

  char f3[] = "%hhd %hhd %hhd %hhd";
  static struct _p3 {
    char *key;
    int16 off;
    bool  set;
  } p3[] = {
    {"CCIF",                  O(CCIF)},
    {"FEED",                  O(feed)},
   
    {"PHASE_MIN_VALUE",       O(phase_min)},
    {"PHASE_MAX_VALUE",       O(phase_max)},
    {"PHASE_START_UP_VALUE",  O(phase_startup)},
    {"PHASE_TARGET_VALUE",    O(phase_target)},
    {"PHASE_SHUT_DOWN_VALUE", O(phase_shutdown)},

    {"REJECT_T_USE",          O(Reject_t.use)},
    {"CASE_T_USE",            O(Case_t.use)},

    {"COLD_TIP_T_USE",        O(Cold_tip_t.use)},
    {"VCS_T_USE",             O(VCS_t.use)}};

  char f4[] = "%d %d %d %d";
  static struct _p4 {
    char *key;
    int16 off;
    bool  set;
  } p4[] = {
    {"POWER_TARGET_VALUE",    O(power_target)},
    {"V_RMS_SHUT_DOWN_VALUE", O(V_RMS_shutdown)},

    {"POWER_MIN_VALUE",       O(power.min.value)},
    {"POWER_MIN_PROBATION",   O(power.min.probation)},
    {"POWER_MAX_VALUE",       O(power.max.value)},
    {"POWER_MAX_PROBATION",   O(power.max.probation)},

    {"P_BUS_MIN_VALUE",       O(P_bus.min.value)},
    {"P_BUS_MIN_PROBATION",   O(P_bus.min.probation)},
    {"P_BUS_MAX_VALUE",       O(P_bus.max.value)},
    {"P_BUS_MAX_PROBATION",   O(P_bus.max.probation)},

    {"V_RMS_MIN_VALUE",       O(V_RMS.min.value)},
    {"V_RMS_MIN_PROBATION",   O(V_RMS.min.probation)},
    {"V_RMS_MAX_VALUE",       O(V_RMS.max.value)},
    {"V_RMS_MAX_PROBATION",   O(V_RMS.max.probation)},

    {"I_RMS_MIN_VALUE",       O(I_RMS.min.value)},
    {"I_RMS_MIN_PROBATION",   O(I_RMS.min.probation)},
    {"I_RMS_MAX_VALUE",       O(I_RMS.max.value)},
    {"I_RMS_MAX_PROBATION",   O(I_RMS.max.probation)},
  
    {"Z_RMS_MIN_VALUE",       O(Z_RMS.min.value)},
    {"Z_RMS_MIN_PROBATION",   O(Z_RMS.min.probation)},
    {"Z_RMS_MAX_VALUE",       O(Z_RMS.max.value)},
    {"Z_RMS_MAX_PROBATION",   O(Z_RMS.max.probation)},

    {"REJECT_T_PROBATION",    O(Reject_t.probation)},
    {"REJECT_T_MIN_VALUE",    O(Reject_t.min.value)},
    {"REJECT_T_MIN_PROBATION",O(Reject_t.min.probation)},
    {"REJECT_T_MAX_VALUE",    O(Reject_t.max.value)},
    {"REJECT_T_MAX_PROBATION",O(Reject_t.max.probation)},
  
    {"CASE_T_PROBATION",      O(Case_t.probation)},
    {"CASE_T_MIN_VALUE",      O(Case_t.min.value)},
    {"CASE_T_MIN_PROBATION",  O(Case_t.min.probation)},
    {"CASE_T_MAX_VALUE",      O(Case_t.max.value)},
    {"CASE_T_MAX_PROBATION",  O(Case_t.max.probation)}};

  char f5[] = "%hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd ";
  static struct _p5 {
    char *key;
    int16 msk;
    bool  set;
  } p5[] = {
    {"FAILED_OSCILLATOR           ", CC_FAILED_OSCILLATOR},
    {"FAILED_TO_POWER_ON_CCIF     ", CC_FAILED_TO_POWER_ON_CCIF},
    {"FAILED_TO_ACTIVATE_RELAYS   ", CC_FAILED_TO_ACTIVATE_RELAYS},
    {"FAILED_TO_CHECK_RELAY_STATUS", CC_FAILED_TO_CHECK_RELAY_STATUS},
    {"FAILED_TO_DEACTIVATE_RELAYS ", CC_FAILED_TO_DEACTIVATE_RELAYS},
    {"FAILED_TO_SETUP_RELAYS      ", CC_FAILED_TO_SETUP_RELAYS},
    {"FAILED_TO_POWER_ON_PA       ", CC_FAILED_TO_POWER_ON_PA},
    {"FAILED_TO_START_PA          ", CC_FAILED_TO_START_PA},
    {"FAILED_TO_CHECK_PA          ", CC_FAILED_TO_CHECK_PA},
    {"FAILED_TO_STOP_PA           ", CC_FAILED_TO_STOP_PA},
    {"FAILED_PA_POWER             ", CC_FAILED_PA_POWER},
    {"FAILED_TO_SET_PHASE         ", CC_FAILED_TO_SET_PHASE}};

  char f6[] = "%hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd ";
  static struct _p6 {
    char *key;
    int16 msk;
    bool  set;
  } p6[] = {
    {"ALARM_REJECT_T_TIMEOUT", CC_ALARM_REJECT_T_TIMEOUT},
    {"ALARM_REJECT_T_LOW    ", CC_ALARM_REJECT_T_LOW},
    {"ALARM_REJECT_T_HIGH   ", CC_ALARM_REJECT_T_HIGH},
    {"ALARM_CASE_T_TIMEOUT  ", CC_ALARM_CASE_T_TIMEOUT},
    {"ALARM_CASE_T_LOW      ", CC_ALARM_CASE_T_LOW},
    {"ALARM_CASE_T_HIGH     ", CC_ALARM_CASE_T_HIGH},
    {"ALARM_P_BUS_LOW       ", CC_ALARM_P_BUS_LOW},
    {"ALARM_P_BUS_HIGH      ", CC_ALARM_P_BUS_HIGH},
    {"ALARM_V_RMS_LOW       ", CC_ALARM_V_RMS_LOW},
    {"ALARM_V_RMS_HIGH      ", CC_ALARM_V_RMS_HIGH},
    {"ALARM_I_RMS_LOW       ", CC_ALARM_I_RMS_LOW},
    {"ALARM_I_RMS_HIGH      ", CC_ALARM_I_RMS_HIGH},
    {"ALARM_Z_RMS_LOW       ", CC_ALARM_Z_RMS_LOW},
    {"ALARM_Z_RMS_HIGH      ", CC_ALARM_Z_RMS_HIGH},
    {"ALARM_POWER_LOW       ", CC_ALARM_POWER_LOW},
    {"ALARM_POWER_HIGH      ", CC_ALARM_POWER_HIGH}};

//~------

//printf("CC_parser: action = %d\n", action);

  msg[0] = '\0';

//~---

  if (action == BEGIN) {
    for (i=0; i<DIM(p1); i++) p1[i].set = FALSE;
    for (i=0; i<DIM(p3); i++) p3[i].set = FALSE;
    for (i=0; i<DIM(p4); i++) p4[i].set = FALSE;
    for (i=0; i<DIM(p5); i++) p5[i].set = FALSE;
    for (i=0; i<DIM(p6); i++) p6[i].set = FALSE;
    for (j=0; j<4; j++) {
      cc_source[j].Cold_tip_t.index = 0xFF;
      cc_source[j].VCS_t.index      = 0xFF;
    }
    adc = NULL;
    for (i=0; i<DIM(ref_table); i++) cernox[i] = &ref_table[i];
    return;
  }

//~---

  if (action == CONTINUE) {
    int k = -1;
    bool accepted = FALSE;

    if (!accepted) {
      for (i=0; i<DIM(p1); i++) {
        struct _p1 *p = &p1[i];
        if (p->set) continue;
        sprintf(fmt, "%s %s", p->key, f1);
        k = sscanf(b, fmt, p->adr);
        if (k == 1) {
          accepted = TRUE;
          p->set   = TRUE;
        }
      }
    }

    if (!accepted) {
      for (i=0; i<DIM(p3); i++) {
        uint8 *a[4];
        struct _p3 *p = &p3[i];
        if (p->set) continue;
        sprintf(fmt, "%s %s", p->key, f3);
        for (j=0; j<4; j++) a[j] = (uint8*)&cc_source[j] + p->off / sizeof(uint8);
        k = sscanf(b, fmt, a[0], a[1], a[2], a[3]);
        if (k == 4) {
          accepted = TRUE;
          p->set   = TRUE;
        }
      }
    }

    if (!accepted) {
      for (i=0; i<DIM(p4); i++) {
        struct _p4 *p = &p4[i];
        if (p->set) continue;
        sprintf(fmt, "%s %s", p->key, f4);
        if (strstr(p->key, "_T_") && strstr(p->key, "VALUE")) {
          char *a[4];
          for (j=0; j<4; j++) a[j] = (char*)&cc_source[j] + p->off / sizeof(char);
          k = sscanf(b, fmt, a[0], a[1], a[2], a[3]);
          if (k == 4) {
            accepted    = TRUE;
            p->set = TRUE;
          }
        }
        else {
          uint32 *a[4];
          for (j=0; j<4; j++) a[j] = (uint32*)&cc_source[j] + p->off / sizeof(uint32);
          k = sscanf(b, fmt, a[0], a[1], a[2], a[3]);
          if (k == 4) {
            accepted = TRUE;
            p->set   = TRUE;
          }
        }
      }
    }

    if (!accepted) {
      for (i=0; i<DIM(p5); i++) {
        uint8 *a[4], bit[4];
        struct _p5 *p = &p5[i];
        if (p->set) continue;
        sprintf(fmt, "%s %s", p->key, f5);
        for (j=0; j<4; j++) a[j] = (uint8*)&cc_source[j].fail.limit[i];
        k = sscanf(b, fmt, &bit[0], a[0], &bit[1], a[1], &bit[2], a[2], &bit[3], a[3]);
        if (k == 8) {
          accepted = TRUE;
          p->set   = TRUE;
          for (j=0; j<4; j++) {
            pack16((int16*)&cc_source[j].check.fail, p->msk, bit[j]);
          }
        }
      }
    }

    if (!accepted) {
      for (i=0; i<DIM(p6); i++) {
        uint8 *a[4], bit[4];
        struct _p6 *p = &p6[i];
        if (p->set) continue;
        sprintf(fmt, "%s %s", p->key, f6);
        for (j=0; j<4; j++) a[j] = (uint8*)&cc_source[j].alarm.limit[i];
        k = sscanf(b, fmt, &bit[0], a[0], &bit[1], a[1], &bit[2], a[2], &bit[3], a[3]);
        if (k == 8) {
          accepted = TRUE;
          p->set   = TRUE;
          for (j=0; j<4; j++) {
            pack16((int16*)&cc_source[j].check.alarm, p->msk, bit[j]);
          }
        }
      }
    }

    if (!accepted) {
      uint8 ind;
      char t_min, t_max;
      uint32 P_max;
      k = sscanf(b, "DALLAS_T_TABLE %hhu %hhd %hhd %d", 
                &ind, &t_min, &t_max, &P_max);
      if (k == 4) {
        if (ind < sizeof(dallas_t_table)/sizeof(dallas_t_table[0])) {
          accepted = TRUE;
          for (j=0; j<4; j++) {
            _dallas_t_table *p = &cc_source[j].dallas_t_table[ind];
            p->t_min = t_min;
            p->t_max = t_max;
            p->P_max = P_max;
          }
        }
      }
    }

    if (!accepted) {
      uint8  ind;
      uint16 t_min_1, t_max_1, t_min_2, t_max_2;
      uint32 P_max;
      k = sscanf(b, "CERNOX_T_TABLE %hhu %hd %hd %hd %hd %d", 
                 &ind, &t_min_1, &t_max_1, &t_min_2, &t_max_2, &P_max);
      if (k == 6) {
        if (ind < DIM(cernox_t_table)) {
          accepted = TRUE;
          for (j=0; j<4; j++) {
            _cernox_t_table *p = &cc_source[j].cernox_t_table[ind];
            p->Cold_tip_t_min = t_min_1;
            p->Cold_tip_t_max = t_max_1;
            p->VCS_t_min      = t_min_2;
            p->VCS_t_max      = t_max_2;
            p->P_max = P_max;
          }
        }
      }
    }

    if (!accepted) {
      int jj;
      char nam[4][80];
      char *fmt[2] = {"COLD_TIP_T_SENSOR_ID %s %s %s %s", "VCS_T_SENSOR_ID %s %s %s %s"};
      for (jj=0; jj<2; jj++) {
        k = sscanf(b, fmt[jj], &nam[0][0], &nam[1][0], &nam[2][0], &nam[3][0]);
        if (k == 4) {
          accepted = TRUE;
          for (j=0; j<4; j++) {
            for (k=0; k<DIM(ref_table); k++) {
              if (!strcmp(ref_table[k].nam, &nam[j][0])) {
                if (!jj) cc_source[j].Cold_tip_t.index = k;
                else     cc_source[j].VCS_t.index = k;
              }
            }
          }
          break;
        }
      }
    }

    if (accepted) sprintf(msg, "*** OK");
    return;
  }

//~---

  if (action == END) {
    bool ok = TRUE;
    for (i=0; i<DIM(p1); i++) {
      if (!p1[i].set) {
        ok = FALSE;
        printf("*** Error: %s is not defined\n", p1[i].key);
      }
    }
    for (i=0; i<DIM(p3); i++) {
      if (!p3[i].set) {
        ok = FALSE;
        printf("*** Error: %s is not defined\n", p3[i].key);
      }
    }
    for (i=0; i<DIM(p4); i++) {
      if (!p4[i].set) {
        ok = FALSE;
        printf("*** Error: %s is not defined\n", p4[i].key);
      }
    }
    for (i=0; i<DIM(p5); i++) {
      if (!p5[i].set) {
        ok = FALSE;
        printf("*** Error: %s is not defined\n", p5[i].key);
      }
    }
    for (i=0; i<DIM(p6); i++) {
      if (!p6[i].set) {
        ok = FALSE;
        printf("*** Error: %s is not defined\n", p6[i].key);
      }
    }
    for (j=0; j<4; j++) {
      if (cc_source[j].Cold_tip_t.index == 0xFF) {
        fprintf(stderr, 
                "*** COLD_TIP_T_SENSOR_ID for CC%d is unknown ***\n", i);
      }
      if (cc_source[j].VCS_t.index == 0xFF) {
        fprintf(stderr, 
                "*** VCS_T_SENSOR_ID for CC%d is unknown ***\n", i);
      }
    }
    if (ok) {
      adc = NULL;
      switch (cceb_model) {
        case 0: adc = &EM1_ADC[0]; break;
        case 1: adc = &EM2_ADC[0]; break;
        case 2: adc = &QM_ADC[0];  break;
        case 3: adc = &FM_ADC[0];  break;
        default: 
          success = FALSE;
          printf("*** Error: unknow CCEB_MODEL = %d\n", cceb_model);
        break;
      }
      if (adc) {
        convert_parameters(TO_USCM);
        write_CCEB_task_parameters(&OSC, cc_converted, &err);
        if (err) {
          success = FALSE;
          sprintf(msg, "*** Error: failed to store previous command");
        }
      }
      else {
        success = FALSE;
        printf("*** Error: CCEB Model is not specified\n");
      }
    }
    else {
      success = FALSE;
      printf("*** Error: CC data are incomplete\n");
    }
    return;
  }

//~---

}

//~============================================================================

int main(int argc, char *argv[]) {

  int k;
  char filename[100][80];
  FILE *file[100];
  int fp = -1;
  
//~---

  setbuf(stdout, NULL);
//setup_command_path();

  printf("Usage:\n");
  printf("%s <file-name> [<USCM_ID> [<JMDC_ID> [P]]]]\n",argv[0]);
  if (argc < 2) exit(1);
  k = 2;
  if (argc > k) USCM_ID = strtoul(argv[k++], NULL, 16);
  if (argc > k) JMDC_ID = atoi(argv[k++]);
  if (argc > k) P       = atoi(argv[k++]);
  printf("filename = %s\n",   argv[1]);
  printf("USCM_ID  = %02X\n", USCM_ID);
  printf("JMDC_ID  = %d\n",   JMDC_ID);
  printf("P        = %d\n",   P);

//~---

  fp = fp + 1;
  strcpy(filename[fp], argv[1]);
  file[fp] = fopen(filename[fp], "r");
  if (!file[fp]) {
    printf("File %s absent.\n", filename[fp]);
    exit(1);
  }
  printf("*** File %s opened...\n", filename[fp]);
  
  open_config_file();

  parser = NULL;
  while (1) {
    char input[160], b[160];
    char token[80], msg[80];
    char *c;
    int k;
    bool switch_parser;
    if (!fgets(input, sizeof(input), file[fp])) {
      if (parser && !fp) parser(END, b, msg);
      fclose(file[fp]);
      printf("*** File %s closed.\n", filename[fp]);
      fp = fp - 1;
      if (fp < 0) break;
      else        continue;
    }
    input[strlen(input)-(feof(file[fp])?0:1)] = '\0';
//  input[strlen(input)-1] = '\0';
    strcpy(b, input);
    c = strstr(b, "//");
    if (c) *c = '\0';
    switch_parser = FALSE;
    k = sscanf(b, "%s", token);
    if (k == 1) {
      int i;
      if (token[0] == '@') {
        fp = fp + 1;
        strcpy(filename[fp], &token[1]);
        file[fp] = fopen(filename[fp], "r");
        if (!file[fp]) {
          printf("File %s absent.\n", filename[fp]);
          exit(1);
        }
        printf("*** File %s opened...\n", filename[fp]);
        continue;
      }
      for (i=0; i<DIM(System); i++) {
        if (!strcmp(token, System[i].name)) {
          switch_parser = TRUE;
          if (parser) parser(END, b, msg);
          parser = System[i].parser;
          parser(BEGIN, b, msg);
          break;
        }
      }
    }
    if (!switch_parser) {
      if (parser) parser(CONTINUE, b, msg);
      if (strlen(msg)) printf("%s %s\n", input, msg);
      else             printf("%s\n", input);
    }
  }

  close_config_file();

//~---

  if (!success) {
    printf("*** Error: Invalid config file(s), not written into USCM\n");
    exit(1);
  }
  
  setup_command_path_special();
  if (!initialize_EPP()) exit(1);

  erase_config_file(&err);
  if (err) EXIT("erase_config_file failed");
  write_config_file(&err);
  if (err) EXIT("write_config_file failed");

  printf("Config file %s successfully written into USCM %X\n",
         filename[0], USCM_ID);
  return 0;
}

//~============================================================================
