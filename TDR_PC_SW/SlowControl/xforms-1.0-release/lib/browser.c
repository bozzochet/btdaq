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
 * $Id: browser.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *   Browser composite.
 *   scrollbar redrawing can be further optimized.
 *
 */
#include "forms.h"
#include "ptextbox.h"
#include "pbrowser.h"

#define SPEC   FL_BROWSER_SPEC
#define Comp   FL_BROWSER_SPEC

/* from child object to get to the browser SPEC */
#define GetPSpec(ob)    ((SPEC*)(ob->parent->spec))

/* from child object ob to get to the browser */
#define GetBR(ob)       GetPSpec(ob)->br

/* from browser to get to SPEC */
#define GetSpec(ob)     GetPSpec(ob)

#define MaxPixels(sp)   (sp->maxpixels + 5)

extern const char *fl_get_textbox_line(FL_OBJECT * ob, int linenumb);

static int D;

static void
attrib_change(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;

    /* this can only happen with fdesign */
    if (sp->br != ob)
	sp->br = ob;

    /* text box stuff */
    sp->tb->x = ob->x;
    sp->tb->y = ob->y;
    sp->tb->visible = 1;
    sp->tb->input = ob->input;
    sp->tb->wantkey = ob->wantkey;

    sp->tb->type = sp->br->type;
    sp->tb->boxtype = sp->br->boxtype;
    sp->tb->lcol = sp->br->lcol;
    sp->tb->col1 = sp->br->col1;
    sp->tb->col2 = sp->br->col2;
    sp->tb->bw = sp->br->bw;

    /* scrollbars */
    if (sp->br->boxtype == FL_DOWN_BOX &&
	sp->hsl->type == FL_HOR_NICE_SCROLLBAR)
    {
	sp->hsl->boxtype = FL_FRAME_BOX;
	sp->vsl->boxtype = FL_FRAME_BOX;
    }
    else if (sp->br->boxtype == FL_DOWN_BOX &&
	     sp->hsl->type == FL_HOR_SCROLLBAR)
    {
	sp->hsl->boxtype = FL_UP_BOX;
	sp->vsl->boxtype = FL_UP_BOX;
    }
    else
    {
	sp->hsl->boxtype = sp->br->boxtype;
	sp->vsl->boxtype = sp->br->boxtype;
    }

    sp->hsl->bw = sp->vsl->bw = sp->br->bw;

    if (!sp->user_set && sp->br->boxtype != FL_DOWN_BOX)
	sp->vw = sp->vw_def = sp->hh = sp->hh_def = fl_get_default_scrollbarsize(ob);
}

