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
 * $Id: ppositioner.h,v 0.89 1999/07/17 03:17:54 zhao Beta $
 *
 *  This file is part of the XForms library package.
 *  Copyright (c) 1995-1997  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * private header for positioner object
 *
 */

#ifndef PPOSITION_H
#define PPOSITION_H

typedef struct
{
    float xmin, ymin;		/* minimal values  */
    float xmax, ymax;		/* maximal values  */
    float xval, yval;		/* current values  */
    float lxval, lyval;		/* previous values */
    float xstep, ystep;		/* step size to which values are rounded  */
    int how_return;		/* whether always returning value  */
    int partial;
    int changed;
}

FL_POSITIONER_SPEC;

#endif
