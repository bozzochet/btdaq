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
 * $Id: textbox.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *  Object class textbox - part of browser.
 *  X is very visible here because of speed considerations.
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_brw = "$Id: textbox.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include "forms.h"
#include "ptextbox.h"

#define LMARGIN   3		/* left margin */
#define NL        10

/*
 * for efficiency reasons, different redraws are handled by different
 * routines. These are the redraw types (complete must be zero)
 */
typedef enum
{
    COMPLETE,			/* non-conditional complete redraw    */
    VSLIDER,			/* slider moved, copyarea can be used */
    SELECTION,			/* selection/deselection change       */
    FULL = 4,			/* draw without drawing the box/slider */
    HSLIDER = FULL
}
DrawType;


#define SPEC FL_TEXTBOX_SPEC

static int textwidth(SPEC *, int, int, const char *, int);


/***************** DATA STRUCTURE MAINTENANCE ************************/

/* free the textbox */
static void
free_spec(SPEC *sp)
{
    int i;

    for (i = 1; sp->text[i] && i < sp->avail_lines; i++)
    {
	fl_free(sp->text[i]->txt);
	fl_free(sp->text[i]);
	sp->text[i] = 0;
    }

    fl_free(sp->text);
    sp->text = 0;

    /* it is possible that a textbox that has never been shown is being
       freed, thus the check. */
    if (sp->primaryGC)
    {
	XFreeGC(flx->display, sp->primaryGC);
	XFreeGC(flx->display, sp->specialGC);
	XFreeGC(flx->display, sp->bkGC);
	XFreeGC(flx->display, sp->selectGC);
	sp->primaryGC = 0;
    }

}

static void
find_longest_line(FL_OBJECT * ob, int recompute)
{
    SPEC *sp = ob->spec;
    LINE **tt = sp->text;
    int i;

    sp->maxpixels = 0;
    for (tt++, i = 1; i <= sp->lines; i++, tt++)
    {
	if (recompute)
	    (*tt)->pixels = textwidth(sp, sp->fontstyle, sp->fontsize,
				      (*tt)->txt, (*tt)->len);

	if ((*tt)->pixels > sp->maxpixels)
	{
	    sp->maxpixels = (*tt)->pixels;
	    sp->maxpixels_line = i;
	}
    }
}

/* Deletes a line from the textbox */
static void
delete_line(FL_OBJECT * ob, int linenumb)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    LINE *ttt;
    int i;

    ttt = sp->text[linenumb];
    for (i = linenumb; i < sp->lines; i++)
	sp->text[i] = sp->text[i + 1];

    sp->text[sp->lines] = ttt;
    sp->lines--;

    if (sp->selectline == linenumb)
	sp->selectline = 0;
    else if (sp->selectline > linenumb)
	sp->selectline--;

    if (linenumb == sp->maxpixels_line)
	find_longest_line(ob, 0);
}

static void
extend_textbox(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int i, newline;

    if (sp->lines < sp->avail_lines - 1)
	return;

    if (sp->avail_lines == 0)
    {
	sp->avail_lines = 100;
	sp->text = fl_malloc(sp->avail_lines * sizeof(*(sp->text)));
	for (i = 0; i < sp->avail_lines; i++)
	    sp->text[i] = 0;
    }
    else
    {
	newline = sp->avail_lines + 200;
	sp->text = fl_realloc(sp->text, newline * sizeof(*(sp->text)));
	for (i = sp->avail_lines; i < newline; i++)
	    sp->text[i] = 0;
	sp->avail_lines = newline;
    }
}

/* Inserts a single line of text to a textbox. */
static void
insert_line(FL_OBJECT * ob, int linenumb, const char *newtext)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    LINE *currline;
    int i;

    extend_textbox(ob);

    sp->lines++;

    /* Create new line if required */
    if (sp->text[sp->lines] == 0)
	sp->text[sp->lines] = fl_calloc(1, sizeof(LINE));

    /* Shift lines */
    currline = sp->text[sp->lines];
    for (i = sp->lines - 1; i >= linenumb; i--)
	sp->text[i + 1] = sp->text[i];

    sp->text[linenumb] = currline;

    /* Fill in line */
    currline->len = strlen(newtext);
    currline->selected = currline->non_selectable = 0;

    if (newtext[0] == sp->specialkey &&
	(newtext[1] == '-' || newtext[1] == 'N'))
	currline->non_selectable = 1;

    if (currline->txt)
	currline->txt = fl_realloc(currline->txt, currline->len + 1);
    else
	currline->txt = fl_malloc(currline->len + 1);

    strcpy(currline->txt, newtext);

    currline->pixels = textwidth(sp, sp->fontstyle, sp->fontsize,
				 newtext, currline->len);

    if (currline->pixels > sp->maxpixels)
    {
	sp->maxpixels_line = linenumb;
	sp->maxpixels = currline->pixels;
    }

    if (sp->selectline >= linenumb)
	sp->selectline++;

}

/* this is only the working line length. */
static int maxlen = FL_TEXTBOX_LINELENGTH;

/* insert multiple lines */
static void
insert_lines(FL_OBJECT * ob, int linenumb, const char *text)
{
    static int cur_maxlen;
    static char *newtext;
    int i, lastc;
    const char *ori_text = text;

    if (!newtext || (cur_maxlen < maxlen))
    {
	if (newtext)
	    fl_free(newtext);
	newtext = fl_malloc((cur_maxlen = maxlen) * sizeof(*newtext));
    }

    for (i = lastc = 0; *text; text++)
    {
	if (*text == NL)
	{
	    newtext[i] = 0;
	    insert_line(ob, linenumb++, newtext);
	    i = 0;
	}
	else if (i < maxlen - 1)
	    newtext[i++] = *text;

	lastc = *text;
    }

    newtext[i] = '\0';

    if (i || lastc == '\n' || !*ori_text)
	insert_line(ob, linenumb, newtext);
}

int
fl_set_textbox_maxlinelength(int n)
{
    int old = maxlen;

    if (n > 0)
	maxlen = n;

    return old;
}

