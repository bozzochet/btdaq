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
 * $Id: chart.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 *  All chart objects. Absolutely needs prototypes for all
 *  drawing functions.
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_chrt = "$Id: chart.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <sys/types.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "forms.h"

#ifndef M_PI
#define M_PI	3.14159265
#endif

#define ARCINC	(M_PI/1800)

/* Object specific information */

typedef struct
{
    float val;			/* Value of the entry       */
    int col;			/* Color of the entry       */
    int lcol;			/* Label color of the entry */
    char str[16];		/* Label of the entry       */
}
ENTRY;

typedef struct
{
    float min, max;		/* The boundaries       */
    int numb;			/* Number of entries    */
    int maxnumb;		/* Maximal number of entries to display */
    int autosize;		/* Whether the x-axis should be scaled  */
    int lstyle, lsize;		/* item label font style & size         */
    int x, y, w, h;		/* drawing area                         */
    FL_COLOR lcol;		/* default label color                  */
    ENTRY *entries;	/* The entries */
    int no_baseline;
}
SPEC;

#define vv(x,y) fl_add_float_vertex((float)x,(float)y)

/*
 * Draws a bar chart. x,y,w,h is the bounding box, entries the array of
 * numb entries and min and max the boundaries.
 */
static void
draw_barchart(SPEC *sp, float min, float max)
{
    int x = sp->x, y = sp->y, w = sp->w, h = sp->h;
    int numb = sp->numb;
    int i, j, n;
    float bwidth;		/* Width of a bar */
    FL_Coord zeroh;		/* Height of zero value */
    FL_Coord val, xx, dx;
    float incr, xfuzzy;		/* Increment per unit value */
    float lh = (float)fl_get_char_height(sp->lstyle, sp->lsize, &i, &j);
    ENTRY *e, *es, *entries = sp->entries;
    int lbox;
    float fx;

    incr = h / (max - min);
    zeroh = (FL_Coord)((y + h) + min * incr);

    if (-min * incr < lh)
    {
	incr = (h - lh) / max;
	zeroh = (FL_Coord)((y + h) - lh);
    }

    bwidth = (float) w / (sp->autosize ? numb : sp->maxnumb);

    /* base line */
    if(!sp->no_baseline)
       fl_line(x, (int)(zeroh + 0.5f), x + w, (int)(zeroh + 0.5f), FL_BLACK);

    if (min == 0.0f && max == 0.0f)
	return;			/* Nothing else to draw */

    /* Draw the bars */
    n = 1;
    if ((xfuzzy = bwidth - (FL_Coord) bwidth) != 0.0f)
	n = (int)(1.0f / xfuzzy + 2);

    for (e = entries, xx = x, i = 0, es = e + numb; e < es; e++, i++)
    {
	dx = (FL_Coord) (bwidth + ((i % n) * xfuzzy));
	if (e->val != 0.0f)
	{
	    val = (FL_Coord)(e->val * incr);
	    fl_rectbound(xx, (zeroh - val), dx, val, e->col);
	}
	xx += dx;
    }

    /* Draw the labels */
    lbox = (FL_Coord)(0.8f * bwidth);
    for (e = entries, es = e + numb, fx = x; e < es; fx += bwidth, e++)
	fl_drw_text_beside(FL_ALIGN_BOTTOM, fx + 0.5f * (bwidth - lbox),
			   zeroh - lbox, lbox, lbox, e->lcol,
			   sp->lstyle, sp->lsize, e->str);
}

/*
 * Draws a horizontal bar chart. x,y,w,h is the bounding box, entries the
 * array of numb entries and min and max the boundaries.
 */
