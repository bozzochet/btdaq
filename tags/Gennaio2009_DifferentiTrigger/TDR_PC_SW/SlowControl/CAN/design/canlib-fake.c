// file canlib-fake.c
//
// to be used for debuging in */design directories

#include "uscmlib.h"

int P1 = 1;
int P2 = 1;
int P3 = 1;
int P4 = 1;

char *title;                         // global variable

static bool timeout_set = FALSE;
static float timeout;

int16 LVDS_dat[256];
int16 DAC_dat[16];
int8 serial_mode[2], serial_rate[2];

struct {
  int16 esw;
  int16 msw;
  int8  swt;
  int8  pump_dac[2];
  int8  PWM[6];
  int8  pres[8];
} TTCE;

//~============================================================================

#if needed

static void reset_all_LVDS_registers(void) {

  FILE *file;
  int i;
  
  for (i=0; i<256; i++) LVDS_dat[i] = 0x0000;

  file = fopen("LVDS.dat","w");
  if (file) {
    fwrite(LVDS_dat, sizeof(LVDS_dat[0]), 256, file);
    fclose(file);
  }
}

#endif

//~----------------------------------------------------------------------------

static void reset_all_DAC(void) {

  FILE *file;
  int i;
  
  for (i=0; i<16; i++) DAC_dat[i] = 0x0000;

  file = fopen("DAC.dat","w");
  if (file) {
    fwrite(DAC_dat, sizeof(DAC_dat[0]), 16, file);
    fclose(file);
  }
}

//~============================================================================
//
//                  CANLIB FUNCTIONS
//
//~============================================================================

void set_CAN_timeout(float value) {

  timeout     = value;
  timeout_set = TRUE;
}

//~----------------------------------------------------------------------------

bool get_CAN_timeout(float *value) {

  if (value) *value = timeout;
  return timeout_set;
}

//~============================================================================

bool initialize_EPP() {

  return 1;
}

//~----------------------------------------------------------------------------

int16 CAN_SND_RCV(int action, _CAN_msg *SND, _CAN_msg *RCV, int16 *error) {

  int i;
  
  *error = 0;
  
  if (action != CONTINUE) return DONE;
  
  if (rand() % 2) *error = 0;
  else            *error = 1;

//  if (!*error) {
    RCV->ID  = SND->ID;
    RCV->len = SND->len + rand() % 2;
    RCV->len = RCV->len & 0x0F;
    for (i=0; i<MIN(RCV->len,8); i++) RCV->dat[i] = SND->dat[i];
//  }
  
  return DONE;
}

//~============================================================================
//
//                  BOOT COMMANDS
//
//~============================================================================

void reset_CPU(int16 *err) {

  if (P2) printf("CMD:: reset_CPU err = %2d(fake)\n", *err);
  reset_all_DAC();
}

//~----------------------------------------------------------------------------

void boot_from_OTP(int16 *err) {

  if (P2) printf("CMD:: boot_from_OTP err = %2d(fake)\n", *err);
}

//~----------------------------------------------------------------------------

void boot_from_FLASH(int16 *err) {

  if (P2) printf("CMD:: boot_from_FLASH err = %2d(fake)\n", *err);
}

//~----------------------------------------------------------------------------

void boot_USCM(int8 from, int16 *err) {

  printf("boot_USCM(%d,...)\n", from);
  sleep(1);
  if (from == 3 || from == 4) *err = 0;
  else                        *err = 1;
}

//~============================================================================
//
//                  "SYSTEM" COMMANDS
//
//~============================================================================

void clear_power_FF(int16 *err) {

  if (rand() % 2) *err = 0;
  else            *err = 1;
*err=0;

  if (P2) printf("CMD:: clear_power_FF err = %2d(fake)\n", *err);
}

//~----------------------------------------------------------------------------

void open_USCM_OTP_backdoor(int16 *err) {

  if (rand() % 2) *err = 0;
  else            *err = 1;
*err=0;

  if (P2) printf("CMD:: open_USCM_OTP_backdoor err = %2d(fake)\n", *err);
}

//~----------------------------------------------------------------------------

