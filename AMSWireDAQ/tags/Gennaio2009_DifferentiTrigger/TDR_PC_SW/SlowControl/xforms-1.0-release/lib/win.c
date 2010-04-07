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
 * $Id: win.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao 
 *  All rights reserved.
 *.
 *
 *
 * To isolate dependencies of XForms on the window system, we provide
 * some system-neutual windowing services. It is expected that all
 * XForms internal windowing will be done using these.
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_xsupt = "$Id: win.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"

/*********************************************************************
 *
 * Windowing support.
 * Geometry preference, opening/closing windows and geometry queries
 *
 * winclose is in appwin.c
 *
 ****************************************************************{****/

static XSetWindowAttributes st_xswa;
static XSizeHints st_xsh;
static XWMHints st_xwmh;
static unsigned int st_wmask;
static int st_wmborder;
static unsigned bwidth = 0;


/*********************************************************************
 * Default window attributes. Subject to pref_winsize and its friends
 **********************************************************************/

void
fl_default_xswa(void)
{
    /* OwnerGrab is needed for pop-up to work correctly */
    st_xswa.event_mask = (ExposureMask | KeyPressMask | KeyReleaseMask |
			  ButtonPressMask | ButtonReleaseMask |
			  OwnerGrabButtonMask |
			  EnterWindowMask | LeaveWindowMask |
			  ButtonMotionMask | PointerMotionMask |
			  PointerMotionHintMask |
    /* VisibilityChangeMask| PropertyChangeMask | */
			  StructureNotifyMask);

 
    /* for input method */
    if(fl_context->xic)
       st_xswa.event_mask |= FocusChangeMask;

    st_xswa.backing_store = fl_cntl.backingStore;
    st_wmask = CWEventMask | CWBackingStore;

    /* border_pixel must be set for 24bit TrueColor displays */

    st_xswa.border_pixel = 0;
    st_wmask |= CWBorderPixel;

    st_xsh.flags = 0;

    /* default size */
    st_xsh.width = st_xsh.base_width = 320;
    st_xsh.height = st_xsh.base_height = 200;

    /* border */
    st_wmborder = FL_FULLBORDER;

    /* keyboard focus. Need window manager's help  */
    st_xwmh.input = True;
    st_xwmh.initial_state = NormalState;
    st_xwmh.flags = InputHint | StateHint;
}


/************** Window sizes ******************{**/

/*  Open window with this size */
void
fl_initial_winsize(FL_Coord w, FL_Coord h)
{
    st_xsh.width = st_xsh.base_width = w;
    st_xsh.height = st_xsh.base_height = h;
    st_xsh.flags |= USSize;
}

void
fl_initial_winstate(int state)
{
    st_xwmh.initial_state = state;
    st_xwmh.flags |= StateHint;
}

void
fl_winicon(Window win, Pixmap p, Pixmap m)
{
    XWMHints lxwmh, *xwmh;

    lxwmh.flags = 0;
    xwmh = win ? &lxwmh : &st_xwmh;
    xwmh->icon_pixmap = p;
    xwmh->icon_mask = m;
    xwmh->flags |= IconPixmapHint | IconMaskHint;
    if (win)
	XSetWMHints(flx->display, win, xwmh);

}

/*
 * Open window with this size and KEEP it this size if window manager
 * coorporates
 */
void
fl_winsize(FL_Coord w, FL_Coord h)
{
    fl_initial_winsize(w, h);

    /* try to disable interactive resizing */
    st_xsh.min_width = st_xsh.max_width = w;
    st_xsh.min_height = st_xsh.max_height = h;
    st_xsh.flags |= (PMinSize | PMaxSize);
}

/*
 * Set a limit to the minimum size a window can take. Can be used
 * while a window is visible. If window is not visible, we take
 * the request to mean a constraint for future windows.
 */
void
fl_winminsize(Window win, FL_Coord w, FL_Coord h)
{
    XSizeHints mxsh, *sh;

    /* copy current constraints */
    mxsh = st_xsh;
    mxsh.flags = 0;
    sh = win ? &mxsh : &st_xsh;
    sh->min_width = w;
    sh->min_height = h;
    sh->flags |= PMinSize;
    if (win)
	XSetWMNormalHints(flx->display, win, sh);
}

