// file gsclib.c
//
// Stolen from Cai, SEU, VK and reworked
//
// A.Lebedev  Nov-2007...
//

#include "gsclib.h"

int PP;
int current_file;

bool READING_HRDL_BOARD;       // used in write_frames
bool WRITING_FRAME_FILES;      // used in write_frames

bool CREATING_BLOCK_FILES;     // used in deframing
bool WRITING_BLOCK_FILES;      // used in deframing

struct {
  int16  num;
  char  *txt;
} APIDs[] = {
  {976, "SCI"},
  {977, "HK"},
  {981, "P2P"},
  {876, "HK"},
  {980, "P2P"}};

char *temp_file_names[2] = {"temp_file_1", "temp_file_2"};

//~============================================================================
//
//            GENERAL LIBRARY
//
//~============================================================================

void remove_trailing_slash(char *s) {

  if (s[strlen(s)-1] == '/') s[strlen(s)-1] = '\0';
}

//~----------------------------------------------------------------------------

FILE *open_file_for_reading(char *directory, int file) {

  FILE *f;
  char b[800];
  int n[6] = {1, 10, 100, 1000, 10000, 100000};

  sprintf(b, "%s/%04d/%03d", directory, file / n[3], file % n[3]);
  f = fopen(b, "rb");
  if (!f) {
//  printf("fopen(%s,\"rb\") failed: %s\n", b, strerror(errno));
  }
  else {
    if (P > 5) printf("File %s opened for reading\n", b);
  }

  return f;
}

//~----------------------------------------------------------------------------

FILE *open_temp_file_for_writing(char *directory, int temp_file) {

  FILE *f;
  char b[800];

  sprintf(b, "%s/%s", directory, temp_file_names[temp_file]);
  f = fopen(b, "wb");
  if (!f) {
    printf("fopen(%s,\"wb\") failed: %s\n", b, strerror(errno));
    exit(1);
  }
  else {
    if (P > 5) printf("File %s opened for writing\n", b);
  }

  return f;
}

//~----------------------------------------------------------------------------

void rename_temp_file(char *directory, int temp_file, int file) {

  DIR *dir;
  struct passwd *pwd;
  struct group *grp;
  char b[800], bb[800];
  int n[6] = {1, 10, 100, 1000, 10000, 100000};

  pwd = getpwnam("ams");
  if (!pwd) {
    printf("getpwnam('ams') failed: %s\n", strerror(errno));
    exit(1);
  }
  grp = getgrnam("ams");
  if (!grp) {
    printf("getgrnam('ams') failed: %s\n", strerror(errno));
    exit(1);
  }

  sprintf(bb, "%s/%04d", directory, file / n[3]);
  dir = opendir(bb);
  if (!dir) {
    if (mkdir(bb, 0777)) {
      printf("mkdir(%s,...) failed: %s\n", bb, strerror(errno));
      exit(1);
    }
    else {
      if (chown(bb, pwd->pw_uid, grp->gr_gid)) {
        printf("chown(%s, 'ams', 'ams') failed: %s\n", bb, strerror(errno));
        exit(1);
      }
    }
  }
  else {
    closedir(dir);
  }
  
  sprintf(b,  "%s/%s", directory, temp_file_names[temp_file]);
  sprintf(bb, "%s/%03d", bb, file % n[3]);
  if (rename(b, bb)) {
    printf("rename(%s,%s) failed: %s\n", b, bb, strerror(errno));
    exit(1);
  }

  if (chown(bb, pwd->pw_uid, grp->gr_gid)) {
    printf("chown(%s, 'ams', 'ams') failed: %s\n", bb, strerror(errno));
    exit(1);
  }

  if (P > 5) printf("File %s is renamed to %s\n", b, bb);
}

//~----------------------------------------------------------------------------

FILE *open_perm_file_for_writing(char *directory, int file) {

  FILE *f;
  DIR *dir;
  char b[800], bb[800];
  int n[6] = {1, 10, 100, 1000, 10000, 100000};

  sprintf(bb, "%s/%04d", directory, file / n[3]);
  dir = opendir(bb);
  if (!dir) {
    if (mkdir(bb, 0777)) {
      printf("mkdir(%s,...) failed: %s\n", bb, strerror(errno));
      exit(1);
    }
  }
  else {
    closedir(dir);
  }

  sprintf(b, "%s/%04d/%03d", directory, file / n[3], file % n[3]);
  f = fopen(b, "wb");
  if (!f) {
    printf("fopen(%s,\"wb\") failed: %s\n", b, strerror(errno));
    exit(1);
  }
  else {
    if (P > 5) printf("File %s opened for writing\n", b);
  }

  return f;
}

//~----------------------------------------------------------------------------

