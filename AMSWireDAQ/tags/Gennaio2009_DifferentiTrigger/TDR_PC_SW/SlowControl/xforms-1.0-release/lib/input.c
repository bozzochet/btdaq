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
 * $Id: input.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *  XForms Class FL_INPUT
 *     handle normal user inputs and exchange data with other
 *     applications via the X Selection machnism.
 *
 *  Data structure is grossly wrong and very inefficient.
 *  Need to complete overhaul this someday.
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_inp = "$Id: input.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include "forms.h"

#define H_PAD  (sp->charh)	/* how much to scroll each time */

#define FL_VALIDATE    FL_INPUTVALIDATOR
#define NL   '\n'

enum
{
    COMPLETE,
    PARTIAL,
    VSLIDER,
    HSLIDER = 4
};


/* Extra information need for input boxes. */
typedef struct
{
    char *str;			/* the input text                  */
    FL_COLOR textcol;		/* text color                      */
    FL_COLOR curscol;		/* cursor color                    */
    int position;		/* cursor position (in chars)      */
    int beginrange;		/* start of the range              */
    int endrange;		/* end of the range                */
    int size;			/* size of the string              */
    int changed;		/* whether the field has changed   */
    int how_return;		/* whether to return value always  */
    int drawtype;		/* if to draw text with background */
    int noscroll;		/* true if no scrollis allowd      */
    int maxchars;		/* limit for normal_input          */
    int attrib1;
    int attrib2;
    FL_VALIDATE validate;

    /* scroll stuff. */
    FL_OBJECT *dummy;		/* only for the size of composite */
    FL_OBJECT *hscroll;
    FL_OBJECT *vscroll;
    FL_OBJECT *input;
    int xoffset;
    int yoffset;
    int screenlines;
    int topline;
    int lines;			/* total number of lines in the field   */
    int xpos, ypos;		/* current cursor position in char,line */
    int cur_pixels;		/* current line length in pixels        */
    int max_pixels;		/* max length of all lines              */
    int max_pixels_line;
    int charh;			/* character height                     */
    int h, w;			/* text area                              */
    float hsize, vsize;
    float hval, vval;
    float hinc1, hinc2;
    float vinc1, vinc2;
    int h_pref, v_pref;		/* scrollbar preference                 */
    int vw, vw_def;
    int hh, hh_def;
    int h_on, v_on;
    int dead_area, attrib;
    int no_cursor;
    int field_char;
}
SPEC;

static void correct_topline(SPEC *, int *);
static void redraw_scrollbar(FL_OBJECT * ob);
static int make_line_visible(FL_OBJECT * ob, int);
static int make_char_visible(FL_OBJECT * ob, int);
static void copy_attributes(FL_OBJECT *, FL_OBJECT *);
static void make_cursor_visible(FL_OBJECT *, SPEC *, int, int);

enum
{
    NORMAL_SELECT, WORD_SELECT, LINE_SELECT
};


#define GetMargin(btype,bw, xm, ym)                              \
    if(btype == FL_FLAT_BOX || btype == FL_NO_BOX || \
       btype == FL_FRAME_BOX || btype == FL_EMBOSSED_BOX)        \
    { *xm = bw + 1; *ym = (0.7f * bw) + 1; }                      \
    else                                                         \
    { *xm = 2 * bw + (bw==1); *ym =  bw + 1 + (bw==1); }


/*
 * check the size of scrollbars and input field.  No drawing is allowed
 */
static void
check_scrollbar_size(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int xmargin, ymargin, bw = FL_abs(ob->bw);
    int delta;
    int h_on = sp->h_on, v_on = sp->v_on;
    int max_pixels = sp->max_pixels + H_PAD;

    if (sp->input->type != FL_MULTILINE_INPUT)
	return;

    /* compute the real input box size */
    sp->input->x = sp->dummy->x;
    sp->input->y = sp->dummy->y;

    GetMargin(sp->input->boxtype, bw, &xmargin, &ymargin);
    sp->charh = fl_get_char_height(sp->input->lstyle, sp->input->lsize, 0, 0);

    /* see how many (potential) lines we can have */
    sp->screenlines = (sp->dummy->h - 2.0 * ymargin) / sp->charh + 0.001f;

    sp->v_on = sp->v_pref == FL_ON ||
	(sp->screenlines && sp->screenlines < sp->lines &&
	 sp->v_pref != FL_OFF);

    if (sp->v_on)
    {
	sp->vw = sp->vw_def;
	sp->vscroll->x = sp->input->x + sp->dummy->w - sp->vw;
	sp->vscroll->y = sp->input->y;
	sp->vscroll->w = sp->vw;
	sp->vscroll->visible = 1;
    }
    else
    {
	sp->vw = 0;
	sp->vscroll->visible = 0;
    }

    sp->input->w = sp->dummy->w - sp->vw;

    sp->h_on = (max_pixels > sp->w && sp->h_pref != FL_OFF) ||
	sp->h_pref == FL_ON;

    if (sp->h_on)
    {
	sp->h_on = 1;
	sp->hh = sp->hh_def;
	sp->hscroll->x = sp->input->x;
	sp->hscroll->y = sp->input->y + sp->dummy->h - sp->hh;
	sp->hscroll->h = sp->hh;
	sp->hscroll->visible = 1;

	if ((delta = max_pixels - sp->w) > 0)
	{
	    sp->hsize = (float) sp->w / max_pixels;
	    sp->hval = (float) sp->xoffset / delta;
	    sp->hinc1 = 8.0f * sp->charh / delta;
	    sp->hinc2 = (sp->charh - 2.0f) / delta;
	}
	else
	    sp->hsize = 1.0f;
    }
    else
    {
	sp->hh = 0;
	sp->hscroll->visible = 0;
    }

    sp->input->h = sp->dummy->h - sp->hh;
    sp->h = sp->input->h - 2 * ymargin;

    sp->screenlines = (float) sp->h / sp->charh + 0.001f;

    if (!sp->v_on && sp->screenlines < sp->lines && sp->h_pref != FL_OFF)
    {
	sp->v_on = 1;
	sp->vw = sp->vw_def;
	sp->vscroll->x = sp->input->x + sp->dummy->w - sp->vw;
	sp->vscroll->y = sp->input->y;
	sp->vscroll->visible = 1;
	sp->input->w = sp->dummy->w - sp->vw;
    }

    if (sp->v_on && (delta = sp->lines - sp->screenlines) > 0)
    {
	sp->vval = (sp->topline - 1.0f) / delta;
	sp->vsize = (float) sp->screenlines / sp->lines;
	sp->vinc1 = (sp->screenlines - 0.99f) / delta;
	sp->vinc2 = 1.01f / delta;
    }
    else
	sp->vsize = 1.0f;

    sp->hscroll->w = sp->input->w;
    sp->vscroll->h = sp->input->h;
    sp->w = sp->input->w - 2 * xmargin;

    if ((h_on != sp->h_on) || v_on != sp->v_on)
    {
	sp->attrib = 1;
	sp->dead_area = !(sp->h_on ^ sp->v_on);
    }
}


