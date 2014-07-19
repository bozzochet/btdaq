// file mylib.h
//
// see file mylib.c
//
// A.Lebedev - Jun-2003...

//-----------------------------------------------------------------------------

#ifndef _MYLIB_H
#define _MYLIB_H

//-----------------------------------------------------------------------------

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stdarg.h>
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

//-----------------------------------------------------------------------------

#define TRUE  1
#define FALSE 0
#define OK 1
#define KO 0

#define REQUEST 0
#define REPLY   1
#define READ    0
#define WRITE   1

#define INITIALIZE 1
#define START      2
#define STOP       3
#define CONTINUE   4
#define DONE       5
#define RXING      6
#define TXING      7
#define WRITING    8
#define READING    9
#define EXPIRED   10

#define ABS(a) (( (a) < 0 ) ? -(a) : (a))

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define MOD(x,y) (((x)+(y)) % (y))

#define QUIT(msg)  {perror(msg); exit(1);}
#define PANIC(msg) {printf(msg); exit(1);}

//-----------------------------------------------------------------------------

typedef unsigned char        bool;

typedef unsigned char        int8;
typedef unsigned short       int16;

#if (UINT_MAX >> 16) !=0
  typedef unsigned int       int32;
#else
#if (ULONG_MAX >> 16) != 0
  typedef unsigned long      int32;
#endif
#endif

#if (ULONG_MAX >> 32) !=0
  typedef unsigned long      int64;
#else
  typedef unsigned long long int64;
#endif

typedef volatile int8  vint8;
typedef volatile int16 vint16;
typedef volatile int32 vint32;
typedef volatile int64 vint64;

typedef void (*sighandler_t)(int);

//-----------------------------------------------------------------------------

typedef struct _AMSBlock {
  int32 DataType;         // DataType for the block 
  int32 DataCount;        // Count of 8 bit bytes
  int16 NodeAddress;      // The node address for the block
  int16 Error;            // AMSBlock handling status per ABE_ above
  int16 Source;           // DAQDSubmit puts: (MDCNo << 6 | TaskHandle)
  int16 BlockType;        // BlockType BT_ bits above
  int32 BufferSize;       // Number of bytes allocated for Data.p8

  union                   // Pointer to our data area.  Nominally this
  {                       // Points into the same malloc buffer to 
    int8  *p8;            // DataStart (next in this structure).  This allows
    int16 *p16;           // us to release this memory with a single
    int32 *p32;           // call.  In practice this could point to any
  } Data;                 // memory.

  int16 DataStart[1];     // First 16 bit word of the block
                          // Data pointed from an AMSBlock should be in
                          // big endian order.  Data is 16-bit byte swapped
                          // on little endian systems when it is moved into
                          // a local block.
} AMSBlock, *pAMSBlock;

//-----------------------------------------------------------------------------

int sprintf_binary(char *b, int32 number, int mode);

void  pack16(int16 *code, int16 mask, int16 number);
int16 unpack16(int16 code, int16 mask);
void  pack32(int32 *code, int32 mask, int16 number);
int32 unpack32(int32 code, int32 mask);

int16 unpack10(int32 n, int p);
void pack10(int32 *n, int p, int d);

int16 invert_bit_order(int16 code);
int32 swapbits(int32 word, int n);

float delta_t(struct timeval *t2, struct timeval *t1);
bool timer(int16 chan, int func, ...);

char *mytime(void);

sighandler_t kbhit_ctrl_C_handler(int signum);
void kbhit_exit_handler(void);

bool kbhit(void);

void print_AMS_data_block(char *txt, pAMSBlock block, int radix);

//=============================================================================

#endif // _MYLIB_H
