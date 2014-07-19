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
 * $Id: errmsg.c,v 0.80 1994/02/24 09:48:11 zhao Exp $
 *
 *.  Copyright(c) 1993,1994 by T.C. Zhao
 *   All rights reserved.
 *.
 *
 *  Error handling routines.
 *
 *  Messages are divided into graphics and non-graphics types.
 *  For graphical error messages, a user input may be demanded,
 *  while for non-graphical messages, a string is printed and
 *  does nothing else.
 *
 *  The graphical output routine must have the following form:
 *    void (*gmout)(const char *, const char *, const char *, int);
 ***********************************************************************/
#if !defined(lint) && defined(F_ID)
char *id_errm = "$Id: errmsg.c,v 0.80 1994/02/24 09:48:11 zhao Exp $";
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdarg.h>
#include <errno.h>
#include "local.h"  /* up stairs */

#include "forms.h"
#include "ulib.h"
extern int errno;		/* system error no            */

#ifdef DONT_HAVE_STRERROR
extern char *sys_errlist[];
#endif

#define MAXESTR 2048		/* maximum error string len   */

/**********************************************************************
 * msg_threshold controls amount of message to print
 * 0: only error      1: error and warning
 * 2: info            3: more info
 * 4: debugging       5: trace
 **********************************************************************/

/************ Local variables ****************************************/

static FILE *errlog;		/* where the msg is going       */
static ML_t threshold;		/* current threshold            */
static ML_t req_level;		/* requested message level      */
static const char *file;	/* source file name             */
static int lineno;		/* line no. in that file        */
static int gout;		/* if demand graphics           */
static Gmsgout_ gmout;

/*************** set up where err is gonna go ********************/
void
fl_set_err_logfp(FILE * fp)
{
    if (fp)
	errlog = fp;
}

void
fl_set_error_handler( FL_ERROR_FUNC user_func)
{
    user_error_function_ = user_func;
}

const char *fl_get_syserror_msg(void)
{
    const char  *pp;
#ifndef DONT_HAVE_STRERROR
    pp = errno ? strerror(errno) : "";
#else
    pp = errno ? sys_errlist[errno]:"";
#endif
    return pp;
}

/********************* Message levels  ****************************/
void
set_msg_threshold(ML_t mlevel)
{
    threshold = mlevel;
}

/************* Graphics output routine ***************(((*****/
void
set_err_msg_func(Gmsgout_ errf)
{
    gmout = errf;
}


FL_ERROR_FUNC efp_;			/* global pointer to shut up lint */
FL_ERROR_FUNC user_error_function_;  /* hooks for application error handler */

/********************************************************************
 * generate two strings that contain where and why an error occured
 *********************************************************************/

/* VARARGS2 */
static void
P_errmsg(const char *func, const char *fmt,...)
{
    va_list args;
    char *where, *why,  line[20];
    const char *pp;
    static char emsg[MAXESTR + 1];

    if (!errlog)
	errlog = stderr;

    /* if there is nothing to do, do nothing ! */

#if 0
    if (req_level >= threshold && (!gout || !gmout))
#else
    /*
     * by commenting out gout, graphics output is also controled by threshold
     */
    if (req_level >= threshold)
#endif
    {
        errno = 0;
	return;
    }

/*
 * where comes in two varieties, one is to print everthing, i.e.,
 * 1. FUNC [file, lineno]: why an error occured.
 * 2. why the mesage is printed
 *
 * If func passed is null, 2 will be used else 1 will be used.
 */

    if (func != 0)
    {
	  strcpy(line, lineno > 0 ? fl_itoa(lineno) : "?");
	  where = *func ?
	      vstrcat("In ", func, " [", file, " ", line, "] ", (char *) 0) :
	      vstrcat("In ", file, "[", line, "]: ", (char *) 0);
    }
    else
    {
	  line[0] = '\0';
	  where = strdup("");
    }

    /* now find out why */

    emsg[0] = '\0';
    why = 0;

    /* parse the fmt */
    if (fmt && *fmt)
      {
	  va_start(args, fmt);
#ifdef HAVE_SNPRINTF
	  (void) vsnprintf(emsg, sizeof(emsg)-5, fmt, args);
#else
	  (void) vsprintf(emsg, fmt, args);
#endif
	  va_end(args);
      }

    /* check if there is any system errors */
    if((pp = fl_get_syserror_msg()) && *pp)
    {
	strncat(strcat(emsg, "--"), pp, MAXESTR);
        emsg[MAXESTR-1] = '\0';
    }

    why = emsg;

/*
 * have gotten the message, where and why, show it
 */
    if (req_level < threshold)
	fprintf(errlog, "%s%s\n", where, why);

    if (gout && gmout)
    {
	gmout("Warning", where, why,  0);
    }

    free_vstrcat(where);

    /* reset system errors */
    errno = 0;
    return;
}

/*********************************************************************
 * get the line number in file where error occurred. gui indicates
 * if graphics output function is to be called
 ********************************************************************/

FL_ERROR_FUNC
whereError(int gui, ML_t level, const char *f, int l)
{
    file = f;
    lineno = l;
    req_level = level;
    gout = gui;
    return user_error_function_ ? user_error_function_:P_errmsg;
}

#ifdef TEST
extern void TC_continue(const char *, const char *, const char *, int);
main()
{
    set_err_msg_fp(stderr);
    Bark("ThisisMain", "error1");
    M_err("ThisisMain", "error1");
}

#endif
