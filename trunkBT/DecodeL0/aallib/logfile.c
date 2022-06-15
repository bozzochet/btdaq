/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2004 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                        All rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                          (281) 334 3800                                   */
/*                      http://www.padsoft.com                               */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  logfile.c   Provides a standardize log file interface

//  Revision History
//    Jul 2007 by Xudong Cai
//      - Initial version
//    Aug 2008 by Peter Dennett
//      - Adapted concepts for use in AALpro
//    Oct 2008 by Xudong Cai
//      - Added directory check
//    Oct 2010 by Peter Dennett at SSPF
//      - Changed to J-day usage and 24-hour files
//    Apr 2013 by Peter Dennett at MCC POCC
//      - Use VA_COPY for vfprintf writing to file as prior use of
//        VA_LIST has consumed it.

#include "sspdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <malloc.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

#include "logfile.h"

FILE *log_fp = NULL;
int logfile_printf = 1;   // If set non-zero logfiles entries to file are duplicated on printf
static int LogLevel = 0;
static char *log_base = NULL;
static struct tm time_then;
static pthread_mutex_t LogFileMutex = PTHREAD_MUTEX_INITIALIZER;

char LogTimeStamp[132];

//----------------------------------------------------------
//  Function: LogfilePath
//      Check environment variable AALLOG to get path for logfile
//
//  Inputs:
//      path ---- result path string
//
//  Return:
//      = 1 : AALLOG is defined
//      = 0 : AALLOG is not defined
//----------------------------------------------------------
int LogfilePath(char *path) {
    char *ptr = getenv("AALLOG");

    if ( ptr )  strcpy(path, ptr);
    else        strcpy(path, "/Data/log");

    return (ptr ? 1 : 0);
}

//----------------------------------------------------------
//  Function: LogTimeTag
//      Add current time string for logfile message
//
//  Inputs:
//      msg ----- message string for time string to be appended
//      mode ---- time string mode (Y:Year, m:month, d:day, H:hour, M:minute, S:second)
//                0 - _YYYYmmdd_HHMMSS (default)
//                1 - _YYYYmmdd_HHMM
//                2 - _YYYYmmdd_HH
//                3 - _YYYYmmdd
//
//  Return:
//      None
//----------------------------------------------------------
void LogTimeTag(char *msg, int mode) {
    time_t tt;
    char str_time[40];

    time(&tt);
    switch (mode) {
      case 1:
        strftime(str_time, 40, "_%Y%m%d_%H%M", localtime(&tt));
        break;
      case 2:
        strftime(str_time, 40, "_%Y%m%d_%H", localtime(&tt));
        break;
      case 3:
        strftime(str_time, 40, "_%Y%m%d", localtime(&tt));
        break;
      case 0:
      default:
        strftime(str_time, 40, "_%Y%m%d_%H%M%S", localtime(&tt));
        break;
  }
	strcat(msg, str_time);
}

//~============================================================================

static pLogfile_t LogOpenBase(void)
{
char *fn;
#ifndef OS_WIN32
char *ln=NULL;
#endif
char ts[40];
time_t tt;
FILE *new_log_fp = NULL;

  time(&tt);
  time_then = *localtime(&tt);
  //strftime(ts, sizeof ts, "%Y%m%d_%H.log", &time_then);
  strftime(ts, sizeof ts, "%Y_%j.log", &time_then);

  fn = malloc(strlen(log_base) + strlen(ts) + 10);
  sprintf(fn, "%s_%s", log_base, ts);

  new_log_fp = fopen(fn, "a");
  if ( ! new_log_fp) 
  {
    printf("LogOpenBase: Failed to open log: %s (%s)\n", fn, strerror(errno));
    if (log_fp)
      fprintf(log_fp, "LogOpenBase: Failed to open log: '%s' (%s) log_base '%s'\n",
             fn, strerror(errno), log_base);
    free(fn);
    return 0;
  }

  if ( log_fp ) 
  {
    fprintf(log_fp, "LogOpenBase: Log rolled over to '%s'\n", fn);
    fprintf(log_fp, "\n---------------------------------------------------------------------\n");
    fclose(log_fp);
  }

  log_fp = new_log_fp;

  //  Create a link that we can tail -F /Data/log/<base>_current.log

#ifndef OS_WIN32
  ln = malloc(strlen(log_base) + 50);
  sprintf(ln, "%s_current.log", log_base);
  unlink(ln);
  IGNORE_RESULT(symlink(fn, ln));
  free(ln);
#endif

  fprintf(log_fp, "\n---------------------------------------------------------------------\n");
  fflush(log_fp);

  free(fn);
  return log_fp;
}

