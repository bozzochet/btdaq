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
 * $Id: ulib.h,v 0.80 1994/02/24 09:48:11 zhao Exp $
 *
 *    Macros and Prototypes for the utilities routines
 */

#ifndef TC_ULIB_H
#define TC_ULIB_H

#include <stdio.h>		/* for FILE */
#include <time.h>		/* time_t */


#ifndef FL_EXPORT
#  if !defined(FL_WIN32) || !defined(SHARED_LIB)
#      define FL_EXPORT extern
#  else
#     ifdef MAKING_FORMS
#        define FL_EXPORT __declspec( dllexport ) extern
#     else
#       define FL_EXPORT __declspec( dllimport ) extern
#     endif                          /* MAKING_FORMS */
#  endif                          /* FL_WIN32 */
#endif     /* !def FL_EXPORT */

/*  Some utility stuff */
#ifndef FL_VN_PAIR_STRUCT
#  define FL_VN_PAIR_STRUCT
   typedef struct
   {
        int val;
         const char *name;
    } FL_VN_PAIR;
#endif

/***************** Portable IO operations *******************{**/

extern int fl_readint(FILE *);
extern int fl_readpint(FILE *);
extern int fl_readhexint(FILE *);
extern int fl_fget4MSBF(FILE *);
extern int fl_fput4MSBF(int, FILE *);
extern int fl_fget2LSBF(FILE *);
extern int fl_fput2LSBF(int, FILE *);
extern int fl_fget2MSBF(FILE *);
extern int fl_fput2MSBF(int, FILE *);
extern int fl_fget4LSBF(FILE *);
extern int fl_fput4LSBF(int, FILE *);

#define put2LSBF(c,f)   (putc((c)&0xff,f),putc(((c) >> 8)&0xff,f))
#define put2MSBF(c,f)   (putc(((c) >> 8)&0xff,f),putc((c)&0xff,f))
#define put4LSBF(c,f)   (put2LSBF(c,f),put2LSBF((c) >> 16, f))
#define put4MSBF(c,f)   (put2MSBF((c) >> 16,f),put2MSBF(c, f))

/********** End of  Portable IO *******************}**/


FL_EXPORT int fl_get_vn_value(FL_VN_PAIR *, const char *);
FL_EXPORT const char *fl_get_vn_name(FL_VN_PAIR *, int);


extern char *fl_de_space(char *);
extern char *fl_space_de(char *);
extern char *fl_de_space_de(char *);
extern char * fl_nuke_all_spaces(char *);
extern char * fl_nuke_all_non_alnum(char *);

/********* Variable number arguments strcat ******************/

extern char *vstrcat(const char *,...);
extern void free_vstrcat(void *);

/******************** Conversions  ********/
extern const char *fl_itoa(int);
extern const char *fl_ftoa(float, int);

#ifndef TC_Abs
#define TC_Abs(a)      ((a) >= 0 ? (a) : -(a))
#endif

#ifndef TC_Min
#define TC_Min(a, b)   ( ((a) > (b)) ? (b) : (a))
#endif

#ifndef TC_Max
#define TC_Max(a, b)   ( ((a) > (b)) ? (a) : (b))
#endif

#ifndef StrReDup
#define StrReDup(a, b)  do { if(a) free(a); a = strdup(b); } while(0)
#endif

#ifndef Readline
#define Readline(s, fp)                                         \
	do                                                      \
	{   register int c_;                                    \
	    while ((c_ = getc(fp)) != EOF && c_ != '\n')        \
	       *s++ = c_;                                       \
            *s = '\0';                                          \
         } while (ZERO)
#endif

/*********************************************************************
 * Basic error handling routines
 ********************************************************************/

#ifndef TC_ERROR_H
#define TC_ERROR_H

#include <errno.h>

/*
 * message levels(verbosity). Error generating routine should
 * have a (positive) control parameter specifying how loud
 * to bark (i.e., amount of messages generated)
 */

#if 0
/*
 * macro version has the advantange that ML_xxx can be used
 * in CPP
 */
typedef enum
  {
      ML_ERR = -1,		/* always print       */
      ML_WARN,			/* warning            */
      ML_INFO1,			/* good to have       */
      ML_INFO2,			/* more               */
      ML_DEBUG,			/* many output        */
      ML_TRACE			/* bad program!       */
  } ML_t;

#else

  typedef int ML_t;
# define ML_ERR    (-1)
# define ML_WARN    0
# define ML_INFO1   1
# define ML_INFO2   2
# define ML_DEBUG   3
# define ML_TRACE   4

#endif

/*
 * actions to take when something is not right.
 * A_WAIT = 0, so that a default action is to wait. Not implemented
 */
typedef enum
  {
      A_WAIT,			/* print msg, demanding a response */
      A_MSG,			/* print msg, do nothing           */
      A_QUIT,			/* fatal, quit gracefully          */
      A_DIE			/* fatal, quit immdediately        */
  } Act_t;

extern FL_ERROR_FUNC whereError(int, ML_t, const char *, int), efp_;
extern FL_ERROR_FUNC user_error_function_;

/*
 * define the actual names that will be used
 */


/* non graphics stuff. Apparently cray has problems with this */
#if 0
/* #if !defined(_CRAY) && !defined(cray) && (1 == 0) */
# define M_err      (efp_ = whereError(0, ML_ERR, __FILE__, __LINE__))
# define M_warn     (efp_ = whereError(0, ML_WARN, __FILE__, __LINE__))
# define M_info     (efp_ = whereError(0, ML_INFO1, __FILE__, __LINE__))
# define M_info2    (efp_ = whereError(0, ML_INFO2, __FILE__, __LINE__))
# define M_debug    (efp_ = whereError(0, ML_DEBUG, __FILE__, __LINE__))
# define M_trace    (efp_ = whereError(0, ML_TRACE, __FILE__, __LINE__))
# define M_msg(a, b, c) (efp_=whereError(0, a, b, c))
#else
# define M_err      (efp_ = whereError(0, ML_ERR, __FILE__, __LINE__)),efp_
# define M_warn     (efp_ = whereError(0, ML_WARN, __FILE__, __LINE__)),efp_
# define M_info     (efp_ = whereError(0, ML_INFO1, __FILE__, __LINE__)),efp_
# define M_info2    (efp_ = whereError(0, ML_INFO2, __FILE__, __LINE__)),efp_
# define M_debug    (efp_ = whereError(0, ML_DEBUG, __FILE__, __LINE__)),efp_
# define M_trace    (efp_ = whereError(0, ML_TRACE, __FILE__, __LINE__)),efp_
# define M_msg(a, b, c) (efp_=whereError(0, a, b, c)),efp_
#endif

/* graphics    */
#define Bark            (efp_= whereError(1, ML_ERR, __FILE__, __LINE__)),efp_
#define GM_msg(a, b, c) (efp_= whereError(1, a, b, c)),efp_

/*
 * the actual output routines takes three constant strings and an integer
 * in the order title, where, why, 0
 */
typedef void (*Gmsgout_) (const char *, const char *, const char *, int);

/****** Misc. control routines **********/
extern void set_err_msg_func(Gmsgout_);

extern void set_msg_threshold(ML_t);
extern const char *fl_get_syserror_msg(void);


#define to_be_written(a) Bark(a,"To be written");

#endif /* ERROR_H */

#endif /* TC_ULIB_H */