void close_USCM_OTP_backdoor(int16 *err) {

  if (rand() % 2) *err = 0;
  else            *err = 1;
*err=0;

  if (P2) printf("CMD:: close_USCM_OTP_backdoor err = %2d(fake)\n", *err);
}

//~============================================================================
//
//                  PING/ECHO COMMAND
//
//~============================================================================

void ping_USCM(
     int cnt_RQ, int8 *dat_RQ, int *cnt_RP, int8 *dat_RP, int16 *err) {

  if (cnt_RP) {
    *cnt_RP = cnt_RQ | (rand() & 1);
    printf("ping_USCM: cnt_RQ = %d, cnt_RP = %d\n", cnt_RQ, *cnt_RP);
  }
  else {
    printf("ping_USCM: cnt_RQ = %d, &cnt_RP = NULL\n", cnt_RQ);
  }
  
  *err = rand() & 1;
}

//~============================================================================
//
//                  STATUS COMMANDS
//
//~============================================================================

void get_short_status(int8 *dat, int16 *err) {

  int i;
  static int8 sta = 0x00;

  printf("get_short_status: ");  
  printf(" dat = ");  
  dat[0] = sta;
  for (i=1; i<8; i++) dat[i] = rand() & 0xFF;
  for (i=0;i<8;i++) printf("%02X ", dat[i]);
  *err = rand() & 1;
  *err = 0;
  printf(" err = 0x%04X\n", *err);
  sta = MOD(sta+1, 15);
}

//~----------------------------------------------------------------------------

void reset_short_status(int16 *err) {

  printf("reset_short_status\n");  
  sleep(1);
  *err = rand() & 1;
}

//~----------------------------------------------------------------------------

void get_long_status(int8 *dat, int16 *err) {

  int i;

  printf("get_long_status: dat = ");  
  sleep(1);
  for (i=0; i<727; i++) dat[i] = 'A';
  printf("Version: 32-Feb-05... ");
  *err = rand() & 1;
  printf(" err = 0x%04X\n", *err);
}

//~----------------------------------------------------------------------------

void reset_long_status(int16 *err) {

  printf("reset_long_status\n");  
  sleep(1);
  *err = rand() & 1;
}

//~============================================================================
//
//                  CONSOLE COMMANDS
//
//~============================================================================

void write_console(int n, int8 *dat, int16 *err) {

  int i;
  
  printf("write_console:: n =%2d: ", n);

  for (i=0; i<n; i++) {
    printf("%02X ", dat[i]);
  }
  printf("\n");
}

//~============================================================================
//
//                  MEMORY COMMANDS
//
//~============================================================================

void read_USCM_memory(int n, int32 adr, int8 *dat, int16 *err) {

  int i;

  *err = 0;
  for (i=0; i<MIN(10,n); i++) dat[i] = i >> 4 | i;

  dat[0] = rand() % 2;

  printf("read_USCM_memory::  n=%d adr=0x%06X dat=", n, adr);
  for (i=0; i<MIN(10,n); i++) printf("0x%02X ", dat[i]);
  printf("\n");
}

//~----------------------------------------------------------------------------

void write_USCM_memory(int n, int32 adr, int8 *dat, int16 *err) {

  int i;
  int16 RQ_cnt;
  
  *err = 0;
  
  RQ_cnt = 5 + n;
  
  if (RQ_cnt > 517) {
    *err = 0x0300;
    return;
  }

  printf("write_USCM_memory:: n=%d adr=0x%06X dat=", n, adr);
  for (i=0; i<MIN(10,n); i++) printf("0x%02X ", dat[i]);
  printf("\n");
}

//~-----------------------------------------------------------------------------

void start_FLASH_erasure(int8 sector, int16 *err) {

  printf("start_FLASH_erasure:: sector=%d\n", sector);
  *err = 0;
}

//~-----------------------------------------------------------------------------

void check_FLASH_erasure(int8 *done, int16 *err) {

  printf("check_FLASH_erasure::\n");
  *done = rand() & 1;
  *err = 0;
}

//~-----------------------------------------------------------------------------

