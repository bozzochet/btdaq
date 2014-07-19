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
 * $Id: bitmap.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *  FL_BITMAP & FL_BITMAPBUTTON class.
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_bmp = "$Id: bitmap.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"

typedef FL_BUTTON_STRUCT SPEC;

static void
free_bitmap(SPEC *p)
{
    if (p->pixmap)
	XFreePixmap(flx->display, p->pixmap);
    p->pixmap = 0;
}

/********************************************************************
 * static bitmap
 ******************************************************************{*/

static void
drawit(Window win, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
       FL_COLOR fcol, FL_COLOR bcol, Pixmap bitmap)
{
    fl_color(fcol);
    fl_bk_color(bcol);
    XCopyPlane(flx->display, bitmap, win, flx->gc, 0, 0, w, h, x, y, 1);
}

static void
draw_bitmap(FL_OBJECT * ob)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    int xx, yy;			/* position of bitmap */

    /* Draw the box */
    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h,
	       ob->col1, ob->bw);

    /* do nothing is not empty data */
    if (sp->bits_w == 0 || !sp->pixmap)
	return;


    /* Calculate position so the bitmap is centered */
    xx = (int) (ob->x + (ob->w - sp->bits_w) / 2);
    yy = (int) (ob->y + (ob->h - sp->bits_h) / 2);

    drawit(FL_ObjWin(ob), xx, yy, sp->bits_w, sp->bits_h,
	   ob->lcol, ob->col1, sp->pixmap);
}



/* Handles an event, returns whether value has changed. */
static int
handle_it(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	  int key, void *ev)
{
    switch (event)
    {
    case FL_DRAW:
	draw_bitmap(ob);
    case FL_DRAWLABEL:
	fl_draw_object_label(ob);
	break;
    case FL_FREEMEM:
	free_bitmap(ob->spec);
	fl_free(ob->spec);
	break;
    }
    return 0;
}

/* Creates an object */
FL_OBJECT *
fl_create_bitmap(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		 const char *label)
{
    FL_OBJECT *ob;

    ob = fl_make_object(FL_BITMAP, type, x, y, w, h, label, handle_it);
    ob->boxtype = FL_BITMAP_BOXTYPE;
    ob->col1 = FL_BITMAP_COL1;
    ob->col2 = FL_BITMAP_COL2;
    ob->lcol = FL_BITMAP_LCOL;
    ob->align = FL_BITMAP_ALIGN;
    ob->active = (type != FL_NORMAL_BITMAP);
    ob->spec = fl_calloc(1, sizeof(SPEC));
    ((SPEC *) (ob->spec))->bits_w = 0;
    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_bitmap(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	      const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_bitmap(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}

/* Fills the bitmap with a bitmap. */
void
fl_set_bitmap_data(FL_OBJECT * ob, int w, int h, unsigned char *data)
{
    SPEC *sp;
    Pixmap p;

    if (ob == NULL || ob->objclass != FL_BITMAP)
	return;

    /* only occurs with fdesign -convert */
    if (!flx->display)
	return;

    sp = ((SPEC *) (ob->spec));

    p = XCreateBitmapFromData(flx->display,
			      FL_ObjWin(ob) ? FL_ObjWin(ob) : fl_root,
			      (char *) data, w, h);

    if (p == None)
    {
	M_err("BitmapData", "Can't create bitmap");
	return;
    }

    sp->bits_w = w;
    sp->bits_h = h;
    sp->pixmap = p;

    fl_redraw_object(ob);
}

Pixmap
fl_read_bitmapfile(Window win, const char *file,
		   unsigned int *w, unsigned int *h, int *hotx, int *hoty)
{
    Pixmap p = None;
    int status;

    status = XReadBitmapFile(flx->display, win, (char *) file,
			     w, h, &p, hotx, hoty);

    if (status != BitmapSuccess)
	M_err("ReadBitmap", "%s: %s", file,
	      status == BitmapFileInvalid ? "Invalid file" : "Can't read");
    return p;
}


void
fl_set_bitmap_file(FL_OBJECT * ob, const char *fname)
{
    int xhot, yhot;
    Pixmap p;
    SPEC *sp = ob->spec;

    if (!flx->display)
	return;

    if (ob == NULL || ob->objclass != FL_BITMAP)
    {
	M_err("set_bitmap_file", "object %s not bitmap class", ob ? ob->label : "null");
	return;
    }

    p = fl_read_bitmapfile(FL_ObjWin(ob) ? FL_ObjWin(ob) : fl_root,
			 fname, &(sp->bits_w), &(sp->bits_h), &xhot, &yhot);
    if (p != None)
    {
	free_bitmap(sp);
	sp->pixmap = p;
    }
    fl_redraw_object(ob);
}

/***** End of static BITMAP ************************/


/*******************************************************************
 * BITMAP buttons
 ****************************************************************{*/

static void
draw_bitmapbutton(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    unsigned long fcol;
    int x, y;

    if (FL_IS_UPBOX(ob->boxtype) && sp->val)
	fl_drw_box(FL_TO_DOWNBOX(ob->boxtype), ob->x, ob->y, ob->w, ob->h,
		   ob->col1, ob->bw);
    else
	fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

    if (sp->pixmap != None)
    {

	x = (int) (ob->x + (ob->w - sp->bits_w) / 2);
	y = (int) (ob->y + (ob->h - sp->bits_h) / 2);

	fcol = ob->belowmouse ? ob->col2 : ob->lcol;
	drawit(FL_ObjWin(ob), x, y, sp->bits_w, sp->bits_h,
	       fcol, ob->col1, sp->pixmap);
    }

    fl_draw_object_label(ob);

}

FL_OBJECT *
fl_create_bitmapbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		       const char *label)
{
    FL_OBJECT *ob;

    fl_add_button_class(FL_BITMAPBUTTON, draw_bitmapbutton, 0);
    ob = fl_create_generic_button(FL_BITMAPBUTTON, type, x, y, w, h, label);
    ob->boxtype = FL_BITMAPBUTTON_BOXTYPE;
    ob->col1 = FL_BITMAPBUTTON_COL1;
    ob->col2 = FL_BITMAPBUTTON_COL2;
    ob->align = FL_BITMAPBUTTON_ALIGN;
    ob->lcol = FL_BITMAP_LCOL;
    return ob;
}

FL_OBJECT *
fl_add_bitmapbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		    const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_bitmapbutton(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}

void
fl_set_bitmapbutton_data(FL_OBJECT * ob, int w, int h, unsigned char *bits)
{
    SPEC *sp;
    Window win;

    if (!ob || ob->objclass != FL_BITMAPBUTTON)
	return;

    win = FL_ObjWin(ob) ? FL_ObjWin(ob) : fl_root;

    sp = ob->spec;
    free_bitmap(sp);
    sp->bits_w = w;
    sp->bits_h = h;

    sp->pixmap = XCreateBitmapFromData(flx->display, win,
				     (char *) bits, sp->bits_w, sp->bits_h);

    fl_redraw_object(ob);
}

void
fl_set_bitmapbutton_file(FL_OBJECT * ob, const char *file)
{
    SPEC *sp;
    int hx, hy;

    if (!flx->display)
	return;

    if (!ob || ob->objclass != FL_BITMAPBUTTON)
	return;

    sp = ob->spec;
    sp->pixmap = fl_read_bitmapfile(FL_ObjWin(ob) ? FL_ObjWin(ob) : fl_root,
			      file, &(sp->bits_w), &(sp->bits_h), &hx, &hy);
    fl_redraw_object(ob);
}

/************* End of BITMAP BUTTON **************************}****/
