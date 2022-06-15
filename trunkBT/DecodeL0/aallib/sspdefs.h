/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                     Copyright 2007 by PADSOFT, Inc.                       */
/*                                                                           */
/*        Developed for use on International Space Station Payloads          */
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

//  sspdefs.h   Common defintions for Space Station support software

//  Revision History
//    Apr 2007 by Peter Dennett
//      - Initial version - MCO -> IAH 39000ft FC
//    Dec 2007 by Peter Dennett
//      - MinGW mods

#ifndef _SSPDEFS_H
#define _SSPDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
# ifdef __MINGW32__
#   define OS_WIN32_MINGW
# endif
# undef __WIN32__
# define __WIN32__ 1
#  ifndef DECLSPEC_EXPORT
#    define DECLSPEC_EXPORT __declspec(dllexport)
#  endif // DECLSPEC_EXPORT
#else
#  define APIENTRY
#  define EXTERN_C
#  define DECLSPEC_EXPORT
#endif //  __WIN32__

#ifdef __WIN32__
# ifndef OS_WIN32
#   define OS_WIN32
# endif
#else
# ifndef OS_LINUX
#   define OS_LINUX
# endif
#endif

#define UNUSED __attribute__ ((__unused__))
#define IGNORE_RESULT(x) ({ typeof(x) z = x; (void)sizeof z; })

#define _USE_STDINT_H 1
#ifdef _USE_STDINT_H
#include <stdint.h>
typedef uint8_t    int8;
#ifndef __cplusplus
typedef uint8_t    bool;
#endif
typedef uint16_t   int16;
typedef uint32_t   int32;

typedef volatile uint8_t  vint8;
typedef volatile uint16_t vint16;
typedef volatile uint32_t vint32;
#else
typedef unsigned char      int8;
#ifndef __cplusplus
typedef unsigned char      bool;
#endif
typedef unsigned short     int16;
typedef unsigned long      int32;

typedef volatile unsigned char  vint8;
typedef volatile unsigned short vint16;
typedef volatile unsigned long  vint32;
#endif

#ifndef TRUE
#  define TRUE  (1)
#endif

#ifndef FALSE
#  define FALSE (0)
#endif

#ifndef NULL
#  define NULL  ((void *) 0)
#endif

#ifdef OS_LINUX
# include <stdio.h>
# include <string.h>
# include <stdarg.h>
# include <stdlib.h>
# include <ctype.h>
#endif // OS_LINUX

#ifdef OS_ECOS
# undef OS_ECOS_KAPI
# ifdef OS_ECOS_KAPI
#   include <cyg/kernel/kapi.h>
# endif
# include <cyg/infra/cyg_type.h>
#endif

#ifdef OS_WIN32
# include <stdio.h>
# include <string.h>
# include <stdarg.h>
# include <stdlib.h>
# include <ctype.h>
#endif

#ifdef OS_WIN32
# include <windows.h>
# include <process.h>
# include <pthread.h>
# define  MUTEX_INIT(m,v) (*(m) = CreateMutex(NULL, FALSE, NULL))
# define  MUTEX_LOCK(m) WaitForSingleObject(m, INFINITE)
# define  MUTEX_UNLOCK(m) ReleaseMutex(m)
# define  MUTEX_TERM(m) CloseHandle(*(m))
# define  vsnprintf _vsnprintf 
# define  snprintf  _snprintf
#endif

#ifdef OS_LINUX
# include <pthread.h>
# define MUTEX_INIT(m,v)   pthread_mutex_init(m, v)
# define MUTEX_LOCK(m)   pthread_mutex_lock(m)
# define MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
# define MUTEX_TERM(m)
# define O_BINARY (0)
# define O_TEXT   (0)
#endif

#include "memwatch.h"

#ifdef __cplusplus
}
#endif
#endif // _SSPDEFS_H