static void
draw_input(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    FL_COLOR col, textcol;
    FL_COLOR curscol = fl_dithered(fl_vmode) ? FL_BLACK : sp->curscol;
    int valign;
    FL_Coord xmargin, ymargin;
    FL_Coord bw = FL_abs(ob->bw);
    int cx, cy;
    int max_pixels, max_pixels_line;
    static char sbuf[128], *saved;

    GetMargin(ob->boxtype, bw, &xmargin, &ymargin);
    sp->w = sp->input->w - 2 * xmargin;
    sp->h = sp->input->h - 2 * ymargin;

    col = ob->focus ? ob->col2 : ob->col1;

    /* partial means only text has changed. Internally DrawImage string will
       be used instead of normal string. --Unimplemented yet */

    sp->drawtype = COMPLETE;

    if (sp->drawtype == COMPLETE)
    {
	fl_drw_box(ob->boxtype, sp->input->x, sp->input->y,
		   sp->input->w, sp->input->h, col, ob->bw);
	fl_draw_object_label_outside(ob);
    }
#if 0
    else
	fl_bk_textcolor(col);
#endif

    valign = (ob->type == FL_MULTILINE_INPUT) ? FL_ALIGN_TOP : FL_ALIGN_CENTER;

    sp->charh = fl_get_char_height(ob->lstyle, ob->lsize, 0, 0);
    textcol = sp->textcol;

    if (ob->type == FL_SECRET_INPUT)
    {
	int len = strlen(sp->str);
	saved = sp->str;
	memset(sbuf, sp->field_char, len);
	sbuf[len] = '\0';
	sp->str = fl_strdup(sbuf);
    }

    cx = sp->input->x + xmargin;
    cy = sp->input->y + ymargin;

    fl_set_text_clipping(cx, cy, sp->w, sp->h);

    max_pixels = fl_drw_string(FL_ALIGN_LEFT,
			       valign,	/* Align left centered. */
			       cx - sp->xoffset,	/* Bounding box */
			       cy - sp->yoffset,
			       sp->w + sp->xoffset,
			       sp->h + sp->yoffset,
			       -1,	/* Do clipping */
			       col, textcol, curscol,
			       ob->lstyle, ob->lsize,	/* Size and style */
                               /* Cursor position */
			       sp->no_cursor ? -1 : sp->position,	
			       sp->beginrange,	/* selection begin */
			       sp->endrange,	/* Selection range */
			       sp->str,
			       sp->drawtype != COMPLETE,
			       sp->topline,
			       sp->topline + sp->screenlines, 0);

    max_pixels_line = fl_get_maxpixel_line() + 1;

    if (max_pixels > sp->max_pixels ||
	(sp->max_pixels_line >= sp->topline &&
	 sp->max_pixels_line <= sp->topline + sp->screenlines))
    {
	sp->max_pixels = max_pixels;
	sp->max_pixels_line = max_pixels_line;
    }

    fl_unset_text_clipping();

    if (ob->type == FL_SECRET_INPUT)
    {
        fl_free(sp->str);
	sp->str = saved;
    }

    sp->drawtype = COMPLETE;
}

#define DELIM(c)  (c==' ' || c==',' || c=='.' || c=='\n')

/* figures out selection region of mouse. Returns whether anything changed */
static int
handle_select(FL_Coord mx, FL_Coord my, FL_OBJECT * ob, int mouse, int what)
{
    SPEC *sp = ob->spec;
    int thepos, valign;
    int oldpos = sp->position, oldbeg = sp->beginrange, oldend = sp->endrange;
    int bw = FL_abs(ob->bw);
    FL_Coord xmargin, ymargin;

    if (ob->type == FL_HIDDEN_INPUT)
	return 0;

    /* Compute the mouse position in the string */
    valign = (ob->type == FL_MULTILINE_INPUT) ? FL_ALIGN_TOP : FL_ALIGN_CENTER;
    GetMargin(ob->boxtype, bw, &xmargin, &ymargin);

    thepos = fl_get_pos_in_string(FL_ALIGN_LEFT, valign,
				  (sp->input->x + xmargin) - sp->xoffset,
				  (sp->input->y + ymargin) - sp->yoffset,
				  sp->w + sp->xoffset,
				  sp->h + sp->yoffset,
				  ob->lstyle, ob->lsize,
				  mx, my, sp->str,
				  &sp->xpos, &sp->ypos);

    if (what == WORD_SELECT)
    {
	int n = thepos;

	if (sp->str[thepos] == ' ')
	    return 0;

	for (; sp->str[n] && !DELIM(sp->str[n]); n++)
	    ;
	sp->endrange = n;

	for (n = thepos; --n >= 0 && !DELIM(sp->str[n]);)
	    ;
	sp->beginrange = n + 1;

    }
    else if (what == LINE_SELECT)
    {
	int n = thepos;
	for (; sp->str[n] && sp->str[n] != '\n'; n++)
	    ;
	sp->endrange = n;
	for (n = thepos; --n >= 0 && sp->str[n] != '\n';)
	    ;
	sp->beginrange = n + 1;
    }
    else
    {
	/* Adapt the range */
	if (mouse)
	{
	    fl_freeze_form(ob->form);
	    make_line_visible(ob, sp->ypos);
	    make_char_visible(ob, sp->xpos);
	    fl_unfreeze_form(ob->form);

	    if (thepos < sp->position)
	    {
		sp->endrange = sp->position;
		sp->beginrange = thepos;
	    }
	    else
	    {
		sp->beginrange = sp->position;
		sp->endrange = thepos;
	    }

	}
	else
	{
	    sp->position = sp->beginrange = thepos;
	    sp->endrange = -1;
	}
    }

    if (sp->beginrange == sp->endrange)
	sp->endrange = -1;

    if (sp->beginrange < 0)
	sp->beginrange = 0;

    return (oldpos != sp->position || oldbeg != sp->beginrange ||
	    oldend != sp->endrange);
}

/*
 * this is not XCUTBUFFER. It is generatede by ^K and can be
 * recalled by ^Y
 */
#define MAXCBLEN   512
static char cutbuf[MAXCBLEN];

/* delete a single char. dir =1 for next, dir -1 for prev */
static void
delete_char(register SPEC *sp, register int dir, register int slen)
{
    register int i = sp->position - (dir < 0);

    if (sp->str[i] == NL)
    {
	sp->lines--;
	sp->ypos -= (dir < 0);
    }

    for (; i < slen; i++)
	sp->str[i] = sp->str[i + 1];
    sp->position -= (dir < 0);
}

/* Removes a piece of the string */
static void
delete_piece(FL_OBJECT * ob, register int start, register int end)
{
    register SPEC *sp = ob->spec;
    register int i = 0;

    do
    {
	i++;
	sp->str[start + i - 1] = sp->str[end + i];
    }
    while (sp->str[end + i] != '\0');

    sp->position = start;
    /* this can be expensive TODO */
    sp->lines = fl_get_input_numberoflines(ob);
    fl_get_input_cursorpos(ob, &sp->xpos, &sp->ypos);
}


/* Returns the width of the substring of the input field */
#if 1
#define get_substring_width(o,b,e)                               \
   fl_get_string_width(o->lstyle, o->lsize, ((SPEC *)o->spec)->str + b, e-b)

#else
static int
get_substring_width(FL_OBJECT * ob, int startpos, int endpos)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    char *str = sp->str + startpos;
    char tmpch = sp->str[endpos];	/* Save end position */
    int wid;			/* The required width */

    sp->str[endpos] = '\0';
    wid = fl_get_string_width(ob->lstyle, ob->lsize, str, endpos - startpos);
    sp->str[endpos] = tmpch;	/* Restore end position */

    return wid;
}

#endif

#define IsRegular(k)  (k==NL || (key >= 32 && key <= 255 && key != 127))

/* Editing command. Need 4 bytes. Byte1 for normal ASCII, byte2 for
 * special keysyms, such as PageUP etc. Byte 3 is used by Latin3 etc.
 * Byte 4 will be used to indicate modifiers.
 */

#define METAMASK   FL_ALT_VAL
static FL_EditKeymap kmap;

static int paste_it(FL_OBJECT *, const unsigned char *, int);
static void set_default_keymap(int);

#define set_to_eol(p)   while((p) < slen && sp->str[p] != NL) (p)++

static void
expand_tab(FL_OBJECT * ob)
{
    return;
}

/*
 * cursor moved. No editing.
 */