void
fl_winmaxsize(Window win, FL_Coord w, FL_Coord h)
{
    XSizeHints mxsh, *sh;

    mxsh = st_xsh;
    mxsh.flags = 0;
    sh = win ? &mxsh : &st_xsh;
    sh->max_width = w;
    sh->max_height = h;
    sh->flags |= PMaxSize;
    if (win)
	XSetWMNormalHints(flx->display, win, sh);
}

void
fl_winstepunit(Window win, FL_Coord dx, FL_Coord dy)
{
    XSizeHints mxsh, *sh;

    /* copy current constraints */
    mxsh = st_xsh;
    mxsh.flags = 0;
    sh = win ? &mxsh : &st_xsh;
    sh->width_inc = dx;
    sh->height_inc = dy;
    sh->flags |= PResizeInc;
    if (win)
	XSetWMNormalHints(flx->display, win, sh);
}

/******* End of basic win size routines **********}***/


/******* Window position routines **************{***/

void
fl_winposition(FL_Coord x, FL_Coord y)
{
    st_xsh.x = x;
    st_xsh.y = y;
    st_xsh.flags |= fl_wmstuff.pos_request;
}

/****** End of window positioning routines ******}*/


/***** Window position and size **************{****/

void
fl_initial_wingeometry(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h)
{
    fl_winposition(x, y);
    fl_initial_winsize(w, h);
}

void
fl_wingeometry(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h)
{
    fl_winposition(x, y);
    fl_winsize(w, h);
}

/***** End of geometry preference routine *******}**/

/***** Misc. windowing routines *****************{*/

/* try to fix the aspect ration */
void
fl_winaspect(Window win, FL_Coord x, FL_Coord y)
{
    float fact;
    XSizeHints lxsh, *xsh;

    if (x == 0 || y == 0)
    {
	M_err("Aspect", "Bad aspect ratio");
	return;
    }

    lxsh.flags = 0;
    xsh = win ? &lxsh : &st_xsh;

    xsh->flags |= PAspect;
    xsh->min_aspect.x = x;
    xsh->min_aspect.y = y;
    xsh->max_aspect.x = x;
    xsh->max_aspect.y = y;

    xsh->base_width = xsh->width = x;
    xsh->base_height = xsh->height = y;

    if (xsh->base_width < 100 || xsh->base_height < 100)
    {
	fact = 100 / FL_max(x, y);
	xsh->base_width *= fact;
	xsh->base_height *= fact;
    }
    if (win)
	XSetNormalHints(flx->display, win, xsh);
}


void
fl_noborder(void)
{
    st_wmborder = FL_NOBORDER;
}

void
fl_transient(void)
{
    st_wmborder = FL_TRANSIENT;
}

void
fl_winmove(Window win, int dx, int dy)
{
    if (win)
	XMoveWindow(flx->display, win, dx, dy);
    else
	fl_winposition(dx, dy);
}

void
fl_winreshape(Window win, int dx, int dy, int w, int h)
{
    if (win)
	XMoveResizeWindow(flx->display, win, dx, dy, w, h);
    else
    {
	fl_winresize(win, w, h);
	fl_winmove(win, dx, dy);
    }
}

/***** End of misc. windowing routines **********}*/


/********* Window geometry query routines ********{*/

void
fl_get_winsize(Window win, FL_Coord * w, FL_Coord * h)
{
    unsigned int ww, hh, bjunk, djunk;
    int xx, yy;
    Window root;

    XGetGeometry(flx->display, win, &root, &xx, &yy, &ww, &hh, &bjunk, &djunk);
    *w = ww;
    *h = hh;
}

void
fl_get_winorigin(Window win, FL_Coord * x, FL_Coord * y)
{
    int xx, yy;
    unsigned int ww, hh, bw, d;
    Window root;

    XGetGeometry(flx->display, win, &root, &xx, &yy, &ww, &hh, &bw, &d);
    XTranslateCoordinates(flx->display, win, root,
			  -((int) bw), -((int) bw), &xx, &yy, &root);
    *x = xx;
    *y = yy;
}

