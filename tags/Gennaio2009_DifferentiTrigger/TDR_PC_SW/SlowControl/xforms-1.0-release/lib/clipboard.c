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
 * $Id: clipboard.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1997-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 *  Implementation of clipboard.  Event handling violates the ICCCM,
 *  but should work ok. Need server time from the mainloop.
 */

#include "forms.h"

typedef struct
{
    FL_OBJECT *ob;		/* the object that stuff'ed cp  */
    FL_OBJECT *req_ob;		/* the object that requested cp */
    Window window;
    Window req_window;
    long type, size;
    FL_LOSE_SELECTION_CB lose_callback;
    FL_SELECTION_CB got_it_callback;
    /* the following not really used */
    void *losecb_data;
    void *gotitcb_data;
}
ClipBoard;

static ClipBoard clipboard, *cp;
static int handle_clipboard_event(void *event);

static int
noop_lose_callback(FL_OBJECT * ob, long type)
{
    return 0;
}


int
fl_stuff_clipboard(FL_OBJECT * ob, long type, const void *data, long size,
		   FL_LOSE_SELECTION_CB lose_callback)
{
    Window win = FL_ObjWin(ob);

    fl_handle_clipboard = handle_clipboard_event;
    cp = &clipboard;

    if (!win)
    {
	M_err("StuffClipBoard", "Bad object %s", ob ? ob->label : "null");
	return 0;
    }

    XSetSelectionOwner(flx->display, XA_PRIMARY, win, CurrentTime);

    /* make sure we got it */
    if (XGetSelectionOwner(flx->display, XA_PRIMARY) != win)
    {
	M_err("StuffClipBoard", "Failed to Get owner");
	return 0;
    }

    /* create structure that holds clipboard info */
    cp->window = win;
    cp->ob = ob;
    cp->size = size;

    cp->lose_callback = lose_callback ? lose_callback : noop_lose_callback;

    /* cheap (and fast!) shot */
    XStoreBuffer(flx->display, data, size, 0);
    return size;
}

static Atom clipboard_prop;
static Atom targets_prop;

int
fl_request_clipboard(FL_OBJECT * ob, long type,
		     FL_SELECTION_CB got_it_callback)
{
    Window win;
    void *thebuf;
    int nb = 0;

    cp = &clipboard;
    cp->req_ob = ob;

    if (!clipboard_prop)
    {
	clipboard_prop = XInternAtom(flx->display, "FL_CLIPBOARD", False);
	fl_handle_clipboard = handle_clipboard_event;
    }

    cp->got_it_callback = got_it_callback;
    cp->req_window = FL_ObjWin(ob);

    win = XGetSelectionOwner(flx->display, XA_PRIMARY);

    if (win == None)
    {
	nb = -1;
	XSetSelectionOwner(flx->display, XA_PRIMARY, cp->req_window, CurrentTime);
	thebuf = XFetchBuffer(flx->display, &nb, 0);
	cp->window = XGetSelectionOwner(flx->display, XA_PRIMARY);
	cp->ob = 0;
	cp->size = nb;
	cp->got_it_callback(cp->req_ob, XA_STRING, thebuf, nb);
	XFree(thebuf);
    }
    else if (win != cp->req_window)
    {
	/* we don't own it. Request it */
	M_warn("clipboard", "Requesting selction from %ld", win);
	XConvertSelection(flx->display,
			  XA_PRIMARY, XA_STRING,
			  clipboard_prop,
			  cp->req_window, CurrentTime);
    }
    else if (win == cp->req_window)
    {
	/* we own the buffer */
	thebuf = XFetchBuffer(flx->display, &nb, 0);
	cp->got_it_callback(cp->req_ob, XA_STRING, thebuf, nb);
	XFree(thebuf);
    }

    return nb;
}

/*
 * reutrn a negative number if don't know how to handle an event
 */