static int
handle_movement(FL_OBJECT * ob, int key, int slen, int startpos, int kmask)
{
    SPEC *sp = ob->spec;
    int ret = 0;
    int ready, wid, i, oldwid, tt;

    if (IsHome(key))
    {
	fl_set_input_topline(ob, 1);
	sp->position = 0;
	sp->xoffset = 0;
	sp->ypos = 1;
    }
    else if (IsPageDown(key))
	fl_set_input_topline(ob, sp->topline + sp->screenlines);
    else if (IsHalfPageDown(key))
	fl_set_input_topline(ob, sp->topline + sp->screenlines / 2);
    else if (Is1LineDown(key))
	fl_set_input_topline(ob, sp->topline + 1);
    else if (IsPageUp(key))
	fl_set_input_topline(ob, sp->topline - sp->screenlines);
    else if (IsHalfPageUp(key))
	fl_set_input_topline(ob, sp->topline - sp->screenlines / 2);
    else if (Is1LineUp(key))
	fl_set_input_topline(ob, sp->topline - 1);
    else if (key == '\t')
	expand_tab(ob);
    else if (key == 12)
	;
    else if (IsEnd(key))
    {
	fl_set_input_topline(ob, sp->lines);
	sp->position = slen;
	fl_get_input_cursorpos(ob, &sp->xpos, &sp->ypos);
    }
    else if (IsLeft(key))	/* Left key */
    {
	if (shiftkey_down(kmask))
	{
	    sp->position = startpos;
	    sp->xoffset = 0;
	}
	else if (sp->position > 0)
	    sp->position--;

	if (sp->str[sp->position] == '\n')
	{
	    sp->ypos--;
	    /* Compute starting position of current line */
	    startpos = sp->position;
	    while (startpos > 0 && sp->str[startpos - 1] != NL)
		startpos--;
	}

	make_cursor_visible(ob, sp, startpos, -1);

    }
    else if (IsRight(key) || key == kmap.moveto_eol)
    {
	if (shiftkey_down(kmask) || key == kmap.moveto_eol)
	    set_to_eol(sp->position);
	else if (sp->position < slen)
	{
	    if (sp->str[sp->position] == NL)
	    {
		sp->ypos++;
		sp->xoffset = 0;
		startpos = sp->position + 1;
	    }
	    sp->position++;
	}

	make_cursor_visible(ob, sp, startpos, 1);

    }
    else if (IsUp(key))		/* Up key */
    {
	if (startpos != 0)
	{
	    wid = get_substring_width(ob, startpos, sp->position);
	    i = startpos - 1;
	    while (i > 0 && sp->str[i - 1] != NL)
		i--;
	    oldwid = 0.0f;
	    sp->position = i;
	    ready = (sp->str[sp->position] == NL);
	    while (!ready)
	    {
		tt = get_substring_width(ob, i, sp->position + 1);
		ready = (oldwid + tt) / 2.0 >= wid;
		oldwid = tt;
		if (!ready)
		    sp->position++;
		if (sp->str[sp->position] == NL)
		    ready = 1;
	    }

	    if (--sp->ypos < 1)
		sp->ypos = 1;
	}
    }
    else if (IsDown(key))	/* Down key */
    {
	wid = get_substring_width(ob, startpos, sp->position);
	i = sp->position + 1;
	while (i < slen && sp->str[i - 1] != NL)
	    i++;
	if (i < slen)
	{
	    oldwid = 0.0f;
	    sp->position = i;
	    ready = (sp->position == slen || sp->str[sp->position] == NL);
	    while (!ready)
	    {
		tt = get_substring_width(ob, i, sp->position + 1);
		ready = (oldwid + tt) / 2.0 >= wid;
		oldwid = tt;
		if (!ready)
		    sp->position++;
		if (sp->position == slen || sp->str[sp->position] == NL)
		    ready = 1;
	    }
	}
	else
	{
	    sp->position = slen;
	    sp->xoffset = 0;
	}
	if (++sp->ypos > sp->lines)
	    sp->ypos = sp->lines;

    }
    else if (key == kmap.moveto_bol)
    {
	sp->position = startpos;
	sp->xoffset = 0;
    }
    else if (key == kmap.moveto_prev_word)
    {
	if (sp->position > 0)
	    sp->position--;
	while (sp->position > 0 && (sp->str[sp->position] == ' '
				    || sp->str[sp->position] == NL))
	{
	    if (sp->str[sp->position] == '\n')
		sp->ypos--;
	    sp->position--;
	}

	while (sp->position > 0 && sp->str[sp->position] != ' '
	       && sp->str[sp->position] != NL)
	    sp->position--;

	if (sp->position > 0)
	    sp->position++;

    }
    else if (key == kmap.moveto_next_word)
    {
	i = sp->position;
	while (i < slen && (sp->str[i] == ' ' || sp->str[i] == NL))
	{
	    if (sp->str[i] == NL)
		sp->ypos++;
	    i++;
	}

	while (i < slen && sp->str[i] != ' ' && sp->str[i] != NL)
	    i++;
	sp->position = i;
    }

    return ret;
}

/*
 * Editing.
 */
static int
handle_edit(FL_OBJECT * ob, int key, int slen, int startpos, int keymask)
{
    SPEC *sp = ob->spec;
    int ret = 1, i, j;
    int prev = 1;

    if (key == kmap.del_prev_char || key == kmap.backspace)
    {
	prev = -1;
	if (sp->endrange >= 0)
	    delete_piece(ob, sp->beginrange, sp->endrange - 1);
	else if (sp->position > 0)
	{
	    delete_char(sp, -1, slen);
	}
	else
	    ret = 0;
    }
    else if (key == kmap.del_next_char)
    {
	if (sp->endrange >= 0)
	    delete_piece(ob, sp->beginrange, sp->endrange - 1);
	else if (sp->position < slen)
	    delete_char(sp, 1, slen);
	else
	    ret = 0;
    }
    else if (key == kmap.del_next_word)
    {
	i = sp->position;
	while (i < slen && (sp->str[i] == ' ' || sp->str[i] == NL))
	    i++;
	while (i < slen && sp->str[i] != ' ' && sp->str[i] != NL)
	    i++;
	delete_piece(ob, sp->position, i - 1);
    }
    else if (key == kmap.del_prev_word)
    {
	prev = -1;
	j = sp->position;
	if (sp->position > 0)
	    sp->position--;
	while (sp->position > 0 && (sp->str[sp->position] == ' '
				    || sp->str[sp->position] == NL))
	    sp->position--;
	while (sp->position > 0 && sp->str[sp->position] != ' '
	       && sp->str[sp->position] != NL)
	    sp->position--;
	if (sp->position != j)
	    delete_piece(ob, sp->position, j - 1);
	else
	    ret = 0;
    }
    else if (key == kmap.clear_field)
    {
	prev = 0;
	sp->xoffset = 0;
	if (slen > 0)
	    delete_piece(ob, 0, slen - 1);
	else
	    ret = 0;
    }
    else if (key == kmap.del_to_eol)
    {
	if (slen > sp->position)
	{
	    if (sp->str[sp->position] != NL)
	    {
		for (i = sp->position; i < slen && sp->str[i] != NL; i++)
		    ;
	    }
	    else
		i = sp->position + 1;

	    /* save buffer */
	    if ((i - sp->position) > 1)
	    {
		strncpy(cutbuf, sp->str + sp->position,
			(i - sp->position));
		cutbuf[i - sp->position] = '\0';
	    }
	    delete_piece(ob, sp->position, i - 1);
	}
	else
	    ret = 0;
    }
    else if (key == kmap.paste)
    {
	paste_it(ob, (unsigned char *) cutbuf, strlen(cutbuf));
    }
    else if (key == kmap.transpose)
    {
	if (sp->position > 0)
	{
	    char t;
	    if (sp->position < slen && sp->str[sp->position] != '\n')
	    {
		t = sp->str[sp->position - 1];
		sp->str[sp->position - 1] = sp->str[sp->position];
		sp->str[sp->position] = t;
		sp->position++;
	    }
	    else
	    {
		t = sp->str[sp->position - 2];
		sp->str[sp->position - 2] = sp->str[sp->position - 1];
		sp->str[sp->position - 1] = t;
	    }
	}
    }

    make_cursor_visible(ob, sp, startpos, prev);

    return ret;
}

