// file mylib.h
//
// see file mylib.c
//
// A.Lebedev - Jun-2003...

//-----------------------------------------------------------------------------

#define OK 1
#define KO 0

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#ifndef MOD
# define MOD(x,y) (((x)+(y)) % (y))
#endif

//-----------------------------------------------------------------------------

//??typedef void (*sighandler_t)(int);

//-----------------------------------------------------------------------------

void clear_screen(void);
void set_cursor(int row, int col);

int mykbhit(void);

void ShortSleep(int seconds, int usec);

//=============================================================================
