/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2004 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                         No rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                          (281) 334 3800                                   */
/*                      http://www.padsoft.com                               */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  logfile.h   Defines for logfile related variables and functions

#ifndef _LOGFILE_H
#define _LOGFILE_H

//  Revision History
//    Oct 2007 by X.Cai

#include <stdio.h>
#include <string.h>
#include "sspdefs.h"

extern FILE *log_fp;
extern char LogTimeStamp[];
extern int logfile_printf;

typedef void * pLogfile_t;

typedef struct _LogMemBuf
{
  char *buffer;
  int  allocated;
  int  index;
} LogMemBuf, *pLogMemBuf;

void LogMemInit(pLogMemBuf lm);
void LogMem(pLogMemBuf lm, char *fmt, ...);
void LogMemFree(pLogMemBuf lm);
void LogMemClear(pLogMemBuf lm);

int dir_check_and_create(char *name);

int LogfilePath(char *path);
void LogTimeTag(char *msg, int mode);
pLogfile_t LogOpen(char *filename, int argc, char *argv[]);
void LogEntryStart(void);
void LogEntryStop(void);
void LogClose(void);
void LogPrintf(char *fmt, ...);

pLogfile_t LogOpen2(char *path, char *filename);
pLogfile_t LogOpen3(char *path, char *filename, char *instance);
void LogDebug(int level, char *fmt, ...);
void LogNormal(char *fmt, ...);
void LogWarning(char *fmt, ...);
void LogError(char *fmt, ...);
//void LogError2(char *msg, char *fmt, void *ap);
void LogError2(char *msg, char *fmt, va_list ap);
void LogFatal(char *fmt, ...);
void LogDisplay(char *fmt, ...);
void LogHexDumpMem(pLogMemBuf lm, void *Buffer, int32 Bytes, bool ByteSwap);
void LogHexDump(void *Buffer, int32 Bytes, bool ByteSwap);
void LogCharacter(char c);

#endif // _LOGFILE_H