int last_file_in(char *DirPath, int n) {

  int last_file;
  DIR  *dir;
  struct dirent *d;
  char *str[8] = {
    "[0-9]",
    "[0-9][0-9]",
    "[0-9][0-9][0-9]",
    "[0-9][0-9][0-9][0-9]",
    "[0-9][0-9][0-9][0-9][0-9]",
    "[0-9][0-9][0-9][0-9][0-9][0-9]",
    "[0-9][0-9][0-9][0-9][0-9][0-9][0-9]",
    "[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]"};

  if (P) printf("-- Start scan of directory:  %s\n", DirPath);
  dir = opendir(DirPath);
  if (!dir) {
    printf("opendir(%s) failed: %s\n", DirPath, strerror(errno));
    return -1;
  }

  last_file = -1;
  while ((d = readdir(dir))) {
    if (!fnmatch(str[n-1], d->d_name, 0)) {
      last_file = MAX(last_file, atol(d->d_name));
    }
  }

  if (closedir(dir)) {
    printf("closedir(%s) failed: %s\n", DirPath, strerror(errno));
    return -1; 
  }

  if (P) {
    if (last_file < 0) {
      printf("-- No files found in %s\n", DirPath);
    }
    else {
      printf("-- Last file found in %s is %0*d\n", DirPath, n, last_file);
    }
  }
  
  return last_file;
}

//~----------------------------------------------------------------------------

int the_last_file_in(char *directory) {

  int last_file[2];
  char b[800];
  int n[6] = {1, 10, 100, 1000, 10000, 100000};
  
  sprintf(b, "%s", directory);
  last_file[0] = last_file_in(b, 4);
  if (last_file[0] < 0) return -1;
  sprintf(b, "%s/%04d", directory, last_file[0]);
  last_file[1] = last_file_in(b, 3);
  if (last_file[1] < 0) return -1;

  return (last_file[0] * n[3] + last_file[1]);
}

//~============================================================================

bool read_AMS_block(FILE *file, int16 *dat, int32 *len) {

  int r_cnt;
  int16 wc1, wc2;
  int s;

  r_cnt = 1;
  s = fread(&wc1, 2, r_cnt, file);
  if (!s) return FALSE;
  if (s != r_cnt) {
    printf("%s: *** 1: r_cnt = %d, s = %d\n", file_name, r_cnt, s);
    return FALSE;
  }

  if (wc1 & 0x8000) {
    r_cnt = 1;
    s = fread(&wc2, 2, r_cnt, file);
    if (!s) return FALSE;
    if (s != r_cnt) {
      printf("%s: *** wc2: r_cnt = %d, s = %d\n", file_name, r_cnt, s);
      return FALSE;
    }
    *len = (wc1 & 0x7FFF) << 16 | wc2;
  }
  else {
    *len = wc1;
  }

  r_cnt = (*len + 1) / 2;
  s = fread(dat, 2, r_cnt, file);
  if (!s) return FALSE;
  if (s != r_cnt) {
    printf("%s: *** dat: r_cnt = %d = %08X, s = %d\n", 
            file_name, r_cnt, r_cnt, s);
    return FALSE;
  }
  
  return TRUE;
}

//~----------------------------------------------------------------------------

bool unpack16_AMS_block(int16 *dat, int32 len, bool shf, AMSBlock *pBlock) {

  int16 *p = dat, *end;

  pBlock->Length    = len;
  pBlock->Data.p8   = NULL;
  pBlock->Data.p16  = NULL;
  pBlock->DataCount = 0;
  pBlock->Error     = 0;

  end = p + len - 1;

  if (p > end) {
    if (P > 0) printf("*** Error 0x1001 in unpack16_AMS_block\n");
    return FALSE;
  }
  
  pBlock->BlockType    = unpack16(*p,   0xC000);
  pBlock->RequestReply = unpack16(*p,   0x8000);
  pBlock->ReadWrite    = unpack16(*p,   0x4000);
  pBlock->NodeAddress  = unpack16(*p,   0x3FE0);
  pBlock->DataType     = unpack16(*p++, 0x001F);
  if (pBlock->DataType == 0x001F) {
    if (p > end) {
      if (P > 0) printf("*** Error 0x1002 in unpack16_AMS_block\n");
      return FALSE;
    }
    pBlock->DataType = 0x001F0000 | *p++;
  }

  if (shf) {
    pBlock->CommandTag = *p++;
    if (p > end) {
      if (P > 0) printf("*** Error 0x1003 in unpack16_AMS_block\n");
      return FALSE;
    }
    pBlock->DataTime = *p++;
    if (p > end) {
      if (P > 0) printf("*** Error 0x1004 in unpack16_AMS_block\n");
      return FALSE;
    }
    pBlock->DataTime = (pBlock->DataTime << 16) | *p++;
    if (p > end) {
      if (P > 0) printf("*** Error 0x1005 in unpack16_AMS_block\n");
      return FALSE;
    }
  }
    
  pBlock->Data.p16 = p;
  pBlock->DataCount = pBlock->Length - (p - dat) * 2;

  return TRUE;
}


