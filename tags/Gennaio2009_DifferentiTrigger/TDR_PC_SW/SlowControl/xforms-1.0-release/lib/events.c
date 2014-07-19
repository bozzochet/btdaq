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
 * $Id: events.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *  Events handlers for the application window
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_evt = "$Id: events.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"

/*
 * Function returns 1 if the event is consumed so it will never
 * reach the application window event Q
 */
int
fl_handle_event_callbacks(XEvent * xev)
{
    Window win = ((XAnyEvent *) xev)->window;
    FL_WIN *fwin = fl_app_win;

    for (; fwin && fwin->win != win; fwin = fwin->next)
	;

    if (!fwin)
    {
	M_warn("EventCallback", "Unknown window=0x%lx", xev->xany.window);
	fl_xevent_name("Ignored", xev);
	return 0;
    }

    if (fwin->pre_emptive)
    {
	if (fwin->pre_emptive(xev, fwin->pre_emptive_data) == FL_PREEMPT)
	    return 1;
    }

    if (fwin->callback[xev->type])
    {
	(fwin->callback[xev->type]) (xev, fwin->user_data[xev->type]);
	return 1;
    }

    return 0;
}

/*** Global event handlers for all windows ******/

static FL_APPEVENT_CB fl_event_callback;
static void *fl_user_data;

/* Sets the call_back routine for the events */
FL_APPEVENT_CB
fl_set_event_callback(FL_APPEVENT_CB callback, void *user_data)
{
    FL_APPEVENT_CB old = fl_event_callback;
    fl_event_callback = callback;
    fl_user_data = user_data;
    return old;
}

/********* End of Application Window management ***********}*****/


/*************** CALL-BACK ROUTINE HANDLING ***********/

#define FL_QSIZE	      64

/*************** THE OBJECT EVENTS *************{******/

static FL_OBJECT *theobj[FL_QSIZE];	/* The object event queue */
static int ohead = 0, otail = 0;

static char *
obj_name(FL_OBJECT * ob)
{
    static char buf[128];
    char *p;

    if (ob == FL_EVENT)
	return "FL_EVENT";

    if (ob->objclass == FL_BUTTON)
	p = "Button";
    else if (ob->objclass == FL_XYPLOT)
	p = "XYPlot";
    else if (ob->objclass == FL_SLIDER || ob->objclass == FL_VALSLIDER)
	p = "Slider";
    else
	p = "?";

    snprintf(buf, sizeof(buf), "%s %s", p, ob->label);
    return buf;
}


/* Adds an object to the queue */
void
fl_object_qenter(FL_OBJECT * obj)
{
    static int warned;

/* flinternal.h controls this */
#ifndef DELAYED_ACTION

    if (obj != FL_EVENT &&
	(!obj || !obj->form || !obj->visible || obj->active <= 0))
    {

#if (FL_DEBUG >= ML_DEBUG)
	M_err("Qenter", "Bad object");
#endif
	return;
    }


    if (obj != FL_EVENT && obj->object_callback)
    {
	XFlush(flx->display);
	obj->object_callback(obj, obj->argument);
	return;
    }
    else if (obj != FL_EVENT && obj->form->form_callback)
    {
	XFlush(flx->display);
	obj->form->form_callback(obj, obj->form->form_cb_data);
	return;
    }
#endif /* DELAYED_ACTION */

    if (ohead == otail - 1 || (ohead == FL_QSIZE - 1 && otail == 0))
    {
	if (!warned)
	{
	    M_warn("QEnter", "object Q overflown:%s", obj_name(obj));
	    warned = 1;
	}
	otail = ohead = 0;
/*      otail = (otail + 1) % FL_QSIZE; */
    }

    theobj[ohead] = obj;
    ohead = (ohead + 1) % FL_QSIZE;
}

FL_OBJECT *
fl_object_qtest(void)
{
    if (ohead == otail)
	return NULL;
    return theobj[otail];
}


/* reads an object from the queue. */
FL_OBJECT *
fl_object_qread(void)
{
    FL_OBJECT *obj;

    if (ohead == otail)
	return NULL;

    obj = theobj[otail];
    otail = (otail + 1) % FL_QSIZE;

    if (obj != FL_EVENT && !obj->form)
       return 0;

    if (obj != FL_EVENT && obj->object_callback)
    {
	obj->object_callback(obj, obj->argument);
	return NULL;
    }
    else if (obj != FL_EVENT && obj->form->form_callback)
    {
	obj->form->form_callback(obj, obj->form->form_cb_data);
	return NULL;
    }
    return obj;
}

/* this is mainly used to handle the input correctly when a form
   is being hidden
 */
