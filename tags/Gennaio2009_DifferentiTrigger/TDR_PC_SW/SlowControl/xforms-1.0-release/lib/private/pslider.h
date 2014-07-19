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
 * $Id: pslider.h,v 0.89 1999/07/17 03:17:54 zhao Beta $
 *
 *  This file is part of the XForms library package.
 *  Copyright (c) 1995-1997  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * private header for slider object
 *
 */

#ifndef PSLIDER_H
#define PSLIDER_H

#if 0

typedef const char *(*SL_FILTER) (FL_OBJECT *, double, int);

/* The special information for sliders. */
typedef struct
{
    float min;			/* minimal value of slider */
    float max;			/* maximal value of slider */
    float val;			/* current value of slider */
    int prec;			/* precision when printing value */
    int how_return;		/* whether always returning value */
    double step;		/* step size             */
    double ldelta;		/* "left mouse" step   */
    double rdelta;		/* "right mouse " step   */
    float slsize;		/* size of the slider    */
    float oldval;
    float norm_val;		/* normalized value between 0 & 1 */
    float oldnorm_val;		/* normalized value between 0 & 1 */
    int drawtype;
    SL_FILTER filter;
    FL_Coord offx;
    FL_Coord offy;
    FL_Coord x, y, w, h;	/* draw (dbl buffer): adjustment report box */
    FL_Coord mx, my, mw, mh;	/* mouse: after adjustment for report box   */
    int mouse, lmouse;		/* part the mouse is on                     */
}

FL_SLIDER_SPEC;
#else
#include "pvaluator.h"
typedef FL_VALUATOR_SPEC FL_SLIDER_SPEC;

#endif


#define FL_MINKNOB_SB    16	/* scrollbar        */
#define FL_MINKNOB_SL    10	/* regular sliders  */

#define is_hslider(t)      (t==FL_HOR_SLIDER        ||\
                           t==FL_HOR_FILL_SLIDER    ||\
                           t==FL_HOR_NICE_SLIDER    ||\
                           t==FL_HOR_BROWSER_SLIDER ||\
                           t==FL_HOR_BROWSER_SLIDER2||\
                           t==FL_HOR_THIN_SLIDER    ||\
                           t==FL_HOR_BASIC_SLIDER)

#define is_vslider(t)     (!is_hslider(t))

#endif /* PSLIDER_H */