//~============================================================================
//
//            SPECIFIC DEFRAMING LIBRARY
//
//~============================================================================

bool get_frame_from_file(FILE *file, pAMSframe p) {

  int i, k, s;
  int r_cnt;

//~--

  k = 0;
  r_cnt = 3;
  s = fread(&p->dat[k], 2, r_cnt, file);
  if (!s) return FALSE;
  if (s != r_cnt) {
    printf("%s: *** r_cnt = %d, s = %d\n", file_name, r_cnt, s);
    return FALSE;
  }
  swap16(&p->dat[k], r_cnt);
  
  p->shf  = unpack16(p->dat[0], 0x0800);
  p->APID = unpack16(p->dat[0], 0x07FF);

  if (p->dat[2] > 4084) {
    printf("%s: *** Wrong (1) p->dat[2] = %d = %04X\n", 
           file_name, p->dat[2], p->dat[2]);
    return FALSE;
  }
  
  if (p->APID == 876 && p->dat[2] > 2560) {
    printf("%s: *** Wrong (2) p->dat[2] = %d = %04X\n", 
           file_name, p->dat[2], p->dat[2]);
    return FALSE;
  }
  
  k = k + r_cnt;
  
  p->len  = r_cnt;
  r_cnt   = (p->dat[2] + 1) / 2;
  
//~--

  s = fread(&p->dat[k], 2, r_cnt, file);
  if (s != r_cnt) {
    printf("%s: *** r_cnt = %d, s = %d\n", file_name, r_cnt, s);
    return FALSE;
  }
  swap16(&p->dat[k], r_cnt);

  p->len = p->len + r_cnt;

{
//int i;
//static int c = 0;
//for (i=0; i<25; i++) printf("%4d ", i);
//printf("\n");
//for (i=0; i<25; i++) printf("%04X ", p->dat[i]);
//printf("\n");
//if (c++ > 10) exit(1);
}
  
  p->seq = unpack16(p->dat[1], 0x3FFF);
  if (p->APID == 876) p->SEQ = unpack16(p->dat[12], 0x3FFF);
  p->chk = unpack16(p->dat[5], 0x0020);
  if (!p->shf) p->chk = FALSE;
  i = (p->APID == 876) ? 13 : 8;
  p->rnd = unpack16(p->dat[i], 0x8000);
  p->crc = unpack16(p->dat[i], 0x4000);
  p->ext = unpack16(p->dat[i], 0x2000);
  p->CHD = unpack16(p->dat[i], 0x1000);
  p->lnk = unpack16(p->dat[i], 0x0FFF);
  p->bgn = i + 1 + p->CHD * 5;
  p->end = p->len - p->chk - p->crc;
  if ((p->lnk != 0x0FFF) && (p->lnk != 0x0FFE)) p->lnk = p->bgn + p->lnk;
  p->ind = p->bgn;

//~--

  if (P > 2) {
    printf("get_frame:\n");
    if (P > 3) {
      int max = 25;
      for (i=0; i<MIN(25,p->len); i++) printf("%4d ", i);
      printf("\n");
//if (p->seq == 4016) max =200;
//if (p->seq == 4017) exit(1);
//if (p->seq == 4016+100) exit(1);
      for (i=0; i<MIN(max,p->len); i++) {
        printf("%04X ", p->dat[i]);
        if (i && !((i + 1) % 25)) printf("\n");
      }
      if (MIN(max,p->len) % 25) printf("\n");
    }
    printf("len=%d ", p->len);
    printf("shf=%d ", p->shf);
    printf("APID=%d ", p->APID);
    printf("chk=%d ", p->chk);
    printf("seq=%4d ", p->seq);
    if (p->APID == 876) printf("SEQ=%4d ", p->SEQ);
    printf("rnd=%d ", p->rnd);
    printf("crc=%d ", p->crc);
    printf("ext=%d ", p->ext);
    printf("CHD=%d ", p->CHD);
    printf("bgn=%4d ", p->bgn);
    printf("end=%4d ", p->end);
    printf("lnk=%4d ", p->lnk);
    printf("\n");
  }

  p->err = 0x0000;
  return TRUE;
}

//~----------------------------------------------------------------------------

