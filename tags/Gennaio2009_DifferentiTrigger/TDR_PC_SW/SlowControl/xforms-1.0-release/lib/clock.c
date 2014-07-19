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
 * $Id: clock.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *   clocks
 */

#include "forms.h"
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI   3.1415926f
#endif

static float hourhand[4][2] =
{
    {-0.6f, 0.0f},
    {0.0f, -1.6f},
    {0.6f, 0.0f},
    {0.0f, 7.0f}
};

static float minhand[4][2] =
{
    {-0.6f, 0.0f},
    {0.0f, -1.6f},
    {0.6f, 0.0f},
    {0.0f, 11.6f}
};

static float sechand[4][2] =
{
    {-0.3f, 0.0f},
    {0.0f, -2.0f},
    {0.3f, 0.0f},
    {0.0f, 11.6f}
};

#define ROTx(x,y,a) (xc + ((x)-xc)*cos(a) + ((y)-yc)*sin(a))
#define ROTy(x,y,a) (yc - ((x)-xc)*sin(a) + ((y)-yc)*cos(a))
#define ROTxy(xx,yy,x,y,a)  xx = ROTx(x,y,a); yy = ROTy(x,y,a)

static void
draw_hand(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	  float a[][2], float ra, FL_COLOR fc, FL_COLOR bc)
{
    int i;
    float ccp[4][2], xx[4], yy[4];
    float xc = x + w / 2, yc = y + h / 2;
    FL_POINT xp[5];

    for (i = 0; i < 4; i++)
    {
	ccp[i][0] = (xc + a[i][0] * w / 28.0f);
	ccp[i][1] = (yc + a[i][1] * h / 28.0f);
    }

    ROTxy(xx[0], yy[0], ccp[0][0], ccp[0][1], ra);
    ROTxy(xx[1], yy[1], ccp[1][0], ccp[1][1], ra);
    ROTxy(xx[2], yy[2], ccp[2][0], ccp[2][1], ra);
    ROTxy(xx[3], yy[3], ccp[3][0], ccp[3][1], ra);

    for (i = 0; i < 4; i++)
    {
	xp[i].x = (short)FL_nint(xx[i]);
	xp[i].y = (short)FL_nint(yy[i]);
    }

    fl_polyf(xp, 4, fc);
    fl_polyl(xp, 4, bc);
}


static int hours, minutes, seconds;	/* hr: 0-23, minutes:0-59 */
static int updating;

static void
show_hands(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	   FL_COLOR fcolor, FL_COLOR bcolor)
{
    float ra;
    float fact = M_PI / 180.0f;

    ra = -(180.0f + hours * 360 / 12 + (minutes * 0.5f)) * fact;
    draw_hand(x, y, w, h, hourhand, ra, fcolor, bcolor);
    ra = -(180 + minutes * 360 / 60 + (seconds / 10)) * fact;
    draw_hand(x, y, w, h, minhand, ra, fcolor, bcolor);
    ra = -(180 + seconds * 360 / 60) * fact;
    draw_hand(x, y, w, h, sechand, ra, fcolor, bcolor);
}


static void
draw_clock(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	   FL_COLOR col1, FL_COLOR col2)
{
    FL_Coord xc = x + w / 2, yc = y + h / 2;
    int a, i, j;
    float ra;
    FL_Coord xx[5], yy[5];
    FL_POINT xp[5];
    float f1, f2, f3;

#if (FL_DEBUG >= M_DEBUG)
    M_info("DrawClock", "entering");
#endif

    w -= 4;
    h -= 4;
    /* draw hour ticks */
    f2 = 0.40f * h;
    f3 = 0.44f * h;
    for (a = 0, i = 0; i < 12; i++, a += 30)
    {
	ra = a * M_PI / 180.0f;
	if (i == 0 || i == 3 || i == 6 || i == 9)
	{
	    f1 = 0.02f * w;
	    ROTxy(xx[0], yy[0], xc - f1, yc + f2, ra);
	    ROTxy(xx[1], yy[1], xc + f1, yc + f2, ra);
	    ROTxy(xx[2], yy[2], xc + f1, yc + f3, ra);
	    ROTxy(xx[3], yy[3], xc - f1, yc + f3, ra);
	}
	else
	{
	    f1 = 0.01f * w;
	    ROTxy(xx[0], yy[0], xc - f1, yc + f2, ra);
	    ROTxy(xx[1], yy[1], xc + f1, yc + f2, ra);
	    ROTxy(xx[2], yy[2], xc + f1, yc + f3, ra);
	    ROTxy(xx[3], yy[3], xc - f1, yc + f3, ra);
	}

	for (j = 0; j < 4; j++)
	{
	    xp[j].x = FL_nint(xx[j]);
	    xp[j].y = FL_nint(yy[j]);
	}

	fl_polygon(1, xp, 4, FL_LEFT_BCOL);
    }

    show_hands(x + 2 + 0.02f * w, y + 2 + 0.02f * h, w, h, FL_RIGHT_BCOL, FL_RIGHT_BCOL);
    show_hands(x, y, w, h, col2, FL_LEFT_BCOL);

#if (FL_DEBUG >= M_DEBUG)
    M_info("DrawClock", "done");
#endif
}