/* Replaces a line of text in a textbox */
static void
replace_line(FL_OBJECT * ob, int linenumb, const char *newtext)
{
    LINE *currline = ((SPEC *) (ob->spec))->text[linenumb];
    char *s;
    SPEC *sp = ob->spec;

    /* Fill in line */
    currline->len = strlen(newtext);
    currline->non_selectable = 0;

    if (newtext[0] == sp->specialkey &&
	(newtext[1] == '-' || newtext[1] == 'N'))
	currline->non_selectable = 1;


    if (linenumb == sp->maxpixels_line)
	find_longest_line(ob, 0);

    /* safe even if newtext is a substring of txt */
    s = currline->txt;
    currline->txt = fl_strdup(newtext);

    currline->pixels = textwidth(sp, sp->fontstyle, sp->fontsize,
				 newtext, currline->len);
    if (currline->pixels > sp->maxpixels)
    {
	sp->maxpixels = currline->pixels;
	sp->maxpixels_line = linenumb;
    }

    if (s)
	fl_free(s);
}

/******************************** DRAWING ********************************/


#define FLTopMargin      ( FL_abs(ob->bw) + 2)
#define YtoLine(y)       ((y - FLTopMargin)/ charheight)

/* if the last line has large font, the descend may leave
 * some junk
 */
static void
fixup(FL_OBJECT * ob, SPEC *sp)
{
    FL_Coord ascend = sp->charheight - sp->chardesc;
    int y = sp->y + ascend + (sp->screenlines - 1) * sp->charheight;

    fl_drw_box(FL_FLAT_BOX, sp->x, y, sp->w,
	       ob->y + ob->h - FL_abs(ob->bw) - y, ob->col1, 0);
}

/*
 * Calculates the area in which the text is drawn.
 */

void
fl_calc_textbox_size(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int bw = FL_abs(ob->bw), junk;

    sp->charheight = fl_get_char_height(sp->fontstyle, sp->fontsize,
					&junk, &sp->chardesc);
    sp->w = ob->w - 3 * bw - 1;
    sp->h = ob->h - bw - FLTopMargin - 1;
    if (sp->h <= 0)
	sp->h = 1;
    sp->screenlines = ((float) sp->h / sp->charheight + 0.001);
}

static void
calc_textarea(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    FL_Coord bw = FL_abs(ob->bw);

    /* total textbox drawing area, to be adjusted for scroll bars */
    sp->x = ob->x + bw + 1;
    sp->y = ob->y + FLTopMargin;
    sp->w = ob->w - 3 * bw - 1;
    sp->h = ob->h - bw - FLTopMargin - 1;

    if (sp->h <= 0)
	sp->h = 1;

    sp->screenlines = ((float) sp->h / sp->charheight + 0.001);
}

/*
 * Before we start drawing, find out some basic info about fontsize,
 * character height etc and most importantly setup the GCs we will
 * be using to reduce GC changes on the fly.
 *
 */
static void
prepare_redraw(FL_OBJECT * ob, SPEC *sp)
{
    FL_Coord x, y, w, h;
    int junk;

    sp->charheight = fl_get_char_height(sp->fontstyle, sp->fontsize,
					&junk, &sp->chardesc);

    /* from no slider to slider, clip might be wrong. Check that */
    w = sp->w;
    h = sp->h;
    x = sp->x;
    y = sp->y;

    calc_textarea(ob);

#if (FL_DEBUG >= ML_DEBUG)
    M_info("DrawPrepare", "x=%d y=%d w=%d h=%d", sp->x, sp->y, sp->w, sp->h);
#endif

    /* x,y can conceivably change in the fdesign */
    if (!sp->primaryGC || sp->vmode != fl_vmode || sp->attrib ||
	w != sp->w || h != sp->h || x != sp->x || y != sp->y ||
	ob->lcol != sp->lcol || ob->col1 != sp->col1 || ob->col2 != sp->col2)

    {
	XGCValues xgcv;
	unsigned long gcvm;
	XFontStruct *xfs;
	int cx, cw;

	M_warn("Browser", "Creating new GCs");

	if (sp->primaryGC)
	{
	    XFreeGC(flx->display, sp->primaryGC);
	    XFreeGC(flx->display, sp->specialGC);
	    XFreeGC(flx->display, sp->bkGC);
	    XFreeGC(flx->display, sp->selectGC);
	}

	if (!(xfs = fl_get_fntstruct(sp->fontstyle, sp->fontsize)))
	{
	    Bark("Font", "Can't find style %d at size=%d",
		 sp->fontstyle, sp->fontsize);
	    xfs = fl_state[fl_vmode].cur_fnt;
	}

#if (FL_DEBUG >= ML_DEBUG)
	M_info("PrepareBrowser", "Font: size=%d style=%d",
	       sp->fontsize, sp->fontstyle);
#endif

	/* solicit graphics_exposure only if no backing store  This is not
	   always correct since although server is capable of backing store
	   it can run out resources. */
	if (fl_cntl.safe)
	    xgcv.graphics_exposures = 1;
	else
	{
	    Screen *scr = ScreenOfDisplay(flx->display, fl_screen);
	    xgcv.graphics_exposures = !DoesBackingStore(scr) ||
		!fl_cntl.backingStore;
	}
	gcvm = GCGraphicsExposures | GCForeground;

	cx = sp->x + LMARGIN - 1;
	cw = sp->w - LMARGIN;

	/* primaryGC. text drawing */
	xgcv.foreground = fl_get_flcolor(ob->lcol);
	sp->primaryGC = XCreateGC(flx->display, FL_ObjWin(ob), gcvm, &xgcv);
	XSetFont(flx->display, sp->primaryGC, xfs->fid);
	fl_set_gc_clipping(sp->primaryGC, cx, sp->y, cw, sp->h);

	/* background GC is used to redraw deselected lines */
	xgcv.foreground = fl_get_flcolor(ob->col1);
	sp->bkGC = XCreateGC(flx->display, FL_ObjWin(ob), gcvm, &xgcv);
	fl_set_gc_clipping(sp->bkGC, cx, sp->y, cw, sp->h);

	/* selectGC is used to mark the current selection */
	xgcv.foreground = fl_get_flcolor(fl_dithered(fl_vmode) ? FL_BLACK : ob->col2);
	sp->selectGC = XCreateGC(flx->display, FL_ObjWin(ob), gcvm, &xgcv);
	fl_set_gc_clipping(sp->selectGC, cx, sp->y, cw, sp->h);

	/* specialGC is used to handle font/color change on the fly */
	sp->specialGC = XCreateGC(flx->display, FL_ObjWin(ob), gcvm, &xgcv);
	fl_set_gc_clipping(sp->specialGC, cx, sp->y, cw, sp->h);

	/* save color attributes */
	sp->lcol = ob->lcol;
	sp->col1 = ob->col1;
	sp->col2 = ob->col2;

	sp->attrib = 0;
	sp->vmode = fl_vmode;
    }
}

