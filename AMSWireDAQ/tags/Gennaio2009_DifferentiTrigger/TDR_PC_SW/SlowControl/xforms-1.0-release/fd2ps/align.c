/*
 *
 * This file is part of XForms.
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
 * $Id: align.c,v 0.36 1999/07/17 03:32:50 zhao beta $
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2000  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * Handles align requests
 *
 */

#include "forms.h"
#include "fd2ps.h"

void
fl_get_hv_align(int align, int *halign, int *valign)
{

    align &= ~FL_ALIGN_INSIDE;

    switch (align)
    {
    case FL_ALIGN_LEFT:
	*halign = FL_ALIGN_LEFT;
	*valign = FL_ALIGN_CENTER;
	break;
    case FL_ALIGN_RIGHT:
	*halign = FL_ALIGN_RIGHT;
	*valign = FL_ALIGN_CENTER;
	break;
    case FL_ALIGN_CENTER:
	*halign = FL_ALIGN_CENTER;
	*valign = FL_ALIGN_CENTER;
	break;
    case FL_ALIGN_TOP:
	*halign = FL_ALIGN_CENTER;
	*valign = FL_ALIGN_TOP;
	break;
    case FL_ALIGN_BOTTOM:
	*halign = FL_ALIGN_CENTER;
	*valign = FL_ALIGN_BOTTOM;
	break;
    case FL_ALIGN_LEFT_BOTTOM:
	*halign = FL_ALIGN_LEFT;
	*valign = FL_ALIGN_BOTTOM;
	break;
    case FL_ALIGN_RIGHT_BOTTOM:
	*halign = FL_ALIGN_RIGHT;
	*valign = FL_ALIGN_BOTTOM;
	break;
    case FL_ALIGN_LEFT_TOP:
	*halign = FL_ALIGN_LEFT;
	*valign = FL_ALIGN_TOP;
	break;
    case FL_ALIGN_RIGHT_TOP:
	*halign = FL_ALIGN_RIGHT;
	*valign = FL_ALIGN_TOP;
	break;
    default:
	fprintf(stderr, "GetAlign - Bad request: %d\n", align);
	*halign = FL_ALIGN_CENTER;
	*valign = FL_ALIGN_CENTER;
	break;
    }
}

void
fl_get_outside_align(int align, int x, int y, int w, int h,
		     int *new_align, int *newx, int *newy)
{
    *newx = x;
    *newy = y;
    *new_align = FL_ALIGN_CENTER;

    align &= ~FL_ALIGN_INSIDE;

    if (align == FL_ALIGN_LEFT)
    {
	*new_align = FL_ALIGN_RIGHT;
	*newx = x - w;
    }
    else if (align == FL_ALIGN_RIGHT)
    {
	*new_align = FL_ALIGN_LEFT;
	*newx = x + w;
    }
    else if (align == FL_ALIGN_TOP)
    {
	*new_align = FL_ALIGN_BOTTOM;
	*newy = y + h;
    }
    else if (align == FL_ALIGN_BOTTOM)
    {
	*new_align = FL_ALIGN_TOP;
	*newy = y - h;
    }
    else if (align == FL_ALIGN_LEFT_TOP)
    {
	*new_align = FL_ALIGN_LEFT_BOTTOM;
	*newy = y + h;
    }
    else if (align == FL_ALIGN_RIGHT_TOP)
    {
	*new_align = FL_ALIGN_RIGHT_BOTTOM;
	*newy = y + h;
    }
    else if (align == FL_ALIGN_LEFT_BOTTOM)
    {
	*new_align = FL_ALIGN_LEFT_TOP;
	*newy = y - h;
    }
    else if (align == FL_ALIGN_RIGHT_BOTTOM)
    {
	*new_align = FL_ALIGN_RIGHT_TOP;
	*newy = y - h;
    }
}


static void
get_align_inside(int align, int x, int y, int w, int h,
		 int xsize, int ysize, int xoff, int yoff,
		 int *xx, int *yy)
{
    int hor, vert;

    fl_get_hv_align(align, &hor, &vert);

    x += xoff;
    y += yoff;
    w -= 2 * xoff;
    h -= 2 * yoff;

    switch (hor)
    {
    case FL_ALIGN_LEFT:
	*xx = x;
	break;
    case FL_ALIGN_RIGHT:
	*xx = x + w - xsize;
	break;
    case FL_ALIGN_CENTER:
    default:
	*xx = x + (w - xsize) / 2;
	break;
    }

    switch (vert)
    {
    case FL_ALIGN_TOP:
	*yy = y + h - ysize;
	break;
    case FL_ALIGN_BOTTOM:
	*yy = y;
	break;
    case FL_ALIGN_CENTER:
    default:
	*yy = y + (h - ysize) / 2;
	break;
    }
}

static void
get_align_outside(int align, int x, int y, int w, int h,
		  int xsize, int ysize, int xoff, int yoff,
		  int *xx, int *yy)
{
    int newx, newy, new_align;

    fl_get_outside_align(align, x, y, w, h, &new_align, &newx, &newy);
    get_align_inside(new_align, newx, newy, w, h,
		     xsize, ysize, xoff, yoff, xx, yy);
}

void
fl_get_align_xy(int align, int x, int y, int w, int h,
		int xsize, int ysize, int xoff, int yoff,
		int *xx, int *yy)
{
    ((align & FL_ALIGN_INSIDE) ? get_align_inside : get_align_outside)
	(align, x, y, w, h, xsize, ysize, xoff, yoff, xx, yy);
}
