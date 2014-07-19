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
 * $Id: xsupport.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 *  Form initialization and Windowing support.
 *
 *  Further isolation of dependencies of FORMS on underlining window
 *  systems.
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_xsupt = "$Id: xsupport.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"

/* for debugging only */
void
fl_check_key_focus(const char *s, Window win)
{
    int r;
    Window w;

    if (fl_cntl.debug > 1)
    {
	XGetInputFocus(flx->display, &w, &r);
	M_info("KBDFocus", "%s:%s FWin=%lu ReqW=%lu",
	       s ? s : "", w == win ? "OK" : "Wrong", w, win);
    }
}


/*****************************************************************
 * Pointer query routines
 ***********************************************************{*****/

/*
 * Return the window ID mouse currently is in and sets the mouse location
 * relative to root
 */

Window
fl_get_mouse(FL_Coord * x, FL_Coord * y, unsigned *keymask)
{
    Window rjunk, childwin;
    int cx, cy, xx, yy;

    XQueryPointer(flx->display, fl_root, &rjunk, &childwin,
		  &xx, &yy, &cx, &cy, keymask);

    *x = xx;
    *y = yy;
    return childwin;
}

/*
 * find the mouse position relative to win and return the child win
 * the mouse is currently in
 */

Window
fl_get_win_mouse(Window win, FL_Coord * x, FL_Coord * y, unsigned *keymask)
{
    Window rjunk, childwin;
    int xx, yy, ix, iy;

    XQueryPointer(flx->display, win, &rjunk, &childwin,
		  &xx, &yy, &ix, &iy, keymask);
    *x = ix;
    *y = iy;
    return childwin;
}


Window
fl_get_form_mouse(FL_FORM * fm, FL_Coord * x, FL_Coord * y, unsigned *keymask)
{
    if (fl_is_good_form(fm))
    {
	Window win;
	FL_pixmap *flp = fm->flpixmap;
	win = (flp && flp->win > 0) ? flp->win : fm->window;
	fl_get_win_mouse(win, x, y, keymask);
	return win;
    }
    return 0;
}

/* warp mouse to (mx, my) relative to root window */
void
fl_set_mouse(FL_Coord mx, FL_Coord my)
{
    XWarpPointer(flx->display, None, fl_root, 0, 0, 0, 0, mx, my);
}


/*** End of pointer query routines ******************/

Pixmap fl_gray_pattern[3];
GC fl_bwgc[3];
GC fl_whitegc;
GC fl_drawgc[18];

static unsigned char gray40_bits[] =
{
    0xee, 0xbb, 0xee, 0xbb, 0xee, 0xbb, 0xee, 0xbb
};
static unsigned char gray50_bits[] =
{
    0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55
};
static unsigned char gray60_bits[] =
{
    0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44
};

void
fl_init_stipples(void)
{
    if (!fl_gray_pattern[0])
    {
	fl_gray_pattern[0] = XCreateBitmapFromData(flx->display, fl_root,
						(char *) gray40_bits, 8, 8);
	fl_gray_pattern[1] = XCreateBitmapFromData(flx->display, fl_root,
						(char *) gray50_bits, 8, 8);
	fl_gray_pattern[2] = XCreateBitmapFromData(flx->display, fl_root,
						(char *) gray60_bits, 8, 8);
    }
}

/*
 * must NOT allow object pixmap and form pixmap active at the
 * same time
 */

static void
change_drawable(FL_pixmap * p, FL_OBJECT * ob)
{
    p->x = ob->x;
    p->y = ob->y;
    p->win = FL_ObjWin(ob);
    ob->form->window = p->pixmap;
    ob->x = 0;
    ob->y = 0;
    fl_winset(p->pixmap);
}


static void
change_form_drawable(FL_pixmap * p, FL_FORM * fm)
{
    p->x = fm->x;
    p->y = fm->y;
    p->win = fm->window;
    fm->window = p->pixmap;
    fm->x = 0;
    fm->y = 0;
    fl_winset(p->pixmap);
}