/* Corrects the position of the topline inside the textbox */
static void
correct_topline(FL_OBJECT * ob)
{
    SPEC *sp = ((SPEC *) (ob->spec));

    if (sp->lines > sp->screenlines)
    {
	if (sp->lines - sp->topline + 1 < sp->screenlines)
	    sp->topline = sp->lines - sp->screenlines + 1;
	if (sp->topline < 1)
	    sp->topline = 1;
    }
    else
	sp->topline = 1;
}

/*
 * Draws the line on the textbox at position xx,yy, with maximal width ww.
 * back indicates whether the background should be drawn
 */
static void
draw_textline(FL_OBJECT * ob, int line, FL_Coord xx, FL_Coord yy, FL_Coord ww,
	      int back)
{
    SPEC *sp = ob->spec;
    char *str;			/* Start of actual string */
    int style = sp->fontstyle;	/* Actual font style used */
    FL_Coord size = sp->fontsize;	/* Actual font size used */
    FL_COLOR lcol = ob->lcol;	/* Actual font color used */
    int align = FL_ALIGN_LEFT;	/* Actual alignment used */
    FL_Coord ascend = sp->charheight - sp->chardesc;	/* character height */
    GC activeGC = sp->primaryGC;
    int has_special = 0, len;
    int dx = sp->xoffset;

    /* Draw the selection box if required */

    if (line > sp->lines)
	return;

    if (line <= sp->lines && sp->text[line]->selected)
	XFillRectangle(flx->display, FL_ObjWin(ob), sp->selectGC,
		       xx, (FL_Coord) (yy - ascend), ww, sp->charheight);
    else if (back)
	XFillRectangle(flx->display, FL_ObjWin(ob), sp->bkGC,
		       xx, (FL_Coord) (yy - ascend), ww, sp->charheight);


    len = sp->text[line]->len;
    xx += LMARGIN;

    str = sp->text[line]->txt;

    /* Check for special lines */
    if (sp->text[line]->non_selectable && ob->type != FL_NORMAL_TEXTBOX)
    {
	activeGC = sp->specialGC;
	has_special = 1;
	lcol = FL_INACTIVE;
    }

    for (; len >= 2 && *str && *str == sp->specialkey; str += 2, len -= 2)
    {
	if (*(str + 1) == sp->specialkey)
	{
	    str++;
	    len--;
	    break;
	}

	activeGC = sp->specialGC;
	has_special = 1;

	switch (str[1])
	{
	case 'l':
	    size = FL_LARGE_FONT;
	    yy += 2.0;
	    break;
	case 'L':
	    size += 6;
	    yy += 2.0;
	    break;
	case 'm':
	    size = FL_MEDIUM_FONT;
	    yy += 1.0;
	    break;
	case 'M':
	    size += 4;
	    yy += 1.0;
	    break;
	case 's':
	    size = FL_SMALL_FONT;
	    break;
	case 'S':
	    size -= 2;
	    break;
	case 'b':
	    style += FL_BOLD_STYLE;
	    break;
	case 'i':
	    style += FL_ITALIC_STYLE;
	    break;
	case 'f':
	    style = FL_FIXED_STYLE;
	    break;
	case 'n':
	    style = FL_NORMAL_STYLE;
	    break;
	case 't':
	    style = FL_TIMES_STYLE;
	    break;
	case 'c':
	    align = FL_ALIGN_CENTER;
	    break;
	case 'r':
	    align = FL_ALIGN_RIGHT;
	    break;
	case 'C':
	    /* requesting color changes. */
	    for (lcol = 0; str[2] >= '0' && str[2] <= '9'; str++, len--)
		lcol = 10 * lcol + str[2] - '0';
            if(lcol >= FL_MAX_COLS)
            {
                M_err("TextBox","bad color %d", lcol);
                lcol %= FL_MAX_COLORS;
            }
	    break;
	case '_':		/* underline. Up 1 to survive deselect */
	    fl_diagline(xx, (yy + sp->chardesc - 1), ww - LMARGIN - 2, 1, lcol);
	    break;
	case '-':
	    fl_drw_text(0, xx - 3, yy - ascend, ww + 2, sp->charheight,
			FL_COL1, 0, 10, "@DnLine");
	    str = " ";
	    len = 1;
	    has_special = 0;
	    sp->text[line]->non_selectable = 1;
	    break;
	case 'N':
	    sp->text[line]->non_selectable = 1;
	    if (ob->type != FL_NORMAL_TEXTBOX)
		lcol = FL_INACTIVE;
	    break;
	}
    }

    /* take care of special lines */

    if (has_special)
    {
	XFontStruct *xfs = fl_get_fntstruct(style, size);
	int sw = fl_get_string_widthTABfs(xfs, str, len);
	int tmpw = sp->maxpixels >= ww ? (sp->maxpixels + 5) : ww;

	XSetForeground(flx->display, activeGC, fl_get_flcolor(lcol));
	XSetFont(flx->display, activeGC, xfs->fid);
	if (align == FL_ALIGN_CENTER)
	    xx += (tmpw - sw) / 2;
	else if (align == FL_ALIGN_RIGHT)
	    xx = (tmpw - sw) - 1;
    }

    /* exception for B&W */
    if (fl_dithered(fl_vmode) && sp->text[line]->selected)
    {
	XFontStruct *xfs = fl_get_fntstruct(style, size);
	activeGC = sp->specialGC;
	XSetFont(flx->display, activeGC, xfs->fid);
	XSetForeground(flx->display, activeGC, fl_get_flcolor(FL_WHITE));
    }

    fl_drw_stringTAB(FL_ObjWin(ob), activeGC, xx - dx, yy, style, size,
		     str, len, 0);
}


/*
 * Do a complete redraw of the browser. This only happens with first
 * draw, or when font/size or other attributes changed. Slider movement
 * and selections are handled by other routines
 */
static void
draw_textbox(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    FL_Coord charheight = sp->charheight;
    FL_Coord ascend = charheight - sp->chardesc;	/* character height */
    int screenlines;		/* lines on screen */
    int i, y;			/* counters, etc. */

    screenlines = sp->screenlines;
    correct_topline(ob);

    if (sp->drawtype == COMPLETE)
    {
	fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

	fl_drw_text_beside(ob->align, ob->x, ob->y, ob->w, ob->h,
			   ob->lcol, ob->lstyle, ob->lsize, ob->label);
    }

    if (sp->lines == 0)
	return;

    fixup(ob, sp);

    y = sp->y + ascend;
    for (i = 0; i < screenlines; i++, y += charheight)
	draw_textline(ob, i + sp->topline, sp->x, y, sp->w,
		      sp->drawtype != COMPLETE);

}

