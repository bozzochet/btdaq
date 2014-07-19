/*
 *
 *  This file is part of the XForms library package.
 *
 * XForms is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1, or
 * (at your option) any later version.
 *
 * XForms is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with XForms; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */


/*
 * $Id: sysdep.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 *  X independent but system (SysV, BSD) or OS dependent utilities.
 *
 *  listdir.c contains another part of the system dependent stuff,
 *  namely the directory structure of the system.
 *
 *  check out local.h, which defines some flags that might affect
 *  this file
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_sysd = "$Id: sysdep.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#ifndef FL_WIN32
#include <sys/time.h>
#include <unistd.h>
#include <pwd.h>
#else
#include <sys/timeb.h>
#include <process.h>
#endif

#include "local.h"

/***********************************************************
 *
 * fl_whoami() returns the username
 *
 **********************************************************/

#ifndef FL_WIN32
const char *
fl_whoami(void)
{
#ifndef __VMS
    struct passwd *passwd = getpwuid(getuid());
    const char *name = passwd ? passwd->pw_name : "Unknown";
    endpwent();
#else
    const char *name = getenv("USER");
#endif
    return name ? name : "unknown";
}
#else /* WIN/NT platform */
const char *
fl_whoami(void)
{
    static char buf[32];
    int len = 32;

    return GetUserName(buf, &len) ? buf : "unknown";
}
#endif


/***************************************************
 * fl_now() returns today's date in ASCII format
 ***************************************************/
const char *
fl_now(void)
{
    time_t t = time(0);
    static char buf[64];

    strcpy(buf, asctime(localtime(&t)));
    buf[strlen(buf) - 1] = '\0';
    return buf;
}

/*****************************************************************
 *
 * fl_msleep() is similar to sleep(), but with milli-second argument
 *
 ******************************************************************/
#ifdef __VMS
#include <lib$routines.h>

unsigned long
fl_msleep(unsigned long msec)
{
    float wait_time;
    wait_time = msec;
    wait_time = wait_time * .001;
    (void) lib$wait(&wait_time);
    return 0;
}

#else

/* Yielding the processor for msec  milli-seconds */
unsigned long
fl_msleep(unsigned long msec)
{

#ifdef __EMX__
    _sleep2(msec);		/* more accurate than select/sleep in OS2 */
    return 0;
#else
#ifdef FL_WIN32
    _sleep(msec);
    return 0;
#else
#if !defined(DONT_HAVE_USLEEP)
    usleep(msec * 1000);
    return 0;
#else
    /* seems everybody has select these days */
    struct timeval timeout;
    timeout.tv_usec = 1000 * (msec % 1000);
    timeout.tv_sec = msec / 1000;
    return select(0, (void *) 0, (void *) 0, (void *) 0, &timeout);
#endif
#endif
#endif
}

#endif /* VMS */

/******* End of fl_msleep() ***************************/

/*************************************************************
 * Return the system time since a fixed point in time (On unix
 * systems, this point is typically 01/01/70).
 * Most useful for elapsed time.
 ************************************************************/
void
fl_gettime(long *sec, long *usec)
{
#ifndef FL_WIN32
    static struct timeval tp;
#ifdef opennt
    static unsigned long tzp;
#else
    static struct timezone tzp;
#endif

    gettimeofday(&tp, &tzp);
    *sec = tp.tv_sec;
    *usec = tp.tv_usec;
#else
    struct _timeb _gtodtmp;
    _ftime(&_gtodtmp);
    *sec = _gtodtmp.time;
    *usec = _gtodtmp.millitm * 1000;
#endif
}


long fl_getpid(void)
{
    return (long)getpid();
}


#if defined(__VMS) && __VMS_VER < 70000000
/* Thanks to David Mathog (mathog@seqaxp.bio.caltech.edu) for
   stealing it from the X11KIT distribution
 */


/*
 *      gettimeofday(2) - Returns the current time
 *
 *      NOTE: The timezone portion is useless on VMS.
 *      Even on UNIX, it is only provided for backwards
 *      compatibilty and is not guaranteed to be correct.
 */
#include "forms.h"
#include <timeb.h>

int
gettimeofday(struct timeval *tv, struct timezone *tz)
{
    timeb_t tmp_time;

    ftime(&tmp_time);

    if (tv != NULL)
    {
	tv->tv_sec = tmp_time.time;
	tv->tv_usec = tmp_time.millitm * 1000;
    }
    if (tz != NULL)
    {
	tz->tz_minuteswest = tmp_time.timezone;
	tz->tz_dsttime = tmp_time.dstflag;
    }

    return (0);

}
/*** End gettimeofday() ***/

#endif /* __VMS_VER */