void
fl_get_wingeometry(Window win, FL_Coord * x, FL_Coord * y,
		   FL_Coord * w, FL_Coord * h)
{
    int xx, yy;
    unsigned int ww, hh, bw, d;
    Window root;

    XGetGeometry(flx->display, win, &root, &xx, &yy, &ww, &hh, &bw, &d);
    XTranslateCoordinates(flx->display, win, root,
			  -((int) bw), -((int) bw), &xx, &yy, &root);
    *x = xx;
    *y = yy;
    *w = ww;
    *h = hh;
}

/***** End of window geometry query routines ********}*/


/******* Open window etc ***********************/

/* if one of the form is destoryed, we want to know about it */

/* All window notices the Close window manager command */
static void
setup_catch_destroy(Window win)
{
    static Atom atom_delete_win;
    static Atom atom_protocols;

    if (!atom_delete_win)
	atom_delete_win = XInternAtom(flx->display, "WM_DELETE_WINDOW", 0);

    if (!atom_protocols)
	atom_protocols = XInternAtom(flx->display, "WM_PROTOCOLS", 0);

    XChangeProperty(flx->display, win, atom_protocols, XA_ATOM, 32,
		    PropModeReplace, (unsigned char *) &atom_delete_win, 1);
}

/*
 * get window manager's decoration size. Caller should check to make sure
 * not to call it when the size is already known as XGetGeometry is used
 * everytime here
 */

static void
get_wm_decoration(const char *what, XEvent * xrp,
		  FL_Coord * x, FL_Coord * y, FL_Coord * bw)
{
    Window junk;
    unsigned uj, rbw;

    if (!*y)
    {
	*x = xrp->xreparent.x;
	*y = xrp->xreparent.y;
	if (*y)
	{
	    int xx, yy;
	    XGetGeometry(flx->display, xrp->xreparent.parent, &junk,
			 &xx, &yy, &uj, &uj, &rbw, &uj);
	    *bw = rbw;
	}
    }

    M_warn("Reparent", "%s x=%d y=%d bw=%d", what, (int) *x, (int) *y, *bw);

    /* window manager tends to lie about reparenting. Can't trust them */
    if (!*y && !*bw)
    {
	/* Querying the geometry of xreparent.window really should work, but
	   it does not, resulting x,y==0 most of the time. Maybe WM decorates
	   with multiple windows. Anyway, querying with xrp->xreparent.parent
	   works most of the time. If it does not work (single parent ?), the
	   last resort using two origins should work.  */
	XGetGeometry(flx->display, xrp->xreparent.parent, &junk,
		     x, y, &uj, &uj, &rbw, &uj);
	*bw = rbw;
	M_warn("Reparent", "%s x=%d y=%d bw=%d", what, (int) *x, (int) *y, *bw);

    }

    /* last resort. Could get into trouble under some WMs */
    if ((!*y && !*bw) || *x > 20 || *y > 40)
    {
	int px, py;
	fl_get_winorigin(xrp->xreparent.parent, &px, &py);
	fl_get_winorigin(xrp->xreparent.window, x, y);
	*x -= px;
	*y -= py;
	M_warn("Reparent", "x=%d y=%d bw=%d", (int) *x, (int) *y, *bw);
    }

    /* can't get them. Take a wild guess */
    if (*x > 20 || *x < 0 || *y > 40 || *y < 0)
    {
	M_warn("Reparent", "Can't find reparent geometry");

	if (strcmp(what, "Full") == 0)
	{
	    *x = 3;
	    *y = 22;
	    *bw = 1;
	}
	else
	{
	    *x = 2;
	    *y = 10;
	    *bw = 1;
	}
    }
}

static void
get_wm_reparent_method(int reqx, int reqy, int x, int y)
{
    int trueb = FL_max(fl_wmstuff.rpx, fl_wmstuff.trpx);
    int ydiff = FL_abs(reqy - y);
    fl_wmstuff.rep_method = (ydiff > trueb) ? FL_WM_SHIFT : FL_WM_NORMAL;
    M_warn("WMReparent", "%s: reqy=%d y=%d",
	   (ydiff > trueb) ? "Shift" : "Normal", reqy, y);
}