/*
 * Slider moved. We simple blit the "scroll region" to its new position
 * and redraw only the lines that just come into view. However, special
 * provision should be made where blit might not perform well as this
 * routine is callable by application program via set_topline.
 */
static void
draw_slider_motion(FL_OBJECT * ob)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    FL_Coord charheight, ascend;	/* character height */
    int screenlines;		/* lines on screen */
    int i;			/* counters, etc. */
    FL_Coord xx, yy, ww;	/* text box */
    int delta, bh;

    charheight = sp->charheight;
    ascend = charheight - sp->chardesc;
    xx = sp->x;
    yy = sp->y;
    ww = sp->w;

    screenlines = sp->screenlines;
    correct_topline(ob);

    delta = (sp->oldtopline - sp->topline);

#if (FL_DEBUG >= ML_DEBUG)
    M_info("SliderMotion", "Delta=%d", delta);
#endif

    /* since user can influence the topline, blit is not always faster or
       appropriate. Set threshold to about 2/3 of the visible lines */


    if (FL_abs(delta) > (2 * screenlines / 3))
    {
	for (yy += ascend, i = 0; i < screenlines; i++, yy += charheight)
	    draw_textline(ob, i + sp->topline, xx, yy, ww, 1);
	return;
    }

    if (sp->oldtopline > sp->topline)
    {
	int sl;
	/* Window win = fl_get_real_object_window(ob); */

	/* calculate the area needs to be blited */
	bh = (screenlines - delta) * charheight;

	XCopyArea(flx->display, FL_ObjWin(ob),	/* win */
		  FL_ObjWin(ob), sp->primaryGC,
		  xx, sp->y, ww, bh, xx, sp->y + delta * charheight);

	fixup(ob, sp);

	/* draw the new lines */
	for (yy += ascend, i = 0; i < delta; yy += charheight, i++)
	    draw_textline(ob, i + sp->topline, xx, yy, ww, 1);

	/* Need to take care of special lines that use larger fonts */
	sl = sp->topline + 1;
	if (sl < sp->lines && sp->text[sl]->txt[0] == sp->specialkey)
	    draw_textline(ob, sl, xx, sp->y + ascend + charheight, ww, 0);

    }
    else if (sp->oldtopline < sp->topline)
    {
	int start;

	delta = (sp->topline - sp->oldtopline);

	/* calculate the area needs to be blited */
	bh = (screenlines - delta) * charheight;

	XCopyArea(flx->display, FL_ObjWin(ob),	/* fl_get_real_object_window(o
						   b), */
		  FL_ObjWin(ob), sp->primaryGC,
		  xx, sp->y + delta * charheight, ww, bh, xx, sp->y);


	fixup(ob, sp);

	yy += ascend;
	yy += (screenlines - delta) * charheight;
	start = screenlines - delta + sp->topline;
	for (i = 0; i < delta; yy += charheight, i++)
	    draw_textline(ob, i + start, xx, yy, ww, 1);

	/* Need to take care of special lines that use larger fonts */
	i = start - 1;
	if (i > 0 && sp->text[i]->txt[0] == sp->specialkey)
	{
	    yy = sp->y + ascend + (i - sp->topline) * charheight;
	    draw_textline(ob, i, xx, yy, ww, 0);
	}
    }
}

/*
 * A selection/deselection has taken place. Only need to draw
 * the relevant lines
 */
static void
draw_selection(FL_OBJECT * ob)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    FL_Coord charh;		/* character height */
    FL_Coord yy = sp->y;	/* text position */
    int sel;

    correct_topline(ob);
    charh = sp->charheight;

#if (FL_DEBUG >= ML_DEBUG)
    M_info("DrawSelect", "selected=%d deselected=%d top=%d",
	   sp->selectline, sp->desel_mark, sp->topline);
#endif

    yy += (charh - sp->chardesc);
    sel = sp->selectline - sp->topline;
    if (sel >= 0 && sel < sp->screenlines)
	draw_textline(ob, sp->selectline, sp->x, yy + sel * charh, sp->w, 0);

    /* desel_mark is measured from top line */
    sp->desel_mark -= sp->topline;
    if (sp->desel_mark >= 0 && sp->desel_mark < sp->screenlines)
	draw_textline(ob, sp->desel_mark + sp->topline,
		      sp->x, yy + sp->desel_mark * charh, sp->w, 1);

    sp->desel_mark = 0;

}

/***************** HANDLING EVENTS **************************/

/* it is possible for multi-browser to miss selections if mouse
 * is moved too quickly. A hack to recover the missed lines
 */
static void
handle_missed_selection(FL_OBJECT * ob, int line)
{
    SPEC *sp = ob->spec;
    int k, ns;

    if (sp->selectline < 0)
	return;

    if (line > sp->selectline)
    {
	for (k = sp->selectline + 1; k < line; k++)
	{
	    ns = sp->text[k]->non_selectable;
	    sp->text[k]->selected = !ns;
	    sp->selectline = k;
	    fl_object_qenter(ob);
	    fl_object_qread();
	}
    }
    else
    {
	for (k = sp->selectline - 1; k > line; k--)
	{
	    ns = sp->text[k]->non_selectable;
	    sp->text[k]->selected = !ns;
	    sp->selectline = k;
	    fl_object_qenter(ob);
	    fl_object_qread();
	}
    }
}

static void
handle_missed_deselection(FL_OBJECT * ob, int line)
{
    SPEC *sp = ob->spec;
    int k;

    if (line > FL_abs(sp->selectline))
    {
	for (k = FL_abs(sp->selectline) + 1; k < line; k++)
	{
	    sp->text[k]->selected = 0;
	    sp->selectline = -k;
	    sp->desel_mark = k;

	    fl_object_qenter(ob);
	    fl_object_qread();
	}
    }
    else
    {
	for (k = FL_abs(sp->selectline) - 1; k > line; k--)
	{
	    sp->text[k]->selected = 0;
	    sp->selectline = -k;
	    sp->desel_mark = k;

	    fl_object_qenter(ob);
	    fl_object_qread();
	}
    }
}

enum
{
    NOEVENT, SELECTEVENT, DESELECTEVENT, PAGEEVENT
};

static int eventtype = NOEVENT;	/* Type of interaction taking place */
static int lastselect, lastdeselect;