static int
fl_xerror_handler(Display * d, XErrorEvent * xev)
{
    if (xev->error_code == BadAlloc)
	M_err("XErrorHandler", "XError: can't allocate - ignored ");
    else
	M_err("XErrorHandler", "XError: %d", xev->error_code);
    return 0;

}

/* non-sqaure box can't be double buffered */
#if 0
#define NON_SQB  FL_IS_NONSQRBOX
#else
#define NON_SQB(a) (a == FL_NO_BOX)
#endif

static int (*oldhandler) (Display *, XErrorEvent *);

/* Pixmap support   */
void
fl_create_object_pixmap(FL_OBJECT * ob)
{
    Window root;
    unsigned junk;
    FL_pixmap *p;
    int i, fdouble;

    /* check to see if we need to create. ShadowBOX can't be used as the left
       corner is not painted in anyway and it is not easy to figure out the
       object color beneath the object we are trying to paint */

    fdouble = ob->form->use_pixmap && ob->form->flpixmap &&
	((FL_pixmap *) (ob->form->flpixmap))->win;

    if (fdouble || NON_SQB(ob->boxtype) || !ob->use_pixmap)
	return;

    if (!(p = ob->flpixmap))
	p = ob->flpixmap = fl_calloc(1, sizeof(FL_pixmap));


    if (p->pixmap && (p->w == ob->w && p->h == ob->h &&
		      p->depth == fl_depth(fl_vmode) &&
		      p->visual == fl_visual(fl_vmode) &&
		      p->dbl_background == ob->dbl_background &&
		      p->pixel == fl_get_pixel(ob->dbl_background)))
    {
	change_drawable(p, ob);
	return;
    }

    if (p->pixmap)
	XFreePixmap(flx->display, p->pixmap);

    oldhandler = XSetErrorHandler(fl_xerror_handler);

    p->pixmap = XCreatePixmap(flx->display, FL_ObjWin(ob),
		    (unsigned) ob->w, (unsigned) ob->h, fl_depth(fl_vmode));

    fl_winset(p->pixmap);
    fl_rectf(0, 0, ob->w, ob->h, ob->dbl_background);

    M_info("ObjPixmap", "Creating depth=%d for %s",
	   fl_depth(fl_vmode), ob->label);

    /* make sure it succeeds by forcing a two way request */
    if (fl_cntl.safe && !XGetGeometry(flx->display, p->pixmap, &root, &i, &i,
				      &junk, &junk, &junk, &junk))
    {
	M_err("ObjPixmap", "Can't create");
	p->pixmap = 0;
	return;
    }

    XSetErrorHandler(oldhandler);

    p->w = ob->w;
    p->h = ob->h;
    p->depth = fl_depth(fl_vmode);
    p->visual = fl_visual(fl_vmode);
    p->dbl_background = ob->dbl_background;
    p->pixel = fl_get_pixel(ob->dbl_background);
    change_drawable(p, ob);
}

void
fl_show_object_pixmap(FL_OBJECT * ob)
{
    FL_pixmap *p = ob->flpixmap;

    if (!p || !p->pixmap || !p->win)
	return;

    if (!NON_SQB(ob->boxtype))
    {
	XCopyArea(flx->display, p->pixmap, p->win, flx->gc,
		  0, 0, p->w, p->h, p->x, p->y);
	ob->x = p->x;
	ob->y = p->y;
	fl_winset(p->win);
	ob->form->window = p->win;
	p->win = 0;
	/* now handle the label */
	fl_handle_object(ob, FL_DRAWLABEL, 0, 0, 0, 0);
    }
}

void
fl_free_flpixmap(FL_pixmap * p)
{
    if (p && p->pixmap)
    {
	XFreePixmap(flx->display, p->pixmap);
	p->pixmap = 0;
    }
}