static void
draw_horbarchart(SPEC *sp, float min, float max)
{
    int x = sp->x, y = sp->y, w = sp->w, h = sp->h;
    int numb = sp->numb;
    float bwidth;		/* Width of a bar                       */
    float incr;			/* Increment per unit value             */
    FL_Coord lw, zeroh, dy, yy;	/* Label width & Position of zero value */
    int i, l, n;
    float yfuzzy;
    char *s;
    ENTRY *e, *entries = sp->entries;
    int lbox;

    /* Compute maximal label width */
    for (lw = 0, i = 0; i < numb; i++)
    {
	s = entries[i].str;
	l = fl_get_string_width(sp->lstyle, sp->lsize, s, strlen(s));
	if (l > lw)
	    lw = l;
    }

    if (lw > 0.0f)
	lw = (FL_Coord)(lw+4.0f);

    incr = (float) w / (max - min);
    zeroh = (FL_Coord)(x - min * incr + 0.1f);

    if (-min * incr < lw)
    {
	zeroh = x + lw;
	incr = (w - lw) / max;
    }


    bwidth = (float) h / (sp->autosize ? numb : sp->maxnumb);

    /* Draw base line */
    if (!sp->no_baseline)
       fl_line((int)(zeroh + 0.5f), y, (int)(zeroh + 0.5f), y + h, FL_BLACK);

    if (min == 0.0f && max == 0.0f)
	return;			/* Nothing else to draw */


    /* Draw the bars. Need to take care of round-off errors */

    yy = y;
    dy = (int) bwidth;
    n = 2;
    if ((yfuzzy = bwidth - dy) != 0)
	n = (FL_Coord)(1.0f / yfuzzy) + 2;

    for (e = entries + numb - 1, i = 0; i < numb; i++, e--)
    {
	dy = (FL_Coord) (bwidth + ((i % n) * yfuzzy));
	if (e->val != 0.0f)
	    fl_rectbound(zeroh, yy, (FL_Coord)(e->val * incr), dy, e->col);
	yy += dy;
    }

    /* Draw the labels */
    lbox = (FL_Coord)(0.8f * bwidth);
    for (e = entries + numb - 1, i = 0; i < numb; i++, e--)
	fl_drw_text_beside(FL_ALIGN_LEFT, zeroh,
			  (FL_Coord)(y + i * bwidth + 0.5f * (bwidth - lbox)),
			  lbox, lbox, e->lcol, sp->lstyle,
			  sp->lsize, e->str);
}

/* Draws a line chart. */
static void
draw_linechart(int type, SPEC *sp, float min, float max)
{
    int x = sp->x, y = sp->y, w = sp->w, h = sp->h;
    int i, numb = sp->numb;
    float ttt;
    float bwidth;		/* distance between points */
    float zeroh;		/* Height of zero value */
    float incr;			/* Increment per unit value */
    float lh = (float)fl_get_char_height(sp->lstyle, sp->lsize, 0, 0);
    ENTRY *e, *es, *entries = sp->entries;
    float xx, val1, val2, val3;	/* tmp vars */
    int lbox;

    incr = (h - 2 * lh) / (max - min);
    zeroh = (y + h) - (lh - min * incr);

    bwidth = (float) w / (sp->autosize ? numb : sp->maxnumb);

    /* Draw the values */
    for (i = 0; i < numb; i++)
    {
	val3 = entries[i].val * incr;
	if (type == FL_SPIKE_CHART)
	{
	    val1 = (i + 0.5f) * bwidth;
	    fl_bgnline();
	    fl_color(entries[i].col);
	    vv(x + val1, zeroh);
	    vv(x + val1, zeroh - val3);
	    fl_endline();
	}
	else if (type == FL_LINE_CHART && i != 0)
	{
	    e = entries + i - 1;
	    fl_bgnline();
	    fl_color(e->col);
	    vv(x + (i - 0.5f) * bwidth, zeroh - e->val * incr);
	    vv(x + (i + 0.5f) * bwidth, zeroh - val3);
	    fl_endline();
	}
	else if (type == FL_FILLED_CHART && i != 0)
	{
	    e = entries + i - 1;
	    val1 = (i - 0.5f) * bwidth;
	    val2 = (i + 0.5f) * bwidth;

	    fl_bgnpolygon();
	    fl_color(e->col);
	    vv(x + val1, zeroh);
	    vv(x + val1, zeroh - e->val * incr);
	    if ((e->val > 0.0f && entries[i].val < 0.0f) ||
		(e->val < 0.0f && entries[i].val > 0.0f))
	    {
		ttt = (e->val / (e->val - entries[i].val));
		vv(x + (i - 0.5f + ttt) * bwidth, zeroh);
		vv(x + (i - 0.5f + ttt) * bwidth, zeroh);
	    }

	    vv(x + val2, zeroh - val3);
	    vv(x + val2, zeroh);
	    fl_endpolygon();

	    fl_bgnline();
	    fl_color(FL_BLACK);
	    vv(x + val1, zeroh - e->val * incr);
	    vv(x + val2, zeroh - val3);
	    fl_endline();

	}
    }

    /* Draw base line */
    if (!sp->no_baseline)
       fl_line(x, (FL_Coord)(zeroh + 0.5f), x + w, 
              (FL_Coord)(zeroh + 0.5f), FL_BLACK);

    /* Draw the labels */
    lbox = (FL_Coord)(0.8f * bwidth);
    xx = x + 0.5f * (bwidth - lbox);
    for (e = entries, es = e + numb; e < es; e++, xx += bwidth)
    {

	if (e->val < 0.0f)
	    fl_drw_text_beside(FL_ALIGN_TOP, xx, 
                              (FL_Coord)(zeroh - e->val * incr + 12),
			       lbox, lbox, e->lcol, sp->lstyle,
			       sp->lsize, e->str);
	else
	    fl_drw_text_beside(FL_ALIGN_BOTTOM, xx,
			       (FL_Coord)(zeroh - e->val * incr - 12) - lbox,
			       lbox, lbox, e->lcol, sp->lstyle,
			       sp->lsize, e->str);
    }
}