/* Wait until we know for sure the newly mapped window is visible */
static void
wait_mapwin(Window win, int border)
{
    XEvent xev;
    unsigned mask;
    static int reparent_method, tran_method, tran_done;

    if (!(st_xswa.event_mask & ExposureMask))
    {
	M_err("WinMap", "XForms Improperly initialized");
	exit(1);
    }

    if (!reparent_method && !tran_method)
	fl_wmstuff.rep_method = FL_WM_SHIFT;

    /* obtain window manager's decoration size for normal and transient
       windows */
    mask = ExposureMask | StructureNotifyMask;

    do
    {
	XWindowEvent(flx->display, win, mask, &xev);
	fl_xevent_name("waiting", &xev);

	if (xev.type == ReparentNotify && border != FL_NOBORDER)
	{
	    FL_WM_STUFF *fb = &fl_wmstuff;
	    if (border == FL_FULLBORDER && !fb->rpy)
		get_wm_decoration("Full", &xev, &(fb->rpx), &(fb->rpy), &(fb->bw));
	    else if (border == FL_TRANSIENT && !fb->trpy && !tran_done)
	    {
		get_wm_decoration("Tran", &xev, &(fb->trpx), &(fb->trpy), &(fb->bw));
		tran_done = 1;
		if (fb->trpx < 0 || fb->trpy < 0 || fb->trpx > 30 || fb->trpy > 30)
		    fb->trpx = fb->trpy = fb->bw ? 0 : 1;
	    }
	}
#if 0
	else if (xev.type == ConfigureNotify && xev.xconfigure.send_event)
	{
	    /* not all WMs send this event, bloody shame */
	}
#endif
	if (xev.type == Expose)
	    fl_handle_event_callbacks(&xev);
    }
    while (xev.type != Expose);

    /* this might still leave a a couple of pixels off, but it is the best we
       can do about it for now. TODO */

    if (!reparent_method && (st_xsh.flags & fl_wmstuff.pos_request) &&
	border != FL_NOBORDER)
    {
	int px, py;
	if (border == FL_FULLBORDER || !tran_method)
	{
	    fl_get_winorigin(xev.xexpose.window, &px, &py);
	    get_wm_reparent_method(st_xsh.x, st_xsh.y, px, py);
	    tran_method = 1;
	    reparent_method = (border == FL_FULLBORDER);
	}
    }
}

#include <ctype.h>

static char *
fl_label_to_res_name(const char *label)
{
    static char res[54];

    res[0] = '\0';
    strncpy(res, label ? label : "", sizeof(res));
    res[sizeof(res) - 1] = '\0';
    fl_nuke_all_non_alnum(res);
    if (res[0] && isupper( ( int ) res[0]))
	res[0] = tolower(res[0]);
    return res;
}

static char *
get_machine_name(Display * d)
{
    static char machine_name[256];

    if (!machine_name[0])
    {
	char *p;
	if (gethostname(machine_name, sizeof(machine_name) - 1))
	{
	    M_err("ClientMachine", "Unable to find");
	    strcpy(machine_name, DisplayString(d));
	    if ((p = strchr(machine_name, ':')))
		*p = '\0';
	}
    }
    return machine_name;
}


void
fl_set_winproperty(Window win, unsigned prop)
{
    char **argv;
    int argc;

    if (prop & FL_COMMAND_PROP)
    {
	argv = fl_get_cmdline_args(&argc);
	XSetCommand(flx->display, win, argv, argc);
    }
}

