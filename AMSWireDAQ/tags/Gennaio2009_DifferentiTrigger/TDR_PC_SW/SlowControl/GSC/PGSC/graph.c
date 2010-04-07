#include "graph.h"

static struct rccoord pos;

static int text_color[8]       = {30, 34, 32, 36, 31, 35, 33, 37};
static int background_color[8] = {40, 44, 42, 46, 41, 45, 43, 47};

#define N 256
static char input_buffer[N];
static int  indx;

FILE *screen;

//-----------------------------------------------------------------------------

void exit_handler( void) {
  fprintf( screen, "\033[0m\n\n");
  fprintf( screen, "\033]1;dtterm\007");
  fprintf( screen, "\033]2;dtterm\007");
}

//-----------------------------------------------------------------------------

void ctrl_C_handler( void) {
  exit( 0);
}

//-----------------------------------------------------------------------------

int kbhit( void) {

  static int first = TRUE;
  static int filedes;
  struct termios termios_p;
  char byte;
  int fd = fileno( stdin);
  struct timeval tp ={ 0L, 10000L};	// 10msec interval
  struct fd_set fds;

  if (first) {
    first = FALSE;
    if ((filedes = open("/dev/tty",O_RDONLY | O_NONBLOCK | O_NDELAY)) == -1) {
      printf( "open : %s\n", strerror(errno));
      exit(0);
    }
    if (tcgetattr( filedes, &termios_p)) {
      printf( "tcgetattr : %s\n", strerror(errno));
      exit(0);
    }
    termios_p.c_lflag    &= ~ICANON;
    termios_p.c_lflag    &= ~ECHO;
    termios_p.c_cc[VMIN]  = 0;
    termios_p.c_cc[VTIME] = 0;
    if (tcsetattr( filedes,  TCSANOW, &termios_p)) {
      printf( "tcsetattr : %s\n", strerror(errno));
      exit(0);
    }
    indx = 0;
  }

  FD_ZERO( &fds);
  FD_SET( fd, &fds);
//select( fd+1, &fds, 0, 0, &tp);	// Mike's line
  select( 2, &fds, NULL, NULL, &tp);

  if (FD_ISSET( fd, &fds)) { 
    if (read( filedes, &byte, 1) != -1) {
      if (indx >= N) exit( 0);
      input_buffer[indx++] = byte;
      return 1;
    }
  }
  return 0;

}

//-----------------------------------------------------------------------------
int getch( void) {
  if (indx > 0) return input_buffer[--indx]; 
  else          return -1;
}

//-----------------------------------------------------------------------------

void _clearscreen( short area) {
  fprintf( screen, "\033[2J");
}

//-----------------------------------------------------------------------------

short _setvideomode( short mode) {
  fclose( screen);
  if ((screen = fopen( "/dev/tty", "w")) == NULL) {
    printf( "open : %s\n", strerror(errno));
    exit(0);
  }
  setbuf( stdout, NULL);
  setbuf( screen, NULL);
  atexit( exit_handler);
  signal( SIGINT, ctrl_C_handler);
  fprintf( screen, "\033[8;26;80t");		// set window 80x26 chars
  fprintf( screen, "\033]1;STS-91\007");
  fprintf( screen, "\033]2;STS-91 AMS 1553 Bus Monitor Emulator\007");
}

//-----------------------------------------------------------------------------

struct rccoord _gettextposition( void) {
  return pos;
}

//-----------------------------------------------------------------------------

long _setbkcolor( long color) {
  if (color > 7) color = color - 8;
  fprintf( screen, "\033[%dm", background_color[color]);
  return color;
}

//-----------------------------------------------------------------------------

short _settextcolor( short indx) {
  if (indx > 7) indx = indx - 8;
  fprintf( screen, "\033[%dm", text_color[indx]);
  return indx;
}

//-----------------------------------------------------------------------------

struct rccoord _settextposition( short row, short col) {
  pos.row = row;
  pos.col = col;
  fprintf( screen, "\033[%hd;%hdH", row, col);
  return pos;
}

//-----------------------------------------------------------------------------

void _outtext( const char *text) {
  fprintf( screen, "%s",text);
  pos.col += strlen( text);
}

//-----------------------------------------------------------------------------