/* Draws a pie chart. x,y,w,h is the bounding box, entries the array of
 * numb entries */

static void
draw_piechart(SPEC *sp, int special)
{
    int x = sp->x, y = sp->y, w = sp->w, h = sp->h;
    int i, numb = sp->numb;
    float xc, yc, rad;		/* center and radius */
    float tot;			/* sum of values */
    float incr;			/* increment in angle */
    float curang;		/* current angle we are drawing */
    float xl, yl;		/* label position */
    float txc, tyc;		/* temporary center */
    float lh = (float)fl_get_char_height(sp->lstyle, sp->lsize, 0, 0);
    int lbox;
    ENTRY *e, *entries = sp->entries;

    /* compute center and radius */
    xc = x + w / 2;
    yc = y + h / 2;
    rad = h / 2 - lh;

    if (special)
    {
	yc += 0.1f * rad;
	rad = 0.9f * rad;
    }

    /* compute sum of values */
    for (tot = 0.0f, i = 0; i < numb; i++)
	if (entries[i].val > 0.0f)
	    tot += entries[i].val;

    if (tot == 0.0f)
	return;

    incr = 3600.0f / tot;

    /* Draw the pie */
    curang = 0.0f;
    for (e = entries, i = 0; i < numb; i++, e++)
	if (e->val > 0.0f)
	{
	    float tt = incr * e->val;

	    txc = xc;
	    tyc = yc;

	    /* Correct for special pies */
	    if (special && i == 0)
	    {
		txc += 0.2f * rad * cos(ARCINC * (curang + tt * 0.5f));
		tyc -= 0.2f * rad * sin(ARCINC * (curang + tt * 0.5f));
	    }

	    tt += curang;
	    fl_arcf(txc, tyc, rad, curang, tt, e->col);
	    fl_arc(txc, tyc, rad, curang, tt, FL_BLACK);

	    fl_bgnline();
	    vv(txc, tyc);
	    vv(txc + rad * cos(ARCINC * curang), tyc - rad * sin(ARCINC * curang));
	    fl_endline();

	    curang += 0.5f * incr * e->val;
	    lbox = 16;

	    /* draw the label */
	    xl = txc + 1.1 * rad * cos(ARCINC * curang);
	    yl = tyc - 1.1 * rad * sin(ARCINC * curang);

	    if (xl < txc)
		fl_drw_text_beside(FL_ALIGN_LEFT, xl, 
                                   (FL_Coord)(yl - 0.5f * lbox),
				   lbox, lbox, e->lcol, sp->lstyle,
				   sp->lsize, e->str);
	    else
		fl_drw_text_beside(FL_ALIGN_RIGHT, xl - lbox, 
                                   (FL_Coord)(yl - 0.5f * lbox),
				   lbox, lbox, e->lcol, sp->lstyle,
				   sp->lsize, e->str);

	    curang += 0.5f * incr * e->val;
	    fl_bgnline();
	    vv(txc, tyc);
	    vv(txc + rad * cos(ARCINC * curang), tyc - rad * sin(ARCINC * curang));
	    fl_endline();
	}
}