/* Pixmap support   */
static int
form_pixmapable(FL_FORM * fm)
{

    FL_OBJECT *first, *second;

    if (!fm->use_pixmap)
	return 0;

    /* check to see if we need to create. ShadowBOX can't be used as the left
       corner is not painted in anyway and it is not easy to figure out the
       object color beneath the object we are trying to paint */

    first = fm->first;
    second = first ? first->next : 0;

    /* some times a form can have a fake NO_BOX as the first object */
    if (!first || (first->boxtype != FL_NO_BOX && NON_SQB(first->boxtype)))
	return 0;

    if (first->boxtype == FL_NO_BOX && second && NON_SQB(second->boxtype))
	return 0;

    return 1;
}

void
fl_create_form_pixmap(FL_FORM * fm)
{
    Window root;
    unsigned junk;
    FL_pixmap *p;
    int i;


    if (!form_pixmapable(fm))
	return;

    if (!(p = fm->flpixmap))
	p = fm->flpixmap = fl_calloc(1, sizeof(FL_pixmap));


    if (p->pixmap && (p->w == fm->w && p->h == fm->h &&
		      p->depth == fl_depth(fl_vmode) &&
		      p->visual == fl_visual(fl_vmode)))
    {
	change_form_drawable(p, fm);
	return;
    }

    if (p->pixmap)
	XFreePixmap(flx->display, p->pixmap);

    oldhandler = XSetErrorHandler(fl_xerror_handler);

    p->pixmap = XCreatePixmap(flx->display, fm->window,
		    (unsigned) fm->w, (unsigned) fm->h, fl_depth(fl_vmode));

    M_info("FormPixmap", "creating(w=%d h=%d)", fm->w, fm->h);

    /* make sure it succeeds by forcing a two way request */

    if (!XGetGeometry(flx->display, p->pixmap, &root, &i, &i,
		      &junk, &junk, &junk, &junk))
    {
	M_warn("FormPixmap", "Can't create pixmap");
	p->pixmap = 0;
	return;
    }

    XSetErrorHandler(oldhandler);

    p->w = fm->w;
    p->h = fm->h;
    p->depth = fl_depth(fl_vmode);
    p->visual = fl_visual(fl_vmode);
    change_form_drawable(p, fm);

    M_info("FormPixmap", "Creation Done");
}

void
fl_show_form_pixmap(FL_FORM * fm)
{
    FL_pixmap *p;

    if (!form_pixmapable(fm))
	return;

    if (!(p = fm->flpixmap) || !p->pixmap || !p->win)
	return;

    XCopyArea(flx->display, p->pixmap, p->win, flx->gc, 0, 0, p->w, p->h, 0, 0);

    fm->x = p->x;
    fm->y = p->y;
    fm->window = p->win;
    fl_winset(p->win);
    p->win = 0;
}


int
fl_doublebuffer_capable(int warn)
{
    return 1;
}

/********  VisualClass name *************{*/

#define VNP(a)   {a,#a}

static FL_VN_PAIR xvclass[] =
{
    VNP(PseudoColor), VNP(TrueColor), VNP(DirectColor), VNP(StaticColor),
    VNP(GrayScale), VNP(GreyScale), VNP(StaticGray), VNP(StaticGrey),
    {FL_DefaultVisual, "DefaultVisual"},
    {FL_DefaultVisual, "default"},
    {FL_DefaultVisual, "Default"},
    {FL_illegalVisual, "XInvalidClass"},
    {-1, "Invalid"}
};

const char *
fl_vclass_name(int n)
{
    return fl_get_vn_name(xvclass, n);
}

/* fl_get_vn_val can't be used. caller relies on the returning
 * of FL_illegalVisual
 */
int
fl_vclass_val(const char *v)
{
    FL_VN_PAIR *vn = xvclass;
    for (; vn->val >= 0 && v; vn++)
	if (strcmp(vn->name, v) == 0)
	    return vn->val;
    return FL_illegalVisual;
}


/******** End of vclass_name **********}****/