/* Handles a key press, returns whether something has changed */
static int
handle_key(FL_OBJECT * ob, int key, unsigned kmask)
{
    int i, ret = 1;
    SPEC *sp = ob->spec;
    int slen;			/* length of the string */
    int startpos;		/* position of start of current line */
    int oldy = sp->ypos;
    int oldl = sp->lines;
    int oldx = sp->xoffset;
    int oldmax = sp->max_pixels;

    /* Extend field size if required */
    slen = strlen(sp->str);
    if (sp->size == slen + 1)
    {
	sp->size += 64;
	sp->str = (char *) fl_realloc(sp->str, sp->size);
    }

    if (ob->type == FL_MULTILINE_INPUT && key == 13)
	key = NL;

    /* Compute starting position of current line */
    startpos = sp->position;
    while (startpos > 0 && sp->str[startpos - 1] != NL)
	startpos--;

    if (controlkey_down(kmask) && key > 255)
	key |= FL_CONTROL_MASK;

    if (metakey_down(kmask))
	key |= METAMASK;

    if (shiftkey_down(kmask))
    {
	if (key == XK_Up)
	    key = XK_Home;
	else if (key == XK_Down)
	    key = XK_End;
    }

    /* translate all move key to cursor keys so we can distinguish edit/move
       keys more easily */

    if (key == kmap.moveto_next_line)
	key = XK_Down;
    else if (key == kmap.moveto_prev_line)
	key = XK_Up;
    else if (key == kmap.moveto_prev_char)
	key = XK_Left;
    else if (key == kmap.moveto_next_char)
	key = XK_Right;
    else if (key == kmap.moveto_bof)
	key = XK_Home;
    else if (key == kmap.moveto_eof)
	key = XK_End;
    else if (key == kmap.moveto_next_page)
	key = XK_PageDn;
    else if (key == kmap.moveto_prev_page)
	key = XK_PageUp;

    if (IsRegular(key))		/* Normal keys  or NL */
    {
	int ok = FL_VALID;
	char *tmpbuf = 0;
	int tmppos = 0, tmpxoffset = 0;

	if (sp->endrange >= 0)
        {
	    delete_piece(ob, sp->beginrange, sp->endrange - 1);
            slen = strlen(sp->str);
        }

	if (sp->maxchars > 0 && slen >= sp->maxchars)
	{
	    ringbell();
	    return 0;
	}

	if (sp->validate)
	{
	    tmpbuf = fl_strdup(sp->str);
	    tmppos = sp->position;
	    tmpxoffset = sp->xoffset;
	}

	/* merge the new character */
	for (i = slen + 1; i > sp->position; i--)
	    sp->str[i] = sp->str[i - 1];
	sp->str[sp->position] = key;
	sp->position++;

	if (key == '\n')
	{
	    sp->lines++;
	    sp->ypos++;
	    sp->xoffset = 0;
	}
	else
	{
	    int tmp = get_substring_width(ob, startpos, sp->position);
	    if (tmp - sp->xoffset > sp->w)
		sp->xoffset = tmp - sp->w + H_PAD;
	}

	if (sp->validate)
	{
	    ok = sp->validate(ob, tmpbuf, sp->str, key);

	    if ((ok & ~FL_RINGBELL) != FL_VALID)
	    {
		strcpy(sp->str, tmpbuf);
		sp->position = tmppos;
		sp->xoffset = tmpxoffset;
		if (key == '\n')
		{
		    sp->lines--;
		    sp->ypos--;
		}
	    }

	    if ((ok & FL_RINGBELL))
		ringbell();
	    fl_free(tmpbuf);
	}
    }
    else if (IsCursorKey(key) || key == kmap.moveto_eol ||
	     key == kmap.moveto_bol || key == kmap.moveto_prev_word ||
	     key == kmap.moveto_next_word ||
	     Is1LineUp(key) || Is1LineDown(key) ||
	     IsHalfPageUp(key) || IsHalfPageDown(key))
    {
	ret = handle_movement(ob, key, slen, startpos, kmask);
    }
    else
    {
	ret = handle_edit(ob, key, slen, startpos, kmask);
    }

    sp->endrange = -1;

    if (ret)
    {
	int junk;
	fl_get_string_dimension(ob->lstyle, ob->lsize,
			  sp->str, strlen(sp->str), &sp->max_pixels, &junk);
    }

    if (sp->noscroll)
    {
	sp->xoffset = sp->yoffset = 0;
	sp->topline = sp->ypos = 1;
	oldmax = sp->max_pixels;
    }

    fl_freeze_form(ob->form);

    if (oldl != sp->lines || oldy != sp->ypos || oldx != sp->xoffset ||
	oldmax != sp->max_pixels)
    {
	check_scrollbar_size(ob);
	make_line_visible(ob, sp->ypos);
	redraw_scrollbar(ob);
    }

    fl_redraw_object(sp->input);
    fl_unfreeze_form(ob->form);

    return ret;
}

/*
 * Given nb bytes of stuff, paste it into the input field. For
 * NORMAL_INPUT, this is very inefficient. Need --FIX---
 */
static int
paste_it(FL_OBJECT * ob, register const unsigned char *thebytes, int nb)
{
    int status = 0;
    register const unsigned char *byte;
    register SPEC *sp = ob->spec;
    int slen, i;
    unsigned char *p;

    M_warn("Xpaste", "%d bytes: %s", nb, thebytes);

    /* for non-text input, we must check each individual characters */
    if (ob->type == FL_FLOAT_INPUT || ob->type == FL_INT_INPUT ||
	sp->maxchars > 0)
    {
	for (byte = thebytes; byte < (thebytes + nb); byte++)
	    status = handle_key(ob, *byte, 0) || status;
    }
    else
    {
	/* must not allow single line input field contain tab or newline */
	if (ob->type == FL_NORMAL_INPUT || ob->type == FL_SECRET_INPUT)
	{
	    if ((p = (unsigned char *) strchr((char *) thebytes, '\t')))
		nb = p - thebytes;
	    if ((p = (unsigned char *) strchr((char *) thebytes, '\n')) &&
		(p - thebytes) < nb)
		nb = p - thebytes;
	}

	slen = strlen(sp->str);
	if (sp->size < (slen + nb + 2))
	{
	    sp->size += 16 * (int) ((slen + nb + 17) / 16);
	    sp->str = fl_realloc(sp->str, sp->size);
	}

	/* shift text after sp->position */
	for (i = slen + nb; i >= sp->position + nb; --i)
	    sp->str[i] = sp->str[i - nb];

	/* insert new text */
	for (byte = thebytes; byte < (thebytes + nb); byte++)
	    sp->str[sp->position++] = *byte;

	sp->lines = fl_get_input_numberoflines(ob);
	fl_get_input_cursorpos(ob, &sp->xpos, &sp->ypos);
	fl_get_string_dimension(ob->lstyle, ob->lsize,
			      sp->str, slen + nb, &sp->max_pixels, &status);

	fl_freeze_form(ob->form);
	{
	    check_scrollbar_size(ob);
	    make_line_visible(ob, sp->ypos);
	    fl_redraw_object(sp->input);
	    redraw_scrollbar(ob);
	}
	fl_unfreeze_form(ob->form);
	status = 1;
    }

    return status;
}


/* Paste request is handled here. If we do not own the selection,
 * need to make a request and actual pasting might happen in
 * handle_special as we have to wait for the SelectionNotify event.
 * If we own the selection, just get it from cut buffer0, and we are
 * done.
 */

/* handle X cut & paste ******************************* */

static int
gotit_cb(FL_OBJECT * ob, long type, const void *buf, long nb)
{
    SPEC *sp = ob->spec;

    sp->changed = paste_it(ob, (unsigned char *) buf, nb) || sp->changed;
    fl_update_display(0);
    return 0;
}

static int
do_XPaste(FL_OBJECT * ob)
{
    return fl_request_clipboard(ob, XA_STRING, gotit_cb);
}


static int
lose_selection(FL_OBJECT * ob, long type)
{
    SPEC *sp = ob->spec;

    sp->beginrange = sp->endrange = -1;
    if (!ob->focus)
	sp->position = -1;
    else if (sp->position < 0)
	sp->position = strlen(sp->str ? sp->str : "");
    fl_redraw_object(sp->input);
    fl_update_display(0);
    return 0;
}

