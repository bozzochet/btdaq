// file mylib.c
//
// A.Lebedev - Jun-2003...

#undef DEBUG_mytypes
#undef DEBUG_kbhit
#undef VERBOSE_kbhit

#include "mylib.h"

//~============================================================================

int sprintf_binary(char *b, int32 number, int mode) {

  int nb_of_digits;
  int i, j = 0, nb_of_ones = 0;

  if (mode  < 0)      nb_of_digits = -mode;
  else if (mode == 0) nb_of_digits =  32;
  else                nb_of_digits =  mode;

  for (i=0; i<nb_of_digits; i++) {
    if (mode < 0) {
      if (i != 0 && i % 4 == 0) {
        sprintf(b+j, " ");
        j++;
      }
    }
    if (number & 1 << (nb_of_digits-1-i)) {
      nb_of_ones++;
      sprintf(b+j, "1");
      j++;
    }
    else {
      if (mode || nb_of_ones || i == nb_of_digits-1) {
        sprintf(b+j, "0");
        j++;
      }
    }
  }

  return strlen(b);
}

//~----------------------------------------------------------------------------

int sprintf_huge(char *b, int64 integer, int d1, int64 fraction, int d2) {

  int i;
  char *p = b;
  bool z = TRUE;

  for (i=d1-1; i>=0; i--) {
    int d = unpack10(integer, i);
    if (d || !i) z = FALSE;
    if (!z) p += sprintf(p, "%d%s", d, (i % 3) ? "" : !i ? "." : ",");
  }

  for (i=d2-1; i>=0; i--) {
    int d = unpack10(fraction, i);
    p += sprintf(p, "%d%s", d, (i % 3) ? "" : !i ? "" : ",");
  }

  return strlen(b);
}

//~============================================================================

#ifdef DEBUG_mytypes

int main(void) {

  printf(" INT_MAX = %08X    LONG_MAX = %08X\n",  INT_MAX,  LONG_MAX);
  printf("UINT_MAX = %08X   ULONG_MAX = %08X\n", UINT_MAX, ULONG_MAX);

  printf("sizeof(int8 ) = %d bytes\n", sizeof(int8 ));
  printf("sizeof(int16) = %d bytes\n", sizeof(int16));
  printf("sizeof(int32) = %d bytes\n", sizeof(int32));
  printf("sizeof(int64) = %d bytes\n", sizeof(int64));
}

#endif

//~============================================================================

int32 packbytes(int32 *d, int8 *s, int n) {

  int i;
  int32 dd = 0;

  for (i=0; i<MIN(4,n); i++) {
    int32 m = 0x000000FF << 8 * (n - i - 1);
    pack32(&dd, m, (int32)s[i]);
  }

  if (d) *d = dd;
  return dd;
}

//~----------------------------------------------------------------------------

void unpackbytes(int8 *d, int32 s, int n) {

  int i;

  for (i=0; i<n; i++) {
    int32 m = 0x000000FF << 8 * (n - i - 1);
    d[i] = unpack32(s, m);
  }
}

//~============================================================================

int16 pack16(int16 *code, int16 mask, int16 number) {

  int16 LSB = mask & (mask ^ mask << 1);
  int16 n = number & unpack16(mask, mask);
  
  if (code) {
    *code = (*code & ~mask) | (n * LSB);
    return *code;
  }
  else {
    return (n * LSB);
  }
}

//~----------------------------------------------------------------------------

int16 unpack16(int16 code, int16 mask) {

  int16 LSB = mask & (mask ^ mask << 1);

  if (LSB) return (code & mask) / LSB;
  else     return 0;
}

//~----------------------------------------------------------------------------

int32 pack32(int32 *code, int32 mask, int32 number) {

  int32 LSB = mask & (mask ^ mask << 1);
  int32 n = number & unpack32(mask, mask);

  if (code) {
    *code = (*code & ~mask) | (n * LSB);
    return *code;
  }
  else {
    return (n * LSB);
  }
}

//~----------------------------------------------------------------------------

int32 unpack32(int32 code, int32 mask) {

  int32 LSB = mask & (mask ^ mask << 1);

  if (LSB) return (code & mask) / LSB;
  else     return 0;
}

//~----------------------------------------------------------------------------

