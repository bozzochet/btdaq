// file mylib.c
//
// A.Lebedev - Jun-2003...
//    Dec 2007 by Peter Dennett
//        Updated for MinGW for WIN32 operation

#include "aallib.h"

#ifdef OS_ECOS
int mykbhit(void) {
  return 0;
}
#endif

#ifdef OS_LINUX

#undef  DEBUG_mykbhit
#undef  VERBOSE_mykbhit

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <termio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "al_lib.h"

typedef void (*sighandler_t)(int);

static struct termio current_io;
static struct termio original_io;


//=============================================================================

void clear_screen(void) {

  printf("\033[2J");
}

//-----------------------------------------------------------------------------

void set_cursor(int row, int col) {

  printf("\033[%hd;%hdH", row, col);
}

//==============================================================================

static sighandler_t mykbhit_ctrl_C_handler(int signum) {

#ifdef VERBOSE_mykbhit
  printf("+mykbhit_ctrl_C_handler call from pid=%d\n", getpid());
#endif

  exit(0);
}

//------------------------------------------------------------------------------

static void mykbhit_exit_handler(void) {

#ifdef VERBOSE_mykbhit
  printf("+mykbhit_exit_handler\n");
#endif

  fflush(stdin);
  if (ioctl(0, TCSETA, &original_io) == -1) {
    perror("ioctl(): TIOCSETP to reset original io");
  }

#ifdef VERBOSE_mykbhit
  printf("-return\n");
#endif
}

//------------------------------------------------------------------------------

int mykbhit(void) {

  fd_set rfds;
  struct timeval tv;
  static int first = 1;
  static int failed = 0;

  if (first) {
    first = 0;
#ifdef VERBOSE_mykbhit
    printf("+mykbhit: First call\n");
#endif
    if (ioctl(STDIN_FILENO, TCGETA, &original_io) == -1) {
      perror("ioctl(...TCGETA...)");
      failed = 1;
      return 0;
    }
    if (atexit(mykbhit_exit_handler)) {
      printf("atexit(mykbhit_exit_handler) failed.\n");
      failed = 1;
      return 0;
    }
    if (signal(SIGINT, (sighandler_t)mykbhit_ctrl_C_handler) == SIG_ERR) {
      printf("signal(SIGINT... failed\n");
      failed = 1;
      return 0;
    }
    memcpy(&current_io, &original_io, sizeof(struct termio));
//  current_io.c_cc[VMIN]  = 1;     // minimum chars to wait for
    current_io.c_cc[VMIN]  = 0;     // minimum chars to wait for
    current_io.c_cc[VTIME] = 0;     // minimum wait time
    current_io.c_lflag &= ~ICANON;  // unbuffered input
    current_io.c_lflag &= ~ECHO;    // turn off local display
    if (ioctl(STDIN_FILENO, TCSETA, &current_io) == -1) {
      perror("ioctl(...TCSETA...)");
      failed = 1;
      return 0;
    }
  }

  if (failed)
  {
    tv.tv_sec  = 0;
    tv.tv_usec = 50;
    select(STDIN_FILENO, NULL, NULL, NULL, &tv);
    return 0;
  }

  FD_ZERO(&rfds);
  FD_SET(STDIN_FILENO, &rfds);
  tv.tv_sec  = 0;
  tv.tv_usec = 50;
  select(1, &rfds, NULL, NULL, &tv);
  if (FD_ISSET(STDIN_FILENO, &rfds)) return 1;
  else                    return 0;
}

//------------------------------------------------------------------------------

#ifdef DEBUG_mykbhit

int main(void) {

  int symbol;

  while (1) {
    if (mykbhit()) {
      symbol = fgetc(stdin);
      if (symbol == 0x1B || symbol == 0xE0) symbol = fgetc(stdin)<<8 | symbol;
      printf("You typed ascii %d   hex %X\r\n", symbol, symbol);
      if (symbol == 'q') break;
    }
  }
}

#endif

//==============================================================================

#endif // OS_LINUX

//=============================================================================

void ShortSleep(int seconds, int usec)
{
#ifdef OS_LINUX
struct timeval tv;

  tv.tv_sec = seconds;
  tv.tv_usec = usec;

  select(0, NULL, NULL, NULL, &tv);
#else
  static int init = 0;
  if ( ! init)
  {
    //??    timeBeginPeriod(1);
    init = 1;
  }
  Sleep((usec+999)/1000);
#endif

  return;
}

#ifdef OS_WIN32
#include <conio.h>

//------------------------------------------------------------------------------

int mykbhit(void) {
  static int init = 0;
  if ( ! init)
  {
    setbuf(stdin, NULL);
    init = 1;
  }

  return kbhit();
} 
#endif // OS_WIN32