static void
do_XCut(FL_OBJECT * ob, int beginrange, int endrange)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    char *buff;
    int i = 0, nc;

    if ((nc = endrange - beginrange + 1) <= 0)
	return;

    buff = fl_malloc(nc + 1);

    for (i = beginrange, nc = 0; i <= endrange && sp->str[i]; i++)
	buff[nc++] = sp->str[i];
    buff[nc] = '\0';

    fl_stuff_clipboard(ob, XA_STRING, buff, nc, lose_selection);

    fl_free(buff);

}

/* Handles an event */
static int
handle_it(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my, int key, void *ev)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    static int motion, lx, ly, paste;
    int ret = 0, val;

    if (fl_handle_mouse_wheel(ob, &event, &key, ev) == 0)
	return 0;

    switch (event)
    {
    case FL_ATTRIB:
	check_scrollbar_size(ob);
    case FL_DRAW:
	/* we always force label outside */
	if (sp->input->type != FL_MULTILINE_INPUT)
	{
	    if (sp->dummy != ob)	/* this can only happen with fdesign */
		sp->dummy = sp->input = ob;
	}
	sp->dummy->align &= ~FL_ALIGN_INSIDE;
	copy_attributes(sp->dummy, sp->input);
	if (event == FL_DRAW && sp->input->type != FL_HIDDEN_INPUT)
	    draw_input(sp->input);
/*      break; */
    case FL_DRAWLABEL:
	if (sp->input->type != FL_MULTILINE_INPUT)
	    fl_draw_object_label_outside(sp->input);
	else
	    fl_drw_text_beside(sp->dummy->align, sp->input->x, sp->input->y,
			       sp->input->w + sp->vw, sp->input->h + sp->hh,
			       sp->input->lcol, sp->input->lstyle,
			       sp->input->lsize, sp->dummy->label);
	break;
    case FL_FOCUS:
	if (ob->type == FL_MULTILINE_INPUT)
	    sp->dummy->focus = 1;
        if(sp->str)
          sp->position = strlen(sp->str);
        else
          sp->position = 0;
	sp->changed = 0;
	fl_redraw_object(sp->input);
	break;
    case FL_UNFOCUS:
        if(sp)
        {
	   if (ob->type == FL_MULTILINE_INPUT)
	       sp->dummy->focus = 0;
           sp->position = -1;
           sp->endrange = -1;
	   fl_redraw_object(sp->input);
	   if (sp->how_return == FL_RETURN_END ||
	      sp->how_return == FL_RETURN_ALWAYS)
	      ret = 1;
	   else
	      ret = sp->changed;
        }
	break;
    case FL_MOUSE:
	motion = (mx != lx || my != ly) && !paste;
	if (motion && handle_select(mx, my, ob, 1, NORMAL_SELECT))
	    fl_redraw_object(sp->input);
	break;
    case FL_RELEASE:
	if (key == 1 && motion)
	    do_XCut(ob, sp->beginrange, sp->endrange - 1);
	motion = 0;
	break;
    case FL_PUSH:
	paste = 0;
	lx = mx;
	ly = my;
	if (key == 2 && (sp->changed = do_XPaste(ob)))
	{
	    if (sp->how_return == FL_RETURN_CHANGED)
		sp->changed = 0;
	    ret = (sp->how_return == FL_RETURN_ALWAYS ||
		   sp->how_return == FL_RETURN_CHANGED);
	    paste = 1;
	}
	else if (handle_select(mx, my, ob, 0, NORMAL_SELECT))
	    fl_redraw_object(sp->input);
	break;
    case FL_DBLCLICK:
    case FL_TRPLCLICK:
	if (handle_select(mx, my, ob, 0,
			  event == FL_DBLCLICK ? WORD_SELECT : LINE_SELECT))
	{
	    fl_redraw_object(sp->input);
	    do_XCut(ob, sp->beginrange, sp->endrange);
	}
	break;
    case FL_KEYBOARD:
	if (handle_key(ob, key, ((XKeyEvent *) ev)->state))
	{
	    sp->changed = 1;
	    if (sp->how_return == FL_RETURN_CHANGED)
		sp->changed = 0;
	    ret = (sp->how_return == FL_RETURN_ALWAYS ||
		   sp->how_return == FL_RETURN_CHANGED);
	}
	break;
    case FL_FREEMEM:
	fl_free(((SPEC *) (ob->spec))->str);
	fl_free(ob->spec);
        ob->spec = 0;
	break;
    case FL_OTHER:
	M_warn("input", "handle sepcial");
	break;
    }

    if (sp && ret && sp->validate &&
	(val = sp->validate(ob, sp->str, sp->str, 0)) != FL_VALID)
    {
	ret = 0;
	if (val & FL_RINGBELL)
	{
	    ringbell();
	    fl_reset_focus_object(ob);
	}
    }
    return ret;
}

/*------------------------------*/

static void
vsl_cb(FL_OBJECT * ob, long data)
{
    SPEC *sp = ob->parent->spec;
    float val = fl_get_scrollbar_value(ob);
    int top = val * (sp->lines - sp->screenlines) + 1.01;

    sp->drawtype = VSLIDER;
    fl_set_input_topline(sp->input, top);
}

static void
hsl_cb(FL_OBJECT * ob, long data)
{
    SPEC *sp = ob->parent->spec;
    float val = fl_get_scrollbar_value(ob);
    int xoff = val * (sp->max_pixels - sp->w) + 0.1f;

    sp->drawtype = HSLIDER;
    fl_set_input_xoffset(sp->input, xoff);
}

/* pre- and post- handlers */
static int
input_pre(FL_OBJECT * ob, int ev, FL_Coord mx, FL_Coord my,
	  int key, void *xev)
{
    FL_OBJECT *ext = ob->parent;
    return (ext && ext->prehandle) ? ext->prehandle(ext, ev, mx, my, key, xev) : 0;
}

static int
input_post(FL_OBJECT * ob, int ev, FL_Coord mx, FL_Coord my,
	   int key, void *xev)
{
    FL_OBJECT *ext = ob->parent;
    return (ext && ext->posthandle) ? ext->posthandle(ext, ev, mx, my, key, xev) : 0;
}

static void
input_cb(FL_OBJECT * ob, long data)
{
    SPEC *sp = ob->spec;

    if (ob != sp->dummy)
    {
	if (sp->dummy->object_callback)
	    fl_call_object_callback(sp->dummy);
	else
	    fl_object_qenter(sp->dummy);
    }
}

static int date_validator(FL_OBJECT *, const char *, const char *, int);
static int float_int_validator(FL_OBJECT *, const char *, const char *, int);


/* creates an object */
FL_OBJECT *
fl_create_input(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;

    set_default_keymap(0);

    ob = fl_make_object(FL_INPUT, type, x, y, w, h, label, handle_it);
    ob->boxtype = FL_INPUT_BOXTYPE;
    ob->col1 = FL_INPUT_COL1;
    ob->col2 = FL_INPUT_COL2;
    ob->align = FL_INPUT_ALIGN;
    ob->lcol = FL_INPUT_LCOL;

    ob->lsize = fl_cntl.inputFontSize ?
	fl_cntl.inputFontSize : FL_DEFAULT_SIZE;

    fl_set_object_prehandler(ob, input_pre);
    fl_set_object_posthandler(ob, input_post);

    ob->wantkey = (ob->type == FL_MULTILINE_INPUT ? FL_KEY_ALL : FL_KEY_NORMAL);
    ob->input = 1;
    ob->click_timeout = FL_CLICK_TIMEOUT;

    sp = ob->spec = fl_calloc(1, ob->spec_size = sizeof(SPEC));

    sp->textcol = FL_INPUT_TCOL;
    sp->curscol = FL_INPUT_CCOL;
    sp->position = -1;
    sp->endrange = -1;
    sp->size = 8;
    sp->lines = sp->ypos = 1;
    sp->str = fl_malloc(sp->size);
    sp->str[0] = '\0';
    sp->how_return = FL_RETURN_END_CHANGED;
    sp->maxchars = ob->type == FL_DATE_INPUT ? 10 :
	(ob->type == FL_SECRET_INPUT ? 16 : 0);

    sp->dummy = ob;
    sp->dummy->spec = sp;
    sp->input = ob;
    sp->field_char = ' ';

    /* can't remember why validated input return is set to RETURN_END 
       but probably with some reason. Wait until 1.0 to reset it
     */
    if (ob->type == FL_FLOAT_INPUT || ob->type == FL_INT_INPUT)
    {
	sp->validate = float_int_validator;
	sp->how_return = FL_RETURN_END;
    }
    else if (ob->type == FL_DATE_INPUT)
    {
	fl_set_input_format(ob, FL_INPUT_MMDD, '/');
	sp->validate = date_validator;
	sp->how_return = FL_RETURN_END;
    }

    fl_set_object_dblbuffer(ob, type != FL_HIDDEN_INPUT);

    return ob;
}

