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
 * $Id: pixmap.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 * Pixmap support. In order to take advantage of Xpm3.4g features,
 * we need both XpmRevision and XpmLibraryVersion check
 *
 */

#include "forms.h"

/*
 * If you've parked xpm.h somewhere other than the standard place,
 * you may need to fiddle with this.
 */

#if !defined( NONSTANDARD_XPM )
#include <X11/xpm.h>
#else
#include <xpm.h>
#endif

#if (FL_DEBUG >= ML_ERR)
#define CHECK(ob,f)  if (!IsValidClass(ob, FL_PIXMAP) &&             \
                         !IsValidClass(ob, FL_PIXMAPBUTTON))         \
                     {                                               \
                       Bark(f, "%s is not Pixmap/pixmapbutton class",\
                            ob ? ob->label : "");                    \
                       return;                                       \
                     }
#else
#define CHECK(ob,f)
#endif

/* turn on good features if we are using 3.4g or later */

#define XPM_34g_or_later  (XpmLibraryVersion() >= 30407)

/*
 * Define this to use override instead of clipping. Slight performace
 * gain, but no as general as clipping
 */
/* #define USE_OVERRIDE */

typedef FL_BUTTON_STRUCT SPEC;

typedef struct
{
    XpmAttributes *xpma;
    GC gc;
    int align;
    int dx, dy;
    int show_focus;
    unsigned int focus_w, focus_h;	/* these should be in button_spec */
}
PixmapSPEC;

static XpmAttributes *xpmattrib;

/* part of pixmap cleanup */
static void
cleanup_xpma_struct(XpmAttributes * xpma)
{
    /* only versions >= 3.4g have alloc_pixels. We always compile with 3.4g
       but have to re-act to dynamic libraries, which may be older */

    if (!xpma || !xpma->colormap)
	return;

    if (XPM_34g_or_later)
    {
	M_warn("XpmCleanUP", "Using 3.4g features");
	XFreeColors(flx->display, xpma->colormap,
		    xpma->alloc_pixels, xpma->nalloc_pixels, 0);
    }
    else
    {
	/* somewhat dangerous */
	M_warn("XpmCleanUP", "Using old xpm libs");
	XFreeColors(flx->display, xpma->colormap,
		    xpma->pixels, xpma->npixels, 0);
    }

    xpma->colormap = 0;

    XpmFreeAttributes(xpma);
    fl_free(xpma);
}

/*
 * Free pixmaps associated with an object
 */
static void
free_pixmap(SPEC *sp)
{
    PixmapSPEC *psp = sp->cspecv;

    fl_free_pixmap(sp->pixmap);
    fl_free_pixmap(sp->mask);
    cleanup_xpma_struct(psp->xpma);
    psp->xpma = 0;
    sp->pixmap = None;
    sp->mask = None;
}

static void
free_focuspixmap(SPEC *sp)
{
    fl_free_pixmap(sp->focus_pixmap);
    fl_free_pixmap(sp->focus_mask);
    sp->focus_pixmap = None;
    sp->focus_mask = None;
}
/*
 * Change pixmap/mask. del==true means we want to destroy old pixmaps
 */
static void
change_pixmap(SPEC *sp, Window win, Pixmap p, Pixmap shape_mask, int del)
{
    PixmapSPEC *psp = sp->cspecv;

    if (del)
	free_pixmap(sp);
    else
    {
	cleanup_xpma_struct(psp->xpma);
	psp->xpma = 0;
    }

    sp->pixmap = p;
    sp->mask = shape_mask;

    M_warn("NewPixmap", "Pixmap=%ld mask=%ld", p, shape_mask);

    if (psp->gc == 0)
    {
	psp->gc = XCreateGC(flx->display, win, 0, 0);
	XSetGraphicsExposures(flx->display, psp->gc, False);
    }

#if !defined(USE_OVERRIDE)
    XSetClipMask(flx->display, psp->gc, sp->mask);
#endif

}

/*
 * Change pixmap/mask. del==true means we want to destroy old pixmaps
 */
static void
change_focuspixmap(SPEC *sp, Window win, Pixmap p, Pixmap shape_mask, int del)
{
    if (del)
	free_focuspixmap(sp);

    sp->focus_pixmap = p;
    sp->focus_mask = shape_mask;
}

