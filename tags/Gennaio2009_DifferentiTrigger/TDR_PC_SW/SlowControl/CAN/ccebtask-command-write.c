// file ccebtask-command-write.c
//
// Reads ccebtask parameters from config-file and writes them into USCM.
//
// A.Lebedev - May 2007...
//

#include "ccebdef.h"
#include "ccebtasklib.h"

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *filename;
FILE *file = NULL;

struct _ADC    *adc;
struct _cernox *cernox[1000];

uint8 cceb_model;
_osc OSC;

_cc cc_source[4], cc_converted[4];
_dallas_t_table dallas_t_table[2];
_cernox_t_table cernox_t_table[3];

struct {
  char        *nam;
  struct _ADC *adr;
} model[] = {
  {"CCEB_CRATE_MODEL_EM1:", &EM1_ADC[0]},
  {"CCEB_CRATE_MODEL_EM2:", &EM2_ADC[0]},
  {"CCEB_CRATE_MODEL_QM:",  &QM_ADC[0] },
  {"CCEB_CRATE_MODEL_FM",   &FM_ADC[0] }};

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

static void read_config_file(char *filename);
static void print_config_parameters(int which);

//~----------------------------------------------------------------------------

void read_config_file(char *filename) {

  int i, j;
  char fmt[80];

  for (i=0; i<n1; i++) p1[i].set = FALSE;
  for (i=0; i<n3; i++) p3[i].set = FALSE;
  for (i=0; i<n4; i++) p4[i].set = FALSE;
  for (i=0; i<n5; i++) p5[i].set = FALSE;
  for (i=0; i<n6; i++) p6[i].set = FALSE;
  for (j=0; j<4; j++) {
    cc_source[j].Cold_tip_t.index = 0xFF;
    cc_source[j].VCS_t.index      = 0xFF;
  }

  if (!file) file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "File %s absent.\n", filename);
    return;
  }

  while (1) {
    char b[8192];
    int k = -1;
    bool accepted = FALSE;
    
    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-(feof(file)?0:1)] = '\0';

    if (!accepted) {
      for (i=0; i<n1; i++) {
        struct _p1 *p = &p1[i];
        if (p->set) continue;
        sprintf(fmt, "%s %s", p->key, f1);
        k = sscanf(b, fmt, p->adr);
        if (k == 1) {
          accepted    = TRUE;
          p->set = TRUE;
        }
      }
    }

    if (!accepted) {
      for (i=0; i<n3; i++) {
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
      for (i=0; i<n4; i++) {
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
      for (i=0; i<n5; i++) {
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
      for (i=0; i<n6; i++) {
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

    if (accepted) fprintf(stderr, "\033[1m%s \033[22m \n", b);
    else          fprintf(stderr, "%s\n", b);
  }

  fprintf(stderr, "\n\n");
  sprintf(fmt, "*** \%%s is not defined ***\n");
  for (i=0; i<n1; i++) if (!p1[i].set) fprintf(stderr, fmt, p1[i].key);
  for (i=0; i<n3; i++) if (!p3[i].set) fprintf(stderr, fmt, p3[i].key);
  for (i=0; i<n4; i++) if (!p4[i].set) fprintf(stderr, fmt, p4[i].key);
  for (i=0; i<n5; i++) if (!p5[i].set) fprintf(stderr, fmt, p5[i].key);
  for (i=0; i<n6; i++) if (!p6[i].set) fprintf(stderr, fmt, p6[i].key);
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
}

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

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int16 err;
  
  setbuf(stdout, NULL);
  setup_command_path();

  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "%s <file-name> [<USCM_ID> [<JMDC_ID> [P]]]\n",argv[0]);

  if (argc < 2) exit(1);
  filename = argv[1];
  if (argc > 2) USCM_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) JMDC_ID = atoi(argv[3]);
  if (argc > 4) P       = atoi(argv[4]);
  
  fprintf(stderr, "Filename = %s\n",   filename);
  fprintf(stderr, "USCM_ID  = %02X\n", USCM_ID);
  fprintf(stderr, "JMDC_ID  = %d\n",   JMDC_ID);
  fprintf(stderr, "P        = %d\n",   P);

  read_config_file(filename);
  if (P) print_config_parameters(SOURCE);
  switch (cceb_model) {
    case 0: adc = &EM1_ADC[0]; break;
    case 1: adc = &EM2_ADC[0]; break;
    case 2: adc = &QM_ADC[0];  break;
    case 3: adc = &FM_ADC[0];  break;
    default: 
      printf("*** Error: unknow CCEB_MODEL = %d\n", cceb_model);
      exit(1);
    break;
  }
  for (i=0; i<DIM(ref_table); i++) cernox[i] = &ref_table[i];
  convert_parameters(TO_USCM);
  if (P) print_config_parameters(CONVERTED);

  write_CCEB_task_parameters(&OSC, cc_converted, &err);
  if (err) EXIT("write_CCEB_task_parameters");
  fprintf(stderr, "\n                   write_CCEB_task_parameters - OK\n\n");

  read_CCEB_task_parameters(&err);
  if (err) EXIT("read_CCEB_task_parameters");
  fprintf(stderr, "\n                   read_CCEB_task_parameters  - OK\n\n");

  if (P) {
    print_config_parameters(CONVERTED);
    convert_parameters(BACK);
    print_config_parameters(SOURCE);
  }

//~--

  return 0;
}

//~============================================================================
