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

//  ssptime.c   General time handling

//  Revision History
//    Mar 2003 by Peter Dennett
//      - Initial version at San Jose Del Cabo - pool side.
//    Jul 2004 by Peter Dennett
//      - Split general time code and DAQD timers
//    Apr 2007 by Peter Dennett
//      - Used as base for SSP time functions
//    Dec 2007 by Peter Dennett
//        Updated for MinGW for WIN32 operation

#include "ssptime.h"

////////////////////////////////////////////////////////////////////////////
//-Extern: timeCmp
bool timeCmp(psspTIMEVAL a, psspTIMEVAL b, char CMP)
//-Synopsis:    Perform a logical compare of two time values
//-Returns:     The logical result of the compare
//-ARG_I:       a              First time value
//-ARG_I:       b              Second time value
//-ARG_I:       CMP            The c boolean operation ('<', '>' '=', )
//-Description: Perform a logical compare of two time values.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:       No >= or <= yet!
//-Bugs:
//-Revisions:
//-End:
{
  if (CMP == '=')
  {
    if ((a->tv_sec == b->tv_sec) && (a->tv_usec == b->tv_usec)) return 1;
    return 0;
  }

  else if (CMP == '>')
  {
    if (a->tv_sec  > b->tv_sec)  return 1;
    if ((a->tv_sec == b->tv_sec) && (a->tv_usec > b->tv_usec)) return 1;
    return 0;
  }

  else if (CMP == '<')
  {
    if (a->tv_sec <  b->tv_sec) return 1;
    if ((a->tv_sec ==  b->tv_sec) && (a->tv_usec < b->tv_usec)) return 1;
    return 0;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: timeAdd
void timeAdd(psspTIMEVAL a, psspTIMEVAL b, psspTIMEVAL result)
//-Synopsis:    Perform an addition of two time values
//-Returns:     N/A
//-ARG_I:       a              First time value
//-ARG_I:       b              Second time value
//-ARG_I:       result         Pointer to place to store results
//-Description: Adds two time values.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  result->tv_sec  = a->tv_sec + b->tv_sec;
  result->tv_usec = a->tv_usec + b->tv_usec;
  while (result->tv_usec >= 1000000)
  {
    ++result->tv_sec;
    result->tv_usec -= 1000000;
  }
}

////////////////////////////////////////////////////////////////////////////
//-Extern: timeSub
void timeSub(psspTIMEVAL a, psspTIMEVAL b, psspTIMEVAL result)
//-Synopsis:    Perform a subtraction of two time values
//-Returns:     N/A
//-ARG_I:       a              First time value
//-ARG_I:       b              Second time value
//-ARG_I:       result         Pointer to place to store results
//-Description: Subtracts two time values. Results may not be satisfactory
//-             unless a >= b.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  result->tv_sec = a->tv_sec - b->tv_sec;
  result->tv_usec = a->tv_usec - b->tv_usec;
  while (result->tv_usec < 0)
  {
    if (result->tv_sec > 0)
       --result->tv_sec;

    result->tv_usec += 1000000;
  }
#if 0
  while (result->tv_usec <= -1000000)
  {
    --result->tv_sec;
    result->tv_usec += 1000000;
  }
#endif
}

////////////////////////////////////////////////////////////////////////////
//-Extern: timeDelta
int timeDelta(psspTIMEVAL a, psspTIMEVAL b, psspTIMEVAL result)
//-Synopsis:    Perform a subtraction of two time values
//-Returns:     1 if result is negative
//-ARG_I:       a              First time value
//-ARG_I:       b              Second time value
//-ARG_I:       result         Pointer to place to store results
//-Description: Subtracts two time values and returns delta and sign
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  if (timeCmp(a, b, '>'))
  {
    timeSub(a, b, result);
    return 0;
  }
  else
  {
    timeSub(b, a, result);
    return 1;
  }
}

////////////////////////////////////////////////////////////////////////////
//-Extern: timeGet
void timeGet(psspTIMEVAL result)
//-Synopsis:    Get the current time
//-Returns:     N/A
//-ARG_I:       result         Pointer to place to store results
//-Description: Gets the current time from the system
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
#ifdef OS_LINUX
  gettimeofday((struct timeval *)result, NULL);
#endif

#ifdef OS_WIN32
# ifdef OS_WIN32_MINGW
  gettimeofday((struct timeval *)result, NULL);
# else
#define EPOCHFILETIME (116444736000000000i64)

FILETIME        ft;
LARGE_INTEGER   li;
__int64         t;

  GetSystemTimeAsFileTime(&ft);

  li.LowPart  = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;

  t  = li.QuadPart;       /* In 100-nanosecond intervals */
  t -= EPOCHFILETIME;     /* Offset to the Epoch time */
  t /= 10;                /* In microseconds */

  result->tv_sec  = (long)(t / 1000000);
  result->tv_usec = (long)(t % 1000000);
#endif // MINGW32
#endif // OS_WIN32

  return;
}

////////////////////////////////////////////////////////////////////////////

#ifdef TEST_MAIN
void dt(int a_sec, int a_usec, int b_sec, int b_usec)
{
sspTIMEVAL a, b, r;
int s;

  a.tv_sec  = a_sec;
  a.tv_usec = a_usec;
  b.tv_sec  = b_sec;
  b.tv_usec  = b_usec;
  s = timeDelta(&a, &b, &r);
  printf("timeDelta %d.%06d and %d.%06d is %s %d.%06d\n", 
         a.tv_sec, a.tv_usec, 
         b.tv_sec, b.tv_usec, 
         s?"-":"+", r.tv_sec, r.tv_usec);
}

int main(int argc, char **argv)
{
sspTIMEVAL time_now;
//sspTIMEVAL a, b, r;
//int s;
sspTIMEVAL time_then, time_out, time_limit;

#ifdef OS_LINUX
  printf("OS_LINUX config\n");
#endif

#ifdef OS_WIN32
# ifdef OS_WIN32_MINGW
  printf("OS_WIN32 MINGW\n");
# else
  printf("OS_WIN32 MSVC\n");
#endif // MINGW32
#endif // OS_WIN32

  timeGet(&time_now);

  printf("Time now is %d sec %d usec\n", time_now.tv_sec, time_now.tv_usec);

  dt(1, 250, 1, 350);
  dt(1, 350, 1, 250);
  dt(1, 900000, 2, 100000);
  dt(2, 100000, 1, 900000);
  dt(1319, 51050, 1320, 974120);
  dt(1320, 974120, 1319, 51050);
  dt(1319, 974120, 1320, 51050);
  dt(1320, 51050, 1319, 974120);

  time_limit.tv_sec = 30;
  time_limit.tv_usec = 0;
          
  timeGet(&time_now);
  time_out = time_now;
  timeAdd(&time_out, &time_limit, &time_out);

  printf("Time out is %d sec %d usec\n", time_out.tv_sec, time_out.tv_usec);

  printf("Time now is %d sec %d usec\n", time_now.tv_sec, time_now.tv_usec);

  time_then = time_now;
  while (1)
  {
    timeGet(&time_now);
    if (timeCmp(&time_now, &time_out, '>'))
    {
      printf("Timed out at %d sec %d usec\n", time_now.tv_sec, time_now.tv_usec);
      break;
    }
    if (time_now.tv_sec != time_then.tv_sec)
    {
      printf("Time now is %d sec %d usec\n", time_now.tv_sec, time_now.tv_usec);
      time_then = time_now;
    }
  }

  return 0;
}
#endif // TEST_MAIN