/* handles a mouse change. returns whether a selection change has occured */
static int
handle_mouse(FL_OBJECT * ob, FL_Coord mx, FL_Coord my, XMotionEvent * xev)
{
    SPEC *sp = ob->spec;
    float charheight = sp->charheight;
    int screenlines;		/* lines on screen */
    int line;			/* new number of lines */

    /* Check whether there are any lines */
    if (sp->lines == 0)
	return 0;

    /* Compute possible slider position change */
    correct_topline(ob);
    screenlines = sp->screenlines;

    /* Determine the type of event */
    if (eventtype == NOEVENT)
    {
	eventtype = SELECTEVENT;
	line = sp->topline + (my - ob->y - FLTopMargin) / charheight;
	if (ob->type == FL_MULTI_TEXTBOX &&
	    line >= 1 && line <= sp->lines &&
	    line < sp->topline + screenlines && sp->text[line]->selected)
	    eventtype = DESELECTEVENT;
    }

    /* Handle the event */
    switch (eventtype)
    {
	/* lot of scrollbar handling code removed */
    default:
	if (ob->type == FL_NORMAL_TEXTBOX)
	    return 0;

	line = sp->topline + (my - ob->y - FLTopMargin) / charheight;

	if (line < sp->topline)
	    line = sp->topline;

	if (line >= sp->topline + screenlines)
	    line = sp->topline + screenlines - 1;
	if (line > sp->lines)
	    line = sp->lines;

	if (eventtype == SELECTEVENT)
	{
	    /* if select a line that is already been selected, do nothing */
	    if (sp->text[line]->selected)
		return (ob->type != FL_MULTI_TEXTBOX);
	    if (sp->text[line]->non_selectable)
		return 0;

	    sp->drawtype = SELECTION;
	    /* mark selected and deselected lines */
	    if (ob->type != FL_MULTI_TEXTBOX && sp->selectline > 0)
	    {
		sp->text[sp->selectline]->selected = 0;
		sp->desel_mark = sp->selectline;
	    }

	    /* this is not exactly correct as we are (potentially) throwing
	       away events */
	    if (fl_object_qtest() == ob)
		fl_object_qread();

	    if (ob->type == FL_MULTI_TEXTBOX && lastselect &&
		FL_abs(line - sp->selectline) > 1)
	    {
		handle_missed_selection(ob, line);
		sp->drawtype = FULL;
	    }

	    sp->text[line]->selected = 1;
	    sp->selectline = line;
	}
	else
	{
	    /* eventtype==DESELECTEVENT && ob->type==FL_MULTI_TEXTBOX */
	    if (!sp->text[line]->selected || sp->text[line]->non_selectable)
		return 0;

	    sp->drawtype = SELECTION;
	    if (fl_object_qtest() == ob)
		fl_object_qread();

	    if (ob->type == FL_MULTI_TEXTBOX && lastdeselect
		&& (FL_abs(-sp->selectline - line) > 1))
	    {
		handle_missed_deselection(ob, line);
		sp->drawtype = FULL;
	    }

	    sp->text[line]->selected = 0;
	    sp->desel_mark = line;
	    sp->selectline = -line;
	}

	fl_redraw_object(ob);
	lastselect = eventtype == SELECTEVENT;
	lastdeselect = eventtype == DESELECTEVENT;
	return 1;
    }
}

/*-----------------------------------------------*/
static int
handle_keyboard(FL_OBJECT * ob, int key, XKeyEvent * xev)
{
    SPEC *sp = ob->spec;
    int old = sp->selectline;

    if (IsHome(key))
	fl_set_browser_topline(ob, 1);
    else if (IsEnd(key))
	fl_set_browser_topline(ob, sp->lines - sp->screenlines + 1);
    else if (IsPageUp(key))
	fl_set_browser_topline(ob, sp->topline - sp->screenlines);
    else if (IsHalfPageUp(key))
	fl_set_browser_topline(ob, sp->topline - sp->screenlines / 2);
    else if (Is1LineUp(key))
	fl_set_browser_topline(ob, sp->topline - 1);
    else if (IsPageDown(key) || key == ' ')
	fl_set_browser_topline(ob, sp->topline + sp->screenlines);
    else if (IsHalfPageDown(key))
	fl_set_browser_topline(ob, sp->topline + sp->screenlines / 2);
    else if (Is1LineDown(key))
	fl_set_browser_topline(ob, sp->topline + 1);
    else if (IsLeft(key))
	fl_set_browser_xoffset(ob, sp->xoffset - 3);
    else if (IsRight(key))
	fl_set_browser_xoffset(ob, sp->xoffset + 3);
    else if (IsUp(key))
    {
	if (ob->type == FL_NORMAL_TEXTBOX || ob->type == FL_SELECT_TEXTBOX ||
	    ob->type == FL_MULTI_TEXTBOX)
	    fl_set_browser_topline(ob, sp->topline - 1);
	else if (ob->type == FL_HOLD_TEXTBOX)
	{
	    fl_select_textbox_line(ob, sp->selectline - 1, 1);

	    /* bring the selection into view if necessary */
	    if (sp->selectline < sp->topline)
		fl_set_browser_topline(ob, sp->selectline);
	    else if ((sp->selectline - sp->topline) >= sp->screenlines)
		fl_set_browser_topline(ob, sp->selectline - sp->screenlines / 2);
	}
    }
    else if (IsDown(key))
    {
	if (ob->type == FL_NORMAL_TEXTBOX || ob->type == FL_SELECT_TEXTBOX ||
	    ob->type == FL_MULTI_TEXTBOX)
	    fl_set_browser_topline(ob, sp->topline + 1);
	else if (ob->type == FL_HOLD_TEXTBOX)
	{
	    fl_select_textbox_line(ob, sp->selectline + 1, 1);

	    /* bring the selection into view if necessary */
	    if ((sp->selectline - sp->topline) >= sp->screenlines)
		fl_set_browser_topline(ob, sp->selectline - sp->screenlines + 1);
	    else if (sp->selectline < sp->topline)
		fl_set_browser_topline(ob, sp->selectline - sp->screenlines / 2);
	}
    }
    
    fl_adjust_browser_scrollbar(ob);

    return (old != sp->selectline);
}


static int statuschanged;