void validate_FLASH(
     int32 begin, int32 *bgn, int32 *end, int16 *chk_1, int16 *chk_2, int16 *err) {

  printf("validate_FLASH::\n");
  *err = 0;
}

//~============================================================================
//
//                  SERIAL PORTS COMMANDS
//
//~============================================================================

void setup_serial_port(int8 port, int8 mode, int8 rate, int16 *err) {

  if (port < 2 ) {
    serial_mode[port] = mode;
    serial_rate[port] = rate;
    *err = 0;
  }
  else {
    *err = 0x0100;
  }

  if (P > 1) {
    printf("setup_serial_port:: port =%2d: mode = 0x%02X, rate = %d, err = 0x%04X\n",
            port, mode, rate, *err);
   }
}

//~----------------------------------------------------------------------------

void check_serial_port(int8 port, int8 *mode, int8 *rate, int16 *err) {

  if (port < 2 ) {
    *mode = serial_mode[port];
    *rate = serial_rate[port];
    *err = 0;
  }
  else {
    *err = 0x0100;
  }

  if (P > 1) {
    printf("check_serial_port:: port =%2d: mode = 0x%02X, rate = %d, err = 0x%04X\n",
            port, *mode, *rate, *err);
  }
}

//~----------------------------------------------------------------------------

void enable_serial_port(int8 port, int16 *err) {

  if (P > 1) printf("enable_serial_port:: port =%2d\n", port);
  *err = 0;
}

//~----------------------------------------------------------------------------

void disable_serial_port(int8 port, int16 *err) {

  if (P > 1) printf("disable_serial_port:: port =%2d\n", port);
  *err = 0;
}

//~----------------------------------------------------------------------------

void flush_serial_port(int8 port, int16 *err) {

  if (P > 1) printf("flush_serial_port:: port =%2d\n", port);
  *err = 0;
}

//~----------------------------------------------------------------------------

void read_serial_port(int8 port, int siz, int *n, int8 *dat, int16 *err) {

  int i;
  
  *n = 7;
  dat[0] = 0x02;
  dat[1] = '3';
  dat[2] = 'A';
  dat[3] = 'B';
  dat[4] = 'C';
  dat[5] = 'D';
  dat[6] = 'E';
  if (P > 1) {
    printf("read_serial_port:: port = %d, n =%2d: ", port, *n);
    for (i=0; i<*n; i++) {
      printf("%02X ", dat[i]);
    }
    printf("\n");
    printf("                                  : ");
    for (i=0; i<*n; i++) {
      printf(" %c ", dat[i] > ' ' ? dat[i] : ' ');
    }
    printf("\n");
  }

  *err = 0;
}

//~----------------------------------------------------------------------------

void write_serial_port(int8 port, int n, int8 *dat, int16 *err) {

  int i;
  
  if (P > 1) {
    printf("write_serial_port:: port = %d, n = %2d:\n", port, n);
    for (i=0; i<n; i++) {
      printf("%02X ", dat[i]);
    }
    printf("\n");
#if 0
  printf("                                   : ");
  for (i=0; i<n; i++) {
    printf(" %c ", dat[i] > ' ' ? dat[i] : ' ');
  }
  printf("\n");
#endif
  }
    
  *err = 0;
  if (((rand() & 0x000F) == 0x0001)) *err = 1;
}

//~============================================================================
//
//                  ADC & DAC COMMANDS
//
//~============================================================================

void read_ADC(int16 *dat, int16 *err) {

  int i;
  
  for (i=0; i<32; i++) {
    dat[i]  = i;
//  dat[i] |= rand() & 0x0800;
  }    

  if (rand() % 2) *err = 0;
  else            *err = 1;

  *err = 0;
  
  if (title && (strncmp(title, "LDDR", 4) == 0)) {
    for (i=0; i<32; i++) dat[i] = DAC_dat[i%16] / 2;
  }
  if (title && (strncmp(title, "CCEB", 4) == 0)) {
    for (i=0; i<32; i++) dat[i] = DAC_dat[i%16];
  }

  if (P2) printf("CMD:: read_ADC:  err = %d(fake)\n", *err);
}

//~----------------------------------------------------------------------------