//----------------------------------------------------------
//  Function: LogOpen3
//      Open logfile (Dennett added)
//
//  Inputs:
//      path -------- Logfile path
//      filename ---- Logfile name
//
//  Return:
//      <> 0 : Success
//       = 0 : Failed
//----------------------------------------------------------
pLogfile_t LogOpen3(char *path, char *filename, char *instance)
{
  char *ptr = getenv("AALLOG");
  int ret;
  char *base, *spath;

  // Get path and check it exists

  spath = (ptr ? ptr : (path ? path : "/Data/log"));

  if ( (ret = dir_check_and_create(spath)) < 0 ) 
  {
    printf("LogOpen3: Failed on directory %s, Error=%s\n", spath, strerror(-ret));
    log_fp = NULL;
    return NULL;
  }

  // Build the base path (without date/time)

  ret = filename?strlen(filename):0;
  ret += instance?strlen(instance):0;

  base = malloc(strlen(spath) + ret + 25);

  sprintf(base, "%s/%s%s%s", spath, (filename ? filename : "logfile"),
          (instance?"_":""), (instance?instance:""));

  log_base = base;

  /* Open file */

  if ( ! LogOpenBase())
    return NULL;

#ifdef MEMWATCH
  mwSetOutFunc(LogCharacter);
#endif

  return log_fp;
}

//----------------------------------------------------------
//  Function: LogOpen2
//      Open logfile (Dennett added)
//
//  Inputs:
//      path -------- Logfile path
//      filename ---- Logfile name
//
//  Return:
//      <> 0 : Success
//       = 0 : Failed
//----------------------------------------------------------
pLogfile_t LogOpen2(char *path, char *filename)
{
  char *ptr = getenv("AALLOG");
  int ret;
  char *base, *spath;

  // Get path and check it exists

  spath = (ptr ? ptr : (path ? path : "/Data/log"));

  if ( (ret = dir_check_and_create(spath)) < 0 ) 
  {
    printf("LogOpen2: Failed on directory %s, Error=%s\n", spath, strerror(-ret));
    log_fp = NULL;
    return NULL;
  }

  // Build the base path (without date/time)

  base = malloc(strlen(spath) + strlen(filename) + 2);

  sprintf(base, "%s/%s", spath, (filename ? filename : "logfile"));

  log_base = base;

  /* Open file */

  if ( ! LogOpenBase())
    return NULL;

#ifdef MEMWATCH
  mwSetOutFunc(LogCharacter);
#endif

  return log_fp;
}

//----------------------------------------------------------
//  Function: LogOpen
//      Open logfile and print command line
//
//  Inputs:
//      filename ---- Logfile name
//      argc -------- Number of command line arguments
//      argv -------- Command line argument strings
//
//  Return:
//      <> 0 : Success
//       = 0 : Failed
//----------------------------------------------------------
pLogfile_t LogOpen(char *filename, int argc, char *argv[]) {
char *p;
char name[100];

  strcpy(name, filename);
  p = strrchr(name, '/');
  p[0] = '\0';    // remove file name and keep the directory

  LogOpen2(name, p+1);

  return log_fp;
}

//----------------------------------------------------------
//  Function: LogClose
//      Close logfile
//
//  Inputs:
//      None
//
//  Return:
//      None
//----------------------------------------------------------
void LogClose(void) {
    if ( log_fp ) 
      fclose(log_fp);
    log_fp = NULL;

    if (log_base)
      free(log_base);
    log_base = NULL;
}

//----------------------------------------------------------
//  Function: LogEntryStart
//      Lock log file
//
//  Inputs:
//      None
//
//  Return:
//      None
//----------------------------------------------------------
void LogEntryStart(void) {
time_t tt;

  if ( ! log_fp ) 
    return;

  time(&tt);

  pthread_mutex_lock(&LogFileMutex);

  //strftime(LogTimeStamp, 40, "%Y/%m/%d %H:%M:%S", localtime(&tt));
  strftime(LogTimeStamp, 40, "%Y-%j.%H:%M:%S", localtime(&tt));

  //if (localtime(&tt)->tm_hour != time_then.tm_hour)
  if (localtime(&tt)->tm_yday != time_then.tm_yday)
  {
    //??LogClose();
    LogOpenBase();
  }
}

//----------------------------------------------------------
//  Function: LogEntryStop
//      Unlock log file
//
//  Inputs:
//      None
//
//  Return:
//      None
//----------------------------------------------------------
void LogEntryStop(void) {
  pthread_mutex_unlock(&LogFileMutex);
}

