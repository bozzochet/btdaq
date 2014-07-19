// file ccebtask-command-read-old.c
//
// Reads ccebtask parameters from USCM.
// Obsolete.
//
// A.Lebedev - May 2007...
//

#define BUILD_FOR_CC

#include "ccebdef.h"
#include "uscmsimlib.h"
#include "cceb.h"

#define REAL 1

#define SOURCE    1
#define CONVERTED 2
#define TO_USCM   1    // convertion direction
#define BACK      2    // convertion direction

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef O
#undef O
#endif
#define O(member) offsetof(_cc, member)

char *filename;
FILE *file = NULL;

struct _ADC *adc;

static uint8 cceb_model;
static _osc OSC;
static _cc cc_source[4], cc_converted[4];

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
  {"ALARM_REJECT_T_LOW    ", CC_ALARM_REJECT_T_LOW},
  {"ALARM_REJECT_T_HIGH   ", CC_ALARM_REJECT_T_HIGH},
  {"ALARM_REJECT_T_TIMEOUT", CC_ALARM_REJECT_T_TIMEOUT},
  {"ALARM_CASE_T_LOW      ", CC_ALARM_CASE_T_LOW},
  {"ALARM_CASE_T_HIGH     ", CC_ALARM_CASE_T_HIGH},
  {"ALARM_CASE_T_TIMEOUT  ", CC_ALARM_CASE_T_TIMEOUT},
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
  
int n1 = sizeof(p1)/sizeof(p1[0]);
int n3 = sizeof(p3)/sizeof(p3[0]);
int n4 = sizeof(p4)/sizeof(p4[0]);
int n5 = sizeof(p5)/sizeof(p5[0]);
int n6 = sizeof(p6)/sizeof(p6[0]);

int8  RQ_dat_dat[1000];
int16 RQ_cnt_cnt;
int8  RP_dat_dat[1000];
int16 RP_cnt_cnt;

static void print_config_parameters(int which);

//~----------------------------------------------------------------------------