/* Handles the textbox */
static int
handle_textbox(FL_OBJECT * ob, int ev, FL_Coord mx, FL_Coord my,
	       int key, void *xev)
{
    SPEC *sp = ((SPEC *) (ob->spec));

#if (FL_DEBUG >= ML_DEBUG)
    M_info2("HandleBrowser", fl_event_name(ev));
#endif

    /* wheel mouse hack */
    if (fl_handle_mouse_wheel(ob, &ev, &key, xev) == 0)
	return 0;

    switch (ev)
    {
    case FL_DRAW:
	ob->align &= ~FL_ALIGN_INSIDE;

	if (sp->drawtype == COMPLETE || sp->attrib)
	{
	    prepare_redraw(ob, sp);
	    draw_textbox(ob);
	    sp->drawtype = COMPLETE;
	}

	/* due to form freeze, both slider&selection can change */
	if ((sp->drawtype & VSLIDER))
	    draw_slider_motion(ob);

	if ((sp->drawtype & SELECTION))
	    draw_selection(ob);

	if ((sp->drawtype & FULL))
	{
	    draw_textbox(ob);
	}

	/* reset draw type */
	sp->drawtype = COMPLETE;
	sp->oldtopline = sp->topline;
	sp->desel_mark = 0;
	break;
    case FL_DRAWLABEL:
	fl_drw_text_beside(ob->align, ob->x, ob->y, ob->w, ob->h,
			   ob->lcol, ob->lstyle, ob->lsize, ob->label);
	break;
    case FL_PUSH:
	eventtype = NOEVENT;
	statuschanged = 0;
	lastselect = lastdeselect = 0;
	/* Fall through */
    case FL_MOUSE:

	if (my == sp->lastmy && my > ob->y && my < ob->y + ob->h - 1)
	    return 0;

	if (eventtype == SELECTEVENT || eventtype == DESELECTEVENT)
	{
	    if (my < ob->y)
		fl_set_browser_topline(ob, sp->topline - 1);
	    else if (my > ob->y + ob->h)
		fl_set_browser_topline(ob, sp->topline + 1);
	}

	if (handle_mouse(ob, mx, my, xev))
	    statuschanged = 1;

	sp->lastmx = mx;
	sp->lastmy = my;

	if (statuschanged && ob->type == FL_MULTI_BROWSER)
	{
	    statuschanged = 0;
	    return 1;
	}
	break;
    case FL_KEYBOARD:
	return handle_keyboard(ob, key, xev);
    case FL_RELEASE:
	sp->lastmy = -1;
	if (ob->type == FL_SELECT_BROWSER)
	{
	    sp->drawtype = SELECTION;
	    sp->desel_mark = sp->selectline;
	    fl_deselect_textbox(ob);
	}
	return statuschanged;
    case FL_DBLCLICK:
	if (sp->callback)
	    sp->callback(ob, sp->callback_data);
	return 0;
    case FL_LEAVE:
    case FL_ENTER:
    case FL_MOTION:
	break;
    case FL_FREEMEM:
	free_spec(ob->spec);
	fl_free(sp);
	ob->spec = 0;
	break;
    case FL_OTHER:
	if (((XEvent *) xev)->type == GraphicsExpose)
	{
	    XGraphicsExposeEvent *xge = xev;
	    if (xge->count == 0 && xge->drawable == FL_ObjWin(ob))
	    {
		sp->drawtype = FULL;
		fl_redraw_object(ob);
		M_warn("Browser", "GraphicsExposeRedraw");
	    }
	}
	break;
    }
    return 0;
}

/*-----------------------------------------------*/

/* Creates a browser */
FL_OBJECT *
fl_create_textbox(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		  const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;
    int junk;

    ob = fl_make_object(FL_TEXTBOX, type, x, y, w, h, label, handle_textbox);
    ob->boxtype = FL_TEXTBOX_BOXTYPE;
    ob->lcol = FL_TEXTBOX_LCOL;
    ob->align = FL_TEXTBOX_ALIGN;
    ob->col1 = FL_TEXTBOX_COL1;
    ob->col2 = FL_TEXTBOX_COL2;
    ob->wantkey = FL_KEY_SPECIAL;

    ob->spec = fl_calloc(1, sizeof(SPEC));

    sp = ((SPEC *) (ob->spec));

    sp->fontsize = fl_cntl.browserFontSize ?
	fl_cntl.browserFontSize : FL_TEXTBOX_FONTSIZE;
    sp->fontstyle = FL_NORMAL_STYLE;

    sp->charheight = fl_get_char_height(sp->fontstyle, sp->fontsize,
					&junk, &sp->chardesc);
    calc_textarea(ob);
    sp->lines = 0;
    sp->topline = 1;
    sp->specialkey = '@';
    sp->avail_lines = 0;


    fl_set_object_dblbuffer(ob, 1);
    extend_textbox(ob);

    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_textbox(int type, FL_Coord x, FL_Coord y,
	       FL_Coord w, FL_Coord h, const char *label)
{
    FL_OBJECT *ob;
    ob = fl_create_textbox(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}

/*-----------------------------------------------*/

/* Sets the topline for the textbox. */
int
fl_set_textbox_topline(FL_OBJECT * ob, int line)
{
    SPEC *sp = ((SPEC *) (ob->spec));

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_TEXTBOX))
    {
	Bark("SetBRTopLine", "%s not a browser", ob ? ob->label : "");
	return sp->topline;
    }
#endif

    if (line < 1)
	line = 1;

    if (line > sp->lines)
	line = sp->lines;

    if (line == sp->topline)
	return sp->topline;

    sp->drawtype |= VSLIDER;
    sp->oldtopline = sp->topline;
    sp->topline = line;
    fl_redraw_object(ob);
    sp->drawtype = COMPLETE;
    return sp->topline;
}

/*-----------------------------------------------*/

void
fl_clear_textbox(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int i;

    /* if already empty, do nothing */
    if (sp->lines == 0 && sp->attrib == 0)
	return;

    sp->lines = 0;
    sp->topline = 1;
    sp->selectline = 0;
    sp->attrib = 1;
    sp->maxpixels = sp->xoffset = 0;

    for (i = 0; i < sp->avail_lines; i++)
    {
	if (sp->text[i])
	{
	    fl_free(sp->text[i]->txt);
	    fl_free(sp->text[i]);
	}
	sp->text[i] = 0;
    }

    fl_redraw_object(ob);
}

/* Adds a line of text to a browser */
void
fl_add_textbox_line(FL_OBJECT * ob, const char *newtext)
{
    insert_lines(ob, ((SPEC *) (ob->spec))->lines + 1, newtext);
    fl_redraw_object(ob);
}