int16 unpack10(int32 n, int p) {

  int32 m[10] = {
  1, 
  10, 
  100, 
  1000, 
  10000, 
  100000, 
  1000000, 
  10000000,
  100000000,
  1000000000};

  return (n / m[p]) % 10;
}

//~----------------------------------------------------------------------------

void pack10(int32 *n, int p, int d) {

  int32 m[10] = {
  1, 
  10, 
  100, 
  1000, 
  10000, 
  100000, 
  1000000, 
  10000000,
  100000000,
  1000000000};

  *n = (*n / m[p+1]) * m[p+1] + d * m[p] + *n % m[p];
}

//~----------------------------------------------------------------------------

int16 invert_bit_order(int16 code) {

  int16 new_code = 0;
  int i;

  for (i=0; i<16; i++) {
    new_code <<= 1;
    if (code & 0x0001 << i) new_code |= 0x0001;
  }

  return new_code;
}

//~----------------------------------------------------------------------------
//
//  stolen from Marco and reworked
//
//  swaps n rightmost bits of word
//

int32 swapbits(int32 word, int n) {

  int32 result;
  int i;

  if (n > 0) {
    result = 0;
    for (i=0; i<n; i++) if ((word >> i) & 0x00000001) result |= 1 << (n - i - 1);
  }
  else {
    result = word;
  }

  return result;
}

//~----------------------------------------------------------------------------

void swap16(int16 *dat, int n) {

  int i;
  
  for (i=0; i<n; i++) dat[i] = dat[i] << 8 | dat[i] >> 8;
}

//~============================================================================

float delta_t(struct timeval *t2, struct timeval *t1) {

  return ((t2->tv_sec - t1->tv_sec) + 0.000001 * (t2->tv_usec - t1->tv_usec));
}

//~----------------------------------------------------------------------------

void delay(float value) {

  struct timeval now, then;

  gettimeofday(&then, NULL);
  while (1) {
    gettimeofday(&now, NULL);
    if (delta_t(&now, &then) >= value) break;
  }
}

//~----------------------------------------------------------------------------

#define NCHA 16

bool timer(int16 chan, int func, ...) {

  va_list ap;
  static struct timeval now[NCHA], then[NCHA];
  float timeout;
  
  switch (func) {

    case START:
      if (chan >= NCHA) QUIT("Wrong chan in timer(chan,START)");
      gettimeofday(&then[chan], NULL);
      return TRUE;
    break;

    case EXPIRED:
      if (chan >= NCHA) QUIT("Wrong chan in timer(chan,EXPIRED,timeout)");
      va_start(ap, func);
      timeout = va_arg(ap, double);
      va_end(ap);
      gettimeofday(&now[chan], NULL);
      if (delta_t(&now[chan], &then[chan]) > timeout) return TRUE;
      else                                            return FALSE;
    break;
    
    default:
      QUIT("Wrong func in timer(func,chan)");
      return FALSE;
    break;
  }
}
#undef NCHA

//~----------------------------------------------------------------------------

void ShortSleep(int sec, int usec) {

  struct timeval tv;

  tv.tv_sec  = sec;
  tv.tv_usec = usec;

  select(0, NULL, NULL, NULL, &tv);
}

//~============================================================================

char *mytime(void) {

  int i;
  long ltime; 
  char *t; 
  static char tt[26];
  
  time(&ltime); 
  t = (char*)ctime(&ltime);
  for (i=0; i<24; i++) tt[i] = t[i];
  tt[24] = '\000';
  return tt;
}

//~----------------------------------------------------------------------------