Window
fl_create_window(Window parent, Colormap m, const char *wname)
{
    Window win;
    XClassHint clh;
    char *tmp;
    XTextProperty xtpwname, xtpmachine;
    char *label = fl_strdup(wname ? wname : "");
    FL_FORM *mainform = fl_get_app_mainform();

    st_xswa.colormap = m;
    st_wmask |= CWColormap;

    /* no decoration means unmanagered windows */
    if (st_wmborder == FL_NOBORDER &&
	(st_xsh.flags & fl_wmstuff.pos_request) == fl_wmstuff.pos_request)
    {
	/* turning this on will make the window truely unmananged, might have
	   problems with the input focus and colormaps */

	st_xswa.override_redirect = True;
	st_wmask |= CWOverrideRedirect;

    }

    /* MWM uses root window's cursor, don't want that */
    if ((st_wmask & CWCursor) != CWCursor)
    {
	st_xswa.cursor = fl_get_cursor_byname(FL_DEFAULT_CURSOR);
	st_wmask |= CWCursor;
    }

    if (st_wmborder != FL_FULLBORDER)
    {
	st_xswa.save_under = True;
	st_wmask |= CWSaveUnder;

	/* for small transient windows, we don't need backing store */
	if (st_xsh.width < 200 || st_xsh.height < 200)
	    st_xswa.backing_store = NotUseful;
    }

    if (mainform && mainform->window)
    {
	st_xwmh.flags |= WindowGroupHint;
	st_xwmh.window_group = mainform->window;
    }


#if (FL_DEBUG >= ML_WARN)
    fl_dump_state_info(fl_vmode, "WinOpen");
#endif

    win = XCreateWindow(flx->display, parent,
		    st_xsh.x, st_xsh.y, st_xsh.width, st_xsh.height, bwidth,
			fl_depth(fl_vmode), InputOutput, fl_visual(fl_vmode),
			st_wmask, &st_xswa);

    if (fl_cntl.debug > 3)
    {
	XFlush(flx->display);
	fprintf(stderr, "****CreateWin OK**** sleeping 1 seconds\n");
	sleep(1);
    }

    clh.res_name = fl_label_to_res_name(label);
    clh.res_class = "XForm";

    /* command property is set elsewhere */
    xtpwname.value = 0;
    XStringListToTextProperty(label ? &label : 0, 1, &xtpwname);

    XSetWMProperties(flx->display, win, &xtpwname, &xtpwname,
		     0, 0, &st_xsh, &st_xwmh, &clh);

    if (xtpwname.value)
	XFree(xtpwname.value);

    xtpmachine.value = 0;
    tmp = get_machine_name(flx->display);
    if ((XStringListToTextProperty(&tmp, 1, &xtpmachine)))
	XSetWMClientMachine(flx->display, win, &xtpmachine);

    if (xtpmachine.value)
	XFree(xtpmachine.value);

    fl_create_gc(win);

    if (st_wmborder == FL_TRANSIENT)
    {
	if (mainform && mainform->window)
	    XSetTransientForHint(flx->display, win, mainform->window);
	else
	    XSetTransientForHint(flx->display, win, fl_root);
    }

    fl_free(label);

    return win;
}

Window
fl_cmap_winopen(Window parent, Colormap m, const char *label)
{
    Window win = fl_create_window(parent, m, label);
    return fl_winshow(win);
}

Window
fl_wincreate(const char *label)
{
    return fl_create_window(fl_root, fl_map(fl_vmode), label);
}

Window
fl_winopen(const char *label)
{
    fl_init_colormap(fl_vmode);
    return fl_cmap_winopen(fl_root, fl_map(fl_vmode), label);
}

Window
fl_winshow(Window win)
{
#if 1
    XMapRaised(flx->display, win);
#else
    XMapSubwindows(flx->display, win);
#endif

    /* wait until the newly mapped win shows up */
    if (st_xwmh.initial_state == NormalState)
	wait_mapwin(win, st_wmborder);

    setup_catch_destroy(win);
    fl_winset(win);

    /* re-initialize window defaults  */
    fl_default_xswa();
    return win;
}

int
fl_winreparentxy(Window win, Window new_parent, int x, int y)
{

    if (!win || !new_parent)
	return -1;
    else
	return XReparentWindow(flx->display, win, new_parent, x, y);
}

int
fl_winreparent(Window win, Window new_parent)
{
    return fl_winreparentxy(win, new_parent, 0, 0);
}



void
fl_winhide(Window win)
{
    if (win)
	XUnmapWindow(flx->display, win);
}

#if 0
Window
fl_swinopen(Window parent, const char *name)
{
    /* some of default winattribe is not appropriate for subwindows. Change
       them. */

    st_xswa.override_redirect = 0;
    st_xswa.save_under = 0;
    st_wmborder = FL_FULLBORDER;

    return fl_cmap_winopen(parent, fl_map(fl_vmode), name);
}

#endif

Window
fl_cmap_swinopen(Window parent, Colormap map)
{
    return fl_cmap_winopen(parent, map, "");
}

void
fl_winbackground(Window win, FL_COLOR bk)
{
    if (win == 0)
    {
	st_xswa.background_pixel = bk;
	st_wmask |= CWBackPixel;
    }
    else
    {
	XSetWindowBackground(flx->display, win, bk);
	XClearWindow(flx->display, win);
    }
}