static void
show_pixmap(FL_OBJECT * ob, int focus)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    PixmapSPEC *psp = sp->cspecv;
    int xx, yy;			/* position of bitmap */
    Pixmap pixmap, mask;
    int bits_w, bits_h, is_focus = (focus && sp->focus_pixmap);

    pixmap = (is_focus) ? sp->focus_pixmap : sp->pixmap;
    mask = (is_focus) ? sp->focus_mask : sp->mask;
    bits_w = (is_focus) ? psp->focus_w : sp->bits_w;
    bits_h = (is_focus) ? psp->focus_h : sp->bits_h;


    /* do nothing if empty data */
    if (sp->bits_w == 0 || pixmap == None)
    {
	fl_drw_text(FL_ALIGN_CENTER, ob->x, ob->y,
		    ob->w, ob->h, ob->lcol, ob->lstyle, FL_TINY_SIZE, "p");
	return;
    }

    fl_get_align_xy(psp->align, ob->x, ob->y, ob->w, ob->h, bits_w, bits_h,
		    FL_abs(ob->bw) + psp->dx, FL_abs(ob->bw) + psp->dy,
		    &xx, &yy);

#if !defined(USE_OVERRIDE)
    /* hopefully, XSetClipMask is smart */
    XSetClipMask(flx->display, psp->gc, mask);
    XSetClipOrigin(flx->display, psp->gc, xx, yy);
#endif

    XCopyArea(flx->display, pixmap, FL_ObjWin(ob),
	      psp->gc, 0, 0, bits_w, bits_h, xx, yy);
}

static int red_closeness = 40000;
static int green_closeness = 30000;
static int blue_closeness = 50000;
/*
 * basic attributes.
 */
static void
init_xpm_attributes(Window win, XpmAttributes * xpma, FL_COLOR tran)
{
    XWindowAttributes xwa;

    XGetWindowAttributes(flx->display, win, &xwa);
    xpma->valuemask = XpmVisual | XpmDepth | XpmColormap;
    xpma->depth = xwa.depth;
    xpma->visual = xwa.visual;
    xpma->colormap = xwa.colormap;

    xpma->valuemask |= XpmRGBCloseness;
    xpma->red_closeness = red_closeness;
    xpma->green_closeness = green_closeness;
    xpma->blue_closeness = blue_closeness;

#if (XpmRevision >= 7)
    xpma->valuemask |= XpmReturnPixels | XpmReturnAllocPixels;
#else
    xpma->valuemask |= XpmReturnPixels;
#endif

#ifdef USE_OVERRIDE
    {
	static XpmColorSymbol xpcm;
	xpcm.name = "None";
	xpcm.value = 0;
	xpcm.pixel = fl_get_flcolor(tran);
	xpma->valuemask |= XpmColorSymbols;
	xpma->colorsymbols = &xpcm;
	xpma->numsymbols = 1;
    }
#endif
}


/**********************************************************************
 * Static PIXMAP
 ******************************************************************{**/

static void
draw_pixmap(FL_OBJECT * ob)
{
    /* Draw the box */
    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h,
	       ob->col2, ob->bw);

    show_pixmap(ob, 0);
}


static int
handle_pixmap(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	      int key, void *ev)
{
    SPEC *sp;

#if (FL_DEBUG >= ML_DEBUG)
    M_info("HandlePixmap", fl_event_name(event));
#endif

    sp = ob->spec;

    switch (event)
    {
    case FL_DRAW:
	draw_pixmap(ob);
    case FL_DRAWLABEL:
	fl_draw_object_label(ob);
	break;
    case FL_FREEMEM:
	free_pixmap(ob->spec);
	if (((PixmapSPEC *) sp->cspecv)->gc)
	    XFreeGC(flx->display, ((PixmapSPEC *) sp->cspecv)->gc);
	fl_free(sp->cspecv);
	fl_free(ob->spec);
	break;
    }
    return 0;
}