static void
get_geometry(FL_OBJECT * ob)
{
    Comp *comp = GetSpec(ob);
    FL_OBJECT *tb = comp->tb;
    FL_TEXTBOX_SPEC *sp = comp->tb->spec;
    int h_on = comp->h_on, v_on = comp->v_on;

    comp->hh = comp->vw = 0;
    comp->h_on = comp->v_on = 0;

    tb->w = ob->w - comp->vw;
    tb->h = ob->h - comp->hh;

    fl_calc_textbox_size(tb);

    if ((sp->screenlines < sp->lines && comp->v_pref != FL_OFF) ||
	comp->v_pref == FL_ON)
    {
	comp->v_on = 1;
	comp->vw = comp->vw_def;
	tb->w = ob->w - comp->vw;
	sp->w -= comp->vw;
    }

    if ((MaxPixels(sp) > sp->w && comp->h_pref != FL_OFF) ||
	comp->h_pref == FL_ON)
    {
	comp->h_on = 1;
	comp->hh = comp->hh_def;
	tb->h = ob->h - comp->hh;
	sp->h -= comp->hh;
    }

    sp->screenlines = (int)(sp->h / (float)sp->charheight + 0.001f);

    if (!comp->v_on && sp->screenlines < sp->lines && comp->v_pref != FL_OFF)
    {
	comp->v_on = 1;
	comp->vw = comp->vw_def;
	sp->w -= comp->vw;
	tb->w = ob->w - comp->vw;
    }

    comp->hsl->visible = comp->h_on;
    comp->vsl->visible = comp->v_on;

    if (comp->v_on)
    {
	int delta = (sp->lines - sp->screenlines);
	comp->vsl->x = (ob->x + ob->w - comp->vw);
	comp->vsl->y = ob->y;
	comp->vsl->w = comp->vw;
	comp->vsl->h = ob->h - comp->hh;
	if (delta > 0)
	{
	    comp->vsize =  sp->screenlines / (float)sp->lines;
	    comp->vinc1 = (sp->screenlines - 0.99f) / delta;
	    comp->vinc2 = 1.01f / delta;
	    comp->vval = (sp->topline - 1.0f) / delta;
	}
	else
	    comp->vsize = 1.0f;
    }

    if (comp->h_on)
    {
	int delta = (MaxPixels(sp) - sp->w);
	comp->hsl->x = ob->x;
	comp->hsl->y = ob->y + ob->h - comp->hh;
	comp->hsl->w = ob->w - comp->vw;
	comp->hsl->h = comp->hh;

	if (delta > 0)
	{
	    comp->hsize = (float) sp->w / (MaxPixels(sp));
	    comp->hinc1 = (8.0f * sp->charheight) / delta;
	    comp->hinc2 = (sp->charheight - 2.0f) / delta;
	    comp->hval = (float) sp->xoffset / delta;
	}
	else
	    comp->hsize = 1.0;
    }

    if ((h_on != comp->h_on || v_on != comp->v_on))
    {
	comp->attrib = 1;
	comp->dead_area = !(comp->h_on ^ comp->v_on);
    }
    else
    {
	comp->attrib = 0;
    }
}

static void
draw_dead_area(FL_OBJECT * ob, Comp * comp)
{

    if (comp->dead_area && FL_ObjWin(comp->tb))
    {
	fl_winset(FL_ObjWin(comp->tb));
	fl_drw_box(FL_FLAT_BOX, ob->x + ob->w - comp->vw,
		   ob->y + ob->h - comp->hh,
		   comp->vw, comp->hh, comp->vsl->col1, 1);
	comp->dead_area = 0;
    }
}

static int
handle(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
       int key, void *ev)
{
    Comp *comp = GetSpec(ob);

    switch (event)
    {
    case FL_DRAW:
	attrib_change(ob);
	get_geometry(ob);
	((FL_TEXTBOX_SPEC *) (comp->tb->spec))->attrib = 1;
	/* initial scrollbar size */
	comp->hsl->visible = 0;
	comp->vsl->visible = 0;
	fl_set_scrollbar_size(comp->hsl, comp->hsize);
	fl_set_scrollbar_size(comp->vsl, comp->vsize);
	comp->hsl->visible = comp->h_on;
	comp->vsl->visible = comp->v_on;
	draw_dead_area(ob, comp);
    case FL_DRAWLABEL:
	fl_draw_object_label(ob);
	break;
    case FL_FREEMEM:
	/* children take care of themselves */
	fl_addto_freelist(comp);
	break;
    }
    return 0;
}


static void
fl_redraw_scrollbar(FL_OBJECT * ob)
{
    Comp *comp = GetSpec(ob);

    get_geometry(ob);

    fl_freeze_form(ob->form);

    if (comp->v_on)
    {
	fl_set_scrollbar_size(comp->vsl, comp->vsize);
	fl_set_scrollbar_value(comp->vsl, comp->vval);

	if (comp->vsize != 1.0)
	    fl_set_scrollbar_increment(comp->vsl, comp->vinc1, comp->vinc2);
    }

    if (comp->h_on)
    {

	fl_set_scrollbar_size(comp->hsl, comp->hsize);
	fl_set_scrollbar_value(comp->hsl, comp->hval);

	if (comp->hsize != 1.0)
	    fl_set_scrollbar_increment(comp->hsl, comp->hinc1, comp->hinc2);
    }

    if (comp->attrib)
    {
	((FL_TEXTBOX_SPEC *) (comp->tb->spec))->attrib = 1;
	fl_redraw_object(comp->vsl);
	fl_redraw_object(comp->hsl);
	fl_redraw_object(comp->tb);
    }

    draw_dead_area(ob, comp);

    fl_unfreeze_form(ob->form);
}