static void
handle_object(FL_OBJECT * obj)
{
    if (obj != FL_EVENT && obj->object_callback)
    {
	obj->object_callback(obj, obj->argument);
    }
    else if (obj != FL_EVENT && obj->form->form_callback)
    {
	obj->form->form_callback(obj, obj->form->form_cb_data);
    }
}

#ifdef DELAYED_ACTION

/***************************************************************
 * Flush all object that belong to form. This should be called
 * as part of free_form process
 *************************************************************{*/

/* read an object directly without invoking callback routines */
FL_OBJECT *
fl_object_qread_direct(void)
{
    FL_OBJECT *obj;

    if (ohead == otail)
	return NULL;
    obj = theobj[otail];
    otail = (otail + 1) % FL_QSIZE;
    return obj;
}

/*
 * flush all the objects in Q that belong to form. This routine is
 * called as part of hide_form
 */
void
fl_object_qflush(FL_FORM * form)
{
    FL_OBJECT *saveobj[50], *obj, **ppp;
    int i = 0, flushed = 0;

    for (ppp = saveobj; ppp < (saveobj + 50) && ohead != otail;)
    {
	if ((obj = fl_object_qread_direct()))
	{
	    if (obj == FL_EVENT || obj->form != form)
	    {
		*ppp++ = obj;
		i++;
	    }
	    else if (obj->objclass == FL_INPUT)
		handle_object(obj);
	    else
		flushed++;
	}
    }

    if (flushed)
	M_warn("obj_qflush", "Total of %d objects flushed for %s form",
	       flushed, form->label ? form->label : "unknown");

    /* re-Q the objects */
    for (ppp = saveobj; ppp < (saveobj + i); ppp++)
	fl_object_qenter(*ppp);
}

/*
 * remove a specific object from object Q. This routine is called
 * as part of delete object.
 */
void
fl_object_qflush_object(FL_OBJECT * ob)
{
    FL_OBJECT *saveobj[50], *obj, **ppp;
    int i = 0;

    for (ppp = saveobj; ppp < (saveobj + 50) && ohead != otail;)
    {
	if ((obj = fl_object_qread_direct()))
	{
	    if (obj != ob)
	    {
		*ppp++ = obj;
		i++;
	    }
#if (FL_DEBUG >= ML_DEBUG)
	    else
	    {
		M_err("QflushObject", "%s flushed", ob->label);
	    }
#endif
	}
    }

    /* re-Q the objects */
    for (ppp = saveobj; ppp < (saveobj + i); ppp++)
	fl_object_qenter(*ppp);
}

#endif /* DELAYED_ACTION * */

/** End of object flush ************* */

/***************** End of object Q *****************/


/**************** Normal Events ********************/

/* Treats all new user events, i.e., either calls the callback routine
 * or places FL_EVENT in the object queue. */

static XEvent appev[FL_QSIZE];
static int tail, head;
static int new_events;

void
fl_XPutBackEvent(XEvent * xev)
{
    static int nn, mm;

    if (fl_handle_event_callbacks(xev))
	return;

    if (fl_event_callback)
    {
	fl_event_callback(xev, fl_user_data);
	return;
    }

    /* these must be from simulating double buffering, throw them away */
    if (xev->type == NoExpose)
    {
	if ((++mm % 20) == 0)
	    M_warn("XPutbackEvent", "20 NoExpose discarded");
	return;
    }


    if (head == tail - 1 || (head == FL_QSIZE - 1 && tail == 0))
    {
	if ((nn++ % 10) == 0)
	{
	    M_err("PutBackEvent", "Q overflow");
	    fl_print_xevent_name("PutBackEvent", xev);
	}
	tail = (tail + 1) % FL_QSIZE;
    }

    new_events++;
    fl_xevent_name("PutbackEvent", xev);
    memcpy(appev + head, xev, sizeof(*xev));
    head = (head + 1) % FL_QSIZE;
}

int
fl_XEventsQueued(int mode)
{

    if (head == tail)
    {
	fl_treat_interaction_events(0);
	fl_treat_user_events();
    }
    return (head != tail);
}

int
fl_XNextEvent(XEvent * xev)
{
    while (head == tail)
    {
	M_err("XNextEvent", "FL_EVENT/FL_XNextEvent not right");
	fl_treat_interaction_events(1);
	fl_treat_user_events();
    }

    memcpy(xev, appev + tail, sizeof(*xev));
    tail = (tail + 1) % FL_QSIZE;
    return 1;
}