bool check_frame(pAMSframe p, pAMSblock pp) {

//~--

//if (p->lnk  == 0x0FFF) return FALSE;

//~--

  if (p->crc) {
    int i;
    int16 CRC = 0xFFFF;
    int16 g   = 0x1021;

    for (i=0; i<p->end+1; i++) {
      int j;
      int16 d = p->dat[i];
      for (j=0; j<16; j++) {
        if ((CRC ^ d) & 0x8000) CRC = (CRC << 1) ^ g;
        else                    CRC = (CRC << 1);
        d <<= 1;
      }
    }

    if (CRC) {
      if (PP) {
        printf("%s: %6d *** Wrong CRC=%04X\n", file_name, frame_number, CRC);
      }
      pp->sta = IDLE;
      return FALSE;
    }
  }  

//~--

  if ((pp->was != 0xFFFF) && (p->seq != MOD(pp->was+1, 0x4000))) {
    if (PP) {
      int i;
      int   n = 0;
      char *t = "?";
      for (i=0; i<sizeof(APIDs)/sizeof(APIDs[0]); i++) {
        if (p->APID == APIDs[i].num) {
          n = APIDs[i].num;
          t = APIDs[i].txt;
        }
      }
      printf("%s: %6d *** Wrong seq: APID=%d=%03X(%s) was=%d now=%d (sta = %d)\n", 
             file_name, frame_number, n, n, t, pp->was, p->seq, pp->sta);
    }
    pp->was = p->seq;
    pp->sta = IDLE;
    return TRUE;
  }
  pp->was = p->seq;

//~--

  if (p->lnk  == 0x0FFF) return FALSE;

//~--

  if (p->lnk < p->bgn) {
    if (PP) printf("%s: %6d *** Wrong lnk=%04X=%4d (bgn=%4d, sta=%d)\n", 
                    file_name, frame_number, p->lnk, p->lnk, p->bgn, pp->sta);
    pp->sta = IDLE;
    return FALSE;
  }
  
  if ((p->lnk != 0x0FFE) && (p->lnk >= p->end)) {
    if (p->lnk == 2039) {
      if (PP) printf("%s: %6d *** Warning: Wrong lnk=%04X=%4d (end=%4d, sta=%d)\n", 
                      file_name, frame_number, p->lnk, p->lnk, p->end, pp->sta);
    }
    else {
      if (PP) printf("%s: %6d *** Wrong lnk=%04X=%4d (end=%4d, sta=%d)\n", 
                     file_name, frame_number, p->lnk, p->lnk, p->end, pp->sta);
      pp->sta = IDLE;
      return FALSE;
    }
  }

//~--

  return TRUE;
}

//~----------------------------------------------------------------------------

void extract_CHD(pAMSframe p, FILE *file) {

  if (P > 2) {
    int i, n = 0;
    char *t = "?";
    printf("extract_CHD: ");
    for (i=0; i<sizeof(APIDs)/sizeof(APIDs[0]); i++) {
      if (p->APID == APIDs[i].num) {
        n = APIDs[i].num;
        t = APIDs[i].txt;
      }
    }
    printf("APID=%d=%03X(%3s) ", n, n, t);
    printf("p->seq = %5d: ", p->seq);
//  printf("p->time = %08X: ", p->time);   // it is zero
  }

  if (p->CHD) {
    if (P > 2) {
      int i;
      for (i=p->bgn-5; i<p->bgn; i++) {
        printf("%02hhX %02hhX ", (int8)(p->dat[i]>>8), (int8)(p->dat[i]&0x00FF));
      }
      printf("\n");
    }
    if (p->wri) {
      time_t now = time(NULL);
      if (WRITING_BLOCK_FILES) {
//      int16 n1 = (int32)now >> 16;
//      int16 n2 = (int32)now >>  0;
//      fwrite(&n1, 2, 1, file);
//      fwrite(&n2, 2, 1, file);
        fwrite(&now, 4, 1, file);
        fwrite(&p->APID, 2, 1, file);
        fwrite(&p->seq, 2, 1, file);
        fwrite(&p->dat[p->bgn-5], 2, 5, file);
      }
    }
  }
  else {
    if (P > 2) {
      printf("no CHD found\n");
    }
  }
}

//~----------------------------------------------------------------------------

