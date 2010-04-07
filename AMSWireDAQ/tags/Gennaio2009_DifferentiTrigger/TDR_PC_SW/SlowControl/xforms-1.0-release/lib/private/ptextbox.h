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
 * $Id: ptextbox.h,v 0.89 1999/07/17 03:17:54 zhao Beta $
 *
 *  This file is part of the XForms library package.
 *  Copyright (c) 1995-1997  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *  private header info for textbox object (part of browser
 *
 */

#ifndef  PTEXTBOX_H
#define  PTEXTBOX_H

typedef struct
{
    char *txt;			/* The text                      */
    unsigned len;		/* line length                   */
    short selected;		/* Whether selected              */
    short non_selectable;	/* if non-selectable             */
    short pixels;		/* length in pixels              */
}

LINE;

typedef struct
{
    LINE **text;		/* lines of text(0 not used) */
    FL_CALLBACKPTR callback;
    long callback_data;
    GC bkGC;			/* background GC             */
    GC selectGC;		/* selected mark GC          */
    GC primaryGC;		/* text drawing GC           */
    GC specialGC;		/* handle font/color change  */
    FL_COLOR lcol;
    FL_COLOR col1;
    FL_COLOR col2;
    FL_Coord x, y, w, h;	/* browser drawing area      */
    unsigned int drawtype;
    int topline;		/* Current topline           */
    int oldtopline;		/* change mark               */
    int lines;			/* Number of lines in browser */
    int avail_lines;		/* Max. available lines      */
    int selectline;		/* Last selected line        */
    int desel_mark;
    int specialkey;		/* Key that indicates a special symbol */
    int fontstyle;		/* Style of font                 */
    int fontsize;		/* The character size            */
    int charheight;		/* base font height              */
    int chardesc;		/* base font descent             */
    int screenlines;		/* no. of visible lines          */
    int vmode;			/* vmode GCs are valid for       */
    int maxpixels_line;
    int maxpixels;
    int attrib;			/* set if there is attrib change */
    int xoffset;		/* horizontal scroll in pixels    */
    int lastmx, lastmy;
}
FL_TEXTBOX_SPEC;

#endif