int
fl_XPeekEvent(XEvent * xev)
{
    while (head == tail)
    {
	M_err("XNextEvent", "FL_EVENT/FL_XNextEvent not right");
	fl_treat_interaction_events(1);
	fl_treat_user_events();
    }
    memcpy(xev, appev + tail, sizeof(*xev));
    return 1;
}

/*
 * get all user events and treat them: either consume by calling
 * the call back routine or put into FL internal Q for later
 * retrival
 */
void
fl_treat_user_events(void)
{
    XEvent xev;

    if (fl_event_callback)
    {
	for (; --new_events >= 0;)
	{
	    fl_XNextEvent(&xev);
	    fl_event_callback(&xev, 0);
	}
    }
    else
    {
	for (; --new_events >= 0;)
	    fl_object_qenter(FL_EVENT);
    }
    new_events = 0;
}

/******************** DEBUG use only *****************/

#define NV(a) {#a,a}
typedef struct
{
    const char *name;
    int type;
}
ev_name;

static ev_name evname[] =
{
    NV(KeyPress), NV(KeyRelease), NV(ButtonPress),
    NV(ButtonRelease), NV(MotionNotify), NV(EnterNotify),
    NV(LeaveNotify), NV(FocusIn), NV(FocusOut),
    NV(KeymapNotify), NV(Expose), NV(GraphicsExpose),
    NV(NoExpose), NV(VisibilityNotify), NV(CreateNotify),
    NV(DestroyNotify), NV(UnmapNotify), NV(MapNotify),
    NV(MapRequest), NV(ReparentNotify), NV(ConfigureNotify),
    NV(ConfigureRequest), NV(GravityNotify), NV(ResizeRequest),
    NV(CirculateNotify), NV(CirculateRequest), NV(PropertyNotify),
    NV(SelectionClear), NV(SelectionRequest), NV(SelectionNotify),
    NV(ColormapNotify), NV(ClientMessage), NV(MappingNotify),
    NV(TIMER3)
};

const char *
fl_get_xevent_name(const XEvent *xev)
{
      int i;
      static char buf[128];
      
      for ( i = 0; i < sizeof(evname); i++)
      {
          if(evname[i].type == xev->type)
          {
             snprintf(buf,sizeof(buf),"%s(0x%x)",evname[i].name, xev->type);
             return buf;
          }
      }
      return "unknown event";
}

XEvent *
fl_print_xevent_name(const char *where, const XEvent * xev)
{
    int i, known;
    Window win = ((XAnyEvent *) xev)->window;

    for (i = known = 0; !known && i < sizeof(evname) / sizeof(evname[0]); i++)
	if (evname[i].type == xev->type)
	{
	    fprintf(stderr, "%s Event(%d,w=0x%lx s=%ld) %s ",
		    where ? where : "",
		    xev->type, win, ((XAnyEvent *) xev)->serial,
		    evname[i].name);

	    if (xev->type == Expose)
		fprintf(stderr, "count=%d serial=%lx\n",
			xev->xexpose.count, xev->xexpose.serial);
	    else if (xev->type == LeaveNotify || xev->type == EnterNotify)
		fprintf(stderr, "Mode %s\n", xev->xcrossing.mode == NotifyGrab ?
			"Grab" :
		(xev->xcrossing.mode == NotifyNormal ? "Normal" : "UnGrab"));
	    else if (xev->type == MotionNotify)
		fprintf(stderr, "Mode %s\n",
			xev->xmotion.is_hint ? "Hint" : "Normal");
	    else if (xev->type == ConfigureNotify)
		fprintf(stderr, "(%d,%d) w=%d h=%d %s\n",
			xev->xconfigure.x, xev->xconfigure.y,
			xev->xconfigure.width, xev->xconfigure.height,
			xev->xconfigure.send_event ? "Syn" : "Non-Syn");
	    else if (xev->type == ButtonRelease)
		fprintf(stderr, "button: %d\n", xev->xbutton.button);
	    else
		fputc('\n', stderr);
	    known = 1;
	}

    if (!known)
	fprintf(stderr, "Unknown event %d, win=%lu", xev->type, win);
    return (XEvent *) xev;
}

XEvent *
fl_xevent_name(const char *where, const XEvent * xev)
{

    if (fl_cntl.debug >= 2)
	fl_print_xevent_name(where, xev);
    return (XEvent *) xev;
}


static int
badwin_handler(Display * dpy, XErrorEvent * xev) 
{
	if (xev->type != BadWindow && xev->type != BadDrawable)
		M_err("badwin_handler", 
			"X error happened when expecting only BadWindow/Drawable\n");
	return 0;
}
 