static void
hcb(FL_OBJECT * ob, long data)
{
    Comp *comp = ob->parent->spec;
    float val = (float)fl_get_scrollbar_value(comp->hsl);
    FL_TEXTBOX_SPEC *sp = comp->tb->spec;
    int np = (int)(val * (MaxPixels(sp) - sp->w) + 0.1f);

    np = fl_set_textbox_xoffset(comp->tb, np);

    if(comp->hcb)
       comp->hcb(comp->br, np, comp->hcb_data);
}

static void
vcb(FL_OBJECT * ob, long data)
{
    Comp *comp = ob->parent->spec;
    FL_TEXTBOX_SPEC *sp = comp->tb->spec;
    float val = (float)fl_get_scrollbar_value(comp->vsl);
    int nl = (int)(val * (sp->lines - sp->screenlines) + 1.001f);

    nl = fl_set_textbox_topline(comp->tb, nl);

    if(comp->vcb)
       comp->vcb(comp->br, nl, comp->vcb_data);
}

/* textbox callback routine. If browser has no callbacks,
 * we're screwed
 */
static void
tbcb(FL_OBJECT * ob, long data)
{
    SPEC *sp = GetPSpec(ob);

    if (sp->tb->type == FL_MULTI_TEXTBOX)
	fl_call_object_callback(sp->br);
    else
	fl_object_qenter(sp->br);
}

/* textbox dblclick callback */
static void
tb_dblcallback(FL_OBJECT * ob, long data)
{
    SPEC *sp = GetPSpec(ob);

    if (sp->callback)
	sp->callback(sp->br, sp->callback_data);
}

static int
tbpost(FL_OBJECT * ob, int ev, FL_Coord mx, FL_Coord my,
       int key, void *xev)
{
    FL_OBJECT *br = GetPSpec(ob)->br;
    return br->posthandle ? br->posthandle(br, ev, mx, my, key, xev) : 0;
}

static int
tbpre(FL_OBJECT * ob, int ev, FL_Coord mx, FL_Coord my,
      int key, void *xev)
{

    FL_OBJECT *br = GetPSpec(ob)->br;
    return br->prehandle ? br->prehandle(br, ev, mx, my, key, xev) : 0;
}

#define IS_FLATBOX(b) (b==FL_FRAME_BOX    ||\
                       b==FL_EMBOSSED_BOX ||\
                       b==FL_BORDER_BOX   || \
                       b == FL_ROUNDED_BOX)

int
fl_get_default_scrollbarsize(FL_OBJECT * ob)
{
    int delta = (FL_abs(ob->bw) + 3 * (ob->bw > 0));
    int flat = IS_FLATBOX(ob->boxtype) ? 2 : 0;

    if (ob->w > 250 && ob->h > 250)
	return 15 + delta - flat;
    else if (ob->w < 150 || ob->h < 150)
	return 13 + delta - flat;
    else
	return 14 + delta - flat;
}