/* Draws a chart object */
static void
draw_chart(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    FL_Coord absbw = FL_abs(ob->bw);
    float min = sp->min, max = sp->max;
    int i;

    /* Find bounding box */
    sp->x = ob->x + 3 + 2 * absbw;
    sp->y = ob->y + 3 + 2 * absbw;
    sp->w = ob->w - 6 - 4 * absbw;
    sp->h = ob->h - 6 - 4 * absbw;

    /* Find bounds */
    if (min == max)
    {
	min = max = sp->numb ? sp->entries[0].val : 0.0f;
	for (i = 0; i < sp->numb; i++)
	{
	    if (sp->entries[i].val < min)
		min = sp->entries[i].val;
	    if (sp->entries[i].val > max)
		max = sp->entries[i].val;
	}

    }

    /* min can equal to max if only one entry */
    if (min == max)
    {
	min -= 1.0;
	max += 1.0;
    }

    /* Do the drawing */
    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

    if (sp->numb == 0)
    {
	fl_drw_text_beside(ob->align, ob->x, ob->y, ob->w, ob->h,
			   ob->lcol, ob->lstyle, ob->lsize, ob->label);
	return;
    }

    fl_set_clipping(sp->x - 1, sp->y - 1, sp->w + 2, sp->h + 2);

    switch (ob->type)
    {
    case FL_BAR_CHART:
	draw_barchart(sp, min, max);
	break;
    case FL_HORBAR_CHART:
	draw_horbarchart(sp, min, max);
	break;
    case FL_PIE_CHART:
	draw_piechart(sp, 0);
	break;
    case FL_SPECIALPIE_CHART:
	draw_piechart(sp, 1);
	break;
    default:
	draw_linechart(ob->type, sp, min, max);
	break;
    }

    fl_unset_clipping();
}

/* Handles an event, returns whether value has changed. */
static int
handle_chart(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	     int key, void *ev)
{

#if (FL_DEBUG >= ML_DEBUG)
    M_info("HandleMenu", fl_event_name(event));
#endif

    switch (event)
    {
    case FL_DRAW:
	draw_chart(ob);
    case FL_DRAWLABEL:
	fl_draw_object_label(ob);
	break;
    case FL_FREEMEM:
	fl_free(ob->spec);
	break;
    }
    return 0;
}

/*------------------------------*/

/* creates an object */
FL_OBJECT *
fl_create_chart(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;

    ob = fl_make_object(FL_CHART, type, x, y, w, h, label, handle_chart);
    ob->boxtype = FL_CHART_BOXTYPE;
    ob->col1 = FL_CHART_COL1;
    ob->col2 = FL_CHART_COL1;
    ob->align = FL_CHART_ALIGN;
    ob->lcol = FL_CHART_LCOL;

    ob->active = 0;

    ob->spec_size = sizeof(SPEC);
    sp = ob->spec = fl_calloc(1, ob->spec_size);
    sp->maxnumb = 512;
    sp->entries = fl_calloc(sp->maxnumb+1,sizeof *sp->entries);
    sp->autosize = 1;
    sp->lsize = FL_TINY_SIZE;
    sp->lstyle = FL_NORMAL_STYLE;
    sp->lcol = FL_BLACK;

    return ob;
}

void
fl_set_chart_lsize(FL_OBJECT * ob, int lsize)
{
    SPEC *sp = ob->spec;

    if (sp->lsize != lsize)
    {
	sp->lsize = lsize;
	fl_redraw_object(ob);
    }
}

void
fl_set_chart_lstyle(FL_OBJECT * ob, int lstyle)
{
    SPEC *sp = ob->spec;

    if (sp->lstyle != lstyle)
    {
	sp->lstyle = lstyle;
	fl_redraw_object(ob);
    }
}

void
fl_set_chart_lcolor(FL_OBJECT * ob, FL_COLOR lcol)
{
    SPEC *sp = ob->spec;

    if (sp->lcol != lcol)
	sp->lcol = lcol;
}

/* Adds an object */
FL_OBJECT *
fl_add_chart(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	     const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_chart(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}

/* Clears the contents of a chart. */
void
fl_clear_chart(FL_OBJECT * ob)
{
    ((SPEC *) (ob->spec))->numb = 0;
    fl_redraw_object(ob);
}

/* Add an item to the chart. */
void
fl_add_chart_value(FL_OBJECT * ob, double val, const char *str, int col)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    int i;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_CHART))
    {
	Bark("AddChartValue", "%s not a chart", ob ? ob->label : "");
	return;
    }