/***********************************************************************
 * Received a redraw event ev, see if next event is the same as the
 * the current one, drop it if it is, but we need consolidate all the
 * dirty rectangles into one.
 *
 * Must not block.
 ************************************************************************/
static void
compress_redraw(XEvent * ev)
{
    XExposeEvent *xpe = (XExposeEvent *) ev;
    Window win = xpe->window;
    XRectangle rec;
    Region reg = XCreateRegion();

    /* this is theoretically not correct as we can't peek ahead and ignore
       the events in between, but it works in XForms as we always update the
       form size and position when dealing with Expose event */
    do
    {
#if (FL_DEBUG >= ML_DEBUG)
	M_warn("CompressExpose", "x=%d y=%d w=%u h=%u count=%d",
	       xpe->x, xpe->y, xpe->width, xpe->height, xpe->count);
#endif
	rec.x = xpe->x;
	rec.y = xpe->y;
	rec.width = xpe->width;
	rec.height = xpe->height;
	XUnionRectWithRegion(&rec, reg, reg);
    }
    while (XCheckWindowEvent(flx->display, win, ExposureMask, ev));

    if (xpe->count != 0)
    {
	M_warn("CompressExpose", "Something is wrong");
	xpe->count = 0;
    }

    /* get the consolidated rectangle. Here somehow if we can get the region
       to the calling routine, XSetRegion might be more efficient as far as
       the total area painted. */

    XClipBox(reg, &rec);
    XDestroyRegion(reg);
#if (FL_DEBUG >= ML_WARN)
    M_warn("CompressExpose", "x=%d y=%d w=%u h=%u Sum\n",
	   rec.x, rec.y, rec.width, rec.height);
#endif
    xpe->x = rec.x;
    xpe->y = rec.y;
    xpe->width = rec.width;
    xpe->height = rec.height;
}

void
fl_compress_motion(XEvent * xme)
{
    Window win = xme->xmotion.window;
    unsigned long evm = PointerMotionMask | ButtonMotionMask;

    do
    {
#if (FL_DEBUG >= ML_DEBUG)
	M_info2("CompressMotion", "win=0x%lx (%d,%d) %s",
		xme->xany.window, xme->xmotion.x, xme->xmotion.y,
		xme->xmotion.is_hint ? "hint" : "")
#endif
	    ;
    }
    while (XCheckWindowEvent(flx->display, win, evm, xme));

    if (xme->xmotion.is_hint)
    {
        int (*old) (Display *, XErrorEvent *);
        /* We must protect against BadWindow here, because we have only
         * looked for Motion events, and there could be a Destroy event
         * which makes the XQueryPointer fail as the window is deleted.
         */
        old = XSetErrorHandler(badwin_handler);
	fl_get_win_mouse(xme->xmotion.window,
			 &(xme->xmotion.x), &(xme->xmotion.y), &fl_keymask);
        XSetErrorHandler(old);
	xme->xmotion.is_hint = 0;
    }
}


void
fl_compress_event(XEvent * xev, unsigned long mask)
{
    if (xev->type == Expose && (mask & ExposureMask))
	compress_redraw(xev);
    else if (xev->type == MotionNotify &&
	     ((mask & PointerMotionMask) || mask & ButtonMotionMask))
	fl_compress_motion(xev);
}

int
fl_keysym_pressed(KeySym k)
{
    char kvec[32];
    KeyCode code;

    if ((code = XKeysymToKeycode(flx->display, k)) == NoSymbol)
    {
	M_warn("CheckKeyPress", "Bad KeySym %d", (int) k);
	return 0;
    }

    XQueryKeymap(flx->display, kvec);
    return (1 & (kvec[code / 8] >> (code & 7)));
}

/* add an event */
long
fl_addto_selected_xevent(Window win, long mask)
{
    XWindowAttributes xwa;

    XGetWindowAttributes(flx->display, win, &xwa);
    xwa.your_event_mask |= mask;

    /* on some SGI machines, your_e_m has bogus value 0x80??????, causing an
       X protocol errors. Fix this here */

    xwa.your_event_mask &= (1L << 25) - 1;
    XSelectInput(flx->display, win, xwa.your_event_mask);

    return xwa.your_event_mask;
}

long
fl_remove_selected_xevent(Window win, long mask)
{
    XWindowAttributes xwa;

    XGetWindowAttributes(flx->display, win, &xwa);
    xwa.your_event_mask &= ~mask;

    /* on some SGI machines, your_e_m has bogus value 0x80??????, causing an
       X protocol errors. Fix this here */

    xwa.your_event_mask &= (1L << 25) - 1;
    XSelectInput(flx->display, win, xwa.your_event_mask);

    return xwa.your_event_mask;
}
