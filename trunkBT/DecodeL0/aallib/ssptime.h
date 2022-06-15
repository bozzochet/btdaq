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

//  time.h   General time handling

//  Revision History
//    Mar 2003 by Peter Dennett
//      - Initial version at San Jose Del Cabo - pool side.
//    Jul 2004 by Peter Dennett
//      - Split general time code and DAQD timers
//    Dec 2007 by Peter Dennett
//        Updated for MinGW for WIN32 operation

#ifndef _SSPTIME_H
#define _SSPTIME_H

#include "sspdefs.h"
#include <sys/time.h>

//-Code: typedef TIMEVAL
//-  Defines a the basic time data type 
//-
//-  Time within DAQD is expressed as microseconds
//-  <pre>
//-  1 second = 1,000             milliseconds (ms) 
//-           = 1,000,000         microseconds (us)
//-           = 1,000,000,000     nanoseconds  (ns)
//-           = 1,000,000,000,000 picoseconds  (ps)
//-  </pre>

#ifdef OS_WIN32
typedef struct _ssptimeval
{
  unsigned long tv_sec;          // seconds
  unsigned long tv_usec;         // microseconds
} sspTIMEVAL, *psspTIMEVAL;
//-Code_End:
#else
#define sspTIMEVAL  struct timeval
#define psspTIMEVAL struct timeval *
#endif

bool timeCmp(psspTIMEVAL a, psspTIMEVAL b, char CMP);
void timeAdd(psspTIMEVAL a, psspTIMEVAL b, psspTIMEVAL result);
void timeSub(psspTIMEVAL a, psspTIMEVAL b, psspTIMEVAL result);
int  timeDelta(psspTIMEVAL a, psspTIMEVAL b, psspTIMEVAL result);
void timeGet(psspTIMEVAL result);

#endif // _SSPTIME_H