void read_DAC(int16 *dat, int16 *err) {

  FILE *file;
  int i;
  
  *err = 0;

  file = fopen("DAC.dat","r");
  if (file) {
    fread(DAC_dat, sizeof(DAC_dat[0]), 16, file);
    fclose(file);
  }

  for (i=0; i<16; i++) {
    dat[i] = DAC_dat[i];
  }
  
  if (P3) {
    printf("CMD::  read_DAC:  ");
    for (i=0; i<16; i++) printf("%04X ", DAC_dat[i]);
    printf(" err =%2d(fake)\n", *err);
  }
}

//~----------------------------------------------------------------------------

void write_DAC(int16 *dat, int16 *err) {

  FILE *file;
  int i;
  
  *err = 0;

  if (P3) {
    printf("CMD:: write_DAC:\n");
    printf("dat     = ");
    for (i=0; i<16; i++) printf("%04X ", dat[i]);
    printf("\n");
  }
  
  for (i=0; i<16; i++) {
    if ((dat[i] & 0xF000) != 0x8000) DAC_dat[i] = dat[i] & 0x0FFF;
  }
  
  file = fopen("DAC.dat","w");
  if (file) {
    fwrite(DAC_dat, sizeof(DAC_dat[0]), 16, file);
    fclose(file);
  }

  if (P3) {
    printf("DAC_dat = ");
    for (i=0; i<16; i++) printf("%04X ", DAC_dat[i]);
    printf(" err =%2d(fake)\n", *err);
  }
}

//~============================================================================
//
//                  PDS COMMANDS
//
//~============================================================================

void enable_CGS_serial_lines(int16 *err) {

  printf("enable_CGS_serial_lines sent\n");
  *err = rand() % 2;
}

//~----------------------------------------------------------------------------

void disable_CGS_serial_lines(int16 *err) {

  printf("disable_CGS_serial_lines sent\n");
  *err = rand() % 2;
}

//~----------------------------------------------------------------------------

void exec_PDS_cmd(int16 cmd, int8 adc_cha, int8 *dig, int16 *adc_dat, int16 *err) {

  printf("exec_PDS_cmd sent: cmd = 0x%04X, adc_cha = %2d\n", cmd, adc_cha);

  if (dig) *dig = rand() % 2;
  if (adc_dat) *adc_dat = rand() & 0x0FFF;
  *err = rand() % 2;
}

//~============================================================================
//
//                  DALLAS TEMPERATURE SENSORS COMMANDS
//
//~============================================================================

void start_DS1820_scan(int8 bus, int16 *err) {

  if (P2) printf("start_DS1820_scan: bus = 0x%02X\n", bus);

  *err = rand() % 2;

  if (P2) printf("start_DS1820_scan: err = 0x%04X\n", *err);
  *err = 0;
}

//~----------------------------------------------------------------------------

void check_DS1820_scan(int8 *bus, int8 *n, int16 *err) {

  int i;
  
  if (P2) printf("check_DS1820_scan\n");

  if (bus) {
    if (rand() & 1) *bus = 0x00;
    else            *bus = rand();
  }
  
  if (n) {
    for (i=0; i<8; i++) n[i] = rand() & 0x003F;
    n[8] = 1;
  }

  *err = rand() & 0x0001;
  *err = 0;
}

//~----------------------------------------------------------------------------

void read_DS1820_table(int8 bus, int8 *n, int64 *ID, int8 *prio, int16 *err) {

  *err = !(rand() % 8);
  if (P2) printf("CMD:: read_DS1820_table,      err = %2d(fake)\n", *err);

  if (P4) {
    int i;
    *n = 13;
    printf("bus = %d, n = %d\n", bus, *n);
    for (i=0; i<*n; i++) {
      if (ID) ID[i] = ((int64)rand() << 32) + (int64)rand();
    }
  }
  *err = 0;
}

//~----------------------------------------------------------------------------

void write_DS1820_table(int8 bus, int8 n, int64 *ID, int8 *prio, int16 *err) {

  *err = 0;
}

//~----------------------------------------------------------------------------

