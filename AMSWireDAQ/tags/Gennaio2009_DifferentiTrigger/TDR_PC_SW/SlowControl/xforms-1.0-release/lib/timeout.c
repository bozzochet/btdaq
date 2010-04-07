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
 * $Id: timeout.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 * Check timeout
 *
 */

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include "forms.h"

/* used to do timeout, now almost does nothing */
int
fl_check_it(const char *et)
{
    if (fl_cntl.debug == 11)
	fprintf(stderr, "%s\r        ", et);
    return 0;
}


int
fl_add_timeout(long msec, FL_TIMEOUT_CALLBACK callback, void *data)
{
    FL_TIMEOUT_REC *rec;
    static int id = 1;
    int retid;

    rec = fl_calloc(1, sizeof(*rec));
    fl_gettime(&rec->sec, &rec->usec);

    retid = id;

    rec->id = retid;
    rec->msec = rec->msec0 = msec;
    rec->callback = callback;
    rec->data = data;

    rec->next = fl_context->timeout_rec;
    fl_context->timeout_rec = rec;

    id++;

    /* we'll never generate timeout ID 0 or -1 */
    if (id == 0 || id == -1)
	id = 1;

    return retid;
}

void
fl_remove_timeout(int id)
{
    FL_TIMEOUT_REC *rec = fl_context->timeout_rec, *last;


    for (last = rec; rec && rec->id != id; last = rec, rec = rec->next)
	;

    if (rec)
    {
	if (rec == fl_context->timeout_rec)
	    fl_context->timeout_rec = rec->next;
	else
	    last->next = rec->next;

	fl_addto_freelist(rec);
    }
    else
    {
	M_err("RemoveTimeout", "ID %d not found", id);
    }
}

#define HALF_TIMER_RES (TIMER_RES/2)

void
fl_handle_timeouts(long msec)
{
    FL_TIMEOUT_REC *rec = fl_context->timeout_rec;
    long sec = 0, usec, elapsed;

    if (!rec)
	return;

    /* re-synchronize if near expiration */
    if (rec->msec < 250 || msec > 2 * TIMER_RES)
	fl_gettime(&sec, &usec);

    for (; rec; rec = rec->next)
    {
	if (sec)
	{
	    elapsed = 1000 * (sec - rec->sec) + (usec - rec->usec) / 1000;
	    rec->msec = rec->msec0 - elapsed;
	    msec = 0;
	}

	if ((rec->msec -= msec) < HALF_TIMER_RES)
	{
	    fl_remove_timeout(rec->id);
	    if (rec->callback)
		rec->callback(rec->id, rec->data);
	}
    }
}
