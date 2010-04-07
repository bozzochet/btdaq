// file mylib.h
//
// see file mylib.c
//
// A.Lebedev - Jun-2003...

//~----------------------------------------------------------------------------

#ifndef _MYLIB_H
#define _MYLIB_H

//~----------------------------------------------------------------------------

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <grp.h>
#include <limits.h>
#include <math.h>
#include <netdb.h>
#include <pthread.h>
#include <pwd.h>
#include <resolv.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/io.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <termio.h>
#include <time.h>
#include <unistd.h>

//~----------------------------------------------------------------------------

#define TRUE  1
#define FALSE 0
#define OK 1
#define KO 0

#define REQUEST 0
#define REPLY   1
#define READ    0
#define WRITE   1

#define BEGIN      1
#define END        2

#define INITIALIZE 1
#define START      2
#define GO         3
#define STOP       4
#define CONTINUE   5
#define DONE       6
#define RXING      7
#define TXING      8
#define WRITING    9
#define READING   10
#define EXPIRED   11

#define ABS(a) (( (a) < 0 ) ? -(a) : (a))

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define MOD(x,y) (((x)+(y)) % (y))

#define DIM(a) (sizeof((a))/sizeof((a)[0]))

#define O(member) offsetof(_cc, member)

#define QUIT(msg)  {perror(msg); exit(1);}
#define PANIC(msg) {printf(msg); exit(1);}
#define EXIT(msg)  {printf("*** %s: err=0x%04X\n", msg, err); exit(1);}

//~----------------------------------------------------------------------------

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

typedef long double real;

#define far

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

//~---

#ifndef _GNU_SOURCE
  typedef void (*sighandler_t)(int);
#endif

//~----------------------------------------------------------------------------

int P;                               // global variable

char *title;                         // global variable
char *date;                          // global variable
char *progname;                      // global variable

char adr_config_file_name[80];       // global variable
char dat_config_file_name[80];       // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool use_MEM;               // global variable
bool use_CAN;               // global variable
bool use_AMSW;              // global variable
bool use_TCP;               // global variable
bool use_EAS;               // global variable

int16 LPT_port;             // global variable
int16 LPT_data;             // global variable
int16 LPT_stat;             // global variable
int16 LPT_ctrl;             // global variable
int16 EPP_addr;             // global variable
int16 EPP_data;             // global variable
int16 ECR_ctrl;             // global variable

int8 *uscm_mem;             // global variable
int JMDC_ID;                // global variable
int USCM_ID;                // global variable
int BUS_FOR_REQUEST;        // global variable
int BUS_FOR_REPLY;          // global variable

bool use_PCIAMSW;           // global variable
int TX;                     // global variable
int RX;                     // global variable
int16 JINF_PATH;            // global variable
int16 AMSW_PATH[5];         // global variable

bool use_Peter_TCP;         // global variable
int16 use_APID;             // global variable
int16 NODE_ADR;             // global variable
char CS_address[100];       // global variable
int  CS_port;               // global variable

int serial_RX_buffer_size;  // global variable

int32 DO_NOT_CREATE;        // global variable

//~----------------------------------------------------------------------------

typedef struct _abiAttachment {
  int8 Source;    // AMSBlock source (MDC and TaskHandle)
  int8 Error;     // ABE_ error code
  int8 Length;    // Number of 16-bit words for attachment data
  int8 Type;      // Attachment type
  int16 Data[1];  // Abstract data
} abiAttachment, *pabiAttachment;

typedef struct _AMSBlock {
  int32 Status;           // This AMSBlock's status (abiStatusXXXXXXX above)
  int16 CommandTag;       // Secondary Block Header 1st 16-bit word
  int32 DataTime;         // Secondary Block Header 2nd & 3rd 16-bit words
  int32 Length;           // Block Length in bytes
  int16 BlockType;        // BlockType BT_ bits above
  int16 RequestReply;     //
  int16 ReadWrite;        //
  int16 NodeAddress;      // The node address for the block
  int32 DataType;         // DataType for the block 
  int32 DataCount;        // Count of 8 bit bytes
  int32 BufferSize;       // Number of bytes allocated for Data.p8
  struct {
    int8  *p8;            // Pointer to data area
    int16 *p16;           // Pointer to data area
  } Data;
  int16 Source;           // DAQDSubmit puts: (MDCNo << 8 | TaskHandle)
  int16 Error;            // AMSBlock handling status
  pabiAttachment Attachment; // Our attachment (see abiStatusFreeAttachment)
  void *Contents;         // Pointer to our data area.  Nominally this
                          // Points into the same malloc buffer to 
                          // DataStart (next in this structure).  This allows
                          // us to release this memory with a single
                          // call.  In practice this could point to any
                          // memory (see abiStatusFreeData).
  int16 DataStart[1];     // First 16 bit word of the block
                          // Data pointed from an AMSBlock should be in
                          // big endian order.  Data is 16-bit byte swapped
                          // on little endian systems when it is moved into
                          // a local block.
} AMSBlock, *pAMSBlock;

//~----------------------------------------------------------------------------

int sprintf_binary(char *b, int32 number, int mode);
int sprintf_huge(char *b, int64 integer, int d1, int64 fraction, int d2);

int32 packbytes(int32 *d, int8 *s, int n);
void unpackbytes(int8 *d, int32 s, int n);

int16 pack16(int16 *code, int16 mask, int16 number);
int16 unpack16(int16 code, int16 mask);
int32 pack32(int32 *code, int32 mask, int32 number);
int32 unpack32(int32 code, int32 mask);

int16 unpack10(int32 n, int p);
void pack10(int32 *n, int p, int d);

int16 invert_bit_order(int16 code);
int32 swapbits(int32 word, int n);

void swap16(int16 *dat, int n);

float delta_t(struct timeval *t2, struct timeval *t1);
void delay(float value);
bool timer(int16 chan, int func, ...);
void ShortSleep(int sec, int usec);

char *mytime(void);
char *timestamp(int opt);

bool DOWCRC(int64 ID);
int16 calculate_CRC16(int16 *dat, int16 len);
int16 CRC_CCITT(int8 *p, int32 n);

void get_tokens(char *input, char delim, int nn, char **token, int *n);

sighandler_t kbhit_ctrl_C_handler(int signum);
void kbhit_exit_handler(void);

bool kbhit(void);

void print_AMS_data_block(char *txt, pAMSBlock block);

void setup_command_path(void);
void setup_command_path_special(void);

//~============================================================================

#endif // _MYLIB_H