typedef struct
{
    long sec;
    long offset;
    int nstep;
    int am_pm;			/* 12hr clock */
}
SPEC;

static void
draw_digitalclock(FL_OBJECT * ob)
{
    char buf[40];
    SPEC *sp = ob->spec;

    if (sp->am_pm)
	sprintf(buf, "%d:%02d:%02d %s", hours > 12 ? (hours - 12) : hours,
		minutes, seconds, (hours > 12 ? "pm" : "am"));
    else
	sprintf(buf, "%d:%02d.%02d", hours, minutes, seconds);

    fl_drw_text(FL_ALIGN_CENTER, ob->x, ob->y, ob->w, ob->h, ob->col2,
		ob->lstyle, ob->lsize, buf);
}


static int
handle_clock(FL_OBJECT * ob, int event, FL_Coord x, FL_Coord y,
	     int k, void *ev)
{
    time_t ticks;
    struct tm *timeofday;
    SPEC *sp = ob->spec;

    switch (event)
    {
    case FL_DRAW:
	fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);
	if (ob->type == FL_DIGITAL_CLOCK)
	    draw_digitalclock(ob);
	else
	    draw_clock(ob->type, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->col2);
	/* FALL THROUGH */
    case FL_DRAWLABEL:
	if (!updating)
	    fl_drw_text_beside(ob->align & ~FL_ALIGN_INSIDE,
			       ob->x, ob->y, ob->w, ob->h,
			       ob->lcol, ob->lstyle, ob->lsize, ob->label);
	updating = 0;
	break;
    case FL_STEP:
	/* clock only has resolution about 1sec. FL_STEP is sent about every
	   0.05 sec. If more than 10 clocks, we might run into trouble   */
	if (++sp->nstep & 1)
	    break;

	ticks = time(0);
	if (sp->sec != ticks)
	{
	    updating = 1;
	    sp->sec = ticks;
	    ticks += sp->offset;
	    timeofday = localtime(&ticks);
	    seconds = timeofday->tm_sec;
	    hours = timeofday->tm_hour;
	    minutes = timeofday->tm_min;
	    fl_redraw_object(ob);
	}
	break;
    case FL_FREEMEM:
	fl_free(ob->spec);
	break;
    }
    return 0;
}

FL_OBJECT *
fl_create_clock(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		const char *s)
{
    FL_OBJECT *obj;

    obj = fl_make_object(FL_CLOCK, type, x, y, w, h, s, handle_clock);
    obj->boxtype = FL_CLOCK_BOXTYPE;
    obj->col1 = FL_CLOCK_COL1;
    obj->col2 = FL_CLOCK_COL2;
    obj->lcol = FL_CLOCK_LCOL;
    obj->align = FL_CLOCK_ALIGN;
    obj->automatic = obj->active = 1;
    obj->spec = fl_calloc(sizeof(SPEC), 1);
    return obj;
}

FL_OBJECT *
fl_add_clock(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	     const char *s)

{
    FL_OBJECT *ob;

    ob = fl_create_clock(type, x, y, w, h, s);
    fl_add_object(fl_current_form, ob);
    fl_set_object_dblbuffer(ob, 1);
    return ob;
}

long
fl_set_clock_adjustment(FL_OBJECT * ob, long offset)
{
    SPEC *sp = ob->spec;
    long old = sp->offset;

    sp->offset = offset;
    return old;
}

void
fl_get_clock(FL_OBJECT * ob, int *h, int *m, int *s)
{
    SPEC *sp = ob->spec;
    time_t ticks;
    struct tm *tm;

    ticks = time(0);
    ticks += sp->offset;
    tm = localtime(&ticks);
    *h = tm->tm_hour;
    *m = tm->tm_min;
    *s = tm->tm_sec;
}

void
fl_set_clock_ampm(FL_OBJECT * ob, int y)
{
    SPEC *sp = ob->spec;

    if (sp->am_pm != y)
    {
	sp->am_pm = y;
	fl_redraw_object(ob);
    }
}
