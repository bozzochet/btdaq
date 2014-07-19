// file read_block_files.c
//
// read and decode block files
//
// A.Lebedev - May 2008...
//

#include "nodedef.h"
#include "treedef.h"
#include "uscmlib.h"
#include "gsclib.h"

bool XXXXX = FALSE;

int P;
char *PP;

#define NO_EVENT_NUMBER 0x0001

#define OFF_MODE        0
#define RAW_MODE        1
#define COMPRESSED_MODE 2
#define MIXED_MODE      3

#define BUILDING_ERRORS  0x0400
#define SEQUENCER_ERRORS 0x0200

#define SHF             0x0001
#define INSIDE_ENVELOPE 0x0002

int current_file;

int data_format_version;

char file_name[80], header[80];
int ident_1, ident_2;

static bool first_block_in_file, first_event_in_file;
static bool this_is_event, this_is_calibration_result;
static int32 run_id, run_tag, event_id;
static int32 old_run_id = -1, old_run_tag = -1;
static int32 event_ids[0x200];
static int64 event_time;
char first_block_time[80], first_event_time[80];

static long long lv1_time;
static float delta_T;

struct {
  char *name;
  bool mark[0x200];
  int  event_cntr[0x200];
  int  other_cntr[0x200];
} errors[] = {
  {"PNTR"}, {"TREE"}, {"PATH"}, {"CRC"}, {"LNK"}, {"RPT"}, {"STA"}, {"BLCK"}, 
  {"ADR"}, {"ENVLP"}, 
  {"SEQ"}, {"BLD"},
  {"0?"}, {"NXT1"}, {"ABO"}, {"ERR"}, {"END"}, {"TOUT1"}, {"AMSW"}, 
  {"7?"}, {"8?"}, {"9?"}, {"10?"}, {"11?"}, {"12?"}, {"13?"}, {"14?"}, {"15?"},
  {"OK?"}, {"BC1"}, {"ROOM1"}, {"DATA"}, {"EV#1"}, {"CRC1"}, {"TOUT2"}, {"NXT2"},
  {"BC2"}, {"ROOM2"}, {"ROOM3"}, {"EV#2"}, {"CRC2"}, {"29?"}, {"30?"}, {"31?"}};

int event_length, length[0x200];
int events[0x200];
struct {
  int sum;
  int cnt;
} ave_event_len;
struct {
  int sum;
  int cnt;
} ave_len[0x200];

int n_events, n_bad_events;
int n_others, n_bad_others;

struct {
  int pattern;
  int n;
} list[10000];

//~---

typedef struct {
  int    len;
  int    lnk;
  int16 *dat;
  int16  sta;
} _fra;

typedef struct {
  int16  sta;
  int16  chk;
  bool   top;
  int32  msk;   // <<<=== bad name
  int32  err;
  _tree *cddc;
  _fra fra[24];
  char *ident;
} _group;

//~---

//int32 qqq_len;
//int16 qqq_dat[1000000];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_err(int16 err, char *where);
void print_AMS_block_header(int ident, AMSBlock *pBlock);

bool is_it(AMSBlock *p, char *n, int32 t);

void work_on_my_tree(void);
void attach_tree(int adr, _tree **node);
void mark_error(int adr, char *key);

bool process_JMDC_data(AMSBlock *pBlock);
bool process_USCM_data(AMSBlock *pBlock);
bool process_JLV1_data(AMSBlock *pBlock, int16 flag);
bool process_ETRG_data(AMSBlock *pBlock, int16 flag);
bool process_JINJ_data(AMSBlock *pBlock, int16 flag);
bool process_JINF_data(AMSBlock *pBlock, int16 flag);
bool process_UDR_data(AMSBlock *pBlock, int16 flag);
bool process_SDR_data(AMSBlock *pBlock, int16 flag);
bool process_TDR_data(AMSBlock *pBlock, int16 flag);
bool process_RDR_data(AMSBlock *pBlock, int16 flag);
bool process_EDR_data(AMSBlock *pBlock, int16 flag);

bool process_AMS_block(AMSBlock *pBlock, int16 flag);

bool unpack_data_group(int16 *dat, int16 len, _group *group);
bool unpack_JINF_block(AMSBlock *pBlock);
bool unpack_JINJ_block(AMSBlock *pBlock);
bool unpack_AMSW_envelope_block(AMSBlock *pBlock);
bool unpack_envelope_block(AMSBlock *pBlock, int16 flag);
bool unpack_and_process_AMS_block(int16 *dat, int32 len, int16 flag);

void process_event(int action);

void zero_file_summary(void);
void print_file_summary(void);
void process_AMS_block_file(FILE *file);

//~============================================================================

void print_err(int16 err, char *where) {

  printf("%s*** Error %d in %s\n", header, err, where);
}

//~----------------------------------------------------------------------------

void print_AMS_block_header(int ident, AMSBlock *pBlock) {

typedef struct {
  int32 typ;
  char *txt[2];
} _typ;

_typ jmdc[] = {
  {0x00, {NULL,    "Boot" }},
  {0x01, {"Ping",   NULL  }},
  {0x02, {"Sleep", "Sleep"}},
  {0x03, {"Memory Read", "Memory Write"}},
  {0x04, {NULL, "Reset"}},
  {0x05, {"Event Envelope", NULL}},
  {0x06, {"Calib Results", NULL}},
  {0x07, {"Event", NULL}},
  {0x1F0210, {"STDOUT Text", NULL}},
  {0x1F0380, {"AMS Envelope", "AMS Envelope"}},
  {0x1F0381, {"AMS-Wire Command", "AMS-Wire Command"}},
  {0x1F0600, {NULL, "DAQ Init"}},
  {0x1F0635, {"Get JLV1 Gen", "Set JLV1 Gen"}},
  {-1}};

_typ jinj[] = {
  {0x00, {NULL,   "Boot"  }},
  {0x01, {"Event",   NULL  }},
  {0x02, {"Event Nb",   "Reset FIFO"  }},
  {0x05, {"Flash Read",   "Flash Write"  }},
  {0x06, {"Flash Test",   "Flash Load"  }},
  {0x07, {"Flash Summary",   "Flash Erase"  }},
  {0x0C, {"Q-List Read",   NULL  }},
  {0x12, {"Read Proc Mode",   "Write Proc Mode"  }},
  {0x16, {"Slave Test Status",   "Slave Test"  }},
  {0x17, {"Read Slave Masks",   "Write Slave Mask"  }},
  {0x1F0381, {"AMS-Wire Command", "AMS-Wire Command"}},
  {-1}};

_typ uscm[] = {
  {0x00, {NULL,   "\"Boot\""  }},
  {0x01, {"Ping",   NULL  }},
  {0x02, {"Read Status", "Reset Status"}},
  {0x0E, {"Check Serial",   "Setup Serial"  }},
  {0x0F, {"Read Serial 0",   "Write Serial 0"  }},
  {0x10, {"Read Serial 1",   "Write Serial 1"  }},
  {0x11, {"Read ADC",   NULL  }},
  {0x12, {"Read DAC",   "Write DAC"  }},
  {0x13, {"Read LVDS",   "Write LVDS"  }},
  {0x14, {"Exec LeCroy",   "Init LeCroy"  }},
  {0x15, {"Check DTS Scan",   "Start DTS Scan"  }},
  {0x16, {"Check DTS",   "Setup DTS"  }},
  {0x17, {"Read DTS Table",   "Write DTS Table"  }},
  {0x18, {"Read DTS Temp Short",   NULL  }},
  {0x19, {"Read DTS Temp Long",   NULL  }},
  {-1}};

_typ jlv1[] = {
  {0x00, {NULL,   "Boot"  }},
  {0x01, {"Event",   NULL  }},
  {0x02, {"Event Nb",   "Reset FIFO"  }},
  {0x05, {"Flash Read",   "Flash Write"  }},
  {0x06, {"Flash Test",   "Flash Load"  }},
  {0x07, {"Flash Summary",   "Flash Erase"  }},
  {0x0C, {"Q-List Read",   NULL  }},
  {0x0F, {"Perform I/O", NULL  }},
  {0x11, {"DM Read",   "DM Write"  }},
  {0x12, {"Read Proc Mode",   "Write Proc Mode"  }},
  {0x1F0381, {"AMS-Wire Command", "AMS-Wire Command"}},
  {-1}};

_typ jinf[] = {
  {0x00, {NULL,   "Boot"  }},
  {0x01, {"Event",   NULL  }},
  {0x02, {"Event Nb",   "Reset FIFO"  }},
  {0x05, {"Flash Read",   "Flash Write"  }},
  {0x06, {"Flash Test",   "Flash Load"  }},
  {0x07, {"Flash Summary",   "Flash Erase"  }},
  {0x0C, {"Q-List Read",   NULL  }},
  {0x11, {"DM Read",   "DM Write"  }},
  {0x12, {"Read Proc Mode",   "Write Proc Mode"  }},
  {0x16, {"Slave Test Status",   "Slave Test"  }},
  {0x17, {"Read Slave Masks",   "Write Slave Mask"  }},
  {0x18, {"Read Busy Mask",   "Write Busy Mask"  }},
  {0x1F0381, {"AMS-Wire Command", "AMS-Wire Command"}},
  {-1}};

_typ edr[] = {
  {0x00, {NULL,   "Boot"  }},
  {0x01, {"Event",   NULL  }},
  {0x02, {"Event Nb",   "Reset FIFO"  }},
  {0x05, {"Flash Read",   "Flash Write"  }},
  {0x06, {"Flash Test",   "Flash Load"  }},
  {0x07, {"Flash Summary",   "Flash Erase"  }},
  {0x0C, {"Q-List Read",   NULL  }},
  {0x11, {"DM Read",   "DM Write"  }},
  {0x12, {"Read Proc Mode",   "Write Proc Mode"  }},
  {0x13, {"Calib Status",   "Calib Control"  }},
  {0x14, {"SD Proc Status",   "SD Proc Control"  }},
  {0x1F0381, {"AMS-Wire Command", "AMS-Wire Command"}},
  {-1}};

_typ rdr[] = {
  {0x00, {NULL,   "Boot"  }},
  {0x01, {"Event",   NULL  }},
  {0x02, {"Event Nb",   "Reset FIFO"  }},
  {0x05, {"Flash Read",   "Flash Write"  }},
  {0x06, {"Flash Test",   "Flash Load"  }},
  {0x07, {"Flash Summary",   "Flash Erase"  }},
  {0x0C, {"Q-List Read",   NULL  }},
  {0x11, {"DM Read",   "DM Write"  }},
  {0x12, {"Read Proc Mode",   "Write Proc Mode"  }},
  {0x13, {"Calib Status",   "Calib Control"  }},
  {0x14, {"SD Proc Status",   "SD Proc Control"  }},
  {0x1F0381, {"AMS-Wire Command", "AMS-Wire Command"}},
  {-1}};

_typ sdr[] = {
  {0x00, {NULL,   "Boot"  }},
  {0x01, {"Event",   NULL  }},
  {0x02, {"Event Nb",   "Reset FIFO"  }},
  {0x05, {"Flash Read",   "Flash Write"  }},
  {0x06, {"Flash Test",   "Flash Load"  }},
  {0x07, {"Flash Summary",   "Flash Erase"  }},
  {0x0C, {"Q-List Read",   NULL  }},
  {0x11, {"DM Read",   "DM Write"  }},
  {0x12, {"Read Proc Mode",   "Write Proc Mode"  }},
  {0x13, {"Calib Status",   "Calib Control"  }},
  {0x14, {"SD Proc Status",   "SD Proc Control"  }},
  {0x1F0381, {"AMS-Wire Command", "AMS-Wire Command"}},
  {-1}};

_typ tdr[] = {
  {0x00, {NULL,   "Boot"  }},
  {0x01, {"Event",   NULL  }},
  {0x02, {"Event Nb",   "Reset FIFO"  }},
  {0x05, {"Flash Read",   "Flash Write"  }},
  {0x06, {"Flash Test",   "Flash Load"  }},
  {0x07, {"Flash Summary",   "Flash Erase"  }},
  {0x0C, {"Q-List Read",   NULL  }},
  {0x11, {"DM Read",   "DM Write"  }},
  {0x12, {"Read Proc Mode",   "Write Proc Mode"  }},
  {0x13, {"Calib Status",   "Calib Control"  }},
  {0x14, {"SD Proc Status",   "SD Proc Control"  }},
  {0x1F0381, {"AMS-Wire Command", "AMS-Wire Command"}},
  {-1}};

_typ udr[] = {
  {0x00, {NULL,   "Boot"  }},
  {0x01, {"Event",   NULL  }},
  {0x02, {"Event Nb",   "Reset FIFO"  }},
  {0x05, {"Flash Read",   "Flash Write"  }},
  {0x06, {"Flash Test",   "Flash Load"  }},
  {0x07, {"Flash Summary",   "Flash Erase"  }},
  {0x0C, {"Q-List Read",   NULL  }},
  {0x11, {"DM Read",   "DM Write"  }},
  {0x12, {"Read Proc Mode",   "Write Proc Mode"  }},
  {0x13, {"Calib Status",   "Calib Control"  }},
  {0x14, {"SD Proc Status",   "SD Proc Control"  }},
  {0x1F0381, {"AMS-Wire Command", "AMS-Wire Command"}},
  {-1}};

struct {
  char *nam;
  _typ *typ;
} hint[] = {
  {"JMDC", jmdc},
  {"USCM", uscm},
  {"JLV1", jlv1},
  {"ETRG", NULL},
  {"JINJ", jinj},
  {"JINF", jinf},
  {"EDR",  edr},
  {"RDR",  rdr},
  {"SDR",  sdr},
  {"TDR",  tdr},
  {"UDR",  udr}};

  int i, ind = 45;
  char b[160], *p = b;

  if (P <= 0) return;

  for (i=0; i<DIM(b); i++) b[i] = '.';
  printf(header);
  for (i=0; i<ident; i++) p += sprintf(p, " ");
  
  p += sprintf(p, "%s ", pBlock->BlockType & 1 ? "W" : "R");
  p += sprintf(p, "NA=%03X", pBlock->NodeAddress);
  if (pBlock->NodeAddress < 512) p += sprintf(p, "=%s ", NodeName[pBlock->NodeAddress]);
  else                           p += sprintf(p, "=?");
  p += sprintf(p, "DT=%02X ", pBlock->DataType);
  p += sprintf(p, "DC=%d", pBlock->DataCount);
  p += sprintf(p, " ");
  if (strcasestr(PP, "A")) {
    bool done = FALSE;
    b[strlen(b)] = ' ';
    for (i=0; i<DIM(hint); i++) {
      if (strcasestr(NodeName[pBlock->NodeAddress], hint[i].nam) && hint[i].typ) {
        _typ *t = hint[i].typ;
        while (t->typ != -1) {
          if (t->typ == pBlock->DataType) {
            if (t->txt[pBlock->ReadWrite]) {
              sprintf(&b[ind], t->txt[pBlock->ReadWrite]);
              done = TRUE;
            }
            break;
          }
          t++;
        }
      }
    }
    if (!done) sprintf(&b[ind], "Failed to decode NA/DT combination.");
  }
  printf("%s\n", b);
}