static int
handle_clipboard_event(void *event)
{
    XSelectionRequestEvent *sreq = event;
    XEvent *xev = event;
    XSelectionEvent sev;
    int what = xev->type;
    char *s;
    int n;

    /* SelectionClear confirms lose of selection */
    /* SelectionRequest indicates other app, wants to own selection */
    /* SelectionNotify confirms requests of selection ok */

    if (!targets_prop)
	targets_prop = XInternAtom(flx->display, "TARGETS", False);
    if (!clipboard_prop)
	clipboard_prop = XInternAtom(flx->display, "FL_CLIPBOARD", False);

    cp = &clipboard;

    if (!cp->req_window && !cp->window)
    {
	M_warn("ClipBoard", "InternalError");
	return -1;
    }

    if (what == SelectionClear)
    {
	if (cp->ob)
	    cp->lose_callback(cp->ob, cp->type);
	cp->ob = 0;
	cp->window = 0;
    }
    else if (what == SelectionNotify && cp->req_ob)
    {
	/* our request went thru. Go and get it */
	Atom ret_type;
	int ret_format;
	unsigned long ret_len = 0, ret_after;
	unsigned char *ret = 0;
	/* X gurantees 16K request size */
	long chunksize = fl_context->max_request_size, offset = 0;
	char *buf = 0;
        int buflen = 0;

	/* Get the stuff. Repeat until we get all  */
	do
	{

	    XGetWindowProperty(flx->display, xev->xselection.requestor,
			       xev->xselection.property, offset, chunksize,
			       False, xev->xselection.target, &ret_type,
			       &ret_format, &ret_len, &ret_after, &ret);

	    if (ret_len && ret)
	    {
		if (ret_after == 0 && !buf)
		{
		    cp->got_it_callback(cp->req_ob, ret_type, ret, ret_len);
		}
		else
		{
		    if (!buf)
		    {
			buf = fl_malloc(1);
			buflen = 0;
		    }

		    buf = fl_realloc(buf, buflen + ret_len);
		    memcpy(buf + buflen, ret, ret_len);
		    buflen += ret_len;
		}

		XFree(ret);
		ret = 0;
	    }

	    offset += ret_len * ret_format / 32;
	    chunksize = (ret_after + 3) / 4;

	    if (chunksize > fl_context->max_request_size)
		chunksize = fl_context->max_request_size;
	}
	while (ret_after);

	if (buflen)
	{
	    cp->got_it_callback(cp->req_ob, ret_type, buf, buflen);
	    fl_free(buf);
	}

	XDeleteProperty(flx->display, xev->xselection.requestor,
			xev->xselection.property);
    }
    else if (SelectionRequest)
    {
	/* someone wants our selection */

	M_warn("clipboard", "SelectionRequest");

	if (sreq->owner != cp->window)
	{
	    M_err("ClipBoard", "Wrong owner");
	    return -1;
	}

	/* now sets the things to the requestor */
	sev.type = SelectionNotify;
	sev.display = sreq->display;
	sev.requestor = sreq->requestor;
	sev.selection = sreq->selection;
	sev.target = sreq->target;
	sev.property = None;
	sev.time = sreq->time;

	if (sreq->selection == XA_PRIMARY)
	{
	    if (sreq->target == XA_STRING)
	    {
		s = XFetchBuffer(flx->display, &n, 0);
		XChangeProperty(flx->display,
			      sreq->requestor, sreq->property, sreq->target,
				8, PropModeReplace, (unsigned char *) s, n);
		sev.property = sreq->property;
		XFree(s);
	    }
	    else if (sreq->target == targets_prop)	/*aixterm wants this*/
	    {
		Atom alist = XA_STRING;
		XChangeProperty(flx->display,
				sreq->requestor, sreq->property, XA_ATOM,
				32, PropModeReplace,
				(unsigned char *) &alist, 1);
		sev.property = sreq->property;
	    }
	    else
	    {
		/* if we have other types, conversion routine should be
		   called here */
		M_err("ClipBoard", "Unknown target: %d\n", sreq->target);
	    }

	    XSendEvent(flx->display, sreq->requestor, False, 0, (XEvent *) & sev);
	}
	else
	{
	    /* not XA_PRIMARY request */
	    M_warn("ClipBoard", "Unknown selection request: %d", sreq->selection);
	    return -1;
	}
    }

    return 0;
}