static int
fake_handle(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	    int key, void *ev)
{
    SPEC *sp = ob->spec;

    switch (event)
    {
    case FL_ATTRIB:
	if (sp->dummy != ob)	/* this can only happen with fdesign */
	    sp->dummy = ob;
	copy_attributes(sp->dummy, sp->input);
    case FL_DRAW:
    case FL_DRAWLABEL:
	check_scrollbar_size(ob);
	break;
    }
    return 0;
}

/* Adds an object */
FL_OBJECT *
fl_add_input(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	     const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;
    int oldu = fl_get_coordunit();

    ob = fl_create_input(type, x, y, w, h, label);
    sp = ob->spec;

    fl_set_coordunit(FL_COORD_PIXEL);

    x = ob->x;
    y = ob->y;
    w = ob->w;
    h = ob->h;

    if (ob->type == FL_MULTILINE_INPUT)
    {
	fl_set_object_label(ob, "");
	sp->dummy = fl_create_box(FL_MULTILINE_INPUT, x, y, w, h, label);
	sp->dummy->objclass = FL_INPUT;
	copy_attributes(ob, sp->dummy);
	sp->dummy->handle = fake_handle;
	sp->dummy->spec = sp;

	fl_add_child(sp->dummy, ob);

	sp->hh_def = sp->vw_def = fl_get_default_scrollbarsize(ob);
	sp->h_pref = sp->v_pref = FL_AUTO;
	sp->vscroll = fl_create_scrollbar(fl_context->vscb, x + w - sp->vw_def,
					  y, sp->vw_def, h, "");
	fl_set_object_resize(sp->vscroll, FL_RESIZE_NONE);

	sp->hscroll = fl_create_scrollbar(fl_context->hscb, x,
				     y + h - sp->hh_def, w, sp->hh_def, "");

	fl_set_object_resize(sp->hscroll, FL_RESIZE_NONE);

	fl_set_scrollbar_value(sp->vscroll, 0.0);
	fl_set_object_callback(sp->vscroll, vsl_cb, 0);
	fl_set_scrollbar_value(sp->hscroll, 0.0);
	fl_set_object_callback(sp->hscroll, hsl_cb, 0);

	fl_add_child(sp->dummy, sp->hscroll);
	fl_add_child(sp->dummy, sp->vscroll);
	fl_set_object_callback(sp->input, input_cb, 0);
    }

    fl_add_object(fl_current_form, sp->dummy);
    fl_set_coordunit(oldu);

    return sp->dummy;
}

/* Sets the particular input string. No checks for validity */
void
fl_set_input(FL_OBJECT * ob, const char *str)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    int len;
    char *p;

    if (!str)
	str = "";

    len = strlen(str);

    if (sp->size < len + 1)
    {
	sp->size = len + 1;
	sp->str = (char *) fl_realloc(sp->str, sp->size);
    }

    strcpy(sp->str, str);

    if (ob->type != FL_MULTILINE_INPUT && (p = strchr(sp->str, '\n')))
    {
	*p = '\0';
	len = strlen(sp->str);
    }

    /* if no focus, need to remain so */
    if (sp->position != -1)
	sp->position = len;

    sp->endrange = -1;

    sp->lines = fl_get_input_numberoflines(ob);
    fl_get_input_cursorpos(ob, &sp->xpos, &sp->ypos);

    /* get max string width. It is possible fl_set_input is used before the
       form is show, draw_object is a no-op, thus we end up with a wrong
       string size */

    fl_get_string_dimension(ob->lstyle, ob->lsize,
			    sp->str, len, &sp->max_pixels, &len);

    fl_freeze_form(ob->form);
    {
	check_scrollbar_size(ob);
	make_line_visible(ob, sp->ypos);
	fl_redraw_object(sp->input);
	sp->xoffset = 0;
	check_scrollbar_size(ob);
	redraw_scrollbar(ob);
    }
    fl_unfreeze_form(ob->form);
}

/* Sets the color of the input string. */
void
fl_set_input_color(FL_OBJECT * ob, int textcol, int curscol)
{
    SPEC *sp = ob->spec;
    sp->textcol = textcol;
    sp->curscol = curscol;
    fl_redraw_object(sp->input);
}

int
fl_set_input_fieldchar(FL_OBJECT * ob, int fchar)
{
    SPEC *sp = ob->spec;
    int ochar = sp->field_char;

    if (ob->objclass != FL_INPUT)
    {
	M_err("InputFieldChar", "%s not input class", ob ? ob->label : "null");
	return 0;
    }

    sp->field_char = fchar;
    return ochar;
}

/* returns a pointer to the text string */
const char *
fl_get_input(FL_OBJECT * ob)
{
    return ((SPEC *) (ob->spec))->str;
}

/* Sets whether to return value all the time or only when pressing return */
void
fl_set_input_return(FL_OBJECT * ob, int value)
{
    ((SPEC *) (ob->spec))->how_return = value;
}

void
fl_set_input_scroll(FL_OBJECT * ob, int yes)
{
    ((SPEC *) (ob->spec))->noscroll = !yes;
}

/* makes a part of an input string selected or deselected */
void
fl_set_input_selected_range(FL_OBJECT * ob, int begin, int end)
{
    SPEC *sp = ob->spec;
    int len;

    if (ob->type == FL_HIDDEN_INPUT)
	return;

    len = strlen(sp->str);

    if (begin < 0)
	sp->beginrange = 0;
    else if (begin > len)
	sp->beginrange = len;
    else
	sp->beginrange = begin;

    if (end < 0)
	sp->endrange = -1;
    else if (end > len)
	sp->endrange = len;
    else
	sp->endrange = end;

    /* move cursor to the head */
    sp->position = sp->beginrange;
    fl_redraw_object(sp->input);
}

const char *
fl_get_input_selected_range(FL_OBJECT * ob, int *begin, int *end)
{
    SPEC *sp = ob->spec;
    static char *selbuf;
    static int nselbuf;
    int n;

    n = sp->endrange - sp->beginrange;

    if (n < 1)
    {
	if (begin)
	    *begin = -1;
	if (end)
	    *end = -1;
	return 0;
    }

    if (begin)
	*begin = sp->beginrange;

    if (end)
	*end = sp->endrange;

    if (n > nselbuf)
    {
	selbuf = selbuf ? fl_realloc(selbuf, n + 1) : fl_malloc(n + 1);
	nselbuf = n;
    }

    strncpy(selbuf, sp->str + sp->beginrange, n);
    selbuf[n] = '\0';

    return selbuf;
}

/* selects the current input programmatically without moving
 * the cursor
 */
void
fl_set_input_selected(FL_OBJECT * ob, int yes)
{
    SPEC *sp = ob->spec;

    if (ob->type == FL_HIDDEN_INPUT)
	return;

    if (yes)
    {
	sp->position = sp->endrange = strlen(sp->str);
	sp->beginrange = 0;
    }
    else
    {
	sp->endrange = -1;
    }

    fl_redraw_object(sp->input);
}

