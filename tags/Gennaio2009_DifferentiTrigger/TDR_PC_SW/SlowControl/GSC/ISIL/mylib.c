// file mylib.c
//
// A.Lebedev - Jun-2003...

#undef DEBUG_mytypes
#undef DEBUG_kbhit
#undef VERBOSE_kbhit

#include "mylib.h"

//=============================================================================

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

//=============================================================================

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

//=============================================================================

void pack16(int16 *code, int16 mask, int16 number) {

  int16 LSB = mask & (mask ^ mask << 1);

  *code = (*code & ~mask) | (number * LSB);
}

//-----------------------------------------------------------------------------

int16 unpack16(int16 code, int16 mask) {

  int16 LSB = mask & (mask ^ mask << 1);

  if (LSB) return (code & mask) / LSB;
  else     return 0;
}

//-----------------------------------------------------------------------------

void pack32(int32 *code, int32 mask, int16 number) {

  int32 LSB = mask & (mask ^ mask << 1);

  *code = (*code & ~mask) | (number * LSB);
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

int16 invert_bit_order(int16 code) {

  int16 new_code = 0;
  int i;

  for (i=0; i<16; i++) {
    new_code <<= 1;
    if (code & 0x0001 << i) new_code |= 0x0001;
  }

  return new_code;
}

//-----------------------------------------------------------------------------
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

//==============================================================================

float delta_t(struct timeval *t2, struct timeval *t1) {

  return ((t2->tv_sec - t1->tv_sec) + 0.000001 * (t2->tv_usec - t1->tv_usec));
}

//------------------------------------------------------------------------------

#define NCHA 10

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

//==============================================================================

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

//==============================================================================

static struct termio current_io;
static struct termio original_io;

//------------------------------------------------------------------------------

sighandler_t kbhit_ctrl_C_handler(int signum) {

#ifdef VERBOSE_kbhit
  printf("+kbhit_ctrl_C_handler call from pid=%d\n", getpid());
#endif

  exit(0);
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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

//==============================================================================

void print_AMS_data_block(char *txt, pAMSBlock block, int radix) {

  int i;

  printf("%s %s %s NA=%03X DT=%06X DC=%d ", 
         txt,
         block->BlockType & 0x0002 ? "RP" : "RQ",
         block->BlockType & 0x0001 ? "W"  : "R",
         block->NodeAddress,
         block->DataType,
         block->DataCount);

  if (radix == 16) {
    for (i=0; i<(block->DataCount+1)/2; i++) {
      if (i == 0) printf("D =");
      printf(" %04X", block->Data.p16[i]);
    }
  }

  if (radix == 8) {
    for (i=0; i<block->DataCount; i++) {
      if (i == 0) printf("D =");
      printf(" %02X", block->Data.p8[i]);
    }
  }

  if (block->BlockType & 0x0002) printf(" Err=%04X", block->Error);
  printf("\n");
}

//==============================================================================