FL_OBJECT *
fl_create_pixmap(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		 const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;
    PixmapSPEC *psp;

    ob = fl_make_object(FL_PIXMAP, type, x, y, w, h, label, handle_pixmap);
    ob->boxtype = FL_BITMAP_BOXTYPE;
    ob->col1 = FL_BITMAP_COL1;
    ob->col2 = FL_BITMAP_COL2;
    ob->lcol = FL_BITMAP_LCOL;
    ob->align = FL_BITMAP_ALIGN;
    ob->active = (type != FL_NORMAL_BITMAP);
    sp = ob->spec = fl_calloc(1, sizeof(SPEC));
    sp->bits_w = 0;
    psp = sp->cspecv = fl_calloc(1, sizeof(PixmapSPEC));
    psp->dx = psp->dy = 3;
    psp->align = FL_ALIGN_CENTER | FL_ALIGN_INSIDE;
    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_pixmap(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	      const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_pixmap(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}



Pixmap
fl_create_from_pixmapdata(Window win, char **data, unsigned *w, unsigned *h,
			  Pixmap * sm, int *hotx, int *hoty, FL_COLOR tran)
{
    Pixmap p = None;
    int s;

    /* this ensures we do not depend on the header/dl having the same size */
    xpmattrib = calloc(1, XpmAttributesSize());
    init_xpm_attributes(win, xpmattrib, tran);

    s = XpmCreatePixmapFromData(flx->display, win, data, &p, sm, xpmattrib);

    if (s != XpmSuccess)
    {
	errno = 0;
	M_err("CreateXPM", "error converting: %s",
	      (s == XpmOpenFailed ? "(Can't open)" :
	       (s == XpmFileInvalid ? "(Invalid file)" :
		(s == XpmColorFailed ? "(Can't get color)" : ""))));

	if (s < 0)
	    return None;
    }

    if (p != None)
    {
	*w = xpmattrib->width;
	*h = xpmattrib->height;
	if (hotx)
	    *hotx = xpmattrib->x_hotspot;
	if (hoty)
	    *hoty = xpmattrib->y_hotspot;
    }

    return p;
}

void
fl_set_pixmap_pixmap(FL_OBJECT * ob, Pixmap id, Pixmap mask)
{
    SPEC *sp;
    FL_Coord w = 0, h = 0;

    CHECK(ob, "SetPixmapId");
    sp = ob->spec;
    change_pixmap(sp, FL_ObjWin(ob), id, mask, 0);	/* 0 don't free old */
    if (sp->pixmap != None)
	fl_get_winsize(sp->pixmap, &w, &h);
    sp->bits_w = w;
    sp->bits_h = h;
    fl_redraw_object(ob);
}

Pixmap
fl_get_pixmap_pixmap(FL_OBJECT * ob, Pixmap * p, Pixmap * m)
{
    SPEC *sp;

    if (!IsValidClass(ob, FL_PIXMAP) && !IsValidClass(ob, FL_PIXMAPBUTTON))
    {
	Bark("GetPixmapPixmap", "%s is not Pixmap/pixmapbutton class",
	     ob ? ob->label : "");
	return None;
    }
    sp = ob->spec;

    /* pixmapbutton and pixmap use the same structure */
    *p = sp->pixmap;
    if (m)
	*m = sp->mask;
    return sp->pixmap;
}

/* generic routine to read a pixmap file. */

Pixmap
fl_read_pixmapfile(Window win, const char *file,
		   unsigned int *w, unsigned int *h,
		   Pixmap * shape_mask,
		   int *hotx, int *hoty, FL_COLOR tran)
{
    Pixmap p = 0;
    int s;

    xpmattrib = fl_calloc(1, XpmAttributesSize());
    init_xpm_attributes(win, xpmattrib, tran);

    s = XpmReadFileToPixmap(flx->display, win,
			    (char *) file, &p, shape_mask, xpmattrib);
    if (s != XpmSuccess)
    {
	errno = 0;
	M_err("LoadXPM", "error reading %s %s", file,
	      (s == XpmOpenFailed ? "(Can't open)" :
	       (s == XpmFileInvalid ? "(Invalid file)" :
		(s == XpmColorFailed ? "(Can't get color)" : ""))));

	if (s < 0)
	    return None;
    }

    *w = xpmattrib->width;
    *h = xpmattrib->height;

    if (hotx)
	*hotx = xpmattrib->x_hotspot;
    if (hoty)
	*hoty = xpmattrib->y_hotspot;

    return p;
}

void
fl_set_pixmap_file(FL_OBJECT * ob, const char *fname)
{
    Pixmap p = 0, shape_mask = 0;
    SPEC *sp;
    int hotx, hoty;
    Window win;

    if (!flx || !flx->display)
	return;

    sp = ob->spec;
    win = FL_ObjWin(ob) ? FL_ObjWin(ob) : fl_default_win();
    p = fl_read_pixmapfile(win, fname, &sp->bits_w, &sp->bits_h,
			   &shape_mask, &hotx, &hoty, ob->col1);

    if (p != None)
    {
	change_pixmap(sp, win, p, shape_mask, 0);
	((PixmapSPEC *) sp->cspecv)->xpma = xpmattrib;
	fl_redraw_object(ob);
    }

}

/******** End of static pixmap ************************}*/

/*****************************************************************
 *      Pixmap button
 ***********************************************************{****/

#define IsFlat(t) ((t)==FL_FLAT_BOX||(t)==FL_FRAME_BOX||(t)==FL_BORDER_BOX)

static void
draw_pixmapbutton(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    PixmapSPEC *psp = sp->cspecv;
    int absbw = FL_abs(ob->bw);
    int newbt = ob->boxtype, focus = 0;

    switch (sp->event)
    {
    case FL_ENTER:
	if (!psp->show_focus)
	    break;
	if (IsFlat(ob->boxtype) && !sp->val)
	{
	    fl_drw_box(FL_UP_BOX, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);
	    show_pixmap(ob, 1);
	}
	else if (ob->boxtype == FL_UP_BOX)
	{
	    if (absbw > 1)
	    {
		fl_color(fl_depth(fl_vmode) == 1 ? FL_BLACK : ob->col2);
		XDrawRectangle(flx->display, FL_ObjWin(ob),
			       flx->gc, ob->x, ob->y, ob->w - 1, ob->h - 1);
	    }

	    if (sp->focus_pixmap)
		show_pixmap(ob, 1);
	}
	break;
    case FL_LEAVE:
	if (!psp->show_focus)
	    break;
	if (IsFlat(ob->boxtype) && !sp->val)
	{
	    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h,
		       ob->col1, ob->bw);
	    show_pixmap(ob, 0);
	}
	else if (ob->boxtype == FL_UP_BOX)
	{
	    if (ob->bw >= 1)
	    {
		fl_color(FL_BLACK);
		XDrawRectangle(flx->display, FL_ObjWin(ob),
			       flx->gc, ob->x, ob->y, ob->w - 1, ob->h - 1);
	    }
	    else
		/* need to undo the focus outline */
	    {
		fl_drw_box(sp->val ? FL_TO_DOWNBOX(ob->boxtype) : ob->boxtype,
			   ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);
	    }
	    show_pixmap(ob, sp->val);
	}
	break;
    default:
	focus = sp->val || ob->belowmouse;
	if (IsFlat(ob->boxtype))
	{
	    if (sp->val)
		newbt = FL_DOWN_BOX;
	    else if (ob->belowmouse)
		newbt = FL_UP_BOX;
	    else
		newbt = ob->boxtype;
	    fl_drw_box(newbt, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);
	}
	else if (FL_IS_UPBOX(ob->boxtype))
	{
	    if (sp->val)
		fl_drw_box(FL_TO_DOWNBOX(ob->boxtype),
			   ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

	    else
		fl_drw_box(ob->boxtype,
			   ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

	    if (ob->belowmouse && psp->show_focus && absbw > 1)
	    {
		fl_color(fl_depth(fl_vmode) == 1 ? FL_BLACK : ob->col2);
		XDrawRectangle(flx->display, FL_ObjWin(ob),
			       flx->gc, ob->x, ob->y, ob->w - 1, ob->h - 1);
	    }
	}
	else
	    fl_drw_box(ob->boxtype,
		       ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

	show_pixmap(ob, focus);
	break;
    }

    fl_draw_object_label(ob);
}

/* button driver will clean up spec after this function returns */
static void
cleanup_pixmapbutton(SPEC *sp)
{
    PixmapSPEC *psp = sp->cspecv;

    if (psp->gc)
    {
	XFreeGC(flx->display, psp->gc);
	psp->gc = 0;
    }

    if (psp->xpma)
    {
	cleanup_xpma_struct(psp->xpma);
	psp->xpma = 0;
    }

    if (sp->cspecv)
    {
	fl_free(sp->cspecv);
	sp->cspecv = 0;
    }
}

FL_OBJECT *
fl_create_pixmapbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		       const char *label)
{
    FL_OBJECT *ob;
    static int class_init;
    PixmapSPEC *psp;

    if (!class_init)
    {
	fl_add_button_class(FL_PIXMAPBUTTON, draw_pixmapbutton, cleanup_pixmapbutton);
	class_init = 1;
    }

    ob = fl_create_generic_button(FL_PIXMAPBUTTON, type, x, y, w, h, label);
    ob->boxtype = FL_PIXMAPBUTTON_BOXTYPE;
    ob->col1 = FL_PIXMAPBUTTON_COL1;
    ob->col2 = FL_PIXMAPBUTTON_COL2;
    ob->align = FL_PIXMAPBUTTON_ALIGN;
    ob->lcol = FL_PIXMAPBUTTON_LCOL;
    psp = ((SPEC *) ob->spec)->cspecv = fl_calloc(1, sizeof(PixmapSPEC));
    psp->show_focus = 1;
    psp->align = FL_ALIGN_CENTER | FL_ALIGN_INSIDE;
    psp->dx = psp->dy = 3;
    return ob;
}

FL_OBJECT *
fl_add_pixmapbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		    const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_pixmapbutton(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}

void
fl_set_pixmap_data(FL_OBJECT * ob, char **bits)
{
    SPEC *sp;
    Window win;
    Pixmap p, shape_mask = 0;
    int hx, hy;

    CHECK(ob, "SetPixmapData");

    if (!flx->display)
	return;

    sp = ob->spec;
    win = FL_ObjWin(ob) ? FL_ObjWin(ob) : fl_default_win();
    p = fl_create_from_pixmapdata(win, bits, &sp->bits_w, &sp->bits_h,
				  &shape_mask, &hx, &hy, ob->col1);

    if (p != None)
    {
	change_pixmap(sp, win, p, shape_mask, 0);
	((PixmapSPEC *) sp->cspecv)->xpma = xpmattrib;
    }

    fl_redraw_object(ob);
}

void
fl_set_pixmap_colorcloseness(int red, int green, int blue)
{
    red_closeness = red;
    green_closeness = green;
    blue_closeness = blue;
}


/****************** End of pixmap stuff ************}**/

void
fl_set_pixmap_align(FL_OBJECT * ob, int align, int xmargin, int ymargin)
{
    SPEC *sp;
    PixmapSPEC *psp;

    CHECK(ob, "SetPixmapAlign");
    sp = ob->spec;
    psp = sp->cspecv;
    if (align != psp->align || xmargin != psp->dx || ymargin != psp->dy)
    {
	psp->align = align;
	psp->dx = xmargin;
	psp->dy = ymargin;
	fl_redraw_object(ob);
    }
}


void
fl_set_pixmapbutton_focus_pixmap(FL_OBJECT * ob, Pixmap id, Pixmap mask)
{
    SPEC *sp = ob->spec;
    PixmapSPEC *psp = sp->cspecv;
    int w, h;

    change_focuspixmap(sp, FL_ObjWin(ob), id, mask, 0);
    if (sp->focus_pixmap != None)
    {
	fl_get_winsize(sp->focus_pixmap, &w, &h);
	psp->focus_w = w;
	psp->focus_h = h;
    }
}

void
fl_set_pixmapbutton_focus_data(FL_OBJECT * ob, char **bits)
{
    SPEC *sp;
    PixmapSPEC *psp;
    Window win;
    Pixmap p, shape_mask = 0;
    int hx, hy;

    CHECK(ob, "SetPixmapData");

    if (!flx->display)
	return;

    sp = ob->spec;
    psp = sp->cspecv;
    win = FL_ObjWin(ob) ? FL_ObjWin(ob) : fl_default_win();
    p = fl_create_from_pixmapdata(win, bits, &psp->focus_w, &psp->focus_h,
				  &shape_mask, &hx, &hy, ob->col1);

    if (p != None)
    {
	change_focuspixmap(sp, win, p, shape_mask, 0);
	((PixmapSPEC *) sp->cspecv)->xpma = xpmattrib;
    }
}

void
fl_set_pixmapbutton_focus_file(FL_OBJECT * ob, const char *fname)
{
    Pixmap p = 0, shape_mask = 0;
    SPEC *sp;
    int hotx, hoty;
    Window win;
    PixmapSPEC *psp;

    if (!flx->display)
	return;

    sp = ob->spec;
    psp = sp->cspecv;
    win = FL_ObjWin(ob) ? FL_ObjWin(ob) : fl_default_win();
    p = fl_read_pixmapfile(win, fname, &psp->focus_w, &psp->focus_h,
			   &shape_mask, &hotx, &hoty, ob->col1);

    if (p != None)
	change_focuspixmap(sp, win, p, shape_mask, 0);
}


void
fl_set_pixmapbutton_focus_outline(FL_OBJECT * ob, int yes)
{
    SPEC *sp;
    PixmapSPEC *psp;

    CHECK(ob, "SetPixmapFocus");
    sp = ob->spec;
    psp = sp->cspecv;

    psp->show_focus = yes;
}


void
fl_free_pixmap_pixmap(FL_OBJECT * ob)
{
    CHECK(ob, "FreePixmapPixmap");
    free_pixmap(ob->spec);
}


/* this can't go into forms.c as it will pull xpm into
 * programs that don't need it
 */
void
fl_set_form_icon_data(FL_FORM * form, char *data[])
{
    Pixmap p, s;
    unsigned int j;

    p = fl_create_from_pixmapdata(fl_root, data, &j, &j, &s, 0, 0, 0);
    fl_set_form_icon(form, p, s);
}