//~============================================================================

bool is_it(AMSBlock *p, char *n, int32 t) {

  if (strcasestr(NodeName[p->NodeAddress], n) && p->DataType == t) return TRUE;
  else                                                             return FALSE;
}

//~----------------------------------------------------------------------------

void work_on_my_tree(void) {

  int i, j, k;

  for (k=0; k<DIM(tree); k++) {
    _tree *n = &tree[k];
    for (j=0; j<24; j++) {
      struct _link *l = &n->lnk[j];
      if (!l->nam) continue;
      for (i=0; i<512; i++) {
//printf("i=%d NodeName[i]=%s l->nam=%s\n", i, NodeName[i], l->nam);
        if (strcasestr(NodeName[i], l->nam)) {
          l->adr = i;
//printf("i=%d NodeName[i]=%s l->nam=%s\n", i, NodeName[i], l->nam);
          break;
        }
      }
//printf("i=%d\n", i);
      if (i > 0x1FF) {
        printf("CDDC link name not found\n");
        exit(1);
      }
    }
  }

  if (P > 10) {
    printf("CDDC Internal Table:\n");
    printf("--------------------\n");
    for (k=0; k<DIM(tree); k++) {
      _tree *n = &tree[k];
      printf("%s:\n", n->nam);
      for (j=0; j<24; j++) {
        struct _link *l = &n->lnk[j];
        if (!l->nam) continue;
        printf("   %-3s %-15s %03X\n", l->crt, l->nam, l->adr);
      }
    }
  }
}

//~----------------------------------------------------------------------------

void attach_tree(int adr, _tree **node) {

  int k;

  *node = NULL;
  
  for (k=0; k<DIM(tree); k++) {
    if (strcasestr(NodeName[adr], tree[k].nam)) {
      *node = &tree[k];
      (*node)->adr = adr;
      return;
    }
  }
}

//~----------------------------------------------------------------------------

void mark_error(int adr, char *key) {

  int i;
    
  for (i=0; i<DIM(errors); i++) if (strcasestr(errors[i].name, key)) break;
  if (i == DIM(errors)) {
    printf("Unknown error key \"%s\"\n", key);
    return;
  }
  errors[i].mark[adr] = TRUE;
}

//~============================================================================