void setup_DS1820_readout(int8 ena, int8 rst, int8 man, int8 mod, int8 par, int16 *err) {

  if (P2) printf("setup_DS1820_readout: ena = 0x%02X\n", ena);

  *err = rand() % 2;

  if (P2) printf("setup_DS1820_readout: err = 0x%04X\n", *err);
}

//~----------------------------------------------------------------------------

void check_DS1820_readout(
     int8 *ena, int8 *par, int8* sta, int8 *man, int8 *mdw, int8 *mdr, int16 *err) {

  int i;

  *ena = rand() & 0x0001;
  *par = rand() & 0x003F;
  for (i=0; i<9; i++) sta[i] = rand() & 0x0003;
  *man = rand() & 0x0001;
  *mdw = rand() & 0x0001;
  *mdr = rand() & 0x0001;

  *err = rand() & 0x0001;
  *err = 0;
}

//~----------------------------------------------------------------------------

void read_DS1820_temp_short(int8 bus, int8 *n, bool *ok, float *val, int16 *err) {

  int i;

//sleep(1);
  
  *n = 11;

  for (i=0; i<64; i++) {
    ok[i]  = rand() % 10;
    val[i] = (float)i;
  }    
  *err = rand() % 2;
  *err = 0;

  if (P2) printf("CMD:: read_DS1820_temp_short, err = %2d(fake)\n", *err);
}

//~----------------------------------------------------------------------------

void read_DS1820_temp_long(
     int8 bus, int8 *n, bool *ok, float *tmp, int16 *age, int16 *err) {

  *err = rand() % 2;
  *err = 0;
  if (P2) printf("CMD:: read_DS1820_temp_long,  err = %2d(fake)\n", *err);

  if (P4) {
    int i;
    printf("bus = %d\n", bus);
    if (bus != 9) *n = 64;
    else          *n = 1;
    for (i=0; i<*n; i++) {
      bool okok;
      if (i == 5) okok = FALSE;
      else        okok = TRUE;
      if (age) {
        if (i == 1) age[i] = 0xFFFF;
        else        age[i] = (int16)rand() & 0x0FFF;
      }
      if (tmp) {
        if (i == bus+1) tmp[i] = 123.45;
        else            tmp[i] = 20.23 + i;
      }
      if (ok) ok[i] = okok;
    }
  }
}

//~============================================================================
//
//                  LVDS BUS COMMANDS
//
//~============================================================================

void read_LVDS_bus(int8 adr, int16 *dat, int16 *err) {

  FILE *file;
  static int i = 0;

  if (rand() % 2) *err = 0;
  else            *err = 1;
*err = 0;

  if ((i / 6) % 2) *dat = 0xFFFF;
  else             *dat = 0x0000;
  i++;
  
  file = fopen("LVDS.dat","r");
  if (file) {
    fread(LVDS_dat, sizeof(LVDS_dat[0]), 256, file);
    fclose(file);
  }
  
  *dat = LVDS_dat[adr];
  
  if (P2) printf("CMD:: read_LVDS_bus:   adr = %02X dat = %04X(fake) err =%2d(fake)\n",
                 adr, *dat, *err);
}

//~----------------------------------------------------------------------------

void Read_LVDS_bus(int8 adr, int n, int16 *dat, int16 *err) {

  int i;
  
  if (rand() % 2) *err = 0;
  else            *err = 1;
*err = 0;

  for (i=0; i<n; i++) {
    dat[i] = (rand() & 0x0FFF) | (i << 12);
  }
  
  if (P2) printf("CMD:: Read_LVDS_bus:   adr = %02X    err =%2d(fake)\n",
                 adr, *err);
}

//~----------------------------------------------------------------------------

