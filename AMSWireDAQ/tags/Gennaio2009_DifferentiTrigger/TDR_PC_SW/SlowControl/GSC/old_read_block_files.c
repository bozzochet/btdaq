// file old_read_block_files.c
//
// read and decode block files
//
// A.Lebedev - Jan 2008...
//

#include "nodedef.h"
#include "gsclib.h"

int P;
char *PP;

#define NG 3

static AMSBlock Block, *pBlock = &Block;

int current_file;

char file_name[80], header[80];

//~---

typedef struct {
  int    len;
  int16 *dat;
} _dat;

typedef struct {
  int    len;
  int    lnk;
  int16 *dat;
  int16  sta;
} _fra;

typedef struct {
  int    len;
  int16 *dat;
  int16  sta;
  int    num;
  int32  msk;   // <<<=== bad name
  int16  CRC;
  int32  err;
  _fra fra[24];
} _event;

_event JINJ_event, *pJINJ_event = &JINJ_event;
int JINJ_fra_pre[24], JINJ_fra_err[24];
int JINJ_fra_nok[24], JINJ_fra_len[24];

_event JINF_event, *pJINF_event = &JINF_event;
int JINF_fra_pre[24], JINF_fra_err[24];

char *jinjlinks[24] = {
  "T2", "T3", "U1",  "T0",  "S1A", "S1B", "S0A", "S0B", 
  "U0", "T1", "R0",  "R1",  "E0",  "E1",  "JA",  "JB", 
  "T4", "T5", "S2A", "S2B", "S3A", "S3B", "T6",  "T7"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void process_JLV1_event_fragment(_fra *fra);

void process_TOF_event_fragment(_fra *fra);

int16 decode_JINJ_event(_event *event);
void process_JINJ_event(_event *pJINJ_event);
int16 decode_JINF_event(_event *event);
void process_JINF_event(_event *pJINF_event);

void print_AMS_block_header(int ident, AMSBlock *pBlock);
void process_AMS_block_file(FILE *file, int file_number);

//~----------------------------------------------------------------------------

void process_JLV1_event_fragment(_fra *fra) {

  int64 event_time = 0;
  static int64 old_event_time = 0;
  static int old_p0 = -1, old_t0 = 0, old_p1 = -1, old_t1 = 0;
  int i;
  int p0, t0, p1, t1;
  float l0, l1;
  float period[4] = {0.25, 0.50, 1.00, 2.00};
  int regs[4] = {0x34, 0x33, 0x36, 0x35};

  if (!strcasestr(PP, "J")) return;

    
#if 0
  printf("JLV1: ");
  printf("len = %d, sta = %04X \n", fra->len, fra->sta);
  for (i=0; i<fra->len; i++) {
    printf("%04X ", fra->dat[i]);
    if (i && !((i + 1) % 10)) printf("\n");
  }
  if (fra->len % 10) printf("\n");
#endif

  if ((fra->sta & 0x00C0) == 0x0040) {
    event_time = (int64)(fra->dat[5] & 0x00FF) << 32 |
                 (int64)(fra->dat[4] & 0xFFFF) << 16 |
                 (int64)(fra->dat[3] & 0xFFFF) <<  0;
  }
  if ((fra->sta & 0x00C0) == 0x0080) {
    event_time = (int64)(fra->dat[6] & 0x00FF) << 32 |
                 (int64)(fra->dat[5] & 0xFFFF) << 16 |
                 (int64)(fra->dat[4] & 0xFFFF) <<  0;
  }

  p0 = unpack16(fra->dat[47], 0x3000);
  t0 = ((int32)unpack16(fra->dat[47], 0x07FF) << 16) + fra->dat[48];
//l0 = (float)t0 * 0.000000020 * period[p0];
  l0 = (float)t0 * 0.000000020 / period[p0];
  
  p1 = unpack16(fra->dat[49], 0x3000);
  t1 = ((int32)unpack16(fra->dat[49], 0x07FF) << 16) + fra->dat[50];
//l1 = (float)t1 * 0.000000020 * period[p1];
  l1 = (float)t1 * 0.000000020 / period[p1];

  if (old_p0 != p0 || old_t0 != t0 || old_p1 != p1 || old_t1 != t1) {
    printf("LiveTime: ");
    for (i=47; i<51; i++) printf("0x%02X=%04X ", regs[i-47], fra->dat[i]);
    printf("p0 = %d, period0 = %4.2f, t0 = %8d, l0 = %4.2f ", p0, period[p0], t0, l0);
    printf("p1 = %d, period1 = %4.2f, t1 = %8d, l1 = %4.2f ", p1, period[p1], t1, l1);
    if (old_event_time) printf("%5.2f ", (event_time - old_event_time) * 0.000000640);
    printf("\n");
    old_p0 = p0;
    old_t0 = t0;
    old_p1 = p1;
    old_t1 = t1;
    old_event_time = event_time;
  }
}

//~----------------------------------------------------------------------------

void process_TOF_event_fragment(_fra *fra) {

  int i, j;
  static _dat *charge = NULL, *pretrig = NULL, *time = NULL, *status = NULL;
  static bool first = TRUE;
  float temp[5];
  char ident[80];
  char b[80];

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

  if (!strcasestr(PP, "S")) return;

#if 0
if (pJINJ_event->num < 20094) return;
if (pJINJ_event->num > 20096) exit(1);
P = 10;
#endif

  sprintf(ident, "%s       %s: ", header, jinjlinks[fra->lnk]);

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
    printf("%slen=%3d sta=%04X", ident, fra->len, fra->sta);
    sprintf_binary(b, fra->sta, -16);
    printf("= %s\n", b);
  }
  
  if (fra->len == 1) return;
  
  charge->len  = 90;
  pretrig->len =  4;
  status->len  = 10;
  time->len    = fra->len - 1 - charge->len - pretrig->len - status->len;

  if (P > 1) {
    printf("%scharge->len=%d pretrig->len=%d status->len=%d time->len=%d \n",
            ident, charge->len, pretrig->len, status->len, time->len);
  }
  
  charge->dat  = &fra->dat[0];
  pretrig->dat = charge->dat  + charge->len;
  time->dat    = pretrig->dat + pretrig->len;
  status->dat  = time->dat    + time->len;

//~------

  if (P > 1) {
    printf("%sCharge:\n", ident);
    for (j=0; j<10; j++) {
      printf("%s  ", ident);
      for (i=9*j; i<9*j+9; i++) printf("%04X ", charge->dat[i]);
      printf("\n");
    }
  }

//~------

  if (P > 1) {
    printf("%sPretrigger (%04X): ", ident, status->dat[1]);
    for (i=0; i<pretrig->len; i++) printf("%04X  ", pretrig->dat[i]);
    printf("\n");
  }

//~------

  if (time->len % 2) {
    if (P > 1) printf("%s *** Error: Time Length is odd\n", ident);
    return;
  }

  for (i=0; i<5; i++) temp[i] = -273.2;
  for (i=0; i<5; i++) tdc[i]->header  = 0x00000000;
  for (i=0; i<5; i++) tdc[i]->trailer = 0x00000000;
  for (i=0; i<5; i++) tdc[i]->error   = 0x00000000;
  for (i=0; i<5; i++) tdc[i]->cnt     = 0;
  
  if (P > 9) printf("Raw Time Info:\n");
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
    if (P > 9) {
      printf("%3d: ", i/2);
      for (j=0; j<lnk; j++) printf("                        ");
    }
    if      (key == 0x2) {
      tdc[lnk]->header = dat;
      if (P > 9) printf("header(%d)=%08X\n", lnk, dat);
    }
    else if (key == 0x3) {
      tdc[lnk]->trailer = dat;
      if (P > 9) printf("trailer(%d)=%08X\n", lnk, dat);
    }
    else if (key == 0x4) {
      tdc[lnk]->hit[tdc[lnk]->cnt++] = dat;
      if (P > 9) printf("hit(%d,%d)=%08X\n", lnk, cha, dat);
    }
    else if (key == 0x6) {
      tdc[lnk]->error = dat;
      if (P > 9) printf("error(%d)=%08X\n", lnk, dat);
    }
    else if (key == 0x8) {
      int t_1 = unpack32(dat, 0x00FFF000);
      int t_2 = unpack32(dat, 0x00000FFF);
      temp[lnk] = 235.0 - 400.0 * t_1 / t_2;
      if (P > 9) printf("temperature(%d)=%08X\n", lnk, dat);
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

//~------

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
}

//~----------------------------------------------------------------------------

int16 decode_JINJ_event(_event *event) {
  
  int16 *p;
  char ident[80];
  
  sprintf(ident, "%s     ", header);
  
  p = event->dat;

  if (P > 1) printf("%sevent->len=%04X=%d(words)=%d(bytes)\n", 
                    ident, event->len, event->len, 2*event->len);

#if 0
for (i=0; i<event->len; i++) printf("%04X%s", p[i], !((i+1)%10) ? "\n" : " ");
printf("\n");
#endif

  event->CRC = calculate_CRC16(p, event->len);
  if (P > 1) printf("%sevent->CRC=%04X ", ident, event->CRC);
  if (event->CRC) {
    if (P > 1) printf("*** Error: wrong CRC\n");
    return 2;
  }
  else {
    if (P > 1) printf("\n");
  }

  event->num = *p++;
  if (P > 1) printf("%sevent->num=%04X=%d\n", 
                    ident, event->num, event->num);

  event->sta = event->dat[event->len-2];
  if (P > 1) printf("%sevent->sta=%04X", ident, event->sta);
  if (P > 1) {
    char b[80];
    sprintf_binary(b, event->sta, -16);
    printf(" = %s ", b);
  }

  if (event->sta & 0xFE00) {
    if (P > 1) printf("*** Error: one/several ERROR bits set in JINJ status");
  }
  if (P > 1) printf("\n");

  event->msk = 0x000000;
  event->err = 0x000000;

  while (1) {
    int16 lnk;
    int32 m;
    if (p + 2 >= event->dat + event->len) {
      if (p + 2 > event->dat + event->len) {
        if (P > 1) printf("%s*** Error: Pointer mismatch(2): p+2=%08X, event->dat+event->len=%08X\n", 
                           ident, (int32)(p+2), (int32)(event->dat+event->len));
        return 3;
      }
      break;
    }
    lnk = *(p + *p) & 0x001F;
    if (P > 1) printf("%slnk=%2d:%-3s ", ident, lnk, (lnk < 24 ? jinjlinks[lnk] : "???"));

    if (lnk > 23) {
      if (P > 1) printf(" *** Error: wrong lnk\n");
      return 4;
    }

    m = 1 << lnk;
    if (P > 1) printf("msk=%06X m=%06X ", event->msk, m);
    if (m & event->msk) {
      if (P > 1) printf(" *** Error: repeated fragment: lnk=%d\n", lnk);
      return 5;
    }

    event->msk         |=  m;
    event->fra[lnk].len = *p++;
    event->fra[lnk].lnk = lnk;
    event->fra[lnk].dat =  p;
    event->fra[lnk].sta = event->fra[lnk].dat[event->fra[lnk].len-1];
    JINJ_fra_pre[lnk]++;

    if (P > 1) printf("len=%5d ", event->fra[lnk].len);

    if (P > 1) {
      char b[80];
      printf("sta=%04X", event->fra[lnk].sta);
      sprintf_binary(b, event->fra[lnk].sta, -16);
      printf("= %s ", b);
    }
    if (event->fra[lnk].sta & 0x8000) {
      if (event->fra[lnk].sta & 0x7E00) {
        event->err |= m;
        JINJ_fra_err[lnk]++;
        if (P > 1) printf("*** Error: one/several ERROR bits set");
      }
    }
    else {
      event->err |= m;
      JINJ_fra_err[lnk]++;
      if (P > 1) printf("*** Error: no DATA bit set");
    }
    if (P > 1) printf("\n");

    p = p + event->fra[lnk].len;
  }

  if (event->err) return 6;
  else            return 0;
}

//~----------------------------------------------------------------------------

void process_JINJ_event(_event *pJINJ_event) {

  int i;
  long long event_time;
  int event_number;
  int t2_len, t3_len, t6_len, t7_len;
  int u0_len, u1_len;
  int l1_len;
  int s1_len;

#if 0
{
int i;
printf("%d ", pJINJ_event->fra[14].len);
for (i=0; i<20; i++) printf("%04X ", pJINJ_event->fra[14].dat[i]);
printf("\n");
}
#endif

if ((pJINJ_event->fra[14].sta & 0x00C0) == 0x00C0 ||
    (pJINJ_event->fra[14].sta & 0x00C0) == 0x0000) {
//printf("%07d: %04X\n", current_file, pJINJ_event->fra[14].sta);
}


  event_number = pJINJ_event->num;
  event_time = 0;

  if ((pJINJ_event->fra[14].sta & 0x00C0) == 0x0040) {
    event_time = (int64)(pJINJ_event->fra[14].dat[5] & 0x00FF) << 32 |
                 (int64)(pJINJ_event->fra[14].dat[4] & 0xFFFF) << 16 |
                 (int64)(pJINJ_event->fra[14].dat[3] & 0xFFFF) <<  0;
  }
  if ((pJINJ_event->fra[14].sta & 0x00C0) == 0x0080) {
    event_time = (int64)(pJINJ_event->fra[14].dat[6] & 0x00FF) << 32 |
                 (int64)(pJINJ_event->fra[14].dat[5] & 0xFFFF) << 16 |
                 (int64)(pJINJ_event->fra[14].dat[4] & 0xFFFF) <<  0;
  }
  t2_len = pJINJ_event->fra[ 0].len;
  t3_len = pJINJ_event->fra[ 1].len;
  t6_len = pJINJ_event->fra[22].len;
  t7_len = pJINJ_event->fra[23].len;
  u0_len = pJINJ_event->fra[ 8].len;
  u1_len = pJINJ_event->fra[ 2].len;
  l1_len = pJINJ_event->fra[14].len;
  s1_len = pJINJ_event->fra[ 4].len;

//printf("event_num = %5d, event_time = %lld\n", event_number, event_time);

#if 1  // why there are events with very small delta_T? (Phillipe + Giovanni)
       // and what is max delta_T? sometimes trigger rate goes down...
{
  static long long old_event_time = -1;
  static int old_event_number = -1;;
  int delta_T;
  static int min_delta_T = 10000000;
  static int max_delta_T = 0;
  if (old_event_number > 0) {
    if (((event_number - old_event_number) == 1) && (event_time > 0)) {
      delta_T = (event_time - old_event_time) * 0.640 + 0.5;
      if (delta_T > 1000000) printf("%s event %5d --> Big delta_T = %4.1f sec\n", 
                             header, event_number, (float)delta_T/1000000.0);
#if 0
      if ((delta_T < min_delta_T) || (delta_T > max_delta_T)) {
        min_delta_T = MIN(delta_T, min_delta_T);
        max_delta_T = MAX(delta_T, max_delta_T);
        printf("event_num = %5d, event_time = %lld, delta_T = %-7d, min = %5dus, max = %.1fs\n", 
                event_number, event_time, delta_T, min_delta_T, 
                (float)max_delta_T/1000000.0);
      }
#endif
    }
    else {
      event_number = -1;
    }
  }
  
  old_event_number = event_number;
  old_event_time   = event_time;
}
#endif

#if 0 // write n-tuples
  if (((event_time - old_event_time) * 0.640) < 400.0) {
    printf("%10lld %5d %5d %5d %5d %5d %5d %5d %d\n", 
           event_time-old_event_time, 
           t2_len, t3_len, t6_len, t7_len, 
           u0_len, u1_len,
           l1_len,
           s1_len);
  }
#endif

  
  for (i=0; i<24; i++) {
    if ((pJINJ_event->msk & (1 << i)) && !(pJINJ_event->err & (1 << i))) {
      if (jinjlinks[i][0] == 'S') process_TOF_event_fragment(&pJINJ_event->fra[i]);
      if (jinjlinks[i][0] == 'J') process_JLV1_event_fragment(&pJINJ_event->fra[i]);
      JINJ_fra_nok[i] += 1;
      JINJ_fra_len[i] += pJINJ_event->fra[i].len;
    }
  }
}

//~----------------------------------------------------------------------------

int16 decode_JINF_event(_event *event) {
  
  int16 *p;
  
  p = event->dat;

  if (P > 1) printf("%s       event->len=%04X=%d(words)=%d(bytes)\n", 
                    header, event->len, event->len, 2*event->len);

#if 0
for (i=0; i<event->len; i++) printf("%04X%s", p[i], !((i+1)%10) ? "\n" : " ");
printf("\n");
#endif

  event->CRC = calculate_CRC16(p, event->len);
  if (P > 1) printf("%s       event->CRC=%04X ", header, event->CRC);
  if (event->CRC) {
    if (P > 1) printf("*** Error: wrong CRC\n");
    return 2;
  }
  else {
    if (P > 1) printf("\n");
  }

  event->num = *p++;
  if (P > 1) printf("%s       event->num=%04X=%d\n", 
                    header, event->num, event->num);
  
  event->sta = event->dat[event->len-2];
  if (P > 1) printf("%s       event->sta=%04X", header, event->sta);
  if (P > 1) {
    char b[80];
    sprintf_binary(b, event->sta, -16);
    printf(" = %s ", b);
  }
  if (event->sta & 0xFE00) {
//printf("%s       *** Error: one/several ERROR bits set in JINF status\n", header);
    if (P > 1) printf("*** Error: one/several ERROR bits set in JINF status");
  }
  if (P > 1) printf("\n");

  event->msk = 0x000000;
  event->err = 0x000000;

  while (1) {
    int16 lnk;
    int32 m;

    if (p + 2 >= event->dat + event->len) {
      if (p + 2 > event->dat + event->len) {
        if (P > 1) printf("%s       *** Error: Pointer mismatch(2): p+2=%08X, event->dat+event->len=%08X\n", 
                           header, (int32)(p+2), (int32)(event->dat+event->len));
        return 3;
      }
      break;
    }
    lnk = *(p + *p) & 0x001F;
    if (P > 1) printf("%s       lnk = %2d, ", header, lnk);

    if (lnk > 23) {
      if (P > 1) printf(" *** Error: wrong lnk\n");
      return 4;
    }

    m = 1 << lnk;
    if (P > 1) printf("event->msk = %06X, m = %06X, ", event->msk, m);
    if (m & event->msk) {
      if (P > 1) printf(" *** Error: repeated fragment: lnk=%d\n", lnk);
      return 5;
    }

    event->msk         |=  m;
    event->fra[lnk].len = *p++;
    event->fra[lnk].dat =  p;
    event->fra[lnk].sta = event->fra[lnk].dat[event->fra[lnk].len-1];
    JINF_fra_pre[lnk]++;

    if (P > 1) printf("event->fra[%2d].len = %5d, ", lnk, event->fra[lnk].len);

    if (P > 1) printf("sta = %04X", event->fra[lnk].sta);
    if (P > 1) {
      char b[80];
      sprintf_binary(b, event->fra[lnk].sta, -16);
      printf(" = %s ", b);
    }
    if (event->fra[lnk].sta & 0x8000) {
      if (event->fra[lnk].sta & 0x7E00) {
        event->err |= m;
        JINF_fra_err[lnk]++;
        if (P > 1) printf("*** Error: one/several ERROR bits set");
      }
    }
    else {
      event->err |= m;
      JINF_fra_err[lnk]++;
      if (P > 1) printf("*** Error: no DATA bit set");
    }
    if (P > 1) printf("\n");

    p = p + event->fra[lnk].len;
  }

  if (event->err) return 6;
  else            return 0;
}

//~----------------------------------------------------------------------------

void process_JINF_event(_event *pJINJ_event) {

  static int64 event_time, old_event_time;

#if 0
{
int i;
printf("%d ", pJINJ_event->fra[14].len);
for (i=0; i<20; i++) printf("%04X ", pJINJ_event->fra[14].dat[i]);
printf("\n");
}
#endif

#if 0
{
  int t2_len, t3_len, t6_len, t7_len;
  int u0_len, u1_len;
  int l1_len;
  int s1_len;
if ((pJINJ_event->fra[14].sta & 0x00C0) == 0x00C0 ||
    (pJINJ_event->fra[14].sta & 0x00C0) == 0x0000) {
//printf("%07d: %04X\n", current_file, pJINJ_event->fra[14].sta);
}


  event_time = 0;
  if ((pJINJ_event->fra[14].sta & 0x00C0) == 0x0040) {
    event_time = (int64)(pJINJ_event->fra[14].dat[6] & 0x00FF) << 32 |
                 (int64)(pJINJ_event->fra[14].dat[5] & 0xFFFF) << 16 |
                 (int64)(pJINJ_event->fra[14].dat[4] & 0xFFFF) <<  0;
  }
  if ((pJINJ_event->fra[14].sta & 0x00C0) == 0x0080) {
    event_time = (int64)(pJINJ_event->fra[14].dat[7] & 0x00FF) << 32 |
                 (int64)(pJINJ_event->fra[14].dat[6] & 0xFFFF) << 16 |
                 (int64)(pJINJ_event->fra[14].dat[5] & 0xFFFF) <<  0;
  }
  t2_len = pJINJ_event->fra[ 0].len;
  t3_len = pJINJ_event->fra[ 1].len;
  t6_len = pJINJ_event->fra[22].len;
  t7_len = pJINJ_event->fra[23].len;
  u0_len = pJINJ_event->fra[ 8].len;
  u1_len = pJINJ_event->fra[ 2].len;
  l1_len = pJINJ_event->fra[14].len;
  s1_len = pJINJ_event->fra[ 4].len;
}
#endif

#if 0 // write n-tuples
  if (((event_time - old_event_time) * 0.640) < 400.0) {
    printf("%10lld %5d %5d %5d %5d %5d %5d %5d %d\n", 
           event_time-old_event_time, 
           t2_len, t3_len, t6_len, t7_len, 
           u0_len, u1_len,
           l1_len,
           s1_len);
  }
#endif

  old_event_time = event_time;
  
//process_TOF_event_fragment(&pJINJ_event->fra[4]);
//print_JLV1_event_fragment(&pJINJ_event->fra[14]);
  

//printf("pBlock->NodeAddress = %03X, pBlock->DataType = %02X\n", pBlock->NodeAddress, pBlock->DataType);

}

//~----------------------------------------------------------------------------

void print_AMS_block_header(int ident, AMSBlock *pBlock) {

  int i;
  
  if (!P) return;
  
  printf(header);
  
  for (i=0; i<ident; i++) printf(" ");
  
  printf("%s ", pBlock->BlockType & 1 ? "W" : "R");
  printf("NA=%03X", pBlock->NodeAddress);
  if (pBlock->NodeAddress < 512) printf("=%s ", NodeName[pBlock->NodeAddress]);
  else                           printf("=?");
  printf("DT=%02X ", pBlock->DataType);
  printf("DC=%d", pBlock->DataCount);
  printf(" ");
  printf("\n");
}

//~----------------------------------------------------------------------------

void process_AMS_block_file(FILE *file, int file_number) {

  int i;
  int32 len;
  int16 dat[1000000];
  int16 *p, *end;
  int32 run_id, run_tag, event_id;
  bool jmdc_id, jinj_id;
  bool jmdc_event, jinj_event, jinf_event;
  bool first_event_in_file = TRUE;
/*
  struct tm *t;
*/
  char first_event_time[80];
  int nev[NG][7], n_bad = 0, n_tot;
  
  sprintf(file_name, "%04d/%03d", file_number/1000, file_number%1000);

  memset(nev, 0, sizeof(nev));
  for (i=0; i<24; i++) JINJ_fra_pre[i] = 0;
  for (i=0; i<24; i++) JINJ_fra_err[i] = 0;
  for (i=0; i<24; i++) JINJ_fra_nok[i] = 0;
  for (i=0; i<24; i++) JINJ_fra_len[i] = 0;
  for (i=0; i<24; i++) JINF_fra_pre[i] = 0;
  for (i=0; i<24; i++) JINF_fra_err[i] = 0;

  while (1) {
    if (!read_AMS_block(file, dat, &len)) break;
//print_AMS_data_block("", pBlock);
    unpack16_AMS_block(dat, len, TRUE, pBlock);

    if (strcasestr(PP, "H")) {
      if (strcasestr(NodeName[pBlock->NodeAddress], "USCM")) {
        int8  *RP_dat;
        int32 RP_cnt;
        int8 bus, n;
        bool ok[64];
        float tmp[64];
        int16 age[64];
  
        sprintf(header, "%s H ", file_name);
        if (P) print_AMS_block_header(1, pBlock);

        swap16(pBlock->Data.p16, 2*pBlock->DataCount);
        RP_dat = (int8*)pBlock->Data.p16;
        RP_cnt = pBlock->DataCount;
        if (pBlock->DataType == 0x19) {
          if (P > 1) printf("p = %08X, pBlock->DataCount = %d\n", (int32)RP_dat, pBlock->DataCount);
          if ((RP_cnt - 1) % 4) {
            if (P > 1) printf("ERROR\n");
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
          if (bus == 6 || bus == 7 || bus == 8) {
            struct tm *t;
            char data_time[80];
            t = localtime((time_t*)&pBlock->DataTime);
            sprintf(data_time, "%4d%02d%02d %02d:%02d ", 
                    t->tm_year+1900, t->tm_mon+1, t->tm_mday,
                    t->tm_hour, t->tm_min);
            printf("%s %s Dallas bus %d\n", 
                   data_time, NodeName[pBlock->NodeAddress], bus);
            for (i=0; i<n; i++) {
              if (ok[i]) printf("%2d %+7.2f %6d\n", i, tmp[i], age[i]);
              else       printf("%2d         %6d\n", i, age[i]);
            }
          }
        }
      }
    }

    jmdc_id = 0x00C <= pBlock->NodeAddress && pBlock->NodeAddress <= 0x00F;
    if (jmdc_id && (pBlock->DataType == 0x05 || pBlock->DataType == 0x06)) {
      bool calibration = pBlock->DataType == 0x06;
      if (calibration) sprintf(header, "%s C ", file_name);
      else             sprintf(header, "%s D ", file_name);
      print_AMS_block_header(1, pBlock);
      end = dat + (len + 1) / 2;
      p   = pBlock->Data.p16;

      while (1) {
        int16 *pp;
        len = *p++;
        unpack16_AMS_block(p, len, FALSE, pBlock);
        jmdc_id = 0x00C <= pBlock->NodeAddress && pBlock->NodeAddress <= 0x00F;
//      jinj_id = 0x084 <= pBlock->NodeAddress && pBlock->NodeAddress <= 0x087;
        jinj_id = 0x080 <= pBlock->NodeAddress && pBlock->NodeAddress <= 0x087;
        jmdc_event =  jmdc_id && pBlock->DataType == 0x07;
        jinj_event =  jinj_id && pBlock->DataType == 0x01;
        jinf_event = !jinj_id && pBlock->DataType == 0x01;
        print_AMS_block_header(3, pBlock);
        pp = pBlock->Data.p16;

        if (jmdc_event) {
//        print_AMS_data_block("", pBlock);
          run_tag = *pp++;
          run_tag = run_tag << 16 | *pp++;
//        printf("%08X ", run_tag);
          run_id = *pp++;
          run_id = run_id << 16 | *pp++;
//        printf("%08X ", run_id);
          event_id = *pp++;
          event_id = event_id << 16 | *pp++;
//        printf("%08X ", event_id);
//        printf("%08X\n", pBlock->DataTime);
          if (first_event_in_file) {
            struct tm *t;
            t = localtime((time_t*)&pBlock->DataTime);
            sprintf(first_event_time, "%4d%02d%02d %02d:%02d ", 
                   t->tm_year+1900, t->tm_mon+1, t->tm_mday,
                   t->tm_hour, t->tm_min);
            first_event_in_file = FALSE;
          }
        }

        if (jinj_event) {
          int16 err;
          pJINJ_event->dat = pp;
          pJINJ_event->len = (pBlock->DataCount + 1) / 2;

/*
          if (first_event_in_file) {
            t = localtime((time_t*)&pBlock->DataTime);
            sprintf(first_event_time, "%4d%02d%02d %02d:%02d ", 
                   t->tm_year+1900, t->tm_mon+1, t->tm_mday,
                   t->tm_hour, t->tm_min);
            first_event_in_file = FALSE;
          }
*/

          err = decode_JINJ_event(pJINJ_event);
          nev[0][MIN(err,sizeof(nev[0])/sizeof(nev[0][0]))]++;
//        if (!err) {
            process_JINJ_event(pJINJ_event);
//        }
        }

        if (jinf_event) {
          int16 err;
          pJINF_event->dat = pp;
          pJINF_event->len = (pBlock->DataCount + 1) / 2;
          if (first_event_in_file) {
            struct tm *t;
            t = localtime((time_t*)&pBlock->DataTime);
            sprintf(first_event_time, "%4d%02d%02d %02d:%02d ", 
                   t->tm_year+1900, t->tm_mon+1, t->tm_mday,
                   t->tm_hour, t->tm_min);
            first_event_in_file = FALSE;
          }

          err = decode_JINF_event(pJINF_event);
          nev[1][MIN(err,sizeof(nev[1])/sizeof(nev[1][0]))]++;
          if (!err) {
            process_JINF_event(pJINF_event);
          }
        }

        p = p + (len + 1) / 2;
        if (p >= end) {
          if (p > end) {
            if (P > 1) printf("%s       *** Error: Pointer mismatch(1): p=%08X, end=%08X\n", 
                          header, (int32)p, (int32)end);
            nev[0][1]++;
          }
          break;
        }
      }
//exit(1);
    }
  }

  for (i=0; i<sizeof(nev[0])/sizeof(nev[0][0]); i++) {
    if (!i) n_bad = 0;
    else    n_bad = n_bad + nev[0][i];
  }
  n_tot = nev[0][0] + n_bad;

  if (n_tot) {
    printf("%s", header);
    printf("%s", first_event_time);
    for (i=0; i<24; i++) {
      if (!(i % 4)) printf(" ");
      if (JINJ_fra_pre[i]) {
        if (JINJ_fra_pre[i] == n_tot) printf("%c", toupper(jinjlinks[i][0]));
        else                          printf("%c", tolower(jinjlinks[i][0]));
      }
      else {
        printf("-");
      }
    }
    printf(" : ");
    for (i=0; i<sizeof(nev[0])/sizeof(nev[0][0]); i++) printf("%7d", nev[0][i]);
    printf(" : %6.4f * ", (float)n_bad/(float)(n_tot));
    if (nev[0][6]) {
      for (i=0; i<24; i++) {
        if (JINJ_fra_err[i]) printf("%s:%d ", jinjlinks[i], JINJ_fra_err[i]);
      }
    }
    printf("\n");

    if (strcasestr(PP, "M")) {
      printf("Average Event Length: ");
      for (i=0; i<24; i++) {
        int mean = 0;
        if (JINJ_fra_nok[i]) mean = (float)JINJ_fra_len[i] / (float)JINJ_fra_nok[i] + 0.5;
        if (mean) printf("%s:%d ", jinjlinks[i], mean);
      }
      printf("\n");
    }
  }
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

  P = 0;
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
      process_AMS_block_file(block_file, current_file);
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