void deframe_frame(pAMSframe p, pAMSblock pp, FILE *file) {

  int s;
  bool first = TRUE;
  
  if (P > 2) printf("deframe_frame:\n");

  while (1) {
    switch (pp->sta) {

    case IDLE:
      if (P > 2) printf("  pp->sta=IDLE:\n");
      first = FALSE;
      if ((p->lnk != 0x0FFE) && (p->lnk != 0x0FFF) && (p->lnk != 2039)) {
        p->ind  = p->lnk;
        pp->sta = BLOCK_LENGTH_1;
      }
      else {
        return;
      }
    break;
    
    case BLOCK_LENGTH_1:
      if (P > 2) printf("  pp->sta=BLOCK_LENGTH_1:");
      first = FALSE;
      pp->ind = 0;
      pp->wc = p->dat[p->ind];
      if (P > 2) printf(" wc=%04X=%d\n", pp->wc, pp->wc);
      if (pp->wc) {
        if (pp->wc & 0x8000) {
          pp->wc  = (pp->wc & 0x00007FFF) << 16;
          p->ind  = p->ind + 1;
          pp->sta = BLOCK_LENGTH_2;
          if (!(p->ind < p->end)) return;
        }
        else {
          p->ind  = p->ind + 1;
          pp->sta = BLOCK_BODY;
          if (!(p->ind < p->end)) return;
        }
      }
      else {
        pp->sta = IDLE;
        return;
      }
    break;

    case BLOCK_LENGTH_2:
      if (P > 2) printf("  pp->sta=BLOCK_LENGTH_2:");
      first = FALSE;
      pp->wc  = pp->wc | p->dat[p->ind];
      if (P > 2) printf(" wc=%04X=%d\n", pp->wc, pp->wc);
      p->ind  = p->ind + 1;
      pp->sta = BLOCK_BODY;
      if (!(p->ind < p->end)) return;
    break;
    
    case BLOCK_BODY:
      if (P > 2) {
        printf("  pp->sta=BLOCK_BODY:");
        printf(" p->ind=%-4d pp->ind=%-4d (pp->wc+1)/2=%-5d (pp->wc+1)/2-pp->ind=%-5d (pp->wc+1)/2-pp->ind+p->bgn=%-5d p->end-p->ind=%-5d\n",
                 p->ind,     pp->ind,     (pp->wc+1)/2,     (pp->wc+1)/2-pp->ind,     (pp->wc+1)/2-pp->ind+p->bgn,     p->end-p->ind);
      }
      if (first) {
        first = FALSE;
        if (p->lnk != 0xFFF && p->lnk != 0xFFE && 
            p->lnk != (pp->wc + 1) / 2 - pp->ind + p->bgn) {
          if (PP) {
            printf("%s: %d *** Wrong lnk = %d != %d\n", 
                   file_name, frame_number, p->lnk, (pp->wc + 1) / 2 - pp->ind + p->bgn);
          }
          pp->sta = IDLE;
          break;
        }
      }
      while (1) {
        pp->dat[pp->ind++] = p->dat[p->ind++];
        if (!(pp->ind < (pp->wc+1)/2) || !(p->ind < p->end)) break;
      }
      if (!(pp->ind < (pp->wc+1)/2)) {
        if (p->wri) {
          int16 w1 = pp->wc >> 16;
          int16 w2 = pp->wc >>  0;
          if (P > 2) printf("write block wc=%d\n", pp->wc);
          if (pp->wc > 0x7FFF) {
            w1 = w1 | 0x8000;
            if (WRITING_BLOCK_FILES) {
              s = fwrite(&w1, 2, 1, file);
              if (s != 1) {
                printf("It seems the disk is full (1)\n");
                exit(1);
              }
            }
          }
          if (WRITING_BLOCK_FILES) {
            s = fwrite(&w2, 2, 1, file);
            if (s != 1) {
              printf("It seems the disk is full (2)\n");
              exit(1);
            }
            s = fwrite(&pp->dat[0], 2, (pp->wc+1)/2, file);
            if (s != (pp->wc+1)/2) {
              printf("It seems the disk is full (2)\n");
              exit(1);
            }
          }
        }
        pp->sta = BLOCK_LENGTH_1;
      }
      if (!(p->ind < p->end)) {
        return;
      }
    break;
    }
  }
}

//~============================================================================
//
//            REED - SOLOMON ENCODING/DECODING
//
//~============================================================================
 
#define j0 1                  // alpha**j0 is the first root of generator polynomial
                              // in fact, one can choose any j0 for RS code

int M = 8;                    // RS code over GF(2**M)
int K;                        // K = number of information symbols
int N = 255;                  // N = number of symbols in code word (=block)

unsigned char Alpha_to[256];  // index->polynomial form conversion table
                              // i.e., anti-logarithmic table
unsigned char Index_of[256];  // polynomial->index form conversion table 
                              // i.e., logarithmic table

unsigned char Index_of_0;     // special value for zero in index form

unsigned char G[256];         // G[] stores coefficients of the RS code
                              // generator polynomial g(x), 
                              // deg g(x) = 2*t = N-K

// This code supports a symbol size of 8 bits only,
// and a block size of 256 8-bit symbols (2048 bits). 
// The code parameter K is set by initialize_RS function.

//~----------------------------------------------------------------------------