static void print_config_parameters(int which) {

  int i, j;
  char *txt[2] = {"source", "converted"};
  char *tag = which == SOURCE ? txt[0] : txt[1];
  
  printf("\n");
  printf("//~-----------------------------------------\n");
  printf("//\n");
  printf("//   USCM cceb task \033[1m%s\033[22m parameters:\n", tag);
  printf("//\n");

  for (i=0; i<n1; i++) {
    struct _p1 *p = &p1[i];
    printf("%-22s   %d\n", p->key, *p->adr);
  }

  printf("//~----------------------------0--------1--------2--------3\n");

  for (i=0; i<n5; i++) {
    uint8 a[8];
    struct _p5 *p = &p5[i];
    printf("%-22s ", p->key);
    
    for (j=0; j<4; j++) {
      uint16 *pp;
      if (which == SOURCE) pp = (uint16*)&cc_source[j].check.fail;
      else                 pp = (uint16*)&cc_converted[j].check.fail;
      a[2*j] = unpack16(*pp, p->msk);
    }

    for (j=0; j<4; j++) {
      uint8 *pp;
      if (which == SOURCE) pp = (uint8*)&cc_source[j].fail.limit[i];
      else                 pp = (uint8*)&cc_converted[j].fail.limit[i];
      a[2*j+1] = *pp;
    }
    
    printf(" %d %d      %d %d      %d %d      %d %d\n",
           a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
  }

  for (i=0; i<n6; i++) {
    uint8 a[8];
    struct _p6 *p = &p6[i];
    printf("%-22s ", p->key);
    
    for (j=0; j<4; j++) {
      uint16 *pp;
      if (which == SOURCE) pp = (uint16*)&cc_source[j].check.alarm;
      else                 pp = (uint16*)&cc_converted[j].check.alarm;
      a[2*j] = unpack16(*pp, p->msk);
    }

    for (j=0; j<4; j++) {
      uint8 *pp;
      if (which == SOURCE) pp = (uint8*)&cc_source[j].alarm.limit[i];
      else                 pp = (uint8*)&cc_converted[j].alarm.limit[i];
      a[2*j+1] = *pp;
    }
    
    printf("       %d %d      %d %d      %d %d      %d %d\n",
           a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
  }

  for (i=0; i<n3; i++) {
    uint8 *a[4];
    struct _p3 *p = &p3[i];
    printf("%-22s ", p->key);
    for (j=0; j<4; j++) {
      if (which == SOURCE) a[j] = (uint8*)&cc_source[j]    + p->off / sizeof(uint8);
      else                 a[j] = (uint8*)&cc_converted[j] + p->off / sizeof(uint8);
    }
    printf(" %8d %8d %8d %8d \n", *a[0], *a[1], *a[2], *a[3]);
  }

  for (i=0; i<n4; i++) {
    struct _p4 *p = &p4[i];
    printf("%-22s ", p->key);
    if (strstr(p->key, "_T_") && strstr(p->key, "VALUE")) {
      char *a[4];
      for (j=0; j<4; j++) {
        if (which == SOURCE) a[j] = (char*)&cc_source[j]    + p->off / sizeof(char);
        else                 a[j] = (char*)&cc_converted[j] + p->off / sizeof(char);
      }
      printf(" %8d %8d %8d %8d (%s)\n", *a[0], *a[1], *a[2], *a[3], tag);
    }
    else {
      uint32 *a[4];
      for (j=0; j<4; j++) {
        if (which == SOURCE) a[j] = (uint32*)&cc_source[j]    + p->off / sizeof(uint32);
        else                 a[j] = (uint32*)&cc_converted[j] + p->off / sizeof(uint32);
      }
      printf(" %8d %8d %8d %8d (%s)\n", *a[0], *a[1], *a[2], *a[3], tag);
    }
  }

  printf("//~-----------------Nb--CC--From----To-----P_max\n");
  for (i=0; i<2; i++) {
    for (j=0; j<4; j++) {
      _dallas_t_table *p;
      if (which == SOURCE) p = &cc_source[j].dallas_t_table[i];
      else                 p = &cc_converted[j].dallas_t_table[i];
      printf("DALLAS_T_TABLE       %d   %d", i, j);
      printf("  %4d  %4d  %8d (%s)\n", p->t_min, p->t_max, p->P_max, tag);
    }
  }

  printf("//~--------------------------0--------1--------2--------3\n");
  printf("COLD_TIP_T_SENSOR_ID  ");
  for (j=0; j<4; j++) {
    int k;
    if (which == SOURCE) k = cc_source[j].Cold_tip_t.index;
    else                 k = cc_converted[j].Cold_tip_t.index;
    printf("%8s ", ref_table[k].nam);
  }
  printf("\n");
  printf("VCS_T_SENSOR_ID       ");
  for (j=0; j<4; j++) {
    int k;
    if (which == SOURCE) k = cc_source[j].VCS_t.index;
    else                 k = cc_converted[j].VCS_t.index;
    printf("%8s ", ref_table[k].nam);
  }
  printf("\n");

  printf("//~-------------------------Cold_tip_t------VCS_t-------------\n");
  printf("//~-----------------Nb--CC--From----To---From----To------P_max\n");
  for (i=0; i<3; i++) {
    for (j=0; j<4; j++) {
      _cernox_t_table *p;
      if (which == SOURCE) p = &cc_source[j].cernox_t_table[i];
      else                 p = &cc_converted[j].cernox_t_table[i];
      printf("CERNOX_T_TABLE       %d   %d", i, j);
      printf("  %4d  %4d   %4d  %4d   %8d (%s)\n", 
             p->Cold_tip_t_min, p->Cold_tip_t_max,
             p->VCS_t_min, p->VCS_t_max, p->P_max, tag);
    }
  }
}

//~----------------------------------------------------------------------------

static void unpack_item(const uint8 siz, const void *adr, uint8 **p) {

  if (siz == 0) {
    uint8 *pp = (uint8*)adr;
    *pp++ = (*(*p)   >> 4) & 0x0F;
    *pp   = (*(*p)++ >> 0) & 0x0F;
  }
  if (siz == 1) {
    uint8 *pp = (uint8*)adr;
    *pp  = *(*p)++ >>  0;
  }
  if (siz == 2) {
    uint16 *pp = (uint16*)adr;
    *pp  = (uint16)*(*p)++ <<  8;
    *pp |= (uint16)*(*p)++ <<  0;
  }
  if (siz == 4) {
    uint32 *pp = (uint32*)adr;
    *pp  = (uint32)*(*p)++ << 24;
    *pp |= (uint32)*(*p)++ << 16;
    *pp |= (uint32)*(*p)++ <<  8;
    *pp |= (uint32)*(*p)++ <<  0;
  }
}

//~----------------------------------------------------------------------------

static void read_CCEB_task_parameters(int16 *err) {

  int i, j;

  uint8 *p;
#if REAL
  int32 RQ_typ = 0x1D;
#endif
  int8  RP_dat[10000];
#if REAL
  int32 RP_siz = 790;
#endif
  int32 RP_cnt;

#if REAL

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;

  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }

#else
  *err = 0x0000;
  RP_cnt = RQ_cnt_cnt;
  for (i=0; i<RP_cnt; i++) RP_dat[i] = RQ_dat_dat[i];
#endif

  p = RP_dat;

  unpack_item(1, &cceb_model,   &p);
  unpack_item(1, &OSC.selected, &p);

  for (i=0; i<4; i++) {
    _cc *c = &cc_converted[i];
    
    unpack_item(1, &c->CCIF, &p);
    unpack_item(1, &c->feed, &p);
    
    unpack_item(2, &c->check.fail, &p);
    for (j=0; j<8; j++) {
      unpack_item(0, &c->fail.limit[2*j], &p);
    }
    
    unpack_item(2, &c->check.alarm, &p);
    for (j=0; j<8; j++) {
      unpack_item(0, &c->alarm.limit[2*j], &p);
    }

    unpack_item(4, &c->power.min.value, &p);
    unpack_item(4, &c->power.min.probation, &p);
    unpack_item(4, &c->power.max.value, &p);
    unpack_item(4, &c->power.max.probation, &p);

    unpack_item(4, &c->P_bus.min.value, &p);
    unpack_item(4, &c->P_bus.min.probation, &p);
    unpack_item(4, &c->P_bus.max.value, &p);
    unpack_item(4, &c->P_bus.max.probation, &p);

    unpack_item(4, &c->V_RMS.min.value, &p);
    unpack_item(4, &c->V_RMS.min.probation, &p);
    unpack_item(4, &c->V_RMS.max.value, &p);
    unpack_item(4, &c->V_RMS.max.probation, &p);

    unpack_item(4, &c->I_RMS.min.value, &p);
    unpack_item(4, &c->I_RMS.min.probation, &p);
    unpack_item(4, &c->I_RMS.max.value, &p);
    unpack_item(4, &c->I_RMS.max.probation, &p);

    unpack_item(4, &c->Z_RMS.min.value, &p);
    unpack_item(4, &c->Z_RMS.min.probation, &p);
    unpack_item(4, &c->Z_RMS.max.value, &p);
    unpack_item(4, &c->Z_RMS.max.probation, &p);

    unpack_item(1, &c->Reject_t.use, &p);
    unpack_item(4, &c->Reject_t.probation, &p);
    unpack_item(1, &c->Reject_t.min.value, &p);
    unpack_item(4, &c->Reject_t.min.probation, &p);
    unpack_item(1, &c->Reject_t.max.value, &p);
    unpack_item(4, &c->Reject_t.max.probation, &p);

    unpack_item(1, &c->Case_t.use, &p);
    unpack_item(4, &c->Case_t.probation, &p);
    unpack_item(1, &c->Case_t.min.value, &p);
    unpack_item(4, &c->Case_t.min.probation, &p);
    unpack_item(1, &c->Case_t.max.value, &p);
    unpack_item(4, &c->Case_t.max.probation, &p);

    for (j=0; j<2; j++) {
      unpack_item(1, &c->dallas_t_table[j].t_min, &p);
      unpack_item(1, &c->dallas_t_table[j].t_max, &p);
      unpack_item(4, &c->dallas_t_table[j].P_max, &p);
    }

    unpack_item(1, &c->Cold_tip_t.index, &p);
    unpack_item(1, &c->Cold_tip_t.use,   &p);
    unpack_item(1, &c->VCS_t.index, &p);
    unpack_item(1, &c->VCS_t.use,   &p);

    for (j=0; j<3; j++) {
      unpack_item(2, &c->cernox_t_table[j].Cold_tip_t_min, &p);
      unpack_item(2, &c->cernox_t_table[j].Cold_tip_t_max, &p);
      unpack_item(2, &c->cernox_t_table[j].VCS_t_min, &p);
      unpack_item(2, &c->cernox_t_table[j].VCS_t_max, &p);
      unpack_item(4, &c->cernox_t_table[j].P_max, &p);
    }
  
    unpack_item(1, &c->phase_startup, &p);
    unpack_item(1, &c->phase_shutdown, &p);
    unpack_item(1, &c->phase_target, &p);
    unpack_item(1, &c->phase_min, &p);
    unpack_item(1, &c->phase_max, &p);

    unpack_item(4, &c->power_target, &p);

    unpack_item(4, &c->V_RMS_shutdown, &p);
  }
}

//~----------------------------------------------------------------------------

void convert_probation(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    int32 *p_s = ((int32*)&cc_source[i]    + off / sizeof(int32));
    int32 *p_c = ((int32*)&cc_converted[i] + off / sizeof(int32));
    switch (direction) {
      case TO_USCM: *p_c = (float)*p_s * 1000.0; break;
      case BACK:    *p_s = (float)*p_c / 1000.0; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_V_RMS(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    float AV = adc[V_RMS_0+8*i].A;
    int32 *p_s = ((int32*)&cc_source[i]    + off / sizeof(int32));
    int32 *p_c = ((int32*)&cc_converted[i] + off / sizeof(int32));
    switch (direction) {
      case TO_USCM: *p_c = (float)*p_s / AV + 0.5; break;
      case BACK:    *p_s = (float)*p_c * AV + 0.5; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_I_RMS(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    float AI = adc[I_RMS_0+8*i].A;
    int32 *p_s = ((int32*)&cc_source[i]    + off / sizeof(int32));
    int32 *p_c = ((int32*)&cc_converted[i] + off / sizeof(int32));
    switch (direction) {
      case TO_USCM: *p_c = (float)*p_s / 1000.0 / AI + 0.5; break;
      case BACK:    *p_s = (float)*p_c * 1000.0 * AI + 0.5; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_Z_RMS(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    float AV = adc[V_RMS_0+8*i].A;
    float AI = adc[I_RMS_0+8*i].A;
    int32 *p_s = ((int32*)&cc_source[i]    + off / sizeof(int32));
    int32 *p_c = ((int32*)&cc_converted[i] + off / sizeof(int32));
    switch (direction) {
      case TO_USCM: *p_c = (float)*p_s * (real)0x10000 / (AV / AI) + 0.5; break;
      case BACK:    *p_s = (float)*p_c / (real)0x10000 * (AV / AI) + 0.5; break;
    }
  }}

//~----------------------------------------------------------------------------

void convert_dallas_t(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    char *p_s = ((char*)&cc_source[i]    + off / sizeof(char));
    char *p_c = ((char*)&cc_converted[i] + off / sizeof(char));
    switch (direction) {
      case TO_USCM: *p_c = *p_s * 2; break;
      case BACK:    *p_s = *p_c / 2; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_cernox_index(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    char *p_s = ((char*)&cc_source[i]    + off / sizeof(char));
    char *p_c = ((char*)&cc_converted[i] + off / sizeof(char));
    switch (direction) {
      case TO_USCM: *p_c = *p_s; break;
      case BACK:    *p_s = *p_c; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_cernox_t(int direction, int j) {

  int i, k;
  int ADC_cha[4] = {CC0_COLD_TIP, CC1_COLD_TIP, CC2_COLD_TIP, CC3_COLD_TIP};

  for (i=0; i<4; i++) {
    _cernox_t_table *p_s = &cc_source[i].cernox_t_table[j];
    _cernox_t_table *p_c = &cc_converted[i].cernox_t_table[j];
    real M0 = adc[ADC_cha[i]+0].A;
    real M1 = adc[ADC_cha[i]+1].A;
    switch (direction) {
      case TO_USCM:
        k = cc_source[i].Cold_tip_t.index;
        p_c->Cold_tip_t_min = cernox_T_to_code((real)p_s->Cold_tip_t_min, M0, &ref_table[k]);
        p_c->Cold_tip_t_max = cernox_T_to_code((real)p_s->Cold_tip_t_max, M0, &ref_table[k]);
        k = cc_source[i].VCS_t.index;
        p_c->VCS_t_min      = cernox_T_to_code((real)p_s->VCS_t_min,      M1, &ref_table[k]);
        p_c->VCS_t_max      = cernox_T_to_code((real)p_s->VCS_t_max,      M1, &ref_table[k]);
      break;
      case BACK:
        k = cc_converted[i].Cold_tip_t.index;
        p_s->Cold_tip_t_min = cernox_code_to_T((real)p_c->Cold_tip_t_min, M0, &ref_table[k]);
        p_s->Cold_tip_t_max = cernox_code_to_T((real)p_c->Cold_tip_t_max, M0, &ref_table[k]);
        k = cc_converted[i].VCS_t.index;
        p_s->VCS_t_min      = cernox_code_to_T((real)p_c->VCS_t_min,      M1, &ref_table[k]);
        p_s->VCS_t_max      = cernox_code_to_T((real)p_c->VCS_t_max,      M1, &ref_table[k]);
      break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_power(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    float AV = adc[V_BUS_0+8*i].A;
    float AI = adc[I_BUS_0+8*i].A;
    int32 *p_s = ((int32*)&cc_source[i]    + off / sizeof(int32));
    int32 *p_c = ((int32*)&cc_converted[i] + off / sizeof(int32));
    switch (direction) {
      case TO_USCM: *p_c = (float)*p_s / (AV * AI) + 0.5; break;
      case BACK:    *p_s = (float)*p_c * (AV * AI) + 0.5; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_parameters(int direction) {

  int j;

  switch (direction) {
    case TO_USCM:
      memcpy(&cc_converted[0], &cc_source[0], sizeof(cc_source));
    break;
    case BACK:
      memcpy(&cc_source[0], &cc_converted[0], sizeof(cc_source));
    break;
  }

  convert_power(    direction, O(power.min.value));
  convert_probation(direction, O(power.min.probation));
  convert_power(    direction, O(power.max.value));
  convert_probation(direction, O(power.max.probation));

  convert_power(    direction, O(P_bus.min.value));
  convert_probation(direction, O(P_bus.min.probation));
  convert_power(    direction, O(P_bus.max.value));
  convert_probation(direction, O(P_bus.max.probation));

  convert_V_RMS(    direction, O(V_RMS.min.value));
  convert_probation(direction, O(V_RMS.min.probation));
  convert_V_RMS(    direction, O(V_RMS.max.value));
  convert_probation(direction, O(V_RMS.max.probation));

  convert_I_RMS(    direction, O(I_RMS.min.value));
  convert_probation(direction, O(I_RMS.min.probation));
  convert_I_RMS(    direction, O(I_RMS.max.value));
  convert_probation(direction, O(I_RMS.max.probation));

  convert_Z_RMS(    direction, O(Z_RMS.min.value));
  convert_probation(direction, O(Z_RMS.min.probation));
  convert_Z_RMS(    direction, O(Z_RMS.max.value));
  convert_probation(direction, O(Z_RMS.max.probation));

  convert_probation(direction, O(Reject_t.probation));
  convert_dallas_t( direction, O(Reject_t.min.value));
  convert_probation(direction, O(Reject_t.min.probation));
  convert_dallas_t( direction, O(Reject_t.max.value));
  convert_probation(direction, O(Reject_t.max.probation));

  convert_probation(direction, O(Case_t.probation));
  convert_dallas_t( direction, O(Case_t.min.value));
  convert_probation(direction, O(Case_t.min.probation));
  convert_dallas_t( direction, O(Case_t.max.value));
  convert_probation(direction, O(Case_t.max.probation));

  for (j=0; j<2; j++) {
    convert_dallas_t( direction, O(dallas_t_table[j].t_min));
    convert_dallas_t( direction, O(dallas_t_table[j].t_max));
    convert_power(    direction, O(dallas_t_table[j].P_max));
  }

  convert_cernox_index(direction, O(Cold_tip_t.index));
  convert_cernox_index(direction, O(VCS_t.index));

  for (j=0; j<3; j++) {
    convert_cernox_t( direction, j);
    convert_power(    direction, O(cernox_t_table[j].P_max));
  }
  
  convert_power(    direction, O(power_target));

  convert_V_RMS(    direction, O(V_RMS_shutdown));
}

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err;
  
  setbuf(stdout, NULL);
  setup_command_path();

  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "%s [<USCM_ID> [<JMDC_ID> [P]]]\n",argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P       = atoi(argv[3]);
  
  fprintf(stderr, "USCM_ID  = %02X\n", USCM_ID);
  fprintf(stderr, "JMDC_ID  = %d\n",   JMDC_ID);
  fprintf(stderr, "P        = %d\n",   P);

  read_CCEB_task_parameters(&err);
  if (err) EXIT("read_CCEB_task_parameters");
  fprintf(stderr, "\n                   read_CCEB_task_parameters  - OK\n\n");

  adc = NULL;
  switch (cceb_model) {
    case 0: adc = &EM1_ADC[0]; break;
    case 1: adc = &EM2_ADC[0]; break;
    case 2: adc = &QM_ADC[0];  break;
    case 3: adc = &FM_ADC[0];  break;
    default: fprintf(stderr, "Unknow cceb_model = %d\n", cceb_model); break;
  }
  if (!adc) PANIC("ADC values cannot be converted!!!\n");

  print_config_parameters(CONVERTED);
  convert_parameters(BACK);
  print_config_parameters(SOURCE);

//~--

  return 0;
}

//~============================================================================
