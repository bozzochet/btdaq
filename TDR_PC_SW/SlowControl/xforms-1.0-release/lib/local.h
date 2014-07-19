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
 * $Id: local.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-1998  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *  Some machine specific stuff.
 *
 *  if sprintf returns char *, need to define BAD_SPRINTF
 *
 */

#ifndef FL_LOCAL_H
#define FL_LOCAL_H		/* { */

#ifndef _WIN32
#include <unistd.h>
#endif

#if defined(_WIN32)
extern int strcasecmp(const char *, const char *);
#endif

#define DONT_HAVE_USLEEP

#define FL_SIGRET_IS_VOID
#define FL_SIGRET void

#if defined(__linux__)||defined(linux)
#undef DONT_HAVE_USLEEP
#define HAVE_SNPRINTF
#endif

/* solaris: do we need to check the version number ? */
#if defined(__SUNPRO_C) && __SUNPRO_C >=0x500
#define HAVE_SNPRINTF
#endif

#if defined(Lynx)
#define NEED_GETCWD
#endif

#if defined(__VMS)
#define NEED_GETCWD

#if __VMS_VER < 70000000
struct timezone
{
    int tz_minuteswest;		/* minutes west of Greenwich */
    int tz_dsttime;		/* type of dst correction */
};

extern int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif /* ___VMS_VER  */

#endif /* __VMS */

/* (some) suns do not have strerror, use sys_errlist */

#if ( defined(sun) || defined(__sun__) ) && !defined( SYSV )
#define DONT_HAVE_STRERROR
#define DONT_HAVE_ATEXIT
#define BAD_SPRINTF		/* returns char * */
#endif

/*#if defined(__sgi)
   #endif
 */

/* #if defined(_HPUX_SOURCE)
   #endif
 */

#if defined(M_XENIX)
#define NO_SOCK
#endif

/* #define NO_CONST     */

/* prototype is currently required */

/* #define NO_PROTOTYPE */

/********* End of Configurable stuff ***********/

/* some header has XK_XP_Left etc */
#if (XlibSpecificationRelease>=6)
#define HAVE_KP_DEFINE 1
#endif

#ifdef NO_CONST
#undef  const
#define const
#endif

#ifndef FL_PATH_MAX
#ifdef PATH_MAX
#define FL_PATH_MAX   PATH_MAX
#else
#define FL_PATH_MAX   1024
#endif
#endif /* FL_PATH_MAX */

#ifndef FL_FLEN
#define FL_FLEN           256
#endif

/* There are two ways to handle motion events, one is to constantly
 * query the server for mouse position, and the other is to use
 * motion events. The first way obviously is slow and expensive
 * if runs across network. The latter however suffers with stepping
 * events.
 */

/*#define NO_MSG_INFO */

#ifdef HAVE_SNPRINTF
#define fl_snprintf snprintf
#else
extern int fl_snprintf(char *, size_t, const char *, ...);
#endif
#endif /* ifndef local.h } */