//----------------------------------------------------------
//  Function: LogPrintf
//      Print log message to both logfile and screen as printf
//
//  Inputs:
//      Format and arguments
//
//  Return:
//      None
//----------------------------------------------------------
void LogPrintf(char *fmt, ...) {
va_list ap, ap2;

  va_start(ap, fmt);
  va_copy(ap2, ap);
  LogEntryStart();

 /* print message */

  if (( ! log_fp) || logfile_printf) 
    vprintf(fmt, ap);

  if ( log_fp ) {
    vfprintf(log_fp, fmt, ap2);
    fflush(log_fp);
  }

  LogEntryStop();
  va_end(ap);
  va_end(ap2);
}

//----------------------------------------------------------
//  Used to send debug output to the display and optionally to log file

void LogDebug(int level, char *fmt, ...)
{
va_list ap, ap2;

  va_start(ap, fmt);
  va_copy(ap2, ap);
  LogEntryStart();

  // print message
  if (( ! log_fp) || logfile_printf) 
    vprintf(fmt, ap);

  if (log_fp && (LogLevel <= level)) 
  {
    fprintf(log_fp, "%s: [D] ", LogTimeStamp);
    vfprintf(log_fp, fmt, ap2);
    fflush(log_fp);
  }

  LogEntryStop();
  va_end(ap);
  va_end(ap2);
}

//----------------------------------------------------------
// Used to log normal events

void LogNormal(char *fmt, ...)
{
va_list ap, ap2;

  va_start(ap, fmt);
  va_copy(ap2, ap);
  LogEntryStart();

  // print message
  if (( ! log_fp) || logfile_printf) 
    vprintf(fmt, ap);

  if (log_fp) 
  {
    fprintf(log_fp, "%s: [ ] ", LogTimeStamp);
    vfprintf(log_fp, fmt, ap2);
    fflush(log_fp);
  }

  LogEntryStop();
  va_end(ap);
  va_end(ap2);
}

//----------------------------------------------------------
// Used to log warning events

void LogWarning(char *fmt, ...)
{
va_list ap, ap2;

  va_start(ap, fmt);
  va_copy(ap2, ap);
  LogEntryStart();

  // print message
  if (( ! log_fp) || logfile_printf) 
    vprintf(fmt, ap);

  if (log_fp) 
  {
    fprintf(log_fp, "%s: [W] ", LogTimeStamp);
    vfprintf(log_fp, fmt, ap2);
    fflush(log_fp);
  }

  LogEntryStop();
  va_end(ap);
  va_end(ap2);
}

//--------------------------------------------------------
// Used to log error events

void LogError(char *fmt, ...)
{
va_list ap, ap2;

  va_start(ap, fmt);
  va_copy(ap2, ap);
  LogEntryStart();

  // print message
  if (( ! log_fp) || logfile_printf) 
    vprintf(fmt, ap);

  if (log_fp) 
  {
    fprintf(log_fp, "%s: [E] ", LogTimeStamp);
    vfprintf(log_fp, fmt, ap2);
    fflush(log_fp);
  }

  LogEntryStop();
  va_end(ap);
  va_end(ap2);
}

//--------------------------------------------------------
// Used to log error events

void LogError2(char *msg, char *fmt, va_list ap)
{
va_list ap2;

  va_copy(ap2, ap);

  LogEntryStart();

  // print message
  if (( ! log_fp) || logfile_printf) 
  {
    printf("%s ", msg);
    vprintf(fmt, ap);
  }

  if (log_fp) 
  {
    fprintf(log_fp, "%s: [E] ", LogTimeStamp);
    fprintf(log_fp, "%s ", msg);
    vfprintf(log_fp, fmt, ap2);
    fflush(log_fp);
  }

  va_end(ap2);
  LogEntryStop();
}

//----------------------------------------------------------
// Used to log fatal events

void LogFatal(char *fmt, ...)
{
va_list ap, ap2;

  va_start(ap, fmt);
  va_copy(ap2, ap);
  LogEntryStart();

  // print message
  if (( ! log_fp) || logfile_printf) 
    vprintf(fmt, ap);

  if (log_fp) 
  {
    fprintf(log_fp, "%s: [F] ", LogTimeStamp);
    vfprintf(log_fp, fmt, ap2);
    fflush(log_fp);
  }

  LogEntryStop();
  va_end(ap);
  va_end(ap2);
}

//----------------------------------------------------------
//  Used to just display the entry to the standard output console (not to file)

