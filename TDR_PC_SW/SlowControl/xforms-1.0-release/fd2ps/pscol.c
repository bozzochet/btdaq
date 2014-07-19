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
 * $Id: pscol.c,v 0.36 1999/07/17 03:32:50 zhao beta $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1997-2000  by T.C. Zhao
 *  All rights reserved.
 *.
 *
 * Take care of color
 */

#include "forms.h"
#include "flinternal.h"
#include <math.h>
#include "fd2ps.h"

#define VN(a)    a,#a
#define NV(a)    #a,a

/* slightly different from XForms' built-in due to the difference
 * between  printer and screen
 */

#define PREVIEW

static FL_IMAP fl_imap[] =
{
    {NV(FL_BLACK), 0, 0, 0},
    {NV(FL_WHITE), 255, 255, 255},

    {NV(FL_COL1), 161, 161, 161},	/* default color, gray63 */
    {NV(FL_MCOL), 191, 191, 191},	/* used as magic, gray75 */
    {NV(FL_RIGHT_BCOL), 41, 41, 41},	/* right  gray16  */
    {NV(FL_BOTTOM_BCOL), 89, 89, 89},	/* bottom  gray35 */
    {NV(FL_LEFT_BCOL), 222, 222, 222},	/* left color  gray87 */

    {NV(FL_SLATEBLUE), 113, 113, 198},
    {NV(FL_INDIANRED), 198, 113, 113},

    {NV(FL_RED), 255, 0, 0},
    {NV(FL_BLUE), 0, 0, 255},
    {NV(FL_GREEN), 0, 255, 0},
    {NV(FL_YELLOW), 255, 255, 0},
    {NV(FL_MAGENTA), 255, 0, 255},
    {NV(FL_CYAN), 0, 255, 255},
    {NV(FL_TOMATO), 255, 99, 71},

    {NV(FL_INACTIVE), 110, 110, 110},	/* gray43       */
    {NV(FL_TOP_BCOL), 204, 204, 204},	/* top  gray80  */

    {NV(FL_PALEGREEN), 113, 198, 113},
    {NV(FL_DARKGOLD), 205, 149, 10},
    {NV(FL_ORCHID), 205, 105, 201},
    {NV(FL_DARKCYAN), 40, 170, 175},
    {NV(FL_DARKTOMATO), 139, 54, 38},
    {NV(FL_WHEAT), 255, 231, 155},
    {NV(FL_DARKORANGE), 255, 128, 0},
    {NV(FL_DEEPPINK), 255, 0, 128},
    {NV(FL_CHARTREUSE), 128, 255, 0},
    {NV(FL_DARKVIOLET), 128, 0, 255},
    {NV(FL_SPRINGGREEN), 0, 255, 128},
    {NV(FL_DODGERBLUE), 0, 128, 255},
    {NV(FL_DOGERBLUE), 0, 128, 255},
    {NV(FL_LIGHTER_COL1), 173, 173, 173},
    {NV(FL_DARKER_COL1), 153, 153, 153}
};

#define builtin   sizeof(fl_imap)/sizeof(fl_imap[0])

/*
 * do gamma adjustment
 */

void
apply_gamma(float gamma)
{
    FL_IMAP *fm = fl_imap, *fs;
    float lastgamma = 1.0;

    if (FL_abs(gamma) < 1.0e-3)
    {
	fprintf(stderr, "fd2ps: Bad Gamma value %.2f\n", gamma);
	return;
    }

    for (fs = fm + builtin; fm < fs; fm++)
    {
	if (psinfo.verbose)
	    fprintf(stderr, "fm->r=%d\n", fm->r);
	fm->r = (0.4 + 255.0 * (pow((double) fm->r / 255.0, lastgamma / gamma)));
	fm->g = (0.4 + 255.0 * (pow((double) fm->g / 255.0, lastgamma / gamma)));
	fm->b = (0.4 + 255.0 * (pow((double) fm->b / 255.0, lastgamma / gamma)));
	if (psinfo.verbose)
	    fprintf(stderr, "fm->r=%d\n", fm->r);
    }

    lastgamma = gamma;
}


void
fl_query_imap(long col, int *r, int *g, int *b)
{
    FL_IMAP *flmap = fl_imap, *flmape = flmap + builtin;

    for (; flmap < flmape; flmap++)
	if (col == flmap->index)
	{
	    *r = flmap->r;
	    *g = flmap->g;
	    *b = flmap->b;
	    return;
	}
}


#define  C2NC(c)             (1.0/255.0*c)

static long cur_color = -1;

void
ps_invalidate_color_cache(void)
{
    cur_color = -1;
}

void
ps_color(long color)
{
    int r, g, b;

    if (color == cur_color)
	return;
    fl_query_imap(color, &r, &g, &b);

    if (psinfo.colorps && (r != g || r != b))
	ps_output("%.3g %.3g %.3g RGB ", C2NC(r), C2NC(g), C2NC(b));
    else
	ps_output("%.3g G ", C2NC(rgb2gray(r, g, b)));

    cur_color = color;
}

int
get_gray255(long color)
{
    int r, g, b;
    fl_query_imap(color, &r, &g, &b);
    return rgb2gray(r, g, b) + 0.1;
}

int
fl_get_namedcolor(const char *s)
{
    FL_IMAP *flmap = fl_imap, *flmape = flmap + builtin;

    for (; s && flmap < flmape; flmap++)
	if (strcmp(s, flmap->name) == 0)
	    return flmap->index;
    /* a wild shot */
    return atoi(s);
}
