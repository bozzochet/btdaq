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
 * $Id: appwin.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao 
 *  All rights reserved.
 *.
 *
 *  Manage application windows
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_evt = "$Id: appwin.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"

/*****************************************************************
 * Application window management. Data structure is grossly wrong
 * ****TODO******
 **************************************************************{**/


/* some constant handlers */
static int
handle_mappingnotify(XEvent * xev, void *user_data)
{
    XRefreshKeyboardMapping((XMappingEvent *) xev);
    return 0;
}


static void
remove_app_win(FL_WIN * appwin)
{
    FL_WIN *fwin;

#if (FL_DEBUG >= ML_DEBUG)
    M_info("RemoveAppwin", "deleting 0x%lx", appwin->win);
#endif
    if (fl_app_win == appwin)
    {
	fl_app_win = appwin->next;
    }
    else
    {
	for (fwin = fl_app_win; fwin && fwin->next != appwin; fwin = fwin->next)
	    ;
	if (fwin)
	    fwin->next = fwin->next->next;
    }
    appwin->win = 0;
    fl_free(appwin);
}

/* given a window, find the correct structure, create if necessary */
static FL_WIN *
find_fl_win_struct(register Window win)
{
    register FL_WIN *fwin = fl_app_win, *lwin = fl_app_win;

    for (; fwin && fwin->win != win; lwin = fwin, fwin = fwin->next)
	;

    if (!fwin)
    {
#if (FL_DEBUG >= ML_DEBUG)
	M_info("AppwinFind", "Creating for 0x%lx", win);
#endif
	/* append to the end */
	fwin = fl_calloc(1, sizeof(*fwin));
	fwin->next = 0;
	fwin->win = win;
	/* default handlers */
	fwin->callback[MappingNotify] = handle_mappingnotify;
	if (!fl_app_win)
	    fl_app_win = fwin;
	else
	    lwin->next = fwin;
    }
    return fwin;
}


FL_APPEVENT_CB
fl_set_preemptive_callback(Window win, FL_APPEVENT_CB pcb, void *data)
{
    FL_WIN *fwin;
    FL_APPEVENT_CB old = 0;

    if (!(fwin = find_fl_win_struct(win)))
	return 0;
    old = fwin->pre_emptive;
    fwin->pre_emptive = pcb;
    fwin->pre_emptive_data = data;
    return old;
}

/*  add an event handler for window */
FL_APPEVENT_CB
fl_add_event_callback(Window win, int ev,
		      FL_APPEVENT_CB wincb, void *user_data)
{
    FL_WIN *fwin;
    int i, nev;
    FL_APPEVENT_CB old = 0;

    if (ev < 0 || ev >= LASTEvent)
	return 0;

    if (!(fwin = find_fl_win_struct(win)))
    {
	M_err("AddEventCallback", "Bad Window x%lx", win);
	return 0;
    }

    /* ev <= 1 means all events. Also events start from 2 */
    nev = ev;
    if (ev < 2)
    {
	ev = 2;
	nev = LASTEvent - 1;
    }

    for (i = ev; i <= nev; i++)
    {
	old = fwin->callback[i];
	fwin->callback[i] = wincb;
	fwin->user_data[i] = user_data;
    }
    return old;
}

void
fl_remove_event_callback(Window win, int ev)
{
    FL_WIN *fwin;

    if (ev >= LASTEvent)
	return;

    if (!(fwin = find_fl_win_struct(win)))
	return;

    if (ev >= 2)
    {
	fwin->callback[ev] = 0;
	fwin->user_data[ev] = 0;
    }
    else
    {
	fwin->win = 0;		/* window deactivated */
	remove_app_win(fwin);
    }
}

typedef struct
{
    int event;
    unsigned long mask;
}
EMS;

static EMS ems[] =
{
    {CirculateNotify, StructureNotifyMask},
    {ConfigureNotify, StructureNotifyMask},
    {DestroyNotify, StructureNotifyMask},
    {CreateNotify, StructureNotifyMask},
    {GravityNotify, StructureNotifyMask},
    {MapNotify, StructureNotifyMask},
    {ReparentNotify, StructureNotifyMask},
    {UnmapNotify, StructureNotifyMask},

    {CirculateRequest, SubstructureRedirectMask},
    {ConfigureRequest, SubstructureRedirectMask},
    {MapRequest, SubstructureRedirectMask},

    {ResizeRequest, ResizeRedirectMask},


    {Expose, ExposureMask},

    {EnterNotify, EnterWindowMask},
    {LeaveNotify, LeaveWindowMask},
    {KeyPress, KeyPressMask},
    {KeyRelease, KeyReleaseMask},
    {ButtonPress, ButtonPressMask /* | OwnerGrabButtonMask */ },
    {ButtonRelease, ButtonReleaseMask /* | OwnerGrabButtonMask */ },
{MotionNotify, PointerMotionMask | ButtonMotionMask | PointerMotionHintMask},

    {FocusIn, FocusChangeMask},
    {FocusOut, FocusChangeMask},

    {KeymapNotify, KeymapStateMask},
    {PropertyNotify, PropertyChangeMask},
    {VisibilityNotify, VisibilityChangeMask},
    {ColormapNotify, ColormapChangeMask},

 /* non-maskable events */
    {MappingNotify, 0},
    {SelectionNotify, 0},
    {SelectionRequest, 0},
    {SelectionClear, 0},
    {ClientMessage, 0},
    {GraphicsExpose, 0},
    {NoExpose, 0}
};

unsigned long
fl_xevent_to_mask(int event)
{
    register EMS *em = ems, *eme = ems + sizeof(ems) / sizeof(ems[0]);

    for (; em < eme; em++)
	if (em->event == event)
	    return em->mask;
    return 0;
}

void
fl_activate_event_callbacks(Window win)
{
    FL_WIN *fwin = fl_app_win;
    int i;
    unsigned long mask;

    for (; fwin && fwin->win != win; fwin = fwin->next)
	;

    if (!fwin)
    {
	M_err("ActiveEventCB", "Unknown window 0x%lx", win);
	return;
    }

    /* solicit all registered events */
    for (mask = i = 0; i < LASTEvent; i++)
	if (fwin->callback[i])
	    mask |= fl_xevent_to_mask(i);
    XSelectInput(flx->display, win, mask);
}


void
fl_winclose(Window win)
{
    XEvent xev;

    XUnmapWindow(flx->display, win);
    XDestroyWindow(flx->display, win);

    XSync(flx->display, 0);
    while (XCheckWindowEvent(flx->display, win, FL_ALL_MASKS, &xev))
	fl_xevent_name("Eaten", &xev);

    fl_remove_event_callback(win, 0);
}