/* Adds a line of text to a browser, and changes focus */
void
fl_addto_textbox(FL_OBJECT * ob, const char *newtext)
{
    SPEC *sp = ob->spec;

    sp->drawtype = sp->lines < sp->screenlines ? FULL : VSLIDER;
    insert_lines(ob, sp->lines + 1, newtext);
    sp->topline = sp->lines;
    fl_redraw_object(ob);
    sp->drawtype = COMPLETE;
}

/* no newline is implied */
void
fl_addto_textbox_chars(FL_OBJECT * ob, const char *str)
{
    SPEC *sp = ob->spec;
    char *s, *p, *news;
    LINE *cur_line;

    if (!str)
	return;

    if (sp->lines == 0)
	sp->lines = 1;

    s = fl_strdup(str);
    if ((p = strchr(s, '\n')))
	*p = '\0';

    /* Create new line if required */
    if (sp->text[sp->lines] == 0)
	sp->text[sp->lines] = fl_calloc(1, sizeof(LINE));

    cur_line = sp->text[sp->lines];
    cur_line->len += strlen(s);
    news = fl_malloc(cur_line->len + 1);
    strcpy(news, cur_line->txt ? cur_line->txt : "");
    strcat(news, s);
    replace_line(ob, sp->lines, news);

    if (p)
    {
	if (*++p)
	    insert_lines(ob, sp->lines + 1, p);
	else
	    insert_line(ob, sp->lines + 1, "");
	sp->topline = sp->lines;
    }

    fl_redraw_object(ob);
    fl_free(news);
    fl_free(s);
}

/* Inserts a line of text in a browser */
void
fl_insert_textbox_line(FL_OBJECT * ob, int linenumb, const char *newtext)
{
    if (linenumb > ((SPEC *) (ob->spec))->lines ||
	((SPEC *) (ob->spec))->lines == 0)
    {
	fl_add_textbox_line(ob, newtext);
	return;
    }

    if (linenumb < 1)
	return;

    insert_line(ob, linenumb, newtext);
    fl_redraw_object(ob);
}

/* Deletes a line from a browser. */
void
fl_delete_textbox_line(FL_OBJECT * ob, int linenumb)
{
    if (linenumb < 1 || linenumb > ((SPEC *) (ob->spec))->lines)
	return;
    delete_line(ob, linenumb);
    fl_redraw_object(ob);
}

/* Replaces a line of text in a browser */
void
fl_replace_textbox_line(FL_OBJECT * ob, int linenumb, const char *newtext)
{
    if (linenumb < 1 || linenumb > ((SPEC *) (ob->spec))->lines)
	return;
    replace_line(ob, linenumb, newtext);
    fl_redraw_object(ob);
}

/* Returns a pointer to a particular line in the browser. */
const char *
fl_get_textbox_line(FL_OBJECT * ob, int linenumb)
{

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_TEXTBOX))
    {
	Bark("GetBRLine", "%s not a browser", ob ? ob->label : "");
	return 0;
    }
#endif

    if (linenumb < 1 || linenumb > ((SPEC *) (ob->spec))->lines)
	return 0;
    return ((SPEC *) (ob->spec))->text[linenumb]->txt;
}

/* Sets the browser to a particular file */
int
fl_load_textbox(FL_OBJECT * ob, const char *filename)
{
    SPEC *sp;
    FILE *fl;
    char *newtext;
    int c, i;

    if (ob == 0 || ob->objclass != FL_TEXTBOX)
	return 0;

    sp = ((SPEC *) (ob->spec));

    fl_clear_textbox(ob);

    if (!filename || !*filename)
    {
	fl_redraw_object(ob);
	return 1;
    }

    /* LOAD THE FILE */
    if (!(fl = fopen(filename, "r")))
	return 0;

    newtext = fl_malloc(sizeof(*newtext) * maxlen);

    i = 0;
    do
    {
	c = getc(fl);
	if (c == NL || c == EOF)
	{
	    newtext[i] = 0;
	    if (c != EOF || i != 0)
		insert_line(ob, sp->lines + 1, newtext);
	    i = 0;
	}
	else if (i < maxlen - 1)
	    newtext[i++] = c;
    }

    while (c != EOF && !ferror(fl));
    fclose(fl);
    sp->drawtype = COMPLETE;
    fl_redraw_object(ob);
    fl_free(newtext);
    return 1;
}

/* Selects a line in the browser. */
void
fl_select_textbox_line(FL_OBJECT * ob, int line, int slide)
{
    SPEC *sp = ((SPEC *) (ob->spec));

    if (line < 1 || line > sp->lines)
	return;

    if (sp->text[line]->non_selectable)
    {
	if (!slide)
	    return;

	if (sp->selectline < line)
        {
	    while (sp->lines >= line && sp->text[line]->non_selectable)
               line++;
        }
	else
        {
            while (line >= 1 && sp->text[line]->non_selectable)
	      line--;
        }
    }

    if (line < 1 || line > sp->lines)
	return;

    if (ob->type != FL_MULTI_BROWSER && sp->selectline > 0)
    {
	sp->text[sp->selectline]->selected = 0;
	sp->drawtype |= SELECTION;
	sp->desel_mark = sp->selectline;
    }

    sp->text[line]->selected = 1;
    sp->selectline = line;

    fl_redraw_object(ob);
    sp->drawtype = COMPLETE;
}

/* Deselects a line in the browser. */
void
fl_deselect_textbox_line(FL_OBJECT * ob, int line)
{
    SPEC *sp = ((SPEC *) (ob->spec));

    if (line < 1 || line > sp->lines)
	return;

    sp->text[line]->selected = 0;
    if (ob->type != FL_MULTI_BROWSER && sp->selectline == line)
    {
	sp->desel_mark = line;
	sp->selectline = 0;
    }
    else if (ob->type == FL_MULTI_BROWSER)
    {
	sp->desel_mark = line;
	sp->selectline = -line;
    }

    lastselect = 0;
    sp->drawtype |= SELECTION;
    fl_redraw_object(ob);
    sp->drawtype = COMPLETE;
}

/* Deselects all lines in the browser. */
void
fl_deselect_textbox(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int i;

    for (i = 1; i <= sp->lines; i++)
	sp->text[i]->selected = 0;

    sp->drawtype = FULL;
    if (ob->type != FL_MULTI_BROWSER)
    {
	sp->desel_mark = sp->selectline;
	sp->drawtype = SELECTION;
    }

    if (ob->type != FL_SELECT_BROWSER)
	sp->selectline = 0;

    fl_redraw_object(ob);
    sp->drawtype = COMPLETE;
}