FL_OBJECT *
fl_create_browser(int type, int x, int y, int w, int h, const char *l)
{
    FL_OBJECT *ob;
    SPEC *sp;

    ob = fl_make_object(FL_BROWSER, type, x, y, w, h, l, handle);
    ob->spec_size = sizeof(*sp);
    sp = ob->spec = fl_calloc(1, ob->spec_size);
    sp->br = ob;
    sp->br->parent = sp->br;

    sp->tb = fl_create_textbox(type, x, y, w, h, "");
    /* copy browser attributes from textbox */
    ob->boxtype = sp->tb->boxtype;
    ob->lcol = sp->tb->lcol;
    ob->align = sp->tb->align;
    ob->col1 = sp->tb->col1;
    ob->col2 = sp->tb->col2;
    ob->wantkey = sp->tb->wantkey;

    /* textbox handlers */
    fl_set_object_callback(sp->tb, tbcb, 0);
    fl_set_textbox_dblclick_callback(sp->tb, tb_dblcallback, 0);
    fl_set_object_posthandler(sp->tb, tbpost);
    fl_set_object_prehandler(sp->tb, tbpre);

    /* scrollbars */
    sp->vw_def = sp->hh_def = D = fl_get_default_scrollbarsize(ob);
    sp->v_pref = sp->h_pref = FL_AUTO;

    sp->hsl = fl_create_scrollbar(fl_context->hscb, x, y + h - D, w - D, D, "");
    sp->hsl->visible = sp->h_pref == FL_ON;
    fl_set_object_callback(sp->hsl, hcb, 0);
    fl_set_scrollbar_value(sp->hsl, 0.0);
    sp->hsl->resize = FL_RESIZE_NONE;;

    sp->vsl = fl_create_scrollbar(fl_context->vscb, x + w - D, y, D, h - D, "");
    sp->vsl->visible = sp->v_pref == FL_ON;
    fl_set_scrollbar_value(sp->vsl, 0.0);
    sp->vsl->resize = FL_RESIZE_NONE;
    fl_set_object_callback(sp->vsl, vcb, 0);
    fl_set_scrollbar_value(sp->hsl, 0);

    return ob;
}

FL_OBJECT *
fl_add_browser(int type, int x, int y, int w, int h, const char *l)
{
    FL_OBJECT *ob = fl_create_browser(type, x, y, w, h, l);
    SPEC *sp = ob->spec;

    fl_add_child(sp->br, sp->tb);
    fl_add_child(sp->br, sp->hsl);
    fl_add_child(sp->br, sp->vsl);
    fl_add_object(fl_current_form, sp->br);

    return ob;
}

void
fl_set_browser_vscrollbar(FL_OBJECT * ob, int on)
{
    Comp *comp = GetSpec(ob);

    if (comp->v_pref != on)
    {
	comp->v_pref = on;
	fl_redraw_scrollbar(ob);
    }
}

void
fl_set_browser_hscrollbar(FL_OBJECT * ob, int on)
{
    Comp *comp = GetSpec(ob);

    if (comp->h_pref != on)
    {
	comp->h_pref = on;
	fl_redraw_scrollbar(ob);
    }
}

void
fl_set_browser_hscroll_callback(FL_OBJECT *ob, 
                               FL_BROWSER_SCROLL_CALLBACK cb,
                               void *data)
{
      Comp *comp = GetSpec(ob);

      comp->hcb = cb;
      comp->hcb_data = data;
}

FL_BROWSER_SCROLL_CALLBACK
fl_get_browser_hscroll_callback(FL_OBJECT *ob)
{
      return ((Comp *) GetSpec(ob))->hcb;
}
 

void
fl_set_browser_vscroll_callback(FL_OBJECT *ob, 
                               FL_BROWSER_SCROLL_CALLBACK cb,
                               void *data)
{
      Comp *comp = GetSpec(ob);

      comp->vcb = cb;
      comp->hcb_data = data;
}

FL_BROWSER_SCROLL_CALLBACK
fl_get_browser_vscroll_callback(FL_OBJECT *ob)
{
      return ((Comp *) GetSpec(ob))->vcb;
}

/* meant for the textbox to handle scroll callback properly */
void
fl_adjust_browser_scrollbar(FL_OBJECT *ob)
{
      Comp *comp = GetSpec(ob);

      fl_call_object_callback(comp->hsl);
      fl_call_object_callback(comp->vsl);
}