/* given an (x,y) location,  return the string position in chars */
static int
xytopos(SPEC *sp, int xpos, int ypos, int len)
{
    int y;
    char *s = sp->str, *se = s + len;

    for (y = 1; y < ypos && s < se; s++)
	if (*s == '\n')
	    y++;
    return (s - sp->str) + xpos;
}


/* move cursor within the input field. curpos is measured in chars */
void
fl_set_input_cursorpos(FL_OBJECT * ob, int xpos, int ypos)
{
    SPEC *sp = ob->spec;
    int newp, len;

    if (ob->type == FL_HIDDEN_INPUT)
	return;

    if (ypos < 1)
	ypos = 1;
    else if (ypos > sp->lines)
	ypos = sp->lines;

    if (xpos < 0)
	xpos = 0;

    len = strlen(sp->str);

    newp = xytopos(sp, xpos, ypos, len);

    if (newp > len)
	newp = len;

    if (newp != sp->position)
    {
	sp->position = newp;
	if (!make_line_visible(ob, sp->ypos))
	    fl_redraw_object(sp->input);
    }
}

int
fl_get_input_cursorpos(FL_OBJECT * ob, int *x, int *y)
{
    register SPEC *sp = ob->spec;
    register int i, j;
    register char *s = sp->str;

    for (i = 1, j = 0; s && *s && sp->position > s - sp->str; s++)
    {
	j++;
	if (*s == '\n')
	{
	    i++;
	    j = 0;
	}
    }

    *x = sp->position >= 0 ? j : -1;
    *y = i;
    return sp->position;
}


#define Control(c)       ((c) - 'a' + 1)
#define Meta(c)          ((c) | METAMASK)

#define Dump(a)     fprintf(stderr,"\t%s: %ld(0x%lx)\n",#a,kmap.a,kmap.a)

/*
 * really should let the user have a chance to override any of the
 * defaults here. _TODO_
 */

static void
set_default_keymap(int force)
{
    static int initialized;

    if (!force && initialized)
	return;

    initialized = 1;

    /* Emacs defaults */
    kmap.moveto_next_char = Control('f');
    kmap.moveto_prev_char = Control('b');
    kmap.moveto_next_line = Control('n');
    kmap.moveto_prev_line = Control('p');
    kmap.moveto_prev_word = Meta('b');
    kmap.moveto_next_word = Meta('f');

    kmap.moveto_bof = Meta('<');
    kmap.moveto_eof = Meta('>');
    kmap.moveto_bol = Control('a');
    kmap.moveto_eol = Control('e');

    kmap.del_prev_char = 127;
    kmap.del_prev_word = Meta(127);
    kmap.del_next_char = Control('d');
    kmap.del_next_word = Meta('d');
    kmap.del_to_eol = Control('k');
    kmap.del_to_eos = Meta('k');

    kmap.backspace = Control('h');
    kmap.transpose = Control('t');
    kmap.paste = Control('y');
    kmap.clear_field = Control('u');

#if (FL_DEBUG > ML_WARN)
    if (fl_cntl.debug > 1)
    {
	Dump(moveto_prev_char);
	Dump(moveto_next_char);
	Dump(moveto_prev_word);
	Dump(moveto_next_word);
	Dump(moveto_prev_line);
	Dump(moveto_next_line);
	Dump(moveto_bol);
	Dump(moveto_eol);
	Dump(moveto_eof);
	Dump(moveto_eol);
    }
#endif
}

void
fl_set_input_maxchars(FL_OBJECT * ob, int maxchars)
{
    ((SPEC *) (ob->spec))->maxchars = maxchars;
}

FL_VALIDATE
fl_set_input_filter(FL_OBJECT * ob, FL_VALIDATE validate)
{
    FL_VALIDATE old = ((SPEC *) (ob->spec))->validate;
    ((SPEC *) (ob->spec))->validate = validate;
    return old;
}

void
fl_set_input_format(FL_OBJECT * ob, int fmt, int sep)
{
    SPEC *sp = ob->spec;

    if (!isprint(sep) || isdigit(sep))
	sep = '/';
    sp->attrib1 = fmt;
    sp->attrib2 = sep;
}

void
fl_get_input_format(FL_OBJECT * ob, int *fmt, int *sep)
{
    SPEC *sp = ob->spec;
    *fmt = sp->attrib1;
    *sep = sp->attrib2;
}

/*
 * validators for specialized inputs
 */

static int
date_validator(FL_OBJECT * ob, const char *oldstr,
	       const char *newstr, int newc)
{
    char *val, *s, sepsep[4];
    char ssep[2] = {0, 0};
    int i, len, ival[3] = {1, 1, 1};
    int fmt, sep;
    int invalid = FL_RINGBELL | FL_INVALID;
    int m, d;

    /* consider empty valid */
    if ((len = strlen(newstr)) == 0)
	return FL_VALID;

    fl_get_input_format(ob, &fmt, &sep);

    ssep[0] = sep;
    strcat(strcpy(sepsep, ssep), ssep);

    if ((newc != sep && newc != 0 && !isdigit(newc)) ||
	newstr[0] == sep || strstr(newstr, sepsep))
	return invalid;

    s = fl_strdup(newstr);
    for (i = 0, val = strtok(s, ssep); val; val = strtok(0, ssep))
    {
	/* must allow incomplete input so 12/01 does not get rejected at 0 */
	if (val[1] == '\0' && val[0] == newstr[len - 1])
	{
	    if (newc != 0)
            {
                fl_free(s);
		return FL_VALID;
            }
	}

	ival[i++] = atoi(val);
    }
    fl_free(s);

    if (i > 3)
	return invalid;

    if (i != 3 && newc == 0)
	return invalid;

    m = fmt == FL_INPUT_MMDD ? 0 : 1;
    d = !m;

    if (ival[m] < 1 || ival[m] > 12 || ival[d] < 1 || ival[d] > 31)
	return invalid;

    if ((ival[m] == 2 && ival[d] > 29) ||
	(ival[d] > 30 && (ival[m] % 2) == 0 && ival[m] < 8) ||
	(ival[d] > 30 && (ival[m] % 2) && ival[m] > 8))
	return invalid;

    return FL_VALID;
}

static int
float_int_validator(FL_OBJECT * ob, const char *oldstr,
		    const char *str, int newc)
{
    const char *lc, *llc;
    char *ptr = 0;		/* pointer to string */
    int slen, c;


    /* empty string is considered valid */
    if (!(slen = strlen(str)))
	return FL_VALID;

    if (newc && !isdigit(newc) && newc != '-' && newc != '+' &&
	ob->type == FL_INT_INPUT)
	return FL_INVALID | FL_RINGBELL;

    if (ob->type == FL_FLOAT_INPUT)
	strtod(str, &ptr);
    else
	strtol(str, &ptr, 10);

    lc = str + slen - 1;
    llc = lc - 1;

    if (newc == 0 && ((c = tolower(*lc)) == '+' || c == '-' || c == 'e'))
	return FL_INVALID | FL_RINGBELL;

    /* -+eE at end can cause strtod to fail, but it is in fact valid. The
       check is too lenient though */

    if (*ptr &&
	strcmp(str, "-") && strcmp(str, "+") &&
	(strcmp(str, ".") || ob->type != FL_FLOAT_INPUT) &&
	((*lc != '-' && *lc != 'e' && *lc != '+' && *lc != 'E') ||
	 (*lc == '-' && *llc != 'e' && *llc != 'E') ||
	 (*lc == '+' && *llc != 'e' && *llc != 'E') ||
	 (*lc == 'e' && !isdigit( ( int ) *llc) && *llc != '.') ||
	 (*lc == 'E' && !isdigit( ( int ) *llc) && *llc != '.')))
	return FL_INVALID | FL_RINGBELL;

    return FL_VALID;
}

void
fl_set_input_xoffset(FL_OBJECT * ob, int xoff)
{
    SPEC *sp = ob->spec;

    if (sp->xoffset != xoff)
    {
	sp->xoffset = xoff;
	if (sp->drawtype != HSLIDER)
	{
	    check_scrollbar_size(ob);
	    redraw_scrollbar(ob);
	}
	sp->drawtype = COMPLETE;
	fl_redraw_object(sp->input);
    }
}

