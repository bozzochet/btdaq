#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#define _DEFAULTMODE 0
#define _GCLEARSCREEN 0
#define TRUE  1
#define FALSE 0

struct rccoord {
  short row;
  short col;
};

int            kbhit( void);
int            getch( void);

void           _clearscreen( short area);
short          _setvideomode( short mode);
long           _setbkcolor( long color);
short          _settextcolor( short index);
struct rccoord _gettextposition( void);
struct rccoord _settextposition( short row, short col);
void           _outtext( const char *text);
