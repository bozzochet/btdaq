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

#ifndef _AALMUTEX_H
#define _AALMUTEX_H

//  aalMutex.h   Wedge to support transportable mutex

//  Revision History
//    Aug 2004 by Peter Dennett
//      - Initial version 
//    Dec 2007 by Peter Dennett
//      - MinGW mods

#ifdef OS_LINUX
# include <pthread.h>

# define aalMutex_t          pthread_mutex_t
# define aalMutexInitializer PTHREAD_MUTEX_INITIALIZER
# define aalMutexInit(m, i)  pthread_mutex_init(m, i)
# define aalMutexDestroy(m)  pthread_mutex_destroy(m)
# define aalMutexLock(m)     pthread_mutex_lock(m)
# define aalMutexUnlock(m)   pthread_mutex_unlock(m)
#endif // OS_LINUX

#ifdef OS_ECOS
#ifndef OS_ECOS_KAPI
# define aalMutex_t          int
# define aalMutexInit(m, i)  
# define aalMutexDestroy(m)  
# define aalMutexLock(m)     
# define aalMutexUnlock(m)   
#else
# include <cyg/kernel/kapi.h>

# define aalMutex_t          cyg_mutex_t
# define aalMutexInit(m, i)  cyg_mutex_init(m)
# define aalMutexDestroy(m)  cyg_mutex_destroy(m)
# define aalMutexLock(m)     cyg_mutex_lock(m)
# define aalMutexUnlock(m)   cyg_mutex_unlock(m)
#endif
#endif

#ifdef OS_WIN32
# include <windows.h>
# include <process.h>
# define aalMutex_t          HANDLE

# define aalMutexInit(m, i)  (*(m) = CreateMutex(NULL, FALSE, NULL))
# define aalMutexDestroy(m)  CloseHandle(*(m))
# define aalMutexLock(m)     WaitForSingleObject(m, INFINITE)
# define aalMutexUnlock(m)   ReleaseMutex(m)
#endif

#endif // _AALMUTEX_H