char *timestamp(int opt) {

  static char b[80];
  char *p = b;
  struct tm *t;
  time_t now = time(NULL);
  
  t = localtime(&now);
  if (opt) {
    p += sprintf(p, "%04d.%02d.%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday);
  }
  p += sprintf(p, "%02d:%02d:%02d ", t->tm_hour, t->tm_min, t->tm_sec);
  
  return b;
}

//~============================================================================
//
// Dallas Semiconductor 1-Wire CRC (DOW CRC) calculation
//

bool DOWCRC(int64 code) {

  int8 CRC;
  int8 g = 0x8C;
  int i, j;
  
  CRC = 0x00;
  
  for (j=0; j<8; j++) {
    int8 d = (code >> (8 * j)) & 0x00000000000000FFLL;
    for (i=0; i<8; i++) {
      if ((CRC ^ d) & 0x01) CRC = (CRC >> 1) ^ g;
      else                  CRC = (CRC >> 1);
      d = d >> 1;
    }
  }

  return CRC ? FALSE : TRUE;  
}

//~----------------------------------------------------------------------------
//
// for debug:
// gcc -o q -DDEBUG_DOWCRC mylib.c
// ./q
//

#ifdef DEBUG_DOWCRC

int main(void) {

  int i;
  bool CRC;
  int64 ID[10] = {
    0xA7000800AA9D6010LL,
    0x1C0008004C26B010LL,
    0x35000800AAACD410LL,
    0x60000800AB040A10LL,
    0xDB0008004C097A10LL,
    0xFE0008004C2C8610LL,
    0xC5000800AA939610LL,
    0x95000800AAB50110LL,
    0xD3000800504E1910LL,
    0x18000800AAB03710LL};

  for (i=0; i<10; i++) {
    CRC = DOWCRC(ID[i]);
    printf("i = %d, ID = %16llX, %s\n", i, ID[i], CRC ? "OK" : "FAIL");
  }
    return 0;
}

#endif

//~----------------------------------------------------------------------------

int16 calculate_CRC16(int16 *dat, int16 len) {

  int i;
  int16 CRC = 0xFFFF;
  int16 g = 0x1021;

  for (i=0; i<len; i++) {
    int j;
    int16 d = dat[i];
    for (j=0; j<16; j++) {
      if ((CRC ^ d) & 0x8000) CRC = (CRC << 1) ^ g;
      else                    CRC = (CRC << 1);
      d <<= 1;
    }
  }
  return CRC;
}

//~----------------------------------------------------------------------------

int16 CRC_CCITT(int8 *p, int32 n) {

  int16 CRC = 0xFFFF;
  int16 g   = 0x1021;
  
  while (n--) {
    int i;
    CRC ^= (int16)((int8)*p++ << 8);
    for (i=0; i<8; i++) CRC = (CRC << 1) ^ ((CRC & 0x8000) ? g : 0);
  }

  return CRC;
}

//~============================================================================

void get_tokens(char *input, char delim, int nn, char **token, int *n) {

  int j;
  char b[160], *c, *p, ch;

  if ((p = index(input, '\n'))) *p = '\0';
  if ((p = index(input, '\r'))) *p = '\0';
  c = b;
  strcpy(c, input);
  if ((p = strstr(c, "//"))) *p = '\0';
    
  j = 0;
  while (1) {
    int i = 0, ii = 0;
    bool first = TRUE;
    int  last = 0;
    while (1) {
      ch = c[i++];
      if (ch == '\0' || ch == delim) break;
      if (!first || (ch != ' ' && ch != '\t')) {
        first = FALSE;
        if (ch != ' ' && ch != '\t') last = ii;
        token[j][ii++] = ch;
      }
    }
    token[j][last+1] = '\0';
    j = j + (ii ? 1 : 0);
    if (ch == '\0') break;
    if (j >= nn) {
      printf("*** Error: Too many tokens.\n");
      break;
    }
    c = c + i;
  }
  *n = j;
}

//~============================================================================

static struct termio current_io;
static struct termio original_io;

//~----------------------------------------------------------------------------

sighandler_t kbhit_ctrl_C_handler(int signum) {

#ifdef VERBOSE_kbhit
  printf("+kbhit_ctrl_C_handler call from pid=%d\n", getpid());
#endif

  exit(0);
}

//~----------------------------------------------------------------------------

void kbhit_exit_handler(void) {

#ifdef VERBOSE_kbhit
  printf("+kbhit_exit_handler\n");
#endif

  fflush(stdin);
  if (ioctl(0, TCSETA, &original_io) == -1) {
    perror("ioctl(): TIOCSETP to reset original io");
  }

#ifdef VERBOSE_kbhit
  printf("-return\n");
#endif
}

//~----------------------------------------------------------------------------

bool kbhit(void) {

  fd_set rfds;
  struct timeval tv;
  static int first = 1;

  if (first) {
    first = 0;
    if (ioctl(0, TCGETA, &original_io) == -1) {
      perror("ioctl(...TCGETA...)");
      exit(1);
    }
    if (atexit(kbhit_exit_handler)) {
      printf("atexit(kbhit_exit_handler) failed.\n");
      exit(1);
    }
    if (signal(SIGINT, (sighandler_t)kbhit_ctrl_C_handler) == SIG_ERR) {
      printf("signal(SIGINT...kbhit_ctrl_C_handler) failed\n");
      exit(1);
    }
    memcpy(&current_io, &original_io, sizeof(struct termio));
//  current_io.c_cc[VMIN]  = 1;     // minimum chars to wait for
    current_io.c_cc[VMIN]  = 0;     // minimum chars to wait for
    current_io.c_cc[VTIME] = 0;     // minimum wait time
    current_io.c_iflag &= ~INLCR;   // do not translate NL to CR
    current_io.c_iflag &= ~ICRNL;   // do not translate CR to NL
    current_io.c_lflag &= ~ICANON;  // unbuffered input
    current_io.c_lflag &= ~ECHO;    // turn off local display

    if (ioctl(0, TCSETA, &current_io) == -1) {
      perror("ioctl(...TCSETA...)");
      exit(1);
    }
  }

  FD_ZERO(&rfds);
  FD_SET(0, &rfds);
  tv.tv_sec  = 0;
//tv.tv_usec = 50;
  tv.tv_usec = 0;                    // changed for Peter
  select(1, &rfds, NULL, NULL, &tv);
  if (FD_ISSET(0, &rfds)) return TRUE;
  else                    return FALSE;
}

//~----------------------------------------------------------------------------

#ifdef DEBUG_kbhit

int main(void) {

  int symbol;

  while (1) {
    if (kbhit()) {
      symbol = fgetc(stdin);
      if (symbol == 0x1B || symbol == 0xE0) symbol = fgetc(stdin)<<8 | symbol;
      printf("You typed ascii %d   hex %X\r\n", symbol, symbol);
      if (symbol == 'q') break;
    }
  }
}

#endif

//~============================================================================

void print_AMS_data_block(char *txt, pAMSBlock block) {

  int i;

  printf("%s [%d] %s %s NA=%03X DT=%06X DC=%d ", 
         txt,
         unpack16(block->Source, 0xFF00),
         block->BlockType & 0x0002 ? "RP" : "RQ",
         block->BlockType & 0x0001 ? "W"  : "R",
         block->NodeAddress,
         block->DataType,
         block->DataCount);

  if (block->Data.p16) {
    for (i=0; i<(block->DataCount+1)/2; i++) {
      if (i == 0) printf("D =");
      printf(" %04X", block->Data.p16[i]);
    }
  }

  if (block->Data.p8) {
    for (i=0; i<block->DataCount; i++) {
      if (i == 0) printf("D =");
      printf(" %02X", block->Data.p8[i]);
    }
  }

  if (block->BlockType & 0x0002) printf(" Err=%04X", block->Error);
  printf("\n");
}

//~============================================================================

void setup_command_path(void) {

  use_MEM     = FALSE;
  use_CAN     = FALSE;
  use_AMSW    = FALSE;

#ifdef USE_CAN
  use_CAN     = TRUE;
#endif

#ifdef USE_AMSW
  use_AMSW    = TRUE;
#endif

  use_PCIAMSW = FALSE;

  use_TCP     = FALSE;
  use_EAS     = FALSE;
}

//~----------------------------------------------------------------------------

void setup_command_path_special(void) {

  use_MEM     = FALSE;
  use_CAN     = FALSE;
  use_AMSW    = FALSE;

#ifdef USE_CAN
  use_CAN     = TRUE;
#endif

#ifdef USE_AMSW
  use_AMSW    = TRUE;
#endif

  use_PCIAMSW = FALSE;

  use_TCP     = FALSE;
  use_EAS     = FALSE;

  if (TRUE) {
    use_CAN       = FALSE;
    use_AMSW      = FALSE;
    strcpy(CS_address, "pcgsc04");
    CS_port       = 10224;    // advised by AE
    use_TCP       = TRUE;
    use_Peter_TCP = FALSE;
    NODE_ADR = USCM_ID;
  }
}

//~============================================================================