void initialize_RS( const int k) {

// Generate all elements of the Galois field GF(2**M) using the irreducible
// (a.k.a. primitive) polynomial p(x) whose (binary) coefficients are 
// stuffed into the bits of the variable P.
//
// Essentially, produce two lookup tables:
//
//       1. index form to polynomial form:  Alpha_to[i] contains j=alpha**i
//       2. polynomial form to index form:  Index_of[j=alpha**i] contains i
//
//            where alpha=2 is the primitive element of GF(2**M)
//
// How to use these lookup tables?
//
// Let "alpha" be the primitive element that is the root of the primitive 
// polynomial p(x). 
// Then in GF(2**M), for any 0 <= i <= 2**M-2,
//
// alpha**i = a(0) + a(1)*alpha + a(2)*alpha**2 + ... + a(M-1)*alpha**(M-1)
//
// where the binary vector (a(0),a(1),a(2),...,a(M-1)) is the representation
// of the integer "alpha_to[i]" with a(0) being the LSB and a(M-1) the MSB. 
// For example, the polynomial representation of alpha**5 would be given by 
// the binary representation of the integer "alpha_to[5]".
//
// Let again "alpha" represent the primitive element of GF(2**M) that is the 
// root of the primitive polynomial p(x). In order to find the power of 
// "alpha" that has the polynomial representation
//
//       a(0) + a(1)*alpha + a(2)*alpha**2 + ... + a(M-1)*alpha**(M-1)
//
// we consider the integer "i" whose binary representation with a(0) being 
// LSB and a(M-1) MSB is (a(0),a(1),...,a(M-1)) and locate the entry 
// "index_of[i]". 
// Now, alpha**index_of[i] is that element whose polynomial representation 
// is (a(0),a(1),a(2),...,a(M-1)).
//
// The element alpha_to[2**M-1] = 0 always signifying that the representation 
// of "alpha**infinity" = 0 is (0,0,0,...,0).
//
// The element Index_of[0] = N always signifying that the power of "alpha" 
// which has the polynomial representation (0,0,...,0) is "infinity".

#define DEBUG 0

  int i, j, P = 0;

  K = k;
  Index_of_0 = N;

  if (DEBUG) {
    printf("\nInitialize_RS: N,M,K = %d,%d,%d\n", N, M, K);
  }

// Primitive (=irreducible) polynomials P are taken from:
// W.W.Peterson, Error-Correcting Codes, Second Edition,
// The MIT Press, 1972, Appendix C, Table C.2, pp 476-495.

  if (M == 2) P = 0007;
  if (M == 3) P = 0013;
  if (M == 4) P = 0023;
  if (M == 5) P = 0045;
  if (M == 6) P = 0103;
  if (M == 7) P = 0211;
  if (M == 8) P = 0435;

  Alpha_to[0] = 1;
  for (i=1; i<N+1; i++) {
    Alpha_to[i] = Alpha_to[i-1] << 1;
    if ((Alpha_to[i-1] & (1 << (M - 1))) != 0) {
      Alpha_to[i] ^= P;
    }
    Index_of[Alpha_to[i]] = i;
  }
  Alpha_to[N] = 0;

  Index_of[0] = Index_of_0;
  Index_of[Alpha_to[0]] = 0;

  if (DEBUG) {
    printf("\n Alpha_to[] = %d %d %d %d ... %d %d \n",
    Alpha_to[0], Alpha_to[1], Alpha_to[2], Alpha_to[3],
    Alpha_to[N-1], Alpha_to[N]);
  }

// Generator polynomial g(x) for (N,K) Reed-Solomon code is calculated
// as it is described in 
// R.E.Blahut, Theory and Practice of Error Control Codes, 
// Addison-Wesley, 1983, p.174. 
// 
// The generator polynomial is a product:
//
// g(x)=(x+alpha**j0)*(x+alpha**(j0+1))*...*(x+alpha**(j0+(N-K-1)))
//
// where: alpha is a primitive element of GF(2**M);
//        j0 is arbitrary number.
//
// The polynomial coefficients are stored in G[], so that
//
// g(x)=G[0]+G[1]*x + ... +G[N-K]x**(N-K)
//
// Of course, all multiplications and additions should be performed
// in the Galois field GF(2**M).
// That's why logarithmic and antilogarithmic tables Alpha_to[] and 
// Index_of[] are used. 
// At the end, the g(x) coefficients G[] are transormed into the index 
// form to speed up the RS encoding.

// set g(x) = (alpha**j0+x)

  G[0] = Alpha_to[j0];
  G[1] = 1;
  for (i=1; i<=N-K-1; i++) {

//  multiply:
//  (G[0]+G[1]*x + ... +G[i]x**i) by (alpha**(j0+i) + x) in GF(2**M)

    G[i+1] = G[i];
    for (j=i; j>0; j--) {

//      calculate G[j] = G[j]<*>alpha**(j0+i) <+> G[j-1]
//      where operations <*> and <+> are in Galois field GF(2**M)

      if (G[j] != 0) {
        G[j] = Alpha_to[MOD((Index_of[G[j]]) + j0 + i, N)];
      }
      G[j] = G[j] ^ G[j - 1];
    }
    G[0] = Alpha_to[MOD((Index_of[G[0]]) + j0 + i, N)];
  }

//  go to index form to facilitate further usage

  for (i=0; i<=N-K; i++) G[i] = Index_of[G[i]];
  if (DEBUG) {
    printf("\n G[] = %d %d %d %d ... %d %d \n", 
    G[0], G[1], G[2], G[3], G[N-K-1], G[N-K]);
  }
}
#undef DEBUG