#endif

    /* Shift entries if required */
    if (sp->numb == sp->maxnumb)
    {
	for (i = 0; i < sp->numb - 1; i++)
	    sp->entries[i] = sp->entries[i + 1];
	sp->numb--;
    }

    /* Fill in the new entry */
    sp->entries[sp->numb].val = val;
    sp->entries[sp->numb].col = col;
    sp->entries[sp->numb].lcol = sp->lcol;
    strncpy(sp->entries[sp->numb].str, str, 16);
    sp->entries[sp->numb].str[15] = '\0';
    sp->numb++;
    fl_redraw_object(ob);
}

/* Inserts an item before indx to the chart. */
void
fl_insert_chart_value(FL_OBJECT * ob, int indx,
		      double val, const char *str, int col)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    int i;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_CHART))
    {
	Bark("AddChartValue", "%s not a chart", ob ? ob->label : "");
	return;
    }
#endif

    if (indx < 1 || indx > sp->numb + 1)
	return;

    /* Shift entries */
    for (i = sp->numb; i >= indx; i--)
	sp->entries[i] = sp->entries[i - 1];

    if (sp->numb < sp->maxnumb)
	sp->numb++;

    /* Fill in the new entry */
    sp->entries[indx - 1].val = val;
    sp->entries[indx - 1].col = col;
    strncpy(sp->entries[indx - 1].str, str, 16);
    sp->entries[indx - 1].str[15] = '\0';
    fl_redraw_object(ob);
}

/* Replaces an item in the chart. */
void
fl_replace_chart_value(FL_OBJECT * ob, int indx,
		       double val, const char *str, int col)
{
    SPEC *sp = ((SPEC *) (ob->spec));

    if (indx < 1 || indx > sp->numb)
	return;

    sp->entries[indx - 1].val = val;
    sp->entries[indx - 1].col = col;
    strncpy(sp->entries[indx - 1].str, str, 16);
    sp->entries[indx - 1].str[15] = '\0';
    fl_redraw_object(ob);
}

/* Sets the boundaries in the value for the object */
void
fl_set_chart_bounds(FL_OBJECT * ob, double min, double max)
{
    SPEC *sp;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_CHART))
    {
	Bark("SetChartBounds", "%s not a chart", ob ? ob->label : "");
	return;
    }
#endif

    sp = ob->spec;
    if (sp->min != min || sp->max != max)
    {
	sp->min = min;
	sp->max = max;
	fl_redraw_object(ob);
    }
}

void
fl_get_chart_bounds(FL_OBJECT * ob, double *min, double *max)
{
    SPEC *sp = ob->spec;

    *min = sp->min;
    *max = sp->max;
}

/* Sets the maximal number of values displayed in the chart */
int
fl_set_chart_maxnumb(FL_OBJECT * ob, int maxnumb)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    int i, curmax;

    /* Fill in the new number */
    if (maxnumb < 0)
	return FL_ARGUMENT;

    curmax = sp->maxnumb;

    if (maxnumb > FL_CHART_MAX)
	sp->maxnumb = FL_CHART_MAX;
    else
	sp->maxnumb = maxnumb;

    if (sp->maxnumb > curmax)
        sp->entries = fl_realloc(sp->entries, 
                      (sp->maxnumb+1)*sizeof *sp->entries);

   if (!sp->entries)
   {
       sp->maxnumb = curmax;
       sp->entries = fl_calloc(curmax+1,sizeof *sp->entries);
       return FL_ALLOC;
   }

    /* Shift entries if required */
    if (sp->numb > sp->maxnumb)
    {
	for (i = 0; i < maxnumb; i++)
	    sp->entries[i] = sp->entries[i + sp->numb - maxnumb];
	sp->numb = sp->maxnumb;
	fl_redraw_object(ob);
    }

    return 0;
}

/* Sets whether the chart should autosize along the x-axis */
void
fl_set_chart_autosize(FL_OBJECT * ob, int autosize)
{
    if (((SPEC *) (ob->spec))->autosize != autosize)
    {
	((SPEC *) (ob->spec))->autosize = autosize;
	fl_redraw_object(ob);
    }
}

void
fl_set_chart_baseline(FL_OBJECT *ob, int iYesNo)
{
    if (((SPEC *) (ob->spec))->no_baseline != !iYesNo)
    {
        ((SPEC *) (ob->spec))->no_baseline = !iYesNo;
        fl_redraw_object(ob);
    }
}

#if 0
void
fl_get_chart_area(FL_OBJECT * ob, int *x, int *y, int *w, int *h)
{
    SPEC *sp = ob->spec;

    *x = sp->x;
    *y = sp->y;
    *w = sp->w;
    *h = sp->h;
}
#endif