void LogDisplay(char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  if (( ! log_fp) || logfile_printf) 
    vprintf(fmt, ap);
  va_end(ap);
}

//----------------------------------------------------------
// Used to log hex buffer dumps

void LogHexDump(void *Buffer, int32 Bytes, bool ByteSwap)
{
int i;
int16 *wp = (int16 *) Buffer;
int8 *cp = (int8 *) Buffer;
#define BPL (16*2)   // 2 bytes/word

  LogEntryStart();

  for (i=0; i<Bytes; i+=2)
  {
    if ((i % BPL) == 0)
    {
      if (( ! log_fp) || logfile_printf) 
        printf("%5d: ", i);
      if (log_fp) fprintf(log_fp, "%5d: ", i);
    }

    if (ByteSwap)
    {
      if (( ! log_fp) || logfile_printf) 
        printf("%02X",  *cp); 
      if (log_fp) fprintf(log_fp, "%02X",  *cp); 
      cp++;

      if (( ! log_fp) || logfile_printf) 
        printf("%02X ",  *cp); 
      if (log_fp) fprintf(log_fp, "%02X ",  *cp); 
      cp++;
    }
    else
    {
      if (( ! log_fp) || logfile_printf) 
        printf("%04X ", *wp); 
      if (log_fp) fprintf(log_fp, "%04X ", *wp);
      wp++;
    }

    if ((i % BPL) == (BPL-2)) 
    {
      if (( ! log_fp) || logfile_printf) 
        printf("\n");
      if (log_fp) fprintf(log_fp, "\n");
    }
  }

  if ((i % BPL) != 0) 
  {
    if (( ! log_fp) || logfile_printf) 
      printf("\n");
    if (log_fp) fprintf(log_fp, "\n");
  }

  LogEntryStop();
  return;
}

//----------------------------------------------------------
// Used to log hex buffer dumps

void LogHexDumpMem(pLogMemBuf lm, void *Buffer, int32 Bytes, bool ByteSwap)
{
int i;
int16 *wp = (int16 *) Buffer;
int8 *cp = (int8 *) Buffer;
#define BPL (16*2)   // 2 bytes/word

  for (i=0; i<Bytes; i+=2)
  {
    if ((i % BPL) == 0)
      LogMem(lm, "%5d: ", i);

    if (ByteSwap)
    {
      LogMem(lm, "%02X",  *cp); 
      cp++;

      LogMem(lm, "%02X ",  *cp); 
      cp++;
    }
    else
    {
      LogMem(lm, "%04X ", *wp);
      wp++;
    }

    if ((i % BPL) == (BPL-2)) 
    {
      LogMem(lm, "\n");
    }
  }

  if ((i % BPL) != 0) 
  {
    LogMem(lm, "\n");
  }

  return;
}

//----------------------------------------------------------
//  Used to just display the entry to the standard output console (not to file)

void LogCharacter(char c)
{
static char buffer[256];
static int  index = 0;

  if (index >= ((sizeof buffer)-1))
  {
    buffer[(index>=(sizeof buffer))?(sizeof buffer)-1:index] = 0;
    LogNormal("[MW] %s\n", buffer);
    index = 0;
  }
    
  buffer[index++] = c;  

  if (c == '\n')
  {
    buffer[(index>=(sizeof buffer))?(sizeof buffer)-1:index] = 0;
    LogNormal("[MW] %s", buffer);
    index = 0;
  }
}

//----------------------------------------------------------
//  Initialize LogMemBuf structure

void LogMemInit(pLogMemBuf lm)
{
  memset((char *)lm, 0, sizeof *lm);
}

//----------------------------------------------------------
//  Particial initialize

void LogMemClear(pLogMemBuf lm)
{
  lm->index = 0;
  if (lm->buffer)
    *(lm->buffer) = 0;
}

//----------------------------------------------------------
//  Initialize LogMemBuf structure

void LogMem(pLogMemBuf lm, char *fmt, ...)
{
va_list ap;

  if ((!lm->buffer) || ((lm->allocated - lm->index) < 512))
  {
    lm->allocated += 512;
    lm->buffer = realloc(lm->buffer, lm->allocated);
  }

  va_start(ap, fmt);
  lm->index += vsprintf(lm->buffer+lm->index, fmt, ap);
  va_end(ap);
}

//----------------------------------------------------------
//  Initialize LogMemBuf structure

void LogMemFree(pLogMemBuf lm)
{
  if (lm->buffer)
    free(lm->buffer);
  memset((char *)lm, 0, sizeof *lm);
}