void
fl_clear_browser(FL_OBJECT * ob)
{
    Comp *comp = GetSpec(ob);

    fl_clear_textbox(comp->tb);
    fl_freeze_form(ob->form);
    fl_redraw_scrollbar(ob);
    fl_set_scrollbar_value(comp->hsl, 0.0);
    fl_set_scrollbar_size(comp->hsl, 1.0);
    fl_set_scrollbar_value(comp->vsl, 0.0);
    fl_set_scrollbar_size(comp->vsl, 1.0);
    fl_unfreeze_form(ob->form);
}

FL_Coord
fl_get_browser_xoffset(FL_OBJECT * ob)
{
    SPEC *sp = ob->parent->spec;
    return fl_get_textbox_xoffset(sp->tb);
}

/* set_xoffset, set_topline and select_line are kind of nasty in that
 * they might get called from browser's child object!
 */
void
fl_set_browser_xoffset(FL_OBJECT * ob, FL_Coord npixels)
{
    SPEC *sp = ob->parent->spec;

    /* do stuff only if we need to */
    if (fl_get_textbox_longestline(sp->tb) > sp->tb->w)
    {
	fl_set_textbox_xoffset(sp->tb, npixels);
	fl_redraw_scrollbar(sp->br);
    }
}

void
fl_set_browser_topline(FL_OBJECT * ob, int topline)
{
    SPEC *sp = ob->parent->spec;
    fl_set_textbox_topline(sp->tb, topline);
    fl_redraw_scrollbar(sp->br);
}

void
fl_select_browser_line(FL_OBJECT * ob, int line)
{
    SPEC *sp = ob->parent->spec;
    fl_select_textbox_line(sp->tb, line, 0);
}

void
fl_addto_browser(FL_OBJECT * ob, const char *newtext)
{
    Comp *comp = GetSpec(ob);
    fl_addto_textbox(comp->tb, newtext);
    fl_redraw_scrollbar(ob);
}

void
fl_insert_browser_line(FL_OBJECT * ob, int linenumb, const char *newtext)
{
    fl_insert_textbox_line(GetSpec(ob)->tb, linenumb, newtext);
    fl_redraw_scrollbar(ob);
}

void
fl_delete_browser_line(FL_OBJECT * ob, int linenumb)
{
    fl_delete_textbox_line(GetSpec(ob)->tb, linenumb);
    fl_redraw_scrollbar(ob);
}

void
fl_replace_browser_line(FL_OBJECT * ob, int linenumb, const char *newtext)
{
    fl_replace_textbox_line(GetSpec(ob)->tb, linenumb, newtext);
    fl_redraw_scrollbar(ob);
}

const char *
fl_get_browser_line(FL_OBJECT * ob, int linenumb)
{
    return fl_get_textbox_line(GetSpec(ob)->tb, linenumb);
}

int
fl_get_browser_maxline(FL_OBJECT * ob)
{
    Comp *comp = GetSpec(ob);
    return ((FL_TEXTBOX_SPEC *) (comp->tb->spec))->lines;
}


void
fl_deselect_browser_line(FL_OBJECT * ob, int line)
{
    Comp *comp = GetSpec(ob);
    fl_deselect_textbox_line(comp->tb, line);
}

void
fl_deselect_browser(FL_OBJECT * ob)
{
    fl_deselect_textbox(GetSpec(ob)->tb);
}

int
fl_isselected_browser_line(FL_OBJECT * ob, int line)
{
    return fl_isselected_textbox_line(GetSpec(ob)->tb, line);
}

int
fl_get_browser(FL_OBJECT * ob)
{
    if (!ob || ob->objclass != FL_BROWSER)
	M_err("GetBrowser", "ob %s is not a browser", ob ? ob->label : "null");

    return fl_get_textbox(GetSpec(ob)->tb);
}

void
fl_set_browser_fontsize(FL_OBJECT * ob, int size)
{
    SPEC *sp = ob->spec;
    FL_TEXTBOX_SPEC *tsp = sp->tb->spec;

    if (tsp->fontsize != size)
    {
	fl_set_textbox_fontsize(GetSpec(ob)->tb, size);
	fl_redraw_scrollbar(ob);
    }
}