//~----------------------------------------------------------------------------

void encode_RS(unsigned char data[/*K*/], unsigned char T[/*N-K*/]) {

// Take the string of symbols in data[i], i=0..(k-1) and encode it
// systematically to produce n-k parity symbols in T[0]..T[N-K-1]. 
//
// Array data[] is input and T[] is output, and both are in polynomial 
// form. 
//
// Encoding is done by using a feedback shift register with appropriate 
// connections specified by the elements of G[], which was generated 
// above.
// 
// The resulting codeword is c(x) = data(x)*x**(N-K) + t(x)
// see Blahut pp.136-137


  int i, j;
  unsigned char carry;

  for (i=0; i<N-K; i++) {
    T[i] = 0;
  }
  for (i=K-1; i>=0; i--) {
    carry = Index_of[data[i] ^ T[N-K-1]];
    if (carry != Index_of_0) {
      for (j=N-K-1; j>0; j--) {
        T[j] = T[j-1];
        if (G[j] != Index_of_0) {
          T[j] ^= Alpha_to[MOD(G[j] + carry, N)];
        }
      }
      T[0] = Alpha_to[MOD(G[0] + carry, N)];
    }
    else {
      for (j=N-K-1; j>0; j--) {
        T[j] = T[j-1];
      }
      T[0] = 0;
    }
  }
}

//~----------------------------------------------------------------------------