void write_LVDS_bus(int8 adr, int16 dat, int16 *err) {

  FILE *file;

  if (rand() % 2) *err = 0;
  else            *err = 1;
*err = 0;

  if (P3) printf("CMD:: write_LVDS_bus:  adr = %02X dat = %04X err =%2d(fake)\n",
                 adr, dat, *err);

//~--

  if (title && (strncmp(title, "CCEB", 4) == 0)) {
    if (adr == 0x00 || adr == 0x04 || adr == 0x08 || adr == 0x0C) {
      int16 LOCK = 0x0080, OPER = 0x0040, LLR = LOCK + OPER;
      int   lock = 0x0000, oper = 0x0080, llr = lock + oper;
      int16 B = 0x0020,       A = 0x0010, FEED = A + B;
      int   b = 0x0000,       a = 0x0040, feed = a + b;
      int16 cmd, sta, new;

      new = LVDS_dat[adr] & (llr + feed);
      
      sta = LVDS_dat[adr] & llr;
      cmd = dat           & LLR;
      printf("CC%d  LLR: ", adr/4);
      if      (cmd == LOCK + OPER) printf("both coils ON\n");
      else if (cmd == LOCK)        printf("coil 1 ON (=LOCK)\n");
      else if (cmd == OPER)        printf("coil 2 ON (=OPER)\n");
      else                         printf("both coils OFF\n");
      if (sta == lock && cmd == OPER) new ^= llr;
      if (sta == oper && cmd == LOCK) new ^= llr;
//printf("CC%d  LLR: ", adr/4);
//printf("old = %04X, cmd = %04X, new = %04X\n", sta, cmd, new);

      cmd = dat           & FEED;
      sta = LVDS_dat[adr] & feed;
      printf("CC%d FEED: ", adr/4);
      if      (cmd == A + B) printf("both coils ON\n");
      else if (cmd == A)     printf("coil 1 ON (FEED A)\n");
      else if (cmd == B)     printf("coil 2 ON (FEED B)\n");
      else                   printf("both coils OFF\n");
      if (sta == b && cmd == A) new ^= feed;
      if (sta == a && cmd == B) new ^= feed;
//printf("CC%d FEED: ", adr/4);
//printf("old = %04X, cmd = %04X, new = %04X\n", sta, cmd, new);

      LVDS_dat[adr] = new;
    }
  }
  else {
    LVDS_dat[adr] = dat;
  }
  
  file = fopen("LVDS.dat","w");
  if (file) {
    fwrite(LVDS_dat, sizeof(LVDS_dat[0]), 256, file);
    fclose(file);
  }
}

//~============================================================================
//
//                  LECROY COMMANDS
//
//~============================================================================

void init_LeCroy_bus(int8 bus, int16* err) {

  sleep(1);

  if (rand() % 2) *err = 0;
  else            *err = 1;
*err=0;
  if (P1) printf("CMD:: init_LeCroy_bus: bus = %02X\n", bus);
}

//~----------------------------------------------------------------------------

void exec_LeCroy_cmd(
     int n, int8 *bus, int32 *wri, int32 *rea, bool *chk, int16* err) {

  sleep(1);
  
  if (rand() < RAND_MAX / 10) *err = 1;
  else                        *err = 0;
  *err = 0;
  
  if (err && rand() < RAND_MAX / 100) *chk = TRUE;
  else                                *chk = FALSE;

//*err=0;*chk=0;

  if (P1) {
    int i;
    printf("CMD:: send_LeCroy_cmd: n = %d\n", n);
    for (i=0; i<n; i++)
    printf("                       %d) bus = %02X wri = %08X\n", 
           i, bus[i], wri[i]);
    printf("                       err = %2d(fake)\n", *err);
  }
}

//~============================================================================
//
//                  CCEB SPECIFIC COMMANDS
//
//~============================================================================

void setup_CCBT_task(int8 ctl, int16 *err) {

  *err = 0;

  if (P3) {
    printf("CMD:: setup_CCBT_task: err =%2d(fake)\n", *err);
  }
}

//~----------------------------------------------------------------------------

void read_CCBT_data(_CCBT_data *dat, int16 *err) {

  *err = 0;

  if (P3) {
    printf("CMD:: read_CCBT_data: err =%2d(fake)\n", *err);
  }
}

//~============================================================================

void setup_16ms_task(int  n, __16ms_par *par, int16 *err) {

  FILE *file;
  int i;
  
  *err = 0;

  file = fopen("16ms.dat","w");
  if (file) {
    int nn = n;
    fwrite(&nn, sizeof(nn),         1, file);
    fwrite(par, sizeof(__16ms_par), n, file);
    fclose(file);
  }

  if (P3) {
    printf("CMD:: setup_16ms_task:\n");
    for (i=0; i<n; i++) {
      printf("%2d %02hhX %04X %02hhX\n", i, par[i].cha, par[i].thr, par[i].pro);
    }
    printf(" err =%2d(fake)\n", *err);
  }
}