bool process_JMDC_data(AMSBlock *pBlock) {

//char *me = {"process_JMDC_data"};
  int16 *p = pBlock->Data.p16;
  char ident[80];

  char *jinjlinks[24] = {
    "T2A", "T3A", "U1A", "T0A", "S1A", "S1B", "S0A", "S0B", 
    "U0A", "T1A", "R0A", "R1A", "E0A", "E1A", "JTA", "JTB", 
    "T4A", "T5A", "S2A", "S2B", "S3A", "S3B", "T6A", "T7A"};
  
//~---

  if (pBlock->DataType == 0x07) {
    int16 Gps, Msk, Grp;
    static bool first = TRUE;

    if (P > 9) print_AMS_data_block("", pBlock);

    sprintf(ident, "%s     ", header);

    if (pBlock->DataCount < 22) {
      if (P > 0) {
        printf("%s*** Error: pBlock->DataCount = %d (22)\n", 
                ident, pBlock->DataCount);
      }
      mark_error(pBlock->NodeAddress, "pntr");
      return FALSE;
    }

    run_tag    = ((int32)p[0] << 16) + ((int32)p[1] <<  0);
    run_id     = ((int32)p[2] << 16) + ((int32)p[3] <<  0);
    event_id   = ((int32)p[4] << 16) + ((int32)p[5] <<  0);
    event_time = ((int64)p[6] << 48) + ((int64)p[7] << 32) + 
                 ((int64)p[8] << 16) + ((int64)p[6] <<  0);

    event_ids[pBlock->NodeAddress] = event_id;

    data_format_version = (run_id < 0x4848EDB2) ? 0 : 1;
//  if (P > 0) printf("run_id:      data_format_version is set to %d\n", data_format_version);

    if (first) {
      first = FALSE;
      old_run_id   = run_id;
      old_run_tag  = run_tag;
    }

    if (run_id != old_run_id || run_tag != old_run_tag) {
      old_run_id   = run_id;
      old_run_tag  = run_tag;
    }
 
    if (first_event_in_file) {
      struct tm *t;
      int32 event_time_msb = event_time >> 32;
      t = localtime((time_t*)&event_time_msb);
      sprintf(first_event_time, "%4d%02d%02d %02d:%02d ", 
             t->tm_year+1900, t->tm_mon+1, t->tm_mday,
             t->tm_hour, t->tm_min);
      first_event_in_file = FALSE;
    }
   
    if (P > 1) {
      char coarse_time[80];
      int32 event_time_msb = event_time >> 32;
      int32 fine_time = (event_time & 0x00000000FFFFFFFF) / 1000;
      struct tm *t;
      t = localtime((time_t*)&event_time_msb);
      sprintf(coarse_time, "%4d%02d%02d %02d:%02d:%02d", 
             t->tm_year+1900, t->tm_mon+1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
      printf(ident);
      printf("Run: %08X %d %08X ", run_id, run_id, run_tag);
      printf("Started: %s ",       first_event_time);
      printf("Event: %d 0x%08X ",  event_id, event_id);
      printf("Time: %s.%03d ",     coarse_time, fine_time);
      printf("\n");
    }

    Gps = unpack16(p[10], 0x8000);
    Msk = unpack16(p[10], 0x03C0);
    Grp = unpack16(p[10], 0x003F);
    if (P > 1) printf("%sGps = %d, Msk = %d, Grp = %d\n", ident, Gps, Msk, Grp);

    if (pBlock->DataCount != (22 + (Gps * 5 + Msk * 2) * 2)) {
      if (P > 0) {
        printf("%s*** Error: pBlock->DataCount = %d (%d)\n", 
               ident, pBlock->DataCount, 22 + (Gps * 5 + Msk * 2) * 2);
      }
      mark_error(pBlock->NodeAddress, "pntr");
      return FALSE;
    }

    p = &p[10] + 1;

    
    if (Gps) {
      if (pBlock->DataCount < 32) {
        if (P > 0) {
          printf("%s*** Error: pBlock->DataCount = %d (32)\n", 
                  ident, pBlock->DataCount);
        }
        mark_error(pBlock->NodeAddress, "pntr");
        return FALSE;
      }
      p = &p[4] + 1;
    }
    
    if (Msk) {
      if (unpack16(p[0], 0xFF00)) {
        if (P > 0) {
          printf("%s*** Error: Msk = %04X\n", 
                  ident, Msk);
        }
        mark_error(pBlock->NodeAddress, "pntr");
        return FALSE;
      }
      if (P > 2) {
        int i;
        printf("%sSlave Masks: JJ:%06X ", ident, (int32)(p[0]<<16)+p[1]);
        for (i=1; i<Msk; i++) {
          int   lnk = unpack16(p[2*i], 0xFF00);
          int32 msk = ((int32)(p[2*i] & 0x00FF) << 16) + p[2*i+1];
          printf("%s:%06X ", jinjlinks[lnk], msk);
        }
        printf("\n");
      }
    }
    return TRUE;
  }

//~---

  if (pBlock->DataType == 0x1F0210) {
//  static bool first = TRUE;

    if (P > 9) print_AMS_data_block("", pBlock);
//print_AMS_data_block("", pBlock);

    sprintf(ident, "%s     ", header);
    return TRUE;
  }

//~---

  return TRUE;
}

//~----------------------------------------------------------------------------

bool process_USCM_data(AMSBlock *pBlock) {

  char *me = {"process_USCM_data"};
  int i;
  int8  RP_dat[0x10000];
  int32 RP_cnt;
  char ident[80];

//~---

//print_AMS_block_header(1+ident_1, pBlock);

  if (!strcasestr(PP, "H")) return TRUE;
  
//if (P > 0) print_AMS_block_header(1, pBlock);

  RP_cnt = pBlock->DataCount;
  for (i=0; i<RP_cnt; i++) {
    RP_dat[i] = pBlock->Data.p16[i/2] >> ((i % 2) ? 0 : 8);
  }

  sprintf(ident,  "%s     ", header);

//~---

  if (is_it(pBlock, "JPD", 0x19)) {
    int8 bus, n;
    bool ok[64];
    float tmp[64];
    int16 age[64];

    if (P > 1) {
      printf("p = %08X, pBlock->DataCount = %d\n", (int32)RP_dat, pBlock->DataCount);
    }
    if ((RP_cnt - 1) % 4) {
      if (P > 1) printf("ERROR\n");
      return FALSE;
    }
    n = MAX((int)(RP_cnt - 1 ) / 4, 0);
    if (P > 1) {
      printf("n = %d\n", n);
      for (i=0; i<MIN(30,RP_cnt); i++) printf("%02hX ", RP_dat[i]);
      printf("\n");
    }

    for (i=0; i<n; i++) {
      int16 temp = (int16)RP_dat[1+4*i+0] << 8 | (int16)RP_dat[1+4*i+1];
      if (temp != 0x8000) {
        ok[i] = TRUE;
        tmp[i] = (float)(short)temp / 256.0 - 0.25;
      }
      else {
        ok[i] = FALSE;
      }
      age[i] = (int16)RP_dat[1+4*i+2] << 8 | (int16)RP_dat[1+4*i+3];
    }

    bus = RP_dat[0];
//  if (bus == 6 || bus == 7 || bus == 8) {
    if (TRUE) {
      struct tm *t;
      char data_time[80];

//printf("%08X %d\n", qqq_len, qqq_len);
//for (i=0; i<qqq_len; i++) printf("%04X ", qqq_dat[i]);
//printf("\n");

      t = localtime((time_t*)&pBlock->DataTime);
/*
      sprintf(data_time, "%4d%02d%02d %02d:%02d ", 
              t->tm_year+1900, t->tm_mon+1, t->tm_mday,
              t->tm_hour, t->tm_min);
*/
      sprintf(data_time, "%4d%02d%02d %02d:%02d:%02d ", 
              t->tm_year+1900, t->tm_mon+1, t->tm_mday,
              t->tm_hour, t->tm_min, t->tm_sec);
      printf("%s %s Dallas bus %d\n", 
             data_time, NodeName[pBlock->NodeAddress], bus);
      for (i=0; i<n; i++) {
        if (ok[i]) printf("%2d %+7.2f %6d\n", i, tmp[i], age[i]);
        else       printf("%2d         %6d\n", i, age[i]);
      }
    }
    return TRUE;
  }

//~---

  if (is_it(pBlock, "USCM", 0x13)) {
    int8  adr;
    int16 dat;
    if (P > 1) {
      printf("p = %08X, pBlock->DataCount = %d\n", (int32)RP_dat, pBlock->DataCount);
    }

    if (RP_cnt != 3) {
      if (P > 1) printf("%s*** Error: Wrong RP_cnt = %d (3)\n", ident, RP_cnt);
      return FALSE;
    }

    adr = RP_dat[0];
    dat = (int16)RP_dat[1]<<8 | (int16)RP_dat[2];

    printf("%sLVDS: adr = %02hX, dat n = %04X\n", ident, adr, dat);
    return TRUE;
  }

//~---

  if (P > 1) {
    printf("%s*** Warning: Unknown DataType %02X in %s\n", header, pBlock->DataType, me);
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool process_JLV1_data(AMSBlock *pBlock, int16 flag) {

  char *me = {"process_JLV1_data"};
  int i;
  char ident[80];
  int16 *dat = pBlock->Data.p16;
  int16  len = pBlock->DataCount / 2;
  int16  sta = dat[len-1];
  int16  cod = unpack16(sta, 0xF800);
  int16  mod = unpack16(sta, 0x00C0);
  static long long old_lv1_time = -1;
  static long old_event_id = -1;
  static int64 old__lv1_time = 0;
  static int old_p0 = -1, old_t0 = 0, old_p1 = -1, old_t1 = 0;
  int p0, t0, p1, t1;
  float l0, l1;
  float period[4] = {0.25, 0.50, 1.00, 2.00};
  int regs[4] = {0x34, 0x33, 0x36, 0x35};
  char *nam0[] = {
    "FTC",        "FTCP0",     "FTCP1",      "FTCT0", 
    "FTCT1",      "FTZ",       "FTE",        "ACC0", 
    "ACC1",       "BZ",        "ECAL-F_and", "ECAL-F_or",
    "ECAL-A_and", "ECAL-A_or", "EXT-GATE0",  "EXT-GATE1"};
  char *nam1[] = {
    "Sub0", "Sub1", "Sub2", "Sub3", 
    "Sub4", "Sub5", "Sub6", "Sub7", 
    "LA-0", "LA-1", "?",     "DSP",
    "Gen",  "??",   "???",   "????"};
  
//~------

  sprintf(ident,  "%s         ", header);

//~------


  if (pBlock->DataType == 0x01) {
  
    if (data_format_version == 1) {
      if (cod != 16) {
        return FALSE;
      }
    }

    if (!(flag & NO_EVENT_NUMBER)) event_ids[pBlock->NodeAddress] = *dat++;

//~---

    switch (mod) {
      case OFF_MODE:
        lv1_time = -1;
      break;
      case RAW_MODE:
        lv1_time = (int64)(dat[5] & 0x00FF) << 32 | 
                   (int64)(dat[4] & 0xFFFF) << 16 | 
                   (int64)(dat[3] & 0xFFFF) <<  0;
      break;
      case COMPRESSED_MODE:
        lv1_time = (int64)(dat[6] & 0x00FF) << 32 | 
                   (int64)(dat[5] & 0xFFFF) << 16 | 
                   (int64)(dat[4] & 0xFFFF) <<  0;
      break;
      case MIXED_MODE:
        lv1_time = -1;
      break;
    }

    if (lv1_time > 0     &&
        old_event_id > 0 && 
        ((event_id - old_event_id) == 1)) {
      delta_T = (lv1_time - old_lv1_time) * 0.640 + 0.5;
      if (delta_T > 1000000) {
        printf("%s event %5d (%08X) --> Big delta_T = %4.1f sec\n", 
//              header, event_id, event_id, (float)delta_T/1000000.0);
                header, event_id, event_id, delta_T/1000000.0);
      }
    }
    else {
      delta_T = -1.0;
    }
    old_lv1_time = lv1_time;
    old_event_id = event_id;

//~---

    if ((P > 2) || strcasestr(PP, "trig")) {
      int32 pattern;
      static bool first = TRUE;
      if (first) {
        int j;
        first = FALSE;
        memset(list, -1, sizeof(list));
        printf("Trigger Setting:\n");
        for (j=0; j<8; j++) {
          int k = 0x1F + j;
          printf("Sub%d: 0x%02X=%04X= ", j, k, dat[k]);
          for (i=1; i<16; i++) if (~dat[k] & (1 << (i-1))) printf("%s ", nam0[i]);
          printf("\n");
        }
      }
      if (P > 2) {
        printf("%sTrigger: run=%08X event=%-5d", ident, run_id, event_id);
        printf(" : 0x00=%04X= ", dat[0x00]);
        for (i=0; i<16; i++) if (dat[0x00] & (1 << i)) printf("%s ", nam0[i]);
        printf(" : 0x01=%04X = ", dat[0x01]);
        for (i=0; i<16; i++) if (dat[0x01] & (1 << i)) printf("%s ", nam1[i]);
        printf("\n");
      }

      pattern = dat[0x00] + ((int32)dat[1] << 16);
      for (i=0; i<DIM(list); i++) {
        if (list[i].pattern == pattern) {
          list[i].n++;
          break;
        }
        if (list[i].n < 0) {
          list[i].pattern = pattern;
          list[i].n   = 1;
          break;
        }
      }
      if (i >= DIM(list)) {
        printf("*** Error: List overflow\n");
        print_file_summary();
        exit(1);
      }
    }

//~---

    if (strcasestr(PP, "J")) {
      p0 = unpack16(dat[47], 0x3000);
      t0 = ((int32)unpack16(dat[47], 0x07FF) << 16) + dat[48];
      l0 = (float)t0 * 0.000000020 / period[p0];
      p1 = unpack16(dat[49], 0x3000);
      t1 = ((int32)unpack16(dat[49], 0x07FF) << 16) + dat[50];
      l1 = (float)t1 * 0.000000020 / period[p1];

      if (old_p0 != p0 || old_t0 != t0 || old_p1 != p1 || old_t1 != t1) {
        printf("LiveTime: ");
        for (i=47; i<51; i++) printf("0x%02X=%04X ", regs[i-47], dat[i]);
        printf("p0 = %d, period0 = %4.2f, t0 = %8d, l0 = %4.2f ", p0, period[p0], t0, l0);
        printf("p1 = %d, period1 = %4.2f, t1 = %8d, l1 = %4.2f ", p1, period[p1], t1, l1);
        if (old__lv1_time) printf("%5.2f ", (lv1_time - old__lv1_time) * 0.000000640);
        printf("\n");
        old_p0 = p0;
        old_t0 = t0;
        old_p1 = p1;
        old_t1 = t1;
        old__lv1_time = lv1_time;
      }
      return TRUE;
    }
//~---

    return TRUE;
  }

//~------

#if 0
  if (pBlock->DataType == 0x0C) {
    return TRUE;
  }
#endif

//~------

  if (P > 1) {
    printf("%s*** Warning: Unknown DataType %02X in %s\n", header, pBlock->DataType, me);
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool process_ETRG_data(AMSBlock *pBlock, int16 flag) {

//print_AMS_block_header(1+ident_1, pBlock);

  return TRUE;
}

//~----------------------------------------------------------------------------

bool process_JINJ_data(AMSBlock *pBlock, int16 flag) {

//print_AMS_block_header(1+ident_1, pBlock);

  return TRUE;
}

//~----------------------------------------------------------------------------

bool process_JINF_data(AMSBlock *pBlock, int16 flag) {

//print_AMS_block_header(1+ident_1, pBlock);

  return TRUE;
}

//~----------------------------------------------------------------------------

bool process_UDR_data(AMSBlock *pBlock, int16 flag) {

//print_AMS_block_header(1+ident_1, pBlock);

  return TRUE;
}

//~----------------------------------------------------------------------------

bool process_SDR_data(AMSBlock *pBlock, int16 flag) {

  char *me = {"process_SDR_data"};
  typedef struct {
    int    len;
    int16 *dat;
  } _dat;
  int i, j;
  char ident[80];
  int16 *dat = pBlock->Data.p16;
  int16  len = pBlock->DataCount / 2;
  int16  sta = dat[len-1];
  int16  cod = unpack16(sta, 0xF800);
//int16  mod = unpack16(sta, 0x00C0);
  static _dat *charge = NULL, *pretrig = NULL, *time = NULL, *status = NULL;
  static bool first = TRUE;
  float temp[5];

  typedef struct {
    int32 dat[1024];
    int32 header;
    int32 trailer;
    int32 error;
    int   cnt;
    int32 hit[1024];
  } _tdc;

  static _tdc *tdc[5] = {NULL, NULL, NULL, NULL, NULL};
  char *toflinks[] = {"SFET2-A", "SFET2-B", "SFET2-C", "SFET2-D", "SFEA2"};
  
//~------

  sprintf(ident,  "%s         ", header);

//~------


  if (pBlock->DataType == 0x01) {
  
    if (!strcasestr(PP, "S")) return TRUE;

    if (data_format_version == 1 && cod != 16) return FALSE;

    if (!(flag & NO_EVENT_NUMBER)) event_ids[pBlock->NodeAddress] = *dat++;

//~---

    sprintf(ident, "%s         %s: ", header, NodeName[pBlock->NodeAddress]);

    if (first) {
      int i;
      first = FALSE;
      charge  = malloc(sizeof(_dat));
      pretrig = malloc(sizeof(_dat));
      time    = malloc(sizeof(_dat));
      status  = malloc(sizeof(_dat));
      for (i=0; i<5; i++) {
        tdc[i] = malloc(sizeof(_tdc));
      }
    }
  
    if (P > 1) {
      char b[80];
      printf("%slen=%3d sta=%04X", ident, len, sta);
      sprintf_binary(b, sta, -16);
      printf("= %s\n", b);
    }
  
    charge->len  = 90;
    pretrig->len =  4;
    status->len  = 10;
    time->len    = len - 1 - charge->len - pretrig->len - status->len;
  
    if (P > 1) {
      printf("%scharge->len=%d pretrig->len=%d status->len=%d time->len=%d ",
              ident, charge->len, pretrig->len, status->len, time->len);
      if (time->len <= 0) printf("*** Error: Wrong Length\n");
      else                printf("\n");
    }
  
    charge->dat  = &dat[0];
    pretrig->dat = charge->dat  + charge->len;
    time->dat    = pretrig->dat + pretrig->len;
    status->dat  = time->dat    + time->len;

//~---

    if (P > 1) {
      printf("%sCharge:\n", ident);
      for (j=0; j<10; j++) {
        printf("%s  ", ident);
        for (i=9*j; i<9*j+9; i++) printf("%04X ", charge->dat[i]);
        printf("\n");
      }
    }

//~---

    if (P > 1) {
      printf("%sPretrigger (%04X): ", ident, status->dat[1]);
      for (i=0; i<pretrig->len; i++) printf("%04X  ", pretrig->dat[i]);
      printf("\n");
    }

//~---

    if (time->len % 2) {
      if (P > 1) printf("%s *** Error: Time Length is odd\n", ident);
      return FALSE;
    }

    for (i=0; i<5; i++) temp[i] = -273.2;
    for (i=0; i<5; i++) tdc[i]->header  = 0x00000000;
    for (i=0; i<5; i++) tdc[i]->trailer = 0x00000000;
    for (i=0; i<5; i++) tdc[i]->error   = 0x00000000;
    for (i=0; i<5; i++) tdc[i]->cnt     = 0;
  
    if (P > 2) printf("Raw Time Info:\n");
    for (i=0; i<time->len;) {
      int32 dat;
      int key, lnk, cha;
      dat  = time->dat[i++] << 16;
      dat |= time->dat[i++];
      key = unpack32(dat, 0xF0000000);
      lnk = unpack32(dat, 0x0F000000);
      cha = unpack32(dat, 0x00E00000);
      if (lnk > 4) {
        if (P > 1) printf("%s *** Error: wrong TDC link: key=%d lnk=%d\n", 
                          ident, key, lnk);
        continue;
      }
      if (P > 2) {
        printf("%3d: ", i/2);
        for (j=0; j<lnk; j++) printf("                        ");
      }
      if      (key == 0x2) {
        tdc[lnk]->header = dat;
        if (P > 2) printf("header(%d)=%08X\n", lnk, dat);
      }
      else if (key == 0x3) {
        tdc[lnk]->trailer = dat;
        if (P > 2) printf("trailer(%d)=%08X\n", lnk, dat);
      }
      else if (key == 0x4) {
        tdc[lnk]->hit[tdc[lnk]->cnt++] = dat;
        if (P > 2) printf("hit(%d,%d)=%08X\n", lnk, cha, dat);
      }
      else if (key == 0x6) {
        tdc[lnk]->error = dat;
        if (P > 2) printf("error(%d)=%08X\n", lnk, dat);
      }
      else if (key == 0x8) {
        int t_1 = unpack32(dat, 0x00FFF000);
        int t_2 = unpack32(dat, 0x00000FFF);
        temp[lnk] = 235.0 - 400.0 * t_1 / t_2;
        if (P > 2) printf("temperature(%d)=%08X\n", lnk, dat);
      }
      else {
        if (P > 1) printf("%s *** Error: wrong TDC key: key=%d lnk=%d\n", 
                          ident, key, lnk);
        continue;
      }
    }

//~---

    if (P > 1) {
      printf("%sTemperature: ", ident);
      for (i=0; i<5; i++) {
        if (temp[i] > -200.0) printf("%s:%-+.1f ", toflinks[i], temp[i]);
        else                  printf("%s:      ",  toflinks[i]);
      }
      printf("\n");
    }

//~---

    if (P > 1) printf("%sTDC:\n", ident);
    for (i=0; i<5; i++) {
      if (P > 1) {
        printf("%s  %-7s: ", ident, toflinks[i]);
        printf("%08X ", tdc[i]->header);
        printf("%2d: ", tdc[i]->cnt);
        for (j=0; j<tdc[i]->cnt; j++) {
          printf("ch%d ",  unpack32(tdc[i]->hit[j], 0x00E00000));
          printf("%4d ", unpack32(tdc[i]->hit[j], 0x0007FFFF));
        }
        printf("%08X ", tdc[i]->trailer);
      }
      if (!tdc[i]->cnt || !tdc[i]->header || !tdc[i]->trailer || tdc[i]->error) {
        if (P > 1) printf("*** Error in TDC: ");
        if (!tdc[i]->cnt) {
          if (P > 1) printf("No hits, ");
        }
        if (!tdc[i]->header) {
          if (P > 1) printf("Header missing, ");
        }
        if (!tdc[i]->trailer) {
          if (P > 1) printf("Trailer missing, ");
        }
        if (tdc[i]->error) {
          if (P > 1) printf("TDC Error %08X, ", tdc[i]->error);
        }
      }
      if (P > 1) printf("\n");
    }

//~---

    if (P > 1) {
      printf("%sStatus:\n", ident);
      printf("%s  ", ident);
      printf("   TIMEOUT");
      printf(" SPT_CMD_H");
      printf(" SPT_CMD_L");
      printf(" SFE_CMD_H");
      printf(" SFE_CMD_L");
      printf("  SH_DELAY");
      printf(" ADC_START");
      printf("  PWR_STAT");
      printf(" PRIMELINE");
      printf(" DATA_CNTR\n");
      printf("%s  ", ident);
      for (i=0; i<status->len; i++) printf("      %04X", status->dat[i]);
      printf("\n");
    }

    return TRUE;
  }
//~------

#if 0
  if (pBlock->DataType == 0x0C) {
    return TRUE;
  }
#endif

//~------

  if (P > 1) {
    printf("%s*** Warning: Unknown DataType %02X in %s\n", header, pBlock->DataType, me);
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool process_TDR_data(AMSBlock *pBlock, int16 flag) {

  char *me = {"process_TDR_data"};
  int16 *dat = pBlock->Data.p16;
  int16  len = pBlock->DataCount / 2;
  int16  sta = dat[len-1];
  int16  mod = unpack16(sta, 0x00C0);
  
//print_AMS_block_header(1+ident_1, pBlock);

//print_AMS_data_block("TDR", pBlock);

//~------

  if (pBlock->DataType == 0x01) {

    if (!(flag & NO_EVENT_NUMBER)) event_ids[pBlock->NodeAddress] = *dat++;

    switch (mod) {
      case OFF_MODE:
      break;
      case RAW_MODE:
      break;
      case COMPRESSED_MODE:
      break;
      case MIXED_MODE:
      break;
    }

    return TRUE;
  }

//~------

  if (P > 1) {
    printf("%s*** Warning: Unknown DataType %02X in %s\n", header, pBlock->DataType, me);
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool process_RDR_data(AMSBlock *pBlock, int16 flag) {

//print_AMS_block_header(1+ident_1, pBlock);
  
  return TRUE;
}

//~----------------------------------------------------------------------------

bool process_EDR_data(AMSBlock *pBlock, int16 flag) {

//print_AMS_block_header(1+ident_1, pBlock);

  return TRUE;
}

//~============================================================================

bool process_AMS_block(AMSBlock *pBlock, int16 flag) {

  char *me = {"process_AMS_block"};

  if (this_is_event) {
    length[pBlock->NodeAddress] = pBlock->DataCount / 2 + 1;
    events[pBlock->NodeAddress] += 1;
  }

  if (strcasestr(NodeName[pBlock->NodeAddress], "JMDC")) {
    process_JMDC_data(pBlock);
    return TRUE;
  }

  if (strcasestr(NodeName[pBlock->NodeAddress], "USCM")) {
    process_USCM_data(pBlock);
    return TRUE;
  }
  
  if (strcasestr(NodeName[pBlock->NodeAddress], "JLV1")) {
    process_JLV1_data(pBlock, flag);
    return TRUE;
  }

  if (strcasestr(NodeName[pBlock->NodeAddress], "ETRG")) {
    process_ETRG_data(pBlock, flag);
    return TRUE;
  }
  
  if (strcasestr(NodeName[pBlock->NodeAddress], "JINJ")) {
    process_JINJ_data(pBlock, flag);
    return TRUE;
  }

  if (strcasestr(NodeName[pBlock->NodeAddress], "JINF")) {
    process_JINF_data(pBlock, flag);
    return TRUE;
  }

  if (strcasestr(NodeName[pBlock->NodeAddress], "UDR")) {
    process_UDR_data(pBlock, flag);
    return TRUE;
  }

  if (strcasestr(NodeName[pBlock->NodeAddress], "SDR")) {
    process_SDR_data(pBlock, flag);
    return TRUE;
  }

  if (strcasestr(NodeName[pBlock->NodeAddress], "TDR")) {
    process_TDR_data(pBlock, flag);
    return TRUE;
  }
  
  if (strcasestr(NodeName[pBlock->NodeAddress], "RDR")) {
    process_RDR_data(pBlock, flag);
    return TRUE;
  }
  
  if (strcasestr(NodeName[pBlock->NodeAddress], "EDR")) {
    process_EDR_data(pBlock, flag);
    return TRUE;
  }
  
  if (P > 0) print_err(99, me);
  return FALSE;
}

//~============================================================================

bool unpack_data_group(int16 *dat, int16 len, _group *group) {

  int16 *p = dat, *end = dat + len - 1;
  char *ident = group->ident;
  struct {
    char *l;
    char *s;
  } erro[32] = {
    {"0?",                  "0?"},
    {"NXT_reply_received",  "NXT1"},
    {"ABO_reply_received",  "ABO"}, 
    {"ERR_reply_received",  "ERR"}, 
    {"END_reply_received",  "END"}, 
    {"Timeout",             "TOUT1"},
    {"AMSW_error",          "AMSW"}, 
    {"7?",                  "7?"},
    {"8?",                  "8?"},
    {"9?",                  "9?"},
    {"10?",                 "10?"},
    {"11?",                 "11?"},
    {"12?",                 "12?"},
    {"13?",                 "13?"},
    {"14?",                 "14?"},
    {"15?",                 "15?"},
    {"OK??????",            "OK?"}, 
    {"BC_err_1",            "BC1"},
    {"No_room_1",           "ROOM1"},
    {"No_data",             "DATA"},
    {"Event_#_mismatch_1",  "EV#1"}, 
    {"Wrong_CRC_1",         "CRC1"},
    {"NXT_timeout",         "TOUT2"},
    {"0_length_to_NXT",     "NXT2"},
    {"BC_err_2",            "BC2"},
    {"No_room_2",           "ROOM2"},
    {"No_room_3",           "ROOM3"},
    {"Event_#_mismatch_2",  "EV#2"},
    {"Wrong CRC_2",         "CRC2"},
    {"29?",                 "29?"},
    {"30?",                 "30?"},
    {"31?",                 "31?"}};

//~---
  
#if 0
int i;
for (i=0; i<len; i++) printf("%04X%s", dat[i], !((i+1)%10) ? "\n" : " ");
printf("\n");
#endif

  if (P > 1) {
    printf("%sData Group (%s):len = %04X = %d(words) = %d(bytes) ", 
            ident, NodeName[group->cddc->adr], len, len, 2*len);
  }
  group->sta = *end;
  if (P > 1) {
    char b[80];
    printf("sta=%04X", group->sta);
    sprintf_binary(b, group->sta, -16);
    printf(" = %s ", b);
  }

  if (data_format_version == 0) {
    if (group->sta & group->chk) {
      if (P > 1) printf("*** Error: one/several ERROR bits set in group status");
      mark_error(group->cddc->adr, "sta");
    }
    if (P > 1) printf("\n");
  }

  if (data_format_version == 1) {
    if (group->sta & (BUILDING_ERRORS | SEQUENCER_ERRORS)) {
      if (P > 1) printf("*** Error: ");
      if (group->sta & BUILDING_ERRORS) {
        if (P > 1) printf("BUILDING_ERRORS ");
        mark_error(group->cddc->adr, "bld");
      }
      if (group->sta & SEQUENCER_ERRORS) {
        if (P > 1) printf("SEQUENCER_ERRORS ");
        mark_error(group->cddc->adr, "seq");
      }
    }

    if (!group->top) {
      int16 code = unpack16(group->sta, 0xF800);
      if (code != 16) {
        mark_error(group->cddc->adr, erro[code].s);
        if (P > 1) {
          printf("*** Error: %s", erro[code].l);
          if (code == 6) {
            printf(": RX_sta=%04X RX_adr=%04X", dat[0], dat[1]);
          }
          if (code > 16) {
            printf(": RX_sta=%04X len=%d frst=%04X ", dat[0], dat[1], dat[2]);
            if (this_is_event) printf("[%06X] ", event_id);
          }
          printf("\n");
        }
        return FALSE;
      }
    }
    if (P > 1) printf("\n");
  }

  group->msk = 0x000000;
  group->err = 0x000000;

//~---

  while (1) {
    int16 lnk;
    int32 m;
    if (p == end) break;
    if (p > end) {;
      if (P > 1) printf("%s*** Error: Pointer mismatch in CDDC: p=%08X, end=%08X\n", 
                         ident, (int32)(p+2), (int32)end);
      mark_error(group->cddc->adr, "pntr");
      return FALSE;
    }

    lnk = *(p + *p) & 0x001F;
    if (P > 1) {
      if (group->cddc) {
        printf("%slnk=%2d:%-3s ", ident, lnk, 
              (lnk < 24 ? group->cddc->lnk[lnk].crt : "???"));
      }
      else              printf("%slnk=%2d ",      ident, lnk);
    }
    if (lnk > 23) {
      if (P > 1) printf(" *** Error: wrong lnk\n");
      mark_error(group->cddc->adr, "lnk");
      return FALSE;
    }

    m = 1 << lnk;
    if (P > 1) printf("msk=%06X m=%06X ", group->msk, m);
    if (m & group->msk) {
      if (P > 1) printf(" *** Error: repeated fragment: lnk=%d\n", lnk);
      mark_error(group->cddc->adr, "rpt");
      return FALSE;
    }
    group->msk         |=  m;
    group->fra[lnk].len = *p++;
    group->fra[lnk].lnk = lnk;
    group->fra[lnk].dat =  p;

    group->fra[lnk].sta = group->fra[lnk].dat[group->fra[lnk].len-1];
    if (P > 1) printf("len=%5d ", group->fra[lnk].len);
    if (P > 1) {
      char b[80];
      printf("sta=%04X", group->fra[lnk].sta);
      sprintf_binary(b, group->fra[lnk].sta, -16);
      printf("= %s ", b);
    }

    if (data_format_version == 0) {
      if (group->fra[lnk].sta & 0x8000) {
        if (group->fra[lnk].sta & 0x7E00) {
          group->err |= m;
          if (P > 1) printf("*** Error: one/several ERROR bits set");
          mark_error(group->cddc->lnk[lnk].adr, "sta");
        }
      }
      else {
        group->err |= m;
        if (P > 1) printf("*** Error: no DATA bit set");
        mark_error(group->cddc->lnk[lnk].adr, "sta");
      }
      if (P > 1) printf("\n");
    }

    if (data_format_version == 1) {
      int16 code = unpack16(group->fra[lnk].sta, 0xF800);
      if (code != 16) {
        mark_error(group->cddc->lnk[lnk].adr, erro[code].s);
        if (P > 1) {
          printf("*** Error: %s", erro[code].l);
          if (code == 6) {
            printf(": RX_sta=%04X RX_adr=%04X", group->fra[lnk].dat[0], group->fra[lnk].dat[1]);
          }
          if (code > 16) {
            printf(": RX_sta=%04X len=%d frst=%04X ", 
                   group->fra[lnk].dat[0], group->fra[lnk].dat[1], group->fra[lnk].dat[2]);
            if (this_is_event) printf("[%06X] ", event_id);
          }
        }
      }
      if (P > 1) printf("\n");
    }

    p = p + group->fra[lnk].len;
  }

  if (group->err) return FALSE;
  else            return TRUE;
}

//~----------------------------------------------------------------------------

bool unpack_JINJ_block(AMSBlock *pBlock) {

//char *me = {"unpack_JINJ_block"};
  bool debug = FALSE;
  int i;
  int16 *dat, len, CRC;
  char ident[80];
  _group group, *g = &group;

//~---
  
//print_AMS_block_header(1+ident_1, pBlock);
  
//~---


  if (P > 9) print_AMS_data_block("", pBlock);

//if (pBlock->DataType == 0x13) print_AMS_data_block("", pBlock);


  sprintf(ident,  "%s     ", header);

  dat =  pBlock->Data.p16;
  len = (pBlock->DataCount + 1) / 2;
  length[pBlock->NodeAddress] = len + 1;
  events[pBlock->NodeAddress] += 1;
  CRC = calculate_CRC16(dat, len);
  len--;
  if (P > 1) printf("%sCRC = %04X ", ident, CRC);
  if (CRC) {
    if (P > 1) printf("*** Error: wrong CRC\n");
    mark_error(pBlock->NodeAddress, "crc");
    return FALSE;
  }
  if (P > 1) printf("\n");

  if (pBlock->DataType == 0x01) {
    event_ids[pBlock->NodeAddress] = *dat++;
    len--;
    if (P > 1) printf("%sevent_id = %04X = %d\n", ident, event_ids[pBlock->NodeAddress], event_ids[pBlock->NodeAddress]);
  }

  sprintf(ident,  "%s       ", header);

  g->ident = ident;
  g->chk   = 0xFE00;
  g->top   = TRUE;
  attach_tree(pBlock->NodeAddress, &g->cddc);
  if (!g->cddc) {
    printf("%s*** Error: No Tree found (5) for name %s\n", 
            header, NodeName[pBlock->NodeAddress]);
    mark_error(pBlock->NodeAddress, "tree");
    return FALSE;
  }

  if (!unpack_data_group(dat, len, g)) return FALSE;

  for (i=0; i<24; i++) {
    sprintf(ident, "%s       ", header);
    if (g->msk & (1 << i)) {
      if (P > 1) {
        printf("%sfragment from JINJ link = %d = %s = %s:  \n", 
                ident, i, g->cddc->lnk[i].crt, g->cddc->lnk[i].nam);
      }
      if (!(index(g->cddc->lnk[i].crt, 'J') || index(g->cddc->lnk[i].crt, 'S'))) {
        int ii;
        _group group, *gg = &group;
        length[g->cddc->lnk[i].adr] = g->fra[i].len + 1;
        events[g->cddc->lnk[i].adr] += 1;
        sprintf(ident, "%s         ", header);
        gg->ident = ident;
        gg->chk   = 0x7E00;
        gg->top   = FALSE;
        attach_tree(g->cddc->lnk[i].adr, &gg->cddc);
        if (unpack_data_group(g->fra[i].dat, g->fra[i].len, gg)) {
          for (ii=0; ii<24; ii++) {
            if (gg->msk & (1 << ii)) {
              pBlock->NodeAddress = gg->cddc->lnk[ii].adr;
              pBlock->DataCount   = gg->fra[ii].len * 2;
              pBlock->Data.p16    = gg->fra[ii].dat;
if (debug) printf("%-20s ", NodeName[pBlock->NodeAddress]); 
if (debug) print_AMS_data_block("", pBlock);
              process_AMS_block(pBlock, NO_EVENT_NUMBER);
            }
          }
        }
        else {
        }
if (debug) printf("\n");
      }
      else {
        pBlock->NodeAddress = g->cddc->lnk[i].adr;
        pBlock->DataCount   = g->fra[i].len * 2;
        pBlock->Data.p16    = g->fra[i].dat;
if (debug) printf("%-20s ", NodeName[pBlock->NodeAddress]); 
if (debug) print_AMS_data_block("", pBlock);
if (debug) printf("\n");
        process_AMS_block(pBlock, NO_EVENT_NUMBER);
      }
    }
  }

//~---

  return TRUE;
}

//~----------------------------------------------------------------------------

bool unpack_JINF_block(AMSBlock *pBlock) {

//char *me = {"unpack_JINF_block"};
  bool debug = FALSE;
  int i;
  int16 *dat, len, CRC, event_id;
  char ident[80];
  _group group, *g = &group;
  
//print_AMS_block_header(1+ident_1, pBlock);

  if (P > 9) print_AMS_data_block("", pBlock);

  sprintf(ident, "%s     ", header);

  dat =  pBlock->Data.p16;
  len = (pBlock->DataCount + 1) / 2;
  length[pBlock->NodeAddress] = len + 1;
  events[pBlock->NodeAddress] += 1;
  CRC = calculate_CRC16(dat, len);
  len--;
  if (P > 1) printf("%sCRC = %04X ", ident, CRC);
  if (CRC) {
    if (P > 1) printf("*** Error: wrong CRC\n");
    mark_error(pBlock->NodeAddress, "crc");
    return FALSE;
  }
  if (P > 1) printf("\n");

  if (pBlock->DataType == 0x01) {
    event_id = *dat++;
    if (P > 1) printf("%sevent_id = %04X = %d\n", ident, event_id, event_id);
    len--;
  }

  g->ident = ident;
  g->chk   = 0x7E00;
  g->top   = TRUE;
  attach_tree(pBlock->NodeAddress, &g->cddc);
  if (!g->cddc) {
    printf("%s*** Error: No Tree found (4) for name %s\n", 
              header, NodeName[pBlock->NodeAddress]);
    mark_error(pBlock->NodeAddress, "tree");
    return FALSE;
  }

  if (!unpack_data_group(dat, len, g)) return FALSE;

  for (i=0; i<24; i++) {
    if (g->msk & (1 << i)) {
      pBlock->NodeAddress = g->cddc->lnk[i].adr;
      pBlock->DataCount   = g->fra[i].len * 2;
      pBlock->Data.p16    = g->fra[i].dat;
if (debug) printf("%-20s ", NodeName[pBlock->NodeAddress]); 
if (debug) print_AMS_data_block("", pBlock);
      process_AMS_block(pBlock, NO_EVENT_NUMBER);
    }
  }
if (debug) printf("\n");

  return TRUE;
}

//~----------------------------------------------------------------------------

bool unpack_AMSW_envelope_block(AMSBlock *pBlock) {

//char *me = {"process_AMSW_envelope_block"};
  bool debug = FALSE;
  int i;
  char ident[80];
  int16 *p, *end, len;
  int16 lnk, prt, link = 0, port, CRC;
  int16 *path;
  int step;
  char adr[3] = {'?', '?', '?'};
  _tree *node[3] = {NULL, NULL, NULL};
  int address[3];
  int32 typ = 0x00FFFFFF;

  if (P > 9) print_AMS_data_block("", pBlock);

  sprintf(ident, "%s   ", header);

  path =  pBlock->Data.p16;

  p   =  pBlock->Data.p16;
  len = (pBlock->DataCount + 1) / 2;
  end = p + len - 1;

  step = 0;

  address[step] = pBlock->NodeAddress;
  attach_tree(pBlock->NodeAddress, &node[step]);
  if (!node[step]) {
    printf("%s*** Error: No Tree found (0) for name %s\n", header, NodeName[address[step]]);
    return FALSE;
  }

  while (1) {
    if (p > end) {;
      if (P > 1) printf("%s*** Error: Pointer mismatch 0x1F0381: p=%08X, end=%08X\n", 
                       ident, (int32)(p+2), (int32)end);
      mark_error(pBlock->NodeAddress, "pntr");
      return FALSE;
    }
    if (step > 2) {
      if (P > 1) printf("%s*** Error: Path too long\n", ident);
      mark_error(pBlock->NodeAddress, "path");
      return FALSE;
    }
    lnk = unpack16(*p  , 0xFF00);
    prt = unpack16(*p++, 0x00FF);
    if (lnk < 23 && (prt == 0x04 || prt == 0x05 || prt == 0x3F)) {
      link = lnk;
      port = prt;
      adr[step++] = 'S';
      if (node[step-1]) {
        address[step] = node[step-1]->lnk[link].adr;
        if (strcasestr(NodeName[address[step]], "JIN")) {
          attach_tree(address[step], &node[step]);
          if (!node[step]) {
            printf("%s*** Error: No Tree found (1) for name %s\n", header, NodeName[address[step]]);
            mark_error(address[step], "tree");
            return FALSE;
          }
          printf("%sstep = %d, node[step]->nam = %s \n", header, step, node[step]->nam);
        }
      }
    }
    else if (lnk == '*' || lnk == '@') {
      adr[step++] = 'G';
    }
    else if (lnk == '#') {
      adr[step++] = 'G';
      p++;
    }
    else if (lnk == '.') {
      adr[step++] = '.';
      typ = prt;
      if (typ == 0x1F) {
        len--;
        typ = typ << 16 | *p++;
      }
      break;
    }
    else {
      if (P > 1) printf("%s*** Error: Wrong Path(2)\n", ident);
      mark_error(pBlock->NodeAddress, "path");
      return FALSE;
    }
  }

  if (P > 1) {
    printf("\n%sReply to AMS-Wire command ", ident);
    printf("Path = ");
    for (i=0; i<5; i++) {
      printf("%04X ", path[i]);
      if (unpack16(path[i], 0xFF00) == '.') break;
      if (unpack16(path[i], 0xFF00) == '#') printf("%04X ", path[++i]);
    }
    printf("DataType = %02X\n", typ);
  }
  
  len = end - p + 1;
  CRC = calculate_CRC16(p, len);
  len--;
  if (P > 1) printf("%sCRC = %04X ", ident, CRC);
  if (CRC) {
    if (P > 1) printf("*** Error: wrong CRC\n");
    mark_error(pBlock->NodeAddress, "crc");
    return FALSE;
  }
  if (P > 1) printf("\n");

  if (P > 1) {
    printf("%s************** step=%d ", ident, step);
    for (i=0; i<step; i++) printf("%c", adr[i]);
    printf(" typ=%02X, len=%d\n", typ, len);
  }

  if ((step == 1 && adr[0] == '.')                  ||
      (step == 2 && adr[0] == 'S' && adr[1] == '.') ||
      (step == 3 && adr[0] == 'S' && adr[1] == 'S' && adr[2] == '.')) {
    if (P > 1) {
      printf("%sPath: JMDC ", ident);
      for (i=0; i<step; i++) printf("==> %s ", NodeName[address[i]]);
      printf("\n");
    }
    pBlock->NodeAddress = address[step-1];
    pBlock->DataType    = typ;
    pBlock->DataCount   = len * 2;
    pBlock->Data.p16    = p;
if (debug) printf("%-20s ", NodeName[pBlock->NodeAddress]); 
if (debug) print_AMS_data_block("", pBlock);
if (debug) printf("\n");
    process_AMS_block(pBlock, 0);
    return TRUE;
  }

  if ((step == 2 && adr[0] == 'G' && adr[1] == '.') ||
      (step == 3 && adr[0] == 'S' && adr[1] == 'G' && adr[2] == '.')) {
    _group group, *g = &group;
    sprintf(ident,  "%s     ", header);
    g->ident = ident;
    g->chk   = 0x0000;
    g->top   = TRUE;
    g->cddc  = node[step-2];
    unpack_data_group(p, len, g);
    for (i=0; i<24; i++) {
      if (g->msk & (1 << i)) {
        pBlock->NodeAddress = g->cddc->lnk[i].adr;
        pBlock->DataType    = typ;
        pBlock->DataCount   = g->fra[i].len * 2;
        pBlock->Data.p16    = g->fra[i].dat;
if (debug) printf("%-20s ", NodeName[pBlock->NodeAddress]); 
if (debug) print_AMS_data_block("", pBlock);
        process_AMS_block(pBlock, 0);
      }
    }
if (debug) printf("\n");
    return TRUE;
  }

  if (step == 3 && adr[0] == 'G' && adr[1] == 'G' && adr[2] == '.') {
    _group group, *g = &group;
    sprintf(ident,  "%s     ", header);
    g->ident = ident;
    g->chk   = 0x0000;
    g->top   = TRUE;
    g->cddc  = node[step-3];
    unpack_data_group(p, len, g);
    for (i=0; i<24; i++) {
      if (g->msk & (1 << i)) {
        int ii;
        _group group, *gg = &group;
        sprintf(ident,  "%s       ", header);
        gg->ident = ident;
        gg->chk   = 0x0000;
        gg->top   = FALSE;
        attach_tree(g->cddc->lnk[i].adr, &gg->cddc);
        if (!gg->cddc) {
          printf("%s*** Error: No Tree found (2) for name %s\n", header, g->cddc->lnk[i].nam);
          mark_error(g->cddc->lnk[i].adr, "tree");
          return FALSE;
        }
        unpack_data_group(g->fra[i].dat, g->fra[i].len, gg);
        for (ii=0; ii<24; ii++) {
          if (gg->msk & (1 << ii)) {
            pBlock->NodeAddress = gg->cddc->lnk[ii].adr;
            pBlock->DataType    = typ;
            pBlock->DataCount   = gg->fra[ii].len * 2;
            pBlock->Data.p16    = gg->fra[ii].dat;
if (debug) printf("%-20s ", NodeName[pBlock->NodeAddress]); 
if (debug) print_AMS_data_block("", pBlock);
            process_AMS_block(pBlock, 0);
          }
        }
      }
    }
if (debug) printf("\n");
    return TRUE;
  }

  if (step == 3 && adr[0] == 'G' && adr[1] == 'S' && adr[2] == '.') {
    _group group, *g = &group;
    sprintf(ident,  "%s     ", header);
    g->ident = ident;
    g->chk   = 0x0000;
    g->top   = TRUE;
    g->cddc  = node[step-3];
    unpack_data_group(p, len, g);
    for (i=0; i<24; i++) {
      if (g->msk & (1 << i)) {
        _group group, *gg = &group;
        attach_tree(g->cddc->lnk[i].adr, &gg->cddc);
        if (!gg->cddc) {
          printf("%s*** Error: No Tree found (3) for name %s\n", header, g->cddc->lnk[i].nam);
          mark_error(g->cddc->lnk[i].adr, "tree");
          return FALSE;
        }
        pBlock->NodeAddress = gg->cddc->lnk[link].adr;
        pBlock->DataType    = typ;
        pBlock->DataCount   = g->fra[link].len * 2;
        pBlock->Data.p16    = g->fra[link].dat;
if (debug) printf("%-20s ", NodeName[pBlock->NodeAddress]); 
if (debug) print_AMS_data_block("", pBlock);
        process_AMS_block(pBlock, 0);
      }
    }
if (debug) printf("\n");
    return TRUE;
  }

  if (P > 1) printf("%s*** Error: Wrong Path(2)\n", ident);
  mark_error(pBlock->NodeAddress, "path");
  return FALSE;
}

//~----------------------------------------------------------------------------

bool unpack_envelope_block(AMSBlock *pBlock, int16 flag) {

//char *me = {"unpack_envelope_block"};
  int16 *p = pBlock->Data.p16, *end = p + (pBlock->DataCount + 1) / 2 - 1;

  while (1) {
    int32 len;
    if (p > end) break;
    len = *p++;
    if (p > end) {
      if (P > 1) {
        printf("%s*** Error: Pointer mismatch in envelope: p=%08X, end=%08X\n", 
               header, (int32)(p+2), (int32)end);
      }
      mark_error(pBlock->NodeAddress, "pntr");
      return FALSE;
    }
    if (len & 0x8000) len = (len << 16) | *p++;
    if (!unpack_and_process_AMS_block(p, len, flag | INSIDE_ENVELOPE)) return FALSE;

//~--- begin special processing for "old" envelope format ---

    if (len & 1) {
      int16 *q, *s;
      pBlock->DataCount++;
      end = pBlock->Data.p16 + (pBlock->DataCount + 1) / 2 - 1;
      q = end;
      s = p + len / 2;
      while (q > s) {
        *q  = (*q >> 8) | (*(q-1) << 8);
        q--;
      }
    }

//~--- end special processing for "old" envelope format ---

    p = p + (len + 1) / 2;
  }
  
  return TRUE;
}

//~============================================================================

bool unpack_and_process_AMS_block(int16 *dat, int32 len, int16 flag) {

//char *me = {"unpack_and_process_AMS_block"};
  bool shf, inside_envelope;
  bool envelope, jinj_block, jinf_block, xdr_block;
  AMSBlock Block, *pBlock = &Block;

//~--

  shf             = (flag & SHF)             ? TRUE : FALSE;
  inside_envelope = (flag & INSIDE_ENVELOPE) ? TRUE : FALSE;

  ident_1 = inside_envelope ? 2 : 0;
  
  if (!unpack16_AMS_block(dat, len, shf, pBlock)) {
    if (P > 0) printf("%s*** Error: unpack16_AMS_block failed\n", header);
    mark_error(pBlock->NodeAddress, "blck");
    return FALSE;
  }

  if (first_block_in_file) {
    struct tm *t;
    data_format_version = (pBlock->DataTime < 0x4848EDB2) ? 0 : 1;
//  if (P > 0) printf("first block: data_format_version is set to %d\n", data_format_version);
    t = localtime((time_t*)&pBlock->DataTime);
    sprintf(first_block_time, "%4d%02d%02d %02d:%02d ", 
           t->tm_year+1900, t->tm_mon+1, t->tm_mday,
           t->tm_hour, t->tm_min);
    first_block_in_file = FALSE;
  }

  if (shf & pBlock->CommandTag & 0xF000) {
    printf("%s*** Error: CommandTag = %04X\n", header, pBlock->CommandTag);
  }

  if (pBlock->NodeAddress > 0x1FF) {
    if (P > 0) printf("%s*** Error: Node Address %03X\n", header, pBlock->NodeAddress);
    mark_error(pBlock->NodeAddress, "adr");
    return FALSE;
  }
  
  print_AMS_block_header(1+ident_1, pBlock);

  if (pBlock->RequestReply != REPLY || pBlock->ReadWrite != READ) return 0;
  
  if (is_it(pBlock, "JMDC", 0x05)) {
    this_is_event = TRUE;
    event_length = (len + 1) / 2 + 1;
  }

  envelope = 
    is_it(pBlock, "JMDC", 0x05    ) ||
    is_it(pBlock, "JMDC", 0x06    ) ||
    is_it(pBlock, "JMDC", 0x1F0380);

  if (envelope && inside_envelope) {
    if (P > 0) printf("%s*** Error: Recursive Envelope\n", header);
    mark_error(pBlock->NodeAddress, "envlp");
    return FALSE;
  }
  
  if (!inside_envelope && pBlock->DataType == 0x1F0380) shf = TRUE;
  else                                                  shf = FALSE;



/*
if (!inside_envelope && pBlock->DataType == 0x1F0380) {
P = 10;
pBlock->Data.p8 = (int8*)pBlock->Data.p16;
print_AMS_data_block("", pBlock);
printf("CommandTag = %04X\n", pBlock->CommandTag);
printf("DataTime   = %08X\n", pBlock->DataTime);
}
*/





    
  jinj_block = 
    is_it(pBlock, "JINJ", 0x01) ||
    is_it(pBlock, "JINJ", 0x13);
    
  jinf_block = 
    is_it(pBlock, "JINF", 0x01) ||
    is_it(pBlock, "JINF", 0x13);
    
  xdr_block = 
    is_it(pBlock, "EDR", 0x01) ||
    is_it(pBlock, "RDR", 0x01) ||
    is_it(pBlock, "TDR", 0x01) ||
    is_it(pBlock, "UDR", 0x01);
    
  if (envelope) {
    if (!unpack_envelope_block(pBlock, shf)) return FALSE;
    ident_1 = inside_envelope ? 2 : 0;
    return TRUE;
  }

  else if (pBlock->DataType == 0x1F0381) {
    if (!unpack_AMSW_envelope_block(pBlock)) return FALSE;
  }

  else if (jinj_block) {
    if (!unpack_JINJ_block(pBlock)) return FALSE;
  }

  else if (jinf_block) {
    if (!unpack_JINF_block(pBlock)) return FALSE;
  }

  else {
    process_AMS_block(pBlock, 0);
  }

  return TRUE;
}

//~----------------------------------------------------------------------------

void zero_file_summary(void) {

  int i, j;

  n_events = n_bad_events = 0;
  n_others = n_bad_others = 0;
  ave_event_len.sum = ave_event_len.cnt = 0;
  for (i=0; i<0x200; i++) ave_len[i].sum = ave_len[i].cnt = 0;
  for (i=0; i<0x200; i++) for(j=0; j<DIM(errors); j++) errors[j].event_cntr[i] = 0;
  for (i=0; i<0x200; i++) for(j=0; j<DIM(errors); j++) errors[j].other_cntr[i] = 0;
  for (i=0; i<0x200; i++) events[i] = 0;
}

//~----------------------------------------------------------------------------

void print_file_summary(void) {

  int i, j, k;

#if 0
  char string[] = {
//             1         2         3         4         5         6         7       
//   012345678901234567890123456789012345678901234567890123456789012345678901234567
    "JT-- JJ-- E:0--1-- R:0--1-- S:0--1--2--3-- T:0--1--2--3--4--5--6--7-- U:0--1--"};
  struct {
    char *name;
    char *mike;
    char *crt;
    int  i;
    char c;
  } obj[] = {
    {"JLV1-A",     "JA",  "JTA",  2, 'A'},
    {"JLV1-B",     NULL,  "JTB",  3, 'A'},
    {"JINJ-2",     NULL,  "JJ2",  7, '2'},
    {"JINJ-3",     NULL,  "JJ3",  8, '3'},
    {"JINF-E-0-A", "E0",  "E0A", 13, 'A'},
    {"JINF-E-0-b", "E0",  "E0B", 14, 'B'},
    {"JINF-E-1-A", "E1",  "E1A", 16, 'A'},
    {"JINF-E-1-B", "E1",  "E1B", 17, 'B'},
    {"JINF-R-0-A", "R0A", "R0A", 22, 'A'},
    {"JINF-R-0-B", "R0B", "R0B", 23, 'B'},
    {"JINF-R-1-A", "R1A", "R1A", 25, 'A'},
    {"JINF-R-1-B", "R1B", "R1B", 26, 'B'},
    {"SDR2-0-A",   "S0A", "S0A", 31, 'A'},
    {"SDR2-0-B",    NULL, "S0B", 32, 'B'},
    {"SDR2-1-A",   "S1A", "S1A", 34, 'A'},
    {"SDR2-1-B",    NULL, "S1B", 35, 'B'},
    {"SDR2-2-A",   "S2A", "S2A", 37, 'A'},
    {"SDR2-2-B",    NULL, "S2B", 38, 'B'},
    {"SDR2-3-A",   "S3A", "S3A", 40, 'A'},
    {"SDR2-3-B",    NULL, "S3B", 41, 'B'},
    {"JINF-T-0-A", "T0",  "T0A", 46, 'A'},
    {"JINF-T-0-B", NULL,  "T0B", 47, 'B'},
    {"JINF-T-1-A", "T1",  "T1A", 49, 'A'},
    {"JINF-T-1-B", NULL,  "T1B", 50, 'B'},
    {"JINF-T-2-A", "T2",  "T2A", 52, 'A'},
    {"JINF-T-2-B", NULL,  "T2B", 53, 'B'},
    {"JINF-T-3-A", "T3",  "T3A", 55, 'A'},
    {"JINF-T-3-B", NULL,  "T3B", 56, 'B'},
    {"JINF-T-4-A", "T4",  "T4A", 58, 'A'},
    {"JINF-T-4-B", NULL,  "T4B", 59, 'B'},
    {"JINF-T-5-A", "T5",  "T5A", 61, 'A'},
    {"JINF-T-5-B", NULL,  "T5B", 62, 'B'},
    {"JINF-T-6-A", "T6",  "T6A", 64, 'A'},
    {"JINF-T-6-B", NULL,  "T6B", 65, 'B'},
    {"JINF-T-7-A", "T7",  "T7A", 67, 'A'},
    {"JINF-T-7-B", NULL,  "T7B", 68, 'B'},
    {"JINF-U-0-A", "U0",  "U0A", 73, 'A'},
    {"JINF-U-8-B", NULL,  "U0B", 74, 'B'},
    {"JINF-U-1-A", "U1",  "U1A", 76, 'A'},
    {"JINF-U-1-B", NULL,  "U1B", 77, 'B'}};
#endif

#if 0
  char string[] = {
//             1         2         3         4         5         6         7       
//   0123456789012345678901234567890123456789012345678901234567890123456789012
    "JT-- JJ-- E0--1-- R0--1-- S0--1--2--3-- T0--1--2--3--4--5--6--7-- U0--1--"};
  struct {
    char *name;
    char *mike;
    char *crt;
    bool use;
    int  i;
    char c;
  } obj[] = {
    {"JLV1-A",     "JA",  "JTA", TRUE,   2, 'A'},
    {"JLV1-B",     NULL,  "JTB", TRUE,   3, 'A'},
    {"JINJ-2",     NULL,  "JJ2", FALSE,  7, '2'},
    {"JINJ-3",     NULL,  "JJ3", FALSE,  8, '3'},
    {"JINF-E-0-A", "E0",  "E0A", TRUE,  12, 'A'},
    {"JINF-E-0-b", "E0",  "E0B", TRUE,  13, 'B'},
    {"JINF-E-1-A", "E1",  "E1A", TRUE,  15, 'A'},
    {"JINF-E-1-B", "E1",  "E1B", TRUE,  16, 'B'},
    {"JINF-R-0-A", "R0A", "R0A", TRUE,  20, 'A'},
    {"JINF-R-0-B", "R0B", "R0B", TRUE,  21, 'B'},
    {"JINF-R-1-A", "R1A", "R1A", TRUE,  23, 'A'},
    {"JINF-R-1-B", "R1B", "R1B", TRUE,  24, 'B'},
    {"SDR2-0-A",   "S0A", "S0A", TRUE,  28, 'A'},
    {"SDR2-0-B",    NULL, "S0B", TRUE,  29, 'B'},
    {"SDR2-1-A",   "S1A", "S1A", TRUE,  31, 'A'},
    {"SDR2-1-B",    NULL, "S1B", TRUE,  32, 'B'},
    {"SDR2-2-A",   "S2A", "S2A", TRUE,  34, 'A'},
    {"SDR2-2-B",    NULL, "S2B", TRUE,  35, 'B'},
    {"SDR2-3-A",   "S3A", "S3A", TRUE,  37, 'A'},
    {"SDR2-3-B",    NULL, "S3B", TRUE,  38, 'B'},
    {"JINF-T-0-A", "T0",  "T0A", TRUE,  42, 'A'},
    {"JINF-T-0-B", NULL,  "T0B", TRUE,  43, 'B'},
    {"JINF-T-1-A", "T1",  "T1A", TRUE,  45, 'A'},
    {"JINF-T-1-B", NULL,  "T1B", TRUE,  46, 'B'},
    {"JINF-T-2-A", "T2",  "T2A", TRUE,  48, 'A'},
    {"JINF-T-2-B", NULL,  "T2B", TRUE,  49, 'B'},
    {"JINF-T-3-A", "T3",  "T3A", TRUE,  51, 'A'},
    {"JINF-T-3-B", NULL,  "T3B", TRUE,  52, 'B'},
    {"JINF-T-4-A", "T4",  "T4A", TRUE,  54, 'A'},
    {"JINF-T-4-B", NULL,  "T4B", TRUE,  55, 'B'},
    {"JINF-T-5-A", "T5",  "T5A", TRUE,  57, 'A'},
    {"JINF-T-5-B", NULL,  "T5B", TRUE,  58, 'B'},
    {"JINF-T-6-A", "T6",  "T6A", TRUE,  60, 'A'},
    {"JINF-T-6-B", NULL,  "T6B", TRUE,  61, 'B'},
    {"JINF-T-7-A", "T7",  "T7A", TRUE,  63, 'A'},
    {"JINF-T-7-B", NULL,  "T7B", TRUE,  64, 'B'},
    {"JINF-U-0-A", "U0",  "U0A", TRUE,  68, 'A'},
    {"JINF-U-8-B", NULL,  "U0B", TRUE,  69, 'B'},
    {"JINF-U-1-A", "U1",  "U1A", TRUE,  71, 'A'},
    {"JINF-U-1-B", NULL,  "U1B", TRUE,  72, 'B'}};
#endif

  char string[] = {
//             1         2         3         4         5         6       
//   0123456789012345678901234567890123456789012345678901234567890123456
    "JT--JJ--E0--1--R0--1--S0--1--2--3--T0--1--2--3--4--5--6--7--U0--1--"};
  struct {
    char *name;
    char *mike;
    char *crt;
    bool use;
    int  i;
    char c;
  } obj[] = {
    {"JLV1-A",     "JA",  "JTA", TRUE,   2, 'A'},
    {"JLV1-B",     NULL,  "JTB", TRUE,   3, 'A'},
    {"JINJ-2",     NULL,  "JJ2", FALSE,  6, '2'},
    {"JINJ-3",     NULL,  "JJ3", FALSE,  7, '3'},
    {"JINF-E-0-A", "E0",  "E0A", TRUE,  10, 'A'},
    {"JINF-E-0-B", NULL,  "E0B", TRUE,  11, 'B'},
    {"JINF-E-1-A", "E1",  "E1A", TRUE,  13, 'A'},
    {"JINF-E-1-B", NULL,  "E1B", TRUE,  14, 'B'},
    {"JINF-R-0-A", "R0A", "R0A", TRUE,  17, 'A'},
    {"JINF-R-0-B", "R0B", "R0B", TRUE,  18, 'B'},
    {"JINF-R-1-A", "R1A", "R1A", TRUE,  20, 'A'},
    {"JINF-R-1-B", "R1B", "R1B", TRUE,  21, 'B'},
    {"SDR2-0-A",   "S0A", "S0A", TRUE,  24, 'A'},
    {"SDR2-0-B",    NULL, "S0B", TRUE,  25, 'B'},
    {"SDR2-1-A",   "S1A", "S1A", TRUE,  27, 'A'},
    {"SDR2-1-B",    NULL, "S1B", TRUE,  28, 'B'},
    {"SDR2-2-A",   "S2A", "S2A", TRUE,  30, 'A'},
    {"SDR2-2-B",    NULL, "S2B", TRUE,  31, 'B'},
    {"SDR2-3-A",   "S3A", "S3A", TRUE,  33, 'A'},
    {"SDR2-3-B",    NULL, "S3B", TRUE,  34, 'B'},
    {"JINF-T-0-A", "T0",  "T0A", TRUE,  37, 'A'},
    {"JINF-T-0-B", NULL,  "T0B", TRUE,  38, 'B'},
    {"JINF-T-1-A", "T1",  "T1A", TRUE,  40, 'A'},
    {"JINF-T-1-B", NULL,  "T1B", TRUE,  41, 'B'},
    {"JINF-T-2-A", "T2",  "T2A", TRUE,  43, 'A'},
    {"JINF-T-2-B", NULL,  "T2B", TRUE,  44, 'B'},
    {"JINF-T-3-A", "T3",  "T3A", TRUE,  46, 'A'},
    {"JINF-T-3-B", NULL,  "T3B", TRUE,  47, 'B'},
    {"JINF-T-4-A", "T4",  "T4A", TRUE,  49, 'A'},
    {"JINF-T-4-B", NULL,  "T4B", TRUE,  50, 'B'},
    {"JINF-T-5-A", "T5",  "T5A", TRUE,  52, 'A'},
    {"JINF-T-5-B", NULL,  "T5B", TRUE,  53, 'B'},
    {"JINF-T-6-A", "T6",  "T6A", TRUE,  55, 'A'},
    {"JINF-T-6-B", NULL,  "T6B", TRUE,  56, 'B'},
    {"JINF-T-7-A", "T7",  "T7A", TRUE,  58, 'A'},
    {"JINF-T-7-B", NULL,  "T7B", TRUE,  59, 'B'},
    {"JINF-U-0-A", "U0",  "U0A", TRUE,  62, 'A'},
    {"JINF-U-8-B", NULL,  "U0B", TRUE,  63, 'B'},
    {"JINF-U-1-A", "U1",  "U1A", TRUE,  65, 'A'},
    {"JINF-U-1-B", NULL,  "U1B", TRUE,  66, 'B'}};

//~--

  if (ABS(P) > 0) {
    printf("%sError Statistics 1 for Events:\n", header);
    for (i=0; i<0x200; i++) {
      for(j=0; j<DIM(errors); j++) {
        if (errors[j].event_cntr[i]) {
          printf("%03X %-20s %-5s %6d\n", 
                  i, NodeName[i], errors[j].name, errors[j].event_cntr[i]);
        }
      }
    }
    printf("%sError Statistics 1 for Others:\n", header);
    for (i=0; i<0x200; i++) {
      for(j=0; j<DIM(errors); j++) {
        if (errors[j].other_cntr[i]) {
          printf("%03X %-20s %-5s %6d\n", 
                  i, NodeName[i], errors[j].name, errors[j].other_cntr[i]);
        }
      }
    }
  }

  if (ABS(P) > 0) {
    printf("%sError Statistics 2 for Events: ", header);
    for (j=0; j<DIM(errors); j++) {
      int n = 0;
      for (i=0; i<0x200; i++) n += errors[j].event_cntr[i];
      if (n) printf("%s:%d ", errors[j].name, n);
    }
    printf("\n");
    printf("%sError Statistics 2 for Others: ", header);
    for (j=0; j<DIM(errors); j++) {
      int n = 0;
      for (i=0; i<0x200; i++) n += errors[j].other_cntr[i];
      if (n) printf("%s:%d ", errors[j].name, n);
    }
    printf("\n");
  }

  if (ABS(P) > 0) {
    printf("%sStatistics:\n", header);
    for (i=0; i<0x200; i++) {
      if (events[i]) printf("%03X %-20s %6d\n",  i, NodeName[i], events[i]);
    }
    printf("n_events = %d, n_others = %d, n_bad_events = %d, n_bad_others = %d\n", 
            n_events, n_others, n_bad_events, n_bad_others);
  }

//~--

  printf("%s", header);
  printf("%s", first_block_time);

  if (n_events) {

    for (k=0; k<DIM(obj); k++) {
      for (i=0; i<0x200; i++) {
        if (strcasestr(NodeName[i], obj[k].name)) {
            if (events[i]) string[obj[k].i] = obj[k].c;
            else           string[obj[k].i] = '-';
          break;
        }
      }
    }
    printf("%s ", string);

    printf(":%8d %8d : ", n_events-n_bad_events, n_bad_events);
    printf("%6.4f ", (float)n_bad_events/(float)n_events);

    if (n_bad_events) {
      printf("* ");
      for (k=0; k<DIM(obj); k++) {
        int n = 0;
        for (i=0; i<0x200; i++) {
          if (strcasestr(NodeName[i], obj[k].name)) {
            for (j=0; j<DIM(errors); j++) n += errors[j].event_cntr[i];
            break;
          }
        }
        if (obj[k].use && n) printf("%s:%d ", obj[k].crt, n);
      }
      printf("= ");
      for (j=0; j<DIM(errors); j++) {
        int n = 0;
        for (i=0; i<0x200; i++) n += errors[j].event_cntr[i];
        if (n) printf("%s:%d ", errors[j].name, n);
      }
    }
  }
  else {
    int i;
    for (i=0; i<96; i++) printf(" ");
  }

  if (n_bad_others) {
    printf("+ ");
    for (i=0; i<0x200; i++) {
      int n = 0;
      for (j=0; j<DIM(errors); j++) n += errors[j].other_cntr[i];
      if (n) printf("%s:%d ", NodeName[i], n);
    }
    printf("= ");
    for (j=0; j<DIM(errors); j++) {
      int n = 0;
      for (i=0; i<0x200; i++) n += errors[j].other_cntr[i];
      if (n) printf("%s:%d ", errors[j].name, n);
    }
  }

  printf("\n");

//~--

#if 0
  if (strcasestr(PP, "M")) {
    printf("Average Event Length:%d ",
           2*(int)((float)ave_event_len.sum/(float)ave_event_len.cnt+0.5));
    for (j=0; j<DIM(obj); j++) {
      for (i=0; i<0x200; i++) {
        if (strcasestr(NodeName[i], obj[j].name)) {
          if (obj[j].mike) printf("%s:%d ", obj[j].mike,
                           2*(int)((float)ave_len[i].sum/(float)ave_len[i].cnt+0.5));
        }
      }
    }
    printf("\n");
  }
#endif
  
  if (strcasestr(PP, "M")) {
    printf("Average Event Length:%d ",
           2*(int)((float)ave_event_len.sum/(float)ave_event_len.cnt+0.5));
    for (j=0; j<DIM(obj); j++) {
      for (i=0; i<0x200; i++) {
        if (strcasestr(NodeName[i], obj[j].name)) {
          printf("%s:%d ", obj[j].crt,
                 2*(int)((float)ave_len[i].sum/(float)ave_len[i].cnt+0.5));
        }
      }
    }
    printf("\n");
  }
  
//~--  

  if (strcasestr(PP, "MM")) {
    printf("%sAverage Event Length in Bytes:\n", header);
    printf("    %-20s %6d\n", "'Event'",
             2*(int)((float)ave_event_len.sum/(float)ave_event_len.cnt+0.5));
    for (i=0; i<0x200; i++) {
      if (ave_len[i].cnt) {
        printf("%03X %-20s %6d\n", i, NodeName[i], 
               2*(int)((float)ave_len[i].sum/(float)ave_len[i].cnt+0.5));
      }
    }
  }

//~--  

  if (strcasestr(PP, "trig")) {
    int j;
    char *nam0[] = {
    "FTC",        "FTCP0",     "FTCP1",      "FTCT0", 
    "FTCT1",      "FTZ",       "FTE",        "ACC0", 
    "ACC1",       "BZ",        "ECAL-F_and", "ECAL-F_or",
    "ECAL-A_and", "ECAL-A_or", "EXT-GATE0",  "EXT-GATE1"};
    char *nam1[] = {
    "Sub0", "Sub1", "Sub2", "Sub3", 
    "Sub4", "Sub5", "Sub6", "Sub7", 
    "LA-0", "LA-1", "?",     "DSP",
    "Gen",  "??",   "???",   "????"};
    for (i=0; i<DIM(list); i++) {
      if (list[i].n < 0) break;
      printf("%04X %04X %5d ", list[i].pattern>>16, list[i].pattern & 0x0000FFFF, list[i].n);
      for (j=0; j<16; j++) if (list[i].pattern & (1 << (j+16))) printf("%s ", nam1[j]);
      for (j=0; j<16; j++) if (list[i].pattern & (1 << j)) printf("%s ", nam0[j]);
      printf("\n");
    }
  }

//~--

}

//~----------------------------------------------------------------------------

void process_AMS_block_file(FILE *file) {

//char *me = {"process_AMS_block_file"};
  int i, j;
  int32 len;
  int16 dat[1000000];

//~--

  sprintf(file_name, "%04d/%03d", current_file/1000, current_file%1000);
  sprintf(header, "%s ", file_name);
  first_block_in_file = TRUE;
  first_event_in_file = TRUE;

  zero_file_summary();

//~--

  while (1) {
    bool bad_block = FALSE;

    if (!read_AMS_block(file, dat, &len)) break;

    for (i=0; i<0x200; i++) for(j=0; j<DIM(errors); j++) errors[j].mark[i] = FALSE;
    this_is_event = FALSE;
    this_is_calibration_result = FALSE;
    event_length = 0;
    for (i=0; i<0x200; i++) length[i] = 0;

//qqq_len = len ;
//for (i=0; i<len; i++) qqq_dat[i] = dat[i];

    unpack_and_process_AMS_block(dat, len, SHF);

    if (this_is_event) {
      n_events++;
      for (i=0; i<0x200; i++) {
        for(j=0; j<DIM(errors); j++) {
          if (errors[j].mark[i]) {
            bad_block = TRUE;
            errors[j].event_cntr[i]++;
          }
        }
      }
      if (bad_block) n_bad_events++;
      ave_event_len.sum += event_length;
      ave_event_len.cnt += 1;

      for (i=0; i<0x200; i++) {
        if (length[i]) {
          ave_len[i].sum += length[i];
          ave_len[i].cnt += 1;
        }
      }
      if (!bad_block) process_event(0);
    }
    else {
      n_others++;
      for (i=0; i<0x200; i++) {
        for(j=0; j<DIM(errors); j++) {
          if (errors[j].mark[i]) {
            bad_block = TRUE;
            errors[j].other_cntr[i]++;
          }
        }
      }
      if (bad_block) n_bad_others++;
    }

  }  // while (1)

  print_file_summary();
}

//~============================================================================

void process_event(int action) {

  int i;
  static bool first = TRUE;
  static FILE *file;
  
  if (!strcasestr(PP, "N")) return;
  
  if (first) {
    if (!(file = fopen("q.txt", "w"))) PANIC("Failed to open n-tuple file\n");
    first = FALSE;
  }

  fprintf(file, "%5.0f ", delta_T);
//fprintf(file, "%5d ",   event_length);
  for (i=0; i<0x200; i++) {
/*
    if (strcasestr(NodeName[i], "JINF-T-0-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "JINF-T-1-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "JINF-T-2-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "JINF-T-3-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "JINF-T-4-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "JINF-T-5-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "JINF-T-6-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "JINF-T-7-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "JINF-U-0-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "JINF-U-1-A")) fprintf(file, "%5d ", length[i]);
*/
    if (strcasestr(NodeName[i], "SDR2-0-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "SDR2-1-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "SDR2-2-A")) fprintf(file, "%5d ", length[i]);
    if (strcasestr(NodeName[i], "SDR2-3-A")) fprintf(file, "%5d ", length[i]);
  }
  fprintf(file, "\n");
}

//~============================================================================

int main( int argc, char *argv[]) {

  int k;
  char *block_directory;
  int first_block_file;
  int last_block_file;
  int nb_of_files;
  static char *blank = {" "};
  FILE *block_file;
    
//~--

  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  printf("Usage: %s <Block Directory> [<First Block File> [<Nb of Files> [P [PP]]]]\n",
         argv[0]);

  if (argc < 2) {
    printf("Too few parameters\n");
    exit(1);
  }

  k = 1;
  block_directory = argv[k++];
  remove_trailing_slash(block_directory);
  first_block_file = 0;
  nb_of_files = 1;
  P  = 0;
  PP = blank;
  if (argc > k) first_block_file = atoi(argv[k++]);
  if (argc > k) nb_of_files      = atoi(argv[k++]);
  if (argc > k) P                = atoi(argv[k++]);
  if (argc > k) PP               = argv[k++];

  work_on_my_tree();

  if (first_block_file < 0) first_block_file = 0;
  last_block_file = first_block_file + nb_of_files - 1;

  printf("First Block File = %07d\n", first_block_file);
  printf("Last Block File  = %07d\n", last_block_file);
  printf("P                = %d\n",   P);
  printf("PP               = %s\n",   PP);
  
//~--

  current_file = first_block_file; 

  while (1) {
    static bool first = TRUE;
//  last_block_file = MIN(last_block_file, the_last_file_in(block_directory));
    for (; current_file <= the_last_file_in(block_directory); current_file++) {
//printf("current_file=%d\n", current_file);
      if (current_file > last_block_file) exit(0);
      first = TRUE;
      block_file = open_file_for_reading(block_directory, current_file);
      if (!block_file) continue;
      process_AMS_block_file(block_file);
      fclose(block_file);
    }
//exit(1);
    if (first) fprintf(stderr, "Waiting for a new file... \r");
    first = FALSE;

    sleep(10);
  }

//~--

  return 0;
}
  
//~============================================================================