int decode_RS(unsigned char data[/*N*/]) {

// Performs ERRORS decoding of RS codes. If decoding is successful,
// writes the codeword into data[] itself. Otherwise data[] is unaltered.
//
// Return number of symbols corrected, or -1 if codeword is illegal
// or uncorrectable.
// 
// First "no_eras" erasures are declared by the calling program. Then, 
// the maximum # of errors correctable is 
// t_after_eras = floor((N-K-no_eras)/2).
// If the number of channel errors is not greater than "t_after_eras"
// the transmitted codeword will be recovered. Details of algorithm 
// can be found in R. Blahut's "Theory ... of Error-Correcting Codes".


#define DEBUG 0

        int deg_lambda, el, deg_omega;
        int i, j, r;
        int q, tmp, num1, num2, den, discr_r;
        unsigned char recd[255];
        unsigned char lambda[255];      // Err+Eras Locator polynomial
        unsigned char S[255];           // syndromes, 
                                        // not syndrome polynomial
        unsigned char b[255], t[255], omega[255];
        unsigned char root[255], reg[255], loc[255];
        int errors, count;
// copy data[] to recd[] converting to index form

 for (i=0; i<N; i++) {
   recd[i] = Index_of[data[i]];
 }

// calculate syndromes as values of recd(x) at roots of 
// generator polynomial g(x) i.e. at 
// x=alpha**(j0+i), i = 0,1,2,..,(N-K-1) (see Blahut p.162)

  errors = 0;
  for (i=0; i<N-K; i++) {   
    tmp = 0;
    for (j=0; j<N; j++) {
      if (recd[j] != Index_of_0) {
        tmp ^= Alpha_to[(recd[j] + (j0+i)*j) % N];
      }
    }
    errors |= tmp;           // accumulate errors
    S[i+1] = Index_of[tmp];  // store syndromes in index form
  }
  if (!errors) {
    return (0);              // no errors found, exit here
  }

  for (i=0; i<N-K+1; i++) {
    lambda[i] = (i == 0) ? 1 : 0;
    b[i] = Index_of[lambda[i]];
  }

// Begin Berlekamp-Massey algorithm 
// to determine error+erasure locator polynomial

  r = 0;
  el = 0;
  while (++r <= N-K) {    // r is the step number

//  Compute discrepancy at the r-th step in poly-form

    discr_r = 0;
    for (i=0; i<r; i++) {
      if ((lambda[i] != 0) && (S[r - i] != Index_of_0)) {
        discr_r ^= Alpha_to[(Index_of[lambda[i]] + S[r - i]) % N];
      }
    }
    discr_r = Index_of[discr_r];     // go to index form

    if (discr_r == Index_of_0) { 
      for (i=N-K-1; i>=0; i--) {
        b[i+1] = b[i];               // b(x) = x*b(x)
      }
      b[0] = Index_of_0;
    } 
    else {                     

//  t(x) = lambda(x) - discr_r*x*b(x)

      for (i=N-K-1; i>=0; i--) {
        t[i+1] = lambda[i+1];
        if (b[i] != Index_of_0) {
          t[i+1] ^= Alpha_to[MOD(discr_r + b[i], N)];
        }
      }
      t[0] = lambda[0];

      if (2*el <= r-1) {
        el = r - el;

// b(x) = lambda(x)/discr_r

        for (i=0; i<=N-K; i++) {
          b[i] = (lambda[i] == 0) ? 
                 Index_of_0 : MOD(Index_of[lambda[i]] - discr_r + N, N);
        }
      } 
      else {
        for (i=N-K-1; i>=0; i--) {
          b[i+1] = b[i];              // b(x) = x*b(x)
        }
        b[0] = Index_of_0;
      }
      for (i=0; i<N-K+1; i++) {
        lambda[i] = t[i];             // lambda(x) = t(x)
      }
    }
  }  // end of   while (++r <= n-k)

// Convert lambda into index form and calculate degree of lambda(x)

  deg_lambda = 0;
  for (i=0; i<N-K+1; i++) {
    lambda[i] = Index_of[lambda[i]];
    if(lambda[i] != Index_of_0) deg_lambda = i;
  }

// Use Chien search algorithm to find roots 
// of the error locator polynomial

  for (i=1; i<N-K+1; i++) {
    reg[i] = lambda[i];
  }

  count = 0;              // Number of roots of lambda(x)
  for (i=1; i<=N; i++) {
    q = 1;
    for (j=deg_lambda; j>0; j--) {
      if (reg[j] != Index_of_0) {
        reg[j] = MOD(reg[j] + j, N);
        q ^= Alpha_to[reg[j]];
      }
    }
    if (!q) {
      root[count] = i;    // store root in index-form
      loc[count] = N - i; // store error location number
      count++;
    }
  }

  if (DEBUG) {
    printf("\n Final error positions:\t");
    for (i=0; i<count; i++) {
      printf("%d ", loc[i]);
    }
    printf("\n");
  }

// if deg(lambda) unequal to number of roots => 
// => uncorrectable error detected

  if (deg_lambda != count) {
    return (-1);
 }

// Calculate error evaluator polynomial 
// omega(x) = s(x)*lambda(x) (modulo x**(N-K))
// (in index form of course). 
// Calculate deg(omega).

  deg_omega = 0;
  for (i=0; i<N-K; i++) {
    tmp = 0;
    for (j=(deg_lambda<i) ? deg_lambda : i; j>=0; j--) {
      if ((S[i+1-j] != Index_of_0) && (lambda[j] != Index_of_0)) {
        tmp ^= Alpha_to[MOD(S[i+1-j] + lambda[j], N)];
      }
    }
    if (tmp != 0) {
      deg_omega = i;
    }
    omega[i] = Index_of[tmp];
  }
  omega[N-K] = Index_of_0;

// Calculate error values in polynomial form:
// num1 = omega(inv(X(l)))
// num2 = inv(X(l))**(j0-1) 
// den  = lambda_pr(inv(X(l)))

  for (j=count-1; j>=0; j--) {
    num1 = 0;
    for (i=deg_omega; i>=0; i--) {
      if (omega[i] != Index_of_0) {
        num1 ^= Alpha_to[(omega[i] + i * root[j]) % N];
      }
    }
    num2 = Alpha_to[MOD(root[j] * (j0-1) + N, N)];
    den = 0;

// lambda[i+1] for i even is the formal derivative 
// lambda_pr of lambda[i] 

    for (i=MIN(deg_lambda,N-K-1) & ~1; i>=0; i-=2) {
      if(lambda[i+1] != Index_of_0) {
        den ^= Alpha_to[(lambda[i+1] + i * root[j]) % N];
      }
    }
    if (den == 0) {
      if (DEBUG) {
        printf("\n ERROR: denominator = 0\n");
      }
      return (-1);
    }

// Apply error to data  (=make correction)

    if (num1 != 0) {
      data[loc[j]] ^= Alpha_to[(Index_of[num1] + Index_of[num2] + 
                      N - Index_of[den]) % N];
    }
  }
  return (count);
}
#undef DEBUG

//~============================================================================