/* Returns whether a line in the browser is selected. */
int
fl_isselected_textbox_line(FL_OBJECT * ob, int line)
{
    if (line < 1 || line > ((SPEC *) (ob->spec))->lines)
	return 0;
    return ((SPEC *) (ob->spec))->text[line]->selected;
}

/* Returns the last selection in the browser. */
int
fl_get_textbox(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_TEXTBOX))
    {
	Bark("GetTB", "%s not a textbox", ob ? ob->label : "");
	return 0;
    }
#endif
    return sp->selectline;
}

/*-----------------------------------------------*/

/* Sets the font size inside the browser. */
void
fl_set_textbox_fontsize(FL_OBJECT * ob, int size)
{
    SPEC *sp;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_TEXTBOX))
    {
	Bark("SetBRFont", "%s not a browser", ob ? ob->label : "");
	return;
    }
#endif
    sp = ob->spec;
    if (sp->fontsize != size)
    {
	sp->fontsize = size;
	sp->attrib = 1;
	find_longest_line(ob, 1);
	fl_redraw_object(ob);
    }
}

/* Sets the font style inside the browser. */
void
fl_set_textbox_fontstyle(FL_OBJECT * ob, int style)
{
    if (((SPEC *) (ob->spec))->fontstyle != style)
    {
	((SPEC *) (ob->spec))->fontstyle = style;
	((SPEC *) (ob->spec))->attrib = 1;
	find_longest_line(ob, 1);
	fl_redraw_object(ob);
    }
}

void
fl_set_textbox_line_selectable(FL_OBJECT * ob, int linenumb, int flag)
{
    SPEC *sp;

    if (linenumb < 1 || linenumb > ((SPEC *) (ob->spec))->lines ||
	ob->type == FL_NORMAL_BROWSER)
	return;

    sp = ob->spec;
    if (sp->text[linenumb]->non_selectable == flag)
    {
	sp->text[linenumb]->non_selectable = !flag;
	fl_redraw_object(ob);
    }
}

void
fl_get_textbox_dimension(FL_OBJECT * ob, FL_Coord * x, FL_Coord * y,
			 FL_Coord * w, FL_Coord * h)
{
    SPEC *sp;
    int junk;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_TEXTBOX))
    {
	Bark("GetBrowserSize", "%s not a browser", ob ? ob->label : "");
	return;
    }
#endif
    sp = ob->spec;
    sp->charheight = fl_get_char_height(sp->fontstyle, sp->fontsize,
					&junk, &sp->chardesc);
    calc_textarea(ob);

    *x = sp->x;
    *y = sp->y;
    *w = sp->w;
    *h = sp->h;
}

FL_Coord
fl_get_textbox_xoffset(FL_OBJECT * ob)
{
    return ((SPEC *) ob->spec)->xoffset;
}

int
fl_set_textbox_xoffset(FL_OBJECT * ob, FL_Coord npixels)
{
    SPEC *sp = ob->spec;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_TEXTBOX))
    {
	Bark("SetBRxoffset", "%s not a textbox", ob ? ob->label : "");
	return sp->xoffset;
    }
#endif

    if (npixels > (sp->maxpixels - sp->w + 5))
	npixels = sp->maxpixels - sp->w + 5;

    if (npixels < 0)
	return sp->xoffset;

    if (sp->xoffset != npixels)
    {
	sp->drawtype = FULL;
	sp->xoffset = npixels;
	fl_redraw_object(ob);
	sp->drawtype = COMPLETE;
    }

    return sp->xoffset;
}

void
fl_set_textbox_dblclick_callback(FL_OBJECT * ob, FL_CALLBACKPTR cb, long a)
{
    SPEC *sp = ob->spec;

    sp->callback = cb;
    sp->callback_data = a;
    fl_set_object_dblclick(ob, cb ? FL_CLICK_TIMEOUT : 0);
}

int
fl_get_textbox_longestline(FL_OBJECT * ob)
{
    return ((SPEC *) ob->spec)->maxpixels + 8;
}

/* probably should merge this into fl_get_string_width proper */
static int
textwidth(SPEC *sp, int style, int size, const char *str, int len)
{
    int lcol;

    for (; len >= 2 && *str && *str == sp->specialkey; str += 2, len -= 2)
    {
	if (*(str + 1) == sp->specialkey)
	{
	    str++;
	    len--;
	    break;
	}

	switch (str[1])
	{
	case 'l':
	    size = FL_LARGE_FONT;
	    break;
	case 'L':
	    size += 6;
	    break;
	case 'm':
	    size = FL_MEDIUM_FONT;
	    break;
	case 'M':
	    size += 4;
	    break;
	case 's':
	    size = FL_SMALL_FONT;
	    break;
	case 'S':
	    size -= 2;
	    break;
	case 'b':
	    style += FL_BOLD_STYLE;
	    break;
	case 'i':
	    style += FL_ITALIC_STYLE;
	    break;
	case 'f':
	    style = FL_FIXED_STYLE;
	    break;
	case 'n':
	    style = FL_NORMAL_STYLE;
	    break;
	case 't':
	    style = FL_TIMES_STYLE;
	    break;
	case 'C':		/* done need this, but have to advance the
				   str ptr */
	    /* requesting color changes. */
	    for (lcol = 0; str[2] >= '0' && str[2] <= '9'; str++, len--)
		lcol = 10 * lcol + str[2] - '0';
	    break;

	}
    }

    return fl_get_string_widthTAB(style, size, str, len);
}

/* mouse wheel hack */
int
fl_handle_mouse_wheel(FL_OBJECT * ob, int *ev, int *key, void *xev)
{
    if (*ev == FL_PUSH && *key > FL_MBUTTON3)
	return 0;

    if (*ev == FL_RELEASE && (*key > FL_MBUTTON3))
    {
	*ev = FL_KEYBOARD;
	if (xev && shiftkey_down((((XButtonEvent *) xev)->state)))
	{
	    ((XButtonEvent *) xev)->state &= ~ShiftMask;
	    ((XKeyEvent *) xev)->state &= ~ShiftMask;
	    *key = (*key == 4 ? FL_1LINE_UP : FL_1LINE_DOWN);
	}
	else if (xev && controlkey_down((((XButtonEvent *) xev)->state)))
	{
	    ((XButtonEvent *) xev)->state &= ~ControlMask;
	    ((XKeyEvent *) xev)->state &= ~ControlMask;
	    *key = (*key == 4 ? XK_Prior : XK_Next);
	}
	else
	    *key = (*key == 4 ? FL_HALFPAGE_UP : FL_HALFPAGE_DOWN);
    }

    return 1;
}