void
fl_winset(Window win)
{
    flx->win = win;
}

Window
fl_winget(void)
{
    return flx->win;
}

int
fl_iconify(Window win)
{
   return XIconifyWindow(flx->display, win, flx->screen);
}


/*
 * relax window constraints: minsize, maxsize, aspect ratio
 */
void
fl_reset_winconstraints(Window win)
{
    if (win)
	XSetWMNormalHints(flx->display, win, &st_xsh);
}

void
fl_winresize(Window win, FL_Coord neww, FL_Coord newh)
{
    XSizeHints lxsh;
    long fields;
    FL_Coord curwh, curww;

    if (!win)
	return;

    /* if sizes are the same, don't have to do anything. some window managers
       are too dumb to optimize this. */

    fl_get_winsize(win, &curww, &curwh);
    if (curww == neww && curwh == newh)
	return;

    lxsh.flags = 0;
    fields = 0;

    if (XGetWMNormalHints(flx->display, win, &lxsh, &fields))
    {
	lxsh.width = lxsh.base_width = neww;
	lxsh.height = lxsh.base_height = newh;
	lxsh.flags |= USSize;

	if ((lxsh.flags & PMinSize) && (lxsh.flags & PMaxSize))
	{
	    if (lxsh.min_width == lxsh.max_width)
		lxsh.min_width = lxsh.max_width = neww;
	    if (lxsh.min_height == lxsh.max_height)
		lxsh.min_height = lxsh.max_height = newh;
	}

	/* reset any contraints. */
	if ((lxsh.flags & PMinSize))
	{
	    if (lxsh.min_width > neww)
		lxsh.min_width = neww;
	    if (lxsh.min_height > newh)
		lxsh.min_height = newh;
	}

	if ((lxsh.flags & PMaxSize))
	{
	    if (lxsh.max_width < neww)
		lxsh.max_width = neww;
	    if (lxsh.max_height < newh)
		lxsh.max_height = newh;
	}

	XSetWMNormalHints(flx->display, win, &lxsh);
    }

    XResizeWindow(flx->display, win, neww, newh);
    XFlush(flx->display);
}

/* check if a given window is valid. At the moment only used by
 * canvas. A dirty hack. *****TODO *****
 * If the main event loop is correct, we don't need to do this stuff
 */

static int badwin;

static int
valid_win_handler(Display * dpy, XErrorEvent * xev)
{
    if (xev->error_code == BadWindow || xev->error_code == BadDrawable)
    {
	badwin = 1;
    }
    return 0;
}

int
fl_winisvalid(Window win)
{
    int (*old) (Display *, XErrorEvent *);
    FL_Coord w, h;

    badwin = 0;
    old = XSetErrorHandler(valid_win_handler);
    fl_get_winsize(win, &w, &h);
    XSetErrorHandler(old);
    return !badwin;
}

void
fl_wintitle(Window win, const char *title)
{
    if (win && title)
    {
	XTextProperty xtp;

	xtp.value = 0;
	XStringListToTextProperty((char **) &title, 1, &xtp);
	XSetWMName(flx->display, win, &xtp);
	XSetWMIconName(flx->display, win, &xtp);
	if (xtp.value)
	    XFree(xtp.value);
    }
}

void
fl_winicontitle(Window win, const char *title)
{
    if (win && title)
    {
	XTextProperty xtp;

	xtp.value = 0;
	XStringListToTextProperty((char **) &title, 1, &xtp);
	XSetWMIconName(flx->display, win, &xtp);
	if (xtp.value)
	    XFree(xtp.value);
    }
}

void
fl_demand_event(int xevent_type)
{
    XEvent xev;

    while (!XCheckTypedEvent(flx->display, ReparentNotify, &xev))
	;
}

/* grab keyboard focus */
void
fl_winfocus(Window win)
{
    XSetInputFocus(flx->display, win, RevertToParent, CurrentTime);
#if 0
    if (fl_context->xic)
       XSetICValues(fl_context->xic, 
                    XNClientWindow, win, XNFocusWindow, win, 0);
#endif
}

/********* END of Windowing support ***}**/