//~----------------------------------------------------------------------------

void check_16ms_task(int *n, __16ms_par *par, int16 *err) {

  FILE *file;
  int i;
  
  *err = 0;

  file = fopen("16ms.dat","r");
  if (file) {
    fread(n,   sizeof(*n),          1, file);
    fread(par, sizeof(__16ms_par), *n, file);
    fclose(file);
  }

  if (P3) {
    printf("CMD:: check_16ms_task:\n");
    for (i=0; i<*n; i++) {
      printf("%2d %02hhX %04X %02hhX\n", i, par[i].cha, par[i].thr, par[i].pro);
    }
    printf(" err =%2d(fake)\n", *err);
  }
}

//~----------------------------------------------------------------------------

void run_16ms_task_procedure(int8 pro, int8 *par, int16 *err) {

  *err = 0;

  if (P3) {
    printf("CMD:: run_16ms_task_procedure: %d, err =%2d(fake)\n", pro, *err);
  }
}

//~============================================================================
//
//                  TTCE SPECIFIC COMMANDS
//
//~============================================================================

void setup_TTCE_28VDC_power(int8 *dat, int16 *err) {

  int16 ena;
  int16 esw = (int16)dat[0] | ((int16)dat[3] << 8);
  int16 msw = (int16)dat[1] | ((int16)dat[4] << 8);
  int8  swt = dat[6];

  ena = (int16)dat[2] | ((int16)dat[5] << 8);
  TTCE.esw = (TTCE.esw & ~ena) | (esw & ena);
  TTCE.msw = (TTCE.msw & ~ena) | (msw & ena);

  ena = dat[7];
  TTCE.swt = (TTCE.swt & ~(int8)ena) | (swt & (int8)ena);

  *err = 0;
}

//~----------------------------------------------------------------------------

void check_TTCE_28VDC_power(int8 *dat, int16 *err) {

  dat[0] = TTCE.esw >> 0;
  dat[2] = TTCE.esw >> 8;
  dat[1] = TTCE.msw >> 0;
  dat[3] = TTCE.msw >> 8;
  dat[4] = TTCE.swt;

  *err = 0;
}

//~----------------------------------------------------------------------------

void write_TTCE_pump_DAC(int8 *wri, int16 *err) {

  TTCE.pump_dac[0] = wri[0];
  TTCE.pump_dac[1] = wri[1];

  *err = 0;
}

//~----------------------------------------------------------------------------

void read_TTCE_pump_data(int8 *DAC, int16 *RPM, int16 *err) {

  DAC[0] = TTCE.pump_dac[0];
  DAC[1] = TTCE.pump_dac[1];
  RPM[0] = 16 * TTCE.pump_dac[0];
  RPM[1] = 16 * TTCE.pump_dac[1];

  *err = 0;
}

//~----------------------------------------------------------------------------

void write_TTCE_PWM(int8 n, int8 *adr, int8 *dat, int16 *err) {

  int i;

  for (i=0; i<n; i++) TTCE.PWM[adr[i]] = dat[i];

  *err = 0;
}

//~----------------------------------------------------------------------------

void read_TTCE_PWM(int8 *dat, int16 *err) {

  int i;

  for (i=0; i<6; i++) dat[i] = TTCE.PWM[i];

  *err = 0;
}

//~----------------------------------------------------------------------------

void read_TTCE_pressure(int16 *dat, int16 *err) {

  int i;
  static int8 fake = 0;

  *err = 0;

  for (i=0; i<8; i++) TTCE.pres[i] = i % 2 ? i / 2 : fake;
  for (i=0; i<4; i++) {
    if (dat) dat[i] = ((int16)TTCE.pres[2*i] << 4) | TTCE.pres[2*i+1];
  }

  fake = MOD(fake+16, 256);
}

//~============================================================================