void
fl_set_browser_fontstyle(FL_OBJECT * ob, int style)
{
    SPEC *sp = ob->spec;
    FL_TEXTBOX_SPEC *tsp = sp->tb->spec;

    if (tsp->fontstyle != style)
    {
	fl_set_textbox_fontstyle(GetSpec(ob)->tb, style);
	fl_redraw_scrollbar(ob);
    }
}

int
fl_get_browser_topline(FL_OBJECT * ob)
{
    Comp *comp = GetSpec(ob);
    return ((FL_TEXTBOX_SPEC *) (comp->tb->spec))->topline;
}

int
fl_load_browser(FL_OBJECT * ob, const char *f)
{
    int status = fl_load_textbox(GetSpec(ob)->tb, f);
    fl_redraw_scrollbar(ob);
    return status;
}

void
fl_add_browser_line(FL_OBJECT * ob, const char *newtext)
{
    fl_add_textbox_line(GetSpec(ob)->tb, newtext);
    fl_redraw_scrollbar(ob);
}

void
fl_set_browser_dblclick_callback(FL_OBJECT * ob, FL_CALLBACKPTR cb, long a)
{
    Comp *comp = ob->spec;

    comp->callback = cb;
    comp->callback_data = a;
}

void
fl_set_browser_scrollbarsize(FL_OBJECT * ob, int hh, int vw)
{
    Comp *comp = GetSpec(ob);
    int redraw = 0;

    if (hh > 0 && (hh != comp->hsl->h))
    {
	comp->hsl->h = comp->hh_def = hh;
	redraw = 1;
    }

    if (vw > 0 && (vw != comp->hsl->w))
    {
	comp->vsl->w = comp->vw_def = vw;
	redraw = 1;
    }

    if (redraw)
    {
	comp->user_set = 1;
	fl_redraw_object(comp->br);
	fl_redraw_object(comp->tb);
	fl_redraw_object(comp->hsl);
	fl_redraw_object(comp->vsl);
    }
}

void
fl_get_browser_dimension(FL_OBJECT * ob, FL_Coord * x, FL_Coord * y,
			 FL_Coord * w, FL_Coord * h)
{
    fl_get_textbox_dimension(GetSpec(ob)->tb, x, y, w, h);
}

void
fl_set_browser_line_selectable(FL_OBJECT * ob, int line, int flag)
{
    fl_set_textbox_line_selectable(ob, line, flag);
}

void
fl_addto_browser_chars(FL_OBJECT * ob, const char *str)
{
    fl_addto_textbox_chars(GetSpec(ob)->tb, str);
    fl_redraw_scrollbar(ob);
}

/*
 * The following accesses the TEXTBOX spec directly
 */

/* If the browser is not shown yet, this won't be correct */
int
fl_get_browser_screenlines(FL_OBJECT * ob)
{
    FL_TEXTBOX_SPEC *sp = GetSpec(ob)->tb->spec;
    return sp->screenlines;
}

/* Sets the escape key used in the text */
void
fl_set_browser_specialkey(FL_OBJECT * ob, int specialkey)
{
    FL_TEXTBOX_SPEC *sp = GetSpec(ob)->tb->spec;

    sp->specialkey = specialkey;
    sp->attrib = 1;
    fl_redraw_object(GetSpec(ob)->tb);
}

/* bring a line into view */
void
fl_show_browser_line(FL_OBJECT * ob, int j)
{
    FL_TEXTBOX_SPEC *sp = GetSpec(ob)->tb->spec;
    int top = sp->topline;
    int total = sp->screenlines;

    if (j < top || j > (top + total - 1))
	fl_set_browser_topline(ob, j - total / 2);
}

int
fl_set_default_browser_maxlinelength(int n)
{
    return fl_set_textbox_maxlinelength(n);
}