int
fl_get_input_xoffset(FL_OBJECT * ob)
{
    return ((SPEC *) ob->spec)->xoffset;
}


static void
correct_topline(SPEC *sp, int *top)
{
    if (sp->lines > sp->screenlines)
    {
	if (sp->lines + 1 - *top < sp->screenlines)
	    *top = sp->lines - sp->screenlines + 1;
	if (*top < 1)
	    *top = 1;
    }
    else
	*top = 1;
}

static int
count_lines(const char *s)
{
    int i;

    for (i = 0; s && *s; s++)
	if (*s == '\n')
	    i++;
    return i + 1;
}

int
fl_get_input_numberoflines(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    return sp->lines = count_lines(sp->str);
}


void
fl_set_input_topline(FL_OBJECT * ob, int top)
{
    SPEC *sp = ob->spec;

    correct_topline(sp, &top);

    if (sp->topline != top)
    {
	sp->topline = top;
	if (sp->drawtype != VSLIDER)
	{
	    check_scrollbar_size(ob);
	    redraw_scrollbar(ob);
	}
	sp->drawtype = COMPLETE;
	sp->yoffset = (sp->topline - 1) * sp->charh;
	fl_redraw_object(sp->input);
    }
}

/* line number, n, is numbered from 1.
 */
static int
make_line_visible(FL_OBJECT * ob, int n)
{
    SPEC *sp = ob->spec;
    int oldtop = sp->topline;

    if (ob->type != FL_MULTILINE_INPUT)
	return 0;

    if (n < sp->topline)
	fl_set_input_topline(ob, n);
    else if ((n - sp->topline + 1) > sp->screenlines)
	fl_set_input_topline(ob, n - sp->screenlines + 1);
    else if (sp->lines + 1 - sp->topline < sp->screenlines)
	fl_set_input_topline(ob, sp->lines);

    return oldtop != sp->topline;
}

static int
make_char_visible(FL_OBJECT * ob, int n)
{
    SPEC *sp = ob->spec;
    int startpos = sp->position;
    int oldxoffset = sp->xoffset;
    int tmp;

    if (n < 0)
	return 0;

    while (startpos > 0 && sp->str[startpos - 1] != NL)
	startpos--;

    tmp = get_substring_width(ob, startpos, n + startpos);

    if (tmp < sp->xoffset)
	sp->xoffset = tmp;
    else if ((tmp - sp->xoffset) > sp->w)
	sp->xoffset = tmp - sp->w;

    if (sp->xoffset != oldxoffset)
    {
	check_scrollbar_size(ob);
	redraw_scrollbar(ob);
	fl_redraw_object(sp->input);
	return 1;
    }
    return 0;
}


void
fl_set_input_vscrollbar(FL_OBJECT * ob, int pref)
{
    SPEC *sp = ob->spec;

    if ((sp->v_pref != pref))
    {
	sp->v_pref = pref;
	check_scrollbar_size(ob);
	redraw_scrollbar(ob);
	fl_redraw_object(sp->input);
    }
}

void
fl_set_input_hscrollbar(FL_OBJECT * ob, int pref)
{
    SPEC *sp = ob->spec;

    if ((sp->h_pref != pref))
    {
	sp->h_pref = pref;
	check_scrollbar_size(ob);
	redraw_scrollbar(ob);
	fl_redraw_object(sp->input);
    }
}

void
fl_set_input_scrollbarsize(FL_OBJECT * ob, int hh, int vw)
{
    SPEC *sp = ob->spec;

    if (sp->hh_def != hh || sp->vw_def != vw)
    {
	sp->hh_def = hh;
	sp->vw_def = vw;
	check_scrollbar_size(ob);
	redraw_scrollbar(ob);
	fl_redraw_object(sp->input);
    }
}

int
fl_get_input_topline(FL_OBJECT * ob)
{
    return ((SPEC *) ob->spec)->topline;
}

int
fl_get_input_screenlines(FL_OBJECT * ob)
{
    return ((SPEC *) ob->spec)->screenlines;
}

#define SetKey(m)  if(keymap->m) kmap.m = keymap->m

void
fl_set_input_editkeymap(const FL_EditKeymap * keymap)
{
    /* if keymap is null, force default */
    if (!keymap)
    {
	set_default_keymap(1);
	return;
    }

    set_default_keymap(0);

    SetKey(del_prev_char);
    SetKey(del_next_char);
    SetKey(del_prev_word);
    SetKey(del_next_word);

    SetKey(moveto_prev_char);
    SetKey(moveto_next_char);
    SetKey(moveto_prev_word);
    SetKey(moveto_next_word);
    SetKey(moveto_prev_line);
    SetKey(moveto_next_line);
    SetKey(moveto_bof);
    SetKey(moveto_eof);
    SetKey(moveto_bol);
    SetKey(moveto_eol);

    SetKey(backspace);
    SetKey(clear_field);
    SetKey(paste);
    SetKey(transpose);
    SetKey(del_to_eos);
    SetKey(del_to_eol);
    SetKey(del_to_bol);
}

static void
copy_attributes(FL_OBJECT * src, FL_OBJECT * dest)
{
    if (src != dest)
    {
	dest->col1 = src->col1;
	dest->col2 = src->col2;
	dest->align = src->align;
	dest->boxtype = src->boxtype;
	dest->lcol = src->lcol;
	dest->lstyle = src->lstyle;
	dest->lsize = src->lsize;
    }
}

static void
redraw_scrollbar(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;

    fl_freeze_form(ob->form);

    if (sp->v_on)
    {
	fl_set_scrollbar_size(sp->vscroll, sp->vsize);
	fl_set_scrollbar_value(sp->vscroll, sp->vval);
	if (sp->vsize != 1.0)
	    fl_set_scrollbar_increment(sp->vscroll, sp->vinc1, sp->vinc2);
    }

    if (sp->h_on)
    {
	fl_set_scrollbar_size(sp->hscroll, sp->hsize);
	fl_set_scrollbar_value(sp->hscroll, sp->hval);
	if (sp->hsize != 1.0)
	    fl_set_scrollbar_increment(sp->hscroll, sp->hinc1, sp->hinc2);
    }

    if (sp->attrib)
    {
	fl_redraw_object(sp->input);
	fl_redraw_object(sp->hscroll);
	fl_redraw_object(sp->vscroll);
	sp->attrib = 0;
    }

    if (sp->dead_area && FL_ObjWin(ob))
    {
	sp->dead_area = 0;
	fl_winset(FL_ObjWin(ob));
	fl_drw_box(FL_FLAT_BOX, sp->dummy->x + sp->dummy->w - sp->vw,
		   sp->dummy->y + sp->dummy->h - sp->hh, sp->vw,
		   sp->hh, sp->hscroll->col1, 1);
    }

    fl_unfreeze_form(ob->form);
}

void
fl_set_input_cursor_visible(FL_OBJECT * ob, int visible)
{
    SPEC *sp = ob->spec;

    if (sp->no_cursor != !visible)
    {
	sp->no_cursor = !visible;
	fl_redraw_object(ob);
    }
}

static void
make_cursor_visible(FL_OBJECT * ob, SPEC *sp, int startpos, int prev)
{
    int tt = get_substring_width(ob, startpos, sp->position);

    if (prev == -1)
    {
	if (tt - sp->xoffset >= sp->w)
	    sp->xoffset = tt - sp->w;
	else if (tt < sp->xoffset)
	    sp->xoffset = tt;
	else if (tt == 0)
	    sp->xoffset = 0;
    }
    else
    {
	if (tt - sp->xoffset > sp->w)
	    sp->xoffset = tt - sp->w;
    }
}

int 
fl_input_changed(FL_OBJECT *obj)
{
    return (obj && (obj->objclass == FL_INPUT)) ? 
           ((SPEC*)(obj->spec))->changed:0;
}
